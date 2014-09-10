
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
 *
 */

#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>

static int	_rtl_mkdir_(char *deb,char *fin)	/* TS */
{
	char	str[512];
	int	nb;

	strcpy(str,deb);
	str[fin-deb]=0;		// TODO ...
	if(mkdir(str,0777)<0)
	{
		if(errno==EEXIST)
		{
			return 0;
		}
		if(errno!=ENOENT)
		{
			return -1;
		}
		for(fin--;fin>deb && *fin!='/';fin--);
		if(fin<=deb)
		{
			return -1;
		}
		if((nb=_rtl_mkdir_(deb,fin))<0)
		{
			return -1;
		}
		if(mkdir(str,0777)<0)
		{
			if(errno==EEXIST)
				return	0;
			return -1;
		}
		return nb+1;
	}
	return 1;
}

/*
 *
 *	Cette fonction cree un repertoire et tous les repertoires
 *	intermediaires, si possible.
 *	
 *	Le nombre de repertoire crees est retourne, ou '-1' si une
 *	erreur survient. Dans ce cas, 'errno' contient le code
 *	d'erreur (voir fonction systeme mkdir()).
 *
 */

int	rtl_mkdirp(char *path)	/* TS */
{
	return _rtl_mkdir_(path,path+strlen(path));
}


