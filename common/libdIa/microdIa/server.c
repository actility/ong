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
/* server.c -- 
 *
 * Copyright (C) 2012 Actility
 *
 * This file is part of the dIa library libdIa. Please see
 * README for terms of use.
 */

#include <stdio.h>
#include <strings.h>
#include <string.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <ctype.h>
#include <netdb.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <sys/stat.h>
#include <errno.h>
#include <signal.h>
#include <poll.h>

#include <rtlbase.h>
#include "microdIa.h"

typedef struct dia_address_t {
  socklen_t size;		/**< size of addr */
  union {
    struct sockaddr     sa;
    struct sockaddr_storage st;
    struct sockaddr_in  sin;
    struct sockaddr_in6 sin6;
  } addr;
} dia_address_t;


char root_str[NI_MAXHOST] = "/tmp";
static int quit = 0;
dia_address_t addr;
dia_address_t from;
int sock;

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
    return s;
}

void handle_sigint(int signum) {
	quit = 1;
}

int network_request (void *tab, int fd, void *ref1, void *ref2, int events) {
	return	POLLIN;
}

int network_input (void *tab, int fd, void *ref1, void *ref2, int events) {
	unsigned char	msg[1500];

	memset(&from, 0, sizeof(dia_address_t));
	from.size = sizeof(from.addr);

	int len = recvfrom (sock, msg, sizeof(msg), 0, &from.addr.sa, &from.size);

	char str[200];
	rtl_trace (5, "Read %d from %s\n", len, get_ip_str(&from.addr.sa, str, 200));
	if (len > 0)
		dIa_input (ref1, msg, len);
	return	0;
}


char *getFile (char *fn, int *sz) {
	static char *buf = NULL;
	static int	szbuf = 0;
	struct stat st;

	*sz	= 0;

	int	fd = open(fn, O_RDONLY);
	if	(fd < 0)
		return	NULL;

	if	(fstat(fd, &st) < 0) {
		close (fd);
		return	NULL;
	}

	if	(!buf) {
		buf	= malloc(st.st_size);
		szbuf	= st.st_size;
	}
	else if (st.st_size > szbuf) {
		buf	= realloc(buf, st.st_size);
		szbuf	= st.st_size;
	}

	read (fd, buf, st.st_size);

	close (fd);
	*sz	= st.st_size;
	return	buf;
}

int cb_diaRetrieveRequest(dia_context_t *dia, char *reqEntity, char *targetID,
	Opt *optAttr, Opt *optHeader, int tid, char *peer) {
	char tmp[200];

	rtl_trace (5, "Retrieve %s\n", targetID);

	//	Check STATUS_CONFLICT => 4.09
	if	(!strcmp(targetID, "/conflict")) {
		diaRetrieveResponse(dia, "STATUS_CONFLICT", NULL, 0, "", NULL, tid, peer);
		return	-1;
	}
/*
	if	(!strcmp(targetID, "/separate")) {
		dia->piggy	= FALSE;
	}
*/
	sprintf (tmp, "%s/%s", root_str, targetID);
	rtl_trace (5, "search into %s\n", tmp);

	rtl_mkdirp (tmp);
	if	(chdir (tmp) < 0) {
		rtl_trace (2, "chdir errno %d\n", errno);
		diaRetrieveResponse(dia, "STATUS_NOT_FOUND", NULL, 0, "", NULL, tid, peer);
		return	-1;
	}
	int	sz;
	char *buf = getFile ("create.xml", &sz);
	if (sz > 0)
		diaRetrieveResponse(dia, "STATUS_OK", buf, sz, "application/xml", NULL, tid, peer);
	else
		diaRetrieveResponse(dia, "STATUS_NOT_FOUND", NULL, 0, "", NULL, tid, peer);
	return	0;
}

void cb_diaRetrieveResponse(dia_context_t *dia, char *scode, void *ctt, int len, char *cttType, Opt *optHeader, int tid) {
}

int cb_diaCreateRequest(dia_context_t *dia, char *reqEntity, char *targetID, void *ctt, int len, char *cttType,
	Opt *optAttr, Opt *optHeader, int tid, char *peer) {
	char tmp[200];

	sprintf(tmp,"%s/%s",root_str,targetID);
	rtl_trace (5, "Create %s\n", tmp);
	rtl_trace (5, "reqEntity (%s)\n", reqEntity);
	rtl_mkdirp (tmp);
	if	(chdir (tmp) < 0) {
		rtl_trace(5,"cannot chdir() '%s'\n",targetID);
		return	-1;
	}
	unlink ("create.xml");
	int  fd = open("create.xml", O_RDWR | O_CREAT, 0600);
	write (fd, ctt, len);
	close (fd);

	int	sz;
	char *buf = getFile ("createResponse.xml", &sz);
	diaCreateResponse(dia, "STATUS_CREATED", targetID, buf, sz, "application/xml", NULL, tid, peer);
	return	0;
}

void cb_diaCreateResponse(dia_context_t *dia, char *scode, char *resourceURI, void *ctt, int len, char* cttType,
	Opt *optHeader, int tid) {
}

int cb_diaUpdateRequest(dia_context_t *dia, char *reqEntity, char *targetID, void *ctt, int len, char *cttType,
	Opt *optAttr, Opt *optHeader, int tid, char *peer) {
	char tmp[200];

	sprintf(tmp,"%s/%s",root_str,targetID);
	rtl_trace (5, "Update %s\n", tmp);

	rtl_mkdirp (tmp);
	if	(chdir (tmp) < 0) {
		rtl_trace(5,"cannot chdir() '%s'\n",tmp);
		return	-1;
	}
	unlink ("create.xml");

	int  fd = open("create.xml", O_RDWR | O_CREAT | O_TRUNC, 0600);
	write (fd, ctt, len);
	close (fd);

	int	sz;
	char *buf = getFile ("updateResponse.xml", &sz);
	diaUpdateResponse(dia, "STATUS_OK", buf, sz, "application/xml", NULL, tid, peer);
	return	0;
}

void cb_diaUpdateResponse(dia_context_t *dia, char *scode, void *ctt, int len, char* cttType, Opt *optHeader, int tid) {
}

int cb_diaDeleteRequest(dia_context_t *dia, char *reqEntity, char *targetID,
	Opt *optAttr, Opt *optHeader, int tid, char *peer) {
	char tmp[200];

	sprintf(tmp,"%s/%s",root_str,targetID);
	rtl_trace (5, "Delete %s\n", tmp);

	rtl_mkdirp (tmp);
	if	(chdir (tmp) < 0) {
		rtl_trace(5,"cannot chdir() '%s'\n",tmp);
		return	-1;
	}
	unlink ("create.xml");
/*
	chdir (root_str);
	if	(rmdir (targetID) < 0)
		perror ("rmdir");
*/
	diaDeleteResponse(dia, "STATUS_DELETED", NULL, tid, peer);
	return	0;
}

void cb_diaDeleteResponse(dia_context_t *dia, char *scode, Opt *optHeader, int tid) {
}

void cb_diaErrorResponse(dia_context_t *dia, void *ctt, int len, char* cttType, Opt *optHeader, int tid) {
}

int cb_write2Network(dia_context_t *dia, unsigned char *buf, int sz) {
	char str[200];
	rtl_trace (5, "Write %d to %s\n", sz, get_ip_str(&from.addr.sa, str, 200));
	if (sendto (sock, buf, sz, 0, &from.addr.sa, from.size) < 0)
		perror ("sendto");
	return	0;
}

dia_callbacks_t callbacks = {
	cb_diaRetrieveRequest,
	cb_diaRetrieveResponse,
	cb_diaCreateRequest,
	cb_diaCreateResponse,
	cb_diaUpdateRequest,
	cb_diaUpdateResponse,
	cb_diaDeleteRequest,
	cb_diaDeleteResponse,
	cb_diaErrorResponse,
	cb_write2Network
};

int resolve_address(const char *server, dia_address_t  *dst) {
	struct addrinfo *res, *ainfo;
	struct addrinfo hints;
	static char addrstr[256];
	int error, sock;

	strcpy (addrstr, server && *server ? server : "localhost");

	memset ((char *)&hints, 0, sizeof(hints));
	hints.ai_socktype = SOCK_DGRAM;
	hints.ai_family = AF_UNSPEC;

	error = getaddrinfo(addrstr, "", &hints, &res);

	if (error != 0) {
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(error));
		return error;
	}

	for (ainfo = res; ainfo != NULL; ainfo = ainfo->ai_next) {
		switch (ainfo->ai_family) {
		case AF_INET6:
		case AF_INET:
			memcpy(&dst->addr.sa, ainfo->ai_addr, ainfo->ai_addrlen);
			dst->size	= ainfo->ai_addrlen;
			sock = socket(ainfo->ai_family, ainfo->ai_socktype, ainfo->ai_protocol);
			freeaddrinfo(res);
			return	sock;
		default:
			;
		}
	}

	freeaddrinfo(res);
	return -1;
}

int main(int argc, char **argv) {
	dia_context_t	*dia;
	void		*tabpoll;
	int result;
	char addr_str[NI_MAXHOST] = "::1";
	char port_str[NI_MAXSERV] = "5683";
	int opt;
	int log_level = 0;
	int initial_sleep = 0;
	char loss = 0;

	rtl_tracesizemax (1024*1024*1024);

	while ((opt = getopt(argc, argv, "A:p:v:R:i:l")) != -1) {
		switch (opt) {
		case 'A' :
			strncpy(addr_str, optarg, NI_MAXHOST-1);
			addr_str[NI_MAXHOST - 1] = '\0';
			break;
		case 'p' :
			strncpy(port_str, optarg, NI_MAXSERV-1);
			port_str[NI_MAXSERV - 1] = '\0';
			break;
		case 'R' :
			*root_str = 0;
			if	(*optarg != '/') {
				getcwd (root_str, NI_MAXHOST-1);
				strcat (root_str, "/");
			}
			strncat(root_str, optarg, NI_MAXHOST-1);
			root_str[NI_MAXHOST - 1] = '\0';
			rtl_trace (5, "ROOT set to %s\n", root_str);
			break;
		case 'l' :
			loss = 1;
			break;
		case 'v' :
			log_level = strtol(optarg, NULL, 10);
			rtl_tracelevel (log_level);
			break;
		case 'i' :
			initial_sleep = strtol(optarg, NULL, 10);
			break;
		default:
//			usage( argv[0], PACKAGE_VERSION );
			exit( 1 );
		}
	}

	mkdir (root_str, 0700);
	chdir (root_str);
/*
	if	(chroot (root_str) < 0) {
		perror ("chroot");
		exit (1);
	}
*/
	dia = dia_createContext(&callbacks);
	if (!dia) return -1;

	dia_withLoss (dia, loss);

	sock	= resolve_address(addr_str, &addr);
	addr.addr.sin.sin_port = htons(atoi(port_str));
	if (bind(sock, &addr.addr.sa, addr.size) < 0) {
		perror ("bind");
		exit (2);
	}

	tabpoll = rtl_pollInit ();
	rtl_pollAdd (tabpoll, sock, network_input, network_request, dia, NULL);

	signal(SIGINT, handle_sigint);

	if	(initial_sleep)
		sleep (initial_sleep);

	while ( !quit ) {
		struct dia_timeval tv;
		dIa_nextTimer (dia, &tv);
		result = rtl_poll(tabpoll, (tv.tv_sec*1000) + (tv.tv_usec/1000));
	}
	return 0;
}
