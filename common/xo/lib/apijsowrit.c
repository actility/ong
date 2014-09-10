
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

#include "xoapipr.h"

static	char	*xo_file	= __FILE__;

#define	JSO_START_ROOT	0
#define	JSO_END_ROOT	1
#define	JSO_START_OBJ	2
#define	JSO_END_OBJ	3
#define	JSO_START_LIST	4
#define	JSO_END_LIST	5
#define	JSO_NEXT_ELEM	6
#define	JSO_LAST_ELEM	7

#define	StartRoot(c,l)		Balise(c,l,JSO_START_ROOT,NULL)
#define	EndRoot(c,l)		Balise(c,l,JSO_END_ROOT,NULL)
#define	StartObj(c,l,o)		Balise(c,l,JSO_START_OBJ,o)
#define	EndObj(c,l)		Balise(c,l,JSO_END_OBJ,NULL)
#define	StartList(c,l,a)	Balise(c,l,JSO_START_LIST,a)
#define	EndList(c,l)		Balise(c,l,JSO_END_LIST,NULL)
#define	NextElem(c,l)		Balise(c,l,JSO_NEXT_ELEM,NULL)
#define	LastElem(c,l)		Balise(c,l,JSO_LAST_ELEM,NULL)


typedef	struct
{
	int	typeio;
	int	passe;
	int	flags;
	int	szcur;
	int	szout;
	char	*bufout;
	void	*root;
	FILE	*fout;
	int	prevtoken;
}	t_wctxt;


static	int	DoLevel(t_wctxt *ctxt,int32 level)
{
	int	i;

	if	(ctxt->typeio == IO_FILE)
	{
		for	(i = 0 ; i < level ; i++)
		{
			fprintf(ctxt->fout,"  ");
		}
		return	1;
	}
	return	0;
}

static	int	Balise(t_wctxt *ctxt,int32 level,int t,char *name)
{
	char	tmp[256];
	char	*b;
	if	(ctxt->typeio == IO_FILE)
	{
		if	(t != JSO_NEXT_ELEM && t != JSO_LAST_ELEM)
			DoLevel(ctxt,level);
		switch	(t)
		{
		case	JSO_START_ROOT :
			b	= "{\n";
		break;
		case	JSO_END_ROOT :
			b	= "}\n";
		break;
		case	JSO_START_OBJ :
			b	= "{\n";
			sprintf	(tmp,"{ \"%s\" : {\n",name);
			b	= tmp;
		break;
		case	JSO_END_OBJ :
			b	= "}}";
		break;
		case	JSO_START_LIST :
			b	= "[\n";
			sprintf	(tmp,"\"%s\" : [\n",name);
			b	= tmp;
		break;
		case	JSO_END_LIST :
			b	= "]";
		break;
		case	JSO_NEXT_ELEM :
			b	= ",\n";
		break;
		case	JSO_LAST_ELEM :
			b	= "\n";
		break;
		default :
			return	0;
		break;
		}
		if	(ctxt->prevtoken == JSO_END_OBJ 
					|| ctxt->prevtoken == JSO_END_LIST)
		{
			if	(t != JSO_NEXT_ELEM && t == JSO_LAST_ELEM)
				fprintf	(ctxt->fout,"\n");
		}
		ctxt->prevtoken	= t;
		fprintf	(ctxt->fout,"%s",b);
		return	1;
	}

	return	0;
}


static	int32	xowritobj(t_wctxt *ctxt,void *obj,int32 level);

static	int32	xowritbasicattr(t_wctxt *ctxt,char *nameattr,void *value,t_attribut_ref *ar,int32 level)
{
	char		tmp[128];
	int32		l ;
	unsigned char	c ;

	switch	(ar->ar_type)
	{
	case	XoBool :
	case	XoInt4B:
	case	XoDate :
		l	= *(int32 *)value ;
		sprintf(tmp,"%d",(int32)l) ;
		value	= tmp;
	break ;
	case	XoChar :
		c	= *(char *)value ;
		sprintf(tmp,"%c",(int32)c) ;
		value	= tmp;
	break ;
	case	XoString :
	break ;
	case	XoBuffer :
	break ;
	default :
		XOERR("xowritbasicattr",0) ;
		return	-2;
	break ;
	}
	if	(ctxt->typeio == IO_FILE)
	{
		DoLevel(ctxt,level);
		if	(ar->ar_mode == LIST_BASIC_ATTR 
					|| ar->ar_mode == LIST_OBJ_ATTR)
		{
			fprintf	(ctxt->fout,"\"%s\"",(char *)value);
		}
		else
		{
			char	*pf = "";
			if	(ar->ar_mode == BASIC_ATTR && IS_EMBATTR(ar))
				pf	= "-";
			fprintf	(ctxt->fout,"\"%s%s\" : \"%s\"",
						pf,nameattr,(char *)value);
		}

		return	1;
	}

	return	1;
}
static	int32	xowritobjectattr(t_wctxt *ctxt,char *nameattr,void *value,t_attribut_ref *ar,int32 level)
{
	if	(ctxt->typeio == IO_FILE)
	{
		DoLevel(ctxt,level);
		fprintf	(ctxt->fout,"\"%s\" : ",nameattr);
		if (xowritobj(ctxt,value,level+1) < 0)
		{
			XOERR("xowritobj",0) ;
			return	-5 ;
		}
		return	1;
	}
	return	1;
}

static	int32	xowritattr(t_wctxt *ctxt,void *obj,int32 numattr,int32 level)
{
	t_attribut_ref	*ar ;
	int32	type ;
	int32	mode ;
	char	*nameattr ;

	int32	lg ;
	int32	nb ;
	int32	svnb ;
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
	{
		return	0 ;
	}

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

//printf	("wattr nameattr='%s'\n",nameattr);
		return	0 ;
	}

	switch	(mode)
	{
	case	BASIC_ATTR :
		value	= XoGetAttr	(obj,numattr,&lg) ;
		if	( !value )
			return	0;
		if (xowritbasicattr(ctxt,nameattr,value,ar,level)<0)
		{
			XOERR("xowritbasicattr",0) ;
			return	-4 ;
		}
	break ;
	case	LIST_BASIC_ATTR :
		svnb	= XoNbInAttr(obj,numattr) ;
		if	( svnb <= 0 )
			return	0;

		nb	= 0;
		pos	= XoFirstAttr(obj,numattr) ;
		while	( pos > 0 )
		{
			value	= XoGetAttr	(obj,numattr,&lg) ;
			if	( value )
			{
				if (xowritbasicattr(ctxt,nameattr,value,ar,level+1)<0)
				{
					XOERR("xowritbasicattr",0) ;
					return	-6 ;
				}
			}
			else
				continue;
			nb++;
			pos	= XoNextAttr(obj,numattr) ;
			if	(nb >= svnb)
				LastElem(ctxt,level);
			else
				NextElem(ctxt,level);
		}
	break ;
	case	OBJ_ATTR :
		value	= XoGetAttr	(obj,numattr,&lg) ;
		if	( !value )
			return	0;
		if (xowritobjectattr(ctxt,nameattr,value,ar,level)<0)
		{
			XOERR("xowritbasicattr",0) ;
			return	-5 ;
		}
#if	0
		if (xowritobj(ctxt,value,level+1) < 0)
		{
			XOERR("xowritobj",0) ;
			return	-5 ;
		}
#endif
	break ;
	case	LIST_OBJ_ATTR :
		svnb	= XoNbInAttr(obj,numattr) ;
		if	( svnb <= 0 )
			return	0;

		nb	= 0;
		pos	= XoFirstAttr(obj,numattr) ;
		while	( pos > 0 )
		{
			value	= XoGetAttr	(obj,numattr,&lg) ;
			if	( value )
			{
				if (xowritobj(ctxt,value,level+1) < 0)
				{
					XOERR("xowritobj",0) ;
					return	-7 ;
				}
			}
			nb++;
			pos	= XoNextAttr(obj,numattr) ;
			if	(nb >= svnb)
				LastElem(ctxt,level);
			else
				NextElem(ctxt,level);
		}
	break ;
	default	:
		XOERR("",0) ;
		return	-10 ;
	break ;
	}

	return	1 ;

}

static	int32	xowritobj(t_wctxt *ctxt,void *obj,int32 level)
{
	t_attribut_ref	*ar ;
	int32	type ;
	int32	nbattr ;
	int32	a ;
	char	*nameobj ;
	char	*nameattr;
	int32	numattr ;
	int32	mode ;
	int32	ret ;

	type	= XoType(obj) ;
	if	( type < 0 )
	{
		XOERR("",0) ;
		return	-10 ;
	}
	nbattr	= XoNbAttr(type) ;
	if	( nbattr < 0 )
	{
		XOERR("",0) ;
		return	-20 ;
	}
	nameobj	= XoGetRefObjName(type) ;
	if	( !nameobj )
	{
		XOERR("",0) ;
		return	-30 ;
	}

	StartObj(ctxt,level,nameobj);
	ret	= -1;
	for	( a = 0 ; a < nbattr ; a++ )
	{
		numattr	= type + a + 1 ;
		mode	= XoModeAttr(numattr) ;
		ar	= XoGetRefAttr(numattr) ;
		nameattr= ar->ar_name;

//printf	("wobj nameattr='%s' ret=%d\n",nameattr,ret);
		switch	(mode)
		{
			case	BASIC_ATTR :
				ret	= xowritattr(ctxt,obj,numattr,level);
				if(ret < 0)
				{
					XOERR("xowritattr",0) ;
					return	-100 ;
				}
			break;
			case	LIST_BASIC_ATTR :
				if	(XoNbInAttr(obj,numattr) <= 0)
					continue;
				StartList(ctxt,level,nameattr);
				ret	= xowritattr(ctxt,obj,numattr,level);
				if(ret < 0)
				{
					XOERR("xowritattr",0) ;
					return	-110 ;
				}
				EndList(ctxt,level);
			break;
			case	OBJ_ATTR :
				ret	= xowritattr(ctxt,obj,numattr,level+1);
				if(ret < 0)
				{
					XOERR("xowritattr",0) ;
					return	-120 ;
				}
			break;
			case	LIST_OBJ_ATTR :
				if	(XoNbInAttr(obj,numattr) <= 0)
					continue;
				StartList(ctxt,level,nameattr);
				ret	= xowritattr(ctxt,obj,numattr,level+1);
				if(ret < 0)
				{
					XOERR("xowritattr",0) ;
					return	-130 ;
				}
				EndList(ctxt,level);
			break;
		}
		if	(ret > 0)
		{
			if	(a >= nbattr-1)
				LastElem(ctxt,level);
			else
				NextElem(ctxt,level);
		}
	}

	EndObj(ctxt,level);
	
	return	1 ;
}

/*DEF*/	int	XoWritJsoMem(void *obj,int32 flags,char **bufout)
{
	t_wctxt		ctxt;
	int32		ret ;

	memset	(&ctxt,0,sizeof(t_wctxt));
	ctxt.typeio	= IO_MEMORY;
	ctxt.flags	= flags;
	ctxt.root	= obj;

	*bufout	= NULL;
	ret	= xowritobj(&ctxt,obj,0) ;
	if	(ret < 0)
	{
		return	-1;
	}

	*bufout	= ctxt.bufout;

	return	ctxt.szout;
}

/*DEF*/	int32	XoWritJsoEx(char *filename,void *obj,int32 flags)
{
	t_wctxt		ctxt;
	int32	ret ;

	memset	(&ctxt,0,sizeof(t_wctxt));
	ctxt.typeio	= IO_FILE;
	ctxt.flags	= flags;
	ctxt.root	= obj;

	if	(!filename || !*filename)
		ctxt.fout	= stdout;

//	StartRoot(&ctxt,0);
	ret	= xowritobj(&ctxt,obj,0) ;
	if	(ret < 0)
	{
		return	-3;
	}
//	EndRoot(&ctxt,0);

	return	ctxt.szout ;
}

