
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

/*! @file diaserver.c
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

#include "diadef.h"

extern	dia_context_t	*DiaCtxt;
/*ML-20121001-retargeting-response+*/
static  t_dia_in_req TbIncomingReq[MAX_REQUEST_DIA];


int
DIA_LEAK
IpuExecuteMethod(char *ident,int targetlevel,void *obix,char *targetid, int tid)
{
    return -50;
}

static  t_dia_in_req *DiaIncomingExistRequest(int tid)
{
  int i;

  for (i = 0 ; i < MAX_REQUEST_DIA ; i++)
  {
    if  (TbIncomingReq[i].rq_waitRsp)
    {
      if  (TbIncomingReq[i].rq_tid == tid)
      {
  RTL_TRDBG(2,"ERROR DIA INCOMING already exists tid=%d\n",tid);
        return  &TbIncomingReq[i];
      }
    }
  }

  return  NULL;
}

/**
 * @brief Collect and store the request elements mandatory to build the response later.
 * @param tid the dIa request identifier.
 * @param reqType dIa request type (i.e. DIA_CREATE, DIA_RETRIEVE, DIA_UPDATE or DIA_DELETE)
 * @param peer the issuer that sent this request.
 * @param dia the dIa layer request context.
 * @param targetID the request URI (mandatory to send the CREATE response back).
 */
static void DiaIncomingAddRequest(int tid, DIA_REQ_TYPE reqType, char *peer, 
  dia_context_t *dia, char * targetID)
{
  int i;
  t_dia_in_req *r;

  if (DiaIncomingExistRequest(tid))
    return;
  for (i = 0 ; i < MAX_REQUEST_DIA ; i++)
  {
    if  (TbIncomingReq[i].rq_waitRsp == 0)
    {
      r = &TbIncomingReq[i];
      r->rq_tid = tid;
      r->reqType = reqType;
/*ML-fixes-20121012+*/
//      r->peer = peer;
      r->peer = NULL;
      if (peer && *peer)
      {
        r->peer = strdup(peer);
      }
/*ML-fixes-20121012-*/
      r->dia = dia;
      r->targetID = NULL;
      if (targetID && *targetID)
      {
/*ML-fixes-20121012+*/
//        r->targetID = malloc(strlen(targetID));
//        memcpy(r->targetID, targetID, strlen(targetID));
        r->targetID = strdup(targetID);
/*ML-fixes-20121012-*/
      }
      r->rq_waitRsp = 1;
      r->rq_sendAt  = time(NULL);
RTL_TRDBG(2,"DIA INCOMING request allocated tid=%d num=%d\n",r->rq_tid,i);
      return;
    }
  }

  RTL_TRDBG(0,"ERROR DIA INCOMING MAX REQUEST\n");
}

// a clever search is not necessary here :
// - requests are serialized (fifo)
// - the probability that a searched tid is not present is quiet nul
static  t_dia_in_req *DiaIncomingFindRequest(int tid)
{
  int i;

  for (i = 0 ; i < MAX_REQUEST_DIA ; i++)
  {
    if  (TbIncomingReq[i].rq_waitRsp)
    {
      if  (TbIncomingReq[i].rq_tid == tid)
      {
        TbIncomingReq[i].rq_waitRsp = 0;
        return  &TbIncomingReq[i];
      }
    }
  }

  RTL_TRDBG(2,"ERROR DIA INCOMING no request found for tid=%d\n",tid);
  return  NULL;
}
/*ML-20121001-retargeting-response-*/


//
//
//
//	callbacks for request
//
//
//	copper : GET
static	int cb_diaRetrieveRequest(dia_context_t *dia, char *reqEntity, char *targetID,
	Opt *optAttr, Opt *optHeader, int tid, char *peer)
{
	int	ret;
	char	ident[128];
	int	targetlevel;
	int	cluster;
	int	numapp;
	int	numm;
	int	type;
	char	*retcode	= "STATUS_OK";
	t_cmn_sensor	*cmn;

	RTL_TRDBG(2,"cb_diaRetrieveRequest tid=%d(%s) req='%s' tar='%s'\n",
					tid,peer,reqEntity,reqEntity);

	// TODO retargeting attributes
	//
/*ML-20121001-retargeting-response+*/
	ret	= WDiaParseMethod(targetID,ident,&numapp,&type,
		&targetlevel,&cluster,&numm);
	if	(ret < 0)
	{
		RTL_TRDBG(0,"cb_diaRetrieveRequest - ERROR ParseMethod('%s') ret=%d\n",targetID,ret);
		goto	endfct;
	}
	if	(ret >= 0 && DiaTraceRequest)
		WDiaTraceRequest(SRV_SIDE_DIA,tid,peer,"",reqEntity,targetID,
				NULL,0,"");

	RTL_TRDBG(2,"cb_diaRetrieveRequest - ParseMethod('%s') ident='%s' type=%d app=%d "
    "tgtlev=%d cluster=%04x method=%d\n", targetID,ident,type,numapp,targetlevel,
    cluster,numm);

	switch(type)
	{
	case	METHOD_IPU :
	break;
	case	METHOD_NET :
	break;
	case	METHOD_DEV :
		numapp	= -1;	// not an app
	case	METHOD_APP :
		cmn	= CmnSensorFindByIeee(ident);
		if	(!cmn)
		{
			RTL_TRDBG(0,"cb_diaRetrieveRequest - ERROR ParseMethod('%s') ident not found\n", targetID);
			ret	= -1;
			goto	endfct;
		}
		ret	= iCmnSensorRetrieveAttrValue(cmn, numapp, cluster, numm,
				targetlevel, targetID, tid);
	break;
	case	METHOD_DRV :
		ret	= 0;
		retcode	= "STATUS_OK";
		goto	respnow;
	break;
	default :
		RTL_TRDBG(0,"cb_diaRetrieveRequest - ERROR ParseMethod('%s') bad type=%d\n",targetID,
					type);
		ret	= -1;
		goto	endfct;
	break;
	}


endfct :
	RTL_TRDBG(2,"Retrieve attribute value('%s') ret=%d\n",targetID,ret);

  if (ret > 0)
  {
    // asynchronous response expected.
    DiaIncomingAddRequest(tid, DIA_RETRIEVE, peer, dia, targetID);
  }
  else if (ret <= -100)
  {
    // failed to send the request, or timeout (?)
    retcode = "STATUS_INTERNAL_SERVER_ERROR"; 
  }
  else if (ret <= -50)
  {
    // Wrong content
    retcode = "STATUS_BAD_REQUEST";
  }
  else // if ((ret < 0) && (ret > -50))
  {
    // Wrong request URI
    retcode = "STATUS_NOT_FOUND";
  }

respnow :
  if (ret <= 0)
  {
  	diaRetrieveResponse(dia,retcode,NULL,0,"",NULL,tid,peer);

  	if	(DiaTraceRequest)
	  	WDiaTraceResponse(SRV_SIDE_DIA,tid,peer,"",retcode,"",NULL,0,"");
  }
/*ML-20121001-retargeting-response-*/

	return	0;
}


//	copper : POST
static	int cb_diaCreateRequest(dia_context_t *dia, char *reqEntity, char *targetID, void *ctt, int len, char *cttType,
	Opt *optAttr, Opt *optHeader, int tid,char *peer)
{
	int	ret;
	char	ident[128];
	int	targetlevel;
	int	cluster;
	int	numapp;
	int	numm;
	int	type;
	char	*retcode	= "STATUS_OK";
	char	*obix	= NULL;

	t_cmn_sensor	*cmn;

RTL_TRDBG(2,"cb_diaCreateRequest tid=%d(%s) req='%s' tar='%s' type='%s' o=%p\n",
		tid,peer,reqEntity,targetID,cttType,ctt);

	// retargeting methods
	//
	ret	= WDiaParseMethod(targetID,ident,&numapp,&type,
		&targetlevel,&cluster,&numm);
	if	(ret < 0)
	{
		RTL_TRDBG(0,"ERROR ParseMethod('%s') ret=%d\n",targetID,ret);
		goto	endfct;
	}
	if	(ret >= 0 && DiaTraceRequest)
		WDiaTraceRequest(SRV_SIDE_DIA,tid,peer,"",reqEntity,targetID,
				ctt,len,cttType);

	obix	= NULL;
	if	(len > 0 && ctt && !strcmp(cttType,"application/xml"))
	{
		// perhaps an obix parameter
		int	err;

		obix	= XoReadXmlMem(ctt,len,NULL,XOML_PARSE_OBIX,&err);
		if	(!obix)
		RTL_TRDBG(0,"ERROR cannot parse method param '%s' err=%d\n",
			targetID,err);
	}

	RTL_TRDBG(2,"ParseMethod('%s') ident='%s' type=%d app=%d tgtlev=%d cluster=%04x method=%d obix=%x\n",
		targetID,ident,type,numapp,targetlevel,cluster,numm,obix);

	switch(type)
	{
	case	METHOD_IPU :
		ret	= IpuExecuteMethod(ident,targetlevel,obix,targetID,tid);
	break;
	case	METHOD_NET :
/*ML-20121001-retargeting-response+*/
//		ret	= 
//		CmnNetworkExecuteMethod(ident,targetlevel,obix,targetID);
		ret	= CmnNetworkExecuteMethod(ident,targetlevel,obix,targetID,tid);
/*ML-20121001-retargeting-response-*/
	break;
	case	METHOD_DEV :
		numapp	= -1;	// not an app
	case	METHOD_APP :
		cmn	= CmnSensorFindByIeee(ident);
		if	(!cmn)
		{
/*ML-20121001-retargeting-response+*/
//			RTL_TRDBG(0,"ERROR ParseMethod('%s') ident not found\n");
			RTL_TRDBG(0,"ERROR ParseMethod('%s') ident not found\n", targetID);
/*ML-20121001-retargeting-response-*/
			ret	= -1;
			goto	endfct;
		}

/*ML-20121001-retargeting-response+*/
//		ret	= iCmnSensorExecuteMethod(cmn,numapp,cluster,numm,
//				targetlevel,obix,targetID);
		ret	= iCmnSensorExecuteMethod(cmn,numapp,cluster,numm,
				targetlevel,obix,targetID,tid);
/*ML-20121001-retargeting-response-*/
	break;
	default :
		RTL_TRDBG(0,"ERROR ParseMethod('%s') bad type=%d\n",targetID,
					type);
		ret	= -1;
		goto	endfct;
	break;
	}


endfct :
//	RTL_TRDBG(2,"ExecMethod('%s') ret=%d\n",targetID,ret);

	if	(obix)
	{
		XoFree(obix,1);
	}

	if	(!dia)	// call by admtcp
	{
		return	0;
	}

/*ML-20121001-retargeting-response+*/
//	if	(ret < 0)
//		retcode	= "STATUS_NOT_FOUND";
//	else
//		retcode	= "STATUS_CREATED";
//
//	diaCreateResponse(dia,retcode,targetID,NULL,0,"",NULL,tid,peer);
//
//	if	(DiaTraceRequest)
//		WDiaTraceResponse(SRV_SIDE_DIA,tid,peer,"",retcode,"",NULL,0,"");
  if (ret > 0)
  {
    // asynchronous response expected.
    DiaIncomingAddRequest(tid, DIA_CREATE, peer, dia, targetID);
  }
  else if (0 == ret)
  {
    // success, and no response expected from the device
    retcode = "STATUS_CREATED";
  }
  else if (ret == -503)
  {
    retcode = "STATUS_SERVICE_UNAVAILABLE";
  }
  else if (ret <= -100)
  {
    // failed to send the request, or timeout (?)
    retcode = "STATUS_INTERNAL_SERVER_ERROR"; 
  }
  else if (ret <= -50)
  {
    // Wrong content
    retcode = "STATUS_BAD_REQUEST";
  }
  else // if ((ret < 0) && (ret > -50))
  {
    // Wrong request URI
    retcode = "STATUS_NOT_FOUND";
  }

RTL_TRDBG(2,"ExecMethod('%s') ret=%d => retcode='%s'\n",targetID,ret,retcode);

  if (ret <= 0)
  {
  	diaCreateResponse(dia,retcode,targetID,NULL,0,"",NULL,tid,peer);

  	if	(DiaTraceRequest)
	  	WDiaTraceResponse(SRV_SIDE_DIA,tid,peer,"",retcode,"",NULL,0,"");
  }
/*ML-20121001-retargeting-response-*/

	return	0;
}

// call by admtcp to simulate a call method
void	CB_DIAcREATErEQUEST(char *reqEntity,char *target)
{
cb_diaCreateRequest(NULL, reqEntity, target, NULL, 0, NULL,NULL,NULL,0,NULL);
}

//	copper : PUT
static	int cb_diaUpdateRequest(dia_context_t *dia, char *reqEntity, char *targetID, void *ctt, int len, char *cttType,
	Opt *optAttr, Opt *optHeader, int tid,char *peer) 
{
	int	ret;
	char	ident[128];
	int	targetlevel;
	int	cluster;
	int	numapp;
	int	numm;
	int	type;
	char	*retcode;
	char	*obix	= NULL;
	t_cmn_sensor	*cmn;

	RTL_TRDBG(2,"cb_diaUpdateRequest tid=%d(%s) req='%s' tar='%s'\n",
					tid,peer,reqEntity,reqEntity);

/*ML-20121001-retargeting-response+*/
	// retargeting attributes - writing
	//
	ret	= WDiaParseMethod(targetID,ident,&numapp,&type,
		&targetlevel,&cluster,&numm);
	if	(ret < 0)
	{
		RTL_TRDBG(0,"cb_diaUpdateRequest - ERROR ParseMethod('%s') ret=%d\n",targetID,ret);
		goto	endfct;
	}
	if	(ret >= 0 && DiaTraceRequest)
		WDiaTraceRequest(SRV_SIDE_DIA,tid,peer,"",reqEntity,targetID,
				ctt,len,cttType);

	obix	= NULL;
	if	(len > 0 && ctt && !strcmp(cttType,"application/xml"))
	{
		// perhaps an obix parameter
		int	err;

		obix	= XoReadXmlMem(ctt,len,NULL,XOML_PARSE_OBIX,&err);
		if	(!obix)
		RTL_TRDBG(0,"cb_diaUpdateRequest - ERROR cannot parse method param '%s' err=%d\n",
			targetID,err);
	}

	RTL_TRDBG(2,"cb_diaUpdateRequest - ParseMethod('%s') ident='%s' type=%d app=%d "
    "tgtlev=%d cluster=%04x method=%d obix=%x\n", targetID,ident,type,numapp,
    targetlevel,cluster,numm,obix);

	switch(type)
	{
	case	METHOD_IPU :
	break;
	case	METHOD_NET :
	break;
	case	METHOD_DEV :
		numapp	= -1;	// not an app
	case	METHOD_APP :
		cmn	= CmnSensorFindByIeee(ident);
		if	(!cmn)
		{
			RTL_TRDBG(0,"cb_diaUpdateRequest - ERROR ParseMethod('%s') ident not found\n", targetID);
			ret	= -1;
			goto	endfct;
		}

		ret	= iCmnSensorUpdateAttrValue(cmn,numapp,cluster,numm,
				targetlevel,obix,targetID,tid);
	break;
	default :
		RTL_TRDBG(0,"cb_diaUpdateRequest - ERROR ParseMethod('%s') bad type=%d\n",targetID,
					type);
		ret	= -1;
		goto	endfct;
	break;
	}


endfct :
	RTL_TRDBG(2,"Update attribute value('%s') ret=%d\n",targetID,ret);

	if	(obix)
	{
		XoFree(obix,1);
	}

  if (ret > 0)
  {
    // asynchronous response expected.
    DiaIncomingAddRequest(tid, DIA_UPDATE, peer, dia, targetID);
  }
  else if (0 == ret)
  {
    // success, and no response expected from the device
    retcode = "STATUS_OK";
  }
  else if (ret <= -100)
  {
    // failed to send the request, or timeout (?)
    retcode = "STATUS_INTERNAL_SERVER_ERROR"; 
  }
  else if (ret <= -50)
  {
    // Wrong content
    retcode = "STATUS_BAD_REQUEST";
  }
  else // if ((ret < 0) && (ret > -50))
  {
    // Wrong request URI
    retcode = "STATUS_NOT_FOUND";
  }

  if (ret <= 0)
  {
  	diaUpdateResponse(dia,retcode,NULL,0,"",NULL,tid,peer);

  	if	(DiaTraceRequest)
	  	WDiaTraceResponse(SRV_SIDE_DIA,tid,peer,"",retcode,"",NULL,0,"");
  }
/*ML-20121001-retargeting-response-*/

	return	0;
}

static	int cb_diaDeleteRequest(dia_context_t *dia, char *reqEntity, char *targetID,
	Opt *optAttr, Opt *optHeader, int tid,char *peer) 
{
	RTL_TRDBG(2,"cb_diaDeleteRequest tid=%d(%s) req='%s' tar='%s'\n",
					tid,peer,reqEntity,reqEntity);
	return	0;
}

/*ML-20121001-retargeting-response+*/
/**
 * @brief Function that the driver should invoke to complete a dIa request processing.
 * @param tid the dIa request identifier.
 * @param ctt the content to encode in the dIa response.
 * @param len the size of the response content.
 * @param cttType the dIa response content type.
 * @param retcode the dIa response status code.
 */
void diaIncomingRequestCompletion(int tid, void *ctt, int len, char * cttType, char * retcode)
{
  t_dia_in_req * context = DiaIncomingFindRequest(tid);
  if (!context) 
  {
    RTL_TRDBG(0,"diaIncomingRequestCompletion ERROR - no matching request for tid=%d\n", tid);
    return;
  }
  
	RTL_TRDBG(2,"diaIncomingRequestCompletion tid=%d(%s) type='%d' tar='%s'\n",
  	tid, context->peer, context->reqType, context->targetID);
  switch(context->reqType)
  {
    case DIA_RETRIEVE:
    	diaRetrieveResponse(context->dia, retcode, ctt, len, cttType, NULL,
        context->rq_tid, context->peer);
      break;
    case DIA_CREATE:
    	diaCreateResponse(context->dia, retcode, context->targetID, ctt, len, cttType, NULL,
        context->rq_tid, context->peer);
      break;
    case DIA_UPDATE:
    	diaUpdateResponse(context->dia, retcode, ctt, len, cttType, NULL,
        context->rq_tid, context->peer);
      break;
    case DIA_DELETE:
    	diaDeleteResponse(context->dia, retcode, NULL, context->rq_tid, context->peer);
      break;
  }

	if	(DiaTraceRequest)
		WDiaTraceResponse(SRV_SIDE_DIA,context->rq_tid,context->peer,"",retcode,context->targetID,
      ctt, len, cttType);

  if (context->targetID) free (context->targetID);
  if (context->peer) free (context->peer);
}
/*ML-20121001-retargeting-response-*/

/*!
 *
 * @brief clock for dia/udp server link, must be called by main loop each second
 * @param now current time in second (time(2))
 * @return void
 * 
 */
void	DiaServerClockSc(time_t now)
{
	int	i;
	t_dia_in_req *r;

	for	(i = 0 ; i < MAX_REQUEST_DIA ; i++)
	{
		r	= &TbIncomingReq[i];
		if	(!r->rq_waitRsp)	continue;
		if	( ABS(now - r->rq_sendAt) < SRV_REQUEST_TIMEOUT_DIA)
			continue;
RTL_TRDBG(2,"Timeout on incoming request tid=%d\n",r->rq_tid);
		diaIncomingRequestCompletion(r->rq_tid,NULL,0,"",
			"STATUS_REQUEST_TIMEOUT");
	}
}


/*!
 *
 * @brief initialize dia/udp server side module
 * @param ptbcb a table of dia call backs for requests
 * @return void
 * 
 */
void	DiaServerInit(void *ptbcb)
{
	dia_callbacks_t	*tbcb	= (dia_callbacks_t *)ptbcb;

	tbcb->cb_diaRetrieveRequest	= cb_diaRetrieveRequest;
	tbcb->cb_diaCreateRequest	= cb_diaCreateRequest;
	tbcb->cb_diaUpdateRequest	= cb_diaUpdateRequest;
	tbcb->cb_diaDeleteRequest	= cb_diaDeleteRequest;
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
