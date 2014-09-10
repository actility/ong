
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
#include <string.h>
#include <stdarg.h>
#include <stddef.h>
#include <unistd.h>
#include <pthread.h>
#include <errno.h>
#include <limits.h>
#include <signal.h>
#include <poll.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>

#include "rtlbase.h"
#include "rtllist.h"

#include "rtlhtbl.h"


#define	EXIT(v,s,...) \
{\
			printf("line=%d %s (%d)",__LINE__,(s),(v));\
			exit((1));\
}

#define	NB_THREAD	4

typedef	struct
{
	int		num;
	int		link;
	char		strkey[64];

	/* users data */
}	t_data;

t_data	TbData[100];

void	*HStr;	// hash string

int	NBELEM	= 10;
int	HASHSZ	= 3;

int	Count;

int	WalkCount(void *h,char *k,void *d,void *u)
{
	Count++;
	return	0;
}

int	WalkPrint(void *h,char *k,void *d,void *u)
{
	int	f	= rtl_htblGetFlags(h);
	t_data	*data	= (t_data *)d;

	if	( (f & HTBL_KEY_STRING) == HTBL_KEY_STRING )
	{
		printf	("k=%s d=%s\n",k,data->strkey);
		return	0;
	}
	return	0;
}

void	CbRemove(void *h,char *k,void *d)
{
	t_data	*data	= (t_data *)d;

	data->link--;
}

void	*RunThread(void *pth)
{
	int	num = (int)pth;
	int	mod = num % NB_THREAD;
	int	i;
	char	key[128];
	t_data	*data;
	int	ret;

//	printf	("num thread %d mod=%d\n",num,mod);

#if	1
	for	(i = 0 ; i < NBELEM ; i++)
	{
		if	(i % NB_THREAD != mod)
			continue;
		data		= &TbData[i];
		data->num	= i;
		data->link	= 0;
		sprintf	(data->strkey,"%08d",i);
		if	((ret=rtl_htblAdd(HStr,data)) < 0)
			EXIT(ret,"error rtl_htblInsert str\n");
		data->link++;
	}
	for	(i = 0 ; i < NBELEM ; i++)
	{
		if	(i % NB_THREAD != mod)
			continue;
		sprintf	(key,"%08d",i);
		data	= rtl_htblGet(HStr,key);
		if	(!data)
			EXIT(1,"error rtl_htblGet\n");
		if	(strcmp(data->strkey,key) != 0)
			EXIT(1,"error strcmp\n");
		if	(rtl_htblRemove(HStr,key) < 0)
			EXIT(1,"error rtl_htblRemove\n");
	}
#endif


	pthread_exit(NULL);
	return	NULL;
}

int	main(int argc,char *argv[])
{
	int	i;
	int	offset;
	t_data	*data;

	pthread_t	th[NB_THREAD];

retry :

	printf	("--NBELEM=%d    HASHSZ=%d--\n",NBELEM,HASHSZ);

	HStr	= rtl_htblCreateSpec(HASHSZ,NULL,
			HTBL_KEY_STRING|HTBL_KEY_INCLUDED|HTBL_USE_MUTEX);
	if	(!HStr)
		EXIT(1,"error rtl_htblNew\n");
	offset	= offsetof(t_data,strkey);
	if	(rtl_htblSetKeyOffset(HStr,offset) < 0)
		EXIT(1,"error rtl_htblSetKeyOffset\n");
	rtl_htblSetRmovFunc(HStr,CbRemove);


	for	(i = 0 ; i < NB_THREAD ; i++)
	{
		if (pthread_create(&th[i],NULL,RunThread,(void *)i))
		{
			EXIT(1,"cannot create thread\n");
		}
	}


	for	(i = 0 ; i < NB_THREAD ; i++)
	{
		void	*res;
		if (pthread_join(th[i],&res))
		{
			EXIT(1,"cannot join thread\n");
		}
		if	(res)
			free(res);
//		printf	("end of thread %d\n",i);
	}


	if	(rtl_htblGetCount(HStr) != 0)
		EXIT(1,"error GetCount != 0\n");

	rtl_htblDestroy(HStr);

	for	(i = 0 ; i < NBELEM ; i++)
	{
		data		= &TbData[i];
		if	(data->link != 0)
			EXIT(1,"error link count != 0\n");
	}


	///////////////////////////////////////////////////////////////////
	//

	printf	("---------------------------------------------\n");

	NBELEM	= (rand() % 10 ) + 1;
	HASHSZ	= (rand() % 10 ) + 1;

	goto	retry;

	exit(0);
}
