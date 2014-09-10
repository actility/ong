
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

#define	BASE_KEY	INT_MAX

typedef	struct
{
	int		num;
	int		link;
	unsigned int	intkey;
	char		strkey[64];
	unsigned int	intkey2;

	/* users data */
}	t_data;

t_data	TbData[100];

void	*HStr;	// hash string
void	*HInt;	// hash integer
void	*HInt2;	// hash integer

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
	if	( (f & HTBL_KEY_UINT) == HTBL_KEY_UINT )
	{
		printf	("k=%d d=%d\n",(unsigned int)k,data->intkey);
		return	0;
	}
	return	0;
}

void	CbRemove(void *h,char *k,void *d)
{
	t_data	*data	= (t_data *)d;

	data->link--;
}

int	main(int argc,char *argv[])
{
	unsigned int	i;
	char	key[128];
	int	offset;
	int	ret;
	int	nb;

	t_data	*data;

retry :

	printf	("--NBELEM=%d    HASHSZ=%d--\n",NBELEM,HASHSZ);

#if	1
	HStr	= rtl_htblCreateSpec(HASHSZ,NULL,
					HTBL_KEY_STRING|HTBL_KEY_INCLUDED);
	if	(!HStr)
		EXIT(1,"error rtl_htblNew\n");
	offset	= offsetof(t_data,strkey);
	if	(rtl_htblSetKeyOffset(HStr,offset) < 0)
		EXIT(1,"error rtl_htblSetKeyOffset\n");
	rtl_htblSetRmovFunc(HStr,CbRemove);



	HInt	= rtl_htblCreateSpec(HASHSZ,NULL,
					HTBL_KEY_UINT|HTBL_KEY_INCLUDED);
	if	(!HInt)
		EXIT(1,"error rtl_htblNew\n");
	offset	= offsetof(t_data,intkey);
	if	(rtl_htblSetKeyOffset(HInt,offset) < 0)
		EXIT(1,"error rtl_htblSetKeyOffset\n");
	rtl_htblSetRmovFunc(HInt,CbRemove);

	HInt2	= rtl_htblCreateSpec(HASHSZ,NULL,
					HTBL_KEY_UINT|HTBL_KEY_INCLUDED);
	if	(!HInt2)
		EXIT(1,"error rtl_htblNew\n");
	offset	= offsetof(t_data,intkey2);
	if	(rtl_htblSetKeyOffset(HInt2,offset) < 0)
		EXIT(1,"error rtl_htblSetKeyOffset\n");
	rtl_htblSetRmovFunc(HInt2,CbRemove);

	for	(i = 0 ; i < NBELEM ; i++)
	{
		data		= &TbData[i];
		data->num	= i;
		data->link	= 0;
		data->intkey	= i+BASE_KEY;
		data->intkey2	= i;
		sprintf	(data->strkey,"%08d",i);
		if	((ret=rtl_htblAdd(HStr,data)) < 0)
			EXIT(ret,"error rtl_htblInsert str\n");
		data->link++;
/*
printf	("insert h=%p data=%p num=%d\n",HInt,data,data->num);
*/
		if	(rtl_htblAdd(HInt,data) < 0)
			EXIT(ret,"error rtl_htblAdd int\n");
		data->link++;

		if	(rtl_htblAdd(HInt2,data) < 0)
			EXIT(ret,"error rtl_htblAdd int\n");
		data->link++;
	}
/*
	rtl_htblWalk(HStr,WalkPrint,NULL);
	rtl_htblWalk(HInt,WalkPrint,NULL);
*/
	for	(i = 0 ; i < NBELEM ; i++)
	{
		sprintf	(key,"%08d",i);
		data	= rtl_htblGet(HStr,key);
		if	(!data)
			EXIT(1,"error rtl_htblGet\n");
		if	(strcmp(data->strkey,key) != 0)
			EXIT(1,"error strcmp\n");
		if	(rtl_htblRemove(HStr,key) < 0)
			EXIT(1,"error rtl_htblRemove\n");
	}

	for	(i = NBELEM-1 ; (int)i >= 0 ; i--)
	{
		data	= rtl_htblGet(HInt,(char *)i+BASE_KEY);
		if	(!data)
			EXIT(1,"error rtl_htblGet\n");
		if	(data->intkey != i+BASE_KEY)
			EXIT(1,"error !=\n");
		if	(rtl_htblRemove(HInt,(char *)i+BASE_KEY) < 0)
			EXIT(1,"error rtl_htblRemove\n");

	}

	nb	= 0;
	while	(rtl_htblGetCount(HInt2) != 0)
	{
		nb++;
		i	= rand() % NBELEM;
		data	= rtl_htblGet(HInt2,(char *)i);
		if	(!data)
		{
			if	(nb >= 1000000)
				EXIT(1,"error random remove\n");

			continue;
		}
		if	(data->intkey2 != i)
			EXIT(1,"error !=\n");
		if	(rtl_htblRemove(HInt2,(char *)i) < 0)
			EXIT(1,"error rtl_htblRemove\n");

	}
	Count	= 0;
	rtl_htblWalk(HStr,WalkCount,NULL);
	if	(Count != 0)
		EXIT(1,"error count != 0\n");

	if	(rtl_htblGetCount(HStr) != 0)
		EXIT(1,"error GetCount != 0\n");

	rtl_htblWalk(HStr,WalkPrint,NULL);
	rtl_htblDestroy(HStr);

	if	(rtl_htblGetCount(HInt) != 0)
		EXIT(1,"error GetCount != 0\n");

	rtl_htblWalk(HInt,WalkPrint,NULL);
	rtl_htblDestroy(HInt);

	rtl_htblWalk(HInt2,WalkPrint,NULL);
	rtl_htblDestroy(HInt2);

	for	(i = 0 ; i < NBELEM ; i++)
	{
		data		= &TbData[i];
		if	(data->link != 0)
			EXIT(1,"error link count != 0\n");
	}


#endif
	///////////////////////////////////////////////////////////////////
	//

	printf	("---------------------------------------------\n");

	NBELEM	= (rand() % 10 ) + 1;
	HASHSZ	= (rand() % 10 ) + 1;

	goto	retry;

	exit(0);
}
