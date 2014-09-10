
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
 *	Manipulation des noms de fichier
 */

#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <ctype.h>

#undef	WIN32


#define	ISSEP(c)		(((c)=='/')||((c)=='\\'))

#ifdef WIN32
#define	PATHSEPARATOR		'\\'
#define PATHITEMCMP(c1,c2)	((ISSEP(c1)&&(ISSEP(c2)))?(1):(toupper(c1)==toupper(c2)))
#else
#define	PATHSEPARATOR		'/'
#define PATHITEMCMP(c1,c2)	((ISSEP(c1)&&(ISSEP(c2)))?(1):((c1)==(c2)))
#endif

/*
 *
 *	Retourne le dernier element d'un chemin
 *
 */

char	*rtl_basename(char *fullName)	/* TS */
{
	register char 	*pt;

	for(pt=fullName;*pt;pt++)
	{
		if(ISSEP(*pt))
		{
			fullName=pt+1;
		}
	}
	return fullName;
}

/*
 *
 *	Retourne le dernier suffixe d'un nom de fichier
 *
 */

char	*rtl_suffixname(char *fullName)	/* TS */
{
	register char *old=0L;

	while(*fullName)
	{
		if(ISSEP(*fullName))
		{
			old=NULL;
		}
		if(*fullName=='.')
		{
			old=fullName;
		}
		fullName++;
	}
	return old;
}

/*
 *
 *	Retourne le chemin d'acces d'un fichier
 *
 */

char	*rtl_dirname(char *fullName,char *pathName)	/* TS */
{
	register char 	*pt,*max,*buf;

	max	= rtl_basename(fullName)-1;
	buf	= pathName;
	for(pt=fullName;pt<max;pt++)
	{
		*buf++ = *pt;
	}
	*buf = 0;
	if(ISSEP(*fullName) && (*pathName)==0)
	{
		*pathName   = PATHSEPARATOR;
		pathName[1] = 0;
	}
	return pathName;
}

/*
 *
 *	Adapte un nom de fichier au format de l'OS cible
 *
 *	Le path en entree peut etre constitue de '/' et de '\'
 *	En sortie, le path est constitue de '/' sous unix, 
 *	et de '\' sous DOS
 *
 *	ex:	C:\ROOT/tools/xsb
 *		-> C:/ROOT/tools/xsb sous UNIX
 *		-> C:\ROOT\tools\xsb sous DOS
 *
 */

void	rtl_checkPathName(char *path)	/* TS */
{
	while(*path)
	{
		if(!ISSEP(*path))
		{
			path++;
			continue;
		}
		*path++=PATHSEPARATOR;
	}
}

/*
 *
 *	Concatenation de deux morceaux de path
 *
 *
 */

void	rtl_catPathNames(char *left,char *right)	/* TS */
{
	char	*pt;

	if(*left)
	{
		pt	= left+strlen(left)-1;
		if(!ISSEP(*pt))
		{
			pt[1]=PATHSEPARATOR;
			pt[2]=0;
		}
	}
	strcat(left,right);
}

/*
 *
 *	Calcul un chemin relatif pour passer d'un chemin a un autre
 *
 *	path		chemin a acceder
 *	pathRef		chemin depuis lequel on veut acceder a 'path'
 *	pathRet		chemin relatif en retour
 *
 */


void rtl_absToRelPath(char *path,char *pathRef,char *pathRet)	/* TS */
{
	char	*pt,*ref;
	char	*newPt,*newRef="";
	int	level;
	int	commun;
	static	char	separator[1+1] = { PATHSEPARATOR, '\0' };


	*pathRet=0;
	/* Recherche de la partie commune */
	commun=0;
	newPt=NULL;
	for(pt=path,ref=pathRef;*pt && PATHITEMCMP(*pt,*ref);pt++,ref++)
	{
		commun++;
		if(ISSEP(*pt))
		{
			newPt=pt+1;
			newRef=ref+1;
		}
	}
	if(!commun)
	{
		strcpy(pathRet,path);
		return;
	}
	if(commun==(int)strlen(path) && commun==(int)strlen(pathRef))
	{
		strcpy(pathRet,".");
		return;
	}
	if(!newPt)
	{
		return;
	}

	/* Calcul du chemin relatif */

	/* on remonte dans la partie diff jusqu'a la partie commune	*/
// printf("newPt=%s newRef=%s\n",newPt,newRef);
	*pathRet=0;
	for(level=0,pt=newRef;pt&&*pt;pt++)
	{
		if(ISSEP(*pt))
		{
			if(*pathRet==0)
			{
				strcat(pathRet,"..");
				strcat(pathRet,separator);
			}
			strcat(pathRet,"..");
			strcat(pathRet,separator);
		}
	}
	if(*pathRet==0)
	{
		strcat(pathRet,"..");
		strcat(pathRet,separator);
	}

	/* on rajoute la partie commune	*/
	strcat(pathRet,newPt);
/*
	printf("path=%s => %s\n",path,newPt);
	printf("pathRef=%s => %s\n",pathRef,newRef);
	printf("ret=%s\n",pathRet);
*/
}


