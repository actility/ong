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
/* microdIa.c -- dIa
 *
 * dIa implementation for Microchips
 *
 * Copyright (C) 2012 Actility
 *
 * This file is part of the dIa library libdIa. Please see
 * README for terms of use.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#if defined(MICROCHIP)
#include "TCPIP Stack/TCPIP.h"
#include "time.h"
static int _gettime(void *x) {
	return SNTPGetUTCSeconds();
}
#endif

#if defined(LINUX) || defined(MACOSX) || defined(FREERTOS)
#include <time.h>
static int _gettime(void *x) {
	return time(x);
}
#endif

#if defined(MSP430)
#include <time.h>
static int _gettime(void *x) {
	return time(0L);
}
#endif

#include <ctype.h>
#include "microdIa.h"


#define	COAP_VERSION	0x40

#define COAP_MESSAGE_CON	       0x00 /* confirmable message (requires ACK/RST) */
#define COAP_MESSAGE_NON	       0x10 /* non-confirmable message (one-shot message) */
#define COAP_MESSAGE_ACK	       0x20 /* used to acknowledge confirmable messages */
#define COAP_MESSAGE_RST	       0x30 /* indicates error in received messages */

#define COAP_OPTION_CONTENT_TYPE  1 /* C, 8-bit uint, 1-2 B, - */
#define COAP_OPTION_MAXAGE	2 /* E, variable length, 1--4 B, 60 Seconds */
#define COAP_OPTION_PROXY_URI     3 /* C, String, 1-270 B, may occur more than once */
#define COAP_OPTION_ETAG	  4 /* E, opaque, 0-8 B, (none) */
#define COAP_OPTION_URI_HOST      5 /* C, String, 1-270 B, destination address */
#define COAP_OPTION_LOCATION_PATH 6 /* E, String, 1-270 B, - */
#define COAP_OPTION_URI_PORT      7 /* C, String, 1-270 B, destination port */
#define COAP_OPTION_LOCATION_QUERY 8 /*  */
#define COAP_OPTION_URI_PATH      9 /* C, String, 1-270 B, - (may occur multiple times) */
#define COAP_OPTION_TOKEN	11 /* C, Sequence of Bytes, 1-8 B, empty */
#define COAP_OPTION_ACCEPT       12 /* E, uint,   0-2 B, (none) */
#define COAP_OPTION_IF_MATCH     13 /* C, opaque, 0-8 B, (none) */
#define COAP_OPTION_URI_QUERY    15 /* C, String, 1-270 B, "" */
#define COAP_OPTION_IF_NONE_MATCH 21 /* C, (none), 0 B, (none) */

#define COAP_MEDIATYPE_TEXT_PLAIN		     0 /* text/plain (UTF-8) */
#define COAP_MEDIATYPE_APPLICATION_LINK_FORMAT       40 /* application/link-format */
#define COAP_MEDIATYPE_APPLICATION_XML	       41 /* application/xml */
#define COAP_MEDIATYPE_APPLICATION_OCTET_STREAM      42 /* application/octet-stream */
#define COAP_MEDIATYPE_APPLICATION_RDF_XML	   43 /* application/rdf+xml */
#define COAP_MEDIATYPE_APPLICATION_EXI	       47 /* application/exi  */
#define COAP_MEDIATYPE_APPLICATION_JSON	      50 /* application/json  */

#define COAP_RESPONSE_CODE(N) (((N)/100 << 5) | (N)%100)

#define COAP_DEFAULT_PORT	   "5683" /* CoAP default UDP port */
#define COAP_DEFAULT_MAX_AGE	  60 /* default maximum object lifetime in seconds */

#define COAP_DEFAULT_SCHEME	"coap" /* the default scheme for CoAP URIs */

#define COAP_HOST_SIZE 100
#define COAP_PORT_SIZE 6
#define COAP_PATH_SIZE 200
#define COAP_QUERY_SIZE 200

typedef struct {
	char	host[COAP_HOST_SIZE+1];
	char	port[COAP_PORT_SIZE+1];
	char	path[COAP_PATH_SIZE+1];
	char	query[COAP_QUERY_SIZE+1];
} coap_uri_t;

coap_uri_t _gUri;

static dia_context_t _gDia;


//==================== _gcoapMsg object =================
static unsigned char _hidden[COAP_MAX_PDU_SIZE+10];
static unsigned char *_gcoapMsg = _hidden+10;
static unsigned char *_gcoapMsgPtr;
static unsigned char *_gcoapMsgMax;

static inline void _gcoapMsg_reinit() {
	_gcoapMsg	= _hidden+10;
	_gcoapMsgPtr	= _gcoapMsg;
	_gcoapMsgMax	= _gcoapMsg + COAP_MAX_PDU_SIZE;
}

static inline void _gcoapMsg_putByte(unsigned char byte) {
	if	(_gcoapMsgPtr+1 < _gcoapMsgMax)
		*_gcoapMsgPtr++	= byte;
}

static inline void _gcoapMsg_putBuffer(unsigned char *buffer, int sz) {
	if	(_gcoapMsgPtr+sz >= _gcoapMsgMax)
		sz	= _gcoapMsgMax - _gcoapMsgPtr;
	memcpy (_gcoapMsgPtr, buffer, sz);
	_gcoapMsgPtr	+= sz;
}
//=======================================================

/**
 * @brief Allocate a request. The request is taken from a global table.
 * The request is added to the list of current requests.
 * If no request is available, try to find a older one.
 * @param dia pointer to the dia context
 * @return the newly request.
 */
dia_request_t *alloc_request(dia_context_t *dia) {
	int	i;
	int	now = _gettime(0);
	dia_request_t *req;
	for	(i=0, req=dia->_gRequests; i<COAP_MAX_REQUESTS; i++, req++) {
		if	(req->inlist == 0) {
			list_add_tail (&req->head, &dia->reqList);
			req->cnt	= 0;
			req->when	= _gettime(0);
			req->inlist	= 1;
			return	req;
		}
		//	reuse old request never responded
		if	(now - req->when >= COAP_DEFAULT_MAX_AGE) {
			req->cnt	= 0;
			req->when	= _gettime(0);
			return	req;
		}
	}
	return	NULL;
}

/**
 * @brief Free the request. A request considered as free is a request with coap_tid=0 and dia_tid=0
 * and inlist=0
 * @param req pointer to the request
 */
void free_request(dia_request_t *req) {
	req->coap_tid	= 0;
	req->dia_tid	= 0;
	req->when	= 0;
	req->cnt	= 0;
	list_del (&req->head);
	req->inlist	= 0;
}

dia_request_t *find_request_by_coap_tid(dia_context_t *dia, int coap_tid) {
	int	i;
	dia_request_t *req;
	for	(i=0, req=dia->_gRequests; i<COAP_MAX_REQUESTS; i++, req++) {
		if	(req->coap_tid == coap_tid)
			return	req;
	}
	return	NULL;
}

dia_request_t *find_request_by_dia_tid(dia_context_t *dia, int dia_tid) {
	int	i;
	dia_request_t *req;
	for	(i=0, req=dia->_gRequests; i<COAP_MAX_REQUESTS; i++, req++) {
		if	(req->dia_tid == dia_tid)
			return	req;
	}
	return	NULL;
}


static char *custom_itoa(long i) {
	static char output[24];  // 64-bit MAX_INT is 20 digits
	char* p = &output[23];

	for(*p--=0;i;*p--=i%10+0x30,i/=10);
	return ++p;    
}

static long custom_atoi(unsigned char *pt, int len) {
	long	val = 0;
	int	i;
	for	(i=0; i<len; i++) {
		val	*= 10;
		val	+= (*pt++ - '0');
	}
	return	val;
}

#ifndef HEXDUMP_COLS
#define HEXDUMP_COLS 16
#endif
 
#if defined(LINUX) || defined(MACOSX)
void hexdump(void *mem, unsigned int len) {
	unsigned int i, j;
	
	for(i = 0; i < len + ((len % HEXDUMP_COLS) ? (HEXDUMP_COLS - len % HEXDUMP_COLS) : 0); i++)
	{
		/* print offset */
		if(i % HEXDUMP_COLS == 0)
		{
			printf("0x%06x: ", i);
		}
 
		/* print hex data */
		if(i < len)
		{
			printf("%02x ", 0xFF & ((char*)mem)[i]);
		}
		else /* end of block, just aligning for ASCII dump */
		{
			printf("   ");
		}
		
		/* print ASCII dump */
		if(i % HEXDUMP_COLS == (HEXDUMP_COLS - 1))
		{
			for(j = i - (HEXDUMP_COLS - 1); j <= i; j++)
			{
				if(j >= len) /* end of block, not really printing */
				{
					putchar(' ');
				}
				else if(isprint(((char*)mem)[j])) /* printable char */
				{
					putchar(0xFF & ((char*)mem)[j]);	
				}
				else /* other char */
				{
					putchar('.');
				}
			}
			putchar('\n');
		}
	}
}
#endif

int my_strcut(char *str, char *tok, char **args, int szargs) {
        register char   *pt, *opt;
        int     n;

        opt = pt = str;
        n       = 0;
        memset (args, 0, szargs*sizeof(char *));
        while   ((pt = strstr(pt, tok)) && (n < szargs-1)) {
                *pt++           = 0;
                args[n++]       = opt;
                opt             = pt;
        }
        if      (*opt && (n < szargs))
                args[n++]       = opt;
        return  n;
}

static char *coap2dia_code(unsigned int code) {
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

static unsigned int dia2coap_code(char *scode, int method) {
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

static int
startWith(char *buf, char *str) {
	return	!strncmp(buf, str, strlen(str));
}

static int
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

static char *
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

int protected_memcpy (void *to, void *from, int sz, int maxsz) {
	if	(sz > maxsz)
		sz	= maxsz;
	memcpy (to, from, sz);
	return	sz;
}

int protected_strcat (char *to, char *from, int sz, int maxsz) {
	maxsz	-= strlen(to);
	if	(sz > maxsz)
		sz	= maxsz;
	memcpy (to+strlen(to), from, sz);
	to[strlen(to)+sz] = 0;
	return	sz;
}

int
dIa_split_uri(unsigned char *str_var, size_t len, coap_uri_t *uri) {
	unsigned char *p, *q;
	int secure = 0, res = 0;

	if (!str_var || !uri)
		return -1;

	memset(uri, 0, sizeof(coap_uri_t));
	strcpy (uri->port, COAP_DEFAULT_PORT);

	/* search for scheme */
	p = str_var;
	if (*p == '/') {
		q = p;
		goto path;
	}

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

		// Version with brackets !!!
		//protected_strcat (uri->host, p, q - p, COAP_HOST_SIZE);
		protected_strcat (uri->host, p-1, q - p + 2, COAP_HOST_SIZE);
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

		protected_strcat (uri->host, p, q - p, COAP_HOST_SIZE);
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
			protected_strcat (uri->port, p, q - p, COAP_PORT_SIZE);
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
			protected_strcat (uri->path, p, q - p, COAP_PATH_SIZE);
			p = q;
		}
	}

	/* Uri_Query */
	/* FIXME: split at & sign, parse:
		query-pattern = search-token [ "*" ]
		search-token = *search-char
		search-char = unreserved / pct-encoded
					/ ":" / "@"	 ; from pchar
					/ "/" / "?"	 ; from query
					/ "!" / "$" / "'" / "(" / ")"
					/ "+" / "," / ";" / "="	; from sub-delims
	 */
	if (len && *p == '?') {
		++p;
		--len;
		protected_strcat (uri->query, p, len, COAP_QUERY_SIZE);
		len = 0;
	}

	end:
	return len ? -1 : 0;

	error:
	return res;
}

int isLocalHost (char *host) {
	if	(!host || !*host)
		return	1;
	if	(!strcmp(host, "localhost"))
		return	1;
	if	(!strcmp(host, "127.0.0.1"))
		return	1;
	if	(!strcmp(host, "::"))
		return	1;
	if	(!strcmp(host, "::1"))
		return	1;
	if	(!strcmp(host, "[::1]"))
		return	1;
	return	0;
}

static int newCoapTid(dia_context_t *dia) {
	if	(++dia->coap_tid > COAP_MAX_TID)
		dia->coap_tid	= 1;
	return	dia->coap_tid;
}

static int newDiaTid(dia_context_t *dia) {
	if	(++dia->dia_tid > COAP_MAX_TOKEN_VALUE)
		dia->dia_tid	= 1;
	return	dia->dia_tid;
}

void encodeOption(int opt, int prevopt, char *value) {
	int sz	= strlen(value);
	if	(sz < 15)
		_gcoapMsg_putByte (((opt-prevopt)<<4) + sz);
	else {
		_gcoapMsg_putByte (((opt-prevopt)<<4) + 0x0f);
		_gcoapMsg_putByte (sz-15);
	}
	_gcoapMsg_putBuffer ((unsigned char *)value, sz);
}

void encodeOption_uint(int opt, int prevopt, char *value) {
	unsigned short x = atoi(value);
	_gcoapMsg_putByte (((opt-prevopt)<<4) + 2);
	_gcoapMsg_putByte (x/256);
	_gcoapMsg_putByte (x%256);
}

static int diaRequest (dia_context_t *dia, int type,
	char *reqEntity, char *targetID, void *ctt, int len, char *cttType,
	Opt *optAttr, Opt *optHeader, int *tid) {

	dia_request_t *req	= alloc_request(dia);
	if	(!req)
		return	-1;

	req->typeReq	= type;
	req->coap_tid	= newCoapTid(dia);
	req->dia_tid	= newDiaTid(dia);
	*tid	= req->dia_tid;

	_gcoapMsg_reinit ();
	int	nbopt = 0;
	int	prevopt = 0;

	_gcoapMsg_putByte (COAP_VERSION + COAP_MESSAGE_CON);
	_gcoapMsg_putByte (type);
	_gcoapMsg_putByte (req->coap_tid >> 8);
	_gcoapMsg_putByte (req->coap_tid & 0xff);

	dIa_split_uri((unsigned char *)targetID, strlen(targetID), &_gUri);

	//	Option #1
	if	(cttType && *cttType) {
		_gcoapMsg_putByte (((COAP_OPTION_CONTENT_TYPE-prevopt)<<4) + 1);
		_gcoapMsg_putByte (dia2coap_cttType(cttType));
		prevopt	= COAP_OPTION_CONTENT_TYPE;
		nbopt	++;
	}

	//	Option #3
	if	(!isLocalHost(_gUri.host)) {
		encodeOption(COAP_OPTION_PROXY_URI, prevopt, targetID);
		prevopt	= COAP_OPTION_PROXY_URI;
		nbopt	++;
	}
	else {
		//	Option #5
		if	(*_gUri.host) {
			encodeOption(COAP_OPTION_URI_HOST, prevopt, _gUri.host);
			prevopt	= COAP_OPTION_URI_HOST;
			nbopt	++;
		}
		//	Option #7
		if	(*_gUri.port && strcmp(_gUri.port, COAP_DEFAULT_PORT)) {
			encodeOption_uint(COAP_OPTION_URI_PORT, prevopt, _gUri.port);
			prevopt	= COAP_OPTION_URI_PORT;
			nbopt	++;
		}
		//	Option #9
		int	nb, i;
		char	*args[20];
		nb = my_strcut(_gUri.path, "/", args, 20);
		for	(i=0; i<nb; i++) {
			encodeOption(COAP_OPTION_URI_PATH, prevopt, args[i]);
			prevopt	= COAP_OPTION_URI_PATH;
			nbopt	++;
		}
	}

	//	Option #11
	char *buf	= custom_itoa (req->dia_tid);
	encodeOption(COAP_OPTION_TOKEN, prevopt, buf);
	prevopt	= COAP_OPTION_TOKEN;
	nbopt	++;

	//	Option #15
	if	(reqEntity && *reqEntity) {
		if (*_gUri.query)
			protected_strcat (_gUri.query, "&", 1, COAP_QUERY_SIZE);
		protected_strcat (_gUri.query, "RequestingEntity=", 17, COAP_QUERY_SIZE);
		protected_strcat (_gUri.query, reqEntity, strlen(reqEntity), COAP_QUERY_SIZE);
	}
	if (*_gUri.query) {
		int	nb, i;
		char	*args[20];
		nb = my_strcut(_gUri.query, "&", args, 20);
		for	(i=0; i<nb; i++) {
			encodeOption(COAP_OPTION_URI_QUERY, prevopt, args[i]);
			prevopt	= COAP_OPTION_URI_QUERY;
			nbopt	++;
		}
	}

	_gcoapMsg[0]	+= nbopt;

	if	(ctt && len) {
		_gcoapMsg_putBuffer (ctt, len);
	}

#ifdef COAP_RETRANSMIT
	req->raw_size = protected_memcpy (req->raw, _gcoapMsg, _gcoapMsgPtr-_gcoapMsg, COAP_MAX_PDU_SIZE);
#endif

#if defined(LINUX) || defined(MACOSX)
	printf (">>>\n");
	hexdump (_gcoapMsg, _gcoapMsgPtr-_gcoapMsg);
#endif

	if (dia->with_loss) {
		static int lost = 0;
		lost = 1 - lost;
		if (lost) {
#if defined(LINUX) || defined(MACOSX)
			printf ("output lost\n");
#endif
			return	0;
		}
	}
	if	(dia->callbacks->cb_sendToNetwork)
		dia->callbacks->cb_sendToNetwork (dia, _gcoapMsg, _gcoapMsgPtr-_gcoapMsg);
	return	0;
}

static int diaResponse(dia_context_t *dia, int typeResp, char *scode, char *resourceURI, void *ctt, int len, char* cttType,
	Opt *optHeader, int tid, char *peer) {

	dia_request_t *req	= find_request_by_dia_tid(dia, tid);
	if	(!req)
		return	-1;

	req->coap_tid	= newCoapTid(dia);

	_gcoapMsg_reinit ();
	int	nbopt = 0;
	int	prevopt = 0;

	_gcoapMsg_putByte (COAP_VERSION + COAP_MESSAGE_CON);
	_gcoapMsg_putByte (COAP_RESPONSE_CODE(dia2coap_code(scode, typeResp)));
	_gcoapMsg_putByte (dia->coap_tid >> 8);
	_gcoapMsg_putByte (dia->coap_tid & 0xff);

	//	Option #1
	if	(cttType && *cttType) {
		_gcoapMsg_putByte (((COAP_OPTION_CONTENT_TYPE-prevopt)<<4) + 1);
		_gcoapMsg_putByte (dia2coap_cttType(cttType));
		prevopt	= COAP_OPTION_CONTENT_TYPE;
		nbopt	++;
	}

	//	Option #6
	if	(resourceURI && *resourceURI) {
		encodeOption(COAP_OPTION_LOCATION_PATH, prevopt, resourceURI);
		prevopt	= COAP_OPTION_LOCATION_PATH;
		nbopt	++;
	}

	//	Option #11
	char *buf	= custom_itoa (tid);
	encodeOption(COAP_OPTION_TOKEN, prevopt, buf);
	prevopt	= COAP_OPTION_TOKEN;
	nbopt	++;

	_gcoapMsg[0]	+= nbopt;

	if	(ctt && len) {
		_gcoapMsg_putBuffer (ctt, len);
	}

#ifdef COAP_RETRANSMIT
	req->raw_size = protected_memcpy (req->raw, _gcoapMsg, _gcoapMsgPtr-_gcoapMsg, COAP_MAX_PDU_SIZE);
#endif

#if defined(LINUX) || defined(MACOSX)
	printf (">>>\n");
	hexdump (_gcoapMsg, _gcoapMsgPtr-_gcoapMsg);
#endif
	if	(dia->callbacks->cb_sendToNetwork)
		dia->callbacks->cb_sendToNetwork (dia, _gcoapMsg, _gcoapMsgPtr-_gcoapMsg);

	//	XXX
	req->dia_tid	= 0;

	return	0;
}

int diaCreateRequest (dia_context_t *dia,
	char *reqEntity, char *targetID, void *ctt, int len, char *cttType,
	Opt *optAttr, Opt *optHeader, int *tid) {
	dia->stat.diaCreateRequest	++;
	return diaRequest (dia, COAP_REQUEST_POST, reqEntity, targetID, ctt, len, cttType,
		optAttr, optHeader, tid);
}

void diaCreateResponse(dia_context_t *dia, char *scode, char *resourceURI, void *ctt, int len, char* cttType,
	Opt *optHeader, int tid, char *peer) {
	dia->stat.diaCreateResponse	++;
	diaResponse (dia, COAP_REQUEST_POST, scode, resourceURI, ctt, len, cttType, optHeader, tid, peer);
}

int
diaRetrieveRequest(dia_context_t *dia, char *reqEntity, char *targetID,
		Opt *optAttr, Opt *optHeader, int *tid) {
	dia->stat.diaRetrieveRequest	++;
	return diaRequest (dia, COAP_REQUEST_GET, reqEntity, targetID, NULL, 0, "",
		optAttr, optHeader, tid);
}

void
diaRetrieveResponse(dia_context_t *dia,
	char *scode, void *ctt, int len, char* cttType,
	Opt *optHeader, int tid, char *peer) {
	dia->stat.diaRetrieveResponse	++;
	diaResponse (dia, COAP_REQUEST_GET, scode, "", ctt, len, cttType, optHeader, tid, peer);
}

int diaUpdateRequest(dia_context_t *dia, char *reqEntity, char *targetID, void *ctt, int len, char *cttType,
	Opt *optAttr, Opt *optHeader, int *tid) {
	dia->stat.diaUpdateRequest	++;
	return diaRequest (dia, COAP_REQUEST_PUT, reqEntity, targetID, ctt, len, cttType,
		optAttr, optHeader, tid);
}

void diaUpdateResponse (dia_context_t *dia, char *scode, void *ctt, int len, char* cttType,
	Opt *optHeader, int tid, char *peer) {
	dia->stat.diaUpdateResponse	++;
	diaResponse (dia, COAP_REQUEST_GET, scode, "", ctt, len, cttType, optHeader, tid, peer);
}

int diaDeleteRequest(dia_context_t *dia, char *reqEntity, char *targetID,
	Opt *optAttr, Opt *optHeader, int *tid) {
	dia->stat.diaDeleteRequest	++;
	return diaRequest (dia, COAP_REQUEST_DELETE, reqEntity, targetID, NULL, 0, "",
		optAttr, optHeader, tid);
}

void diaDeleteResponse(dia_context_t *dia, char *scode, Opt *optHeader, int tid, char *peer) {
	dia->stat.diaDeleteResponse	++;
	diaResponse (dia, COAP_REQUEST_DELETE, scode, "", NULL, 0, "", optHeader, tid, peer);
}



dia_context_t * dia_createContext(dia_callbacks_t *callbacks) {
	dia_context_t *dia = &_gDia;

	srand(_gettime(0));
	dia->callbacks	= callbacks;
	dia->coap_tid	= rand() % COAP_MAX_TID;
	dia->dia_tid	= rand() % COAP_MAX_TOKEN_VALUE;
	memset (dia->_gRequests, 0, sizeof(dia->_gRequests));
	INIT_LIST_HEAD (&dia->reqList);
	return dia;
}

static int send_ack(dia_context_t *dia, int coap_tid) {
	unsigned char *pt = _gcoapMsg;

	*pt++	= COAP_VERSION + COAP_MESSAGE_ACK;
	*pt++	= 0x00;
	*pt++	= coap_tid >> 8;
	*pt++	= coap_tid & 0xff;

#if defined(LINUX) || defined(MACOSX)
	printf (">>>\n");
	hexdump (_gcoapMsg, pt-_gcoapMsg);
#endif
	if	(dia->callbacks->cb_sendToNetwork)
		dia->callbacks->cb_sendToNetwork (dia, _gcoapMsg, pt-_gcoapMsg);
	return	0;
}


static int hnd_request (dia_context_t *dia, unsigned char *msg, int len) {
	unsigned char *pt = msg;
	char	*cttType = "";
	int	coap_tid, dia_tid = 0, code;
	int	i;
	int	option = 0;
	int	optcount, cttlen;
	char	*ctt;
	char	reqEntity[400+1] = "";
	char	targetID[COAP_HOST_SIZE+COAP_PORT_SIZE+COAP_PATH_SIZE+COAP_QUERY_SIZE+5] = "";
/*
	char	host[COAP_HOST_SIZE+1], port[64], path[200], query[200];
	*host = *port = *path = *query = *reqEntity = *targetID = 0;
*/

	optcount = *pt++ & 0x0f;

	code	= *pt++;
	coap_tid = (*pt++) << 8;
	coap_tid += *pt++;

	dia->stat.request	++;

	dia_request_t *req = find_request_by_coap_tid(dia, coap_tid);
	if	(!req) {
		req	= alloc_request(dia);
		if	(!req)
			return	-1;
	}

	req->typeReq	= code;
	req->coap_tid	= coap_tid;
	memset (&_gUri, 0, sizeof(_gUri));

	send_ack (dia, coap_tid);

	for	(i=0; i<optcount; i++) {
		int	delta = *pt >> 4;
		int	len = *pt++ & 0x0f;
		if	(len == 15)
			len	+= *pt++;
		option	+= delta;

		switch	(option) {
		case	COAP_OPTION_CONTENT_TYPE :
			cttType	= coap2dia_cttType(*pt);
			break;
		case	COAP_OPTION_TOKEN :
			dia_tid	= custom_atoi(pt, len);
			req->dia_tid	= dia_tid;
			break;
		case	COAP_OPTION_PROXY_URI :
			protected_strcat (targetID, pt, len, 400);
			break;
		case	COAP_OPTION_URI_HOST :
			protected_strcat (_gUri.host, pt, len, COAP_HOST_SIZE);
			break;
		case	COAP_OPTION_URI_PORT :
			{
			unsigned short	x = (*pt)*256 + *(pt+1);
			strcpy (_gUri.port, custom_itoa(x));
			//protected_strcat (_gUri.port, pt, len, COAP_PORT_SIZE);
			}
			break;
		case	COAP_OPTION_URI_PATH :
			if	(*_gUri.path)
				protected_strcat (_gUri.path, "/", 1, COAP_PATH_SIZE);
			protected_strcat (_gUri.path, pt, len, COAP_PATH_SIZE);
			break;
		case	COAP_OPTION_URI_QUERY :
			if	(!strncmp(pt, "RequestingEntity=", 17)) {
				protected_strcat (reqEntity, pt+17, len-17, 400);
			} else {
				if	(*_gUri.query)
					protected_strcat (_gUri.query, "&", 1, COAP_QUERY_SIZE);
				protected_strcat (_gUri.query, pt, len, COAP_QUERY_SIZE);
			}
			break;
		}

		pt	+= len;
	}

	if	(*_gUri.host) {
		strcat (targetID, COAP_DEFAULT_SCHEME);
		strcat (targetID, "://");
		strcat (targetID, _gUri.host);
	}
	if	(*_gUri.port) {
		strcat (targetID, ":");
		strcat (targetID, _gUri.port);
	}
	if	(*_gUri.path) {
		strcat (targetID, "/");
		strcat (targetID, _gUri.path);
	}
	if	(*_gUri.query) {
		strcat (targetID, "?");
		strcat (targetID, _gUri.query);
	}

	ctt	= (char *)pt;
	cttlen	= len - (pt-msg);

	switch (req->typeReq) {
	case	COAP_REQUEST_GET :
		if	(dia->callbacks && dia->callbacks->cb_diaRetrieveRequest) {
			dia->stat.cb_diaRetrieveRequest	++;
			(*dia->callbacks->cb_diaRetrieveRequest)(dia, reqEntity, targetID, NULL, NULL, dia_tid, NULL);
		}
		break;
	case	COAP_REQUEST_POST :
		if	(dia->callbacks && dia->callbacks->cb_diaCreateRequest) {
			dia->stat.cb_diaCreateRequest	++;
			dia->callbacks->cb_diaCreateRequest(dia, reqEntity, targetID, ctt, cttlen, cttType, NULL, NULL, dia_tid, NULL);
		}
		break;
	case	COAP_REQUEST_PUT :
		if	(dia->callbacks && dia->callbacks->cb_diaUpdateRequest) {
			dia->stat.cb_diaUpdateRequest	++;
			dia->callbacks->cb_diaUpdateRequest(dia, reqEntity, targetID, ctt, cttlen, cttType, NULL, NULL, dia_tid, NULL);
		}
		break;
	case	COAP_REQUEST_DELETE :
		if	(dia->callbacks && dia->callbacks->cb_diaDeleteRequest) {
			dia->stat.cb_diaDeleteRequest	++;
			dia->callbacks->cb_diaDeleteRequest(dia, reqEntity, targetID, NULL, NULL, dia_tid, NULL);
		}
		break;
	}
	return	0;
}

static int hnd_response (dia_context_t *dia, unsigned char *msg, int len, int should_send_ack) {
	unsigned char *pt = msg;
	char	*cttType = "", *scode;
	int	coap_tid, dia_tid = 0;
	int	i;
	int	option = 0;
	int	optcount, cttlen;
	char	*ctt = "";
	char	resourceURI[COAP_PATH_SIZE+1];

	optcount = *pt++ & 0x0f;

	scode	= coap2dia_code(*pt++);
	coap_tid = (*pt++) << 8;
	coap_tid += *pt++;

	//printf ("hnd_response scode=%s coap_tid=%d\n", scode, coap_tid);

	if	(should_send_ack)
		send_ack (dia, coap_tid);

	for	(i=0; i<optcount; i++) {
		int	delta = *pt >> 4;
		int	len = *pt++ & 0x0f;
		if	(len == 15)
			len	+= *pt++;
		option	+= delta;

		switch	(option) {
		case	COAP_OPTION_CONTENT_TYPE :
			cttType	= coap2dia_cttType(*pt);
			break;
		case	COAP_OPTION_URI_PATH :
			break;
		case	COAP_OPTION_TOKEN :
			dia_tid	= custom_atoi(pt, len);
			break;
		case	COAP_OPTION_LOCATION_PATH :
			*resourceURI = 0;
			protected_strcat (resourceURI, pt, len, COAP_PATH_SIZE);
			break;
		}

		pt	+= len;
	}

//	dia_request_t *req = find_request_by_coap_tid(dia, coap_tid);
	dia_request_t *req = find_request_by_dia_tid(dia, dia_tid);
	if	(!req)
		return	-1;

	ctt	= (char *)pt;
	cttlen	= len - (pt-msg);

	dia->stat.complete	++;

	switch (req->typeReq) {
	case	COAP_REQUEST_POST :
		if	(dia->callbacks && dia->callbacks->cb_diaCreateResponse) {
			dia->stat.cb_diaCreateResponse	++;
			dia->callbacks->cb_diaCreateResponse(dia, scode, resourceURI, ctt, len, cttType, NULL, dia_tid);
		}
		break;
	case	 COAP_REQUEST_GET :
		if	(dia->callbacks && dia->callbacks->cb_diaRetrieveResponse) {
			dia->stat.cb_diaRetrieveResponse	++;
			dia->callbacks->cb_diaRetrieveResponse(dia, scode, ctt, cttlen, cttType, NULL, dia_tid);
		}
		break;
	case	COAP_REQUEST_PUT :
		if	(dia->callbacks && dia->callbacks->cb_diaUpdateResponse) {
			dia->stat.cb_diaUpdateResponse	++;
			dia->callbacks->cb_diaUpdateResponse(dia, scode, ctt, len, cttType, NULL, dia_tid);
		}
		break;
	case	COAP_REQUEST_DELETE :
		if	(dia->callbacks && dia->callbacks->cb_diaDeleteResponse) {
			dia->stat.cb_diaDeleteResponse	++;
			dia->callbacks->cb_diaDeleteResponse(dia, scode, NULL, dia_tid);
		}
		break;
	}

	free_request (req);
	return	0;
}

int hnd_ack (dia_context_t *dia, unsigned char *msg, int len) {
	unsigned char *pt = msg + 2;

	int coap_tid = (*pt++) << 8;
	coap_tid += *pt++;

	//printf ("hnd_ack coap_tid=%d\n", coap_tid);

	dia_request_t *req = find_request_by_coap_tid(dia, coap_tid);
	if	(!req) {
		//printf ("Request not found\n");
		return	-1;
	}

	req->coap_tid	= 0;
	if	(req->dia_tid == 0)
		free_request (req);
	return	0;
}

int dIa_input (dia_context_t *dia, unsigned char *msg, int len) {
	unsigned char *pt = msg;

#if defined(LINUX) || defined(MACOSX)
	printf ("<<<\n");
	hexdump (msg, len);
#endif

	if (dia->with_loss) {
		static int lost = 1;
		//lost = 1 - lost;
		if (lost) {
#if defined(LINUX) || defined(MACOSX)
			printf ("input lost\n");
#endif
			lost = 0;
			return	0;
		}
	}
	//	Version
	if	(*pt >> 6 != 1)
		return	-1;

	switch (*pt & 0x30) {
	case	COAP_MESSAGE_ACK :
		if	(*++pt != 0)
			hnd_response (dia, msg, len, 0);	// ACK + response
		else
			hnd_ack (dia, msg, len);	// ACK tout seul
		break;
	case	COAP_MESSAGE_CON :
		switch (*++pt) {
		case	COAP_REQUEST_GET :
		case	COAP_REQUEST_POST :
		case	COAP_REQUEST_PUT :
		case	COAP_REQUEST_DELETE :
			hnd_request (dia, msg, len);
			break;
		default :
			hnd_response (dia, msg, len, 1);
			break;
		}
		break;
	}
	return	0;
}

void dIa_status(dia_context_t *dia) {
#if defined(LINUX) || defined(MACOSX)
	printf ("Table of active requests :\n");
	int	i;
	int now = _gettime(0);
	dia_request_t *req;
	for	(i=0, req=dia->_gRequests; i<COAP_MAX_REQUESTS; i++, req++) {
		if	(req->inlist)
			printf ("Req #%d : coap_tid=%d dia_tid=%ld date=%ds ago\n", i, req->coap_tid, req->dia_tid, (int)(now-req->when));
	}
#endif
}

#ifdef COAP_RETRANSMIT
void dIa_nextTimer(dia_context_t *dia, struct dia_timeval *tv) {
	dia_request_t *req;
/*
	coap_tick_t now;
	coap_ticks(&now);
*/
	int	now = _gettime(0);

	int	i;
	for	(i=0, req=dia->_gRequests; i<COAP_MAX_REQUESTS; i++, req++) {
		if	(req->inlist && (req->when + COAP_DEFAULT_RESPONSE_TIMEOUT <= now)) {
			if	(req->cnt < COAP_DEFAULT_MAX_RETRANSMIT) {
#if defined(LINUX) || defined(MACOSX)
				printf ("retransmit %d %d\n", now - req->when, req->coap_tid);
				printf (">>>\n");
				hexdump (req->raw, req->raw_size);
#endif
				if	(dia->callbacks->cb_sendToNetwork)
					dia->callbacks->cb_sendToNetwork (dia, req->raw, req->raw_size);
				req->cnt	++;
				req->when	= now;
			}
			else {
				if	(dia->callbacks && dia->callbacks->cb_diaErrorResponse) {
					dia->stat.cb_diaErrorResponse	++;
					dia->callbacks->cb_diaErrorResponse(dia, NULL, 0, NULL, NULL, req->dia_tid);
				}
				free_request (req);
			}
		}
	}

	tv->tv_sec	= 0;
	tv->tv_usec	= 10000;
//	printf ("."); fflush(stdout);
}
#else
void dIa_nextTimer(dia_context_t *dia, struct dia_timeval *tv) {
	tv->tv_sec	= 0;
	tv->tv_usec	= 10000;
}
#endif

void dia_withLoss(dia_context_t *dia, char loss) {
	dia->with_loss	= loss;
}


int diaEncodeRequest (dia_context_t *dia, int type, char *reqEntity, char *targetID, char *cttType, int *tid) {
	dia_request_t *req	= alloc_request(dia);
	if	(!req)
		return	-1;

	req->typeReq	= type;
	req->coap_tid	= newCoapTid(dia);
	req->dia_tid	= newDiaTid(dia);
	*tid	= req->dia_tid;

	_gcoapMsg_reinit ();
	int	nbopt = 0;
	int	prevopt = 0;

	_gcoapMsg_putByte (COAP_VERSION + COAP_MESSAGE_CON);
	_gcoapMsg_putByte (type);
	_gcoapMsg_putByte (req->coap_tid >> 8);
	_gcoapMsg_putByte (req->coap_tid & 0xff);

	dIa_split_uri((unsigned char *)targetID, strlen(targetID), &_gUri);

	//	Option #1
	if	(cttType && *cttType) {
		_gcoapMsg_putByte (((COAP_OPTION_CONTENT_TYPE-prevopt)<<4) + 1);
		_gcoapMsg_putByte (dia2coap_cttType(cttType));
		prevopt	= COAP_OPTION_CONTENT_TYPE;
		nbopt	++;
	}

	//	Option #3
	if	(!isLocalHost(_gUri.host)) {
		encodeOption(COAP_OPTION_PROXY_URI, prevopt, targetID);
		prevopt	= COAP_OPTION_PROXY_URI;
		nbopt	++;
	}
	else {
		//	Option #5
		if	(*_gUri.host) {
			encodeOption(COAP_OPTION_URI_HOST, prevopt, _gUri.host);
			prevopt	= COAP_OPTION_URI_HOST;
			nbopt	++;
		}
		//	Option #7
		if	(*_gUri.port && strcmp(_gUri.port, COAP_DEFAULT_PORT)) {
			encodeOption_uint(COAP_OPTION_URI_PORT, prevopt, _gUri.port);
			prevopt	= COAP_OPTION_URI_PORT;
			nbopt	++;
		}
		//	Option #9
		int	nb, i;
		char	*args[20];
		nb = my_strcut(_gUri.path, "/", args, 20);
		for	(i=0; i<nb; i++) {
			encodeOption(COAP_OPTION_URI_PATH, prevopt, args[i]);
			prevopt	= COAP_OPTION_URI_PATH;
			nbopt	++;
		}
	}

	//	Option #11
	char *buf	= custom_itoa (req->dia_tid);
	encodeOption(COAP_OPTION_TOKEN, prevopt, buf);
	prevopt	= COAP_OPTION_TOKEN;
	nbopt	++;

	//	Option #15
	if	(reqEntity && *reqEntity) {
		if (*_gUri.query)
			protected_strcat (_gUri.query, "&", 1, COAP_QUERY_SIZE);
		protected_strcat (_gUri.query, "RequestingEntity=", 17, COAP_QUERY_SIZE);
		protected_strcat (_gUri.query, reqEntity, strlen(reqEntity), COAP_QUERY_SIZE);
	}
	if (*_gUri.query) {
		int	nb, i;
		char	*args[20];
		nb = my_strcut(_gUri.query, "&", args, 20);
		for	(i=0; i<nb; i++) {
			encodeOption(COAP_OPTION_URI_QUERY, prevopt, args[i]);
			prevopt	= COAP_OPTION_URI_QUERY;
			nbopt	++;
		}
	}

	_gcoapMsg[0]	+= nbopt;
	return	0;
}

unsigned char *diaGetBuffer(int *sz) {
	*sz	= _gcoapMsgPtr-_gcoapMsg;
	return	_gcoapMsg;
}

int diaEncodeResponse(dia_context_t *dia, int typeResp, char *scode, char *resourceURI, char* cttType, int tid) {
	dia_request_t *req	= find_request_by_dia_tid(dia, tid);
	if	(!req)
		return	-1;

	req->coap_tid	= newCoapTid(dia);

	_gcoapMsg_reinit ();
	int	nbopt = 0;
	int	prevopt = 0;

	_gcoapMsg_putByte (COAP_VERSION + COAP_MESSAGE_CON);
	_gcoapMsg_putByte (COAP_RESPONSE_CODE(dia2coap_code(scode, typeResp)));
	_gcoapMsg_putByte (dia->coap_tid >> 8);
	_gcoapMsg_putByte (dia->coap_tid & 0xff);

	//	Option #1
	if	(cttType && *cttType) {
		_gcoapMsg_putByte (((COAP_OPTION_CONTENT_TYPE-prevopt)<<4) + 1);
		_gcoapMsg_putByte (dia2coap_cttType(cttType));
		prevopt	= COAP_OPTION_CONTENT_TYPE;
		nbopt	++;
	}

	//	Option #6
	if	(resourceURI && *resourceURI) {
		encodeOption(COAP_OPTION_LOCATION_PATH, prevopt, resourceURI);
		prevopt	= COAP_OPTION_LOCATION_PATH;
		nbopt	++;
	}

	//	Option #11
	char *buf	= custom_itoa (tid);
	encodeOption(COAP_OPTION_TOKEN, prevopt, buf);
	prevopt	= COAP_OPTION_TOKEN;
	nbopt	++;

	_gcoapMsg[0]	+= nbopt;
	//	XXX
	req->dia_tid	= 0;
	return	0;
}
