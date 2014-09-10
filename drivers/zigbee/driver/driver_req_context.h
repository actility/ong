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
//** File : driver/driver_req_context.h
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



#ifndef __DRIVER_REQUEST_CONTEXT__H_
#define __DRIVER_REQUEST_CONTEXT__H_

typedef enum
{
  DRV_REQ_UNKNOWN = 0,
  DRV_REQ_NO_RESPONSE_EXPECTED,
  DRV_REQ_DEVICE_AUDIT,
  DRV_REQ_OPERATION_FROM_DIA,
  DRV_REQ_OPERATION_FROM_DIA_NO_COMPLETION,
  DRV_REQ_READ_ATTR_FROM_DIA,
  DRV_REQ_WRITE_ATTR_FROM_DIA,
  DRV_REQ_FROM_CLI,
} DRV_REQ_TYPE;

/**
 * @struct driverReqContext_t
 * This object is used as request context in AZAP API/callback exchanges (so called
 * "issuerContext" in all AZAP API function prototypes)
 */
typedef struct driverReqContext_t
{
  /* methods */
  /** @ref driverReqContext_t_newFree */
  void (*free) (struct driverReqContext_t *);

  /* members */
  /** The request context type: it specifies the reason context for this request. */
  DRV_REQ_TYPE type;

  // NOTE: the sensor instance is not stored here directly to prevent from
  //       bugs consecutive to it deletion between the request and the response.
  //       We store its identifier instead.
  /** the sensor unique identifier. */
  int serialNum;
  /** The targeted sensor network address for the ZigBee HAN. */
  azap_uint16 nwkAddr;
  /** The number of pending ZigBee request, relevant in case of device audit. */
  short nbPendingRequests;

  /** Placeholder for user data. */
  void *issuerData;
  /** A second placeholder for user data. */
  void *issuerData2;

} driverReqContext_t;

/* dynamic allocation */
driverReqContext_t *new_driverReqContext_t(DRV_REQ_TYPE type);
void driverReqContext_t_newFree(driverReqContext_t *This);


#endif

