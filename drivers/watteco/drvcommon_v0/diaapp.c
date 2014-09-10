
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

/*! @file diaapp.c
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

static	void DiaAppOk(t_dia_req *preq, t_dia_rspparam *par);
static	void DiaAppError(t_dia_req *preq, t_dia_rspparam *par);
static	void DiaAppInstCreate(t_dia_req *preq, t_dia_rspparam *par, ...);
static	void DiaAppContCreate(t_dia_req *preq, t_dia_rspparam *par, ...);
static	void DiaAppElemCreate(t_dia_req *preq, t_dia_rspparam *par, ...);
static	void DiaAppContRetrieve(t_dia_req *preq, t_dia_rspparam *par, ...);
static	void DiaAppElemRetrieve(t_dia_req *preq, t_dia_rspparam *par, ...);


static	void	DiaAppOk(t_dia_req *preq,t_dia_rspparam *par)
{
	DIA_DEVICE_CONTEXT();

	RTL_TRDBG(2,"OK DIA APP '%s'\n",cmn->cmn_ieee);

	cmn->cmn_dia_appok++;

	WDiaUCBRequestOk(DIA_FUNC,preq,cmn);
}

static	void	DiaAppError(t_dia_req *preq,t_dia_rspparam *par)
{
	DIA_DEVICE_CONTEXT();

	RTL_TRDBG(0,"ERROR DIA APP FAILURE '%s' req='%s' tid=%d code='%s'\n",
		cmn->cmn_ieee,preq->rq_name,preq->rq_tid,preq->rq_scode);

	cmn->cmn_dia_appok	= 0;

	if	(par && par->pr_timeout)	// TIMEOUT => RETRY NO
	{
		WDiaUCBRequestTimeout(DIA_FUNC,preq,cmn);
		return;
	}
	RTL_TRDBG(0,"ERROR DIA APP DEFINTIVE FAILURE\n");

	WDiaUCBRequestError(DIA_FUNC,preq,cmn);
}

static	void	DiaAppInstCreate(t_dia_req *preq,t_dia_rspparam *par,...)
{
	DIA_LOCAL_VAR();
	parseflags	= XOML_PARSE_OBIX;

	DIA_DEVICE_CONTEXT();

if	(PseudoOng)
	sprintf(target,"%s%s/APP_%s.%d/containers/DESCRIPTOR/contentInstances/latest",
	GetHostTarget(),SCL_ROOT_APP,cmn->cmn_ieee,preq->rq_app);
else
	sprintf(target,"%s%s/APP_%s.%d/containers/DESCRIPTOR/contentInstances",
	GetHostTarget(),SCL_ROOT_APP,cmn->cmn_ieee,preq->rq_app);

	if	(par == NULL)
	{
	// REQUEST
retry	:
//	SetVar("d_ieee",cmn->cmn_ieee);
	WDiaUCBPreLoadTemplate(reqname,preq,target,cmn,xo);
	xo	= WXoNewTemplate("app_inst.xml",parseflags);
	if	(!xo)
		return;
	WDiaUCBPostLoadTemplate(reqname,preq,target,cmn,xo);
//	iCmnSensorSetInterfaceListInObj(cmn,preq->rq_app,xo);

	DIA_CREATE_REQUEST(DiaAppInstCreate);
return;
	}
	
	// RESPONSE
	DIA_CREATE_RESPONSE(DiaAppError,DiaAppError,DiaAppOk);
}

static	void	DiaAppContCreate(t_dia_req *preq,t_dia_rspparam *par,...)
{
	DIA_LOCAL_VAR();
	DIA_DEVICE_CONTEXT();

if	(PseudoOng)
	sprintf(target,"%s%s/APP_%s.%d/containers/DESCRIPTOR",
	GetHostTarget(),SCL_ROOT_APP,cmn->cmn_ieee,preq->rq_app);
else
	sprintf(target,"%s%s/APP_%s.%d/containers",
	GetHostTarget(),SCL_ROOT_APP,cmn->cmn_ieee,preq->rq_app);

	if	(par == NULL)
	{
	// REQUEST
retry	:
//	SetVar("d_ieee",cmn->cmn_ieee);
	WDiaUCBPreLoadTemplate(reqname,preq,target,cmn,xo);
	xo	= WXoNewTemplate("app_cont.xml",0);
	if	(!xo)
		return;
	WDiaUCBPostLoadTemplate(reqname,preq,target,cmn,xo);

	DIA_CREATE_REQUEST(DiaAppContCreate);
return;
	}

	// RESPONSE
	DIA_CREATE_RESPONSE(DiaAppError,DiaAppError,DiaAppInstCreate);
}

static	void	DiaAppElemCreate(t_dia_req *preq,t_dia_rspparam *par,...)
{
	DIA_LOCAL_VAR();
	DIA_DEVICE_CONTEXT();

if	(PseudoOng)
	sprintf(target,"%s%s/APP_%s.%d",GetHostTarget(),SCL_ROOT_APP,cmn->cmn_ieee,preq->rq_app);
else
	sprintf(target,"%s%s",GetHostTarget(),SCL_ROOT_APP);

	if	(par == NULL)
	{
	// REQUEST
retry	:
//	SetVar("d_ieee",cmn->cmn_ieee);
	WDiaUCBPreLoadTemplate(reqname,preq,target,cmn,xo);
	xo	= WXoNewTemplate("app_elem.xml",parseflags);
	if	(!xo)
		return;
	WDiaUCBPostLoadTemplate(reqname,preq,target,cmn,xo);
//	iCmnSensorSetSearchStringInObj(cmn,xo);

	DIA_CREATE_REQUEST(DiaAppElemCreate);
return;
	}
	
	// RESPONSE
	DIA_CREATE_RESPONSE(DiaAppError,DiaAppError,DiaAppContRetrieve);
}

static	void	DiaAppContRetrieve(t_dia_req *preq,t_dia_rspparam *par,...)
{
	DIA_LOCAL_VAR();
	DIA_DEVICE_CONTEXT();

if	(PseudoOng)
	sprintf(target,"%s%s/APP_%s.%d/containers/DESCRIPTOR",
	GetHostTarget(),SCL_ROOT_APP,cmn->cmn_ieee,preq->rq_app);
else
	sprintf(target,"%s%s/APP_%s.%d/containers/DESCRIPTOR",
	GetHostTarget(),SCL_ROOT_APP,cmn->cmn_ieee,preq->rq_app);

	if	(par == NULL)
	{
retry	:
	// REQUEST
	DIA_RETRIEVE_REQUEST(DiaAppContRetrieve);
return;
	}
	
	// RESPONSE
	DIA_RETRIEVE_RESPONSE(DiaAppError,DiaAppContCreate,DiaAppInstCreate);
}

static	void	DiaAppElemRetrieve(t_dia_req *preq,t_dia_rspparam *par,...)
{
	DIA_LOCAL_VAR();
	DIA_DEVICE_CONTEXT();

if	(PseudoOng)
	sprintf(target,"%s%s/APP_%s.%d",
	GetHostTarget(),SCL_ROOT_APP,cmn->cmn_ieee,preq->rq_app);
else
	sprintf(target,"%s%s/APP_%s.%d",
	GetHostTarget(),SCL_ROOT_APP,cmn->cmn_ieee,preq->rq_app);

	if	(par == NULL)
	{
	// REQUEST
retry	:
	DIA_RETRIEVE_REQUEST(DiaAppElemRetrieve);
return;
	}
	
	// RESPONSE
	DIA_RETRIEVE_RESPONSE(DiaAppError,DiaAppElemCreate,DiaAppContRetrieve);
}



/*!
 *
 * @brief start DIA macro request for "network-device-application" M2M application
 * @param dev device/sensor number
 * @param serial serial number of the device
 * @param app application number
 * @return void
 * 
 */
void	DiaAppStart(int dev,int serial,int app)
{
	t_dia_req	req;

	memset	(&req,0,sizeof(t_dia_req));
	req.rq_cli	= NULL;
	req.rq_dev	= dev;
	req.rq_serial	= serial;
	req.rq_app	= app;
	req.rq_name	= (char *)__func__;
	DiaAppElemRetrieve(&req,NULL);
}


void	CmdDiaApp(t_cli *cli,char *buf)
{
	t_dia_req	req;

	memset	(&req,0,sizeof(t_dia_req));
	req.rq_cli	= cli;
//	req.rq_cli	= NULL;
	DiaAppElemRetrieve(&req,NULL);
}
