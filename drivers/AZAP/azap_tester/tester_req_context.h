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
//** File : ./azap_tester/tester_req_context.h
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



#ifndef __TESTER_REQUEST_CONTEXT__H_
#define __TESTER_REQUEST_CONTEXT__H_

typedef enum
{
  TSTER_REQ_UNKNOWN = 0,
  TSTER_REQ_AUDIT,
  TSTER_REQ_FROM_CLI,
} TSTER_REQ_TYPE;

/**
 * @struct testerReqContext_t
 * This object is used as request context in AZAP API/callback exchanges (so called
 * "issuerContext" in all AZAP API function prototypes)
 */
typedef struct testerReqContext_t
{
  /* methods */
  /** @ref testerReqContext_t_newFree */
  void (*free) (struct testerReqContext_t *);

  /* members */
  /** The request context type: it specifies the reason context for this request. */
  TSTER_REQ_TYPE type;

  /** The targeted sensor network address for the ZigBee HAN. */
  azap_uint16 nwkAddr;
  /** The number of pending ZigBee request, relevant in case of device audit. */
  short nbPendingRequests;

  azap_uint16 clusterId;
  azap_uint8 srcEP;
  azap_uint8 dstEP;

  /** Placeholder for user data. */
  void *issuerData;

} testerReqContext_t;

/* dynamic allocation */
testerReqContext_t *new_testerReqContext_t(TSTER_REQ_TYPE type);
void testerReqContext_t_newFree(testerReqContext_t *This);


#endif

