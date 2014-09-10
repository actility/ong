
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
#include <unistd.h>


#include <time.h>
#include <sys/time.h>

#include "rtlbase.h"

extern	time_t	RtlBaseSec;

#ifdef  WIRMAV2
#undef  CLOCK_MONOTONIC_RAW	// does not work on wirmav2 ...	
#endif


#if	defined(_POSIX_TIMERS) && defined(_POSIX_MONOTONIC_CLOCK)
#ifdef	CLOCK_MONOTONIC_RAW
static	char *TimeType	= "@(#) Actility clock_gettime_CLOCK_MONOTONIC_RAW";
#else
static	char *TimeType	= "@(#) Actility clock_gettime_POSIX_MONOTONIC_CLOCK";
#endif
#else
static	char *TimeType	= "@(#) Actility gettimeofday";
#endif

char    *rtl_timespecmonoType()
{
        return  TimeType;
}


#if	defined(_POSIX_TIMERS) && defined(_POSIX_MONOTONIC_CLOCK)
void	rtl_timespecmono(struct timespec *tv)
{
#ifdef	CLOCK_MONOTONIC_RAW
	clock_gettime(CLOCK_MONOTONIC_RAW,tv);
#else
	clock_gettime(CLOCK_MONOTONIC,tv);
#endif
}
#else
void	rtl_timespecmono(struct timespec *tv)
{
	struct	timeval		now;
	gettimeofday(&now,NULL);
	tv->tv_sec	= now.tv_sec;
	tv->tv_nsec	= now.tv_usec * 1000;
}
#endif

time_t	rtl_timemono(time_t *part)
{
	struct timespec	tv;

	rtl_timespecmono(&tv);
	if	(part)
	{
		*part	= tv.tv_sec;
	}

	return	tv.tv_sec;
}

time_t	rtl_tmmsmono()
{
	struct timespec	tv;
	time_t		ret;

	rtl_timespecmono(&tv);
	if(!RtlBaseSec)
	{
		RtlBaseSec	= tv.tv_sec;
	}
	tv.tv_sec -= RtlBaseSec;
	ret	= (tv.tv_nsec/1000000)+(1000*tv.tv_sec);
	return	ret;

}

void	rtl_timebasemono(struct timespec *tv)
{
	rtl_timespecmono(tv);
	if(!RtlBaseSec)
	{
		RtlBaseSec	= tv->tv_sec;
	}
	tv->tv_sec -= RtlBaseSec;
}

void	rtl_timebasemonoVal(time_t *sec,time_t *ns)
{
	struct timespec	tv;
	rtl_timespecmono(&tv);
	if(!RtlBaseSec)
	{
		RtlBaseSec	= tv.tv_sec;
	}
	tv.tv_sec -= RtlBaseSec;

	*sec	= tv.tv_sec;
	*ns	= tv.tv_nsec;
}

void	rtl_timespecaddms(struct timespec *tv,int ms)
{
	tv->tv_sec	+= ms / 1000;
	tv->tv_nsec	+= (ms % 1000) * 1000000;
	if	(tv->tv_nsec >= 1000000000)
	{
		tv->tv_sec	+= 1;
		tv->tv_nsec	-= 1000000000;
	}
}

int	rtl_timespeccmp(struct timespec *tv1, struct timespec *tv2)
{
	return	RTL_TIMESPECMP(tv1,tv2);
}
