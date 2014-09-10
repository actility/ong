
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

#include	<stdio.h>
#include	<stdlib.h>
#include	<string.h>
#include	<fcntl.h>
#include	<time.h>

#include	"xoapipr.h"

t_objet_ref	*XoTabObjUser ;
int		XoNbObjUser  = 0 ;


int	main(int argc,char *argv[])
{
//	char	*hd  = NULL ;
	char	*cfg  = NULL ;
	char	*obj  = NULL ;
	int	f = 0;
	int	nb ;
	int	ret ;
	char	nexttype = '\0' ;
	char	*pt;
	char	*file;
	char	*svfile;
	char	*typeconv;
	char	out[128] ;
	char	conv[128] ;
	char	*comment = NULL;


	strcpy	(out,"") ;
	strcpy	(conv,"") ;

	if	( argc < 2 )
		exit	(1) ;


	XoInit	(0) ;
	XoAcceptDuplObjRef() ;
	for	( nb = 1 ; nb < argc ; nb++ )
	{
		pt		= argv[nb];
		file		= pt ;
		typeconv	= pt ;
		svfile		= pt ;
		if	( memcmp(pt,"-t",2) == 0 )
		{
			nexttype	= 't' ;
			if	( *(pt+2) == '\0' )
				continue ;
			else
				typeconv	= pt+2 ;
		}
		if	( memcmp(pt,"-r",2) == 0 )
		{
			nexttype	= 'r' ;
			if	( *(pt+2) == '\0' )
				continue ;
			else
				file	= pt+2 ;
		}
		if	( memcmp(pt,"-d",2) == 0 )
		{
			nexttype	= 'd' ;
			if	( *(pt+2) == '\0' )
				continue ;
			else
				file	= pt+2 ;
		}
		if	( memcmp(pt,"-s",2) == 0 )
		{
			nexttype	= 's' ;
			if	( *(pt+2) == '\0' )
				continue ;
			else
				svfile	= pt+2 ;
		}

		if	( nexttype == 'r' || nexttype == 'd' )
		{
			f	= open(file,XO_FIC_BINARY|O_RDONLY) ;
			if	( !f )
			{
				fprintf(stderr,"cannot open '%s'\n",file) ;
				fflush(stderr) ;
				exit(1) ;
			}
#if	0
			hd	= XoLoad(f) ;
			if	( !hd )
			{
				fprintf(stderr,"\tHeader error : %s\n",file) ;	
				fflush(stderr) ;
				exit(1) ;
			}
#endif
		}
		switch	(nexttype)
		{
			case	't':
				strcpy	(conv,typeconv);
				printf	("'%s' type racine\n",typeconv) ;
			break ;
			case	's':
				strcpy	(out,svfile);
				printf	("'%s' fichier sauvegarde\n",svfile) ;
			break ;
			case	'r':		/* references */
				cfg	= XoLoadRef(f) ;
				if	( !cfg )
				{
					fprintf(stderr,"\tload ref err : %s\n",
							file) ;	
					fflush(stderr) ;
					exit(1) ;
				}
			break ;
			case	'd':		/* donnees */
				if	( obj )
				{
					fprintf(stderr,
						"\tobj already loaded : %s\n",
						file) ;	
					fflush(stderr) ;
					exit(1) ;
				}
				obj	= XoLoad(f) ;
				if	( !obj )
				{
					fprintf(stderr,"\tload obj err : %s\n",
						file) ;	
					fflush(stderr) ;
					exit(1) ;
				}
#if	0
				comment = XoNmGetAttr(hd,"XoHeaderDesc",0) ;
				if	( !comment )
					comment	= "" ;
#endif
			break ;
		}
	}
	if	( !obj )
	{
		fprintf(stderr,"\t-d file ???\n") ;	
		fflush(stderr) ;
		exit(1) ;
	}

	if	( strlen(conv) == 0 )
	{
		fprintf(stderr,"\t-t type ???\n") ;	
		fflush(stderr) ;
		exit(1) ;
	}
	if	( strlen(out) == 0 )
	{
		fprintf(stderr,"\t-s svfile ???\n") ;	
		fflush(stderr) ;
		exit(1) ;
	}

	ret	= XoGetNumType(conv) ;
	if	( ret < 0 )
	{
		fprintf(stderr,"\ttype '%s' inexistant\n", conv) ;	
		fflush(stderr) ;
		exit(1) ;
	}

	ret	= XoNmChangeRef(obj,conv,1) ;
	if	( ret < 0 )
	{
		fprintf(stderr,"\tChangeRef err : %d\n", ret) ;	
		fflush(stderr) ;
		exit(1) ;
	}
	ret	= XoExtSave(out,obj,comment) ;
	if	( ret < 0 )
	{
		fprintf(stderr,"\tExtSave err : %d\n", ret) ;	
		fflush(stderr) ;
		exit(1) ;
	}

	exit	(0) ;
}

