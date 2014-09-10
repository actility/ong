
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

/*! @file dianet.c
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

#include "rtlbase.h"
#include "rtlimsg.h"
#include "xoapipr.h"
#include "dIa.h"


#include "cmndefine.h"
#include "cmnstruct.h"
#include "speccproto.h"
#include "cmnproto.h"
#include "specextern.h"

#include "diadef.h"

extern	void	*MainTbPoll;
extern	int	TraceLevel;

extern	dia_context_t	*DiaCtxt;
extern	unsigned int DiaDefaultRequestMode;
extern	unsigned int DiaRequestMode;

void DIA_LEAK WDiaUCBRequestOk_DiaNetOk(t_dia_req *preq, t_cmn_sensor *cmn) {}
void DIA_LEAK WDiaUCBRequestOk_DiaNetDelOk(t_dia_req *preq, t_cmn_sensor *cmn) {}

int DIA_LEAK WDiaUCBPreLoadTemplate_DiaNetInstCreate(t_dia_req *preq, char *target, t_cmn_sensor *cmn, void **xo,char *templ)
{
    return 0;
}
int DIA_LEAK WDiaUCBPreLoadTemplate_DiaNetContCreate(t_dia_req *preq, char *target, t_cmn_sensor *cmn, void **xo,char *templ)
{
    return 0;
}
int DIA_LEAK WDiaUCBPreLoadTemplate_DiaNetElemCreate(t_dia_req *preq, char *target, t_cmn_sensor *cmn, void **xo,char *templ)
{
    return 0;
}
int DIA_LEAK WDiaUCBPostLoadTemplate_DiaNetInstCreate(t_dia_req *preq, char *target, t_cmn_sensor *cmn, void *xo)
{
    return 0;
}
int DIA_LEAK WDiaUCBPostLoadTemplate_DiaNetContCreate(t_dia_req *preq, char *target, t_cmn_sensor *cmn, void *xo)
{
    return 0;
}
int DIA_LEAK WDiaUCBPostLoadTemplate_DiaNetElemCreate(t_dia_req *preq, char *target, t_cmn_sensor *cmn, void *xo)
{
    return 0;
}


static	void DiaNetOk(t_dia_req *preq, t_dia_rspparam *par);
static	void DiaNetError(t_dia_req *preq, t_dia_rspparam *par);
static	void DiaNetInstCreate(t_dia_req *preq, t_dia_rspparam *par, ...);
static	void DiaNetContCreate(t_dia_req *preq, t_dia_rspparam *par, ...);
static	void DiaNetElemCreate(t_dia_req *preq, t_dia_rspparam *par, ...);
static	void DiaNetContRetrieve(t_dia_req *preq, t_dia_rspparam *par, ...);
	void DiaNetElemRetrieve(t_dia_req *preq, t_dia_rspparam *par, ...);

static	void	DiaNetOk(t_dia_req *preq,t_dia_rspparam *par)
{
	DIA_DEVICE_CONTEXT();
	if	(!cmn || !xxxid)
		xxxid	= GetNetId();

	RTL_TRDBG(2,"OK DIA NET '%s'\n",xxxid);

	if	(cmn)
		cmn->cmn_dia_netok	= 1;

	WDiaUCBRequestOk(DIA_FUNC,preq,cmn);
	WDiaUCBRequestOk_DiaNetOk(preq,cmn);
}

static	void	DiaNetDelOk(t_dia_req *preq,t_dia_rspparam *par)
{
	DIA_DEVICE_CONTEXT();
	if	(!cmn || !xxxid)
		xxxid	= GetNetId();

	RTL_TRDBG(2,"OK DIA NET delete '%s'\n",xxxid);

	if	(cmn)
		cmn->cmn_dia_netok	= 0;

	WDiaUCBRequestOk(DIA_FUNC,preq,cmn);
	WDiaUCBRequestOk_DiaNetDelOk(preq,cmn);
}

static	void	DiaNetError(t_dia_req *preq,t_dia_rspparam *par)
{
	DIA_DEVICE_CONTEXT();
	if	(!cmn || !xxxid)
		xxxid	= GetNetId();

	RTL_TRDBG(0,"ERROR DIA NET FAILURE '%s' req='%s' tid=%d code='%s'\n",
		xxxid,preq->rq_name,preq->rq_tid,preq->rq_scode);

	if	(cmn)
		cmn->cmn_dia_netok	= 0;

	if	(par && par->pr_timeout)	// TIMEOUT => RETRY YES
	{
//		DiaNetStart();
		WDiaUCBRequestTimeout(DIA_FUNC,preq,cmn);
		return;
	}
	RTL_TRDBG(0,"ERROR DIA NET DEFINTIVE FAILURE\n");

	WDiaUCBRequestError(DIA_FUNC,preq,cmn);
}

static	void	DiaNetInstCreate(t_dia_req *preq,t_dia_rspparam *par,...)
{
	DIA_LOCAL_VAR();
	DIA_DEVICE_CONTEXT();
	if	(!cmn || !xxxid)
		xxxid	= GetNetId();

	parseflags	= XOML_PARSE_OBIX;

if	(PseudoOng)
	sprintf(target,"%s%s/NW_%s/containers/DESCRIPTOR/contentInstances/latest",
	GetHostTarget(),SCL_ROOT_APP,xxxid);
else
	sprintf(target,"%s%s/NW_%s/containers/DESCRIPTOR/contentInstances",
	GetHostTarget(),SCL_ROOT_APP,xxxid);

	if	(par == NULL)
	{
	// REQUEST
retry	:
	if	(preq->rq_waitRsp == NUSE_REQUEST_DIA)
	{ // xo load + UCB are called only for new requests
		xo	= NULL;
		strcpy	(tmp,"net_inst.xml");
		WDiaUCBPreLoadTemplate(reqname,preq,target,cmn,&xo,tmp);
		WDiaUCBPreLoadTemplate_DiaNetInstCreate(preq,target,cmn,&xo,tmp);
		if	(!xo)
		{
			xo	= WXoNewTemplate(tmp,parseflags);
			if	(!xo)
				return;
		}
		WDiaUCBPostLoadTemplate(reqname,preq,target,cmn,xo);
		WDiaUCBPostLoadTemplate_DiaNetInstCreate(preq,target,cmn,xo);
	}

	DIA_CREATE_REQUEST(DiaNetInstCreate);
return;
	}
	
	// RESPONSE
	DIA_CREATE_RESPONSE(DiaNetError,DiaNetError,DiaNetOk);
}

static	void	DiaNetContCreate(t_dia_req *preq,t_dia_rspparam *par,...)
{
	DIA_LOCAL_VAR();
	DIA_DEVICE_CONTEXT();
	if	(!cmn || !xxxid)
		xxxid	= GetNetId();

if	(PseudoOng || updatecont)
	sprintf(target,"%s%s/NW_%s/containers/DESCRIPTOR",
	GetHostTarget(),SCL_ROOT_APP,xxxid);
else
	sprintf(target,"%s%s/NW_%s/containers",
	GetHostTarget(),SCL_ROOT_APP,xxxid);

	if	(par == NULL)
	{
	// REQUEST
retry	:
	if	(preq->rq_waitRsp == NUSE_REQUEST_DIA)
	{ // xo load + UCB are called only for new requests
		xo	= NULL;
		strcpy	(tmp,"net_cont.xml");
		WDiaUCBPreLoadTemplate(reqname,preq,target,cmn,&xo,tmp);
		WDiaUCBPreLoadTemplate_DiaNetContCreate(preq,target,cmn,&xo,tmp);
		if	(!xo)
		{
			xo	= WXoNewTemplate(tmp,0);
			if	(!xo)
				return;
		}
		WDiaUCBPostLoadTemplate(reqname,preq,target,cmn,xo);
		WDiaUCBPostLoadTemplate_DiaNetContCreate(preq,target,cmn,xo);
		if	(updatecont)
			XoNmUnSetAttr(xo,"m2m:id",0,0);
	}

	DIA_CREATE_REQUEST(DiaNetContCreate);
return;
	}

	// RESPONSE
	preq->rq_flags	&= ~RQT_UPDATE_CONT;
	DIA_CREATE_RESPONSE(DiaNetError,DiaNetError,DiaNetInstCreate);
}

static	void	DiaNetElemCreate(t_dia_req *preq,t_dia_rspparam *par,...)
{
	DIA_LOCAL_VAR();
	DIA_DEVICE_CONTEXT();
	if	(!cmn || !xxxid)
		xxxid	= GetNetId();

if	(PseudoOng || updateelem)
	sprintf(target,"%s%s/NW_%s",GetHostTarget(),SCL_ROOT_APP,xxxid);
else
	sprintf(target,"%s%s",GetHostTarget(),SCL_ROOT_APP);

	if	(par == NULL)
	{
	// REQUEST
retry	:
	if	(preq->rq_waitRsp == NUSE_REQUEST_DIA)
	{ // xo load + UCB are called only for new requests
		xo	= NULL;
		strcpy	(tmp,"net_elem.xml");
		WDiaUCBPreLoadTemplate(reqname,preq,target,cmn,&xo,tmp);
		WDiaUCBPreLoadTemplate_DiaNetElemCreate(preq,target,cmn,&xo,tmp);
		if	(!xo)
		{
			xo	= WXoNewTemplate(tmp,parseflags);
			if	(!xo)
				return;
		}
		WDiaUCBPostLoadTemplate(reqname,preq,target,cmn,xo);
		WDiaUCBPostLoadTemplate_DiaNetElemCreate(preq,target,cmn,xo);
		if	(updateelem)
			XoNmUnSetAttr(xo,"appId",0,0);
	}

	DIA_CREATE_REQUEST(DiaNetElemCreate);
return;
	}
	
	// RESPONSE
	preq->rq_flags	&= ~RQT_UPDATE_ELEM;
	DIA_CREATE_RESPONSE(DiaNetError,DiaNetError,DiaNetContRetrieve);
}

static	void	DiaNetContRetrieve(t_dia_req *preq,t_dia_rspparam *par,...)
{
	DIA_LOCAL_VAR();
	DIA_DEVICE_CONTEXT();
	if	(!cmn || !xxxid)
		xxxid	= GetNetId();

if	(PseudoOng)
	sprintf(target,"%s%s/NW_%s/containers/DESCRIPTOR",
	GetHostTarget(),SCL_ROOT_APP,xxxid);
else
	sprintf(target,"%s%s/NW_%s/containers/DESCRIPTOR",
	GetHostTarget(),SCL_ROOT_APP,xxxid);

	if	(par == NULL)
	{
	// REQUEST
retry	:
	DIA_RETRIEVE_REQUEST(DiaNetContRetrieve);
return;
	}
	
	// RESPONSE
	if	(updatecont)
	{
	DIA_RETRIEVE_RESPONSE(DiaNetError,DiaNetContCreate,DiaNetContCreate);
	}
	else
	{
	DIA_RETRIEVE_RESPONSE(DiaNetError,DiaNetContCreate,DiaNetInstCreate);
	}
}

void	DiaNetElemRetrieve(t_dia_req *preq,t_dia_rspparam *par,...)
{
	DIA_LOCAL_VAR();
	DIA_DEVICE_CONTEXT();
	if	(!cmn || !xxxid)
		xxxid	= GetNetId();

if	(PseudoOng)
	sprintf(target,"%s%s/NW_%s",GetHostTarget(),SCL_ROOT_APP,xxxid);
else
	sprintf(target,"%s%s/NW_%s",GetHostTarget(),SCL_ROOT_APP,xxxid);

	if	(par == NULL)
	{
	// REQUEST
retry	:
	DIA_RETRIEVE_REQUEST(DiaNetElemRetrieve);
return;
	}
	
	// RESPONSE
	if	(updateelem)
	{
	DIA_RETRIEVE_RESPONSE(DiaNetError,DiaNetElemCreate,DiaNetElemCreate);
	}
	else
	{
	DIA_RETRIEVE_RESPONSE(DiaNetError,DiaNetElemCreate,DiaNetContRetrieve);
	}
}

void	DiaNetElemDelete(t_dia_req *preq,t_dia_rspparam *par,...)
{
	DIA_LOCAL_VAR();
	DIA_DEVICE_CONTEXT();
	if	(!cmn || !xxxid)
		xxxid	= GetNetId();

if	(PseudoOng)
	sprintf(target,"%s%s/NW_%s",GetHostTarget(),SCL_ROOT_APP,xxxid);
else
	sprintf(target,"%s%s/NW_%s",GetHostTarget(),SCL_ROOT_APP,xxxid);

	if	(par == NULL)
	{
	// REQUEST
retry	:
	DIA_DELETE_REQUEST(DiaNetElemDelete);
return;
	}
	
	// RESPONSE
	DIA_DELETE_RESPONSE(DiaNetError,DiaNetError,DiaNetDelOk);
}



/*!
 *
 * @brief start DIA macro request for "network" M2M application
 * @return void
 * 
 */
void	DiaNetStart()
{
	t_dia_req	req;

	memset	(&req,0,sizeof(t_dia_req));
	req.rq_cli	= NULL;
	req.rq_name	= (char *)__func__;
	req.rq_flags	= DiaRequestMode; DiaRequestMode = DiaDefaultRequestMode;
	DiaNetElemRetrieve(&req,NULL);
}

void	DiaNetDelete()
{
	t_dia_req	req;

	memset	(&req,0,sizeof(t_dia_req));
	req.rq_cli	= NULL;
	req.rq_name	= (char *)__func__;
	req.rq_flags	= DiaRequestMode; DiaRequestMode = DiaDefaultRequestMode;
	DiaNetElemDelete(&req,NULL);
}

void	DiaNetStartWithSerial(int dev,int serial,t_cmn_sensor *(*fct)(int dev,int serial))
{
	t_dia_req	req;

	memset	(&req,0,sizeof(t_dia_req));
	req.rq_cli	= NULL;
	req.rq_name	= (char *)__func__;
	req.rq_dev	= dev;
	req.rq_serial	= serial;
	req.rq_cbserial	= fct;
	req.rq_flags	= DiaRequestMode; DiaRequestMode = DiaDefaultRequestMode;
	DiaNetElemRetrieve(&req,NULL);
}

void	DiaNetDeleteWithSerial(int dev,int serial,t_cmn_sensor *(*fct)(int dev,int serial))
{
	t_dia_req	req;

	memset	(&req,0,sizeof(t_dia_req));
	req.rq_cli	= NULL;
	req.rq_name	= (char *)__func__;
	req.rq_dev	= dev;
	req.rq_serial	= serial;
	req.rq_cbserial	= fct;
	req.rq_flags	= DiaRequestMode; DiaRequestMode = DiaDefaultRequestMode;
	DiaNetElemDelete(&req,NULL);
}


void	CmdDiaNet(t_cli *cli,char *buf)
{
	t_dia_req	req;

	memset	(&req,0,sizeof(t_dia_req));
	req.rq_cli	= cli;
//	req.rq_cli	= NULL;
	req.rq_flags	= DiaRequestMode; DiaRequestMode = DiaDefaultRequestMode;
	DiaNetElemRetrieve(&req,NULL);
}
