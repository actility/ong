
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

/*! @file diaclient.c
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

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#include "rtlbase.h"
#include "rtlimsg.h"
#include "xoapipr.h"
#include "dIa.h"

#include "cmndefine.h"
#include "cmnstruct.h"

#include "speccproto.h"
#include "cmnproto.h"
#include "specextern.h"

extern	dia_context_t	*DiaCtxt;

static	t_dia_req	TbReq[MAX_REQUEST_DIA];
int			DiaNbReqInuse;	// not used in drvcommon_v0
int			DiaMaxReqInuse;	// not used in drvcommon_v0
int			DiaNbReqPending;
int			DiaMaxReqPending;
int			DiaConsecutiveTimeout;	// == DiaDisconnected

unsigned	int	DiaSendCount;
unsigned	int	DiaRtryCount;
unsigned	int	DiaRecvCount;
unsigned	int	DiaTimeCount;


int	DiaDisconnected()
{
	return	0;
//	return	DiaConsecutiveTimeout > 3;	// TODO
}

int	DiaFull75()
{
	if	((float)DiaNbReqPending / (float)MAX_REQUEST_DIA >= 0.75)
		return	1;
	return	0;
}



int WDiaCreateRequest (char *reqname, char *reqEntity, char *targetID, void *ctt, int len, char *cttType, void *optAttr, void *optHeader, int *tid)
{
	int	ret;
	ret =	diaCreateRequest(DiaCtxt,reqEntity,targetID,ctt,len,cttType,
		optAttr,optHeader,tid);

	if	(ret >= 0 && DiaTraceRequest)
		WDiaTraceRequest(CLT_SIDE_DIA,*tid,"",reqname,reqEntity,targetID,
				ctt,len,cttType);
	return	ret;
}

int WDiaRetrieveRequest(char *reqname, char *reqEntity, char *targetID,
	void *optAttr, void *optHeader, int *tid)
{
	int	ret;

	ret =	diaRetrieveRequest(DiaCtxt,reqEntity,targetID,
		optAttr,optHeader,tid);

	if	(ret >=0 && DiaTraceRequest)
		WDiaTraceRequest(CLT_SIDE_DIA,*tid,"",reqname,reqEntity,targetID,
				NULL,0,"");
	return	ret;
}


void	WDiaAddRequest(t_dia_req *req)
{
	int	i;
	t_dia_req	*r;

	for	(i = 0 ; i < MAX_REQUEST_DIA ; i++)
	{
		if	(TbReq[i].rq_waitRsp == 0)
		{
			r	= &TbReq[i];
			memcpy	(r,req,sizeof(t_dia_req));

			r->rq_waitRsp	= 1;
			r->rq_sendAt	= time(NULL);
			r->rq_waitMax	= (r->rq_retry+1)*REQUEST_TIMEOUT_DIA;
RTL_TRDBG(5,"DIA request allocated tid=%d num=%d\n",r->rq_tid,i);
			DiaNbReqPending++;
			if	(DiaNbReqPending >= DiaMaxReqPending)
				DiaMaxReqPending	= DiaNbReqPending;
			return;
		}
	}

	RTL_TRDBG(0,"ERROR DIA MAX REQUEST\n");
}

// a clever search is not necessary here :
// - requests are serialized (fifo)
// - the probability that a searched tid is not present is quiet nul
static	t_dia_req *FindRequest(int tid)
{
	int	i;

	for	(i = 0 ; i < MAX_REQUEST_DIA ; i++)
	{
		if	(TbReq[i].rq_waitRsp)
		{
			if	(TbReq[i].rq_tid == tid)
			{
				TbReq[i].rq_waitRsp	= 0;
				DiaNbReqPending--;
				return	&TbReq[i];
			}
		}
	}

	RTL_TRDBG(2,"ERROR DIA no request found for tid=%d\n",tid);
	return	NULL;
}

t_dia_req	*DiaFindRequestByDevSerial(int dev,int serial)
{
	int	i;

	for	(i = 0 ; i < MAX_REQUEST_DIA ; i++)
	{
		if	(TbReq[i].rq_waitRsp)
		{
			if	(TbReq[i].rq_dev == dev 
						&& TbReq[i].rq_serial == serial)
			{
				return	&TbReq[i];
			}
		}
	}
	return	NULL;
}

//
//
//
//	callbacks for response
//
//
//
static	void cb_diaRetrieveResponse(dia_context_t *dia, char *scode, void *ctt, int len, char *cttType, Opt *optHeader, int tid) 
{
	t_dia_req	*req;
	t_dia_rspparam	par;

	DiaConsecutiveTimeout	= 0;
	DiaRecvCount++;
	RTL_TRDBG(2,"cb_diaRetrieveResponse tid=%d ret='%s'\n",tid,scode);
	req	= FindRequest(tid);
	if	(DiaTraceRequest)
		WDiaTraceResponse(CLT_SIDE_DIA,tid,"",req?"":"correlation error",
			scode,"",ctt,len,cttType);
	if	(!req)
		return;

	memset	(&par,0,sizeof(t_dia_rspparam));
	par.pr_dia	= dia;
	par.pr_scode	= scode;
	par.pr_ctt	= ctt;
	par.pr_len	= len;
	par.pr_cttType	= cttType;
	par.pr_tid	= tid;
	par.pr_error	= 0;
	if	(req->rq_cb)
		(*req->rq_cb)(req,&par);
}

static	void cb_diaCreateResponse(dia_context_t *dia, char *scode, char *resourceURI, void *ctt, int len, char* cttType,
	Opt *optHeader, int tid) 
{
	t_dia_req	*req;
	t_dia_rspparam	par;

	DiaConsecutiveTimeout	= 0;
	DiaRecvCount++;
	RTL_TRDBG(2,"cb_diaCreateResponse tid=%d ret='%s'\n",tid,scode);
	req	= FindRequest(tid);
	if	(DiaTraceRequest)
		WDiaTraceResponse(CLT_SIDE_DIA,tid,"",req?"":"correlation error",
			scode,"",ctt,len,cttType);
	if	(!req)
		return;

	memset	(&par,0,sizeof(t_dia_rspparam));
	par.pr_dia	= dia;
	par.pr_scode	= scode;
	par.pr_ctt	= ctt;
	par.pr_len	= len;
	par.pr_cttType	= cttType;
	par.pr_tid	= tid;
	par.pr_error	= 0;
	if	(req->rq_cb)
		(*req->rq_cb)(req,&par);
}

static	void cb_diaUpdateResponse(dia_context_t *dia, char *scode, void *ctt, int len, char* cttType, Opt *optHeader, int tid) 
{
	t_dia_req	*req;
	t_dia_rspparam	par;

	DiaConsecutiveTimeout	= 0;
	DiaRecvCount++;
	RTL_TRDBG(2,"cb_diaUpdateResponse tid=%d ret='%s'\n",tid,scode);
	req	= FindRequest(tid);
	if	(DiaTraceRequest)
		WDiaTraceResponse(CLT_SIDE_DIA,tid,"",req?"":"correlation error",
			scode,"",ctt,len,cttType);
	if	(!req)
		return;
	memset	(&par,0,sizeof(t_dia_rspparam));
	par.pr_dia	= dia;
	par.pr_scode	= scode;
	par.pr_ctt	= ctt;
	par.pr_len	= len;
	par.pr_cttType	= cttType;
	par.pr_tid	= tid;
	par.pr_error	= 0;
	if	(req->rq_cb)
		(*req->rq_cb)(req,&par);

}

static	void cb_diaDeleteResponse(dia_context_t *dia, char *scode, Opt *optHeader, int tid) 
{
	t_dia_req	*req;
	t_dia_rspparam	par;

	DiaConsecutiveTimeout	= 0;
	DiaRecvCount++;
	RTL_TRDBG(2,"cb_diaDeleteResponse tid=%d ret='%s'\n",tid,scode);
	req	= FindRequest(tid);
	if	(DiaTraceRequest)
		WDiaTraceResponse(CLT_SIDE_DIA,tid,"",req?"":"correlation error",
			scode,"",NULL,0,"");
	if	(!req)
		return;
	memset	(&par,0,sizeof(t_dia_rspparam));
	par.pr_dia	= dia;
	par.pr_scode	= scode;
	par.pr_ctt	= NULL;
	par.pr_len	= 0;
	par.pr_cttType	= "";
	par.pr_tid	= tid;
	par.pr_error	= 0;
	if	(req->rq_cb)
		(*req->rq_cb)(req,&par);
}

static	void cb_diaErrorResponse(dia_context_t *dia, void *ctt, int len, char* cttType, Opt *optHeader, int tid) 
{
	t_dia_req	*req;
	t_dia_rspparam	par;

	RTL_TRDBG(2,"cb_diaErrorResponse tid=%d\n",tid);
	req	= FindRequest(tid);
	if	(DiaTraceRequest)
		WDiaTraceResponse(CLT_SIDE_DIA,tid,"",req?"":"correlation error",
			"error","",ctt,len,cttType);
	if	(!req)
		return;
	memset	(&par,0,sizeof(t_dia_rspparam));
	par.pr_dia	= dia;
	par.pr_scode	= "INTERNAL_ERROR_DIA";
	par.pr_ctt	= ctt;
	par.pr_len	= len;
	par.pr_cttType	= cttType;
	par.pr_tid	= tid;
	par.pr_error	= 1;
	if	(req->rq_cb)
		(*req->rq_cb)(req,&par);
}

void	DiaFlushRequest()
{
}

/*!
 *
 * @brief clock for dia/udp client link, may be called by main loop each 100ms
 * @param now current time in second (time(2))
 * @return void
 * 
 */
void	DiaClientClockMs(time_t now)
{
	DiaFlushRequest();
}

/*!
 *
 * @brief clock for dia/udp client link, must be called by main loop each second
 * 	- insert a timeout response for non answered requests
 * @param now current time in second (time(2))
 * @return void
 * 
 */
void	DiaClientClockSc(time_t now)
{
	int	i;
	t_dia_req	*req;
	t_dia_rspparam	par;

	for	(i = 0 ; i < MAX_REQUEST_DIA ; i++)
	{
		if	(TbReq[i].rq_waitRsp == 0)	continue;
		req	= &TbReq[i];
		if	( ABS(now - req->rq_sendAt) < req->rq_waitMax)	
			continue;
		req->rq_waitRsp	= 0;
		DiaNbReqPending--;
		DiaTimeCount++;
		DiaConsecutiveTimeout++;
		memset	(&par,0,sizeof(t_dia_rspparam));
		par.pr_dia	= NULL;		// TODO
		par.pr_scode	= "INTERNAL_TIMEOUT_DIA";
		par.pr_ctt	= NULL;
		par.pr_len	= 0;
		par.pr_cttType	= "";
		par.pr_tid	= req->rq_tid;
		par.pr_error	= 1;
		par.pr_timeout	= 1;
		RTL_TRDBG(2,"Pseudo cb_diaTimeoutResponse tid=%d\n",req->rq_tid);
		if	(DiaTraceRequest)
			WDiaTraceResponse(CLT_SIDE_DIA,req->rq_tid,"",
				"internal timeout","","",NULL,0,"");
		if	(req->rq_cb)
			(*req->rq_cb)(req,&par);
	}
}

/*!
 *
 * @brief initialize dia/udp client side module
 * @param ptbcb a table of dia call backs for responses
 * @return void
 * 
 */
void	DiaClientInit(void *ptbcb)
{
	dia_callbacks_t	*tbcb	= (dia_callbacks_t *)ptbcb;

	tbcb->cb_diaRetrieveResponse	= cb_diaRetrieveResponse;
	tbcb->cb_diaCreateResponse	= cb_diaCreateResponse;
	tbcb->cb_diaUpdateResponse	= cb_diaUpdateResponse;
	tbcb->cb_diaDeleteResponse	= cb_diaDeleteResponse;
	tbcb->cb_diaErrorResponse	= cb_diaErrorResponse;
/*
static	dia_callbacks_t callbacks = {
	cb_diaRetrieveRequest,
	cb_diaRetrieveResponse,
	cb_diaCreateRequest,
	cb_diaCreateResponse,
	cb_diaUpdateRequest,
	cb_diaUpdateResponse,
	cb_diaDeleteRequest,
	cb_diaDeleteResponse,
	cb_diaErrorResponse
};
*/
}


