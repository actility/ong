
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

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <time.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/stat.h>

#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <err.h>

void	AwPrintBin(FILE *f,unsigned char *bin,int lg)	/* TS */
{
	int	i;

	fprintf(f,"[");
	for (i = 0 ; i < lg ; i++)
		fprintf(f,"%02x ",bin[i]);
	fprintf(f,"]\n");
	fflush(f);
}

char *AwBinToStr(unsigned char *bin,int lg,char dst[],int max)	/* TS */
{
	int	i;
	int	j = 0;

	static	char	*tb	= "0123456789abcdef";

	for (i = 0 ; i < lg && j < max ; i++)
	{
		dst[j++]	= tb[(unsigned char)(bin[i]/16)];
		dst[j++]	= tb[(unsigned char)(bin[i]%16)];
//printf("%c %c\n",dst[j-2],dst[j-1]);
	}
	dst[j++]	= '\0';
	return	dst;
}

unsigned char	*AwStrToBin(char *str,unsigned char bin[],int *lg)	/* TS */
{
	int	max = *lg;
	int	i = 0;

	*lg	= 0;
	while (str && *str && i < max)
	{
		int	x;
		char	tmp[3];

		if (!isxdigit(*str))	{ str++ ; continue; }

		tmp[0]	= *str++;
		if (!*str)	break;
		tmp[1]	= *str++;
		tmp[2]	= '\0';
		if (sscanf(tmp,"%x",&x) == 1)
		{
			bin[i]	= (unsigned char)x;
			i++;
		}
		else
			break;
	}
	*lg	= i;
	bin[i]	= '\0';
#if	0
printf("[");
for (i = 0 ; i < *lg ; i++)
	printf("%02x ",bin[i]);
printf("]\n");
#endif
	return	bin;
}

int AwFileCmp(char *f1,char *f2) /* TS */
{
	int	fd1 = -1;
	int	fd2 = -1;
	struct	stat	st1;
	struct	stat	st2;

	char	buf1[1024];
	char	buf2[1024];
	int	ret;
	int	sz;

	ret	= 0;

	fd1	= open(f1,0);
	if	(fd1 < 0)
	{
		ret	= -1;
		goto	fctend;
	}
	if	(fstat(fd1,&st1) < 0)
	{
		ret	= -11;
		goto	fctend;
	}

	fd2	= open(f2,0);
	if	(fd2 < 0)
	{
		ret	= -2;
		goto	fctend;
	}
	if	(fstat(fd2,&st2) < 0)
	{
		ret	= -22;
		goto	fctend;
	}

	if	(st1.st_size != st2.st_size)
	{
		ret	= 1;
		goto	fctend;
	}

	while	((sz=read(fd1,buf1,sizeof(buf1))) > 0)
	{
		if	(read(fd2,buf2,sizeof(buf2)) != sz)
		{
			ret	= -222;
			goto	fctend;
		}
		if	(memcmp(buf1,buf2,sz))
		{
			ret	= 2;
			goto	fctend;
		}
	}
	if	(sz < 0)
	{
		ret	= -111;
		goto	fctend;
	}

fctend :

	if	(fd1 > 0)
		close(fd1);
	if	(fd2 > 0)
		close(fd2);

	return	ret;

}

