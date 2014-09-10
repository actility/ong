
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

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#include "aw.h"
#include "watteco.h"

/*
 *	for the list of current Sensors (tbcur) find those to be :
 *		- created 	// add Sensor in tbnew
 *		- deleted	// add Sensor in tbdel
 *	by comparing Sensors detected in tbfil
 */

static	int AwSensorFromFile(char *fdata,time_t *last,char *tbcur[],char *tbdel[],char *tbnew[]) /* TS */
{
	char	*tbfil[MAX_SSR_PER_RTR];	// load from file
	FILE	*f;
	char	*pt;
	char	file[512];
	char	buf[512];
	int	i;
	int	nbfil	= 0;
	int	nbnew	= 0;
	int	nbdel	= 0;
	struct	stat	st;

	for	(i = 0 ; i < MAX_SSR_PER_RTR ; i++)
	{
		tbfil[i]	= NULL;
		tbnew[i]	= NULL;
		tbdel[i]	= NULL;
	}

	pt	= getenv(AW_DATA_DIR);
	if	(!pt || !*pt)
	{
		return	-1;
	}
	sprintf	(file,"%s/%s",pt,fdata);
	f	= fopen(file,"r");
	if	(!f)
	{
		return	0;	// file not ready
	}
	if	(last)
	{	// verify last modification date
		if	(fstat(fileno(f),&st) < 0)	// ???
		{
			fclose(f);
			return	-2;
		}
		if	(st.st_mtime == *last)
		{	// file date does not change
			fclose(f);
			return	0;
		}
		*last	= st.st_mtime;
	}

	// load all Sensors and search new one
	nbfil	= 0;
	nbnew	= 0;
	while	(fgets(buf,sizeof(buf)-1,f) && nbfil < MAX_SSR_PER_RTR)
	{	
		int	found;

		if	(!strlen(buf))	continue;
		if	(buf[0] == '#')	continue;
		pt	= strchr(buf,'\n');
		if	(pt) *pt = '\0';
		pt	= strchr(buf,'\r');
		if	(pt) *pt = '\0';

		tbfil[nbfil++]	= strdup(buf);

		found	= 0;
		for	(i = 0 ; tbcur[i] && i < MAX_SSR_PER_RTR ; i++)
		{
			if	(strcmp(buf,tbcur[i]) == 0)
			{
				found	= 1;
				break;
			}
		}
		if	(!found)
		{
//warnx("%s:%d new Sensor detect '%s'\n",__FILE__,__LINE__,buf);
			tbnew[nbnew++]	= strdup(buf);
		}
	}

	// find Sensors to delete
	for	(i = 0 ; tbcur[i] && i < MAX_SSR_PER_RTR ; i++)
	{
		int	found;
		int	j;

		found	= 0;
		for	(j = 0 ; tbfil[j] && j < MAX_SSR_PER_RTR ; j++)
		{
			if	(strcmp(tbcur[i],tbfil[j]) == 0)
			{
				found	= 1;
				break;
			}
		}
		if	(!found)
		{
//warnx("%s:%d delete Sensor detect '%s'\n",__FILE__,__LINE__,tbcur[i]);
			tbdel[nbdel++]	= strdup(tbcur[i]);
		}
	}

	for	(i = 0 ; i < MAX_SSR_PER_RTR ; i++)
	{
		if	(tbfil[i])
			free(tbfil[i]);
	}

//warnx("%s:%d ca=%d +=%d -=%d\n",__FILE__,__LINE__,nbfil,nbnew,nbdel);

	fclose(f);
	return	nbdel+nbnew;
}

int AwSensorDiscover(time_t *last,char *tbcur[],char *tbdel[],char *tbnew[]) /* TS */
{
	return	AwSensorFromFile("discoverpan.txt",last,tbcur,tbdel,tbnew);
}

int AwSensorSoftware(time_t *last,char *tbcur[],char *tbdel[],char *tbnew[]) /* TS */
{
	return	AwSensorFromFile("discoversoft.txt",last,tbcur,tbdel,tbnew);
}
