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
//** File : driver/azap_listener.c
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
 * @file azap_listener.c
 * @brief Implementation of completion methods provided in t_azap_api_callbacks for AZAP init.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stddef.h>

#include "rtlbase.h"
#include "rtllist.h"
#include "rtlimsg.h"
#include "xoapipr.h"
#include "dIa.h"

#include "cmnstruct.h"
#include "cmnproto.h"
#include "speccproto.h"
#include "specextern.h"

#include "adaptor.h"

#include "azap_api.h"
#include "azap_tools.h"

#include "azap_listener.h"
#include "azap_zcl_attr_val.h"
#include "azap_attribute.h"
#include "azap_endpoint.h"

#include "interface_mapping_desc.h"
#include "driver_req_context.h"
#include "zigbee_node.h"
#include "zigbee_han.h"
#include "driver_req_context.h"
#include "m2m_abstraction.h"
#include "m2m_zcl_abstraction.h"

extern zigbeeHan_t *theHan;
extern azap_uint8 azapPort;
extern void *MdlCfg;
extern char *knownSensorPath;
extern t_cli *TbCli[];

/* Global variables */
static t_azap_api_callbacks *g_pCallbacks = NULL;

static azap_uint16 adminEndPointTable[MAX_ADMIN_END_POINTS];

/* Prototypes for local functions */
static void initCallbacks(t_azap_api_callbacks *pCb);


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
    memset(adminEndPointTable, 0, sizeof(azap_uint16) * MAX_ADMIN_END_POINTS);
  }
  return g_pCallbacks;
}

/**
 * @brief Ask for adding an administrative end point.
 * @param profId the profile identifier associated to the end point.
 * @return the end point identifier assigned to the administrative end point if
 * the maximum number of admin end point is not reached yet, 0 otherwise.
 */
azap_uint8 addAdminEndPoint(azap_uint16 profId)
{
  azap_uint8 epId = 0;
  while ((epId < MAX_ADMIN_END_POINTS) && (0 != adminEndPointTable[epId]))
  {
    epId++;
  }
  if (epId < MAX_ADMIN_END_POINTS)
  {
    adminEndPointTable[epId] = profId;
    epId++;
  }
  else
  {
    epId = 0;
  }

  return epId;
}

/**
 * @brief Retrieve the administrative end point that matches the provided profile identifier.
 * @param profId the profile identifier associated to the end point.
 * @return the end point identifier assigned to the administrative end point if matching
 * found, 0 otherwise.
 */
azap_uint8 getMatchingAdminEndPoint(azap_uint16 profId)
{
  azap_uint8 epId = 0;
  while ((epId < MAX_ADMIN_END_POINTS) && (profId != adminEndPointTable[epId]))
  {
    epId++;
  }
  if (epId < MAX_ADMIN_END_POINTS)
  {
    epId++;
  }
  else
  {
    epId = 0;
  }

  return epId;
}

/**
 * Initialize the call back methods for AZAP layer notifications.
 * @param pCb the structure that convey the pointers on call back methods.
 */
static void initCallbacks(t_azap_api_callbacks *pCb)
{
  pCb->azapStartedCb = zbDriverStartedCb;
  pCb->azapZdpBindReqCb = zbDriverZdpBindReqCb;
  pCb->azapZdpUnbindReqCb = zbDriverZdpUnbindReqCb;
  pCb->azapZdpDeviceAnnceCb = zbDriverZdpDeviceAnnceCb;
  pCb->azapZdpNwkAddrRspCb = zbDriverZdpNwkAddrRspCb;
  pCb->azapZdpIeeeAddrRspCb = zbDriverZdpIeeeAddrRspCb;
  pCb->azapZdpNodeDescRspCb = zbDriverZdpNodeDescRspCb;
  pCb->azapZdpPowerDescRspCb = zbDriverZdpPowerDescRspCb;
  pCb->azapZdpSimpleDescRspCb = zbDriverZdpSimpleDescRspCb;
  pCb->azapZdpActiveEPRspCb = zbDriverZdpActiveEPRspCb;
  pCb->azapZdpBindRspCb = zbDriverZdpBindRspCb;
  pCb->azapZdpUnbindRspCb = zbDriverZdpUnbindRspCb;
  pCb->azapZdpMgmtRtgRspCb = zbDriverZdpMgmtRtgRspCb;
  pCb->azapZdpMgmtBindRspCb = zbDriverZdpMgmtBindRspCb;
  pCb->azapZdpMgmtPermitJoinRspCb = zbDriverZdpMgmtPermitJoinRspCb;
  pCb->azapZdpMgmtLeaveRspCb = zbDriverZdpMgmtLeaveRspCb;
  pCb->azapSapiFindDeviceConfCb = zbDriverSapiFindDeviceConfCb;
  pCb->azapZclReadCmdRspCb = zbDriverZclReadCmdRspCb;
  pCb->azapZclWriteCmdRspCb = zbDriverZclWriteCmdRspCb;
  pCb->azapZclConfigureReportCmdRspCb = zbDriverZclConfigureReportCmdRspCb;
  pCb->azapZclReportCmdCb = zbDriverZclReportCmdCb;
  pCb->azapZclDiscoverCmdRspCb = zbDriverZclDiscoverCmdRspCb;
  pCb->azapZclIncomingCmdCb = zbDriverZclIncomingCmdCb;
  pCb->azapZclConfigureReportCmdCb = zbDriverZclConfigureReportCmdCb;
  pCb->azapZclReadReportConfigCmdRspCb = zbDriverZclReadReportConfigCmdCb;
}

/**
 * @brief Send a dIa response with a failure status, providing ZigBee status.
 * @param diaReqId the dIa request identifier to provide in completion call back.
 * @param status the ZigBee status response.
 * @param family "ZDO" or "ZCL"
 */
static void sendDIaFailureResponse(int diaReqId, azap_uint8 status, char *family)
{
  int size = 0;
  char *statusCode = NULL;
  char *type = NULL;
  char *content = NULL;
  content = getErrorInfoResponse(family, status, &statusCode, &size, &type);
  diaIncomingRequestCompletion(diaReqId, content, size, type, statusCode);
  if (content)
  {
    free(content);
  }
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
void zbDriverStartedCb(
  azap_uint8 port,
  bool success,
  azap_uint16 shortAddr,
  azap_uint8 deviceIEEE[IEEE_ADDR_LEN],
  azap_uint16 panId,
  azap_uint8 panIdExt[IEEE_ADDR_LEN])
{
  theHan->panId = panId;
  memcpy(theHan->panIdExt, panIdExt, IEEE_ADDR_LEN);
  memcpy(theHan->coordExtAddr, deviceIEEE, IEEE_ADDR_LEN);

  SetVar("w_panid", azap_uint16ToString(theHan->panId, false));
  SetVar("w_panextid", azap_extAddrToString(theHan->panIdExt, false));
  SetVar("w_netid", azap_extAddrToString(theHan->coordExtAddr, false));

  theHan->reloadKnownNodes(theHan, knownSensorPath);

  // deny joinging to the HAN
  azapZdpMgmtPermitJoinReq(azapPort, AZAP_REQ_TTL_START_AT_API, NULL, 0x0000, 0, true, true);
  // and also broadcast it on the HAN
  azapZdpMgmtPermitJoinReq(azapPort, AZAP_REQ_TTL_START_AT_API, NULL, 0xFFFF, 0, true, true);

  //
  DiaIpuStart();
  DiaNetStart();
}


void zbDriverZdpBindReqCb (
  azap_uint8 port,
  void *issuerContext,
  azap_uint16 nwkAddrSrc,
  azap_uint8 srcIeeeAddr[IEEE_ADDR_LEN],
  azap_uint8 srcEndPoint,
  azap_uint16 clusterId,
  azap_uint8 destIeeeAddr[IEEE_ADDR_LEN],
  azap_uint8 destEndPoint)
{
  printf("AZAP ZdpBindReqCb\n");
}

void zbDriverZdpUnbindReqCb(
  azap_uint8 port,
  void *issuerContext,
  azap_uint16 nwkAddrSrc,
  azap_uint8 srcIeeeAddr[IEEE_ADDR_LEN],
  azap_uint8 srcEndPoint,
  azap_uint16 clusterId,
  azap_uint8 destIeeeAddr[IEEE_ADDR_LEN],
  azap_uint8 destEndPoint)
{
  printf("AZAP ZdpUnbindReqCb\n");
}

/**
 * @brief Call back function for ZDP Device_annce
 * @param port the port context for the targeted hardware device
 * @param nwkAddr the short address of the node that send the message.
 * @param ieeeAddr the IEEE address of the node that send the message.
 * @param alternatePanCoordinator MAC capability of the local device
 * @param deviceType MAC capability of the local device
 * @param powerSource MAC capability of the local device
 * @param receiverOnWhenIdle MAC capability of the local device
 * @param securityCapability MAC capability of the local device
 * @param allocateAddress MAC capability of the local device
 */
void zbDriverZdpDeviceAnnceCb(
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
  zigbeeNode_t *node = theHan->getNodeByIeeeAddr(theHan, ieeeAddr);

  if ( (NULL == node) || (NULL == node->azapAuditContext) )
  {
    if (NULL == node)
    {
      node = new_zigbeeNode_t(nwkAddr, ieeeAddr);
      theHan->addNode(theHan, node);
    }
    else if (node->cp_cmn.cmn_num != nwkAddr)
    {
      RTL_TRDBG(TRACE_API, "zbDriverZdpDeviceAnnceCb - device has a new network address "
          "(former nwkAddr:%s) (nwkAddr:%s) (ieee:%s)\n",
          azap_uint16ToString(node->cp_cmn.cmn_num, true),
          azap_uint16ToString(nwkAddr, true), azap_extAddrToString(ieeeAddr, true));
      node->cp_cmn.cmn_num = nwkAddr;
    }

    driverReqContext_t *issuerContext = new_driverReqContext_t(DRV_REQ_DEVICE_AUDIT);

    // start the device audit
    node->azapAuditContext = issuerContext;
    node->clearDiaCounters(node);
    issuerContext->nbPendingRequests ++;
    azapZdpNodeDescReq(port, AZAP_REQ_TTL_START_AT_ZB_SEND, issuerContext, nwkAddr, nwkAddr);
  }
  else
  {
    RTL_TRDBG(TRACE_INFO, "zbDriverZdpDeviceAnnceCb - duplicated announce is ignored "
        "(nwkAddr:%s) (ieee:%s)\n", azap_uint16ToString(nwkAddr, true),
        azap_extAddrToString(ieeeAddr, true));
  }
}

/**
 * @brief Call back method for ZDP NWK_addr_rsp received message.
 *
 * @param port the port context for the targeted hardware device
 * @param issuerContext The context handler of the issuer, as provided on API.
 * @param nwkAddrSrc network address that sent this response.
 * @param status response status (success or failure)
 * @param ieeeAddrRemoteDev resulting IEEE address
 * @param nwkAddrRemoteDev network address of the device we were looking at
 * @param numAssocDev number of associated devices.
 * @param startIndex device starting index
 * @param nwkAddrAssocDevList devices list.
 */
void zbDriverZdpNwkAddrRspCb (
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
  printf("AZAP ZdpNwkAddrRspCb\n");
}

/**
 * @brief Call back method for ZDP IEEE_addr_rsp received message.
 *
 * @param port the port context for the targeted hardware device
 * @param issuerContext The context handler of the issuer, as provided on API.
 * @param nwkAddrSrc network address that sent this response.
 * @param status response status (success or failure)
 * @param ieeeAddrRemoteDev resulting IEEE address
 * @param nwkAddrRemoteDev network address of the device we were looking at
 * @param numAssocDev number of associated devices.
 * @param startIndex device starting index
 * @param nwkAddrAssocDevList devices list.
 */
void zbDriverZdpIeeeAddrRspCb (
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
  printf("AZAP ZdpIeeeAddrRspCb\n");
}

/**
 * @brief Call back method for ZDP Node_Desc_rsp received message.
 *
 * @param port the port context for the targeted hardware device
 * @param issuerContext The context handler of the issuer, as provided on API.
 * @param nwkAddrSrc Network address of the node that sent this message (retrieved from
 * NWK layer).
 * @param status The status of the Node_Desc_req command.
 * @param nwkAddrOfInterest NWK address for the request.
 * @param logicalType This field specifies the device type of the ZigBee node. 0 for
 * ZigBee coordinator, 1 for ZigBee router or 2 for ZigBee end device.
 * @param complexDescriptorAvailable This field specifies whether a complex descriptor
 * is available on this device.
 * @param userDescriptorAvailable This field specifies whether a user descriptor is
 * available on this device.
 * @param apsFlags This field specifies the application support sub-layer capabilities
 * of the node.
 * @param frequencyBand This field specifies the frequency bands that are supported by
 * the underlying IEEE 802.15.4 radio utilized by the node.
 * @param macCapabilityFlags This field specifies the node capabilities, as required by
 * the IEEE 802.15.4-2003 MAC sub-layer.
 * @param manufacturerCode This field specifies a manufacturer code that is allocated by
 * the ZigBee Alliance, relating the manufacturer to the device.
 * @param maximumBufferSize This field specifies the maximum size, in octets, of the
 * network sub-layer data unit (NSDU) for this node. This is the maximum size of data or
 * commands passed to or from the application by the application support sub-layer,
 * before any fragmentation or re-assembly.
 * @param maximumIncomingTransferSize This field specifies the maximum size, in octets,
 * of the application sub-layer data unit (ASDU) that can be transferred to this node in
 * one single message transfer.
 * @param serverMask This field is used to facilitate discovery of particular system
 * servers by other nodes on the system.
 * @param maximumOutgoingTransferSize This field specifies the maximum size, in octets,
 * of the application sub-layer data unit (ASDU) that can be transferred from this node
 * in one single message transfer.
 * @param descriptorCapabilityField This field is used to facilitate discovery of
 * particular features of the descriptor fields by other nodes on the system.
 */
void zbDriverZdpNodeDescRspCb (
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
  zigbeeNode_t *node;
  driverReqContext_t *reqContext = (driverReqContext_t *) issuerContext;

  if (!reqContext)
  {
    return;
  }

  switch (reqContext->type)
  {
    case DRV_REQ_DEVICE_AUDIT:
    {
      reqContext->nbPendingRequests --;
      node = theHan->getNodeByNwkAddr(theHan, nwkAddrSrc);
      if (ZCL_STATUS_SUCCESS == status)
      {
        node->nodeType = logicalType;
        node->manuCode = manufacturerCode;
        // continue the device audit
        if (azapZdpPowerDescReq(port, AZAP_REQ_TTL_START_AT_ZB_SEND, reqContext,
            nwkAddrSrc, nwkAddrSrc))
        {
          reqContext->nbPendingRequests ++;
        }
      }
      if (0 >= reqContext->nbPendingRequests)
      {
        RTL_TRDBG(TRACE_ERROR, "zbDriverZdpNodeDescRspCb - error: "
            "the device didn't respond. Device audit aborted. (port:%d)\n", port);
        reqContext->free(reqContext);
        node->endAudit(node, false, 0, NULL, 0, NULL);
        if (node->isFirstAudit(node))
        {
          RTL_TRDBG(TRACE_ERROR, "zbDriverZdpNodeDescRspCb - error: "
              "this was the first audit attempt, node has been deleted. (port:%d)\n", port);
          // at this stage, remove the node
          theHan->deleteNode(theHan, node->ieeeAddr);
        }
      }
    }
    break;

    default:
    {
      RTL_TRDBG(TRACE_ERROR, "zbDriverZdpNodeDescRspCb - error: "
          "unexpected context type here (port:%d) (type:%d)\n", port,
          reqContext->type);
      reqContext->free(reqContext);
    }
    break;
  }
}

/**
 * @brief Call back method for ZDP Power_Desc_rsp received message.
 *
 * @param port the port context for the targeted hardware device
 * @param issuerContext The context handler of the issuer, as provided on API.
 * @param nwkAddrSrc Network address of the node that sent this message (retrieved from
 * NWK layer).
 * @param status The status of the Power_Desc_req command.
 * @param nwkAddrOfInterest NWK address for the request.
 * @param currentPowerMode This field specifies the current sleep/power-saving mode of
 * the node.
 * @param availablePowerSources This field specifies the power sources available on this
 * node.
 * @param currentPowerSource This field specifies the current power source being
 * utilized by the node.
 * @param currentPowerSourceLevel This field specifies the level of charge of the power
 * source.
 */
void zbDriverZdpPowerDescRspCb (
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
  zigbeeNode_t *node;
  driverReqContext_t *reqContext = (driverReqContext_t *) issuerContext;

  if (!reqContext)
  {
    return;
  }

  switch (reqContext->type)
  {
    case DRV_REQ_DEVICE_AUDIT:
    {
      reqContext->nbPendingRequests --;
      node = theHan->getNodeByNwkAddr(theHan, nwkAddrSrc);
      if (ZCL_STATUS_SUCCESS == status)
      {
        node->powerMode = currentPowerMode;
        node->powerSource = currentPowerSource;
        node->powerLevel = currentPowerSourceLevel;
        // continue the device audit
        if (azapZdpActiveEPReq(port, AZAP_REQ_TTL_START_AT_ZB_SEND, reqContext,
            nwkAddrSrc, nwkAddrSrc))
        {
          reqContext->nbPendingRequests ++;
        }
      }
      if (0 >= reqContext->nbPendingRequests)
      {
        RTL_TRDBG(TRACE_ERROR, "zbDriverZdpPowerDescRspCb - error: "
            "the device didn't respond. Device audit aborted. (port:%d)\n", port);
        reqContext->free(reqContext);
        node->endAudit(node, false, 0, NULL, 0, NULL);
        if (node->isFirstAudit(node))
        {
          RTL_TRDBG(TRACE_ERROR, "zbDriverZdpPowerDescRspCb - error: "
              "this was the first audit attempt, node has been deleted. (port:%d)\n", port);
          // at this stage, remove the node
          theHan->deleteNode(theHan, node->ieeeAddr);
        }
      }
    }
    break;

    default:
    {
      RTL_TRDBG(TRACE_ERROR, "zbDriverZdpPowerDescRspCb - error: "
          "unexpected context type here (port:%d) (type:%d)\n", port,
          reqContext->type);
      reqContext->free(reqContext);
    }
    break;
  }
}

/**
 * @brief Call back method for ZDP Simple_Desc_rsp received message.
 *
 * @param port the port context for the targeted hardware device
 * @param issuerContext The context handler of the issuer, as provided on API.
 * @param nwkAddrSrc Network address of the node that sent this message (retrieved from
 * NWK layer).
 * @param status The status of the Simple_Desc_req command.
 * @param nwkAddrOfInterest NWK address for the request.
 * @param endpoint This field specifies the endpoint within the node to which this
 * description refers.
 * @param applicationProfileIdentifier This field specifies the profile that is
 * supported on this endpoint.
 * @param applicationDeviceIdentifier This field specifies the device description
 * supported on this endpoint.
 * @param applicationDeviceVersion This field specifies the version of the device
 * description supported on this endpoint.
 * @param applicationInputClusterCount This field specifies the number of input
 * clusters, supported on this endpoint, that
 *            will appear in the application input cluster list field.
 * @param applicationInputClusterList This field specifies the list of input clusters
 * supported on this endpoint, for use
 *            during the service discovery and binding procedures.
 * @param applicationOutputClusterCount This field specifies the number of output
 * clusters, supported on this endpoint, that
 *            will appear in the application output cluster list field.
 * @param applicationOutputClusterList This field specifies the list of output clusters
 * supported on this endpoint, for use
 *            during the service discovery and binding procedures.
 */
void zbDriverZdpSimpleDescRspCb (
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
  driverReqContext_t *reqContext = (driverReqContext_t *) issuerContext;
  zigbeeNode_t *node;
  azapEndPoint_t *ep;
  azap_uint16 clId;


  if (!reqContext)
  {
    return;
  }

  switch (reqContext->type)
  {
    case DRV_REQ_DEVICE_AUDIT:
    {
      reqContext->nbPendingRequests --;
      node = theHan->getNodeByNwkAddr(theHan, nwkAddrSrc);
      if (ZCL_STATUS_SUCCESS == status)
      {
        ep = new_azapEndPoint_t(endpoint, applicationProfileIdentifier,
            applicationDeviceIdentifier, applicationDeviceVersion,
            applicationInputClusterCount, applicationInputClusterList,
            applicationOutputClusterCount, applicationOutputClusterList);
        node->addEndPoint(node, ep);

        // continue the device audit, looking in server cluster attributes
        azapZclDiscoverCommand_t discoverCmd;
        discoverCmd.startAttrId = 0;
        discoverCmd.maxNumAttrIds = 100; // arbitrary choosen...
        for (; applicationInputClusterCount > 0; applicationInputClusterCount--)
        {
          clId = applicationInputClusterList[applicationInputClusterCount - 1];
          if (NULL != MdlGetInterface(MdlCfg, NULL, "server",
              azap_uint16ToString(clId, true)))
          {
            node->addM2mZclAbstraction(node, endpoint, clId);
            // we have to discover cluster attributes that expose this device
            if (azapZclDiscoverCmdReq(port, AZAP_REQ_TTL_START_AT_ZB_SEND,
                reqContext, nwkAddrSrc, getMatchingAdminEndPoint(ep->appProfId),
                endpoint, clId, ZCL_FRAME_CLIENT_SERVER_DIR, &discoverCmd))
            {
              reqContext->nbPendingRequests ++;
            }
          }
        }
      }
      if (0 >= reqContext->nbPendingRequests)
      {
        RTL_TRDBG(TRACE_ERROR, "zbDriverZdpSimpleDescRspCb - error: "
            "the device didn't respond. Device audit stops here, no APP "
            "will be created. (port:%d)\n", port);
        reqContext->free(reqContext);
        node->endAudit(node, true, theHan->panId, theHan->panIdExt,
            theHan->rfChan, knownSensorPath);
      }
    }
    break;

    default:
    {
      RTL_TRDBG(TRACE_ERROR, "zbDriverZdpSimpleDescRspCb - error: "
          "unexpected context type here (port:%d) (type:%d)\n", port,
          reqContext->type);
      reqContext->free(reqContext);
    }
    break;
  }
}

/**
 * @brief Call back method for ZDP Active_EP_rsp received message.
 *
 * @param port the port context for the targeted hardware device
 * @param issuerContext The context handler of the issuer, as provided on API.
 * @param nwkAddrSrc Network address of the node that sent this message (retrieved from
 * NWK layer).
 * @param status The status of the Active_EP_req command.
 * @param nwkAddrOfInterest NWK address for the request.
 * @param activeEPCount This field specifies the number of active endpoints on that
 * device.
 * @param activeEPList This field specifies an ascending list of all the identifiers of
 * the active endpoints on that device.
 */
void zbDriverZdpActiveEPRspCb (
  azap_uint8 port,
  void *issuerContext,
  azap_uint16 nwkAddrSrc,
  azap_uint8 status,
  azap_uint16 nwkAddrOfInterest,
  azap_uint8 activeEPCount,
  azap_uint8 activeEPList[])
{
  zigbeeNode_t *node;
  driverReqContext_t *reqContext = (driverReqContext_t *) issuerContext;

  if (!reqContext)
  {
    return;
  }

  switch (reqContext->type)
  {
    case DRV_REQ_DEVICE_AUDIT:
    {
      reqContext->nbPendingRequests --;
      if (ZCL_STATUS_SUCCESS == status)
      {
        // continue the device audit
        for (; activeEPCount > 0; activeEPCount--)
        {
          if (azapZdpSimpleDescReq(port, AZAP_REQ_TTL_START_AT_ZB_SEND, reqContext,
              nwkAddrSrc, nwkAddrSrc, activeEPList[activeEPCount - 1]))
          {
            reqContext->nbPendingRequests ++;
          }
        }
      }

      if (0 >= reqContext->nbPendingRequests)
      {
        RTL_TRDBG(TRACE_ERROR, "zbDriverZdpActiveEPRspCb - error: "
            "the device didn't respond. Device audit aborted. (port:%d)\n", port);
        reqContext->free(reqContext);
        node = theHan->getNodeByNwkAddr(theHan, nwkAddrSrc);
        node->endAudit(node, false, 0, NULL, 0, NULL);
        //if (node->isFirstAudit(node))
        //{
        //RTL_TRDBG(TRACE_ERROR, "zbDriverZdpActiveEPRspCb - error: "
        //  "this was the first audit attempt, node has been deleted. (port:%d)\n", port);
        //// at this stage, remove the node
        //node = theHan->getNodeByNwkAddr(theHan, nwkAddrSrc);
        //theHan->deleteNode(theHan, node->ieeeAddr);
        //}
      }
    }
    break;

    default:
    {
      RTL_TRDBG(TRACE_ERROR, "zbDriverZdpActiveEPRspCb - error: "
          "unexpected context type here (port:%d) (type:%d)\n", port,
          reqContext->type);
      reqContext->free(reqContext);
    }
    break;
  }
}

/**
 * @brief Call back method for ZDP Bind_rsp received message.
 *
 * @param port the port context for the targeted hardware device
 * @param issuerContext The context handler of the issuer, as provided on API.
 * @param nwkAddrSrc Network address of the node that sent this message (retrieved from
 * NWK layer).
 * @param status The status of the Bind_req command.
 */
void zbDriverZdpBindRspCb (
  azap_uint8 port,
  void *issuerContext,
  azap_uint16 nwkAddrSrc,
  azap_uint8 status)
{
  zigbeeNode_t *node;
  driverReqContext_t *reqContext = (driverReqContext_t *) issuerContext;
  if (!reqContext)
  {
    return;
  }

  switch (reqContext->type)
  {
    case DRV_REQ_FROM_CLI:
    {
      int fd = (long)reqContext->issuerData;
      t_cli *cl = TbCli[fd];
      if (cl)
      {
        AdmPrint(cl, "ZDO Bind status response from 0x%.4x: %s (%d)\n",
            nwkAddrSrc, (0 == status) ? "OK" : "ERROR", status);
      }
      reqContext->free(reqContext);
    }
    break;

    case DRV_REQ_DEVICE_AUDIT:
    {
      reqContext->nbPendingRequests --;
      node = theHan->getNodeByNwkAddr(theHan, nwkAddrSrc);
      if (0 >= reqContext->nbPendingRequests)
      {
        reqContext->free(reqContext);
        node->endAudit(node, true, theHan->panId, theHan->panIdExt,
            theHan->rfChan, knownSensorPath);
      }
      else
      {
        RTL_TRDBG(TRACE_DETAIL, "zbDriverZdpBindRspCb - still %d responses "
            "expected for device audit completion (port:%d)\n",
            reqContext->nbPendingRequests, port);
      }
    }
    break;

    case DRV_REQ_OPERATION_FROM_DIA:
    {
      if (ZCL_STATUS_SUCCESS == status)
      {
        diaIncomingRequestCompletion((long)reqContext->issuerData, NULL, 0, "",
            "STATUS_CREATED");
      }
      else
      {
        sendDIaFailureResponse((long)reqContext->issuerData, status, "ZDO");
      }
      reqContext->free(reqContext);
    }
    break;

    default:
    {
      RTL_TRDBG(TRACE_ERROR, "zbDriverZdpBindRspCb - error: "
          "unexpected context type here (port:%d) (type:%d)\n", port,
          reqContext->type);
      reqContext->free(reqContext);
    }
    break;
  }
}

/**
 * @brief Call back method for ZDP Unbind_rsp received message.
 *
 * @param port the port context for the targeted hardware device
 * @param issuerContext The context handler of the issuer, as provided on API.
 * @param nwkAddrSrc Network address of the node that sent this message (retrieved from
 * NWK layer).
 * @param status The status of the Unbind_req command.
 */
void zbDriverZdpUnbindRspCb (
  azap_uint8 port,
  void *issuerContext,
  azap_uint16 nwkAddrSrc,
  azap_uint8 status)
{
  driverReqContext_t *reqContext = (driverReqContext_t *) issuerContext;

  if (!reqContext)
  {
    return;
  }

  switch (reqContext->type)
  {
    case DRV_REQ_OPERATION_FROM_DIA:
    {
      if (ZCL_STATUS_SUCCESS == status)
      {
        diaIncomingRequestCompletion((long)reqContext->issuerData, NULL, 0, "",
            "STATUS_CREATED");
      }
      else
      {
        sendDIaFailureResponse((long)reqContext->issuerData, status, "ZDO");
      }
      reqContext->free(reqContext);
    }
    break;

    default:
    {
      RTL_TRDBG(TRACE_ERROR, "zbDriverZdpUnbindRspCb - error: "
          "unexpected context type here (port:%d) (type:%d)\n", port,
          reqContext->type);
      reqContext->free(reqContext);
    }
    break;
  }
}

/**
 * @brief Call back method for ZDP Mgmt_Rtg_rsp received message.
 *
 * @param port the port context for the targeted hardware device
 * @param issuerContext The context handler of the issuer, as provided on API.
 * @param nwkAddrSrc Network address of the node that sent this message (retrieved from
 * NWK layer).
 * @param status
 * @param routingTableEntries
 * @param startIndex
 * @param routingTableListCount
 * @param routeTableList
 */
void zbDriverZdpMgmtRtgRspCb (
  azap_uint8 port,
  void *issuerContext,
  azap_uint16 nwkAddrSrc,
  azap_uint8 status,
  azap_uint8 routingTableEntries,
  azap_uint8 startIndex,
  azap_uint8 routingTableListCount,
  zdoRtgEntry_t routeTableList[])
{
  driverReqContext_t *reqContext = (driverReqContext_t *) issuerContext;
  char szTemp[8];
  void *xo, *obj;
  azap_uint8 i;
  zdoRtgEntry_t *entry;
  char *content = NULL;
  char *contentType = "";
  int contentSize = 0;

  if (!reqContext)
  {
    return;
  }

  switch (reqContext->type)
  {
    case DRV_REQ_OPERATION_FROM_DIA:
    {
      if (ZCL_STATUS_SUCCESS == status)
      {
        // generate the body from templates
        snprintf(szTemp, sizeof(szTemp), "%d", routingTableEntries);
        SetVar("a_configVal0", szTemp);
        snprintf(szTemp, sizeof(szTemp), "%d", startIndex);
        SetVar("a_configVal1", szTemp);
        snprintf(szTemp, sizeof(szTemp), "%d", routingTableListCount);
        SetVar("a_configVal2", szTemp);
        xo = WXoNewTemplate("ope_resp_mgmt_rtg.xml", XOML_PARSE_OBIX);
        if (xo)
        {
          for (i = 0; i < routingTableListCount; i++)
          {
            entry = &routeTableList[i];
            SetVar("a_configVal0", azap_uint16ToString(entry->dstAddress, true));
            SetVar("a_configVal1", azap_uint8ToString(entry->status, true));
            SetVar("a_configVal2", azap_uint16ToString(entry->nextHopAddress, true));
            obj = WXoNewTemplate("mgmt_rtg_inst.xml", XOML_PARSE_OBIX);
            if (obj)
            {
              XoNmAddInAttr(xo, "[name=zbRoutingTableList].[]", obj, 0, 0);
            }
          }
          content = WXoSerializeFree(xo, XO_FMT_STD_XML, XOML_PARSE_OBIX, &contentSize,
              &contentType);
        }
        diaIncomingRequestCompletion((long)reqContext->issuerData, content, contentSize,
            contentType, "STATUS_CREATED");
      }
      else
      {
        sendDIaFailureResponse((long)reqContext->issuerData, status, "ZDO");
      }
      reqContext->free(reqContext);
    }
    break;

    default:
    {
      RTL_TRDBG(TRACE_ERROR, "zbDriverZdpMgmtRtgRspCb - error: "
          "unexpected context type here (port:%d) (type:%d)\n", port,
          reqContext->type);
      reqContext->free(reqContext);
    }
    break;
  }
}

/**
 * @brief Call back method for ZDP Mgmt_Bind_rsp received message.
 *
 * @param port the port context for the targeted hardware device
 * @param issuerContext The context handler of the issuer, as provided on API.
 * @param nwkAddrSrc Network address of the node that sent this message (retrieved from
 * NWK layer).
 * @param status
 * @param bindingTableEntries
 * @param startIndex
 * @param bindingTableListCount
 * @param bindingTableList
 */
void zbDriverZdpMgmtBindRspCb (
  azap_uint8 port,
  void *issuerContext,
  azap_uint16 nwkAddrSrc,
  azap_uint8 status,
  azap_uint8 bindingTableEntries,
  azap_uint8 startIndex,
  azap_uint8 bindingTableListCount,
  zdoBindEntry_t bindingTableList[])
{
  driverReqContext_t *reqContext = (driverReqContext_t *) issuerContext;
  char szTemp[8];
  void *xo, *obj;
  azap_uint8 i;
  zdoBindEntry_t *entry;
  char *content = NULL;
  char *contentType = "";
  int contentSize = 0;

  if (!reqContext)
  {
    return;
  }

  switch (reqContext->type)
  {
    case DRV_REQ_OPERATION_FROM_DIA:
    {
      if (ZCL_STATUS_SUCCESS == status)
      {
        // generate the body from templates
        snprintf(szTemp, sizeof(szTemp), "%d", bindingTableEntries);
        SetVar("a_configVal0", szTemp);
        snprintf(szTemp, sizeof(szTemp), "%d", startIndex);
        SetVar("a_configVal1", szTemp);
        snprintf(szTemp, sizeof(szTemp), "%d", bindingTableListCount);
        SetVar("a_configVal2", szTemp);
        xo = WXoNewTemplate("ope_resp_mgmt_bind.xml", XOML_PARSE_OBIX);
        if (xo)
        {
          for (i = 0; i < bindingTableListCount; i++)
          {
            entry = &bindingTableList[i];
            SetVar("a_configVal0", azap_extAddrToString(entry->srcAddr, true));
            SetVar("a_configVal1", azap_uint8ToString(entry->srcEndPoint, true));
            SetVar("a_configVal2", azap_uint16ToString(entry->clusterId, true));
            SetVar("a_configVal3", azap_uint8ToString(entry->dstAddr.addrMode, true));
            if (ZDO_RTG_ADDR_MODE_16BIT == entry->dstAddr.addrMode)
            {
              SetVar("a_configVal4", azap_uint16ToString(entry->dstAddr.addr.shortAddr, true));
            }
            else
            {
              SetVar("a_configVal4", azap_extAddrToString(entry->dstAddr.addr.extAddr, true));
            }
            SetVar("a_configVal5", azap_uint8ToString(entry->dstEndPoint, true));
            obj = WXoNewTemplate("mgmt_bind_inst.xml", XOML_PARSE_OBIX);
            if (obj)
            {
              XoNmAddInAttr(xo, "[name=zbBindingTableList].[]", obj, 0, 0);
            }
          }
          content = WXoSerializeFree(xo, XO_FMT_STD_XML, XOML_PARSE_OBIX, &contentSize,
              &contentType);
        }
        diaIncomingRequestCompletion((long)reqContext->issuerData, content, contentSize,
            contentType, "STATUS_CREATED");
      }
      else
      {
        sendDIaFailureResponse((long)reqContext->issuerData, status, "ZDO");
      }
      reqContext->free(reqContext);
    }
    break;

    default:
    {
      RTL_TRDBG(TRACE_ERROR, "zbDriverZdpMgmtBindRspCb - error: "
          "unexpected context type here (port:%d) (type:%d)\n", port,
          reqContext->type);
      reqContext->free(reqContext);
    }
    break;
  }
}

/**
 * @brief Call back method for ZDP Mgmt_Permit_Join_rsp received message.
 *
 * @param port the port context for the targeted hardware device
 * @param issuerContext The context handler of the issuer, as provided on API.
 * @param nwkAddrSrc Network address of the node that sent this message (retrieved from
 * NWK layer).
 * @param status The status of the Mgmt_Permit_Join_req command.
 */
void zbDriverZdpMgmtPermitJoinRspCb (
  azap_uint8 port,
  void *issuerContext,
  azap_uint16 nwkAddrSrc,
  azap_uint8 status)
{
  driverReqContext_t *reqContext = (driverReqContext_t *) issuerContext;

  if (!reqContext)
  {
    return;
  }

  switch (reqContext->type)
  {
    case DRV_REQ_FROM_CLI:
    {
      int fd = (long)reqContext->issuerData;
      t_cli *cl = TbCli[fd];
      if (cl)
      {
        AdmPrint(cl, "ZDO Mgmt Permit join status response from 0x%.4x: %s (%d)\n", nwkAddrSrc,
            (0 == status) ? "OK" : "ERROR", status);
      }
      reqContext->free(reqContext);
    }
    break;

    case DRV_REQ_OPERATION_FROM_DIA:
    {
      if (ZCL_STATUS_SUCCESS == status)
      {
        diaIncomingRequestCompletion((long)reqContext->issuerData, NULL, 0, "",
            "STATUS_CREATED");
      }
      else
      {
        diaIncomingRequestCompletion((long)reqContext->issuerData, NULL, 0, "",
            "STATUS_INTERNAL_SERVER_ERROR");
      }
      reqContext->free(reqContext);
    }
    break;

    case DRV_REQ_OPERATION_FROM_DIA_NO_COMPLETION:
    {
      // silent completion
      reqContext->free(reqContext);
    }
    break;

    default:
    {
      RTL_TRDBG(TRACE_ERROR, "zbDriverZdpBindRspCb - error: "
          "unexpected context type here (port:%d) (type:%d)\n", port,
          reqContext->type);
      reqContext->free(reqContext);
    }
    break;
  }
}

/**
 * @brief Call back method for ZDP Mgmt_Leave_rsp received message.
 *
 * @param port the port context for the targeted hardware device
 * @param issuerContext The context handler of the issuer, as provided on API.
 * @param nwkAddrSrc Network address of the node that sent this message (retrieved from
 * NWK layer).
 * @param status The status of the Mgmt_Leave_req command.
 */
void zbDriverZdpMgmtLeaveRspCb (
  azap_uint8 port,
  void *issuerContext,
  azap_uint16 nwkAddrSrc,
  azap_uint8 status)
{
  driverReqContext_t *reqContext = (driverReqContext_t *) issuerContext;

  if (!reqContext)
  {
    return;
  }

  switch (reqContext->type)
  {
    case DRV_REQ_OPERATION_FROM_DIA:
    {
      if (ZCL_STATUS_SUCCESS == status)
      {
        diaIncomingRequestCompletion((long)reqContext->issuerData, NULL, 0, "",
            "STATUS_CREATED");
      }
      else
      {
        sendDIaFailureResponse((long)reqContext->issuerData, status, "ZDO");
      }
      reqContext->free(reqContext);
    }
    break;

    default:
    {
      RTL_TRDBG(TRACE_ERROR, "zbDriverZdpMgmtLeaveRspCb - error: "
          "unexpected context type here (port:%d) (type:%d)\n", port,
          reqContext->type);
      reqContext->free(reqContext);
    }
    break;
  }
}


/* SAPI call backs */
void zbDriverSapiFindDeviceConfCb (
  azap_uint8 port,
  azap_uint16 nwkAddr,
  azap_uint8 ieeeAddr[IEEE_ADDR_LEN])
{
  printf("AZAP SapiFindDeviceConfCb\n");
}


/* ZCL call backs */
/**
 * @brief Call back function for ZCL "Read" response command.
 * @param port the port context for the targeted hardware device
 * @param issuerContext The context handler of the issuer, as provided on API.
 * @param nwkAddrSrc the short address of the node that send the response.
 * @param endpoint the end point identifier on the remote node that send the response.
 * @param clusterId the cluster identifier that the command targets.
 * @param clusterDir the cluster direction that the command targets.
 * @param manufacturerCode the manufacturer code if the command targets specific attributes,
 * 0 otherwise.
 * @param data the ReadCmd response.
 */
void zbDriverZclReadCmdRspCb (
  azap_uint8 port,
  void *issuerContext,
  azap_uint16 nwkAddrSrc,
  azap_uint8 endpoint,
  azap_uint16 clusterId,
  azap_uint8 clusterDir,
  azap_uint16 manufacturerCode,
  azapZclReadRspCommand_t *data)
{
  zigbeeNode_t *node;
  driverReqContext_t *reqContext = (driverReqContext_t *) issuerContext;
  azap_byte *pPtr;
  azapZclAttrVal_t *attrVal;
  azap_uint8 i, len;
  azapEndPoint_t *ep;
  azapAttribute_t *attr;
  void *obix = NULL;
  int size = 0;
  char *type;
  char *content;
  char szTemp[32];

  if (!reqContext)
  {
    return;
  }

  switch (reqContext->type)
  {
    case DRV_REQ_DEVICE_AUDIT:
    {
      reqContext->nbPendingRequests --;
      node = theHan->getNodeByNwkAddr(theHan, nwkAddrSrc);

      if ((data) && (data->numAttr))
      {
        ep = node->getEndPoint(node, endpoint);
        for (i = 0; i < data->numAttr; i++)
        {
          if (ZCL_STATUS_SUCCESS == data->attrList[i].status)
          {
            attr = ep->addAttribute(ep, clusterId, data->attrList[i].attrId);
            attr->manufacturerCode = manufacturerCode;
            attr->value = new_azapZclAttrVal_t(data->attrList[i].dataType);
            len = azapGetZclDataLength(data->attrList[i].dataType,
                data->attrList[i].data);
            pPtr = data->attrList[i].data;
            attr->value->deserialize(attr->value, &pPtr, &len);
          }
        }
      }

      if (0 >= reqContext->nbPendingRequests)
      {
        reqContext->free(reqContext);
        node->endAudit(node, true, theHan->panId, theHan->panIdExt,
            theHan->rfChan, knownSensorPath);
      }
      else
      {
        RTL_TRDBG(TRACE_DETAIL, "zbDriverZclReadCmdRspCb - still %d responses "
            "expected for device audit completion (port:%d)\n",
            reqContext->nbPendingRequests, port);
      }
    }
    break;

    case DRV_REQ_READ_ATTR_FROM_DIA:
    {
      // the status provided in case of request timeout
      i = ZCL_STATUS_FAILURE;
      if ((data) && (data->numAttr))
      {
        if (ZCL_STATUS_SUCCESS == data->attrList[0].status)
        {
          attrVal = new_azapZclAttrVal_t(data->attrList[0].dataType);
          len = azapGetZclDataLength(data->attrList[0].dataType,
              data->attrList[0].data);
          pPtr = data->attrList[0].data;
          attrVal->deserialize(attrVal, &pPtr, &len);
          // TODO: need to be more precise, maybe require to implement getOBIXRep in
          // azapZclAttrVal_t
          switch (attrVal->valueType)
          {
            case AZAP_BOOLEAN_VAL:
              obix = XoNmNew("o:bool");
              break;
            case AZAP_FLOAT_VAL:
              obix = XoNmNew("o:real");
              break;
            case AZAP_INTEGER_VAL:
              obix = XoNmNew("o:int");
              break;
            case AZAP_CHAR_STRING_VAL:
              obix = XoNmNew("o:str");
              break;
            case AZAP_DATE_TIME_VAL:
              obix = XoNmNew("o:abstime");
              break;
            case AZAP_RAW_VAL:
            default:
              obix = XoNmNew("o:enum");
              break;
          }
          if (obix)
          {
            sprintf(szTemp, "0x%.4x.0x%.4x.0.attr", clusterId, data->attrList[0].attrId);
            XoNmSetAttr(obix, "name", GetVar(szTemp), 0);
            XoNmSetAttr(obix, "val", attrVal->getStringRep(attrVal), 0);
          }
        }
        else
        {
          i = data->attrList[0].status;
        }
      }

      if (obix)
      {
        content = WXoSerializeFree(obix, XO_FMT_STD_XML, XOML_PARSE_OBIX, &size, &type);
        diaIncomingRequestCompletion((long)reqContext->issuerData, content, size, type,
            "STATUS_OK");
        if (content)
        {
          free(content);
        }
      }
      else
      {
        sendDIaFailureResponse((long)reqContext->issuerData, i, "ZCL READ");
      }
      reqContext->free(reqContext);
    }
    break;

    default:
    {
      RTL_TRDBG(TRACE_ERROR, "zbDriverZclReadCmdRspCb - error: "
          "unexpected context type here (port:%d) (type:%d)\n", port,
          reqContext->type);
      reqContext->free(reqContext);
    }
    break;
  }
}

/**
 * @brief Call back function for ZCL "Write" response command.
 * @param port the port context for the targeted hardware device
 * @param issuerContext The context handler of the issuer, as provided on API.
 * @param nwkAddrSrc the short address of the node that send the response.
 * @param endPoint the end point identifier on the remote node that send the response.
 * @param clusterId the cluster identifier that the command targets.
 * @param clusterDir the cluster direction that the command targets.
 * @param manufacturerCode the manufacturer code if the command targets specific attributes,
 * 0 otherwise.
 * @param data the WriteCmd response.
 */
void zbDriverZclWriteCmdRspCb (
  azap_uint8 port,
  void *issuerContext,
  azap_uint16 nwkAddrSrc,
  azap_uint8 endPoint,
  azap_uint16 clusterId,
  azap_uint8 clusterDir,
  azap_uint16 manufacturerCode,
  azapZclWriteRspCommand_t *data)
{
  driverReqContext_t *reqContext = (driverReqContext_t *) issuerContext;
  azap_uint8 status;
  if (!reqContext)
  {
    return;
  }

  switch (reqContext->type)
  {
    case DRV_REQ_WRITE_ATTR_FROM_DIA:
    {
      // the status provided in case of request timeout
      status = ZCL_STATUS_FAILURE;
      if ((data) && (data->numAttr))
      {
        status = data->attrList[0].status;
      }

      if (ZCL_STATUS_SUCCESS == status)
      {
        diaIncomingRequestCompletion((long)reqContext->issuerData, NULL, 0, "",
            "STATUS_OK");
      }
      else
      {
        sendDIaFailureResponse((long)reqContext->issuerData, status, "ZCL WRITE");
      }
      free(reqContext->issuerData2);
      reqContext->free(reqContext);
    }
    break;

    default:
    {
      RTL_TRDBG(TRACE_ERROR, "zbDriverZclWriteCmdRspCb - error: "
          "unexpected context type here (port:%d) (type:%d)\n", port,
          reqContext->type);
      reqContext->free(reqContext);
    }
    break;
  }
}

/**
 * @brief Call back function for ZCL "ConfigureReporting" response command.
 * @param port the port context for the targeted hardware device
 * @param issuerContext The context handler of the issuer, as provided on API.
 * @param nwkAddrSrc the short address of the node that send the response.
 * @param endPoint the end point identifier on the remote node that send the response.
 * @param clusterId the cluster identifier that the command targets.
 * @param data the ConfigureReportingCmd command.
 */
void zbDriverZclConfigureReportCmdRspCb (
  azap_uint8 port,
  void *issuerContext,
  azap_uint16 nwkAddrSrc,
  azap_uint8 endPoint,
  azap_uint16 clusterId,
  azapZclCfgReportRspCommand_t *data)
{
  driverReqContext_t *reqContext = (driverReqContext_t *) issuerContext;
  zigbeeNode_t *node;
  azap_uint8 status = 0xff;
  azap_uint16 attrId = 0xffff;

  if (!reqContext)
  {
    return;
  }

  switch (reqContext->type)
  {
    case DRV_REQ_FROM_CLI:
    {
      int fd = (long)reqContext->issuerData;
      t_cli *cl = TbCli[fd];
      if (cl)
      {
        if ((data) && (data->numAttr))
        {
          status = data->attrList[0].status;
          attrId = data->attrList[0].attrId;
        }
        AdmPrint(cl, "ZCL Configure Reporting status response from 0x%.4x: %s (%d) (attrId:0x%.4x)\n",
            nwkAddrSrc, (0 == status) ? "OK" : "ERROR", status, attrId);
      }
      reqContext->free(reqContext);
    }
    break;

    case DRV_REQ_DEVICE_AUDIT:
    {
      reqContext->nbPendingRequests --;
      if (0 >= reqContext->nbPendingRequests)
      {
        node = theHan->getNodeByNwkAddr(theHan, nwkAddrSrc);
        reqContext->free(reqContext);
        node->endAudit(node, true, theHan->panId, theHan->panIdExt,
            theHan->rfChan, knownSensorPath);
      }
      else
      {
        RTL_TRDBG(TRACE_DETAIL, "zbDriverZclConfigureReportCmdRspCb - still %d responses "
            "expected for device audit completion (port:%d)\n",
            reqContext->nbPendingRequests, port);
      }

    }
    break;

    default:
    {
      RTL_TRDBG(TRACE_ERROR, "zbDriverZclConfigureReportCmdRspCb - error: "
          "unexpected context type here (port:%d) (type:%d)\n", port,
          reqContext->type);
      reqContext->free(reqContext);
    }
    break;
  }
}

/**
 * @brief Call back function for ZCL "ReportAttribute" command.
 * @param port the port context for the targeted hardware device
 * @param nwkAddrSrc the short address of the node that send the response.
 * @param endPoint the end point identifier on the remote node that send the response.
 * @param clusterId the cluster identifier that the command targets.
 * @param data the ReportAttributeCmd message content.
 */
void zbDriverZclReportCmdCb (
  azap_uint8 port,
  azap_uint16 nwkAddrSrc,
  azap_uint8 endPoint,
  azap_uint16 clusterId,
  azapZclReportCommand_t *data)
{
  int i;
  azapZclAttrVal_t *val;
  azap_byte *pPtr;
  azap_uint8 len;
  m2mZclAbstraction_t *abstraction;
  char *m2mVal;
  zigbeeNode_t *node = theHan->getNodeByNwkAddr(theHan, nwkAddrSrc);

  if (!data)
  {
    return;
  }
  if (!node)
  {
    RTL_TRDBG(TRACE_INFO, "zbDriverZclReportCmdCb - warning: unknown device "
        "(port:%d) (nwkAddr:0x%.4x) (endPoint:0x%.2x) (clusterId:0x%.4x)\n",
        port, nwkAddrSrc, endPoint, clusterId);
    return;
  }
  if (node->azapAuditContext)
  {
    RTL_TRDBG(TRACE_INFO, "zbDriverZclReportCmdCb - ignoring received ZCL Report as "
        "the coordinator is auditing the device (port:%d) (nwkAddr:0x%.4x) "
        "(endPoint:0x%.2x) (clusterId:0x%.4x)\n", port, nwkAddrSrc, endPoint, clusterId);
    return;
  }
  abstraction = node->getM2mZclAbstraction(node, endPoint, clusterId);
  if (!abstraction)
  {
    RTL_TRDBG(TRACE_ERROR, "zbDriverZclReportCmdCb - error: unable to find "
        "associated abstraction (port:%d) (nwkAddr:0x%.4x) (endPoint:0x%.2x) "
        "(clusterId:0x%.4x)\n", port, nwkAddrSrc, endPoint, clusterId);
    return;
  }

  for ( i = 0; i < data->numAttr; i++)
  {
    if (abstraction->canReport(abstraction, data->attrList[i].attrId))
    {
      val = new_azapZclAttrVal_t(data->attrList[i].dataType);
      pPtr = data->attrList[i].attrData;
      len = azapGetZclDataLength(data->attrList[i].dataType, data->attrList[i].attrData);
      val->deserialize(val, &pPtr, &len);
      m2mVal = abstraction->getConsolidatedData(abstraction, data->attrList[i].attrId, val);
      if (m2mVal)
      {
        DiaReportValue(node->cp_cmn.cmn_num, node->cp_cmn.cmn_serial, endPoint, clusterId,
            data->attrList[i].attrId, 0, "%s", m2mVal);
      }
      else
      {
        RTL_TRDBG(TRACE_INFO, "zbDriverZclReportCmdCb - ignoring received ZCL Report, "
            "unsupported attribute (attrId:0x%.4x)\n", data->attrList[i].attrId);
      }

      val->free(val);
    }
    else
    {
      RTL_TRDBG(TRACE_INFO, "zbDriverZclReportCmdCb - ignoring received ZCL Report, "
          "minimal reporting interval not respected (attrId:0x%.4x)\n",
          data->attrList[i].attrId);
    }
  }

}

/**
 * @brief Call back function for ZCL "Discover" command.
 * @param port the port context for the targeted hardware device
 * @param issuerContext The context handler of the issuer, as provided on API.
 * @param nwkAddrSrc the short address of the node that send the response.
 * @param endpoint the end point identifier on the remote node that send the response.
 * @param clusterId the cluster identifier that the command targets.
 * @param clusterDir the cluster direction that the command targets.
 * @param data the DiscoverCmd response.
 */
void zbDriverZclDiscoverCmdRspCb(
  azap_uint8 port,
  void *issuerContext,
  azap_uint16 nwkAddrSrc,
  azap_uint8 endpoint,
  azap_uint16 clusterId,
  azap_uint8 clusterDir,
  azapZclDiscoverRspCommand_t *data)
{
  driverReqContext_t *reqContext = (driverReqContext_t *) issuerContext;
  zigbeeNode_t *node;
  azapEndPoint_t *ep = NULL;
  azapZclReadCommand_t *readCmd;
  azapZclWriteCommand_t *writeCmd;
  azapZclCfgReportCommand_t *cfgReportCmd;
  int nb;
  void *itf;
  azap_uint8 indexAttr;
  azap_uint8 reqNumAttr;
  azap_uint8 i;
  azapZclAttrVal_t *attrVal;
  azap_byte writeValue[AZAP_ATTR_VAL_MAX_SIZE];
  azap_byte *pPtr;
  m2mZclAbstraction_t *abstraction;

  if (!reqContext)
  {
    return;
  }

  switch (reqContext->type)
  {
    case DRV_REQ_DEVICE_AUDIT:
    {
      reqContext->nbPendingRequests --;
      node = theHan->getNodeByNwkAddr(theHan, nwkAddrSrc);
      if ((node) && (data) && (data->numAttr))
      {
        indexAttr = 0;

        while (indexAttr != data->numAttr)
        {
          // Some NetVox sensors does not response to ZCL ReadCmd when
          // more than 8 attributes are requested.
          if (data->numAttr - indexAttr > 8)
          {
            reqNumAttr = 8;
          }
          else
          {
            reqNumAttr = data->numAttr - indexAttr;
          }
          // NOTE: we ignore here the 'discComplete' parameter in the response. If more
          // attributes exists, or if the device is not able to send all attributes in the
          // response, we'll miss some attribute values
          ep = node->getEndPoint(node, endpoint);
          readCmd = malloc((reqNumAttr * sizeof(azap_uint16)) + sizeof(azapZclReadCommand_t) );
          readCmd->numAttr = reqNumAttr;
          for (i = 0; i < reqNumAttr; i++)
          {
            readCmd->attrId[i] = data->attrList[i + indexAttr].attrId;
          }
          // continue the device audit
          if (azapZclReadCmdReq(port, AZAP_REQ_TTL_START_AT_ZB_SEND, reqContext, nwkAddrSrc,
              getMatchingAdminEndPoint(ep->appProfId), endpoint, clusterId,
              ZCL_FRAME_CLIENT_SERVER_DIR, 0, readCmd))
          {
            reqContext->nbPendingRequests ++;
          }
          free(readCmd);
          indexAttr += reqNumAttr;
        }

        if (NULL != (itf = MdlGetInterface(MdlCfg, NULL, "server",
            azap_uint16ToString(clusterId, true))))
        {
          // look for covConfigurations entries (in modelingconfig.xml) if there is
          // at least one entry; if yes, bind and configure the device
          if ((nb = MdlGetNbCovConfigurationEntries(itf)) > 0)
          {
            if (azapZdpBindReq(port, AZAP_REQ_TTL_START_AT_ZB_SEND, reqContext,
                nwkAddrSrc, node->ieeeAddr, endpoint, clusterId, theHan->coordExtAddr,
                getMatchingAdminEndPoint(ep->appProfId)))
            {
              reqContext->nbPendingRequests ++;
            }

            cfgReportCmd = getCfgReportCmdFromCovConfig(itf, nb, data);
            // retrieve associated abstraction
            abstraction = node->getM2mZclAbstraction(node, endpoint, clusterId);
            // and propagate the configuration to the abstraction if found
            if (abstraction)
            {
              abstraction->configureReporting(abstraction, cfgReportCmd);
            }
            // and finally send the ZCL ConfigureReporting command
            if ((cfgReportCmd) && (azapZclConfigureReportCmdReq(port,
                AZAP_REQ_TTL_START_AT_ZB_SEND, reqContext,
                nwkAddrSrc, getMatchingAdminEndPoint(ep->appProfId), endpoint,
                clusterId, ZCL_FRAME_CLIENT_SERVER_DIR, cfgReportCmd)))
            {
              reqContext->nbPendingRequests ++;
            }
            // free the resource
            for (i = 0; i < cfgReportCmd->numAttr; i++)
            {
              if (cfgReportCmd->attrList[i].reportableChange)
              {
                free(cfgReportCmd->attrList[i].reportableChange);
              }
            }
            free(cfgReportCmd);
          }
          else
          {
            RTL_TRDBG(TRACE_INFO, "zbDriverZclDiscoverCmdRspCb - no cov config "
                "(cluster:0x%.4x)\n", clusterId);
          }
        }
        else
        {
          RTL_TRDBG(TRACE_INFO, "zbDriverZclDiscoverCmdRspCb - no matching interface "
              "(cluster:0x%.4x)\n", clusterId);
        }
      }
      else
      {
      }

      if (0 >= reqContext->nbPendingRequests)
      {
        reqContext->free(reqContext);
        node->endAudit(node, true, theHan->panId, theHan->panIdExt,
            theHan->rfChan, knownSensorPath);
      }
      else
      {
        RTL_TRDBG(TRACE_DETAIL, "zbDriverZclDiscoverCmdRspCb - still %d responses "
            "expected for device audit completion (port:%d)\n",
            reqContext->nbPendingRequests, port);
      }

    }
    break;

    case DRV_REQ_WRITE_ATTR_FROM_DIA:
    {
      i = 0; // i indicates if there is an error
      node = theHan->getNodeByNwkAddr(theHan, nwkAddrSrc);
      if ((node) && (data) && (data->numAttr))
      {
        ep = node->getEndPoint(node, endpoint);
        if (!ep)
        {
          i++;
        }
        attrVal = new_azapZclAttrVal_t(data->attrList[0].dataType);
        if (! attrVal->buildFromStringRep(attrVal, reqContext->issuerData2))
        {
          i++;
        }
      }
      else
      {
        i++;
      }

      if (0 == i)
      {
        writeCmd = malloc(sizeof(azapZclWriteCommand_t) + sizeof(azapZclWriteAttr_t));
        if (!writeCmd)
        {
          i++;
        }
      }

      if (0 == i)
      {
        memset(writeValue, 0, AZAP_ATTR_VAL_MAX_SIZE);
        pPtr = writeValue;
        indexAttr = AZAP_ATTR_VAL_MAX_SIZE;
        attrVal->serialize(attrVal, &pPtr, &indexAttr);
        writeCmd->numAttr = 1;
        writeCmd->attrList[0].attrId = data->attrList[0].attrId;
        writeCmd->attrList[0].dataType = data->attrList[0].dataType;
        writeCmd->attrList[0].attrData = writeValue;

        if (!azapZclWriteCmdReq(port, AZAP_REQ_TTL_START_AT_API, reqContext,
            node->cp_cmn.cmn_num, getMatchingAdminEndPoint(ep->appProfId), ep->id, clusterId,
            ZCL_FRAME_CLIENT_SERVER_DIR, 0, false, writeCmd))
        {
          i++;
        }
      }

      if (i)
      {
        sendDIaFailureResponse((long)reqContext->issuerData, i, "ZCL WRITE");
        free(reqContext->issuerData2);
        reqContext->free(reqContext);
      }
    }
    break;

    default:
    {
      RTL_TRDBG(TRACE_ERROR, "zbDriverZclDiscoverCmdRspCb - error: "
          "unexpected context type here (port:%d) (type:%d)\n", port,
          reqContext->type);
      reqContext->free(reqContext);
    }
    break;
  }
}

/**
 * @brief Call back function for ZCL generic command (i.e. not foundation command).
 * @param port the port context for the targeted hardware device
 * @param issuerContext The context handler of the issuer, as provided on API. (may be NULL
 * if it does not match any expected response to a ZCL command priorly sent)
 * @param nwkAddrSrc the short address of the node that send the response.
 * @param endPoint the end point identifier on the remote node that send the response.
 * @param clusterId the cluster identifier that the command targets.
 * @param cmdCode the ZCL command code.
 * @param cmdData the ZCL command payload.
 * @param cmdLen the length of the ZCL command payload.
 */
void zbDriverZclIncomingCmdCb (
  azap_uint8 port,
  void *issuerContext,
  azap_uint16 nwkAddrSrc,
  azap_uint8 endPoint,
  azap_uint16 clusterId,
  azap_uint8 cmdCode,
  azap_byte cmdData[],
  azap_uint8 cmdLen)
{
  m2mZclAbstraction_t *abstraction;
  zigbeeNode_t *node;
  driverReqContext_t *reqContext = (driverReqContext_t *) issuerContext;

  if (!reqContext)
  {
    return;  // unsupported yet
  }

  node = theHan->getNodeByNwkAddr(theHan, nwkAddrSrc);
  if (!node)
  {
    RTL_TRDBG(TRACE_ERROR, "zbDriverZclIncomingCmdCb - error: unknown device "
        "(port:%d) (nwkAddr:0x%.4x) (endPoint:0x%.2x) (clusterId:0x%.4x) (cmd:0x%.2x)\n",
        port, nwkAddrSrc, endPoint, clusterId, cmdCode);
    return;
  }

  switch (reqContext->type)
  {
    case DRV_REQ_OPERATION_FROM_DIA:
    {
      abstraction = node->getM2mZclAbstraction(node, endPoint, clusterId);
      if (!abstraction)
      {
        RTL_TRDBG(TRACE_ERROR, "zbDriverZclIncomingCmdCb - error: unable to find "
            "associated abstraction (port:%d) (nwkAddr:0x%.4x) (endPoint:0x%.2x) "
            "(clusterId:0x%.4x) (cmd:0x%.2x)\n", port, nwkAddrSrc, endPoint, clusterId,
            cmdCode);
        return;
      }
      else
      {
        abstraction->doZclOperationResp(abstraction, cmdCode, cmdData, cmdLen,
            (long)reqContext->issuerData);
      }

      reqContext->free(reqContext);
    }
    break;

    default:
      reqContext->free(reqContext);
      break;
  }

}


/* when used as a node */
/**
 * @brief Call back function for ZCL "configureReport" command.
 * @param port the port context for the targeted hardware device
 * @param nwkAddrDest the short address of the node that send the response.
 * @param endPoint the end point identifier on the remote node that send the response.
 * @param clusterId the cluster identifier that the command targets.
 * @param data the "ConfigureReporting" command parameters.
 */
void zbDriverZclConfigureReportCmdCb (
  azap_uint8 port,
  azap_uint16 nwkAddrDest,
  azap_uint8 endPoint,
  azap_uint16 clusterId,
  azapZclCfgReportCommand_t *data)
{
  printf("AZAP zbDriverZclConfigureReportCmdCb\n");
}


/**
 * @brief Call back function for ZCL "ReadReportConfig" command.
 * @param port the port context for the targeted hardware device
 * @param issuerContext The context handler of the issuer, as provided on API. (may be NULL
 * if it does not match any expected response to a ZCL command priorly sent)
 * @param nwkAddrSrc the short address of the node that send the response.
 * @param endPoint the end point identifier on the remote node that send the response.
 * @param clusterId the cluster identifier that the command targets.
 * @param data the "ConfigureReporting" command parameters.
 */
void zbDriverZclReadReportConfigCmdCb (
  azap_uint8 port,
  void *issuerContext,
  azap_uint16 nwkAddrSrc,
  azap_uint8 endPoint,
  azap_uint16 clusterId,
  azapZclReadReportCfgRspCommand_t *data)
{
  driverReqContext_t *reqContext = (driverReqContext_t *) issuerContext;
  azap_uint8 status = 0xff;
  azap_uint16 attrId = 0xffff;
  azap_uint8 len;
  azapZclAttrVal_t *val;

  if (!reqContext)
  {
    return;
  }

  switch (reqContext->type)
  {
    case DRV_REQ_FROM_CLI:
    {
      int fd = (long)reqContext->issuerData;
      t_cli *cl = TbCli[fd];
      if (cl)
      {
        if ((data) && (data->numAttr))
        {
          status = data->attrList[0].status;
          attrId = data->attrList[0].attrId;
        }
        AdmPrint(cl, "ZCL Read Reporting Configuration status response from 0x%.4x: %s (%d) "
            "(attrId:0x%.4x)\n", nwkAddrSrc, (0 == status) ? "OK" : "ERROR", status, attrId);
        if (0 == status)
        {
          AdmPrint(cl, "dataType: 0x%.2x\n", data->attrList[0].dataType);
          AdmPrint(cl, "minReportInt: %d\n", data->attrList[0].minReportInt);
          AdmPrint(cl, "maxReportInt: %d\n", data->attrList[0].maxReportInt);
          val = new_azapZclAttrVal_t(data->attrList[0].dataType);
          len = azapGetZclDataLength(data->attrList[0].dataType,
              data->attrList[0].reportableChange);
          val->deserialize(val, &(data->attrList[0].reportableChange), &len);
          AdmPrint(cl, "reportableChange: %s\n", val->getStringRep(val));
          val->free(val);
        }
      }
      reqContext->free(reqContext);
    }
    break;

    default:
    {
      RTL_TRDBG(TRACE_ERROR, "zbDriverZclReadReportConfigCmdCb - error: "
          "unexpected context type here (port:%d) (type:%d)\n", port,
          reqContext->type);
      reqContext->free(reqContext);
    }
    break;
  }
}


/**
 * @brief Build the data structure to use with a ZCL "ConfigureReporting" command.
 * @param itf the xo object for that represents the interface for the modeling
 * configuration.
 * @param nb the number of "covConfiguration" entries in the interface.
 * @param data the DiscoverCmd response.
 */
azapZclCfgReportCommand_t *getCfgReportCmdFromCovConfig(void *itf, int nb,
    azapZclDiscoverRspCommand_t *data)
{
  char *filter, *minInterval, *maxInterval, *minCOV;
  int attrId;
  azap_byte *pPtr;
  azap_uint8 len, i, dataType;
  azapZclAttrVal_t *val;
  void *covConfig;
  azapZclCfgReportCommand_t *cfgReportCmd = malloc((sizeof(azapZclCfgReportAttr_t) * nb) +
      sizeof(azapZclCfgReportCommand_t));

  if (!cfgReportCmd)
  {
    return NULL;
  }

  cfgReportCmd->numAttr = nb;
  while (nb > 0)
  {
    nb--; // now set to the corresponding position in the attribute table
    covConfig = MdlGetCovConfigurationByNumEntry(itf, nb);

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

    dataType = 0;
    for (i = 0; (i < data->numAttr) && (0 == dataType); i++)
    {
      if (attrId == data->attrList[i].attrId)
      {
        dataType = data->attrList[i].dataType;
      }
    }

    if (0 == dataType)
    {
      continue;  // attribute not supported by the device
    }

    minInterval = XoNmGetAttr(covConfig, "ong:minInterval", 0, 0);
    maxInterval = XoNmGetAttr(covConfig, "ong:maxInterval", 0, 0);
    minCOV = XoNmGetAttr(covConfig, "ong:minCOV", 0, 0);

    cfgReportCmd->attrList[nb].direction = ZCL_SEND_ATTR_REPORTS;
    cfgReportCmd->attrList[nb].attrId = attrId;
    cfgReportCmd->attrList[nb].timeoutPeriod = 0;
    cfgReportCmd->attrList[nb].dataType = dataType;
    cfgReportCmd->attrList[nb].minReportInt = parseISO8601Duration(minInterval);
    cfgReportCmd->attrList[nb].maxReportInt = parseISO8601Duration(maxInterval);
    // limitation here, minCOV is only supported in case of integer value...
    if (NULL != minCOV)
    {
      len = azapGetZclDataLength(dataType, NULL);
      cfgReportCmd->attrList[nb].reportableChange = malloc(len);
      pPtr = cfgReportCmd->attrList[nb].reportableChange;
      val = new_azapZclAttrVal_t(dataType);
      val->buildFromStringRep(val, minCOV);
      val->serialize(val, &pPtr, &len);
      val->free(val);
    }
    else
    {
      cfgReportCmd->attrList[nb].reportableChange = NULL;
    }
  }
  return cfgReportCmd;
}


