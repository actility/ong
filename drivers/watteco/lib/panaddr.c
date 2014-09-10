
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
#include <stdarg.h>
#include <string.h>
#include <time.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/stat.h>

#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <err.h>

#include "rtlbase.h"
#include "aw.h"
#include "watteco.h"


/*
 * tunslip6 (ie USB router watteco)  writes the following file 
 * sometime @1 is not present ... but @2 is
 */

/*
IPv6@:{
@1=aaaa::ff:ff00:23f4
@2=fe80::ff:ff00:23f4
}
*/

char *AwGetPanAddr(char *pan)	/* TS */
{
	static	int	traced = 0;
	static	int	errtraced = 0;
	char	sfile[512];
	char	buf[512];
	char	addr[512];
	char	v2[512];
	FILE	*f;
	char	*pt;
	int	found = 0;

	pt	= getenv(AW_DATA_DIR);
	if	(!pt || !*pt)
	{
		return	NULL;
	}
	pan[0]	= '\0';
	v2[0]	= '\0';
	sprintf	(sfile,"%s/routeraddr.txt",pt);
	f	= fopen(sfile,"r");
	if	(!f)
	{
		if	(traced)
			return	NULL;
		RTL_TRDBG(0,"ERROR cannot open file '%s'\n",sfile);
		traced	= 1;
		return	NULL;
	}

	while	(fgets(buf,sizeof(buf)-1,f))
	{
		if	(!strlen(buf))	continue;
		if	(buf[0] == '@' && buf[1] == '1' && buf[2] == '=')
		{
			pt	= strchr(buf,'\n');
			if	(pt)
				*pt	= '\0';
			pt	= strchr(buf,'\r');
			if	(pt)
				*pt	= '\0';
			strcpy	(pan,buf+3);
			found = 1;
			break;
		}
		if	(buf[0] == '@' && buf[1] == '2' && buf[2] == '=')
		{
			pt	= strchr(buf,'\n');
			if	(pt)
				*pt	= '\0';
			pt	= strchr(buf,'\r');
			if	(pt)
				*pt	= '\0';
			strcpy	(v2,buf+3);
		}
	}

	if	(!found)
	{
		if	(!errtraced)
		RTL_TRDBG(0,"ERROR router address (@1) not found in '%s'\n",
							"routeraddr.txt");
		addr[0]	= '\0';
		AwGetPanPref(addr);
		if	(strlen(addr) && strlen(v2))
		{
			if	(!errtraced)
		RTL_TRDBG(0,"ERROR router address built with '%s' & '%s'\n",
						addr,v2);
			addr[4]	= '\0';	// because aaaa::1
			memcpy(v2,addr,strlen(addr));
			strcpy(pan,v2);
		}
		errtraced	= 1;
	}

	fclose(f);
	return	pan;
}

/*
 radio:{
 type=lora|rf212
 }
 */

char *AwGetRadioMode(char *mode)	/* TS */
{
	char	sfile[512];
	char	buf[512];
	FILE	*f;
	char	*pt;

	pt	= getenv(AW_DATA_DIR);
	if	(!pt || !*pt)
	{
		return	NULL;
	}
	mode[0]	= '\0';
	sprintf	(sfile,"%s/routeraddr.txt",pt);
	f	= fopen(sfile,"r");
	if	(!f)
	{
		RTL_TRDBG(0,"ERROR cannot open file '%s'\n",sfile);
		return	NULL;
	}

	while	(fgets(buf,sizeof(buf)-1,f))
	{
		pt	= strchr(buf,'\n');
		if	(pt)
			*pt	= '\0';
		pt	= strchr(buf,'\r');
		if	(pt)
			*pt	= '\0';
		if	(strncmp(buf,"type=",5) == 0)
		{
			pt	= buf + 5;
			if	(!strcmp(pt,"rf212") || !strcmp(pt,"lora"))
			{
				strcpy	(mode,pt);
				break;
			}
		}
	}

	fclose(f);
	return	mode;
}

char *AwGetPanPref(char *addr)	/* TS */
{
	char	sfile[512];
	char	buf[512];
	FILE	*f;
	char	*pt;

	pt	= getenv(AW_DATA_DIR);
	if	(!pt || !*pt)
	{
		return	NULL;
	}
	addr[0]	= '\0';
	sprintf	(sfile,"%s/prefipv6.txt",pt);
	f	= fopen(sfile,"r");
	if	(!f)
	{
		RTL_TRDBG(0,"ERROR cannot open file '%s'\n",sfile);
		return	NULL;
	}

	while	(fgets(buf,sizeof(buf)-1,f))
	{
//		if	(!strlen(buf))	continue;
//		si vide local address
		pt	= strchr(buf,'\n');
		if	(pt)
			*pt	= '\0';
		pt	= strchr(buf,'\r');
		if	(pt)
			*pt	= '\0';
		if	(strstr(buf,"::") == NULL)	// TODO
			sprintf	(addr,"%s::1",buf);
		else
			sprintf	(addr,"%s:1",buf);
		break;
	}

	fclose(f);
	return	addr;
}
