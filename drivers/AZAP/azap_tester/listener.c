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
//** File : ./azap_tester/listener.c
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


/**
 * @file listener.c
 * @brief Implementation of completion methods provided in t_azap_api_callbacks for AZAP init.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdarg.h>
#include <stddef.h>

#include "rtlbase.h"
#include "rtllist.h"

#include "azap_api.h"
#include "azap_tools.h"

#include "listener.h"
#include "admin_cli.h"
#include "tester_req_context.h"
#include "azap_zcl_attr_val.h"

/* Global variables */
static t_azap_api_callbacks *g_pCallbacks = NULL;

/* Prototypes for local functions */
static void initCallbacks(t_azap_api_callbacks *pCb);
static void processAlarm(azap_uint16 nwkAddrSrc, azap_uint8 endPoint,
    azap_uint8 cmdCode, azap_byte cmdData[], azap_uint8 cmdLen);

static azap_uint8 myIeeeAddr[IEEE_ADDR_LEN];
static azap_uint8 lastAuditedNodeIeeeAddr[IEEE_ADDR_LEN];

#define MAX_NB_KNOWN_DEVICES 255
static azap_uint8 nbKnownDevices = 0;
static azap_uint16 knownDevices[MAX_NB_KNOWN_DEVICES];


static void completionOutput(testerReqContext_t *cntxt, char *fmt, ...)
{
  va_list listArg;
  char tmp[1024];

  va_start(listArg, fmt);
  vsprintf(tmp, fmt, listArg);
  va_end(listArg);

  if (!cntxt)
  {
    printf(tmp);
  }
  else
  {
    switch (cntxt->type)
    {
      case TSTER_REQ_FROM_CLI:
        AdmPrint((t_cli *)cntxt->issuerData, tmp);
        break;
      default:
        printf(tmp);
        fflush(stdout);
        break;
    }
  }
}

/**
 * @brief Get the listener object, which is a singleton.
 * @return the single program listener.
 */
t_azap_api_callbacks *getListener()
{
  if (NULL == g_pCallbacks)
  {
    g_pCallbacks = new_t_azap_api_callbacks();
    initCallbacks(g_pCallbacks);
  }
  return g_pCallbacks;
}

/**
 * Initialize the call back methods for AZAP layer notifications.
 * @param pCb the structure that convey the pointers on call back methods.
 */
static void initCallbacks(t_azap_api_callbacks *pCb)
{
  pCb->azapStartedCb = azapTesterStartedCb;
  pCb->azapZdpBindReqCb = azapTesterZdpBindReqCb;
  pCb->azapZdpUnbindReqCb = azapTesterZdpUnbindReqCb;
  pCb->azapZdpDeviceAnnceCb = azapTesterZdpDeviceAnnceCb;
  pCb->azapZdpNwkAddrRspCb = azapTesterZdpNwkAddrRspCb;
  pCb->azapZdpIeeeAddrRspCb = azapTesterZdpIeeeAddrRspCb;
  pCb->azapZdpNodeDescRspCb = azapTesterZdpNodeDescRspCb;
  pCb->azapZdpPowerDescRspCb = azapTesterZdpPowerDescRspCb;
  pCb->azapZdpSimpleDescRspCb = azapTesterZdpSimpleDescRspCb;
  pCb->azapZdpActiveEPRspCb = azapTesterZdpActiveEPRspCb;
  pCb->azapZdpBindRspCb = azapTesterZdpBindRspCb;
  pCb->azapZdpUnbindRspCb = azapTesterZdpUnbindRspCb;
  pCb->azapZdpMgmtRtgRspCb = azapTesterZdpMgmtRtgRspCb;
  pCb->azapZdpMgmtBindRspCb = azapTesterZdpMgmtBindRspCb;
  pCb->azapZdpMgmtPermitJoinRspCb = azapTesterZdpMgmtPermitJoinRspCb;
  pCb->azapZdpMgmtLeaveRspCb = azapTesterZdpMgmtLeaveRspCb;
  pCb->azapSapiFindDeviceConfCb = azapTesterSapiFindDeviceConfCb;
  pCb->azapZclReadCmdRspCb = azapTesterZclReadCmdRspCb;
  pCb->azapZclWriteCmdRspCb = azapTesterZclWriteCmdRspCb;
  pCb->azapZclConfigureReportCmdRspCb = azapTesterZclConfigureReportCmdRspCb;
  pCb->azapZclReportCmdCb = azapTesterZclReportCmdCb;
  pCb->azapZclDiscoverCmdRspCb = azapTesterZclDiscoverCmdRspCb;
  pCb->azapZclIncomingCmdCb = azapTesterZclIncomingCmdCb;
  pCb->azapZclConfigureReportCmdCb = azapTesterZclConfigureReportCmdCb;
  pCb->azapZclReadReportConfigCmdRspCb = azapTesterZclReadReportConfigCmdRspCb;
}

/**
 * Call back for start completion notification.
 * @param port the reference port number
 * @param success the start completion result; i.e. 1 if the ZigBee device on this
 * port managed to form or to join a PAN, 0 otherwise.
 * @param shortAddr the short address within the formed PAN associated to the ZigBee
 * device on this port.
 * @param deviceIEEE the extended address associated to the ZigBee device on this port.
 * @param panId the PAN identifier the ZigBee device belongs to.
 * @param panIdExt the extended address associated to PAN the ZigBee device belongs to.
 */
void azapTesterStartedCb(
  azap_uint8 port,
  bool success,
  azap_uint16 shortAddr,
  azap_uint8 deviceIEEE[IEEE_ADDR_LEN],
  azap_uint16 panId,
  azap_uint8 panIdExt[IEEE_ADDR_LEN])
{
  completionOutput(NULL,
      "AZAP started on port %d (success:%d) (shortAddr:0x%.4x) (deviceIEEE:%s) "
      "(panId:0x%.4x) (panIdExt:%s)\n", port, success, shortAddr,
      azap_extAddrToString(deviceIEEE, true), panId, azap_extAddrToString(panIdExt, true));

  memcpy(myIeeeAddr, deviceIEEE, IEEE_ADDR_LEN);
  nbKnownDevices = 0;
  memset(knownDevices, 0, MAX_NB_KNOWN_DEVICES);
}


void azapTesterZdpBindReqCb (
  azap_uint8 port,
  void *issuerContext,
  azap_uint16 nwkAddrSrc,
  azap_uint8 srcIeeeAddr[IEEE_ADDR_LEN],
  azap_uint8 srcEndPoint,
  azap_uint16 clusterId,
  azap_uint8 destIeeeAddr[IEEE_ADDR_LEN],
  azap_uint8 destEndPoint)
{
  testerReqContext_t *ctxt = (testerReqContext_t *)issuerContext;
  completionOutput(issuerContext, "AZAP Tester ZdpBindReqCb\n");

  if (ctxt)
  {
    ctxt->free(ctxt);
  }
}

void azapTesterZdpUnbindReqCb(
  azap_uint8 port,
  void *issuerContext,
  azap_uint16 nwkAddrSrc,
  azap_uint8 srcIeeeAddr[IEEE_ADDR_LEN],
  azap_uint8 srcEndPoint,
  azap_uint16 clusterId,
  azap_uint8 destIeeeAddr[IEEE_ADDR_LEN],
  azap_uint8 destEndPoint)
{
  testerReqContext_t *ctxt = (testerReqContext_t *)issuerContext;
  completionOutput(issuerContext, "AZAP Tester ZdpUnbindReqCb\n");
  if (ctxt)
  {
    ctxt->free(ctxt);
  }
}

void azapTesterZdpDeviceAnnceCb(
  azap_uint8 port,
  azap_uint16 nwkAddr,
  azap_uint8 ieeeAddr[IEEE_ADDR_LEN],
  azap_uint8 alternatePanCoordinator,
  azap_uint8 deviceType,
  azap_uint8 powerSource,
  azap_uint8 receiverOnWhenIdle,
  azap_uint8 securityCapability,
  azap_uint8 allocateAddress)
{
  int i;
  bool alreadyKnown = false;
  completionOutput(NULL, "AZAP Tester ZdpDeviceAnnceCb\n");

  for (i = 0; i < nbKnownDevices; i++)
  {
    if (knownDevices[i] == nwkAddr)
    {
      alreadyKnown = true;
    }
  }

  if (! alreadyKnown)
  {
    memcpy(lastAuditedNodeIeeeAddr, ieeeAddr, IEEE_ADDR_LEN);
    azapZdpIeeeAddrReq(port, AZAP_REQ_TTL_START_AT_ZB_SEND, NULL, nwkAddr, nwkAddr, 1, 0);
    knownDevices[nbKnownDevices] = nwkAddr;
    nbKnownDevices++;
  }
  else
  {
    completionOutput(NULL,
        "AZAP Tester ZdpDeviceAnnceCb - device is already known by the system "
        "(nwkAddr:0x%.4x)\n", nwkAddr);
  }
}

void azapTesterZdpNwkAddrRspCb (
  azap_uint8 port,
  void *issuerContext,
  azap_uint16 nwkAddrSrc,
  azap_uint8 status,
  azap_uint8 ieeeAddrRemoteDev[IEEE_ADDR_LEN],
  azap_uint16 nwkAddrRemoteDev,
  azap_uint8 numAssocDev,
  azap_uint8 startIndex,
  azap_uint16 nwkAddrAssocDevList[])
{
  testerReqContext_t *ctxt = (testerReqContext_t *)issuerContext;
  completionOutput(issuerContext, "AZAP Tester ZdpNwkAddrRspCb\n");
  if (ctxt)
  {
    ctxt->free(ctxt);
  }
}

void azapTesterZdpIeeeAddrRspCb (
  azap_uint8 port,
  void *issuerContext,
  azap_uint16 nwkAddrSrc,
  azap_uint8 status,
  azap_uint8 ieeeAddrRemoteDev[IEEE_ADDR_LEN],
  azap_uint16 nwkAddrRemoteDev,
  azap_uint8 numAssocDev,
  azap_uint8 startIndex,
  azap_uint16 nwkAddrAssocDevList[])
{
  testerReqContext_t *ctxt = (testerReqContext_t *)issuerContext;
  completionOutput(issuerContext, "AZAP Tester ZdpIeeeAddrRspCb\n");
  completionOutput(issuerContext, "* status: %d\n", status);
  completionOutput(issuerContext, "* ieeeAddrRemoteDev: %s\n",
      azap_extAddrToString(ieeeAddrRemoteDev, true));
  completionOutput(issuerContext, "* nwkAddrRemoteDev: 0x%.4x\n", nwkAddrRemoteDev);
  completionOutput(issuerContext, "* numAssocDev: %d\n", numAssocDev);
  completionOutput(issuerContext, "* startIndex: 0x%.4x\n", startIndex);
  completionOutput(issuerContext, "* nwkAddrAssocDevList: %s\n",
      azap_uint16ArrayToString(nwkAddrAssocDevList, numAssocDev));

  if (!issuerContext)
  {
    azapZdpNodeDescReq(port, AZAP_REQ_TTL_START_AT_ZB_SEND, NULL, nwkAddrSrc, nwkAddrSrc);
  }
  if (ctxt)
  {
    ctxt->free(ctxt);
  }
}

void azapTesterZdpNodeDescRspCb (
  azap_uint8 port,
  void *issuerContext,
  azap_uint16 nwkAddrSrc,
  azap_uint8 status,
  azap_uint16 nwkAddrOfInterest,
  azap_uint8 logicalType,
  azap_uint8 complexDescriptorAvailable,
  azap_uint8 userDescriptorAvailable,
  azap_uint8 apsFlags,
  azap_uint8 frequencyBand,
  azap_uint8 macCapabilityFlags,
  azap_uint16 manufacturerCode,
  azap_uint8 maximumBufferSize,
  azap_uint16 maximumIncomingTransferSize,
  azap_uint16 serverMask,
  azap_uint16 maximumOutgoingTransferSize,
  azap_uint8 descriptorCapabilityField)
{
  testerReqContext_t *ctxt = (testerReqContext_t *)issuerContext;
  completionOutput(issuerContext, "AZAP Tester ZdpNodeDescRspCb\n");
  azapZdpPowerDescReq(port, AZAP_REQ_TTL_START_AT_ZB_SEND, NULL, nwkAddrSrc, nwkAddrSrc);
  if (ctxt)
  {
    ctxt->free(ctxt);
  }
}

void azapTesterZdpPowerDescRspCb (
  azap_uint8 port,
  void *issuerContext,
  azap_uint16 nwkAddrSrc,
  azap_uint8 status,
  azap_uint16 nwkAddrOfInterest,
  azap_uint8 currentPowerMode,
  azap_uint8 availablePowerSources,
  azap_uint8 currentPowerSource,
  azap_uint8 currentPowerSourceLevel)
{
  testerReqContext_t *ctxt = (testerReqContext_t *)issuerContext;
  completionOutput(issuerContext, "AZAP Tester ZdpPowerDescRspCb\n");
  azapZdpActiveEPReq(port, AZAP_REQ_TTL_START_AT_ZB_SEND, NULL, nwkAddrSrc, nwkAddrSrc);
  if (ctxt)
  {
    ctxt->free(ctxt);
  }
}

void azapTesterZdpSimpleDescRspCb (
  azap_uint8 port,
  void *issuerContext,
  azap_uint16 nwkAddrSrc,
  azap_uint8 status,
  azap_uint16 nwkAddrOfInterest,
  azap_uint8 endpoint,
  azap_uint16 applicationProfileIdentifier,
  azap_uint16 applicationDeviceIdentifier,
  azap_uint8 applicationDeviceVersion,
  azap_uint8 applicationInputClusterCount,
  azap_uint16 applicationInputClusterList[],
  azap_uint8 applicationOutputClusterCount,
  azap_uint16 applicationOutputClusterList[])
{
  testerReqContext_t *ctxt = (testerReqContext_t *)issuerContext;
  azap_uint8 i;

  completionOutput(issuerContext, "AZAP Tester ZdpSimpleDescRspCb\n");
  if (applicationInputClusterCount > 0)
  {
    azapZclDiscoverCommand_t zclCmd;
    zclCmd.startAttrId = 0;
    zclCmd.maxNumAttrIds = 0xFF;
    azapZclDiscoverCmdReq(port, AZAP_REQ_TTL_START_AT_ZB_SEND, NULL, nwkAddrSrc,
        0x01, endpoint, applicationInputClusterList[0], ZCL_FRAME_CLIENT_SERVER_DIR, &zclCmd);

    // look for configurable clusters on which we can expect reporting
    for (i = 0; i < applicationInputClusterCount; i++)
    {
      if (ZCL_CLUSTER_ID_MS_TEMPERATURE_MEASUREMENT == applicationInputClusterList[i])
      {
        testerReqContext_t *ctxt = new_testerReqContext_t(TSTER_REQ_AUDIT);
        ctxt->clusterId = applicationInputClusterList[i];
        ctxt->srcEP = endpoint;
        ctxt->dstEP = 0x01;

        // ZDO Bind request first
        azapZdpBindReq(port, AZAP_REQ_TTL_START_AT_ZB_SEND, ctxt, nwkAddrSrc,
            lastAuditedNodeIeeeAddr, endpoint, ZCL_CLUSTER_ID_MS_TEMPERATURE_MEASUREMENT,
            myIeeeAddr, 0x01);

      }
      else if (ZCL_CLUSTER_ID_MS_RELATIVE_HUMIDITY == applicationInputClusterList[i])
      {
        testerReqContext_t *ctxt = new_testerReqContext_t(TSTER_REQ_AUDIT);
        ctxt->clusterId = applicationInputClusterList[i];
        ctxt->srcEP = endpoint;
        ctxt->dstEP = 0x01;

        // ZDO Bind request first
        azapZdpBindReq(port, AZAP_REQ_TTL_START_AT_ZB_SEND, ctxt, nwkAddrSrc,
            lastAuditedNodeIeeeAddr, endpoint, ZCL_CLUSTER_ID_MS_RELATIVE_HUMIDITY,
            myIeeeAddr, 0x01);

      }
      else if (ZCL_CLUSTER_ID_SE_SIMPLE_METERING == applicationInputClusterList[i])
      {
        testerReqContext_t *ctxt = new_testerReqContext_t(TSTER_REQ_AUDIT);
        ctxt->clusterId = applicationInputClusterList[i];
        ctxt->srcEP = endpoint;
        ctxt->dstEP = 0x01;

        // ZDO Bind request first
        azapZdpBindReq(port, AZAP_REQ_TTL_START_AT_ZB_SEND, ctxt, nwkAddrSrc,
            lastAuditedNodeIeeeAddr, endpoint, ZCL_CLUSTER_ID_SE_SIMPLE_METERING,
            myIeeeAddr, 0x01);

      }
    }
  }

  if (ctxt)
  {
    ctxt->free(ctxt);
  }
}

void azapTesterZdpActiveEPRspCb (
  azap_uint8 port,
  void *issuerContext,
  azap_uint16 nwkAddrSrc,
  azap_uint8 status,
  azap_uint16 nwkAddrOfInterest,
  azap_uint8 activeEPCount,
  azap_uint8 activeEPList[])
{
  testerReqContext_t *ctxt = (testerReqContext_t *)issuerContext;
  completionOutput(issuerContext, "AZAP Tester ZdpActiveEPRspCb\n");
  azap_uint8 i;
  for (i = 0; i < activeEPCount; i++)
  {
    azapZdpSimpleDescReq(port, AZAP_REQ_TTL_START_AT_ZB_SEND, NULL, nwkAddrSrc,
        nwkAddrSrc, activeEPList[i]);
  }
  if (ctxt)
  {
    ctxt->free(ctxt);
  }
}

void azapTesterZdpBindRspCb (
  azap_uint8 port,
  void *issuerContext,
  azap_uint16 nwkAddrSrc,
  azap_uint8 status)
{
  completionOutput(issuerContext, "AZAP Tester ZdpBindRspCb\n");
  azap_uint8 j, s;
  testerReqContext_t *ctxt = (testerReqContext_t *)issuerContext;

  switch (ctxt->type)
  {
    case TSTER_REQ_AUDIT:
    {
      if (ZCL_CLUSTER_ID_MS_TEMPERATURE_MEASUREMENT == ctxt->clusterId)
      {
        // One attribute only:
        azapZclCfgReportCommand_t *zclCfgReportCmd = malloc(sizeof(azapZclCfgReportCommand_t) +
            (sizeof(azapZclCfgReportAttr_t) * 1));
        zclCfgReportCmd->numAttr = 1;
        // ATTRID_MS_TEMPERATURE_MEASURED_VALUE
        zclCfgReportCmd->attrList[0].direction = ZCL_FRAME_CLIENT_SERVER_DIR;
        zclCfgReportCmd->attrList[0].attrId = ATTRID_MS_TEMPERATURE_MEASURED_VALUE;
        zclCfgReportCmd->attrList[0].dataType = ZCL_DATATYPE_INT16;
        zclCfgReportCmd->attrList[0].minReportInt = 0x001E;
        zclCfgReportCmd->attrList[0].maxReportInt = 0x003C;
        s = azapGetZclDataLength(ZCL_DATATYPE_INT16, NULL);
        zclCfgReportCmd->attrList[0].reportableChange = malloc(s);
        for (j = 0; j < s; j++)
        {
          zclCfgReportCmd->attrList[0].reportableChange[j] = 0xff;
        }

        azapZclConfigureReportCmdReq(port, AZAP_REQ_TTL_START_AT_ZB_SEND, NULL, nwkAddrSrc,
            ctxt->dstEP, ctxt->srcEP, ZCL_CLUSTER_ID_MS_TEMPERATURE_MEASUREMENT,
            ZCL_FRAME_CLIENT_SERVER_DIR, zclCfgReportCmd);

        free(zclCfgReportCmd->attrList[0].reportableChange);
        free(zclCfgReportCmd);
      }
      else if (ZCL_CLUSTER_ID_MS_RELATIVE_HUMIDITY == ctxt->clusterId)
      {
        // One attribute only:
        azapZclCfgReportCommand_t *zclCfgReportCmd = malloc(sizeof(azapZclCfgReportCommand_t) +
            (sizeof(azapZclCfgReportAttr_t) * 1));
        zclCfgReportCmd->numAttr = 1;
        // ATTRID_MS_RELATIVE_HUMIDITY_MEASURED_VALUE
        zclCfgReportCmd->attrList[0].direction = ZCL_FRAME_CLIENT_SERVER_DIR;
        zclCfgReportCmd->attrList[0].attrId = ATTRID_MS_RELATIVE_HUMIDITY_MEASURED_VALUE;
        zclCfgReportCmd->attrList[0].dataType = ZCL_DATATYPE_UINT16;
        zclCfgReportCmd->attrList[0].minReportInt = 0x001E;
        zclCfgReportCmd->attrList[0].maxReportInt = 0x003C;
        s = azapGetZclDataLength(ZCL_DATATYPE_UINT16, NULL);
        zclCfgReportCmd->attrList[0].reportableChange = malloc(s);
        for (j = 0; j < s; j++)
        {
          zclCfgReportCmd->attrList[0].reportableChange[j] = 0xff;
        }

        azapZclConfigureReportCmdReq(port, AZAP_REQ_TTL_START_AT_ZB_SEND, NULL, nwkAddrSrc,
            ctxt->dstEP, ctxt->srcEP, ZCL_CLUSTER_ID_MS_RELATIVE_HUMIDITY,
            ZCL_FRAME_CLIENT_SERVER_DIR, zclCfgReportCmd);

        free(zclCfgReportCmd->attrList[0].reportableChange);
        free(zclCfgReportCmd);
      }
      else if (ZCL_CLUSTER_ID_SE_SIMPLE_METERING == ctxt->clusterId)
      {
        // Two attributes here:
        azapZclCfgReportCommand_t *zclCfgReportCmd = malloc(sizeof(azapZclCfgReportCommand_t) +
            (sizeof(azapZclCfgReportAttr_t) * 2));
        zclCfgReportCmd->numAttr = 2;
        // ATTRID_SE_CURRENT_SUMMATION_DELIVERED
        zclCfgReportCmd->attrList[0].direction = ZCL_FRAME_CLIENT_SERVER_DIR;
        zclCfgReportCmd->attrList[0].attrId = ATTRID_SE_CURRENT_SUMMATION_DELIVERED;
        zclCfgReportCmd->attrList[0].dataType = ZCL_DATATYPE_UINT48;
        zclCfgReportCmd->attrList[0].minReportInt = 0x001E;
        zclCfgReportCmd->attrList[0].maxReportInt = 0x003C;
        s = azapGetZclDataLength(ZCL_DATATYPE_UINT48, NULL);
        zclCfgReportCmd->attrList[0].reportableChange = malloc(s);
        for (j = 0; j < s; j++)
        {
          zclCfgReportCmd->attrList[0].reportableChange[j] = 0xff;
        }

        // ATTRID_SE_INSTANTANEOUS_DEMAND
        zclCfgReportCmd->attrList[1].direction = ZCL_FRAME_CLIENT_SERVER_DIR;
        zclCfgReportCmd->attrList[1].attrId = ATTRID_SE_INSTANTANEOUS_DEMAND;
        zclCfgReportCmd->attrList[1].dataType = ZCL_DATATYPE_INT24;
        zclCfgReportCmd->attrList[1].minReportInt = 0x001E;
        zclCfgReportCmd->attrList[1].maxReportInt = 0x003C;
        s = azapGetZclDataLength(ZCL_DATATYPE_INT24, NULL);
        zclCfgReportCmd->attrList[1].reportableChange = malloc(s);
        for (j = 0; j < s; j++)
        {
          zclCfgReportCmd->attrList[1].reportableChange[j] = 0xff;
        }

        azapZclConfigureReportCmdReq(port, AZAP_REQ_TTL_START_AT_ZB_SEND, NULL, nwkAddrSrc,
            ctxt->dstEP, ctxt->srcEP, ZCL_CLUSTER_ID_SE_SIMPLE_METERING,
            ZCL_FRAME_CLIENT_SERVER_DIR, zclCfgReportCmd);

        free(zclCfgReportCmd->attrList[0].reportableChange);
        free(zclCfgReportCmd->attrList[1].reportableChange);
        free(zclCfgReportCmd);
      }
    }
    break;

    default:
      break;
  }

  if (ctxt)
  {
    ctxt->free(ctxt);
  }
}

void azapTesterZdpUnbindRspCb (
  azap_uint8 port,
  void *issuerContext,
  azap_uint16 nwkAddrSrc,
  azap_uint8 status)
{
  testerReqContext_t *ctxt = (testerReqContext_t *)issuerContext;
  completionOutput(issuerContext, "AZAP Tester ZdpUnbindRspCb\n");
  if (ctxt)
  {
    ctxt->free(ctxt);
  }
}

void azapTesterZdpMgmtRtgRspCb (
  azap_uint8 port,
  void *issuerContext,
  azap_uint16 nwkAddrSrc,
  azap_uint8 status,
  azap_uint8 routingTableEntries,
  azap_uint8 startIndex,
  azap_uint8 routingTableListCount,
  zdoRtgEntry_t routeTableList[])
{
  testerReqContext_t *ctxt = (testerReqContext_t *)issuerContext;
  azap_uint8 i;
  completionOutput(issuerContext, "AZAP Tester ZdpMgmtRtgRspCb\n");
  completionOutput(issuerContext, "* status: %d\n", status);
  completionOutput(issuerContext, "* routingTableEntries: %d\n", routingTableEntries);
  completionOutput(issuerContext, "* startIndex: %d\n", startIndex);
  completionOutput(issuerContext, "* routingTableListCount: %d\n", routingTableListCount);
  for (i = 0; i < routingTableListCount; i++)
  {
    completionOutput(issuerContext, "* elt #%d: (dstAddress:0x%.4x) (status:%d) "
        "(nextHopAddress:0x%.4x)\n", i, routeTableList[i].dstAddress,
        routeTableList[i].status, routeTableList[i].nextHopAddress);
  }
  if (ctxt)
  {
    ctxt->free(ctxt);
  }
}

void azapTesterZdpMgmtBindRspCb (
  azap_uint8 port,
  void *issuerContext,
  azap_uint16 nwkAddrSrc,
  azap_uint8 status,
  azap_uint8 bindingTableEntries,
  azap_uint8 startIndex,
  azap_uint8 bindingTableListCount,
  zdoBindEntry_t bindingTableList[])
{
  testerReqContext_t *ctxt = (testerReqContext_t *)issuerContext;
  completionOutput(issuerContext, "AZAP Tester ZdpMgmtBindRspCb\n");
  if (ctxt)
  {
    ctxt->free(ctxt);
  }
}

void azapTesterZdpMgmtPermitJoinRspCb (
  azap_uint8 port,
  void *issuerContext,
  azap_uint16 nwkAddrSrc,
  azap_uint8 status)
{
  testerReqContext_t *context = (testerReqContext_t *)issuerContext;
  completionOutput(issuerContext, "AZAP Tester ZdpMgmtPermitJoinRspCb\n");
  if (context)
  {
    context->free(context);
  }
}

void azapTesterZdpMgmtLeaveRspCb (
  azap_uint8 port,
  void *issuerContext,
  azap_uint16 nwkAddrSrc,
  azap_uint8 status)
{
  testerReqContext_t *ctxt = (testerReqContext_t *)issuerContext;
  completionOutput(issuerContext, "AZAP Tester ZdpMgmtLeaveRspCb\n");
  if (ctxt)
  {
    ctxt->free(ctxt);
  }
}


/* SAPI call backs */
void azapTesterSapiFindDeviceConfCb (
  azap_uint8 port,
  azap_uint16 nwkAddr,
  azap_uint8 ieeeAddr[IEEE_ADDR_LEN])
{
  completionOutput(NULL, "AZAP Tester SapiFindDeviceConfCb\n");
}


/* ZCL call backs */
void azapTesterZclReadCmdRspCb (
  azap_uint8 port,
  void *issuerContext,
  azap_uint16 nwkAddrSrc,
  azap_uint8 endPoint,
  azap_uint16 clusterId,
  azap_uint8 clusterDir,
  azap_uint16 manufacturerCode,
  azapZclReadRspCommand_t *data)
{
  azap_uint8 i;
  testerReqContext_t *ctxt = (testerReqContext_t *)issuerContext;
  completionOutput(issuerContext,
      "AZAP Tester ZclReadCmdRspCb (port:%d) (nwkAddrSrc:0x%.4x) (endPoint:0x%.2x) "
      "(clusterId:0x%.4x) (clusterDir:0x%.2x) (manufacturerCode:0x%.4x)\n",
      port, nwkAddrSrc, endPoint, clusterId, clusterDir, manufacturerCode);

  if (data)
  {
    completionOutput(issuerContext, "\tnumAttr: %d\n", data->numAttr);
    for (i = 0; i < data->numAttr; i++)
    {
      azapZclAttrVal_t *val = new_azapZclAttrVal_t(data->attrList[i].dataType);
      azap_uint8 *pPtr = data->attrList[i].data;
      azap_uint8 len = azapGetZclDataLength(data->attrList[i].dataType, data->attrList[i].data);
      azap_uint8 len2 = len;
      val->deserialize(val, &pPtr, &len2);
      completionOutput(issuerContext,
          "\t - attr #%d : ID=0x%.4x - STATUS=0x%.2x - TYPE=0x%.2x - DATA=%s - VAL=%s\n", i,
          data->attrList[i].attrId, data->attrList[i].status, data->attrList[i].dataType,
          azap_uint8ArrayToString(data->attrList[i].data, len), val->getStringRep(val));
      val->free(val);
    }

    completionOutput(issuerContext, "\n");
  }


  // also test ZclWriteCmd if cluster is BASIC, change "Location" attribute (attrId: 0x0010)
  if ((data) && (ZCL_CLUSTER_ID_GEN_BASIC == clusterId))
  {
    azapZclWriteCommand_t *writeCmd = malloc(sizeof(azapZclWriteCommand_t) +
        sizeof(azapZclWriteAttr_t));
    if (writeCmd)
    {
      writeCmd->numAttr = 1;
      writeCmd->attrList[0].attrId = ATTRID_BASIC_LOCATION_DESC;
      writeCmd->attrList[0].dataType = ZCL_DATATYPE_CHAR_STR;

      azap_uint8 len = strlen("ACTILITY") + 1;
      writeCmd->attrList[0].attrData = malloc(sizeof(azap_uint8) * len);
      azap_uint8 *pPtr = writeCmd->attrList[0].attrData;
      azapZclAttrVal_t *val = new_azapZclAttrVal_t(ZCL_DATATYPE_CHAR_STR);
      val->setStringValue(val, "ACTILITY");
      val->serialize(val, &pPtr, &len);

      azapZclWriteCmdReq(port, AZAP_REQ_TTL_START_AT_ZB_SEND, NULL, nwkAddrSrc, 0x01,
          endPoint, clusterId, ZCL_FRAME_CLIENT_SERVER_DIR, 0x0000, true, writeCmd);
      val->free(val);
      free(writeCmd->attrList[0].attrData);
      free(writeCmd);
    }
  }
  if (ctxt)
  {
    ctxt->free(ctxt);
  }
}

void azapTesterZclWriteCmdRspCb (
  azap_uint8 port,
  void *issuerContext,
  azap_uint16 nwkAddrSrc,
  azap_uint8 endPoint,
  azap_uint16 clusterId,
  azap_uint8 clusterDir,
  azap_uint16 manufacturerCode,
  azapZclWriteRspCommand_t *data)
{
  testerReqContext_t *ctxt = (testerReqContext_t *)issuerContext;
  completionOutput(issuerContext, "AZAP Tester ZclWriteCmdRspCb\n");
  if (ctxt)
  {
    ctxt->free(ctxt);
  }
}

void azapTesterZclConfigureReportCmdRspCb (
  azap_uint8 port,
  void *issuerContext,
  azap_uint16 nwkAddrSrc,
  azap_uint8 endPoint,
  azap_uint16 clusterId,
  azapZclCfgReportRspCommand_t *data)
{
  testerReqContext_t *ctxt = (testerReqContext_t *)issuerContext;
  completionOutput(issuerContext, "AZAP Tester ZclConfigureReportCmdRspCb\n");
  if (ctxt)
  {
    ctxt->free(ctxt);
  }
}

void azapTesterZclReadReportConfigCmdRspCb (
  azap_uint8 port,
  void *issuerContext,
  azap_uint16 nwkAddrSrc,
  azap_uint8 endPoint,
  azap_uint16 clusterId,
  azapZclReadReportCfgRspCommand_t *data)
{
  testerReqContext_t *ctxt = (testerReqContext_t *)issuerContext;
  completionOutput(issuerContext, "AZAP Tester ZclReadReportConfigCmdRspCb\n");
  if (ctxt)
  {
    ctxt->free(ctxt);
  }
}

void azapTesterZclReportCmdCb (
  azap_uint8 port,
  azap_uint16 nwkAddrSrc,
  azap_uint8 endPoint,
  azap_uint16 clusterId,
  azapZclReportCommand_t *data)
{
  azap_uint8 i;
  completionOutput(NULL,
      "AZAP Tester ZclReportCmdCb (port:%d) (nwkAddrSrc:0x%.4x) (endPoint:0x%.2x) "
      "(clusterId:0x%.4x)\n", port, nwkAddrSrc, endPoint, clusterId);

  if (data)
  {
    completionOutput(NULL, "\tnumAttr: %d\n", data->numAttr);
    for (i = 0; i < data->numAttr; i++)
    {
      azapZclAttrVal_t *val = new_azapZclAttrVal_t(data->attrList[i].dataType);
      azap_uint8 *pPtr = data->attrList[i].attrData;
      azap_uint8 len = azapGetZclDataLength(data->attrList[i].dataType,
          data->attrList[i].attrData);
      azap_uint8 len2 = len;
      val->deserialize(val, &pPtr, &len2);
      completionOutput(NULL,
          "\t - attr #%d : ID=0x%.4x - TYPE=0x%.2x - DATA=%s - VAL=%s\n", i,
          data->attrList[i].attrId, data->attrList[i].dataType,
          azap_uint8ArrayToString(data->attrList[i].attrData, len), val->getStringRep(val));
      val->free(val);
    }

    completionOutput(NULL, "\n");
  }

}

void azapTesterZclDiscoverCmdRspCb (
  azap_uint8 port,
  void *issuerContext,
  azap_uint16 nwkAddrSrc,
  azap_uint8 endPoint,
  azap_uint16 clusterId,
  azap_uint8 clusterDir,
  azapZclDiscoverRspCommand_t *data)
{
  testerReqContext_t *ctxt = (testerReqContext_t *)issuerContext;
  azap_uint8 i;
  azapZclReadCommand_t *readCmd = NULL;
  completionOutput(issuerContext,
      "AZAP Tester ZclDiscoverCmdRspCb (port:%d) (nwkAddrSrc:0x%.4x) (endPoint:0x%.2x) "
      "(clusterId:0x%.4x) (clusterDir:0x%.2x)\n", port, nwkAddrSrc, endPoint,
      clusterId, clusterDir);
  if (data)
  {
    completionOutput(issuerContext, "\tdiscComplete: %d\n", data->discComplete);
    completionOutput(issuerContext, "\tnumAttr: %d\n", data->numAttr);
    for (i = 0; i < data->numAttr; i++)
    {
      completionOutput(issuerContext,
          "\t - attr #%d : ID=0x%.4x - TYPE=0x%.2x\n", i, data->attrList[i].attrId,
          data->attrList[i].dataType);
    }

    completionOutput(issuerContext, "\n");

    readCmd = malloc(sizeof(azapZclReadCommand_t) + ( sizeof(azap_uint16) * data->numAttr));
    if (readCmd)
    {
      readCmd->numAttr = data->numAttr;
      for (i = 0; i < data->numAttr; i++)
      {
        readCmd->attrId[i] = data->attrList[i].attrId;
      }
      azapZclReadCmdReq(port, AZAP_REQ_TTL_START_AT_ZB_SEND, NULL, nwkAddrSrc, 0x01,
          endPoint, clusterId, ZCL_FRAME_CLIENT_SERVER_DIR, 0x0000, readCmd);
      free(readCmd);
    }
  }
  if (ctxt)
  {
    ctxt->free(ctxt);
  }
}

void azapTesterZclIncomingCmdCb (
  azap_uint8 port,
  void *issuerContext,
  azap_uint16 nwkAddrSrc,
  azap_uint8 endPoint,
  azap_uint16 clusterId,
  azap_uint8 cmdCode,
  azap_byte cmdData[],
  azap_uint8 cmdLen)
{
  testerReqContext_t *ctxt = (testerReqContext_t *)issuerContext;
  completionOutput(issuerContext, "AZAP Tester ZclIncomingCmdCb (port:%d) "
      "(nwkAddrSrc:0x%.4x) (endPoint:0x%.2x) (clusterId:0x%.4x) "
      "(ctxt:0x%.8x)\n", port, nwkAddrSrc, endPoint, clusterId, ctxt);

  completionOutput(issuerContext, "\t - cmd code: %d\n"
      "\t - data: %s\n", cmdCode, azap_uint8ArrayToString(cmdData, cmdLen));
  if (ctxt)
  {
    ctxt->free(ctxt);
    completionOutput(issuerContext, "\tNot implemented - message ignored.\n");
  }
  else
  {
    switch (clusterId)
    {
      case ZCL_CLUSTER_ID_GEN_ALARMS:
        processAlarm(nwkAddrSrc, endPoint, cmdCode, cmdData, cmdLen);
        break;

      default:
        completionOutput(issuerContext, "\tNot implemented - message ignored.\n");
        break;
    }
  }
}


/* when used as a node */
void azapTesterZclConfigureReportCmdCb (
  azap_uint8 port,
  azap_uint16 nwkAddrDest,
  azap_uint8 endPoint,
  azap_uint16 clusterId,
  azapZclCfgReportCommand_t *data)
{
  completionOutput(NULL, "AZAP Tester ZclConfigureReportCmdCb\n");
}



void processAlarm(azap_uint16 nwkAddrSrc, azap_uint8 endPoint,
    azap_uint8 cmdCode, azap_byte cmdData[], azap_uint8 cmdLen)
{
  switch (cmdCode)
  {
    case COMMAND_ALARMS_ALARM:
    {
      short i = 0;

      // see 3.11.2.3.1 Alarm Table Format in ZigBee ZCL specification
      // alarm code
      azap_uint8 alarmCode = cmdData[i++];
      // Cluster identifier
      azap_uint16 clusterId = (azap_uint16)(0x00FF & cmdData[i++]);
      clusterId += (azap_uint16)((0x00FF & cmdData[i++]) << 8);
      // timestamp (may be not present on NetVox devices)
      //azap_uint32 timestamp = 0;
      if (i + 4 <= cmdLen)
      {
        /*timestamp = ((azap_uint32)((azap_uint32)((cmdData[i]) & 0x00FF) +
            ((azap_uint32)((cmdData[i + 1]) & 0x00FF) << 8) +
            ((azap_uint32)((cmdData[i + 2]) & 0x00FF) << 16) +
            ((azap_uint32)((cmdData[i + 3]) & 0x00FF) << 24)));*/
        i += 4;
      }

      if (i != cmdLen)
      {
        completionOutput(NULL, "processAlarm - internal error: data remains (i:%d) "
            "(cmdLen:%d)\n", i, cmdLen);
      }
      else
      {
        switch (clusterId)
        {
          case ZCL_CLUSTER_ID_GEN_POWER_CFG:
            // see "3.3.2.2.2.2 MainsVoltageMinThreshold Attribute" in ZigBee ZCL
            // specification
          {
            switch (alarmCode)
            {
              case MAINS_ALARM_MASK_VOLT_2_LOW:
                completionOutput(NULL, "ALARM raised: Mains Voltage Min Threshold "
                    "reached !!!\n");
                break;
              case MAINS_ALARM_MASK_VOLT_2_HI:
                completionOutput(NULL, "ALARM raised: Mains Voltage Max Threshold "
                    "reached !!!\n");
                break;
              case BAT_ALARM_MASK_VOLT_2_LOW:
                completionOutput(NULL, "ALARM raised: Battery Voltage Min Threshold "
                    "reached !!!\n");
                break;
              default:
                completionOutput(NULL, "processAlarm - unknown alarm code 0x%.2d\n",
                    alarmCode);
                break;
            }
          }
          break;

          default:
            completionOutput(NULL, "processAlarm - alarms not supported for this "
                "cluster 0x%.4d\n", cmdCode);
            break;
        }
      }
    }
    break;

    default:
      completionOutput(NULL, "processAlarm - unsupported command code 0x%.2d\n", cmdCode);
      break;
  }

}
