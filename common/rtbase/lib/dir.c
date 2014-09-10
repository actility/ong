
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
 *	Consultation des entrees d'un repertoire
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <errno.h>
#include <dirent.h>
#include <limits.h>


typedef	struct
{
	DIR	*p_sysdir;
	char	p_dir[PATH_MAX];
	char	p_file[PATH_MAX];
}	t_path;

void	*rtl_openDir(char *path)	/* TS */
{
	t_path	*dir	= NULL;

	dir	= (t_path *)malloc(sizeof(t_path));
	if	(!dir)
	{
		return NULL;
	}
	if(!(dir->p_sysdir=opendir(path)))
	{
		free	(dir);
		return NULL;
	}
	strcpy	(dir->p_dir,path);
	return (void *)dir;
}

char	*rtl_readAbsDir(void *pdir)	/* TS */
{
	struct dirent *ent;
	t_path	*dir	= (t_path *)pdir;

	if(!dir)
		return	NULL;

	if((ent=readdir(dir->p_sysdir)))
	{
		sprintf	(dir->p_file,"%s/%s",dir->p_dir,ent->d_name);
//printf("filename '%s'\n",dir->p_file);
		return dir->p_file;
	}
	return NULL;
}

char	*rtl_readDir(void *pdir)	/* TS */
{
	struct dirent *ent;
	t_path	*dir	= (t_path *)pdir;

	if(!dir)
		return	NULL;

	if((ent=readdir(dir->p_sysdir)))
	{
		sprintf	(dir->p_file,"%s/%s",dir->p_dir,ent->d_name);
//printf("filename '%s'\n",ent->d_name);
		return ent->d_name;
	}
	return NULL;
}

void	rtl_closeDir(void *pdir)	/* TS */
{
	t_path	*dir	= (t_path *)pdir;

	if(!dir)
		return;
	closedir(dir->p_sysdir);
	free(dir);
}



