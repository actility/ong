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
/* client.c -- 
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
#include <signal.h>
#include <unistd.h>
#include <poll.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

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


char root_str[200] = "/tmp";
char method[100];
char reqEntity[100] = "";
char file[100];
static int more = 0;
static int quit = 0;
dia_address_t addr;
int sock;

void handle_sigint(int signum) {
	quit = 1;
	exit (0);
}

int network_request (void *tab, int fd, void *ref1, void *ref2, int events) {
	return	POLLIN;
}

int network_input (void *tab, int fd, void *ref1, void *ref2, int events) {
	unsigned char	msg[1500];
	struct sockaddr from;
	unsigned int len = sizeof(from);
	len = recvfrom (sock, msg, sizeof(msg), 0, &from, &len);
//printf ("Read %d\n", len);
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

void cb_diaRetrieveResponse(dia_context_t *dia, char *scode, void *ctt, int len, char *cttType, Opt *optHeader, int tid) {
	fwrite (ctt, len, 1, stdout);
	more --;
}

void cb_diaCreateResponse(dia_context_t *dia, char *scode, char *resourceURI, void *ctt, int len, char* cttType,
	Opt *optHeader, int tid) {
	more --;
}

void cb_diaUpdateResponse(dia_context_t *dia, char *scode, void *ctt, int len, char* cttType, Opt *optHeader, int tid) {
	more --;
}

void cb_diaDeleteResponse(dia_context_t *dia, char *scode, Opt *optHeader, int tid) {
	more --;
}

void cb_diaErrorResponse(dia_context_t *dia, void *ctt, int len, char* cttType, Opt *optHeader, int tid) {
	printf ("*** cb_diaErrorResponse\n");
	more --;
}

int cb_write2Network(dia_context_t *dia, unsigned char *buf, int sz) {
//	printf ("Write Net %d\n", sz);
	if (sendto (sock, buf, sz, 0, &addr.addr.sa, addr.size) < 0)
		perror ("sendto");
	return	0;
}

dia_callbacks_t callbacks = {
	NULL,
	cb_diaRetrieveResponse,
	NULL,
	cb_diaCreateResponse,
	NULL,
	cb_diaUpdateResponse,
	NULL,
	cb_diaDeleteResponse,
	cb_diaErrorResponse,
	cb_write2Network
};

void do_create(dia_context_t *dia, char *targetID, int group) {
	int	sz, tid, i;
	char *buf = getFile (file, &sz);
	for	(i=0; i<group; i++) {
		diaCreateRequest (dia, reqEntity, targetID, buf, sz, "application/xml", NULL, NULL, &tid);
		more ++;
	}
}

void do_retrieve(dia_context_t *dia, char *targetID, int group) {
	int	tid, i;
	for	(i=0; i<group; i++) {
		diaRetrieveRequest (dia, reqEntity, targetID, NULL, NULL, &tid);
		more ++;
	}
}

void do_update(dia_context_t *dia, char *targetID, int group) {
	int	sz, tid, i;
	char *buf = getFile (file, &sz);
	for	(i=0; i<group; i++) {
		diaUpdateRequest (dia, reqEntity, targetID, buf, sz, "application/xml", NULL, NULL, &tid);
		more ++;
	}
}

void do_delete(dia_context_t *dia, char *targetID, int group) {
	int	tid, i;
	for	(i=0; i<group; i++) {
		diaDeleteRequest (dia, reqEntity, targetID, NULL, NULL, &tid);
		more ++;
	}
}

void usage() {
	fprintf (stderr, "-m create|retrieve|update|delete\n");
	fprintf (stderr, "-A <host>\n");
	fprintf (stderr, "-p <port>\n");
	fprintf (stderr, "-v <verbose level>\n");
	fprintf (stderr, "-f <file>\n");
	fprintf (stderr, "-n <count>\n");
	fprintf (stderr, "-g <group requests>\n");
	fprintf (stderr, "-R <requestingEntity>\n");
	fprintf (stderr, "-l (with loss)\n");
	exit (1);
}

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
	void		*tabpoll;
	dia_context_t	*dia;
	int result;
	char addr_str[200] = "::1";
	char port_str[10] = "5683";
	int opt;
	int log_level = 0;
	int count = 1, i;
	int group = 1;
	char loss = 0;

	while ((opt = getopt(argc, argv, "A:p:v:m:f:n:g:R:l")) != -1) {
		switch (opt) {
		case 'A' :
			strncpy(addr_str, optarg, 200-1);
			addr_str[200 - 1] = '\0';
			break;
		case 'p' :
			strncpy(port_str, optarg, 10-1);
			port_str[10 - 1] = '\0';
			break;
		case 'm' :
			strcpy (method, optarg);
			break;
		case 'R' :
			strcpy (reqEntity, optarg);
			break;
		case 'l' :
			loss = 1;
			break;
		case 'f' :
			strcpy (file, optarg);
			break;
		case 'n' :
			count = atoi(optarg);
			break;
		case 'g' :
			group = atoi(optarg);
			break;
		case 'v' :
			log_level = strtol(optarg, NULL, 10);
			break;
		default:
			usage();
			exit( 1 );
		}
	}

	if (optind == argc) {
		fprintf (stderr, "You must specify a targetID\n");
		usage();
		exit (1);
	}

	char *targetID	= argv[optind];

	dia = dia_createContext(&callbacks);
	if (!dia) {
		fprintf (stderr, "dia_createContext failed\n");
		usage();
		exit (1);
	}

	dia_withLoss (dia, loss);

	tabpoll = rtl_pollInit ();

	sock	= resolve_address(addr_str, &addr);
	addr.addr.sin.sin_port = htons(atoi(port_str));

	rtl_pollAdd (tabpoll, sock, network_input, network_request, dia, NULL);

	signal(SIGINT, handle_sigint);

	for	(i=0; i<count; i++) {
		if	(!strcmp(method, "create"))
			do_create (dia, targetID, group);
		else if	(!strcmp(method, "retrieve"))
			do_retrieve (dia, targetID, group);
		else if	(!strcmp(method, "update"))
			do_update (dia, targetID, group);
		else if	(!strcmp(method, "delete"))
			do_delete (dia, targetID, group);
		else {
			usage ();
			exit (1);
		}

		dIa_status (dia);

		while ( more ) {
			struct dia_timeval tv;
			dIa_nextTimer (dia, &tv);
			result = rtl_poll(tabpoll, (tv.tv_sec*1000) + (tv.tv_usec/1000));
		}

		dIa_status (dia);

		if	(quit)
			break;
	}

	return 0;
}
