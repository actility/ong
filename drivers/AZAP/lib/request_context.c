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
//** File : ./lib/request_context.c
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
 * @file postponed_request.c
 * @brief This object allows storing parameters for ZigBee pending requests.
 * It is used for requestManager_t.
 * @author mlouiset
 * @date 2012-08-28
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <rtlbase.h>

#include "azap_types.h"
#include "azap_api_zcl.h"

#include "postponed_request_elts.h"
#include "request_context.h"

/*****************************************************************************/
/* Prototypes for local functions */
static void requestContext_t_init(requestContext_t *This);

/**
 * @brief Gives the string representation of a AZAP_REQUEST_TYPE.
 * @param type the AZAP_REQUEST_TYPE to represent.
 * @return the resulting string representation.
 */
char *AZAP_REQUEST_TYPE_toString(AZAP_REQUEST_TYPE type)
{
  switch (type)
  {
      // ZDO requests family
    case AZAP_ZDO_NWK_ADDR_REQ:
      return "AZAP_ZDO_NWK_ADDR_REQ";
    case AZAP_ZDO_IEEE_ADDR_REQ:
      return "AZAP_ZDO_IEEE_ADDR_REQ";
    case AZAP_ZDO_NODE_DESC_REQ:
      return "AZAP_ZDO_NODE_DESC_REQ";
    case AZAP_ZDO_POWER_DESC_REQ:
      return "AZAP_ZDO_POWER_DESC_REQ";
    case AZAP_ZDO_ACTIVE_EP_REQ:
      return "AZAP_ZDO_ACTIVE_EP_REQ";
    case AZAP_ZDO_SIMPLE_DESC_REQ:
      return "AZAP_ZDO_SIMPLE_DESC_REQ";
    case AZAP_ZDO_BIND_REQ:
      return "AZAP_ZDO_BIND_REQ";
    case AZAP_ZDO_UNBIND_REQ:
      return "AZAP_ZDO_UNBIND_REQ";
    case AZAP_ZDO_MGMT_RTG_REQ:
      return "AZAP_ZDO_MGMT_RTG_REQ";
    case AZAP_ZDO_MGMT_BIND_REQ:
      return "AZAP_ZDO_MGMT_BIND_REQ";
    case AZAP_ZDO_MGMT_PERMIT_JOIN_REQ:
      return "AZAP_ZDO_MGMT_PERMIT_JOIN_REQ";
    case AZAP_ZDO_MGMT_LEAVE_REQ:
      return "AZAP_ZDO_MGMT_LEAVE_REQ";
      // ZCL requests family
    case AZAP_ZCL_READ_CMD:
      return "AZAP_ZCL_READ_CMD";
    case AZAP_ZCL_WRITE_CMD:
      return "AZAP_ZCL_WRITE_CMD";
    case AZAP_ZCL_CONFIGURE_REPORTING_CMD:
      return "AZAP_ZCL_CONFIGURE_REPORTING_CMD";
    case AZAP_ZCL_DISCOVER_CMD:
      return "AZAP_ZCL_DISCOVER_CMD";
    case AZAP_ZCL_OUTGOING_CMD:
      return "AZAP_ZCL_OUTGOING_CMD";
    case AZAP_ZCL_OUTGOING_EXT_CMD:
      return "AZAP_ZCL_OUTGOING_EXT_CMD";
    case AZAP_ZCL_REPORT_CMD:
      return "AZAP_ZCL_REPORT_CMD";
    case AZAP_ZCL_READ_REPORTING_CONF_CMD:
      return "AZAP_ZCL_READ_REPORTING_CONF_CMD";
  }
  return NULL;
}

/*****************************************************************************/
/* Constructors / destructor */

/* static allocation */

/**
 * @brief Initialize the requestContext_t struct
 * @param This the class instance
 */
static void requestContext_t_init(requestContext_t *This)
{
  RTL_TRDBG(TRACE_DETAIL, "requestContext_t::init\n");

  /* Assign the pointers on functions */
  This->tic = requestContext_t_tic;

  /* Initialize the members */
  This->issuerContext = NULL;
  This->type = AZAP_ZDO_NWK_ADDR_REQ;
  This->transactionId = 0;

  This->nwkAddrDest = 0;
  This->clusterId = 0;
  This->clusterDir = 0;
  This->manufacturerCode = 0;
  This->remoteEndp = 0;
  This->expectedCmdCode = 0;
  This->requestParameters = NULL;
}

/* dynamic allocation */
/**
 * @brief do dynamic allocation of a requestContext_t.
 * @param timeoutValue the request time to live (in seconds).
 * @param ttlMode the TTL request management model (@see AZAP_REQ_TTL_MGMT_MODE).
 * @return the copy of the object
 */
requestContext_t *new_requestContext_t(int timeoutValue, AZAP_REQ_TTL_MGMT_MODE ttlMode)
{
  requestContext_t *This = malloc(sizeof(requestContext_t));
  if (!This)
  {
    return NULL;
  }
  requestContext_t_init(This);
  This->free = requestContext_t_newFree;
  This->timeToLive = timeoutValue;
  This->ttlMode = ttlMode;
  RTL_TRDBG(TRACE_DETAIL, "new requestContext_t (This:0x%.8x)\n", This);
  return This;
}

/**
 * @brief Destructor for dynamic allocation
 * @anchor requestContext_t_newFree
 * @param This the class instance
 */
void requestContext_t_newFree(requestContext_t *This)
{
  RTL_TRDBG(TRACE_DETAIL, "requestContext_t::newFree (This:0x%.8x)\n", This);
  if (This->requestParameters)
  {
    free(This->requestParameters);
  }
  free(This);
}

/**
 * @brief Decrease the instance time to live.
 * @anchor requestContext_t_tic
 * Remove 1 second to remaining time before considering the request has timed out.
 * @param This the class instance
 * @param requestSent indicates if the request has been sent or if the request is queued
 * waiting for being sent.
 * @return the remaining time to live. If less than 0, the request has timed out.
 */
int requestContext_t_tic (requestContext_t *This, bool requestSent)
{
  if ((requestSent) || (AZAP_REQ_TTL_START_AT_API == This->timeToLive))
  {
    This->timeToLive--;
  }
  return This->timeToLive;
}

