
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
#include	<stdarg.h>
#ifndef		DOS
#include	<unistd.h>
#endif
#include	<string.h>
#include	<ctype.h>
#include	<memory.h>
#include	<errno.h>
#include	<fcntl.h>

#ifdef		DOS
#include	<process.h>
#endif


#include	"xoapipr.h"
#ifdef	RTL_MBR
#include	"mbr/include/mbr.h"
#endif

static	char	*xo_file = __FILE__ ;
/*
static int32	_xogetfieldref_(char *obj, char *nom, char **objRet, int32 crea) ;
*/

/*
 *	retourne le nom du type de l'objet
 */

/*DEF*/	char	*XoNmType(void *obj)
{
	char	*name ;
	int32	type ;


	type	= XoType(obj) ;
	if	( type < 0 )
	{
		XOERR("",type) ;
		return	NULL ;
	}
	name	= XoGetRefObjName(type);
	if	( !name )
	{
		XOERR("",0) ;
		return	NULL ;
	}
	return	name ;
}


/* 
 *
 *	Creation d'un objet XO et des sous objets en cascade
 *
 */

static	char *_xonewnest(int32 numClass)
{
	int32	nbAttr, chp;
	int32	modeAttr;
	int32	typeAttr;
	int32	ret;
	void	*obj, *fils;

	obj	= XoNew(numClass);
	if	(!obj)
	{
		XOERR("",0) ;
		return	NULL ;
	}
	nbAttr	= XoNbAttr(numClass);
	if	( nbAttr < 0 )
	{
		XOERR("",0) ;
		return	NULL ;
	}
	for	( chp = 1 ; chp <= nbAttr ; chp ++ )
	{
		modeAttr	= XoModeAttr(numClass + chp);
		if	( modeAttr < 0 )
		{
			XOERR("",0) ;
			return	NULL ;
		}
		typeAttr = XoTypeAttr(numClass + chp);
		if	(modeAttr == OBJ_ATTR && typeAttr != XoVoidObj)
		{
			fils = _xonewnest(typeAttr);
			if	(!fils)
			{
				XOERR("",0) ;
				return	NULL ;
			}
			ret	= XoSetAttr(obj, numClass + chp, fils, 0);
			if	( ret < 0 )
			{
				XOERR("",ret) ;
				return	NULL ;
			}
		}
	}

	return obj;
}

/* 
 *
 *	Creation d'un objet XO et des sous objets en cascade
 *
 */

/*DEF*/ void	*XoNewNest(int32 numClass)
{
	void	*obj;

	obj	= _xonewnest(numClass);
	if	( !obj )
	{
		XOERR("",0) ;
		return	NULL ;
	}
	return	obj;
}

/*DEF*/	void	*XoNmNewNest(char *class)
{
	int32	numClass;

	if	((numClass=XoGetNumType(class))<0)
	{
		XOERR	(class,0) ;
		return	NULL ;
	}
	return XoNewNest(numClass);
}

/*DEF*/	void	*XoNmNew(char *class)
{
	int32	numClass;

	if	((numClass=XoGetNumType(class))<0)
	{
		XOERR	(class,0) ;
		return	NULL ;
	}
	return XoNew(numClass);
}

static	int32	CompareValue(void *obj,int32 numAttr,char *valueSearch)
{
	char	tmp[64] ;
	char	*value ;

	if	( !valueSearch )
		return	0 ;
	
	value	= XoAttrToPtrAscii(obj,numAttr,tmp) ;
	if	( !value )
		return	0 ;

	if	( strcmp(value,valueSearch) == 0 )
		return	1 ;
	return	0 ;
}

static	int32	SearchIndex(char *txt,char *obj,int32 numAttrLst)
{
	int32	ret = 0 ;	// not found
	int32	idx ;
	char	*end ;
	char	*egal ;
	char	*quote ;

	quote	= NULL ;
	ret	= -1 ;
	end	= strchr(txt,']') ;
	if	( end )
		*end	= '\0' ;

//printf(" txt index '%s'\n",txt) ;

	egal	= strchr(txt,'=') ;
	if	( egal )
	{
		char	*chpName ;
		char	*valueSearch ;
		int32	numAttrSearch ;
		int32	pos ;
		void	*objlst ;

		*egal	= '\0' ;
		chpName	= txt ;
		valueSearch	= egal+1 ;
		if	( *valueSearch == '\'' )
			valueSearch++ ;
		quote	= strrchr(valueSearch,'\'') ;
		if	( quote )
			*quote	= '\0' ;
/*
fprintf(stderr,"cherche '%s' dans '%s.%s[].%s'\n",valueSearch, 
		XoNmType(obj),XoGetRefAttrName(numAttrLst),chpName) ;
fflush(stderr) ;
*/
		pos	= XoFirstAttr(obj,numAttrLst) ;
		while	( pos > 0 )
		{
			// si la liste est d'un type unique (!=VoidObj), la
			// recherche de l'attribut peut etre sorti de la loop
			objlst	= XoGetAttr(obj,numAttrLst,0L) ;
			if	( !objlst )	goto	arret ;
			numAttrSearch = XoNumAttrInObj(objlst,chpName) ;
			if	( numAttrSearch < 0 )	goto	arret ;
			if	(CompareValue(objlst,numAttrSearch,valueSearch))
			{
				ret	= 1;
				goto	arret;
			}
			pos	= XoNextAttr(obj,numAttrLst) ;
		}
		ret	= 0;
		goto	arret;
	}

	{
		int	nb ;

		idx	= xoatoi(txt) ;
		nb	= XoNbInAttr(obj,numAttrLst) ;
		if	( idx < 0 || idx >= nb )
		{
			ret	= 0 ;
			goto	arret ;
		}
		if	( XoSetIndexInAttr(obj,numAttrLst,idx) < 0 )
		{
			XOERR("bad idx",idx) ;
			goto	arret ;
		}
	}

	ret	= 1 ;
arret	 :
	if	( end )
		*end	= ']' ;
	if	( egal )
		*egal	= '=' ;
	if	( quote )
		*quote	= '\'' ;
	return	ret ;
}


/*
 *
 *	Recherche les r‚f‚rences d'un champ xo par son nom recursif
 *	ex: champ1.(type_cast)souschamp.jksdhksdhds...
 *
 *	Retourne le numero du champ, ou -1 en cas d'erreur
 *
 *	ou -6789 en cas d'acces a une liste sur un element inexistant
 *	pour eviter les messages d'erreur
 */



static int32	_xogetfieldref_(char *obj, char *nom, char **objRet, int32 crea)
{
	char	*pt;
	char	*ptind;
	char	tmp[512];
	char	buf[512];
	char	type[128];
	int32	numAttr;
	int32	lg;
	int32	ret;
	int32	ret2;
#if	0
	int32	idx;
#endif

	if	(!obj)
	{
		XOERR("null obj",0) ;
		return -1;
	}
	if	(*nom=='(')
	{	/* cast */
		for	( pt = type , nom++ ; *nom && *nom != ')' ; )
		{
			*pt++	= *nom++;
		}
		*pt	= 0;
		if	(*nom)
		{
			nom++;
		}
	}
	else
	{	/* pas de cast */
		*type	= 0;
	}
	*objRet	= obj;
//printf	("start nom='%s'\n",nom);
	pt	= strchr(nom,'.') ;
	if	( pt )
	{	/* champs composes */
		// *pt	= '\0'; NON !!! si le parametre est un litteral !!!
		if (XoGetRefObjWrapped(XoType(obj)) > 0 && nom[0] == '[')
		{
			strncpy		(tmp,nom,(size_t)(pt-nom));
			tmp[pt-nom]	= 0;
			sprintf	(buf,"%s%s",XOML_WRAPPE_ATTR,tmp);
		}
		else
		{
			strncpy		(buf,nom,(size_t)(pt-nom));
			buf[pt-nom]	= 0;
		}
		// *pt	= '.'; NON !!!
		ptind	= strchr(buf,'[') ;
		if	(ptind)
			*ptind	= '\0' ;
		if	( (numAttr = XoNumAttrInObj(obj,buf)) <0 )
		{
			XOERR("bad attribut name",0) ;
			return -1;
		}
		if	(ptind)
		{
			ret2	= SearchIndex(ptind+1,obj,numAttr) ;
			if	( ret2 == 0 )
				return	-6789 ;
			if	( ret2 < 0 )
			{
				XOERR("bad idx",0) ;
				return	-1 ;
			}
		}
		nom	= pt+1;
		if	( !( pt = XoGetAttr(obj,numAttr,&lg)) )
		{
			if	(!crea)
			{
				return -1;
			}
			pt	= XoNew(XoTypeAttr(numAttr));
			if	( !pt )
			{
				XOERR("",0) ;
				return	-1 ;
			}
			ret	= XoSetAttr(obj,numAttr,pt,0);
			if	( ret < 0 )
			{
				XOERR("",ret) ;
				return	-1 ;
			}
		}
		if	(*type && strcmp(type,XoNmType(pt)) )
		{
			return -1;
		}
		ret	= _xogetfieldref_(pt,nom,objRet,crea);
		if	(( ret < 0 ) && ( ret != -6789 ))
		{
			XOERR("",ret) ;
		}
//printf	("step nom='%s'\n",nom);
		return	ret ;
	}
//printf	("end nom='%s'\n",nom);
	if (XoGetRefObjWrapped(XoType(obj)) > 0 && nom[0] == '[')
	{
		if ( nom[1] == ']')
			strcpy	(tmp,XOML_WRAPPE_ATTR);
		else
			sprintf	(tmp,"%s%s",XOML_WRAPPE_ATTR,nom);
	}
	else
		strcpy(tmp,nom) ;
	ptind	= strchr(tmp,'[') ;
	if	(ptind)
		*ptind	= '\0' ;
	/*
	 *	ici on n'a plus qu'un objet obj et un nom simple tmp
	 */
/*
fprintf(stderr,"type='%s' attr='%s'\n",XoNmType(obj),tmp) ;
fflush(stderr) ;
*/
	numAttr	= XoNumAttrInObj(obj,tmp) ;
	if	(numAttr < 0 )
	{	/* cet attribut n'existe pas dans l'objet */
#if	0
		XOERR("bad attr",numAttr) ;
#endif
		return	-1 ;
	}
	if	(ptind)
	{
#if	0
		idx	= xoatoi(ptind+1) ;
		if	( XoSetIndexInAttr(obj,numAttr,idx) < 0 )
		{
			XOERR("bad idx",idx) ;
			return	-1 ;
		}
#endif
		ret2	= SearchIndex(ptind+1,obj,numAttr) ;
		if	( ret2 == 0 )
			return	-6789 ;
		if	( ret2 < 0 )
		{
			XOERR("bad idx",0) ;
			return	-1 ;
		}
	}
	if	(*type)
	{	/* cast */
		if	((pt=XoGetAttr(obj,numAttr,&lg)))
		{
			if	(!strcmp(XoNmType(pt),type))
			{
				return	numAttr;
			}
			else
			{	/* l'objet recuperer n'est pas du type du
					cast demande
				*/
				XOERR("bad cast",0) ;
				return	-1 ;
			}
		}
		return -1;
	}
	return	numAttr;
}




static	 int32	xogetfieldref(void *obj, char *nom, void **objRet, int32 crea)
{
	void	*tmp;
	int32	ret;

	ret	= _xogetfieldref_((char *)obj,nom,(char **)&tmp,crea);
	if	(objRet)
	{
		*objRet	= tmp;
	}
	return	ret;
}

static	 int32	va_xogetfieldref(void *obj, char *nom, void **objRet, int32 crea , va_list listArg)
{
	char	tmp[512] ;
	char	*pt ;
	char	*svnom ;
	int32	vint ;
	char	ascvint[32] ;
	char	*vstr ;
	int32	lg ;

	if	( strchr(nom,(int)'%') == NULL )
	{
		return	xogetfieldref(obj,nom,objRet,crea);
	}

	svnom	= nom ;
	pt	= tmp ;
	while	( *nom )
	{
		if	( *nom == '%' )
		{
			nom++ ;
			*pt	= '\0' ;
			switch(*nom)
			{
				case	'd' :
					vint	= va_arg(listArg,int32) ;
					sprintf	(ascvint,"%d",vint) ;
					vstr	= ascvint ;
				break ;
				case	's' :
					vstr	= va_arg(listArg,char *) ;
				break ;
				default :
					XOERR(svnom,0) ;
					return	-4000 ;
				break ;
			}
			lg	= strlen(vstr) ;
			strcat	(pt,vstr) ;
			nom++ ;
			pt	+= lg ;
		}
		else
		{
			*pt	= *nom ;
			nom++ ;
			pt++ ;
		}
	}
	*pt	= '\0' ;

/*
fprintf(stderr,"apres transformation '%s'\n",tmp) ;
fflush(stderr) ;
*/

	return	xogetfieldref(obj,tmp,objRet,crea);
}



/*
 *	est-ce qu'un champ existe dans un obj
 */

/*DEF*/	int32	XoNmExistField(void *obj, char *nom, ...)
{
	va_list	listArg;
	int32	ret ;
	
	va_start(listArg,nom) ;
	ret = va_xogetfieldref(obj,nom,0L,0,listArg);
	va_end(listArg) ;
	return	ret ;
}


/*DEF*/	int32	XoNmGetIndexInAttr(void *obj, char *nomChp, void *elem,...)
{
	int32	numAttr;
	va_list	listArg;
	
	va_start(listArg,elem) ;
	if((numAttr=va_xogetfieldref(obj,nomChp,&obj,0,listArg))>=0)
	{
		va_end(listArg) ;
		return XoGetIndexInAttr(obj,numAttr,elem);
	}
	XOERR(nomChp,0) ;
	va_end(listArg) ;
	return -1;
}

/*DEF*/	int32	XoNmSetIndexInAttr(void *obj, char *nomChp, int32 idx,...)
{
	int32	numAttr;
	va_list	listArg;
	
	va_start(listArg,idx) ;
	if((numAttr=va_xogetfieldref(obj,nomChp,&obj,0,listArg))>=0)
	{
		va_end(listArg) ;
		return XoSetIndexInAttr(obj,numAttr,idx);
	}
	XOERR(nomChp,0) ;
	va_end(listArg) ;
	return -1;
}



/*DEF*/	int32	XoNmSupInAttr(void *obj, char *nomChp,...)
{
	int32	numAttr;
	va_list	listArg;
	
	va_start(listArg,nomChp) ;
	if((numAttr=va_xogetfieldref(obj,nomChp,&obj,0,listArg))>=0)
	{
		va_end(listArg) ;
		return XoSupInAttr(obj,numAttr);
	}
	XOERR(nomChp,0) ;
	va_end(listArg) ;
	return -1;
}

/*DEF*/	int32	XoNmAddInAttr(void *obj, char *nomChp, void *val, int32 lg,...)
{
	int32	numAttr;
	va_list	listArg;
	
	va_start(listArg,lg) ;
	if((numAttr=va_xogetfieldref(obj,nomChp,&obj,1,listArg))>0)
	{
		va_end(listArg) ;
		return XoAddInAttr(obj,numAttr,val,lg);
	}
	XOERR(nomChp,0) ;
	va_end(listArg) ;
	return -1;
}

/*DEF*/	int32	XoNmInsInAttr(void *obj, char *nomChp, void *val, int32 lg,...)
{
	int32	numAttr;
	va_list	listArg;
	
	va_start(listArg,lg) ;
	if((numAttr=va_xogetfieldref(obj,nomChp,&obj,1,listArg))>0)
	{
		va_end(listArg) ;
		return XoInsInAttr(obj,numAttr,val,lg);
	}
	XOERR(nomChp,0) ;
	va_end(listArg) ;
	return -1;
}

/*DEF*/	int32	XoNmSetAttr(void *obj, char *nomChp, void *val, int32 lg,...)
{
	int32	numAttr;
	va_list	listArg;
	
	va_start(listArg,lg) ;
	if((numAttr=va_xogetfieldref(obj,nomChp,&obj,1,listArg))>=0)
	{
		va_end(listArg) ;
		return XoSetAttr(obj,numAttr,val,lg);
	}
	XOERR(nomChp,numAttr) ;
	va_end(listArg) ;
	return -1;
}

/*DEF*/	void	*XoNmGetAttr(void *obj, char *nomChp, int32 *lg,...)
{
	int32	lgLoc = 0;
	int32	numAttr;
	va_list	listArg;

	va_start(listArg,lg) ; if (lg) *lg = 0; else lg = &lgLoc;
	if((numAttr=va_xogetfieldref(obj,nomChp,&obj,0,listArg))>=0)
	{
//		if(!lg)
//		{
//			lg	= &lgLoc;
//		}
		va_end(listArg) ;
		return XoGetAttr(obj,numAttr,lg);
	}
	if	( numAttr != -6789 )
	{
		XOERR(nomChp,0) ;
	}

	/* acces sur un element de liste incorrect => pas de trace */

	va_end(listArg) ;
	return	NULL ;
}

/*DEF*/	int32	XoNmNbInAttr(void *obj, char *nomChp,...)
{
	int32	numAttr;
	va_list	listArg;
	
	va_start(listArg,nomChp) ;
	if((numAttr=va_xogetfieldref(obj,nomChp,&obj,0,listArg))>=0)
	{
		va_end(listArg) ;
		return XoNbInAttr(obj,numAttr);
	}
	if	( numAttr != -6789 )
		XOERR(nomChp,0) ;
	va_end(listArg) ;
	return -1;
}

/*DEF*/	int32	XoNmUnSetAttr(void *obj, char *nomChp,...)
{
	int32	numAttr;
	va_list	listArg;
	
	va_start(listArg,nomChp) ;
	if((numAttr=va_xogetfieldref(obj,nomChp,&obj,0,listArg))>=0)
	{
		va_end(listArg) ;
		return XoUnSetAttr(obj,numAttr);
	}
	XOERR(nomChp,0) ;
	va_end(listArg) ;
	return -1;
}

/*DEF*/	int32	XoNmIsSetAttr(void *obj, char *nomChp,...)
{
	int32	numAttr;
	va_list	listArg;
	
	va_start(listArg,nomChp) ;
	if((numAttr=va_xogetfieldref(obj,nomChp,&obj,0,listArg))>=0)
	{
		va_end(listArg) ;
		return XoIsSetAttr(obj,numAttr);
	}
	XOERR(nomChp,0) ;
	va_end(listArg) ;
	return -1;
}

/*DEF*/	int32	XoNmAttrValueIsDef(void *obj, char *nomChp,...)
{
	int32	numAttr;
	va_list	listArg;
	
	va_start(listArg,nomChp) ;
	if((numAttr=va_xogetfieldref(obj,nomChp,&obj,0,listArg))>=0)
	{
		va_end(listArg) ;
		return XoAttrValueIsDef(obj,numAttr);
	}
	XOERR(nomChp,0) ;
	va_end(listArg) ;
	return -1;
}

/*DEF*/	int32	XoNmFirstAttr(void *obj, char *nomChp,...)
{
	int32	numAttr;
	va_list	listArg;
	
	va_start(listArg,nomChp) ;
	if((numAttr=va_xogetfieldref(obj,nomChp,&obj,0,listArg))>=0)
	{
		va_end(listArg) ;
		return XoFirstAttr(obj,numAttr);
	}
	XOERR(nomChp,0) ;
	va_end(listArg) ;
	return -1;
}

/*DEF*/	int32	XoNmNextAttr(void *obj, char *nomChp,...)
{
	int32	numAttr;
	va_list	listArg;
	
	va_start(listArg,nomChp) ;
	if((numAttr=va_xogetfieldref(obj,nomChp,&obj,0,listArg))>=0)
	{
		va_end(listArg) ;
		return XoNextAttr(obj,numAttr);
	}
	XOERR(nomChp,0) ;
	va_end(listArg) ;
	return -1;
}

/*DEF*/	int32	XoNmPrevAttr(void *obj, char *nomChp,...)
{
	int32	numAttr;
	va_list	listArg;
	
	va_start(listArg,nomChp) ;
	if((numAttr=va_xogetfieldref(obj,nomChp,&obj,0,listArg))>=0)
	{
		va_end(listArg) ;
		return XoNextAttr(obj,numAttr);
	}
	XOERR(nomChp,0) ;
	va_end(listArg) ;
	return -1;
}

/*DEF*/	int32	XoNmLastAttr(void *obj, char *nomChp,...)
{
	int32	numAttr;
	va_list	listArg;
	
	va_start(listArg,nomChp) ;
	if((numAttr=va_xogetfieldref(obj,nomChp,&obj,0,listArg))>=0)
	{
		va_end(listArg) ;
		return XoLastAttr(obj,numAttr);
	}
	XOERR(nomChp,0) ;
	va_end(listArg) ;
	return -1;
}

static	int32	xoclearlist(char *obj, int32 numAttr)
{
	int32	nb ;


	nb	= 0 ;
	while	(XoFirstAttr(obj,numAttr)>0)
	{
		if	( XoSupInAttr(obj,numAttr) < 0 )
		{
			XOERR("",0) ;
			return	-2001 ;
		}
		nb++ ;
	}
	return	nb ;
}

static	int32	xofreelistobj(char *obj, int32 numAttr, int32 reccur)
{
	int32	lg;
	int32	nb ;
	void	*lobj ;

	nb	= 0 ;
	while(XoFirstAttr(obj,numAttr)>0)
	{
		lobj	= XoGetAttr(obj,numAttr,&lg);
		if	( lobj )
		{
			if	( XoFree(lobj,reccur) < 0 )
			{
				XOERR("",0) ;
				return	-1000 ;
			}
			if	( XoSupInAttr(obj,numAttr) < 0 )
			{
				XOERR("",0) ;
				return	-1001 ;
			}
			nb++ ;
		}
		else
		{
			XOERR("",0) ;
			return	-1002 ;
		}
	}
	return	nb ;
}

/*DEF*/	int32	XoDropList(void *obj,int32 numAttr,int32 withfree,int32 reccur)
{
	int32	numClass ;
	int32	mode ;
	int32	ret ;

	numClass	= XoType(obj) ;
	if	( numClass < 0 )
	{
		XOERR("bad obj",numClass) ;
		return	-1 ;
	}
	mode	= XoModeAttr(numAttr) ;
	switch	(mode)
	{
		case	BASIC_ATTR :
		case	OBJ_ATTR :
			XOERR("DropList bad mode",mode) ;
			return	-2 ;
		break ;
		case	LIST_BASIC_ATTR :
			ret	= xoclearlist(obj,numAttr) ;
		break ;
		case	LIST_OBJ_ATTR :
			if	( withfree )
				ret	= xofreelistobj(obj,numAttr,reccur) ;
			else
				ret	= xoclearlist(obj,numAttr) ;
		break ;
		default :
			XOERR("DropList unknowed mode",mode) ;
			return	-3 ;
		break ;
	}

	if	( ret < 0 )
	{
		XOERR("",ret) ;
	}
	return	ret ;

}



/*DEF*/	int32	XoNmDropList(void *obj, char *nomChp, int32 withfree, int32 reccur,...)
{
	int32	numAttr;
	va_list	listArg;
	
	va_start(listArg,reccur) ;
	if((numAttr=va_xogetfieldref(obj,nomChp,&obj,0,listArg))>=0)
	{
		va_end(listArg) ;
		return XoDropList(obj,numAttr,withfree,reccur);
	}
	XOERR(nomChp,0) ;
	va_end(listArg) ;
	return -1;
}



/*DEF*/	char	*XoNmAttrToAscii(void *obj, char *nom, char *str,...)
{
	int32	numAttr;
	va_list	listArg;
	char	*pt ;
	

	if(!obj)
	{
		XOERR("null obj",0);
		return	NULL ;
	}
	if(!str)
	{
		XOERR("null buffer",0);
		return	NULL ;
	}
	va_start(listArg,str) ;
	if((numAttr=va_xogetfieldref(obj,nom,&obj,1,listArg))<0)
	{
		if	( numAttr != -6789 )
		{
			XOERR(nom,0) ;
		}

		/* acces sur un element de liste incorrect => pas de trace */
		va_end(listArg) ;
		return	NULL ;
	}
	va_end(listArg) ;
	pt	= XoAttrToPtrAscii(obj,numAttr,str) ;
	if	( !pt )
		return	NULL ;
	strcpy	(str,pt) ;
	return	str ;
}

/*DEF*/	char	*XoNmAttrToPtrAscii(void *obj, char *nom, char *str,...)
{
	int32	numAttr;
	va_list	listArg;
#if	0
	char	*val;
	int32	lg;
#endif
	

	if(!obj)
	{
		XOERR("null obj",0);
		return	NULL ;
	}
	va_start(listArg,str) ;
	if((numAttr=va_xogetfieldref(obj,nom,&obj,1,listArg))<0)
	{
		if	( numAttr != -6789 )
		{
			XOERR(nom,0) ;
		}

		/* acces sur un element de liste incorrect => pas de trace */
		va_end(listArg) ;
		return	NULL ;
	}
	va_end(listArg) ;
	return	XoAttrToPtrAscii(obj,numAttr,str) ;
}


/*DEF*/	int32	XoNmAsciiToAttr(void *obj, char *nom, char *val,...)
{
	int32	numAttr;
	va_list	listArg;
	


	if(!obj)
	{
		XOERR("null obj",0);
		return	-1 ;
	}
	va_start(listArg,val) ;
	if((numAttr=va_xogetfieldref(obj,nom,&obj,1,listArg))<0)
	{
		va_end(listArg) ;
		return -2;
	}
	va_end(listArg) ;
	return	XoAsciiToAttr(obj,numAttr,val) ;
}

/*
 *	idem XoNmAsciiToAttr2() mais on autorise la valorisation
 *	avec la chaine vide
 */

/*DEF*/	int32	XoNmAsciiToAttr2(void *obj, char *nom, char *val,...)
{
	int32	numAttr;
	va_list	listArg;
	


	if(!obj)
	{
		XOERR("null obj",0);
		return	-1 ;
	}
	va_start(listArg,val) ;
	if((numAttr=va_xogetfieldref(obj,nom,&obj,1,listArg))<0)
	{
		va_end(listArg) ;
		return -2;
	}
	va_end(listArg) ;
	if(!val)
	{
		XOERR("null value",0);
		return -3;
	}
	return	XoAsciiToAttr2(obj,numAttr,val) ;
}

/*
 *
 *	Sauve un objet XO dans un fichier en ajoutant une entete
 *	la sauvegarde est faite en premier lieu dans un fichier temporaire
 *	si une erreur se produit en cours de route , le fichier destination
 *	n'est donc pas detruit .
 *
 */

int32	XoRenameFile(char *old,char *new)
{
#ifdef	IS32
	unlink	(new) ;
	if	( link(old,new) )
	{
		return	-1 ;
	}
	if	( unlink(old) )
	{
		return	-2 ;
	}
	return	0 ;
#else
	return	rename(old,new) ;
#endif
}

/*DEF*/	int32	__XoExtSave(char *file, void *obj, char *info,int32 flags)
{
	int32	fd;
	int32	pid=getpid() ;
	char	tmpfile[256] ;
	char	*sl ;

	if	( !file || !obj )
	{
		XOERR("null param",0);
		return	-1 ;
	}


	sl	= strrchr(file,XO_CAR_SPEC) ;
	if	( sl )
	{
		*sl	= '\0' ;
		sprintf	(tmpfile,"%s%c_xo%ld.tmp",file,XO_CAR_SPEC,(long)pid) ;
		*sl	= XO_CAR_SPEC ;
	}
	else
	{
		sprintf	(tmpfile,"_xo%ld.tmp",(long)pid) ;
	}
	if((fd=open(tmpfile,XO_FIC_BINARY|O_WRONLY|O_CREAT|O_TRUNC,0666))>=0)
	{
#if	0
		if	( XoSaveHeader(info,fd) < 0 )
		{
			XOERR("XoSaveHeader() failed",0);
			return	-2 ;
		}
#endif
		if	( XoSaveFlg(obj,fd,flags) < 0 )
		{
			XOERR("XoSave() failed",0);
			return	-3 ;
		}
		close((int)fd);
#ifdef		DOS
		unlink	(file) ;
#endif
		if	( XoRenameFile(tmpfile,file) < 0 )
		{
			XOERR("XoRenameFile() failed",0);
			return	-4 ;
		}
	}
	else
	{
		XOERR("open() failed",0);
		return	-5 ;
	}
	return	0 ;
}

/*DEF*/	int32	XoExtSave(char *file, void *obj, char *info)
{
	return	__XoExtSave(file,obj,info,0);
}

/*DEF*/	int32	XoExtSaveFlg(char *file, void *obj, char *info,int32 flags)
{
	return	__XoExtSave(file,obj,info,flags);
}


/*
 *
 *	Charge un objet XO depuis un fichier
 *
 */

/*DEF*/	void	*XoExtLoad(char *file)
{
	int32	fd;
	void	*obj;

	if((fd=open(file,XO_FIC_BINARY|O_RDONLY))>=0)
	{
#if	0
	char	*hd ;
		hd=XoLoad(fd);
		if(!hd)
		{
			close((int)fd) ;
			XOERR("XoLoad() header failed",0);
			return	NULL ;
		}
		XoFree(hd,1);
#endif

		obj=XoLoad(fd);
		if(!obj)
		{
			close((int)fd) ;
			XOERR("XoLoad() failed",0);
			return	NULL ;
		}
		close((int)fd);
		return obj;
	}
	else
	{
		XOERR("open() failed",0);
		return	NULL ;
	}
	return NULL;
}

/*
 *
 *	Charge un objet de reference XO depuis un fichier
 *
 */

/*DEF*/	int32	XoExtLoadRef(char *file)
{
	int32	fd;
	void	*ref ;

	if((fd=open(file,XO_FIC_BINARY|O_RDONLY))>=0)
	{
#if	0
		void	*hd ;
		hd=XoLoad(fd);
		if(!hd)
		{
			close((int)fd) ;
			XOERR("XoLoad() header failed",0);
			return	-1 ;
		}
		XoFree(hd,1);
#endif

		ref	= XoLoadRef(fd);
		if(!ref)
		{
			close((int)fd) ;
			XOERR("XoLoadRef() failed",0);
			return	-2 ;
		}
		close((int)fd);
	}
	else
	{
		XOERR("open() failed",0);
		return	-3 ;
	}
	XoFree	(ref,1) ;
	return	1 ;
}

// TODO chercher une numerotation file_123.xor en fin de fichier pour chosir
// un ordre de chargement

/*DEF*/	int32	XoExtLoadRefDir(char *path)
{
	void	*dir;
	char	*name;
	char	*point;
	int	nb = 0;

	dir	= xo_openDir(path);
	if	(!dir)
		return	-1;

	while	( (name = xo_readAbsDir(dir)) && *name )
	{
		if	(*name == '.' && *(name+1) == '.')	continue;
		point	= xo_suffixname(name);
		if	( point && *point && strcmp(point,XO_XOR_EXT) == 0 )
		{
//printf("filename='%s'\n",name);
			if (XoExtLoadRef(name) < 0)
			{
				XOERR("XoExtLoadRefDir() failed",0);
				return	-2;
			}
			nb++;
		}
	}

	xo_closeDir(dir);
	return	nb;
}


/*DEF*/	int32	XoNmDelObjRef(char *class)
{
	int32	numClass;

	if	((numClass=XoGetNumType(class))<0)
	{
		XOERR	(class,0) ;
		return	-1000 ;
	}
	return XoDelObjRef(numClass);
}

/*
 *	change le type d'une occurence 
 *	attention si reccur == 0 on perd tous les attributs composes .
 */


/*DEF*/	int32	XoChangeRef(void *obj, int32 classe, int32 reccur)
{
	void	*otmp ;
	int32	ret ;


	ret	= XoType(obj) ;
	if	( ret < 0 )
	{
		XOERR("XoType",ret) ;
		return	ret;
	}
#if	0
	if	( ret == classe )
	{
		XOERR("same type",ret) ;
		return	-1;
	}
#endif
	otmp	= XoNew(classe) ;
	if	( !otmp )
	{
		XOERR("XoNew",classe) ;
		return	-2;
	}
	ret	= XoTryCopy(otmp,obj,reccur) ;
	if	( ret < 0 )
	{
		XOERR("XoTryCopy",ret) ;
		return	ret;
	}
	ret	= XoReUse(obj,classe) ;
	if	( ret < 0 )
	{
		XOERR("XoReUse",ret) ;
		return	ret;
	}
	ret	= XoMove(otmp,obj) ;
	if	( ret < 0 )
	{
		XOERR("XoMove",ret) ;
		return	ret;
	}
	return	ret ;
}

/*
 *	change le type d'une occurence 
 */


/*DEF*/	int32	XoNmChangeRef(void *obj, char *class, int32 reccur)
{
	int32	numClass;

	if	((numClass=XoGetNumType(class))<0)
	{
		XOERR	(class,0) ;
		return	-1000 ;
	}
	return	XoChangeRef(obj,numClass,reccur);
}

/*DEF*/	int32	XoNmChangeRef2(void *obj, char *class)
{
	int32	numClass;

	if	((numClass=XoGetNumType(class))<0)
	{
		XOERR	(class,0) ;
		return	-1000 ;
	}
	return	XoChangeRef2(obj,numClass);
}


/*
 *	ecrit dans buf les attributs nommes de obj
 *
 *	ex :
 *	XoSprintf2(buf,oA,
 *		"%-10.10s ","chp5.chp2",
 *		"%ld ","chp5.chp1",
 *		"%s ","chp6",
 *		"%ld ","chp7",
 *		"%ld","chp8[1].chp1",NULL) ;
 */

/*DEF*/	int32	XoSprintf2(char *buf,char *obj,...)
{
	va_list	listArg ;

	void	*tmpobj ;
	char	*fmt ;
	char	*txt ;


	int32	lg ;
	int32	lgBuf ;
	int32	numAttr ;
	char	*val ;

	if	(!obj)
	{
		XOERR("null obj",0);
		return	-1 ;
	}
	if	(!buf)
	{
		XOERR("null buf",0);
		return	-2 ;
	}

	*buf	= '\0' ;
	lgBuf	= 0 ;
	va_start(listArg,obj) ;
	while	((fmt=va_arg(listArg,char *)))
	{
		tmpobj	= obj ;
		txt	= va_arg(listArg,char *) ;
		if	(!txt)
		{
			XOERR	("bad format",0) ;
			return	-1000 ;
		}
#ifdef	TRACE
fprintf(stderr,"%lx %s %s\n",(long)tmpobj,fmt,txt);
fflush(stderr) ;
#endif
		if	((numAttr=xogetfieldref(tmpobj,txt,&tmpobj,1))<0)
		{
			XOERR(txt,0);
			return	-3 ;
		}
		if	(!(val=XoGetAttr(tmpobj,numAttr,&lg)))
		{
			continue ;
		}
		switch	(XoTypeAttr(numAttr))
		{
		case XoInt4B:	lg = sprintf(buf,fmt,*(int32 *)val);	break;
#if	0
		case XoVoid:	lg = sprintf(buf,fmt,(long)val);	break;
#endif
		case XoString:	lg = sprintf(buf,fmt,val);		break;
		case XoBool:	lg = sprintf(buf,fmt,*(int32 *)val);	break;
		case XoChar:	lg = sprintf(buf,fmt,*(char *)val);	break;
		default:
			XOERR("bad type",0);
			return	-5 ;
		break ;
		}
#ifdef	TRACE
fprintf(stderr,"%ld %s %s -> '%s'\n",(long)lg,fmt,txt,buf);
fflush(stderr) ;
#endif
		buf	+= lg ;
		lgBuf	+= lg ;
	}
	va_end(listArg) ;
	return	lgBuf ;
}

/*DEF*/	int32	XoSprintf(char *buf,char *obj,char *fmt,...)
{
	va_list	listArg ;

	void	*tmpobj ;
	char	*txt ;
	char	*pt ;


	int32	lg ;
	int32	lgBuf ;
	int32	numAttr ;
	void	*val ;

	if	(!obj)
	{
		XOERR("null obj",0);
		return	-1 ;
	}
	if	(!buf)
	{
		XOERR("null buf",0);
		return	-2 ;
	}

	*buf	= '\0' ;
	lgBuf	= 0 ;
	va_start(listArg,fmt) ;
	pt	= strchr(fmt,'%') ;
	if	( pt && pt != fmt )
	{	/* "aaaa%s...." */
		*pt	= '\0' ;
		lg	= sprintf(buf,fmt,"") ;
		*pt	= '%' ;
		fmt	= pt ;
		buf	+= lg ;
		lgBuf	+= lg ;
	}

	while	( fmt && *fmt && (txt=va_arg(listArg,char *)) )
	{
		tmpobj	= obj ;
		pt	= strchr(fmt+1,'%') ;	/* jusqu'au prochain fmt */
		if	(pt)
			*pt	= '\0' ;
#ifdef	TRACE
fprintf(stderr,"%lx %s %s\n",(long)tmpobj,fmt,txt);
fflush(stderr) ;
#endif
		if	((numAttr=xogetfieldref(tmpobj,txt,&tmpobj,1))<0)
		{
			if	(pt)
				*pt	= '%' ;
			XOERR(txt,0);
			return	-3 ;
		}
		if	(!(val=XoGetAttr(tmpobj,numAttr,&lg)))
		{
			if	(pt)
				*pt	= '%' ;
			fmt	= pt ;
			continue ;
		}
		switch	(XoTypeAttr(numAttr))
		{
		case XoInt4B:	lg = sprintf(buf,fmt,*(int32 *)val);	break;
#if	0
		case XoVoid:	lg = sprintf(buf,fmt,(long)val);	break;
#endif
		case XoString:	lg = sprintf(buf,fmt,val);		break;
		case XoBool:	lg = sprintf(buf,fmt,*(int32 *)val);	break;
		case XoChar:	lg = sprintf(buf,fmt,*(char *)val);	break;
		default:
			if	(pt)
				*pt	= '%' ;
			XOERR("bad type",0);
			return	-5 ;
		break ;
		}
#ifdef	TRACE
fprintf(stderr,"%ld %s %s -> '%s'\n",(long)lg,fmt,txt,buf);
fflush(stderr) ;
#endif
		buf	+= lg ;
		lgBuf	+= lg ;
		if	(pt)
			*pt	= '%' ;
		fmt	= pt ;
	}
	va_end(listArg) ;
	return	lgBuf ;
}

static	char	*fmt_attr(char *obj,char *txt,char **val,int32 *type)
{
	void	*tmpobj ;
	int32	numAttr ;
	int32	lg ;

	tmpobj	= obj ;
	if	((numAttr=xogetfieldref(tmpobj,txt,&tmpobj,1))<0)
	{
		XOERR(txt,0);
		return	NULL ;
	}
	*val=XoGetAttr(tmpobj,numAttr,&lg) ;
	if(!*val)
		return	"s" ;
	switch	((*type=XoTypeAttr(numAttr)))
	{
	case XoInt4B:
	case XoBool:
		return	"d" ;
	break ;
#if	0
	case XoVoid :
		return "ld" ;
	break ;
#endif
	case XoString:
		return	"s" ;
	break ;
	case XoChar:
		return	"c" ;
	break ;
	default:
		XOERR("bad type",0);
		return	NULL ;
	break ;
	}
	return	NULL ;
}


/*DEF*/	int32	XoExtSprintf(char *buf,char *obj,char *fmt,int32 nbarg,...)
{
	va_list	listArg ;
	char	tmpfmt[256] ;
	char	tmptxt[256] ;
	char	*spfmt ;
	char	*pt ;
	char	*tpt ;

	int32	i ;
	int32	j ;
	int32	type = 0 ;
	int32	lg = 0 ;
	int32	lgBuf ;
	char	*val ;

	if	(!obj)
	{
		XOERR("null obj",0);
		return	-1 ;
	}
	if	(!buf)
	{
		XOERR("null buf",0);
		return	-2 ;
	}

	*buf	= '\0' ;
	lgBuf	= 0 ;
	va_start(listArg,nbarg) ;
	pt	= strchr(fmt,'%') ;
	if	( pt && pt != fmt )
	{	/* "aaaa%s...." */
		*pt	= '\0' ;
		lg	= sprintf(buf,fmt,"") ;
		*pt	= '%' ;
		fmt	= pt ;
		buf	+= lg ;
		lgBuf	+= lg ;
	}

	while	(fmt && *fmt)
	{
		pt	= strchr(fmt+1,'%') ;	/* jusqu'au prochain fmt */
		if	(pt)
			*pt	= '\0' ;

		tpt	= fmt ;
		*tmptxt	= '\0' ;
		j	= 0 ;
		*tmpfmt	= '\0' ;
		i	= 0 ;
		while	( *tpt == '%' || *tpt=='-' || *tpt=='.'
			||	(*tpt>='0' && *tpt<='9') )
		{
			tmpfmt[i++]	= *tpt ;
			tpt++ ;
		}
		tmpfmt[i]	= '\0' ;
		switch	(*tpt)
		{
			case	's' :
			case	'd' :
			case	'c' :
				tmpfmt[i++]	= *tpt ;
				tmpfmt[i]	= '\0' ;
				tpt++ ;
				if	(nbarg>0)
				{
					val	= va_arg(listArg,char *) ;
					nbarg-- ;
				}
				else
				{
					XOERR("arg ????",0);
					return	-3 ;
				}
			break ;
			case	'<' :
				tpt++ ;
				while	( *tpt && *tpt != '>' )
				{
					tmptxt[j++]	= *tpt ;
					tpt++ ;
				}
				tmptxt[j]	= '\0' ;
				tpt++ ;
				spfmt	= fmt_attr(obj,tmptxt,&val,&type) ;
				if	( !spfmt )
				{
					if	(pt)
						*pt	= '%' ;
					XOERR("fmt ????",0);
					return	-4 ;
				}
				strcat(tmpfmt,spfmt) ;
			break ;
			default	:
				if	(pt)
					*pt	= '%' ;
				XOERR("fmt ????",0);
				return	-5 ;
			break ;
		}
		/*
		 *	il y a du texte apres le format
		 */
		if	(*tpt)
			strcat(tmpfmt,tpt) ;
/*
fprintf(stderr,"tmpfmt '%s' tmptxt '%s'\n",tmpfmt,tmptxt) ;
fflush(stderr) ;
*/
		if	(strlen(tmptxt))
		{
		/*
			lg	= XoSprintf(buf,obj,tmpfmt,tmptxt) ;
		*/
			if	(val)
			{
				switch	(type)
				{
				case XoInt4B:lg=sprintf(buf,tmpfmt,*(int32 *)val);	
					break;
#if	0
				case XoVoid:lg=sprintf(buf,tmpfmt,(long)val);	
					break;
#endif
				case XoString:lg=sprintf(buf,tmpfmt,val);
					break;
				case XoBool:lg=sprintf(buf,tmpfmt,*(int32 *)val);	
					break;
				case XoChar:lg=sprintf(buf,tmpfmt,*(char *)val);
					break;
				}
			}
			else	/* cas des attributs non renseignes */
				lg	= sprintf(buf,tmpfmt,"") ;
		}
		else
		{
			lg	= sprintf(buf,tmpfmt,val) ;
		}
		buf	+= lg ;
		lgBuf	+= lg ;



		if	(pt)
			*pt	= '%' ;
		fmt	= pt ;
	}

	va_end(listArg) ;
	return	lgBuf ;
}


/*DEF*/ int32 XoSaveAsciiEx( void * pObj, char * pFileName )
{
FILE*	file;
int32	cr = -1;

#ifdef	WIN32
	file = fopen(pFileName,"wb");
#else
	file = fopen(pFileName,"w");
#endif
	if	( file )
	{
		cr = XoSaveAscii( pObj, file );
		fclose ( file );
	}
	return cr;
}

/*DEF*/	void	*XoLoadAsciiEx(char *pFileName)
{
FILE*	file;
void	*ret	= NULL;

#ifdef	WIN32
	file = fopen(pFileName,"rb");
#else
	file = fopen(pFileName,"r");
#endif
	if	( file )
	{
		ret = XoLoadAscii( file );
		fclose ( file );
	}
	return ret;
}


/*DEF*/	int32	XoDumpNbOccEx(char *pFileName, int32 not0, int32 notpredef)
{
	FILE	*file;

#ifdef	WIN32
	if( (file = fopen( pFileName, "wb" )) )
#else
	if( (file = fopen( pFileName, "w" )) )
#endif
	if	( !file )
		return	-1;

	XoDumpNbOcc(file,not0,notpredef);

	fclose	(file);
	return	0;
}
