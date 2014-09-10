/*
 * Copyright (C) Actility, SA. All Rights Reserved.
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License version
 * 2 only, as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License version 2 for more details (a copy is
 * included at /legal/license.txt).
 *
 * You should have received a copy of the GNU General Public License
 * version 2 along with this work; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA
 * 02110-1301 USA
 *
 * Please contact Actility, SA.,  4, rue Ampere 22300 LANNION FRANCE
 * or visit www.actility.com if you need additional
 * information or have any questions.
 */

//************************************************************************
//************************************************************************
//**
//** File : ./_test_uart_buf_control/uart_buf_control_test.c
//**
//** Description  :
//**
//**
//** Date : 11/19/2012
//** Author :  Mathias Louiset, Actility (c), 2012.
//** Modified by :
//**
//** Task :
//**
//** REVISION HISTORY:
//**
//**  Date      Version   Name
//**  --------    -------   ------------
//**
//************************************************************************
//************************************************************************


#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <assert.h>
#include <unistd.h>
#include <rtlbase.h>

#include "azap_types.h"
#include "azap_api_common.h"
#include "azap_api_af.h"
#include "azap_api_zcl.h"
#include "uart_buf_control.h"

#define MAX_BUF_SIZE 64

int TraceLevel = 2;
int TraceDebug = 1;

static char c_szBufExample1[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";

/******************************************************************************/

/**
 * @brief extract a sub-part of the buffer for display purpose
 * @param buf the source buffer
 * @param len the length to extract
 * @return the resulting buffer
 */
char *getFilledBuf(char *buf, int len)
{
  static char res[MAX_BUF_SIZE];
  memset(res, 0, sizeof(res));
  memcpy(res, buf, len);
  return res;
}


/******************************************************************************/
/*   _   _                      _          _
 *  | | | |_ _  __ _ _ _ _  _  | |_ ___ __| |_ ___
 *  | |_| | ' \/ _` | '_| || | |  _/ -_|_-<  _(_-<
 *   \___/|_||_\__,_|_|  \_, |  \__\___/__/\__/__/
 *                       |__/
 */

/**
 *
 */
void test_basicPut()
{
  printf("******************************************\n");
  printf("******** test_basicPut *******************\n");

  uartBufControl_t buf = uartBufControl_t_create(20);
  // too large to put
  assert(buf.put(&buf, 21, (void *)c_szBufExample1) == 0);
  // fit exactly
  assert(buf.put(&buf, 20, (void *)c_szBufExample1) == 20);
  // cannot put anymore
  assert(buf.put(&buf, 1, (void *)c_szBufExample1) == 0);
  buf.free(&buf);

  uartBufControl_t *buf2 = new_uartBufControl_t(4);
  assert(buf2->put(buf2, 1, (void *)c_szBufExample1) == 1);
  assert(buf2->put(buf2, 1, (void *)c_szBufExample1) == 1);
  assert(buf2->put(buf2, 1, (void *)c_szBufExample1) == 1);
  assert(buf2->put(buf2, 1, (void *)c_szBufExample1) == 1);
  assert(buf2->put(buf2, 1, (void *)c_szBufExample1) == 0);
  buf2->free(buf2);

  printf("\ntest_basicPut OK\n\n");
}

/**
 *
 */
void test_basicPutAndGet()
{
  char szTemp[30];

  printf("******************************************\n");
  printf("******** test_basicPutAndGet *************\n");

  uartBufControl_t buf = uartBufControl_t_create(20);
  // nothing to get, buf is empty
  assert(buf.get(&buf, 20, (void *)szTemp) == 0);
  // fill in the buf completely
  assert(buf.put(&buf, 20, (void *)c_szBufExample1) == 20);
  // cannot get more than buffer size
  assert(buf.get(&buf, 21, (void *)szTemp) == 0);
  // get all the bufferized bytes
  memset(szTemp, 0, sizeof(szTemp));
  assert(buf.get(&buf, 20, (void *)szTemp) == 20);
  assert(0 == memcmp(szTemp, c_szBufExample1, 20));
  // nothing to get, no more bufferized bytes available
  assert(buf.get(&buf, 1, (void *)szTemp) == 0);

  buf.free(&buf);


  printf("\ntest_basicPutAndGet OK\n\n");
}

/**
 * @brief test if order is kept with put and get.
 */
void test_putAndGetOrdering()
{
  int fillingSize;
  char szTemp[62];
  azap_uint16 i;

  fillingSize = (sizeof(c_szBufExample1) - 1) / 3;
  printf("******************************************\n");
  printf("***** test_putAndGetOrdering ******\n");

  uartBufControl_t buf = uartBufControl_t_create(101);

  printf("\t* put c_szBufExample1 at three times, and get it at once\n");
  for (i = 0; i < 100; i++)
  {
    assert(buf.put(&buf, fillingSize, (void *)c_szBufExample1) == fillingSize);
    assert(buf.put(&buf, fillingSize, (void *)&c_szBufExample1[fillingSize]) == fillingSize);
    assert(buf.put(&buf, fillingSize, (void *)&c_szBufExample1[2 * fillingSize]) == fillingSize);

    assert(buf.get(&buf, 3 * fillingSize, (void *)szTemp) == 3 * fillingSize);

    assert(!strncmp(szTemp, c_szBufExample1, 3 * fillingSize));

  }

  fillingSize = (sizeof(c_szBufExample1) - 1) / 4;
  printf("\t* put 3, get 2, put 1, get 2\n");
  for (i = 0; i < 100; i++)
  {
    assert(buf.put(&buf, fillingSize, (void *)c_szBufExample1) == fillingSize);
    assert(buf.put(&buf, fillingSize, (void *)&c_szBufExample1[fillingSize]) == fillingSize);
    assert(buf.put(&buf, fillingSize, (void *)&c_szBufExample1[2 * fillingSize]) == fillingSize);

    assert(buf.get(&buf, 2 * fillingSize, (void *)szTemp) == 2 * fillingSize);
    assert(!strncmp(szTemp, c_szBufExample1, 2 * fillingSize));

    assert(buf.put(&buf, fillingSize, (void *)&c_szBufExample1[3 * fillingSize]) == fillingSize);

    assert(buf.get(&buf, 2 * fillingSize, (void *)szTemp) == 2 * fillingSize);
    assert(!strncmp(szTemp, &c_szBufExample1[2 * fillingSize], 2 * fillingSize));
  }

  buf.free(&buf);

  printf("\ntest_putAndGetOrdering OK\n\n");
}

/**
 * @brief do various sequences of put and get on buffers of different sizes
 */
void test_putAndGetVariousBufSize()
{
  int fillingSize = 30;
  char szTemp[30];
  azap_uint16 rc;
  azap_uint16 i;

  printf("******************************************\n");
  printf("***** test_putAndGetVariousBufSize ******\n");
  assert(fillingSize * 2 < sizeof(c_szBufExample1) );

  for (i = 25; i < 100; i++)
  {
    printf("\n* buf size = %d\n", i);
    uartBufControl_t buf = uartBufControl_t_create(i);

    // bufferize the first "fillingSize" characters
    printf("\tput the %d first bytes (%s)\n", fillingSize,
        getFilledBuf(c_szBufExample1, fillingSize));
    if (fillingSize != buf.put(&buf, fillingSize, (void *)c_szBufExample1))
    {
      fprintf(stderr, "\tFail to bufferize (l:%d)\n", __LINE__);
      assert(fillingSize >= i);
    }
    else
    {
      // read the alphabet
      memset(szTemp, 0, sizeof(szTemp));
      rc = buf.get(&buf, 26, (void *)szTemp);
      printf("\tthe alphabet (rc=%d): %s\n", rc, szTemp);
      assert(!strncmp(szTemp, &c_szBufExample1[0], 26));

      // bufferize the next "fillingSize" characters
      printf("\tput the %d next bytes (%s)\n", fillingSize,
          getFilledBuf(&c_szBufExample1[fillingSize], fillingSize));
      if (fillingSize != buf.put(&buf, fillingSize, (void *)&c_szBufExample1[fillingSize]))
      {
        fprintf(stderr, "\tFail to bufferize (l:%d)\n", __LINE__);
        assert((fillingSize * 2) - 26 >= i);
      }
      else
      {
        // read the alphabet (uppercase)
        memset(szTemp, 0, sizeof(szTemp));
        rc = buf.get(&buf, 26, (void *)szTemp);
        printf("\tthe alphabet uppercase (rc=%d): %s\n", rc, szTemp);
        assert( rc == 26 );
        assert(!strncmp(szTemp, &c_szBufExample1[26], 26));

        // read the remaining 8 bytes
        memset(szTemp, 0, sizeof(szTemp));
        rc = buf.get(&buf, 8, (void *)szTemp);
        printf("\tand the remaining (rc=%d): %s\n", rc, szTemp);
        assert(!strncmp(szTemp, &c_szBufExample1[52], 8));
      }
    }

    buf.free(&buf);
  }

  printf("\ntest_putAndGetVariousBufSize OK\n\n");
}

/**
 * @brief do various sequences of put and get on
 */
void test_putAndGetLoopOnFixedBufSize()
{
  char szTemp[sizeof(c_szBufExample1) + 1];
  //azap_uint16 rc;
  unsigned int nbLoop = 10000000;

  printf("******************************************\n");
  printf("**** test_putAndGetLoopOnFixedBufSize ****\n");

  TraceLevel = 0;

  uartBufControl_t buf = uartBufControl_t_create(512);

  srandom(time(NULL));
  while (nbLoop > 0)
  {
    azap_uint8 len = (random() % sizeof(c_szBufExample1)) + 1;
    assert( len == buf.put(&buf, len, (void *)c_szBufExample1) );

    memset(szTemp, 0, sizeof(szTemp));
    /* rc = */
    buf.get(&buf, len, (void *)szTemp);
    assert(!strncmp(szTemp, &c_szBufExample1[0], len));

    if (0 == (nbLoop % 1000000))
    {
      printf("\t%d loops remaining\n", nbLoop);
    }
    nbLoop--;
  }

  TraceLevel = 2;
  printf("\ntest_putAndGetLoopOnFixedBufSize OK\n\n");
}


/**
 * @brief This function aims at testing "getNextZNPMsg" exclusively.
 * We have to test each state transition and error cases that come with
 */
void test_getNextZNPMsg()
{
  char szTemp[30];
  azap_uint8 len = 0;
  azap_uint8 cmd0 = 0;
  azap_uint8 cmd1 = 0;
  azap_uint8 i;
  // This is a SREQ for ZDO_MGMT_PERMIT_JOIN_REQ
  azap_byte msg1[] = { 0xfe, 0x04, 0x25, 0x36, 0x00, 0x00, 0xff, 0x01, 0xe9 };
  // This is a SRSP for ZDO_MGMT_PERMIT_JOIN_REQ (here received status is failure)
  azap_byte msg2[] = { 0xfe, 0x01, 0x65, 0x36, 0x01, 0x53 };

  printf("******************************************\n");
  printf("******* test_getNextZNPMsg **************\n");

  uartBufControl_t buf = uartBufControl_t_create(512);

  /////////////////////////////////////////////////////////////////////////////
  printf("\n* buffer empty, nothing to get\n");
  assert(buf.getNextZNPMsg(&buf, &len, &cmd0, &cmd1, (void *)szTemp) == false);
  assert(buf.statePending == ST_WAITING_SOP);

  /////////////////////////////////////////////////////////////////////////////
  printf("\n* put msg1 at once\n");
  assert(buf.put(&buf, sizeof(msg1), msg1) == sizeof(msg1));
  assert(buf.getNextZNPMsg(&buf, &len, &cmd0, &cmd1, (void *)szTemp) == true);
  assert(buf.statePending == ST_WAITING_SOP);
  assert(len == msg1[1]);
  assert(cmd0 == msg1[2]);
  assert(cmd1 == msg1[3]);
  assert(szTemp[0] == msg1[4]);

  /////////////////////////////////////////////////////////////////////////////
  printf("\n* put msg2 step by step and check current state\n");
  assert(buf.statePending == ST_WAITING_SOP);

  printf("\tput the SOP byte\n");
  assert(buf.put(&buf, 1, msg2) == 1);
  assert(buf.getNextZNPMsg(&buf, &len, &cmd0, &cmd1, (void *)szTemp) == false);
  assert(buf.statePending == ST_WAITING_LEN);

  printf("\tput the LEN byte\n");
  assert(buf.put(&buf, 1, &msg2[1]) == 1);
  assert(buf.getNextZNPMsg(&buf, &len, &cmd0, &cmd1, (void *)szTemp) == false);
  assert(buf.statePending == ST_WAITING_CMD0);
  assert(buf.lenPending == msg2[1]);

  printf("\tput the CMD0 byte\n");
  assert(buf.put(&buf, 1, &msg2[2]) == 1);
  assert(buf.getNextZNPMsg(&buf, &len, &cmd0, &cmd1, (void *)szTemp) == false);
  assert(buf.statePending == ST_WAITING_CMD1);
  assert(buf.cmd0Pending == msg2[2]);

  printf("\tput the CMD1 byte\n");
  assert(buf.put(&buf, 1, &msg2[3]) == 1);
  assert(buf.getNextZNPMsg(&buf, &len, &cmd0, &cmd1, (void *)szTemp) == false);
  assert(buf.statePending == ST_WAITING_DATA_AND_FCS);
  assert(buf.cmd1Pending == msg2[3]);

  printf("\tput the DATA\n");
  assert(buf.put(&buf, 1, &msg2[4]) == 1);
  assert(buf.getNextZNPMsg(&buf, &len, &cmd0, &cmd1, (void *)szTemp) == false);
  assert(buf.statePending == ST_WAITING_DATA_AND_FCS);

  printf("\tput the FCS byte\n");
  assert(buf.put(&buf, 1, &msg2[5]) == 1);
  assert(buf.getNextZNPMsg(&buf, &len, &cmd0, &cmd1, (void *)szTemp) == true);
  assert(buf.statePending == ST_WAITING_SOP);
  assert(len == msg2[1]);
  assert(cmd0 == msg2[2]);
  assert(cmd1 == msg2[3]);
  assert(szTemp[0] == msg2[4]);

  /////////////////////////////////////////////////////////////////////////////
  printf("\n* start putting msg2 and then pause for more than 5s before restart\n");
  assert(buf.statePending == ST_WAITING_SOP);

  printf("\tput the SOP and LEN byte\n");
  assert(buf.put(&buf, 2, msg2) == 2);
  assert(buf.getNextZNPMsg(&buf, &len, &cmd0, &cmd1, (void *)szTemp) == false);
  assert(buf.statePending == ST_WAITING_CMD0);

  for (i = 0; i < 4; i++)
  {
    printf("\tpause 1s\n");
    sleep(1);
    assert(buf.getNextZNPMsg(&buf, &len, &cmd0, &cmd1, (void *)szTemp) == false);
    assert(buf.statePending == ST_WAITING_CMD0);
  }

  printf("\tand finally pause 2s more\n");
  sleep(2);
  assert(buf.getNextZNPMsg(&buf, &len, &cmd0, &cmd1, (void *)szTemp) == false);
  assert(buf.statePending == ST_WAITING_SOP);



  buf.free(&buf);

  printf("\ntest_getNextZNPMsg OK\n\n");
}

/**
 * @brief This function aims at testing "put" and "getNextZNPMsg" together.
 */
void test_putAndGetNextZNPMsgOrdering()
{
  azap_byte szTemp[30];
  azap_uint8 len = 0;
  azap_uint8 cmd0 = 0;
  azap_uint8 cmd1 = 0;
  azap_uint8 i;
  // This is a SREQ for ZDO_MGMT_PERMIT_JOIN_REQ
  azap_byte msg1[] = { 0xfe, 0x04, 0x25, 0x36, 0x00, 0x00, 0xff, 0x01, 0xe9 };
  // This is a SRSP for ZDO_MGMT_PERMIT_JOIN_REQ (here received status is failure)
  azap_byte msg2[] = { 0xfe, 0x01, 0x65, 0x36, 0x01, 0x53 };

  printf("******************************************\n");
  printf("**** test_putAndGetNextZNPMsgOrdering ****\n");

  uartBufControl_t buf = uartBufControl_t_create(512);

  for (i = 0; i < 100; i++)
  {
    printf("* Loop #%d\n", i);
    printf("\t- put msg1 step by step til CMD0 byte, then remaining msg1 and msg2\n");
    assert(buf.statePending == ST_WAITING_SOP);

    printf("\t\t- put the SOP byte\n");
    assert(buf.put(&buf, 1, msg1) == 1);
    assert(buf.getNextZNPMsg(&buf, &len, &cmd0, &cmd1, (void *)szTemp) == false);
    assert(buf.statePending == ST_WAITING_LEN);

    printf("\t\t- put the LEN byte\n");
    assert(buf.put(&buf, 1, &msg1[1]) == 1);
    assert(buf.getNextZNPMsg(&buf, &len, &cmd0, &cmd1, (void *)szTemp) == false);
    assert(buf.statePending == ST_WAITING_CMD0);
    assert(buf.lenPending == msg1[1]);

    printf("\t\t- put the CMD0 byte\n");
    assert(buf.put(&buf, 1, &msg1[2]) == 1);
    assert(buf.getNextZNPMsg(&buf, &len, &cmd0, &cmd1, (void *)szTemp) == false);
    assert(buf.statePending == ST_WAITING_CMD1);
    assert(buf.cmd0Pending == msg1[2]);

    printf("\t\t- put remaining of msg1 and msg2\n");
    assert(buf.put(&buf, 6, &msg1[3]) == 6);
    assert(buf.put(&buf, 6, msg2) == 6);

    printf("\t\t- get msg1\n");
    assert(buf.getNextZNPMsg(&buf, &len, &cmd0, &cmd1, (void *)szTemp) == true);
    assert(buf.statePending == ST_WAITING_SOP);
    assert(len == msg1[1]);
    assert(cmd0 == msg1[2]);
    assert(cmd1 == msg1[3]);
    assert(szTemp[0] == msg1[4]);
    assert(szTemp[1] == msg1[5]);
    assert(szTemp[2] == msg1[6]);
    assert(szTemp[3] == msg1[7]);

    printf("\t\t- get msg2\n");
    assert(buf.getNextZNPMsg(&buf, &len, &cmd0, &cmd1, (void *)szTemp) == true);
    assert(buf.statePending == ST_WAITING_SOP);
    assert(len == msg2[1]);
    assert(cmd0 == msg2[2]);
    assert(cmd1 == msg2[3]);
    assert(szTemp[0] == msg2[4]);

    printf("\t* put msg1 except FCS, then FCS and msg2\n");
    printf("\t\t- put msg1 except FCS\n");
    assert(buf.put(&buf, 8, msg1) == 8);
    assert(buf.getNextZNPMsg(&buf, &len, &cmd0, &cmd1, (void *)szTemp) == false);
    assert(buf.statePending == ST_WAITING_DATA_AND_FCS);
    assert(buf.lenPending == msg1[1]);
    assert(buf.cmd0Pending == msg1[2]);
    assert(buf.cmd1Pending == msg1[3]);

    printf("\t\t- put FCS and msg2\n");
    assert(buf.put(&buf, 1, &msg1[8]) == 1);
    assert(buf.put(&buf, 6, msg2) == 6);

    printf("\t\t- get msg1\n");
    assert(buf.getNextZNPMsg(&buf, &len, &cmd0, &cmd1, (void *)szTemp) == true);
    assert(buf.statePending == ST_WAITING_SOP);
    assert(len == msg1[1]);
    assert(cmd0 == msg1[2]);
    assert(cmd1 == msg1[3]);
    assert(szTemp[0] == msg1[4]);
    assert(szTemp[1] == msg1[5]);
    assert(szTemp[2] == msg1[6]);
    assert(szTemp[3] == msg1[7]);

    printf("\t\t- get msg2\n");
    assert(buf.getNextZNPMsg(&buf, &len, &cmd0, &cmd1, (void *)szTemp) == true);
    assert(buf.statePending == ST_WAITING_SOP);
    assert(len == msg2[1]);
    assert(cmd0 == msg2[2]);
    assert(cmd1 == msg2[3]);
    assert(szTemp[0] == msg2[4]);

  }
  buf.free(&buf);

  printf("\ntest_putAndGetNextZNPMsgOrdering OK\n\n");
}

/**
 * @brief This function aims at testing "put" and "getNextZNPMsg" together with large messages.
 * This can be particularly critical because of packet length data type limited to 255 (azap_uint8)
 */
void test_putAndGetNextZNPMsgBigMsgs()
{
  azap_byte szTemp[255];
  azap_uint8 len = 0;
  azap_uint8 cmd0 = 0;
  azap_uint8 cmd1 = 0;
  azap_uint8 i;

  azap_uint8 msg1[] =  { 0xfe, 0x36, 0x44, 0x81, 0x00, 0x00, 0x00, 0x00, 0xf1,
      0x0e, 0x01, 0x01, 0x00, 0x5e, 0x00, 0xfc, 0x7b, 0x01, 0x00, 0x00, 0x25, 0x18,
      0x00, 0x0d, 0x01, 0x00, 0x00, 0x20, 0x01, 0x00, 0x20, 0x02, 0x00, 0x20, 0x03,
      0x00, 0x20, 0x04, 0x00, 0x42, 0x05, 0x00, 0x42, 0x06, 0x00, 0x42, 0x07, 0x00,
      0x30, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x87
                       }; // size = 59
  azap_uint8 msg2[] =  { 0xfe, 0x1f, 0x45, 0xff, 0xf1, 0x0e, 0x00, 0x04, 0x80,
      0x00, 0x0d, 0x00, 0x00, 0x00, 0xf1, 0x0e, 0x12, 0x01, 0x04, 0x01, 0x00, 0x00,
      0x00, 0x04, 0x00, 0x00, 0x01, 0x00, 0x03, 0x00, 0x07, 0x00, 0x01, 0x06, 0x00,
      0x3c
                       }; // size = 34
  azap_uint8 msg3[] =  { 0xfe, 0x58, 0x44, 0x81, 0x00, 0x00, 0x00, 0x00, 0xf1,
      0x0e, 0x01, 0x01, 0x00, 0x5c, 0x00, 0x66, 0x92, 0x01, 0x00, 0x00, 0x47, 0x18,
      0x00, 0x01, 0x00, 0x00, 0x00, 0x20, 0x03, 0x01, 0x00, 0x00, 0x20, 0x16, 0x02,
      0x00, 0x00, 0x20, 0x2a, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x8a
                       }; // size = 93

  printf("******************************************\n");
  printf("**** test_putAndGetNextZNPMsgBigMsgs *****\n");

  uartBufControl_t buf = uartBufControl_t_create(512);

  printf("\t- put msg1\n");
  assert(buf.put(&buf, sizeof(msg1), msg1) == sizeof(msg1));
  printf("\t- put msg2\n");
  assert(buf.put(&buf, sizeof(msg2), msg2) == sizeof(msg2));
  printf("\t- put msg3\n");
  assert(buf.put(&buf, sizeof(msg3), msg3) == sizeof(msg3));
  printf("\t- put msg1\n");
  assert(buf.put(&buf, sizeof(msg1), msg1) == sizeof(msg1));
  printf("\t- put msg2\n");
  assert(buf.put(&buf, sizeof(msg2), msg2) == sizeof(msg2));
  printf("\t- put msg3\n");
  assert(buf.put(&buf, sizeof(msg3), msg3) == sizeof(msg3));

  printf("\t- get msg1\n");
  assert(buf.getNextZNPMsg(&buf, &len, &cmd0, &cmd1, (void *)szTemp) == true);
  assert(buf.statePending == ST_WAITING_SOP);
  assert(len == msg1[1]);
  assert(cmd0 == msg1[2]);
  assert(cmd1 == msg1[3]);
  for (i = 0; i < (sizeof(msg1) - 5); i++)
  {
    assert(szTemp[i] == msg1[i + 4]);
  }
  printf("\t- get msg2\n");
  assert(buf.getNextZNPMsg(&buf, &len, &cmd0, &cmd1, (void *)szTemp) == true);
  assert(buf.statePending == ST_WAITING_SOP);
  assert(len == msg2[1]);
  assert(cmd0 == msg2[2]);
  assert(cmd1 == msg2[3]);
  for (i = 0; i < (sizeof(msg2) - 5); i++)
  {
    assert(szTemp[i] == msg2[i + 4]);
  }
  printf("\t- get msg3\n");
  assert(buf.getNextZNPMsg(&buf, &len, &cmd0, &cmd1, (void *)szTemp) == true);
  assert(buf.statePending == ST_WAITING_SOP);
  assert(len == msg3[1]);
  assert(cmd0 == msg3[2]);
  assert(cmd1 == msg3[3]);
  for (i = 0; i < (sizeof(msg3) - 5); i++)
  {
    assert(szTemp[i] == msg3[i + 4]);
  }

  printf("\t- get msg1\n");
  assert(buf.getNextZNPMsg(&buf, &len, &cmd0, &cmd1, (void *)szTemp) == true);
  assert(buf.statePending == ST_WAITING_SOP);
  assert(len == msg1[1]);
  assert(cmd0 == msg1[2]);
  assert(cmd1 == msg1[3]);
  for (i = 0; i < (sizeof(msg1) - 5); i++)
  {
    assert(szTemp[i] == msg1[i + 4]);
  }
  printf("\t- get msg2\n");
  assert(buf.getNextZNPMsg(&buf, &len, &cmd0, &cmd1, (void *)szTemp) == true);
  assert(buf.statePending == ST_WAITING_SOP);
  assert(len == msg2[1]);
  assert(cmd0 == msg2[2]);
  assert(cmd1 == msg2[3]);
  for (i = 0; i < (sizeof(msg2) - 5); i++)
  {
    assert(szTemp[i] == msg2[i + 4]);
  }
  printf("\t- get msg3\n");
  assert(buf.getNextZNPMsg(&buf, &len, &cmd0, &cmd1, (void *)szTemp) == true);
  assert(buf.statePending == ST_WAITING_SOP);
  assert(len == msg3[1]);
  assert(cmd0 == msg3[2]);
  assert(cmd1 == msg3[3]);
  for (i = 0; i < (sizeof(msg3) - 5); i++)
  {
    assert(szTemp[i] == msg3[i + 4]);
  }

  buf.free(&buf);

  printf("\ntest_putAndGetNextZNPMsgBigMsgs OK\n\n");
}

/******************************************************************************/
/**
 * @brief The main entry point oof the test program.
 * It invokes every tests.
 * @param argc
 * @param argv
 * @return 0 if all tests complete successfully; the program is
 * aborted if a test fails (see tests)
 */
int main(int argc, char *argv[])
{
  if (argc == 2)
  {
    TraceLevel = atoi(argv[1]);
    rtl_init();
    rtl_tracelevel(TraceLevel);
  }

  test_basicPut();
  test_basicPutAndGet();
  test_putAndGetOrdering();
  test_putAndGetVariousBufSize();
  test_putAndGetLoopOnFixedBufSize();
  test_getNextZNPMsg();
  test_putAndGetNextZNPMsgOrdering();
  test_putAndGetNextZNPMsgBigMsgs();

  printf("\t ___\n");
  printf("\t/ __|_  _ __ __ ___ ______\n");
  printf("\t\\__ \\ || / _/ _/ -_|_-<_-<\n");
  printf("\t|___/\\_,_\\__\\__\\___/__/__/\n");
  printf("\t\n");

  return 0;
}
