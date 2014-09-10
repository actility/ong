
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
	char	tmp[256];

	DIA_DEVICE_CONTEXT();

if	(PseudoOng)
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
//	sprintf(cnum,"%04x.0x%04x.%d.m2m",preq->rq_cluster,preq->rq_attribut,preq->rq_member);
//	SetVar("c_num",cnum);
//	SetVar("d_ieee",cmn->cmn_ieee);

	sprintf	(tmp,"dat_cont_%04x_%04x_%d.xml",
		preq->rq_cluster,preq->rq_attribut,preq->rq_member);

	WDiaUCBPreLoadTemplate(reqname,preq,target,cmn,xo);
	if	(WXoIsTemplate(tmp))
	{	// specific template for this container
		// => no post load => no model config
		xo	= WXoNewTemplate(tmp,0);
	}
	else
	{	// read model config to get storage configuration TODO
#if	0
		char	m2mvar[256];
		void	*mdlitf	= NULL;

		sprintf	(tmp,"0x%04x",preq->rq_cluster);
		if	(MdlCfg)
		{
			mdlitf	= MdlGetInterface(MdlCfg,NULL,"server",tmp);
		}

		sprintf	(m2mvar,"0x%04x.0x%04x.%d.m2m",
			preq->rq_cluster,preq->rq_attribut,preq->rq_member);
#endif

		xo	= WXoNewTemplate("dat_cont.xml",0);
#if	0
		if	(MdlCfg)
		{
			MdlConfigureDataContainerTemplate(mdlitf,xo,m2mvar);
		}
#endif
		WDiaUCBPostLoadTemplate(reqname,preq,target,cmn,xo);
	}

	if	(!xo)
		return;

	DIA_CREATE_REQUEST(DiaDatContCreate);
return;
	}

	// RESPONSE
	DIA_CREATE_RESPONSE(DiaDatError,DiaDatError,DiaDatOk);
}

// same as DiaAppElemCreate()
static	void	DiaDatElemCreate(t_dia_req *preq,t_dia_rspparam *par,...)
{
	DIA_LOCAL_VAR();
	DIA_DEVICE_CONTEXT();

if	(PseudoOng)
	sprintf(target,"%s%s/APP_%s.%d",
		GetHostTarget(),SCL_ROOT_APP,cmn->cmn_ieee,preq->rq_app);
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

	DIA_CREATE_REQUEST(DiaDatElemCreate);
return;
	}
	
	// RESPONSE
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
	DIA_RETRIEVE_RESPONSE(DiaDatError,DiaDatContCreate,DiaDatOk);
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
	DIA_RETRIEVE_RESPONSE(DiaDatError,DiaDatElemCreate,DiaDatContRetrieve);
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
	DiaDatElemRetrieve(&req,NULL);
}


void	CmdDiaDat(t_cli *cli,char *buf)
{
	t_dia_req	req;

	memset	(&req,0,sizeof(t_dia_req));
	req.rq_cli	= cli;
//	req.rq_cli	= NULL;
	DiaDatElemRetrieve(&req,NULL);
}
