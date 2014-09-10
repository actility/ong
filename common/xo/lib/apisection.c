
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


/*  RTL Technologies */
/*  Keyxord             : %W% */
/*  Current module name : %M% */
/*  SCCS file name      : %F% */
/*  SID                 : %I% */
/*  Delta Date          : %E% */
/*  Current date/time   : %D%,%T% */

#include	<stdio.h>
#include	<stdlib.h>
#ifndef		DOS
#include	<unistd.h>
#endif
#include	<string.h>
#include	<ctype.h>
#include	<memory.h>
#include	<errno.h>

#include	<pthread.h>

#include	"xoapipr.h"
#include	"intstr.h"
#include	"xointpr.h"
#ifdef	RTL_MBR
#include	"mbr/include/mbr.h"
#endif

/*
#define TRACE
*/

#ifdef	WIN32

#include	<windows.h>

static	CRITICAL_SECTION	XoCS;

/*DEF*/	void	XoInitCS(void)
{
	InitializeCriticalSection(&XoCS);
}

/*DEF*/	void	XoEnterCS(void)
{
	EnterCriticalSection(&XoCS);
}

/*DEF*/	void	XoLeaveCS(void)
{
	LeaveCriticalSection(&XoCS);
}

#else

static	pthread_mutex_t	CS;

/*DEF*/	void	XoInitCS(void)
{
#ifdef TRACE
printf("InitCS(%s,%d) %x\n",f,l,pthread_self());
#endif
	pthread_mutex_init(&CS,NULL);
}

/*DEF*/	void	XoEnterCS(void)
{
#ifdef TRACE
printf("EnterCS(%s,%d) %x\n",f,l,pthread_self());
#endif
	pthread_mutex_lock(&CS);
}

/*DEF*/	void	XoLeaveCS(void)
{
#ifdef TRACE
printf("LeaveCS(%s,%d) %x\n",f,l,pthread_self());
#endif
	pthread_mutex_unlock(&CS);
}

#endif
