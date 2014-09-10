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
/* dIa.h -- dIa
 *
 * Copyright (C) 2012 Actility
 *
 * This file is part of the dIa library libdIa. Please see
 * README for terms of use.
 */

#ifndef _DIA_H_
#define _DIA_H_

#include <coap.h>
#include <coap_time.h>
#include <debug.h>
#include "hashtbl.h"

typedef struct {
	char *name;
	char *val;
} Opt;

typedef unsigned char boolean;
#ifndef TRUE
#define TRUE (boolean)1
#define FALSE (boolean)0
#endif

#define COAP_MAX_TOKEN_VALUE 99999999

typedef struct {
	unsigned long request;
	unsigned long block1, block2;
	unsigned long complete;
	unsigned long response;
	unsigned long coapreset;
	unsigned long cb_diaRetrieveRequest;
	unsigned long cb_diaCreateRequest;
	unsigned long cb_diaUpdateRequest;
	unsigned long cb_diaDeleteRequest;
	unsigned long cb_diaCreateResponse;
	unsigned long cb_diaRetrieveResponse;
	unsigned long cb_diaUpdateResponse;
	unsigned long cb_diaDeleteResponse;
	unsigned long cb_diaErrorResponse;
	unsigned long diaRetrieveRequest;
	unsigned long diaCreateRequest;
	unsigned long diaUpdateRequest;
	unsigned long diaDeleteRequest;
	unsigned long diaCreateResponse;
	unsigned long diaRetrieveResponse;
	unsigned long diaUpdateResponse;
	unsigned long diaDeleteResponse;
	unsigned long reject;
	unsigned long alloc_error;
} dia_stat_t;

typedef struct {
	coap_context_t	*transport_ctx;
	coap_pdu_t	*response;		// valid only when piggybacking
	struct dia_callbacks_s	*callbacks;
	int		maxRead;
	unsigned int	tid;
	boolean		piggy;			// internal flag indicating that a response should be piggybacked
	boolean		blocking;		// Indicate blocking socket read
	boolean		run;			// flag used by JNI to stop the reading thread
	boolean		isProxied;
//	void		*userdata;
//	coap_block_t	block;
	pthread_mutex_t iq_CS;
	char    iq_CSDone;
	HASHTBL *peertbl;
	HASHTBL *tidtbl;
	dia_stat_t	stat;
	char	*proxyAddr;
} dia_context_t;


void dia_freeContext (dia_context_t *ctx);

void dia_nextTimer(dia_context_t *ctx, struct timeval *tv);

void dia_input(dia_context_t *ctx);

void diaProxy(dia_context_t *dia, char *proxyAddr);

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

} dia_callbacks_t;

char *get_ip_fam(const struct sockaddr *sa);
char *get_ip_str(const struct sockaddr *sa, char *s, size_t maxlen);

int dia_dns_lookup(const char *server, struct sockaddr *dst);
int coap_dns_lookup2(const char *host, unsigned short port, coap_address_t *dst);
int coap_dns_lookup(str host, unsigned short port, coap_address_t *dst);

int dia_getFd(dia_context_t *ctx);
void dia_setNonBlocking (dia_context_t *dia, int maxRead);
void dia_setBlocking (dia_context_t *dia);

void dia_setTransactionID (dia_context_t *dia, int tid);
void dia_set_log_level(int log_level);

void dia_setCallbacks(dia_context_t *dia, dia_callbacks_t *callbacks);

dia_context_t * dia_createContext(const char *node, const char *port, dia_callbacks_t *callbacks);

char    *dia_getVersion();

void	dia_EnterCS(dia_context_t *dia);
void	dia_LeaveCS(dia_context_t *dia);

void dia_setTraceFunction (void (*fct)(int level, char *message));
void dia_withLoss(dia_context_t *dia, char loss);

boolean diaIsProxied(dia_context_t *dia);

int dIa_split_uri(unsigned char *str_var, size_t len, coap_uri_t *uri);

#endif /* _DIA_H_ */
