
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
			printf("%d %s",__LINE__,(s));\
			exit((v));\
}

/*
#define	NBELEM	(100000)
#define	HASHSZ	(9999)
*/

int	NBELEM	= 100;
int	HASHSZ	= 11;

int	Count;

void	DataFree(void *h,char *k,void *d)
{
//	printf	("free k=%d d=%s\n",(size_t)k,(char *)d);
	free(d);
}

int	WalkCount(void *h,char *k,void *d,void *u)
{
	Count++;
	return	0;
}

int	WalkPrint(void *h,char *k,void *d,void *u)
{
	int	f	= rtl_htblGetFlags(h);

	if	( (f & HTBL_KEY_STRING) == HTBL_KEY_STRING )
	{
		printf	("k=%s d=%s\n",k,(char *)d);
		return	0;
	}
	if	( (f & HTBL_KEY_UINT) == HTBL_KEY_UINT )
	{
		printf	("k=%d d=%s\n",(size_t)k,(char *)d);
		return	0;
	}
	return	0;
}

int	WalkDeleteOdd(void *h,char *k,void *d,void *u)
{
	int	f	= rtl_htblGetFlags(h);

	if	( (f & HTBL_KEY_STRING) == HTBL_KEY_STRING )
	{
		if	(atoi(k) % 2 == 0)
		{
			rtl_htblRemove(h,k);
			free(d);
		}
		return	0;
	}
	if	( (f & HTBL_KEY_UINT) == HTBL_KEY_UINT )
	{
		if	(((size_t)k) % 2 == 0)
		{
			rtl_htblRemove(h,k);
		}
		return	0;
	}
	return	0;
}

int	WalkDeleteEven(void *h,char *k,void *d,void *u)
{
	int	f	= rtl_htblGetFlags(h);

	if	( (f & HTBL_KEY_STRING) == HTBL_KEY_STRING )
	{
		if	(atoi(k) % 2 != 0)
		{
			rtl_htblRemove(h,k);
			free(d);
		}
		return	0;
	}
	if	( (f & HTBL_KEY_UINT) == HTBL_KEY_UINT )
	{
		if	(((size_t)k) % 2 != 0)
		{
			rtl_htblRemove(h,k);
		}
		return	0;
	}
	return	0;
}


int	main(int argc,char *argv[])
{
	void	*h;
	int	i;
	char	key[128];
	void	*data;
	int	offset;
	int	ret;

retry :

	printf	("--NBELEM=%d    HASHSZ=%d--\n",NBELEM,HASHSZ);

#if	1
	offset	= 0;
	h	= rtl_htblNew(HASHSZ);
	if	(!h)
		EXIT(1,"error rtl_htblNew\n");

	for	(i = 0 ; i < NBELEM ; i++)
	{
		sprintf	(key,"%08d",i);
		data	= strdup(key);
		if	((ret=rtl_htblInsert(h,key,data)) < 0)
		{
			printf("ret=%d\n",ret);
			EXIT(1,"error rtl_htblInsert\n");
		}
	}
	for	(i = 0 ; i < NBELEM ; i++)
	{
		sprintf	(key,"%08d",i);
		data	= rtl_htblGet(h,key);
		if	(!data)
			EXIT(1,"error rtl_htblGet\n");
		if	(strcmp(data,key) != 0)
			EXIT(1,"error strcmp\n");
	}
//	for	(i = 0 ; i < NBELEM ; i++)
	for	(i = 0 ; i < NBELEM - (((NBELEM)*50)/100) ; i++)
	{
		sprintf	(key,"%08d",i);
		data	= rtl_htblGet(h,key);
		if	(!data)
			EXIT(1,"error rtl_htblGet\n");
		if	(strcmp(data,key) != 0)
			EXIT(1,"error strcmp\n");
		if	(rtl_htblRemove(h,key) < 0)
			EXIT(1,"error rtl_htblRemove\n");
		free	(data);
	}

	rtl_htblWalk(h,WalkDeleteOdd,NULL);
	rtl_htblWalk(h,WalkDeleteEven,NULL);

	Count	= 0;
	rtl_htblWalk(h,WalkCount,NULL);
	if	(Count != 0)
		EXIT(1,"error count != 0\n");

	if	(rtl_htblGetCount(h) != 0)
		EXIT(1,"error GetCount != 0\n");

	rtl_htblWalk(h,WalkPrint,NULL);
	rtl_htblDestroy(h);
#endif

	///////////////////////////////////////////////////////////////////

#if	1
	h	= rtl_htblCreateSpec(HASHSZ,NULL,HTBL_KEY_UINT);
	if	(!h)
		EXIT(1,"error rtl_htblNew\n");

	rtl_htblSetRmovFunc(h,DataFree);

	for	(i = 0 ; i < NBELEM ; i++)
	{
		sprintf	(key,"%08d",i);
		data	= strdup(key);
		if	((ret=rtl_htblInsert(h,(char *)i,data)) < 0)
		{
			printf	("ret=%d i=%d\n",ret,i);
			EXIT(1,"error rtl_htblInsert\n");
		}
	}
	for	(i = 0 ; i < NBELEM ; i++)
	{
		sprintf	(key,"%08d",i);
		data	= rtl_htblGet(h,(char *)i);
		if	(!data)
			EXIT(1,"error rtl_htblGet\n");
		if	(strcmp(data,key) != 0)
			EXIT(1,"error strcmp\n");
	}

	rtl_htblWalk(h,WalkDeleteOdd,NULL);
	rtl_htblWalk(h,WalkDeleteEven,NULL);

	Count	= 0;
	rtl_htblWalk(h,WalkCount,NULL);
	if	(Count != 0)
		EXIT(1,"error count != 0\n");

	if	(rtl_htblGetCount(h) != 0)
		EXIT(1,"error GetCount != 0\n");

	rtl_htblWalk(h,WalkPrint,NULL);
	rtl_htblDestroy(h);
#endif
	
	///////////////////////////////////////////////////////////////////
	//
#if	1
	offset	= 4;
	h	= rtl_htblCreateSpec(HASHSZ,NULL,HTBL_KEY_STRING|HTBL_KEY_INCLUDED);
	if	(!h)
		EXIT(1,"error rtl_htblNew\n");

	if	(rtl_htblSetKeyOffset(h,offset) < 0)
		EXIT(1,"error rtl_htblSetKeyOffset\n");

	for	(i = 0 ; i < NBELEM ; i++)
	{
		sprintf	(key,"XXXX%08d",i);	// key starts at offset=4
		data	= strdup(key);
		if	(rtl_htblAdd(h,data) < 0)
			EXIT(1,"error rtl_htblInsert\n");
	}
	for	(i = 0 ; i < NBELEM ; i++)
	{
		sprintf	(key,"%08d",i);
		data	= rtl_htblGet(h,key);
		if	(!data)
			EXIT(1,"error rtl_htblGet\n");
		if	(strcmp(data+offset,key) != 0)
			EXIT(1,"error strcmp\n");
	}
	for	(i = 0 ; i < NBELEM - (((NBELEM)*50)/100) ; i++)
	{
		sprintf	(key,"%08d",i);
		data	= rtl_htblGet(h,key);
		if	(!data)
			EXIT(1,"error rtl_htblGet\n");
		if	(strcmp(data+offset,key) != 0)
			EXIT(1,"error strcmp\n");
		if	(rtl_htblRemove(h,key) < 0)
			EXIT(1,"error rtl_htblRemove\n");
		free	(data);
	}

	rtl_htblWalk(h,WalkDeleteOdd,NULL);
	rtl_htblWalk(h,WalkDeleteEven,NULL);

	Count	= 0;
	rtl_htblWalk(h,WalkCount,NULL);
	if	(Count != 0)
		EXIT(1,"error count != 0\n");

	if	(rtl_htblGetCount(h) != 0)
		EXIT(1,"error GetCount != 0\n");

	rtl_htblWalk(h,WalkPrint,NULL);
	rtl_htblDestroy(h);


#endif
	///////////////////////////////////////////////////////////////////
	//
#if	1
	h	= rtl_htblCreateSpec(HASHSZ,NULL,
		HTBL_KEY_STRING|HTBL_KEY_INCLUDED|HTBL_FREE_DATA);
	if	(!h)
		EXIT(1,"error rtl_htblNew\n");

	if	(rtl_htblSetKeyOffset(h,0) < 0)
		EXIT(1,"error rtl_htblSetKeyOffset\n");

	for	(i = 0 ; i < NBELEM ; i++)
	{
		sprintf	(key,"%08d",i);
		data	= strdup(key);
		if	(rtl_htblAdd(h,data) < 0)
			EXIT(1,"error rtl_htblInsert\n");
	}
	for	(i = 0 ; i < NBELEM ; i++)
	{
		sprintf	(key,"%08d",i);
		data	= rtl_htblGet(h,key);
		if	(!data)
			EXIT(1,"error rtl_htblGet\n");
		if	(strcmp(data,key) != 0)
			EXIT(1,"error strcmp\n");
		if	(rtl_htblRemove(h,key) < 0)
			EXIT(1,"error rtl_htblRemove\n");
//		free	(data); because FREE_DATA
	}

	Count	= 0;
	rtl_htblWalk(h,WalkCount,NULL);
	if	(Count != 0)
		EXIT(1,"error count != 0\n");

	if	(rtl_htblGetCount(h) != 0)
		EXIT(1,"error GetCount != 0\n");

	rtl_htblWalk(h,WalkPrint,NULL);
	rtl_htblDestroy(h);


#endif
	///////////////////////////////////////////////////////////////////


	printf	("---------------------------------------------\n");

	NBELEM	= (rand() % 10 ) + 1;
	HASHSZ	= (rand() % 10 ) + 1;

	goto	retry;

	exit(0);
}
