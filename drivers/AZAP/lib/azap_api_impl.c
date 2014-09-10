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
//** File : ./lib/azap_api_impl.c
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
#include <string.h>

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

#include "format_znp.h"
#include "format_znp_af.h"

#include "azap_api_impl.h"

extern portContext_t *g_portContexts[];

unsigned long azapNbZdoNotifications[MAX_NB_PORT_CONTEXT];
unsigned long azapNbZdoRequestsIssued[MAX_NB_PORT_CONTEXT];
unsigned long azapNbZdoRequestsCompleted[MAX_NB_PORT_CONTEXT];
unsigned long azapNbZdoRequestsFailed[MAX_NB_PORT_CONTEXT];
unsigned long azapNbZclNotifications[MAX_NB_PORT_CONTEXT];
unsigned long azapNbZclRequestsIssued[MAX_NB_PORT_CONTEXT];
unsigned long azapNbZclRequestsCompleted[MAX_NB_PORT_CONTEXT];
unsigned long azapNbZclRequestsFailed[MAX_NB_PORT_CONTEXT];

///////////////////////////////////////////////////////////////////////////////


#define PROCESS_CALL_BACK_NOT_FOUND(port,cb,minSize,maxSize)               \
    if ( (len < minSize) || (len > maxSize) )                              \
    {                                                                      \
      RTL_TRDBG(TRACE_ERROR, "CALLBACK Error in length on %s (port:%d) "   \
        "(len:%d)\n", #cb, port->portNumber, len);                         \
      return;                                                              \
    }                                                                      \
    if ( (! port->pCallbacks) || (! port->pCallbacks->cb) )                \
    {                                                                      \
      RTL_TRDBG(TRACE_ERROR, "CALLBACK Error on %s (port:%d) "             \
        "(data:0x%.8x)\n", #cb, port->portNumber, data);                   \
      return;                                                              \
    }

/**
 * @brief Initialize the AZAP API implementation layer.
 * @param port the port context for the targeted hardware device
 * @param targetedIeee the 802.15.4 IEEE address that the AZAP has to look for
 * auto-detection. If null, the AZAP layer will assume the first hardware device
 * that matches the CC2531 specificities.
 * @param mainTbPoll the rtl_poll global table
 * @param pCallbacks Callback functions fir ZAP events.
 * @param defaultChannel radio channel used by default to build the HAN (range:11-26)
 * @param deviceType device type to implement (zc: 0, zr: 1, zed: 2)
 * @param resetHanAtReboot specifies if HAN parameters (panId, panExtId)
 * must either be reset at ZAP startup when set to true, either be restored as
 * they were saved at previous run.
 */
void azapInitImpl(azap_uint8 port, azap_uint8 *targetedIeee, void *mainTbPoll,
    t_azap_api_callbacks *pCallbacks, azap_uint8 defaultChannel, azap_uint8 deviceType,
    bool resetHanAtReboot)
{
  if (NULL == g_portContexts[port])
  {
    portContext_t *portCntxt = new_portContext_t(port);
    portCntxt->pCallbacks = pCallbacks;
    portCntxt->pMainTbPoll = mainTbPoll;
    if ((defaultChannel >= 11) && (defaultChannel <= 26))
    {
      portCntxt->defaultChannel = (1 << defaultChannel);
    }
    portCntxt->deviceType = deviceType;
    portCntxt->resetHanAtReboot = resetHanAtReboot;
    if (targetedIeee)
    {
      memcpy(portCntxt->deviceIeee, targetedIeee, IEEE_ADDR_LEN);
    }
    g_portContexts[port] = portCntxt;
  }
}

/**
 * Release handler on listener, and free resources.
 * @param port the port context for the targeted hardware device
 */
void azapUnInitImpl(azap_uint8 port)
{
  portContext_t *portCntxt = g_portContexts[port];
  if (portCntxt)
  {
    portCntxt->free(portCntxt);
  }
}


/**
 * @brief Wrapping function for AZAP layer starting phase completion
 * @param port the port context for the targeted hardware device
 * @param success indicates if start-up was successful
 */
void azapProcessStartedCb(
  azap_uint8 port,
  bool success)
{
  portContext_t *cntxt;
  if ( (cntxt = g_portContexts[port]) && (cntxt->pCallbacks) &&
      (cntxt->pCallbacks->azapStartedCb) )
  {
    RTL_TRDBG(TRACE_API, "<<< CALLBACK azapProcessStartedCb (port:%d) (ZStackVersion:%s) "
        "(success:%d) (shortAdd:0x%.4x) (deviceIEEE:%s) (panId:0x%.4x) (panIdExt:%s)\n",
        port, cntxt->zstackVersion, success, cntxt->shortAddr,
        azap_extAddrToString(cntxt->deviceIeee, true), cntxt->panId,
        azap_extAddrToString(cntxt->panIdExt, true));
    cntxt->pCallbacks->azapStartedCb(port, success, cntxt->shortAddr, cntxt->deviceIeee,
        cntxt->panId, cntxt->panIdExt);
  }
  else
  {
    RTL_TRDBG(TRACE_ERROR, "CALLBACK Error on azapProcessStartedCb (port:%d) (cntxt:0x%.8x) "
        "(success:%d)\n", port, cntxt, success);
  }
}

/***************************************************************************************
 *        ____    _    ____ ___             _ _   _                _                   *
 *       / ___|  / \  |  _ \_ _|   ___ __ _| | | | |__   __ _  ___| | _____            *
 *       \___ \ / _ \ | |_) | |   / __/ _` | | | | '_ \ / _` |/ __| |/ / __|           *
 *        ___) / ___ \|  __/| |  | (_| (_| | | | | |_) | (_| | (__|   <\__ \           *
 *       |____/_/   \_\_|  |___|  \___\__,_|_|_| |_.__/ \__,_|\___|_|\_\___/           *
 *                                                                                     *
 ***************************************************************************************/
////////////////////////////////////////////////////////////////////////////////////////
// SAPI call backs
/**
 * @brief Parse a SAPI FindDeviceConf call back and process the result.
 * @param port the port context for the targeted hardware device
 * @param srcAddr the source address retrieved from ZDO_MSG_CB_INCOMING call back.
 * @param len length of the message payload
 * @param data the message payload
 */
void azapProcessSapiFindDeviceConfCb(
  portContext_t *port,
  azap_uint16 srcAddr,
  azap_uint8 len,
  azap_byte *data)
{
  PROCESS_CALL_BACK_NOT_FOUND(port, azapZdpDeviceAnnceCb, 0x0B, 0x0B);

#ifndef __NO_TRACE__
  // search type
  azap_uint8 searchType = parseUint8AndShift(&data, &len);
#endif
  // search key
  azap_uint16 nwkAddr = parseUint16AndShift(&data, &len);
  // result
  azap_uint8 ieeeAddr[IEEE_ADDR_LEN];
  parseExtAddrAndShift(ieeeAddr, &data, &len);

  RTL_TRDBG(TRACE_API, "<<< CALLBACK azapProcessSapiFindDeviceConfCb (port:%d) "
      "(srcAddr:0x%.4x) (searchType:0x%.2x) (nwkAddr:0x%.4x) (result:%s)\n",
      port->portNumber, srcAddr, searchType, nwkAddr, azap_extAddrToString(ieeeAddr, true));
  port->pCallbacks->azapSapiFindDeviceConfCb(port->portNumber, nwkAddr, ieeeAddr);
}


/***************************************************************************************
 *       _________   ___    _                           _   _                          *
 *      |__  /  _ \ / _ \  (_)_ ____   _____   ___ __ _| |_(_) ___  _ __  ___          *
 *        / /| | | | | | | | | '_ \ \ / / _ \ / __/ _` | __| |/ _ \| '_ \/ __|         *
 *       / /_| |_| | |_| | | | | | \ V / (_) | (_| (_| | |_| | (_) | | | \__ \         *
 *      /____|____/ \___/  |_|_| |_|\_/ \___/ \___\__,_|\__|_|\___/|_| |_|___/         *
 *                                                                                     *
 ***************************************************************************************/

////////////////////////////////////////////////////////////////////////////////////////
// ZDO request messages call backs

/**
 * @brief Wrapping function for ZDP Bind_req messages callback
 * @param port the port context for the targeted hardware device
 * @param srcAddr the source address retrieved from ZDO_MSG_CB_INCOMING call back.
 * @param len length of the message payload
 * @param data the message payload
 */
void azapProcessZdoBindReqCb(
  portContext_t *port,
  azap_uint16 srcAddr,
  azap_uint8 len,
  azap_byte *data)
{
  // TODO required by ZigBee testerman probe
}

/**
 * @brief Wrapping function for ZDP Bind_req messages callback
 * @param port the port context for the targeted hardware device
 * @param srcAddr the source address retrieved from ZDO_MSG_CB_INCOMING call back.
 * @param len length of the message payload
 * @param data the message payload
 */
void azapProcessZdoUnbindReqCb(
  portContext_t *port,
  azap_uint16 srcAddr,
  azap_uint8 len,
  azap_byte *data)
{
  // TODO required by ZigBee testerman probe
}


////////////////////////////////////////////////////////////////////////////////////////
// ZDO response messages call backs

/**
 * @brief Wrapping function for ZDP Device_annce messages callback
 * @param port the port context for the targeted hardware device
 * @param srcAddr the source address retrieved from ZDO_MSG_CB_INCOMING call back.
 * @param len length of the message payload
 * @param data the message payload
 */
void azapProcessZdoDeviceAnnceCb(
  portContext_t *port,
  azap_uint16 srcAddr,
  azap_uint8 len,
  azap_byte *data)
{
  (void)srcAddr;
  PROCESS_CALL_BACK_NOT_FOUND(port, azapZdpDeviceAnnceCb, 0x0B, 0x0B);

  azap_uint16 nwkAddrSrc;
  azap_uint8 ieeeAddr[IEEE_ADDR_LEN];
  azap_uint8 capabilities;
  azap_uint8 alternatePanCoordinator;
  azap_uint8 deviceType;
  azap_uint8 powerSource;
  azap_uint8 receiverOnWhenIdle;
  azap_uint8 securityCapability;
  azap_uint8 allocateAddress;

  // Short address
  nwkAddrSrc = parseUint16AndShift(&data, &len);
  // IEEE address
  parseExtAddrAndShift(ieeeAddr, &data, &len);
  // capabilities bitmask
  capabilities = parseUint8AndShift(&data, &len);
  alternatePanCoordinator = capabilities & 0x01;
  deviceType  = (capabilities & 0x02) >> 1;
  powerSource  = (capabilities & 0x04) >> 2;
  receiverOnWhenIdle  = (capabilities & 0x08) >> 3;
  securityCapability  = (capabilities & 0x40) >> 6;
  allocateAddress  = (capabilities & 0x80) >> 7;

  RTL_TRDBG(TRACE_API, "<<< CALLBACK azapProcessZdoDeviceAnnceCb (port:%d) "
      "(nwkAddrSrc:0x%.4x) (ieeeAddr:%s) (capabilities:0x%.2x)\n", port->portNumber,
      nwkAddrSrc, azap_extAddrToString(ieeeAddr, true), capabilities);
  port->pCallbacks->azapZdpDeviceAnnceCb(port->portNumber, nwkAddrSrc, ieeeAddr,
      alternatePanCoordinator, deviceType, powerSource, receiverOnWhenIdle,
      securityCapability, allocateAddress);
}

/**
 * @brief Wrapping function for ZDP NWK_addr_rsp messages callback
 * @param port the port context for the targeted hardware device
 * @param srcAddr the source address retrieved from ZDO_MSG_CB_INCOMING call back.
 * @param len length of the message payload
 * @param data the message payload
 */
void azapProcessZdoNwkAddrRspCb(
  portContext_t *port,
  azap_uint16 srcAddr,
  azap_uint8 len,
  azap_byte *data)
{
  /**
   * NOTE: According to the ZNP specification this call back should always has a payload
   * size between 0x0D and 0x53... but in real, 'numAssocDev' is provided before
   * 'startIndex' contrary to what is documented. Furthermore if 'numAssocDev' equals 0
   * 'startIndex' is not provided at all... Indeed looking at ZStack sources, one can see
   * that this parameter is not expected in "ZDO_ParseAddrRsp" method in such a case.
   * So we use the interval 0x0C-0x52 instead.
   */
  PROCESS_CALL_BACK_NOT_FOUND(port, azapZdpNwkAddrRspCb, 0x0C, 0x52);

  azap_uint8 status = 0;
  azap_uint8 startIndex = 0;
  azap_uint8 numAssocDev = 0;
  azap_uint16 nwkAddr = 0;
  azap_uint8 ieeeAddr[IEEE_ADDR_LEN];
  azap_uint16 *nwkAddrAssocDevList = NULL;
  requestContext_t *context = NULL;

  memset(ieeeAddr, 0, IEEE_ADDR_LEN);
  status = parseUint8AndShift(&data, &len);
  if (STATUS_CODE_SUCCESS == status)
  {
    parseExtAddrAndShift(ieeeAddr, &data, &len);
    nwkAddr = parseUint16AndShift(&data, &len);
    startIndex = parseUint8AndShift(&data, &len);
    numAssocDev = parseUint8AndShift(&data, &len);
    nwkAddrAssocDevList = parseUint16ArrayAndShift(&data, numAssocDev, &len);
  }

  context = port->requestManager->removePending(port->requestManager, srcAddr,
      AZAP_ZDO_NWK_ADDR_REQ, 0, 0, 0);
  RTL_TRDBG(TRACE_API, "<<< CALLBACK azapProcessZdoNwkAddrRspCb (port:%d) (srcAddr:0x%.4x) "
      "(status:0x%.2x) (nwkAddr:0x%.4x) (ieeeAddr:%s) (numAssocDev:%d) "
      "(nwkAddrAssocDevList:%s)\n", port->portNumber, srcAddr, status, nwkAddr,
      azap_extAddrToString(ieeeAddr, true), numAssocDev,
      azap_uint16ArrayToString(nwkAddrAssocDevList, numAssocDev));
  port->pCallbacks->azapZdpNwkAddrRspCb(port->portNumber,
      context ? context->issuerContext : NULL, srcAddr, status, ieeeAddr, nwkAddr,
      numAssocDev, startIndex, nwkAddrAssocDevList);

  if (nwkAddrAssocDevList)
  {
    free(nwkAddrAssocDevList);
  }
  if (context)
  {
    context->free(context);
  }

}

/**
 * @brief Wrapping function for ZDP IEEE_addr_rsp messages callback
 * @param port the port context for the targeted hardware device
 * @param srcAddr the source address retrieved from ZDO_MSG_CB_INCOMING call back.
 * @param len length of the message payload
 * @param data the message payload
 */
void azapProcessZdoIeeeAddrRspCb(
  portContext_t *port,
  azap_uint16 srcAddr,
  azap_uint8 len,
  azap_byte *data)
{
  /**
   * NOTE: According to the ZNP specification this call back should always has a payload
   * size between 0x0D and 0x53... but in real, 'numAssocDev' is provided before
   * 'startIndex' contrary to what is documented. Furthermore if 'numAssocDev' equals 0
   * 'startIndex' is not provided at all... Indeed looking at ZStack sources, one can see
   * that this parameter is not expected in "ZDO_ParseAddrRsp" method in such a case.
   * So we use the interval 0x0C-0x52 instead.
   */
  PROCESS_CALL_BACK_NOT_FOUND(port, azapZdpIeeeAddrRspCb, 0x0B, 0x52);

  azap_uint8 status = 0;
  azap_uint8 startIndex = 0;
  azap_uint8 numAssocDev = 0;
  azap_uint16 nwkAddr = 0;
  azap_uint8 ieeeAddr[IEEE_ADDR_LEN];
  azap_uint16 *nwkAddrAssocDevList = NULL;
  requestContext_t *context = NULL;

  memset(ieeeAddr, 0, IEEE_ADDR_LEN);
  status = parseUint8AndShift(&data, &len);
  if (STATUS_CODE_SUCCESS == status)
  {
    parseExtAddrAndShift(ieeeAddr, &data, &len);
    nwkAddr = parseUint16AndShift(&data, &len);
    numAssocDev = parseUint8AndShift(&data, &len);
    startIndex = parseUint8AndShift(&data, &len);
    nwkAddrAssocDevList = parseUint16ArrayAndShift(&data, numAssocDev, &len);
  }

  context = port->requestManager->removePending(port->requestManager, srcAddr,
      AZAP_ZDO_IEEE_ADDR_REQ, 0, 0, 0);
  RTL_TRDBG(TRACE_API, "<<< CALLBACK azapProcessZdoIeeeAddrRspCb (port:%d) (srcAddr:0x%.4x) "
      "(status:0x%.2x) (nwkAddr:0x%.4x) (ieeeAddr:%s) (numAssocDev:%d) "
      "(nwkAddrAssocDevList:%s)\n", port->portNumber, srcAddr, status, nwkAddr,
      azap_extAddrToString(ieeeAddr, true), numAssocDev,
      azap_uint16ArrayToString(nwkAddrAssocDevList, numAssocDev));
  port->pCallbacks->azapZdpIeeeAddrRspCb(port->portNumber,
      context ? context->issuerContext : NULL, srcAddr, status, ieeeAddr, nwkAddr,
      numAssocDev, startIndex, nwkAddrAssocDevList);

  if (nwkAddrAssocDevList)
  {
    free(nwkAddrAssocDevList);
  }
  if (context)
  {
    context->free(context);
  }

}

/**
 * @brief Wrapping function for ZDP Node_Desc_rsp messages callback
 * @param port the port context for the targeted hardware device
 * @param srcAddr the source address retrieved from ZDO_MSG_CB_INCOMING call back.
 * @param len length of the message payload
 * @param data the message payload
 */
void azapProcessZdoNodeDescRspCb(
  portContext_t *port,
  azap_uint16 srcAddr,
  azap_uint8 len,
  azap_byte *data)
{
  /**
   * NOTE: According to the ZNP specification this call back should always has a payload
   * size equals to 0x12... but in real the srcAddr first parameter is never provided.
   * Indeed looking at ZStack sources, one can see that this parameter is not expected in
   * "ZDO_ParseNodeDescRsp" method. So we use the 0x10 instead.
   */
  PROCESS_CALL_BACK_NOT_FOUND(port, azapZdpNodeDescRspCb, 0x10, 0x10);

  azap_uint8 readByte;
  azap_uint8 status = 0;
  azap_uint16 nwkAddr = 0;
  azap_uint8 logicalType = 0;
  azap_uint8 complexDescriptorAvailable = 0;
  azap_uint8 userDescriptorAvailable = 0;
  azap_uint8 apsFlags = 0;
  azap_uint8 frequencyBand = 0;
  azap_uint8 macCapabilityFlags = 0;
  azap_uint16 manufacturerCode = 0;
  azap_uint8 maximumBufferSize = 0;
  azap_uint16 maximumIncomingTransferSize = 0;
  azap_uint16 serverMask = 0;
  azap_uint16 maximumOutgoingTransferSize = 0;
  azap_uint8 descriptorCapabilityField = 0;
  requestContext_t *context = NULL;

  status = parseUint8AndShift(&data, &len);
  if (STATUS_CODE_SUCCESS == status)
  {
    nwkAddr = parseUint16AndShift(&data, &len);
    // LogicalType / ComplexDescriptorAvailable / UserDescriptorAvailable
    readByte = parseUint8AndShift(&data, &len);
    logicalType = readByte & 0x07;
    complexDescriptorAvailable = (readByte & 0x10) >> 4;
    userDescriptorAvailable = (readByte & 0xE0) >> 5;
    // APSFlags / FrequencyBand
    readByte = parseUint8AndShift(&data, &len);
    apsFlags = readByte & 0x1F;
    frequencyBand = (readByte & 0xE0) >> 5;
    macCapabilityFlags = parseUint8AndShift(&data, &len);
    manufacturerCode = parseUint16AndShift(&data, &len);
    maximumBufferSize = parseUint8AndShift(&data, &len);
    maximumIncomingTransferSize = parseUint16AndShift(&data, &len);
    serverMask = parseUint16AndShift(&data, &len);
    maximumOutgoingTransferSize = parseUint16AndShift(&data, &len);
    descriptorCapabilityField = parseUint8AndShift(&data, &len);
  }

  context = port->requestManager->removePending(port->requestManager, srcAddr,
      AZAP_ZDO_NODE_DESC_REQ, 0, 0, 0);
  RTL_TRDBG(TRACE_API, "<<< CALLBACK azapProcessZdoNodeDescRspCb (port:%d) (srcAddr:0x%.4x) "
      "(status:0x%.2x) (nwkAddr:0x%.4x) (logicalType:0x%.2x) (complexDescriptorAvailable:0x%.2x) "
      "(userDescriptorAvailable:0x%.2x) (apsFlags:0x%.2x) (frequencyBand:0x%.2x) "
      "(macCapabilityFlags:0x%.2x) (manufacturerCode:0x%.4x) (maximumBufferSize:0x%.2x) "
      "(maximumIncomingTransferSize:0x%.4x) (serverMask:0x%.4x) "
      "(maximumOutgoingTransferSize:0x%.4x) (descriptorCapabilityField:0x%.2x)\n",
      port->portNumber, srcAddr, status, nwkAddr, logicalType, complexDescriptorAvailable,
      userDescriptorAvailable, apsFlags, frequencyBand, macCapabilityFlags, manufacturerCode,
      maximumBufferSize, maximumIncomingTransferSize, serverMask, maximumOutgoingTransferSize,
      descriptorCapabilityField);
  port->pCallbacks->azapZdpNodeDescRspCb(port->portNumber,
      context ? context->issuerContext : NULL, srcAddr, status, nwkAddr,
      logicalType, complexDescriptorAvailable, userDescriptorAvailable, apsFlags, frequencyBand,
      macCapabilityFlags, manufacturerCode, maximumBufferSize, maximumIncomingTransferSize,
      serverMask, maximumOutgoingTransferSize, descriptorCapabilityField);

  if (context)
  {
    context->free(context);
  }
}

/**
 * @brief Wrapping function for ZDP Power_Desc_rsp messages callback
 * @param port the port context for the targeted hardware device
 * @param srcAddr the source address retrieved from ZDO_MSG_CB_INCOMING call back.
 * @param len length of the message payload
 * @param data the message payload
 */
void azapProcessZdoPowerDescRspCb(
  portContext_t *port,
  azap_uint16 srcAddr,
  azap_uint8 len,
  azap_byte *data)
{
  /**
   * NOTE: According to the ZNP specification this call back should always has a payload
   * size equals to 0x07... but in real the srcAddr first parameter is never provided.
   * Indeed looking at ZStack sources, one can see that this parameter is not expected in
   * "ZDO_ParsePowerDescRsp" method. So we use the 0x05 instead.
   */
  PROCESS_CALL_BACK_NOT_FOUND(port, azapZdpPowerDescRspCb, 0x05, 0x05);

  azap_uint8 readByte;
  azap_uint8 status = 0;
  azap_uint16 nwkAddr = 0;
  azap_uint8 currentPowerMode = 0;
  azap_uint8 availablePowerSources = 0;
  azap_uint8 currentPowerSource = 0;
  azap_uint8 currentPowerSourceLevel = 0;
  requestContext_t *context = NULL;

  status = parseUint8AndShift(&data, &len);
  if (STATUS_CODE_SUCCESS == status)
  {
    nwkAddr = parseUint16AndShift(&data, &len);
    // CurrentPowerMode / AvailablePowerSources
    readByte = parseUint8AndShift(&data, &len);
    currentPowerMode = readByte & 0x0F;
    availablePowerSources = (readByte & 0xF0) >> 4;
    // CurrentPowerSource / CurrentPowerSourceLevel
    readByte = parseUint8AndShift(&data, &len);
    currentPowerSource = readByte & 0x0F;
    currentPowerSourceLevel = (readByte & 0xF0) >> 4;
  }

  context = port->requestManager->removePending(port->requestManager, srcAddr,
      AZAP_ZDO_POWER_DESC_REQ, 0, 0, 0);
  RTL_TRDBG(TRACE_API, "<<< CALLBACK azapProcessZdoPowerDescRspCb (port:%d) (srcAddr:0x%.4x) "
      "(status:0x%.2x) (nwkAddr:0x%.4x) (currentPowerMode:0x%.2x) (availablePowerSources:0x%.2x) "
      "(currentPowerSource:0x%.2x) (currentPowerSourceLevel:0x%.2x)\n", port->portNumber, srcAddr,
      status, nwkAddr, currentPowerMode, availablePowerSources, currentPowerSource,
      currentPowerSourceLevel);
  port->pCallbacks->azapZdpPowerDescRspCb(port->portNumber,
      context ? context->issuerContext : NULL, srcAddr, status, nwkAddr,
      currentPowerMode, availablePowerSources, currentPowerSource, currentPowerSourceLevel);

  if (context)
  {
    context->free(context);
  }
}

/**
 * @brief Wrapping function for ZDP Simple_Desc_rsp messages callback
 * @param port the port context for the targeted hardware device
 * @param srcAddr the source address retrieved from ZDO_MSG_CB_INCOMING call back.
 * @param len length of the message payload
 * @param data the message payload
 */
void azapProcessZdoSimpleDescRspCb(
  portContext_t *port,
  azap_uint16 srcAddr,
  azap_uint8 len,
  azap_byte *data)
{
  /**
   * NOTE: According to the ZNP specification this call back should always has a payload
   * size between 0x06 and 0x4E... but in real the srcAddr first parameter is never provided.
   * Indeed looking at ZStack sources, one can see that this parameter is not expected in
   * "ZDO_ParseSimpleDescRsp" method. So we use 0x04-0x4C instead.
   */
  PROCESS_CALL_BACK_NOT_FOUND(port, azapZdpSimpleDescRspCb, 0x04, 0x4C);

  azap_uint8 status = 0;
  azap_uint16 nwkAddr = 0;
  azap_uint8 simpleDescLen = 0;
  azap_uint8 endPointId = 0;
  azap_uint16 profileId = 0;
  azap_uint16 deviceId = 0;
  azap_uint8 deviceVersion = 0;
  azap_uint8 numInClusters = 0;
  azap_uint16 *inClusterList = NULL;
  azap_uint8 numOutClusters = 0;
  azap_uint16 *outClusterList = NULL;
  requestContext_t *context = NULL;

  status = parseUint8AndShift(&data, &len);
  if (STATUS_CODE_SUCCESS == status)
  {
    nwkAddr = parseUint16AndShift(&data, &len);
    simpleDescLen = parseUint8AndShift(&data, &len);
    endPointId = parseUint8AndShift(&data, &len);
    profileId = parseUint16AndShift(&data, &len);
    deviceId = parseUint16AndShift(&data, &len);
    deviceVersion = parseUint8AndShift(&data, &len);
    numInClusters = parseUint8AndShift(&data, &len);
    inClusterList = parseUint16ArrayAndShift(&data, numInClusters, &len);
    numOutClusters = parseUint8AndShift(&data, &len);
    outClusterList = parseUint16ArrayAndShift(&data, numOutClusters, &len);
  }

  context = port->requestManager->removePending(port->requestManager, srcAddr,
      AZAP_ZDO_SIMPLE_DESC_REQ, 0, endPointId, 0);
  RTL_TRDBG(TRACE_API, "<<< CALLBACK azapProcessZdoSimpleDescRspCb (port:%d) (srcAddr:0x%.4x) "
      "(status:0x%.2x) (nwkAddr:0x%.4x) (simpleDescLen:0x%.2x) (endPointId:0x%.2x) "
      "(profileId:0x%.4x) (deviceId:0x%.4x) (deviceVersion:0x%.2x) (numInClusters:%d) "
      "(inClusterList:%s) (numOutClusters:%d) (outClusterList:%s)\n", port->portNumber,
      srcAddr, status, nwkAddr, simpleDescLen, endPointId, profileId, deviceId, deviceVersion,
      numInClusters, azap_uint16ArrayToString(inClusterList, numInClusters), numOutClusters,
      azap_uint16ArrayToString(outClusterList, numOutClusters));

  port->pCallbacks->azapZdpSimpleDescRspCb(port->portNumber,
      context ? context->issuerContext : NULL, srcAddr, status, nwkAddr,
      endPointId, profileId, deviceId, deviceVersion, numInClusters, inClusterList,
      numOutClusters, outClusterList);

  if (inClusterList)
  {
    free(inClusterList);
  }
  if (outClusterList)
  {
    free(outClusterList);
  }
  if (context)
  {
    context->free(context);
  }
}

/**
 * @brief Wrapping function for ZDP Active_EP_rsp messages callback
 * @param port the port context for the targeted hardware device
 * @param srcAddr the source address retrieved from ZDO_MSG_CB_INCOMING call back.
 * @param len length of the message payload
 * @param data the message payload
 */
void azapProcessZdoActiveEPRspCb(
  portContext_t *port,
  azap_uint16 srcAddr,
  azap_uint8 len,
  azap_byte *data)
{
  /**
   * NOTE: According to the ZNP specification this call back should always has a payload
   * size between 0x06 and 0x53... but in real the srcAddr first parameter is never provided.
   * Indeed looking at ZStack sources, one can see that this parameter is not expected in
   * "ZDO_ParseEPListRsp" method. So we use 0x04-0x51 instead.
   */
  PROCESS_CALL_BACK_NOT_FOUND(port, azapZdpActiveEPRspCb, 0x04, 0x51);

  azap_uint8 status = 0;
  azap_uint16 nwkAddr = 0;
  azap_uint8 activeEpCount = 0;
  azap_uint8 *activeEpList = NULL;
  requestContext_t *context = NULL;

  status = parseUint8AndShift(&data, &len);
  if (STATUS_CODE_SUCCESS == status)
  {
    nwkAddr = parseUint16AndShift(&data, &len);
    activeEpCount = parseUint8AndShift(&data, &len);
    activeEpList = parseUint8ArrayAndShift(&data, activeEpCount, &len);
  }

  context = port->requestManager->removePending(port->requestManager, srcAddr,
      AZAP_ZDO_ACTIVE_EP_REQ, 0, 0, 0);
  RTL_TRDBG(TRACE_API, "<<< CALLBACK azapProcessZdoActiveEPRspCb (port:%d) (srcAddr:0x%.4x) "
      "(status:0x%.2x) (nwkAddr:0x%.4x) (activeEpCount:0x%.2x) (activeEpList:%s)\n",
      port->portNumber, srcAddr, status, nwkAddr, activeEpCount,
      azap_uint8ArrayToString(activeEpList, activeEpCount));
  port->pCallbacks->azapZdpActiveEPRspCb(port->portNumber,
      context ? context->issuerContext : NULL, srcAddr, status, nwkAddr,
      activeEpCount, activeEpList);

  if (activeEpList)
  {
    free(activeEpList);
  }
  if (context)
  {
    context->free(context);
  }
}

/**
 * @brief Wrapping function for ZDP Bind_rsp messages callback
 * @param port the port context for the targeted hardware device
 * @param srcAddr the source address retrieved from ZDO_MSG_CB_INCOMING call back.
 * @param len length of the message payload
 * @param data the message payload
 */
void azapProcessZdoBindRspCb(
  portContext_t *port,
  azap_uint16 srcAddr,
  azap_uint8 len,
  azap_byte *data)
{
  /**
   * NOTE: According to the ZNP specification this call back should always has a payload
   * size of 0x03... but in real the srcAddr first parameter is never provided.
   * Indeed looking at ZStack sources, one can see that this parameter is not expected in
   * "ZDO_ParseBindRsp" method. So we use 0x01 instead.
   */
  PROCESS_CALL_BACK_NOT_FOUND(port, azapZdpBindRspCb, 0x01, 0x01);

  requestContext_t *context = NULL;
  azap_uint8 status = parseUint8AndShift(&data, &len);

  context = port->requestManager->removePending(port->requestManager, srcAddr,
      AZAP_ZDO_BIND_REQ, 0, 0, 0);
  RTL_TRDBG(TRACE_API, "<<< CALLBACK azapProcessZdoBindRspCb (port:%d) (srcAddr:0x%.4x) "
      "(status:0x%.2x)\n", port->portNumber, srcAddr, status);
  port->pCallbacks->azapZdpBindRspCb(port->portNumber,
      context ? context->issuerContext : NULL, srcAddr, status);

  if (context)
  {
    context->free(context);
  }
}

/**
 * @brief Wrapping function for ZDP Unbind_rsp messages callback
 * @param port the port context for the targeted hardware device
 * @param srcAddr the source address retrieved from ZDO_MSG_CB_INCOMING call back.
 * @param len length of the message payload
 * @param data the message payload
 */
void azapProcessZdoUnbindRspCb(
  portContext_t *port,
  azap_uint16 srcAddr,
  azap_uint8 len,
  azap_byte *data)
{
  /**
   * NOTE: According to the ZNP specification this call back should always has a payload
   * size of 0x03... but in real the srcAddr first parameter is never provided.
   * Indeed looking at ZStack sources, one can see that this parameter is not expected in
   * "ZDO_ParseBindRsp" method. So we use 0x01 instead.
   */
  PROCESS_CALL_BACK_NOT_FOUND(port, azapZdpUnbindRspCb, 0x01, 0x01);

  requestContext_t *context = NULL;
  azap_uint8 status = parseUint8AndShift(&data, &len);

  context = port->requestManager->removePending(port->requestManager, srcAddr,
      AZAP_ZDO_UNBIND_REQ, 0, 0, 0);
  RTL_TRDBG(TRACE_API, "<<< CALLBACK azapProcessZdoUnbindRspCb (port:%d) (srcAddr:0x%.4x) "
      "(status:0x%.2x)\n", port->portNumber, srcAddr, status);
  port->pCallbacks->azapZdpUnbindRspCb(port->portNumber,
      context ? context->issuerContext : NULL, srcAddr, status);

  if (context)
  {
    context->free(context);
  }
}

/**
 * @brief Wrapping function for ZDP Mgmt_Rtg_rsp messages callback
 * @param port the port context for the targeted hardware device
 * @param srcAddr the source address retrieved from ZDO_MSG_CB_INCOMING call back.
 * @param len length of the message payload
 * @param data the message payload
 */
void azapProcessZdoMgmtRtgRspCb(
  portContext_t *port,
  azap_uint16 srcAddr,
  azap_uint8 len,
  azap_byte *data)
{
  /**
   * NOTE: According to the ZNP specification this call back should always has a payload
   * size between 0x06 and 0x51... but in real the srcAddr first parameter is never
   * provided.
   * Indeed looking at ZStack sources, one can see that this parameter is not expected in
   * "ZDO_ParseMgmtRtgRsp" method. So we use 0x04-0x4F instead.
   */
  PROCESS_CALL_BACK_NOT_FOUND(port, azapZdpMgmtRtgRspCb, 0x04, 0x4F);

  azap_uint8 i;
  azap_uint8 status = 0;
  azap_uint8 routingTableEntries = 0;
  azap_uint8 startIndex = 0;
  azap_uint8 routingTableListCount = 0;
  zdoRtgEntry_t *routeTableList = NULL;
  requestContext_t *context = NULL;

  status = parseUint8AndShift(&data, &len);

  if (STATUS_CODE_SUCCESS == status)
  {
    routingTableEntries = parseUint8AndShift(&data, &len);
    startIndex = parseUint8AndShift(&data, &len);
    routingTableListCount = parseUint8AndShift(&data, &len);
    routeTableList = malloc(routingTableListCount * sizeof(zdoRtgEntry_t));
    for (i = 0; i < routingTableListCount ; i++)
    {
      routeTableList[i].dstAddress = parseUint16AndShift(&data, &len);
      routeTableList[i].status = parseUint8AndShift(&data, &len);
      routeTableList[i].nextHopAddress = parseUint16AndShift(&data, &len);
    }
  }

  context = port->requestManager->removePending(port->requestManager, srcAddr,
      AZAP_ZDO_MGMT_RTG_REQ, 0, 0, 0);
  RTL_TRDBG(TRACE_API, "<<< CALLBACK azapProcessZdoMgmtRtgRspCb (port:%d) (srcAddr:0x%.4x) "
      "(status:0x%.2x) (routingTableEntries:%d) (startIndex:%d) (routingTableListCount:%d)\n",
      port->portNumber, srcAddr, status, routingTableEntries, startIndex,
      routingTableListCount);
  port->pCallbacks->azapZdpMgmtRtgRspCb(port->portNumber,
      context ? context->issuerContext : NULL, srcAddr, status,
      routingTableEntries, startIndex, routingTableListCount, routeTableList);

  free(routeTableList);
  if (context)
  {
    context->free(context);
  }
}

/**
 * @brief Wrapping function for ZDP Mgmt_Bind_rsp messages callback
 * @param port the port context for the targeted hardware device
 * @param srcAddr the source address retrieved from ZDO_MSG_CB_INCOMING call back.
 * @param len length of the message payload
 * @param data the message payload
 */
void azapProcessZdoMgmtBindRspCb(
  portContext_t *port,
  azap_uint16 srcAddr,
  azap_uint8 len,
  azap_byte *data)
{
  /**
   * NOTE: According to the ZNP specification this call back should always has a payload
   * size between 0x06 and 0x51... but in real the srcAddr first parameter is never
   * provided.
   * Indeed looking at ZStack sources, one can see that this parameter is not expected in
   * "ZDO_ParseMgmtBindRsp" method. So we use 0x04-0x4F instead.
   */
  PROCESS_CALL_BACK_NOT_FOUND(port, azapZdpMgmtBindRspCb, 0x04, 0x4F);

  azap_uint8 i;
  azap_uint8 status = 0;
  azap_uint8 bindingTableEntries = 0;
  azap_uint8 startIndex = 0;
  azap_uint8 bindingTableListCount = 0;
  zdoBindEntry_t *bindingTableList = NULL;
  requestContext_t *context = NULL;

  status = parseUint8AndShift(&data, &len);

  if (STATUS_CODE_SUCCESS == status)
  {
    bindingTableEntries = parseUint8AndShift(&data, &len);
    startIndex = parseUint8AndShift(&data, &len);
    bindingTableListCount = parseUint8AndShift(&data, &len);
    bindingTableList = malloc(bindingTableListCount * sizeof(zdoBindEntry_t));
    for (i = 0; i < bindingTableListCount ; i++)
    {
      parseExtAddrAndShift(bindingTableList[i].srcAddr, &data, &len);
      bindingTableList[i].srcEndPoint = parseUint8AndShift(&data, &len);
      bindingTableList[i].clusterId = parseUint16AndShift(&data, &len);
      bindingTableList[i].dstAddr.addrMode = parseUint8AndShift(&data, &len);
      if (ZDO_RTG_ADDR_MODE_16BIT == bindingTableList[i].dstAddr.addrMode)
      {
        bindingTableList[i].dstAddr.addr.shortAddr = parseUint16AndShift(&data, &len);
      }
      else
      {
        parseExtAddrAndShift(bindingTableList[i].dstAddr.addr.extAddr, &data, &len);
        bindingTableList[i].dstEndPoint = parseUint8AndShift(&data, &len);
      }
    }
  }

  context = port->requestManager->removePending(port->requestManager, srcAddr,
      AZAP_ZDO_MGMT_BIND_REQ, 0, 0, 0);
  RTL_TRDBG(TRACE_API, "<<< CALLBACK azapProcessZdoMgmtBindRspCb (port:%d) (srcAddr:0x%.4x) "
      "(status:0x%.2x) (bindingTableEntries:%d) (startIndex:%d) (bindingTableListCount:%d)\n",
      port->portNumber, srcAddr, status, bindingTableEntries, startIndex,
      bindingTableListCount);
  port->pCallbacks->azapZdpMgmtBindRspCb(port->portNumber,
      context ? context->issuerContext : NULL, srcAddr, status,
      bindingTableEntries, startIndex, bindingTableListCount, bindingTableList);

  if (context)
  {
    context->free(context);
  }
}

/**
 * @brief Wrapping function for ZDP Mgmt_Permit_Join_rsp messages callback
 * @param port the port context for the targeted hardware device
 * @param srcAddr the source address retrieved from ZDO_MSG_CB_INCOMING call back.
 * @param len length of the message payload
 * @param data the message payload
 */
void azapProcessZdoMgmtPermitJoinRspCb(
  portContext_t *port,
  azap_uint16 srcAddr,
  azap_uint8 len,
  azap_byte *data)
{
  PROCESS_CALL_BACK_NOT_FOUND(port, azapZdpMgmtPermitJoinRspCb, 0x01, 0x01);

  azap_uint8 status = 0;
  requestContext_t *context = NULL;

  status = parseUint8AndShift(&data, &len);

  context = port->requestManager->removePending(port->requestManager, srcAddr,
      AZAP_ZDO_MGMT_PERMIT_JOIN_REQ, 0, 0, 0);
  RTL_TRDBG(TRACE_API, "<<< CALLBACK azapProcessZdoMgmtPermitJoinRspCb (port:%d) "
      "(srcAddr:0x%.4x) (status:0x%.2x)\n", port->portNumber, srcAddr, status);
  port->pCallbacks->azapZdpMgmtPermitJoinRspCb(port->portNumber,
      context ? context->issuerContext : NULL, srcAddr, status);

  if (context)
  {
    context->free(context);
  }
}

/**
 * @brief Wrapping function for ZDP Mgmt_Permit_Join_rsp messages callback
 * @param port the port context for the targeted hardware device
 * @param srcAddr the source address retrieved from ZDO_MSG_CB_INCOMING call back.
 * @param len length of the message payload
 * @param data the message payload
 */
void azapProcessZdoMgmtLeaveRspCb(
  portContext_t *port,
  azap_uint16 srcAddr,
  azap_uint8 len,
  azap_byte *data)
{
  PROCESS_CALL_BACK_NOT_FOUND(port, azapZdpMgmtLeaveRspCb, 0x01, 0x01);

  azap_uint8 status = 0;
  requestContext_t *context = NULL;

  status = parseUint8AndShift(&data, &len);

  context = port->requestManager->removePending(port->requestManager, srcAddr,
      AZAP_ZDO_MGMT_LEAVE_REQ, 0, 0, 0);
  RTL_TRDBG(TRACE_API, "<<< CALLBACK azapProcessZdoMgmtLeaveRspCb (port:%d) "
      "(srcAddr:0x%.4x) (status:0x%.2x)\n", port->portNumber, srcAddr, status);
  port->pCallbacks->azapZdpMgmtLeaveRspCb(port->portNumber,
      context ? context->issuerContext : NULL, srcAddr, status);

  if (context)
  {
    context->free(context);
  }
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
 * @brief Wrapping function for ZCL "Read" command messages callback
 * @param port the port context for the targeted hardware device
 * @param srcAddr the short address of the node that send the response.
 * @param srcEndPointId the end point identifier on the remote node that send the response.
 * @param clusterId the cluster identifier that the command targets.
 * @param zclHdr the parsed ZCL header associated to this command
 * @param len length of the message payload
 * @param data the message payload
 */
void azapProcessZclReadCmdRspCb(
  portContext_t *port,
  azap_uint16 srcAddr,
  azap_uint8 srcEndPointId,
  azap_uint16 clusterId,
  azapZclFrameHeader_t *zclHdr,
  azap_uint8 len,
  azap_byte *data)
{
  PROCESS_CALL_BACK_NOT_FOUND(port, azapZclReadCmdRspCb, len, len);

  requestContext_t *context = NULL;
  azapZclReadRspCommand_t *parsedData = parseZCLReadCmdRsp(len, data);

  context = port->requestManager->removePending(port->requestManager, srcAddr,
      AZAP_ZCL_READ_CMD, clusterId, srcEndPointId, zclHdr->commandId);
  RTL_TRDBG(TRACE_API, "<<< CALLBACK azapProcessZclReadCmdRspCb (port:%d) "
      "(srcAddr:0x%.4x)\n", port->portNumber, srcAddr);
  port->pCallbacks->azapZclReadCmdRspCb(port->portNumber,
      context ? context->issuerContext : NULL, srcAddr, srcEndPointId,
      clusterId, zclHdr->fc.direction, zclHdr->manuCode, parsedData);

  free(parsedData);
  if (context)
  {
    context->free(context);
  }
}

/**
 * @brief Wrapping function for ZCL "Write" command messages call back.
 * @param port the port context for the targeted hardware device
 * @param srcAddr the short address of the node that send the response.
 * @param srcEndPointId the end point identifier on the remote node that send the response.
 * @param clusterId the cluster identifier that the command targets.
 * @param zclHdr the parsed ZCL header associated to this command
 * @param len length of the message payload
 * @param data the message payload
 */
void azapProcessZclWriteCmdRspCb(
  portContext_t *port,
  azap_uint16 srcAddr,
  azap_uint8 srcEndPointId,
  azap_uint16 clusterId,
  azapZclFrameHeader_t *zclHdr,
  azap_uint8 len,
  azap_byte *data)
{
  PROCESS_CALL_BACK_NOT_FOUND(port, azapZclWriteCmdRspCb, len, len);

  requestContext_t *context = NULL;
  azapZclWriteRspCommand_t *parsedData = parseZCLWriteCmdRsp(len, data);

  context = port->requestManager->removePending(port->requestManager, srcAddr,
      AZAP_ZCL_WRITE_CMD, clusterId, srcEndPointId, zclHdr->commandId);
  RTL_TRDBG(TRACE_API, "<<< CALLBACK azapProcessZclWriteCmdRspCb (port:%d) "
      "(srcAddr:0x%.4x)\n", port->portNumber, srcAddr);
  port->pCallbacks->azapZclWriteCmdRspCb(port->portNumber,
      context ? context->issuerContext : NULL, srcAddr, srcEndPointId,
      clusterId, zclHdr->fc.direction, zclHdr->manuCode, parsedData);

  free(parsedData);
  if (context)
  {
    context->free(context);
  }
}

/**
 * @brief Wrapping function for ZCL "Configure Reporting" command messages callback
 * @param port the port context for the targeted hardware device
 * @param srcAddr the short address of the node that send the response.
 * @param srcEndPointId the end point identifier on the remote node that send the response.
 * @param clusterId the cluster identifier that the command targets.
 * @param zclHdr the parsed ZCL header associated to this command
 * @param len length of the message payload
 * @param data the message payload
 */
void azapProcessZclConfigReportCmdRspCb(
  portContext_t *port,
  azap_uint16 srcAddr,
  azap_uint8 srcEndPointId,
  azap_uint16 clusterId,
  azapZclFrameHeader_t *zclHdr,
  azap_uint8 len,
  azap_byte *data)
{
  PROCESS_CALL_BACK_NOT_FOUND(port, azapZclConfigureReportCmdRspCb, len, len);

  requestContext_t *context = NULL;
  azapZclCfgReportRspCommand_t *parsedData = parseZCLConfigureReportCmdRsp(len, data);

  context = port->requestManager->removePending(port->requestManager, srcAddr,
      AZAP_ZCL_CONFIGURE_REPORTING_CMD, clusterId, srcEndPointId, zclHdr->commandId);
  RTL_TRDBG(TRACE_API, "<<< CALLBACK azapProcessZclConfigReportCmdRspCb (port:%d) "
      "(srcAddr:0x%.4x)\n", port->portNumber, srcAddr);
  port->pCallbacks->azapZclConfigureReportCmdRspCb(port->portNumber,
      context ? context->issuerContext : NULL, srcAddr,
      srcEndPointId, clusterId, parsedData);

  free(parsedData);
  if (context)
  {
    context->free(context);
  }
}

/**
 * @brief Wrapping function for ZCL "Report Attributes" command messages callback
 * @param port the port context for the targeted hardware device
 * @param srcAddr the short address of the node that send the response.
 * @param srcEndPointId the end point identifier on the remote node that send the response.
 * @param clusterId the cluster identifier that the command targets.
 * @param zclHdr the parsed ZCL header associated to this command
 * @param len length of the message payload
 * @param data the message payload
 */
void azapProcessZclReportCmdCb(
  portContext_t *port,
  azap_uint16 srcAddr,
  azap_uint8 srcEndPointId,
  azap_uint16 clusterId,
  azapZclFrameHeader_t *zclHdr,
  azap_uint8 len,
  azap_byte *data)
{
  PROCESS_CALL_BACK_NOT_FOUND(port, azapZclReportCmdCb, len, len);

  azapZclReportCommand_t *parsedData = parseZCLReportCmd(len, data);

  RTL_TRDBG(TRACE_API, "<<< CALLBACK azapProcessZclReportCmdCb (port:%d) "
      "(srcAddr:0x%.4x)\n", port->portNumber, srcAddr);
  port->pCallbacks->azapZclReportCmdCb(port->portNumber, srcAddr, srcEndPointId,
      clusterId, parsedData);

  free(parsedData);
}

/**
 * @brief Wrapping function for ZCL "Read Report Config" command messages callback
 * @param port the port context for the targeted hardware device
 * @param srcAddr the short address of the node that send the response.
 * @param srcEndPointId the end point identifier on the remote node that send the response.
 * @param clusterId the cluster identifier that the command targets.
 * @param zclHdr the parsed ZCL header associated to this command
 * @param len length of the message payload
 * @param data the message payload
 */
void azapProcessZclReadReportConfigCmdRspCb(
  portContext_t *port,
  azap_uint16 srcAddr,
  azap_uint8 srcEndPointId,
  azap_uint16 clusterId,
  azapZclFrameHeader_t *zclHdr,
  azap_uint8 len,
  azap_byte *data)
{
  PROCESS_CALL_BACK_NOT_FOUND(port, azapZclReadReportConfigCmdRspCb, len, len);

  requestContext_t *context = NULL;
  azapZclReadReportCfgRspCommand_t *parsedData = parseZCLReadReportConfigCmdRsp(len, data);

  context = port->requestManager->removePending(port->requestManager, srcAddr,
      AZAP_ZCL_READ_REPORTING_CONF_CMD, clusterId, srcEndPointId, zclHdr->commandId);
  RTL_TRDBG(TRACE_API, "<<< CALLBACK azapProcessZclReadReportConfigCmdRspCb (port:%d) "
      "(srcAddr:0x%.4x)\n", port->portNumber, srcAddr);
  port->pCallbacks->azapZclReadReportConfigCmdRspCb(port->portNumber,
      context ? context->issuerContext : NULL, srcAddr,
      srcEndPointId, clusterId, parsedData);

  free(parsedData);
  if (context)
  {
    context->free(context);
  }
}


/**
 * @brief Wrapping function for ZCL "Discover" command messages call back
 * @param port the port context for the targeted hardware device
 * @param srcAddr the short address of the node that send the response.
 * @param srcEndPointId the end point identifier on the remote node that send the response.
 * @param clusterId the cluster identifier that the command targets.
 * @param zclHdr the parsed ZCL header associated to this command
 * @param len length of the message payload
 * @param data the message payload
 */
void azapProcessZclDiscoverCmdRspCb(
  portContext_t *port,
  azap_uint16 srcAddr,
  azap_uint8 srcEndPointId,
  azap_uint16 clusterId,
  azapZclFrameHeader_t *zclHdr,
  azap_uint8 len,
  azap_byte *data)
{
  PROCESS_CALL_BACK_NOT_FOUND(port, azapZclDiscoverCmdRspCb, len, len);

  requestContext_t *context = NULL;
  azapZclDiscoverRspCommand_t *parsedData = parseZCLDiscoverRspCmd(len, data);

  context = port->requestManager->removePending(port->requestManager, srcAddr,
      AZAP_ZCL_DISCOVER_CMD, clusterId, srcEndPointId, zclHdr->commandId);
  RTL_TRDBG(TRACE_API, "<<< CALLBACK azapProcessZclDiscoverCmdRspCb (port:%d) "
      "(srcAddr:0x%.4x)\n", port->portNumber, srcAddr);
  port->pCallbacks->azapZclDiscoverCmdRspCb(port->portNumber,
      context ? context->issuerContext : NULL, srcAddr,
      srcEndPointId, clusterId, zclHdr->fc.direction, parsedData);

  free(parsedData);
  if (context)
  {
    context->free(context);
  }
}


/**
 * @brief Wrapping function for ZCL Configure reporting command messages call back
 * @param port the port context for the targeted hardware device
 * @param srcAddr the short address of the node that send the request.
 * @param srcEndPointId the end point identifier on the remote node that send the request.
 * @param clusterId the cluster identifier that the command targets.
 * @param zclHdr the parsed ZCL header associated to this command
 * @param len length of the message payload
 * @param data the message payload
 */
void azapProcessZclConfigReportCmdCb(
  portContext_t *port,
  azap_uint16 srcAddr,
  azap_uint8 srcEndPointId,
  azap_uint16 clusterId,
  azapZclFrameHeader_t *zclHdr,
  azap_uint8 len,
  azap_byte *data)
{
  PROCESS_CALL_BACK_NOT_FOUND(port, azapZclConfigureReportCmdCb, len, len);

  azapZclCfgReportCommand_t *parsedData = parseZCLConfigureReportCmd(len, data);

  RTL_TRDBG(TRACE_API, "<<< CALLBACK azapProcessZclConfigReportCmdCb (port:%d) "
      "(srcAddr:0x%.4x)\n", port->portNumber, srcAddr);
  port->pCallbacks->azapZclConfigureReportCmdCb(port->portNumber, srcAddr,
      srcEndPointId, clusterId, parsedData);

  free(parsedData);
}

/**
 * @brief Wrapping function for ZCL command (generic) messages call back
 * @param port the port context for the targeted hardware device
 * @param srcAddr the short address of the node that send the command.
 * @param srcEndPointId the end point identifier on the remote node that send the command.
 * @param clusterId the cluster identifier that the command targets.
 * @param zclHdr the parsed ZCL header associated to this command
 * @param len length of the message payload
 * @param data the message payload
 */
void azapProcessZclIncomingCmdCb(
  portContext_t *port,
  azap_uint16 srcAddr,
  azap_uint8 srcEndPointId,
  azap_uint16 clusterId,
  azapZclFrameHeader_t *zclHdr,
  azap_uint8 len,
  azap_byte *data)
{
  PROCESS_CALL_BACK_NOT_FOUND(port, azapZclIncomingCmdCb, len, len);

  requestContext_t *context = NULL;

  context = port->requestManager->removePending(port->requestManager, srcAddr,
      AZAP_ZCL_OUTGOING_CMD, clusterId, srcEndPointId, zclHdr->commandId);
  RTL_TRDBG(TRACE_API, "<<< CALLBACK azapProcessZclIncomingCmdCb (port:%d) "
      "(srcAddr:0x%.4x)\n", port->portNumber, srcAddr);
  port->pCallbacks->azapZclIncomingCmdCb(port->portNumber,
      context ? context->issuerContext : NULL, srcAddr, srcEndPointId, clusterId,
      zclHdr->commandId, data, len);

  if (context)
  {
    context->free(context);
  }
}


