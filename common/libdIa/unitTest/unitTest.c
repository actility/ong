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

#include <unitTest.h>


char root_str[NI_MAXHOST] = "./root";
char reqEntity[100] = "";
static int more = 0;
void		*tabpoll;
dia_context_t	*client;
dia_context_t	*server;
char g_scode[30];
char *g_content = NULL;
int g_content_size = 0;
boolean g_piggy = TRUE;


int network_request (void *tab, int fd, void *ref1, void *ref2, int events) {
	return	POLLIN;
}

int network_input (void *tab, int fd, void *ref1, void *ref2, int events) {
	dia_input(ref1);
	return	0;
}


char *coapstr_to_Cstring(str source) {
	static char tmp[100+1];
	int	n = source.length > 100 ? 100 : source.length;
	memcpy (tmp, source.s, n);
	tmp[n] = 0;
	return tmp;
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

	dia->piggy	= g_piggy;
	coap_uri_t uri;
	dIa_split_uri((unsigned char *)targetID, strlen(targetID), &uri);

	sprintf(tmp,"%s/%s/CONTENT",root_str,coapstr_to_Cstring(uri.path));
	rtl_trace (5, "Retrieve %s\n", tmp);
	int	sz;
	char *buf = getFile (tmp, &sz);
	if (buf)
		diaRetrieveResponse(dia, "STATUS_OK", buf, sz, "application/xml", NULL, tid, peer);
	else
		diaRetrieveResponse(dia, "STATUS_NOT_FOUND", NULL, 0, "", NULL, tid, peer);
	return	0;
}

void cb_diaRetrieveResponse(dia_context_t *dia, char *scode, void *ctt, int len, char *cttType, Opt *optHeader, int tid) {
	rtl_trace (6, "In cb_diaRetrieveResponse len=%d\n", len);
	strcpy (g_scode, scode);
	if	(g_content) {
		free (g_content);
		g_content = NULL;
		g_content_size = 0;
	}
	if	(len > 0) {
		g_content = malloc(len+1);
		memcpy (g_content, ctt, len);
		g_content[len] = 0;
		g_content_size = len;
	}
	more --;
}

int cb_diaCreateRequest(dia_context_t *dia, char *reqEntity, char *targetID, void *ctt, int len, char *cttType,
	Opt *optAttr, Opt *optHeader, int tid, char *peer) {
	char tmp[200];

	coap_uri_t uri;
	dIa_split_uri((unsigned char *)targetID, strlen(targetID), &uri);
	sprintf(tmp,"%s/%s",root_str,coapstr_to_Cstring(uri.path));

	rtl_trace (5, "Create %s of size %d\n", tmp, len);
	rtl_mkdirp (tmp);
	strcat (tmp, "/CONTENT");

	unlink (tmp);
	int  fd = open(tmp, O_RDWR | O_CREAT, 0600);
	write (fd, ctt, len);
	close (fd);

	int	sz;
	char *buf = getFile ("createResponse.xml", &sz);
	diaCreateResponse(dia, "STATUS_CREATED", targetID, buf, sz, "application/xml", NULL, tid, peer);
	return	0;
}


void cb_diaCreateResponse(dia_context_t *dia, char *scode, char *resourceURI, void *ctt, int len, char* cttType,
	Opt *optHeader, int tid) {
	strcpy (g_scode, scode);
	more --;
}

int cb_diaUpdateRequest(dia_context_t *dia, char *reqEntity, char *targetID, void *ctt, int len, char *cttType,
	Opt *optAttr, Opt *optHeader, int tid, char *peer) {
	char tmp[200];

	coap_uri_t uri;
	dIa_split_uri((unsigned char *)targetID, strlen(targetID), &uri);
	sprintf(tmp,"%s/%s/CONTENT",root_str,coapstr_to_Cstring(uri.path));

	rtl_trace (5, "Update %s\n", tmp);
	unlink (tmp);

	int  fd = open(tmp, O_RDWR | O_CREAT | O_TRUNC, 0600);
	write (fd, ctt, len);
	close (fd);

	int	sz;
	char *buf = getFile ("updateResponse.xml", &sz);
	diaUpdateResponse(dia, "STATUS_OK", buf, sz, "application/xml", NULL, tid, peer);
	return	0;
}

void cb_diaUpdateResponse(dia_context_t *dia, char *scode, void *ctt, int len, char* cttType, Opt *optHeader, int tid) {
	strcpy (g_scode, scode);
	more --;
}

int cb_diaDeleteRequest(dia_context_t *dia, char *reqEntity, char *targetID,
	Opt *optAttr, Opt *optHeader, int tid, char *peer) {
	char tmp[200];

	coap_uri_t uri;
	dIa_split_uri((unsigned char *)targetID, strlen(targetID), &uri);
	sprintf(tmp,"%s/%s/CONTENT",root_str,coapstr_to_Cstring(uri.path));

	rtl_trace (5, "Delete %s\n", tmp);
	unlink (tmp);
	diaDeleteResponse(dia, "STATUS_DELETED", NULL, tid, peer);
	return	0;
}

void cb_diaDeleteResponse(dia_context_t *dia, char *scode, Opt *optHeader, int tid) {
	strcpy (g_scode, scode);
	more --;
}

void cb_diaErrorResponse(dia_context_t *dia, void *ctt, int len, char* cttType, Opt *optHeader, int tid) {
	printf ("*** cb_diaErrorResponse\n");
	strcpy (g_scode, "ERROR");
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


void do_createBin(dia_context_t *dia, char *targetID, int sz) {
	static char _big[1024*1024];
	int	tid;
	targetID = strdup(targetID);
	diaCreateRequest (dia, reqEntity, targetID, _big, sz, "application/xml", NULL, NULL, &tid);
	free (targetID);
	more ++;
}

void do_create(dia_context_t *dia, char *targetID, char *file) {
	int	sz, tid;
	char *buf = getFile (file, &sz);
	targetID = strdup(targetID);
	diaCreateRequest (dia, reqEntity, targetID, buf, sz, "application/xml", NULL, NULL, &tid);
	free (targetID);
	more ++;
}

void do_retrieve(dia_context_t *dia, char *targetID) {
	int	tid;
	targetID = strdup(targetID);
	diaRetrieveRequest (dia, reqEntity, targetID, NULL, NULL, &tid);
	free (targetID);
	more ++;
}

void do_update(dia_context_t *dia, char *targetID, char *file) {
	int	sz, tid;
	char *buf = getFile (file, &sz);
	targetID = strdup(targetID);
	diaUpdateRequest (dia, reqEntity, targetID, buf, sz, "application/xml", NULL, NULL, &tid);
	free (targetID);
	more ++;
}

void do_delete(dia_context_t *dia, char *targetID) {
	int	tid;
	targetID = strdup(targetID);
	diaDeleteRequest (dia, reqEntity, targetID, NULL, NULL, &tid);
	free (targetID);
	more ++;
}

wait_response() {
	while ( more ) {
		//printf ("more %d\n", more);
		struct timeval tv;
		dia_nextTimer (client, &tv);
		rtl_poll(tabpoll, tv.tv_sec * 1000);
	}
}

int main(int argc, char **argv) {
	rtl_tracesizemax (10*1024*1024);
	dia_set_log_level(9);

	tabpoll = rtl_pollInit ();

	client = dia_createContext("", "5683", &callbacks);
	if (!client) return -1;
	dia_setNonBlocking (client, 20);
	g_piggy	= TRUE;

	server = dia_createContext("", "15683", &callbacks);
	if (!server) return -1;
	dia_setNonBlocking (server, 20);

	rtl_pollAdd (tabpoll, dia_getFd(client), network_input, network_request, client, NULL);
	rtl_pollAdd (tabpoll, dia_getFd(server), network_input, network_request, server, NULL);

	STInit();

	//	Now IPv4 tests
	do_create (client, "coap://127.0.0.1:15683/test1", "test1.xml");
	wait_response();
	STAssertStringEquals (g_scode, "STATUS_CREATED", "dIa CREATE");

	//	2048 document
	do_create (client, "coap://127.0.0.1:15683/doc2048", "doc2048.xml");
	wait_response();
	STAssertStringEquals (g_scode, "STATUS_CREATED", "dIa CREATE 2048");

	do_retrieve (client, "coap://127.0.0.1:15683/doc2048");
	wait_response();
	STAssertStringEquals (g_scode, "STATUS_OK", "dIa RETRIEVE 2048");
	STAssertEquals (g_content_size, 2048, "dIa RETRIEVE 2048");

	//	Different size documents
	srand (time(0)+getpid());
	int i;
	for	(i=0; i<100; i++) {
		int sz	= rand() % 200000;
		do_createBin (client, "coap://127.0.0.1:15683/binary", sz);
		wait_response();
		STAssertStringEquals (g_scode, "STATUS_CREATED", "dIa CREATE %d bytes document", sz);
		do_retrieve (client, "coap://127.0.0.1:15683/binary");
		wait_response();
		STAssertStringEquals (g_scode, "STATUS_OK", "dIa RETRIEVE %d bytes document", sz);
		STAssertEquals (g_content_size, sz, "dIa RETRIEVE %d bytes document", sz);
	}

	do_retrieve (client, "coap://127.0.0.1:15683/test1");
	wait_response();
	STAssertStringEquals (g_scode, "STATUS_OK", "dIa RETRIEVE");
	STAssertStringEquals (g_content, "<xml>32.5</xml>\n", "dIa RETRIEVE");

	g_piggy	= FALSE;
	do_retrieve (client, "coap://127.0.0.1:15683/test1");
	wait_response();
	STAssertStringEquals (g_scode, "STATUS_OK", "dIa RETRIEVE no piggy");
	STAssertStringEquals (g_content, "<xml>32.5</xml>\n", "dIa RETRIEVE no piggy");
	g_piggy	= TRUE;

	dia_withLoss (client, TRUE);
	do_retrieve (client, "coap://127.0.0.1:15683/test1");
	wait_response();
	STAssertStringEquals (g_scode, "STATUS_OK", "dIa RETRIEVE with loss");
	STAssertStringEquals (g_content, "<xml>32.5</xml>\n", "dIa RETRIEVE with loss");
	dia_withLoss (client, FALSE);

	do_update (client, "coap://127.0.0.1:15683/test1", "test2.xml");
	wait_response();
	STAssertStringEquals (g_scode, "STATUS_OK", "dIa UPDATE");

	do_retrieve (client, "coap://127.0.0.1:15683/test1");
	wait_response();
	STAssertStringEquals (g_scode, "STATUS_OK", "dIa RETRIEVE after UPDATE");
	STAssertStringEquals (g_content, "<xml>44.8</xml>\n", "dIa RETRIEVE after UPDATE");

	do_delete (client, "coap://127.0.0.1:15683/test1");
	wait_response();
	STAssertStringEquals (g_scode, "STATUS_DELETED", "dIa DELETE");

	do_retrieve (client, "coap://127.0.0.1:15683/test1");
	wait_response();
	STAssertStringEquals (g_scode, "STATUS_NOT_FOUND", "dIa RETRIEVE after DELETE");


	rtl_pollRmv (tabpoll, dia_getFd(client));
	rtl_pollRmv (tabpoll, dia_getFd(server));

	dia_freeContext(client);
	dia_freeContext(server);


	//	Now IPv6 tests
	client = dia_createContext("::", "5683", &callbacks);
	if (!client) return -1;
	dia_setNonBlocking (client, 20);

	server = dia_createContext("::", "15683", &callbacks);
	if (!server) return -1;
	dia_setNonBlocking (server, 20);

	rtl_pollAdd (tabpoll, dia_getFd(client), network_input, network_request, client, NULL);
	rtl_pollAdd (tabpoll, dia_getFd(server), network_input, network_request, server, NULL);

	do_create (client, "coap://[::1]:15683/test1", "test1.xml");
	wait_response();
	STAssertStringEquals (g_scode, "STATUS_CREATED", "dIa CREATE IPv6");

	do_retrieve (client, "coap://[::1]:15683/test1");
	wait_response();
	STAssertStringEquals (g_scode, "STATUS_OK", "dIa RETRIEVE IPv6");
	STAssertStringEquals (g_content, "<xml>32.5</xml>\n", "dIa RETRIEVE IPv6");

	//	Now proxy !
	diaProxy (client, "coap://[::1]:15683");
	do_create (client, "http://nsc1.actility.com/test1", "test1.xml");
	wait_response();
	STAssertStringEquals (g_scode, "STATUS_CREATED", "dIa CREATE proxied");

	STStatistics();
	if	(STErrorCount() != 0)
		exit (1);

	rtl_pollRmv (tabpoll, dia_getFd(client));
	rtl_pollRmv (tabpoll, dia_getFd(server));

	dia_freeContext(client);
	dia_freeContext(server);

	exit (0);
}
