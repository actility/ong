
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
#include	<sys/types.h>
#include	<sys/stat.h>
#include	<unistd.h>

#include	"xoapipr.h"

int RLE_Compress( unsigned char *in, unsigned char *out,
    unsigned int insize );

t_objet_ref	*XoTabObjUser ;
int		XoNbObjUser  = 0 ;


int32	Count(int c,char *pt,int lg)
{
	int32	nb = 0;

	while	(lg-- > 0)
	{
		if	(*pt == c)
			nb++;
		pt++;
	}
	return	nb;
}

int32	CountMax(char *pt,int lg)
{
	int	tab[256];
	int	svlg;
	int	i;
	float	tx;

	memset	(tab,0,sizeof(tab));

	svlg	= lg;
	while	(lg-- > 0)
	{
		tab[(unsigned char)*pt]++;
		pt++;
	}
	lg	= svlg;

	for	(i = 0 ; i <= 255 ; i++)
	{
		tx	= (float)tab[i] / (float)lg;
//printf("tx %d => %f\n",i,tx,tab[i],lg);
		if	( tx >= 0.2)
			return	i;
	}
	return	-1;
}


int	main(int argc,char *argv[])
{
	struct	stat st;
	char	*file;
	int32	i;
	int32	fd;
	int32	szin;
	int32	svszin;
	int32	szout;
	int32	szucp;
	int32	c0;
	char	*in;
	char	*svin;
	char	*out;
	char	*vfy;
	float	txout;

	if	( argc < 2 )
		exit	(1) ;

	XoInit	(0) ;

	for	(i = 1 ; i < argc ; i++)
	{
	file	= argv[i];
	fd	= open(file,0);
	if	(fd < 0)
	{
		printf("cannot not open file '%s'\n",file);
		exit(1);
	}
	if	(fstat(fd,&st) < 0)
	{
		printf("cannot not fstat file '%s'\n",file);
		exit(1);
	}
	szin	= st.st_size;
	if	(szin <= 16)
	{
		printf("file too small '%s'\n",file);
		exit(1);
	}
	printf	("%s szin=%d\n",file,szin);
	in	= (char *)malloc(szin);
	out	= (char *)malloc(szin+100);
	vfy	= (char *)malloc(szin+100);
	if	(!in || !out || !vfy)
	{
		printf("cannot alloc buffers in/out %d\n",szin);
		exit(1);
	}
	if	(read(fd,in,szin) != szin)
	{
		printf("cannot not read file '%s'\n",file);
		exit(1);
	}
	svin	= in;
	svszin	= szin;
	in	= in + 12;
	szin	= szin - 12;
	c0	= XoMemCanDeflate(in,szin);
	if	(c0 <= 0)
	{
		printf("compression 0 not effective\n");
		continue;
	}

	szout	= XoMemDeflate(in,szin,out);
	if	(!szout)
	{
		printf("compression 0 error\n");
		exit(1);
	}
	txout	= (1.0 - (float)szout/(float)szin);
	printf	("def : szout=%d bitsleft=%d txout=%.2f c0=%d\n",
			szout,*out,txout,c0);


	szucp	= XoMemInflate(out,szout,vfy+12);
	printf	("inf : szucp=%d szin=%d\n",szucp,szin);
	if	(szucp != szin)
	{
		printf("decompression error szucp=%d != szin=%d\n",szucp,szin);
		exit(1);
	}

	memcpy	(vfy,svin,12);
	if	(memcmp(svin,vfy,svszin) != 0)
	{
		printf("decompression error bufin != bufout\n");
		exit(1);
	}

	printf("OK\n");

	free	(svin);
	free	(out);
	free	(vfy);
	close	(fd);
	}

	XoEnd() ;
	exit	(0) ;
}

