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
//** File : driver/admcli.c
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

#include <stdio.h>      /************************************/
#include <stdlib.h>      /*     run in main thread           */
#include <string.h>      /************************************/
#include <stdarg.h>
#include <stddef.h>
#include <unistd.h>
#include <pthread.h>
#include <errno.h>
#include <limits.h>
#include <signal.h>
#include <poll.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <netdb.h>

#include "rtlbase.h"
#include "rtllist.h"
#include "rtlimsg.h"

#include "xoapipr.h"
#include "dIa.h"

#include "cmndefine.h"
#include "cmnstruct.h"

#include "cmnproto.h"
#include "speccproto.h"
#include "specextern.h"
#include "adaptor.h"

#include "azap_types.h"
#include "azap_tools.h"
#include "azap_api.h"
#include "azap_api_common.h"
#include "azap_api_zcl.h"
#include "azap_api_zdo.h"
#include "azap_zcl_attr_val.h"
#include "azap_attribute.h"
#include "azap_endpoint.h"
#include "interface_mapping_desc.h"
#include "driver_req_context.h"
#include "zigbee_node.h"
#include "m2m_zcl_abstraction.h"
#include "zigbee_han.h"
#include "driver_req_context.h"
#include "azap_listener.h"

extern t_cli *TbCli[];
extern int ActiveSniffer;
extern int TraceLevel;
extern int TraceDebug;
extern int TraceProto;
extern int WithDia;
extern void *MainIQ;
extern zigbeeHan_t *theHan;
extern void *MdlCfg;

///////////////////////////////////////////////////////////////////////////////
// Prototype of local functions
static void admUsage(t_cli *cl);
static void admInfo(t_cli *cl);
static int checkHexIntArg(t_cli *cl, char *arg, char *argname, int min,
    int max, int *val);
static int checkIntArg(t_cli *cl, char *arg, char *argname, int min, int max,
    int *val);
//static int checkStrArg(t_cli *cl, char * arg, char * argname, char * possibleVal,
//  char * val);

void doNodeCmd(t_cli *cl, char *buf);
void doSyncNodeCmd(t_cli *cl, char *buf);
void doTraceCmd(t_cli *cl, char *buf);
void doPermitJoinCmd(t_cli *cl, char *buf);
void doAttrAuditValueCmd(t_cli *cl, char *buf);
void doReloadMdlCmd(t_cli *cl);
void doDisplaySrvMdlCmd(t_cli *cl, char *buf);
void doConfigReportCmd(t_cli *cl, char *buf);
void doReadReportCfgCmd(t_cli *cl, char *buf);


///////////////////////////////////////////////////////////////////////////////
/**
 * @brief Display on CLI a help message about the supported commands.
 * @param cl the CLI context.
 */
static void admUsage(t_cli *cl)
{
  AdmPrint(cl, "quit|exit - quit the telnet session.\n");
  AdmPrint(cl, "help - display this help message.\n");
  AdmPrint(cl, "stop - shutdown the driver.\n");
  AdmPrint(cl, "who - display the list of active CLI connections.\n");
  AdmPrint(cl, "close - ends all CLI sessions.\n");
  AdmPrint(cl, "core - dump a core file of the current process.\n");
  AdmPrint(cl, "xoocc - display statistics on Xo usage.\n");
  AdmPrint(cl, "\n");
  AdmPrint(cl, "trace [level] - display the trace level or change it.\n");
  AdmPrint(cl, "getvar [varname] - display the internal variable(s) values.\n");
  AdmPrint(cl, "setvar <varname> [value] - change the value of internal variable.\n");
  AdmPrint(cl, "addvar <varname> - add a new internal variable.\n");
  AdmPrint(cl, "\n");
  AdmPrint(cl, "info - display general information on driver state.\n");
  AdmPrint(cl, "azapStats - display statistics regarding AZAP layer\n");
  AdmPrint(cl, "nodes - display information on known nodes.\n");
  AdmPrint(cl, "node <nwk addr> - display information on one known node.\n");
  AdmPrint(cl, "syncNode <nwk addr> - synchronize SCL tree with GIP collected infos.\n");
  AdmPrint(cl, "syncIpu - synchronize SCL tree with GIP IPU resources.\n");
  AdmPrint(cl, "syncNwk - synchronize SCL tree with GIP Network resources.\n");
  AdmPrint(cl, "\n");
  AdmPrint(cl, "reloadMdl - re-load the M2M resources modeling configuration file.\n");
  AdmPrint(cl, "displaySrvMdl <itf> - display the active modeling rules for a "
      "server interface.\n");
  AdmPrint(cl, "\n");
  AdmPrint(cl, "permitjoin <duration> - sends a ZigBee MGMT Permit Join request.\n");
  AdmPrint(cl, "configReport <nwk addr> <endpointId> <clusterId>\n");
  AdmPrint(cl, "    where <nwk addr> and <clusterId> are 16-bit integer hex format\n");
  AdmPrint(cl, "readReportCfg <nwk addr> <endpointId> <clusterId> <attrId>\n");
  AdmPrint(cl, "    where <nwk addr>, <clusterId> and <attrId> are 16-bit integer hex format\n");
  AdmPrint(cl, "attrAuditValue <nwk addr> <endpointId> <clusterId> <attributeId>\n");
  AdmPrint(cl, "    where <nwk addr>, <clusterId> and <attributeId> are 16-bit "
      "integer hex format\n");

}

/**
 * @brief Display on CLI the collected information on driver.
 * @param cl the CLI context.
 */
static void admInfo(t_cli *cl)
{
  unsigned long vsize;
  unsigned int nbm;
  unsigned int nbt;

  vsize  = rtl_vsize(getpid());

  nbm  = rtl_imsgVerifCount(MainIQ);
  nbt  = rtl_timerVerifCount(MainIQ);

  AdmPrint(cl, "vsize=%fMo (%u)\n", ((float)vsize) / 1024.0 / 1024.0, vsize);
  AdmPrint(cl, "sniffer=%d\n", ActiveSniffer);
  AdmPrint(cl, "pseudoong=%d withdia=%d\n", PseudoOng, WithDia);
  AdmPrint(cl, "#msg=%d\n", nbm);
  AdmPrint(cl, "#timer=%d\n", nbt);
  AdmPrint(cl, "diapending=%u(%u)\n", DiaNbReqPending, DiaMaxReqPending);
  AdmPrint(cl, "diasendcount=%u\n", DiaSendCount);
  AdmPrint(cl, "diarecvcount=%u\n", DiaRecvCount);
  AdmPrint(cl, "diatimecount=%u\n", DiaTimeCount);
  AdmPrint(cl, "diadisconnect=%u\n", DiaDisconnected());
}

/**
 * @brief Evaluate the read string argument as an hexa integer, and assign the value.
 * It also checks that the gathered value fit into the specified range of values.
 * @param cl the CLI context.
 * @param arg the read argument to evaluate.
 * @param argname the name of the argument.
 * @param min the minimal admitted value.
 * @param max the maximal admitted value.
 * @param val the placeholder where evaluated value is stored (by reference).
 * @return 1 if integer value can be retrieved successfully, 0 otherwise.
 */
static int checkHexIntArg(t_cli *cl, char *arg, char *argname, int min,
    int max, int *val)
{
  if (strlen(arg) == 0)
  {
    AdmPrint(cl, "Missing argument\n");
    return 0;
  }
  if (! sscanf(arg, "0x%x", val))
  {
    AdmPrint(cl, "Wrong argument format (%s=%s)\n", argname, arg);
    return 0;
  }
  if ((*val > max) || (*val < min))
  {
    AdmPrint(cl, "Wrong argument value (%s=%s)\n", argname, arg);
    return 0;
  }
  return 1;
}

/**
 * @brief Evaluate the read string argument as an decimal integer, and assign the value.
 * It also checks that the gathered value fit into the specified range of values.
 * @param cl the CLI context.
 * @param arg the read argument to evaluate.
 * @param argname the name of the argument.
 * @param min the minimal admitted value.
 * @param max the maximal admitted value.
 * @param val the placeholder where evaluated value is stored (by reference).
 * @return 1 if integer value can be retrieved successfully, 0 otherwise.
 */
static int checkIntArg(t_cli *cl, char *arg, char *argname, int min, int max,
    int *val)
{
  if (strlen(arg) == 0)
  {
    AdmPrint(cl, "Missing argument\n");
    return 0;
  }
  *val = atoi(arg);
  if ((*arg < '0') || (*arg > '9') || (*val > max) || (*val < min))
  {
    AdmPrint(cl, "Wrong argument (%s=%s)\n", argname, arg);
    return 0;
  }
  return 1;
}

///**
// * @brief Evaluate the read string argument character string, and assign the value.
// * It also checks that the gathered value fit with the specified possible values.
// * @param cl the CLI context.
// * @param arg the read argument to evaluate.
// * @param argname the name of the argument.
// * @param possibleVal if set, provides with the list of admitted values.
// * @param val the placeholder where evaluated value is stored (by reference).
// * @return 1 if argument value can be retrieved successfully, 0 otherwise.
// */
//static int checkStrArg(t_cli *cl, char * arg, char * argname, char * possibleVal,
//  char * val)
//{
//  char *b, *e;
//  char szTemp[32];
//  int found = 0;
//  if (strlen(arg) == 0)
//  {
//    AdmPrint(cl, "Missing argument\n");
//    return 0;
//  }
//  if ((possibleVal) && (*possibleVal))
//  {
//    b = possibleVal;
//    while ((*b) && (!found))
//    {
//      e = strchr(b, '|');
//      if (e)
//      {
//        strncpy(szTemp, b, e - b);
//        szTemp[e - b] = '\0';
//        e++;
//      }
//      else
//      {
//        e = b + strlen(b);
//      }
//      if (0 == strcmp(b, arg)) found = 1;
//      b = e;
//    }
//    if (!found)
//    {
//      AdmPrint(cl, "Wrong argument (%s=%s)\n", argname, arg);
//      return 0;
//    }
//  }
//  sprintf(val, "%s", arg);
//  return 1;
//}

/**
 * @brief Call back function invoked when a command does not match any built-in command.
 * @param cl the CLI context.
 * @param pbuf the entered command line.
 * @return an integer that indicates the action to perform next (e.g. display the prompt).
 */
int AdmCmd(t_cli *cl, char *pbuf)
{
  char data[1024];
  char *buf = pbuf;

  if  (!strlen(buf))
  {
    return CLI_PROMPT;
  }

  if  (strcmp(buf, "help") == 0 || buf[0] == '?')
  {
    admUsage(cl);
    return CLI_PROMPT;
  }
  if ( 0 == strcmp(buf, "version") )
  {
    AdmPrint(cl, "%s\n", rtl_version());
    AdmPrint(cl, "%s\n", XoVersion());
    AdmPrint(cl, "%s\n", dia_getVersion());
    AdmPrint(cl, "%s\n", azapVersion());
    AdmPrint(cl, "%s\n", GetAdaptorVersion());
    AdmPrint(cl, "@(#) TI Z-Stack %s\n", zStackVersion(0));

    return CLI_PROMPT;
  }
  if ( 0 == strcmp(buf, "info") )
  {
    admInfo(cl);
    return CLI_PROMPT;
  }
  if ( 0 == strcmp(buf, "azapStats") )
  {
    memset(data, 0, sizeof(data));
    azapDumpStatistics(data);
    AdmPrint(cl, data);
    return CLI_PROMPT;
  }
  if ( 0 == strcmp(buf, "nodes") )
  {
    theHan->dumpNodeList(theHan, AdmPrint, cl);
    return CLI_PROMPT;
  }

  if ( 0 == strncmp(buf, "node ", 5) )
  {
    doNodeCmd(cl, buf);
    return CLI_PROMPT;
  }

  if ( 0 == strncmp(buf, "syncNode ", 9) )
  {
    doSyncNodeCmd(cl, buf);
    return CLI_PROMPT;
  }

  if ( 0 == strcmp(buf, "syncIpu") )
  {
    DiaIpuStart();
    return CLI_PROMPT;
  }

  if ( 0 == strcmp(buf, "syncNwk") )
  {
    DiaNetStart();
    return CLI_PROMPT;
  }

  if ( 0 == strncmp(buf, "attrAuditValue ", 15) )
  {
    doAttrAuditValueCmd(cl, buf);
    return CLI_PROMPT;
  }

  if ( 0 == strncmp(buf, "configReport ", 13) )
  {
    doConfigReportCmd(cl, buf);
    return CLI_PROMPT;
  }

  if ( 0 == strncmp(buf, "readReportCfg ", 14) )
  {
    doReadReportCfgCmd(cl, buf);
    return CLI_PROMPT;
  }

  if ( 0 == strncmp(buf, "permitjoin ", 11) )
  {
    doPermitJoinCmd(cl, buf);
    return CLI_PROMPT;
  }

  if ( 0 == strncmp(buf, "trace", 5) )
  {
    doTraceCmd(cl, buf);
    return CLI_PROMPT;
  }

  if ( 0 == strcmp(buf, "reloadMdl") )
  {
    doReloadMdlCmd(cl);
    return CLI_PROMPT;
  }

  if ( 0 == strncmp(buf, "displaySrvMdl ", 14) )
  {
    doDisplaySrvMdlCmd(cl, buf);
    return CLI_PROMPT;
  }


  return CLI_NFOUND;
}

/**
 * Do evaluate and run the "node" CLI command.
 * @param cl the CLI context.
 * @param buf the entered command.
 */
void doNodeCmd(t_cli *cl, char *buf)
{
  char arg[32];
  int val;
  azap_uint16 nwkAddr;
  arg[0]  = '\0';
  buf = AdmAccept(buf);
  buf = AdmAdvance(buf);
  buf = AdmAcceptKeep(buf, arg);
  if (!checkHexIntArg(cl, arg, "nwk addr", 1, 0x0000fffc, &val))
  {
    return ;
  }
  nwkAddr = val;
  theHan->dumpNode(theHan, AdmPrint, cl, nwkAddr);
}

/**
 * Do evaluate and run the "syncNode" CLI command.
 * @param cl the CLI context.
 * @param buf the entered command.
 */
void doSyncNodeCmd(t_cli *cl, char *buf)
{
  char arg[32];
  int val;
  azap_uint16 nwkAddr;
  arg[0]  = '\0';
  buf = AdmAccept(buf);
  buf = AdmAdvance(buf);
  buf = AdmAcceptKeep(buf, arg);
  if (!checkHexIntArg(cl, arg, "nwk addr", 1, 0x0000fffc, &val))
  {
    return ;
  }
  nwkAddr = val;
  theHan->syncNode(theHan, AdmPrint, cl, nwkAddr);
}

/**
 * Do evaluate and run the "trace" CLI command.
 * @param cl the CLI context.
 * @param buf the entered command.
 */
void doTraceCmd(t_cli *cl, char *buf)
{
  int  v;
  buf  += 5;
  buf  = AdmAdvance(buf);
  if  (strlen(buf))
  {
    v  = atoi(buf);
    TraceLevel  = v;
    rtl_tracelevel(TraceLevel);
  }
  AdmPrint(cl, "trace=%d debug=%d proto=%d\n",
      TraceLevel, TraceDebug, TraceProto);
}

/**
 * Do evaluate and run the "permitjoin" CLI command.
 * @param cl the CLI context.
 * @param buf the entered command.
 */
void doPermitJoinCmd(t_cli *cl, char *buf)
{
  char arg[32];
  int duration;
  driverReqContext_t *ctxtCoord, *ctxtBroadcast;

  arg[0]  = '\0';
  buf = AdmAccept(buf);
  buf = AdmAdvance(buf);
  buf = AdmAcceptKeep(buf, arg);
  buf = AdmAdvance(buf);
  if (strlen(arg) == 0)
  {
    AdmPrint(cl, "Missing argument\n");
    return ;
  }

  duration = atoi(arg);
  if ((*arg < '0') || (*arg > '9') || (duration > 255) || duration < 0)
  {
    AdmPrint(cl, "Wrong argument (%s)\n", arg);
    return ;
  }

  if (duration == 255)
  {
    theHan->computeNextPermitJoinDuration(theHan, INT_MAX);
  }
  else
  {
    theHan->computeNextPermitJoinDuration(theHan, duration);
  }


  // we have to send two ZDO PermitJoin requests:
  // one to the coordinator
  ctxtCoord = new_driverReqContext_t(DRV_REQ_FROM_CLI);
  ctxtCoord->issuerData = (void *)((long)(cl->cl_fd));
  azapZdpMgmtPermitJoinReq(0, AZAP_REQ_TTL_START_AT_API, ctxtCoord, 0x0000,
      duration, true, true);

  // and one is to broadcast to the entire network
  ctxtBroadcast = new_driverReqContext_t(DRV_REQ_FROM_CLI);
  ctxtBroadcast->issuerData = (void *)((long)(cl->cl_fd));
  azapZdpMgmtPermitJoinReq(0, AZAP_REQ_TTL_START_AT_API, ctxtBroadcast,
      0xFFFF, duration, true, true);
}

/**
 * Do evaluate and run the attrAuditValue CLI command.
 * @param cl the CLI context.
 * @param buf the entered command.
 */
void doAttrAuditValueCmd(t_cli *cl, char *buf)
{
  // attrAuditValue <nwk addr> <endpointId> <clusterId> <attributeId>
  zigbeeNode_t *node = NULL;
  azapEndPoint_t *ep = NULL;
  azapAttribute_t *attr = NULL;
  char arg[32];
  int val;
  azap_uint16 nwkAddr;
  azap_uint8 epId;
  azap_uint16 clId;
  azap_uint16 attrId;

  arg[0]  = '\0';
  buf = AdmAccept(buf);
  buf = AdmAdvance(buf);
  buf = AdmAcceptKeep(buf, arg);
  if (!checkHexIntArg(cl, arg, "nwk addr", 1, 0x0000fffc, &val))
  {
    return ;
  }
  nwkAddr = val;
  buf = AdmAdvance(buf);
  buf = AdmAcceptKeep(buf, arg);
  if (!checkIntArg(cl, arg, "endpointId", 1, 240, &val))
  {
    return ;
  }
  epId = val;
  buf = AdmAdvance(buf);
  buf = AdmAcceptKeep(buf, arg);
  if (!checkHexIntArg(cl, arg, "clusterId", 0, 0x0000ffff, &val))
  {
    return ;
  }
  clId = val;
  buf = AdmAdvance(buf);
  buf = AdmAcceptKeep(buf, arg);
  if (!checkHexIntArg(cl, arg, "attributeId", 0, 0x0000ffff, &val))
  {
    return ;
  }
  attrId = val;
  buf = AdmAdvance(buf);

  node = theHan->getNodeByNwkAddr(theHan, nwkAddr);
  if (!node)
  {
    AdmPrint(cl, "no matching node found (nwkAddr:0x%.4x)\n", nwkAddr);
    return ;
  }
  ep = node->getEndPoint(node, epId);
  if (!ep)
  {
    AdmPrint(cl, "no matching end point found (ep:%d)\n", epId);
    return ;
  }
  attr = ep->getAttribute(ep, clId, attrId);
  if ((!attr) || (!attr->value))
  {
    AdmPrint(cl, "no value found for this attribute (clusterId:0x%.4x) "
        "(attributeId:0x%.4x) (attr:%p)\n", clId, attrId, attr);
    return ;
  }
  AdmPrint(cl, "value = %s\n", attr->value->getStringRep(attr->value));
  return ;
}

/**
 * Do evaluate and run the reladMdl CLI command.
 * @param cl the CLI context.
 */
void doReloadMdlCmd(t_cli *cl)
{
  int hot;
  void *newMdl = MdlLoad(hot = 1);
  if (!newMdl)
  {
    AdmPrint(cl, "Unable to load modelconfig.xml file !\n");
    return ;
  }
  MdlCfg = newMdl;
  AdmPrint(cl, "Done\n");
}

/**
 * Do evaluate and run the "displaySrvMdl" CLI command.
 * @param cl the CLI context.
 * @param buf the entered command.
 */
void doDisplaySrvMdlCmd(t_cli *cl, char *buf)
{
  char arg[32];
  char line[256];
  int val;
  azap_uint16 clusterId;
  void *mdlitf;
  void *xoCtxt;
  char *xmlBuf;
  char *pPtrBegin;
  char *pPtrEnd;

  arg[0]  = '\0';
  buf = AdmAccept(buf);
  buf = AdmAdvance(buf);
  buf = AdmAcceptKeep(buf, arg);
  if (!checkHexIntArg(cl, arg, "cluster ID", 0, 0x0000ffff, &val))
  {
    return ;
  }
  clusterId = val;
  mdlitf = MdlGetInterface(MdlCfg, NULL, "server", azap_uint16ToString(clusterId, true));
  if (!mdlitf)
  {
    AdmPrint(cl, "No modeling found for this server interface (clusterId:0x%.4x)\n",
        clusterId);
    return ;
  }

  xoCtxt = XoWritXmlMem(mdlitf, 0, &xmlBuf, "");
  if (! xoCtxt)
  {
    AdmPrint(cl, "ERROR - unable to serialize Xo buffer into XML format (%s)\n",
        XoNmType(mdlitf));
    return ;
  }

  XoWritXmlFreeCtxt(xoCtxt);

  pPtrBegin = xmlBuf;
  while ((pPtrEnd = strstr(pPtrBegin, ">")))
  {
    snprintf(line, pPtrEnd - pPtrBegin + 2, pPtrBegin);
    line[pPtrEnd - pPtrBegin + 3] = 0;
    AdmPrint(cl, "%s\n", line);
    pPtrBegin = pPtrEnd + 1;
  }
  AdmPrint(cl, "%s\n", pPtrBegin);
  free(xmlBuf);
}

/**
 * Do evaluate and run the configReport CLI command.
 * @param cl the CLI context.
 * @param buf the entered command.
 */
void doConfigReportCmd(t_cli *cl, char *buf)
{
  // configReport <nwk addr> <endpointId> <clusterId>
  zigbeeNode_t *node = NULL;
  azapEndPoint_t *ep = NULL;
  azapAttribute_t *attr = NULL;
  char arg[32];
  int val;
  int nb;
  int i;
  azap_uint16 nwkAddr;
  azap_uint8 epId;
  azap_uint16 clId;
  void *itf;
  void *covConfig;
  driverReqContext_t *azapBindCtxt;
  driverReqContext_t *azapCfgReportCtxt;
  m2mZclAbstraction_t *abstraction;
  azapZclCfgReportCommand_t *cfgReportCmd;
  azapZclDiscoverRspCommand_t *discoverRspCmd;
  char *filter;
  int attrId;

  arg[0]  = '\0';
  buf = AdmAccept(buf);
  buf = AdmAdvance(buf);
  buf = AdmAcceptKeep(buf, arg);
  if (!checkHexIntArg(cl, arg, "nwk addr", 1, 0x0000fffc, &val))
  {
    return ;
  }
  nwkAddr = val;
  buf = AdmAdvance(buf);
  buf = AdmAcceptKeep(buf, arg);
  if (!checkIntArg(cl, arg, "endpointId", 1, 240, &val))
  {
    return ;
  }
  epId = val;
  buf = AdmAdvance(buf);
  buf = AdmAcceptKeep(buf, arg);
  if (!checkHexIntArg(cl, arg, "clusterId", 0, 0x0000ffff, &val))
  {
    return ;
  }
  clId = val;
  buf = AdmAdvance(buf);

  node = theHan->getNodeByNwkAddr(theHan, nwkAddr);
  if (!node)
  {
    AdmPrint(cl, "no matching node found (nwkAddr:0x%.4x)\n", nwkAddr);
    return ;
  }
  ep = node->getEndPoint(node, epId);
  if (!ep)
  {
    AdmPrint(cl, "no matching end point found (ep:%d)\n", epId);
    return ;
  }

  itf = MdlGetInterface(MdlCfg, NULL, "server", azap_uint16ToString(clId, true));
  if (!itf)
  {
    AdmPrint(cl, "no matching interface found in modelconfig.xml file (clusterId:0x%.4x)\n",
        clId);
    return ;
  }

  if ( 0 == (nb = MdlGetNbCovConfigurationEntries(itf)) )
  {
    AdmPrint(cl, "no matching COV configuration found for this interface (clusterId:0x%.4x)\n",
        clId);
    return ;
  }

  AdmPrint(cl, "Sending ZDO Bind request...\n");
  azapBindCtxt = new_driverReqContext_t(DRV_REQ_FROM_CLI);
  azapBindCtxt->issuerData = (void *)((long)(cl->cl_fd));
  if (! azapZdpBindReq(0, AZAP_REQ_TTL_START_AT_ZB_SEND, azapBindCtxt, nwkAddr, node->ieeeAddr,
      epId, clId, theHan->coordExtAddr, getMatchingAdminEndPoint(ep->appProfId)))
  {
    AdmPrint(cl, "Failed to send ZDO bind request\n");
    return;
  }

  // Build a fake discover rsp
  discoverRspCmd = malloc(
      sizeof(azapZclDiscoverRspCommand_t) + (nb * sizeof(azapZclDiscoverAttr_t)));
  memset(discoverRspCmd, 0, sizeof(azapZclDiscoverRspCommand_t) +
      (nb * sizeof(azapZclDiscoverAttr_t)));
  discoverRspCmd->discComplete = 1;
  discoverRspCmd->numAttr = nb;
  for (i = 0; i < nb; i++)
  {
    covConfig = MdlGetCovConfigurationByNumEntry(itf, i);
    if (!covConfig)
    {
      continue;
    }

    filter = XoNmGetAttr(covConfig, "ong:filter", 0, 0);
    if (!filter)
    {
      continue;
    }
    if (0 == sscanf(filter, "zigbee:/attribute/0x%04x", &attrId))
    {
      continue;
    }

    attr = ep->getAttribute(ep, clId, attrId);
    if ((!attr) || (!attr->value))
    {
      continue;
    }

    discoverRspCmd->attrList[i].attrId = attrId;
    discoverRspCmd->attrList[i].dataType = attr->value->zigbeeDataType;
  }

  cfgReportCmd = getCfgReportCmdFromCovConfig(itf, nb, discoverRspCmd);
  // retrieve associated abstraction
  abstraction = node->getM2mZclAbstraction(node, epId, clId);
  // and propagate the configuration to the abstraction if found
  if (abstraction)
  {
    abstraction->configureReporting(abstraction, cfgReportCmd);
  }
  // and finally send the ZCL ConfigureReporting command
  if (!cfgReportCmd)
  {
    AdmPrint(cl, "Failed to create configure reporting data structure\n");
    return;
  }
  AdmPrint(cl, "Sending ZCL Configure Reporting request...\n");
  azapCfgReportCtxt = new_driverReqContext_t(DRV_REQ_FROM_CLI);
  azapCfgReportCtxt->issuerData = (void *)((long)(cl->cl_fd));
  if (! azapZclConfigureReportCmdReq(0, AZAP_REQ_TTL_START_AT_ZB_SEND,
      azapCfgReportCtxt, nwkAddr, getMatchingAdminEndPoint(ep->appProfId), epId,
      clId, ZCL_FRAME_CLIENT_SERVER_DIR, cfgReportCmd))
  {
    AdmPrint(cl, "Failed to send ZCL configure reporting request\n");
    return;
  }

  AdmPrint(cl, "Now make sure that the device is awake to receive the requests !\n");
  return ;
}

/**
 * Do evaluate and run the readReportCfg CLI command.
 * @param cl the CLI context.
 * @param buf the entered command.
 */
void doReadReportCfgCmd(t_cli *cl, char *buf)
{
  // readReportCfg <nwk addr> <endpointId> <clusterId> <attrId>
  zigbeeNode_t *node = NULL;
  azapEndPoint_t *ep = NULL;
  char arg[32];
  int val;
  int nb;
  azap_uint16 nwkAddr;
  azap_uint8 epId;
  azap_uint16 clId;
  azap_uint16 attrId;
  void *itf;
  driverReqContext_t *azapReadReportCfgCtxt;
  azapZclReadReportCfgCommand_t *readReportCfgCmd;

  arg[0]  = '\0';
  buf = AdmAccept(buf);
  buf = AdmAdvance(buf);
  buf = AdmAcceptKeep(buf, arg);
  if (!checkHexIntArg(cl, arg, "nwk addr", 1, 0x0000fffc, &val))
  {
    return ;
  }
  nwkAddr = val;
  buf = AdmAdvance(buf);
  buf = AdmAcceptKeep(buf, arg);
  if (!checkIntArg(cl, arg, "endpointId", 1, 240, &val))
  {
    return ;
  }
  epId = val;
  buf = AdmAdvance(buf);
  buf = AdmAcceptKeep(buf, arg);
  if (!checkHexIntArg(cl, arg, "clusterId", 0, 0x0000ffff, &val))
  {
    return ;
  }
  clId = val;
  buf = AdmAdvance(buf);
  buf = AdmAcceptKeep(buf, arg);
  if (!checkHexIntArg(cl, arg, "attrId", 0, 0x0000ffff, &val))
  {
    return ;
  }
  attrId = val;
  buf = AdmAdvance(buf);

  node = theHan->getNodeByNwkAddr(theHan, nwkAddr);
  if (!node)
  {
    AdmPrint(cl, "no matching node found (nwkAddr:0x%.4x)\n", nwkAddr);
    return ;
  }
  ep = node->getEndPoint(node, epId);
  if (!ep)
  {
    AdmPrint(cl, "no matching end point found (ep:%d)\n", epId);
    return ;
  }

  itf = MdlGetInterface(MdlCfg, NULL, "server", azap_uint16ToString(clId, true));
  if (!itf)
  {
    AdmPrint(cl, "no matching interface found in modelconfig.xml file (clusterId:0x%.4x)\n",
        clId);
    return ;
  }

  if ( 0 == (nb = MdlGetNbCovConfigurationEntries(itf)) )
  {
    AdmPrint(cl, "no matching COV configuration found for this interface (clusterId:0x%.4x)\n",
        clId);
    return ;
  }

  AdmPrint(cl, "Sending ZCL Read Reporting Configuration request...\n");
  azapReadReportCfgCtxt = new_driverReqContext_t(DRV_REQ_FROM_CLI);
  azapReadReportCfgCtxt->issuerData = (void *)((long)(cl->cl_fd));
  readReportCfgCmd = malloc(
      sizeof(azapZclReadReportCfgCommand_t) + sizeof(azapZclReadReportCfgAttr_t));
  memset(readReportCfgCmd, 0, sizeof(azapZclReadReportCfgCommand_t) +
      sizeof(azapZclReadReportCfgAttr_t));
  readReportCfgCmd->numAttr = 1;
  readReportCfgCmd->attrList[0].attrId = attrId;
  readReportCfgCmd->attrList[0].direction = ZCL_FRAME_CLIENT_SERVER_DIR;

  if (! azapZclReadReportConfigCmdReq(0, AZAP_REQ_TTL_START_AT_ZB_SEND,
      azapReadReportCfgCtxt, nwkAddr, getMatchingAdminEndPoint(ep->appProfId), epId,
      clId, ZCL_FRAME_CLIENT_SERVER_DIR, readReportCfgCmd))
  {
    AdmPrint(cl, "Failed to send ZCL Read Reporting Configuration request\n");
    return;
  }

  AdmPrint(cl, "Now make sure that the device is awake to receive the requests !\n");
  return ;
}


/**
 * @brief Call back function invoked when a telnet session ends.
 * It allows freeing resources.
 * @param cl the CLI context.
 */
void AdmCliFree(t_cli *cl)
{
  if  (!cl)
  {
    return;
  }

  strcpy  (cl->cl_sensor, "");
  //AdmSnifferCount();
}


