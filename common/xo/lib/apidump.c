
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



/*  RTL Technologies */
/*  Keyxord             : %W% */
/*  Current module name : %M% */
/*  SCCS file name      : %F% */
/*  SID                 : %I% */
/*  Delta Date          : %E% */
/*  Current date/time   : %D%,%T% */

#include	<stdio.h>
#include	<stdlib.h>
#ifndef		DOS
#include	<unistd.h>
#endif
#include	<string.h>
#include	<ctype.h>
#include	<memory.h>
#include	<errno.h>


#include	"xoapipr.h"
#ifdef	RTL_MBR
#include	"mbr/include/mbr.h"
#endif

static	char	*xo_file = __FILE__ ;

extern	int32	XoNbObj ;

static int32 xodump (void *obj , FILE *fout , int32 reccur , int32 level );
static int32 xodumpattr (void *obj,int32 numattr,FILE *fout,int32 reccur,int32 lev);
static int32 xodumpvalue (int32 type , char *value , int32 lg, FILE *fout );


static	char	WithAdr	= 1 ;

static	void	tab(int32 lev, FILE *fout)
{
	char	tmp[128] ;

	lev	*= 2 ;

	memset	(tmp,'.',128) ;
	tmp[lev] = '\0' ;
	fprintf(fout,"%s",tmp) ;
}

static	int32	xodump(void *obj, FILE *fout, int32 reccur, int32 level)
{
	int32	type ;
	int32	nbattr ;
	char	*nameobj ;
	int32	a ;
	int32	numattr ;

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

	tab(level,fout) ;
	if	(WithAdr)
		fprintf(fout,"Obj=[%s] NbAttr=%ld Adr=%lx\n",
		nameobj,(long)nbattr,(long)obj) ;
	else
		fprintf(fout,"Obj=[%s] NbAttr=%ld\n",
		nameobj,(long)nbattr) ;
	fflush(fout) ;

	for	( a = 0 ; a < nbattr ; a++ )
	{
		numattr	= type + a + 1 ;
		xodumpattr(obj,numattr,fout,reccur,level) ;
	}
	fflush(fout) ;

	return	1 ;
}


static	int32	xodumpattr(void *obj, int32 numattr, FILE *fout, int32 reccur, int32 level)
{
	int32	type ;
	int32	mode ;
	char	*nameattr ;
	char	*nametype ;

	int32	lg ;
	void	*value ;

	int32	pos ;

	nameattr	= XoGetRefAttrName(numattr) ;
	if	( !nameattr )
	{
		XOERR("",0) ;
		return	-1 ;
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

	nametype= XoGetRefObjName(type) ;
	if	( !nametype )
	{
		XOERR("",0) ;
		return	-3 ;
	}

	tab(level,fout) ;
	fprintf(fout,"Attr=[%s] Type=[%s]  ",nameattr,nametype) ;
	fflush(fout) ;

	switch	(mode)
	{
	case	BASIC_ATTR :
		value	= XoGetAttr	(obj,numattr,&lg) ;
		if	( value )
		{
			xodumpvalue(type,value,lg,fout) ;
		}
		fprintf(fout,"\n") ;
	break ;
	case	LIST_BASIC_ATTR :
		pos	= XoFirstAttr(obj,numattr) ;
		while	( pos > 0 )
		{
			value	= XoGetAttr	(obj,numattr,&lg) ;
			if	( value )
			{
				xodumpvalue(type,value,lg,fout) ;
			}
			pos	= XoNextAttr(obj,numattr) ;
		}
		fprintf(fout,"\n") ;
	break ;
	case	OBJ_ATTR :
		if	( !reccur )
		{
			fprintf(fout,"<Objet inclus>\n") ;
			break ;
		}
		value	= XoGetAttr	(obj,numattr,&lg) ;
		if	( value )
		{
			fprintf(fout,"<Objet inclus>\n") ;
			xodump(value,fout,reccur,level+1) ;
		}
		else
			fprintf(fout,"<Objet inclus NULL>\n") ;
	break ;
	case	LIST_OBJ_ATTR :
		fprintf(fout,"<Liste d'objets inclus [%ld]>\n",
			(long)XoNbInAttr(obj,numattr)) ;
		if	( !reccur )
		{
			break ;
		}
		pos	= XoFirstAttr(obj,numattr) ;
		while	( pos > 0 )
		{
			value	= XoGetAttr	(obj,numattr,&lg) ;
			if	( value )
			{
				xodump(value,fout,reccur,level+1) ;
			}
			pos	= XoNextAttr(obj,numattr) ;
		}
	break ;
	default	:
		XOERR("",0) ;
		return	-3 ;
	break ;
	}

	return	1 ;

}


static	int32	xodumpvalue(int32 type, char *value, int32 lg, FILE *fout)
{
	int32	l ;
	char	c ;

	switch	(type)
	{
	case	XoBool :
	case	XoInt4B :
	case	XoDate :
		l	= *(int32 *)value ;
		fprintf(fout," %ld",(long)l) ;
	break ;
	case	XoChar :
		c	= *(char *)value ;
		fprintf(fout," %c",c) ;
	break ;
#if	0
	case	XoVoid :
		fprintf(fout," %ld",(long)value) ;
	break ;
#endif
	case	XoString :
		fprintf(fout," %s",value) ;
	break ;
	case	XoBuffer :
	{
		int	i ;
		unsigned char	*v ;

		v	= (unsigned char *)value ;
		fprintf(fout," ") ;
		for	( i = 0 ; i < lg ; i++ )
		{
			fprintf	(fout,"%02x",v[i]) ;
		}
	}
	break ;
	default :
		return	-1 ;
	break ;
	}
	return	0 ;
}

/*DEF*/	int32	XoDump(void *obj, FILE *fout, int32 reccur)
{
	return	(xodump(obj,fout,reccur,0)) ;
}

/*DEF*/	int32	XoDumpWithoutAdr(void *obj, FILE *fout, int32 reccur)
{
	int32	ret ;
	WithAdr	= 0 ;
	ret	= xodump(obj,fout,reccur,0) ;
	WithAdr	= 1 ;
	return	ret ;
}

/*DEF*/	int32	XoDumpNbOcc(FILE *fout, int32 not0, int32 notpredef)
{
	int32	i ;
	int32	onum ;
	char	*name ;
	int32	nbocc ;
	int32	nb;

	nb	= 0 ;
	for	( i = 0 ; i < XoNbObj ; i++ )
	{
		onum	= (i+1) * BaseObj ;
		name	= XoGetRefObjName(onum) ;
		if	( !name )	continue ;
		if	( notpredef && onum < BaseUserObj(0) )	continue ;
		nbocc	= XoNbOccObj(onum) ;
		if	( not0 && nbocc == 0 )	continue ;

		fprintf	(fout,"%ld %ld '%s' nb = %ld\n",(long)i,(long)onum,name,(long)nbocc) ;
		fflush	(fout) ;
		nb	+= nbocc ;
	}
	return	nb ;
}



/*DEF*/	int32	XoDumpListObj(FILE *fout, int32 onum, int32 reccur)
{
	int32	nb ;
	void	*obj ;

	nb	= 0 ;
	obj	= XoFirstObj(onum) ;
	while	( obj )
	{
		fprintf	(fout,"--------------- %lx ------------------\n",(long)obj) ;
		fflush	(fout) ;
		XoDump	(obj,fout,reccur) ;
		nb++ ;
		obj	= XoNextObj(onum) ;
	}
	return	nb ;
}


/*
 *	convertit une chaine de caracteres <src> dans un buffer <dest>
 *	retourne la longueur occupee dans dest.
 */


int XoAsc2Hex( unsigned char *src, unsigned char *dest)
{
	int	count;
	int	eos;
	unsigned char byt;

	count	= 0;
	eos	= 0;

	while	(!eos && *src)
	{
		if	((*src >= '0') && (*src <= '9'))
			byt	= (unsigned char)(*src - '0');
		else if	((*src >= 'a') && (*src <= 'f'))
			byt	= (unsigned char)(*src - 'a' + 10);
		else if	((*src >= 'A') && (*src <= 'F'))
			byt	= (unsigned char)(*src - 'A' + 10);
		else
		{
			unsigned	char	sep = '\0' ;

			switch	(*src)
			{
				case	'\'' :	sep	= '\'' ;	break;
				case	'"' :	sep	= '"' ;		break;
				case	'<' :	sep	= '>' ;		break;
				case	'{' :	sep	= '}' ;		break;
				case	'(' :	sep	= ')' ;		break;
			}
			switch	(*src++)
			{
			case	'\''	:
			case	'"' 	:
			case	'<' 	:
			case	'{' 	:
			case	'(' 	:
				while	(*src && *src != sep)
				{
					*dest++	= *src++ ;
					count	+= 2 ;
				}
				if	( !*src )	eos	= 1 ;
				src	++ ;	/* '\'' */
			break ;
			case	'#'	:
			{
				char	tmp[32] ;
				char	*pt ;

				memset	(tmp,0,sizeof(tmp)) ;
				pt	= &tmp[0] ;
				while	(*src && *src >= '0' && *src <= '9')
				{
					if	( pt - &tmp[0] >= 4 )
						break ;
					*pt++	= *src++ ;
				}
				*dest++	= (unsigned char)atoi(tmp) ;
				count	+= 2 ;
				if	( !*src )	eos	= 1 ;
			}
			break ;
			default	:
			break ;
			}
			continue;
		}

		src	++;

		if	(count % 2)
			*dest++	|= byt;
		else
			*dest	= (unsigned char)(byt << 4);
		count	++;
	}
	return	count/2;
}
