
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

/*! @file diadat.c
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

void DIA_LEAK WDiaUCBRequestOk_DiaDatOk(t_dia_req *preq, t_cmn_sensor *cmn) {}

int DIA_LEAK WDiaUCBPreLoadTemplate_DiaDatInstCreate(t_dia_req *preq, char *target, t_cmn_sensor *cmn, void **xo,char *templ)
{
    return 0;
}
int DIA_LEAK WDiaUCBPreLoadTemplate_DiaDatContCreate(t_dia_req *preq, char *target, t_cmn_sensor *cmn, void **xo,char *templ)
{
    return 0;
}
int DIA_LEAK WDiaUCBPreLoadTemplate_DiaDatElemCreate(t_dia_req *preq, char *target, t_cmn_sensor *cmn, void **xo,char *templ)
{
    return 0;
}
int DIA_LEAK WDiaUCBPostLoadTemplate_DiaDatInstCreate(t_dia_req *preq, char *target, t_cmn_sensor *cmn, void *xo)
{
    return 0;
}
int DIA_LEAK WDiaUCBPostLoadTemplate_DiaDatContCreate(t_dia_req *preq, char *target, t_cmn_sensor *cmn, void *xo)
{
    return 0;
}
int DIA_LEAK WDiaUCBPostLoadTemplate_DiaDatElemCreate(t_dia_req *preq, char *target, t_cmn_sensor *cmn, void *xo)
{
    return 0;
}


static	void DiaDatOk(t_dia_req *preq, t_dia_rspparam *par);
static	void DiaDatError(t_dia_req *preq, t_dia_rspparam *par);
static	void DiaDatContCreate(t_dia_req *preq, t_dia_rspparam *par, ...);
static	void DiaDatElemCreate(t_dia_req *preq, t_dia_rspparam *par, ...);
static	void DiaDatContRetrieve(t_dia_req *preq, t_dia_rspparam *par, ...);
static	void DiaDatElemRetrieve(t_dia_req *preq, t_dia_rspparam *par, ...);

static	void	DiaDatOk(t_dia_req *preq,t_dia_rspparam *par)
{
	DIA_DEVICE_CONTEXT();


	RTL_TRDBG(2,"OK DIA DAT '%s' num=%04x attr=%04x member=%d\n",
		cmn->cmn_ieee,preq->rq_cluster,preq->rq_attribut,preq->rq_member);
	cmn->cmn_dia_datok++;

	WDiaUCBRequestOk(DIA_FUNC,preq,cmn);
	WDiaUCBRequestOk_DiaDatOk(preq,cmn);
}

static	void	DiaDatError(t_dia_req *preq,t_dia_rspparam *par)
{
	DIA_DEVICE_CONTEXT();

	RTL_TRDBG(0,"ERROR DIA DAT FAILURE '%s' req='%s' tid=%d code='%s' num=%04x\n",
	cmn->cmn_ieee,preq->rq_name,preq->rq_tid,preq->rq_scode,preq->rq_cluster);

	cmn->cmn_dia_datok	= 0;

	if	(par && par->pr_timeout)	// TIMEOUT => RETRY NO
	{
		WDiaUCBRequestTimeout(DIA_FUNC,preq,cmn);
		return;
	}
	RTL_TRDBG(0,"ERROR DIA DAT DEFINTIVE FAILURE\n");

	WDiaUCBRequestError(DIA_FUNC,preq,cmn);
}

static	void	DiaDatContCreate(t_dia_req *preq,t_dia_rspparam *par,...)
{
	DIA_LOCAL_VAR();
//	char	cnum[64];

	DIA_DEVICE_CONTEXT();

if	(PseudoOng || updatecont)
	sprintf(target,"%s%s/APP_%s.%d/containers/0x%04x.0x%04x.%d.m2m",
	GetHostTarget(),SCL_ROOT_APP,
	cmn->cmn_ieee,preq->rq_app,preq->rq_cluster,preq->rq_attribut,preq->rq_member);
else
	sprintf(target,"%s%s/APP_%s.%d/containers",
	GetHostTarget(),SCL_ROOT_APP,cmn->cmn_ieee,preq->rq_app);

	if	(par == NULL)
	{
	// REQUEST
retry	:
	if	(preq->rq_waitRsp == NUSE_REQUEST_DIA)
	{ // xo load + UCB are called only for new requests
		sprintf	(tmp,"dat_cont_%04x_%04x_%d.xml",
			preq->rq_cluster,preq->rq_attribut,preq->rq_member);

		xo	= NULL;
		WDiaUCBPreLoadTemplate(reqname,preq,target,cmn,&xo,tmp);
		WDiaUCBPreLoadTemplate_DiaDatContCreate(preq,target,cmn,&xo,tmp);
		if	(!xo)
		{
			if	(WXoIsTemplate(tmp))
			{	// specific template for this container
				// => no post load => no model config
				xo	= WXoNewTemplate(tmp,0);
			}
			else
			{
				xo	= WXoNewTemplate("dat_cont.xml",0);
				WDiaUCBPostLoadTemplate(reqname,preq,target,cmn,xo);
				WDiaUCBPostLoadTemplate_DiaDatContCreate(preq,target,cmn,xo);
			}
		}
		else
		{
			WDiaUCBPostLoadTemplate(reqname,preq,target,cmn,xo);
			WDiaUCBPostLoadTemplate_DiaDatContCreate(preq,target,cmn,xo);
		}

		if	(!xo)
			return;
		if	(updatecont)
			XoNmUnSetAttr(xo,"m2m:id",0,0);
	}

	DIA_CREATE_REQUEST(DiaDatContCreate);
return;
	}

	// RESPONSE
	preq->rq_flags	&= ~RQT_UPDATE_CONT;
	DIA_CREATE_RESPONSE(DiaDatError,DiaDatError,DiaDatOk);
}

// same as DiaAppElemCreate()
static	void	DiaDatElemCreate(t_dia_req *preq,t_dia_rspparam *par,...)
{
	DIA_LOCAL_VAR();
	DIA_DEVICE_CONTEXT();

if	(PseudoOng || updateelem)
	sprintf(target,"%s%s/APP_%s.%d",
		GetHostTarget(),SCL_ROOT_APP,cmn->cmn_ieee,preq->rq_app);
else
	sprintf(target,"%s%s",GetHostTarget(),SCL_ROOT_APP);

	if	(par == NULL)
	{
	// REQUEST
retry	:
	if	(preq->rq_waitRsp == NUSE_REQUEST_DIA)
	{ // xo load + UCB are called only for new requests
		xo	= NULL;
		strcpy	(tmp,"app_elem.xml");
		WDiaUCBPreLoadTemplate(reqname,preq,target,cmn,&xo,tmp);
		WDiaUCBPreLoadTemplate_DiaDatElemCreate(preq,target,cmn,&xo,tmp);
		if	(!xo)
		{
			xo	= WXoNewTemplate(tmp,parseflags);
			if	(!xo)
				return;
		}
		WDiaUCBPostLoadTemplate(reqname,preq,target,cmn,xo);
		WDiaUCBPostLoadTemplate_DiaDatElemCreate(preq,target,cmn,xo);
		if	(updateelem)
			XoNmUnSetAttr(xo,"appId",0,0);
	}

	DIA_CREATE_REQUEST(DiaDatElemCreate);
return;
	}
	
	// RESPONSE
	preq->rq_flags	&= ~RQT_UPDATE_ELEM;
	DIA_CREATE_RESPONSE(DiaDatError,DiaDatError,DiaDatContRetrieve);
}

static	void	DiaDatContRetrieve(t_dia_req *preq,t_dia_rspparam *par,...)
{
	DIA_LOCAL_VAR();
	DIA_DEVICE_CONTEXT();

if	(PseudoOng)
	sprintf(target,"%s%s/APP_%s.%d/containers/0x%04x.0x%04x.%d.m2m",
	GetHostTarget(),SCL_ROOT_APP,
	cmn->cmn_ieee,preq->rq_app,preq->rq_cluster,preq->rq_attribut,preq->rq_member);
else
	sprintf(target,"%s%s/APP_%s.%d/containers/0x%04x.0x%04x.%d.m2m",
	GetHostTarget(),SCL_ROOT_APP,cmn->cmn_ieee,preq->rq_app,
	preq->rq_cluster,preq->rq_attribut,preq->rq_member);

	if	(par == NULL)
	{
	// REQUEST
retry	:
	DIA_RETRIEVE_REQUEST(DiaDatContRetrieve);
return;
	}
	
	// RESPONSE
	if	(updatecont)
	{
	DIA_RETRIEVE_RESPONSE(DiaDatError,DiaDatContCreate,DiaDatContCreate);
	}
	else
	{
	DIA_RETRIEVE_RESPONSE(DiaDatError,DiaDatContCreate,DiaDatOk);
	}
}

// same as DiaAppElemRetrieve()
static	void	DiaDatElemRetrieve(t_dia_req *preq,t_dia_rspparam *par,...)
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
	DIA_RETRIEVE_REQUEST(DiaDatElemRetrieve);
return;
	}
	
	// RESPONSE
	if	(updateelem)
	{
	DIA_RETRIEVE_RESPONSE(DiaDatError,DiaDatElemCreate,DiaDatElemCreate);
	}
	else
	{
	DIA_RETRIEVE_RESPONSE(DiaDatError,DiaDatElemCreate,DiaDatContRetrieve);
	}
}

/*!
 *
 * @brief start DIA macro request for "interface-datafield" M2M application
 * 	- contentInstances are not created here but in file diarep.c
 * @param dev device/sensor number
 * @param serial serial number of the device
 * @param app application number
 * @param cluster cluster number
 * @param attribut attribut number
 * @param member member number
 * @return void
 * 
 */
void	DiaDatStart(int dev,int serial,int app,int cluster,int attribut,int member)
{
	t_dia_req	req;

	memset	(&req,0,sizeof(t_dia_req));
	req.rq_cli	= NULL;
	req.rq_dev	= dev;
	req.rq_serial	= serial;
	req.rq_app	= app;
	req.rq_cluster	= cluster;
	req.rq_attribut	= attribut;
	req.rq_member	= member;
	req.rq_name	= (char *)__func__;
	req.rq_flags	= DiaRequestMode; DiaRequestMode = 0;
	DiaDatElemRetrieve(&req,NULL);
}


void	CmdDiaDat(t_cli *cli,char *buf)
{
	t_dia_req	req;

	memset	(&req,0,sizeof(t_dia_req));
	req.rq_cli	= cli;
//	req.rq_cli	= NULL;
	req.rq_flags	= DiaRequestMode; DiaRequestMode = 0;
	DiaDatElemRetrieve(&req,NULL);
}
