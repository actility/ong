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
/* dIa.c -- 
 *
 * Copyright (C) 2012 Actility
 *
 * This file is part of the dIa library libdIa. Please see
 * README for terms of use.
 **
 * According to TS 102 921 v1.1.2 stage3, D.2.5 page 353 :
 *	RequestingEntity shall be included in CoAP uri-query header option in the format of the type AnyURI.
 *	   ?RequestingEntity=<value>
 *	targetID should be stored in Proxy-Uri
 *
 */

#include <stdio.h>
#include <strings.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <ctype.h>
#include <sys/select.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/stat.h>
#include <dirent.h>
#include <errno.h>
#include <signal.h>
#include <fcntl.h>
#include <pthread.h>

#include "dIa.h"
#include <rtlbase.h>
#include <rtllist.h>


void	dia_InitCS(dia_context_t *dia) {
	int	err;
	if	( dia->iq_CSDone )
		return;
	if ((err=pthread_mutex_init(&dia->iq_CS,NULL)))
	{
		rtl_trace(0,"'%s':%d error=%d while initializing mutex !!!\n",
			__FILE__,__LINE__,err);
		return;
	}
	dia->iq_CSDone	= 1;
}

void	*dia_GetCS(dia_context_t *dia) {
	if (!dia->iq_CSDone)
		return	NULL;
	return	&dia->iq_CS;
}

void	dia_EnterCS(dia_context_t *dia) {
	int	err;
	if (!dia->iq_CSDone)
	{
		rtl_trace(0,"'%s':%d error mutex not initialized !!!\n",
			__FILE__,__LINE__);
		return;
	}
	if ((err=pthread_mutex_lock(&dia->iq_CS)))
	{
		rtl_trace(0,"'%s':%d error=%d while locking mutex !!!\n",
			__FILE__,__LINE__,err);
	}
}

void	dia_LeaveCS(dia_context_t *dia) {
	if (!dia->iq_CSDone)
		return;
	pthread_mutex_unlock(&dia->iq_CS);
}


void copy_options (coap_pdu_t *from, coap_pdu_t *to) {
	coap_opt_filter_t opts;
	coap_opt_iterator_t opt_iter;

	memset(opts, 0, sizeof(coap_opt_filter_t));
	coap_option_setb(opts, COAP_OPTION_CONTENT_TYPE);
#if LIBCOAP_VERSION < 4
	coap_option_setb(opts, COAP_OPTION_TOKEN);
#endif
	coap_option_setb(opts, COAP_OPTION_PROXY_URI);
	coap_option_setb(opts, COAP_OPTION_URI_HOST);
	coap_option_setb(opts, COAP_OPTION_URI_PORT);
	coap_option_setb(opts, COAP_OPTION_URI_PATH);
	coap_option_setb(opts, COAP_OPTION_URI_QUERY);

	coap_opt_t *option;
	coap_option_iterator_init(from, &opt_iter, opts);
	while	((option=coap_option_next(&opt_iter))) {
		coap_add_option(to, opt_iter.type, COAP_OPT_LENGTH(option), COAP_OPT_VALUE(option));
	}
}

//------------------------- Structures -------------------

typedef struct {
	size_t length;                /* length of data */
	unsigned char *data;         /* the actual contents */
} coap_payload_t;

typedef struct {
	struct list_head head;
	coap_payload_t payload;
	coap_block_t	block2;
	coap_block_t	block1;
	int tid;
	int typeReq;
	boolean early_nego;
} dia_request_t;

typedef struct {
	coap_address_t peer;
	char	*peer_str;
	struct list_head reqList;
} dia_box_t;


//------------------------- Box object -------------------

dia_box_t *alloc_box() {
	//rtl_trace (5, "alloc_box\n");
	dia_box_t *p = (dia_box_t *)calloc(sizeof(dia_box_t), 1);
	return	p;
}


dia_box_t *
getBoxWithString(dia_context_t *dia, char *peer) {
	rtl_trace (5, "getBoxWithString (%s)\n", peer);
	dia_box_t *p = hashtbl_get(dia->peertbl, peer);
	return	p;
}

dia_box_t *
getBox(dia_context_t *dia, coap_address_t *peer, boolean create) {
	char tmp[100];
	get_ip_str(&peer->addr.sa, tmp, sizeof(tmp));
	sprintf (tmp+strlen(tmp), ",%d", ntohs(peer->addr.sin.sin_port));

	rtl_trace (5, "getBox (%s)\n", tmp);
	dia_box_t *p = hashtbl_get(dia->peertbl, tmp);
	if	(!p && create) {
		p	= alloc_box();
		if	(!p) return NULL;
		p->peer = *peer;
		p->peer_str = strdup(tmp);
		INIT_LIST_HEAD (&p->reqList);
		hashtbl_insert (dia->peertbl, tmp, p);
	}
	return	p;
}

//------------------------- Payload object -------------------

void free_payload(coap_payload_t *p) {
	if	(p->length == 0)
		return;
	rtl_trace (6, "free_payload %d\n", p->length);
	if	(p->data) coap_free (p->data);
	p->length = 0;
	p->data = NULL;
}

void alloc_payload (void *ptr, int len, coap_payload_t *p) {
	free_payload(p);
	rtl_trace (6, "alloc_payload %d\n", len);
	p->data	= malloc(len);
	memcpy (p->data, ptr, len);
	p->length = len;
}

void
storeFragment(coap_payload_t *payload, void *ctt, int len) {
	rtl_trace (5, "Store fragment of len %ld\n", len);
	if	(!payload->length)
		payload->data	= malloc(len);
	else
		payload->data	= realloc(payload->data, payload->length + len);
	memcpy (payload->data+payload->length, ctt, len);
	payload->length	+= len;
}


//------------------------- dia_request list  -------------------

dia_request_t * alloc_dr(int tid) {
	dia_request_t *dr = calloc(sizeof(dia_request_t), 1);
	dr->tid	= tid;
	//rtl_trace (5, "alloc_dr tid=%d %p\n", tid, dr);
	return	dr;
}

dia_request_t *search_tid (struct list_head *l, int tid, boolean create) {
	struct list_head *elem;
	dia_request_t *dr;

	//rtl_trace (6, "search_tid(%d)\n", tid);
	list_for_each (elem, l) {
//		dr	= (dia_request_t *)elem;
		dr	= list_entry(elem, dia_request_t, head);
		//rtl_trace (6, "\tsearch_tid(%d)\n", dr->tid);
		if	(dr->tid == tid)
			return dr;
	}
	if	(create) {
		dr = alloc_dr(tid);
		elem = &dr->head;
		list_add_tail (elem, l);
		return dr;
	}
	return	NULL;
}

void free_dr (dia_request_t *dr) {
	//rtl_trace (5, "free_dr %p\n", dr);
	list_del (&dr->head);
	free (dr);
}



void
dump_all(dia_context_t *dia) {
	void fct(char *key, void *data) {
		rtl_trace (5, "\ttid=%s, ptr=%lx\n", key, data);
	}
	rtl_trace (5, "Dump of tidtbl :\n");
	hashtbl_dump(dia->tidtbl, fct);

	void fct2(char *key, void *data) {
		rtl_trace (5, "\tbox=%s, ptr=%lx\n", key, data);
	}
	rtl_trace (5, "Dump of peertbl :\n");
	hashtbl_dump(dia->peertbl, fct2);
}

void
dump_reqs(dia_box_t *p) {
	struct list_head *elem;
	dia_request_t *dr;
	char	tmp[100];

	rtl_trace (5, "Dump of reqList of %s,%d\n", get_ip_str (&p->peer.addr.sa, tmp, sizeof(tmp)),
		ntohs(p->peer.addr.sin.sin_port));
	list_for_each (elem, &p->reqList) {
		dr	= list_entry(elem, dia_request_t, head);
		rtl_trace (5, "\t%d\n", dr->tid);
	}
}

char *coap2dia_code(unsigned int code) {
	switch (code) {
		case COAP_RESPONSE_CODE(201) : return "STATUS_CREATED";
		case COAP_RESPONSE_CODE(202) : return "STATUS_DELETED";
		case COAP_RESPONSE_CODE(204) : return "STATUS_OK";
		case COAP_RESPONSE_CODE(205) : return "STATUS_OK";
		case COAP_RESPONSE_CODE(400) : return "STATUS_BAD_REQUEST";
		case COAP_RESPONSE_CODE(401) : return "STATUS_PERMISSION_DENIED";
		case COAP_RESPONSE_CODE(404) : return "STATUS_NOT_FOUND";
		case COAP_RESPONSE_CODE(405) : return "STATUS_METHOD_NOT_ALLOWED";
		case COAP_RESPONSE_CODE(406) : return "STATUS_NOT_ACCEPTABLE";
		case COAP_RESPONSE_CODE(409) : return "STATUS_CONFLICT";
		case COAP_RESPONSE_CODE(415) : return "STATUS_UNSUPPORTED_MEDIA_TYPE";
		case COAP_RESPONSE_CODE(500) : return "STATUS_INTERNAL_SERVER_ERROR";
		case COAP_RESPONSE_CODE(501) : return "STATUS_NOT_IMPLEMENTED";
		case COAP_RESPONSE_CODE(502) : return "STATUS_BAD_GATEWAY";
		case COAP_RESPONSE_CODE(503) : return "STATUS_SERVICE_UNAVAILABLE";
		case COAP_RESPONSE_CODE(504) : return "STATUS_GATEWAY_TIMEOUT";
		default : return	"STATUS_BAD_REQUEST";
	}
}

unsigned int dia2coap_code(char *scode, int method) {
	if	(!strcmp(scode, "STATUS_CREATED"))		return 201;
	if	(!strcmp(scode, "STATUS_DELETED"))		return 202;
	if	(!strcmp(scode, "STATUS_OK")) {
		if	(method == COAP_REQUEST_PUT)		return 204;
		else						return 205;
	}
	if	(!strcmp(scode, "STATUS_BAD_REQUEST"))		return 400;
	if	(!strcmp(scode, "STATUS_PERMISSION_DENIED"))	return 401;
	if	(!strcmp(scode, "STATUS_NOT_FOUND"))		return 404;
	if	(!strcmp(scode, "STATUS_METHOD_NOT_ALLOWED"))	return 405;
	if	(!strcmp(scode, "STATUS_NOT_ACCEPTABLE"))	return 406;
	if	(!strcmp(scode, "STATUS_CONFLICT"))		return 409;
	if	(!strcmp(scode, "STATUS_UNSUPPORTED_MEDIA_TYPE"))return 415;
	if	(!strcmp(scode, "STATUS_INTERNAL_SERVER_ERROR"))	return 500;
	if	(!strcmp(scode, "STATUS_NOT_IMPLEMENTED"))	return 501;
	if	(!strcmp(scode, "STATUS_BAD_GATEWAY"))		return 502;
	if	(!strcmp(scode, "STATUS_SERVICE_UNAVAILABLE"))	return 503;
	if	(!strcmp(scode, "STATUS_GATEWAY_TIMEOUT"))	return 504;
	return 405;
}

int
startWith(char *buf, char *str) {
	return	!strncmp(buf, str, strlen(str));
}

int
dia2coap_cttType(char *cttType) {
	int type = COAP_MEDIATYPE_TEXT_PLAIN;
	if	(!cttType)	return type;
	if	(startWith(cttType, "text/plain"))		type	= COAP_MEDIATYPE_TEXT_PLAIN;
	else if	(startWith(cttType, "application/link-format"))	type	= COAP_MEDIATYPE_APPLICATION_LINK_FORMAT;
	else if	(startWith(cttType, "application/xml"))		type	= COAP_MEDIATYPE_APPLICATION_XML;
	else if	(startWith(cttType, "application/octet-stream"))	type	= COAP_MEDIATYPE_APPLICATION_OCTET_STREAM;
	else if	(startWith(cttType, "application/rdf+xml"))	type	= COAP_MEDIATYPE_APPLICATION_RDF_XML;
	else if	(startWith(cttType, "application/exi"))		type	= COAP_MEDIATYPE_APPLICATION_EXI;
	else if	(startWith(cttType, "application/json"))		type	= COAP_MEDIATYPE_APPLICATION_JSON;
	return	type;
}

char *
coap2dia_cttType(int type) {
	switch (type) {
	case COAP_MEDIATYPE_TEXT_PLAIN			: return "text/plain"; 
	case COAP_MEDIATYPE_APPLICATION_LINK_FORMAT	: return "application/link-format"; 
	case COAP_MEDIATYPE_APPLICATION_XML		: return "application/xml"; 
	case COAP_MEDIATYPE_APPLICATION_OCTET_STREAM	: return "application/octet-stream"; 
	case COAP_MEDIATYPE_APPLICATION_RDF_XML		: return "application/rdf+xml"; 
	case COAP_MEDIATYPE_APPLICATION_EXI		: return "application/exi"; 
	case COAP_MEDIATYPE_APPLICATION_JSON		: return "application/json"; 
	default: return "text/plain"; 
	}
}

/*
int
peercmp(const struct sockaddr *sa1, const struct sockaddr *sa2) {
	char	tmp1[100], tmp2[100];

	get_ip_str (sa1, tmp1, sizeof(tmp1));
	get_ip_str (sa2, tmp2, sizeof(tmp2));
	return	strcmp(tmp1, tmp2);
}
*/

/*
int protected_memcpy (void *to, void *from, int sz, int maxsz) {
	if	(sz > maxsz)
		sz	= maxsz;
	memcpy (to, from, sz);
	return	sz;
}
*/

int protected_strcat (char *to, char *from, int sz, int maxsz) {
	maxsz	-= strlen(to);
	if	(sz > maxsz)
		sz	= maxsz;
	memcpy (to+strlen(to), from, sz);
	to[strlen(to)+sz] = 0;
	return	sz;
}

void
hnd_request(coap_context_t *ctx, coap_address_t *peer, coap_pdu_t *request, coap_pdu_t *response) {
	char	reqEntity[400+1] = "";
	char	targetID[400+1] = "";
	char	query[400+1] = "";
	Opt	*optAttr = NULL;
	Opt	*optHeader = NULL;
	int	tid;
	coap_opt_iterator_t opt_iter;
	coap_opt_t *option;
	size_t len;
	unsigned char *ctt;
	char	*cttType;
	uint16_t	media_type = 0;
	//boolean _last = FALSE;

	dia_context_t *dia = (dia_context_t *)ctx->userdata;

	rtl_trace (5, "Entering hnd_request\n");
	dia->stat.request	++;

	char buf[40+1] = "";

#if LIBCOAP_VERSION < 4
	option = coap_check_option(request, COAP_OPTION_TOKEN, &opt_iter);
	if (!option) {
		rtl_trace (3, "Error : TOKEN option (%d) not found in CoAP message.\n", COAP_OPTION_TOKEN);
		rtl_trace (3, "Here is the pdu :\n");
		dia->stat.reject	++;
		return;
	}
	//protected_strcat (buf, (char *)COAP_OPT_VALUE(opt_iter.option), COAP_OPT_LENGTH(opt_iter.option), 40);
	protected_strcat (buf, (char *)COAP_OPT_VALUE(option), COAP_OPT_LENGTH(option), 40);
#else
	protected_strcat (buf, (char *)request->hdr->token, request->hdr->token_length, 40);
#endif

	tid	= atoi(buf);

	// 1. Retrieve the box speaking
	dia_box_t *p = getBox(dia, peer, TRUE);
	if (hashtbl_get(dia->tidtbl, buf) == NULL) {
		//rtl_trace (5, "hashtbl_insert tidtbl %s\n", buf);
		hashtbl_insert (dia->tidtbl, buf, p);
	}

	// 2. Allocate a dia_request
	dia_request_t *dr = search_tid (&p->reqList, tid, TRUE);
	dr->early_nego = FALSE;
	//printf ("### REQ %d %p %d\n", tid, dr, dr->typeReq);

	//	Block case A : client ask us for another block (GET)
	if (coap_get_block(request, COAP_OPTION_BLOCK2, &dr->block2)) {
		rtl_trace (5, "Block case A (BLOCK2) client ask us for another block (GET) num=%d m=%d szx=%d len=%d\n",
			dr->block2.num, dr->block2.m, dr->block2.szx, dr->payload.length);
		dia->stat.block2	++;
		//	On est dans le cas ou le client propose une taille (early negociation)
		if	(dr->payload.length == 0) {
			dr->early_nego = TRUE;
			goto	x;
		}
		copy_options (request, response);
		coap_write_block_opt(&dr->block2, COAP_OPTION_BLOCK2, response, dr->payload.length);
		// send next block
		coap_add_block(response, dr->payload.length, dr->payload.data,
			 dr->block2.num, dr->block2.szx);

		if	(dr->block2.m == 0) {
			free_payload (&dr->payload);
			free_dr (dr);
			//rtl_trace (5, "hashtbl_remove tidtbl %s\n", buf);
			hashtbl_remove(dia->tidtbl, (char *)buf);
			dump_all(dia);
			dump_reqs(p);
		}

		response->hdr->code = COAP_RESPONSE_CODE(205);
		// libcoap will automatically send the response
		return;
	}

x:

	coap_get_data(request, &len, &ctt);

	//	Block case B : client send us another block (PUT or POST)
	if (coap_get_block(request, COAP_OPTION_BLOCK1, &dr->block1)) {
		rtl_trace (5, "Block case B (BLOCK1) : client send us another block (PUT or POST) num=%d m=%d szx=%d len=%d\n",
			dr->block1.num, dr->block1.m, dr->block1.szx, dr->payload.length);
		dia->stat.block1	++;

		storeFragment (&dr->payload, ctt, len);

		if	(request->hdr->code == COAP_REQUEST_POST)
			response->hdr->code = COAP_RESPONSE_CODE(201);
		else
			response->hdr->code = COAP_RESPONSE_CODE(204);

		// if more fragments, do nothing
		// libcoap will automatically send the response
		if	(dr->block1.m) {
			rtl_trace (5, "More fragments\n");
			copy_options (request, response);
			// send ACK, requesting next block
			dr->block1.num	++;
			//	Not yet. We accept the client's negociation
			//dr->block1.szx = 6;	// we accept size max, e.g. 1024
			unsigned char buf[4];
			coap_add_option(response, COAP_OPTION_BLOCK1, coap_encode_var_bytes(buf, 
				(dr->block1.num << 4) | dr->block1.szx), buf);
			return;
		}

		rtl_trace (5, "Last fragment\n");
		// last fragment. Message complete
		len	= dr->payload.length;
		ctt	= dr->payload.data;
		//_last	= TRUE;
	}

	// Process complete message
	dia->stat.complete	++;

	query[0] = 0;
	reqEntity[0] = 0;
	if ((option=coap_check_option(request, COAP_OPTION_URI_QUERY, &opt_iter))) {
		int len = COAP_OPT_LENGTH(option);
		char *value = (char *)COAP_OPT_VALUE(option);

		if	(!strncmp(value, "RequestingEntity=", 17))
			protected_strcat (reqEntity, value+17, len-17, 400);
		else
			protected_strcat (query, value, len, 400);

		while ((option=coap_option_next(&opt_iter))) {
			len = COAP_OPT_LENGTH(option);
			value = (char *)COAP_OPT_VALUE(option);

			if	(!strncmp(value, "RequestingEntity=", 17))
				protected_strcat (reqEntity, value+17, len-17, 400);
			else {
				if	(*query)
					protected_strcat (query, "&", 1, 400);
				protected_strcat (query, value, len, 400);
			}
		}
	}

	targetID[0] = 0;
	if ((option=coap_check_option(request, COAP_OPTION_PROXY_URI, &opt_iter))) {
		dia->isProxied = TRUE;
		protected_strcat (targetID, (char *)COAP_OPT_VALUE(option), COAP_OPT_LENGTH(option), 400);
	}
	else {
		dia->isProxied = FALSE;
		char	host[100+1] = "", port[6+1] = "", path[200+1] = "";
		if ((option=coap_check_option(request, COAP_OPTION_URI_HOST, &opt_iter))) {
			protected_strcat (host, (char *)COAP_OPT_VALUE(option), COAP_OPT_LENGTH(option), 100);
		}
		if ((option=coap_check_option(request, COAP_OPTION_URI_PORT, &opt_iter))) {
			int x;
			unsigned char *_p = COAP_OPT_VALUE(option);
			x	= *_p * 256 + *(_p+1);
			sprintf (port, "%d", x);
//			protected_strcat (port, (char *)COAP_OPT_VALUE(option), COAP_OPT_LENGTH(option), 6);
		}
		if ((option=coap_check_option(request, COAP_OPTION_URI_PATH, &opt_iter))) {
			protected_strcat (path, (char *)COAP_OPT_VALUE(option), COAP_OPT_LENGTH(option), 200);
			while ((option=coap_option_next(&opt_iter))) {
				protected_strcat (path, "/", 1, 200);
				protected_strcat (path, (char *)COAP_OPT_VALUE(option), COAP_OPT_LENGTH(option), 200);
			}
		}

		if	(*host) {
			protected_strcat (targetID, "coap://", 7, 400);
			protected_strcat (targetID, host, strlen(host), 400);
		}
		if	(*port) {
			protected_strcat (targetID, ":", 1, 400);
			protected_strcat (targetID, port, strlen(port), 400);
		}
		if	(*path) {
			protected_strcat (targetID, "/", 1, 400);
			protected_strcat (targetID, path, strlen(path), 400);
		}
		if	(*query) {
			protected_strcat (targetID, "?", 1, 400);
			protected_strcat (targetID, query, strlen(query), 400);
		}
	}

	if ((option=coap_check_option(request, COAP_OPTION_CONTENT_TYPE, &opt_iter))) {
		media_type = coap_decode_var_bytes(COAP_OPT_VALUE(option), COAP_OPT_LENGTH(option));
	}
	cttType	= coap2dia_cttType(media_type);
rtl_trace (5, "cttType = %s COAP_OPTION_CONTENT_TYPE=%d\n", cttType, media_type);

	/*	Nous rentrons dans l'appel des callbacks application. L'application peut
		immediatement faire une reponse; dans ce cas on enverra cette reponse dans un ACK (piggybacking) */
	dia->piggy	= TRUE;
	dia->response	= response;

	switch (request->hdr->code) {
	case	COAP_REQUEST_GET :
		if	(dia->callbacks && dia->callbacks->cb_diaRetrieveRequest) {
			rtl_trace (5, "calling cb_diaRetrieveRequest(reqEntity=%s, targetID=%s tid=%d, peer=%s)\n",
				reqEntity, targetID, tid, p->peer_str);
			dia->stat.cb_diaRetrieveRequest	++;
			(*dia->callbacks->cb_diaRetrieveRequest)(dia, reqEntity, targetID, optAttr, optHeader, tid, p->peer_str);
		}
		break;
	case	COAP_REQUEST_POST :
		if	(dia->callbacks && dia->callbacks->cb_diaCreateRequest) {
			rtl_trace (5, "calling cb_diaCreateRequest(reqEntity=%s, targetID=%s tid=%d, peer=%s)\n",
				reqEntity, targetID, tid, p->peer_str);
			dia->stat.cb_diaCreateRequest	++;
			dia->callbacks->cb_diaCreateRequest(dia, reqEntity, targetID, ctt, len, cttType, optAttr, optHeader, tid, p->peer_str);
		}
		break;
	case	COAP_REQUEST_PUT :
		if	(dia->callbacks && dia->callbacks->cb_diaUpdateRequest) {
			rtl_trace (5, "calling cb_diaUpdateRequest(reqEntity=%s, targetID=%s tid=%d, peer=%s)\n",
				reqEntity, targetID, tid, p->peer_str);
			dia->stat.cb_diaUpdateRequest	++;
			dia->callbacks->cb_diaUpdateRequest(dia, reqEntity, targetID, ctt, len, cttType, optAttr, optHeader, tid, p->peer_str);
		}
		break;
	case	COAP_REQUEST_DELETE :
		if	(dia->callbacks && dia->callbacks->cb_diaDeleteRequest) {
			rtl_trace (5, "calling cb_diaDeleteRequest(reqEntity=%s, targetID=%s tid=%d, peer=%s)\n",
				reqEntity, targetID, tid, p->peer_str);
			dia->stat.cb_diaDeleteRequest	++;
			dia->callbacks->cb_diaDeleteRequest(dia, reqEntity, targetID, optAttr, optHeader, tid, p->peer_str);
		}
		break;
	}

	/*	Nous sortons de l'appel des callbacks application. Plus de piggy. */
	dia->piggy	= FALSE;
/*
	if	(_last) {
		free_payload (&dr->payload);
		free_dr (dr);
	}
*/
}

void
hnd_response(struct coap_context_t *ctx, const coap_address_t *remote,
	coap_pdu_t *sent, coap_pdu_t *received, const coap_tid_t id) {
	coap_opt_iterator_t opt_iter;
	coap_opt_t *option;
	size_t len;
	char	*scode, resourceURI[400+1] = "";
//	Opt	*optHeader = NULL;
	int	tid;
	uint16_t	media_type = 0;
	unsigned char *ctt;
	char	*cttType;

	dia_context_t *dia = (dia_context_t *)ctx->userdata;
	dia->stat.response	++;
/*
	if	(!sent) {
		rtl_trace (3, "Error : hnd_response called with sent=NULL !\n");
		dia->stat.reject	++;
		return;
	}
*/
	if	(received == NULL) {
		rtl_trace (5, "Entering hnd_response with REQUEST TIMEOUT\n");
		char buf[40+1] = "";
#if LIBCOAP_VERSION < 4
		if (!sent || !(option=coap_check_option(sent, COAP_OPTION_TOKEN, &opt_iter))) {
			rtl_trace (3, "Error : hnd_response called with sent=NULL !\n");
			dia->stat.reject	++;
			return;
		}
		protected_strcat (buf, (char *)COAP_OPT_VALUE(option), COAP_OPT_LENGTH(option), 40);
#else
		if (!sent) {
			rtl_trace (3, "Error : hnd_response called with sent=NULL !\n");
			dia->stat.reject	++;
			return;
		}
		protected_strcat (buf, (char *)received->hdr->token, received->hdr->token_length, 40);
#endif

		tid	= atoi(buf);

		if	(dia->callbacks && dia->callbacks->cb_diaErrorResponse) {
			rtl_trace (5, "calling cb_diaErrorResponse(tid=%d)\n", tid);
			dia->stat.cb_diaErrorResponse	++;
			dia->callbacks->cb_diaErrorResponse(dia, NULL, 0, "", NULL, tid);
		}
		// 1. Retrieve the box speaking using tid.
		dia_box_t *p = getBox(dia, (coap_address_t *)remote, FALSE);
		if	(!p) {
			rtl_trace (3, "Error : tid %d unknown\n", tid);
			dia->stat.reject	++;
			return;
		}

		// 2. Allocate a dia_request or retrieve it if still exists
		dia_request_t *dr = search_tid (&p->reqList, tid, FALSE);
		if	(dr) {
			free_payload (&dr->payload);
			free_dr (dr);
		}
		return;
	}

	rtl_trace (5, "Entering hnd_response %d.%02d response\n",
		(received->hdr->code >> 5), received->hdr->code & 0x1F);

#if LIBCOAP_VERSION < 4
	if (!(option=coap_check_option(received, COAP_OPTION_TOKEN, &opt_iter))) {
/*
		rtl_trace (3, "Error : TOKEN option (%d) not found in CoAP message.\n", COAP_OPTION_TOKEN);
		rtl_trace (3, "Here is the pdu :\n");
		coap_show_pdu(received);
		dia->stat.reject	++;
*/
		return;
	}

	char buf[40+1] = "";
	protected_strcat (buf, (char *)COAP_OPT_VALUE(option), COAP_OPT_LENGTH(option), 40);
	tid	= atoi(buf);
#else
	char buf[40+1] = "";
	protected_strcat (buf, (char *)received->hdr->token, received->hdr->token_length, 40);
	tid	= atoi(buf);
#endif

	// 1. Retrieve the box speaking
	dia_box_t *p = getBox(dia, (coap_address_t *)remote, FALSE);
	if	(!p) {
//		rtl_trace (3, "Error : peer %s unknown\n", tmp);
		dia->stat.reject	++;
		return;
	}

	// 2. Allocate a dia_request or retrieve it if still exists
	dia_request_t *dr = search_tid (&p->reqList, tid, TRUE);

	switch (received->hdr->type) {
	case COAP_MESSAGE_CON:
		/* acknowledge received response if confirmable (TODO: check Token) */
		coap_send_ack(ctx, remote, received);
		break;
	case COAP_MESSAGE_RST:
		rtl_trace (3, "got CoAP RST\n");
		dia->stat.coapreset	++;
		return;
	default:
		break;
	}

	coap_get_data(received, &len, &ctt);

	//	Block case C : server ask us for another block
	if (coap_get_block(received, COAP_OPTION_BLOCK1, &dr->block1)) {
		rtl_trace (5, "Block case C (BLOCK1) : server ask us for another block num=%d m=%d szx=%d len=%d\n",
			dr->block1.num, dr->block1.m, dr->block1.szx, dr->payload.length);
		dia->stat.block1	++;

		// send next block
		coap_pdu_t *pdu  = coap_new_pdu();
		if	(!pdu) {
			rtl_trace (5, "Memory allocation failed for a new pdu.\n");
			dia->stat.alloc_error	++;
			return;
		}
		pdu->hdr->type = COAP_MESSAGE_CON;
		pdu->hdr->id = coap_new_message_id(ctx);
		pdu->hdr->code = dr->typeReq;
		copy_options (received, pdu);

		coap_write_block_opt(&dr->block1, COAP_OPTION_BLOCK1, pdu, dr->payload.length);
		coap_add_block(pdu, dr->payload.length, dr->payload.data,
			 dr->block1.num, dr->block1.szx);
		if (coap_send_confirmed(dia->transport_ctx, remote, pdu) == COAP_INVALID_TID)
			coap_delete_pdu(pdu);

		return;
	}

	//	Block case D : server send us another block
	if (coap_get_block(received, COAP_OPTION_BLOCK2, &dr->block2)) {
		rtl_trace (5, "Block case D (BLOCK2) : server send us another block num=%d m=%d szx=%d len=%d\n",
			dr->block2.num, dr->block2.m, dr->block2.szx, dr->payload.length);
		dia->stat.block2	++;

		storeFragment (&dr->payload, ctt, len);

		// if more fragments, ask next block
		if	(dr->block2.m) {
			coap_pdu_t *pdu  = coap_new_pdu();
			if	(!pdu) {
				rtl_trace (3, "Memory allocation failed for a new pdu.\n");
				dia->stat.alloc_error	++;
				return;
			}
			pdu->hdr->type = COAP_MESSAGE_CON;
			pdu->hdr->id = coap_new_message_id(ctx);
			pdu->hdr->code = dr->typeReq;
			copy_options (received, pdu);

			dr->block2.num	++;
			//	Not yet. We accept the servers's proposal
			// dr->block2.szx = 6;	// we accept size max, e.g. 1024
			unsigned char buf[4];
			coap_add_option(pdu, COAP_OPTION_BLOCK2, coap_encode_var_bytes(buf, 
				(dr->block2.num << 4) | dr->block2.szx), buf);

			if (coap_send_confirmed(dia->transport_ctx, remote, pdu) == COAP_INVALID_TID)
				coap_delete_pdu(pdu);
			return;
		}

		// last fragment. Message complete
		rtl_trace (5, "Last fragment\n");
		len	= dr->payload.length;
		ctt	= dr->payload.data;
	}

	//	Pass complete message to upper level (application level)
	rtl_trace (5, "Pass complete message to upper level\n");
	dia->stat.complete	++;

	scode	= coap2dia_code(received->hdr->code);

	if ((option=coap_check_option(received, COAP_OPTION_CONTENT_TYPE, &opt_iter))) {
		media_type = coap_decode_var_bytes(COAP_OPT_VALUE(option), COAP_OPT_LENGTH(option));
	}
	cttType	= coap2dia_cttType(media_type);
	rtl_trace (5, "cttType = %s COAP_OPTION_CONTENT_TYPE=%d\n", cttType, media_type);

	//printf ("### RESP %d %p %d\n", tid, dr, dr->typeReq);
	switch (dr->typeReq) {
	case	COAP_REQUEST_POST :
		/*
		o   "STATUS_CREATED" (success)		: 2.01 Created
		o   "STATUS_BAD_REQUEST"		: 4.00 Bad Request
		o   "STATUS_PERMISSION_DENIED"		: 4.01 Unauthorized
		o   "STATUS_NOT_FOUND"			: 4.04 Not Found
		o   "STATUS_METHOD_NOT_ALLOWED"		: 4.05 Method Not Allowed
		o   "STATUS_NOT_ACCEPTABLE"		: 4.06 Not Acceptable
		o   "STATUS_REQUEST_TIMEOUT"		: received == NULL
		o   "STATUS_CONFLICT"			: 4.09
		o   "STATUS_UNSUPPORTED_MEDIA_TYPE"	: 4.15 Unsupported Media Type
		o   "STATUS_INTERNAL_SERVER_ERROR"	: 5.00 Internal Server Error
		o   "STATUS_NOT_IMPLEMENTED"		: 5.01 Not Implemented
		o   "STATUS_BAD_GATEWAY"		: 5.02 Bad Gateway
		o   "STATUS_SERVICE_UNAVAILABLE"	: 5.03 Service Unavailable
		o   "STATUS_GATEWAY_TIMEOUT"		: 5.04 Gateway Timeout
		o   "STATUS_DELETED"			: 2.02 Deleted
		*/
		if	(dia->callbacks && dia->callbacks->cb_diaCreateResponse) {
			if (received && (option=coap_check_option(received, COAP_OPTION_LOCATION_PATH, &opt_iter))) {
				protected_strcat (resourceURI, (char *)COAP_OPT_VALUE(option),
					COAP_OPT_LENGTH(option), 400);
			}

			rtl_trace (5, "calling cb_diaCreateResponse(scode=%s resourceURI=%s tid=%d)\n", scode, resourceURI, tid);
			dia->stat.cb_diaCreateResponse	++;
			dia->callbacks->cb_diaCreateResponse(dia, scode, resourceURI, ctt, len, cttType, NULL, tid);
		}
		break;
	case	COAP_REQUEST_GET :
		/*
		o   "STATUS_OK" (success)		: 2.05 Content
		o   "STATUS_PERMISSION_DENIED"		: 4.01 Unauthorized
		o   "STATUS_NOT_FOUND"			: 4.04 Not Found
		o   "STATUS_METHOD_NOT_ALLOWED"		: 4.05 Method Not Allowed
		o   "STATUS_NOT_ACCEPTABLE"		: 4.06 Not Acceptable
		o   "STATUS_REQUEST_TIMEOUT"		: received == NULL
		o   "STATUS_INTERNAL_SERVER_ERROR"	: 5.00 Internal Server Error
		o   "STATUS_NOT_IMPLEMENTED"		: 5.01 Not Implemented
		o   "STATUS_BAD_GATEWAY"		: 5.02 Bad Gateway
		o   "STATUS_SERVICE_UNAVAILABLE"	: 5.03 Service Unavailable
		o   "STATUS_GATEWAY_TIMEOUT"		: 5.04 Gateway Timeout
		o   "STATUS_DELETED"			: 2.02 Deleted
		*/
		if	(dia->callbacks && dia->callbacks->cb_diaRetrieveResponse) {
			rtl_trace (5, "calling cb_diaRetrieveResponse(scode=%s tid=%d)\n", scode, tid);
			dia->stat.cb_diaRetrieveResponse	++;
			dia->callbacks->cb_diaRetrieveResponse(dia, scode, ctt, len, cttType, NULL, tid);
		}
		break;
	case	COAP_REQUEST_PUT :
		/*
		o   "STATUS_OK" (success)		: 2.04 Changed
		o   "STATUS_BAD_REQUEST"		: 4.00 Bad Request
		o   "STATUS_PERMISSION_DENIED"		: 4.01 Unauthorized
		o   "STATUS_NOT_FOUND"			: 4.04 Not Found
		o   "STATUS_METHOD_NOT_ALLOWED"		: 4.05 Method Not Allowed
		o   "STATUS_NOT_ACCEPTABLE"		: 4.06 Not Acceptable
		o   "STATUS_REQUEST_TIMEOUT"		: received == NULL
		o   "STATUS_UNSUPPORTED_MEDIA_TYPE"	: 4.15 Unsupported Media Type
		o   "STATUS_INTERNAL_SERVER_ERROR"	: 5.00 Internal Server Error
		o   "STATUS_NOT_IMPLEMENTED"		: 5.01 Not Implemented
		o   "STATUS_BAD_GATEWAY"		: 5.02 Bad Gateway
		o   "STATUS_SERVICE_UNAVAILABLE"	: 5.03 Service Unavailable
		o   "STATUS_GATEWAY_TIMEOUT"		: 5.04 Gateway Timeout
		o   "STATUS_DELETED"			: 2.02 Deleted
		*/
		if	(dia->callbacks && dia->callbacks->cb_diaUpdateResponse) {
			rtl_trace (5, "calling cb_diaUpdateResponse(scode=%s tid=%d)\n", scode, tid);
			dia->stat.cb_diaUpdateResponse	++;
			dia->callbacks->cb_diaUpdateResponse(dia, scode, ctt, len, cttType, NULL, tid);
		}
		break;
	case	COAP_REQUEST_DELETE :
		/*
		o   "STATUS_OK" (success)		: 2.02 Deleted
		o   "STATUS_PERMISSION_DENIED"		: 4.01 Unauthorized
		o   "STATUS_NOT_FOUND"			: 4.04 Not Found
		o   "STATUS_METHOD_NOT_ALLOWED"		: 4.05 Method Not Allowed
		o   "STATUS_REQUEST_TIMEOUT"		: received == NULL
		o   "STATUS_INTERNAL_SERVER_ERROR"	: 5.00 Internal Server Error
		o   "STATUS_NOT_IMPLEMENTED"		: 5.01 Not Implemented
		o   "STATUS_BAD_GATEWAY"		: 5.02 Bad Gateway
		o   "STATUS_SERVICE_UNAVAILABLE"	: 5.03 Service Unavailable
		o   "STATUS_GATEWAY_TIMEOUT"		: 5.04 Gateway Timeout
		o   "STATUS_DELETED"			: 2.02 Deleted
		*/
		if	(dia->callbacks && dia->callbacks->cb_diaDeleteResponse) {
			rtl_trace (5, "calling cb_diaDeleteResponse(scode=%s tid=%d)\n", scode, tid);
			dia->stat.cb_diaDeleteResponse	++;
			dia->callbacks->cb_diaDeleteResponse(dia, scode, NULL, tid);
		}
		break;
	}

	free_payload (&dr->payload);
	free_dr (dr);
}


static void init_resources(coap_context_t *ctx) {
/*
	coap_resource_t *r;
	if	(!ctx)	return;
	r = coap_resource_init((unsigned char *)"", 0);
	coap_register_handler(r, COAP_REQUEST_GET, hnd_request);
	coap_register_handler(r, COAP_REQUEST_POST, hnd_request);
	coap_register_handler(r, COAP_REQUEST_PUT, hnd_request);
	coap_register_handler(r, COAP_REQUEST_DELETE, hnd_request);
	coap_add_resource(ctx, r);
*/
	coap_register_request_handler(ctx, hnd_request);
	coap_register_response_handler(ctx, hnd_response);

	coap_register_option(ctx, COAP_OPTION_BLOCK1);
	coap_register_option(ctx, COAP_OPTION_BLOCK2);
}

//	port : "0" for a client, "5683" for a server
dia_context_t *
dia_createContext(const char *node, const char *port, dia_callbacks_t *callbacks) {
	coap_address_t addr;
	dia_context_t *dia = (dia_context_t *)calloc(sizeof(dia_context_t), 1);
	if	(!dia) {
		rtl_trace (3, "dia_context allocation failed.\n");
		return	NULL;
	}

	srand(time(0)+getpid());
	dia->callbacks	= callbacks;
	dia->tid	= rand() % COAP_MAX_TOKEN_VALUE;
	dia->blocking	= TRUE;
	if (coap_dns_lookup2(node, atoi(port), &addr) < 0) {
		rtl_trace (3, "dns_lookup(%s) failed.\n", node);
		free (dia);
		return	NULL;
	}
	dia->transport_ctx = coap_new_context(&addr);
	if	(!dia->transport_ctx) {
		rtl_trace (3, "coap_new_context failed\n");
		free (dia);
		return	NULL;
	}
	init_resources (dia->transport_ctx);
	dia->transport_ctx->userdata = dia;
	dia_InitCS(dia);

	dia->peertbl = hashtbl_create(1024, NULL);
	dia->tidtbl = hashtbl_create(1024, NULL);
	rtl_trace (5, "dia_createContext libCoAP version %d\n", LIBCOAP_VERSION);
	return dia;
}

void dia_setTransactionID (dia_context_t *dia, int tid) {
	dia->tid	= tid;
}

void dia_freeContext (dia_context_t *dia) {
	coap_free_context(dia->transport_ctx);
	free (dia);
}

void dia_setNonBlocking (dia_context_t *dia, int maxRead) {
	int	fd = dia_getFd(dia);
	int	x=fcntl(fd, F_GETFL, 0);
	fcntl	(fd, F_SETFL, x | O_NONBLOCK);
	dia->blocking	= FALSE;
	dia->maxRead	= maxRead;
}

void dia_setBlocking (dia_context_t *dia) {
	int	fd = dia_getFd(dia);
	int	x=fcntl(fd, F_GETFL, 0);
	fcntl	(fd, F_SETFL, x & ~O_NONBLOCK);
	dia->blocking	= TRUE;
}

#define COAP_RESOURCE_CHECK_TIME 2

int dia_getFd(dia_context_t *dia) {
	return	dia->transport_ctx->sockfd;
}

void dia_set_log_level(int log_level) {
	coap_set_log_level(log_level);
	rtl_tracelevel (log_level);
	coap_set_log_impl (rtl_trace);
}

void dia_nextTimer(dia_context_t *dia, struct timeval *tv) {
	coap_queue_t *nextpdu;
	coap_tick_t now;

	nextpdu = coap_peek_next(dia->transport_ctx);
	coap_ticks(&now);
	while ( nextpdu && nextpdu->t <= now ) {
		coap_retransmit(dia->transport_ctx, coap_pop_next(dia->transport_ctx));
		nextpdu = coap_peek_next(dia->transport_ctx);
	}

	if ( nextpdu && nextpdu->t <= now + COAP_RESOURCE_CHECK_TIME ) {
		/* set timeout if there is a pdu to send before our automatic timeout occurs */
		int delta = abs(nextpdu->t - now);
		tv->tv_usec = (delta % COAP_TICKS_PER_SECOND) << 10;
		tv->tv_sec = delta / COAP_TICKS_PER_SECOND;
		//printf ("now=%d pdu->t=%d\n", now, nextpdu->t);
	} else {
		tv->tv_usec = 0;
		tv->tv_sec = COAP_RESOURCE_CHECK_TIME;
	}
	//printf ("nextTimer tv_sec=%d tv_usec=%d\n", tv->tv_sec, tv->tv_usec);
}

void dia_input(dia_context_t *dia) {
	if	(dia->blocking) {
		coap_read(dia->transport_ctx);
	} else {
		int n = 0;
		while ((coap_read(dia->transport_ctx) == 0) && (n < dia->maxRead))
			n	++;
	}
	coap_dispatch(dia->transport_ctx);	/* and dispatch PDUs from receivequeue */
}


boolean isLocalHost (char *host) {
	if	(!host || !*host)
		return	TRUE;
	if	(!strcmp(host, "localhost"))
		return	TRUE;
	if	(!strcmp(host, "127.0.0.1"))
		return	TRUE;
	if	(!strcmp(host, "::"))
		return	TRUE;
	if	(!strcmp(host, "::1"))
		return	TRUE;
	if	(!strcmp(host, "[::1]"))
		return	TRUE;
	return	FALSE;
}

/*----------------------------- dIa API ----------------------------*/

static char *getParam(str *s) {
	char *ret = malloc(s->length + 1);
	memcpy (ret, s->s, s->length);
	ret[s->length] = 0;
	return ret;
}

int
dIa_split_uri(unsigned char *str_var, size_t len, coap_uri_t *uri) {
  unsigned char *p, *q;
  //int secure = 0, res = 0;
  int res = 0;

  if (!str_var || !uri)
    return -1;

  memset(uri, 0, sizeof(coap_uri_t));
  uri->port = COAP_DEFAULT_PORT;

  /* search for scheme */
  p = str_var;
  if (*p == '/') {
    q = p;
    goto path;
  }
#if 0
  q = (unsigned char *)COAP_DEFAULT_SCHEME;
  while (len && *q && tolower(*p) == *q) {
    ++p; ++q; --len;
  }
  
  /* If q does not point to the string end marker '\0', the schema
   * identifier is wrong. */
  if (*q) {
    res = -1;
    goto error;
  }

  /* There might be an additional 's', indicating the secure version: */
  if (len && (secure = tolower(*p) == 's')) {
    ++p; --len;
  }
  q = (unsigned char *)"://";
  while (len && *q && tolower(*p) == *q) {
    ++p; ++q; --len;
  }

  if (*q) {
    res = -2;
    goto error;
  }
#endif
  q = (unsigned char *)strstr((char *)p, "://");
  if (!q) {
    res = -2;
    goto error;
  }
  len -= q+3-p;
  p = q+3;

  /* p points to beginning of Uri-Host */
  q = p;
  if (len && *p == '[') {	/* IPv6 address reference */
    ++p;
    
    while (len && *q != ']') {
      ++q; --len;
    }

    if (!len || *q != ']' || p == q) {
      res = -3;
      goto error;
    } 

//    COAP_SET_STR(&uri->host, q - p, p);
    // Version with brackets !!!
    COAP_SET_STR(&uri->host, q - p + 2, p - 1);
    ++q; --len;
  } else {			/* IPv4 address or FQDN */
    while (len && *q != ':' && *q != '/' && *q != '?') {
      *q = tolower(*q);
      ++q;
      --len;
    }

    if (p == q) {
      res = -3;
      goto error;
    }

    COAP_SET_STR(&uri->host, q - p, p);
  }

  /* check for Uri-Port */
  if (len && *q == ':') {
    p = ++q;
    --len;
    
    while (len && isdigit(*q)) {
      ++q;
      --len;
    }

    if (p < q) {		/* explicit port number given */
      int uri_port = 0;
    
      while (p < q)
	uri_port = uri_port * 10 + (*p++ - '0');

      uri->port = uri_port;
    } 
  }
  
 path:		 /* at this point, p must point to an absolute path */

  if (!len)
    goto end;
  
  if (*q == '/') {
    p = ++q;
    --len;

    while (len && *q != '?') {
      ++q;
      --len;
    }
  
    if (p < q) {
      COAP_SET_STR(&uri->path, q - p, p);
      p = q;
    }
  }

  /* Uri_Query */
  /* FIXME: split at & sign, parse:
        query-pattern = search-token [ "*" ]
        search-token = *search-char
        search-char = unreserved / pct-encoded
                    / ":" / "@"   ; from pchar
                    / "/" / "?"   ; from query
                    / "!" / "$" / "'" / "(" / ")"
                    / "+" / "," / ";" / "="  ; from sub-delims
   */
  if (len && *p == '?') {
    ++p;
    --len;
    COAP_SET_STR(&uri->query, len, p);
    len = 0;
  }

  end:
  return len ? -1 : 0;
  
  error:
  return res;
}

#if 0
	Draft-09 order

#define COAP_OPTION_CONTENT_TYPE  1 /* C, 8-bit uint, 1-2 B, - */
#define COAP_OPTION_MAXAGE        2 /* E, variable length, 1--4 B, 60 Seconds */
#define COAP_OPTION_PROXY_URI     3 /* C, String, 1-270 B, may occur more than once */
#define COAP_OPTION_ETAG          4 /* E, opaque, 0-8 B, (none) */
#define COAP_OPTION_URI_HOST      5 /* C, String, 1-270 B, destination address */
#define COAP_OPTION_LOCATION_PATH 6 /* E, String, 1-270 B, - */
#define COAP_OPTION_URI_PORT      7 /* C, String, 1-270 B, destination port */
#define COAP_OPTION_LOCATION_QUERY 8 /*  */
#define COAP_OPTION_URI_PATH      9 /* C, String, 1-270 B, - (may occur multiple times) */
#define COAP_OPTION_TOKEN        11 /* C, Sequence of Bytes, 1-8 B, empty */
#define COAP_OPTION_ACCEPT       12 /* E, uint,   0-2 B, (none) */
#define COAP_OPTION_IF_MATCH     13 /* C, opaque, 0-8 B, (none) */
#define COAP_OPTION_URI_QUERY    15 /* C, String, 1-270 B, "" */
#define COAP_OPTION_BLOCK2       17 /* C, unsigned integer, 1--3 B, 0 */
#define COAP_OPTION_BLOCK1       19 /* C, unsigned integer, 1--3 B, 0 */
#define COAP_OPTION_IF_NONE_MATCH 21 /* C, (none), 0 B, (none) */
#endif

#if LIBCOAP_VERSION < 4
static void setUriParameters (dia_context_t *dia, char *targetID, char *reqEntity, int tid, coap_pdu_t *request,
	char *cttType) {
	coap_uri_t uri;
	char	buf[256];
	char *host;

	dIa_split_uri((unsigned char *)targetID, strlen(targetID), &uri );
	//rtl_trace (5, "coap_split_uri(%s) => %d\n", targetID, uri.query.length);
	rtl_trace (5, "setUriParameters %s contentType=%s\n", targetID, cttType);

	host = getParam (&uri.host);

	if	(cttType) {
		unsigned char buf[20];
		int x	= dia2coap_cttType(cttType);
		rtl_trace (5, "cttType = %s COAP_OPTION_CONTENT_TYPE=%d\n", cttType, x);
		//	coap_add_option(request, COAP_OPTION_CONTENT_TYPE, coap_encode_var_bytes(buf, x), buf);
		buf[0] = x;
		coap_add_option(request, COAP_OPTION_CONTENT_TYPE, 1, buf);
	}

	if	(dia->proxyAddr) {
//	if	(!isLocalHost(host)) {
//		rtl_trace (5, "%s is remote\n", host);
		rtl_trace (5, "PROXY_URI=%s\n", targetID);
		coap_add_option(request, COAP_OPTION_PROXY_URI, strlen(targetID), (unsigned char *)targetID);
	}
	else {
//		rtl_trace (5, "%s is local\n", host);
		if (uri.host.length) {
			rtl_trace (5, "URI_HOST=%s\n", host);
			coap_add_option(request, COAP_OPTION_URI_HOST, uri.host.length, uri.host.s);
		}
		if (uri.port != COAP_DEFAULT_PORT) {
			rtl_trace (5, "URI_PORT=%d\n", uri.port);
			coap_add_option(request, COAP_OPTION_URI_PORT,
				coap_encode_var_bytes((unsigned char *)buf, uri.port), (unsigned char *)buf);
		}
		if (uri.path.length) {
			char *path = getParam(&uri.path);
			char *token;
			rtl_trace (5, "URI_PATH=%s\n", path);

			char	*work = path;
			while ((token = strsep(&work, "/")) != NULL) {
				coap_add_option(request, COAP_OPTION_URI_PATH, strlen(token), (unsigned char *)token);
			}
			free (path);
		}
	}

	sprintf ((char *)buf, "%d", tid);
	coap_add_option(request, COAP_OPTION_TOKEN, strlen((char *)buf), (unsigned char *)buf);

	if	(!dia->proxyAddr) {
		if (uri.query.length) {
			char *query = getParam(&uri.query);
			char *token;
			rtl_trace (5, "URI_QUERY=%s\n", query);
	
			char *work = query;
			while ((token = strsep(&work, "&")) != NULL) {
				coap_add_option(request, COAP_OPTION_URI_QUERY, strlen(token), (unsigned char *)token);
			}
			free (query);
		}
	}

	if	(reqEntity && *reqEntity) {
		strcpy (buf, "RequestingEntity=");
		strcat (buf, reqEntity);
		coap_add_option(request, COAP_OPTION_URI_QUERY, strlen(buf), (unsigned char *)buf);
	}

	free (host);
}
#else
#if 0
	Draft-18 order

#define COAP_OPTION_IF_MATCH      1 /* C, opaque, 0-8 B, (none) */
#define COAP_OPTION_URI_HOST      3 /* C, String, 1-255 B, destination address */
#define COAP_OPTION_ETAG          4 /* E, opaque, 1-8 B, (none) */
#define COAP_OPTION_IF_NONE_MATCH 5 /* empty, 0 B, (none) */
#define COAP_OPTION_URI_PORT      7 /* C, uint, 0-2 B, destination port */
#define COAP_OPTION_LOCATION_PATH 8 /* E, String, 0-255 B, - */
#define COAP_OPTION_URI_PATH     11 /* C, String, 0-255 B, (none) */
#define COAP_OPTION_CONTENT_FORMAT 12 /* E, uint, 0-2 B, (none) */
#define COAP_OPTION_CONTENT_TYPE COAP_OPTION_CONTENT_FORMAT
#define COAP_OPTION_MAXAGE       14 /* E, uint, 0--4 B, 60 Seconds */
#define COAP_OPTION_URI_QUERY    15 /* C, String, 1-255 B, (none) */
#define COAP_OPTION_ACCEPT       17 /* C, uint,   0-2 B, (none) */
#define COAP_OPTION_LOCATION_QUERY 20 /* E, String,   0-255 B, (none) */
#define COAP_OPTION_BLOCK2       23 /* C, uint, 0--3 B, (none) */
#define COAP_OPTION_BLOCK1       27 /* C, uint, 0--3 B, (none) */
#define COAP_OPTION_PROXY_URI    35 /* C, String, 1-1034 B, (none) */
#define COAP_OPTION_PROXY_SCHEME 39 /* C, String, 1-255 B, (none) */
#define COAP_OPTION_SIZE0        60 /* E, uint, 0-4 B, (none) */
#endif
static void setUriParameters (dia_context_t *dia, char *targetID, char *reqEntity, int tid, coap_pdu_t *request,
	char *cttType) {
	coap_uri_t uri;
	char	buf[256];
	char *host;

	dIa_split_uri((unsigned char *)targetID, strlen(targetID), &uri );
	rtl_trace (5, "coap_split_uri(%s) => %d\n", targetID, uri.query.length);

	host = getParam (&uri.host);

	sprintf ((char *)buf, "%d", tid);
	coap_add_token(request, strlen((char *)buf), (unsigned char *)buf);

	if	(!dia->proxyAddr) {
		if (uri.host.length) {
			rtl_trace (5, "URI_HOST=%s\n", host);
			coap_add_option(request, COAP_OPTION_URI_HOST, uri.host.length, uri.host.s);
		}
		if (uri.port != COAP_DEFAULT_PORT) {
			rtl_trace (5, "URI_PORT=%d\n", uri.port);
			coap_add_option(request, COAP_OPTION_URI_PORT,
				coap_encode_var_bytes((unsigned char *)buf, uri.port), (unsigned char *)buf);
		}
		if (uri.path.length) {
			char *path = getParam(&uri.path);
			char *token;
			rtl_trace (5, "path=%s\n", path);

			char	*work = path;
			while ((token = strsep(&work, "/")) != NULL) {
				rtl_trace (5, "URI_PATH=%s\n", token);
				coap_add_option(request, COAP_OPTION_URI_PATH, strlen(token), (unsigned char *)token);
			}
			free (path);
		}
	}

	if	(cttType) {
		unsigned char buf[20];
		int x	= dia2coap_cttType(cttType);
		rtl_trace (5, "cttType = %s COAP_OPTION_CONTENT_TYPE=%d\n", cttType, x);
		//	coap_add_option(request, COAP_OPTION_CONTENT_TYPE, coap_encode_var_bytes(buf, x), buf);
		buf[0] = x;
		coap_add_option(request, COAP_OPTION_CONTENT_TYPE, 1, buf);
	}

	if	(!dia->proxyAddr) {
		if (uri.query.length) {
			char *query = getParam(&uri.query);
			char *token;
			rtl_trace (5, "URI_QUERY=%s\n", query);
	
			char *work = query;
			while ((token = strsep(&work, "&")) != NULL) {
				coap_add_option(request, COAP_OPTION_URI_QUERY, strlen(token), (unsigned char *)token);
			}
			free (query);
		}
	}

	if	(reqEntity && *reqEntity) {
		strcpy (buf, "RequestingEntity=");
		strcat (buf, reqEntity);
		coap_add_option(request, COAP_OPTION_URI_QUERY, strlen(buf), (unsigned char *)buf);
	}

	free (host);
}
#endif

static coap_pdu_t *diaAllocResponse(dia_context_t *dia) {
//	rtl_trace (5, "diaAllocResponse piggy=%d\n", dia->piggy);
	if	(dia->piggy && dia->response) {
		dia->response->hdr->type = COAP_MESSAGE_ACK;
		return dia->response;
	}
	else {
		coap_pdu_t *response = coap_new_pdu();
		if	(!response) {
			rtl_trace (2, "Memory allocation failed for a new pdu.\n");
			dia->stat.alloc_error	++;
			return NULL;
		}
		response->hdr->type = COAP_MESSAGE_CON;
		response->hdr->id = coap_new_message_id(dia->transport_ctx);
		return	response;
	}
}

void diaProxy(dia_context_t *dia, char *proxyAddr) {
	if	(dia->proxyAddr) {
		free (dia->proxyAddr);
		dia->proxyAddr = NULL;
	}
	if	(proxyAddr)
		dia->proxyAddr = strdup(proxyAddr);
}

int diaRequest (dia_context_t *dia, int type,
	char *reqEntity, char *targetID, void *ctt, int len, char *cttType,
	Opt *optAttr, Opt *optHeader, int *tid) {
	coap_uri_t uri;
	coap_address_t peer;

	if	(dia->proxyAddr) {
		coap_split_uri((unsigned char *)dia->proxyAddr, strlen(dia->proxyAddr), &uri);
	}
	else {
		coap_split_uri((unsigned char *)targetID, strlen(targetID), &uri);
	}

	if (coap_dns_lookup(uri.host, uri.port, &peer) < 0) {
		rtl_trace (3, "dns_lookup(%s) failed. dIa request can not be sent\n", uri.host.s);
		dia->stat.reject	++;
		return	-1;
	}

	//	Token must be between 1-8 bytes
	if	(dia->tid >= COAP_MAX_TOKEN_VALUE)
		dia->tid	= 0;
	*tid	= dia->tid++;

	// 1. Retrieve the box speaking
	dia_box_t *p = getBox(dia, &peer, TRUE);

	// 2. Allocate a dia_request
	dia_request_t *dr = search_tid (&p->reqList, *tid, TRUE);

	coap_pdu_t *request  = coap_new_pdu();
	if	(!request) {
		rtl_trace (2, "Memory allocation failed for a new pdu.\n");
		dia->stat.alloc_error	++;
		return -3;
	}
	request->hdr->type = COAP_MESSAGE_CON;
	request->hdr->id = coap_new_message_id(dia->transport_ctx);
	request->hdr->code = type;
	dr->typeReq	= type;

	setUriParameters (dia, targetID, reqEntity, *tid, request, cttType);

	if	(ctt && len) {
		if	(!coap_add_data(request, len, (unsigned char *)ctt)) {
			rtl_trace (5, "PDU too big (%d). Use blocks.\n", len);
			// coap_add_data returned an error : message too big. We'll use blocks.

			/* set initial block size, will be lowered by
	 		   coap_write_block_opt) automatically */
			dr->block1.szx = 6;
			coap_write_block_opt(&dr->block1, COAP_OPTION_BLOCK1, request, len);
			coap_add_block(request, len, ctt, dr->block1.num, dr->block1.szx);

			alloc_payload (ctt, len, &dr->payload);
		}
	}

#if LIBCOAP_VERSION >= 4
	// PROXY_URI after BLOCK1
	if	(dia->proxyAddr) {
		rtl_trace (5, "PROXY_URI=%s\n", targetID);
		coap_add_option(request, COAP_OPTION_PROXY_URI, strlen(targetID), (unsigned char *)targetID);
	}
#endif

//	TODO : a enlever. Uniquement pour le test early nego (BLOCK_01 des Plugtests ETSI)
/*
{
unsigned char buf[4];
dr->block2.szx = 3;
dr->block2.num = 0;
coap_add_option(request, COAP_OPTION_BLOCK2, coap_encode_var_bytes(buf, 
	(dr->block2.num << 4) | dr->block2.szx), buf);
}
*/

	//	send anyway
	if (coap_send_confirmed(dia->transport_ctx, &peer, request) == COAP_INVALID_TID)
		coap_delete_pdu(request);

	return	0;
}

void diaResponse(dia_context_t *dia, int typeResp, char *scode, char *resourceURI, void *ctt, int len, char* cttType,
	Opt *optHeader, int tid, char *peer) {
	int	type, code;
	unsigned char buf[20];

	if	(tid < 0)
		return;

	coap_pdu_t *response	= diaAllocResponse (dia);
	if	(!response) {
		return;
	}

	code	= dia2coap_code(scode, typeResp);
	response->hdr->code = COAP_RESPONSE_CODE(code);

#if LIBCOAP_VERSION < 4
	type	= dia2coap_cttType(cttType);
	rtl_trace (5, "cttType = %s COAP_OPTION_CONTENT_TYPE=%d\n", cttType, type);
	buf[0] = type;
	coap_add_option(response, COAP_OPTION_CONTENT_TYPE, 1, buf);

	if	(resourceURI)
		coap_add_option(response, COAP_OPTION_LOCATION_PATH, strlen(resourceURI), (unsigned char *)resourceURI);

	sprintf ((char *)buf, "%d", tid);
	coap_add_option(response, COAP_OPTION_TOKEN, strlen((char *)buf), buf);
#else
	sprintf ((char *)buf, "%d", tid);
	coap_add_token(response, strlen((char *)buf), buf);

	if	(resourceURI && *resourceURI) {
		rtl_trace (5, "resourceURI = %s\n", resourceURI);
		coap_add_option(response, COAP_OPTION_LOCATION_PATH, strlen(resourceURI), (unsigned char *)resourceURI);
	}

	type	= dia2coap_cttType(cttType);
	rtl_trace (5, "cttType = %s COAP_OPTION_CONTENT_TYPE=%d\n", cttType, type);
	buf[0] = type;
	coap_add_option(response, COAP_OPTION_CONTENT_TYPE, 1, buf);
#endif

	dia_box_t *p;

	// 1. Retrieve the box speaking using tid or peer
	if	(peer && *peer) {
		p = getBoxWithString(dia, peer);
		if	(!p) {
			rtl_trace (3, "Error : peer %s unknown\n", peer);
			dia->stat.reject	++;
			return;
		}
	}
	else {
		p = hashtbl_get(dia->tidtbl, (char *)buf);
		if	(!p) {
			rtl_trace (3, "Error : tid %d unknown\n", tid);
			dia->stat.reject	++;
			return;
		}
	}

// TODO : for Observed values, code must be changed to allow multiple replies of the same request
	// 2. Retrieve dia_request
	dia_request_t *dr = search_tid (&p->reqList, tid, FALSE);
	if	(!dr) {
		rtl_trace (3, "Error : tid %d not in reqList\n", tid);
		dia->stat.reject	++;
		return;
	}

	boolean withBlocks = FALSE;

	if	(ctt && len) {
		if	(!coap_add_data(response, len, (unsigned char *)ctt)) {
			rtl_trace (5, "PDU too big (%d). Use blocks.\n", len);
			// coap_add_data returned an error : message too big. We'll use blocks.

			/* set initial block size, will be lowered by
 			   coap_write_block_opt) automatically */
			if	(dr->early_nego == FALSE)
				dr->block2.szx = 6;
			rtl_trace (5, "early_nego=%d szx=%d\n", dr->early_nego, dr->block2.szx);
			coap_write_block_opt(&dr->block2, COAP_OPTION_BLOCK2, response, len);
			coap_add_block(response, len, ctt, dr->block2.num, dr->block2.szx);

			alloc_payload (ctt, len, &dr->payload);
			withBlocks = TRUE;
		}
	}

	if	(!dia->piggy) {
		if (coap_send(dia->transport_ctx, &p->peer, response) == COAP_INVALID_TID)
			coap_delete_pdu(response);
	}

	if	(!withBlocks) {
		free_payload (&dr->payload);
		free_dr (dr);
		//rtl_trace (5, "hashtbl_remove tidtbl %s\n", buf);
		hashtbl_remove(dia->tidtbl, (char *)buf);
		dump_all(dia);
		dump_reqs(p);
	}

	//	Here, we are in piggy ! ACK will be returned by libcoap
}

int diaCreateRequest (dia_context_t *dia,
	char *reqEntity, char *targetID, void *ctt, int len, char *cttType,
	Opt *optAttr, Opt *optHeader, int *tid) {
	dia->stat.diaCreateRequest	++;
	rtl_trace (5, "Entering diaCreateRequest(reqEntity=%s targetID=%s tid=%d)\n", reqEntity, targetID, dia->tid);
	return diaRequest (dia, COAP_REQUEST_POST, reqEntity, targetID, ctt, len, cttType,
		optAttr, optHeader, tid);
}

void diaCreateResponse(dia_context_t *dia, char *scode, char *resourceURI, void *ctt, int len, char* cttType,
	Opt *optHeader, int tid, char *peer) {
	dia->stat.diaCreateResponse	++;
	rtl_trace (5, "Entering diaCreateResponse(scode=%s tid=%d, peer=%s)\n", scode, tid, peer);
	diaResponse (dia, COAP_REQUEST_POST, scode, resourceURI, ctt, len, cttType, optHeader, tid, peer);
}

int
diaRetrieveRequest(dia_context_t *dia, char *reqEntity, char *targetID,
		Opt *optAttr, Opt *optHeader, int *tid) {
	dia->stat.diaRetrieveRequest	++;
	rtl_trace (5, "Entering diaRetrieveRequest(reqEntity=%s targetID=%s tid=%d)\n", reqEntity, targetID, dia->tid);
	return diaRequest (dia, COAP_REQUEST_GET, reqEntity, targetID, NULL, 0, NULL,
		optAttr, optHeader, tid);
}

void
diaRetrieveResponse(dia_context_t *dia,
	char *scode, void *ctt, int len, char* cttType,
	Opt *optHeader, int tid, char *peer) {
	dia->stat.diaRetrieveResponse	++;
	rtl_trace (5, "Entering diaRetrieveResponse(scode=%s tid=%d, peer=%s)\n", scode, tid, peer);
	diaResponse (dia, COAP_REQUEST_GET, scode, "", ctt, len, cttType, optHeader, tid, peer);
}

int diaUpdateRequest(dia_context_t *dia, char *reqEntity, char *targetID, void *ctt, int len, char *cttType,
	Opt *optAttr, Opt *optHeader, int *tid) {
	dia->stat.diaUpdateRequest	++;
	rtl_trace (5, "Entering diaUpdateRequest(reqEntity=%s targetID=%s tid=%d)\n", reqEntity, targetID, dia->tid);
	return diaRequest (dia, COAP_REQUEST_PUT, reqEntity, targetID, ctt, len, cttType,
		optAttr, optHeader, tid);
}

void diaUpdateResponse (dia_context_t *dia, char *scode, void *ctt, int len, char* cttType,
	Opt *optHeader, int tid, char *peer) {
	dia->stat.diaUpdateResponse	++;
	rtl_trace (5, "Entering diaUpdateResponse(scode=%s tid=%d, peer=%s)\n", scode, tid, peer);
	diaResponse (dia, COAP_REQUEST_PUT, scode, "", ctt, len, cttType, optHeader, tid, peer);
}

int diaDeleteRequest(dia_context_t *dia, char *reqEntity, char *targetID,
	Opt *optAttr, Opt *optHeader, int *tid) {
	dia->stat.diaDeleteRequest	++;
	rtl_trace (5, "Entering diaDeleteRequest(reqEntity=%s targetID=%s tid=%d)\n", reqEntity, targetID, dia->tid);
	return diaRequest (dia, COAP_REQUEST_DELETE, reqEntity, targetID, NULL, 0, NULL,
		optAttr, optHeader, tid);
}

void diaDeleteResponse(dia_context_t *dia, char *scode, Opt *optHeader, int tid, char *peer) {
	dia->stat.diaDeleteResponse	++;
	rtl_trace (5, "Entering diaDeleteResponse(scode=%s tid=%d, peer=%s)\n", scode, tid, peer);
	diaResponse (dia, COAP_REQUEST_DELETE, scode, "", NULL, 0, "", optHeader, tid, peer);
}



void dia_setCallbacks(dia_context_t *dia, dia_callbacks_t *callbacks) {
	dia->callbacks	= callbacks;
}

void dia_setTraceFunction (void (*fct)(int level, char *message)) {
	// rtl_setTraceFunction (fct);
}

void dia_withLoss(dia_context_t *dia, char loss) {
	dia->transport_ctx->with_loss	= loss;
}

boolean diaIsProxied(dia_context_t *dia) {
	return	dia->isProxied;
}
