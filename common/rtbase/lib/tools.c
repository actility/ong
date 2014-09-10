
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
#include 	<time.h>
#include 	<ctype.h>
#include 	<sys/timeb.h>
#include 	<sys/stat.h>

#include	<sys/time.h>
#include	<sys/resource.h>
#include	<unistd.h>
#include	<pthread.h>


#define		ABS(x)	((x) > 0 ? (x) : -(x))

typedef	struct
{
	unsigned long size,resident,share,text,lib,data,dt;
}	t_statm;

unsigned long rtl_vsize(int pid)
{
	t_statm	result;
	char	path[512];
	FILE	*f;

	if	(pid == 0)
		sprintf	(path,"/proc/self/statm");
	else
		sprintf	(path,"/proc/%d/statm",pid);

	f	= fopen(path,"r");
	if	(!f)
		return	0;

	if(7 != fscanf(f,"%ld %ld %ld %ld %ld %ld %ld",
		&result.size,&result.resident,&result.share,
		&result.text,&result.lib,&result.data,&result.dt))
	{
		fclose(f);
		return	0;
	}

	fclose(f);
	return	(result.size*1024*4);	// nbre de pages de 4Ko
}

unsigned int rtl_cpuloadavg()
{
	char	path[512];
	FILE	*f;
	float	result;

	sprintf	(path,"/proc/loadavg");
	f	= fopen(path,"r");
	if	(!f)
		return	0;

	if(1 != fscanf(f,"%f",&result))
	{
		fclose(f);
		return	0;
	}

	fclose(f);
	return	(unsigned long)(result*100);
}

int	rtl_pseudothreadid()	/* TS */
{
	return	(unsigned int) ((unsigned long)pthread_self()%9973);
}

void	rtl_enablecore(int flg)	/* TS */
{
	struct	rlimit	lim;

	if	( flg )
	{
		lim.rlim_cur	= RLIM_INFINITY;
		lim.rlim_max	= RLIM_INFINITY;
	}
	else
	{
		lim.rlim_cur	= 0;
		lim.rlim_max	= 0;
	}
	setrlimit(RLIMIT_CORE,&lim);
}

int	rtl_docore()
{
	int	ret;
	// TODO
	// fork() + abort()


	ret	= fork();
	if	(ret < 0)	// parent
		return	ret;	// error
	if	(ret > 0)	// parent
		return	ret;	// pid of child

	rtl_enablecore(1);
	// child
	abort();	
}


char	*rtl_aaaammjj(time_t t,char *buf)	/* TS */
{
	struct	timeb	tp ;
	struct	tm	*dt;

	if (t == 0)
	{
		ftime	(&tp) ;
		dt	= localtime(&tp.time);
	}
	else
		dt	= localtime(&t);
	sprintf(buf,"%04d%02d%02d", dt->tm_year+1900,dt->tm_mon+1,dt->tm_mday);

	return	buf ;
}

char	*rtl_hhmmss(time_t t,char *buf)		/* TS */
{
	struct	timeb	tp ;
	struct	tm	*dt;

	if (t == 0)
	{
		ftime	(&tp) ;
		dt	= localtime(&tp.time);
	}
	else
		dt	= localtime(&t);
	sprintf(buf,"%02d:%02d:%02d",dt->tm_hour,dt->tm_min,dt->tm_sec);

	return	buf ;
}

char	*rtl_aaaammjjhhmmss(time_t t,char *buf)	/* TS */
{
	struct	timeb	tp ;
	struct	tm	*dt;

	if (t == 0)
	{
		ftime	(&tp) ;
		dt	= localtime(&tp.time);
	}
	else
		dt	= localtime(&t);
	sprintf(buf,"%04d/%02d/%02d %02d:%02d:%02d",
		dt->tm_year+1900,dt->tm_mon+1,dt->tm_mday,
		dt->tm_hour,dt->tm_min,dt->tm_sec);

	return	buf ;
}

char	*rtl_hhmmssms(char *buf)	/* TS */
{
	struct	timeb	tp ;
	struct	tm	*dt;

	ftime	(&tp) ;
	dt	= localtime(&tp.time);
	sprintf(buf,"%02d:%02d:%02d.%03d ",
		dt->tm_hour,dt->tm_min,dt->tm_sec,tp.millitm);

	return	buf ;
}

char *rtl_binToStr(unsigned char *bin,int lg,char dst[],int max)	/* TS */
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

unsigned char	*rtl_strToBin(char *str,unsigned char bin[],int *lg)	/* TS */
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
