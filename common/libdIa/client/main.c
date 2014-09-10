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
/* main.c -- 
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
#include <sys/stat.h>
#include <errno.h>
#include <signal.h>
#include <poll.h>

#include <rtlbase.h>
#include <dIa.h>


char root_str[NI_MAXHOST] = "/tmp";
char method[100];
char reqEntity[100] = "";
char file[100];
static int more = 0;
static int quit = 0;

void handle_sigint(int signum) {
	quit = 1;
	exit (0);
}

int network_request (void *tab, int fd, void *ref1, void *ref2, int events) {
	return	POLLIN;
}

int network_input (void *tab, int fd, void *ref1, void *ref2, int events) {
	dia_input(ref1);
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
	return	0;
}

void cb_diaRetrieveResponse(dia_context_t *dia, char *scode, void *ctt, int len, char *cttType, Opt *optHeader, int tid) {
	rtl_trace (6, "Dans cb_diaRetrieveResponse len=%d\n", len);
	write (1, ctt, len);
	more --;
}

int cb_diaCreateRequest(dia_context_t *dia, char *reqEntity, char *targetID, void *ctt, int len, char *cttType,
	Opt *optAttr, Opt *optHeader, int tid, char *peer) {
	return	0;
}

void cb_diaCreateResponse(dia_context_t *dia, char *scode, char *resourceURI, void *ctt, int len, char* cttType,
	Opt *optHeader, int tid) {
	more --;
}

int cb_diaUpdateRequest(dia_context_t *dia, char *reqEntity, char *targetID, void *ctt, int len, char *cttType,
	Opt *optAttr, Opt *optHeader, int tid, char *peer) {
	return	0;
}

void cb_diaUpdateResponse(dia_context_t *dia, char *scode, void *ctt, int len, char* cttType, Opt *optHeader, int tid) {
	more --;
}

int cb_diaDeleteRequest(dia_context_t *dia, char *reqEntity, char *targetID,
	Opt *optAttr, Opt *optHeader, int tid, char *peer) {
	return	0;
}

void cb_diaDeleteResponse(dia_context_t *dia, char *scode, Opt *optHeader, int tid) {
	more --;
}

void cb_diaErrorResponse(dia_context_t *dia, void *ctt, int len, char* cttType, Opt *optHeader, int tid) {
	printf ("*** cb_diaErrorResponse\n");
	more --;
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
	cb_diaErrorResponse
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
		//diaRetrieveRequest (dia, NULL, targetID, NULL, NULL, &tid);
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
	fprintf (stderr, "-P <proxy>\n");
	exit (1);
}

int main(int argc, char **argv) {
	void		*tabpoll;
	dia_context_t	*dia;
	int result;
	char addr_str[NI_MAXHOST] = "";
	char Proxy_str[NI_MAXHOST] = "";
	char port_str[NI_MAXSERV] = "15683";
	int opt;
	int log_level = 0;
	int count = 1, i;
	int group = 1;
	char loss = 0;

	while ((opt = getopt(argc, argv, "A:p:v:m:f:n:g:R:lP:")) != -1) {
		switch (opt) {
		case 'A' :
			strncpy(addr_str, optarg, NI_MAXHOST-1);
			addr_str[NI_MAXHOST - 1] = '\0';
			break;
		case 'p' :
			strncpy(port_str, optarg, NI_MAXSERV-1);
			port_str[NI_MAXSERV - 1] = '\0';
			break;
		case 'm' :
			strcpy (method, optarg);
			break;
		case 'R' :
			strcpy (reqEntity, optarg);
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
		case 'l' :
			loss = 1;
			break;
		case 'v' :
			log_level = strtol(optarg, NULL, 10);
			break;
		case 'P' :
			strncpy(Proxy_str, optarg, NI_MAXHOST-1);
			Proxy_str[NI_MAXHOST - 1] = '\0';
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

	dia_set_log_level(log_level);
	dia = dia_createContext(addr_str, port_str, &callbacks);
	if (!dia) return -1;
	dia_setNonBlocking (dia, 20);

	dia_withLoss (dia, loss);

	if (*Proxy_str)
		diaProxy (dia, Proxy_str);

	rtl_tracesizemax (1024*1024*1024);
	tabpoll = rtl_pollInit ();
	rtl_pollAdd (tabpoll, dia_getFd(dia), network_input, network_request, dia, NULL);

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
		else
			usage ();

		while ( more ) {
			struct timeval tv;
			dia_nextTimer (dia, &tv);
			result = rtl_poll(tabpoll, tv.tv_sec * 1000);
		}
		if	(quit)
			break;
	}

	dia_freeContext( dia );
	return 0;
}
