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
//** File : ./include/azap_api_common.h
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
 * @file azap_api_common.h
 * The following defines the structures exchanged in API functions.
 * It implements what the norm specifies.
 * They are mandatory for using the AZAP library API.
 */
#ifndef _ZB_COMMON__H_
#define _ZB_COMMON__H_

#define STATUS_CODE_SUCCESS 0x00
#define STATUS_CODE_FAILURE 0x01

#define ZDO_RTG_ADDR_MODE_16BIT 0x01
#define ZDO_RTG_ADDR_MODE_64BIT 0x03

typedef struct
{
  azap_uint8 addrMode;        /**< The addressing mode for the address. */
  union
  {
    azap_uint16 shortAddr;
    azap_uint8 extAddr[IEEE_ADDR_LEN];
  } addr;
} zbAddress_t;

#endif

