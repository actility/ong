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
//** File : ./lib/format_znp_af.h
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
 * @file format_znp_af.h
 * @brief Defines tool functions for AF interface.
 */
#ifndef _FORMAT_ZNP_AF__H_
#define _FORMAT_ZNP_AF__H_

/* AF messages family */
#define MT_AF_REGISTER_SREQ                   0x0024
#define MT_AF_REGISTER_SRSP                   0x0064
#define MT_AF_DATA_EXT_SREQ                   0x0224
#define MT_AF_DATA_EXT_SRSP                   0x0264
#define MT_AF_DATA_CONFIRM_AREQ               0x8044
#define MT_AF_INCOMING_MSG_AREQ               0x8144


#define MT_AF_ADDR_MODE_NOT_PRESENT           0
#define MT_AF_ADDR_MODE_GROUP                 1
#define MT_AF_ADDR_MODE_16BIT                 2
#define MT_AF_ADDR_MODE_64BIT                 3
#define MT_AF_ADDR_MODE_BROADCAST             15

/** ZCL header - frame control field */
typedef struct
{
  azap_uint8 type;
  azap_uint8 manuSpecific;
  azap_uint8 direction;
  azap_uint8 disableDefaultRsp;
  azap_uint8 reserved;
} azapZclFrameControl_t;

/** ZCL header */
typedef struct
{
  azapZclFrameControl_t fc;
  azap_uint16 manuCode;
  azap_uint8 transSeqNum;
  azap_uint8 commandId;
} azapZclFrameHeader_t;

// building and send messages
bool znpSendAfRegister(uartBufControl_t *buf, azapEndPoint_t *ep);
bool znpSendAfDataRequest(uartBufControl_t *buf, azap_uint16 dstAddr, azap_uint8 dstEp,
    azap_uint8 srcEp, azap_uint16 clusterId, azap_uint8 *transactionId, azap_uint16 bufLen,
    azap_uint8 *afBufCmd);
bool znpSendZclCommand(uartBufControl_t *buf, azap_uint16 dstAddr, azap_uint8 dstEp,
    azap_uint8 srcEp, azap_uint8 zclCmd, bool specific, azap_uint16 clusterId, azap_uint8 direction,
    azap_uint16 manuCode, azap_uint16 bufLen, azap_uint8 *afBufCmf, azap_uint8 *transID);

// parsing
void parseZCLHeader(azap_byte **payload, azap_uint8 *len, azapZclFrameHeader_t *hdr);
azapZclReadRspCommand_t *parseZCLReadCmdRsp(azap_uint8 len, azap_byte *data);
azapZclWriteRspCommand_t *parseZCLWriteCmdRsp(azap_uint8 len, azap_byte *data);
azapZclCfgReportRspCommand_t *parseZCLConfigureReportCmdRsp(azap_uint8 len, azap_byte *data);
azapZclReportCommand_t *parseZCLReportCmd(azap_uint8 len, azap_byte *data);
azapZclReadReportCfgRspCommand_t *parseZCLReadReportConfigCmdRsp(azap_uint8 len, azap_byte *data);
azapZclDiscoverRspCommand_t *parseZCLDiscoverRspCmd(azap_uint8 len, azap_byte *data);
azapZclCfgReportCommand_t *parseZCLConfigureReportCmd(azap_uint8 len, azap_byte *data);

#endif

