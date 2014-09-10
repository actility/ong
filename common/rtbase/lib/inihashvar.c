
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
#include 	<sys/timeb.h>
#include 	<sys/stat.h>

#include	<sys/time.h>
#include	<sys/resource.h>
#include	<unistd.h>
#include	<pthread.h>

#include	"rtlbase.h"
#include	"rtlhtbl.h"

char	*rtl_iniStr(void *ht,char *sec,int index,char *var,char *def)
{
	char	path[256];
	char	tmp[256];
	char	*pt;
	char	*val;

	pt	= path;
	path[0]	= '\0';
	if	(sec && *sec)
		strcat(path,sec);	
	if	(index >= 0)
	{
		sprintf	(tmp,":%d",index);
		strcat(path,tmp);	
	}
	if	(strlen(path))
		strcat(path,".");
	strcat(path,var);	
	if	(*pt == '.')
		pt++;

	val	= rtl_htblGet(ht,pt);
	if	(!val)
		return	def;
	return	val;
}

int	rtl_iniInt(void *ht,char *sec,int index,char *var,int def)
{
	char	more[256];
	char	*val;
	int	ret	= 0;
	int	hex	= 0;

	val	= rtl_iniStr(ht,sec,index,var,NULL);
	if	(!val || !*val)
		return	def;

retry:
	if	(*val == '0' && (*(val+1) == 'x' || *(val+1) == 'X'))
	{
		more[0]	= '\0';
		sscanf(val+2,"%x%s",&hex,more);
//RTL_TRDBG(1,"'%s:%d.%s' 0x%x\n",sec,index,var,hex);
		ret	= ret | hex;
		if	(strlen(more))
		{
			val	= more;
			if	(*val == '|')
			{
				val++;
				goto	retry;
			}
			return	ret;
		}
		return	ret;
	}
	ret	= atoi(val);
	if	(ret == 0)
	{
		if	(strcasecmp(val,"true") == 0)
			return	1;
		if	(strcasecmp(val,"false") == 0)
			return	0;
	}
	return	ret;
}

static	char *CfgCBIniFctVar(void *user,char *var)
{
	char	*val;
	val	= rtl_htblGet(user,var);
	if	(!val)
		val	= (char *)getenv(var);
	if	(!val)
		val	= NULL;
	return	val;
}

int rtl_iniLoadCB(void *user,const char *section,const char *name,const char *value)
{
	char	bvar[256];
	char	buf[256];
	char	*var;
	char	*nvalue;


	nvalue	= value;
	sprintf	(bvar,"%s.%s",section,name);
	var	= bvar;
	if	(*var == '.')	// because section can be empty
		var++;

	if	(!nvalue||!*nvalue)
	{
		nvalue	= "";
	}

//	RTL_TRDBG(9,"var='%s' val='%s'\n",var,nvalue);
	if	(nvalue && *nvalue && *nvalue == '$')
	{

		nvalue	= rtl_evalPath2(nvalue,buf,sizeof(buf)-1,
							CfgCBIniFctVar,user);
	}
	rtl_htblRemove(user,var);
	rtl_htblInsert(user,var,(void *)strdup(nvalue));
	return	1;
}
