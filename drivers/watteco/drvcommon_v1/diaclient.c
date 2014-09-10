
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
int			DiaNbReqInuse;
int			DiaMaxReqInuse;
int			DiaNbReqPending;
int			DiaMaxReqPending;
int			DiaConsecutiveTimeout;	// == DiaDisconnected

unsigned	int	DiaSendCount;
unsigned	int	DiaRtryCount;
unsigned	int	DiaRecvCount;
unsigned	int	DiaTimeCount;

#define		MAX_REQUEST_PLC	10
static	int	TbPlc[MAX_REQUEST_PLC];
static	unsigned	int	NbPlc;

unsigned	int	DiaRequestMode;

int	DiaDisconnected()
{
	return	0;
//	return	DiaConsecutiveTimeout > 3;	// TODO
}

int	DiaFull75()
{
	if	((float)DiaNbReqInuse / (float)MAX_REQUEST_DIA >= 0.75)
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

int WDiaUpdateRequest (char *reqname, char *reqEntity, char *targetID, void *ctt, int len, char *cttType, void *optAttr, void *optHeader, int *tid)
{
	int	ret;
	ret =	diaUpdateRequest(DiaCtxt,reqEntity,targetID,ctt,len,cttType,
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

int WDiaDeleteRequest(char *reqname, char *reqEntity, char *targetID,
	void *optAttr, void *optHeader, int *tid)
{
	int	ret;

	ret =	diaDeleteRequest(DiaCtxt,reqEntity,targetID,
		optAttr,optHeader,tid);

	if	(ret >=0 && DiaTraceRequest)
		WDiaTraceRequest(CLT_SIDE_DIA,*tid,"",reqname,reqEntity,targetID,
				NULL,0,"");
	return	ret;
}

static	t_dia_req	*_WDiaInitRequest(t_dia_req *req)
{
	int	i;
	t_dia_req	*r;

	// duplicate reports to plc/iec, just change the hostname of target uri
	// by the hostname of plc/iec
	if (req->rq_plcDup)
	{
		char	plcdup[256];
		int	ret;
		int	tid;
		char	*plchost;
		char	*proto	= "coap://";
		char	*target;

		plchost	= GetPlcTarget();
		if	(!plchost || !*plchost)
			goto	drop;
		target	= req->rq_targetId;
		if	(!target || !*target)
		{
RTL_TRDBG(0,"DIA PLC no targetid\n");
			goto	drop;
		}
		if	(strncmp(target,proto,strlen(proto)) != 0)
		{
RTL_TRDBG(0,"DIA PLC targetid proto != '%s' '%s'\n",proto,target);
			goto	drop;
		}
		target	= target + strlen(proto);
		while	(*target && *target != '/')
			target++;
		if	(*target != '/' || !*(target+1))
		{
RTL_TRDBG(0,"DIA PLC bad targetid '%s'\n",target);
			goto	drop;
		}
		sprintf	(plcdup,"%s%s",plchost,target);

		ret =	diaCreateRequest(DiaCtxt,GetReqId(),plcdup,
			req->rq_buf,req->rq_sz,req->rq_cttType,
			NULL,NULL,&tid);
		if	(ret >= 0)
		{
			TbPlc[NbPlc%MAX_REQUEST_PLC]	= tid;
RTL_TRDBG(3,"DIA PLC request allocated tid=%d slot=%d count=%u to='%s'\n",
				tid,NbPlc%MAX_REQUEST_PLC,NbPlc,plchost);
			NbPlc++;
		}
	}

drop :

	for	(i = 0 ; i < MAX_REQUEST_DIA ; i++)
	{
		if	(TbReq[i].rq_waitRsp == NUSE_REQUEST_DIA)
		{
			r	= &TbReq[i];
			if	(r->rq_buf)
				free(r->rq_buf);
			if	(r->rq_targetId)
				free(r->rq_targetId);
			req->rq_tid	= -1;
			req->rq_waitRsp	= INIT_REQUEST_DIA;
			req->rq_initAt	= time(NULL);
			memcpy	(r,req,sizeof(t_dia_req));
			if	(req->rq_targetId)
				r->rq_targetId	= strdup(req->rq_targetId);

RTL_TRDBG(3,"DIA request allocated req='%s' num=%d\n",r->rq_name,i);
			DiaNbReqInuse++;
			if	(DiaNbReqInuse >= DiaMaxReqInuse)
				DiaMaxReqInuse	= DiaNbReqInuse;
			return	r;
		}
	}

	RTL_TRDBG(0,"ERROR DIA MAX REQUEST\n");
	return	NULL;
}

static	t_dia_req *LastRequest;

t_dia_req	*WDiaInitRequest(t_dia_req *req)
{
	t_dia_req	*r;

	r	= _WDiaInitRequest(req);
	if	(r)
	{
		LastRequest	= r;
	}
	return	r;
}

void	DiaSetRequestMode(int flags)
{
	DiaRequestMode	= flags;
}


t_dia_req	*WDiaSendRequest()
{
	int	i;
	t_dia_req	*r = NULL;

	for	(i = 0 ; i < MAX_REQUEST_DIA ; i++)
	{
		if	(TbReq[i].rq_waitRsp == INIT_REQUEST_DIA)
		{
			r	= &TbReq[i];
RTL_TRDBG(3,"DIA request need to be sent req='%s' cb=%p pending=%d\n",
				r->rq_name,r->rq_cb,DiaNbReqPending);
			if	(DiaNbReqPending >= MAX_PENDING_DIA)
			{
RTL_TRDBG(6,"DIA max pending requests reached=%d\n",DiaNbReqPending);
				return	NULL;
			}
			if	(r->rq_cb)
				(*r->rq_cb)(r,NULL);
		}
	}

RTL_TRDBG(5,"DIA no more request to send\n");

	return	r;
}


void	WDiaAddRequest(t_dia_req *req)
{

	if	(req->rq_waitRsp != INIT_REQUEST_DIA)
	{
		RTL_TRDBG(0,"ERROR DIA BAD STATE=%d tid=%d\n",
			req->rq_waitRsp,req->rq_tid);
		return;
	}
	req->rq_waitRsp	= SENT_REQUEST_DIA;
	req->rq_sendAt	= time(NULL);
	req->rq_waitMax	= (req->rq_retry+1)*REQUEST_TIMEOUT_DIA;
RTL_TRDBG(5,"DIA request pending tid=%d\n",req->rq_tid);
	DiaNbReqPending++;
	if	(DiaNbReqPending >= DiaMaxReqPending)
		DiaMaxReqPending	= DiaNbReqPending;
}

// a clever search is not necessary here :
// - requests are serialized (fifo)
// - the probability that a searched tid is not present is quiet nul
static	t_dia_req *FindRequest(int tid)
{
	int	i;

	for	(i = 0 ; i < MAX_REQUEST_DIA ; i++)
	{
		if	(TbReq[i].rq_waitRsp == SENT_REQUEST_DIA)
		{
			if	(TbReq[i].rq_tid == tid)
			{
				TbReq[i].rq_waitRsp	= NUSE_REQUEST_DIA;
				DiaNbReqPending--;
				DiaNbReqInuse--;
				return	&TbReq[i];
			}
		}
	}

	// if PLC/IEC is set this can happen frequently
	for	(i = 0 ; i < MAX_REQUEST_PLC ; i++)
	{
		if	(TbPlc[i] == tid)
		{
RTL_TRDBG(3,"DIA PLC request retrieved tid=%d slot=%d count=%u\n",
				tid,i,NbPlc);
			return	NULL;
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
		if	(TbReq[i].rq_waitRsp != NUSE_REQUEST_DIA)
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

	WDiaSendRequest();
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

	WDiaSendRequest();
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

	WDiaSendRequest();
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

	WDiaSendRequest();
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

	WDiaSendRequest();
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
	char	*scode;
	t_dia_req	*req;
	t_dia_rspparam	par;

	for	(i = 0 ; i < MAX_REQUEST_DIA ; i++)
	{
		scode	= "INTERNAL_ERROR_DIA";
		req	= &TbReq[i];
		switch	(req->rq_waitRsp)
		{
		case	NUSE_REQUEST_DIA:
			continue;
		break;
		case	INIT_REQUEST_DIA:
			if	(ABS(now - req->rq_initAt) < INIT_TIMEOUT_DIA)	
				continue;
			scode	= "INTERNAL_TIMEOUT0_DIA";
		break;
		case	SENT_REQUEST_DIA:
			if	( ABS(now - req->rq_sendAt) < req->rq_waitMax)	
				continue;
			DiaNbReqPending--;
			DiaConsecutiveTimeout++;
			scode	= "INTERNAL_TIMEOUT1_DIA";
		break;
		default :
			req->rq_waitRsp	= NUSE_REQUEST_DIA;
			continue;
		break;
		}
		if	(req->rq_buf)
			free(req->rq_buf);
		req->rq_buf	= NULL;
		req->rq_waitRsp	= NUSE_REQUEST_DIA;
		DiaNbReqInuse--;
		DiaTimeCount++;
		memset	(&par,0,sizeof(t_dia_rspparam));
		par.pr_dia	= NULL;		// TODO
		par.pr_scode	= scode;
		par.pr_ctt	= NULL;
		par.pr_len	= 0;
		par.pr_cttType	= "";
		par.pr_tid	= req->rq_tid;
		par.pr_error	= 1;
		par.pr_timeout	= 1;
		par.pr_targetId	= req->rq_targetId;
		RTL_TRDBG(2,"Pseudo cb_diaTimeoutResponse tid=%d\n",req->rq_tid);
		if	(DiaTraceRequest)
			WDiaTraceResponse(CLT_SIDE_DIA,req->rq_tid,"",
				scode,"","",NULL,0,"");
		if	(req->rq_cb)
			(*req->rq_cb)(req,&par);
		if	(req->rq_targetId)
			free(req->rq_targetId);
		req->rq_targetId	= NULL;
	}

	WDiaSendRequest();
}

void	DiaFlushRequest()
{
	struct	timeval	tv;
	if	(DiaNbReqInuse > 0)
	{	// there are some requests
		if      (DiaNbReqPending < MAX_PENDING_DIA)
		{	// not too much in pending state
			WDiaSendRequest();
		}
	}

	if	(DiaCtxt)
	dia_nextTimer(DiaCtxt,&tv);
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
