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
/* song_over_http_api.h - mIa or dIa over HTTP 
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

#ifndef __SOH_OVER_HTTP_API__H_
#define __SOH_OVER_HTTP_API__H_


typedef struct {
	char *name;
	char *values;
} sohHeader_t;

typedef enum _SOH_RCODE {
  SOH_RC_OK = 0,
  SOH_RC_NOK,
  SOH_RC_NOT_IMPLEMENTED,
  SOH_RC_NO_ROUTE,
  SOH_RC_CANNOT_CONNECT,

} SOH_RCODE;


typedef enum _SOH_STATUS_CODE {

  SOH_SC_OK = 200,
  SOH_SC_CREATED = 201,
  SOH_SC_ACCEPTED = 202,
  SOH_SC_NO_CONTENT = 204,

  SOH_SC_BAD_REQUEST = 400,
  SOH_SC_UNAUTHORIZED = 401,
  SOH_SC_FORBIDDEN = 403,
  SOH_SC_NOT_FOUND = 404,
  SOH_SC_METHOD_NOT_ALLOWED = 405,
  SOH_SC_CONFLICT = 409,
    
  SOH_SC_INTERNAL_SERVER_ERROR = 500,
  SOH_SC_NOT_IMPLEMENTED = 501,
  SOH_SC_SERVICE_UNAVAILABLE = 503,
  SOH_SC_GATEWAY_TIMEOUT = 504,

} SOH_STATUS_CODE;


// callback method prototypes
// call back method for HTTP server
typedef SOH_RCODE (*PF_SOH_CREATE_REQUEST_CB) (char *reqEntity, char *targetID, 
  unsigned char *content, size_t len, char *contentType, sohHeader_t **optHeaders, char *tid);

typedef SOH_RCODE (*PF_SOH_RETRIEVE_REQUEST_CB) (char *reqEntity, char *targetID, 
  sohHeader_t **optHeaders, char *tid);

typedef SOH_RCODE (*PF_SOH_UPDATE_REQUEST_CB) (char *reqEntity, char *targetID, 
  unsigned char *content, size_t len, char *contentType, sohHeader_t **optHeaders, char *tid);

typedef SOH_RCODE (*PF_SOH_DELETE_REQUEST_CB) (char *reqEntity, char *targetID, 
  sohHeader_t **optHeaders, char *tid);

// call back method for HTTP client
typedef void (*PF_SOH_CREATE_RESPONSE_CB) (SOH_STATUS_CODE statusCode, 
  char *resourceURI, unsigned char *content, size_t len, char *contentType,
  sohHeader_t **optHeaders, char *tid, void *issuerData);

typedef void (*PF_SOH_RETRIEVE_RESPONSE_CB) (SOH_STATUS_CODE statusCode, 
  unsigned char *content, size_t len, char *contentType,
  sohHeader_t **optHeaders, char *tid, void *issuerData);

typedef void (*PF_SOH_UPDATE_RESPONSE_CB) (SOH_STATUS_CODE statusCode, 
  unsigned char *content, size_t len, char *contentType,
  sohHeader_t **optHeaders, char *tid, void *issuerData);

typedef void (*PF_SOH_DELETE_RESPONSE_CB) (SOH_STATUS_CODE statusCode, 
  unsigned char *content, size_t len, char *contentType,
  sohHeader_t **optHeaders, char *tid, void *issuerData);

typedef void (*PF_DUMP_INFO_CB) (void *issuerHandle, char *msg, ...);

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
  PF_SOH_DELETE_REQUEST_CB deleteReqCb);

/**
 * Uninitialize the library.
 * @return SOH_RC_OK if initialization succeeds, SOH_RC_NOK otherwise.
 */
SOH_RCODE sohUninit();

/**
 * Get the library version string.
 * @return a formatted string that provides information regarding the library versioning.
 */
char *sohGetVersion();

/**
 * Do library regular processing.
 */
void sohClockMs();

/**
 * Send a CREATE request.
 * @param reqEntity the requesting entity
 * @param targetID the targetID query parameter ; should be a full URI, and when 
 * targeting an ONG, must start with the link URI provided by the ONG when it registered
 * on the RMS.
 * @param content the request content.
 * @param len the request content length.
 * @param contentType the request content type (most of the time is "application/xml")
 * @param optHeaders the additional headers to encode in the outgoing request.
 * @param tid the transactionId (by reference)
 * @param issuerData a pointer on issuer data structure, passed transparently over the library.
 * @param createRespCb the callback that is to be invoked when receiving the response.
 * @return SOH_RC_OK if the request can be sent successfully, any other SOH_RCODE otherwise.
 */
SOH_RCODE sohCreateRequest(char *reqEntity, char *targetID, unsigned char *content, size_t len,
  char *contentType, sohHeader_t **optHeaders, char **tid, void *issuerData,
  PF_SOH_CREATE_RESPONSE_CB createRespCb);

/**
 * Send a RETRIEVE request.
 */
SOH_RCODE sohRetrieveRequest(char *reqEntity, char *targetID, sohHeader_t **optHeaders, 
  char **tid, void *issuerData, PF_SOH_RETRIEVE_RESPONSE_CB retrieveRespCb);

/**
 * Send an UPDATE request.
 */
SOH_RCODE sohUpdateRequest(char *reqEntity, char *targetID, unsigned char *content, size_t len,
  char *contentType, sohHeader_t **optHeaders, char **tid, void *issuerData,
  PF_SOH_UPDATE_RESPONSE_CB updateRespCb);

/**
 * Send a DELETE request.
 */
SOH_RCODE sohDeleteRequest(char *reqEntity, char *targetID, sohHeader_t **optHeaders, 
  char **tid, void *issuerData, PF_SOH_DELETE_RESPONSE_CB deleteRespCb);

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
  size_t len, char *contentType, sohHeader_t **optHeaders, char *tid);

/**
 * Send a response to a received SONG RETRIEVE request.
 */
void sohRetrieveResponse(SOH_STATUS_CODE statusCode, unsigned char *content,
  size_t len, char *contentType, sohHeader_t **optHeaders, char *tid);

/**
 * Send a response to a received SONG UPDATE request.
 */
void sohUpdateResponse (SOH_STATUS_CODE statusCode, unsigned char *content,
  size_t len, char *contentType, sohHeader_t **optHeaders, char *tid);

/**
 * Send a response to a received SONG DELETE request.
 */
void sohDeleteResponse(SOH_STATUS_CODE statusCode, unsigned char *content,
  size_t len, char *contentType, sohHeader_t **optHeaders, char *tid);


#endif /* __SOH_OVER_HTTP_API__H_ */

