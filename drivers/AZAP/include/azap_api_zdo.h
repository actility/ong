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
//** File : ./include/azap_api_zdo.h
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
 * @file zdo.h
 * The following defines the structures exchanged in API functions.
 * It implements what the norm specifies.
 * They are mandatory for using the AZAP library API.
 */
#ifndef _ZDO__H_
#define _ZDO__H_

#define DEV_LOGICAL_TYPE_COORD 0
#define DEV_LOGICAL_TYPE_ROUTER 1
#define DEV_LOGICAL_TYPE_DEV 2

#define ZDO_STATUS_TIMEOUT 0x85

typedef struct
{
  azap_uint16 dstAddress;     /**< Destination short address */
  azap_uint8 status;          /**< Route entry status */
  azap_uint16 nextHopAddress; /**< Next hop short address */
} zdoRtgEntry_t;

typedef struct
{
  azap_uint8 srcAddr[IEEE_ADDR_LEN];  /**< The source IEEE address for the binding entry. */
  azap_uint8 srcEndPoint;             /**< The source endpoint for the binding entry. */
  azap_uint16 clusterId;              /**< The identifier of the cluster on the source device
                                       *   that is bound to the destination device. */
  zbAddress_t dstAddr;                /**< The destination address for the binding entry. */
  azap_uint8 dstEndPoint;             /**< The destination endpoint for the binding entry. */
} zdoBindEntry_t;

#endif

