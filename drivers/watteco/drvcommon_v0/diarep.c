
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

/*! @file diarep.c
 *
 */
#include <stdio.h>			/************************************/
#include <stdlib.h>			/*     run in main thread           */
#include <string.h>			/************************************/
#include <stdarg.h>
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

static	void	DiaRepOk(t_dia_req *preq,t_dia_rspparam *par)
{
	DIA_DEVICE_CONTEXT();

	RTL_TRDBG(2,"OK DIA REP '%s' num=%04x\n",
					cmn->cmn_ieee,preq->rq_cluster);

	cmn->cmn_dia_repok++;

	WDiaUCBRequestOk(DIA_FUNC,preq,cmn);
}

static	void	DiaRepError(t_dia_req *preq,t_dia_rspparam *par)
{
	DIA_DEVICE_CONTEXT();

	RTL_TRDBG(0,"ERROR DIA REP FAILURE '%s' req='%s' tid=%d code='%s' num=%04x\n",
	cmn->cmn_ieee,preq->rq_name,preq->rq_tid,preq->rq_scode,preq->rq_cluster);


	cmn->cmn_dia_reperr++;

	if	(par && par->pr_timeout)	// TIMEOUT => RETRY NO
	{	// pb de r_value qui a pu changer
		WDiaUCBRequestTimeout(DIA_FUNC,preq,cmn);
		return;
	}
	RTL_TRDBG(0,"ERROR DIA REP DEFINTIVE FAILURE\n");

	WDiaUCBRequestError(DIA_FUNC,preq,cmn);
}

static	void	DiaRepInstCreate(t_dia_req *preq,t_dia_rspparam *par,...)
{
	DIA_LOCAL_VAR();
	char	tmp[256];

	parseflags	= XOML_PARSE_OBIX;

	DIA_DEVICE_CONTEXT();

if	(PseudoOng)
	sprintf(target,"%s%s/APP_%s.%d/containers/0x%04x.0x%04x.%d.m2m/contentInstances/latest",
	GetHostTarget(),SCL_ROOT_APP,
	cmn->cmn_ieee,preq->rq_app,preq->rq_cluster,preq->rq_attribut,preq->rq_member);
else
	sprintf(target,"%s%s/APP_%s.%d/containers/0x%04x.0x%04x.%d.m2m/contentInstances",
	GetHostTarget(),SCL_ROOT_APP,
	cmn->cmn_ieee,preq->rq_app,preq->rq_cluster,preq->rq_attribut,preq->rq_member);

	if	(par == NULL)
	{
	// REQUEST
retry	:

//	SetVar("d_ieee",cmn->cmn_ieee);
	sprintf	(tmp,"dat_inst_%04x_%04x_%d.xml",
		preq->rq_cluster,preq->rq_attribut,preq->rq_member);
	WDiaUCBPreLoadTemplate(reqname,preq,target,cmn,xo);
	xo	= WXoNewTemplate(tmp,parseflags);
	if	(!xo)
		return;
	WDiaUCBPostLoadTemplate(reqname,preq,target,cmn,xo);

	DIA_CREATE_REQUEST(DiaRepInstCreate);
return;
	}
	
	// RESPONSE
	DIA_CREATE_RESPONSE(DiaRepError,DiaRepError,DiaRepOk);
}

/*!
 *
 * @brief start DIA macro request for "interface-datafield" reporting value
 * 	- $r_value is set with fmt ... parameters
 * 	- load and parse diatemplates/dat_inst_%04x_%04x_%d.xml
 * @param dev device/sensor number
 * @param serial serial number of the device
 * @param app application number
 * @param cluster cluster number
 * @param attribut attribut number
 * @param member member number
 * @param fmt ... printf(3) like
 * @return void
 * 
 */
void	DiaReportValue(int dev,int serial,int app,int cluster,int attr,int member,char *fmt,...)
{
	t_dia_req	req;
	char	value[1024];
	va_list	listArg;

	if	(DiaDisconnected())
		return;

	va_start(listArg,fmt);
	vsprintf (value, fmt, listArg);
	va_end(listArg);
	SetVar("r_value",value);

	memset	(&req,0,sizeof(t_dia_req));
	req.rq_cli	= NULL;
	req.rq_dev	= dev;
	req.rq_serial	= serial;
	req.rq_app	= app;
	req.rq_cluster	= cluster;
	req.rq_attribut	= attr;
	req.rq_member	= member;
	req.rq_name	= (char *)__func__;
	DiaRepInstCreate(&req,NULL);
}
