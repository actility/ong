
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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
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

int	DiaUp;


static	void	DiaKalOk(t_dia_req *preq,t_dia_rspparam *par)
{
	DiaUp++;
	if	(DiaUp == INT_MAX)
		DiaUp	= 1;
	RTL_TRDBG(2,"OK DIA KAL %u '%s'\n",DiaUp,GetIpuId());
}

static	void	DiaKalError(t_dia_req *preq,t_dia_rspparam *par)
{
	RTL_TRDBG(0,"ERROR DIA KAL FAILURE '%s' req='%s' tid=%d code='%s'\n",
		GetIpuId(),preq->rq_name,preq->rq_tid,preq->rq_scode);


	DiaUp	= 0;
	if	(par && par->pr_timeout)	// TIMEOUT => RETRY NO
	{
		return;
	}
	RTL_TRDBG(0,"ERROR DIA KAL DEFINTIVE FAILURE\n");
}

static	void	DiaKalElemRetrieve(t_dia_req *preq,t_dia_rspparam *par,...)
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
	DIA_RETRIEVE_REQUEST(DiaKalElemRetrieve);
return;
	}
	
	// RESPONSE
	DIA_RETRIEVE_RESPONSE(DiaKalError,DiaKalError,DiaKalOk);
}



void	DiaKalStart()
{
	t_dia_req	req;

	memset	(&req,0,sizeof(t_dia_req));
	req.rq_cli	= NULL;
	req.rq_name	= (char *)__func__;
	req.rq_flags	= DiaRequestMode; DiaRequestMode = 0;
	DiaKalElemRetrieve(&req,NULL);
}


void	CmdDiaKal(t_cli *cli,char *buf)
{
	t_dia_req	req;

	memset	(&req,0,sizeof(t_dia_req));
	req.rq_cli	= cli;
//	req.rq_cli	= NULL;
	req.rq_flags	= DiaRequestMode; DiaRequestMode = 0;
	DiaKalElemRetrieve(&req,NULL);
}
