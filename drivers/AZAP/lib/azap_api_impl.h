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
//** File : ./lib/azap_api_impl.h
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



#ifndef _ACTILITY_ZAP_API_IMPL__H_
#define _ACTILITY_ZAP_API_IMPL__H_

#define MAX_NB_PORT_CONTEXT 5
/*********************************************************************
 * Static variables
 */
unsigned long azapNbZdoNotifications[MAX_NB_PORT_CONTEXT];
unsigned long azapNbZdoRequestsIssued[MAX_NB_PORT_CONTEXT];
unsigned long azapNbZdoRequestsCompleted[MAX_NB_PORT_CONTEXT];
unsigned long azapNbZdoRequestsFailed[MAX_NB_PORT_CONTEXT];
unsigned long azapNbZclNotifications[MAX_NB_PORT_CONTEXT];
unsigned long azapNbZclRequestsIssued[MAX_NB_PORT_CONTEXT];
unsigned long azapNbZclRequestsCompleted[MAX_NB_PORT_CONTEXT];
unsigned long azapNbZclRequestsFailed[MAX_NB_PORT_CONTEXT];


/*********************************************************************
 * Prototypes
 */

void azapInitImpl(azap_uint8 port, azap_uint8 *targetedIeee, void *mainTbPoll,
    t_azap_api_callbacks *pCallbacks, azap_uint8 defaultChannel, azap_uint8 deviceType,
    bool resetHanAtReboot);

void azapUnInitImpl(azap_uint8 port);

void azapProcessStartedCb(azap_uint8 port, bool success);

// SAPI call backs
void azapProcessSapiFindDeviceConfCb(portContext_t *port, azap_uint16 srcAddr,
    azap_uint8 len, azap_byte *data);

// ZDO methods
void azapProcessZdoDeviceAnnceCb(portContext_t *port, azap_uint16 srcAddr,
    azap_uint8 len, azap_byte *data);
void azapProcessZdoBindReqCb(portContext_t *port, azap_uint16 srcAddr,
    azap_uint8 len, azap_byte *data);
void azapProcessZdoUnbindReqCb(portContext_t *port, azap_uint16 srcAddr,
    azap_uint8 len, azap_byte *data);

void azapProcessZdoNwkAddrRspCb(portContext_t *port, azap_uint16 srcAddr,
    azap_uint8 len, azap_byte *data);
void azapProcessZdoIeeeAddrRspCb(portContext_t *port, azap_uint16 srcAddr,
    azap_uint8 len, azap_byte *data);
void azapProcessZdoNodeDescRspCb(portContext_t *port, azap_uint16 srcAddr,
    azap_uint8 len, azap_byte *data);
void azapProcessZdoPowerDescRspCb(portContext_t *port, azap_uint16 srcAddr,
    azap_uint8 len, azap_byte *data);
void azapProcessZdoSimpleDescRspCb(portContext_t *port, azap_uint16 srcAddr,
    azap_uint8 len, azap_byte *data);
void azapProcessZdoActiveEPRspCb(portContext_t *port, azap_uint16 srcAddr,
    azap_uint8 len, azap_byte *data);
void azapProcessZdoBindRspCb(portContext_t *port, azap_uint16 srcAddr,
    azap_uint8 len, azap_byte *data);
void azapProcessZdoUnbindRspCb(portContext_t *port, azap_uint16 srcAddr,
    azap_uint8 len, azap_byte *data);
void azapProcessZdoMgmtRtgRspCb(portContext_t *port, azap_uint16 srcAddr,
    azap_uint8 len, azap_byte *data);
void azapProcessZdoMgmtBindRspCb(portContext_t *port, azap_uint16 srcAddr,
    azap_uint8 len, azap_byte *data);
void azapProcessZdoMgmtPermitJoinRspCb(portContext_t *port, azap_uint16 srcAddr,
    azap_uint8 len, azap_byte *data);
void azapProcessZdoMgmtLeaveRspCb(portContext_t *port, azap_uint16 srcAddr,
    azap_uint8 len, azap_byte *data);

// ZCL methods
void azapProcessZclReadCmdRspCb(portContext_t *port, azap_uint16 srcAddr,
    azap_uint8 srcEndPointId, azap_uint16 clusterId, azapZclFrameHeader_t *zclHdr,
    azap_uint8 len, azap_byte *data);
void azapProcessZclWriteCmdRspCb(portContext_t *port, azap_uint16 srcAddr,
    azap_uint8 srcEndPointId, azap_uint16 clusterId, azapZclFrameHeader_t *zclHdr,
    azap_uint8 len, azap_byte *data);
void azapProcessZclConfigReportCmdRspCb(portContext_t *port, azap_uint16 srcAddr,
    azap_uint8 srcEndPointId, azap_uint16 clusterId, azapZclFrameHeader_t *zclHdr,
    azap_uint8 len, azap_byte *data);
void azapProcessZclReportCmdCb(portContext_t *port, azap_uint16 srcAddr,
    azap_uint8 srcEndPointId, azap_uint16 clusterId, azapZclFrameHeader_t *zclHdr,
    azap_uint8 len, azap_byte *data);
void azapProcessZclReadReportConfigCmdRspCb(portContext_t *port, azap_uint16 srcAddr,
    azap_uint8 srcEndPointId, azap_uint16 clusterId, azapZclFrameHeader_t *zclHdr,
    azap_uint8 len, azap_byte *data);
void azapProcessZclDiscoverCmdRspCb(portContext_t *port, azap_uint16 srcAddr,
    azap_uint8 srcEndPointId, azap_uint16 clusterId, azapZclFrameHeader_t *zclHdr,
    azap_uint8 len, azap_byte *data);
void azapProcessZclIncomingCmdCb(portContext_t *port, azap_uint16 srcAddr,
    azap_uint8 srcEndPointId, azap_uint16 clusterId, azapZclFrameHeader_t *zclHdr,
    azap_uint8 len, azap_byte *data);
void azapProcessZclConfigReportCmdCb(portContext_t *port, azap_uint16 srcAddr,
    azap_uint8 srcEndPointId, azap_uint16 clusterId, azapZclFrameHeader_t *zclHdr,
    azap_uint8 len, azap_byte *data);

#endif


