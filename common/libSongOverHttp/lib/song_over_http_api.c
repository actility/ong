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
/* song_over_http_api.c - mIa or dIa over HTTP 
 *
 * Copyright (C) 2014 Actility
 *
 * id $Id$
 * author $Author$
 * version $Revision$
 * lastrevision $Date$
 * modifiedby $LastChangedBy$
 * lastmodified $LastChangedDate$
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <curl/curl.h>
#include <errno.h>
#include <poll.h>
#include <microhttpd.h>

#include "rtlbase.h"

#include "common.h"
#include "song_over_http_api.h"
#include "header_t.h"                                                                               
#include "response_t.h"
#include "cltTransaction_t.h"
#include "cltTransactionsList_t.h"
#include "freeCltTransactionsList_t.h"
#include "httpClient_t.h"
#include "srvTransaction_t.h"
#include "srvTransactionsList_t.h"
#include "httpd_impl.h"
#include "_whatstr.h"

extern int TraceLevel;

struct MHD_Daemon *songHttpDaemon = NULL;
srvTransactionsList_t *pendingSrvTransactions = NULL;
httpClient_t *httpClient = NULL;

/**
 * Initialize the library.
 * @param httpSrvPort the listening port for the server.
 * @param createReqCb the call back method to invoke when receiving a CREATE request.
 * @param retrieveReqCb the call back method to invoke when receiving a UPDATE request.
 * @param updateReqCb the call back method to invoke when receiving a RETRIEVE request.
 * @param deleteReqCb the call back method to invoke when receiving a DELETE request.
 * @return SOH_RC_OK if initialization succeeds, SOH_RC_NOK otherwise.
 */
SOH_RCODE sohInit(int httpSrvPort, PF_SOH_CREATE_REQUEST_CB createReqCb,
  PF_SOH_RETRIEVE_REQUEST_CB retrieveReqCb, PF_SOH_UPDATE_REQUEST_CB updateReqCb, 
  PF_SOH_DELETE_REQUEST_CB deleteReqCb)
{
  short debugEnabled = 0;
  if (TraceLevel >= TRACE_DEBUG)
  {
    debugEnabled = 1;
  }

  httpClient = new_httpClient_t(debugEnabled);
  pendingSrvTransactions = new_srvTransactionsList_t();
  httpdInit(createReqCb, retrieveReqCb, updateReqCb, deleteReqCb);
  songHttpDaemon = MHD_start_daemon (0, httpSrvPort, NULL, NULL,
    &answerToConnection, NULL, 
    MHD_OPTION_NOTIFY_COMPLETED, requestCompleted, NULL, 
    MHD_OPTION_END);
  if (NULL == songHttpDaemon)
  {
    RTL_TRDBG(TRACE_ERROR, "Failed to start libmicrohttpd !!!\n");
    return SOH_RC_NOK;
  }

  return SOH_RC_OK;
}

/**
 * Uninitialize the library.
 * @return SOH_RC_OK if initialization succeeds, SOH_RC_NOK otherwise. 
 */
SOH_RCODE sohUninit()
{
  MHD_stop_daemon (songHttpDaemon);
  httpClient->free(httpClient);
  pendingSrvTransactions->free(pendingSrvTransactions);

  songHttpDaemon = NULL;
  httpClient = NULL;
  return SOH_RC_OK;
}

/**
 * Get the library version string.
 * @return a formatted string that provides information regarding the library versioning.           
 */
char *sohGetVersion()
{
  return libSongOverHttp_whatStr;
}


/**
 * Do library regular processing.
 */
void sohClockMs()
{
  // client processing
  httpClient->processMessages(httpClient);
  httpClient->trackTimedOutMessages(httpClient);
  // server processing
  httpdClockMs();
}

/**
 * Send a request.
 * @param method the request method to use for that request.
 * @param nsclProxyHostPort the host and port (':'-seprated) of the NSCL that will proxy 
 * the request. May be NULL. In this case the request is directly sent using 'targetID'.
 * @param reqEntity the requesting entity
 * @param targetID the targetID query parameter ; should be a full URI, and when 
 * targeting an ONG, must start with the link URI provided by the ONG when it registered
 * on the RMS.
 * @param content the request content.
 * @param len the request content length.
 * @param contentType the request content type (most of the time is "application/xml")
 * @param optHeaders the additional headers to encode in the outgoing request.
 * @param tid the transactionId (by reference).
 * @param issuerData a pointer on issuer data structure, passed transparently over the library.
 * @param respCb pointer on the issuer callback function.
 * @return SOH_RC_OK if the request can be sent successfully, any other SOH_RCODE otherwise.
 */
static SOH_RCODE sohRequest(SOH_METHOD method, char *nsclProxyHostPort, char *reqEntity, 
  char *targetID, unsigned char *content, size_t len, char *contentType, 
  sohHeader_t **optHeaders, char **tid, void *issuerData, void *respCb)
{
  cltTransaction_t *trans;
  int i = 0;

  *tid = httpClient->createReq(httpClient);
  trans = httpClient->getTransaction(httpClient, *tid);

  if (! trans) return SOH_RC_NOK;

  trans->setCompletionParam(trans, respCb, issuerData);
  trans->setReqMethod(trans, method);
  trans->setReqUrl(trans, targetID);

  if (nsclProxyHostPort)
  {
    trans->setReqProxy(trans, nsclProxyHostPort);
  } 
 
  trans->setReqRequestingEntity(trans, reqEntity);
  if (optHeaders)
  {
    while (optHeaders[i])
    {
      trans->addReqHeader(trans, optHeaders[i]->name, optHeaders[i]->values);
      i++;
    }
  }

  if (content && len)
  {
    trans->addReqHeader(trans, HEADER_CONTENT_TYPE, contentType);
    trans->setReqBody(trans, content, len);
  }

  if (! trans->sendReq(trans))
  {
    return SOH_RC_NOK;
  }
  return SOH_RC_OK;
}

/**
 * Send a CREATE request.
 * @param nsclProxyHostPort the host and port (':'-seprated) of the NSCL that will proxy 
 * the request. May be NULL. In this case the request is directly sent using 'targetID'.
 * @param reqEntity the requesting entity
 * @param targetID the targetID query parameter ; should be a full URI, and when 
 * targeting an ONG, must start with the link URI provided by the ONG when it registered
 * on the RMS.
 * @param content the request content.
 * @param len the request content length.
 * @param contentType the request content type (most of the time is "application/xml")
 * @param optHeaders the additional headers to encode in the outgoing request.
 * @param tid the transactionId (by reference).
 * @param issuerData a pointer on issuer data structure, passed transparently over the library.
 * @return SOH_RC_OK if the request can be sent successfully, any other SOH_RCODE otherwise.
 */
SOH_RCODE sohCreateRequest(char *nsclProxyHostPort, char *reqEntity, char *targetID, 
  unsigned char *content, size_t len, char *contentType, sohHeader_t **optHeaders, 
  char **tid, void *issuerData, PF_SOH_CREATE_RESPONSE_CB createRespCb)
{
  return sohRequest(SOH_CREATE, nsclProxyHostPort, reqEntity, targetID, content, len, 
    contentType, optHeaders, tid, issuerData, createRespCb);
}

/**
 * Send a RETRIEVE request.
 */
SOH_RCODE sohRetrieveRequest(char *nsclProxyHostPort, char *reqEntity, char *targetID, 
  sohHeader_t **optHeaders, char **tid, void *issuerData, 
  PF_SOH_RETRIEVE_RESPONSE_CB retrieveRespCb)
{
  return sohRequest(SOH_RETRIEVE, nsclProxyHostPort, reqEntity, targetID, NULL, 0, NULL, 
    optHeaders, tid, issuerData, retrieveRespCb);
}

/**
 * Send an UPDATE request.
 */
SOH_RCODE sohUpdateRequest(char *nsclProxyHostPort, char *reqEntity, char *targetID,
  unsigned char *content, size_t len, char *contentType, sohHeader_t **optHeaders, 
  char **tid, void *issuerData, PF_SOH_UPDATE_RESPONSE_CB updateRespCb)
{
  return sohRequest(SOH_UPDATE, nsclProxyHostPort, reqEntity, targetID, content, len, 
    contentType, optHeaders, tid, issuerData, updateRespCb);
}

/**
 * Send a DELETE request.
 */
SOH_RCODE sohDeleteRequest(char *nsclProxyHostPort, char *reqEntity, char *targetID,
  sohHeader_t **optHeaders, char **tid, void *issuerData, 
  PF_SOH_DELETE_RESPONSE_CB deleteRespCb)
{
  return sohRequest(SOH_DELETE, nsclProxyHostPort, reqEntity, targetID, NULL, 0, NULL, 
    optHeaders, tid, issuerData, deleteRespCb);
}


/**
 * Send response to a received SONG CREATE request.
 * @param statusCode the status code to use to respond.
 * @param resourceURI the full URI for the created resource.
 * @param content the response content.
 * @param len the response content length.
 * @param contentType the response content type (most of the time is "application/xml")
 * @param optHeaders the additional headers to encode in the outgoing response.
 * @param tid the transactionId
 */
void sohCreateResponse(SOH_STATUS_CODE statusCode, char *resourceURI, unsigned char *content,
  size_t len, char *contentType, sohHeader_t **optHeaders, char *tid)
{
  srvTransaction_t *trans = pendingSrvTransactions->get(pendingSrvTransactions, tid);
  if (! trans) return;
  if (trans->method != SOH_CREATE) return;

  trans->sendResp(trans, statusCode, resourceURI, content, len, contentType, optHeaders);

  // once responded, the transaction will be removed from pending transactions list 
  // in requestCompleted callback (see httpd_impl.c)
}

/**
 * Send a response to a received SONG RETRIEVE request.
 */
void sohRetrieveResponse(SOH_STATUS_CODE statusCode, unsigned char *content,
  size_t len, char *contentType, sohHeader_t **optHeaders, char *tid)
{
  srvTransaction_t *trans = pendingSrvTransactions->get(pendingSrvTransactions, tid);
  if (! trans) return;
  if (trans->method != SOH_RETRIEVE) return;

  trans->sendResp(trans, statusCode, NULL, content, len, contentType, optHeaders);

  // once responded, the transaction will be removed from pending transactions list 
  // in requestCompleted callback (see httpd_impl.c)
}

/**
 * Send a response to a received SONG UPDATE request.
 */
void sohUpdateResponse (SOH_STATUS_CODE statusCode, unsigned char *content,
  size_t len, char *contentType, sohHeader_t **optHeaders, char *tid)
{
  srvTransaction_t *trans = pendingSrvTransactions->get(pendingSrvTransactions, tid);
  if (! trans) return;
  if (trans->method != SOH_UPDATE) return;

  trans->sendResp(trans, statusCode, NULL, content, len, contentType, optHeaders);

  // once responded, the transaction will be removed from pending transactions list 
  // in requestCompleted callback (see httpd_impl.c)
}

/**
 * Send a response to a received SONG DELETE request.
 */
void sohDeleteResponse(SOH_STATUS_CODE statusCode, unsigned char *content,
  size_t len, char *contentType, sohHeader_t **optHeaders, char *tid)
{
  srvTransaction_t *trans = pendingSrvTransactions->get(pendingSrvTransactions, tid);
  if (! trans) return;
  if (trans->method != SOH_DELETE) return;

  trans->sendResp(trans, statusCode, NULL, content, len, contentType, optHeaders);

  // once responded, the transaction will be removed from pending transactions list 
  // in requestCompleted callback (see httpd_impl.c)
}



