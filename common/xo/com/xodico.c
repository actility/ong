
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

extern	char	*XoDico;


int	main(int argc,char *argv[])
{
	int32	ret;

	if	( argc < 2 )
		exit	(1) ;

	XoInit	(0) ;

	ret	= XoLoadSharedDico(argv[1]);
	if	(ret < 0)
	{
		fprintf(stderr,"\tload error : %s\n",argv[1]) ;	
		exit(1) ;
	}

	if	(argc >= 3)
	{
		char	*pt	= argv[2];
		int	offset;

		if	(*pt == '+')
		{
			offset	= atoi(pt+1);
			printf	("'%s'\n",XoDico+offset);
			XoEnd() ;
			exit	(0) ;
		}

		ret	= XoFindInSharedDicoPrefixed(pt,&offset);
		if	(ret <= 0)
		{
			printf	("cannot find '%s' in shared dico %d %d\n",
				argv[2],ret,offset);
			if	(offset > 0)
			{
				printf	("prefixe found '%s' in shared dico\n",
					XoDico+offset);
				printf	("ret=%d %08x\n",offset,offset);
			}
		}
		else
			printf	("ret=%d %08x\n",ret,ret);
	}

	XoEnd() ;
	exit	(0) ;
}
