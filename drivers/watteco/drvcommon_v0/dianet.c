
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


static	void DiaNetOk(t_dia_req *preq, t_dia_rspparam *par);
static	void DiaNetError(t_dia_req *preq, t_dia_rspparam *par);
static	void DiaNetInstCreate(t_dia_req *preq, t_dia_rspparam *par, ...);
static	void DiaNetContCreate(t_dia_req *preq, t_dia_rspparam *par, ...);
static	void DiaNetElemCreate(t_dia_req *preq, t_dia_rspparam *par, ...);
static	void DiaNetContRetrieve(t_dia_req *preq, t_dia_rspparam *par, ...);
static	void DiaNetElemRetrieve(t_dia_req *preq, t_dia_rspparam *par, ...);

static	void	DiaNetOk(t_dia_req *preq,t_dia_rspparam *par)
{
	RTL_TRDBG(2,"OK DIA NET '%s'\n",GetNetId());

	WDiaUCBRequestOk(DIA_FUNC,preq,NULL);
}

static	void	DiaNetError(t_dia_req *preq,t_dia_rspparam *par)
{
	RTL_TRDBG(0,"ERROR DIA NET FAILURE '%s' req='%s' tid=%d code='%s'\n",
		GetNetId(),preq->rq_name,preq->rq_tid,preq->rq_scode);


	if	(par && par->pr_timeout)	// TIMEOUT => RETRY YES
	{
//		DiaNetStart();
		WDiaUCBRequestTimeout(DIA_FUNC,preq,NULL);
		return;
	}
	RTL_TRDBG(0,"ERROR DIA NET DEFINTIVE FAILURE\n");

	WDiaUCBRequestError(DIA_FUNC,preq,NULL);
}

static	void	DiaNetInstCreate(t_dia_req *preq,t_dia_rspparam *par,...)
{
	DIA_LOCAL_VAR();

	parseflags	= XOML_PARSE_OBIX;

if	(PseudoOng)
	sprintf(target,"%s%s/NW_%s/containers/DESCRIPTOR/contentInstances/latest",
	GetHostTarget(),SCL_ROOT_APP,GetNetId());
else
	sprintf(target,"%s%s/NW_%s/containers/DESCRIPTOR/contentInstances",
	GetHostTarget(),SCL_ROOT_APP,GetNetId());

	if	(par == NULL)
	{
	// REQUEST
retry	:
	WDiaUCBPreLoadTemplate(reqname,preq,target,NULL,xo);
	xo	= WXoNewTemplate("net_inst.xml",parseflags);
	if	(!xo)
		return;
	WDiaUCBPostLoadTemplate(reqname,preq,target,NULL,xo);
//	SensorSetDeviceListInObj(xo);

	DIA_CREATE_REQUEST(DiaNetInstCreate);
return;
	}
	
	// RESPONSE
	DIA_CREATE_RESPONSE(DiaNetError,DiaNetError,DiaNetOk);
}

static	void	DiaNetContCreate(t_dia_req *preq,t_dia_rspparam *par,...)
{
	DIA_LOCAL_VAR();

if	(PseudoOng)
	sprintf(target,"%s%s/NW_%s/containers/DESCRIPTOR",
	GetHostTarget(),SCL_ROOT_APP,GetNetId());
else
	sprintf(target,"%s%s/NW_%s/containers",
	GetHostTarget(),SCL_ROOT_APP,GetNetId());

	if	(par == NULL)
	{
	// REQUEST
retry	:
	WDiaUCBPreLoadTemplate(reqname,preq,target,NULL,xo);
	xo	= WXoNewTemplate("net_cont.xml",0);
	if	(!xo)
		return;
	WDiaUCBPostLoadTemplate(reqname,preq,target,NULL,xo);

	DIA_CREATE_REQUEST(DiaNetContCreate);
return;
	}

	// RESPONSE
	DIA_CREATE_RESPONSE(DiaNetError,DiaNetError,DiaNetInstCreate);
}

static	void	DiaNetElemCreate(t_dia_req *preq,t_dia_rspparam *par,...)
{
	DIA_LOCAL_VAR();

if	(PseudoOng)
	sprintf(target,"%s%s/NW_%s",GetHostTarget(),SCL_ROOT_APP,GetNetId());
else
	sprintf(target,"%s%s",GetHostTarget(),SCL_ROOT_APP);

	if	(par == NULL)
	{
	// REQUEST
retry	:
	WDiaUCBPreLoadTemplate(reqname,preq,target,NULL,xo);
	xo	= WXoNewTemplate("net_elem.xml",parseflags);
	if	(!xo)
		return;
	WDiaUCBPostLoadTemplate(reqname,preq,target,NULL,xo);

	DIA_CREATE_REQUEST(DiaNetElemCreate);
return;
	}
	
	// RESPONSE
	DIA_CREATE_RESPONSE(DiaNetError,DiaNetError,DiaNetContRetrieve);
}

static	void	DiaNetContRetrieve(t_dia_req *preq,t_dia_rspparam *par,...)
{
	DIA_LOCAL_VAR();

if	(PseudoOng)
	sprintf(target,"%s%s/NW_%s/containers/DESCRIPTOR",
	GetHostTarget(),SCL_ROOT_APP,GetNetId());
else
	sprintf(target,"%s%s/NW_%s/containers/DESCRIPTOR",
	GetHostTarget(),SCL_ROOT_APP,GetNetId());

	if	(par == NULL)
	{
	// REQUEST
retry	:
	DIA_RETRIEVE_REQUEST(DiaNetContRetrieve);
return;
	}
	
	// RESPONSE
	DIA_RETRIEVE_RESPONSE(DiaNetError,DiaNetContCreate,DiaNetInstCreate);
}

static	void	DiaNetElemRetrieve(t_dia_req *preq,t_dia_rspparam *par,...)
{
	DIA_LOCAL_VAR();

if	(PseudoOng)
	sprintf(target,"%s%s/NW_%s",GetHostTarget(),SCL_ROOT_APP,GetNetId());
else
	sprintf(target,"%s%s/NW_%s",GetHostTarget(),SCL_ROOT_APP,GetNetId());

	if	(par == NULL)
	{
	// REQUEST
retry	:
	DIA_RETRIEVE_REQUEST(DiaNetElemRetrieve);
return;
	}
	
	// RESPONSE
	DIA_RETRIEVE_RESPONSE(DiaNetError,DiaNetElemCreate,DiaNetContRetrieve);
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
	DiaNetElemRetrieve(&req,NULL);
}


void	CmdDiaNet(t_cli *cli,char *buf)
{
	t_dia_req	req;

	memset	(&req,0,sizeof(t_dia_req));
	req.rq_cli	= cli;
//	req.rq_cli	= NULL;
	DiaNetElemRetrieve(&req,NULL);
}
