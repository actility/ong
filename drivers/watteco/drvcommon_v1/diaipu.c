
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

/*! @file diaipu.c
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
extern	unsigned int DiaRequestMode;

void DIA_LEAK WDiaUCBRequestOk_DiaIpuOk(t_dia_req *preq, t_cmn_sensor *cmn) {}
void DIA_LEAK WDiaUCBRequestOk_DiaIpuDelOk(t_dia_req *preq, t_cmn_sensor *cmn) {}

int DIA_LEAK WDiaUCBPreLoadTemplate_DiaIpuInstCreate(t_dia_req *preq, char *target, t_cmn_sensor *cmn, void **xo,char *templ)
{
    return 0;
}
int DIA_LEAK WDiaUCBPreLoadTemplate_DiaIpuContCreate(t_dia_req *preq, char *target, t_cmn_sensor *cmn, void **xo,char *templ)
{
    return 0;
}
int DIA_LEAK WDiaUCBPreLoadTemplate_DiaIpuElemCreate(t_dia_req *preq, char *target, t_cmn_sensor *cmn, void **xo,char *templ)
{
    return 0;
}
int DIA_LEAK WDiaUCBPostLoadTemplate_DiaIpuInstCreate(t_dia_req *preq, char *target, t_cmn_sensor *cmn, void *xo)
{
    return 0;
}
int DIA_LEAK WDiaUCBPostLoadTemplate_DiaIpuContCreate(t_dia_req *preq, char *target, t_cmn_sensor *cmn, void *xo)
{
    return 0;
}
int DIA_LEAK WDiaUCBPostLoadTemplate_DiaIpuElemCreate(t_dia_req *preq, char *target, t_cmn_sensor *cmn, void *xo)
{
    return 0;
}

static	void DiaIpuOk(t_dia_req *preq, t_dia_rspparam *par);
static	void DiaIpuError(t_dia_req *preq, t_dia_rspparam *par);
	void DiaIpuInstCreate(t_dia_req *preq, t_dia_rspparam *par, ...);
static	void DiaIpuContCreate(t_dia_req *preq, t_dia_rspparam *par, ...);
static	void DiaIpuElemCreate(t_dia_req *preq, t_dia_rspparam *par, ...);
static	void DiaIpuContRetrieve(t_dia_req *preq, t_dia_rspparam *par, ...);
static	void DiaIpuElemRetrieve(t_dia_req *preq, t_dia_rspparam *par, ...);


static	void	DiaIpuOk(t_dia_req *preq,t_dia_rspparam *par)
{
	DIA_DEVICE_CONTEXT();
	if	(!cmn || !xxxid)
		xxxid	= GetIpuId();

	RTL_TRDBG(2,"OK DIA IPU '%s'\n",xxxid);

	if	(cmn)
		cmn->cmn_dia_ipuok	= 1;

	WDiaUCBRequestOk(DIA_FUNC,preq,cmn);
	WDiaUCBRequestOk_DiaIpuOk(preq,cmn);
}

static	void	DiaIpuDelOk(t_dia_req *preq,t_dia_rspparam *par)
{
	DIA_DEVICE_CONTEXT();
	if	(!cmn || !xxxid)
		xxxid	= GetIpuId();

	RTL_TRDBG(2,"OK DIA IPU delete '%s'\n",xxxid);

	if	(cmn)
		cmn->cmn_dia_ipuok	= 0;

	WDiaUCBRequestOk(DIA_FUNC,preq,cmn);
	WDiaUCBRequestOk_DiaIpuDelOk(preq,cmn);
}

static	void	DiaIpuError(t_dia_req *preq,t_dia_rspparam *par)
{
	DIA_DEVICE_CONTEXT();
	if	(!cmn || !xxxid)
		xxxid	= GetIpuId();

	RTL_TRDBG(0,"ERROR DIA IPU FAILURE '%s' req='%s' tid=%d code='%s'\n",
		xxxid,
		preq->rq_name,preq->rq_tid,
		preq->rq_scode);

	if	(cmn)
		cmn->cmn_dia_ipuok	= 0;

	if	(par && par->pr_timeout)	// TIMEOUT => RETRY YES
	{
		WDiaUCBRequestTimeout(DIA_FUNC,preq,cmn);
		return;
	}
	RTL_TRDBG(0,"ERROR DIA IPU DEFINTIVE FAILURE\n");

	WDiaUCBRequestError(DIA_FUNC,preq,cmn);
}

	void	DiaIpuInstCreate(t_dia_req *preq,t_dia_rspparam *par,...)
{
	DIA_LOCAL_VAR();
	DIA_DEVICE_CONTEXT();
	if	(!cmn || !xxxid)
		xxxid	= GetIpuId();
	parseflags	= XOML_PARSE_OBIX;

if	(PseudoOng)
	sprintf(target,"%s%s/%s/containers/DESCRIPTOR/contentInstances/latest",
		GetHostTarget(),SCL_ROOT_APP,xxxid);
else
	sprintf(target,"%s%s/%s/containers/DESCRIPTOR/contentInstances",
		GetHostTarget(),SCL_ROOT_APP,xxxid);

	if	(par == NULL)
	{
	// REQUEST
retry	:
	if	(preq->rq_waitRsp == NUSE_REQUEST_DIA)
	{ // xo load + UCB are called only for new requests
		xo	= NULL;
		strcpy	(tmp,"ipu_inst.xml");
		WDiaUCBPreLoadTemplate(reqname,preq,target,cmn,&xo,tmp);
		WDiaUCBPreLoadTemplate_DiaIpuInstCreate(preq,target,cmn,&xo,tmp);
		if	(!xo)
		{
			xo	= WXoNewTemplate(tmp,parseflags);
			if	(!xo)
				return;
		}
		WDiaUCBPostLoadTemplate(reqname,preq,target,cmn,xo);
		WDiaUCBPostLoadTemplate_DiaIpuInstCreate(preq,target,cmn,xo);
	}

	DIA_CREATE_REQUEST(DiaIpuInstCreate);
return;
	}
	
	// RESPONSE
	DIA_CREATE_RESPONSE(DiaIpuError,DiaIpuError,DiaIpuOk);
}

static	void	DiaIpuContCreate(t_dia_req *preq,t_dia_rspparam *par,...)
{
	DIA_LOCAL_VAR();
	DIA_DEVICE_CONTEXT();
	if	(!cmn || !xxxid)
		xxxid	= GetIpuId();


if	(PseudoOng || updatecont)
	sprintf(target,"%s%s/%s/containers/DESCRIPTOR",
		GetHostTarget(),SCL_ROOT_APP,xxxid);
else
	sprintf(target,"%s%s/%s/containers",
		GetHostTarget(),SCL_ROOT_APP,xxxid);

	if	(par == NULL)
	{
	// REQUEST
retry	:
	if	(preq->rq_waitRsp == NUSE_REQUEST_DIA)
	{ // xo load + UCB are called only for new requests
		xo	= NULL;
		strcpy	(tmp,"ipu_cont.xml");
		WDiaUCBPreLoadTemplate(reqname,preq,target,cmn,&xo,tmp);
		WDiaUCBPreLoadTemplate_DiaIpuContCreate(preq,target,cmn,&xo,tmp);
		if	(!xo)
		{
			xo	= WXoNewTemplate(tmp,0);
			if	(!xo)
				return;
		}
		WDiaUCBPostLoadTemplate(reqname,preq,target,cmn,xo);
		WDiaUCBPostLoadTemplate_DiaIpuContCreate(preq,target,cmn,xo);
		if	(updatecont)
			XoNmUnSetAttr(xo,"m2m:id",0,0);
	}

	DIA_CREATE_REQUEST(DiaIpuContCreate);
return;
	}

	// RESPONSE
	preq->rq_flags	&= ~RQT_UPDATE_CONT;
	DIA_CREATE_RESPONSE(DiaIpuError,DiaIpuError,DiaIpuInstCreate);
}

static	void	DiaIpuElemCreate(t_dia_req *preq,t_dia_rspparam *par,...)
{
	DIA_LOCAL_VAR();
	DIA_DEVICE_CONTEXT();
	if	(!cmn || !xxxid)
		xxxid	= GetIpuId();

if	(PseudoOng || updateelem)
	sprintf(target,"%s%s/%s",
		GetHostTarget(),SCL_ROOT_APP,xxxid);
else
	sprintf(target,"%s%s",
		GetHostTarget(),SCL_ROOT_APP);

	if	(par == NULL)
	{
	// REQUEST
retry	:
	if	(preq->rq_waitRsp == NUSE_REQUEST_DIA)
	{ // UCB are called only if request is initialized
		xo	= NULL;
		strcpy	(tmp,"ipu_elem.xml");
		WDiaUCBPreLoadTemplate(reqname,preq,target,cmn,&xo,tmp);
		WDiaUCBPreLoadTemplate_DiaIpuElemCreate(preq,target,cmn,&xo,tmp);
		if	(!xo)
		{
			xo	= WXoNewTemplate(tmp,parseflags);
			if	(!xo)
				return;
		}
		WDiaUCBPostLoadTemplate(reqname,preq,target,cmn,xo);
		WDiaUCBPostLoadTemplate_DiaIpuElemCreate(preq,target,cmn,xo);
		if	(updateelem)
			XoNmUnSetAttr(xo,"appId",0,0);
	}

	DIA_CREATE_REQUEST(DiaIpuElemCreate);
return;
	}
	
	// RESPONSE
	preq->rq_flags	&= ~RQT_UPDATE_ELEM;
	DIA_CREATE_RESPONSE(DiaIpuError,DiaIpuError,DiaIpuContRetrieve);
}

static	void	DiaIpuContRetrieve(t_dia_req *preq,t_dia_rspparam *par,...)
{
	DIA_LOCAL_VAR();
	DIA_DEVICE_CONTEXT();
	if	(!cmn || !xxxid)
		xxxid	= GetIpuId();

if	(PseudoOng)
	sprintf(target,"%s%s/%s/containers/DESCRIPTOR",
		GetHostTarget(),SCL_ROOT_APP,xxxid);
else
	sprintf(target,"%s%s/%s/containers/DESCRIPTOR",
		GetHostTarget(),SCL_ROOT_APP,xxxid);

	if	(par == NULL)
	{
	// REQUEST
retry	:
	DIA_RETRIEVE_REQUEST(DiaIpuContRetrieve);
return;
	}
	
	// RESPONSE
	if	(updatecont)
	{
	DIA_RETRIEVE_RESPONSE(DiaIpuError,DiaIpuContCreate,DiaIpuContCreate);
	}
	else
	{
	DIA_RETRIEVE_RESPONSE(DiaIpuError,DiaIpuContCreate,DiaIpuInstCreate);
	}
}

static	void	DiaIpuElemRetrieve(t_dia_req *preq,t_dia_rspparam *par,...)
{
	DIA_LOCAL_VAR();
	DIA_DEVICE_CONTEXT();
	if	(!cmn || !xxxid)
		xxxid	= GetIpuId();

if	(PseudoOng)
	sprintf(target,"%s%s/%s",GetHostTarget(),SCL_ROOT_APP,xxxid);
else
	sprintf(target,"%s%s/%s",GetHostTarget(),SCL_ROOT_APP,xxxid);

	if	(par == NULL)
	{
	// REQUEST
retry	:
	DIA_RETRIEVE_REQUEST(DiaIpuElemRetrieve);
return;
	}
	
	// RESPONSE
	if	(updateelem)
	{
	DIA_RETRIEVE_RESPONSE(DiaIpuError,DiaIpuElemCreate,DiaIpuElemCreate);
	}
	else
	{
	DIA_RETRIEVE_RESPONSE(DiaIpuError,DiaIpuElemCreate,DiaIpuContRetrieve);
	}
}

static	void	DiaIpuElemDelete(t_dia_req *preq,t_dia_rspparam *par,...)
{
	DIA_LOCAL_VAR();
	DIA_DEVICE_CONTEXT();
	if	(!cmn || !xxxid)
		xxxid	= GetIpuId();

if	(PseudoOng)
	sprintf(target,"%s%s/%s",GetHostTarget(),SCL_ROOT_APP,xxxid);
else
	sprintf(target,"%s%s/%s",GetHostTarget(),SCL_ROOT_APP,xxxid);

	if	(par == NULL)
	{
	// REQUEST
retry	:
	DIA_DELETE_REQUEST(DiaIpuElemDelete);
return;
	}
	
	// RESPONSE
	DIA_DELETE_RESPONSE(DiaIpuError,DiaIpuError,DiaIpuDelOk);
}



/*!
 *
 * @brief start DIA macro request for "interworking unit" M2M application
 * @return void
 * 
 */
void	DiaIpuStart()
{
	t_dia_req	req;

	memset	(&req,0,sizeof(t_dia_req));
	req.rq_cli	= NULL;
	req.rq_name	= (char *)__func__;
	req.rq_flags	= DiaRequestMode; DiaRequestMode = 0;
	DiaIpuElemRetrieve(&req,NULL);
}

void	DiaIpuDelete()
{
	t_dia_req	req;

	memset	(&req,0,sizeof(t_dia_req));
	req.rq_cli	= NULL;
	req.rq_name	= (char *)__func__;
	req.rq_flags	= DiaRequestMode; DiaRequestMode = 0;
	DiaIpuElemDelete(&req,NULL);
}

void	DiaIpuStartWithSerial(int dev,int serial,t_cmn_sensor *(*fct)(int dev,int serial))
{
	t_dia_req	req;

	memset	(&req,0,sizeof(t_dia_req));
	req.rq_cli	= NULL;
	req.rq_name	= (char *)__func__;
	req.rq_dev	= dev;
	req.rq_serial	= serial;
	req.rq_cbserial	= fct;
	req.rq_flags	= DiaRequestMode; DiaRequestMode = 0;
	DiaIpuElemRetrieve(&req,NULL);
}

void	DiaIpuDeleteWithSerial(int dev,int serial,t_cmn_sensor *(*fct)(int dev,int serial))
{
	t_dia_req	req;

	memset	(&req,0,sizeof(t_dia_req));
	req.rq_cli	= NULL;
	req.rq_name	= (char *)__func__;
	req.rq_dev	= dev;
	req.rq_serial	= serial;
	req.rq_cbserial	= fct;
	req.rq_flags	= DiaRequestMode; DiaRequestMode = 0;
	DiaIpuElemDelete(&req,NULL);
}


void	CmdDiaIpu(t_cli *cli,char *buf)
{
	t_dia_req	req;

	memset	(&req,0,sizeof(t_dia_req));
	req.rq_cli	= cli;
//	req.rq_cli	= NULL;
	req.rq_flags	= DiaRequestMode; DiaRequestMode = 0;
	DiaIpuElemRetrieve(&req,NULL);
}
