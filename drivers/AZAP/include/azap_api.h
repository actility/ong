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
//** File : ./include/azap_api.h
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

/**
 * @file azap_api.h
 * @brief The file brings the definition of API functions and call backs for AZAP library.
 * @author mlouiset
 * @date 2012-07-05
 */


#ifndef _ACTILITY_ZAP_API__H_
#define _ACTILITY_ZAP_API__H_


/*
 * INCLUDES
 */

#include "azap_types.h"
#include "azap_api_common.h"
#include "azap_api_af.h"
#include "azap_api_zcl.h"
#include "azap_api_zdo.h"

/*****************************************************************************/
/*   ___       __ _      _ _   _                                             */
/*  |   \ ___ / _(_)_ _ (_) |_(_)___ _ _  ___                                */
/*  | |) / -_)  _| | ' \| |  _| / _ \ ' \(_-<                                */
/*  |___/\___|_| |_|_||_|_|\__|_\___/_||_/__/                                */
/*                                                                           */
/*****************************************************************************/

/*****************************************************************************/
/* The various call back prototypes definitions */

typedef void (*pf_func_azapStartedCb) (
  azap_uint8 port,
  bool success,
  azap_uint16 shortAddr,
  azap_uint8 deviceIEEE[IEEE_ADDR_LEN],
  azap_uint16 panId,
  azap_uint8 panIdExt[IEEE_ADDR_LEN]);

typedef void (*pf_func_azapZdpBindReqCb) (
  azap_uint8 port,
  void *issuerContext,
  azap_uint16 nwkAddrSrc,
  azap_uint8 srcIeeeAddr[IEEE_ADDR_LEN],
  azap_uint8 srcEndPoint,
  azap_uint16 clusterId,
  azap_uint8 destIeeeAddr[IEEE_ADDR_LEN],
  azap_uint8 destEndPoint);
/**
 * Call back for ZDO UnbindReq notification.
 * @param port the reference port number
 */
typedef void (*pf_func_azapZdpUnbindReqCb)(
  azap_uint8 port,
  void *issuerContext,
  azap_uint16 nwkAddrSrc,
  azap_uint8 srcIeeeAddr[IEEE_ADDR_LEN],
  azap_uint8 srcEndPoint,
  azap_uint16 clusterId,
  azap_uint8 destIeeeAddr[IEEE_ADDR_LEN],
  azap_uint8 destEndPoint);

/* ZDO response messages call backs */
/**
 * Call back for ZDO DeviceAnnce notification.
 * @param port the reference port number
 * @param nwkAddr the short address of the node that emitted the DeviceAnnce
 * @param ieeeAddr the IEEE address of the node that emitted the DeviceAnnce
 * @param alternatePanCoordinator set to 1 if this node is capable of becoming a PAN
 * coordinator.
 * @param deviceType set to 1 if this node is a full function device (FFD).
 * Otherwise, the device type sub-field shall be set to 0, indicating a reduced
 * function device (RFD).
 * @param powerSource set to 1 if the current power source is mains power.
 * Otherwise, the power source sub-field shall be set to 0. This information
 * is derived from the node current power source field of the node power descriptor.
 * @param receiverOnWhenIdle set to 1 if the device does not disable its receiver
 * to conserve power during idle periods. Otherwise, the receiver on when idle
 * sub-field shall be set to 0.
 * @param securityCapability set to 1 if the device is capable of sending and
 * receiving frames secured. Otherwise, the security capability sub-field shall
 * be set to 0.
 * @param allocateAddress Indicates whether the device will allocate short
 * (network) addresses or not.
 */
typedef void (*pf_func_azapZdpDeviceAnnceCb)(
  azap_uint8 port,
  azap_uint16 nwkAddr,
  azap_uint8 ieeeAddr[IEEE_ADDR_LEN],
  azap_uint8 alternatePanCoordinator,
  azap_uint8 deviceType,
  azap_uint8 powerSource,
  azap_uint8 receiverOnWhenIdle,
  azap_uint8 securityCapability,
  azap_uint8 allocateAddress);
/**
 * Call back for ZDO NwkAddrRsp notification.
 * @param port the reference port number
 */
typedef void (*pf_func_azapZdpNwkAddrRspCb) (
  azap_uint8 port,
  void *issuerContext,
  azap_uint16 nwkAddrSrc,
  azap_uint8 status,
  azap_uint8 ieeeAddrRemoteDev[IEEE_ADDR_LEN],
  azap_uint16 nwkAddrRemoteDev,
  azap_uint8 numAssocDev,
  azap_uint8 startIndex,
  azap_uint16 nwkAddrAssocDevList[]);
/**
 * Call back for ZDO IeeeAddrRsp notification.
 * @param port the reference port number
 */
typedef void (*pf_func_azapZdpIeeeAddrRspCb) (
  azap_uint8 port,
  void *issuerContext,
  azap_uint16 nwkAddrSrc,
  azap_uint8 status,
  azap_uint8 ieeeAddrRemoteDev[IEEE_ADDR_LEN],
  azap_uint16 nwkAddrRemoteDev,
  azap_uint8 numAssocDev,
  azap_uint8 startIndex,
  azap_uint16 nwkAddrAssocDevList[]);
/**
 * Call back for ZDO NodeDescRsp notification.
 * @param port the reference port number
 */
typedef void (*pf_func_azapZdpNodeDescRspCb) (
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
  azap_uint8 descriptorCapabilityField);
/**
 * Call back for ZDO PowerDescRsp notification.
 * @param port the reference port number
 */
typedef void (*pf_func_azapZdpPowerDescRspCb) (
  azap_uint8 port,
  void *issuerContext,
  azap_uint16 nwkAddrSrc,
  azap_uint8 status,
  azap_uint16 nwkAddrOfInterest,
  azap_uint8 currentPowerMode,
  azap_uint8 availablePowerSources,
  azap_uint8 currentPowerSource,
  azap_uint8 currentPowerSourceLevel);
/**
 * Call back for ZDO SimpleDescRsp notification.
 * @param port the reference port number
 */
typedef void (*pf_func_azapZdpSimpleDescRspCb) (
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
  azap_uint16 applicationOutputClusterList[]);
/**
 * Call back for ZDO ActiveEPRsp notification.
 * @param port the reference port number
 */
typedef void (*pf_func_azapZdpActiveEPRspCb) (
  azap_uint8 port,
  void *issuerContext,
  azap_uint16 nwkAddrSrc,
  azap_uint8 status,
  azap_uint16 nwkAddrOfInterest,
  azap_uint8 activeEPCount,
  azap_uint8 activeEPList[]);
/**
 * Call back for ZDO BindRsp notification.
 * @param port the reference port number
 */
typedef void (*pf_func_azapZdpBindRspCb) (
  azap_uint8 port,
  void *issuerContext,
  azap_uint16 nwkAddrSrc,
  azap_uint8 status);
/**
 * Call back for ZDO UnbindRsp notification.
 * @param port the reference port number
 */
typedef void (*pf_func_azapZdpUnbindRspCb) (
  azap_uint8 port,
  void *issuerContext,
  azap_uint16 nwkAddrSrc,
  azap_uint8 status);
/**
 * Call back for ZDO MgmtRtgRsp notification.
 * @param port the reference port number
 */
typedef void (*pf_func_azapZdpMgmtRtgRspCb) (
  azap_uint8 port,
  void *issuerContext,
  azap_uint16 nwkAddrSrc,
  azap_uint8 status,
  azap_uint8 routingTableEntries,
  azap_uint8 startIndex,
  azap_uint8 routingTableListCount,
  zdoRtgEntry_t routeTableList[]);
/**
 * Call back for ZDO MgmtBindRsp notification.
 * @param port the reference port number
 */
typedef void (*pf_func_azapZdpMgmtBindRspCb) (
  azap_uint8 port,
  void *issuerContext,
  azap_uint16 nwkAddrSrc,
  azap_uint8 status,
  azap_uint8 bindingTableEntries,
  azap_uint8 startIndex,
  azap_uint8 bindingTableListCount,
  zdoBindEntry_t bindingTableList[]);
/**
 * Call back for ZDO MgmtPermitJoinRsp notification.
 * @param port the reference port number
 */
typedef void (*pf_func_azapZdpMgmtPermitJoinRspCb) (
  azap_uint8 port,
  void *issuerContext,
  azap_uint16 nwkAddrSrc,
  azap_uint8 status);
/**
 * Call back for ZDO MgmtLeaveRsp notification.
 * @param port the reference port number
 */
typedef void (*pf_func_azapZdpMgmtLeaveRspCb) (
  azap_uint8 port,
  void *issuerContext,
  azap_uint16 nwkAddrSrc,
  azap_uint8 status);

/* SAPI call backs */
/**
 * Call back for SAPI FindDeviceConf notification.
 * @param port the reference port number
 */
typedef void (*pf_func_azapSapiFindDeviceConfCb) (
  azap_uint8 port,
  azap_uint16 nwkAddr,
  azap_uint8 ieeeAddr[IEEE_ADDR_LEN]);

/* ZCL call backs */
/**
 * Call back for ZCL Read cmd Rsp notification.
 * @param port the reference port number
 */
typedef void (*pf_func_azapZclReadCmdRspCb) (
  azap_uint8 port,
  void *issuerContext,
  azap_uint16 nwkAddrSrc,
  azap_uint8 endPoint,
  azap_uint16 clusterId,
  azap_uint8 clusterDir,
  azap_uint16 manufacturerCode,
  azapZclReadRspCommand_t *data);
/**
 * Call back for ZCL Write cmd response notification.
 * @param port the reference port number
 */
typedef void (*pf_func_azapZclWriteCmdRspCb) (
  azap_uint8 port,
  void *issuerContext,
  azap_uint16 nwkAddrSrc,
  azap_uint8 endPoint,
  azap_uint16 clusterId,
  azap_uint8 clusterDir,
  azap_uint16 manufacturerCode,
  azapZclWriteRspCommand_t *data);
/**
 * Call back for ZCL ConfigureReporting cmd response notification.
 * @param port the reference port number
 */
typedef void (*pf_func_azapZclConfigureReportCmdRspCb) (
  azap_uint8 port,
  void *issuerContext,
  azap_uint16 nwkAddrSrc,
  azap_uint8 endPoint,
  azap_uint16 clusterId,
  azapZclCfgReportRspCommand_t *data);
/**
 * Call back for ZCL Report cmd notification.
 * @param port the reference port number
 */
typedef void (*pf_func_azapZclReportCmdCb) (
  azap_uint8 port,
  azap_uint16 nwkAddrSrc,
  azap_uint8 endPoint,
  azap_uint16 clusterId,
  azapZclReportCommand_t *data);
/**
 * Call back for ZCL Read Reporting Config cmd response notification.
 * @param port the reference port number
 */
typedef void (*pf_func_azapZclReadReportConfigCmdRspCb) (
  azap_uint8 port,
  void *issuerContext,
  azap_uint16 nwkAddrSrc,
  azap_uint8 endPoint,
  azap_uint16 clusterId,
  azapZclReadReportCfgRspCommand_t *data);
/**
 * Call back for ZCL Discover cmd response notification.
 * @param port the reference port number
 */
typedef void (*pf_func_azapZclDiscoverCmdRspCb) (
  azap_uint8 port,
  void *issuerContext,
  azap_uint16 nwkAddrSrc,
  azap_uint8 endPoint,
  azap_uint16 clusterId,
  azap_uint8 clusterDir,
  azapZclDiscoverRspCommand_t *data);
/**
 * Call back for ZCL ConfigureReporting cmd request notification.
 * @param port the reference port number
 */
typedef void (*pf_func_azapZclConfigureReportCmdCb) (
  azap_uint8 port,
  azap_uint16 nwkAddrDest,
  azap_uint8 endPoint,
  azap_uint16 clusterId,
  azapZclCfgReportCommand_t *data);

/**
 * Call back for ZCL generic cmd response notification.
 * @param port the reference port number
 */
typedef void (*pf_func_azapZclIncomingCmdCb) (
  azap_uint8 port,
  void *issuerContext,
  azap_uint16 nwkAddrSrc,
  azap_uint8 endPoint,
  azap_uint16 clusterId,
  azap_uint8 cmdCode,
  azap_byte cmdData[],
  azap_uint8 cmdLen);




/**
 * @struct t_azap_api_callbacks
 * Defines the set of call backs the AZAP lib will call for ZigBee operation completion.
 */
typedef struct t_azap_api_callbacks
{

  /**
   * Deallocate the resources of the structure, and the structure itself.
   * @param This the t_azap_api_callbacks instance to free
   */
  void (*free) (struct t_azap_api_callbacks *This);

  /* Driver invocations */
  pf_func_azapStartedCb azapStartedCb;
  pf_func_azapZdpBindReqCb azapZdpBindReqCb;
  pf_func_azapZdpUnbindReqCb azapZdpUnbindReqCb;
  pf_func_azapZdpDeviceAnnceCb azapZdpDeviceAnnceCb;
  pf_func_azapZdpNwkAddrRspCb azapZdpNwkAddrRspCb;
  pf_func_azapZdpIeeeAddrRspCb azapZdpIeeeAddrRspCb;
  pf_func_azapZdpNodeDescRspCb azapZdpNodeDescRspCb;
  pf_func_azapZdpPowerDescRspCb azapZdpPowerDescRspCb;
  pf_func_azapZdpSimpleDescRspCb azapZdpSimpleDescRspCb;
  pf_func_azapZdpActiveEPRspCb azapZdpActiveEPRspCb;
  pf_func_azapZdpBindRspCb azapZdpBindRspCb;
  pf_func_azapZdpUnbindRspCb azapZdpUnbindRspCb;
  pf_func_azapZdpMgmtRtgRspCb azapZdpMgmtRtgRspCb;
  pf_func_azapZdpMgmtBindRspCb azapZdpMgmtBindRspCb;
  pf_func_azapZdpMgmtPermitJoinRspCb azapZdpMgmtPermitJoinRspCb;
  pf_func_azapZdpMgmtLeaveRspCb azapZdpMgmtLeaveRspCb;
  pf_func_azapSapiFindDeviceConfCb azapSapiFindDeviceConfCb;
  pf_func_azapZclReadCmdRspCb azapZclReadCmdRspCb;
  pf_func_azapZclWriteCmdRspCb azapZclWriteCmdRspCb;
  pf_func_azapZclConfigureReportCmdRspCb azapZclConfigureReportCmdRspCb;
  pf_func_azapZclReportCmdCb azapZclReportCmdCb;
  pf_func_azapZclDiscoverCmdRspCb azapZclDiscoverCmdRspCb;
  pf_func_azapZclIncomingCmdCb azapZclIncomingCmdCb;
  pf_func_azapZclConfigureReportCmdCb azapZclConfigureReportCmdCb;
  pf_func_azapZclReadReportConfigCmdRspCb azapZclReadReportConfigCmdRspCb;

} t_azap_api_callbacks;

/* static allocation */
t_azap_api_callbacks t_azap_api_callbacks_create();

/* dynamic allocation */
t_azap_api_callbacks *new_t_azap_api_callbacks();


/*****************************************************************************/
/*     _   ___ ___                _       _                                  */
/*    /_\ | _ \_ _|  _ __ _ _ ___| |_ ___| |_ _  _ _ __  ___ ___             */
/*   / _ \|  _/| |  | '_ \ '_/ _ \  _/ _ \  _| || | '_ \/ -_|_-<             */
/*  /_/ \_\_| |___| | .__/_| \___/\__\___/\__|\_, | .__/\___/__/             */
/*                  |_|                       |__/|_|                        */
/*****************************************************************************/


/*****************************************************************************/

/**
 * @brief Get the version string of AZAP library.
 * @return the version string.
 */
char *azapVersion();
char *zStackVersion(azap_uint8 port);

void azapClockMs();
void azapClockSc(time_t now);

/* C API prototypes */
bool azapInit(
  azap_uint8 *targetedIeee,
  azap_uint8 *port,
  void *mainTbPoll,
  t_azap_api_callbacks *pCallbacks,
  azap_uint8 defaultChannel,
  azap_uint8 deviceType,
  bool resetHanAtReboot);
bool azapStart(azap_uint8 port);
bool azapStop(azap_uint8 port);
void azapDumpStatistics(char *out);

bool azapAddEndPoint(
  azap_uint8 port,
  azap_uint8 endPoint,
  azap_uint16 appProfId,
  azap_uint16 appDeviceId,
  azap_uint8 appDevVer,
  azap_uint8 appNumInClusters,
  azap_uint16 appInClusterList[],
  azap_uint8 appNumOutClusters,
  azap_uint16 appOutClusterList[]);
bool azapAddZCLAttribute(
  azap_uint8 port,
  azap_uint8 endPoint,
  azap_uint16 clusterId,
  azap_uint16 attributeId,
  azap_uint8 dataType,
  azap_uint8 accessControl,
  azap_byte *data,
  azap_uint16 dataLen);
bool azapChangeZCLAttributeValue(
  azap_uint8 port,
  azap_uint8 endPoint,
  azap_uint16 clusterId,
  azap_uint16 attributeId,
  azap_uint8 dataType,
  azap_byte *data,
  azap_uint16 dataLen);

/* ZAP invocations */
bool azapZdpNwkAddrReq(
  azap_uint8 port,
  AZAP_REQ_TTL_MGMT_MODE ttlMode,
  void *issuerContext,
  azap_uint8 ieeeAddr[IEEE_ADDR_LEN],
  azap_uint8 requestType,
  azap_uint8 startIndex);
bool azapZdpIeeeAddrReq(
  azap_uint8 port,
  AZAP_REQ_TTL_MGMT_MODE ttlMode,
  void *issuerContext,
  azap_uint16 nwkAddrDest,
  azap_uint16 nwkAddrOfInterest,
  azap_uint8 requestType,
  azap_uint8 startIndex);
bool azapZdpNodeDescReq(
  azap_uint8 port,
  AZAP_REQ_TTL_MGMT_MODE ttlMode,
  void *issuerContext,
  azap_uint16 nwkAddrDest,
  azap_uint16 nwkAddrOfInterest);
bool azapZdpPowerDescReq(
  azap_uint8 port,
  AZAP_REQ_TTL_MGMT_MODE ttlMode,
  void *issuerContext,
  azap_uint16 nwkAddrDest,
  azap_uint16 nwkAddrOfInterest);
bool azapZdpActiveEPReq(
  azap_uint8 port,
  AZAP_REQ_TTL_MGMT_MODE ttlMode,
  void *issuerContext,
  azap_uint16 nwkAddrDest,
  azap_uint16 nwkAddrOfInterest);
bool azapZdpSimpleDescReq(
  azap_uint8 port,
  AZAP_REQ_TTL_MGMT_MODE ttlMode,
  void *issuerContext,
  azap_uint16 nwkAddrDest,
  azap_uint16 nwkAddrOfInterest,
  azap_uint8 endpoint);
bool azapZdpBindReq(
  azap_uint8 port,
  AZAP_REQ_TTL_MGMT_MODE ttlMode,
  void *issuerContext,
  azap_uint16 nwkAddrDest,
  azap_uint8 srcAddress[IEEE_ADDR_LEN],
  azap_uint8 srcEndp,
  azap_uint16 clusterId,
  azap_uint8 dstAddress[IEEE_ADDR_LEN],
  azap_uint8 dstEndp);
bool azapZdpUnbindReq(
  azap_uint8 port,
  AZAP_REQ_TTL_MGMT_MODE ttlMode,
  void *issuerContext,
  azap_uint16 nwkAddrDest,
  azap_uint8 srcAddress[IEEE_ADDR_LEN],
  azap_uint8 srcEndp,
  azap_uint16 clusterId,
  azap_uint8 dstAddress[IEEE_ADDR_LEN],
  azap_uint8 dstEndp);
bool azapZdpMgmtRtgReq(
  azap_uint8 port,
  AZAP_REQ_TTL_MGMT_MODE ttlMode,
  void *issuerContext,
  azap_uint16 nwkAddrDest,
  azap_uint8 startIndex);
bool azapZdpMgmtBindReq(
  azap_uint8 port,
  AZAP_REQ_TTL_MGMT_MODE ttlMode,
  void *issuerContext,
  azap_uint16 nwkAddrDest,
  azap_uint8 startIndex);
bool azapZdpMgmtPermitJoinReq(
  azap_uint8 port,
  AZAP_REQ_TTL_MGMT_MODE ttlMode,
  void *issuerContext,
  azap_uint16 nwkAddrDest,
  azap_uint8 permitDuration,
  bool tcSignificance,
  bool responseExpected);
bool azapZdpMgmtLeaveReq(
  azap_uint8 port,
  AZAP_REQ_TTL_MGMT_MODE ttlMode,
  void *issuerContext,
  azap_uint16 nwkAddrDest,
  azap_uint8 deviceAddress[IEEE_ADDR_LEN],
  bool removeChildren,
  bool rejoin);


/* ZCL invocations */
azap_uint8 azapGetZclDataLength(azap_uint8 type, azap_uint8 *data);
bool azapZclIsAnalogDataType(azap_uint8 type);

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
  azapZclReadCommand_t *zclReadCmd);
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
  azapZclWriteCommand_t *zclWriteCmd);
bool azapZclConfigureReportCmdReq(
  azap_uint8 port,
  AZAP_REQ_TTL_MGMT_MODE ttlMode,
  void *issuerContext,
  azap_uint16 nwkAddrDest,
  azap_uint8 localEndp,
  azap_uint8 remoteEndp,
  azap_uint16 clusterId,
  azap_uint8 clusterDir,
  azapZclCfgReportCommand_t *zclCfgReportCmd);
bool azapZclConfigureReportCmdRsp(
  azap_uint8 port,
  AZAP_REQ_TTL_MGMT_MODE ttlMode,
  void *issuerContext,
  azap_uint16 nwkAddrDest,
  azap_uint8 localEndp,
  azap_uint8 remoteEndp,
  azap_uint16 clusterId,
  azapZclCfgReportRspCommand_t *zclCfgReportRspCmd);
bool azapZclReadReportConfigCmdReq(
  azap_uint8 port,
  AZAP_REQ_TTL_MGMT_MODE ttlMode,
  void *issuerContext,
  azap_uint16 nwkAddrDest,
  azap_uint8 localEndp,
  azap_uint8 remoteEndp,
  azap_uint16 clusterId,
  azap_uint8 clusterDir,
  azapZclReadReportCfgCommand_t *zclCmd);
bool azapZclDiscoverCmdReq(
  azap_uint8 port,
  AZAP_REQ_TTL_MGMT_MODE ttlMode,
  void *issuerContext,
  azap_uint16 nwkAddrDest,
  azap_uint8 localEndp,
  azap_uint8 remoteEndp,
  azap_uint16 clusterId,
  azap_uint8 clusterDir,
  azapZclDiscoverCommand_t *zclDiscoverCmd);
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
  azap_uint16 dataLen);
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
  azap_uint16 dataLen);

/* useful when used as a node */
bool azapZclReportCmdInd(
  azap_uint8 port,
  azap_uint16 nwkAddrDest,
  azap_uint8 localEndp,
  azap_uint8 remoteEndp,
  azap_uint16 clusterId,
  azapZclReportCommand_t *zclReportCmd);


#endif

