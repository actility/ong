
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

#include "knx.h"

#include "cmndefine.h"
#include "cmnstruct.h"
#include "speccproto.h"
#include "cmnproto.h"
#include "specextern.h"

#include "diadef.h"

static NetworkList_t *fetchForNetwork = NULL;

extern	void	*MainTbPoll;
extern	int	TraceLevel;

extern	dia_context_t	*DiaCtxt;

void DIA_LEAK WDiaUCBRequestOk_DiaTopologyDatOk(t_dia_req *preq, t_cmn_sensor *cmn) {}

int DIA_LEAK WDiaUCBPreLoadTemplate_DiaTopologyDatContCreate(t_dia_req *preq, char *target, t_cmn_sensor *cmn, void **xo,char *templ)
{
    return 0;
}

int DIA_LEAK WDiaUCBPostLoadTemplate_DiaTopologyDatContCreate(t_dia_req *preq, char *target, t_cmn_sensor *cmn, void *xo)
{
    return 0;
}

static	void DiaTopologyDatOk(t_dia_req *preq, t_dia_rspparam *par);
static	void DiaTopologyDatError(t_dia_req *preq, t_dia_rspparam *par);
static	void DiaTopologyDatContCreate(t_dia_req *preq, t_dia_rspparam *par, ...);
static	void DiaTopologyDatContRetrieve(t_dia_req *preq, t_dia_rspparam *par, ...);

static	void	DiaTopologyDatOk(t_dia_req *preq,t_dia_rspparam *par)
{
	RTL_TRDBG(2,"OK DIA DAT 'IPU' num=%04x attr=%04x member=%d\n",
		preq->rq_cluster,preq->rq_attribut,preq->rq_member);

	WDiaUCBRequestOk(DIA_FUNC,preq,NULL);
	WDiaUCBRequestOk_DiaTopologyDatOk(preq,NULL);
}

static	void	DiaTopologyDatError(t_dia_req *preq,t_dia_rspparam *par)
{
	RTL_TRDBG(0,"ERROR DIA DAT FAILURE 'IPU' req='%s' tid=%d code='%s' num=%04x\n",
	preq->rq_name,preq->rq_tid,preq->rq_scode,preq->rq_cluster);

	if	(par && par->pr_timeout)	// TIMEOUT => RETRY NO
	{
		WDiaUCBRequestTimeout(DIA_FUNC,preq,NULL);
		return;
	}
	
	if (strcmp(preq->rq_scode, "STATUS_CONFLICT") == 0) {
	    WDiaUCBRequestOk(DIA_FUNC,preq,NULL);
	    WDiaUCBRequestOk_DiaTopologyDatOk(preq,NULL);
	    return;
	}
	
	RTL_TRDBG(0,"ERROR DIA DAT DEFINTIVE FAILURE\n");

	WDiaUCBRequestError(DIA_FUNC,preq,NULL);
}

// Create the topology container
static
void
DiaTopologyDatContCreate(t_dia_req *preq, t_dia_rspparam *par, ...)
{
	DIA_LOCAL_VAR();

if	(PseudoOng)
	sprintf(target,"%s%s/%s/containers/topology", GetHostTarget(), SCL_ROOT_APP, GetVar("w_ipuid"));
else
	sprintf(target,"%s%s/%s/containers", GetHostTarget(), SCL_ROOT_APP, GetVar("w_ipuid"));

	if	(par == NULL)
	{
	// REQUEST
retry	:
	if	(preq->rq_waitRsp == NUSE_REQUEST_DIA) {
	    xo	= NULL;
		strcpy (tmp, "dat_ipu_cont.xml");
		
		WDiaUCBPreLoadTemplate(reqname,preq,target,NULL,&xo,tmp);
		WDiaUCBPreLoadTemplate_DiaTopologyDatContCreate(preq,target,NULL,&xo,tmp);
		
		if (!xo) {
			xo	= WXoNewTemplate("dat_ipu_cont.xml",0);
			if (!xo) {
			    return;
		    }
		}
			
		WDiaUCBPostLoadTemplate(reqname,preq,target,NULL,xo);
		WDiaUCBPostLoadTemplate_DiaTopologyDatContCreate(preq,target,NULL,xo);
	}

	DIA_CREATE_REQUEST(DiaTopologyDatContCreate);
return;
	}

	// RESPONSE
	DIA_CREATE_RESPONSE(DiaTopologyDatError,DiaTopologyDatError,DiaTopologyDatOk);
}

// Ensure topology container exists
static
void
DiaTopologyDatContRetrieve(t_dia_req *preq, t_dia_rspparam *par, ...)
{
	DIA_LOCAL_VAR();

    sprintf(target,"%s%s/%s/containers/topology", GetHostTarget(), SCL_ROOT_APP, GetVar("w_ipuid"));

	if	(par == NULL)
	{
	// REQUEST
retry	:
	DIA_RETRIEVE_REQUEST(DiaTopologyDatContRetrieve);
return;
	}
	
	// RESPONSE
	DIA_RETRIEVE_RESPONSE(DiaTopologyDatError,DiaTopologyDatContCreate,DiaTopologyDatOk);
}

void
DiaTopologyDatStart(void)
{
	t_dia_req	req;

	memset	(&req,0,sizeof(t_dia_req));
	req.rq_cli	= NULL;
	req.rq_name	= (char *)__func__;
	req.rq_flags = 0;
	DiaTopologyDatContRetrieve(&req, NULL);
}

/* -------------------------------------------- */

static	void DiaTopologyOk(t_dia_req *preq, t_dia_rspparam *par);
static	void DiaTopologyError(t_dia_req *preq, t_dia_rspparam *par);
static	void DiaTopologyRetrieve(t_dia_req *preq, t_dia_rspparam *par, ...);

static
void
DiaTopologyOk(t_dia_req *preq,t_dia_rspparam *par)
{
    int parseflags = 0;
    void *xo = NULL;
    
    xo = WDiaXoLoadResponse(preq, parseflags);
    
    SensorInitializeNetwork(fetchForNetwork, xo);
    SensorCreateNetworkCache(fetchForNetwork, fetchForNetwork->xo);
    fetchForNetwork = NULL;
    
    SensorStartupNetworks();
}

static
void
DiaTopologyError(t_dia_req *preq,t_dia_rspparam *par)
{
	RTL_TRDBG(0,"ERROR DIA DAT FAILURE 'IPU' req='%s' tid=%d code='%s' num=%04x\n",
	preq->rq_name,preq->rq_tid,preq->rq_scode,preq->rq_cluster);

	if (par && par->pr_timeout) {
        return;
    }
    RTL_TRDBG(0,"ERROR DIA DAT DEFINTIVE FAILURE\n");

    fetchForNetwork->xo = NULL;
    SensorCreateNetworkCache(fetchForNetwork, NULL);
    fetchForNetwork = NULL;
    SensorStartupNetworks();    // Create an empty network
}

static
void
DiaTopologyRetrieve(t_dia_req *preq, t_dia_rspparam *par, ...)
{
	DIA_LOCAL_VAR();
	
    sprintf(target,"%s%s/%s/containers/topology/contentInstances/latest/content", GetHostTarget(), SCL_ROOT_APP, GetVar("w_ipuid"));

	if	(par == NULL)
	{
	// REQUEST
retry	:
	DIA_RETRIEVE_REQUEST(DiaTopologyRetrieve);
return;
	}
	
	// RESPONSE
	DIA_RETRIEVE_RESPONSE(DiaTopologyError,DiaTopologyError,DiaTopologyOk);
}

int
DiaTopologyFetch(NetworkList_t *network, void *cl)
{
	t_dia_req	req;

    if (fetchForNetwork != NULL) {
        return -1;  // Busy
    }
    fetchForNetwork = network;

	memset	(&req,0,sizeof(t_dia_req));
	req.rq_cli	= cl;
	req.rq_name	= (char *)__func__;
	DiaTopologyRetrieve(&req, NULL);
	
	return 0;
}
