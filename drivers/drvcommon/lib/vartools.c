
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

/*! @file var.c
 *
 */
#include <stdio.h>			/************************************/
#include <stdlib.h>			/*     run in main thread           */
#include <string.h>			/************************************/
#include <unistd.h>
#include <pthread.h>
#include <errno.h>
#include <limits.h>
#include <signal.h>
#include <poll.h>
#include <ctype.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>

#include "rtlbase.h"
#include "rtlimsg.h"
#include "xoapipr.h"

/*ML-20121001-retargeting-response+*/
#include "dIa.h"
/*ML-20121001-retargeting-response-*/

#include "cmndefine.h"
#include "cmnstruct.h"

#include "cmnproto.h"


extern	t_var	TbVar[MAX_VAR];
extern	int	NbVar;
extern	int	InternalVar;


static	int	objcmp(const void *o1, const void *o2)
{
	t_var	*v1	= (t_var *)o1;
	t_var	*v2	= (t_var *)o2;

	return	strcmp(v1->v_name,v2->v_name);
}

static	void	VarSort()
{
	qsort(TbVar,(size_t)NbVar,sizeof(t_var),objcmp);
}

static	t_var	*FindVar(char *name)
{
	int	l , h , m ;
	int	ret ;
	t_var	*v;

	l	= 0 ;
	h	= NbVar - 1 ;
	while	( l <= h )
	{
		m	= ( l + h ) / 2 ;
		v	= &TbVar[m] ;
		ret	= strcmp(name,v->v_name) ;
/*
fprintf(stderr,"[%s] =?= [%s]\n",name,v->v_name) ;
*/
		if	( ret < 0 )
			h	= m - 1 ;
		else
			if	( ret > 0 )
				l	= m + 1 ;
			else
				return	v ;
	}
	return	NULL;
}

/*!
 *
 * @brief add a variable to the predefined variables
 * 	internaly call VarInit() to sort variables by names.
 * @param name the name of the variable
 * @return void
 * 
 */
void	AddVar(char *name)
{
	t_var	*v;

	if	(!name)
		return;

	v	= FindVar(name);
	if	(v)
	{
		RTL_TRDBG(0,"ERROR var '%s' already exists\n",name);
		return;
	}

	if	(NbVar >= MAX_VAR)
	{
/*ML-20121001-retargeting-response+*/
//		RTL_TRDBG(0,"ERROR var '%s' max var reached\n",name,NbVar);
		RTL_TRDBG(0,"ERROR var '%s' max var reached (%d)\n",name,NbVar);
/*ML-20121001-retargeting-response-*/
		return;
	}

	v	= &TbVar[NbVar];
	memset	(v,0,sizeof(t_var));

	v->v_name	= strdup(name);
	v->v_value	= strdup("");

	NbVar++;

	VarSort();
}

/*!
 *
 * @brief change the value of a variable
 * @param name the name of the variable
 * @param value the new value (NULL is not a possible value)
 * @return 
 * 	- "" on errors
 * 	- the value otherwise
 * 
 */
char	*SetVar(char *name,char *value)
{
	t_var	*v;

	if	(!value)
		return	"";

	v	= FindVar(name);
	if	(!v)
	{
		RTL_TRDBG(0,"ERROR var '%s' not found\n",name);
		return	"";
	}
	if	(v->v_fct)
	{
		RTL_TRDBG(0,"ERROR cannot set var '%s' hard coded\n",name);
		return	"";
	}
	if	(v->v_value)
		free(v->v_value);

	v->v_value	= strdup(value);

	RTL_TRDBG(5,"set var '%s' with '%s'\n",v->v_name,v->v_value);

	return	v->v_value;
}

/*!
 *
 * @brief return the value of a variable
 * 	- if name starts with '$' getenv(3) is used
 *	- if variable is driven by a function return the result of this function
 *	- if variable has a current value return this value
 *	- otherwise return the default value if any
 * @param name the name of the variable
 * @return 
 * 	- "" on errors
 * 	- the value otherwise
 * 
 */
char	*_GetVar(char *name,int warn)
{
	char	*sv	= name;
	char	*pt;
	char	var[128];
	int	i;
	t_var	*v;

	if	(!name || !*name)
		return	"";

	if	(!InternalVar)
		RTL_TRDBG(5,"search var '%s'\n",sv);

	i	= 0;
	var[i]	= '\0';
	while	(*name)
	{
		if	( isalnum(*name) || *name == '_' || *name == '.'
						|| (i == 0 && *name == '$') )
		{
			var[i++]	= *name;
		}
		name++;
	}
	var[i]	= '\0';

	if	(strlen(var) == 0)
	{
		RTL_TRDBG(0,"ERROR var '%s'\n",sv);
		return	"";
	}

	if	(var[0] == '$')	// ${$PATH}
	{
		pt	= getenv(var+1);
		if	(pt && *pt)
		{
			RTL_TRDBG(2,"var '%s' found in env\n",sv);
			return	pt;
		}
		RTL_TRDBG(0,"ERROR var '%s' not found in env\n",sv);
	}

	v	= FindVar(var);
	if	(!v)
	{
		if	(warn)
			RTL_TRDBG(0,"ERROR var '%s' not found\n",sv);
		return	"";
	}

	if	( v->v_fct )
	{
		return	(*v->v_fct)();
	}

	if	( v->v_value )
	{
		return	v->v_value;
	}

	if	( v->v_default )
	{
		return	v->v_default;
	}

	RTL_TRDBG(0,"ERROR var '%s' bad configuration\n",v->v_name);
	return	"";

}

char	*GetVar(char *name)
{
	return	_GetVar(name,1);
}

char	*GetVarNoWarn(char *name)
{
	return	_GetVar(name,0);
}


/*!
 *
 * @brief intialize the variables module
 * @return void
 * 
 */
void	VarInit()
{
	int	i;

	NbVar	= 0;
	for	(i = 0 ; i < MAX_VAR ; i++)
	{
		if	( TbVar[i].v_name && strlen(TbVar[i].v_name))
			NbVar++;
	}
	RTL_TRDBG(1,"var %d variables defined max=%d\n",NbVar,MAX_VAR);
	VarSort();
}


void	VarDump(void *cl)
{
	int	i;
	char	*val;

	for	(i = 0 ; i < NbVar ; i++)
	{
		val	= GetVar(TbVar[i].v_name);
		AdmPrint(cl,"%-20.20s '%s'\n",TbVar[i].v_name,val);
	}
}
