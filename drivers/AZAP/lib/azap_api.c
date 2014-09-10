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
//** File : ./lib/azap_api.c
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

/*
 * Copyright   Actility, SA. All Rights Reserved.
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
 *
 * id $Id$
 * author $Author$
 * version $Revision$
 * lastrevision $Date$
 * modifiedby $LastChangedBy$
 * lastmodified $LastChangedDate$
 */

#include <stdlib.h>
#include <stdio.h>
#include <signal.h>

#include <rtlbase.h>
#include <stddef.h>
#include <rtllist.h>
#include <string.h>

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
#include "format_znp.h"
#include "format_znp_zdo.h"
#include "format_znp_af.h"

#include "azap_api_impl.h"




// exported global variable
portContext_t *g_portContexts[] = { NULL, NULL, NULL, NULL, NULL };
azap_uint8 g_portContextIndex = 0;

/*
 * Local function
 */
static void t_azap_api_callbacks_free(t_azap_api_callbacks *This);
static void t_azap_api_callbacks_newFree(t_azap_api_callbacks *This);
static void t_azap_api_callbacks_Init(t_azap_api_callbacks *This);

/*
 * API handler
 */

/*
 * GLOBAL VARIABLES
 */

///////////////////////////////////////////////////////////////////////////////

/**
 * @brief Constructor for static allocation
 * @return the copy of the object
 */
t_azap_api_callbacks t_azap_api_callbacks_create()
{
  t_azap_api_callbacks This;
  t_azap_api_callbacks_Init(&This);
  This.free = t_azap_api_callbacks_free;
  return This;
}

/**
 * @brief Destructor for static allocation
 * @param This the t_azap_api_callbacks object instance
 */
void t_azap_api_callbacks_free(t_azap_api_callbacks *This)
{
  (void)This;
}

/**
 * @brief Constructor for dynamic allocation
 * @return a pointer on the object
 */
t_azap_api_callbacks *new_t_azap_api_callbacks()
{
  t_azap_api_callbacks *This = malloc(sizeof(t_azap_api_callbacks));
  if (!This)
  {
    return NULL;
  }
  t_azap_api_callbacks_Init(This);
  This->free = t_azap_api_callbacks_newFree;
  return This;
}

/**
 * @brief Destructor for dynamic allocation
 */
void t_azap_api_callbacks_newFree(t_azap_api_callbacks *This)
{
  free(This);
}

/**
 * @brief Initialize the t_azap_api_callbacks struct
 * @param This the class instance
 */
void t_azap_api_callbacks_Init(t_azap_api_callbacks *This)
{
  This->azapStartedCb = NULL;
  This->azapZdpBindReqCb = NULL;
  This->azapZdpUnbindReqCb = NULL;
  This->azapZdpDeviceAnnceCb = NULL;
  This->azapZdpNwkAddrRspCb = NULL;
  This->azapZdpIeeeAddrRspCb = NULL;
  This->azapZdpNodeDescRspCb = NULL;
  This->azapZdpPowerDescRspCb = NULL;
  This->azapZdpSimpleDescRspCb = NULL;
  This->azapZdpActiveEPRspCb = NULL;
  This->azapZdpBindRspCb = NULL;
  This->azapZdpUnbindRspCb = NULL;
  This->azapZdpMgmtRtgRspCb = NULL;
  This->azapZdpMgmtBindRspCb = NULL;
  This->azapZdpMgmtPermitJoinRspCb = NULL;
  This->azapZdpMgmtLeaveRspCb = NULL;
  This->azapSapiFindDeviceConfCb = NULL;
  This->azapZclReadCmdRspCb = NULL;
  This->azapZclWriteCmdRspCb = NULL;
  This->azapZclConfigureReportCmdRspCb = NULL;
  This->azapZclReportCmdCb = NULL;
  This->azapZclReadReportConfigCmdRspCb = NULL;
  This->azapZclDiscoverCmdRspCb = NULL;
  This->azapZclIncomingCmdCb = NULL;
  This->azapZclConfigureReportCmdCb = NULL;
}

/**
 * @brief Initialize AZAP layer, passing reference on call back listener instance.
 * The AZAP API is designed to manage several hardware devices within the same process.
 * The reference port (provided by AZAP layer through this function call) allows
 * distinguishing the hardware. One can specify the hardware it need to drive by the
 * 802.15.4 IEEE address provided with argument "targetedIeee". When not provided, the
 * AZAP look for the first found hardware device that matches the CC2531 description.
 *
 * @param targetedIeee the 802.15.4 IEEE address that the AZAP has to look for
 * auto-detection. If null, the AZAP layer will assume the first hardware device
 * that matches the CC2531 specificities.
 * @param port the port context for the hardware device (by reference)
 * @param mainTbPoll the rtl_poll global table
 * @param pCallbacks Callback functions fir AZAP events.
 * @param defaultChannel radio channel used by default to build the HAN (range:11-26)
 * @param deviceType device type to implement (zc: 0, zr: 1, zed: 2)
 * @param resetHanAtReboot specifies if HAN parameters (panId, panExtId)
 * must either be reset at AZAP startup when set to true, either be restored as
 * they were saved at previous run.
 * @return 1 if initialization succeeds, 0 otherwise
 */
bool azapInit(
  azap_uint8 *targetedIeee,
  azap_uint8 *port,
  void *mainTbPoll,
  t_azap_api_callbacks *pCallbacks,
  azap_uint8 defaultChannel,
  azap_uint8 deviceType,
  bool resetHanAtReboot)
{
  if (g_portContextIndex >= 5)
  {
    RTL_TRDBG(TRACE_ERROR, "azapInit: too many port initialized\n");
    return false;
  }

  if (0 == g_portContextIndex)
  {
    memset(azapNbZdoNotifications, 0, sizeof(azapNbZdoNotifications));
    memset(azapNbZdoRequestsIssued, 0, sizeof(azapNbZdoRequestsIssued));
    memset(azapNbZdoRequestsCompleted, 0, sizeof(azapNbZdoRequestsCompleted));
    memset(azapNbZdoRequestsFailed, 0, sizeof(azapNbZdoRequestsFailed));
    memset(azapNbZclNotifications, 0, sizeof(azapNbZclNotifications));
    memset(azapNbZclRequestsIssued, 0, sizeof(azapNbZclRequestsIssued));
    memset(azapNbZclRequestsCompleted, 0, sizeof(azapNbZclRequestsCompleted));
    memset(azapNbZclRequestsFailed, 0, sizeof(azapNbZclRequestsFailed));
  }

  // multi-re-entrance is not supported yet
  *port = g_portContextIndex++;

  if (NULL == pCallbacks)
  {
    RTL_TRDBG(TRACE_ERROR, "azapInit: bad parameters\n");
    return false;
  }

  azapInitImpl(*port, targetedIeee, mainTbPoll, pCallbacks, defaultChannel, deviceType,
      resetHanAtReboot);

  RTL_TRDBG(TRACE_API, ">>> API azapInit (port:%d) (targetedIeee:%s) (callbacks:0x%.8x) "
      "(defaultChannel:%d) (deviceType:%d) (resetHanAtReboot:%d)\n", *port,
      targetedIeee ? azap_extAddrToString(targetedIeee, true) : "<any>",
      pCallbacks, defaultChannel, deviceType, resetHanAtReboot);

  return true;
}

/**
 * @brief This function starts the Actility AZAP (AZAP) service layer
 * @param port the reference port associated to a hardware/HAN
 * @return 1 if service started successfully, 0 otherwise.
 */
bool azapStart(azap_uint8 port)
{
  bool ret = false;

  RTL_TRDBG(TRACE_API, ">>> API azapStart (port:%d)\n", port);

  if (NULL == g_portContexts[port])
  {
    RTL_TRDBG(TRACE_ERROR, "azapStart - error: no associated port context found (port:%d)\n",
        port);
  }
  else
  {
    portContext_t *portCntxt = g_portContexts[port];
    portCntxt->startZnp(portCntxt);
    ret = true;
  }

  return ret;
}

/**
 * @brief This function stops the Actility AZAP (AZAP) service layer
 * @param port the reference port associated to a hardware/HAN
 * @return 1 if service stopped successfully, 0 otherwise.
 */
bool azapStop(azap_uint8 port)
{
  bool ret = false;
  portContext_t *portCntxt = g_portContexts[port];

  RTL_TRDBG(TRACE_API, ">>> API azapStop (port:%d)\n", port);

  if (portCntxt)
  {
    ret = portCntxt->disconnect(portCntxt);
  }

  RTL_TRDBG(TRACE_API, "<<< API azapStop done (port:%d)\n", port);

  azapUnInitImpl(port);

  return ret;
}

/**
 * @brief Get the Z-Stack version running on the TI CC2530 ship.
 * @param port the reference port associated to a hardware/HAN
 * @return the Z-Stack version string.
 */
char *zStackVersion(azap_uint8 port)
{
  char *ret = "<unknown>";
  portContext_t *portCntxt = g_portContexts[port];

  if (portCntxt)
  {
    ret = portCntxt->zstackVersion;
  }

  return ret;
}

/**
 * @brief Dumps statistics regarding AZAP layer.
 * @param out the placeholder where the statistics are dumped.
 */
void azapDumpStatistics(char *out)
{
  int i;
  static char szTemp[1024];
  if (!out)
  {
    return;
  }

  for (i = 0; i < MAX_NB_PORT_CONTEXT; i++)
  {
    portContext_t *portCntxt = g_portContexts[i];
    if (portCntxt)
    {
      snprintf(szTemp, sizeof(szTemp), "Statistics for port #%d\n"
          "- ZDO MESSAGES [nbNotif:%ld][issuedReq:%ld][completedReq:%ld][failedReq:%ld]\n"
          "- ZCL MESSAGES [nbNotif:%ld][issuedReq:%ld][completedReq:%ld][failedReq:%ld]\n",
          i, azapNbZdoNotifications[i], azapNbZdoRequestsIssued[i],
          azapNbZdoRequestsCompleted[i], azapNbZdoRequestsFailed[i],
          azapNbZclNotifications[i], azapNbZclRequestsIssued[i],
          azapNbZclRequestsCompleted[i], azapNbZclRequestsFailed[i]);
      strcat(out, szTemp);
      portCntxt->dumpStatistics(portCntxt, out);
    }
  }
}

/**
 * Add an new end point to device
 * @param port the reference port associated to a hardware/HAN
 * @param endPointId end point identifier
 * @param appProfId application profile identifier (0x0104: HA, 0X0109: SE)
 * @param appDeviceId application device identifier
 * @param appDevVer application device version
 * @param appNumInClusters number of server clusters
 * @param appInClusterList list of server cluster identifiers
 * @param appNumOutClusters number of client clusters
 * @param appOutClusterList list of client cluster identifiers
 * @return 1 when added the end point successfully, 0 otherwise
 */
bool azapAddEndPoint(
  azap_uint8 port,
  azap_uint8 endPointId,
  azap_uint16 appProfId,
  azap_uint16 appDeviceId,
  azap_uint8 appDevVer,
  azap_uint8 appNumInClusters,
  azap_uint16 appInClusterList[],
  azap_uint8 appNumOutClusters,
  azap_uint16 appOutClusterList[])
{
  /** comment */
  bool res = false;
  portContext_t *portCntxt = g_portContexts[port];

  RTL_TRDBG(TRACE_API, ">>> API azapAddEndPoint (port:%d) (endPoint:0x%.2x) (appProfId:0x%.4x) "
      "(appDeviceId:0x%.4x) (appDevVer:0x%.2x) (appNumInClusters:0x%.2x) (appInClusterList:%s) "
      "(appNumOutClusters:0x%.2x) (appOutClusterList:%s)\n", port, endPointId, appProfId,
      appDeviceId, appDevVer, appNumInClusters,
      azap_uint16ArrayToString(appInClusterList, appNumInClusters), appNumOutClusters,
      azap_uint16ArrayToString(appOutClusterList, appNumOutClusters));

  if (portCntxt)
  {
    azapEndPoint_t *ep = new_azapEndPoint_t(endPointId, appProfId, appDeviceId, appDevVer,
        appNumInClusters, appInClusterList, appNumOutClusters, appOutClusterList);
    res = portCntxt->addEndPoint(portCntxt, ep);
  }

  return res;
}

/**
 * Add an new ZCL attribute to an end point
 * @todo create structure for ZCL attributes and methods for manipulating them.
 * @param port the reference port associated to a hardware/HAN
 * @param endPoint end point identifier
 * @param clusterId cluster identifier associated to this attribute
 * @param attributeId attribute identifier within this clusterId
 * @param dataType attribute data type
 * @param accessControl is this attribute read only or read/write
 * @param data a pointer to the attribute instanciation
 * @param dataLen the length of the msg payload
 * @return 1 when added the end point successfully, 0 otherwise
 */
bool azapAddZCLAttribute(
  azap_uint8 port,
  azap_uint8 endPoint,
  azap_uint16 clusterId,
  azap_uint16 attributeId,
  azap_uint8 dataType,
  azap_uint8 accessControl,
  azap_byte *data,
  azap_uint16 dataLen)
{
  bool res = false;

  RTL_TRDBG( TRACE_API, ">>> API azapAddZCLAttribute (port:%d) (endPoint:0x%.2x) "
      "(clusterId:0x%.4x) (attributeId:0x%.4x) (dataType:0x%.2x) (accessControl:0x%.2x) "
      "(data:0x%.8x) (dataLen:%d)\n", port, endPoint, clusterId, attributeId, dataType,
      accessControl, data, dataLen);


  return res;
}

/**
 * Change the value of a ZCL attribute associated to an end point
 * @todo create structure for ZCL attributes and methods for manipulating them.
 * @param port the reference port associated to a hardware/HAN
 * @param endPoint end point identifier
 * @param clusterId cluster identifier associated to this attribute
 * @param attributeId attribute identifier within this clusterId
 * @param dataType attribute data type
 * @param data a pointer to the attribute value
 * @param dataLen number of bytes used to encode the value
 * @return 1 when added the end point successfully, 0 otherwise
 */
bool azapChangeZCLAttributeValue(
  azap_uint8 port,
  azap_uint8 endPoint,
  azap_uint16 clusterId,
  azap_uint16 attributeId,
  azap_uint8 dataType,
  azap_byte *data,
  azap_uint16 dataLen)
{
  bool res = false;

  RTL_TRDBG( TRACE_API, ">>> API azapChangeZCLAttributeValue (port:%d) (endPoint:0x%.2x) "
      "(clusterId:0x%.4x) (attributeId:0x%.4x) (dataType:0x%.2x) (data:0x%.8x) "
      "(dataLen:%d)\n", port, endPoint, clusterId, attributeId, dataType, data, dataLen);

  return res;
}

/***************************************************************************************
 *       _________   ___    _                           _   _                          *
 *      |__  /  _ \ / _ \  (_)_ ____   _____   ___ __ _| |_(_) ___  _ __  ___          *
 *        / /| | | | | | | | | '_ \ \ / / _ \ / __/ _` | __| |/ _ \| '_ \/ __|         *
 *       / /_| |_| | |_| | | | | | \ V / (_) | (_| (_| | |_| | (_) | | | \__ \         *
 *      /____|____/ \___/  |_|_| |_|\_/ \___/ \___\__,_|\__|_|\___/|_| |_|___/         *
 *                                                                                     *
 ***************************************************************************************/

/**
 * Stimulate AZAP layer for sending a NWK_add_req.
 * @param port the reference port associated to a hardware/HAN
 * @param ttlMode the TTL request management model (@see AZAP_REQ_TTL_MGMT_MODE).
 * @param issuerContext the request issuer context.
 * @param ieeeAddr The IEEE address to be matched by the Remote Device
 * @param requestType Request type for this command:
 *  0x00 : Single device response
 *  0x01 : Extended response
 *  0x02-0xff : reserved
 * @param startIndex If the Request type for this command is Extended response, the
 * startIndex provides the starting index for the requested elements of the associated
 * devices list.
 * @return 1 if message sent successfully, 0 otherwise.
 */
bool azapZdpNwkAddrReq(
  azap_uint8 port,
  AZAP_REQ_TTL_MGMT_MODE ttlMode,
  void *issuerContext,
  azap_uint8 ieeeAddr[IEEE_ADDR_LEN],
  azap_uint8 requestType,
  azap_uint8 startIndex)
{
  bool retVal = false;

  RTL_TRDBG(TRACE_API, ">>> API azapZdpNwkAddrReq (port:%d) (ieeeAddr:%s) "
      "(requestType:0x%.2x) (startIndex:%d)\n", port, azap_extAddrToString(ieeeAddr,
          true), requestType, startIndex);

  portContext_t *portCntxt = g_portContexts[port];
  if (portCntxt)
  {
    azapNbZdoRequestsIssued[port] ++;

    azap_byte payload[0x0A];
    azap_byte *ptr = &payload[0];
    azap_extAddrCpyPayloadAndShift(ieeeAddr, &ptr);
    azap_uint8CpyPayloadAndShift(requestType, &ptr);
    azap_uint8CpyPayloadAndShift(startIndex, &ptr);

    // no nwkAddrDest here... so send it to the coordinator ?
    retVal = portCntxt->sendZdo(portCntxt, ttlMode, issuerContext,
        AZAP_ZDO_NWK_ADDR_REQ, true, 0, 0, sizeof(payload), payload);
  }

  return retVal;
}

/**
 * Stimulate AZAP layer for sending a IEEE_add_req.
 * @param port the reference port associated to a hardware/HAN
 * @param ttlMode the TTL request management model (@see AZAP_REQ_TTL_MGMT_MODE).
 * @param issuerContext the request issuer context.
 * @param nwkAddrDest Network address of the targeted device (retrieved from NWK layer).
 * @param nwkAddrOfInterest NWK address that is used for IEEE address mapping.
 * @param requestType Request type for this command:
 *  0x00 : Single device response
 *  0x01 : Extended response
 *  0x02-0xff : reserved
 * @param startIndex If the Request type for this command is Extended response, the
 * startIndex provides the starting index for the requested elements of the associated
 * devices list.
 * @return 1 if message sent successfully, 0 otherwise.
 */
bool azapZdpIeeeAddrReq(
  azap_uint8 port,
  AZAP_REQ_TTL_MGMT_MODE ttlMode,
  void *issuerContext,
  azap_uint16 nwkAddrDest,
  azap_uint16 nwkAddrOfInterest,
  azap_uint8 requestType,
  azap_uint8 startIndex)
{
  bool retVal = false;

  RTL_TRDBG(TRACE_API, ">>> API azapZdpIeeeAddrReq (port:%d) (nwkAddrDest:0x%.4x) "
      "(nwkAddrOfInterest:0x%.4x) (requestType:0x%.2x)\n", port, nwkAddrDest, nwkAddrOfInterest,
      requestType);

  portContext_t *portCntxt = g_portContexts[port];
  if (portCntxt)
  {
    azapNbZdoRequestsIssued[port] ++;

    azap_byte payload[0x04];
    azap_byte *ptr = &payload[0];
    azap_uint16CpyPayloadAndShift(nwkAddrOfInterest, &ptr);
    azap_uint8CpyPayloadAndShift(requestType, &ptr);
    azap_uint8CpyPayloadAndShift(startIndex, &ptr);

    retVal = portCntxt->sendZdo(portCntxt, ttlMode, issuerContext,
        AZAP_ZDO_IEEE_ADDR_REQ, true, nwkAddrDest, 0, sizeof(payload), payload);
  }

  return retVal;
}

/**
 * Stimulate AZAP layer for sending a Node_Desc_req.
 * @param port the reference port associated to a hardware/HAN
 * @param ttlMode the TTL request management model (@see AZAP_REQ_TTL_MGMT_MODE).
 * @param issuerContext the request issuer context.
 * @param nwkAddrDest Network address of the targeted device (retrieved from NWK layer).
 * @param nwkAddrOfInterest NWK address for the request.
 * @return 1 if message sent successfully, 0 otherwise.
 */
bool azapZdpNodeDescReq(
  azap_uint8 port,
  AZAP_REQ_TTL_MGMT_MODE ttlMode,
  void *issuerContext,
  azap_uint16 nwkAddrDest,
  azap_uint16 nwkAddrOfInterest)
{
  bool retVal = false;

  RTL_TRDBG( TRACE_API, ">>> API azapZdpNodeDescReq (port:%d) (nwkAddrDest:0x%.4x) "
      "(nwkAddrOfInterest:0x%.4x)\n", port, nwkAddrDest, nwkAddrOfInterest);

  portContext_t *portCntxt = g_portContexts[port];
  if (portCntxt)
  {
    azapNbZdoRequestsIssued[port] ++;

    azap_byte payload[0x04];
    azap_byte *ptr = &payload[0];
    azap_uint16CpyPayloadAndShift(nwkAddrDest, &ptr);
    azap_uint16CpyPayloadAndShift(nwkAddrOfInterest, &ptr);

    retVal = portCntxt->sendZdo(portCntxt, ttlMode, issuerContext,
        AZAP_ZDO_NODE_DESC_REQ, true, nwkAddrDest, 0, sizeof(payload), payload);
  }

  return retVal;
}

/**
 * Stimulate AZAP layer for sending a Power_Desc_req.
 * @param port the reference port associated to a hardware/HAN
 * @param ttlMode the TTL request management model (@see AZAP_REQ_TTL_MGMT_MODE).
 * @param issuerContext the request issuer context.
 * @param nwkAddrDest Network address of the targeted device (retrieved from NWK layer).
 * @param nwkAddrOfInterest NWK address for the request.
 * @return 1 if message sent successfully, 0 otherwise.
 */
bool azapZdpPowerDescReq(
  azap_uint8 port,
  AZAP_REQ_TTL_MGMT_MODE ttlMode,
  void *issuerContext,
  azap_uint16 nwkAddrDest,
  azap_uint16 nwkAddrOfInterest)
{
  bool retVal = false;

  RTL_TRDBG(TRACE_API, ">>> API azapZdpPowerDescReq (port:%d) (nwkAddrDest:0x%.4x) "
      "(nwkAddrOfInterest:0x%.4x)\n", port, nwkAddrDest, nwkAddrOfInterest);

  portContext_t *portCntxt = g_portContexts[port];
  if (portCntxt)
  {
    azapNbZdoRequestsIssued[port] ++;

    azap_byte payload[0x04];
    azap_byte *ptr = &payload[0];
    azap_uint16CpyPayloadAndShift(nwkAddrDest, &ptr);
    azap_uint16CpyPayloadAndShift(nwkAddrOfInterest, &ptr);

    retVal = portCntxt->sendZdo(portCntxt, ttlMode, issuerContext,
        AZAP_ZDO_POWER_DESC_REQ, true, nwkAddrDest, 0, sizeof(payload), payload);
  }

  return retVal;
}

/**
 * Stimulate AZAP layer for sending a Active_EP_req.
 * @param port the reference port associated to a hardware/HAN
 * @param ttlMode the TTL request management model (@see AZAP_REQ_TTL_MGMT_MODE).
 * @param issuerContext the request issuer context.
 * @param nwkAddrDest Network address of the targeted device (retrieved from NWK layer).
 * @param nwkAddrOfInterest NWK address for the request.
 * @return 1 if message sent successfully, 0 otherwise.
 */
bool azapZdpActiveEPReq(
  azap_uint8 port,
  AZAP_REQ_TTL_MGMT_MODE ttlMode,
  void *issuerContext,
  azap_uint16 nwkAddrDest,
  azap_uint16 nwkAddrOfInterest)
{
  bool retVal = false;

  RTL_TRDBG(TRACE_API, ">>> API azapZdpActiveEPReq (port:%d) (nwkAddrDest:0x%.4x) "
      "(nwkAddrOfInterest:0x%.4x)\n", port, nwkAddrDest, nwkAddrOfInterest);

  portContext_t *portCntxt = g_portContexts[port];
  if (portCntxt)
  {
    azapNbZdoRequestsIssued[port] ++;

    azap_byte payload[0x04];
    azap_byte *ptr = &payload[0];
    azap_uint16CpyPayloadAndShift(nwkAddrDest, &ptr);
    azap_uint16CpyPayloadAndShift(nwkAddrOfInterest, &ptr);

    retVal = portCntxt->sendZdo(portCntxt, ttlMode, issuerContext,
        AZAP_ZDO_ACTIVE_EP_REQ, true, nwkAddrDest, 0, sizeof(payload), payload);
  }

  return retVal;
}

/**
 * Stimulate AZAP layer for sending a Simple_Desc_req.
 * @param port the reference port associated to a hardware/HAN
 * @param ttlMode the TTL request management model (@see AZAP_REQ_TTL_MGMT_MODE).
 * @param issuerContext the request issuer context.
 * @param nwkAddrDest Network address of the targeted device (retrieved from NWK layer).
 * @param nwkAddrOfInterest NWK address for the request.
 * @param endpoint The endpoint on the destination
 * @return 1 if message sent successfully, 0 otherwise.
 */
bool azapZdpSimpleDescReq(
  azap_uint8 port,
  AZAP_REQ_TTL_MGMT_MODE ttlMode,
  void *issuerContext,
  azap_uint16 nwkAddrDest,
  azap_uint16 nwkAddrOfInterest,
  azap_uint8 endpoint)
{
  bool retVal = false;

  RTL_TRDBG(TRACE_API, ">>> API azapZdpSimpleDescReq (port:%d) (nwkAddrDest:0x%.4x) "
      "(nwkAddrOfInterest:0x%.4x) (endpoint:0x%.2x)\n", port, nwkAddrDest, nwkAddrOfInterest,
      endpoint);

  portContext_t *portCntxt = g_portContexts[port];
  if (portCntxt)
  {
    azapNbZdoRequestsIssued[port] ++;

    azap_byte payload[0x05];
    azap_byte *ptr = &payload[0];
    azap_uint16CpyPayloadAndShift(nwkAddrDest, &ptr);
    azap_uint16CpyPayloadAndShift(nwkAddrOfInterest, &ptr);
    azap_uint8CpyPayloadAndShift(endpoint, &ptr);

    retVal = portCntxt->sendZdo(portCntxt, ttlMode, issuerContext,
        AZAP_ZDO_SIMPLE_DESC_REQ, true, nwkAddrDest, endpoint, sizeof(payload), payload);
  }

  return retVal;
}

/**
 * Stimulate AZAP layer for sending a Bind_req.
 * @param port the reference port associated to a hardware/HAN
 * @param ttlMode the TTL request management model (@see AZAP_REQ_TTL_MGMT_MODE).
 * @param issuerContext the request issuer context.
 * @param nwkAddrDest Network address of the targeted device (retrieved from NWK layer).
 * @param srcAddress The IEEE address of the source binding node.
 * @param srcEndp The source binding end point identifier.
 * @param clusterId The identifier of the cluster on the source device that is bound to
 * the destination.
 * @param dstAddress The IEEE address of the destination binding node.
 * @param dstEndp The destination binding end point identifier.
 * @return 1 if message sent successfully, 0 otherwise.
 */
bool azapZdpBindReq(
  azap_uint8 port,
  AZAP_REQ_TTL_MGMT_MODE ttlMode,
  void *issuerContext,
  azap_uint16 nwkAddrDest,
  azap_uint8 srcAddress[IEEE_ADDR_LEN],
  azap_uint8 srcEndp,
  azap_uint16 clusterId,
  azap_uint8 dstAddress[IEEE_ADDR_LEN],
  azap_uint8 dstEndp)
{
  bool retVal = false;

  RTL_TRDBG(TRACE_API, ">>> API azapZdpBindReq (port:%d) (nwkAddrDest:0x%.4x) "
      "(srcAddress:%s) (srcEndp:0x%.2x) (clusterId:0x%.4x) (dstAddress:%s) "
      "(dstEndp:0x%.2x)\n", port, nwkAddrDest, azap_extAddrToString(srcAddress,
          true), srcEndp, clusterId, azap_extAddrToString(dstAddress,
              true), dstEndp);

  portContext_t *portCntxt = g_portContexts[port];
  if (portCntxt)
  {
    azapNbZdoRequestsIssued[port] ++;

    azap_byte payload[0x17];
    azap_byte *ptr = &payload[0];
    azap_uint16CpyPayloadAndShift(nwkAddrDest, &ptr);
    azap_extAddrCpyPayloadAndShift(srcAddress, &ptr);
    azap_uint8CpyPayloadAndShift(srcEndp, &ptr);
    azap_uint16CpyPayloadAndShift(clusterId, &ptr);
    azap_uint8CpyPayloadAndShift(ZDO_RTG_ADDR_MODE_64BIT, &ptr);
    azap_extAddrCpyPayloadAndShift(dstAddress, &ptr);
    azap_uint8CpyPayloadAndShift(dstEndp, &ptr);

//    retVal = portCntxt->sendZdo(portCntxt, issuerContext, AZAP_ZDO_BIND_REQ, true,
//      nwkAddrDest, 0, sizeof(payload), payload);
    retVal = portCntxt->sendZdo(portCntxt, ttlMode, issuerContext,
        AZAP_ZDO_BIND_REQ, true, nwkAddrDest, srcEndp, sizeof(payload), payload);
  }

  return retVal;
}

/**
 * Stimulate AZAP layer for sending a Unbind_req.
 * @param port the reference port associated to a hardware/HAN
 * @param ttlMode the TTL request management model (@see AZAP_REQ_TTL_MGMT_MODE).
 * @param issuerContext the request issuer context.
 * @param nwkAddrDest Network address of the targeted device (retrieved from NWK layer).
 * @param srcAddress The IEEE address of the source binding node.
 * @param srcEndp The source binding end point identifier.
 * @param clusterId The identifier of the cluster on the source device that is bound to
 * the destination.
 * @param dstAddress The IEEE address of the destination binding node.
 * @param dstEndp The destination binding end point identifier.
 * @return 1 if message sent successfully, 0 otherwise.
 */
bool azapZdpUnbindReq(
  azap_uint8 port,
  AZAP_REQ_TTL_MGMT_MODE ttlMode,
  void *issuerContext,
  azap_uint16 nwkAddrDest,
  azap_uint8 srcAddress[IEEE_ADDR_LEN],
  azap_uint8 srcEndp,
  azap_uint16 clusterId,
  azap_uint8 dstAddress[IEEE_ADDR_LEN],
  azap_uint8 dstEndp)
{
  bool retVal = false;

  RTL_TRDBG(TRACE_API, ">>> API azapZdpUnbindReq (port:%d) (nwkAddrDest:0x%.4x) "
      "(srcAddress:%s) (srcEndp:0x%.2x) (clusterId:0x%.4x) (dstAddress:%s) "
      "(dstEndp:0x%.2x)\n", port, nwkAddrDest, azap_extAddrToString(srcAddress,
          true), srcEndp, clusterId, azap_extAddrToString(dstAddress, true), dstEndp);

  portContext_t *portCntxt = g_portContexts[port];
  if (portCntxt)
  {
    azapNbZdoRequestsIssued[port] ++;

    azap_byte payload[0x17];
    azap_byte *ptr = &payload[0];
    azap_uint16CpyPayloadAndShift(nwkAddrDest, &ptr);
    azap_extAddrCpyPayloadAndShift(srcAddress, &ptr);
    azap_uint8CpyPayloadAndShift(srcEndp, &ptr);
    azap_uint16CpyPayloadAndShift(clusterId, &ptr);
    azap_uint8CpyPayloadAndShift(ZDO_RTG_ADDR_MODE_64BIT, &ptr);
    azap_extAddrCpyPayloadAndShift(dstAddress, &ptr);
    azap_uint8CpyPayloadAndShift(dstEndp, &ptr);

//    retVal = portCntxt->sendZdo(portCntxt, issuerContext, AZAP_ZDO_UNBIND_REQ, true,
//      nwkAddrDest, 0, sizeof(payload), payload);
    retVal = portCntxt->sendZdo(portCntxt, ttlMode, issuerContext,
        AZAP_ZDO_UNBIND_REQ, true, nwkAddrDest, srcEndp, sizeof(payload), payload);
  }

  return retVal;
}

/**
 * Stimulate AZAP layer for sending a Mgmt_Bind_req.
 * @param port the reference port associated to a hardware/HAN
 * @param ttlMode the TTL request management model (@see AZAP_REQ_TTL_MGMT_MODE).
 * @param issuerContext the request issuer context.
 * @param nwkAddrDest Network address of the targeted device (retrieved from NWK layer).
 * @param startIndex Specifies where to start in the response array list. The result may
 * contain more entries than can be reported, so this field allows the user to retrieve
 * the responses anywhere in the array list.
 * @return 1 if message sent successfully, 0 otherwise.
 */
bool azapZdpMgmtRtgReq(
  azap_uint8 port,
  AZAP_REQ_TTL_MGMT_MODE ttlMode,
  void *issuerContext,
  azap_uint16 nwkAddrDest,
  azap_uint8 startIndex)
{
  bool retVal = false;

  RTL_TRDBG(TRACE_API, ">>> API azapZdpMgmtRtgReq (port:%d) (nwkAddrDest:0x%.4x) "
      "(startIndex:0x%.2x)\n", port, nwkAddrDest, startIndex);

  portContext_t *portCntxt = g_portContexts[port];
  if (portCntxt)
  {
    azapNbZdoRequestsIssued[port] ++;

    azap_byte payload[0x03];
    azap_byte *ptr = &payload[0];
    azap_uint16CpyPayloadAndShift(nwkAddrDest, &ptr);
    azap_uint8CpyPayloadAndShift(startIndex, &ptr);

    retVal = portCntxt->sendZdo(portCntxt, ttlMode, issuerContext,
        AZAP_ZDO_MGMT_RTG_REQ, true, nwkAddrDest, 0, sizeof(payload), payload);
  }

  return retVal;
}

/**
 * Stimulate AZAP layer for sending a Mgmt_Bind_req.
 * @param port the reference port associated to a hardware/HAN
 * @param ttlMode the TTL request management model (@see AZAP_REQ_TTL_MGMT_MODE).
 * @param issuerContext the request issuer context.
 * @param nwkAddrDest Network address of the targeted device (retrieved from NWK layer).
 * @param startIndex Specifies where to start in the response array list. The result may
 * contain more entries than can be reported, so this field allows the user to retrieve
 * the responses anywhere in the array list.
 * @return 1 if message sent successfully, 0 otherwise.
 */
bool azapZdpMgmtBindReq(
  azap_uint8 port,
  AZAP_REQ_TTL_MGMT_MODE ttlMode,
  void *issuerContext,
  azap_uint16 nwkAddrDest,
  azap_uint8 startIndex)
{
  bool retVal = false;

  RTL_TRDBG(TRACE_API, ">>> API azapZdpMgmtBindReq (port:%d) (nwkAddrDest:0x%.4x) "
      "(startIndex:0x%.2x)\n", port, nwkAddrDest, startIndex);

  portContext_t *portCntxt = g_portContexts[port];
  if (portCntxt)
  {
    azapNbZdoRequestsIssued[port] ++;

    azap_byte payload[0x03];
    azap_byte *ptr = &payload[0];
    azap_uint16CpyPayloadAndShift(nwkAddrDest, &ptr);
    azap_uint8CpyPayloadAndShift(startIndex, &ptr);

    retVal = portCntxt->sendZdo(portCntxt, ttlMode, issuerContext,
        AZAP_ZDO_MGMT_BIND_REQ, true, nwkAddrDest, 0, sizeof(payload), payload);
  }

  return retVal;
}

/**
 * Stimulate AZAP layer for sending a Mgmt_Permit_Join_req.
 * @param port the reference port associated to a hardware/HAN
 * @param ttlMode the TTL request management model (@see AZAP_REQ_TTL_MGMT_MODE).
 * @param issuerContext the request issuer context.
 * @param nwkAddrDest Network address of the targeted device (retrieved from NWK layer).
 * @param permitDuration Specifies the duration to permit joining. 0 = join disabled.
 * 0xff = join enabled. 0x01-0xfe = number of seconds to permit joining.
 * @param tcSignificance Trust Center Significance.
 * @param responseExpected specifies if we should look for a response to this request. For
 * instance, we may not expect any response when broadcasting this request
 * @return 1 if message sent successfully, 0 otherwise.
 */
bool azapZdpMgmtPermitJoinReq(
  azap_uint8 port,
  AZAP_REQ_TTL_MGMT_MODE ttlMode,
  void *issuerContext,
  azap_uint16 nwkAddrDest,
  azap_uint8 permitDuration,
  bool tcSignificance,
  bool responseExpected)
{
  bool retVal = false;

  RTL_TRDBG(TRACE_API, ">>> API azapZdpMgmtPermitJoinReq (port:%d) (nwkAddrDest:0x%.4x) "
      "(permitDuration:0x%.2x) (tcSignificance:0x%.2x)\n", port, nwkAddrDest,
      permitDuration, (azap_uint8)tcSignificance);

  portContext_t *portCntxt = g_portContexts[port];
  if (portCntxt)
  {
    azapNbZdoRequestsIssued[port] ++;

    azap_byte payload[0x04];
    azap_byte *ptr = &payload[0];
    azap_uint16CpyPayloadAndShift(nwkAddrDest, &ptr);
    azap_uint8CpyPayloadAndShift(permitDuration, &ptr);
    azap_uint8CpyPayloadAndShift((azap_uint8)tcSignificance, &ptr);

    retVal = portCntxt->sendZdo(portCntxt, ttlMode, issuerContext,
        AZAP_ZDO_MGMT_PERMIT_JOIN_REQ, responseExpected, nwkAddrDest, 0,
        sizeof(payload), payload);
  }

  return retVal;
}


/**
 * Stimulate AZAP layer for sending a Mgmt_Bind_req.
 * @param port the reference port associated to a hardware/HAN
 * @param ttlMode the TTL request management model (@see AZAP_REQ_TTL_MGMT_MODE).
 * @param issuerContext the request issuer context.
 * @param nwkAddrDest Network address of the targeted device (retrieved from NWK layer).
 * @param deviceAddress the extended address of the device that should be asked to leave.
 * @param removeChildren Set to 1 to remove the children of the device as well. 0 otherwise.
 * @param rejoin Set to 1 if the removed device should rejoin afterwards. 0 otherwise.
 * @return 1 if message sent successfully, 0 otherwise.
 */
bool azapZdpMgmtLeaveReq(
  azap_uint8 port,
  AZAP_REQ_TTL_MGMT_MODE ttlMode,
  void *issuerContext,
  azap_uint16 nwkAddrDest,
  azap_uint8 deviceAddress[IEEE_ADDR_LEN],
  bool removeChildren,
  bool rejoin)
{
  bool retVal = false;
  azap_uint8 options = 0;
  RTL_TRDBG(TRACE_API, ">>> API azapZdpMgmtLeaveReq (port:%d) (nwkAddrDest:0x%.4x) "
      "(deviceAddress:%s) (removeChildren:%d) (rejoin:%d)\n", port, nwkAddrDest,
      azap_extAddrToString(deviceAddress, true), removeChildren, rejoin);

  portContext_t *portCntxt = g_portContexts[port];
  if (portCntxt)
  {
    azapNbZdoRequestsIssued[port] ++;

    azap_byte payload[0x0B];
    azap_byte *ptr = &payload[0];
    azap_uint16CpyPayloadAndShift(nwkAddrDest, &ptr);
    azap_extAddrCpyPayloadAndShift(deviceAddress, &ptr);
//    azap_uint8CpyPayloadAndShift( (removeChildren && rejoin) , &ptr);
    if (removeChildren)
    {
      options |= 0x40;
    }
    if (rejoin)
    {
      options |= 0x80;
    }
    azap_uint8CpyPayloadAndShift(options , &ptr);

    retVal = portCntxt->sendZdo(portCntxt, ttlMode, issuerContext,
        AZAP_ZDO_MGMT_LEAVE_REQ, true, nwkAddrDest, 0, sizeof(payload), payload);
  }

  return retVal;
}


/****************************************************************************************
 *     _________ _       _                           _   _                              *
 *    |__  / ___| |     (_)_ ____   _____   ___ __ _| |_(_) ___  _ __  ___              *
 *      / / |   | |     | | '_ \ \ / / _ \ / __/ _` | __| |/ _ \| '_ \/ __|             *
 *     / /| |___| |___  | | | | \ V / (_) | (_| (_| | |_| | (_) | | | \__ \             *
 *    /____\____|_____| |_|_| |_|\_/ \___/ \___\__,_|\__|_|\___/|_| |_|___/             *
 *                                                                                      *
 ****************************************************************************************/

/**
 * Stimulate AZAP layer for sending a ZCL "Read" command on a set of attributes.
 * @param port the reference port associated to a hardware/HAN
 * @param ttlMode the TTL request management model (@see AZAP_REQ_TTL_MGMT_MODE).
 * @param issuerContext the request issuer context.
 * @param nwkAddrDest Network address of the targeted device (retrieved from NWK layer).
 * @param localEndp This field shall be the source endpoint that emits this request
 * @param remoteEndp This field shall be the destination endpoint for the binding entry.
 * @param clusterId The identifier of the targeted cluster.
 * @param clusterDir the cluster direction (0 = client->server; 1 = server->client)
 * @param manufacturerCode manufacturer specific code associated to each attribute to write.
 * @param zclCmd The data to encode in the payload for the ZCL read command.
 * @return 1 if message sent successfully, 0 otherwise.
 */
bool azapZclReadCmdReq(
  azap_uint8 port,
  AZAP_REQ_TTL_MGMT_MODE ttlMode,
  void *issuerContext,
  azap_uint16 nwkAddrDest,
  azap_uint8 localEndp,
  azap_uint8 remoteEndp,
  azap_uint16 clusterId,
  azap_uint8 clusterDir,
  azap_uint16 manufacturerCode,
  azapZclReadCommand_t *zclCmd)
{
  bool retVal = false;
  azap_uint8 payload[MAX_ZNP_PAYLOAD_SIZE];
  azap_uint8 i;

  RTL_TRDBG(TRACE_API, ">>> API azapZclReadCmdReq (port:%d) (nwkAddrDest:0x%.4x) "
      "(localEndp:0x%.2x) (remoteEndp:0x%.2x) (clusterId:0x%.4x) (clusterDir:0x%.2x) "
      "(manufacturerCode:0x%.4x) (numAttr:%d) (attrId:%s)\n", port, nwkAddrDest, localEndp,
      remoteEndp, clusterId, clusterDir, manufacturerCode, zclCmd ? zclCmd->numAttr : 0,
      azap_uint16ArrayToString(zclCmd ? zclCmd->attrId : NULL, zclCmd ? zclCmd->numAttr : 0));

  portContext_t *portCntxt = g_portContexts[port];
  if ((portCntxt) && (zclCmd) &&
      ((zclCmd->numAttr * sizeof(azap_uint16)) < MAX_ZNP_PAYLOAD_SIZE))
  {
    azapNbZclRequestsIssued[port] ++;

    for (i = 0; i < zclCmd->numAttr; i++)
    {
      payload[ i * 2 ] = LO_UINT16(zclCmd->attrId[i]);
      payload[ (i * 2) + 1 ] = HI_UINT16(zclCmd->attrId[i]);
    }

    retVal = portCntxt->sendZcl(portCntxt, ttlMode, issuerContext, AZAP_ZCL_READ_CMD,
        true, nwkAddrDest, remoteEndp, localEndp, clusterId, clusterDir, manufacturerCode,
        true, ZCL_CMD_READ, ZCL_CMD_READ_RSP, zclCmd->numAttr * 2, payload);
  }

  return retVal;
}

/**
 * Stimulate AZAP layer for sending a ZCL "Write" command on a set of attributes.
 * @param port the reference port associated to a hardware/HAN
 * @param ttlMode the TTL request management model (@see AZAP_REQ_TTL_MGMT_MODE).
 * @param issuerContext the request issuer context.
 * @param nwkAddrDest Network address of the targeted device (retrieved from NWK layer).
 * @param localEndp This field shall be the source endpoint that emits this request
 * @param remoteEndp This field shall be the destination endpoint for the binding entry.
 * @param clusterId The identifier of the targeted cluster.
 * @param clusterDir the cluster direction (0 = client->server; 1 = server->client)
 * @param manufacturerCode manufacturer specific code associated to each attribute to write.
 * @param undivided specifies if request command should be "Write Attributes" (if false) or
 *  "Write Attributes Undivided" (if true)
 * @param zclCmd The data to encode in the payload for the ZCL write command.
 * @return 1 if message sent successfully, 0 otherwise.
 */
bool azapZclWriteCmdReq(
  azap_uint8 port,
  AZAP_REQ_TTL_MGMT_MODE ttlMode,
  void *issuerContext,
  azap_uint16 nwkAddrDest,
  azap_uint8 localEndp,
  azap_uint8 remoteEndp,
  azap_uint16 clusterId,
  azap_uint8 clusterDir,
  azap_uint16 manufacturerCode,
  bool undivided,
  azapZclWriteCommand_t *zclCmd)
{
  bool retVal = false;
  azap_uint8 payload[MAX_ZNP_PAYLOAD_SIZE];
  azap_uint8 payloadLen = 0;
  azap_uint8 attrLen = 0;
  azap_uint8 i;

  RTL_TRDBG(TRACE_API, ">>> API azapZclWriteCmdReq (port:%d) (nwkAddrDest:0x%.4x) "
      "(localEndp:0x%.2x) (remoteEndp:0x%.2x) (clusterId:0x%.4x) (clientDir:0x%.2x) "
      "(manufacturerCode:0x%.4x) (numAttr:%d)\n", port, nwkAddrDest,
      localEndp, remoteEndp, clusterId, clusterDir, manufacturerCode,
      zclCmd ? zclCmd->numAttr : 0);

  portContext_t *portCntxt = g_portContexts[port];
  if ((portCntxt) && (zclCmd))
  {
    azapNbZclRequestsIssued[port] ++;

    retVal = true;
    // in order to prevent the program against buffer overflow, check that there are at
    // least 3 more free bytes in payload (for the next attrId and dataType)
    for (i = 0; i < zclCmd->numAttr && (payloadLen < MAX_ZNP_PAYLOAD_SIZE - 3) && retVal; i++)
    {
      payload[ payloadLen++ ] = LO_UINT16(zclCmd->attrList[i].attrId);
      payload[ payloadLen++ ] = HI_UINT16(zclCmd->attrList[i].attrId);
      payload[ payloadLen++ ] = zclCmd->attrList[i].dataType;
      attrLen = azapGetZclDataLength(zclCmd->attrList[i].dataType, zclCmd->attrList[i].attrData);
      if (attrLen + payloadLen >= MAX_ZNP_PAYLOAD_SIZE)
      {
        retVal = false;
      }
      else
      {
        memcpy(&(payload[payloadLen]), zclCmd->attrList[i].attrData, attrLen);
        payloadLen += attrLen;
      }
    }
    if (retVal)
    {
      retVal = portCntxt->sendZcl(portCntxt, ttlMode, issuerContext, AZAP_ZCL_WRITE_CMD,
          true, nwkAddrDest, remoteEndp, localEndp, clusterId, clusterDir, manufacturerCode,
          true, undivided ? ZCL_CMD_WRITE_UNDIVIDED : ZCL_CMD_WRITE, ZCL_CMD_WRITE_RSP,
          payloadLen, payload);
    }
  }

  return retVal;
}

/**
 * Stimulate AZAP layer for sending a ZCL "Configure Reporting" command on a set of attributes.
 * @param port the reference port associated to a hardware/HAN
 * @param ttlMode the TTL request management model (@see AZAP_REQ_TTL_MGMT_MODE).
 * @param issuerContext the request issuer context.
 * @param nwkAddrDest Network address of the targeted device (retrieved from NWK layer).
 * @param localEndp This field shall be the source endpoint that sends this request.
 * @param remoteEndp This field shall be the destination endpoint for the binding entry.
 * @param clusterId The identifier of the targeted cluster.
 * @param clusterDir the cluster direction (0 = client->server; 1 = server->client)
 * @param zclCmd The data to encode in the payload for the ZCL Configure Report command.
 * @return 1 if message sent successfully, 0 otherwise.
 */
bool azapZclConfigureReportCmdReq(
  azap_uint8 port,
  AZAP_REQ_TTL_MGMT_MODE ttlMode,
  void *issuerContext,
  azap_uint16 nwkAddrDest,
  azap_uint8 localEndp,
  azap_uint8 remoteEndp,
  azap_uint16 clusterId,
  azap_uint8 clusterDir,
  azapZclCfgReportCommand_t *zclCmd)
{
  bool retVal = false;
  azap_uint8 payload[MAX_ZNP_PAYLOAD_SIZE];
  azap_uint8 payloadLen = 0;
  azap_uint8 attrLen = 0;
  azap_uint8 i;

  RTL_TRDBG(TRACE_API, ">>> API azapZclConfigureReportCmdReq (port:%d) (nwkAddrDest:0x%.4x) "
      "(localEndp:0x%.2x) (remoteEndp:0x%.2x) (clusterId:0x%.4x) (clientDir:0x%.2x) "
      "(numAttr:%d)\n", port, nwkAddrDest, localEndp, remoteEndp, clusterId,
      clusterDir, zclCmd ? zclCmd->numAttr : 0);

  portContext_t *portCntxt = g_portContexts[port];
  if ((portCntxt) && (zclCmd))
  {
    azapNbZclRequestsIssued[port] ++;

    retVal = true;
    // in order to prevent the program against buffer overflow, check that there are at
    // least 8 more free bytes in payload (for the next values of direction, attrId,
    // dataType, minReportInt and maxReportInt)
    for (i = 0; i < zclCmd->numAttr && (payloadLen < MAX_ZNP_PAYLOAD_SIZE - 8) && retVal; i++)
    {
      payload[ payloadLen++ ] = zclCmd->attrList[i].direction;
      payload[ payloadLen++ ] = LO_UINT16(zclCmd->attrList[i].attrId);
      payload[ payloadLen++ ] = HI_UINT16(zclCmd->attrList[i].attrId);

      if (ZCL_SEND_ATTR_REPORTS == zclCmd->attrList[i].direction)
      {
        payload[ payloadLen++ ] = zclCmd->attrList[i].dataType;
        payload[ payloadLen++ ] = LO_UINT16(zclCmd->attrList[i].minReportInt);
        payload[ payloadLen++ ] = HI_UINT16(zclCmd->attrList[i].minReportInt);
        payload[ payloadLen++ ] = LO_UINT16(zclCmd->attrList[i].maxReportInt);
        payload[ payloadLen++ ] = HI_UINT16(zclCmd->attrList[i].maxReportInt);

        if ((azapZclIsAnalogDataType(zclCmd->attrList[i].dataType)) &&
            (zclCmd->attrList[i].reportableChange))
        {
          attrLen = azapGetZclDataLength(zclCmd->attrList[i].dataType,
              zclCmd->attrList[i].reportableChange);
          if (attrLen + payloadLen >= MAX_ZNP_PAYLOAD_SIZE)
          {
            retVal = false;
          }
          else
          {
            memcpy(&(payload[payloadLen]), zclCmd->attrList[i].reportableChange, attrLen);
            payloadLen += attrLen;
          }
        }
      }
      else
      {
        payload[ payloadLen++ ] = LO_UINT16(zclCmd->attrList[i].timeoutPeriod);
        payload[ payloadLen++ ] = HI_UINT16(zclCmd->attrList[i].timeoutPeriod);
      }
    }
    if (retVal)
    {
      retVal = portCntxt->sendZcl(portCntxt, ttlMode, issuerContext,
          AZAP_ZCL_CONFIGURE_REPORTING_CMD, true, nwkAddrDest, remoteEndp, localEndp,
          clusterId, clusterDir, 0, true, ZCL_CMD_CONFIG_REPORT,
          ZCL_CMD_CONFIG_REPORT_RSP, payloadLen, payload);
    }
  }

  return retVal;
}

/**
 * Stimulate AZAP layer for sending a ZCL "Configure Reporting" command response.
 * @param port the reference port associated to a hardware/HAN
 * @param ttlMode the TTL request management model (@see AZAP_REQ_TTL_MGMT_MODE).
 * @param issuerContext the request issuer context.
 * @param nwkAddrDest Network address of the targeted device (retrieved from NWK layer).
 * @param localEndp end point that sends this response.
 * @param remoteEndp This field shall be the destination endpoint for the binding entry.
 * @param clusterId The identifier of the targeted cluster.
 * @param zclCmd The data to encode in the payload for the ZCL Configure Report rsp command.
 * @return 1 if message sent successfully, 0 otherwise.
 */
bool azapZclConfigureReportCmdRsp(
  azap_uint8 port,
  AZAP_REQ_TTL_MGMT_MODE ttlMode,
  void *issuerContext,
  azap_uint16 nwkAddrDest,
  azap_uint8 localEndp,
  azap_uint8 remoteEndp,
  azap_uint16 clusterId,
  azapZclCfgReportRspCommand_t *zclCmd)
{
  bool retVal = false;
//  azap_uint8 payload[MAX_ZNP_PAYLOAD_SIZE];
//  azap_uint8 i;

  RTL_TRDBG(TRACE_API, ">>> API azapZclConfigureReportCmdRsp - NOT IMPLEMENTED YET "
      "(port:%d) (nwkAddrDest:0x%.4x) "
      "(localEndp:0x%.2x) (remoteEndp:0x%.2x) (clusterId:0x%.4x) (numAttr:%d)\n",
      port, nwkAddrDest, localEndp, remoteEndp, clusterId, zclCmd ? zclCmd->numAttr : 0);

  return retVal;
}

////////////////////////////////////////////////////////////////
/**
 * Stimulate AZAP layer for sending a ZCL "Read Reporting Configuration" command on a
 * set of attributes.
 * @param port the reference port associated to a hardware/HAN
 * @param ttlMode the TTL request management model (@see AZAP_REQ_TTL_MGMT_MODE).
 * @param issuerContext the request issuer context.
 * @param nwkAddrDest Network address of the targeted device (retrieved from NWK layer).
 * @param localEndp This field shall be the source endpoint that sends this request.
 * @param remoteEndp This field shall be the destination endpoint for the binding entry.
 * @param clusterId The identifier of the targeted cluster.
 * @param clusterDir the cluster direction (0 = client->server; 1 = server->client)
 * @param zclCmd The data to encode in the payload for the ZCL Configure Report command.
 * @return 1 if message sent successfully, 0 otherwise.
 */
bool azapZclReadReportConfigCmdReq(
  azap_uint8 port,
  AZAP_REQ_TTL_MGMT_MODE ttlMode,
  void *issuerContext,
  azap_uint16 nwkAddrDest,
  azap_uint8 localEndp,
  azap_uint8 remoteEndp,
  azap_uint16 clusterId,
  azap_uint8 clusterDir,
  azapZclReadReportCfgCommand_t *zclCmd)
{
  bool retVal = false;
  azap_uint8 payload[MAX_ZNP_PAYLOAD_SIZE];
  azap_uint8 payloadLen = 0;
  azap_uint8 i;

  RTL_TRDBG(TRACE_API, ">>> API azapZclReadReportConfigCmdReq (port:%d) "
      "(nwkAddrDest:0x%.4x) (localEndp:0x%.2x) (remoteEndp:0x%.2x) (clusterId:0x%.4x) "
      "(clientDir:0x%.2x) (numAttr:%d)\n", port, nwkAddrDest, localEndp, remoteEndp,
      clusterId, clusterDir, zclCmd ? zclCmd->numAttr : 0);

  portContext_t *portCntxt = g_portContexts[port];
  if ((portCntxt) && (zclCmd))
  {
    azapNbZclRequestsIssued[port] ++;
    for (i = 0; i < zclCmd->numAttr && (payloadLen < MAX_ZNP_PAYLOAD_SIZE - 3); i++)
    {
      payload[ payloadLen++ ] = zclCmd->attrList[i].direction;
      payload[ payloadLen++ ] = LO_UINT16(zclCmd->attrList[i].attrId);
      payload[ payloadLen++ ] = HI_UINT16(zclCmd->attrList[i].attrId);
    }
    retVal = portCntxt->sendZcl(portCntxt, ttlMode, issuerContext,
        AZAP_ZCL_READ_REPORTING_CONF_CMD, true, nwkAddrDest, remoteEndp, localEndp,
        clusterId, clusterDir, 0, true, ZCL_CMD_READ_REPORT_CFG,
        ZCL_CMD_READ_REPORT_CFG_RSP, payloadLen, payload);
  }

  return retVal;
}


/**
 * Stimulate AZAP layer for sending a ZCL "Discover" command.
 * @param port the reference port associated to a hardware/HAN
 * @param ttlMode the TTL request management model (@see AZAP_REQ_TTL_MGMT_MODE).
 * @param issuerContext the request issuer context.
 * @param nwkAddrDest Network address of the targeted device (retrieved from NWK layer).
 * @param localEndp end point identifier of the local zigbee node.
 * @param remoteEndp end point identifier of the remote zigbee node to target.
 * @param clusterId The identifier of the targeted cluster.
 * @param clusterDir the cluster direction (0 = client->server; 1 = server->client)
 * @param zclCmd The data to encode in the payload for the ZCL Discover command.
 * @return 1 if message sent successfully, 0 otherwise.
 */
bool azapZclDiscoverCmdReq(
  azap_uint8 port,
  AZAP_REQ_TTL_MGMT_MODE ttlMode,
  void *issuerContext,
  azap_uint16 nwkAddrDest,
  azap_uint8 localEndp,
  azap_uint8 remoteEndp,
  azap_uint16 clusterId,
  azap_uint8 clusterDir,
  azapZclDiscoverCommand_t *zclCmd)
{
  bool retVal = false;
  azap_uint8 payload[3]; // The exact payload size

  RTL_TRDBG(TRACE_API, ">>> API azapZclDiscoverCmdReq (port:%d) (nwkAddrDest:0x%.4x) "
      "(localEndp:0x%.2x) (remoteEndp:0x%.2x) (clusterId:0x%.4x) (clientDir:0x%.2x) "
      "(startAttrId:%d)\n", port, nwkAddrDest, localEndp, remoteEndp, clusterId, clusterDir,
      zclCmd ? zclCmd->startAttrId : 0);

  portContext_t *portCntxt = g_portContexts[port];
  if ((portCntxt) && (zclCmd))
  {
    azapNbZclRequestsIssued[port] ++;

    payload[ 0 ] = LO_UINT16(zclCmd->startAttrId);
    payload[ 1 ] = HI_UINT16(zclCmd->startAttrId);
    payload[ 2 ] = zclCmd->maxNumAttrIds;

    retVal = portCntxt->sendZcl(portCntxt, ttlMode, issuerContext,
        AZAP_ZCL_DISCOVER_CMD, true, nwkAddrDest, remoteEndp, localEndp,
        clusterId, clusterDir, 0, true, ZCL_CMD_DISCOVER, ZCL_CMD_DISCOVER_RSP,
        3, payload);
  }

  return retVal;
}


/**
 * Stimulate AZAP layer for sending a ZCL command (generic).
 * @param port the reference port associated to a hardware/HAN
 * @param ttlMode the TTL request management model (@see AZAP_REQ_TTL_MGMT_MODE).
 * @param issuerContext the request issuer context.
 * @param nwkAddrDest Network address of the targeted device (retrieved from NWK layer).
 * @param localEndp end point identifier of the local zigbee node that sends this ZCl
 * message.
 * @param remoteEndp end point identifier of the remote zigbee node to target.
 * @param clusterId The identifier of the targeted cluster.
 * @param clusterDir the cluster direction (0 = client->server; 1 = server->client)
 * @param defaultResponse specifies if a Zigbee DefaultResponse must be generated in return.
 * @param cmdCode command code, specific to this clusterId and direction.
 * @param expectedCmdCode the command code expected as response to this request.
 * @param cmdData command parameters (corresponding to the command code)
 * @param dataLen size of the command parameters.
 * @return 1 if message sent successfully, 0 otherwise.
 */
bool azapZclOutgoingCmdReq(
  azap_uint8 port,
  AZAP_REQ_TTL_MGMT_MODE ttlMode,
  void *issuerContext,
  azap_uint16 nwkAddrDest,
  azap_uint8 localEndp,
  azap_uint8 remoteEndp,
  azap_uint16 clusterId,
  azap_uint8 clusterDir,
  bool defaultResponse,
  azap_uint8 cmdCode,
  azap_uint8 expectedCmdCode,
  azap_byte *cmdData,
  azap_uint16 dataLen)
{
  bool retVal = false;
  bool responseExpected = true;

  RTL_TRDBG(TRACE_API, ">>> API azapZclOutgoingCmdReq (port:%d) (nwkAddrDest:0x%.4x) "
      "(localEndp:0x%.2x) (remoteEndp:0x%.2x) (clusterId:0x%.4x) (clientDir:0x%.2x) "
      "(cmdCode:0x%.2x)\n", port, nwkAddrDest, localEndp, remoteEndp, clusterId, clusterDir,
      cmdCode);

  portContext_t *portCntxt = g_portContexts[port];
  if (portCntxt)
  {
    azapNbZclRequestsIssued[port] ++;

    if (0xFF == expectedCmdCode)
    {
      responseExpected = false;
    }
    retVal = portCntxt->sendZcl(portCntxt, ttlMode, issuerContext, AZAP_ZCL_OUTGOING_CMD,
        responseExpected, nwkAddrDest, remoteEndp, localEndp, clusterId, clusterDir, 0,
        defaultResponse, cmdCode, expectedCmdCode, dataLen, cmdData);
  }
  return retVal;
}

/**
 * Stimulate AZAP layer for sending a ZCL command (generic), using IEEE destination address.
 * @param port the reference port associated to a hardware/HAN
 * @param ttlMode the TTL request management model (@see AZAP_REQ_TTL_MGMT_MODE).
 * @param issuerContext the request issuer context.
 * @param nwkAddrDest Network address of the targeted device (retrieved from NWK layer).
 * @param localEndp end point identifier of the local zigbee node that send this ZCL command.
 * @param remoteIeeeAddr IEEE address of the remote zigbee node to target.
 * @param remoteEndp end point identifier of the remote zigbee node to target.
 * @param clusterId The identifier of the targeted cluster.
 * @param clusterDir the cluster direction (0 = client->server; 1 = server->client)
 * @param defaultResponse specifies if a Zigbee DefaultResponse must be generated in return.
 * @param cmdCode command code, specific to this clusterId and direction.
 * @param expectedCmdCode the command code expected as response to this request.
 * @param cmdData command parameters (corresponding to the command code)
 * @param dataLen size of the command parameters.
 * @return 1 if message sent successfully, 0 otherwise.
 */
bool azapZclOutgoingCmdReq_ext(
  azap_uint8 port,
  AZAP_REQ_TTL_MGMT_MODE ttlMode,
  void *issuerContext,
  azap_uint16 nwkAddrDest,
  azap_uint8 localEndp,
  azap_uint8 remoteIeeeAddr[IEEE_ADDR_LEN],
  azap_uint8 remoteEndp,
  azap_uint16 clusterId,
  azap_uint8 clusterDir,
  bool defaultResponse,
  azap_uint8 cmdCode,
  azap_uint8 expectedCmdCode,
  azap_byte *cmdData,
  azap_uint16 dataLen)
{
  bool retVal = false;
//  azap_uint8 payload[MAX_ZNP_PAYLOAD_SIZE];
//  azap_uint8 i;

  RTL_TRDBG(TRACE_API, ">>> API azapZclOutgoingCmdReq_ext - NOT IMPLEMENTED YET "
      "(port:%d) (nwkAddrDest:0x%.4x) "
      "(localEndp:0x%.2x) (remoteIeeeAddr:%s) (remoteEndp:0x%.2x) (clusterId:0x%.4x) "
      "(clientDir:0x%.2x) (cmdCode:0x%.2x)\n", port, nwkAddrDest, localEndp,
      azap_extAddrToString(remoteIeeeAddr, true), remoteEndp, clusterId, clusterDir,
      cmdCode);

  return retVal;
}

// useful when used as a node
/**
 * Stimulate AZAP layer for sending a ZCL report command.
 * @param port the reference port associated to a hardware/HAN
 * @param nwkAddrDest Network address of the targeted device (retrieved from NWK layer).
 * @param localEndp end point that initiates this report
 * @param remoteEndp end point identifier of the remote zigbee node to target.
 * @param clusterId The identifier of the targeted cluster.
 * @param zclCmd The data to encode in the payload for the ZCL Report command.
 * @return 1 if message sent successfully, 0 otherwise.
 */
bool azapZclReportCmdInd(
  azap_uint8 port,
  azap_uint16 nwkAddrDest,
  azap_uint8 localEndp,
  azap_uint8 remoteEndp,
  azap_uint16 clusterId,
  azapZclReportCommand_t *zclCmd)
{
  bool retVal = false;
//  azap_uint8 payload[MAX_ZNP_PAYLOAD_SIZE];
//  azap_uint8 i;

  RTL_TRDBG(TRACE_API, ">>> API azapZclReportCmdInd - NOT IMPLEMENTED YET "
      "(port:%d) (nwkAddrDest:0x%.4x) "
      "(remoteEndp:0x%.2x) (clusterId:0x%.4x) (numAttr:%d)\n", port,
      nwkAddrDest, remoteEndp, clusterId, zclCmd ? zclCmd->numAttr : 0);

  return retVal;
}

/**
 * @brief Retrieve the data length depending on the type of the encoded data.
 * @param type the data type.
 * @param data pointer on the buffer that convey the encoded data.
 * @return the data length, or 0 if data type is not supported.
 */
azap_uint8 azapGetZclDataLength(azap_uint8 type, azap_uint8 *data)
{
  azap_uint8 res = 0;
  switch (type)
  {
    case ZCL_DATATYPE_CHAR_STR:
    case ZCL_DATATYPE_OCTET_STR:
      if (data)
      {
        res = data[0] + 1;
      }
      break;

    case ZCL_DATATYPE_LONG_CHAR_STR:
    case ZCL_DATATYPE_LONG_OCTET_STR:
      // for now we only support string size less than 255
      if ((data) && (0x00 == data[1]))
      {
        res = data[0] + 2;
      }
      break;

    case ZCL_DATATYPE_DATA8:
    case ZCL_DATATYPE_BOOLEAN:
    case ZCL_DATATYPE_BITMAP8:
    case ZCL_DATATYPE_INT8:
    case ZCL_DATATYPE_UINT8:
    case ZCL_DATATYPE_ENUM8:
      res = 1;
      break;

    case ZCL_DATATYPE_DATA16:
    case ZCL_DATATYPE_BITMAP16:
    case ZCL_DATATYPE_UINT16:
    case ZCL_DATATYPE_INT16:
    case ZCL_DATATYPE_ENUM16:
    case ZCL_DATATYPE_SEMI_PREC:
    case ZCL_DATATYPE_CLUSTER_ID:
    case ZCL_DATATYPE_ATTR_ID:
      res = 2;
      break;

    case ZCL_DATATYPE_DATA24:
    case ZCL_DATATYPE_BITMAP24:
    case ZCL_DATATYPE_UINT24:
    case ZCL_DATATYPE_INT24:
      res = 3;
      break;

    case ZCL_DATATYPE_DATA32:
    case ZCL_DATATYPE_BITMAP32:
    case ZCL_DATATYPE_UINT32:
    case ZCL_DATATYPE_INT32:
    case ZCL_DATATYPE_SINGLE_PREC:
    case ZCL_DATATYPE_TOD:
    case ZCL_DATATYPE_DATE:
    case ZCL_DATATYPE_UTC:
    case ZCL_DATATYPE_BAC_OID:
      res = 4;
      break;

    case ZCL_DATATYPE_UINT40:
      res = 5;
      break;

    case ZCL_DATATYPE_UINT48:
      res = 6;
      break;

    case ZCL_DATATYPE_DOUBLE_PREC:
    case ZCL_DATATYPE_IEEE_ADDR:
      res = 8;
      break;

    default:
      break;
  }

  return res;
}

/**
 * @brief Determines if the data encoding match an analog data type.
 * @param type the data type.
 * @return true if the data is analog, false otherwise.
 */
bool azapZclIsAnalogDataType(azap_uint8 type)
{
  bool res = false;
  switch (type)
  {
    case ZCL_DATATYPE_UINT8:
    case ZCL_DATATYPE_UINT16:
    case ZCL_DATATYPE_UINT24:
    case ZCL_DATATYPE_UINT32:
    case ZCL_DATATYPE_UINT40:
    case ZCL_DATATYPE_UINT48:
    case ZCL_DATATYPE_INT8:
    case ZCL_DATATYPE_INT16:
    case ZCL_DATATYPE_INT24:
    case ZCL_DATATYPE_INT32:
    case ZCL_DATATYPE_SEMI_PREC:
    case ZCL_DATATYPE_SINGLE_PREC:
    case ZCL_DATATYPE_DOUBLE_PREC:
    case ZCL_DATATYPE_TOD:
    case ZCL_DATATYPE_DATE:
    case ZCL_DATATYPE_UTC:
      res = true;
      break;

    default:
      res = false;
      break;
  }
  return res;
}

///////////////////////////////////////////////////////////////////////////////
// regular processing at the scale of milliseconds

/**
 * @fn static void processZnpRxMsg()
 * @brief process the ZNP received messages for all ports
 */
static void processZnpRxMsg()
{
  int i;

  for (i = 0; i < MAX_NB_PORT_CONTEXT; i++)
  {
    portContext_t *portCntxt = g_portContexts[i];
    if (portCntxt)
    {
      if (portCntxt->isStarted(portCntxt))
      {
        portCntxt->processRxMsg(portCntxt);
      }
    }
  }
}

/**
 * @brief Main loop tick for regular processings.
 */
void azapClockMs()
{
  processZnpRxMsg();
}

///////////////////////////////////////////////////////////////////////////////
// regular processing at the scale of second

/**
 * @brief Main loop tick for regular processings.
 */
void azapClockSc(time_t now)
{
  int i;

  for (i = 0; i < MAX_NB_PORT_CONTEXT; i++)
  {
    portContext_t *portCntxt = g_portContexts[i];
    if (portCntxt)
    {
      if (portCntxt->isStarted(portCntxt))
      {
        // the ZNP start procedure is currently running
        if (portCntxt->startInst)
        {
          portCntxt->startInst->watchdog(portCntxt->startInst);
        }
        portCntxt->requestManager->tic(portCntxt->requestManager, portCntxt);
      }
    }
  }
}


