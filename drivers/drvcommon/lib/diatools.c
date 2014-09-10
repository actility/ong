
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

/*! @file diatools.c
 *
 */
#include <stdio.h>			/************************************/
#include <stdlib.h>			/*     run in main thread           */
#include <string.h>			/************************************/
#include <unistd.h>
#include <pthread.h>
#include <errno.h>
#include <limits.h>
#include <signal.h>
#include <poll.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#include "rtlbase.h"
#include "rtlimsg.h"
#include "xoapipr.h"
#include "dIa.h"

#include "cmndefine.h"
#include "cmnstruct.h"

#include "speccproto.h"
#include "cmnproto.h"
#include "specextern.h"

dia_context_t	*DiaCtxt;

static	int _WDiaUserContext(t_dia_req *preq,t_cmn_sensor **cmn,char **xxxid)
{
	*cmn	= NULL;
	*xxxid	= NULL;

	if	(preq->rq_serial && preq->rq_userctxt == NULL)
	{
		if	(!preq->rq_cbserial)
		{
			*cmn = CmnSensorNumSerial(preq->rq_dev,preq->rq_serial);
			if	(!*cmn)
				return	-1;
			*xxxid	= (*cmn)->cmn_ieee;
			return	1;
		}
		*cmn = preq->rq_cbserial(preq->rq_dev,preq->rq_serial);
		if	(!*cmn)
			return	-2;
		*xxxid	= (*cmn)->cmn_ieee;
		return	2;
	}
	if	(preq->rq_cbuserctxt)
	{
		*cmn	= preq->rq_cbuserctxt(preq);
		if	(!*cmn)
			return	-3;
		*xxxid	= (*cmn)->cmn_ieee;
		return	3;
	}

	// this is not an error, see DiaIpuXXXX & DiaNetXXXX watteco & zigbee

	return	0;
}

int	WDiaUserContext(t_dia_req *preq,t_cmn_sensor **cmn,char **xxxid)
{
	int	ret;

	ret	= _WDiaUserContext(preq,cmn,xxxid);
	if	(ret < 0)
	{
		// TODO creer un etat ERR serait mieux
		preq->rq_waitRsp	= NUSE_REQUEST_DIA;
		RTL_TRDBG(0,"ERROR CTXT USER ret=%d dia request '%s' tid=%d\n",
			ret,preq->rq_name,preq->rq_tid);
		return	ret;
	}

	RTL_TRDBG(5,"DIA CTXT USER ret=%d dev=%d app=%d xxxid='%s'\n",ret,
			preq->rq_dev,preq->rq_app,*xxxid);

	return	ret;
}


int	WDiaCodeOk(char *code)
{
#if	0
	if	(strcmp(code,"STATUS_CONFLICT") == 0)
		return	1;
#endif
	if	(strcmp(code,"STATUS_OK") == 0)
		return	1;

	if	(strcmp(code,"STATUS_NOT_FOUND") == 0)
		return	0;

	if	(strcmp(code,"STATUS_CREATED") == 0)
		return	1;

	if	(strcmp(code,"STATUS_DELETED") == 0)
		return	1;

	if	(strcmp(code,"OK") == 0)
		return	1;


	return	0;
}


//
//	parse a retargeting URI like :
//	/m2m/applications/APP_020000ffff00240f.1/retargeting2/0x0006.0x02.ope
//	                  ^   ^                ^            ^   ^      ^
//	                  |   |                |            |   |      |
//	                type  ident ieee      app       level cluster num
//
//	- skip /m2m/application (SCL_ROOT_APP) old version
//	- skip ${w_driverpath}
//	- return in type SCL_IPU, SCL_NET, SCL_DEV, SCL_APP

int	WDiaParseMethod(char *target,char *ident,int *app,int *type,
	int *level,int *cluster,int *num)
{
	char	retarget[128];
	char	asclevel[128];
	char	*pt;
	char	*pt2;
	char	*drvpath;

	if	(!target || !*target)
		return	-1;

	*ident	= '\0';
	*type	= 0;
	*app	= 0;
	*level	= -1;
	*cluster= -1;
	*num	= -1;

	drvpath	= GetVar("w_driverpath");

	if	(*target == '/' && strcmp(target+1,drvpath) == 0)
	{
		*type	= METHOD_DRV;
		return	0;
	}

	pt	= strstr(target,drvpath);
	if	(pt)
		target	= pt+strlen(drvpath);

	while	(*target == '/')	target++;
	if	(!target || !*target)
		return	-2;

	if	(strncmp(target,SCL_IPU,strlen(SCL_IPU)) == 0)
	{
		*type	= METHOD_IPU;
		target	= target+strlen(SCL_APP);
	}
	if	(strncmp(target,SCL_NET,strlen(SCL_NET)) == 0)
	{
		*type	= METHOD_NET;
		target	= target+strlen(SCL_NET);
	}
	if	(strncmp(target,SCL_DEV,strlen(SCL_DEV)) == 0)
	{
		*type	= METHOD_DEV;
		target	= target+strlen(SCL_DEV);
	}
	if	(strncmp(target,SCL_APP,strlen(SCL_APP)) == 0)
	{
		*type	= METHOD_APP;
		target	= target+strlen(SCL_APP);
	}

	if	(*type <= 0)
	{
		return	-3;
	}

	while	(*target == '/')	target++;
	if	(!target || !*target)
		return	-4;

	pt	= ident;
	*pt	= '\0';
	while	(*target && *target != '/')
	{
		*pt++	= *target++;
	}
	*pt	= '\0';
	if	(*target != '/' || strlen(ident) == 0)
		return	-5;
	target++;

	pt	= strchr(ident,'.');
	if	(pt)
	{
		*app	= atoi(pt+1);
		*pt	= '\0';
	}
/*
printf	("ident='%s'\n",ident);
printf	("left='%s'\n",target);
*/

	pt	= retarget;
	*pt	= '\0';
	pt2	= asclevel;
	*pt2	= '\0';
	while	(*target && *target != '/')
	{
		if	(*target >= '0' && *target <= '9')
		{
			*pt2++	= *target;
		}
		else
		{
			pt2	= asclevel;
			*pt2	= '\0';
		}
		*pt++	= *target++;
	}
	*pt	= '\0';
	*pt2	= '\0';
	if	(*target != '/' || strlen(retarget) == 0)
		return	-6;
	target++;
	if	(strlen(asclevel))
		*level	= atoi(asclevel);
/*
printf	("ident='%s'\n",ident);
printf	("retar='%s'\n",retarget);
printf	("left='%s'\n",target);
*/

	if	(strlen(target) == 0)
		return	-7;

	if	(*type == METHOD_APP || *type == METHOD_DEV)
	{
/*ML-20121001-retargeting-response+*/
//		if	(sscanf(target,"%x.%x",cluster,num) != 2)
//			return	-8;
    // try to retrieve the ZCL operation
		sscanf(target,"%x.%x",cluster,num);
/*ML-20121001-retargeting-response-*/
	}

/*
printf	("ident='%s'\n",ident);
printf	("retar='%s'\n",retarget);
printf	("cluster=%x\n",*cluster);
printf	("num=%d\n",*num);
*/

	return	0;
}


char	*WDiaAccessRightsTxt(int level)
{
	switch(level)
	{
	case	2 : return	"AR3";
	case	1 : return	"AR2";
	default :   return	"AR";
	}
}


void	WAdmDumpRequest(void *cl,char *reqname,char *reqid,char *target,char *buf,int sz)
{
	AdmPrint((t_cli *)cl,">>>>>>>>>>> '%s'\n",reqname);
	AdmPrint((t_cli *)cl,"requesting='%s'\n",reqid);
	AdmPrint((t_cli *)cl,"targetid='%s'\n",target);
	AdmPrint((t_cli *)cl,"'%.*s'\n",sz,buf);
}

void	WDiaTraceRequest(char *side,int tid,char *peer,char *reqname,char *reqid,char *target,char *buf,int sz,char *type)
{

	fprintf(stdout,">>>> %s >>>>>>>>>>> %u/'%s' '%s'\n",
						side,tid,peer,reqname);
	fprintf(stdout,"requesting='%s'\n",reqid);
	fprintf(stdout,"targetid='%s'\n",target);
	fprintf(stdout,"type='%s' sz=%d\n",type,sz);
	if	(!type || !*type)
	{
		fflush(stdout);
		return;
	}
	if	(!strcmp(type,"application/xml") || !strcmp(type,"text/plain"))
	{
		char	fmt[128];

		sprintf(fmt,"'%%%d.%ds'\n",sz,sz);
		fprintf(stdout,fmt,buf);
	}
	fflush(stdout);
}

void	WDiaTraceResponse(char *side,int tid,char *peer,char *reqname,char *scode,char *target,char *buf,int sz,char *type)
{
	fprintf(stdout,"<<<< %s <<<<<<<<<<< %u/'%s' '%s'\n",
						side,tid,peer,reqname);
	fprintf(stdout,"scode='%s'\n",scode);
	fprintf(stdout,"targetid='%s'\n",target);
	fprintf(stdout,"type='%s' sz=%d\n",type,sz);
	if	(!type || !*type)
	{
		fflush(stdout);
		return;
	}
	if	(!strcmp(type,"application/xml") || !strcmp(type,"text/plain"))
	{
		char	fmt[128];

		sprintf(fmt,"'%%%d.%ds'\n",sz,sz);
		fprintf(stdout,fmt,buf);
	}
	fflush(stdout);
}

/*!
 *
 * @brief clock for dia/udp link, must be called by main loop each second
 * 	- call client side clock
 * 	- call server side clock
 * @param now current time in second (time(2))
 * @return void
 * 
 */
void	DiaClockSc(time_t now)
{
	DiaClientClockSc(now);
	DiaServerClockSc(now);
}

static	dia_callbacks_t callbacksong 
#if	0
= {
	cb_diaRetrieveRequest,
	cb_diaRetrieveResponse,
	cb_diaCreateRequest,
	cb_diaCreateResponse,
	cb_diaUpdateRequest,
	cb_diaUpdateResponse,
	cb_diaDeleteRequest,
	cb_diaDeleteResponse,
	cb_diaErrorResponse
}
#endif
;

/*!
 *
 * @brief Call back function for rtlPoll to get the requested events.
 * This call back function is raised when rtlpoll needs to set the appropriate
 * POLL flags for the provided file descriptor. Here we have an UDP socket, we
 * request for POLLIN only.
 * @param tb the table of all polled file descriptors
 * @param fd the file descriptor on which events need to be detected
 * @param r1 user's data placeholder #1
 * @param r2 user's data placeholder #2
 * @param revents the current requested events
 * @return -1 in case of error detection, 0 otherwise
 * 
 */

int	CB_DiaUdpRequest(void *tb,int fd,void *r1,void *r2,int revents)
{
	return	POLLIN;
}

/*!
 *
 * @brief Call back function for rtlPoll to treat the detected events.
 * This call back function is raised when rtlpoll detects events on the 
 * the provided file descriptor. Here the file descriptor is an UDP socket.
 * If POLLIN is detected we try to read data 
 * @param tb the table of all polled file descriptors
 * @param fd the file descriptor on which events were detected
 * @param r1 user's data placeholder #1
 * @param r2 user's data placeholder #2
 * @param revents the current detected events
 * @return -1 in case of error detection, 0 otherwise
 * 
 */
int	CB_DiaUdpEvent(void *tb,int fd,void *r1,void *r2,int revents)
{
	RTL_TRDBG(6,"DIA udp event\n");
	dia_input(r1);
	return	0;
}

static	void	SetCB()
{
	rtl_pollAdd (MainTbPoll,dia_getFd(DiaCtxt),
			CB_DiaUdpEvent,CB_DiaUdpRequest,DiaCtxt,NULL);
}

/*!
 *
 * @brief initialize dia/udp client & server modules
 * @return void
 * 
 */
void	*DiaUdpInit()
{
	char addr_str[NI_MAXHOST];	// bind coap local
	char port_str[NI_MAXSERV];	// bind coap local

	DiaClientInit(&callbacksong);	// CB from diaclient.c
	DiaServerInit(&callbacksong);	// CB from diaserver.c

	strcpy	(addr_str,GetCoapAddrL());
	strcpy	(port_str,GetCoapPortL());
//	dia_set_log_level(9);
	DiaCtxt = dia_createContext(addr_str, port_str, &callbacksong);
	if (!DiaCtxt)
	{
		RTL_TRDBG(0,"ERROR cannot create dia [%s]:%s\n",
							addr_str,port_str);
		return	NULL;
	}
	// read non bloquant 2 lectures max
//	dia_setNonBlocking (DiaCtxt, 2);


	SetCB();

	return	DiaCtxt;
}

void	DiaUdpClose()
{
	if	(DiaCtxt)
		dia_freeContext(DiaCtxt);
	DiaCtxt	= NULL;
}

void	*WDiaXoLoadResponse(t_dia_req *req,int parseflags)
{
	void	*xo	= NULL;
	int	err;
	char	*type;

	if	(!req)
		return	NULL;
	if	(!req->rq_par)
		return	NULL;

	type	= req->rq_par->pr_cttType;

	if	(!strcmp(type,"application/xml") || !strcmp(type,"text/plain"))
	{
	xo	= XoReadXmlMem(req->rq_par->pr_ctt,req->rq_par->pr_len,
							NULL,parseflags,&err);
	}
	if	(!xo)
	{
	RTL_TRDBG(0,"ERROR cannot parse dia response '%s' tid=%d err=%d\n",
				req->rq_name,req->rq_name,err);
		return	NULL;
	}

	return	xo;
}
