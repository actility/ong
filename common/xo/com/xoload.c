
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


int	NbMalloc ;

/*
char		BUFF[2048] ;
*/
char		BUFF[4*8192] ;

int	main(int argc,char *argv[])
{
//	char	*hd  = NULL ;
	char	*cfg  = NULL ;
	char	*obj  = NULL ;
	int	f ;
	int	nb ;

	if	( argc < 2 )
		exit	(1) ;


	XoInit	(0) ;

	for	( nb = 1 ; nb < argc ; nb++ )
	{
		f	= open(argv[nb],XO_FIC_BINARY|O_RDONLY) ;
		if	( !f )
			exit(1) ;
#if	0
		hd	= XoLoad(f) ;
		if	( !hd )
		{
			fprintf(stderr,"\tHeader error : %s\n",argv[nb]) ;	
			exit(1) ;
		}
		XoFree	(hd,1) ;
#endif
		if	( nb == 1 )
		{
			cfg	= XoLoadRef(f) ;
			if	( !cfg )
			{
				fprintf(stderr,"\tload ref error : %s\n",argv[nb]) ;	
				exit(1) ;
			}
		}
		else
		{
			obj	= XoLoad(f) ;
			if	( !obj )
			{
				fprintf(stderr,"\tload obj error : %s\n",argv[nb]) ;	
				exit(1) ;
			}
		}
	}

	exit	(0) ;


	if	( obj)	
		XoFree	(obj,1) ;

	if	( cfg)
		XoFree	(cfg,1) ;

	XoEnd() ;

	exit	(0) ;
}
