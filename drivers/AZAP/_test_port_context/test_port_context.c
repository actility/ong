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
//** File : ./_test_port_context/test_port_context.c
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
#include <stddef.h>
#include <rtllist.h>

#include "azap_api.h"
#include "azap_tools.h"

#include "uart_buf_control.h"
#include "azap_zcl_attr_val.h"
#include "azap_attribute.h"
#include "azap_endpoint.h"
#include "znp_start_sm.h"
#include "postponed_request_elts.h"
#include "request_context.h"
#include "request_manager.h"
#include "port_context.h"

int TraceLevel;
int TraceDebug;


/**
 * Tests for getTTYMappedName function
 */
void test_getTTYMappedName()
{
  printf("******************************************\n");
  printf("******* test_getTTYMappedName ******\n");

  assert(false == getTTYMappedName(NULL, NULL));

  printf("\ntest_getTTYMappedName OK\n\n");
}

void test_azapZclReportCmdCb (
  azap_uint8 port,
  azap_uint16 nwkAddrSrc,
  azap_uint8 endPoint,
  azap_uint16 clusterId,
  azapZclReportCommand_t *data)
{
}

/**
 * Tests for parseZCLReportCmd function
 */
void test_processZnpMsg()
{
  printf("******************************************\n");
  printf("******** test_processZnpMsg **********\n");

  t_azap_api_callbacks callbacks;
  memset(&callbacks, 0, sizeof(callbacks));

  callbacks.azapZclReportCmdCb = test_azapZclReportCmdCb;

  azapEndPoint_t *ep = new_azapEndPoint_t(0x01, 0x0104, 0x0000, 0x00, 0, NULL, 0, NULL);

  portContext_t *portCntxt = new_portContext_t(0);
  portCntxt->pCallbacks = &callbacks;
  portCntxt->addEndPoint(portCntxt, ep);

  azap_byte array[] =
  {
    0x00, 0x00, 0x02, 0x07, 0xd3, 0xa2, 0x0a, 0x01,
    0x00, 0x1f, 0x00, 0xeb, 0x77, 0x04, 0x00, 0x00,
    0x1b, 0x1c, 0x9f, 0x10, 0x00, 0x0a, 0x00, 0xe0,
    0x21, 0x8a, 0x00, 0x01, 0xe0, 0x21, 0xea, 0x00,
    0x02, 0xe0, 0x21, 0x20, 0x00, 0x03, 0xe0, 0x23,
    0x42, 0xef, 0x01, 0x00
  };

  azap_byte *data = &array[0];
  azap_uint8 len = 44;

  portCntxt->processZnpMsg(portCntxt, len, 0x44, 0x81, data);

  printf("\ntest_processZnpMsg OK\n\n");
}



/**
 * @brief Print the help message on console.
 * @param prgm the program name.
 */
void usage(char *prgm)
{
  printf("Usage: %s [-h] [-a <ieee-address>] [-g]\n", prgm);
  printf("\t-h : display this help message\n");
  printf("\t-a : use the provided IEEE address for getting the path\n");
  printf("\t     to the mapped TTY (only if -g is used)\n");
  printf("\t-g : try to retrieve the path to the mapped TTY for the\n");
  printf("\t     plugged CC2531 device\n\n");
}

/******************************************************************************/
/**
 * @brief The main entry point of the test program for targeted function set.
 * It invokes every tests.
 * @param argc
 * @param argv
 * @return 0 if all tests complete successfully; the program is
 * aborted if a test fails (see tests)
 */
int main(int argc, char *argv[])
{

  //////////////////////////////////////////////////////////////////////////////
  // The first part allow testing manually getTTYMappedName function depending
  // on plugged device.
  if (argc > 1)
  {
    char *fmtgetopt  = "a:gh";
    char *ieeeAddr = NULL;
    int getTTY = 0;
    int c;
    while ((c = getopt (argc, argv, fmtgetopt)) != -1)
    {
      switch (c)
      {
        case  'h' :
          usage(argv[0]);
          exit(0);
          break;
        case  'a' :
          ieeeAddr = optarg;
          break;
        case  'g' :
          getTTY = 1;
          break;
        default:
          break;
      }
    }

    if (getTTY)
    {
      char szPathToTTY[64];
      azap_uint8 extAddr[IEEE_ADDR_LEN];
      memset(szPathToTTY, 0, sizeof(szPathToTTY));
      azap_uint8 *addr = NULL;

      if (NULL != ieeeAddr)
      {
        if (!stringToAzap_extAddr(extAddr, ieeeAddr))
        {
          fprintf(stderr, "ERROR: Malformatted extended address provided (%s)\n\n", ieeeAddr);
          return 1;
        }
        addr = &(extAddr[0]);
      }

      getTTYMappedName(addr, szPathToTTY);
      printf("Path to mapped TTY: %s\n\n", szPathToTTY);
      return 0;
    }
  }

  //////////////////////////////////////////////////////////////////////////////
  // Other unary tests otherwise...
  test_getTTYMappedName();
  test_processZnpMsg();


  printf("\t ___\n");
  printf("\t/ __|_  _ __ __ ___ ______\n");
  printf("\t\\__ \\ || / _/ _/ -_|_-<_-<\n");
  printf("\t|___/\\_,_\\__\\__\\___/__/__/\n");
  printf("\t\n");

  return 0;
}


