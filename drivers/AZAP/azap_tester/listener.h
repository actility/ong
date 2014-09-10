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
//** File : ./azap_tester/listener.h
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
 * @file listener.h
 * @brief Defines the prototypes for methods provided in t_azap_api_callbacks for AZAP init.
 */

#ifndef _AZAP_LISTENER__H_
#define _AZAP_LISTENER__H_



t_azap_api_callbacks *getListener();

void azapTesterStartedCb(
  azap_uint8 port,
  bool success,
  azap_uint16 shortAddr,
  azap_uint8 deviceIEEE[IEEE_ADDR_LEN],
  azap_uint16 panId,
  azap_uint8 panIdExt[IEEE_ADDR_LEN]);
void azapTesterZdpBindReqCb (
  azap_uint8 port,
  void *issuerContext,
  azap_uint16 nwkAddrSrc,
  azap_uint8 srcIeeeAddr[IEEE_ADDR_LEN],
  azap_uint8 srcEndPoint,
  azap_uint16 clusterId,
  azap_uint8 destIeeeAddr[IEEE_ADDR_LEN],
  azap_uint8 destEndPoint);
void azapTesterZdpUnbindReqCb(
  azap_uint8 port,
  void *issuerContext,
  azap_uint16 nwkAddrSrc,
  azap_uint8 srcIeeeAddr[IEEE_ADDR_LEN],
  azap_uint8 srcEndPoint,
  azap_uint16 clusterId,
  azap_uint8 destIeeeAddr[IEEE_ADDR_LEN],
  azap_uint8 destEndPoint);
void azapTesterZdpDeviceAnnceCb(
  azap_uint8 port,
  azap_uint16 nwkAddr,
  azap_uint8 ieeeAddr[IEEE_ADDR_LEN],
  azap_uint8 alternatePanCoordinator,
  azap_uint8 deviceType,
  azap_uint8 powerSource,
  azap_uint8 receiverOnWhenIdle,
  azap_uint8 securityCapability,
  azap_uint8 allocateAddress);
void azapTesterZdpNwkAddrRspCb (
  azap_uint8 port,
  void *issuerContext,
  azap_uint16 nwkAddrSrc,
  azap_uint8 status,
  azap_uint8 ieeeAddrRemoteDev[IEEE_ADDR_LEN],
  azap_uint16 nwkAddrRemoteDev,
  azap_uint8 numAssocDev,
  azap_uint8 startIndex,
  azap_uint16 nwkAddrAssocDevList[]);
void azapTesterZdpIeeeAddrRspCb (
  azap_uint8 port,
  void *issuerContext,
  azap_uint16 nwkAddrSrc,
  azap_uint8 status,
  azap_uint8 ieeeAddrRemoteDev[IEEE_ADDR_LEN],
  azap_uint16 nwkAddrRemoteDev,
  azap_uint8 numAssocDev,
  azap_uint8 startIndex,
  azap_uint16 nwkAddrAssocDevList[]);
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
  azap_uint8 descriptorCapabilityField);
void azapTesterZdpPowerDescRspCb (
  azap_uint8 port,
  void *issuerContext,
  azap_uint16 nwkAddrSrc,
  azap_uint8 status,
  azap_uint16 nwkAddrOfInterest,
  azap_uint8 currentPowerMode,
  azap_uint8 availablePowerSources,
  azap_uint8 currentPowerSource,
  azap_uint8 currentPowerSourceLevel);
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
  azap_uint16 applicationOutputClusterList[]);
void azapTesterZdpActiveEPRspCb (
  azap_uint8 port,
  void *issuerContext,
  azap_uint16 nwkAddrSrc,
  azap_uint8 status,
  azap_uint16 nwkAddrOfInterest,
  azap_uint8 activeEPCount,
  azap_uint8 activeEPList[]);
void azapTesterZdpBindRspCb (
  azap_uint8 port,
  void *issuerContext,
  azap_uint16 nwkAddrSrc,
  azap_uint8 status);
void azapTesterZdpUnbindRspCb (
  azap_uint8 port,
  void *issuerContext,
  azap_uint16 nwkAddrSrc,
  azap_uint8 status);
void azapTesterZdpMgmtRtgRspCb (
  azap_uint8 port,
  void *issuerContext,
  azap_uint16 nwkAddrSrc,
  azap_uint8 status,
  azap_uint8 routingTableEntries,
  azap_uint8 startIndex,
  azap_uint8 routingTableListCount,
  zdoRtgEntry_t routeTableList[]);
void azapTesterZdpMgmtBindRspCb (
  azap_uint8 port,
  void *issuerContext,
  azap_uint16 nwkAddrSrc,
  azap_uint8 status,
  azap_uint8 bindingTableEntries,
  azap_uint8 startIndex,
  azap_uint8 bindingTableListCount,
  zdoBindEntry_t bindingTableList[]);
void azapTesterZdpMgmtPermitJoinRspCb (
  azap_uint8 port,
  void *issuerContext,
  azap_uint16 nwkAddrSrc,
  azap_uint8 status);
void azapTesterZdpMgmtLeaveRspCb (
  azap_uint8 port,
  void *issuerContext,
  azap_uint16 nwkAddrSrc,
  azap_uint8 status);

/* SAPI call backs */
void azapTesterSapiFindDeviceConfCb (
  azap_uint8 port,
  azap_uint16 nwkAddr,
  azap_uint8 ieeeAddr[IEEE_ADDR_LEN]);

/* ZCL call backs */
void azapTesterZclReadCmdRspCb (
  azap_uint8 port,
  void *issuerContext,
  azap_uint16 nwkAddrSrc,
  azap_uint8 endPoint,
  azap_uint16 clusterId,
  azap_uint8 clusterDir,
  azap_uint16 manufacturerCode,
  azapZclReadRspCommand_t *data);
void azapTesterZclWriteCmdRspCb (
  azap_uint8 port,
  void *issuerContext,
  azap_uint16 nwkAddrSrc,
  azap_uint8 endPoint,
  azap_uint16 clusterId,
  azap_uint8 clusterDir,
  azap_uint16 manufacturerCode,
  azapZclWriteRspCommand_t *data);
void azapTesterZclConfigureReportCmdRspCb (
  azap_uint8 port,
  void *issuerContext,
  azap_uint16 nwkAddrSrc,
  azap_uint8 endPoint,
  azap_uint16 clusterId,
  azapZclCfgReportRspCommand_t *data);
void azapTesterZclReportCmdCb (
  azap_uint8 port,
  azap_uint16 nwkAddrSrc,
  azap_uint8 endPoint,
  azap_uint16 clusterId,
  azapZclReportCommand_t *data);
void azapTesterZclReadReportConfigCmdRspCb (
  azap_uint8 port,
  void *issuerContext,
  azap_uint16 nwkAddrSrc,
  azap_uint8 endPoint,
  azap_uint16 clusterId,
  azapZclReadReportCfgRspCommand_t *data);
void azapTesterZclDiscoverCmdRspCb (
  azap_uint8 port,
  void *issuerContext,
  azap_uint16 nwkAddrSrc,
  azap_uint8 endPoint,
  azap_uint16 clusterId,
  azap_uint8 clusterDir,
  azapZclDiscoverRspCommand_t *data);
void azapTesterZclIncomingCmdCb (
  azap_uint8 port,
  void *issuerContext,
  azap_uint16 nwkAddrSrc,
  azap_uint8 endPoint,
  azap_uint16 clusterId,
  azap_uint8 cmdCode,
  azap_byte cmdData[],
  azap_uint8 cmdLen);

/* when used as a node */
void azapTesterZclConfigureReportCmdCb (
  azap_uint8 port,
  azap_uint16 nwkAddrDest,
  azap_uint8 endPoint,
  azap_uint16 clusterId,
  azapZclCfgReportCommand_t *data);


#endif


