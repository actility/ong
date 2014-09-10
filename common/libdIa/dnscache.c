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
/* dnscache.c --
 *
 * Copyright (C) 2012 Actility
 *
 * This file is part of the dIa library libdIa. Please see
 * README for terms of use.
 */

#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <ctype.h>
#include <sys/select.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#include <coap.h>
#include <rtlbase.h>

static coap_list_t *gCache = NULL;

typedef struct {
	char	host[255];
  union {
    struct sockaddr     sa;
    struct sockaddr_storage st;
    struct sockaddr_in  sin;
    struct sockaddr_in6 sin6;
  } dst;
	int	len;
} dns_cache_t;

int
_order_null( void *lhs, void *rhs ) {
	return	-1;
}

char *get_ip_fam(const struct sockaddr *sa) {
    switch(sa->sa_family) {
        case AF_INET:
		return	"IPv4";
        case AF_INET6:
		return	"IPv6";
	default :
		return	"IP??";
	}
}

char *get_ip_str(const struct sockaddr *sa, char *s, size_t maxlen) {
    switch(sa->sa_family) {
        case AF_INET:
            inet_ntop(AF_INET, &(((struct sockaddr_in *)sa)->sin_addr), s, maxlen);
            break;
        case AF_INET6:
            inet_ntop(AF_INET6, &(((struct sockaddr_in6 *)sa)->sin6_addr), s, maxlen);
            break;
        default:
            strncpy(s, "Unknown AF", maxlen);
            return NULL;
    }
    if (!strncmp(s, "::ffff:", 7))
	strcpy (s, s+7);
    return s;
}

// Returns IPv6 address first, then IPv4
static struct addrinfo *
searchBest(struct addrinfo *res) {
	struct addrinfo *pt;

	for (pt=res; pt; pt=pt->ai_next) {
		char tmp[100];
		rtl_trace (5, "best : '%s' '%s'\n", get_ip_fam(pt->ai_addr), get_ip_str(pt->ai_addr, tmp, sizeof(tmp)));
		if	(pt->ai_family == AF_INET6)
			return pt;
	}
	return res;
}

int 
dia_dns_lookup(const char *server, void *dst) {
	struct addrinfo *res, *best;
	struct addrinfo hints;
	static char addrstr[256];
	int error;
	coap_list_t *node;
	dns_cache_t *entry;

	if	(server && *server)
		strcpy (addrstr, server);
	else
		strcpy (addrstr, "localhost");

	//	Already in cache ?
	for	(node = gCache; node; node = node->next) {
		entry	= (dns_cache_t *)node->data;
		if	(!strcmp(addrstr, entry->host)) {
			rtl_trace (5, "dns_lookup : %s found in cache\n", addrstr);
			memcpy (dst, &entry->dst, entry->len);
			return	entry->len;
		}
	}

//	rtl_trace (5, "dns_lookup : %s not found. search in DNS\n", addrstr);

	//	DNS lookup

	memset ((char *)&hints, 0, sizeof(hints));
	hints.ai_socktype = SOCK_DGRAM;
	hints.ai_family = AF_UNSPEC;
#ifdef ANDROID
	error = getaddrinfo(addrstr, NULL, &hints, &res);
#else
	error = getaddrinfo(addrstr, "", &hints, &res);
#endif

	//error = getaddrinfo(addrstr, NULL, NULL, &res);

	if (error) {
		rtl_trace (4, "getaddrinfo: %s\n", gai_strerror(error));
		return -1;
	}

	best	= searchBest (res);
	if	(!best)	return 0;

	int len = best->ai_addrlen;
	memcpy(dst, best->ai_addr, len);

	//	Create DNS cache entry
	entry	= (dns_cache_t *)malloc(sizeof(dns_cache_t));
	strcpy (entry->host, addrstr);
	memcpy(&entry->dst, best->ai_addr, len);
	entry->len	= len;

	char tmp[100];
	rtl_trace (5, "dns_lookup : %s found in DNS %s %s\n", addrstr, get_ip_fam(dst), get_ip_str(dst, tmp, sizeof(tmp)));

	//	Add to the list
	node = coap_new_listnode((void *)entry, NULL);
	coap_insert( &gCache, node, _order_null);

	freeaddrinfo(res);
	return len;
}

int
coap_dns_lookup(str server, unsigned short port, coap_address_t *dst) {
	char host[100];
	strncpy (host, (char *)server.s, server.length);
	host[server.length] = 0;

	int len = dia_dns_lookup (host, (void *)&dst->addr);
	if	(len <= 0)	return len;
	dst->size = len;
	dst->addr.sin.sin_port = htons(port);
	return	len;
}

int
coap_dns_lookup2(const char *host, unsigned short port, coap_address_t *dst) {
	int len = dia_dns_lookup (host, (void *)&dst->addr);
	if	(len <= 0)	return len;
	dst->size = len;
	dst->addr.sin.sin_port = htons(port);
	return	len;
}
