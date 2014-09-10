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
//** File : ./_test_format_znp/format_znp_test.c
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

#include "azap_types.h"
#include "azap_tools.h"
#include "uart_buf_control.h"
#include "format_znp.h"




/**
 * Tests for parseUint8AndShift function
 */
void test_parseUint8AndShift()
{
  printf("******************************************\n");
  printf("******** test_parseUint8AndShift *********\n");

  azap_byte array[] =
  {
    0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
    0x7E, 0x7F, 0x81, 0x82, 0xFC, 0xFD, 0xFE, 0xFF
  };

  azap_byte *data = &array[0];
  azap_uint8 remainingLen = 1;
  printf("\t- test null data\n");
  assert( 0 == parseUint8AndShift(NULL, &remainingLen) );
  remainingLen = 0;
  assert( 0 == parseUint8AndShift(&data, &remainingLen) );

  remainingLen = sizeof(array);
  printf("\t- test array\n");
  assert( 0 == parseUint8AndShift(&data, &remainingLen) );
  assert( 1 == parseUint8AndShift(&data, &remainingLen) );
  assert( 2 == parseUint8AndShift(&data, &remainingLen) );
  assert( 3 == parseUint8AndShift(&data, &remainingLen) );
  assert( 4 == parseUint8AndShift(&data, &remainingLen) );
  assert( 5 == parseUint8AndShift(&data, &remainingLen) );
  assert( 6 == parseUint8AndShift(&data, &remainingLen) );
  assert( 7 == parseUint8AndShift(&data, &remainingLen) );
  assert( 126 == parseUint8AndShift(&data, &remainingLen) );
  assert( 127 == parseUint8AndShift(&data, &remainingLen) );
  assert( 129 == parseUint8AndShift(&data, &remainingLen) );
  assert( 130 == parseUint8AndShift(&data, &remainingLen) );
  assert( 252 == parseUint8AndShift(&data, &remainingLen) );
  assert( 253 == parseUint8AndShift(&data, &remainingLen) );
  assert( 254 == parseUint8AndShift(&data, &remainingLen) );
  assert( 255 == parseUint8AndShift(&data, &remainingLen) );

  printf("\ntest_parseUint8AndShift OK\n\n");
}

/**
 * Tests for parseUint8ArrayAndShift function
 */
void test_parseUint8ArrayAndShift()
{
  printf("******************************************\n");
  printf("***** test_parseUint8ArrayAndShift *******\n");

  azap_byte array[] =
  {
    0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
    0x7E, 0x7F, 0x81, 0x82, 0xFC, 0xFD, 0xFE, 0xFF
  };

  azap_byte *data = &array[0];
  azap_uint8 remainingLen = sizeof(array);
  printf("\t- test null data\n");
  assert( NULL == parseUint8ArrayAndShift(NULL, 1, &remainingLen) );
  assert( NULL == parseUint8ArrayAndShift(&data, 0, &remainingLen) );
  remainingLen = 0;
  assert( NULL == parseUint8ArrayAndShift(&data, 1, &remainingLen) );

  remainingLen = sizeof(array);
  printf("\t- test array\n");
  azap_uint8 *res = parseUint8ArrayAndShift(&data, 2, &remainingLen);
  assert( NULL != res );
  assert( 0 == res[0] );
  assert( 1 == res[1] );
  free(res);

  res = parseUint8ArrayAndShift(&data, 6, &remainingLen);
  assert( NULL != res );
  assert( 2 == res[0] );
  assert( 3 == res[1] );
  assert( 4 == res[2] );
  assert( 5 == res[3] );
  assert( 6 == res[4] );
  assert( 7 == res[5] );
  free(res);

  res = parseUint8ArrayAndShift(&data, 1, &remainingLen);
  assert( NULL != res );
  assert( 126 == res[0] );
  free(res);

  res = parseUint8ArrayAndShift(&data, 7, &remainingLen);
  assert( NULL != res );
  assert( 127 == res[0] );
  assert( 129 == res[1] );
  assert( 130 == res[2] );
  assert( 252 == res[3] );
  assert( 253 == res[4] );
  assert( 254 == res[5] );
  assert( 255 == res[6] );
  free(res);

  printf("\ntest_parseUint8ArrayAndShift OK\n\n");
}

/**
 * Tests for parseUint16AndShift function
 */
void test_parseUint16AndShift()
{
  printf("******************************************\n");
  printf("******* test_parseUint16AndShift *******\n");

  azap_byte array[] =
  {
    0x01, 0x00, 0x02, 0x00, 0x03, 0x00, 0x04, 0x00,
    0xE8, 0x03, 0x00, 0x01, 0x00, 0x00, 0xFF, 0xFF
  };

  azap_byte *data = &array[0];
  azap_uint8 remainingLen = sizeof(array);
  printf("\t- test null data\n");
  assert( 0 == parseUint16AndShift(NULL, &remainingLen) );
  remainingLen = 0;
  assert( 0 == parseUint16AndShift(&data, &remainingLen) );
  remainingLen = 1;
  assert( 0 == parseUint16AndShift(&data, &remainingLen) );

  remainingLen = sizeof(array);
  printf("\t- test array\n");
  assert( 1 == parseUint16AndShift(&data, &remainingLen) );
  assert( 2 == parseUint16AndShift(&data, &remainingLen) );
  assert( 3 == parseUint16AndShift(&data, &remainingLen) );
  assert( 4 == parseUint16AndShift(&data, &remainingLen) );
  assert( 1000 == parseUint16AndShift(&data, &remainingLen) );
  assert( 256 == parseUint16AndShift(&data, &remainingLen) );
  assert( 0 == parseUint16AndShift(&data, &remainingLen) );
  assert( 65535 == parseUint16AndShift(&data, &remainingLen) );

  printf("\ntest_parseUint16AndShift OK\n\n");
}

/**
 * Tests for parseUint16ArrayAndShift function
 */
void test_parseUint16ArrayAndShift()
{
  printf("******************************************\n");
  printf("***** test_parseUint16ArrayAndShift ******\n");

  azap_byte array[] =
  {
    0x01, 0x00, 0x02, 0x00, 0x03, 0x00, 0x04, 0x00,
    0xE8, 0x03, 0x00, 0x01, 0x00, 0x00, 0xFF, 0xFF
  };

  azap_byte *data = &array[0];
  azap_uint8 remainingLen = 2;
  printf("\t- test null data\n");
  assert( NULL == parseUint16ArrayAndShift(NULL, 1, &remainingLen) );
  assert( NULL == parseUint16ArrayAndShift(&data, 0, &remainingLen) );
  remainingLen = 0;
  assert( NULL == parseUint16ArrayAndShift(&data, 1, &remainingLen) );
  remainingLen = 1;
  assert( NULL == parseUint16ArrayAndShift(&data, 1, &remainingLen) );
  remainingLen = sizeof(array);

  printf("\t- test array\n");
  azap_uint16 *res = parseUint16ArrayAndShift(&data, 1, &remainingLen);
  assert( NULL != res );
  assert( 1 == res[0] );
  free(res);

  res = parseUint16ArrayAndShift(&data, 7, &remainingLen);
  assert( NULL != res );
  assert( 2 == res[0] );
  assert( 3 == res[1] );
  assert( 4 == res[2] );
  assert( 1000 == res[3] );
  assert( 256 == res[4] );
  assert( 0 == res[5] );
  assert( 65535 == res[6] );
  free(res);

  printf("\ntest_parseUint16ArrayAndShift OK\n\n");
}

/**
 * Tests for parseUint16AndShift function
 */
void test_parseUint32AndShift()
{
  printf("******************************************\n");
  printf("******* test_parseUint32AndShift *******\n");

  azap_byte array[] =
  {
    0x01, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00,
    0x03, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00,
    0xE8, 0x03, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0x00, 0x00,
    0x00, 0x00, 0x01, 0x00, 0xFF, 0xFF, 0xFF, 0x7F
  };

  azap_byte *data = &array[0];
  azap_uint8 remainingLen = sizeof(array);
  printf("\t- test null data\n");
  assert( 0 == parseUint32AndShift(NULL, &remainingLen) );
  remainingLen = 0;
  assert( 0 == parseUint32AndShift(&data, &remainingLen) );
  remainingLen = 1;
  assert( 0 == parseUint32AndShift(&data, &remainingLen) );

  remainingLen = sizeof(array);
  printf("\t- test array\n");
  assert( 1 == parseUint32AndShift(&data, &remainingLen) );
  assert( 2 == parseUint32AndShift(&data, &remainingLen) );
  assert( 3 == parseUint32AndShift(&data, &remainingLen) );
  assert( 4 == parseUint32AndShift(&data, &remainingLen) );
  assert( 1000 == parseUint32AndShift(&data, &remainingLen) );
  assert( 256 == parseUint32AndShift(&data, &remainingLen) );
  assert( 0 == parseUint32AndShift(&data, &remainingLen) );
  assert( 65535 == parseUint32AndShift(&data, &remainingLen) );
  assert( 65536 == parseUint32AndShift(&data, &remainingLen) );
  assert( 2147483647 == parseUint32AndShift(&data, &remainingLen) );

  printf("\ntest_parseUint32AndShift OK\n\n");
}

/**
 * Tests for parseExtAddrAndShift function
 */
void test_parseExtAddrAndShift()
{
  printf("******************************************\n");
  printf("******* test_parseExtAddrAndShift *******\n");

  azap_uint8 ieee[IEEE_ADDR_LEN];
  azap_uint8 array[] =
  {
    0x08, 0x07, 0x06, 0x05, 0x04, 0x03, 0x02, 0x01,
    0x10, 0x0F, 0x0E, 0x0D, 0x0C, 0x0B, 0x0A, 0x09,
    0x18, 0x17, 0x16, 0x15, 0x14, 0x13, 0x12, 0x11
  };
  azap_uint8 *data = &array[0];

  azap_uint8 remainingLen = sizeof(array);
  printf("\t- straight test\n");
  parseExtAddrAndShift(ieee, &data, &remainingLen);
  assert(0 == strcmp(azap_extAddrToString(ieee, true), "0x0102030405060708"));

  printf("\t- same calls after shift\n");
  parseExtAddrAndShift(ieee, &data, &remainingLen);
  assert(0 == strcmp(azap_extAddrToString(ieee, true), "0x090a0b0c0d0e0f10"));

  parseExtAddrAndShift(ieee, &data, &remainingLen);
  assert(0 == strcmp(azap_extAddrToString(ieee, true), "0x1112131415161718"));

  printf("\t- test null data\n");
  memset(ieee, 0, IEEE_ADDR_LEN);
  parseExtAddrAndShift(ieee, &data, &remainingLen);
  assert(0 == strcmp(azap_extAddrToString(ieee, true), "0x0000000000000000"));
  remainingLen = IEEE_ADDR_LEN;
  parseExtAddrAndShift(ieee, NULL, &remainingLen);
  assert(0 == strcmp(azap_extAddrToString(ieee, true), "0x0000000000000000"));

  printf("\ntest_parseExtAddrAndShift OK\n\n");
}


void test_azap_uint8CpyPayloadAndShift()
{
  printf("******************************************\n");
  printf("**** test_azap_uint8CpyPayloadAndShift ***\n");

  azap_byte array[16];

  azap_byte *dataPtr = &array[0];

  printf("\t- test null data\n");
  azap_uint8CpyPayloadAndShift(0, NULL);

  printf("\t- test array\n");
  azap_uint8CpyPayloadAndShift(0, &dataPtr);
  azap_uint8CpyPayloadAndShift(1, &dataPtr);
  azap_uint8CpyPayloadAndShift(2, &dataPtr);
  azap_uint8CpyPayloadAndShift(3, &dataPtr);
  azap_uint8CpyPayloadAndShift(4, &dataPtr);
  azap_uint8CpyPayloadAndShift(5, &dataPtr);
  azap_uint8CpyPayloadAndShift(6, &dataPtr);
  azap_uint8CpyPayloadAndShift(7, &dataPtr);
  azap_uint8CpyPayloadAndShift(126, &dataPtr);
  azap_uint8CpyPayloadAndShift(127, &dataPtr);
  azap_uint8CpyPayloadAndShift(129, &dataPtr);
  azap_uint8CpyPayloadAndShift(130, &dataPtr);
  azap_uint8CpyPayloadAndShift(252, &dataPtr);
  azap_uint8CpyPayloadAndShift(253, &dataPtr);
  azap_uint8CpyPayloadAndShift(254, &dataPtr);
  azap_uint8CpyPayloadAndShift(255, &dataPtr);

  assert( array[0] == 0x00 );
  assert( array[1] == 0x01 );
  assert( array[2] == 0x02 );
  assert( array[3] == 0x03 );
  assert( array[4] == 0x04 );
  assert( array[5] == 0x05 );
  assert( array[6] == 0x06 );
  assert( array[7] == 0x07 );
  assert( array[8] == 0x7E );
  assert( array[9] == 0x7F );
  assert( array[10] == 0x81 );
  assert( array[11] == 0x82 );
  assert( array[12] == 0xFC );
  assert( array[13] == 0xFD );
  assert( array[14] == 0xFE );
  assert( array[15] == 0xFF );

  printf("\ntest_azap_uint8CpyPayloadAndShift OK\n\n");
}

void test_azap_uint8ArrayCpyPayloadAndShift()
{
  printf("******************************************\n");
  printf("* test_azap_uint8ArrayCpyPayloadAndShift *\n");

  azap_byte array[16];

  azap_byte *dataPtr = &array[0];

  printf("\t- test null data\n");
  azap_uint8 tab1[] = {0, 1};
  azap_uint8 tab2[] = {2, 3, 4, 5, 6, 7};;
  azap_uint8 tab3[] = {126, 127, 129, 130, 252, 253, 254, 255};
  azap_uint8ArrayCpyPayloadAndShift(tab1, 1, NULL);
  azap_uint8ArrayCpyPayloadAndShift(tab1, 0, &dataPtr);

  printf("\t- test array\n");
  azap_uint8ArrayCpyPayloadAndShift(tab1, 2, &dataPtr);
  azap_uint8ArrayCpyPayloadAndShift(tab2, 6, &dataPtr);
  azap_uint8ArrayCpyPayloadAndShift(tab3, 8, &dataPtr);

  assert( array[0] == 0x00 );
  assert( array[1] == 0x01 );
  assert( array[2] == 0x02 );
  assert( array[3] == 0x03 );
  assert( array[4] == 0x04 );
  assert( array[5] == 0x05 );
  assert( array[6] == 0x06 );
  assert( array[7] == 0x07 );
  assert( array[8] == 0x7E );
  assert( array[9] == 0x7F );
  assert( array[10] == 0x81 );
  assert( array[11] == 0x82 );
  assert( array[12] == 0xFC );
  assert( array[13] == 0xFD );
  assert( array[14] == 0xFE );
  assert( array[15] == 0xFF );

  printf("\ntest_azap_uint8ArrayCpyPayloadAndShift OK\n\n");
}

void test_azap_int16CpyPayloadAndShift()
{
  printf("******************************************\n");
  printf("*** test_azap_int16CpyPayloadAndShift ****\n");

  azap_byte array[16];

  azap_byte *dataPtr = &array[0];

  printf("\t- test null data\n");
  azap_uint16CpyPayloadAndShift(0, NULL);

  printf("\t- test array\n");
  azap_uint16CpyPayloadAndShift(1, &dataPtr);
  azap_uint16CpyPayloadAndShift(2, &dataPtr);
  azap_uint16CpyPayloadAndShift(3, &dataPtr);
  azap_uint16CpyPayloadAndShift(4, &dataPtr);
  azap_uint16CpyPayloadAndShift(1000, &dataPtr);
  azap_uint16CpyPayloadAndShift(256, &dataPtr);
  azap_uint16CpyPayloadAndShift(0, &dataPtr);
  azap_uint16CpyPayloadAndShift(65535, &dataPtr);

  assert( array[0] == 0x01 );
  assert( array[1] == 0x00 );
  assert( array[2] == 0x02 );
  assert( array[3] == 0x00 );
  assert( array[4] == 0x03 );
  assert( array[5] == 0x00 );
  assert( array[6] == 0x04 );
  assert( array[7] == 0x00 );
  assert( array[8] == 0xE8 );
  assert( array[9] == 0x03 );
  assert( array[10] == 0x00 );
  assert( array[11] == 0x01 );
  assert( array[12] == 0x00 );
  assert( array[13] == 0x00 );
  assert( array[14] == 0xFF );
  assert( array[15] == 0xFF );


  printf("\ntest_azap_int16CpyPayloadAndShift OK\n\n");
}

void test_azap_int16ArrayCpyPayloadAndShift()
{
  printf("******************************************\n");
  printf("* test_azap_int16ArrayCpyPayloadAndShift *\n");

  azap_byte array[16];

  azap_byte *dataPtr = &array[0];

  azap_uint16 tab1[] = { 1 };
  azap_uint16 tab2[] = { 2, 3 };
  azap_uint16 tab3[] = { 4, 1000, 256, 0, 65535 };

  printf("\t- test null data\n");
  azap_uint16ArrayCpyPayloadAndShift(0, 1, NULL);
  azap_uint16ArrayCpyPayloadAndShift(0, 0, &dataPtr);

  printf("\t- test array\n");

  azap_uint16ArrayCpyPayloadAndShift(tab1, 1, &dataPtr);
  azap_uint16ArrayCpyPayloadAndShift(tab2, 2, &dataPtr);
  azap_uint16ArrayCpyPayloadAndShift(tab3, 5, &dataPtr);

  assert( array[0] == 0x01 );
  assert( array[1] == 0x00 );
  assert( array[2] == 0x02 );
  assert( array[3] == 0x00 );
  assert( array[4] == 0x03 );
  assert( array[5] == 0x00 );
  assert( array[6] == 0x04 );
  assert( array[7] == 0x00 );
  assert( array[8] == 0xE8 );
  assert( array[9] == 0x03 );
  assert( array[10] == 0x00 );
  assert( array[11] == 0x01 );
  assert( array[12] == 0x00 );
  assert( array[13] == 0x00 );
  assert( array[14] == 0xFF );
  assert( array[15] == 0xFF );

  printf("\ntest_azap_int16ArrayCpyPayloadAndShift OK\n\n");
}

void test_azap_extAddrCpyPayloadAndShift()
{
  printf("******************************************\n");
  printf("*** test_azap_extAddrCpyPayloadAndShift **\n");

  azap_byte array[16];

  azap_byte *dataPtr = &array[0];

  azap_uint8 ieee1[IEEE_ADDR_LEN] = { 0x08, 0x07, 0x06, 0x05, 0x04, 0x03, 0x02, 0x01 };
  azap_uint8 ieee2[IEEE_ADDR_LEN] = { 0x10, 0x0F, 0x0E, 0x0D, 0x0C, 0x0B, 0x0A, 0x09 };

  printf("\t- test null data\n");
  azap_extAddrCpyPayloadAndShift(ieee1, NULL);
  azap_extAddrCpyPayloadAndShift(NULL, &dataPtr);

  printf("\t- test array\n");
  azap_extAddrCpyPayloadAndShift(ieee1, &dataPtr);
  azap_extAddrCpyPayloadAndShift(ieee2, &dataPtr);

  assert( array[0] == 0x08 );
  assert( array[1] == 0x07 );
  assert( array[2] == 0x06 );
  assert( array[3] == 0x05 );
  assert( array[4] == 0x04 );
  assert( array[5] == 0x03 );
  assert( array[6] == 0x02 );
  assert( array[7] == 0x01 );
  assert( array[8] == 0x10 );
  assert( array[9] == 0x0F );
  assert( array[10] == 0x0E );
  assert( array[11] == 0x0D );
  assert( array[12] == 0x0C );
  assert( array[13] == 0x0B );
  assert( array[14] == 0x0A );
  assert( array[15] == 0x09 );

  printf("\ntest_azap_extAddrCpyPayloadAndShift OK\n\n");
}


/******************************************************************************/
/**
 * @brief The main entry point of the test program for format_znp function set.
 * It invokes every tests.
 * @param argc
 * @param argv
 * @return 0 if all tests complete successfully; the program is
 * aborted if a test fails (see tests)
 */
int main(int argc, char *argv[])
{

  test_parseUint8AndShift();
  test_parseUint8ArrayAndShift();
  test_parseUint16AndShift();
  test_parseUint16ArrayAndShift();
  test_parseExtAddrAndShift();


  test_azap_uint8CpyPayloadAndShift();
  test_azap_uint8ArrayCpyPayloadAndShift();
  test_azap_int16CpyPayloadAndShift();
  test_azap_int16ArrayCpyPayloadAndShift();
  test_azap_extAddrCpyPayloadAndShift();

  printf("\t ___\n");
  printf("\t/ __|_  _ __ __ ___ ______\n");
  printf("\t\\__ \\ || / _/ _/ -_|_-<_-<\n");
  printf("\t|___/\\_,_\\__\\__\\___/__/__/\n");
  printf("\t\n");

  return 0;
}


