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
//** File : ./lib/format_znp_zdo.h
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


#ifndef _FORMAT_ZNP_ZDO__H_
#define _FORMAT_ZNP_ZDO__H_

/* ZDO messages family */
#define MT_ZDO_STARTUP_FROM_APP_SREQ          0x4025
#define MT_ZDO_STARTUP_FROM_APP_SRSP          0x4065

#define MT_ZDO_STATE_CHG_IND_AREQ             0xC045

#define MT_ZDO_MSG_CB_REGISTER_SREQ           0x3E25
#define MT_ZDO_MSG_CB_REGISTER_SRSP           0x3E65

#define MT_ZDO_MSG_CB_INCOMING_AREQ           0xFF45

#define MT_ZDO_NWK_ADDR_SREQ                  0x0025
#define MT_ZDO_NWK_ADDR_SRSP                  0x0065
#define MT_ZDO_IEEE_ADDR_SREQ                 0x0125
#define MT_ZDO_IEEE_ADDR_SRSP                 0x0165
#define MT_ZDO_NODE_DESC_SREQ                 0x0225
#define MT_ZDO_NODE_DESC_SRSP                 0x0265
#define MT_ZDO_POWER_DESC_SREQ                0x0325
#define MT_ZDO_POWER_DESC_SRSP                0x0365
#define MT_ZDO_SIMPLE_DESC_SREQ               0x0425
#define MT_ZDO_SIMPLE_DESC_SRSP               0x0465
#define MT_ZDO_ACTIVE_EP_SREQ                 0x0525
#define MT_ZDO_ACTIVE_EP_SRSP                 0x0565
#define MT_ZDO_BIND_SREQ                      0x2125
#define MT_ZDO_BIND_SRSP                      0x2165
#define MT_ZDO_UNBIND_SREQ                    0x2225
#define MT_ZDO_UNBIND_SRSP                    0x2265
#define MT_ZDO_MGMT_RTG_SREQ                  0x3225
#define MT_ZDO_MGMT_RTG_SRSP                  0x3265
#define MT_ZDO_MGMT_BIND_SREQ                 0x3325
#define MT_ZDO_MGMT_BIND_SRSP                 0x3365
#define MT_ZDO_MGMT_LEAVE_SREQ                0x3425
#define MT_ZDO_MGMT_LEAVE_SRSP                0x3465
#define MT_ZDO_MGMT_PERMIT_JOIN_SREQ          0x3625
#define MT_ZDO_MGMT_PERMIT_JOIN_SRSP          0x3665


// ZDO Cluster IDs
#define ZDO_RESPONSE_BIT_V1_0   ((azap_uint8)0x80)
#define ZDO_RESPONSE_BIT        ((azap_uint16)0x8000)

#define NWK_addr_req            ((azap_uint16)0x0000)
#define IEEE_addr_req           ((azap_uint16)0x0001)
#define Node_Desc_req           ((azap_uint16)0x0002)
#define Power_Desc_req          ((azap_uint16)0x0003)
#define Simple_Desc_req         ((azap_uint16)0x0004)
#define Active_EP_req           ((azap_uint16)0x0005)
#define Match_Desc_req          ((azap_uint16)0x0006)
#define NWK_addr_rsp            (NWK_addr_req | ZDO_RESPONSE_BIT)
#define IEEE_addr_rsp           (IEEE_addr_req | ZDO_RESPONSE_BIT)
#define Node_Desc_rsp           (Node_Desc_req | ZDO_RESPONSE_BIT)
#define Power_Desc_rsp          (Power_Desc_req | ZDO_RESPONSE_BIT)
#define Simple_Desc_rsp         (Simple_Desc_req | ZDO_RESPONSE_BIT)
#define Active_EP_rsp           (Active_EP_req | ZDO_RESPONSE_BIT)
#define Match_Desc_rsp          (Match_Desc_req | ZDO_RESPONSE_BIT)

#define Complex_Desc_req        ((azap_uint16)0x0010)
#define User_Desc_req           ((azap_uint16)0x0011)
#define Discovery_Cache_req     ((azap_uint16)0x0012)
#define Device_annce            ((azap_uint16)0x0013)
#define User_Desc_set           ((azap_uint16)0x0014)
#define Server_Discovery_req    ((azap_uint16)0x0015)
#define Complex_Desc_rsp        (Complex_Desc_req | ZDO_RESPONSE_BIT)
#define User_Desc_rsp           (User_Desc_req | ZDO_RESPONSE_BIT)
#define Discovery_Cache_rsp     (Discovery_Cache_req | ZDO_RESPONSE_BIT)
#define User_Desc_conf          (User_Desc_set | ZDO_RESPONSE_BIT)
#define Server_Discovery_rsp    (Server_Discovery_req | ZDO_RESPONSE_BIT)

#define End_Device_Bind_req     ((azap_uint16)0x0020)
#define Bind_req                ((azap_uint16)0x0021)
#define Unbind_req              ((azap_uint16)0x0022)
#define Bind_rsp                (Bind_req | ZDO_RESPONSE_BIT)
#define End_Device_Bind_rsp     (End_Device_Bind_req | ZDO_RESPONSE_BIT)
#define Unbind_rsp              (Unbind_req | ZDO_RESPONSE_BIT)

#define Mgmt_NWK_Disc_req       ((azap_uint16)0x0030)
#define Mgmt_Lqi_req            ((azap_uint16)0x0031)
#define Mgmt_Rtg_req            ((azap_uint16)0x0032)
#define Mgmt_Bind_req           ((azap_uint16)0x0033)
#define Mgmt_Leave_req          ((azap_uint16)0x0034)
#define Mgmt_Direct_Join_req    ((azap_uint16)0x0035)
#define Mgmt_Permit_Join_req    ((azap_uint16)0x0036)
#define Mgmt_NWK_Update_req     ((azap_uint16)0x0038)
#define Mgmt_NWK_Disc_rsp       (Mgmt_NWK_Disc_req | ZDO_RESPONSE_BIT)
#define Mgmt_Lqi_rsp            (Mgmt_Lqi_req | ZDO_RESPONSE_BIT)
#define Mgmt_Rtg_rsp            (Mgmt_Rtg_req | ZDO_RESPONSE_BIT)
#define Mgmt_Bind_rsp           (Mgmt_Bind_req | ZDO_RESPONSE_BIT)
#define Mgmt_Leave_rsp          (Mgmt_Leave_req | ZDO_RESPONSE_BIT)
#define Mgmt_Direct_Join_rsp    (Mgmt_Direct_Join_req | ZDO_RESPONSE_BIT)
#define Mgmt_Permit_Join_rsp    (Mgmt_Permit_Join_req | ZDO_RESPONSE_BIT)
#define Mgmt_NWK_Update_notify  (Mgmt_NWK_Update_req | ZDO_RESPONSE_BIT)

// ZDO application start-up status
typedef enum
{
  ZDO_DEV_HOLD = 0x00,              /**< Initialized - not started automatically */
  ZDO_DEV_INIT = 0x01,              /**< Initialized - not connected to anything */
  ZDO_DEV_NWK_DISC = 0x02,          /**< Discovering PAN's to join */
  ZDO_DEV_NWK_JOINING = 0x03,       /**< Joining a PAN */
  ZDO_DEV_NWK_REJOIN = 0x04,        /**< ReJoining a PAN, only for end devices */
  ZDO_DEV_END_DEVICE_UNAUTH = 0x05, /**< Joined but not yet authenticated by trust center */
  ZDO_DEV_END_DEVICE = 0x06,        /**< Started as device after authentication */
  ZDO_DEV_ROUTER = 0x07,            /**< Device joined, authenticated and is a router */
  ZDO_DEV_COORD_STARTING = 0x08,    /**< Starting as Zigbee Coordinator */
  ZDO_DEV_ZB_COORD = 0x09,          /**< Started as Zigbee Coordinator */
  ZDO_DEV_NWK_ORPHAN = 0x0A,        /**< Device has lost information about its parent... */
} ZDO_DEV_STATUS;


bool znpSendZdoMsgCbRegister(uartBufControl_t *buf, azap_uint16 id);


#endif

