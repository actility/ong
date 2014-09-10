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
#ifndef _MICRODIA_H_
#define _MICRODIA_H_

#include "dialist.h"

typedef struct {
	unsigned int request;
	unsigned int block1, block2;
	unsigned int complete;
	unsigned int response;
	unsigned int coapreset;
	unsigned int cb_diaRetrieveRequest;
	unsigned int cb_diaCreateRequest;
	unsigned int cb_diaUpdateRequest;
	unsigned int cb_diaDeleteRequest;
	unsigned int cb_diaCreateResponse;
	unsigned int cb_diaRetrieveResponse;
	unsigned int cb_diaUpdateResponse;
	unsigned int cb_diaDeleteResponse;
	unsigned int cb_diaErrorResponse;
	unsigned int diaRetrieveRequest;
	unsigned int diaCreateRequest;
	unsigned int diaUpdateRequest;
	unsigned int diaDeleteRequest;
	unsigned int diaCreateResponse;
	unsigned int diaRetrieveResponse;
	unsigned int diaUpdateResponse;
	unsigned int diaDeleteResponse;
	unsigned int reject;
	unsigned int alloc_error;
} dia_stat_t;

typedef unsigned long coap_tick_t;

#ifndef COAP_MAX_PDU_SIZE
#define COAP_MAX_PDU_SIZE           1400 /* maximum size of a CoAP PDU */
#endif /* COAP_MAX_PDU_SIZE */

#ifndef COAP_MAX_REQUESTS
#define COAP_MAX_REQUESTS 50
#endif

#ifndef COAP_DEFAULT_RESPONSE_TIMEOUT
#define COAP_DEFAULT_RESPONSE_TIMEOUT  2 /* response timeout in seconds */
#endif

#ifndef COAP_DEFAULT_MAX_RETRANSMIT
#define COAP_DEFAULT_MAX_RETRANSMIT    4 /* max number of retransmissions */
#endif

#define COAP_MAX_TOKEN_VALUE	99999999
#define COAP_MAX_TID		0xffff

#define COAP_REQUEST_GET       1
#define COAP_REQUEST_POST      2
#define COAP_REQUEST_PUT       3
#define COAP_REQUEST_DELETE    4

#define	DIA_REQUEST_RETRIEVE	1
#define	DIA_REQUEST_CREATE	2
#define	DIA_REQUEST_UPDATE	3
#define	DIA_REQUEST_DELETE	4

typedef struct {
	struct list_head head;
	int coap_tid;
	long dia_tid;
	int typeReq;
	char	inlist;
	int	cnt;
	int	when;
#ifdef COAP_RETRANSMIT
	unsigned char	raw[COAP_MAX_PDU_SIZE];
	int	raw_size;
#endif
} dia_request_t;

typedef struct {
	struct dia_callbacks_s	*callbacks;
	unsigned int	dia_tid;
	unsigned int	coap_tid;
	struct list_head reqList;
	dia_stat_t	stat;
	dia_request_t _gRequests[COAP_MAX_REQUESTS];
	char	with_loss;
	void	*user_data;
} dia_context_t;

struct dia_timeval {
	int	tv_sec;
	int	tv_usec;
};

typedef struct {
	char *name;
	char *val;
} Opt;


int diaCreateRequest (dia_context_t *dia, char *reqEntity, char *targetID, void *ctt, int len, char *cttType,
	Opt *optAttr, Opt *optHeader, int *tid);

void diaCreateResponse(dia_context_t *dia, char *scode, char *resourceURI, void *ctt, int len, char* cttType,
	Opt *optHeader, int tid, char *peer);

int diaRetrieveRequest(dia_context_t *ctx, char *reqEntity, char *targetID,
	Opt *optAttr, Opt *optHeader, int *tid);

void diaRetrieveResponse(dia_context_t *dia,
	char *scode, void *ctt, int len, char* cttType,
	Opt *optHeader, int tid, char *peer);

int diaUpdateRequest(dia_context_t *dia, char *reqEntity, char *targetID, void *ctt, int len, char *cttType,
	Opt *optAttr, Opt *optHeader, int *tid);

void diaUpdateResponse (dia_context_t *dia, char *scode, void *ctt, int len, char* cttType,
	Opt *optHeader, int tid, char *peer);

int diaDeleteRequest(dia_context_t *dia, char *reqEntity, char *targetID,
	Opt *optAttr, Opt *optHeader, int *tid);

void diaDeleteResponse(dia_context_t *dia, char *scode, Opt *optHeader, int tid, char *peer);


typedef struct dia_callbacks_s {
	int (*cb_diaRetrieveRequest)(dia_context_t *dia,
		char *reqEntity, char *targetID,
		Opt *optAttr, Opt *optHeader, int tid, char *peer);

	void (*cb_diaRetrieveResponse)(dia_context_t *dia, char *scode, void *ctt, int len, char *cttType, Opt *optHeader, int tid);

	int (*cb_diaCreateRequest)(dia_context_t *dia, char *reqEntity, char *targetID, void *ctt, int len, char *cttType,
		Opt *optAttr, Opt *optHeader, int tid, char *peer);

	void (*cb_diaCreateResponse)(dia_context_t *dia, char *scode, char *resourceURI, void *ctt, int len, char* cttType,
		Opt *optHeader, int tid);

	int (*cb_diaUpdateRequest)(dia_context_t *dia, char *reqEntity, char *targetID, void *ctt, int len, char *cttType,
		Opt *optAttr, Opt *optHeader, int tid, char *peer);

	void (*cb_diaUpdateResponse)(dia_context_t *dia, char *scode, void *ctt, int len, char* cttType, Opt *optHeader, int tid);

	int (*cb_diaDeleteRequest)(dia_context_t *dia, char *reqEntity, char *targetID,
		Opt *optAttr, Opt *optHeader, int tid, char *peer);

	void (*cb_diaDeleteResponse)(dia_context_t *dia, char *scode, Opt *optHeader, int tid);

	void (*cb_diaErrorResponse)(dia_context_t *dia, void *ctt, int len, char* cttType, Opt *optHeader, int tid);

	int (*cb_sendToNetwork)(dia_context_t *dia, unsigned char *buf, int sz);

} dia_callbacks_t;

dia_context_t * dia_createContext(dia_callbacks_t *callbacks);
int dIa_input (dia_context_t *dia, unsigned char *msg, int len);
void dIa_status(dia_context_t *dia);

int diaCreateRequest (dia_context_t *dia,
	char *reqEntity, char *targetID, void *ctt, int len, char *cttType,
	Opt *optAttr, Opt *optHeader, int *tid);
void diaCreateResponse(dia_context_t *dia, char *scode, char *resourceURI, void *ctt, int len, char* cttType,
	Opt *optHeader, int tid, char *peer);
int diaRetrieveRequest(dia_context_t *dia, char *reqEntity, char *targetID,
		Opt *optAttr, Opt *optHeader, int *tid);
void diaRetrieveResponse(dia_context_t *dia,
	char *scode, void *ctt, int len, char* cttType,
	Opt *optHeader, int tid, char *peer);
int diaUpdateRequest(dia_context_t *dia, char *reqEntity, char *targetID, void *ctt, int len, char *cttType,
	Opt *optAttr, Opt *optHeader, int *tid);
void diaUpdateResponse (dia_context_t *dia, char *scode, void *ctt, int len, char* cttType,
	Opt *optHeader, int tid, char *peer);
int diaDeleteRequest(dia_context_t *dia, char *reqEntity, char *targetID,
	Opt *optAttr, Opt *optHeader, int *tid);
void diaDeleteResponse(dia_context_t *dia, char *scode, Opt *optHeader, int tid, char *peer);

void dIa_nextTimer(dia_context_t *dia, struct dia_timeval *tv);
void dia_withLoss(dia_context_t *dia, char loss);


int diaEncodeRequest (dia_context_t *dia, int type, char *reqEntity, char *targetID, char *cttType, int *tid);
unsigned char *diaGetBuffer(int *sz);
int diaEncodeResponse(dia_context_t *dia, int typeResp, char *scode, char *resourceURI, char* cttType, int tid);

#endif
