
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

#include	"xoapipr.h"




#define	HEAD	"<?xml version=\"1.0\" encoding=\"UTF-8\" ?>"

// TODO
/*
 *	'&'	&amp;
 *	'<'	&lt;
 *	'>'	&gt;
 *	'"'	&quot;
 *	'''	&apos;
 */


#define	NICE_CHAR(c)	( (c) > ' ' && (c) < 127 && (c) != '<' && (c) != '>' \
	&& (c) != '&')

static	char	*xo_file = __FILE__ ;

static	char	StartFlux[128] ;
static	char	EndFlux[128] ;


/*
 *	partie sauvegarde 
 */
static	int32	xosaveobj(char *tag,void *obj,int32 i,FILE *fout,int32 level);
static	int32	xosaveattr(void *obj, int32 numattr, 
						FILE *fout, int32 level) ;
static	void xosavebasicvalue(char *tag,int32 type, char *value, int32 lg,
				int32 i,FILE *fout, int32 level) ;

static	void	local_tab(int32 lev, FILE *fout)
{
	char	tmp[128] ;
	int	i;

	for	(i = 0 ; i < 2*lev ; i++)
	{
		tmp[i]	= ' ';
	}
	tmp[i]	= '\0';

	fprintf(fout,"%s",tmp) ;
}


static	void xosavestringvalue(char *tag,char *value, FILE *fout, int32 level)
{
	if	(!tag || !*tag)			// PSEUDO_ATTR
		fprintf(fout,"%s",value) ;	// TODO escape
	else
	{
		local_tab(level,fout) ;
		fprintf(fout,"<%s>%s</%s>\n",tag,value,tag) ;	// TODO escape
	}
}

static	void xosavebuffervalue(char *tag,char *value, int32 lg, FILE *fout, int32 level)
{
	if	(!tag || !*tag)			// PSEUDO_ATTR
		fprintf(fout,"%s",value) ;	// TODO escape
	else
	{
		local_tab(level,fout) ;
		fprintf(fout,"<%s>%s</%s>\n",tag,value,tag) ;	// TODO escape
	}
}

static	void xosavebasicvalue(char *tag,int32 type, char *value, int32 lg, int32 inList, FILE *fout, int32 level)
{
	int32	l ;
	unsigned	char	c ;

	if	(tag && strcmp(tag,XOML_PSEUDO_ATTR)==0)
		tag	= NULL;

	switch	(type)
	{
	case	XoBool :
	case	XoInt4B:
	case	XoDate :
		l	= *(int32 *)value ;
		if	(!tag || !*tag)			// PSEUDO_ATTR
			fprintf(fout,"%d",l) ;
		else
		{
			local_tab(level,fout) ;
			fprintf(fout,"<%s>%d</%s>\n",tag,(int32)l,tag) ;
		}
	break ;
	case	XoChar :
		c	= *(char *)value ;
		local_tab(level,fout) ;
		if	( NICE_CHAR(c) )
		{
			if	(!tag || !*tag)			// PSEUDO_ATTR
				fprintf(fout,"%c",c) ;
			else
				fprintf(fout,"<%s>%c</%s>\n",tag,c,tag) ;
		}
		else
		{	// TODO use &amp,&gt,&lt ...
			if	(!tag || !*tag)			// PSEUDO_ATTR
				fprintf(fout,"&#%d;",c) ;
			else
				fprintf(fout,"<%s>&#%d;</%s>\n",tag,c,tag) ;
		}
	break ;
	case	XoString :
		xosavestringvalue(tag,value,fout,level) ;
	break ;
	case	XoBuffer :
		xosavebuffervalue(tag,value,lg,fout,level) ;
	break ;
	default :
	break ;
	}
}

static	int32	xosaveattr(void *obj, int32 numattr, FILE *fout, int32 level)
{
	t_attribut_ref	*ar ;
	int32	type ;
	int32	mode ;
	char	*nameattr ;

	int32	lg ;
	int32	nb ;
	void	*value ;

	int32	pos ;

	nameattr	= XoGetRefAttrName(numattr) ;
	if	( !nameattr )
	{
		XOERR("",0) ;
		return	-1 ;
	}
	ar	= XoGetRefAttr(numattr) ;
	if	( !ar )
	{
		XOERR("",0) ;
		return	-11 ;
	}

	if	( IS_NO_SAVE(ar) )
		return	0 ;

	mode	= XoModeAttr(numattr) ;
	if	( mode < 0 )
	{
		XOERR("",0) ;
		return	-2 ;
	}

	type	= XoTypeAttr(numattr) ;
	if	( type < 0 )
	{
		XOERR("",0) ;
		return	-3 ;
	}

	if	(mode==BASIC_ATTR && IS_SAVE_UNSET(ar) && 
					XoIsSetAttr(obj,numattr)==0)
	{
		/* attribut non valorise et flag saveUnset */

		return	1 ;
	}

	switch	(mode)
	{
	case	BASIC_ATTR :
		value	= XoGetAttr	(obj,numattr,&lg) ;

		if	( !value )
			break;
		if	(strcmp(nameattr,XOML_PSEUDO_ATTR))
			local_tab(level,fout) ;
		xosavebasicvalue(nameattr,type,value,lg,-1,fout,level);
	break ;
	case	LIST_BASIC_ATTR :
		nb	= XoNbInAttr(obj,numattr) ;
		if	( nb <= 0 )
			break ;

		nb	= 0;
		pos	= XoFirstAttr(obj,numattr) ;
		while	( pos > 0 )
		{
			value	= XoGetAttr	(obj,numattr,&lg) ;
			if	( value )
			{
				xosavebasicvalue(nameattr,type,value,lg,nb++,
							fout,level+1) ;
			}
			pos	= XoNextAttr(obj,numattr) ;
		}
	break ;
	case	OBJ_ATTR :
		value	= XoGetAttr	(obj,numattr,&lg) ;
		if	( value )
		{
			local_tab(level,fout) ;
			xosaveobj(NULL,value,-1,fout,level+1) ;
		}
	break ;
	case	LIST_OBJ_ATTR :
		nb	= XoNbInAttr(obj,numattr) ;
		if	( nb <= 0 )
			break ;

		local_tab(level,fout) ;
		fprintf	(fout,"<%s>\n",nameattr);
		nb	= 0;
		pos	= XoFirstAttr(obj,numattr) ;
		while	( pos > 0 )
		{
			value	= XoGetAttr	(obj,numattr,&lg) ;
			if	( value )
			{
				xosaveobj(NULL,value,nb++,fout,level+1) ;
			}
			pos	= XoNextAttr(obj,numattr) ;
		}
		fprintf	(fout,"</%s>\n",nameattr);
		fflush	(stdout);
	break ;
	default	:
		XOERR("",0) ;
		return	-3 ;
	break ;
	}

	return	1 ;

}

static	int32	xosaveobj(char *tag,void *obj,int32 inList,FILE *fout,int32 level)
{
	t_attribut_ref	*ar ;
	int32	type ;
	int32	nbattr ;
	char	*nameobj ;
	int32	a ;
	int32	numattr ;
	int32	numbalise = -1 ;
	int32	mode ;
	int32	lg ;

	type	= XoType(obj) ;
	if	( type < 0 )
	{
		XOERR("",0) ;
		return	-1 ;
	}
	nbattr	= XoNbAttr(type) ;
	if	( nbattr < 0 )
	{
		XOERR("",0) ;
		return	-2 ;
	}
	nameobj	= XoGetRefObjName(type) ;
	if	( !nameobj )
	{
		XOERR("",0) ;
		return	-3 ;
	}


	if	( !tag )
	{
		tag	= "????";
		numbalise	= XoNumAttrInObj(obj,XOML_BALISE_ATTR);
		if	(numbalise > 0)
			tag	= XoGetAttr(obj,numbalise,&lg);
	}

	if (tag && *tag && *tag != '<')	// partial encoding
	{
		local_tab(level,fout) ;
		fprintf(fout,"<%s",tag) ;
		fflush(fout) ;
	}

	// embeded attr
	for	( a = 0 ; a < nbattr ; a++ )
	{
		char	*value;

		numattr	= type + a + 1 ;
		if	(numbalise > 0 && numattr == numbalise)
			continue;
		mode	= XoModeAttr(numattr) ;
		ar	= XoGetRefAttr(numattr) ;
		if	(mode == BASIC_ATTR && IS_EMBATTR(ar))
		{
			value	= XoGetAttr(obj,numattr,&lg) ;
			if	(!value)	continue;

			if	(strchr(value,'"'))
				fprintf(fout, " %s='%s'",ar->ar_name,value);
			else
				fprintf(fout, " %s=\"%s\"",ar->ar_name,value);
		}
	}
	if (tag && *tag && *tag != '<')	// partial encoding
	{
		fprintf(fout,">") ;
		fflush(fout) ;
	}

	if	(XoGetRefObjStExt(type) > 0)
	{
		char	*value;

		value	= XoNmGetAttr(obj,XOML_PSEUDO_ATTR,&lg) ;
		if	(value && *value)
		{	// TODO
			xosavebasicvalue(NULL,XoString,value,strlen(value),
							0, fout,level);
		}
		if (tag && *tag && *tag != '<')	// partial encoding
		{
			fprintf(fout,"</%s>\n",tag) ;
			fflush(fout) ;
		}
		return	1;
	}

/*	TODO ca cree un content alors qu'il n'y en a pas	
*/
	if (tag && *tag && *tag != '<')	// partial encoding
	{
		fprintf(fout,"\n") ;
		fflush(fout) ;
	}

	for	( a = 0 ; a < nbattr ; a++ )
	{
		numattr	= type + a + 1 ;
		if	(numbalise > 0 && numattr == numbalise)
			continue;
		mode	= XoModeAttr(numattr) ;
		ar	= XoGetRefAttr(numattr) ;
		if	( IS_EMBATTR(ar) )
			continue;

		switch	(mode)
		{	// do not save empty list
			case	LIST_BASIC_ATTR :
			case	LIST_OBJ_ATTR :
				if	(XoNbInAttr(obj,numattr) <= 0)
				{
					continue;
				}
			break;
		}
		switch	(mode)
		{
			case	BASIC_ATTR :
			case	LIST_BASIC_ATTR :
				xosaveattr(obj,numattr,fout,level) ;
			break;
			case	OBJ_ATTR :
			case	LIST_OBJ_ATTR :
				xosaveattr(obj,numattr,fout,level) ;
			break;
		}
	}
	
	if (tag && *tag && *tag != '<')	// partial encoding
	{
		local_tab(level,fout) ;
		fprintf(fout,"</%s>\n",tag) ;
	}
	fflush(fout) ;

	return	1 ;
}

/*DEF*/	int32	XoSaveXml(void *obj, FILE *fout,int flags)
{
	int32	ret ;

	if	( strlen(StartFlux) )
	{
		fprintf(fout,"%s",StartFlux) ;
		fflush(fout) ;
	}

	fprintf(fout,"%s\n",HEAD) ;
	ret	= xosaveobj(NULL,obj,-1,fout,0) ;
	if	( strlen(EndFlux) )
	{
		fprintf(fout,"%s",EndFlux) ;
		fflush(fout) ;
	}
	return	ret ;
}

/*DEF*/	void	XoStartEndFluxXml(char *pstart,char *pend)
{
	strcpy	(StartFlux,pstart) ;
	strcpy	(EndFlux,pend) ;
}
