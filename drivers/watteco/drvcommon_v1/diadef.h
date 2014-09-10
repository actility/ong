
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

#ifndef _DIADEF_H_ 
#define _DIADEF_H_ 

int	WDiaUCBPreLoadTemplate(char *reqname,t_dia_req *preq,char *target,
		t_cmn_sensor *cmn,void **xo,char *templatename);
int	WDiaUCBPostLoadTemplate(char *reqname,t_dia_req *preq,char *target,
		t_cmn_sensor *cmn,void *xo);
int	WDiaUCBRequestOk(char *reqname,t_dia_req *preq,t_cmn_sensor *cmn);
int	WDiaUCBRequestTimeout(char *reqname,t_dia_req *preq,t_cmn_sensor *cmn);
int	WDiaUCBRequestError(char *reqname,t_dia_req *preq,t_cmn_sensor *cmn);

#define	DIA_FUNC	(char *)__func__
#define	DIA_LEAK	__attribute__ ((weak))

/***********************************************************************/
/***********************************************************************/
#define	DIA_LOCAL_VAR()							\
	int	ret;							\
	int	tid;							\
	void	*xo		= NULL;					\
	char	*buf;							\
	char	*ctype;							\
	int	parseflags	= 0;					\
	int	serialtype	= XO_FMT_STD_XML;			\
	int	sz;							\
	char	target[256];						\
	char	*reqname	= (char *)__func__;			\
	char	*scode;							\
	char	tmp[256];						\
	int	updateelem	= ((preq->rq_flags&RQT_UPDATE_ELEM) ==	\
				RQT_UPDATE_ELEM);			\
	int	updatecont	= ((preq->rq_flags&RQT_UPDATE_CONT) ==	\
				RQT_UPDATE_CONT);
/***********************************************************************/
#define	DIA_DEVICE_CONTEXT()						\
	t_cmn_sensor	*cmn	= NULL;					\
	char		*xxxid	= NULL;					\
	if	(preq->rq_serial)					\
	{								\
	if	(!preq->rq_cbserial)					\
	cmn	= CmnSensorNumSerial(preq->rq_dev,preq->rq_serial);	\
	else								\
	cmn	= preq->rq_cbserial(preq->rq_dev,preq->rq_serial);	\
	if	(!cmn)							\
	{								\
	RTL_TRDBG(0,"ERROR CTXT dia request '%s' tid=%d\n",		\
				preq->rq_name,preq->rq_tid);		\
	return;								\
	}								\
	xxxid	= cmn->cmn_ieee;					\
	RTL_TRDBG(5,"DIA CTX DEV dev=%d app=%d xxxid='%s'\n",		\
			preq->rq_dev,preq->rq_app,xxxid);		\
	}
/***********************************************************************/
/***********************************************************************/
#define	DIA_CREATE_REQUEST(_cbfct)					\
if	(!DiaCtxt)							\
	return;								\
if	(preq->rq_waitRsp == NUSE_REQUEST_DIA)				\
{									\
	t_dia_req	req;						\
	memcpy	(&req,preq,sizeof(t_dia_req));				\
	req.rq_tid	= -1;						\
	req.rq_name	= reqname;					\
	req.rq_cb	= _cbfct;					\
	buf	= WXoSerializeFree(xo,serialtype,parseflags,&sz,&ctype);\
	if	(!buf)							\
		return;							\
	req.rq_buf	= buf;						\
	req.rq_sz	= sz;						\
	req.rq_cttType	= ctype;					\
	req.rq_targetId	= target;					\
	if	(WDiaInitRequest(&req) == NULL)				\
		free(buf);						\
	return;								\
}									\
if	(updateelem || updatecont)					\
{									\
RTL_TRDBG(2,"force UPDATE '%s' elem=%d cont=%d\n",reqname,		\
						updateelem,updatecont);	\
ret	= WDiaUpdateRequest(reqname,GetReqId(),target,			\
			preq->rq_buf,preq->rq_sz,preq->rq_cttType,	\
			NULL,NULL,&tid);				\
}									\
else									\
{									\
ret	= WDiaCreateRequest(reqname,GetReqId(),target,			\
			preq->rq_buf,preq->rq_sz,preq->rq_cttType,	\
			NULL,NULL,&tid);				\
}									\
if	(preq->rq_buf)							\
	free(preq->rq_buf);						\
preq->rq_buf	= NULL;							\
preq->rq_sz	= 0;							\
if	(ret < 0)							\
{									\
	RTL_TRDBG(0,"ERROR dia create '%s' ret=%d tid=%d\n",		\
						reqname,ret,tid);	\
	return;								\
}									\
DiaSendCount++;								\
RTL_TRDBG(2,"send dia requestC '%s' ret=%d tid=%d sz=%d\n",		\
					reqname,ret,tid,preq->rq_sz);	\
									\
preq->rq_tid	= tid;							\
WDiaAddRequest(preq);

/***********************************************************************/
#define	DIA_CREATE_RESPONSE(_fcterr,_fctnok,_fctok)			\
tid	= preq->rq_tid;							\
ret	= par->pr_error;						\
scode	= par->pr_scode;						\
if	(ret >= 1 || par->pr_timeout)					\
{									\
	if	(par->pr_timeout && (preq->rq_retry)++ < MAX_RETRY_DIA)	\
	{								\
		DiaRtryCount++;						\
		RTL_TRDBG(0,"RETRY dia create '%s' ret=%d tid=%d %d\n",	\
				reqname,ret,tid,preq->rq_retry);	\
		goto	retry;						\
	}								\
	RTL_TRDBG(2,"ERROR dia create '%s' ret=%d tid=%d code='%s'\n",\
						reqname,ret,tid,scode);	\
	_fcterr(preq,par);						\
	return;								\
}									\
preq->rq_scode	= scode;						\
if	(WDiaCodeOk(scode) == NOK_DIA)					\
{									\
	RTL_TRDBG(2,"NOK dia response '%s' ret=%d tid=%d code='%s'\n",	\
						reqname,ret,tid,scode);	\
	_fctnok(preq,NULL);						\
	return;								\
}									\
RTL_TRDBG(2,"recv dia responseC '%s' ret=%d tid=%d code='%s'\n",	\
					reqname,ret,tid,scode);		\
_fctok(preq,NULL);

/***********************************************************************/
/***********************************************************************/
#define	DIA_RETRIEVE_REQUEST(_cbfct)					\
if	(!DiaCtxt)							\
	return;								\
if	(preq->rq_waitRsp == NUSE_REQUEST_DIA)				\
{									\
	t_dia_req	req;						\
	memcpy	(&req,preq,sizeof(t_dia_req));				\
	req.rq_tid	= -1;						\
	req.rq_name	= reqname;					\
	req.rq_cb	= _cbfct;					\
	WDiaInitRequest(&req);						\
	return;								\
}									\
ret	= WDiaRetrieveRequest(reqname,GetReqId(),target,		\
						NULL,NULL,&tid);	\
if	(ret < 0)							\
{									\
	RTL_TRDBG(0,"ERROR dia retrieve '%s' ret=%d tid=%d\n",		\
						reqname,ret,tid);	\
	return;								\
}									\
DiaSendCount++;								\
RTL_TRDBG(2,"send dia requestR '%s' ret=%d tid=%d\n",reqname,ret,tid);	\
									\
preq->rq_tid	= tid;							\
WDiaAddRequest(preq);


/***********************************************************************/
#define	DIA_RETRIEVE_RESPONSE(_fcterr,_fctnok,_fctok)			\
tid	= preq->rq_tid;							\
ret	= par->pr_error;						\
scode	= par->pr_scode;						\
sz	= par->pr_len;							\
buf	= par->pr_ctt;							\
ctype	= par->pr_cttType;						\
xo	= NULL;								\
preq->rq_par	= par;							\
if	(ret >= 1 || par->pr_timeout)					\
{									\
	if	(par->pr_timeout && (preq->rq_retry)++ < MAX_RETRY_DIA)	\
	{								\
		DiaRtryCount++;						\
		RTL_TRDBG(0,"RETRY dia retrieve '%s' ret=%d tid=%d %d\n",\
				reqname,ret,tid,preq->rq_retry);	\
		goto	retry;						\
	}								\
	RTL_TRDBG(2,"ERROR dia retrieve '%s' ret=%d tid=%d code='%s'\n",\
						reqname,ret,tid,scode);	\
	_fcterr(preq,par);						\
	return;								\
}									\
preq->rq_scode	= scode;						\
if	(sz <= 0 || !buf || !*buf || WDiaCodeOk(scode) == NOK_DIA)	\
{									\
RTL_TRDBG(2,"NOK dia response '%s' ret=%d tid=%d code='%s' sz=%d\n",	\
					reqname,ret,tid,scode,sz);	\
	_fctnok(preq,NULL);						\
	return;								\
}									\
RTL_TRDBG(2,"recv dia responseR '%s' ret=%d tid=%d code='%s' sz=%d\n",	\
				reqname,ret,tid,scode,sz);		\
if	(preq->rq_cli && serialtype == XO_FMT_STD_XML)			\
	WAdmDumpRequest(preq->rq_cli,reqname,GetReqId(),target,buf,sz);	\
_fctok(preq,NULL);
/***********************************************************************/
/***********************************************************************/
#define	DIA_DELETE_REQUEST(_cbfct)					\
if	(!DiaCtxt)							\
	return;								\
if	(preq->rq_waitRsp == NUSE_REQUEST_DIA)				\
{									\
	t_dia_req	req;						\
	memcpy	(&req,preq,sizeof(t_dia_req));				\
	req.rq_tid	= -1;						\
	req.rq_name	= reqname;					\
	req.rq_cb	= _cbfct;					\
	WDiaInitRequest(&req);						\
	return;								\
}									\
ret	= WDiaDeleteRequest(reqname,GetReqId(),target,			\
						NULL,NULL,&tid);	\
if	(ret < 0)							\
{									\
	RTL_TRDBG(0,"ERROR dia delete '%s' ret=%d tid=%d\n",		\
						reqname,ret,tid);	\
	return;								\
}									\
DiaSendCount++;								\
RTL_TRDBG(2,"send dia requestD '%s' ret=%d tid=%d\n",reqname,ret,tid);	\
									\
preq->rq_tid	= tid;							\
WDiaAddRequest(preq);


/***********************************************************************/
#define	DIA_DELETE_RESPONSE(_fcterr,_fctnok,_fctok)			\
tid	= preq->rq_tid;							\
ret	= par->pr_error;						\
scode	= par->pr_scode;						\
sz	= par->pr_len;							\
buf	= par->pr_ctt;							\
ctype	= par->pr_cttType;						\
xo	= NULL;								\
preq->rq_par	= par;							\
if	(ret >= 1 || par->pr_timeout)					\
{									\
	if	(par->pr_timeout && (preq->rq_retry)++ < MAX_RETRY_DIA)	\
	{								\
		DiaRtryCount++;						\
		RTL_TRDBG(0,"RETRY dia delete '%s' ret=%d tid=%d %d\n",	\
				reqname,ret,tid,preq->rq_retry);	\
		goto	retry;						\
	}								\
	RTL_TRDBG(2,"ERROR dia delete '%s' ret=%d tid=%d code='%s'\n",	\
						reqname,ret,tid,scode);	\
	_fcterr(preq,par);						\
	return;								\
}									\
preq->rq_scode	= scode;						\
if	(WDiaCodeOk(scode) == NOK_DIA)					\
{									\
RTL_TRDBG(2,"NOK dia response '%s' ret=%d tid=%d code='%s' sz=%d\n",	\
					reqname,ret,tid,scode,sz);	\
	_fctnok(preq,NULL);						\
	return;								\
}									\
RTL_TRDBG(2,"recv dia responseD '%s' ret=%d tid=%d code='%s' sz=%d\n",	\
				reqname,ret,tid,scode,sz);		\
_fctok(preq,NULL);
/***********************************************************************/
/***********************************************************************/


#endif
