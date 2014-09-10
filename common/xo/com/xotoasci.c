
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
#include	<sys/types.h>
#include	<sys/stat.h>
#include	<unistd.h>
#include	<string.h>
#include	<fcntl.h>
#include	<time.h>

#include	"xoapipr.h"

t_objet_ref	*XoTabObjUser ;
int		XoNbObjUser  = 0 ;

int		Xml	= 0;

void usage()
{
	fprintf(stderr,"xotoasci.x [-xml] [-shdico] [-r f1.xor ... -r fN.xor] -[d|m] f1.xo ... -[d|m] fN.xo\n") ;
	fflush(stderr) ;
}

void aff(char *o)
{
	static	int nsdone = 0;
	FILE	*f;
	int	ret;
	char	*tmpf;
	char	tmp[2048];

	if	(!Xml)
	{
		XoSaveAscii(o,stdout) ;
		return;
	}
	if	(!nsdone)
	{
		nsdone	= 1;
		XoLoadNameSpaceDir(".");
	}
	tmpf	= tempnam("/tmp","xoxml");
	if	(!tmpf)
	{
		printf("cannot create temp file in /tmp ...\n");
		exit(1);
	}
	if	((ret=XoWritXmlEx(tmpf,o,0,NULL)) < 0)
	{
		unlink(tmpf);
		printf("XoWritXmlEx() error=%d\n",ret);
		exit(1);
	}
	f	= fopen(tmpf,"r");
	if	(!f)
	{
		unlink(tmpf);
		printf("fopen(%s) error\n",tmpf);
		exit(1);
	}
	while	(fgets(tmp,sizeof(tmp),f))
		printf("%s",tmp);
	fclose(f);
	unlink(tmpf);
}

void DefaultLoadRef()
{
	char	tmp[1024];
	int	nb;

	nb	= XoExtLoadRefDir(".");
	if	(nb == 0)
	{
		sprintf	(tmp,"%s/m2mxoref/xoref",getenv("ROOTACT"));
		nb	= XoExtLoadRefDir(tmp);
	}
	if (nb <= 0)
	{
		fprintf(stderr,"XoExtLoadRefDir() error\n");
		fflush(stderr);
		exit(1);
	}
}

int	main(int argc,char *argv[])
{
//	char	*hd  = NULL ;
	char	*cfg  = NULL ;
	char	*obj  = NULL ;
	int	f ;
	int	nb ;
	char	nexttype = '\0' ;
	char	*pt;
	char	*file;
	int	nbr	= 0;

	char	*mem;
	struct	stat	st;


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
		if	( memcmp(pt,"-xml",4) == 0 )
		{
			Xml	= 1;
			continue;
		}
		if	( memcmp(pt,"-shdico",7) == 0 )
		{
			XoLoadSharedDicoDir(".");
			continue;
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
		if	( memcmp(pt,"-m",2) == 0 )
		{
			nexttype	= 'm' ;
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
					DefaultLoadRef();
					nbr	= -1;
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
			case	'm':		/* donnees */
				if (nbr == 0)
				{
					DefaultLoadRef();
					nbr	= -1;
				}
				if	(fstat(f,&st) < 0)
				{
					fprintf(stderr,"cannot fstat '%s'\n",
							file) ;
					fflush(stderr) ;
					exit(1) ;
				}
				mem	= (char *)malloc(st.st_size);
				if	(!mem)
				{
					fprintf(stderr,"cannot alloc %d '%s'\n",
							(int)st.st_size,file) ;
					fflush(stderr) ;
					exit(1) ;
				}
				if	(read(f,mem,st.st_size) != st.st_size)
				{
					fprintf(stderr,"cannot read %d '%s'\n",
							(int)st.st_size,file) ;
					fflush(stderr) ;
					exit(1) ;
				}
				obj	= XoLoadMem(mem,st.st_size) ;
				if	( !obj )
				{
					fprintf(stderr,"\tload obj err : %s\n",
						file) ;	
					fflush(stderr) ;
					exit(1) ;
				}
				aff	(obj) ;
				XoFree	(obj,1);
				free	(mem);
			break ;
		}
	}

	XoEnd();
	exit	(0) ;
}

