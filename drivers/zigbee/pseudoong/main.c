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

int dosleep	= 0;
int piggy	= 1;


char	*DoTarget(char *t)
{
	if	(!t || !*t)
	{
		rtl_trace (0, "NULL targetid\n");
		exit(1);
	}
	if	(!*t)
	{
		rtl_trace (0, "empty targetid\n");
		exit(1);
	}
	if	(strncmp(t,"coap://",7) != 0)
	{
		rtl_trace (0, "targetid must be coap://\n");
		exit(1);
	}
	t	= t+7;
	return	strchr(t,'/');
}

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

	if	(dosleep)
		usleep(dosleep*1000);

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
	*sz	= szbuf;
	return	buf;
}

int cb_diaRetrieveRequest(dia_context_t *dia, char *reqEntity, char *targetID,
	Opt *optAttr, Opt *optHeader, int tid,char *peer) {
	char tmp[200];

	if	(!piggy)
		dia->piggy = FALSE;

	rtl_trace (5, "Retrieve %s\n", targetID);
	sprintf (tmp, "%s/%s", root_str, DoTarget(targetID));

	rtl_mkdirp (tmp);
	if	(chdir (tmp) < 0) {
		rtl_trace (2, "chdir errno %d\n", errno);
		return	-1;
	}
	int	sz;
	char *buf = getFile ("create.xml", &sz);
	if (sz > 0)
	diaRetrieveResponse(dia, "STATUS_OK", buf, sz, "application/xml", NULL, tid,peer);
	else
	diaRetrieveResponse(dia, "STATUS_NOT_FOUND", NULL, 0, "", NULL, tid,peer);

	chdir (root_str);
	return	0;
}

void cb_diaRetrieveResponse(dia_context_t *dia, char *scode, void *ctt, int len, char *cttType, Opt *optHeader, int tid) {
}

int cb_diaCreateRequest(dia_context_t *dia, char *reqEntity, char *targetID, void *ctt, int len, char *cttType,
	Opt *optAttr, Opt *optHeader, int tid,char *peer) {
	char tmp[200];
	char tmpTime[64];

	if	(!piggy)
		dia->piggy = FALSE;

	rtl_trace (5, "Create %s\n",targetID);
	sprintf(tmp,"%s/%s",root_str,DoTarget(targetID));
	rtl_mkdirp (tmp);
	if	(chdir (tmp) < 0)
	{
		rtl_trace(5,"cannot chdir() '%s'\n",tmp);
		return	-1;
	}


	int  fd = open("create.xml", O_RDWR | O_CREAT | O_TRUNC, 0600);
	write (fd, ctt, len);
	close (fd);

	printf	("%s  '%s' created sz=%d\n",rtl_hhmmssms(tmpTime), tmp+strlen(root_str),len);

	int	sz;
	char *buf = getFile ("createResponse.xml", &sz);
	diaCreateResponse(dia, "STATUS_CREATED", targetID, buf, sz, "application/xml", NULL, tid,peer);
//	diaCreateResponse(dia, "STATUS_NOT_FOUND", targetID, buf, sz, "application/xml", NULL, tid);

	chdir (root_str);
	return	0;
}

void cb_diaCreateResponse(dia_context_t *dia, char *scode, char *resourceURI, void *ctt, int len, char* cttType,
	Opt *optHeader, int tid) {
}

int cb_diaUpdateRequest(dia_context_t *dia, char *reqEntity, char *targetID, void *ctt, int len, char *cttType,
	Opt *optAttr, Opt *optHeader, int tid,char *peer) {

	rtl_trace (5, "Update %s\n", targetID);
	chdir (root_str);
	if	(unlink (DoTarget(targetID)) < 0)
		perror ("unlink");

	rtl_mkdirp (DoTarget(targetID));
	if	(chdir (DoTarget(targetID)) < 0)
		return	-1;
	int  fd = open("create.xml", O_RDWR | O_CREAT | O_TRUNC, 0600);
	write (fd, ctt, len);
	close (fd);

	int	sz;
	char *buf = getFile ("updateResponse.xml", &sz);
	diaUpdateResponse(dia, "STATUS_OK", buf, sz, "application/xml", NULL, tid,peer);

	chdir (root_str);
	return	0;
}

void cb_diaUpdateResponse(dia_context_t *dia, char *scode, void *ctt, int len, char* cttType, Opt *optHeader, int tid) {
}

int cb_diaDeleteRequest(dia_context_t *dia, char *reqEntity, char *targetID,
	Opt *optAttr, Opt *optHeader, int tid,char *peer) {
	rtl_trace (5, "Delete %s\n", targetID);
	chdir (root_str);
	if	(chdir (DoTarget(targetID)) < 0)
		return -1;
	unlink ("create.xml");
	chdir (root_str);
	if	(rmdir (DoTarget(targetID)) < 0)
		perror ("rmdir");
	diaDeleteResponse(dia, "STATUS_OK", NULL, tid,peer);
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

	while ((opt = getopt(argc, argv, "A:p:v:R:m:P")) != -1) {
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
			strncpy(root_str, optarg, NI_MAXHOST-1);
			root_str[NI_MAXHOST - 1] = '\0';
			break;
		case 'v' :
			log_level = strtol(optarg, NULL, 10);
			break;
		case 'm' :
			dosleep = atoi(optarg);
			break;
		case 'P' :	
			piggy	= 0;
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
	dia_set_log_level(log_level);
	dia = dia_createContext(addr_str, port_str, &callbacks);
	if (!dia) return -1;
	dia_setNonBlocking (dia, 20);

	tabpoll = rtl_pollInit ();
	rtl_pollAdd (tabpoll, dia_getFd(dia), network_input, network_request, dia, NULL);

	signal(SIGINT, handle_sigint);

	while ( !quit ) {
		struct timeval tv;
		dia_nextTimer (dia, &tv);
		result = rtl_poll(tabpoll, tv.tv_sec * 1000);
	}
	dia_freeContext( dia );
	return 0;
}
