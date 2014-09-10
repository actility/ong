
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


#define	 WALK_OBJ	1
#define	 WALK_ATTR	2
#define	 WALK_LIST_OBJ	4
#define	 WALK_LIST_ATTR	8

#define	 WALK_ALL	(WALK_OBJ|WALK_ATTR|WALK_LIST_OBJ|WALK_LIST_ATTR)


static	int32	xowalkobjin
(
int32 mode,
void *obj,
int32 (*fctobj)(void *o,int32 t,void *p1,void *p2,void *p3),
void *r1,void *r2,void *r3,
int32 (*fctattr)(void *o,int32 t,int32 na,int32 ma,int32 ta,void *val,void *p1,void *p2,void *p3)
)
{
	int32	typeobj ;
	int32	nbattr ;
	int32	a ;
	int32	numattr ;
	int32	ret ;

	if	(!obj)
		return	0 ;

	typeobj	= XoType(obj) ;
	if	( typeobj < 0 )
	{
		XOERR("",0) ;
		return	-1 ;
	}
	nbattr	= XoNbAttr(typeobj) ;
	if	( nbattr < 0 )
	{
		XOERR("",0) ;
		return	-2 ;
	}
	if	(fctobj)
	{
		ret	= (*fctobj)(obj,typeobj,r1,r2,r3) ;
		if	( ret <= 0 )
			return	0 ;
	}
	for	( a = 0 , numattr = typeobj + 1 ; a < nbattr ; a++ , numattr++ )
	{
		int32	typeattr ;
		int32	modeattr ;
		int32	lg ;
		void	*value ;
		int32	pos ;

		numattr		= typeobj + a + 1 ;
		modeattr	= XoModeAttr(numattr) ;
		if	( modeattr < 0 )
		{
			XOERR("",0) ;
			return	-3 ;
		}

		typeattr	= XoTypeAttr(numattr) ;
		if	( typeattr < 0 )
		{
			XOERR("",0) ;
			return	-4 ;
		}

		switch	(modeattr)
		{
		case	OBJ_ATTR :
			if	( ( mode & WALK_OBJ ) != WALK_OBJ )
				break ;
			value	= XoGetAttr	(obj,numattr,&lg) ;
			if	( value )
			{
				ret= xowalkobjin(mode,value,fctobj,r1,r2,r3,fctattr) ;
				if	(ret < 0)
					return	ret ;
			}
		break ;
		case	BASIC_ATTR :
			if	( ( mode & WALK_ATTR ) != WALK_ATTR )
				break ;
			if	( !fctattr )
				break ;
			value	= XoGetAttr	(obj,numattr,&lg) ;
			ret 	= (*fctattr)
			(obj,typeobj,numattr,modeattr,typeattr,value,r1,r2,r3) ;
			if	( ret <= 0 )
				return	ret ;
		break ;
		case	LIST_OBJ_ATTR :
			if	( ( mode & WALK_LIST_OBJ ) != WALK_LIST_OBJ )
				break ;
			pos	= XoFirstAttr(obj,numattr) ;
			while	( pos > 0 )
			{
				value	= XoGetAttr	(obj,numattr,&lg) ;
				if	( value )
				{
					ret=
					xowalkobjin(mode,value,fctobj,r1,r2,r3,fctattr);
					if	(ret < 0)
						return	ret ;
				}
				pos	= XoNextAttr(obj,numattr) ;
			}
		break ;
		case	LIST_BASIC_ATTR :
			if	( ( mode & WALK_LIST_ATTR ) != WALK_LIST_ATTR )
				break ;
			if	( !fctattr )
				break ;
			pos	= XoFirstAttr(obj,numattr) ;
			while	( pos > 0 )
			{
				value	= XoGetAttr	(obj,numattr,&lg) ;
				ret 	= (*fctattr)(obj,typeobj,numattr,
					modeattr,typeattr,value,r1,r2,r3) ;
				if	( ret <= 0 )
					return	ret ;
				pos	= XoNextAttr(obj,numattr) ;
			}
		break ;
		default	:
		break ;
		}

	}
	return	1 ;
}

/*
 *	parcours des objets rattaches a un objet
 *	ie : OBJ_ATTR et LIST_OBJ_ATTR
 */

/*DEF*/int32	XoWalkObjInObj ( void *obj, int32 (*fctobj)(void *o,int32 type,void *p1,void *p2,void *p3), void *r1,void *r2,void *r3)
{
	int32	mode;

	mode	= WALK_OBJ | WALK_LIST_OBJ ;
	return	xowalkobjin(mode,obj,fctobj,r1,r2,r3,NULL) ;
}

#if	0
/*DEF*/int32	XoWalkAllInObj ( void *obj, int32 (*fctobj)(void *o,int32 type,void *p1,void *p2,void *p3), void *r1,void *r2,void *r3, int32 (*fctattr)(void *o,int32 t,int32 na,int32 ma,int32 ta,void *val,void *p1,void *p2,void *p3))
{
	int32	mode;

	mode	= WALK_ALL ;
	return	xowalkobjin(mode,obj,fctobj,r1,r2,r3,fctattr) ;
}
#endif

static	int32 fctattr_XoObjDiffDef(void *o,int32 t,int32 na,int32 ma,int32 ta,void *val,void *p1,void *p2,void *p3)
{
	int32	*ptStop ;

/*
printf	("fctattr\n") ;
*/

	ptStop	= (int32 *)p1 ;

	if	( XoIsSetAttr(o,na) > 0 && XoAttrValueIsDef(o,na) == 0 )
	{
		if	( p2 )
		{
			void	**retObj ;

			retObj	= (void **)p2 ;
			*retObj	= o ;
		}
		*ptStop	= na ;
		return	0 ;
	}

	return	1 ;
}

/*
 *	est-ce qu'un des attributs simples de l'objet est different
 *	de sa valeur par defaut,retourne le numero d'attribut qui est different
 */

/*DEF*/int32	XoObjDiffDef (void *obj)
{
	int32	mode;
	int32	stop;
	int32	ret;

	mode	= WALK_ATTR ;
	stop	= 0 ;
	ret	= xowalkobjin(mode,obj,NULL,
		(void *)&stop,NULL,NULL,fctattr_XoObjDiffDef) ;

	if	( ret < 0 )
		return	ret ;
	return	stop ;
}

static	int32	fctobj_XoObjInObjDiffDef(void *o,int32 type,void *p1,void *p2,void *p3)
{
	int32	*ptStop ;

/*
printf	("fctobj\n") ;
*/

	ptStop	= (int32 *)p1 ;
	if	( *ptStop )
		return	0 ;	/* pas la peine d'aller plus loin */
	return	1 ;
}


/*
 *	est-ce qu'un des attributs simples de l'objet (ou de ses sous-objets)
 *	est different de sa valeur par defaut,retourne le numero d'attribut
 *	qui est different et l'objet dans retObj
 */

/*DEF*/int32	XoObjInObjDiffDef (void *obj,void **retObj)
{
	int32	mode;
	int32	stop;
	int32	ret;

	*retObj	= NULL ;
	mode	= WALK_OBJ|WALK_LIST_OBJ|WALK_ATTR ;
	stop	= 0 ;
	ret	= xowalkobjin(mode,obj,fctobj_XoObjInObjDiffDef,
		(void *)&stop,retObj,NULL,fctattr_XoObjDiffDef) ;

	if	( ret < 0 )
		return	ret ;
	return	stop ;
}

static	int32 fctattr_XoRecoverFrom(void *o,int32 t,int32 na,int32 ma,int32 ta,void *val,void *p1,void *p2,void *p3)
{
	int32	type ;
	void	*src ;
	void	*valueSrc ;
	void	*value ;
	char	convSrc[64] ;
	char	conv[64] ;

	src	= p1 ;
	type	= (int32)p2 ;
	switch	(type)
	{
	case	'R' :		/* recover	*/
		if	( XoIsSetAttr(o,na) == 0 )
		{
			valueSrc= XoAttrToPtrAscii(src,na,convSrc) ;
			if	(valueSrc)
			{
				XoAsciiToAttr2(o,na,valueSrc) ;
			}
		}
	break ;
	case	'U' :		/* undo de recover	*/
		if	( XoIsSetAttr(o,na) > 0 )
		{
			value	= XoAttrToPtrAscii(o,na,conv) ;
			valueSrc= XoAttrToPtrAscii(src,na,convSrc) ;
			if	(value && valueSrc)
			{
				if	( strcmp(value,valueSrc) == 0 )
				{
					XoUnSetAttr(o,na) ;
				}
			}
		}
	break ;
	}

	return	1 ;
}

/*
 *	pour chaque attribut simple non renseigne de dst recupere
 *	la valeur de l'attribut depuis src et copie la .
 */

/*DEF*/int32	XoRecoverFrom (void *src,void *dst)
{
	int32	mode;
	int32	ret;
	int32	type ;

	mode	= WALK_ATTR ;
	type	= 'R' ;
	ret	= xowalkobjin(mode,dst,NULL,
		src,(void *)type,NULL,fctattr_XoRecoverFrom) ;

	return	ret ;
}

/*
 *	pour chaque attribut simple renseigne de dst recupere
 *	si l'attribut est identique dans src il est devalorise
 */

/*DEF*/int32	XoUndoRecoverFrom (void *src,void *dst)
{
	int32	mode;
	int32	ret;
	int32	type ;

	mode	= WALK_ATTR ;
	type	= 'U' ;
	ret	= xowalkobjin(mode,dst,NULL,
		src,(void *)type,NULL,fctattr_XoRecoverFrom) ;

	return	ret ;
}


