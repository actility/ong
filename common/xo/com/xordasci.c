
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

void	usage()
{
	fprintf(stderr,"xordascii.x -r f1.xor ... -r fN.xor [-d out.xo]\n") ;
	fflush(stderr) ;
}


int	main(int argc,char *argv[])
{
//	char	*hd  = NULL ;
	char	*cfg  = NULL ;
	char	*obj  = NULL ;
	int	f ;
	int	nb ;
	char	*pt ;
	char	*file ;
	char	fout[128] ;

#if	0
	if	( argc < 2 )
	{
		usage	() ;
		exit	(1) ;
	}
#endif


	*fout	= '\0' ;
	XoInit	(0) ;

	for	( nb = 1 ; nb < argc ; nb++ )
	{
		pt	= argv[nb];
		file	= pt ;
		if	( memcmp(pt,"-r",2) == 0 )
		{
			if	( *(pt+2) == '\0' )
				continue ;
		}
		if	( memcmp(pt,"-d",2) == 0 )
		{
			if	( *(pt+2) != '\0' )
			{
				strcpy(fout,pt+2) ;
			}
			else
			{
				if	( nb < argc - 1 )
				{
					strcpy(fout,argv[nb+1]) ;
					nb++ ;
				}
			}
			continue ;
		}
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
		XoFree	(hd,1) ;
#endif


		cfg	= XoLoadRef(f) ;
		if	( !cfg )
		{
			fprintf(stderr,"\tload ref err : %s\n",file) ;	
			fflush(stderr) ;
			exit(1) ;
		}
	}
	nb	= 0 ;
	if	( !*fout )
	{
		while	( (obj=XoLoadAscii(stdin)) )
		{
			nb++ ;
			if	( XoSaveAscii(obj,stdout) < 0 )
			{
				fprintf(stderr,"\tsave ascii err\n") ;	
				fflush(stderr) ;
				exit	(1) ;
			}
		}
	}
	else
	{
		FILE	*f ;
		int	fd ;

#ifdef	WIN32
		f	= fopen(fout,"wb") ;
#else
		f	= fopen(fout,"w") ;
#endif
		if	( !f )
		{
			fprintf(stderr,"err. open '%s'\n",fout) ;	
			fflush(stderr) ;
			exit	(1) ;
		}
		fd	= fileno(f) ;
		while	( (obj=XoLoadAscii(stdin)) )
		{
			nb++ ;
			if	( XoSave(obj,fd) < 0 )
			{
				fprintf(stderr,"XoSave err\n") ;	
				fflush(stderr) ;
				exit	(1) ;
			}
		}

		fclose	(f) ;
	}
	if	( !nb )
	{
		fprintf(stderr,"\tload ascii err\n") ;	
		fflush(stderr) ;
		exit	(1) ;
	}
	exit	(0) ;
}

