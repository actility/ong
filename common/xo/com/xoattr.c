
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

#include	"xoapipr.h"

t_objet_ref	*XoTabObjUser ;
int		XoNbObjUser  = 0 ;

main(argc,argv)
int	argc ;
char	*argv[] ;
{
	int	ref ;
	int	nb ;
//	char	*hd ;
	char	*cfg ;

	if	( argc < 2 )
	{
		fprintf(stderr,"%s file.xor ... file.xor\n",argv[0]) ;
		exit(1) ;
	}


	XoInit	(0) ;

	for	( nb = 1 ; nb < argc ; nb++ )
	{
		ref	= open(argv[nb],XO_FIC_BINARY|O_RDONLY) ;
		if	( !ref )
		{
			perror(argv[nb]) ;
			exit(1) ;
		}
#if	0
		hd	= XoLoad	(ref) ;
		if	( !hd )
		{
			fprintf(stderr,"erreur XoLoad() header\n") ;
			exit(1) ;
		}
#endif
		cfg	= XoLoadRef	(ref) ;
		if	( !cfg )
		{
			fprintf(stderr,"erreur XoLoadRef()\n") ;
			exit(1) ;
		}
	}
	
	XoInit	(1) ;

	XoEnd	() ;
/*
	valloc_bilan() ;
*/
	exit(0) ;
}
