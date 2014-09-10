
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


typedef	struct
{
	t_objet_ref	*os_or ;
}	t_obj_sort ;

static	char	*xo_file = __FILE__ ;


static	t_obj_sort	*mybsearchobj(char *name, int32 lg) ;
static	t_attribut_ref	*mybsearchattr(t_objet_ref *or,char *name, int32 lg);

extern	t_objet_ref	XoTabObjInt[] ;
extern	int32		XoNbObjInt ;

#ifndef	DOS
extern	t_objet_ref	XoTabObjUser[] ;
extern	int32		XoNbObjUser ;
#endif

t_objet_ref	**XoTabObj ;
int32		XoNbObj ;

t_obj_sort	*XoTabObjSort ;
int32		XoNbObjSort ;

int32		(*XofDeamonNew)() ;
int32		(*XofDeamonFree)() ;

/*
 *	ce flag permet a l'api d'utiliser les fonctions de l'api
 *	sans activer les deamons
 */

int32		XoDisableDeamon ;

static		int32		Joint32Done	= 0 ;


static		char		*BoolArValue	= "0|1" ;

static	int32		SzObj ;
static	int32		SzTabattr ;
static	int32		SzTabbasic ;
static	int32		AcceptDuplObjRef = 1;

/*DEF*/	int32	XoAcceptDuplObjRef(void)
{
	AcceptDuplObjRef = 1 ;
	return	0 ;
}

/*DEF*/	int32	XoNoAcceptDuplObjRef(void)
{
	AcceptDuplObjRef = 0 ;
	return	0 ;
}

/*
 *	activation des deamons
 */

/*DEF*/	int32	XoDeamonNew(int32 (*f) (/* ??? */))
{
	XofDeamonNew	= f ;
	return	1 ;
}


/*DEF*/	int32	XoDeamonFree(int32 (*f) (/* ??? */))
{
	XofDeamonFree	= f ;
	return	1 ;
}

/*
 *	jonction des definitions int32ernes et utilisateur
 */

static	int32	xojoint32(void)
{
	int32	i ;

#ifndef	DOS
	XoNbObj	= XoNbObjInt + XoNbObjUser ;
#else
	XoNbObj	= XoNbObjInt ;
#endif

	XoTabObj= (t_objet_ref **)
		malloc((size_t)(XoNbObj * sizeof(t_objet_ref *))) ;

	if	( !XoTabObj )
	{
		XOERR("malloc()",errno) ;
		return	-301 ;
	}

	XoTabObjSort= (t_obj_sort *)
		malloc((size_t)(XoNbObj * sizeof(t_obj_sort))) ;

	if	( !XoTabObjSort )
	{
		XOERR("malloc()",errno) ;
		return	-302 ;
	}

	for	( i = 0 ; i < XoNbObjInt ; i++ )
	{
		XoTabObj[i]	= &XoTabObjInt[i] ;
	}

#ifndef	DOS
	for	( i = 0 ; i < XoNbObjUser ; i++ )
	{
		XoTabObj[XoNbObjInt+i]	= &XoTabObjUser[i] ;
	}
#endif

	for	( i = 0 ; i < XoNbObj ; i++ )
	{
		XoTabObjSort[i].os_or	= XoTabObj[i] ;
	}
	XoNbObjSort	= XoNbObj ;

	return	XoNbObj ;
}


static	void	free_or(t_objet_ref *or)
{
	int32	i ;
	t_attribut_ref	*ar ;

	if	(!or)
		return ;

	if	(or->or_name)
		free	(or->or_name) ;
	if	(or->or_from)
		free	(or->or_from) ;
	if	(or->or_alias)
		free	(or->or_alias) ;
	if	(or->or_alias_has)
		free	(or->or_alias_has) ;

	for	( i = 0 ; i < or->or_nb_attr ; i++ )
	{
		ar	= &or->or_tab_attr[i] ;
		if	(ar->ar_name)
			free	(ar->ar_name) ;
		if	(ar->ar_type_name)
			free	(ar->ar_type_name) ;
		if	(ar->ar_value && ar->ar_value != BoolArValue )
			free	(ar->ar_value) ;
		if	(ar->ar_default)
			free	(ar->ar_default) ;
		if	(ar->ar_help_line)
			free	(ar->ar_help_line) ;
		if	(ar->ar_help_file)
			free	(ar->ar_help_file) ;
	}
	if	( or->or_tab_attr )
		free	(or->or_tab_attr) ;
	if	( or->or_stab_attr )
		free	(or->or_stab_attr) ;
}


/*DEF*/	int32	XoEnd(void)
{
	t_objet_ref	*or ;
	int32	i ;

	XoSharedDicoFree();

	if	( !Joint32Done )
	{
		XOERR("",0) ;
		return -1 ;
	}

#ifndef	DOS
	if	( XoNbObjUser > 0 )
	{
		XOERR("",0) ;
		return -2 ;
	}
#endif

	Joint32Done	= 0 ;

	for	( i = 0 ; i < XoNbObj ; i++ )
	{
		or	= XoTabObj[i] ;
		if	( !or )				continue ;
		if	( or->or_num < BaseUserObj(0) )	continue ;

		free_or	(or) ;
		free	(or) ;
	}

	free	(XoTabObj) ;
	free	(XoTabObjSort) ;
	return	0 ;
}

/*DEF*/	int32	XoFreeAll(void)
{
	t_objet_ref	*or ;
	unsigned int	nb = 0;
	int32	i ;
	void	*obj;

	for	( i = 0 ; i < XoNbObj ; i++ )
	{
		or	= XoTabObj[i] ;
		if	( !or )				continue ;
		obj	= XoFirstObj(or->or_num);
		while	(obj)
		{
			nb++;
			XoFree(obj,0);
			obj	= XoNextObj(or->or_num);
		}
	}

	return	nb ;
}

/*
 *	retourne le nom d'un objet a partir de son numero
 */

/*DEF*/	char	*XoGetRefObjName(int32 obj)
{
	obj	= NUM_OBJ_TO_INDICE(obj) ;
	if	( obj >= 0 && obj < XoNbObj )
	{
		if	( !XoTabObj[obj] )
		{
			XOERR("undefined obj",obj) ;
			return	NULL ;
		}
		return	XoTabObj[obj]->or_name ;
	}
	XOERR("bad num obj",obj) ;
	return	NULL ;
}

/*DEF*/	char	*XoGetRefObjNameSilent(int32 obj)
{
	obj	= NUM_OBJ_TO_INDICE(obj) ;
	if	( obj >= 0 && obj < XoNbObj )
	{
		if	( !XoTabObj[obj] )
		{
			return	NULL ;
		}
		return	XoTabObj[obj]->or_name ;
	}
	return	NULL ;
}

/*DEF*/	char	*XoGetRefObjNameFrom(int32 obj)
{
	obj	= NUM_OBJ_TO_INDICE(obj) ;
	if	( obj >= 0 && obj < XoNbObj )
	{
		if	( !XoTabObj[obj] )
		{
			return	NULL ;
		}
		return	XoTabObj[obj]->or_from ;
	}
	return	NULL ;
}

/*DEF*/	char	*XoGetRefObjNameAlias(int32 obj)
{
	obj	= NUM_OBJ_TO_INDICE(obj) ;
	if	( obj >= 0 && obj < XoNbObj )
	{
		if	( !XoTabObj[obj] )
		{
			return	NULL ;
		}
		return	XoTabObj[obj]->or_alias ;
	}
	return	NULL ;
}

/*DEF*/	char	*XoGetRefObjNameHasAlias(int32 obj)
{
	obj	= NUM_OBJ_TO_INDICE(obj) ;
	if	( obj >= 0 && obj < XoNbObj )
	{
		if	( !XoTabObj[obj] )
		{
			return	NULL ;
		}
		return	XoTabObj[obj]->or_alias_has ;
	}
	return	NULL ;
}

/*DEF*/ int32	XoGetRefObjStExt(int32 obj)
{
	obj	= NUM_OBJ_TO_INDICE(obj) ;
	if	( obj >= 0 && obj < XoNbObj )
	{
		if	( !XoTabObj[obj] )
		{
			return	-1 ;
		}
		return	XoTabObj[obj]->or_stext ;
	}
	return	0 ;
}

// 
// object contains only an hidden wrapper attribut ?
//
/*DEF*/ int32	XoGetRefObjWrapHidden(int32 obj)
{
	obj	= NUM_OBJ_TO_INDICE(obj) ;
	if	( obj >= 0 && obj < XoNbObj )
	{
		if	( !XoTabObj[obj] )
		{
			return	-1 ;
		}
		return	XoTabObj[obj]->or_wraphidden ;
	}
	return	0 ;
}

// 
// object has an attribut name XOML_WRAPPE_ATTR ?
//
/*DEF*/ int32	XoGetRefObjWrapped(int32 obj)
{
	obj	= NUM_OBJ_TO_INDICE(obj) ;
	if	( obj >= 0 && obj < XoNbObj )
	{
		if	( !XoTabObj[obj] )
		{
			return	-1 ;
		}
		return	XoTabObj[obj]->or_wrapped ;
	}
	return	0 ;
}

// 
// object has an attribut name XOML_CUSTOM_ATTR ?
//
/*DEF*/ int32	XoGetRefObjCustomAttributs(int32 obj)
{
	obj	= NUM_OBJ_TO_INDICE(obj) ;
	if	( obj >= 0 && obj < XoNbObj )
	{
		if	( !XoTabObj[obj] )
		{
			return	-1 ;
		}
		return	XoTabObj[obj]->or_custom ;
	}
	return	0 ;
}

/*DEF*/ int32	XoDumpRefObjFromList(int32 obj,FILE *fout)
{
	t_objet_ref	*or ;
	int		ret;

	obj	= NUM_OBJ_TO_INDICE(obj) ;
	if	( obj >= 0 && obj < XoNbObj )
	{
		if	( !XoTabObj[obj] )
		{
			return	-1;
		}
		or	= XoTabObj[obj];
		if	(or->or_from && *(or->or_from))
		{
			if	(fout)
			fprintf(fout,"%-20.20s from %-20.20s alias %-20.20s\n",
			or->or_name,or->or_from,or->or_alias);
			ret = XoDumpRefObjFromList(XoGetNumType(or->or_from),
						fout);
			if	(ret < 0)
				return	ret;
			return	ret+1;
		}
#if	0
		if	(fout)
		fprintf(fout,"%-20.20s\n",or->or_name);
#endif
		return	1;
	}
	return	-2;
}

/*DEF*/ int32	XoGetRefObjFromList(int32 obj,char *tab[],int32 lev)
{
	t_objet_ref	*or ;
	int		ret;

	obj	= NUM_OBJ_TO_INDICE(obj) ;
	if	( obj >= 0 && obj < XoNbObj )
	{
		if	( !XoTabObj[obj] )
		{
			return	-1;
		}
		or	= XoTabObj[obj];
		if	(lev == 0)
			tab[lev]	= or->or_name;
		if	(or->or_from && *(or->or_from))
		{
			lev	= lev + 1;
			tab[lev]	= or->or_from;
			ret = XoGetRefObjFromList(XoGetNumType(or->or_from),
						tab,lev);
			if	(ret < 0)
				return	ret;
			return	ret+1;
		}
		return	1;
	}
	return	-2;
}

/*
 *	retourne le numero d'objet en fonction de son nom
 */

/*DEF*/	int32	XoGetNumType(char *name)
{
	int32	lg ;
	t_obj_sort	*ptos ;

	if	( !name /* || !(lg=strlen(name)) */ )
	{
		XOERR("bad name obj",0) ;
		return	-1 ;
	}
	lg	= strlen(name) ;
	if	( !lg )
	{
		XOERR("bad name obj",0) ;
		return	-2 ;
	}
	ptos	= (t_obj_sort *)
		mybsearchobj(name,lg) ;
	if	( !ptos )
	{
/*
		XOERR(name,0) ;
*/
		return	-3 ;
	}
	return	ptos->os_or->or_num ;

}

/*
 *	fonction de comparaison des objets pour qsort
 */
static	int	objcmp(const void *o1, const void *o2)
{
	return	(
	memcmp (((t_obj_sort *)o1)->os_or->or_name,
		((t_obj_sort *)o2)->os_or->or_name,
		(size_t)(((t_obj_sort *)o1)->os_or->or_lg_name+1))
	) ;
}


/*
 *	tri des objets de reference
 */

static	void sortobjref(void)
{
	qsort(XoTabObjSort,(size_t)XoNbObjSort,sizeof(t_obj_sort),objcmp) ;
}


/*
 *	fonction de comparaison des attributs pour qsort
 */
static	int	attrcmp(const void *pa1, const void *pa2)
{
	t_attribut_ref	**a1;
	t_attribut_ref	**a2;


	a1	= *((t_attribut_ref **)pa1);
	a2	= *((t_attribut_ref **)pa2);

	return	(
	memcmp (((t_attribut_ref *)a1)->ar_name,
		((t_attribut_ref *)a2)->ar_name,
		(size_t)(((t_attribut_ref *)a1)->ar_lg_name+1))
	) ;
}

/*
 *	tri des attributs de reference dans un objet
 */

static	void sortattrref(t_objet_ref *or)
{
	if	(!or || or->or_nb_attr <= 1)
		return;
	qsort(&(or->or_stab_attr[0]),(size_t)or->or_nb_attr,
				sizeof(t_attribut_ref *),attrcmp) ;
}

/*
 *	renomme une classe d'objets
 */

/*DEF*/	int32	XoRenameRefObj(char *oldname, char *newname)
{
	char		*val ;
	int32		ornum ;
	int32		lg ;
	t_objet_ref	*or;

	if	( !oldname || !*oldname )
	{
		XOERR("",0) ;
		return	-1 ;
	}
	if	( !newname || !*newname )
	{
		XOERR("",0) ;
		return	-2 ;
	}
	if	( strcmp(oldname,newname) == 0 )
		return	0 ;

	ornum	= XoGetNumType(oldname) ;
	if	( ornum < 0 )
		return	-3 ;

	if	( ornum < BaseUserObj(0) )
	{
		XOERR("predefined class",0) ;
		return	-4 ;
	}
	ornum	= NUM_OBJ_TO_INDICE(ornum) ;
	if	( ornum < 0 || ornum >= XoNbObj )
	{
		XOERR("??? numobj",ornum) ;
		return	-5 ;
	}

	if	( XoGetNumType(newname) > 0 )
	{
		XOERR("class exists",0) ;
		return	-6 ;
	}
	or	= XoTabObj[ornum] ;
	if	( !or )
		return	-7 ;

	lg	= strlen(newname) ;
	val	= (char *)malloc((size_t)(lg+1)) ;
	if	( !val )
	{
		XOERR("err malloc()",0) ;
		return	-8 ;
	}
	strcpy	(val,newname) ;

	if	( or->or_name )
		free	(or->or_name) ;
	or->or_name	= val ;
	or->or_lg_name	= lg ;

	sortobjref() ;

	return	1 ;
}

/*
 *	retourne le nbre d'attributs d'un objet
 */

/*DEF*/	int32	XoNbAttr(int32 obj)
{
	obj	= NUM_OBJ_TO_INDICE(obj) ;
	if	( obj >= 0 && obj < XoNbObj )
	{
		if	( !XoTabObj[obj] )
		{
			XOERR("undefined obj",obj) ;
			return	-1 ;
		}
		return	XoTabObj[obj]->or_nb_attr ;
	}
	XOERR("bad num obj",obj) ;
	return	-1 ;
}

/*
 *	retourne le nbre d'objets alloues du type
 */

/*DEF*/	int32	XoNbOccObj(int32 obj)
{
	obj	= NUM_OBJ_TO_INDICE(obj) ;
	if	( obj >= 0 && obj < XoNbObj )
	{
		if	( !XoTabObj[obj] )
		{
			XOERR("undefined obj",obj) ;
			return	-1 ;
		}
		return	XoTabObj[obj]->or_nb_obj ;
	}
	XOERR("bad num obj",obj) ;
	return	-1 ;
}


/*
 *	retourne le 1er objet du type
 */

/*DEF*/	void	*XoFirstObj(int32 obj)
{
	obj	= NUM_OBJ_TO_INDICE(obj) ;
	if	( obj < 0 || obj >= XoNbObj )
	{
		XOERR("bad num obj",obj) ;
		return	NULL ;
	}
	if	( !XoTabObj[obj] )
	{
		XOERR("undefined obj",obj) ;
		return	NULL ;
	}
	XoTabObj[obj]->or_curr = XoTabObj[obj]->or_first ;
	return	XoTabObj[obj]->or_curr ;
}


/*
 *	retourne l'objet suivant du type
 */

/*DEF*/	void	*XoNextObj(int32 obj)
{
	t_objet_ref	*or ;

	obj	= NUM_OBJ_TO_INDICE(obj) ;
	if	( obj < 0 || obj >= XoNbObj )
	{
		XOERR("bad num obj",obj) ;
		return	NULL ;
	}

	or	= XoTabObj[obj] ;
	if	( !or )
	{
		XOERR("undefined obj",obj) ;
		return	NULL ;
	}
	if	( !or->or_curr )
		return	NULL ;

	or->or_curr	= (char *)((t_obj *)or->or_curr)->o_next ;
	return	or->or_curr ;
}


/*
 *	retourne un objet a partir de son numero
 *	ou
 *	retourne l'objet auquel appartient un attribut
 *	en fonction de son numero
 */

/*DEF*/	t_objet_ref	*XoGetRefObj(int32 obj)
{
	obj	= NUM_OBJ_TO_INDICE(obj) ;
	if	( obj >= 0 && obj < XoNbObj )
	{
		if	( !XoTabObj[obj] )
		{
			XOERR("undefined obj",obj) ;
			return	NULL ;
		}
		return	XoTabObj[obj] ;
	}
	XOERR("bad num obj",obj) ;
	return	NULL ;
}

/*
 *	retourne le nom d'un attribut a partir de son numero
 */

/*DEF*/	char	*XoGetRefAttrName(register int32 num)
{
	register	int32	obj ;

	obj	= NUM_OBJ_TO_INDICE(num) ;
	if	( obj < 0 || obj >= XoNbObj )
	{
		XOERR("bad num obj/attr",num) ;
		return	NULL ;
	}
	if	( !XoTabObj[obj] )
	{
		XOERR("undefined obj/attr",num) ;
		return	NULL ;
	}
	num	= NUM_ATTR_TO_INDICE(num) ;
	if	( num >= 0 && num < XoTabObj[obj]->or_nb_attr )
	{
		return	XoTabObj[obj]->or_tab_attr[num].ar_name ;
	}
	XOERR("bad num attr",num) ;
	return(NULL) ;
}


/*
 *	retourne la lg max d'un attribut a partir de son numero
 */

/*DEF*/	int32	XoGetRefAttrLgMax(int32 num)
{
	int32	obj ;

	obj	= NUM_OBJ_TO_INDICE(num) ;
	if	( obj < 0 || obj >= XoNbObj )
	{
		XOERR("bad num obj/attr",num) ;
		return	-100 ;
	}
	if	( !XoTabObj[obj] )
	{
		XOERR("undefined obj/attr",num) ;
		return	-101 ;
	}
	num	= NUM_ATTR_TO_INDICE(num) ;
	if	( num >= 0 && num < XoTabObj[obj]->or_nb_attr )
	{
		return	XoTabObj[obj]->or_tab_attr[num].ar_lg_max ;
	}
	XOERR("bad num attr",num) ;
	return(-102) ;
}

/*
 *	retourne la valeur par defaut d'un attribut a partir de son numero
 */

/*DEF*/	void	*XoGetRefAttrDefault(int32 num)
{
	int32	obj ;

	obj	= NUM_OBJ_TO_INDICE(num) ;
	if	( obj < 0 || obj >= XoNbObj )
	{
		XOERR("bad num obj/attr",num) ;
		return	NULL ;
	}
	if	( !XoTabObj[obj] )
	{
		XOERR("undefined obj/attr",num) ;
		return	NULL ;
	}
	num	= NUM_ATTR_TO_INDICE(num) ;
	if	( num >= 0 && num < XoTabObj[obj]->or_nb_attr )
	{
		return	XoTabObj[obj]->or_tab_attr[num].ar_default ;
	}
	XOERR("bad num attr",num) ;
	return(NULL) ;
}


/*
 *	retourne la liste des valeurs possibles d'un attribut 
 * 	a partir de son numero
 */

/*DEF*/	void	*XoGetRefAttrValue(int32 num)
{
	int32	obj ;

	obj	= NUM_OBJ_TO_INDICE(num) ;
	if	( obj < 0 || obj >= XoNbObj )
	{
		XOERR("bad num obj/attr",num) ;
		return	NULL ;
	}
	if	( !XoTabObj[obj] )
	{
		XOERR("undefined obj/attr",num) ;
		return	NULL ;
	}
	num	= NUM_ATTR_TO_INDICE(num) ;
	if	( num >= 0 && num < XoTabObj[obj]->or_nb_attr )
	{
		return	XoTabObj[obj]->or_tab_attr[num].ar_value ;
	}
	XOERR("bad num attr",num) ;
	return(NULL) ;
}


/*
 *	retourne la ligne d'aide d'un attribut a partir de son numero
 */

/*DEF*/	void	*XoGetRefAttrHelpLine(int32 num)
{
	int32	obj ;

	obj	= NUM_OBJ_TO_INDICE(num) ;
	if	( obj < 0 || obj >= XoNbObj )
	{
		XOERR("bad num obj/attr",num) ;
		return	NULL ;
	}
	if	( !XoTabObj[obj] )
	{
		XOERR("undefined obj/attr",num) ;
		return	NULL ;
	}
	num	= NUM_ATTR_TO_INDICE(num) ;
	if	( num >= 0 && num < XoTabObj[obj]->or_nb_attr )
	{
		return	XoTabObj[obj]->or_tab_attr[num].ar_help_line ;
	}
	XOERR("bad num attr",num) ;
	return(NULL) ;
}


/*
 *	retourne le fichier d'aide d'un attribut a partir de son numero
 */

/*DEF*/	void	*XoGetRefAttrHelpFile(int32 num)
{
	int32	obj ;

	obj	= NUM_OBJ_TO_INDICE(num) ;
	if	( obj < 0 || obj >= XoNbObj )
	{
		XOERR("bad num obj/attr",num) ;
		return	NULL ;
	}
	if	( !XoTabObj[obj] )
	{
		XOERR("undefined obj/attr",num) ;
		return	NULL ;
	}
	num	= NUM_ATTR_TO_INDICE(num) ;
	if	( num >= 0 && num < XoTabObj[obj]->or_nb_attr )
	{
		return	XoTabObj[obj]->or_tab_attr[num].ar_help_file ;
	}
	XOERR("bad num attr",num) ;
	return(NULL) ;
}


/*
 *	retourne l'attribut a partir de son numero
 */

/*DEF*/	t_attribut_ref	*XoGetRefAttr(int32 num)
{
	int32	obj ;

	obj	= NUM_OBJ_TO_INDICE(num) ;
	if	( obj < 0 || obj >= XoNbObj )
	{
		XOERR("bad num obj/attr",num) ;
		return	NULL ;
	}
	if	( !XoTabObj[obj] )
	{
		XOERR("undefined obj/attr",num) ;
		return	NULL ;
	}
	num	= NUM_ATTR_TO_INDICE(num) ;
	if	( num >= 0 && num < XoTabObj[obj]->or_nb_attr )
	{
		return	&(XoTabObj[obj]->or_tab_attr[num]) ;
	}
	XOERR("bad num attr",num) ;
	return(NULL) ;
}

/*
 *	retourne le type d'un attribut
 */

/*DEF*/	int32	XoTypeAttr(int32 num)
{
	int32		obj ;
	t_attribut_ref	*a_ref ;

	obj	= NUM_OBJ_TO_INDICE(num) ;
	if	( obj < 0 || obj >= XoNbObj )
	{
		XOERR("bad num obj/attr",num) ;
		return	-1 ;
	}
	if	( !XoTabObj[obj] )
	{
		XOERR("undefined obj/attr",num) ;
		return	-2 ;
	}
	num	= NUM_ATTR_TO_INDICE(num) ;
	if	( num >= 0 && num < XoTabObj[obj]->or_nb_attr )
	{
		a_ref	= &(XoTabObj[obj]->or_tab_attr[num]) ;
		return	a_ref->ar_type ;
	}
	XOERR("bad num attr",num) ;
	return	-3 ;
}

/*
 *	retourne un numero d'objet a partir de son nom
 */

/*DEF*/	int32	XoNumObj(char *name)
{
	int32	ret ;

	ret	= XoGetNumType(name) ;
	if	( ret < 0 )
	{
		char	tmp[128] ;

		sprintf(tmp,"bad name obj [%s]\n",name) ;
		XOERR(tmp,ret) ;
	}
	return	ret ;
}

/*DEF*/	int32	XoNumObjSilent(char *name)
{
	int32	ret ;

	ret	= XoGetNumType(name) ;
	return	ret ;
}

static	int32	_xonumattr(t_objet_ref *or,char *name)
{
	t_attribut_ref	*ar ;
	int32		nb ;
	int32		i ;
	int32		lg ;

	lg	= strlen(name) ;
	nb	= or->or_nb_attr ;
	if	( 1 && nb > 2 && or->or_stab_attr != NULL)	// TODO ???
	{	// les attributs des objets predefinis ne sont pas tries !!!
		ar	= mybsearchattr(or,name,lg);
		if	( !ar )
			return	-4444;
//printf("or_name '%s' contains ar_name '%s'\n",or->or_name,ar->ar_name);
		return	ar->ar_num ;
	}
	else
	{
		ar	= &(or->or_tab_attr[0]) ;
		for	( i = 0 ; i < nb ; ar++ , i++ )
		{
/*
			if	( strcmp(name,ar->ar_name) == 0 )
*/
			if	( lg==ar->ar_lg_name 
			&&	*name == *(ar->ar_name)
			&&	!memcmp(name,ar->ar_name,(size_t)(lg+1)) )
			{
				return	ar->ar_num ;
			}
		}
	}
	return	-444;
}

/*
 *	retourne un numero d'attribut d'un numero d'obj a partir de son nom
 */

/*DEF*/	int32	XoNumAttr(int32 numobj, char *name)
{
	t_objet_ref	*or ;
#if	0
	t_attribut_ref	*ar ;
	int32		nb ;
	int32		i ;
#endif
	int32		lg ;

	if	( !name )
	{
		XOERR("null name attr",0) ;
		return	-1 ;
	}

	lg	= strlen(name) ;
	if	( !lg )
	{
		XOERR("null name attr",0) ;
		return	-2 ;
	}

	or	= XoGetRefObj(numobj) ;
	if	( !or )
	{
		XOERR("bad num obj",numobj) ;
		return	-3 ;
	}
	return	_xonumattr(or,name);
#if	0
	nb	= or->or_nb_attr ;
	ar	= &(or->or_tab_attr[0]) ;
	for	( i = 0 ; i < nb ; ar++ , i++ )
	{
/*
		if	( strcmp(name,ar->ar_name) == 0 )
*/
		if	( lg==ar->ar_lg_name 
		&&	*name == *(ar->ar_name)
		&&	!memcmp(name,ar->ar_name,(size_t)(lg+1)) )
		{
			return	ar->ar_num ;
		}
	}

	return	-4 ;
#endif
}

/*
 *	retourne un numero d'attribut d'un obj a partir de son nom
 */

/*DEF*/	int32	XoNumAttrInObj(void *obj, char *name)
{
	register	t_obj		*o ;
	register	t_objet_ref	*or ;
#if	0
	register	t_attribut_ref	*ar ;
	int32		nb ;
	int32		i ;
#endif
	int32		lg ;

	if	( !name )
	{
		XOERR("null name attr",0) ;
		return	-1 ;
	}
	lg	= strlen(name) ;
	if	( !lg )
	{
		XOERR("null name attr",0) ;
		return	-2 ;
	}

	o	= (t_obj *)obj ;
	if	( !o )
	{
		XOERR("null",0) ;
		return	-3 ;
	}
	if	( o->o_magic != O_MAGIC )
	{
		XOERR("magic",0) ;
		return	-4 ;
	}
	or	= o->o_or ;
	if	( or->or_num != o->o_num )
	{
		XOERR("ref failure",0) ;
		return	-5 ;
	}

	return	_xonumattr(or,name);
#if	0
	nb	= or->or_nb_attr ;
	ar	= &(or->or_tab_attr[0]) ;
	for	( i = 0 ; i < nb ; ar++ , i++ )
	{
/*
		if	( strcmp(name,ar->ar_name) == 0 )
*/
		if	( lg==ar->ar_lg_name 
		&&	*name == *(ar->ar_name)
		&&	!memcmp(name,ar->ar_name,(size_t)(lg+1)) )
		{
			return	ar->ar_num ;
		}
	}

	return	-6 ;
#endif
}

/*
 *	cet attribut est-il de type :
 *		BASIC_ATTR
 *		LIST_BASIC_ATTR
 *		OBJ_ATTR
 *		LIST_OBJ_ATTR
 */

/*DEF*/	int32	XoModeAttr(int32 num)
{
	int32		obj ;
	register	t_objet_ref	*or ;

	obj	= NUM_OBJ_TO_INDICE(num) ;
	if	( obj < 0 || obj >= XoNbObj )
	{
		XOERR("bad num obj/attr",num) ;
		return	-1 ;
	}
	or	= XoTabObj[obj] ;
	if	( !or )
	{
		XOERR("undefined obj/attr",num) ;
		return	-2 ;
	}
	num	= NUM_ATTR_TO_INDICE(num) ;
	if	( num < or->or_nb_attr )
	{
		return	or->or_tab_attr[num].ar_mode ;
	}
	XOERR("bad num attr",num) ;
	return	-3 ;
}

static	int32	xomodeattr(t_attribut_ref *a_ref)
{
	int32		ret ;

	switch(a_ref->ar_type)
	{
		case	XoDate :
		case	XoInt4B :
		case	XoChar :
		case	XoBool :
#if	0
		case	XoVoid :
#endif
		case	XoString :
		case	XoBuffer :
			if	( IS_A_LIST(a_ref) )
				ret	= LIST_BASIC_ATTR ;
			else
				ret	= BASIC_ATTR ;
		break ;
		default :
			if	( IS_A_LIST(a_ref) )
				ret	= LIST_OBJ_ATTR ;
			else
				ret	= OBJ_ATTR ;
		break ;
	}
	return	ret ;
}


/*
 *	pour on objet de reference :
 *	1) calcule la taille des differents elements dont la longueur
 *	ne changera pas cad :
 *		la table des attributs
 *		la table des attributs basic
 *		la longueur totale des XoString fixes est deja calculee
 *	toutes ces tailles sont forcees a une taille % 4 pour permettre
 *	des cast
 *	2) calcule les deplacements
 *	3) calcule la longueur du nom
 */

static	int32	calcobjref(t_objet_ref *o)
{
	o->or_lg_name	= strlen(o->or_name) ;
	SzTabattr	= o->or_nb_attr * sizeof(u_attr) ;
	SzTabbasic	= o->or_nb_basic * sizeof(t_basic_attr) ;
	
	SzTabattr	= do_modulo4(SzTabattr) ;
	SzTabbasic	= do_modulo4(SzTabbasic) ;
	o->or_sz_str	= do_modulo4(o->or_sz_str) ;
	SzObj		= do_modulo4(sizeof(t_obj)) ;

#if	0
	o->or_dep_attr	= SzObj ;
	o->or_dep_basic	= SzObj + SzTabattr ;
	o->or_dep_str	= SzObj + SzTabattr + SzTabbasic ;

	o->or_sz_tot	= SzObj + SzTabattr + SzTabbasic + o->or_sz_str;
#endif
	o->or_dep_attr	= 0 ;
	o->or_dep_basic	= SzTabattr ;
	o->or_dep_str	= SzTabattr + SzTabbasic ;

	o->or_sz_tot	= SzTabattr + SzTabbasic + o->or_sz_str;
	return	o->or_sz_tot ;
}


/*
 *	pour un objet reference parcourt la liste de ses attributs
 *	compte les differents types d'attributs
 *		or_nb_basic
 *		or_nb_objet
 *		or_nb_list_basic
 *		or_nb_list_objet
 *	compte la taille maximale que peuvent occuper les chaines a taille
 *	fixe
 *		or_sz_str
 *	pour chaque attribut calcule son mode 
 *		ar_mode
 *	longueur du nom de l'attribut
 */

static	void lattr(register t_objet_ref *o, int32 trace)
{
	register	t_attribut_ref	*a ;
	int32		nb ;
	int32		nbbasic	= 0 ;
	int32		nbobjet	= 0 ;
	int32		nblistbasic	= 0 ;
	int32		nblistobjet	= 0 ;
	int32		szstr		= 0 ;
	char		*txtmode ;
	int32		mode ;
	int32		dropwraphidden	= 0;

	o->or_wraphidden	= 0;
	a 	= &(o->or_tab_attr[0]) ; 
	nb	= o->or_nb_attr ;
	for	( ; a < &(o->or_tab_attr[nb]) ; a++ )
	{
		a->ar_lg_name	= strlen(a->ar_name) ;
		mode		= xomodeattr(a) ;
		a->ar_mode	= mode ;
		switch(mode)
		{
	 		case	BASIC_ATTR :
				if	( !IS_EMBATTR(a) &&
						strchr(a->ar_name,'$') == NULL)
					dropwraphidden	= 1;
				if	( AR_PRE_ALLOC(a) )
				{
					a->ar_flag	|= AR_FPREALLOC ;
					a->ar_dep_str	= szstr ;
					szstr		+= (a->ar_lg_max+1) ;
				}
				else
				{
					a->ar_flag	&= ~AR_FPREALLOC ;
					a->ar_dep_str	= -1 ;
				}
				if	( AR_NICE_STRING(a) )
				{
					a->ar_flag	|= AR_FNICE_STR ;
				}
				else
				{
					a->ar_flag	&= ~AR_FNICE_STR ;
				}
				txtmode	= "BA" ;
				a->ar_indice_basic	= nbbasic++ ;
			break ;
			case	LIST_BASIC_ATTR :
				dropwraphidden	= 1;
				txtmode	= "LB" ;
				nblistbasic++ ;
			break ;
			case	OBJ_ATTR :
				dropwraphidden	= 1;
				txtmode	= "OB" ;
				nbobjet++ ;
			break ;
			case	LIST_OBJ_ATTR :
				if	( IS_WRAPHIDDEN(a) )
				{
//printf("object '%s' with one wraphidden attribut %s/%d\n",
//		o->or_name,a->ar_name,a->ar_num);
					o->or_wraphidden	= a->ar_num;
				}
				else
					dropwraphidden	= 1;
				txtmode	= "LO" ;
				nblistobjet++ ;
			break ;
			default :
				txtmode	= "";
			break ;
		}
		a->ar_mode_sv	= xomodesv(a) ;
		if	( trace )
		{
#ifndef		DOS
			fprintf(stdout,"\t%-20.20s",
					XoGetRefAttrName(a->ar_num)) ;
			fprintf(stdout,"\t%-10.10s",
					XoGetRefObjName(a->ar_type)) ;
			fprintf(stdout,"\t%c",(IS_A_LIST(a)?'L':' ')) ;
			fprintf(stdout,"%c",(IS_NO_SAVE(a)?'s':' ')) ;
			fprintf(stdout,"%c",(IS_SAVE_UNSET(a)?'u':' ')) ;
			fprintf(stdout,"%c",(a->ar_value?'V':' ')) ;
			fprintf(stdout,"%c",(IS_PREALLOC(a)?'P':' ')) ;
			fprintf(stdout,"%c",(IS_EMBATTR(a)?'E':' ')) ;
			fprintf(stdout,"%c",(IS_WRAPHIDDEN(a)?'W':' ')) ;
			fprintf(stdout,"%c",(IS_SHDICO(a)?'S':' ')) ;
			fprintf(stdout,"%c",(a->ar_default?'D':' ')) ;
			fprintf(stdout,"%ld ",(long)a->ar_lg_max) ;
			fprintf(stdout,"%lx ",(long)a->ar_num) ;
			fprintf(stdout,"%s ",txtmode) ;
			fprintf(stdout,"\n") ;
#endif
		}
	}
	if	(dropwraphidden)
		o->or_wraphidden	= 0;

	if	(o->or_wraphidden)
	{
//printf("object '%s' with only one wraphidden attribut %d\n",o->or_name,o->or_wraphidden);
	}

	o->or_nb_basic		= nbbasic ;
	o->or_nb_objet		= nbobjet ;
	o->or_nb_list_basic	= nblistbasic ;
	o->or_nb_list_objet	= nblistobjet ;

	o->or_sz_str		= szstr ;
}



/*
 *	tri initial des objets de reference
 */

static	void initsortobjref(void)
{
	sortobjref() ;
}

#if	0
static	void dumpsortobj(void)
{
	int32	i ;
	t_objet_ref	*or ;

	for	( i = 0 ; i < XoNbObjSort ; i++ )
	{
		or	= XoTabObjSort[i].os_or ;
		fprintf(stderr,"%ld [%s] %ld\n",(long)i,or->or_name,(long)or->or_lg_name) ;
	}
}
#endif

/*
 *	reorganisation du tri 
 *	ici on ne sait pas si le nbre de classe a augmente ou diminue
 *	=> on recommence a zero
 */

static	int32	reorgsortobjref(void)
{
	int32	i ;
	int32	nb ;

	nb	= 0 ;
	for	( i = 0 ; i < XoNbObj ; i++ )
	{
		if	( XoTabObj[i] )
			nb++ ;
	}
	if	( !nb )
	{
		XOERR("???",0) ;
		return	-1 ;
	}

	XoNbObjSort	= nb ;
	if	( XoTabObjSort )
		free(XoTabObjSort) ;

	XoTabObjSort= (t_obj_sort *)
			malloc((size_t)(XoNbObjSort * sizeof(t_obj_sort))) ;
	if	( !XoTabObjSort )
	{
		XOERR("malloc()",errno) ;
		return	-2 ;
	}
	for	( i = 0 , nb = 0 ; i < XoNbObj ; i++ )
	{
		if	( XoTabObj[i] )
		{
			XoTabObjSort[nb++].os_or	= XoTabObj[i] ;
		}
	}

	sortobjref() ;

	return	nb ;
}

/*
 *	tri des objets de reference apres un ajout
 */

static	int32	moresortobjref(t_objet_ref *or)
{
	t_obj_sort	*tab ;

	tab	= (t_obj_sort *)
	realloc(XoTabObjSort,(size_t)((XoNbObjSort+1)*sizeof(t_obj_sort))) ;
	if	( !tab )
	{
		XOERR("cannot realloc XoTabObjSort",XoNbObjSort+1) ;
		return	-1 ;
	}
	tab[XoNbObjSort].os_or	= or ;
	XoTabObjSort		= tab ;
	XoNbObjSort++ ;
	sortobjref() ;
	return	XoNbObjSort ;
}

/*
 *	recherche dicho dans les objets tries
 */
static	t_obj_sort	*mybsearchobj(char *name, int32 lg)
{
	int32	l , h , m ;
	int32	ret ;
	t_obj_sort	*os ;

	l	= 0 ;
	h	= XoNbObjSort - 1 ;
	while	( l <= h )
	{
		m	= ( l + h ) / 2 ;
		os	= &XoTabObjSort[m] ;
		ret	= memcmp(name,os->os_or->or_name,(size_t)(lg+1)) ;
/*
fprintf(stderr,"[%s] =?= [%s]\n",name,os->os_or->or_name) ;
*/
		if	( ret < 0 )
			h	= m - 1 ;
		else
			if	( ret > 0 )
				l	= m + 1 ;
			else
				return	os ;
	}
	return	((t_obj_sort *)NULL) ;
}

/*
 *	recherche dicho dans les attributs tries
 */
static	t_attribut_ref	*mybsearchattr(t_objet_ref *or,char *name, int32 lg)
{
	int32	l , h , m ;
	int32	ret ;
	t_attribut_ref	*ar;

	l	= 0 ;
	h	= or->or_nb_attr - 1 ;
	while	( l <= h )
	{
		m	= ( l + h ) / 2 ;
		ar	= or->or_stab_attr[m] ;
		ret	= memcmp(name,ar->ar_name,(size_t)(lg+1)) ;
/*
fprintf(stderr,"[%s] =?= [%s]\n",name,ar->ar_name) ;
*/
		if	( ret < 0 )
			h	= m - 1 ;
		else
			if	( ret > 0 )
				l	= m + 1 ;
			else
				return	ar ;
	}
	return	((t_attribut_ref *)NULL) ;
}

/* 	
 *	force un lg a une lg % 4
 */

int32	do_modulo4(uint32 lg)
{
	return	(((lg+3)>>2)<<2) ;
}


/*DEF*/	int32	__XoInit(int32 trace)
{
	int32		i ;
	t_objet_ref	*o ;

	if	( !Joint32Done )
	{
		if	( xojoint32() < 0 )
		{
			XOERR("",0) ;
			return	-1 ;
		}
		Joint32Done	= 1 ;
	}

	for	( i = 0 ; i < XoNbObj ; i++ )
	{
		o	= XoTabObj[i] ;
		if	( !o )			continue ;
		if	( trace )
		{
#ifndef		DOS
			fprintf(stdout,"%-20.20s ",o->or_name) ;
			fprintf(stdout,"\t%lx",(long)o->or_num) ;
			if	( o->or_root )
				fprintf(stdout," root") ;
			if	( o->or_from )
				fprintf(stdout," from (%s)",o->or_from) ;
			if	( o->or_alias )
				fprintf(stdout," of (%s)",o->or_alias) ;
			if	( o->or_stext > 0 )
				fprintf(stdout," stext") ;
			fprintf(stdout,"\n") ;
#endif
		}
		lattr(o,trace) ;
		calcobjref(o) ;
		if	( trace )
		{
#if		0
			fprintf(stdout,"\t%ld=",(long)o->or_nb_attr) ;
			fprintf(stdout,"%ld+",(long)o->or_nb_basic) ;
			fprintf(stdout,"%ld+",(long)o->or_nb_objet) ;
			fprintf(stdout,"%ld+",(long)o->or_nb_list_basic) ;
			fprintf(stdout,"%ld",(long)o->or_nb_list_objet) ;
			fprintf(stdout,"\n") ;

			fprintf(stdout,"\t%ld=",(long)o->or_sz_tot) ;
			fprintf(stdout,"%ld(0)+",(long)SzObj) ;
			fprintf(stdout,"%ld(%ld)+",(long)SzTabattr,
					(long)o->or_dep_attr) ;
			fprintf(stdout,"%ld(%ld)+",(long)SzTabbasic,
					(long)o->or_dep_basic) ;
			fprintf(stdout,"%ld(%ld)",(long)o->or_sz_str,
					(long)o->or_dep_str) ;
			fprintf(stdout,"\n") ;
#endif
		}
	}
	initsortobjref() ;
	return	XoNbObj ;
}

int32	XolittleBig	= 0;
static	void	FindLittleBig()
{
	if(XolittleBig == 0)
	{
		char buf[4];
		char *pt = &buf[0];

		buf[0] = 1;
		buf[1] = 0;
		buf[2] = 0;
		buf[3] = 0;

		if(*(int32 *) pt == (int32)1)
		{
			XolittleBig = 1;
		}
		else
		{
			XolittleBig = 2;
		}
	}
}

/*DEF*/	int32	XoInit(int32 trace)
{
	int32	ret;

	FindLittleBig();
	XoInitCS();
	ret = __XoInit(trace);

	return	ret;
}


/*
 *	retourne le type d'un objet
 */

/*DEF*/	int32	XoType(void *obj)
{
	t_obj		*o ;
	t_objet_ref	*or ;

	o	= (t_obj *)obj ;

	if	( !o )
	{
		XOERR("null",0) ;
		return	-1 ;
	}

	if	( o->o_magic != O_MAGIC )
	{
		XOERR("magic",0) ;
		return	-2 ;
	}

	or	= o->o_or ;
	if	( or->or_num != o->o_num )
	{
		XOERR("ref failure",0) ;
		return	-3 ;
	}

	return	or->or_num ;
}

/*
 *	trouve une place libre pour un objet de reference utilisateur
 */

static	int32	get_free_indice_obj(void)
{
	int32	i ;

	for	( i = XoNbObjInt ; i < XoNbObj ; i++ )
	{
		if	( !XoTabObj[i] )
			return	i ;
	}
	return	-1 ;
}


/*
 *
 */

static	int32	valo_obj_ref(t_objet_ref *or, t_obj *o)
{
	void	*value ;
	char	*name ;
	char	*from ;
	char	*alias ;
	int32	lg ;

	name	= XoGetAttr(o,XoObjRefName,&lg) ;
	if	( !name )
		return	-507 ;
	if	( XoGetNumType(name) >= 0 )
		return	-5071 ;

	value	= XoGetAttr(o,XoObjRefNbAttr,&lg) ;	
	if	( !value )
		return	-501 ;
	or->or_nb_attr	= *(int32 *)value ;
	if	( or->or_nb_attr < 0 )
		return	-502 ;

	value	= XoGetAttr(o,XoObjRefType,&lg) ;	
	if	( !value )
		return	-503 ;
	or->or_type	= *(int32 *)value ;
	if	( or->or_type < 0 )
		return	-504 ;
/*
	value	= XoGetAttr(o,XoObjRefNum,&lg) ;
	if	( !value )
		return	-505 ;
*/
	or->or_num	= get_free_indice_obj() ;
	if	( or->or_num < 0 )
	{
		/* *	pas de place libre */
		or->or_num	= (XoNbObj+1) * BaseObj ;
	}
	else
	{
		or->or_num	= (or->or_num+1) * BaseObj ;
	}
/*
fprintf(stderr,"ajout d'une definition d'objet :\n") ;
fprintf(stderr,"nbre d'objets :%ld\n",(long)XoNbObj) ;
fprintf(stderr,"num/nom objet ajoute :%ld '%s'\n",(long)or->or_num,name) ;
fprintf(stderr,"indice objet ajoute :%ld\n",(long)NUM_OBJ_TO_INDICE(or->or_num)) ;
*/
/*
	if	( NUM_OBJ_TO_INDICE(or->or_num) != XoNbObj )
		return	-506 ;
*/
	if	( !IS_NUM_OBJ(or->or_num) )
		return	-506 ;
	if	( or->or_num < BaseUserObj(0) )
		return	-5061 ;

	lg		= strlen(name) ;
	or->or_name	= (char *)malloc((size_t)(lg+1)) ;
	if	( !or->or_name )
		return	-508 ;
	strcpy	(or->or_name,name) ;

	or->or_root	= 0 ;
	value	= XoGetAttr(o,XoObjRefRoot,&lg) ;
	if	( value )
	{
		if	( *(int32 *)value )
			or->or_root	= 1 ;
	}

	or->or_stext	= 0 ;
	value	= XoGetAttr(o,XoObjRefStExt,&lg) ;
	if	( value )
	{
		if	( *(int32 *)value )
			or->or_stext	= 1 ;
	}

	from	= XoGetAttr(o,XoObjRefFrom,&lg) ;
	if	(from && *from)
	{
		lg		= strlen(from) ;
#if	0	// valgrind rale ici
		if	(or->or_from)
		{
printf("or_from is not null ...\n");
		}
		or->or_from	= NULL;
#endif
		or->or_from	= (char *)malloc((size_t)(lg+1)) ;
		if	( !or->or_from )
			return	-510 ;
		strcpy	(or->or_from,from) ;
	}

	alias	= XoGetAttr(o,XoObjRefAlias,&lg) ;
	if	(alias && *alias)
	{
		lg		= strlen(alias) ;
		or->or_alias	= (char *)malloc((size_t)(lg+1)) ;
		if	( !or->or_alias )
			return	-511 ;
		strcpy	(or->or_alias,alias) ;
	}

	alias	= XoGetAttr(o,XoObjRefAliasHas,&lg) ;
	if	(alias && *alias)
	{
		lg		= strlen(alias) ;
		or->or_alias_has= (char *)malloc((size_t)(lg+1)) ;
		if	( !or->or_alias_has )
			return	-512 ;
		strcpy	(or->or_alias_has,alias) ;
	}

	return	or->or_nb_attr ;
}


static	int32	valo_attr_ref(int32 ornum, t_attribut_ref *tab, t_obj *o, int32 nbint32ab)
{
	int32	nb ;
	int32	pos ;
	int32	lg ;
	void	*value ;
	t_attribut_ref	*ar ;
	char	*a ;

	nb	= 0 ;
	pos	= XoFirstAttr	(o,XoObjRefListAttr) ;
	while	( pos > 0 )
	{
		ar	= &tab[nb] ;
		a	= XoGetAttr	(o,XoObjRefListAttr,&lg) ;
		if	( !a )
			return	-600 ;
		nb++ ;
		if	( nb > nbint32ab )
		{
			return	-599 ;
		}
/*
		value	= XoGetAttr	(a,XoAttrRefNum,&lg) ;
		if	( !value )
			return	-601 ;
*/
		ar->ar_num	= ornum + nb ;
		if	( ar->ar_num < 0 )
			return	-602 ;

		value	= XoGetAttr	(a,XoAttrRefTypeName,&lg) ;
		if	( !value )
			return	-603 ;
		ar->ar_type	= XoGetNumType(value) ;
		if	( ar->ar_type < 0 )
		{
			XOERR("",0) ;
			fprintf(stderr,"type '%s' ???\n",(char *)value) ;
			fflush(stderr) ;
			return	-604 ;
		}

		value	= XoGetAttr	(a,XoAttrRefLgMax,&lg) ;
		if	( !value )
			return	-605 ;
		ar->ar_lg_max	= *(int32 *)value ;

		value	= XoGetAttr	(a,XoAttrRefList,&lg) ;
		if	( !value )
			return	-607 ;
		if	( *(int32 *)value )
			ar->ar_flag	|= AR_FLIST ;
		else
			ar->ar_flag	&= ~AR_FLIST ;

		ar->ar_flag	&= ~AR_FNO_SAVE ;
		value	= XoGetAttr	(a,XoAttrRefNoSave,&lg) ;
		if	( value && *(int32 *)value )
			ar->ar_flag	|= AR_FNO_SAVE ;

#if	0
		if	( ar->ar_type == XoVoid )
		{
			ar->ar_flag	|= AR_FNO_SAVE ;
		}
#endif

		ar->ar_flag	&= ~AR_FSAVE_UNSET ;
		value	= XoGetAttr	(a,XoAttrRefSaveUnset,&lg) ;
		if	( value && *(int32 *)value )
			ar->ar_flag	|= AR_FSAVE_UNSET ;

		ar->ar_flag	&= ~AR_FEMBATTR ;
		value	= XoGetAttr	(a,XoAttrRefEmbAttr,&lg) ;
		if	( value && *(int32 *)value )
			ar->ar_flag	|= AR_FEMBATTR ;

		ar->ar_flag	&= ~AR_FWRAPHIDDEN ;
		value	= XoGetAttr	(a,XoAttrRefWrapHidden,&lg) ;
		if	( value && *(int32 *)value )
			ar->ar_flag	|= AR_FWRAPHIDDEN ;

		ar->ar_flag	&= ~AR_FSHDICO ;
		value	= XoGetAttr	(a,XoAttrRefShDico,&lg) ;
		if	( value && *(int32 *)value )
			ar->ar_flag	|= AR_FSHDICO ;

		value	= XoGetAttr	(a,XoAttrRefName,&lg) ;
		if	( !value )
			return	-609 ;
		ar->ar_name	= (char *)malloc((size_t)(lg+1)) ;
		if	( !ar->ar_name )
			return	-610 ;
		strcpy	(ar->ar_name,value) ;
#if	0
if	(strncmp(ar->ar_name,"xmlns:",6) == 0)
printf	("xxxx arname='%s' flag=%d\n",ar->ar_name,ar->ar_flag);
#endif

		value	= XoGetAttr	(a,XoAttrRefTypeName,&lg) ;
		if	( !value )
			return	-609 ;
		ar->ar_type_name	= (char *)malloc((size_t)(lg+1)) ;
		if	( !ar->ar_type_name )
			return	-6101 ;
		strcpy	(ar->ar_type_name,value) ;
#if	0
		if	( ar->ar_type == XoBool )
			ar->ar_value	= BoolArValue ;
		else
#endif
		{
			value	= XoGetAttr	(a,XoAttrRefValue,&lg) ;
			if	( value )
			{
				ar->ar_value =(char *)malloc((size_t)(lg+1)) ;
				if	( !ar->ar_value )
					return	-611 ;
				strcpy	(ar->ar_value,value) ;
			}
		}

		value	= XoGetAttr	(a,XoAttrRefDefault,&lg) ;
		if	( value )
		{
			ar->ar_default	= (char *)malloc((size_t)(lg+1)) ;
			if	( !ar->ar_default )
				return	-612 ;
			strcpy	(ar->ar_default,value) ;
		}

		value	= XoGetAttr	(a,XoAttrRefHelpLine,&lg) ;
		if	( value )
		{
			ar->ar_help_line = (char *)malloc((size_t)(lg+1)) ;
			if	( !ar->ar_help_line )
				return	-613 ;
			strcpy	(ar->ar_help_line,value) ;
		}

		value	= XoGetAttr	(a,XoAttrRefHelpFile,&lg) ;
		if	( value )
		{
			ar->ar_help_file = (char *)malloc((size_t)(lg+1)) ;
			if	( !ar->ar_help_file )
				return	-614 ;
			strcpy	(ar->ar_help_file,value) ;
		}

/*
printf("valo_attr_ref() %s\n",ar->ar_name) ;
fflush(stdout) ;
*/
		pos	= XoNextAttr	(o,XoObjRefListAttr) ;
	}

	return	nb ;

}


/*
 *	ajoute l'objet dans la table des objets de reference
 */

/*DEF*/	int32	__XoAddObjRef(void *obj)
{
	int32		i ;
	int32		newindice ;
	int32		objenplus ;
	t_obj		*o ;		/* def. de l'objet a ajoute	*/

	t_objet_ref	*oradd ;	/* objet ajoute			*/
	t_attribut_ref	*aradd ;	/* attributs dans oradd 	*/

	int32		ret ;
	int32		nbaradd ;
	char		*name ;
	int32		lg ;

	t_objet_ref	**tabor ;

	o	= (t_obj *)obj ;

	if	( !o )
	{
		XOERR("null",0) ;
		return	-1 ;
	}

	if	( o->o_magic != O_MAGIC )
	{
		XOERR("magic",0) ;
		return	-2 ;
	}

	if	( o->o_num != XoObjRef )
	{
		XOERR("type obj err",0) ;
		return	-4 ;
	}

	name	= XoGetAttr(o,XoObjRefName,&lg) ;
	if	( !name )
	{
		XOERR("no name",0) ;
		return	-41 ;
	}
	ret	= XoGetNumType(name) ;
	if	( ret >= 0 )
	{
		if	( !AcceptDuplObjRef )
		{
			char	tmp[256] ;

			sprintf(tmp,"error type redefinition '%s'",name) ;
			XOERR(tmp,ret) ;
			return	-42 ;
		}
		else
		{
#if	0
			char	tmp[256] ;

			sprintf(tmp,"warning type redefinition '%s'",name) ;
			XOERR(tmp,ret) ;
#endif
			return	0 ;
		}
	}

	oradd	= (t_objet_ref *)malloc(sizeof(t_objet_ref)) ;
	if	( !oradd )
	{
		XOERR("malloc()",errno) ;
		return	-5 ;
	}
	memset	(oradd,0,sizeof(t_objet_ref)) ;

	nbaradd	= valo_obj_ref	(oradd,o) ;
	if	( nbaradd < 0 )
	{
		XOERR("err",nbaradd) ;
		free	(oradd) ;
		return	nbaradd ;
	}

/*
printf("valo_obj_ref() ok %ld\n",(long)nbaradd) ;
*/

	if	( nbaradd )
	{
		aradd	= (t_attribut_ref *)
			malloc((size_t)(nbaradd * sizeof(t_attribut_ref)));
		if	( !aradd )
		{
			XOERR("malloc()",errno) ;
			free	(oradd) ;
			return	-6 ;
		}
		memset	( aradd , 0 , (size_t)(nbaradd * sizeof(t_attribut_ref)));

		ret	= valo_attr_ref	(oradd->or_num,aradd,o,nbaradd) ;
		if	( ret != nbaradd )
		{
			XOERR("nb attr != #(list attr)",ret) ;
			free	(oradd) ;
			free	(aradd) ;
			return	-7 ;
		}
		oradd->or_tab_attr	= aradd ;

		oradd->or_stab_attr = (t_attribut_ref **)
			malloc((size_t)(nbaradd * sizeof(t_attribut_ref *)));
		if	( !oradd->or_stab_attr )
		{
			XOERR("malloc()",errno) ;
			free	(oradd) ;
			free	(aradd) ;
			return	-66 ;
		}
		for	(i = 0 ; i < nbaradd ; i++)
		{
			oradd->or_stab_attr[i]	= &aradd[i];
		}
/*
printf("valo_attr_ref() ok\n") ;
*/
	}
	else
	{	/* pas d'attributs dans cet objet */
		oradd->or_tab_attr	= NULL ;
		oradd->or_stab_attr	= NULL ;
		aradd			= NULL ;
	}


	newindice	= NUM_OBJ_TO_INDICE(oradd->or_num) ;
	if	( newindice > XoNbObj-1 )
	{
		objenplus	= newindice - (XoNbObj-1) ;
		tabor	= (t_objet_ref **)realloc(XoTabObj,
			(size_t)((XoNbObj+objenplus) *sizeof(t_objet_ref *))) ;
		if	( !tabor )
		{
			XOERR("realloc() XoTabObj",errno) ;
			free	(oradd) ;
			if	( aradd )
				free	(aradd) ;
			if	( oradd->or_stab_attr )
				free	( oradd->or_stab_attr );
			return	-8 ;
		}
		for	( i = XoNbObj ; i <= newindice ; i++ )
			tabor[i]	= NULL ;
		XoTabObj		= tabor ;
		XoTabObj[newindice]	= oradd ;
		XoNbObj 		= newindice + 1 ;
	}
	else
	{	/*	deja realloue	*/
		objenplus	= 0 ;
		if	( XoTabObj[newindice] )
		{
			XOERR("redifined %ld",(long)oradd->or_num) ;
			free	(oradd) ;
			if	( aradd )
				free	(aradd) ;
			if	( oradd->or_stab_attr )
				free	( oradd->or_stab_attr );
			return	-9 ;
		}
		XoTabObj[newindice]	= oradd ;
	}
/*
fprintf(stderr,"apres ajout obj : indice = %ld nbobj = %ld enplus = %ld\n",
(long)newindice,(long)XoNbObj,(long)objenplus) ;
*/
	lattr	(oradd,(int32)0) ;
	calcobjref(oradd) ;
	if	( objenplus > 0 )
	{
		if	( moresortobjref(oradd) < 0 )
		{
			XOERR("moresortobjref error",0) ;
			return	-10 ;
		}
	}
	else
	{
		reorgsortobjref() ;
	}

	sortattrref(oradd);
#if	0
	printf	("or_name='%s'\n",oradd->or_name);
	for	(i = 0 ; i < nbaradd ; i++)
	{
		printf	("\tar_name='%s'\n",
			oradd->or_stab_attr[i]->ar_name);
	}
#endif

	if 	(_xonumattr(oradd,XOML_WRAPPE_ATTR) > 0)
	{
//printf("obj '%s' is obix\n",oradd->or_name);
		oradd->or_wrapped	= 1;
	}

	if 	(_xonumattr(oradd,XOML_CUSTOM_ATTR) > 0)
	{
//printf("obj '%s' with custom attributs\n",oradd->or_name);
		oradd->or_custom	= 1;
	}

	return	XoNbObj ;
}

/*DEF*/	int32	XoAddObjRef(void *obj)
{
	int32	ret;

	ret = __XoAddObjRef(obj);
	return	ret;
}

/*
 *	suppression d'un objet de reference
 */

/*DEF*/	int32	XoDelObjRef(int32 numobj)
{
	t_objet_ref	*or ;
	int32		indice ;
	int32		ret ;

	or	= XoGetRefObj(numobj) ;
	if	( !or )
	{
		XOERR("bad num obj",numobj) ;
		return	-1 ;
	}

	if	( numobj < BaseUserObj(0) )
	{
		XOERR("predefined class",numobj) ;
		return	-2 ;
	}


	if	( or->or_nb_obj > 0 )
	{
		XOERR("objects occur != 0",numobj) ;
		return	-3 ;
	}

	indice	= NUM_OBJ_TO_INDICE(numobj) ;

	or	= XoTabObj[indice] ;
	if	( !or || or->or_num != numobj )
	{
		XOERR("???",numobj) ;
		return	-4 ;
	}
/*
fprintf(stderr,"suppression de la classe '%s' %ld\n",or->or_name,(long)indice) ;
fflush(stderr) ;
*/
	free_or	(or) ;
	free	(or) ;

	XoTabObj[indice]	= NULL ;

	ret	= reorgsortobjref() ;
	if	( ret < 0 )
	{
		XOERR("???",ret) ;
		return	-5 ;
	}

	return	ret ;
}




/*DEF*/	void	*XoNewObjRef(int32 numobj)
{

	char		*retobj ;
	char		*retattr ;
	t_objet_ref	*or ;
	t_attribut_ref	*ar ;
	int32		ret ;
	int32		a ;


	retobj	= XoNew(XoObjRef) ;
	if	( !retobj )
	{
		XOERR("",0) ;
		return	NULL ;
	}

	or	= XoGetRefObj(numobj) ;
	if	( !or )
	{
		XOERR("",0) ;
		return	NULL ;
	}

	ret	= XoSetAttr(retobj,XoObjRefNbAttr,(void *)or->or_nb_attr,0) ;
	if	( ret < 0 )
	{
		XOERR("",ret) ;
		return	NULL ;
	}
	ret	= XoSetAttr(retobj,XoObjRefType,(void *)or->or_type,0) ;
	if	( ret < 0 )
	{
		XOERR("",ret) ;
		return	NULL ;
	}
	ret	= XoSetAttr(retobj,XoObjRefNum,(void *)or->or_num,0) ;
	if	( ret < 0 )
	{
		XOERR("",ret) ;
		return	NULL ;
	}
	ret	= XoSetAttr(retobj,XoObjRefName,or->or_name,0) ;
	if	( ret < 0 )
	{
		XOERR("",ret) ;
		return	NULL ;
	}
	ret	= XoSetAttr(retobj,XoObjRefRoot,(void *)or->or_root,0) ;
	if	( ret < 0 )
	{
		XOERR("",ret) ;
		return	NULL ;
	}
	ret	= XoSetAttr(retobj,XoObjRefStExt,(void *)or->or_stext,0) ;
	if	( ret < 0 )
	{
		XOERR("",ret) ;
		return	NULL ;
	}
	if	(or->or_from && *or->or_from)
	{
		ret	= XoSetAttr(retobj,XoObjRefFrom,(void *)or->or_from,0) ;
		if	( ret < 0 )
		{
			XOERR("",ret) ;
			return	NULL ;
		}
	}
	if	(or->or_alias && *or->or_alias)
	{
		ret = XoSetAttr(retobj,XoObjRefAlias,(void *)or->or_alias,0) ;
		if	( ret < 0 )
		{
			XOERR("",ret) ;
			return	NULL ;
		}
	}
	if	(or->or_alias_has && *or->or_alias_has)
	{
		ret = XoSetAttr(retobj,XoObjRefAliasHas,(void *)or->or_alias_has,0) ;
		if	( ret < 0 )
		{
			XOERR("",ret) ;
			return	NULL ;
		}
	}

	for	( a = 0 ; a < or->or_nb_attr ; a++ )
	{
		ar	= &(or->or_tab_attr[a]) ;
		retattr	= XoNew(XoAttrRef) ;
		if	( !retattr )
		{
			XOERR("",0) ;
			return	NULL ;
		}
		ret	=XoSetAttr(retattr,XoAttrRefNum,(void *)ar->ar_num,0) ;
		if	( ret < 0 )
		{
			XOERR("",ret) ;
			return	NULL ;
		}
		ret=XoSetAttr(retattr,XoAttrRefType,(void *)ar->ar_type,0) ;
		if	( ret < 0 )
		{
			XOERR("",ret) ;
			return	NULL ;
		}
		ret=XoSetAttr(retattr,XoAttrRefLgMax,(void *)ar->ar_lg_max,0) ;
		if	( ret < 0 )
		{
			XOERR("",ret) ;
			return	NULL ;
		}
		ret	= 
		XoSetAttr(retattr,XoAttrRefList,(void *)IS_A_LIST(ar),0) ;
		if	( ret < 0 )
		{
			XOERR("",ret) ;
			return	NULL ;
		}
		ret	= 
		XoSetAttr(retattr,XoAttrRefNoSave,(void *)IS_NO_SAVE(ar),0) ;
		if	( ret < 0 )
		{
			XOERR("",ret) ;
			return	NULL ;
		}
		ret	= XoSetAttr(retattr,
				XoAttrRefSaveUnset,(void *)IS_SAVE_UNSET(ar),0);
		if	( ret < 0 )
		{
			XOERR("",ret) ;
			return	NULL ;
		}
		ret	= XoSetAttr(retattr,
				XoAttrRefEmbAttr,(void *)IS_EMBATTR(ar),0);
		if	( ret < 0 )
		{
			XOERR("",ret) ;
			return	NULL ;
		}
		ret	= XoSetAttr(retattr,
				XoAttrRefWrapHidden,(void *)IS_WRAPHIDDEN(ar),0);
		if	( ret < 0 )
		{
			XOERR("",ret) ;
			return	NULL ;
		}
		ret	= XoSetAttr(retattr,
				XoAttrRefShDico,(void *)IS_SHDICO(ar),0);
		if	( ret < 0 )
		{
			XOERR("",ret) ;
			return	NULL ;
		}


		ret	= XoSetAttr	(retattr,XoAttrRefName,ar->ar_name,0) ;
		if	( ret < 0 )
		{
			XOERR("",ret) ;
			return	NULL ;
		}
		ret	= XoSetAttr	(retattr,XoAttrRefTypeName,
						ar->ar_type_name,0) ;
		if	( ret < 0 )
		{
			XOERR("",ret) ;
			return	NULL ;
		}

		if	( ar->ar_value )
		{
			ret	= 
			XoSetAttr(retattr,XoAttrRefValue,ar->ar_value,0) ;
			if	( ret < 0 )
			{
				XOERR("",ret) ;
				return	NULL ;
			}
		}

		if	( ar->ar_default )
		{
			ret	= 
			XoSetAttr(retattr,XoAttrRefDefault,ar->ar_default,0) ;
			if	( ret < 0 )
			{
				XOERR("",ret) ;
				return	NULL ;
			}
		}

		if	( ar->ar_help_line )
		{
			ret	= 
			XoSetAttr(retattr,XoAttrRefHelpLine,ar->ar_help_line,0);
			if	( ret < 0 )
			{
				XOERR("",ret) ;
				return	NULL ;
			}
		}

		if	( ar->ar_help_file )
		{
			ret	= 
			XoSetAttr(retattr,XoAttrRefHelpFile,ar->ar_help_file,0);
			if	( ret < 0 )
			{
				XOERR("",ret) ;
				return	NULL ;
			}
		}

#if	0
if	(strncmp(ar->ar_name,"xmlns:",6) == 0)
printf	("yyyy flag=%d\n",ar->ar_flag);
#endif

		ret	= XoAddInAttr	(retobj,XoObjRefListAttr,retattr,0) ;
		if	( ret < 0 )
		{
			XOERR("",ret) ;
			return	NULL ;
		}

	}

	return	retobj ;
}


/*
 *	sauvegarde des objets de reference definis par l'utilisateur
 *	cette fonction produit une sauvegarde homegene avec celle
 *	des donnees sous forme d'objets
 *
 *	ATTENTION cette fonction est dupliquee dans com/mkxo.y
 */

/*DEF*/	int32	XoSaveRef(int32 fout)
{
	int32	i ;
	t_objet_ref	*or ;
	void	*cfg ;
	void	*oor ;
	int32	nb ;
	int32	ret ;

	nb	= 0 ;
	for	( i = XoNbObjInt ; i < XoNbObj ; i++ )
	{
		or	= XoTabObj[i] ;
		if	( !or )			continue ;
		if	( or->or_num >= BaseUserObj(0) )	/* !!! */
			nb++ ;
	}

	if	( !nb )
	{
		XOERR("no ref",0) ;
		return	-1 ;
	}

	cfg	= XoNew(XoCfg) ;
	if	( !cfg )
	{
		XOERR("XoNew(XoCfg)",0) ;
		return	-2 ;
	}

	for	( i = XoNbObjInt ; i < XoNbObj ; i++ )
	{
		or	= XoTabObj[i] ;
		if	( !or )			continue ;
		if	( or->or_num < BaseUserObj(0) )		/* !!! */
			continue ;

		oor	= XoNewObjRef(or->or_num) ;
		if	( !oor )
		{
			XOERR("XoNewObjRef()",or->or_num) ;
			XoFree	(cfg,1) ;
			return	-3 ;
		}
		ret	= XoAddInAttr	((void *)cfg,XoCfgListObjRef,oor,0) ;
		if	( ret < 0 )
		{
			XOERR("XoAddInAttr()",ret) ;
			XoFree	(cfg,1) ;
			return	-4 ;
		}

	}

	ret	= XoSave((void *)cfg,fout) ;
	if	( ret < 0 )
	{
		XOERR("XoSave()",ret) ;
		XoFree	(cfg,1) ;
		return	-5 ;
	}

	XoFree	(cfg,1) ;
	return	nb ;
}
