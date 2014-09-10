
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

/*! @file diaacc.c
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

static	void	DiaAccOk(t_dia_req *preq,t_dia_rspparam *par)
{
	DIA_DEVICE_CONTEXT();

	RTL_TRDBG(2,"OK DIA ACC '%s'\n",cmn->cmn_ieee);

	cmn->cmn_dia_accok++;

	WDiaUCBRequestOk(DIA_FUNC,preq,cmn);
}

static	void	DiaAccError(t_dia_req *preq,t_dia_rspparam *par)
{
	DIA_DEVICE_CONTEXT();

	RTL_TRDBG(0,"ERROR DIA ACC FAILURE '%s' req='%s' tid=%d code='%s'\n",
		cmn->cmn_ieee,preq->rq_name,preq->rq_tid,preq->rq_scode);

	cmn->cmn_dia_accok	= 0;

	if	(par && par->pr_timeout)	// TIMEOUT => RETRY NO
	{
		WDiaUCBRequestTimeout(DIA_FUNC,preq,cmn);
		return;
	}
	RTL_TRDBG(0,"ERROR DIA ACC DEFINTIVE FAILURE\n");

	WDiaUCBRequestError(DIA_FUNC,preq,cmn);
}

static	void	DiaAccElemCreate(t_dia_req *preq,t_dia_rspparam *par,...)
{
	DIA_LOCAL_VAR();
	char	tmp[256];

	DIA_DEVICE_CONTEXT();

if	(PseudoOng)
	sprintf(target,"%s%s/APP_%s.%d_%s",GetHostTarget(),SCL_ROOT_ACC,
		cmn->cmn_ieee,preq->rq_app,WDiaAccessRightsTxt(preq->rq_access));
else
	sprintf(target,"%s%s",GetHostTarget(),SCL_ROOT_ACC);

	if	(par == NULL)
	{
	// REQUEST
retry	:
//	SetVar("d_ieee",cmn->cmn_ieee);
	sprintf	(tmp,"acc_elem_%s.xml",WDiaAccessRightsTxt(preq->rq_access));
	WDiaUCBPreLoadTemplate(reqname,preq,target,cmn,xo);
	xo	= WXoNewTemplate(tmp,parseflags);
	if	(!xo)
		return;
	WDiaUCBPostLoadTemplate(reqname,preq,target,cmn,xo);

	DIA_CREATE_REQUEST(DiaAccElemCreate);
return;
	}
	
	// RESPONSE
	DIA_CREATE_RESPONSE(DiaAccError,DiaAccError,DiaAccOk);
}

static	void	DiaAccElemRetrieve(t_dia_req *preq,t_dia_rspparam *par,...)
{
	DIA_LOCAL_VAR();
	DIA_DEVICE_CONTEXT();

if	(PseudoOng)
	sprintf(target,"%s%s/APP_%s.%d_%s",GetHostTarget(),SCL_ROOT_ACC,
		cmn->cmn_ieee,preq->rq_app,WDiaAccessRightsTxt(preq->rq_access));
else
	sprintf(target,"%s%s/APP_%s.%d_%s",GetHostTarget(),SCL_ROOT_ACC,
		cmn->cmn_ieee,preq->rq_app,WDiaAccessRightsTxt(preq->rq_access));

	if	(par == NULL)
	{
	// REQUEST
retry	:
	DIA_RETRIEVE_REQUEST(DiaAccElemRetrieve);
return;
	}
	
	// RESPONSE
	DIA_RETRIEVE_RESPONSE(DiaAccError,DiaAccElemCreate,DiaAccOk);
}



/*!
 *
 * @brief start DIA macro request for "access rights" M2M application
 * @param dev device/sensor number
 * @param serial serial number of the device
 * @param app application number
 * @return void
 * 
 */
void	DiaAccStart(int dev,int serial,int app,int acc)
{
	t_dia_req	req;

	memset	(&req,0,sizeof(t_dia_req));
	req.rq_cli	= NULL;
	req.rq_dev	= dev;
	req.rq_serial	= serial;
	req.rq_app	= app;
	req.rq_access	= acc;
	req.rq_name	= (char *)__func__;
	DiaAccElemRetrieve(&req,NULL);
}


void	CmdDiaAcc(t_cli *cli,char *buf)
{
	t_dia_req	req;

	memset	(&req,0,sizeof(t_dia_req));
	req.rq_cli	= cli;
//	req.rq_cli	= NULL;
	DiaAccElemRetrieve(&req,NULL);
}
