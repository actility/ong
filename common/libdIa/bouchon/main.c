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
static int quit = 0;
boolean Piggy = TRUE;

void handle_sigint(int signum) {
	quit = 1;
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
	char tmp[200];

	rtl_trace (5, "Retrieve %s\n", targetID);

	//	Check STATUS_CONFLICT => 4.09
	if	(!strcmp(targetID, "/conflict")) {
		diaRetrieveResponse(dia, "STATUS_CONFLICT", NULL, 0, "", NULL, tid, peer);
		return	-1;
	}

	if	(!strcmp(targetID, "/separate")) {
		dia->piggy	= FALSE;
	}

	if	(Piggy == FALSE)
		dia->piggy	= FALSE;

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
	rtl_mkdirp (tmp);
	if	(chdir (tmp) < 0) {
		rtl_trace(5,"cannot chdir() '%s'\n",targetID);
		return	-1;
	}
	unlink ("create.xml");
	int  fd = open("create.xml", O_RDWR | O_CREAT, 0600);
	write (fd, ctt, len);
	close (fd);

	if	(Piggy == FALSE)
		dia->piggy	= FALSE;

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

	if	(Piggy == FALSE)
		dia->piggy	= FALSE;

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
	if	(Piggy == FALSE)
		dia->piggy	= FALSE;

	diaDeleteResponse(dia, "STATUS_DELETED", NULL, tid, peer);
	return	0;
}

void cb_diaDeleteResponse(dia_context_t *dia, char *scode, Opt *optHeader, int tid) {
}

void cb_diaErrorResponse(dia_context_t *dia, void *ctt, int len, char* cttType, Opt *optHeader, int tid) {
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


int main(int argc, char **argv) {
	dia_context_t	*dia;
	void		*tabpoll;
	int result;
	char addr_str[NI_MAXHOST] = "";
	char port_str[NI_MAXSERV] = "5683";
	int opt;
	int log_level = 0;
	int initial_sleep = 0;

	rtl_tracesizemax (1024*1024*1024);

	while ((opt = getopt(argc, argv, "A:p:v:R:i:n")) != -1) {
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
		case 'v' :
			log_level = strtol(optarg, NULL, 10);
			dia_set_log_level(log_level);
			break;
		case 'i' :
			initial_sleep = strtol(optarg, NULL, 10);
			break;
		case 'n' :
			Piggy	= FALSE;
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
	dia = dia_createContext(addr_str, port_str, &callbacks);
	if (!dia) return -1;
	dia_setNonBlocking (dia, 20);

	dia_withLoss (dia, 1);

	tabpoll = rtl_pollInit ();
	rtl_pollAdd (tabpoll, dia_getFd(dia), network_input, network_request, dia, NULL);

	signal(SIGINT, handle_sigint);

	if	(initial_sleep)
		sleep (initial_sleep);

	while ( !quit ) {
		struct timeval tv;
		dia_nextTimer (dia, &tv);
		result = rtl_poll(tabpoll, tv.tv_sec * 1000);
	}
	dia_freeContext( dia );
	return 0;
}
