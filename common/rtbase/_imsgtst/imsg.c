
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

#include "rtlimsg.h"

#define	IM_DEF		0

#define	IM_TIMER_GEN	100
#define	IM_TIMER_GEN_V	1000

#define	IM_MSG_START	1000
#define	IM_MSG_STOP	1001
#define	IM_MSG_ADD	1002
#define	IM_MSG_SUB	1003

#define	NB_THREAD	40
#define	NB_MESS		100000

#define	MSTIC		10

void	*MainTbPoll;

typedef	struct
{
	int	th_num;
	void	*th_iq;
}	t_th;

t_th	TabTh[NB_THREAD];

void	*IQ;

void	*RunThread(void *pth)
{
	t_th	*th	= (t_th *)pth;
	t_imsg	*msg;
	int	op;
	int	num;
	int	i;
	int	nb;

	num	= th->th_num;
//	printf	("thread %d started\n",num);

	msg	= rtl_imsgGet(th->th_iq,IMSG_MSG);
	if (!msg || msg->im_type != IM_MSG_START)
	{
//		printf	("thread %d signal error on start %lx\n",num,pth);
		goto	endthread;
	}

	usleep	(rand()%1000000);
//	printf	("thread %d continue\n",num);

	for (i = 0; i < NB_MESS; i++) 
	{
		if (num % 2 == 0) 
			op	= IM_MSG_SUB;
		else
			op	= IM_MSG_ADD;
		nb	= rtl_imsgAdd(IQ,rtl_imsgAlloc(IM_DEF,op,NULL,0));
		if (i%1000 == 0)
			usleep	(100000+rand()%100000);
	}

endthread :
//	printf	("thread %d signal stop %lx\n",num,pth);
	msg	= rtl_imsgAlloc(IM_DEF,IM_MSG_STOP,NULL,0);
//	msg	= rtl_imsgCpyData(msg,th,sizeof(t_th));
	msg	= rtl_imsgSetData(msg,th);
	rtl_imsgAdd(IQ,msg);

	pthread_exit(NULL);

	return	NULL;
}

int	main(int argc,char *argv[])
{
	t_imsg	*msg;
	int	i;
	int	stop = NB_THREAD;
	int	tmt = IM_TIMER_GEN_V;
	int	nbrecv = 0;
	int	loop = 0;
	long	value = 0;

	MainTbPoll	= rtl_pollInit();
	if (!MainTbPoll)
	{
		printf("cannot alloc main tbpoll\n");
		exit(1);
	}
	rtl_tracemutex();

retry :
	loop	= 0;
	nbrecv	= 0;
	value	= 0;
	stop 	= NB_THREAD;
	IQ	= rtl_imsgInitIq();
	if (!IQ)
	{
		printf("cannot alloc main queue\n");
		exit(1);
	}
	rtl_imsgAdd(IQ,rtl_timerAlloc(IM_DEF,IM_TIMER_GEN,tmt,NULL,0));

/*
*/
	for (i = 0; i < NB_THREAD; i++) 
	{
		pthread_attr_t	at;
		pthread_t	th;

		TabTh[i].th_num	= i;
		TabTh[i].th_iq	= rtl_imsgInitIq();
		if (!TabTh[i].th_iq)
		{
			printf("cannot alloc thread queue\n");
			exit(1);
		}
		rtl_imsgAdd(TabTh[i].th_iq,
				rtl_imsgAlloc(IM_DEF,IM_MSG_START,NULL,0));

		if (pthread_attr_init(&at))
		{
			rtl_trace(0,"cannot init thread attr\n");
			return	1;
		}
		pthread_attr_setdetachstate(&at,PTHREAD_CREATE_DETACHED);
		if (pthread_create(&th,&at,RunThread,(void *)&TabTh[i]))
		{
			rtl_trace(0,"cannot create thread\n");
			return	1;
		}
	}

	while	(stop > 0)
	{
		t_th	*th;

		loop++;
		while ((msg= rtl_imsgGet(IQ,IMSG_BOTH)) != NULL)
		{
			if (msg->im_type == IM_MSG_STOP)
			{
				th	= (t_th *)msg->im_dataptr;
/*
				printf("end thread %d value=%ld\n",
					th->th_num,value);
*/
				stop--;
				if (&TabTh[th->th_num] == th)
					rtl_imsgFreeIq(th->th_iq);
			}
			if (msg->im_type == IM_MSG_ADD)
			{
//				printf("+");
				value++;
				nbrecv++;
			}
			if (msg->im_type == IM_MSG_SUB)
			{
//				printf("-");
				value--;
				nbrecv++;
			}
			if (msg->im_type == IM_TIMER_GEN)
			{
				char	when[128];
				rtl_hhmmssms(when);
				printf("%s diffms=%d loop=%d\n",
						when,msg->im_diffms,loop);
				rtl_imsgAdd(IQ,
				rtl_timerAlloc(IM_DEF,IM_TIMER_GEN,tmt,NULL,0));
				loop	= 0;
/*
				printf("value=%ld recv=%d left=%d\n",
					value,nbrecv,rtl_imsgCount(IQ));
*/
			}
			rtl_imsgFree(msg);
		}
		rtl_poll(MainTbPoll,MSTIC);
	}

	printf("nbrecv=%d\n",nbrecv);
	printf("value=%ld\n",value);

	if (!value && nbrecv == NB_THREAD * NB_MESS && rtl_imsgCount(IQ) == 0)
	{
		printf("##############################################\n");
		printf("####     SUCCESS    SUCCESS               ####\n");
		printf("##############################################\n");
	}
	else
	{
		printf("##############################################\n");
		printf("####     ERROR      ERROR                 ####\n");
		printf("##############################################\n");
		exit(0);
	}

	rtl_imsgFreeIq(IQ);

	goto	retry;

	return	0;

}
