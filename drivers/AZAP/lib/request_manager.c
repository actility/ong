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
//** File : ./lib/request_manager.c
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


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>

#include "rtlbase.h"
#include "rtllist.h"

#include "azap_types.h"
#include "azap_api_zcl.h"
#include "azap_api.h"
#include "uart_buf_control.h"
#include "azap_zcl_attr_val.h"
#include "azap_attribute.h"
#include "azap_endpoint.h"
#include "format_znp.h"
#include "format_znp_zdo.h"
#include "format_znp_af.h"
#include "znp_start_sm.h"

#include "postponed_request_elts.h"
#include "request_context.h"
#include "request_manager.h"
#include "port_context.h"
#include "azap_api_impl.h"

#define MAX_CC2531_SIMULTANEOUS_TOTAL_REQ     10
#define MAX_CC2531_SIMULTANEOUS_PER_NODE_REQ  3
#define MAX_CC2531_SIMULTANEOUS_PER_EP_REQ    1

/*****************************************************************************/
/* Prototypes for local functions */
static void requestManager_t_init(requestManager_t *This);
static void requestManager_t_age1sAndLookForTimedOutReq(requestManager_t *This,
    struct list_head *list, portContext_t *portCtxt, bool requestSent);
static void requestManager_t_lookForPostponedReqToSend(requestManager_t *This,
    portContext_t *portCtxt);

/*****************************************************************************/
/* Constructors / destructor */

/* static allocation */
/**
 * @brief Initialize the requestManager_t struct
 * @anchor requestManager_t_init
 * @param This the class instance
 */
static void requestManager_t_init(requestManager_t *This)
{
  RTL_TRDBG(TRACE_DETAIL, "requestManager_t::init\n");

  /* Assign the pointers on functions */
  This->tic = requestManager_t_tic;
  This->addPendingRequest = requestManager_t_addPendingRequest;
  This->addPostponedRequest = requestManager_t_addPostponedRequest;
  This->removePending = requestManager_t_removePending;
  This->removePendingAf = requestManager_t_removePendingAf;
  This->readyToSend = requestManager_t_readyToSend;
  This->readyToSendDoublonCheck = requestManager_t_readyToSendDoublonCheck;
  This->sendRequest = requestManager_t_sendRequest;
  This->callbackOnFailure = requestManager_t_callbackOnFailure;
  /* Initialize the members */
  INIT_LIST_HEAD(&This->postponedRequests);
  INIT_LIST_HEAD(&This->pendingRequests);
}

/**
 * @brief do static allocation of a requestManager_t.
 * @anchor requestManager_t_create
 * @return the copy of the object
 */
requestManager_t requestManager_t_create()
{
  requestManager_t This;
  requestManager_t_init(&This);
  This.free = requestManager_t_free;
  RTL_TRDBG(TRACE_DETAIL, "requestManager_t::create (This:0x%.8x)\n", &This);
  return This;
}

/**
 * @brief Destructor for static allocation
 * @anchor requestManager_t_free
 * @param This the class instance
 */
void requestManager_t_free(requestManager_t *This)
{
  requestContext_t *ctxt;
  RTL_TRDBG(TRACE_DETAIL, "requestManager_t::free (This:0x%.8x)\n", This);
  while ( ! list_empty(&This->postponedRequests))
  {
    ctxt = list_entry(This->postponedRequests.next, requestContext_t, chainLink);
    list_del(&ctxt->chainLink);
    ctxt->free(ctxt);
  }
  while ( ! list_empty(&This->pendingRequests))
  {
    ctxt = list_entry(This->pendingRequests.next, requestContext_t, chainLink);
    list_del(&ctxt->chainLink);
    ctxt->free(ctxt);
  }
}

/* dynamic allocation */
/**
 * @brief do dynamic allocation of a requestManager_t.
 * @return the copy of the object
 */
requestManager_t *new_requestManager_t()
{
  requestManager_t *This = malloc(sizeof(requestManager_t));
  if (!This)
  {
    return NULL;
  }
  requestManager_t_init(This);
  This->free = requestManager_t_newFree;
  RTL_TRDBG(TRACE_DETAIL, "new requestManager_t (This:0x%.8x)\n", This);
  return This;
}

/**
 * @brief Destructor for dynamic allocation
 * @anchor requestManager_t_newFree
 * @param This the class instance
 */
void requestManager_t_newFree(requestManager_t *This)
{
  requestContext_t *ctxt;
  RTL_TRDBG(TRACE_DETAIL, "requestManager_t::newFree (This:0x%.8x)\n", This);
  while ( ! list_empty(&This->postponedRequests))
  {
    ctxt = list_entry(This->postponedRequests.next, requestContext_t, chainLink);
    list_del(&ctxt->chainLink);
    ctxt->free(ctxt);
  }
  while ( ! list_empty(&This->pendingRequests))
  {
    ctxt = list_entry(This->pendingRequests.next, requestContext_t, chainLink);
    list_del(&ctxt->chainLink);
    ctxt->free(ctxt);
  }
  free(This);
}

/* implementation methods */
/**
 * @brief Invoked at each elapsed second to process timed out request.
 * @anchor requestManager_t_tic
 * @param This the class instance
 * @param portCtxt The working port context.
 */
void requestManager_t_tic(requestManager_t *This, portContext_t *portCtxt)
{
  requestManager_t_age1sAndLookForTimedOutReq(This, &This->postponedRequests,
      portCtxt, false);
  requestManager_t_age1sAndLookForTimedOutReq(This, &This->pendingRequests,
      portCtxt, true);

  requestManager_t_lookForPostponedReqToSend(This, portCtxt);
}

/**
 * @brief Add a request in the list of pending requests.
 * @anchor requestManager_t_addPendingRequest
 * @param This the class instance
 * @param issuerContext a placeholder where the issuer can store anything he needs,
 * and which will provided by AZAP in call back method.
 * @param nwkAddrDest the network address of the remote node targeted by the request.
 * @param type the kind of ZigBee request.
 * @param clusterId The cluster identifier on which the request refers to. It is
 * only relevant for ZCL command requests.
 * @param clusterDir The cluster direction the request implements. It is
 * only relevant for ZCL command requests.
 * @param manufacturerCode The manufacturer code the request implements in case of
 * manufacturer specific attribute or command, 0 otherwise. It is
 * only relevant for ZCL command requests.
 * @param remoteEndPointId The end point identifier on the targeted node.
 * @param expectedCmdCode The command code of the ZCL message expected in return. It is
 * only relevant when type is AZAP_ZCL_OUTGOING_CMD or AZAP_ZCL_OUTGOING_EXT_CMD.
 * @param timeoutValue the request maximum duration.
 * @param ttlMode the TTL request management model (@see AZAP_REQ_TTL_MGMT_MODE).
 * @return the added request context.
 */
requestContext_t *requestManager_t_addPendingRequest(
  requestManager_t *This, void *issuerContext, azap_uint16 nwkAddrDest,
  AZAP_REQUEST_TYPE type, azap_uint16 clusterId, azap_uint8 clusterDir,
  azap_uint16 manufacturerCode, azap_uint8 remoteEndPointId,
  azap_uint8 expectedCmdCode, int timeoutValue, AZAP_REQ_TTL_MGMT_MODE ttlMode)
{
  RTL_TRDBG(TRACE_INFO, "requestManager_t::addPendingRequest (This:0x%.8x) "
      "(type:%s) (nwkAddr:0x%.4x) (clusterId:0x%.4x) (EP:0x%.2x)\n", This,
      AZAP_REQUEST_TYPE_toString(type), nwkAddrDest, clusterId, remoteEndPointId);

  requestContext_t *newElt = new_requestContext_t(timeoutValue, ttlMode);
  newElt->issuerContext = issuerContext;
  newElt->type = type;
  newElt->nwkAddrDest = nwkAddrDest;
  newElt->clusterId = clusterId;
  newElt->clusterDir = clusterDir;
  newElt->manufacturerCode = manufacturerCode;
  newElt->remoteEndp = remoteEndPointId;
  newElt->expectedCmdCode = expectedCmdCode;

  list_add_tail(&newElt->chainLink, &This->pendingRequests);
  return newElt;
}

/**
 * @brief Add a request in the list of postponed requests.
 * @anchor requestManager_t_addPostponedRequest
 * @param This the class instance
 * @param issuerContext a placeholder where the issuer can store anything he needs,
 * and which will provided by AZAP in call back method.
 * @param nwkAddrDest the network address of the remote node targeted by the request.
 * @param type the kind of ZigBee request.
 * @param clusterId The cluster identifier on which the request refers to. It is
 * only relevant for ZCL command requests.
 * @param clusterDir The cluster direction the request implements. It is
 * only relevant for ZCL command requests.
 * @param manufacturerCode The manufacturer code the request implements in case of
 * manufacturer specific attribute or command, 0 otherwise. It is
 * only relevant for ZCL command requests.
 * @param remoteEndPointId The end point identifier on the targeted node.
 * @param expectedCmdCode The command code of the ZCL message expected in return. It is
 * only relevant when type is AZAP_ZCL_OUTGOING_CMD or AZAP_ZCL_OUTGOING_EXT_CMD.
 * @param requestParameters The request parameters.
 * @param timeoutValue the request maximum duration.
 * @param ttlMode the TTL request management model (@see AZAP_REQ_TTL_MGMT_MODE).
 * @return the added request context.
 */
requestContext_t *requestManager_t_addPostponedRequest(
  requestManager_t *This, void *issuerContext, azap_uint16 nwkAddrDest,
  AZAP_REQUEST_TYPE type, azap_uint16 clusterId, azap_uint8 clusterDir,
  azap_uint16 manufacturerCode, azap_uint8 remoteEndPointId,
  azap_uint8 expectedCmdCode, postponedRequestElts_t *requestParameters,
  int timeoutValue, AZAP_REQ_TTL_MGMT_MODE ttlMode)
{
  RTL_TRDBG(TRACE_INFO, "requestManager_t::addPostponedRequest (This:0x%.8x) "
      "(type:%s) (nwkAddr:0x%.4x) (clusterId:0x%.4x) (EP:0x%.2x)\n", This,
      AZAP_REQUEST_TYPE_toString(type), nwkAddrDest, clusterId, remoteEndPointId);

  requestContext_t *newElt = new_requestContext_t(timeoutValue, ttlMode);
  newElt->issuerContext = issuerContext;
  newElt->type = type;
  newElt->nwkAddrDest = nwkAddrDest;
  newElt->clusterId = clusterId;
  newElt->clusterDir = clusterDir;
  newElt->manufacturerCode = manufacturerCode;
  newElt->remoteEndp = remoteEndPointId;
  newElt->expectedCmdCode = expectedCmdCode;
  newElt->requestParameters = requestParameters;

  list_add_tail(&newElt->chainLink, &This->postponedRequests);
  return newElt;
}

/**
 * @brief Correlate the received response with its pending request context, and free it.
 * @anchor requestManager_t_removePending
 * @param This the class instance
 * @param nwkAddrSrc the network address from where the response was received.
 * @param type the kind of ZigBee request.
 * @param clusterId The cluster identifier on which the request refers to. It is
 * only relevant for ZCL command requests.
 * @param remoteEndPointId The end point identifier that has sent this response.
 * @param cmdCode The command code of the received ZCL message. It is
 * only relevant when type is AZAP_ZCL_OUTGOING_CMD or AZAP_ZCL_OUTGOING_EXT_CMD.
 * @return The request context that was removed from the list of pending request if
 * found, NULL otherwise.
 * NOTE: this is then up to the issuer to free the returned context.
 */
requestContext_t *requestManager_t_removePending(requestManager_t *This,
    azap_uint16 nwkAddrSrc, AZAP_REQUEST_TYPE type, azap_uint16 clusterId,
    azap_uint8 remoteEndPointId, azap_uint8 cmdCode)
{
  RTL_TRDBG(TRACE_INFO, "requestManager_t::removePending (This:0x%.8x) "
      "(type:%s) (nwkAddr:0x%.4x) (clusterId:0x%.4x) (EP:0x%.2x)\n", This,
      AZAP_REQUEST_TYPE_toString(type), nwkAddrSrc, clusterId, remoteEndPointId);

  struct list_head *link;
  requestContext_t *elt;
  requestContext_t *toDelete = NULL;
  list_for_each (link, &This->pendingRequests)
  {
    elt = list_entry(link, requestContext_t, chainLink);
    if ((type == elt->type) &&
        (nwkAddrSrc == elt->nwkAddrDest) &&
        (clusterId == elt->clusterId) &&
        ((0 == remoteEndPointId) || (remoteEndPointId == elt->remoteEndp)) &&
        (cmdCode == elt->expectedCmdCode))
    {
      toDelete = elt;
    }
  }
  if (toDelete)
  {
    list_del(&toDelete->chainLink);
  }

  return toDelete;
}

/**
 * @brief Correlate AF_DATA_CONFIRM with fail status with pending context, and free it.
 * @anchor requestManager_t_removePendingAf
 * @param This the class instance
 * @param transId the AF exchange transaction identifier
 * @return The request context that was removed from the list of pending request if
 * found, NULL otherwise.
 * NOTE: this is then up to the issuer to free the returned context.
 */
requestContext_t *requestManager_t_removePendingAf(requestManager_t *This,
    azap_uint8 transId)
{
  struct list_head *link;
  requestContext_t *elt;
  requestContext_t *toDelete = NULL;
  list_for_each (link, &This->pendingRequests)
  {
    elt = list_entry(link, requestContext_t, chainLink);
    if (transId == elt->transactionId)
    {
      toDelete = elt;
    }
  }
  if (toDelete)
  {
    RTL_TRDBG(TRACE_INFO, "requestManager_t::removePendingAf (This:0x%.8x) "
        "(type:%s) (nwkAddr:0x%.4x) (clusterId:0x%.4x) (EP:0x%.2x)\n", This,
        AZAP_REQUEST_TYPE_toString(toDelete->type), toDelete->nwkAddrDest,
        toDelete->clusterId, toDelete->remoteEndp);

    list_del(&toDelete->chainLink);
  }
  return toDelete;
}

/**
 * @brief Look if the ZStack is ready to process a new request to a given nwkAddrDest.
 * The ZStack running on the hardware device has a limited number of request it can process
 * simultaneously, and can only handle a limited number of simultaneous request to each
 * node.
 * @anchor requestManager_t_readyToSend
 * @param This the class instance
 * @param nwkAddrDest the shot address of the request targeted node.
 * @param remoteEndPointId The end point identifier that has sent this response.
 * @return true if the ZStack can handle a new request to that destination, false otherwise.
 */
bool requestManager_t_readyToSend(requestManager_t *This, azap_uint16 nwkAddrDest,
    azap_uint8 remoteEndPointId)
{
  short nbTotalReq = 0;
  short nbReqOnDest = 0;
  short nbReqOnEp = 0;
  bool res = false;
  struct list_head *link;
  requestContext_t *elt;
  list_for_each (link, &This->pendingRequests)
  {
    elt = list_entry(link, requestContext_t, chainLink);
    nbTotalReq++;
    if (nwkAddrDest == elt->nwkAddrDest)
    {
      nbReqOnDest++;
      if ((0 != remoteEndPointId) && (remoteEndPointId == elt->remoteEndp))
      {
        nbReqOnEp++;
      }
    }
  }

//  RTL_TRDBG(TRACE_FULL, "requestManager_t::readyToSend (This:0x%.8x) "
//    "(nwkAddr:0x%.4x) (nbTotalReq:%d) (nbReqOnDest:%d) (nbReqOnEp:%d)\n",
//    This, nwkAddrDest, nbTotalReq, nbReqOnDest, nbReqOnEp);

  if ((nbTotalReq < MAX_CC2531_SIMULTANEOUS_TOTAL_REQ) &&
      (nbReqOnDest < MAX_CC2531_SIMULTANEOUS_PER_NODE_REQ) &&
      (nbReqOnEp < MAX_CC2531_SIMULTANEOUS_PER_EP_REQ))
  {
    res = true;
  }
  return res;
}

/**
 * @brief Look if the ZStack is ready to process a new request checking for doublon.
 * The ZStack running on the hardware device has a limited number of request it can process
 * simultaneously, and can only handle a limited number of simultaneous request to each
 * node.
 * @anchor requestManager_t_readyToSendDoublonCheck
 * @param This the class instance
 * @param type the kind of ZigBee request.
 * @param nwkAddrDest the shot address of the request targeted node.
 * @param clusterId The cluster identifier on which the request refers to. It is
 * only relevant for ZCL command requests.
 * @param remoteEndPointId The end point identifier that has sent this response.
 * @param cmdCode The command code of the received ZCL message. It is
 * only relevant when type is AZAP_ZCL_OUTGOING_CMD or AZAP_ZCL_OUTGOING_EXT_CMD.
 * @return true if the ZStack can handle a new request to that destination, false otherwise.
 */
bool requestManager_t_readyToSendDoublonCheck(requestManager_t *This,
    azap_uint16 nwkAddrDest, AZAP_REQUEST_TYPE type, azap_uint16 clusterId,
    azap_uint8 remoteEndPointId, azap_uint8 cmdCode)
{
  short nbTotalReq = 0;
  short nbReqOnDest = 0;
  short nbReqOnEp = 0;
  bool res = false;
  bool isDoublon = false;
  struct list_head *link;
  requestContext_t *elt;
  list_for_each (link, &This->pendingRequests)
  {
    elt = list_entry(link, requestContext_t, chainLink);
    nbTotalReq++;
    if (nwkAddrDest == elt->nwkAddrDest)
    {
      nbReqOnDest++;
      if ((0 != remoteEndPointId) && (remoteEndPointId == elt->remoteEndp))
      {
        nbReqOnEp++;
      }
    }
    if ((type == elt->type) &&
        (nwkAddrDest == elt->nwkAddrDest) &&
        (clusterId == elt->clusterId) &&
        (remoteEndPointId == elt->remoteEndp) &&
        (cmdCode == elt->expectedCmdCode))
    {
      isDoublon = true;
    }

  }

  RTL_TRDBG(TRACE_DETAIL, "requestManager_t::readyToSendDoublonCheck (This:0x%.8x) "
      "(nwkAddr:0x%.4x) (nbTotalReq:%d) (nbReqOnDest:%d) (nbReqOnEp:%d) (isDoublon:%d) "
      "(type:%s) (clusterId:0x%.4x) (EP:0x%.2x)\n", This, nwkAddrDest, nbTotalReq,
      nbReqOnDest, nbReqOnEp, isDoublon, AZAP_REQUEST_TYPE_toString(type), clusterId,
      remoteEndPointId);

  if ((!isDoublon) &&
      (nbTotalReq < MAX_CC2531_SIMULTANEOUS_TOTAL_REQ) &&
      (nbReqOnDest < MAX_CC2531_SIMULTANEOUS_PER_NODE_REQ) &&
      (nbReqOnEp < MAX_CC2531_SIMULTANEOUS_PER_EP_REQ))
  {
    res = true;
  }
  return res;
}

/**
 * @brief Do send a postponed request over ZNP interface.
 * @anchor bool requestManager_t_sendRequest
 * @param This the class instance
 * @param elt The request context to work with.
 * @param portCtxt The working port context.
 */
bool requestManager_t_sendRequest(requestManager_t *This,
    requestContext_t *elt, portContext_t *portCtxt)
{
  bool res = false;
  bool specific = false;
  azap_uint16 zdoSReqCode = 0;
  if (! elt)
  {
    return false;
  }
  if (! elt->requestParameters)
  {
    return false;
  }

  enum
  {
    REQ_ZDO_FAMILY = 0,
    REQ_ZCL_FAMILY,
  } requestFamily = REQ_ZDO_FAMILY;

  switch (elt->type)
  {
      // ZDO
    case AZAP_ZDO_NWK_ADDR_REQ:
    {
      requestFamily = REQ_ZDO_FAMILY;
      zdoSReqCode = MT_ZDO_NWK_ADDR_SREQ;
    }
    break;
    case AZAP_ZDO_IEEE_ADDR_REQ:
    {
      requestFamily = REQ_ZDO_FAMILY;
      zdoSReqCode = MT_ZDO_IEEE_ADDR_SREQ;
    }
    break;
    case AZAP_ZDO_NODE_DESC_REQ:
    {
      requestFamily = REQ_ZDO_FAMILY;
      zdoSReqCode = MT_ZDO_NODE_DESC_SREQ;
    }
    break;
    case AZAP_ZDO_POWER_DESC_REQ:
    {
      requestFamily = REQ_ZDO_FAMILY;
      zdoSReqCode = MT_ZDO_POWER_DESC_SREQ;
    }
    break;
    case AZAP_ZDO_ACTIVE_EP_REQ:
    {
      requestFamily = REQ_ZDO_FAMILY;
      zdoSReqCode = MT_ZDO_ACTIVE_EP_SREQ;
    }
    break;
    case AZAP_ZDO_SIMPLE_DESC_REQ:
    {
      requestFamily = REQ_ZDO_FAMILY;
      zdoSReqCode = MT_ZDO_SIMPLE_DESC_SREQ;
    }
    break;
    case AZAP_ZDO_BIND_REQ:
    {
      requestFamily = REQ_ZDO_FAMILY;
      zdoSReqCode = MT_ZDO_BIND_SREQ;
    }
    break;
    case AZAP_ZDO_UNBIND_REQ:
    {
      requestFamily = REQ_ZDO_FAMILY;
      zdoSReqCode = MT_ZDO_UNBIND_SREQ;
    }
    break;
    case AZAP_ZDO_MGMT_RTG_REQ:
    {
      requestFamily = REQ_ZDO_FAMILY;
      zdoSReqCode = MT_ZDO_MGMT_RTG_SREQ;
    }
    break;
    case AZAP_ZDO_MGMT_BIND_REQ:
    {
      requestFamily = REQ_ZDO_FAMILY;
      zdoSReqCode = MT_ZDO_MGMT_BIND_SREQ;
    }
    break;
    case AZAP_ZDO_MGMT_PERMIT_JOIN_REQ:
    {
      requestFamily = REQ_ZDO_FAMILY;
      zdoSReqCode = MT_ZDO_MGMT_PERMIT_JOIN_SREQ;
    }
    break;
    case AZAP_ZDO_MGMT_LEAVE_REQ:
    {
      requestFamily = REQ_ZDO_FAMILY;
      zdoSReqCode = MT_ZDO_MGMT_LEAVE_SREQ;
    }
    break;

    // ZCL
    case AZAP_ZCL_READ_CMD:
    {
      requestFamily = REQ_ZCL_FAMILY;
      specific = false;
    }
    break;
    case AZAP_ZCL_WRITE_CMD:
    {
      requestFamily = REQ_ZCL_FAMILY;
      specific = false;
    }
    break;
    case AZAP_ZCL_CONFIGURE_REPORTING_CMD:
    {
      requestFamily = REQ_ZCL_FAMILY;
      specific = false;
    }
    break;
    case AZAP_ZCL_READ_REPORTING_CONF_CMD:
    {
      requestFamily = REQ_ZCL_FAMILY;
      specific = false;
    }
    break;
    case AZAP_ZCL_DISCOVER_CMD:
    {
      requestFamily = REQ_ZCL_FAMILY;
      specific = false;
    }
    break;
    // FFS
    case AZAP_ZCL_REPORT_CMD:
    {
      requestFamily = REQ_ZCL_FAMILY;
      specific = false;
    }
    break;
    case AZAP_ZCL_OUTGOING_CMD:
    case AZAP_ZCL_OUTGOING_EXT_CMD:
    {
      requestFamily = REQ_ZCL_FAMILY;
      specific = true;
    }
    break;
  }

  switch (requestFamily)
  {
    case REQ_ZCL_FAMILY:
    {
      res = znpSendZclCommand(portCtxt->tx, elt->nwkAddrDest, elt->remoteEndp,
          elt->requestParameters->localEndp, elt->requestParameters->cmdCode, specific,
          elt->clusterId, elt->clusterDir,
          elt->manufacturerCode, elt->requestParameters->dataLen,
          elt->requestParameters->cmdData, &elt->transactionId);
    }
    break;
    case REQ_ZDO_FAMILY:
    {
      res = znpSend(portCtxt->tx, zdoSReqCode, elt->requestParameters->dataLen,
          elt->requestParameters->cmdData);
    }
    break;
  }

  return res;
}

/**
 * @brief The request has failed or has timed out, we need to invoke callback to inform customer.
 * @anchor requestManager_t_callbackOnFailure
 * @param This the class instance
 * @param elt The request context to work with.
 * @param portCtxt The working port context.
 */
void requestManager_t_callbackOnFailure(requestManager_t *This,
    requestContext_t *elt, portContext_t *portCtxt)
{
  if (! elt)
  {
    return;
  }
  if (! portCtxt)
  {
    return;
  }

  azap_uint8 defaultZdoFailureCode = ZDO_STATUS_TIMEOUT;

  RTL_TRDBG(TRACE_API, "<<< CALLBACK callbackOnFailure (requestManager:0x%.8x) "
      "(type:%s) (nwkAddr:0x%.4x) (clusterId:0x%.4x) (EP:0x%.2x)\n", This,
      AZAP_REQUEST_TYPE_toString(elt->type), elt->nwkAddrDest, elt->clusterId,
      elt->remoteEndp);
  switch (elt->type)
  {
    case AZAP_ZDO_NWK_ADDR_REQ:
      azapNbZdoRequestsFailed[portCtxt->portNumber] ++;
      portCtxt->pCallbacks->azapZdpNwkAddrRspCb(portCtxt->portNumber,
          elt->issuerContext, elt->nwkAddrDest, defaultZdoFailureCode,
          NULL, 0, 0, 0, NULL);
      break;
    case AZAP_ZDO_IEEE_ADDR_REQ:
      azapNbZdoRequestsFailed[portCtxt->portNumber] ++;
      portCtxt->pCallbacks->azapZdpIeeeAddrRspCb(portCtxt->portNumber,
          elt->issuerContext, elt->nwkAddrDest, defaultZdoFailureCode,
          NULL, 0, 0, 0, NULL);
      break;
    case AZAP_ZDO_NODE_DESC_REQ:
      azapNbZdoRequestsFailed[portCtxt->portNumber] ++;
      portCtxt->pCallbacks->azapZdpNodeDescRspCb(portCtxt->portNumber,
          elt->issuerContext, elt->nwkAddrDest, defaultZdoFailureCode,
          0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
      break;
    case AZAP_ZDO_POWER_DESC_REQ:
      azapNbZdoRequestsFailed[portCtxt->portNumber] ++;
      portCtxt->pCallbacks->azapZdpPowerDescRspCb(portCtxt->portNumber,
          elt->issuerContext, elt->nwkAddrDest, defaultZdoFailureCode,
          0, 0, 0, 0, 0);
      break;
    case AZAP_ZDO_ACTIVE_EP_REQ:
      azapNbZdoRequestsFailed[portCtxt->portNumber] ++;
      portCtxt->pCallbacks->azapZdpActiveEPRspCb(portCtxt->portNumber,
          elt->issuerContext, elt->nwkAddrDest, defaultZdoFailureCode,
          0, 0, NULL);
      break;
    case AZAP_ZDO_SIMPLE_DESC_REQ:
      azapNbZdoRequestsFailed[portCtxt->portNumber] ++;
      portCtxt->pCallbacks->azapZdpSimpleDescRspCb(portCtxt->portNumber,
          elt->issuerContext, elt->nwkAddrDest, defaultZdoFailureCode,
          0, 0, 0, 0, 0, 0, NULL, 0, NULL);
      break;
    case AZAP_ZDO_BIND_REQ:
      azapNbZdoRequestsFailed[portCtxt->portNumber] ++;
      portCtxt->pCallbacks->azapZdpBindRspCb(portCtxt->portNumber,
          elt->issuerContext, elt->nwkAddrDest, defaultZdoFailureCode);
      break;
    case AZAP_ZDO_UNBIND_REQ:
      azapNbZdoRequestsFailed[portCtxt->portNumber] ++;
      portCtxt->pCallbacks->azapZdpUnbindRspCb(portCtxt->portNumber,
          elt->issuerContext, elt->nwkAddrDest, defaultZdoFailureCode);
      break;
    case AZAP_ZDO_MGMT_RTG_REQ:
      azapNbZdoRequestsFailed[portCtxt->portNumber] ++;
      portCtxt->pCallbacks->azapZdpMgmtRtgRspCb(portCtxt->portNumber,
          elt->issuerContext, elt->nwkAddrDest, defaultZdoFailureCode,
          0, 0, 0, NULL);
      break;
    case AZAP_ZDO_MGMT_BIND_REQ:
      azapNbZdoRequestsFailed[portCtxt->portNumber] ++;
      portCtxt->pCallbacks->azapZdpMgmtBindRspCb(portCtxt->portNumber,
          elt->issuerContext, elt->nwkAddrDest, defaultZdoFailureCode,
          0, 0, 0, NULL);
      break;
    case AZAP_ZDO_MGMT_PERMIT_JOIN_REQ:
      azapNbZdoRequestsFailed[portCtxt->portNumber] ++;
      portCtxt->pCallbacks->azapZdpMgmtPermitJoinRspCb(portCtxt->portNumber,
          elt->issuerContext, elt->nwkAddrDest, defaultZdoFailureCode);
      break;
    case AZAP_ZDO_MGMT_LEAVE_REQ:
      azapNbZdoRequestsFailed[portCtxt->portNumber] ++;
      portCtxt->pCallbacks->azapZdpMgmtLeaveRspCb(portCtxt->portNumber,
          elt->issuerContext, elt->nwkAddrDest, defaultZdoFailureCode);
      break;
    case AZAP_ZCL_READ_CMD:
      azapNbZclRequestsFailed[portCtxt->portNumber] ++;
      portCtxt->pCallbacks->azapZclReadCmdRspCb(portCtxt->portNumber,
          elt->issuerContext, elt->nwkAddrDest, elt->remoteEndp, elt->clusterId,
          elt->clusterDir, elt->manufacturerCode, NULL);
      break;
    case AZAP_ZCL_WRITE_CMD:
      azapNbZclRequestsFailed[portCtxt->portNumber] ++;
      portCtxt->pCallbacks->azapZclWriteCmdRspCb(portCtxt->portNumber,
          elt->issuerContext, elt->nwkAddrDest, elt->remoteEndp, elt->clusterId,
          elt->clusterDir, elt->manufacturerCode, NULL);
      break;
    case AZAP_ZCL_CONFIGURE_REPORTING_CMD:
      azapNbZclRequestsFailed[portCtxt->portNumber] ++;
      portCtxt->pCallbacks->azapZclConfigureReportCmdRspCb(portCtxt->portNumber,
          elt->issuerContext, elt->nwkAddrDest, elt->remoteEndp, elt->clusterId,
          NULL);
      break;
    case AZAP_ZCL_READ_REPORTING_CONF_CMD:
      azapNbZclRequestsFailed[portCtxt->portNumber] ++;
      portCtxt->pCallbacks->azapZclReadReportConfigCmdRspCb(portCtxt->portNumber,
          elt->issuerContext, elt->nwkAddrDest, elt->remoteEndp, elt->clusterId,
          NULL);
      break;
    case AZAP_ZCL_DISCOVER_CMD:
      azapNbZclRequestsFailed[portCtxt->portNumber] ++;
      portCtxt->pCallbacks->azapZclDiscoverCmdRspCb(portCtxt->portNumber,
          elt->issuerContext, elt->nwkAddrDest, elt->remoteEndp, elt->clusterId,
          elt->clusterDir, NULL);
      break;
    case AZAP_ZCL_OUTGOING_CMD:
    case AZAP_ZCL_OUTGOING_EXT_CMD:
      // What can we do then ? invoke azapZclIncomingCmdCb with "expectedCode" ?
      break;
    case AZAP_ZCL_REPORT_CMD:
      // nothing to do
      break;
  }
}


/*   _                 _    __              _   _
 *  | |   ___  __ __ _| |  / _|_  _ _ _  __| |_(_)___ _ _  ___
 *  | |__/ _ \/ _/ _` | | |  _| || | ' \/ _|  _| / _ \ ' \(_-<
 *  |____\___/\__\__,_|_| |_|  \_,_|_||_\__|\__|_\___/_||_/__/
 */

/**
 * @brief Age each request of 1 second more and complete timed out requests.
 * @param This the class instance
 * @param list the list of requests on with working on.
 * @param portCtxt The working port context.
 * @param requestSent specifies if the request has been forwarded to the coordinator or not.
 */
static void requestManager_t_age1sAndLookForTimedOutReq(requestManager_t *This,
    struct list_head *list, portContext_t *portCtxt, bool requestSent)
{
  struct list_head *link;
  struct list_head *pos;
  requestContext_t *elt;
  list_for_each_safe (link, pos, list)
  {
    elt = list_entry(link, requestContext_t, chainLink);
    if (elt->tic(elt, requestSent) <= 0)
    {
      // The request has timed out... need to invoke callback
      requestManager_t_callbackOnFailure(This, elt, portCtxt);
      list_del(link);
      elt->free(elt);
    }
  }
}

/**
 * @brief Look if some of the postponed requests can now be sent.
 * @param This the class instance
 * @param portCtxt The working port context.
 */
static void requestManager_t_lookForPostponedReqToSend(requestManager_t *This,
    portContext_t *portCtxt)
{
  struct list_head *link;
  struct list_head *pos;
  requestContext_t *elt;
  short nbRemainingPostponedReq = 0;

  if (! This->readyToSend(This, 0, 0))
  {
    // No need to go further, the max simultaneous requests threshold is already reached
    return;
  }

  list_for_each_safe (link, pos, &This->postponedRequests)
  {
    elt = list_entry(link, requestContext_t, chainLink);
    if ((This->readyToSendDoublonCheck(This, elt->nwkAddrDest, elt->type, elt->clusterId,
        elt->remoteEndp, elt->expectedCmdCode)) &&
        (This->sendRequest(This, elt, portCtxt)))
    {
      RTL_TRDBG(TRACE_INFO, "requestManager_t::lookForPostponedReqToSend "
          "moving request from postponed to pending list (This:0x%.8x) "
          "(type:%s) (nwkAddr:0x%.4x) (clusterId:0x%.4x) (EP:0x%.2x)\n", This,
          AZAP_REQUEST_TYPE_toString(elt->type), elt->nwkAddrDest,
          elt->clusterId, elt->remoteEndp);
      // remove item from postponed requests
      list_del(link);
      // and add it to pending requests
      list_add_tail(&elt->chainLink, &This->pendingRequests);
    }
    else
    {
      nbRemainingPostponedReq++;
    }
  }
  if (nbRemainingPostponedReq > 0)
  {
    RTL_TRDBG(TRACE_INFO, "requestManager_t::lookForPostponedReqToSend - still %d "
        "request to be sent (This:0x%.8x)\n", nbRemainingPostponedReq, This);
  }
}



