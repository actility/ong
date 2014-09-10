
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
#include	"intstr.h"
#include	"xointpr.h"
#ifdef	RTL_MBR
#include	"mbr/include/mbr.h"
#endif

static	char	*xo_file = __FILE__ ;


static	int32		(*fctConvType)() ;

static	int32		(*fctFilterCopy)() ;

static	void		*parFilterCopy1 ;
static	void		*parFilterCopy2 ;


static	int32	try_copy_list_basic(t_obj *src, t_obj *dst, int32 anumsrc, int32 anumdst, int32 atype)
{
	int32	pos ;
	int32	ret ;
	void	*value ;
	int32	nb ;
	int32	lg ;

	nb	= 0 ;
	pos	= XoFirstAttr((void *)src,anumsrc) ;
	while	( pos > 0 )
	{
		value 	= XoGetAttr((void *)src,anumsrc,&lg) ;
		if	( value )
		{
			ret	=preaddattr((void *)dst,anumdst,value,lg,atype);
			if	( ret < 0 )
			{
				XOERR("",0) ;
				return	-1 ;
			}
		}
		else
		{	/* postionnement correct et pas de valeur !!! */
			XOERR("",0) ;
			return	-2 ;
		}
		pos	= XoNextAttr((void *)src,anumsrc) ;
		nb++ ;
	}
	if	( pos < 0 )
	{
		XOERR("",0) ;
		return	-3 ;
	}
	return	nb ;
}

static	int32	try_copy_list_obj(t_obj *src, t_obj *dst, int32 anum, int32 anumdst, int32 atypedst)
{
	int32	pos ;
	int32	ret ;
	void	*value ;
	void	*valuedst ;
	int32	nb ;
	int32	lg ;
	int32	ltype ;

	nb	= 0 ;
	pos	= XoFirstAttr((void *)src,anum) ;
	while	( pos > 0 )
	{
		value 	= XoGetAttr((void *)src,anum,&lg) ;
		if	( value )
		{
			if	( atypedst == XoVoidObj )
			{
				char	lbuf[512] ;

				ltype	= XoType(value) ;
				sprintf(lbuf,
				"warning : conv type '%s' -> VoidObj",
					XoGetRefObjName(ltype)) ;
				XOERR(lbuf,0) ;	
				if	( fctConvType )
					ltype	= (*fctConvType)(ltype) ;
			}
			else
				ltype	= atypedst ;
			if	( ltype < 0 )
			{
				XOERR("err. type",ltype) ;
				return	-9 ;
			}
			valuedst= XoNew(ltype) ;
			if	( !valuedst )
			{
				XOERR("err. New",ltype) ;
				return	-8 ;
			}
			ret	= xotrycopy(valuedst,value,1) ;
			if	( ret < 0 )
			{
				XOERR("",ret) ;
				return	-1 ;
			}
			ret	= XoAddInAttr((void *)dst,anumdst,valuedst,lg) ;
			if	( ret < 0 )
			{
				XOERR("",ret) ;
				return	-6 ;
			}
		}
		else
		{	/* postionnement correct et pas de valeur !!! */
			XOERR("",0) ;
			return	-3 ;
		}
		pos	= XoNextAttr((void *)src,anum) ;
		nb++ ;
	}
	if	( pos < 0 )
	{
		XOERR("",0) ;
		return	-4 ;
	}
	return	nb ;
}



/*DEF*/	int32	XoFctConvType(int32 (*fct) (/* ??? */))
{
	fctConvType	= fct ;
	return	0 ;
}

/*
 *	copie d'objets de classes differentes
 *	la copie se fait au mieux champ par champ
 */

/*DEF*/	int32	XoTryCopy(void *objdst, void *objsrc, int32 reccur)
{
	int32	ret ;

	fctFilterCopy	= NULL ;
	ret	= xotrycopy(objdst,objsrc,reccur) ;
	if	( ret < 0 )
	{
		XOERR("XoTryCopy",ret) ;
	}
	return	ret ;
}

/*
 *	copie d'objets de classes differentes
 *	la copie se fait au mieux champ par champ
 *	la copie est controlee par une fonction filtre
 */

/*DEF*/	int32	XoTryCopyFilter(void *objdst, void *objsrc, int32 reccur, int32 (*fctfilter) (/* ??? */), void *par1, void *par2)
{
	int32	ret ;

	fctFilterCopy	= fctfilter ;
	parFilterCopy1	= par1 ;
	parFilterCopy2	= par2 ;
	ret	= xotrycopy(objdst,objsrc,reccur) ;
	if	( ret < 0 )
	{
		XOERR("XoTryCopy",ret) ;
	}
	fctFilterCopy	= NULL ;
	return	ret ;
}

int32	xotrycopy(void *objdst, void *objsrc, int32 reccur)
{
	t_obj	*os ;
	t_obj	*od ;

	t_objet_ref	*ors ;
	t_objet_ref	*ord ;

	t_attribut_ref	*ars ;
	t_attribut_ref	*ard ;

	int32		nb ;
	int32		ret ;
	int32		retFilter ;
	int32		i ;

	int32		amode ;
	int32		anum ;
	int32		atype ;
	char		*aname ;

	int32		anumdst ;

	char		*value ;
	char		*valuedst ;
	int32		lg ;

	os	= (t_obj *)objsrc ;
	if	( !os )
	{
		XOERR("null",0) ;
		return	-1 ;
	}
	if	( os->o_magic != O_MAGIC )
	{
		XOERR("magic",os->o_magic) ;
		return	-2 ;
	}
	ors	= os->o_or ;
	if	( ors->or_num != os->o_num )
	{
		XOERR("ref failure",os->o_num) ;
		return	-3 ;
	}



	od	= (t_obj *)objdst ;
	if	( !od )
	{
		XOERR("null",0) ;
		return	-1 ;
	}
	if	( od->o_magic != O_MAGIC )
	{
		XOERR("magic",od->o_magic) ;
		return	-2 ;
	}
	ord	= od->o_or ;
	if	( ord->or_num != od->o_num )
	{
		XOERR("ref failure",od->o_num) ;
		return	-3 ;
	}

	ret	= 1 ;
	nb	= ors->or_nb_attr ;
	for	( i = 0 ; i < nb && ret >= 0 ; i++ )
	{
		ars	= &(ors->or_tab_attr[i]) ;
		amode	= ars->ar_mode ;
		anum	= ars->ar_num ;
		atype	= ars->ar_type ;
		aname	= ars->ar_name ;
		if	(fctFilterCopy)
		{
			value	= NULL ;
			retFilter=(*fctFilterCopy)
				(objsrc,anum,aname,&value
				,parFilterCopy1,parFilterCopy2) ;
			if	(retFilter==0)
				continue ;
		}
/*
fprintf(stderr,"name attr '%s' mode %ld type %ld typename '%s'\n"
	,aname,(long)amode,(long)atype,ars->ar_type_name) ;
fflush(stderr) ;	
*/
		anumdst	= XoNumAttrInObj((void *)od,aname) ;
		if	( anumdst < 0 )
		{
			continue ;
		}
/*
fprintf(stderr,"attr '%s' exists\n",aname) ;
fflush(stderr) ;
*/
		ard	= (t_attribut_ref *)XoGetRefAttr(anumdst) ;
		if	( !ard )			continue ;
		if	( ard->ar_mode != amode )	continue ;
		switch	(amode)
		{
	 	case	BASIC_ATTR :
			if	( ard->ar_type != atype )	break ;
/*
fprintf(stderr,"attr '%s' ok\n",aname) ;
fflush(stderr) ;
*/
			value	= XoGetAttr(objsrc,anum,&lg) ;
			if	( !value )	 break ;
			ret	= presetattr(objdst,anumdst,value,lg,atype);
			if	( ret < 0 )
			{
				XOERR("",ret) ;
				break ;
			}
		break ;
		case	OBJ_ATTR :
			if	( !reccur )		continue ;
			value	= XoGetAttr(objsrc,anum,&lg) ;
			if	( !value )		 break ;
			valuedst= XoGetAttr(objdst,anumdst,&lg) ;
			if	( !valuedst )
			{
				int32	ltype ;

				if	( ard->ar_type == XoVoidObj )
				{
					char	lbuf[512] ;

					ltype	= XoType(value) ;
					sprintf(lbuf,
					"warning : conv type '%s' -> VoidObj",
						XoGetRefObjName(ltype)) ;
					XOERR(lbuf,0) ;	
					if	( fctConvType )
						ltype=(*fctConvType)(ltype) ;
				}
				else
					ltype	= ard->ar_type ;
				if	( ltype < 0 )
				{
					XOERR("err. type",ltype) ;
					break ;
				}
				valuedst = XoNew(ltype) ;
				if	( !valuedst )
				{
					XOERR("err. New",ltype) ;
					break ;
				}
				else
					XoSetAttr(objdst,anumdst,valuedst,0) ;
			}
			ret = 	xotrycopy(valuedst,value,reccur) ;
			if	( ret < 0 )
			{
				XOERR("",ret) ;
				break ;
			}
		break ;
		case	LIST_BASIC_ATTR :
			if	( ard->ar_type != atype )	break ;
			ret= try_copy_list_basic(os,od,anum,anumdst,atype);
			if	( ret < 0 )
			{
				XOERR("",ret) ;
				break ;
			}
		break ;
		case	LIST_OBJ_ATTR :
			if	( !reccur )		continue ;
			ret =try_copy_list_obj(os,od,anum,anumdst,ard->ar_type);
			if	( ret < 0 )
			{
				XOERR("",ret) ;
				break ;
			}
		break ;
		default :
			XOERR("mode ???",amode) ;
		break ;
		}
	}

	if	( ret < 0 )
	{
		XOERR("",ret) ;
		return	ret ;
	}
	return	1 ;
}

static	int32	change_list_obj(t_obj *os, t_obj *od, int32 anum, int32 anumdst, int32 ardtype)
{
	int32	pos ;
	int32	ret ;
	void	*value ;
	int32	nb ;
	int32	lg ;
	int32	ltype ;

	nb	= 0 ;
	pos	= XoFirstAttr((void *)os,anum) ;
	while	( pos > 0 )
	{
		value 	= XoGetAttr((void *)os,anum,&lg) ;
		if	( value )
		{

			if	( ardtype == XoVoidObj )
			{
				char	lbuf[512] ;
				ltype	= XoType(value) ;
				sprintf(lbuf,
				"warning : conv type '%s' -> VoidObj",
					XoGetRefObjName(ltype)) ;
				XOERR(lbuf,0) ;	
				if	( fctConvType )
					ltype=(*fctConvType)(ltype) ;
			}
			else
				ltype	= ardtype ;
			if	( ltype < 0 )
			{
				XOERR("err. type",ltype) ;
				break ;
			}
			ret	= XoChangeRef2(value,ltype) ;
			if	( ret < 0 )
			{
				XOERR("",ret) ;
				break ;
			}
			ret	= XoAddInAttr((void *)od,anumdst,value,0) ;
			if	( ret < 0 )
			{
				XOERR("",ret) ;
				return	-6 ;
			}
		}
		else
		{	/* postionnement correct et pas de valeur !!! */
			XOERR("",0) ;
			return	-3 ;
		}
		pos	= XoNextAttr((void *)os,anum) ;
		nb++ ;
	}
	if	( pos < 0 )
	{
		XOERR("",0) ;
		return	-4 ;
	}
	return	nb ;
}



/*DEF*/	int32	XoChangeRef2(void *objsrc, int32 numclasse)
{
	char		*objdst ;
	t_obj		*os ;
	t_obj		*od ;
	t_objet_ref	*ors ;
	t_objet_ref	*ord ;

	t_attribut_ref	*ars ;
	t_attribut_ref	*ard ;

	int32		nb ;
	int32		ret ;
	int32		i ;

	int32		amode ;
	int32		anum ;
	int32		atype ;
	char		*aname ;

	int32		anumdst ;

	char		*value ;
	int32		lg ;
	int32		ltype ;

	int32		withfree ;
	int32		reccur ;

	os	= (t_obj *)objsrc ;
	if	( !os )
	{
		XOERR("null",0) ;
		return	-1 ;
	}
	if	( os->o_magic != O_MAGIC )
	{
		XOERR("magic",os->o_magic) ;
		return	-2 ;
	}
	ors	= os->o_or ;
	if	( ors->or_num != os->o_num )
	{
		XOERR("ref failure",os->o_num) ;
		return	-3 ;
	}


	ord	= XoGetRefObj(numclasse) ;
	if	( !ord )
	{
		XOERR("type ???",numclasse) ;
		return	-4 ;
	}
	objdst	= XoNew(numclasse) ;
	if	( !objdst )
	{
		XOERR("XoNew ",numclasse) ;
		return	-5 ;
	}
	od	= (t_obj *)objdst ;

	ret	= 1 ;
	nb	= ors->or_nb_attr ;
	for	( i = 0 ; i < nb && ret >= 0 ; i++ )
	{
		ars	= &(ors->or_tab_attr[i]) ;
		amode	= ars->ar_mode ;
		anum	= ars->ar_num ;
		atype	= ars->ar_type ;
		aname	= ars->ar_name ;
/*
fprintf(stderr,"name attr '%s' mode %ld type %ld typename '%s'\n"
	,aname,(long)amode,(long)atype,ars->ar_type_name) ;
fflush(stderr) ;	
*/
		anumdst	= XoNumAttrInObj((void *)od,aname) ;
		if	( anumdst < 0 )
		{
			continue ;
		}
/*
fprintf(stderr,"attr '%s' exists\n",aname) ;
fflush(stderr) ;
*/
		ard	= (t_attribut_ref *)XoGetRefAttr(anumdst) ;
		if	( !ard )			continue ;
		if	( ard->ar_mode != amode )	continue ;
		switch	(amode)
		{
	 	case	BASIC_ATTR :
			if	( ard->ar_type != atype )	break ;
/*
fprintf(stderr,"attr '%s' ok\n",aname) ;
fflush(stderr) ;
*/
			value	= XoGetAttr(objsrc,anum,&lg) ;
			if	( !value )	 break ;
			ret	= presetattr(objdst,anumdst,value,lg,atype);
			if	( ret < 0 )
			{
				XOERR("",ret) ;
				break ;
			}
		break ;
		case	OBJ_ATTR :
			value	= XoGetAttr(objsrc,anum,&lg) ;
			if	( !value )		 break ;

			if	( ard->ar_type == XoVoidObj )
			{
				char	lbuf[512] ;

				ltype	= XoType(value) ;
				sprintf(lbuf,
				"warning : conv type '%s' -> VoidObj",
					XoGetRefObjName(ltype)) ;
				XOERR(lbuf,0) ;	
				if	( fctConvType )
					ltype=(*fctConvType)(ltype) ;
			}
			else
				ltype	= ard->ar_type ;
			if	( ltype < 0 )
			{
				XOERR("err. type",ltype) ;
				break ;
			}
			ret	= XoChangeRef2(value,ltype) ;
			if	( ret < 0 )
			{
				XOERR("",ret) ;
				break ;
			}
			ret	= XoSetAttr(objdst,anumdst,value,0) ;
			if	( ret < 0 )
			{
				XOERR("",ret) ;
				break ;
			}
			ret	= XoUnSetAttr(objsrc,anum) ;
			if	( ret < 0 )
			{
				XOERR("",ret) ;
				break ;
			}
		break ;
		case	LIST_BASIC_ATTR :
			if	( ard->ar_type != atype )	break ;
			ret= try_copy_list_basic(os,od,anum,anumdst,atype);
			if	( ret < 0 )
			{
				XOERR("",ret) ;
				break ;
			}
		break ;
		case	LIST_OBJ_ATTR :
			ret =change_list_obj(os,od,anum,anumdst,ard->ar_type);
			if	( ret < 0 )
			{
				XOERR("",ret) ;
				break ;
			}
			withfree	= 0 ;
			reccur		= 1 ;
			ret	= XoDropList((void *)os,anum,withfree,reccur) ;
			if	( ret < 0 )
			{
				XOERR("",ret) ;
				break ;
			}
		break ;
		default :
			XOERR("mode ???",amode) ;
		break ;
		}
	}

	if	( ret < 0 )
	{
		XOERR("",ret) ;
		return	ret ;
	}


	ret	= XoReUse(objsrc,numclasse) ;
	if	( ret < 0 )
	{
		XOERR("XoReUse",ret) ;
		return	ret;
	}

	ret	= XoMove(objdst,objsrc) ;
	if	( ret < 0 )
	{
		XOERR("XoMove",ret) ;
		return	ret;
	}

	return	1 ;
}
