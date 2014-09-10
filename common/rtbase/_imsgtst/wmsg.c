
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

void	*IQ;

int	main(int argc,char *argv[])
{
	int	tmt = IM_TIMER_GEN_V;
	int	maxwait;
	t_imsg	*msg;

	rtl_tracemutex();

	IQ	= rtl_imsgInitIq();
	if (!IQ)
	{
		printf("cannot alloc main queue\n");
		exit(1);
	}

	rtl_imsgAdd(IQ,rtl_timerAlloc(IM_DEF,IM_TIMER_GEN,tmt,NULL,0));

	while (1)
	{
		maxwait	= 500;	// milli sec
		if	((msg= rtl_imsgWaitTime(IQ,IMSG_BOTH,&maxwait)) != NULL)
		{
			if (msg->im_type == IM_MSG_STOP)
			{
				printf("IM_MSG_STOP\n");
			}
			if (msg->im_type == IM_TIMER_GEN)
			{
				printf("IM_TIMER_GEN\n");
				tmt	+= 100;
	rtl_imsgAdd(IQ,rtl_timerAlloc(IM_DEF,IM_TIMER_GEN,tmt,NULL,0));
			}
			rtl_imsgFree(msg);
		}
		else
		{
			rtl_trace(0,"timeout %d (m=%d t=%d)\n",
				maxwait,rtl_imsgCount(IQ),rtl_timerCount(IQ));
		}
	}

	rtl_imsgFreeIq(IQ);

	return	0;

}
