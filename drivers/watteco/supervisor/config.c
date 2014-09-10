
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

/*! @file config.c
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

#include "adaptor.h"

#include "define.h"
#include "state.h"
#include "struct.h"

#include "cproto.h"
#include "cmnproto.h"
#include "extern.h"


static	char	*LoadConfig(char *fconfig,char *pref)
{
	FILE	*f;
	char	buf[1024];
	char	*pt;
	char	*ret	= NULL;

	RTL_TRDBG(0,"load config cluster '%s' from '%s'\n",pref,fconfig);

	f	= fopen(fconfig,"r");
	if	(!f)
		return	NULL;
	while	(fgets(buf,sizeof(buf)-1,f))
	{	
		if	(!strlen(buf))	continue;
		if	(buf[0] == '#')	continue;
		pt	= strchr(buf,'\n');
		if	(pt) *pt = '\0';
		pt	= strchr(buf,'\r');
		if	(pt) *pt = '\0';
		if	(!strlen(buf))	continue;

		ret	= strdup(buf);
		break;
	}

	fclose(f);

	return	ret;
}


static	char	*FindInDir(char *dir,unsigned int cluster)
{
	void	*D;
	char	*rel;
	char	pref[32];
	char	fconfig[1024];
	int	found	= 0;



	sprintf	(pref,"%04x",cluster);

	RTL_TRDBG(10,"search config cluster '%s' in '%s'\n",pref,dir);

	D	= rtl_openDir(dir);
	if	(!D)
		return	NULL;

	while	( !found && (rel = rtl_readDir(D)) )
	{
RTL_TRDBG(10,"cmp config cluster '%s' with '%s'\n",pref,rel);
		if	(rel && *rel && strncasecmp(pref,rel,strlen(pref)) == 0)
		{
			sprintf	(fconfig,"%s/%s",dir,rel);
			found	= 1;
		}
	}

	rtl_closeDir(D);
	if	(found)
	{
		return	LoadConfig(fconfig,pref);
	}
	return	NULL;
}


char	*FindConfigCluster(unsigned int cluster,char *addr)
{
	char	*awd;
	char	dir[1024];
	char	*ret	= NULL;


	awd	= getenv(GetEnvVarName());
	if	(awd)
	{
		sprintf	(dir,"%s/sensorconfig",awd);
		ret	= FindInDir(dir,cluster);
		if	(ret)
			return	ret;
	}
#if	0	// on cherche plus la config des clusters dans la livraison
	awd	= getenv("ROOTACT");
	if	(awd)
	{
		sprintf	(dir,"%s/%s/sensorconfig",awd,GetAdaptorName());
		ret	= FindInDir(dir,cluster);
		if	(ret)
			return	ret;
	}
#endif


	return	ret;
}

