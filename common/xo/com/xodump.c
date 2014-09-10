
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

int		WithAdr = 1 ;

void usage()
{
	fprintf(stderr,"xodump.x [-a] [-r f1.xor ... -r fN.xor] -d f1.xo ... -d fN.xo\n") ;
	fflush(stderr) ;
}

void	aff(char *obj)
{
	if	(WithAdr)
		XoDump(obj,stdout,1) ;
	else
		XoDumpWithoutAdr(obj,stdout,1) ;
}


int	main(int argc,char *argv[])
{
//	char	*hd  = NULL ;
	char	*cfg  = NULL ;
	char	*obj  = NULL ;
	int	f ;
	int	nb ;
	char	nexttype = 0 ;
	char	*pt;
	char	*file;
	int	nbr = 0;

	if	( argc < 2 )
	{
		usage	() ;
		exit	(1) ;
	}


	XoInit	(0) ;

	for	( nb = 1 ; nb < argc ; nb++ )
	{
		pt	= argv[nb];
		file	= pt ;
		if	( memcmp(pt,"-a",2) == 0 )
		{
			WithAdr	= 0 ;
			continue ;
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

		f	= open(file,XO_FIC_BINARY|O_RDONLY) ;
		if	( !f )
		{
			fprintf(stderr,"cannot open '%s'\n",file) ;
			fflush(stderr) ;
			exit(1) ;
		}
		switch	(nexttype)
		{
			case	'r':		/* references */
				cfg	= XoLoadRef(f) ;
				if	( !cfg )
				{
					fprintf(stderr,"\tload ref err : %s\n",
							file) ;	
					fflush(stderr) ;
					exit(1) ;
				}
				nbr++;
			break ;
			case	'd':		/* donnees */
				if (nbr == 0)
				{
					if (XoExtLoadRefDir(".") < 0)
					{
						fprintf(stderr,"XoExtLoadRefDir() error\n");
						fflush(stderr);
						exit(1);
					}
				}
				obj	= XoLoad(f) ;
				if	( !obj )
				{
					fprintf(stderr,"\tload obj err : %s\n",
						file) ;	
					fflush(stderr) ;
					exit(1) ;
				}
				aff	(obj) ;
			break ;
		}
	}
#if 0
	{
	int	nbmalloc ;
	int	lgmalloc ;
	xogetmalloc(&nbmalloc,&lgmalloc) ;
	printf	("szmalloc=%d malloccount=%d\n",lgmalloc,nbmalloc) ;
	nb	= XoDumpNbOcc(stdout,1,0) ;
	printf	("nbre d'objets %d\n",nb) ;
	}
#endif
	exit	(0) ;
}
