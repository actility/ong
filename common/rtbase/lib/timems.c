
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


/*
 *	Horloge au millieme de secondes
 */


#include <stdio.h>
/*
 *	Utilisation de la fonction gettimeofday()
 */


#include <time.h>
#include <sys/time.h>

time_t	RtlBaseSec;

void	rtl_timespec(struct timespec *tv)
{
	struct	timeval	now;
	struct	timezone tz;
	gettimeofday(&now,&tz);
	tv->tv_sec	= now.tv_sec;
	tv->tv_nsec	= now.tv_usec * 1000;
}

time_t	rtl_time(time_t *part)
{
	return	time(part);
}

time_t	rtl_tmms()	/* TS */
{
	struct timeval tv;
	struct timezone tz;
	time_t	ret;

	gettimeofday(&tv,&tz);
	if(!RtlBaseSec)
	{
		RtlBaseSec	= tv.tv_sec;
	}
	tv.tv_sec -= RtlBaseSec;

#ifdef SCO32v4
	tv.tv_usec%=1000000;
#endif

	ret	= (tv.tv_usec/1000)+(1000*tv.tv_sec);

	return	ret;
}

void	rtl_timebase(struct timespec *tv)
{
	rtl_timespec(tv);
	if(!RtlBaseSec)
	{
		RtlBaseSec	= tv->tv_sec;
	}
	tv->tv_sec -= RtlBaseSec;
}

void	rtl_timebaseVal(time_t *sec,time_t *ns)
{
	struct timespec	tv;
	rtl_timespec(&tv);
	if(!RtlBaseSec)
	{
		RtlBaseSec	= tv.tv_sec;
	}
	tv.tv_sec -= RtlBaseSec;

	*sec	= tv.tv_sec;
	*ns	= tv.tv_nsec;
}



#if	0
// horloge en millis secondes
time_t	rtl_timems(time_t *base,time_t *etms,time_t *dtms)	/* TS */
{
	struct timeval tv;
	struct timezone tz;
	time_t	ret;

	gettimeofday(&tv,&tz);
	if(!*base)
	{
		*base	= tv.tv_sec;
	}
	tv.tv_sec -= *base;

#ifdef SCO32v4
	tv.tv_usec%=1000000;
#endif

	ret	= (tv.tv_usec/1000)+(1000*tv.tv_sec);

	*dtms	= (ret - *etms);
	*etms	= ret;

	return	ret;
}

// horloge en centis secondes
time_t	rtl_timecs(time_t *base,time_t *etcs,time_t *dtcs)	/* TS */
{
	struct timeval tv;
	struct timezone tz;
	time_t	ret;

	gettimeofday(&tv,&tz);
	if(!*base)
	{
		*base	= tv.tv_sec;
	}
	tv.tv_sec -= *base;

	ret	= (tv.tv_usec/10000)+(100*tv.tv_sec);

	*dtcs	= (ret - *etcs);
	*etcs	= ret;

	return	ret;
}
#endif
