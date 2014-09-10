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
//** File : _test_m2m_zcl_0x0000_abstraction/m2m_zcl_0x0000_abstraction_test.c
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
#include <time.h>
#include <string.h>
#include <strings.h>

#include "rtlbase.h"
#include "rtllist.h"
#include "rtlimsg.h"

#include "cmnstruct.h"
#include "cmnproto.h"
#include "adaptor.h"

//#include "azap_types.h"
//#include "azap_api.h"
//#include "azap_api_zcl.h"
//#include "azap_zcl_attr_val.h"
//#include "azap_attribute.h"
//#include "azap_endpoint.h"
//#include "driver_req_context.h"
//#include "zigbee_node.h"
//#include "interface_mapping_desc.h"
//#include "zigbee_han.h"
//
//#include "m2m_abstraction.h"
//#include "m2m_zcl_abstraction.h"
//#include "m2m_zcl_0x0000_abstraction.h"


/*************************************************************/
/* set of mock functions and extern variables */
int TraceDebug = 1;
int TraceLevel = 2;
//zigbeeHan_t *theHan;
void *MdlCfg;
int ActiveSniffer = 0;
int  TraceProto = 0;
int  PseudoOng = 0;
int  WithDia = 1;
int  Operating = 1;
int resetHanAtReboot = 0;
int  DiaTraceRequest = 0;
void *MainTbPoll;
void *MainIQ;
//azap_uint8 azapPort;


//// Mocked functions :
//azap_uint8 ieeeMock[IEEE_ADDR_LEN];
//bool azapZclOutgoingCmdReq(
//  azap_uint8 port,
//  AZAP_REQ_TTL_MGMT_MODE ttlMode,
//  void *issuerContext,
//  azap_uint16 nwkAddrDest,
//  azap_uint8 localEndp,
//  azap_uint8 remoteEndp,
//  azap_uint16 clusterId,
//  azap_uint8 clusterDir,
//  bool defaultResponse,
//  azap_uint8 cmdCode,
//  azap_uint8 expectedCmdCode,
//  azap_byte *cmdData,
//  azap_uint16 dataLen)
//{
//  printf("AZAP azapZclOutgoingCmdReq mock\n");
//  return true;
//}
//
//bool azapZdpMgmtPermitJoinReq(
//  azap_uint8 port,
//  AZAP_REQ_TTL_MGMT_MODE ttlMode,
//  void *issuerContext,
//  azap_uint16 nwkAddrDest,
//  azap_uint8 permitDuration,
//  bool tcSignificance,
//  bool responseExpected)
//{
//  printf("AZAP azapZdpMgmtPermitJoinReq mock\n");
//  return true;
//}
//
//bool azapZdpMgmtRtgReq(
//  azap_uint8 port,
//  AZAP_REQ_TTL_MGMT_MODE ttlMode,
//  void *issuerContext,
//  azap_uint16 nwkAddrDest,
//  azap_uint8 startIndex)
//{
//  printf("AZAP azapZdpMgmtRtgReq mock\n");
//  return true;
//}
//
//bool azapZdpMgmtBindReq(
//  azap_uint8 port,
//  AZAP_REQ_TTL_MGMT_MODE ttlMode,
//  void *issuerContext,
//  azap_uint16 nwkAddrDest,
//  azap_uint8 startIndex)
//{
//  printf("AZAP azapZdpMgmtBindReq mock\n");
//  return true;
//}
//
//bool azapZdpMgmtLeaveReq(
//  azap_uint8 port,
//  AZAP_REQ_TTL_MGMT_MODE ttlMode,
//  void *issuerContext,
//  azap_uint16 nwkAddrDest,
//  azap_uint8 deviceAddress[IEEE_ADDR_LEN],
//  bool removeChildren,
//  bool rejoin)
//{
//  printf("AZAP azapZdpMgmtLeaveReq mock\n");
//  return true;
//}
//
//bool azapZdpBindReq(
//  azap_uint8 port,
//  AZAP_REQ_TTL_MGMT_MODE ttlMode,
//  void *issuerContext,
//  azap_uint16 nwkAddrDest,
//  azap_uint8 srcAddress[IEEE_ADDR_LEN],
//  azap_uint8 srcEndp,
//  azap_uint16 clusterId,
//  azap_uint8 dstAddress[IEEE_ADDR_LEN],
//  azap_uint8 dstEndp)
//{
//  printf("AZAP azapZdpBindReq mock\n");
//  return true;
//}
//
//bool azapZdpUnbindReq(
//  azap_uint8 port,
//  AZAP_REQ_TTL_MGMT_MODE ttlMode,
//  void *issuerContext,
//  azap_uint16 nwkAddrDest,
//  azap_uint8 srcAddress[IEEE_ADDR_LEN],
//  azap_uint8 srcEndp,
//  azap_uint16 clusterId,
//  azap_uint8 dstAddress[IEEE_ADDR_LEN],
//  azap_uint8 dstEndp)
//{
//  printf("AZAP azapZdpUnbindReq mock\n");
//  return true;
//}
//
//bool azapZclReadCmdReq(
//  azap_uint8 port,
//  AZAP_REQ_TTL_MGMT_MODE ttlMode,
//  void *issuerContext,
//  azap_uint16 nwkAddrDest,
//  azap_uint8 localEndp,
//  azap_uint8 remoteEndp,
//  azap_uint16 clusterId,
//  azap_uint8 clusterDir,
//  azap_uint16 manufacturerCode,
//  azapZclReadCommand_t *zclReadCmd)
//{
//  printf("AZAP azapZclReadCmdReq mock\n");
//  return true;
//}
//
//bool azapZclDiscoverCmdReq(
//  azap_uint8 port,
//  AZAP_REQ_TTL_MGMT_MODE ttlMode,
//  void *issuerContext,
//  azap_uint16 nwkAddrDest,
//  azap_uint8 localEndp,
//  azap_uint8 remoteEndp,
//  azap_uint16 clusterId,
//  azap_uint8 clusterDir,
//  azapZclDiscoverCommand_t *zclDiscoverCmd)
//{
//  printf("AZAP azapZclDiscoverCmdReq mock\n");
//  return true;
//}
//
//
//
//azap_uint8 azapGetZclDataLength(azap_uint8 type, azap_uint8 *data)
//{
//  return 1;
//}
//
//int DoSerial()
//{
//  return 1;
//}
//
//azap_uint8 getMatchingAdminEndPoint(azap_uint16 profId)
//{
//  return 1;
//}
//
//azapZclCfgReportCommand_t *getCfgReportCmdFromCovConfig(void *itf, int nb,
//    azapZclDiscoverRspCommand_t *data)
//{
//  return NULL;
//}

/*************************************************************/

/**
 * Tests for doZclOperation
 */
void test_doZclOperation()
{
//  printf("******************************************\n");
//  printf("*********** test_doZclOperation **********\n");
//  azap_uint8 i;
//  for (i = 0; i < IEEE_ADDR_LEN; i++)
//  {
//    ieeeMock[i] = i;
//  }
//  zigbeeNode_t *node = new_zigbeeNode_t(0x0001, ieeeMock);
//  azapEndPoint_t *ep = new_azapEndPoint_t(1, 0x0104, 0, 0, 0, NULL, 0, NULL);
//  m2mZcl0x0000Abstraction_t *abstraction = new_m2mZcl0x0000Abstraction_t(ep->id);
//  assert( OK_ONE_WAY_REQUEST == abstraction->doZclOperation((m2mZclAbstraction_t *)abstraction,
//      COMMAND_BASIC_RESET_FACT_DEFAULT, node, ep, NULL, 1));
//  printf("\ntest_doZclOperation OK\n\n");
}

//void test_doRtlTimeFuncs()
//{
//  time_t now;
//  char szTemp[32];
//
//  rtl_timemono(&now);
//  printf("timemono:%d, formatted:%s\n", now, rtl_aaaammjjhhmmss(now, szTemp)); 
//  printf("timemono:%d, formatted:%s\n", rtl_timemono(now), rtl_aaaammjjhhmmss(
//    rtl_timemono(now), szTemp)); 
//  
//  printf("timemono:%d, formatted:%s\n", -1, rtl_aaaammjjhhmmss(-1, szTemp));
//
//}

void test_parseISO8601Duration()
{
  printf("parseISO8601Duration: PT240S=%d\n", parseISO8601Duration("PT240S"));
  printf("parseISO8601Duration: PT6M=%d\n", parseISO8601Duration("PT6M"));
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
  test_doZclOperation();
  //test_doRtlTimeFuncs();
  test_parseISO8601Duration();

  printf("\t ___\n");
  printf("\t/ __|_  _ __ __ ___ ______\n");
  printf("\t\\__ \\ || / _/ _/ -_|_-<_-<\n");
  printf("\t|___/\\_,_\\__\\__\\___/__/__/\n");
  printf("\t\n");

  return 0;
}


