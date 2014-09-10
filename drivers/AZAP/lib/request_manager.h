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
//** File : ./lib/request_manager.h
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


#ifndef __REQUEST_MANAGER__H_
#define __REQUEST_MANAGER__H_

/**
 * @file request_manager.h
 * @brief The ZStack running on the dongle brings constraints on requests
 * that can be sent simultaneously to the HAN, and to a node in the HAN.
 * The file implements the manager that is in charge of controlling that
 * these constraints are respected. It is also in charge of managing the
 * requests maximum duration, and the potential requests re-emission.
 * By default, the dongle is not able to handle more than 10 requests
 * simultaneously, and no more than 3 requests per targeted node.
 * There is one manager per port implemented via AZAP layer.
 * @author mlouiset
 * @date 2012-08-28
 */

struct portContext_t;

typedef struct requestManager_t
{
  /* methods */
  /** @ref requestManager_t_free */
  void (* free) (struct requestManager_t *);
  /** @ref requestManager_t_tic */
  void (* tic) (struct requestManager_t *, struct portContext_t *portCtxt);

  /** @ref requestManager_t_addPendingRequest */
  requestContext_t *(* addPendingRequest) (struct requestManager_t *,
      void *issuerContext, azap_uint16 nwkAddrDest, AZAP_REQUEST_TYPE type,
      azap_uint16 clusterId, azap_uint8 clusterDir, azap_uint16 manufacturerCode,
      azap_uint8 remoteEndPointId, azap_uint8 expectedCmdCode, int timeoutValue,
      AZAP_REQ_TTL_MGMT_MODE ttlMode);
  /** @ref requestManager_t_addPostponedRequest */
  requestContext_t *(* addPostponedRequest) (struct requestManager_t *,
      void *issuerContext, azap_uint16 nwkAddrDest, AZAP_REQUEST_TYPE type,
      azap_uint16 clusterId, azap_uint8 clusterDir, azap_uint16 manufacturerCode,
      azap_uint8 remoteEndPointId, azap_uint8 expectedCmdCode,
      postponedRequestElts_t *requestParameters, int timeoutValue,
      AZAP_REQ_TTL_MGMT_MODE ttlMode);
  /** @ref requestManager_t_removePending */
  requestContext_t *(* removePending) (struct requestManager_t *,
      azap_uint16 nwkAddrSrc, AZAP_REQUEST_TYPE type, azap_uint16 clusterId,
      azap_uint8 remoteEndPointId, azap_uint8 cmdCode);
  /** @ref requestManager_t_removePendingAf */
  requestContext_t *(* removePendingAf) (struct requestManager_t *,
      azap_uint8 transactionId);
  /** @ref requestManager_t_readyToSend */
  bool (* readyToSend) (struct requestManager_t *, azap_uint16 nwkAddrDest,
      azap_uint8 remoteEndPointId);
  /** @ref requestManager_t_readyToSendDoublonCheck */
  bool (* readyToSendDoublonCheck) (struct requestManager_t *This,
      azap_uint16 nwkAddrDest, AZAP_REQUEST_TYPE type, azap_uint16 clusterId,
      azap_uint8 remoteEndPointId, azap_uint8 cmdCode);
  /** @ref requestManager_t_sendRequest */
  bool (* sendRequest) (struct requestManager_t *, requestContext_t *,
      struct portContext_t *);
  /** @ref requestManager_t_callbackOnFailure */
  void (* callbackOnFailure) (struct requestManager_t *,
      requestContext_t *, struct portContext_t *);

  /** The requests that have not been sent yet. */
  struct list_head postponedRequests;
  /** The requests that were sent, for which a response is expected. */
  struct list_head pendingRequests;

} requestManager_t;

/* static allocation */
requestManager_t requestManager_t_create();
void requestManager_t_free(requestManager_t *This);

/* dynamic allocation */
requestManager_t *new_requestManager_t();
void requestManager_t_newFree(requestManager_t *This);

/* implementation methods */
void requestManager_t_tic(requestManager_t *, struct portContext_t *);

requestContext_t *requestManager_t_addPendingRequest(requestManager_t *,
    void *issuerContext, azap_uint16 nwkAddrDest, AZAP_REQUEST_TYPE type,
    azap_uint16 clusterId, azap_uint8 clusterDir, azap_uint16 manufacturerCode,
    azap_uint8 remoteEndPointId, azap_uint8 expectedCmdCode, int timeoutValue,
    AZAP_REQ_TTL_MGMT_MODE ttlMode);
requestContext_t *requestManager_t_addPostponedRequest(requestManager_t *,
    void *issuerContext,  azap_uint16 nwkAddrDest, AZAP_REQUEST_TYPE type,
    azap_uint16 clusterId, azap_uint8 clusterDir, azap_uint16 manufacturerCode,
    azap_uint8 remoteEndPointId, azap_uint8 expectedCmdCode,
    postponedRequestElts_t *requestParameters, int timeoutValue,
    AZAP_REQ_TTL_MGMT_MODE ttlMode);
requestContext_t *requestManager_t_removePending(requestManager_t *,
    azap_uint16 nwkAddrSrc, AZAP_REQUEST_TYPE type, azap_uint16 clusterId,
    azap_uint8 remoteEndPointId, azap_uint8 cmdCode);
requestContext_t *requestManager_t_removePendingAf(requestManager_t *,
    azap_uint8 transactionId);
bool requestManager_t_readyToSend(requestManager_t *,
    azap_uint16 nwkAddrDest, azap_uint8 remoteEndPointId);
bool requestManager_t_readyToSendDoublonCheck(requestManager_t *,
    azap_uint16 nwkAddrDest, AZAP_REQUEST_TYPE type, azap_uint16 clusterId,
    azap_uint8 remoteEndPointId, azap_uint8 cmdCode);
bool requestManager_t_sendRequest(requestManager_t *,
    requestContext_t *, struct portContext_t *);
void requestManager_t_callbackOnFailure(requestManager_t *,
    requestContext_t *, struct portContext_t *);

#endif


