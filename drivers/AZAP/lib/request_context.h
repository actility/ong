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
//** File : ./lib/request_context.h
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


#ifndef _PENDING_REQUEST__H_
#define _PENDING_REQUEST__H_

/**
 * @file request_context.h
 * @brief This object allows storing parameters for ZigBee pending requests.
 * It is used for requestManager_t.
 * @author mlouiset
 * @date 2012-08-28
 */

typedef enum AZAP_REQUEST_TYPE
{
  // ZDO requests family
  AZAP_ZDO_NWK_ADDR_REQ             = 0,
  AZAP_ZDO_IEEE_ADDR_REQ            = 1,
  AZAP_ZDO_NODE_DESC_REQ            = 2,
  AZAP_ZDO_POWER_DESC_REQ           = 3,
  AZAP_ZDO_ACTIVE_EP_REQ            = 4,
  AZAP_ZDO_SIMPLE_DESC_REQ          = 5,
  AZAP_ZDO_BIND_REQ                 = 6,
  AZAP_ZDO_UNBIND_REQ               = 7,
  AZAP_ZDO_MGMT_RTG_REQ             = 8,
  AZAP_ZDO_MGMT_BIND_REQ            = 9,
  AZAP_ZDO_MGMT_PERMIT_JOIN_REQ     = 10,
  AZAP_ZDO_MGMT_LEAVE_REQ           = 11,
  // ZCL requests family
  AZAP_ZCL_READ_CMD                 = 100,
  AZAP_ZCL_WRITE_CMD                = 101,
  AZAP_ZCL_CONFIGURE_REPORTING_CMD  = 102,
  AZAP_ZCL_DISCOVER_CMD             = 103,
  AZAP_ZCL_OUTGOING_CMD             = 104,
  AZAP_ZCL_OUTGOING_EXT_CMD         = 105,
  AZAP_ZCL_REPORT_CMD               = 106,
  AZAP_ZCL_READ_REPORTING_CONF_CMD  = 107,
} AZAP_REQUEST_TYPE;

char *AZAP_REQUEST_TYPE_toString(AZAP_REQUEST_TYPE type);

/**
 * @struct requestContext_t
 * The object for AZAP pending requests, that could not have been sent directly.
 * This data structure aims at storing parameters for requests that have to be sent later
 * on, when ZStack running on the CC253X is able to process it again. As a consequence, it
 * contains the intersection of parameters for all API functions.
 */
typedef struct requestContext_t
{
  /* methods */
  /** @ref requestContext_t_newFree */
  void (* free) (struct requestContext_t *);
  /** @ref requestContext_t_tic */
  int (* tic) (struct requestContext_t *, bool requestSent);

  /* members */
  /** For RTL_List management. */
  struct list_head chainLink;
  /** The number of seconds that remain before considering the request has timed out. */
  int timeToLive;
  /** Indicates how the TTL is to be managed. */
  AZAP_REQ_TTL_MGMT_MODE ttlMode;

  /** TODO: add member "maxRetries" to manage requests re-emission for long term requests */

  /** Any issuer context can be stored here. */
  void *issuerContext;
  /** The request type (i.e. ieee_addr_req, node_desc_req, ZclReadCmd, ...) */
  AZAP_REQUEST_TYPE type;
  /** only relevant in case of AF message, to correlate AF_DATA_CONFIRM with requests */
  azap_uint8 transactionId;

  azap_uint16 nwkAddrDest;
  azap_uint16 clusterId;
  azap_uint8 clusterDir;
  azap_uint16 manufacturerCode;
  azap_uint8 remoteEndp;
  azap_uint8 expectedCmdCode;

  /** The stored parameter when the request is postponed. */
  postponedRequestElts_t *requestParameters;

} requestContext_t;

/* dynamic allocation */
requestContext_t *new_requestContext_t(int timeoutValue, AZAP_REQ_TTL_MGMT_MODE ttlMode);
void requestContext_t_newFree(requestContext_t *This);

int requestContext_t_tic (requestContext_t *, bool requestSent);

#endif


