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
//** File : ./include/azap_types.h
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
 * @file azap_types.h
 * This file contains the type definitions that are largely used in AZAP layer.
 */

#ifndef _AZAP_TYPES__H_
#define _AZAP_TYPES__H_

#ifdef __NO_TRACE__
#undef RTL_TRDBG
#define RTL_TRDBG(lev,...)
#endif

#define STRUCTPACKED __attribute__ ((packed))

typedef unsigned char azap_byte;

typedef signed char azap_int8;
typedef unsigned char azap_uint8;

typedef signed short azap_int16;
typedef unsigned short azap_uint16;

typedef signed long azap_int32;
typedef unsigned long azap_uint32;

#ifndef __cplusplus
typedef unsigned char   bool;
#define true 1
#define false 0
#endif

enum
{
  AddrNotPresent = 0,
  AddrGroup = 1,
  Addr16Bit = 2,
  Addr64Bit = 3,
  AddrBroadcast = 15
};

#define IEEE_ADDR_LEN   8

typedef struct
{
  union
  {
    azap_uint16 shortAddr;
    azap_byte extAddr[IEEE_ADDR_LEN];
  } addr;
  azap_byte addrMode;
} zAddrType_t;


// rtl_trace levels
enum
{
  TRACE_ERROR = 0,
  TRACE_API = 1,
  TRACE_INFO = 2,
  TRACE_IMPL = 5,
  TRACE_DETAIL = 6,
  TRACE_FULL = 10,
};

/**
 * @enum AZAP_REQ_TTL_MGMT_MODE
 * @brief Defines the TTL management modes for requests.
 * AZAP_REQ_TTL_START_AT_API means that the TTL starts decreasing as soon as
 * the request is handled at the API layer,
 * AZAP_REQ_TTL_START_AT_ZB_SEND means that the  TTL starts decreasing as soon as
 * the request can be forwarded to the CC2531 ship.
 */
typedef enum
{
  AZAP_REQ_TTL_START_AT_API = 0,
  AZAP_REQ_TTL_START_AT_ZB_SEND = 1,
} AZAP_REQ_TTL_MGMT_MODE;

#endif

