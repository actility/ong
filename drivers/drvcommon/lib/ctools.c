
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

/*! @file ctools.c
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

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>

#include "rtlbase.h"
#include "rtlimsg.h"
#include "xoapipr.h"

extern	int	Operating;

char	*AddBrackets(char *addr)
{
	static	char	buf[128];

	if	(!addr || !*addr)
		return	"";

	if	(strchr(addr,':') != NULL)	// ipv6
		sprintf(buf,"[%s]",addr);
	else
		sprintf(buf,"%s",addr);

	return	buf;
}


char	*SinceHHHHMMSS(time_t t,char *dst)
{
	time_t	now;
	int	h;
	int	m;
	int	s;

	if	(t == 0)
	{
		strcpy	(dst,"");
		return	dst;
	}

	now	= rtl_timemono(NULL);
	t	= now - t;

	h	= t / 3600;
	t	= t % 3600;	
	m	= t / 60;
	s	= t % 60;

	sprintf	(dst,"%d:%02d:%02d",h,m,s);
	return	dst;
}


/*!
 *
 * @brief load M2M xo/xml definitions
 * 	- ~/m2mxoref/xoref/m2m.xor
 * 	- ~/m2mxoref/xoref/xobix.xor
 * @return 
 * 	- <= on errors
 * 	- > on success
 * 
 */
int	LoadM2MXoRef()
{
	char	*awd;
	char	xfile[1024];
	int	ret;


	awd	= getenv("ROOTACT");
	if	(!awd)
	{
		RTL_TRDBG(0,"ERROR ROOTACT not set\n");
		return	-1;
	}

/*ML-20121001-retargeting-response+*/
	if	(!Operating)
		sprintf	(xfile,"%s/m2mxoref/xoref/m2m.xor",awd);
	else
		sprintf	(xfile,"%s/etc/xo/m2m.xor",awd);
/*ML-20121001-retargeting-response-*/
	ret	= XoExtLoadRef(xfile);
	if	(ret < 0)
	{
		RTL_TRDBG(0,"ERROR cannot load '%s' ret=%d\n",xfile,ret);
		return	-2;
	}

/*ML-20121001-retargeting-response+*/
	if	(!Operating)
		sprintf	(xfile,"%s/m2mxoref/xoref/xobix.xor",awd);
	else
		sprintf	(xfile,"%s/etc/xo/xobix.xor",awd);
/*ML-20121001-retargeting-response-*/
	ret	= XoExtLoadRef(xfile);
	if	(ret < 0)
	{
		RTL_TRDBG(0,"ERROR cannot load '%s' ret=%d\n",xfile,ret);
		return	-3;
	}

	return	1;
}

/*!
 *
 * @brief load specific (adaptor/driver) xo/xml definitions
 *	- ~/<adap>/xoref/<adap>.xor
 * @param adap	the adaptor/driver name
 * @return 
 * 	- <= on errors
 * 	- > on success
 * 
 */
int	LoadDrvXoRef(char *adap)
{
	char	*awd;
	char	xfile[1024];
	int	ret;

	awd	= getenv("ROOTACT");
	if	(!awd)
	{
		RTL_TRDBG(0,"ERROR ROOTACT not set\n");
		return	-1;
	}

/*ML-20121001-retargeting-response+*/
	if	(!Operating)
		sprintf	(xfile,"%s/%s/xoref/%s.xor",awd,adap,adap);
	else
		sprintf	(xfile, "%s/etc/%s/%s.xor", awd, adap, adap);
/*ML-20121001-retargeting-response-*/
	ret	= XoExtLoadRef(xfile);
	if	(ret < 0)
	{
		RTL_TRDBG(0,"ERROR cannot load '%s' ret=%d\n",xfile,ret);
		return	-2;
	}

	return	1;
}

