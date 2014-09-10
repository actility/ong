
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

static	void DiaIpuOk(t_dia_req *preq, t_dia_rspparam *par);
static	void DiaIpuError(t_dia_req *preq, t_dia_rspparam *par);
static	void DiaIpuInstCreate(t_dia_req *preq, t_dia_rspparam *par, ...);
static	void DiaIpuContCreate(t_dia_req *preq, t_dia_rspparam *par, ...);
static	void DiaIpuElemCreate(t_dia_req *preq, t_dia_rspparam *par, ...);
static	void DiaIpuContRetrieve(t_dia_req *preq, t_dia_rspparam *par, ...);
static	void DiaIpuElemRetrieve(t_dia_req *preq, t_dia_rspparam *par, ...);

void	DiaTest()
{ 
	int	ret;
	int	tid;

	ret	= diaRetrieveRequest(DiaCtxt,"/driverwatteco",
//			"coap://[::]:5683/zclbase",
//			"coap://:::5683/zclbase",
//			"coap://[fe80::219:99ff:fe62:8a90]:5683/zclbase",
			"/zclbase",
			NULL,NULL,&tid);


	RTL_TRDBG(2,"diaRetrieveRequest ret=%d tid=%d\n",ret,tid);

}

static	void	DiaIpuOk(t_dia_req *preq,t_dia_rspparam *par)
{
	RTL_TRDBG(2,"OK DIA IPU '%s'\n",GetIpuId());

	WDiaUCBRequestOk(DIA_FUNC,preq,NULL);
}

static	void	DiaIpuError(t_dia_req *preq,t_dia_rspparam *par)
{
	RTL_TRDBG(0,"ERROR DIA IPU FAILURE '%s' req='%s' tid=%d code='%s'\n",
		GetIpuId(),
		preq->rq_name,preq->rq_tid,
		preq->rq_scode);

	if	(par && par->pr_timeout)	// TIMEOUT => RETRY YES
	{
		WDiaUCBRequestTimeout(DIA_FUNC,preq,NULL);
		return;
	}
	RTL_TRDBG(0,"ERROR DIA IPU DEFINTIVE FAILURE\n");

	WDiaUCBRequestError(DIA_FUNC,preq,NULL);
}

static	void	DiaIpuInstCreate(t_dia_req *preq,t_dia_rspparam *par,...)
{
	DIA_LOCAL_VAR();
	parseflags	= XOML_PARSE_OBIX;

if	(PseudoOng)
	sprintf(target,"%s%s/%s/containers/DESCRIPTOR/contentInstances/latest",
		GetHostTarget(),SCL_ROOT_APP,GetIpuId());
else
	sprintf(target,"%s%s/%s/containers/DESCRIPTOR/contentInstances",
		GetHostTarget(),SCL_ROOT_APP,GetIpuId());

	if	(par == NULL)
	{
	// REQUEST
retry	:
	WDiaUCBPreLoadTemplate(reqname,preq,target,NULL,xo);
	xo	= WXoNewTemplate("ipu_inst.xml",parseflags);
	if	(!xo)
		return;
	WDiaUCBPostLoadTemplate(reqname,preq,target,NULL,xo);

	DIA_CREATE_REQUEST(DiaIpuInstCreate);
return;
	}
	
	// RESPONSE
	DIA_CREATE_RESPONSE(DiaIpuError,DiaIpuError,DiaIpuOk);
}

static	void	DiaIpuContCreate(t_dia_req *preq,t_dia_rspparam *par,...)
{
	DIA_LOCAL_VAR();

if	(PseudoOng)
	sprintf(target,"%s%s/%s/containers/DESCRIPTOR",
		GetHostTarget(),SCL_ROOT_APP,GetIpuId());
else
	sprintf(target,"%s%s/%s/containers",
		GetHostTarget(),SCL_ROOT_APP,GetIpuId());

	if	(par == NULL)
	{
	// REQUEST
retry	:
	WDiaUCBPreLoadTemplate(reqname,preq,target,NULL,xo);
	xo	= WXoNewTemplate("ipu_cont.xml",0);
	if	(!xo)
		return;
	WDiaUCBPostLoadTemplate(reqname,preq,target,NULL,xo);

	DIA_CREATE_REQUEST(DiaIpuContCreate);
return;
	}

	// RESPONSE
	DIA_CREATE_RESPONSE(DiaIpuError,DiaIpuError,DiaIpuInstCreate);
}

static	void	DiaIpuElemCreate(t_dia_req *preq,t_dia_rspparam *par,...)
{
	DIA_LOCAL_VAR();

if	(PseudoOng)
	sprintf(target,"%s%s/%s",
		GetHostTarget(),SCL_ROOT_APP,GetIpuId());
else
	sprintf(target,"%s%s",
		GetHostTarget(),SCL_ROOT_APP);

	if	(par == NULL)
	{
	// REQUEST
retry	:
	WDiaUCBPreLoadTemplate(reqname,preq,target,NULL,xo);
	xo	= WXoNewTemplate("ipu_elem.xml",parseflags);
	if	(!xo)
		return;
	WDiaUCBPostLoadTemplate(reqname,preq,target,NULL,xo);

	DIA_CREATE_REQUEST(DiaIpuElemCreate);
return;
	}
	
	// RESPONSE
	DIA_CREATE_RESPONSE(DiaIpuError,DiaIpuError,DiaIpuContRetrieve);
}

static	void	DiaIpuContRetrieve(t_dia_req *preq,t_dia_rspparam *par,...)
{
	DIA_LOCAL_VAR();

if	(PseudoOng)
	sprintf(target,"%s%s/%s/containers/DESCRIPTOR",
		GetHostTarget(),SCL_ROOT_APP,GetIpuId());
else
	sprintf(target,"%s%s/%s/containers/DESCRIPTOR",
		GetHostTarget(),SCL_ROOT_APP,GetIpuId());

	if	(par == NULL)
	{
	// REQUEST
retry	:
	DIA_RETRIEVE_REQUEST(DiaIpuContRetrieve);
return;
	}
	
	// RESPONSE
	DIA_RETRIEVE_RESPONSE(DiaIpuError,DiaIpuContCreate,DiaIpuInstCreate);
}

static	void	DiaIpuElemRetrieve(t_dia_req *preq,t_dia_rspparam *par,...)
{
	DIA_LOCAL_VAR();

if	(PseudoOng)
	sprintf(target,"%s%s/%s",GetHostTarget(),SCL_ROOT_APP,GetIpuId());
else
	sprintf(target,"%s%s/%s",GetHostTarget(),SCL_ROOT_APP,GetIpuId());

	if	(par == NULL)
	{
	// REQUEST
retry	:
	DIA_RETRIEVE_REQUEST(DiaIpuElemRetrieve);
return;
	}
	
	// RESPONSE
	DIA_RETRIEVE_RESPONSE(DiaIpuError,DiaIpuElemCreate,DiaIpuContRetrieve);
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
	DiaIpuElemRetrieve(&req,NULL);
}


void	CmdDiaIpu(t_cli *cli,char *buf)
{
	t_dia_req	req;

	memset	(&req,0,sizeof(t_dia_req));
	req.rq_cli	= cli;
//	req.rq_cli	= NULL;
	DiaIpuElemRetrieve(&req,NULL);
}
