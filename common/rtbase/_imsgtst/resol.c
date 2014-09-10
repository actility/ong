
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

/* tests */
#define	IM_DUMP_MSG	9000
#define	IM_CLEAR_SCR	9001
#define	IM_REFRESH_SCR	9002
#define	IM_STOP		9999

int	Resol	= 100;

int	main(int argc,char *argv[])
{
	void	*IQ;
	t_imsg	*msg;
	int	tmt;
	int	i;
	int	nbtimer = 0;
	int	nbmsg = 0;

	rtl_timerResol(Resol);

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

	// 8 hours
	rtl_imsgAddDelayed(IQ,rtl_imsgAlloc(IM_DEF,IM_STOP,NULL,0),8*3600*1000);
	// 10 days
	rtl_imsgAddDelayed(IQ,rtl_imsgAlloc(IM_DEF,IM_STOP,NULL,0),10*24*3600*1000);

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
				rtl_tmms());
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
