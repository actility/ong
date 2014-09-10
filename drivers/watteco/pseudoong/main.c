
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
#include <ctype.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <signal.h>
#include <poll.h>
#include <pthread.h>
#include <libgen.h>

#include <rtlbase.h>
#include <dIa.h>


char	link_format[10*1024];

char root_str[NI_MAXHOST] = "/tmp";
static int quit = 0;

int dosleep	= 0;
int piggy	= 1;


char	*DoTarget(char *t,int *cis)
{
	static	char	target[512];

	*cis	= 0;
	target[0] = '\0';
	if	(!t)
	{
		rtl_trace (0, "NULL targetid\n");
		return	target;
	}
	if	(!*t)
	{
		rtl_trace (0, "empty targetid\n");
		return	target;
	}
	if	(strncmp(t,"coap://",7) == 0)
	{
		t	= t + 7;
	}
	t	= strchr(t,'/');
	if	(!t)
	{
		rtl_trace (0, "invalid targetid '%s'\n",t);
		return	target;
	}
	strcpy	(target,t);
	t	= strrchr(target,'/');
	if	(strncmp(target,"/M2M",4) != 0)	// TODO
	{
		if	(t)
		{
			char	sv;
	
			sv	= *t;
			if	(strcmp(t+1,"contentInstances") == 0)
			{
				strcat	(target,"/latest");
				*cis	= 1;
			}
			*t	= sv;
		}
	}
	if	(strstr(target,"/latest"))
	{
		*cis	= 1;
	}

	return	target;
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
	int	szbuf = 0;
	struct	stat st;

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

	*sz	= read (fd, buf, st.st_size);
	close	(fd);
	return	buf;
}

int cb_diaRetrieveRequestCopper(dia_context_t *dia, char *reqEntity, char *targetID, Opt *optAttr, Opt *optHeader, int tid,char *peer)
{
	char tmp[200];
	int cis	= 0;		// contentInstances
	int cnt	= 0;		// containers
	int dirlist = 0;	// .../*
	char *sl;
	int num;
	int sz;
	char *buf;

	if	(strcmp(targetID,"/.well-known/core") == 0)
	{
		buf = "</M2M/applications>;rt=\"m2m\";\
		</M2M/accessRights>;rt=\"m2m\"";
		sz	= strlen(buf);
		diaRetrieveResponse(dia,"STATUS_OK",buf,sz,
				"application/link-format",NULL,tid,peer);
		return	0;
	}

	memcpy	(targetID,"/m2m",4);	// TODO

	rtl_trace (0,"Copper Retrieve %s\n",targetID);

	dirlist	= 0;
	num	= -1;
	if	(strcmp(basename(targetID),"*") == 0)
	{
		dirlist	= 1;
		sl	= strrchr(targetID,'/');
		if	(sl)
			*sl	= '\0';
	}
	else
	{
		if	(strcmp(basename(targetID),"containers") == 0)
		{
			cnt	= 1;
		}
		if	(strcmp(basename(targetID),"contentInstances") == 0)
		{
			cis	= 1;
		}
		if	(strstr(targetID,"contentInstances"))
		{
			sl	= strrchr(targetID,'/');
			if	(sl && *(sl+1) >= '0' && *(sl+1) <= '9')
			{
				*sl	= '\0';
				sl++;
				num	= atoi(sl);
			}
		}
	}

	sprintf (tmp,"%s/%s",root_str,targetID);
//	rtl_mkdirp (tmp);
	if	(chdir (tmp) < 0) 
	{
		rtl_trace (0,"cannot chdir '%s'\n",tmp);
		diaRetrieveResponse(dia,"STATUS_NOT_FOUND",NULL,0,"",NULL,tid,peer);
		return	-1;
	}
	printf("chdir(%s) ok\n",tmp);

	if	(dirlist)
		goto	do_link_format;

	if	(num < 0)
		sprintf	(tmp,"create.xml");
	else
		sprintf	(tmp,"create-%d.xml",num);
	buf = getFile (tmp,&sz);
	if (sz > 0)
	{
	printf	("'%s' num=%d retrieved sz=%d\n",targetID,num,sz);
	diaRetrieveResponse(dia,"STATUS_OK",buf,sz,"application/xml",NULL,tid,peer);
	return	0;
	}

do_link_format :

//	diaRetrieveResponse(dia,"STATUS_NOT_FOUND",NULL,0,"",NULL,tid,peer);
	printf	("'%s' num=%d does not exist; try link-format\n",targetID,num);


	{
	void	*dir;
	char	*f;
	char	entry[256];

	link_format[0]	= '\0';

	dir	= rtl_openDir(".");
	if	(!dir)
		return	0;
	while	( (f = rtl_readDir(dir)) )
	{
		if	(strcmp(f,".") == 0 || strcmp(f,"..") == 0)
			continue;
		if	(strcmp(f,"create.xml") == 0)
			continue;
		if	(strncmp(f,"create-",7) == 0)
		{
			f	+= 7;
			num	= atoi(f);
			sprintf	(tmp,"%d",num);
			f	= tmp;
		}
//		sprintf	(entry,"<%s/%s>;in=\"default\";rt=\"m2m\" ",targetID,f);
		sprintf	(entry,"<%s/%s> ",targetID,f);
		memcpy	(entry,"</M2M",5);	// TODO
		strcat	(link_format,entry);
	}
	if	(cnt || cis)
	{
		sprintf	(entry,"<%s/%s> ",targetID,"*");
		memcpy	(entry,"</M2M",5);	// TODO
		strcat	(link_format,entry);
	}
	rtl_closeDir(dir);
	buf	= link_format;
	sz	= strlen(buf);
	diaRetrieveResponse(dia,"STATUS_OK",buf,sz,"application/link-format",NULL,tid,peer);
	printf	("link_format=(%s)\n",link_format);
	}


	chdir (root_str);
	return	0;
	return	0;
}

int cb_diaRetrieveRequest(dia_context_t *dia, char *reqEntity, char *targetID,
	Opt *optAttr, Opt *optHeader, int tid,char *peer) {
	char tmp[200];
	int cis;
	char *sl;
	int num;
	int sz;
	char *buf;

	if	(!piggy)
		dia->piggy = FALSE;

	targetID	= DoTarget(targetID,&cis);
	if	(!targetID || !*targetID)
	{
	diaRetrieveResponse(dia,"STATUS_NOT_ACCEPTABLE",NULL,0,"",NULL,tid,peer);
	return	0;
	}

	if	(strcmp(targetID,"/.well-known/core") == 0
	||	strncmp(targetID,"/M2M",4) == 0)	// TODO
	{
		return	cb_diaRetrieveRequestCopper( dia,reqEntity,targetID,
				optAttr,optHeader,tid,peer);
	}

	rtl_trace (0,"Retrieve %s\n",targetID);

	num	= -1;
	if	(strstr(targetID,"contentInstances"))
	{
		sl	= strrchr(targetID,'/');
		if	(sl && *(sl+1) >= '0' && *(sl+1) <= '9')
		{
			*sl	= '\0';
			sl++;
			num	= atoi(sl);
		}
	}

	sprintf (tmp,"%s/%s",root_str,targetID);
//	rtl_mkdirp (tmp);
	if	(chdir (tmp) < 0) 
	{
		rtl_trace (0,"cannot chdir '%s'\n",tmp);
		diaRetrieveResponse(dia,"STATUS_NOT_FOUND",NULL,0,"",NULL,tid,peer);
		return	-1;
	}

	if	(num < 0)
		sprintf	(tmp,"create.xml");
	else
		sprintf	(tmp,"create-%d.xml",num);
	buf = getFile (tmp,&sz);
	if (sz > 0)
	{
	printf	("'%s' num=%d retrieved sz=%d\n",targetID,num,sz);
	diaRetrieveResponse(dia,"STATUS_OK",buf,sz,"application/xml",NULL,tid,peer);
	return	0;
	}

	printf	("'%s' num=%d does not exist\n",targetID,num);
	diaRetrieveResponse(dia,"STATUS_NOT_FOUND",NULL,0,"",NULL,tid,peer);

	chdir (root_str);
	return	0;
}

void cb_diaRetrieveResponse(dia_context_t *dia, char *scode, void *ctt, int len, char *cttType, Opt *optHeader, int tid) {
}

int cb_diaCreateRequest(dia_context_t *dia, char *reqEntity, char *targetID, void *ctt, int len, char *cttType,
	Opt *optAttr, Opt *optHeader, int tid,char *peer) {
	char tmp[200];
	int cis;

	if	(!piggy)
		dia->piggy = FALSE;

	targetID	= DoTarget(targetID,&cis);
	if	(!targetID || !*targetID)
	{
	diaCreateResponse(dia, "STATUS_NOT_ACCEPTABLE", targetID, "", 0, "application/xml", NULL, tid,peer);
	return	0;
	}

	rtl_trace (5,"Create %s\n",targetID);
	sprintf(tmp,"%s/%s",root_str,targetID);
	rtl_mkdirp (tmp);
	if	(chdir (tmp) < 0)
	{
		rtl_trace(1,"cannot chdir() '%s'\n",tmp);
		return	-1;
	}

	if	(cis)
	{
		int	i;
		int	max	= 9;

		unlink	("../create-9.xml");
		for	(i = max-1 ; i >= 1; i--)
		{
			char	src[128];
			char	dst[128];

			sprintf	(src,"../create-%d.xml",i);
			sprintf	(dst,"../create-%d.xml",i+1);
			rename	(src,dst);
		}
		rename	("create.xml","../create-1.xml");
	}

	int  fd = open("create.xml",O_RDWR|O_CREAT|O_TRUNC,0600);
	write (fd, ctt, len);
	close (fd);

	if	(cis)
	{
		unlink	("../create-0.xml");
		link	("create.xml","../create-0.xml");
	}

	printf	("'%s' created sz=%d\n",tmp+strlen(root_str),len);

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
	char tmp[200];
	int cis;

	targetID	= DoTarget(targetID,&cis);
	if	(!targetID || !*targetID)
	{
	diaUpdateResponse(dia, "STATUS_NOT_ACCEPTABLE", "", 0, "application/xml", NULL, tid,peer);
	return	0;
	}

	rtl_trace (5, "Update %s\n", targetID);
	chdir (root_str);
	if	(unlink (targetID) < 0)
		perror ("unlink");

	rtl_mkdirp (targetID);
	if	(chdir (targetID) < 0)
	{
		rtl_trace(1,"cannot chdir() '%s'\n",targetID);
		return	-1;
	}

	printf	("'%s' updated sz=%d\n",tmp+strlen(root_str),len);

	int  fd = open("create.xml",O_RDWR|O_CREAT|O_TRUNC,0600);
	write (fd,ctt,len);
	close (fd);

	int	sz;
	char *buf = getFile ("create.xml", &sz);
	diaUpdateResponse(dia, "STATUS_OK", buf, sz, "application/xml", NULL, tid,peer);

	chdir (root_str);
	return	0;
}

void cb_diaUpdateResponse(dia_context_t *dia, char *scode, void *ctt, int len, char* cttType, Opt *optHeader, int tid) {
}

int cb_diaDeleteRequest(dia_context_t *dia, char *reqEntity, char *targetID,
	Opt *optAttr, Opt *optHeader, int tid,char *peer) {
	int cis;

	targetID	= DoTarget(targetID,&cis);
	if	(!targetID || !*targetID)
	{
	diaDeleteResponse(dia, "STATUS_NOT_ACCEPTABLE", NULL, tid,peer);
	return	0;
	}

	if	(strncmp(targetID,"/M2M",4) == 0)	// TODO
	{
		rtl_trace (0, "Copper Delete %s\n", targetID);
		diaDeleteResponse(dia, "STATUS_DELETED", NULL, tid,peer);
		return	0;
	}

	rtl_trace (5, "Delete %s\n", targetID);
	chdir (root_str);
	if	(chdir (targetID) < 0)
	{
		rtl_trace(1,"cannot chdir() '%s'\n",targetID);
		return -1;
	}
	unlink ("create.xml");
	chdir (root_str);
	if	(rmdir (targetID) < 0)
		perror ("rmdir");
	diaDeleteResponse(dia, "STATUS_DELETED", NULL, tid,peer);
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
