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
//** File : ./lib/nvram.h
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


#ifndef _NV_RAM__H_
#define _NV_RAM__H_

// OSAL NV item IDs
#define ZCD_NV_EXTADDR                 0x0001
#define ZCD_NV_BOOTCOUNTER             0x0002
#define ZCD_NV_STARTUP_OPTION          0x0003
#define ZCD_NV_START_DELAY             0x0004

// NWK Layer NV item IDs
#define ZCD_NV_NIB                     0x0021
#define ZCD_NV_DEVICE_LIST             0x0022
#define ZCD_NV_ADDRMGR                 0x0023
#define ZCD_NV_POLL_RATE               0x0024
#define ZCD_NV_QUEUED_POLL_RATE        0x0025
#define ZCD_NV_RESPONSE_POLL_RATE      0x0026
#define ZCD_NV_REJOIN_POLL_RATE        0x0027
#define ZCD_NV_DATA_RETRIES            0x0028
#define ZCD_NV_POLL_FAILURE_RETRIES    0x0029
#define ZCD_NV_STACK_PROFILE           0x002A
#define ZCD_NV_INDIRECT_MSG_TIMEOUT    0x002B
#define ZCD_NV_ROUTE_EXPIRY_TIME       0x002C
#define ZCD_NV_EXTENDED_PAN_ID         0x002D
#define ZCD_NV_BCAST_RETRIES           0x002E
#define ZCD_NV_PASSIVE_ACK_TIMEOUT     0x002F
#define ZCD_NV_BCAST_DELIVERY_TIME     0x0030
#define ZCD_NV_NWK_MODE                0x0031
#define ZCD_NV_CONCENTRATOR_ENABLE     0x0032
#define ZCD_NV_CONCENTRATOR_DISCOVERY  0x0033
#define ZCD_NV_CONCENTRATOR_RADIUS     0x0034
#define ZCD_NV_CONCENTRATOR_RC         0x0036
#define ZCD_NV_NWK_MGR_MODE            0x0037
#define ZCD_NV_SRC_RTG_EXPIRY_TIME     0x0038
#define ZCD_NV_ROUTE_DISCOVERY_TIME    0x0039
#define ZCD_NV_NWK_ACTIVE_KEY_INFO     0x003A
#define ZCD_NV_NWK_ALTERN_KEY_INFO     0x003B

// APS Layer NV item IDs
#define ZCD_NV_BINDING_TABLE           0x0041
#define ZCD_NV_GROUP_TABLE             0x0042
#define ZCD_NV_APS_FRAME_RETRIES       0x0043
#define ZCD_NV_APS_ACK_WAIT_DURATION   0x0044
#define ZCD_NV_APS_ACK_WAIT_MULTIPLIER 0x0045
#define ZCD_NV_BINDING_TIME            0x0046
#define ZCD_NV_APS_USE_EXT_PANID       0x0047
#define ZCD_NV_APS_USE_INSECURE_JOIN   0x0048
#define ZCD_NV_APSF_WINDOW_SIZE        0x0049 // Window size for fregmentation
#define ZCD_NV_APSF_INTERFRAME_DELAY   0x004A // delay between tx blocks when using fragmentation
#define ZCD_NV_APS_NONMEMBER_RADIUS    0x004B // Multicast non_member radius
#define ZCD_NV_APS_LINK_KEY_TABLE      0x004C

// Security NV Item IDs
#define ZCD_NV_SECURITY_LEVEL          0x0061
#define ZCD_NV_PRECFGKEY               0x0062
#define ZCD_NV_PRECFGKEYS_ENABLE       0x0063
#define ZCD_NV_SECURITY_MODE           0x0064
#define ZCD_NV_SECURE_PERMIT_JOIN      0x0065

#define ZCD_NV_IMPLICIT_CERTIFICATE    0x0069
#define ZCD_NV_DEVICE_PRIVATE_KEY      0x006A
#define ZCD_NV_CA_PUBLIC_KEY           0x006B

#define ZCD_NV_USE_DEFAULT_TCLK        0x006D
#define ZCD_NV_TRUSTCENTER_ADDR        0x006E
#define ZCD_NV_RNG_COUNTER             0x006F
#define ZCD_NV_RANDOM_SEED             0x0070

// ZDO NV Item IDs
#define ZCD_NV_USERDESC                0x0081
#define ZCD_NV_NWKKEY                  0x0082
#define ZCD_NV_PANID                   0x0083
#define ZCD_NV_CHANLIST                0x0084
#define ZCD_NV_LEAVE_CTRL              0x0085
#define ZCD_NV_SCAN_DURATION           0x0086
#define ZCD_NV_LOGICAL_TYPE            0x0087
#define ZCD_NV_NWKMGR_MIN_TX           0x0088

#define ZCD_NV_ZDO_DIRECT_CB           0x008F

// ZCL NV item IDs
#define ZCD_NV_SCENE_TABLE             0x0091
// Non-standard NV item IDs
#define ZCD_NV_SAPI_ENDPOINT           0x00A1

// NV Items Reserved for Trust Center Link Key Table entries
// 0x0101 - 0x01FF
#define ZCD_NV_TCLK_TABLE_START        0x0101
#define ZCD_NV_TCLK_TABLE_END          0x01FF

// NV Items Reserved for APS Link Key Table entries
// 0x0201 - 0x02FF
#define ZCD_NV_APS_LINK_KEY_DATA_START 0x0201     // APS key data
#define ZCD_NV_APS_LINK_KEY_DATA_END   0x02FF

// NV Items Reserved for Master Key Table entries
// 0x0301 - 0x03FF
#define ZCD_NV_MASTER_KEY_DATA_START   0x0301     // Master key data
#define ZCD_NV_MASTER_KEY_DATA_END     0x03FF

// NV Items Reserved for applications (user applications)
// 0x0401 <96> 0x0FFF


// ZCD_NV_STARTUP_OPTION values
//   These are bit weighted - you can OR these together.
//   Setting one of these bits will set their associated NV items
//   to code initialized values.
#define ZCD_STARTOPT_DEFAULT_CONFIG_STATE  0x01
#define ZCD_STARTOPT_DEFAULT_NETWORK_STATE 0x02
#define ZCD_STARTOPT_AUTO_START            0x04
#define ZCD_STARTOPT_CLEAR_CONFIG   ZCD_STARTOPT_DEFAULT_CONFIG_STATE
#define ZCD_STARTOPT_CLEAR_STATE    ZCD_STARTOPT_DEFAULT_NETWORK_STATE


#define ZCL_KE_IMPLICIT_CERTIFICATE_LEN    48
#define ZCL_KE_CA_PUBLIC_KEY_LEN           22
#define ZCL_KE_DEVICE_PRIVATE_KEY_LEN      21

#endif


