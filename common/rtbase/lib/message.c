
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
#include <stdarg.h>
#include <stddef.h>
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
#include "rtllist.h"

#define	RTL_INTERNAL_IQMSG_STRUCT
#include "rtlimsg.h"

#if	0
typedef	struct
{
	t_imsg	iq_msglist;	// internal messages list
	t_imsg	iq_timerlist;	// internal timers list
	int	iq_msgcount;
	int	iq_timercount;
	int	iq_count;
	pthread_mutex_t	iq_CS;
	pthread_cond_t	iq_CD;
	char	iq_CSDone;
}	t_iqmsg;
#endif


static	unsigned int _MsgSerial;

static	unsigned int TimerResol	= 100;	// rtl_tmms() / 100

int	RtlTimerMonotonic	= 1;

static	void	TimeBaseVal(time_t *now,time_t *nowns)
{
	if	(RtlTimerMonotonic)
		return	rtl_timebasemonoVal(now,nowns);
	else
		return	rtl_timebaseVal(now,nowns);
}

void	rtl_timerResol(int res)
{
	switch	(res)
	{
	case	1 :			// ms	0.001
	case	10 :			// cs	0.01
	case	100 :			// ds	0.1
	case	1000 :			// s	1
		TimerResol	= res;	// rtl_tmms() / res
	break;
	default :
	break;
	}
}

static	void	InitCS(t_iqmsg *iq)
{
	int	err;
	if	( iq->iq_CSDone )
		return;
	if ((err=pthread_mutex_init(&iq->iq_CS,NULL)))
	{
		rtl_trace(0,"'%s':%d error=%d while initializing mutex !!!\n",
			__FILE__,__LINE__,err);
		return;
	}
	if ((err=pthread_cond_init(&iq->iq_CD,NULL)))
	{
		rtl_trace(0,"'%s':%d error=%d while initializing cond !!!\n",
			__FILE__,__LINE__,err);
		return;
	}
	iq->iq_CSDone	= 1;
}

static	void	*GetCS(t_iqmsg *iq)
{
	if (!iq->iq_CSDone)
		return	NULL;
	return	&iq->iq_CS;
}

static	void	EnterCS(t_iqmsg *iq)
{
	int	err;
	if (!iq->iq_CSDone)
	{
		rtl_trace(0,"'%s':%d error mutex not initialized !!!\n",
			__FILE__,__LINE__);
		return;
	}
	if ((err=pthread_mutex_lock(&iq->iq_CS)))
	{
		rtl_trace(0,"'%s':%d error=%d while locking mutex !!!\n",
			__FILE__,__LINE__,err);
	}

//	rtl_trace(0,"enterCS\n");

}

static	void	LeaveCS(t_iqmsg *iq)
{
	if (!iq->iq_CSDone)
		return;
	pthread_mutex_unlock(&iq->iq_CS);
//	rtl_trace(0,"leaveCS\n");
}


static	unsigned int	MsgSerial()
{
	_MsgSerial++;
	if (_MsgSerial == 0)
		_MsgSerial++;
	return	_MsgSerial;
}

int	rtl_imsgCount(void *piq)
{
	t_iqmsg	*iq = (t_iqmsg *)piq;

	if (!iq->iq_CSDone)
		return	-1;
	return	iq->iq_msgcount;
}

int	rtl_timerCount(void *piq)
{
	t_iqmsg	*iq = (t_iqmsg *)piq;

	if (!iq->iq_CSDone)
		return	-1;
	return	iq->iq_timercount;
}


int	rtl_imsgVerifCount(void *piq)
{
	t_iqmsg	*iq = (t_iqmsg *)piq;
	int	nb	= 0;
	t_imsg *msg;

	if (!iq->iq_CSDone)
		return	-1;
	list_for_each_entry(msg,&iq->iq_msglist.list,list) 
	{
		nb++;
	}
	return	nb;
}

int	rtl_timerVerifCount(void *piq)
{
	t_iqmsg	*iq = (t_iqmsg *)piq;
	int	nb	= 0;
	t_imsg *msg;

	if (!iq->iq_CSDone)
		return	-1;
	list_for_each_entry(msg,&iq->iq_timerlist.list,list) 
	{
		nb++;
	}
	return	nb;
}


t_imsg	*rtl_timerAlloc(int class,int type,time_t delta,void *to,uint32 serialto)
{
	t_imsg		*msg;
	time_t		now;
	time_t		nowns;

	TimeBaseVal(&now,&nowns);

	msg	= (t_imsg *)malloc(sizeof(t_imsg));
	if	( !msg )
		return	NULL;
	memset	(msg,0,sizeof(t_imsg));
//	msg->im_serial	= MsgSerial(); done in MsgAdd()

	msg->im_timer	= 1;
	msg->im_delayed	= 0;
	msg->im_class	= class;
	msg->im_type	= type;
	msg->im_from	= NULL;
	msg->im_to	= to;
	msg->im_serialto= serialto;

	msg->im_start	= now;
	msg->im_startns	= nowns;
	msg->im_end	= now + (delta/1000);
	msg->im_endns	= nowns + ((delta%1000)*1000000);
	if	(msg->im_endns >= 1000000000)
	{
		msg->im_end	+= 1;
		msg->im_endns	-= 1000000000;
	}
	msg->im_diffms	= delta;

	return	msg;
}

t_imsg	*rtl_imsgAlloc(int class,int type,void *to,uint32 serialto)
{
	t_imsg		*msg;
	time_t		now;
	time_t		nowns;

	TimeBaseVal(&now,&nowns);

	msg	= (t_imsg *)malloc(sizeof(t_imsg));
	if	( !msg )
		return	NULL;
	memset	(msg,0,sizeof(t_imsg));

	msg->im_timer	= 0;
	msg->im_delayed	= 0;
	msg->im_class	= class;
	msg->im_type	= type;
	msg->im_from	= NULL;
	msg->im_to	= to;
	msg->im_serialto= serialto;

	msg->im_start	= now;
	msg->im_startns	= nowns;
	msg->im_end	= now;
	msg->im_endns	= nowns;
	msg->im_diffms	= 0;

	return	msg;
}

t_imsg	*rtl_imsgSetData(t_imsg *msg,void *data)
{
	msg->im_datasz	= 0;	// not allocated data
	msg->im_dataptr	= data;
	return	msg;
}

t_imsg	*rtl_imsgCpyData(t_imsg *msg,void *data,int sz)
{
	if (sz < 0 || sz > sizeof(msg->im_u))
		return	NULL;
	msg->im_datasz	= 0;	// not allocated data
	msg->im_dataptr	= &msg->im_data[0];
	memcpy(msg->im_data,data,sz);
	return	msg;
}

t_imsg	*rtl_imsgDupData(t_imsg *msg,void *data,int sz)
{
	void	*pt;

	if (sz < 0)
		return	NULL;
	pt = (void *)malloc(sz);
	if (!pt)
		return	NULL;
	memcpy(pt,data,sz);
	msg->im_datasz	= sz;	// allocated data
	msg->im_dataptr	= pt;
	return	msg;
}

#define	CMP_TIME(m1,m2)\
	((m1)->im_end > (m2)->im_end ? +1000000000 :\
	((m1)->im_end < (m2)->im_end ? -1000000000 :\
			(m1)->im_endns - (m2)->im_endns))


static	int	__MsgAdd(t_iqmsg *iq,t_imsg *msg)
{
	t_imsg	*cur;
	t_imsg	*lst;
	int	svcount;

	if (!msg)
	{
		return	-1;
	}
	svcount	= iq->iq_count;
	msg->im_serial	= MsgSerial();
	if	(msg->im_timer || msg->im_delayed)
	{
		int	pos;

		lst	= &iq->iq_timerlist;
#if	0
		// do not sort if the last timer is less than the msg to add
		int	sort;
		sort	= 1;
		if	(!list_empty(&(lst->list)))
		{
			cur	= lst->list.prev;	// last
			if	(CMP_TIME(cur,msg) <= 0)
				sort	= 0;
		}
		else
			sort	= 0;
		if	(sort)
#endif
		// sort only if the last timer is greater than the msg to add
		if	(!list_empty(&(lst->list)) && (cur = lst->list.prev)
				&& CMP_TIME(cur,msg) > 0)
		{
			pos	= 0;
			list_for_each_entry(cur,&(lst->list),list) 
			{
				if	(CMP_TIME(cur,msg) > 0)
				{
//printf("add timer at pos=%d\n",pos);
					__list_add( &(msg->list), 
						cur->list.prev,
						&(cur->list) );
					iq->iq_timercount++;
					iq->iq_count++;
//					pthread_cond_broadcast(&iq->iq_CD);
					return	iq->iq_timercount;
				}
				pos++;
			}
		}
//printf("add timer at end\n");
		list_add_tail( &(msg->list), &(lst->list) );
		iq->iq_timercount++;
		iq->iq_count++;
//		pthread_cond_broadcast(&iq->iq_CD);
		return	iq->iq_timercount;
	}

//printf("add msg at end\n");
	lst	= &iq->iq_msglist;
	list_add_tail( &(msg->list), &(lst->list) );
	iq->iq_msgcount++;
	iq->iq_count++;
	pthread_cond_broadcast(&iq->iq_CD);
	return	iq->iq_msgcount;
}

int	rtl_imsgAdd(void *piq,t_imsg *msg)
{
	t_iqmsg *iq = (t_iqmsg *)piq;
	int	ret;

	if (!msg)
	{
		return	-1;
	}
	EnterCS(iq);
	ret	= __MsgAdd(iq,msg);
	LeaveCS(iq);
	return	ret;
}

int	rtl_imsgAddDelayed(void *piq,t_imsg *msg,time_t delta)
{
	t_iqmsg *iq = (t_iqmsg *)piq;
	int	ret;
	time_t		now;
	time_t		nowns;

	if (!msg)
	{
		return	-1;
	}

	TimeBaseVal(&now,&nowns);

	msg->im_delayed	= 1;
	msg->im_end	= now + (delta/1000);
	msg->im_endns	= nowns + ((delta%1000)*1000000);
	if	(msg->im_endns >= 1000000000)
	{
		msg->im_end	+= 1;
		msg->im_endns	-= 1000000000;
	}
	msg->im_diffms	= delta;

	EnterCS(iq);
	ret	= __MsgAdd(iq,msg);
	LeaveCS(iq);
	return	ret;
}

static	time_t	Anticip = 550;

static	t_imsg	*__MsgGet(t_iqmsg *iq,int mode)
{
	t_imsg	*msg;


	if (mode == IMSG_TIMER || mode == IMSG_BOTH)
	{
		t_imsg	*cur;
		t_imsg	mnow;
		time_t	diff;

		TimeBaseVal(&mnow.im_end,&mnow.im_endns);
		list_for_each_entry_safe(msg,cur,&iq->iq_timerlist.list,list) 
		{
			diff	= CMP_TIME(&mnow,msg);
			if	(diff >= -(Anticip*1000) /*|| diff >= 0*/)
			{
#if	0
printf("timer #### %d.%d %d.%d %d.%d\n",mnow.im_end,mnow.im_endns,
msg->im_start,msg->im_startns,msg->im_end,msg->im_endns);
#endif
				list_del(&msg->list);
				iq->iq_timercount--;
				iq->iq_count--;
				msg->im_diffms	= diff/1000000;
				if (msg->im_delayed == 0) 
				{	// normal timer
					return	msg;
				}
				// the delayed msg becomes a normal msg
				msg->im_timer	= 0;
				msg->im_delayed	= 0;
				__MsgAdd(iq,msg);
			}
		}
		if (mode == IMSG_TIMER)	// timer only
			return	NULL;
	}
	while (!list_empty(&iq->iq_msglist.list))
	{
		msg = list_entry(iq->iq_msglist.list.next,t_imsg,list);
		list_del(&msg->list);
		iq->iq_msgcount--;
		iq->iq_count--;
		return	msg;
	}

	return	NULL;
}

t_imsg	*rtl_imsgGet(void *piq,int mode)
{
	t_iqmsg *iq = (t_iqmsg *)piq;
	t_imsg	*ret;

	EnterCS(iq);
	ret	= __MsgGet(iq,mode);
	LeaveCS(iq);
	return	ret;
}

// TODO cannot be used with timed/delayed messages
t_imsg	*rtl_imsgWait(void *piq,int mode)
{
	t_iqmsg *iq = (t_iqmsg *)piq;
	t_imsg	*ret	= NULL;
	int	err;

	if	(mode == IMSG_TIMER || mode == IMSG_BOTH)
	{
		ret	= __MsgGet(iq,IMSG_TIMER);
		if	(ret)
		{
			return	ret;
		}
	}
	EnterCS(iq);
	while	(iq->iq_msgcount <= 0)
	{
//		rtl_trace(0,"thread will sleep now %d\n",iq->iq_count);
		err	= pthread_cond_wait(&iq->iq_CD,&iq->iq_CS);
		if	(err)
		{
			rtl_trace(0,"'%s':%d error=%d while cond_wait !!!\n",
			__FILE__,__LINE__,err);
			goto	leave;
		}
	}
//	rtl_trace(0,"thread wakeup %d\n",iq->iq_count);
	ret	= __MsgGet(iq,mode);
	leave : 
	LeaveCS(iq);
	return	ret;
}

// TODO cannot be used with timed/delayed messages
t_imsg	*rtl_imsgWaitTime(void *piq,int mode,int *ms)
{
	t_iqmsg *iq = (t_iqmsg *)piq;
	t_imsg	*ret	= NULL;
	int	err;
	struct	timeval		now;
	struct	timeval		dif;
	struct	timespec	tmt;

	if	(mode == IMSG_TIMER || mode == IMSG_BOTH)
	{
		ret	= __MsgGet(iq,IMSG_TIMER);
		if	(ret)
		{
			*ms	= 2;
			return	ret;
		}
//		rtl_trace(0,"no timer in queue => wait\n");
	}
	gettimeofday(&now,NULL);
	tmt.tv_sec	= now.tv_sec + (*ms / 1000);
	tmt.tv_nsec	= (now.tv_usec * 1000) + ((*ms % 1000)*1000000);
	if	(tmt.tv_nsec > 1000000000)
	{
		tmt.tv_sec	= tmt.tv_sec + 1;
		tmt.tv_nsec	= tmt.tv_nsec % 1000000000;
	}
	EnterCS(iq);
	while	(iq->iq_msgcount <= 0)
	{
//		rtl_trace(0,"thread will sleep now %d\n",iq->iq_count);
		err	= pthread_cond_timedwait(&iq->iq_CD,&iq->iq_CS,&tmt);
		if	(err == ETIMEDOUT)
		{
			*ms	= 0;
//			rtl_trace(0,"thread wakeup on tmt %d\n",iq->iq_count);
			goto	leave;
		}
		if	(err)
		{
			*ms	= -1;
			rtl_trace(0,"'%s':%d error=%d while cond_wait !!!\n",
			__FILE__,__LINE__,err);
			goto	leave;
		}
	}
//	rtl_trace(0,"thread wakeup %d\n",iq->iq_count);
	ret	= __MsgGet(iq,mode);
	// compute time waiting for a message
	gettimeofday(&dif,NULL);
	*ms	= (dif.tv_sec - now.tv_sec) * 1000;
	err	= (dif.tv_usec - now.tv_usec) / 1000;
	if	(err < 1000)
		*ms	= *ms + err;
	else
		*ms	= *ms + 1 + (err % 1000);
	*ms	= *ms + 1;	// to avoid confusion with timeout
	leave : 
	LeaveCS(iq);
	return	ret;
}

int	rtl_imsgFree(t_imsg *msg)
{
	if (msg->im_dataptr && msg->im_datasz > 0)
	{
		free (msg->im_dataptr);
	}
	free(msg);
	return 0;
}

int	rtl_imsgFastRemove(void *piq,t_imsg *rmv)
{
	t_iqmsg *iq = (t_iqmsg *)piq;
	int	nb = 0;

	EnterCS(iq);
	if	(rmv->im_timer || rmv->im_delayed)
	{
		list_del(&rmv->list);
		rtl_imsgFree(rmv);
		iq->iq_timercount--;
		iq->iq_count--;
		nb++;
	}
	else
	{
		list_del(&rmv->list);
		rtl_imsgFree(rmv);
		iq->iq_msgcount--;
		iq->iq_count--;
		nb++;
	}
	LeaveCS(iq);
	return	nb;
}

int	rtl_imsgRemove(void *piq,t_imsg *rmv)
{
	t_iqmsg *iq = (t_iqmsg *)piq;
	int	nb = 0;
	t_imsg	*msg;
	t_imsg	*tmp;

	EnterCS(iq);
	if	(rmv->im_timer || rmv->im_delayed)
	{
		list_for_each_entry_safe(msg,tmp,&(iq->iq_timerlist.list),list) 
		{
			if (msg == rmv)
			{
				list_del(&msg->list);
				rtl_imsgFree(msg);
				iq->iq_timercount--;
				iq->iq_count--;
				nb++;
				break;
			}
		}
	}
	else
	{
		list_for_each_entry_safe(msg,tmp,&(iq->iq_msglist.list),list) 
		{
			if (msg == rmv)
			{
				list_del(&msg->list);
				rtl_imsgFree(msg);
				iq->iq_msgcount--;
				iq->iq_count--;
				nb++;
				break;
			}
		}
	}
	LeaveCS(iq);
	return	nb;
}

int	rtl_imsgRemoveToType(void *piq,void *to,int class,int type)
{
	t_iqmsg *iq = (t_iqmsg *)piq;
	int	nb = 0;
	t_imsg	*msg;
	t_imsg	*tmp;

	EnterCS(iq);
	list_for_each_entry_safe(msg,tmp,&(iq->iq_msglist.list),list) 
	{
		if (msg->im_to == to && 
			(class == -1 || msg->im_class == class) && 
			(type == -1 || msg->im_type == type))
		{
			list_del(&msg->list);
			rtl_imsgFree(msg);
			iq->iq_msgcount--;
			iq->iq_count--;
			nb++;
		}
	}
	list_for_each_entry_safe(msg,tmp,&(iq->iq_timerlist.list),list) 
	{
		if (msg->im_to == to && 
			(class == -1 || msg->im_class == class) && 
			(type == -1 || msg->im_type == type))
		{
			list_del(&msg->list);
			rtl_imsgFree(msg);
			iq->iq_timercount--;
			iq->iq_count--;
			nb++;
		}
	}
	LeaveCS(iq);
	return	nb;
}

int	rtl_imsgRemoveTo(void *piq,void *to)
{
	return	rtl_imsgRemoveToType(piq,to,-1,-1);
}

static	int	__MsgRemoveAll(t_iqmsg *iq)
{
	int	nb = 0;
	t_imsg	*msg;
	t_imsg	*tmp;

	list_for_each_entry_safe(msg,tmp,&(iq->iq_msglist.list),list) 
	{
		list_del(&msg->list);
		rtl_imsgFree(msg);
		iq->iq_msgcount--;
		iq->iq_count--;
		nb++;
	}
	list_for_each_entry_safe(msg,tmp,&(iq->iq_timerlist.list),list) 
	{
		list_del(&msg->list);
		rtl_imsgFree(msg);
		iq->iq_timercount--;
		iq->iq_count--;
		nb++;
	}
	return	nb;
}

int	rtl_imsgRemoveAll(void *piq)
{
	t_iqmsg *iq = (t_iqmsg *)piq;
	int	ret;

	EnterCS(iq);
	ret	= __MsgRemoveAll(iq);
	LeaveCS(iq);
	return	ret;
}

void	rtl_imsgDump(t_imsg *msg,FILE *f)
{
	time_t	now;
	time_t	nowns;
	time_t	left	= 0;
	time_t	leftms	= 0;

	TimeBaseVal(&now,&nowns);

	if (msg->im_timer || msg->im_delayed) 
	{
		left	= msg->im_end-now;
		if	(msg->im_endns > nowns)
			leftms	= msg->im_endns-nowns;
		else
			leftms	= nowns-msg->im_endns;
		leftms	= leftms/1000000;
	}
fprintf(f,"timer=%d class=%d type=%d ser=%08x end=%ld.%03d left=%ld.%03d\n",
		msg->im_timer,
		msg->im_class,
		msg->im_type,
		msg->im_serial,
		msg->im_end,msg->im_endns/1000000,left,leftms);
	fflush(f);
}

void	rtl_imsgDumpIq(void *piq,FILE *f)
{
	t_iqmsg *iq = (t_iqmsg *)piq;
	t_imsg	*msg;

	EnterCS(iq);
	list_for_each_entry(msg,&(iq->iq_msglist.list),list) 
	{
		rtl_imsgDump(msg,f);
	}
	list_for_each_entry(msg,&(iq->iq_timerlist.list),list) 
	{
		rtl_imsgDump(msg,f);
	}
	LeaveCS(iq);

}

void	*rtl_imsgInitIq()
{
	t_iqmsg	*iq;

	iq	= (t_iqmsg *)malloc(sizeof(t_iqmsg));
	if (!iq) 
		return	NULL;

	memset(iq,0,sizeof(t_iqmsg));
	InitCS(iq);
	INIT_LIST_HEAD(&iq->iq_msglist.list);
	INIT_LIST_HEAD(&iq->iq_timerlist.list);
	return (void *)iq;
}

int	rtl_imsgFreeIq(void *piq)
{
	t_iqmsg	*iq = (t_iqmsg *)piq;
	int	ret;
	int	err;

	if (!iq)
		return	-1;
	EnterCS(iq);
	ret	= __MsgRemoveAll(iq);
	LeaveCS(iq);
	iq->iq_CSDone	= 0;
	if ((err=pthread_cond_destroy(&iq->iq_CD)))	// TODO
	{
		rtl_trace(0,"'%s':%d error=%d while destroying cond !!!\n",
			__FILE__,__LINE__,err);
	}
	if ((err=pthread_mutex_destroy(&iq->iq_CS)))	// TODO
	{
		rtl_trace(0,"'%s':%d error=%d while destroying mutex !!!\n",
			__FILE__,__LINE__,err);
	}
	free(iq);
	return	ret;
}

#ifdef rtl_imsg_MAIN	// tests only
#define rtl_imsg_MAIN

#define	IM_DEF		0

#define	IM_TIMER_GEN	100
#define	IM_TIMER_GEN_V	10000


#define	IM_LIST_CAPTOR_UPDATE	1000
#define	IM_LIST_CAPTOR_SAME	1001



/* tests */
#define	IM_DUMP_MSG	9000
#define	IM_CLEAR_SCR	9001
#define	IM_REFRESH_SCR	9002
#define	IM_STOP		9999

int	main(int argc,char *argv[])
{
	t_iqmsg	*IQ;
	t_imsg	*msg;
	int	tmt;
	int	i;
	int	nbtimer = 0;
	int	nbmsg = 0;

	IQ	= rtl_imsgInitIq();
/*
	rtl_imsgAdd(IQ,rtl_timerAlloc(IM_DEF,IM_TIMER_GEN,30000,NULL,0));
	rtl_imsgAdd(IQ,rtl_timerAlloc(IM_DEF,IM_TIMER_GEN,20000,NULL,0));
	rtl_imsgAdd(IQ,rtl_timerAlloc(IM_DEF,IM_TIMER_GEN,10000,NULL,0));
	rtl_imsgAdd(IQ,rtl_timerAlloc(IM_DEF,IM_TIMER_GEN,10000,NULL,0));
	rtl_imsgAdd(IQ,rtl_timerAlloc(IM_DEF,IM_TIMER_GEN,10000,NULL,0));
*/

	for ( i = 0 ; i < 60 ; i++ )
	{
		tmt	= rand() % 10;
		tmt	= tmt * 10;
		rtl_imsgAdd(IQ,rtl_timerAlloc(IM_DEF,IM_TIMER_GEN,tmt,NULL,0));
	}

	rtl_imsgAddDelayed(IQ,rtl_imsgAlloc(IM_DEF,IM_STOP,NULL,0),8*3600*1000);

	while (rtl_imsgCount(IQ) > 0 || rtl_timerCount(IQ) > 0)
	{
		int	needdump;

		// get all internal messages if any
		while ((msg= rtl_imsgGet(IQ,0)) != NULL)
		{
			if (msg->im_type == IM_STOP)
			{
				rtl_imsgRemoveTo(IQ,NULL);
				printf	("------ liste  %d %d  -----\n",
				rtl_imsgVerifCount(IQ),rtl_timerVerifCount(IQ));
				printf	("Ciao.\n");
				exit(0);
			}
			if (msg->im_type == IM_DUMP_MSG)
			{
				rtl_imsgAdd(IQ,rtl_imsgAlloc(IM_DEF,IM_CLEAR_SCR,NULL,0));
				rtl_imsgAdd(IQ,rtl_imsgAlloc(IM_DEF,IM_REFRESH_SCR,NULL,0));
			}
			if (msg->im_type == IM_CLEAR_SCR)
			{
				printf("[H[2J");
			}
			if (msg->im_type == IM_REFRESH_SCR)
			{
				rtl_imsgDumpIq(IQ,stdout);
				printf	("------ liste  %d %d %d %d %d %d -----\n",
				rtl_imsgVerifCount(IQ),rtl_timerVerifCount(IQ),
				nbmsg,nbtimer,needdump,
				rtl_tmms()/TimerResol);
			}
			rtl_imsgFree(msg);
			nbmsg++;
		}

		// wait for exernal events (poll())
		usleep(10000);

		// do timers if any
		needdump = 0;
		while ((msg= rtl_imsgGet(IQ,1)) != NULL)
		{
			rtl_imsgFree(msg);
			nbtimer++;

			tmt	= rand() % 20;
			tmt	= tmt * 1000;
			rtl_imsgAdd(IQ,rtl_timerAlloc(IM_DEF,IM_TIMER_GEN,tmt,NULL,0));
			needdump++;
		}
		if (needdump) 
		{
			rtl_imsgAdd(IQ,rtl_imsgAlloc(IM_DEF,IM_DUMP_MSG,NULL,0));
		}
	}

	return	0;
}
#endif
