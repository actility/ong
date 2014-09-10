
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

#include	"_whatstr.h"
#include	<stdio.h>
#include	<stdlib.h>
#if		!defined(DOS) && !defined(WIN32)
#include	<unistd.h>
#endif
#include	<string.h>
#include	<ctype.h>
#include	<memory.h>
#include	<errno.h>


#if	defined(DOS) && !defined(WIN32)
void _cdecl AfxTrace(char * pszFormat, ...);
#endif


#define		RIGHT_ADD	1
#define		LEFT_ADD	2


#define		IS_MODIFIED	1
#define		SET_MODIFIED	2
#define		UNSET_MODIFIED	3

static		long		NbMalloc ;
static		long		LgMalloc ;

/*
 *	a mettre avant include intstr.h
 */
char		*xomalloc(size_t lg)
{
	NbMalloc++ ;
	LgMalloc+=lg ;
	return	((char *)malloc(lg)) ;
}

#include	"xoapipr.h"
#include	"intstr.h"
#include	"xointpr.h"

#ifdef	RTL_MBR
#include	"mbr/include/mbr.h"
#endif


static int32 copy_list_obj(char *src,char *dst,int32 an,int32 at,int32 strict) ;
static	int32	copy_list_basic(char *src,char *dst,int32 anum,int32 atype) ;

void		xogetmalloc(int32 *nb, int32 *lg)
{
	*nb	= (int32)NbMalloc ;
	*lg	= (int32)LgMalloc ;
}

int32		xoatoi(register char *s)
{
	return	atoi(s);
#if	0
	int32	neg ;
	register	int32	n	= 0 ;

	if	( *s == '-' )
	{
		neg	= 1 ;
		s++ ;
	}
	else
		neg	= 0 ;

	while	( *s >= '0' && *s <= '9' )
	{
		n	= ( n * 10 ) + *s - '0' ;
		s++ ;
	}

	return	( neg ? -n : n ) ;
#endif
}



extern	int32		(*XofDeamonNew)(t_obj *o) ;
extern	int32		(*XofDeamonFree)(t_obj *o) ;
extern	int32		XoDisableDeamon ;

static	char	*xo_file = __FILE__ ;


static	t_list_info	ListInfoNull ;

static	void		(*xoErrorHandler)();

char	*XoVersion()
{
	return	xo_whatStr;
}

void	xoerr(char *file, int32 line, char *mes, int32 par)
{
	if(xoErrorHandler)
	{
		(*xoErrorHandler)(file,line,mes,par);
		return;
	}
#if	!defined(DOS) || defined(WIN32)
fprintf(stderr,"### XO : %14.14s:%ld [%s %ld]\n",file,(long)line,mes,(long)par);
fflush(stderr) ;
#else
AfxTrace("### XO : %14.14s:%ld [%s %ld]\n",file,(long)line,mes,(long)par);
#endif
}

/* AJOUT RVM 13/12/1996 - Surcharge fonction d'erreur */

void XoSetErrorHandler(void (*errFunc)(char *file,int32 line ,char *mes,int32 par))
{
	xoErrorHandler=errFunc;
}

static	int32	valide_bool_value(int32 val)
{
	if	( val != 0 && val != 1 )
		return	0 ;
	return	1 ;
}

static	int32	valide_str_value(char *lval, char *val)
{
	char	annul = '\0' ;
	char	*pt ;
	int32	ok ;
	int32	dep ;
	int32	dernier ;

	if	( !lval || !*lval )
		return	1 ;

	ok	= 0 ;
	dernier	= 0 ;
	while	(*lval && !ok && !dernier)
	{
		pt	= (char *)strchr(lval,'|') ;
		if	( pt )
		{
			annul	= *pt ;
			*pt	= '\0' ;
		}
		else
		{
			dernier	= 1 ;
		}
/*
fprintf(stderr,"lval=[%s] val=[%s]\n",lval,val) ;
*/
		if	( strcmp(lval,val) == 0 )
			ok	= 1 ;
		dep	= strlen(lval) ;
		lval	+= dep + 1 ;
		if	( pt )
			*pt	= annul ;
	}
	return	ok ;
}

static	int32	valide_int_value(char *lval, int32 val)
{
	char	tmp[32] ;

	sprintf	(tmp,"%ld",(long)val) ;
	return	(valide_str_value(lval,tmp)) ;
}

static	int32	valide_char_value(char *lval, int32 val)
{
	char	tmp[32] ;

#ifndef	IS32
#ifdef	isprint
#undef	isprint
#endif
#endif
	if	( !isprint((int)val) )
	{
		XOERR("non printable char",val) ;
		return	0 ;
	}
	sprintf	(tmp,"%c",(unsigned char)val) ;
	return	(valide_str_value(lval,tmp)) ;
}

/*
 *	ajoute un elem dans une liste a droite de l'elem courant
 *	cet elem devient l'elem courant
 */

static	void	add_in_list(t_list_info *li, t_list *el)
{

	if	( !li->li_nb || !li->li_first || !li->li_last )
	{
		li->li_first	= el ;
		li->li_last	= el ;
		li->li_curr	= el ;
		li->li_nb++ ;
		return ;
	}

	if	( !li->li_curr )
		li->li_curr	= li->li_first ;

	el->l_next		= li->li_curr->l_next ;
	li->li_curr->l_next	= el ;
	if	( el->l_next )		/* insertion a droite */
		el->l_next->l_prev	= el ;
	else
		li->li_last	= el ;	/* ajout a la fin */

	el->l_prev		= li->li_curr ;
	li->li_curr		= el ;
	li->li_nb++ ;
	return ;
}

/*
 *	ajoute un elem dans une liste a gauche de l'elem courant
 *	cet elem devient l'elem courant
 */

static	void	ins_in_list(t_list_info *li, t_list *el)
{

	if	( !li->li_nb || !li->li_first || !li->li_last )
	{
		li->li_first	= el ;
		li->li_last	= el ;
		li->li_curr	= el ;
		li->li_nb++ ;
		return ;
	}

	if	( !li->li_curr )
		li->li_curr	= li->li_first ;

	el->l_prev		= li->li_curr->l_prev ;
	li->li_curr->l_prev	= el ;

	if	( el->l_prev )		/* insertion a droite */
		el->l_prev->l_next	= el ;
	else
		li->li_first	= el ;	/* ajout au debut */

	el->l_next		= li->li_curr ;
	li->li_curr		= el ;
	li->li_nb++ ;
	return ;
}

/*
 *	delete l'elem courant dans une liste
 *	l'elem se trouvant a sa gauche devient l'elem courant
 */

static	t_list	*sup_in_list(t_list_info *li)
{
	t_list	*curr ;

	if	( !li->li_nb || !li->li_first || !li->li_last )
	{
		return NULL ;
	}

	if	( !li->li_curr )
		li->li_curr	= li->li_first ;

	curr		= li->li_curr ;
	li->li_curr	= curr->l_prev ;

	if	( curr->l_next )
		/* le courant a un voisin droit => ce n'est pas le dernier */
		curr->l_next->l_prev	= curr->l_prev ;
	else
		/* le courant n'a pas de voisin droit => c'est le dernier */
		li->li_last		= curr->l_prev ; 


	if	( curr->l_prev )
		/* le courant a un voisin gauche => ce n'est pas le premier */
		curr->l_prev->l_next	= curr->l_next ;
	else
		/* le courant n'a pas de voisin gauche => c'est le premier */
		li->li_first	= curr->l_next ;

	li->li_nb-- ;
	return	curr ;
}


#if	0
/*
 *	pour un objet ref et un num attribut retourne l'indice basic
 *	cette fonction n'est plus utilisee , le calcul est fait une fois
 *	pour toute dans lattr() .
 */

static	int32	indice_basic(or,nrefattr)
t_objet_ref	*or ;
int32		nrefattr ;
{
	int32	indice	= 0 ;
	int32	nb ;
	t_attribut_ref	*a ;

	a 	= &(or->or_tab_attr[0]) ; 
	nb	= or->or_nb_attr ;
	for	( ; a < &(or->or_tab_attr[nb]) ; a++ )
	{
		switch(a->ar_mode)
		{
	 		case	BASIC_ATTR :
				if	( a->ar_num == nrefattr )
					return	( indice ) ;
				indice++ ;
			break ;
			default :
			break ;
		}
	}
	XOERR("indice_basic()",nrefattr) ;
	return	-1 ;
}
#endif


/*
 *	valorise les attributs basic ayant une valeur par defaut
 *	fonction appelee par XoNew()
 *
 *	les XoBool sont valorises a zero s'il n'ont pas de defaut
 *	ce qui est fait de toute facon puisque memset a zero de la
 *	table des basic attr
 *	les XoBuffer n'ont pas de valeur par defaut
 */

static	int32	prevalo_obj(t_objet_ref *or, t_obj *o)
{
	t_attribut_ref	*ar ;
	int32		nb ;
	int32		ret ;
	int32		value ;


	ar 	= &(or->or_tab_attr[0]) ; 
	nb	= or->or_nb_attr ;
	for	( ; ar < &(or->or_tab_attr[nb]) ; ar++ )
	{
		if	(ar->ar_mode != BASIC_ATTR)		continue ;
		if	(IS_A_LIST(ar))				continue ;
		if	(!ar->ar_default && ar->ar_type != XoBool)
			continue ;
#if	0
		if	(ar->ar_type == XoString 
				&& XoNumAttrInObj(o,XOML_BALISE_ATTR) > 0)
		{
			// c'est un objet xml on ne valorise pas les defaults
			// sur les attributs xmlns && xmlns:
			//
			if	(strcmp(ar->ar_name,"xmlns") == 0)
				continue;
			if	(strncmp(ar->ar_name,"xmlns:",6) == 0)
				continue;
		}
#endif
		switch	(ar->ar_type)
		{
#if	0
		case	XoVoid :
			ret=XoSetAttr((void *)o,ar->ar_num,(void *)0,0) ;
			if	( ret < 0 )
			{
				XOERR("",ret) ;
				return	-1 ;
			}
		break ;
#endif
		case	XoString :
			ret=XoSetAttr((void *)o,ar->ar_num,ar->ar_default,0) ;
			if	( ret < 0 )
			{
				XOERR("",ret) ;
				return	-1 ;
			}
		break ;
		case	XoBool :
			if	( !ar->ar_default )
				value	= 0 ;
			else
				value	= xoatoi(ar->ar_default) ;
			ret=XoSetAttr((void *)o,ar->ar_num,(void *)value,0) ;
			if	( ret < 0 )
			{
				XOERR("",ret) ;
				return	-1 ;
			}
		break ;
		case	XoInt4B :
			value	= xoatoi(ar->ar_default) ;
			ret=XoSetAttr((void *)o,ar->ar_num,(void *)value,0) ;
			if	( ret < 0 )
			{
				XOERR("",ret) ;
				return	-1 ;
			}
		break ;
		case	XoChar :
		{
			char	c ;

			c=ar->ar_default[0] ;
			ret=XoSetAttr((void *)o,ar->ar_num,(void *)(int32)c,0);
			if	( ret < 0 )
			{
				XOERR("",ret) ;
				return	-1 ;
			}
		}
		break ;
		default :
			XOERR("default value ???",ar->ar_num) ;
		break ;
		}
	}
	return	1 ;
}



/*
 *	attache a un objet des donnees utilisateur et un destructeur
 */

/*DEF*/	int32	XoSetUsr(void *obj, void *data, int32 (*destruc) (/* ??? */))
{
	t_obj	*o ;

	o	= (t_obj *)obj ;
	if	( !o )
	{
		XOERR("null",0) ;
		return	-1 ;
	}

	if	( o->o_magic != O_MAGIC )
	{
		XOERR("magic",o->o_magic) ;
		return	-2 ;
	}

	o->o_usr_ptr	= data ;
	o->o_free_usr	= destruc ;

	return	1 ;
}

/*
 *	retourne les donnees utilisateur d'un objet
 */

/*DEF*/	void	*XoGetUsr(void *obj)
{
	t_obj	*o ;

	o	= (t_obj *)obj ;
	if	( !o )
	{
		XOERR("null",0) ;
		return	NULL ;
	}

	if	( o->o_magic != O_MAGIC )
	{
		XOERR("magic",o->o_magic) ;
		return	NULL ;
	}

	return	o->o_usr_ptr ;
}

#if	0
int32	__destruction__(o,data)
t_obj	*o ;
void	*data ;
{

	fprintf(stderr,"destruct %s\n",o->o_or->or_name) ;
}
#endif

/*
 *	creation int32erne d'une occurence d'objet
 */

static	void	*__xonew(int32 nobj, t_obj *recupobj)
     		      
     		           	/* on a deja une structure t_obj */
{
	t_obj		*o ;
	t_objet_ref	*or ;

	char		*bufobj ; /* contient tout l'objet */
	int32		aliasobj;

	/*
	 *	on ne peut faire ce test pour le moment 
	 *	une erreur dans l'atelier a entraine la sauvegarde
	 *	de XoVoidObj dans les applis .
	 */
#if	0
#endif

alias	:
	if	( nobj == XoVoidObj )
	{
		XOERR("XoNew VoidObj ???",nobj) ;
		return	NULL ;
	}

	if	( !IS_NUM_OBJ(nobj) )
	{
		XOERR("bad num obj",nobj) ;
		return	NULL ;
	}

	or	= XoGetRefObj(nobj) ;
	if	( !or )
	{
		XOERR("undefined obj",nobj) ;
		return	NULL ;
	}

	if	(or->or_alias && *(or->or_alias))
	{

		aliasobj	= XoGetNumType(or->or_alias);
		if	(aliasobj < 0)
		{
			XOERR("undefined alias",nobj) ;
			return	NULL ;
		}

		nobj	= aliasobj;
		goto	alias;
	}

	if	( recupobj )
	{
		/*	on recupere un objet existant */
		o	= recupobj ;
		if	( o->o_magic != O_MAGIC )
		{
			XOERR("magic",o->o_magic) ;
			return	NULL ;
		}
		if	( o->o_buffer )
		{
			free	(o->o_buffer) ;
			o->o_buffer	= NULL ;
		}
/*
fprintf(stderr,"realloc obj %lx : %s -> %s %ld\n",o,
(long)XoGetRefObjName(o->o_num),XoGetRefObjName(or->or_num),(long)(or->or_nb_obj+1)) ;
fflush(stderr) ;
*/
	}
	else
	{
		o	= (t_obj *)malloc(sizeof(t_obj)) ;
		if	( !o )
		{
			XOERR("malloc(%ld) object ",(long)nobj) ;
			return	NULL ;
		}
/*
fprintf(stderr,"alloc obj %s %lx %ld\n",XoGetRefObjName(or->or_num),(long)o,(long)(or->or_nb_obj+1)) ;
fflush(stderr) ;
*/
	}
	memset	(o,0,sizeof(t_obj)) ;

	bufobj	= (char *)malloc((size_t)(or->or_sz_tot+1)) ;
	if	( !bufobj )
	{
		XOERR("malloc(%ld) buffer object ",(long)or->or_sz_tot) ;
		return	NULL ;
	}
	memset	(bufobj,0,(size_t)or->or_sz_tot) ;

	o->o_buffer	= bufobj ;
	o->o_tab_attr	= (u_attr *)(bufobj + or->or_dep_attr) ;

	if	( or->or_dep_basic )
	{
		o->o_tab_ba	= (t_basic_attr *)(bufobj + or->or_dep_basic) ;
	}

	if	( or->or_dep_str )
	{
		o->o_buf_str	= (char *)(bufobj + or->or_dep_str) ;
	}

	o->o_magic	= O_MAGIC ;
	o->o_num	= or->or_num ;
	o->o_or		= or ;

	prevalo_obj(or,o) ;
XoEnterCS();
	if	( !or->or_first )
	{	/* 1er objet du type */
		or->or_first	= (char *)o ;	
		or->or_last	= NULL ;	
		or->or_nb_obj	= 0 ;
	}
	else
	{	/* il y en a deja */
		((t_obj *)or->or_last)->o_next	= o ;
	}
	o->o_prev	= (t_obj *)or->or_last ;
	or->or_last	= (char *)o ;	
	or->or_nb_obj++ ;

	if	(or->or_custom)
	{
		O_SET_OBIX(o);
//printf("set obix flags\n");
	}

	if	( !XoDisableDeamon && XofDeamonNew )
	{
		(*XofDeamonNew)(o) ;
	}
XoLeaveCS();
	return	( (char *)o) ;
}

static	void	*xonew(int32 nobj, t_obj *recupobj)
{
	void	*ret;

	ret = __xonew(nobj,recupobj);
	return	ret;
}

/*
 *	creation d'une occurence d'objet
 */

/*DEF*/	void	*XoNew(int32 nobj)
{
	void	*ret;
	ret	= xonew(nobj,NULL) ;
	if	( !ret )
	{
		XOERR("XoNew err.",nobj) ;
		return	NULL ;
	}
	return	ret ;
}

/*
 *	detache l'objet de sa classe initiale
 */

static	int32	__xodetache(t_obj *o)
{
	t_obj		*op ;
	t_obj		**ptop ;
	t_obj		*on ;
	t_obj		**pton ;
	t_objet_ref	*or ;
	int		ret = 1;

	if	( !o )
	{
		XOERR("null",0) ;
		return	-1 ;
	}

	if	( o->o_magic != O_MAGIC )
	{
		XOERR("magic",o->o_magic) ;
		return	-2 ;
	}

	or	= o->o_or ;
	if	( or->or_num != o->o_num )
	{
		XOERR("ref failure",o->o_num) ;
		return	-3 ;
	}


XoEnterCS();
	if	( !or->or_first )
	{	/* pas d'objet ??? */
		XOERR("list obj empty",o->o_num) ;
		ret	= -4 ;
		goto	leavecs;
	}
	op	= o->o_prev ;
	on	= o->o_next ;
	if	( op && op->o_magic != O_MAGIC )
	{
		XOERR("list obj bad magic",op->o_magic) ;
		ret	= -6 ;
		goto	leavecs;
	}
	if	( on && on->o_magic != O_MAGIC )
	{
		XOERR("list obj bad magic",on->o_magic) ;
		ret	=-7 ;
		goto	leavecs;
	}
	if	( !op )
		ptop	= (t_obj **)&(or->or_first) ;
	else
		ptop	= &(op->o_next) ;
	*ptop	= o->o_next ;

	if	( !on )
		pton	= (t_obj **)&(or->or_last) ;
	else
		pton	= &(on->o_prev) ;
	*pton	= o->o_prev ;

	or->or_nb_obj-- ;
leavecs :
XoLeaveCS();

	return	ret ;
}

static	int32	xodetache(t_obj *o)
{
	int32	ret;

	ret = __xodetache(o);
	return	ret;
}

/*
 *	reutilisation d'une occurence d'objet pour un type different
 */

/*DEF*/	int32	XoReUse(void *obj, int32 numobj)
{
	int32	numret ;
	char	*ret;

	if	( !obj )
	{
		XOERR("obj null",0) ;
		return	-1 ;
	}

	numret	= XoClear(obj) ;
	if	( numret < 0 )
	{
		XOERR("XoReUse err.",numret) ;
		return	numret ;
	}

	/*
	 *	detache l'objet de sa classe initiale
	 */
	numret	= xodetache((t_obj *)obj) ;
	if	( numret < 0 )
	{
		XOERR("XoReUse err.",numret) ;
		return	numret ;
	}

	ret	= xonew(numobj,(t_obj *)obj) ;
	if	( !ret )
	{
		XOERR("XoReUse err. 1",numobj) ;
		return	-2 ;
	}
	if	( ret != obj )
	{
		XOERR("XoReUse err. 2",numobj) ;
		return	-3 ;
	}
	return	1 ;
}

/*
 *	free d'une value ayant comme attr ref ar
 */

static	void	free_value(u_val *v, t_attribut_ref *ar)
{
	switch	(ar->ar_type)
	{
#if	0
		case	XoVoid :
			return	;	/* pas de free sur void * */
		break ;
#endif
		case	XoString :
			if	( IS_PREALLOC(ar) )
				return ;	/* dans l'objet	*/
			if	( IS_NICE_STR(ar) )
				return ;	/* dans u_val	*/
		break ;
		case	XoBuffer :
			if	( IS_PREALLOC(ar) )
				return ;	/* dans l'objet	*/
		break ;
		default :
			return	;		/* dans u_val	*/
		break ;
	}
	if	( v->v_dyn )
	{
/*
printf("[%s]\n",v->v_dyn) ;
*/
		free		(v->v_dyn) ;
		v->v_dyn	= NULL ;
	}
	return	;
}



/*
 *	free d'une liste d'attributs basic
 */

static	int32	free_list_attr(t_list_info *li)
{
	t_list		*l ;
	t_list		*sv ;
	t_attribut_ref	*ar ;

	if	( !li )
	{
		XOERR("",0) ;
		return(-1000) ;
	}

	ar	= XoGetRefAttr(li->li_num) ;
	if	( !ar )
	{
		XOERR("",li->li_num) ;
		return(-1001) ;				/* !!! */
	}
	l	= li->li_first ;
	while	(l)
	{
		free_value	(&(l->l_val),ar) ;
		sv		= l->l_next ;
		free		(l) ;
		l		= sv ;
	}
	free	(li) ;
	return	1 ;
}




/*
 *	free d'une liste d'attributs objets
 */

static	int32	free_list_obj(t_list_info *li, int32 reccur)
{
	t_list		*l ;
	t_list		*sv ;
	t_attribut_ref	*ar ;
	int32		ret ;

	ret	= 1 ;
	if	( !li )
	{
		XOERR("",0) ;
		return(-2000) ;
	}

	ar	= XoGetRefAttr(li->li_num) ;
	if	( !ar )
	{
		XOERR("",li->li_num) ;
		return(-2001) ;				/* !!! */
	}
	l	= li->li_first ;
	while	(l && ret > 0 )
	{
		if	( reccur )
			ret	= XoFree(l->l_obj,reccur) ;
		sv	= l->l_next ;
		free	(l) ;
		l	= sv ;
	}
	free	(li) ;
	if	( ret < 0 )
		XOERR("",ret) ;
	return	ret ;
}

#if	0
/*
 *	trouve l'objet o dans la liste de nb objet commencant a first
 *	retourne dans prev l'elem precedent
 */

static	int32	find_obj(t_obj *first, int32 nb, t_obj *o, t_obj **prev)
{
	t_obj	*op ;

	*prev	= NULL ;
	op	= NULL ;
	while	( first && nb > 0 )
	{
		if	( first == o )
		{
			nb	= -1 ;
		}
		else
		{
			op	= first ;
			first	= first->o_next ;
			nb-- ;
		}
	}
	if	( nb != -1 )
		return	0 ;
	*prev	= op ;
	return	1 ;
}
#endif


/*
 *	libere tous les attributs d'une occurrence
 */

static	int32	free_all_elem(t_obj *o, t_objet_ref *or, int32 reccur)
{
	u_attr		*a ;
	t_attribut_ref	*ar ;

	int32		mode ;
	int32		nb ;
	int32		i ;
	int32		ret ;

	ret	= 1 ;
	nb	= or->or_nb_attr ;
	for	( i = 0 ; i < nb && ret > 0 ; i++ )
	{
		a	= &(o->o_tab_attr[i]) ;
		ar	= &(or->or_tab_attr[i]) ;
		mode	= ar->ar_mode ;
		switch	(mode)
		{
	 		case	BASIC_ATTR :
				if	( a->a_ba )
				{
					t_basic_attr	*ba ;

					ba	= a->a_ba ;
					free_value(&(ba->ba_val),ar) ;
					a->a_ba	= NULL ;
				}
			break ;
			case	OBJ_ATTR :
				if	( a->a_ob && reccur )
				{
					ret	= XoFree(a->a_ob,reccur) ;
					a->a_ob	= NULL ;
				}
			break ;
			case	LIST_BASIC_ATTR :
				if	( a->a_li )
				{
/*
printf("free_list_attr\n") ;
*/
					ret	= free_list_attr(a->a_li);
					a->a_li	= NULL ;
				}
			break ;
			case	LIST_OBJ_ATTR :
				if	( a->a_li )
				{
/*
printf("free_list_obj\n") ;
*/
					ret	= free_list_obj(a->a_li,reccur);
					a->a_li	= NULL ;
				}
			break ;
			default :
			break ;
		}
	}

	return	ret ;
}

static	int32	xofree(char *obj, int32 reccur, int32 objonly)
    		      
     		        	/*	obj. imbriques a detruire	*/
     		         	/*	on ne touche pas aux attributs	*/
{
	t_obj		*o ;
	t_objet_ref	*or ;

	int32		ret ;

	o	= (t_obj *)obj ;
	if	( !o )
	{
		XOERR("null",0) ;
		return	-1 ;
	}

	if	( o->o_magic != O_MAGIC )
	{
		XOERR("magic",o->o_magic) ;
		return	-2 ;
	}

	or	= o->o_or ;
	if	( or->or_num != o->o_num )
	{
		XOERR("ref failure",o->o_num) ;
		return	-3 ;
	}

	ret	= xodetache((t_obj *)o) ;
	if	( ret < 0 )
	{
		XOERR("detache failure",ret) ;
		return	ret ;
	}

	if	( !XoDisableDeamon && XofDeamonFree )
	{
		(*XofDeamonFree)(o) ;
	}

	if	( o->o_free_usr )
	{
		/*
		 *	appel le destructeur des donnees utilisateur
		 */
		 (*(o->o_free_usr))(o,o->o_usr_ptr) ;
	}

	if	( !objonly )
	{
		ret	= free_all_elem(o,or,reccur) ;
		if	(o->o_buffer)
		{
			free	(o->o_buffer) ;
			o->o_buffer	= NULL ;
		}
	}
	else
		/* on ne touche pas aux attributs */
		ret	= 1 ;

	o->o_magic	+= 123 ;
	free	(o) ;
/*
fprintf(stderr,"free obj %s %lx %ld\n",XoGetRefObjName(or->or_num),(long)o,(long)(or->or_nb_obj+1)) ;
fflush(stderr) ;
*/
	if	( ret < 0 )
		XOERR("",ret) ;
	return	ret ;
}

/*
 *	liberation d'une occurence d'objet 
 *	par defaut on libere les attributs => objonly = 0
 */

/*DEF*/	int32	XoFree(void *obj, int32 reccur)
{
	int32	objonly	= 0 ;

	return	xofree ( obj , reccur , objonly ) ;
}

/*
 *	clear d'une occurence d'objet 
 *	liberation des attributs mais pas des structures propres a l'objet
 *	qui permet de contenir ces attributs .
 */

/*DEF*/	int32	XoClear(void *obj)
{
	t_obj		*o ;
	t_objet_ref	*or ;

	int32		ret ;
	int32		reccur ;

	o	= (t_obj *)obj ;
	if	( !o )
	{
		XOERR("null",0) ;
		return	-1 ;
	}

	if	( o->o_magic != O_MAGIC )
	{
		XOERR("magic",o->o_magic) ;
		return	-2 ;
	}

	or	= o->o_or ;
	if	( or->or_num != o->o_num )
	{
		XOERR("ref failure",o->o_num) ;
		return	-3 ;
	}

	reccur	= 1 ;
	ret	= free_all_elem(o,or,reccur) ;
	if	( ret < 0 )
		XOERR("",ret) ;
	return	ret ;
}

/*
 *	remplacement d'une occurence d'objet 
 */
#if	0
/*DEF*/	int32	XoMove(void *objsrc,void *objdst)
{
	t_objet_ref	*or ;
	t_attribut_ref	*ar ;
	t_obj	*os ;
	t_obj	*od ;
	t_obj	*svp ;		/* sauve previous	*/
	t_obj	*svn ;		/* sauve next		*/
	int32	objonly = 1 ;
	int32	reccur  = 0 ;
	int32	ret ;

	int32	i ;
	int32	ibasic ;
	u_attr	*a ;

	os	= (t_obj *)objsrc ;
	od	= (t_obj *)objdst ;
	if	( !os || !od )
	{
		XOERR("null",0) ;
		return	-1 ;
	}
	if	( os->o_magic != O_MAGIC || od->o_magic != O_MAGIC )
	{
		XOERR("magic",0) ;
		return	-2 ;
	}
	if	( od->o_or->or_num != os->o_or->or_num )
	{
		XOERR("types of obj do not match",0) ;
		return	-3 ;
	}
	or	= od->o_or ;

	ret	= XoClear	(objdst) ;	
	if	( ret < 0 )
	{
		XOERR("",ret) ;
		return	-4 ;
	}

	/*
	 *	passe les attributs de l'objet source dans l'objet destination
	 */

	memcpy(od->o_tab_attr,os->o_tab_attr,
		(size_t)(or->or_nb_attr*sizeof(u_attr))) ;
	memcpy(od->o_tab_ba,os->o_tab_ba,
		(size_t)(or->or_nb_basic*sizeof(t_basic_attr))) ;
	memcpy(od->o_buf_str,os->o_buf_str,
		(size_t)(or->or_sz_str)) ;

	/*
	 *	les adresses des structures basic changent !!!
	 *	les adresses des chaines preallouees changent !!!
	 */
	for	( i = 0 ; i < or->or_nb_attr ; i++ )
	{
		a	= &(od->o_tab_attr[i]) ;
		ar	= &(or->or_tab_attr[i]) ;
		if	( ar->ar_mode == BASIC_ATTR && a->a_ba )
		{
			ibasic	= ar->ar_indice_basic ;
			a->a_ba	= &(od->o_tab_ba[ibasic]) ;
			if	( IS_PREALLOC(ar) )
			{
				a->a_ba->ba_val.v_dyn =
					od->o_buf_str + ar->ar_dep_str ;
			}
		}
	}


	/*
	 *	libere l'objet source mais pas ses attributs qui sont
	 *	passes dans l'objet destination
	 */
	objonly = 1 ;
	reccur  = 0 ;
	ret	= xofree	(objsrc,reccur,objonly) ;
	if	( ret < 0 )
	{
		XOERR("",ret) ;
		return	-5 ;
	}
	return	1 ;
}
#endif

/*
 *	comme l'objet est alloue en 2 parties le move est maint32enant 
 *	plus facile , independant de la structure des attributs,et plus rapide .
 */

/*DEF*/	int32	XoMove(void *objsrc, void *objdst)
{
	t_obj	*os ;
	t_obj	*od ;
	int32	objonly = 1 ;
	int32	reccur  = 0 ;
	int32	ret ;

	os	= (t_obj *)objsrc ;
	od	= (t_obj *)objdst ;
	if	( !os || !od )
	{
		XOERR("null",0) ;
		return	-1 ;
	}
	if	( os->o_magic != O_MAGIC || od->o_magic != O_MAGIC )
	{
		XOERR("magic",0) ;
		return	-2 ;
	}
	if	( od->o_or->or_num != os->o_or->or_num )
	{
		XOERR("types of obj do not match",0) ;
		return	-3 ;
	}

	ret	= XoClear	(objdst) ;	
	if	( ret < 0 )
	{
		XOERR("",ret) ;
		return	-4 ;
	}

	/*
	 *	passe les attributs de l'objet source dans l'objet destination
	 */
	if	( od->o_buffer )
		free	(od->o_buffer) ;

	od->o_buffer	= os->o_buffer ;
	od->o_buf_str	= os->o_buf_str ;
	od->o_tab_ba	= os->o_tab_ba ;
	od->o_tab_attr	= os->o_tab_attr ;
	od->o_flag	= os->o_flag ;

	/*
	 *	libere l'objet source mais pas ses attributs qui sont
	 *	passes dans l'objet destination 
	 *		=> os->o_buffer ne sera pas libere 
	 */
	objonly = 1 ;		
	reccur  = 0 ;
	ret	= xofree	(objsrc,reccur,objonly) ;
	if	( ret < 0 )
	{
		XOERR("",ret) ;
		return	-5 ;
	}
	return	1 ;
}

/*
 *	fonction de transformation des arguments des fonctions de l'api
 *	en point32eurs int32ernes
 */

static	int32	translate(char *obj, int32 nrefattr, register t_obj **o, register t_objet_ref **or, u_attr **a, register t_attribut_ref **ar, int32 *mode)
{
	register	t_obj		*pto ;
	t_objet_ref	*ptor ;
	t_attribut_ref	*ptar ;
	int32	nattr ;

	pto	= (t_obj *)obj ;
	if	( !pto )
	{
		XOERR("null",0) ;
		return	-1 ;
	}

	if	( pto->o_magic != O_MAGIC )
	{
		XOERR("magic",0) ;
		return	-2 ;
	}

	ptor	= pto->o_or ;
	if	( ptor->or_num != pto->o_num )
	{
		XOERR("ref failure",pto->o_num) ;
		return	-3 ;
	}

	ptar	= XoGetRefAttr(nrefattr) ;
	if	( !ptar )
	{
		XOERR("ref",nrefattr) ;
		return	-5 ;
	}

	*mode	= ptar->ar_mode ;
	if	( *mode < 0 )
	{
		XOERR("ref",nrefattr) ;
		return	-6 ;
	}

	nattr	= NUM_ATTR_TO_INDICE(nrefattr) ;
	(*a)	= &(pto->o_tab_attr[nattr]) ;

	*o	= pto ;
	*or	= ptor ;
	*ar	= ptar ;

	if	( nattr < 0 )
		XOERR("",nattr) ;
	return	nattr ;
}


/*
 *	alloue/realloue une zone dynamique
 */

static	int32	allodyna(u_val *v, int32 lg)
     	    
     	    	/* nouvelle longueur */
{
	if	( v->v_dyn )
	{	/* deja allouee */
		if	( lg > V_LG(v) )
		{	/* pas assez grand */
			free	(v->v_dyn) ;
			v->v_dyn	= NULL ;
		}
	}
	if	( !v->v_dyn )
	{	/* pas allouee ou liberee */
		v->v_dyn	= (char *)malloc((size_t)lg) ;
		if	( !v->v_dyn )
		{
			XOERR("malloc()",errno) ;
			return -3 ;
		}
#ifdef	D_LG
		v->v_lg		= lg ;
#endif
	}
	return	lg ;
}

/*
 *	valorisation d'une value
 *	1) verification si il y a une liste de valeur possible
 *	2) Long Bool Char Date ras
 *	3) XoString	est-ce que la chaine tient dans u_val ?
 *			est-ce que la chaine est dans l'objet PRE_ALLOC
 *	4) verifie si la valeur change
 */

static	int32	set_value(register t_attribut_ref *ar, register u_val *v, register char *value, int32 lg,int32 *modif)
{
	*modif	= 0 ;
	switch(ar->ar_type)
	{
		case	XoDate :
		case	XoInt4B :
			if	( !valide_int_value(ar->ar_value,(int32)value) )
			{
				XOERR	("invalide int value",ar->ar_num) ;
				return	-22 ;
			}
			if	(v->v_l != (int32)value )
				*modif	= 1 ;
			v->v_l	= (int32)value ;	
/*
printf("%s set to %ld\n",XoGetRefAttrName(ar->ar_num),(long)v->v_l) ;
*/
		break ;
		case	XoBool :
			if	( !valide_bool_value((int32)value) )
			{
				XOERR	("invalide bool value",ar->ar_num) ;
				return	-22 ;
			}
			if	(v->v_l != (int32)value )
				*modif	= 1 ;
			v->v_l	= (int32)value ;	
/*
printf("%s set to %ld\n",XoGetRefAttrName(ar->ar_num),(long)v->v_l) ;
*/
		break ;
		case	XoChar :
			if	( !valide_char_value(ar->ar_value,(int32)value) )
			{
				char	tmp[128] ;


				sprintf	(tmp,"for '%s' value=%d",XoGetRefAttrName(ar->ar_num),(int32)value) ;
				XOERR	("invalide char value",ar->ar_num) ;
				XOERR	(tmp,ar->ar_num) ;
				return	-22 ;
			}
			if	(v->v_c != (unsigned char)(int32)value )
				*modif	= 1 ;
			v->v_c	= (unsigned char)(int32)value ;	
/*
printf("%s set to %ld\n",XoGetRefAttrName(ar->ar_num),(long)v->v_c) ;
*/
		break ;
#if	0
		case	XoVoid :
			if	(v->v_void != (void *)value )
				*modif	= 1 ;
			v->v_void	= (void *)value ;	
/*
fprintf(stderr,"set void value %10.10f\n",*(double *)(v->v_void)) ;
fflush(stderr) ;
*/
		break ;
#endif
		case	XoString :
#if 0
			if	( lg == -9999 )
			{
				if	( v->v_dyn )
					free(v->v_dyn);
				lg		= strlen(value)+1 ;
				v->v_dyn	= value;
				v->v_lg		= lg;
/*
fprintf(stderr,"dont alloc/copy %d\n",lg);
*/
				return	1;
			}
#endif
			if	( !valide_str_value(ar->ar_value,value) )
			{
				XOERR	("invalide string value",ar->ar_num) ;
/*
fprintf(stderr,"lst_val='%s' val='%s'\n",ar->ar_value,value) ;
fflush(stderr) ;
*/
				return	-22 ;
			}
			lg	= strlen(value) ;
			if	( ar->ar_lg_max != -1 && lg > ar->ar_lg_max )
			{
				XOERR("str too long",lg) ;
				return	-2 ;
			}
			if	( IS_NICE_STR(ar) )
			{
				if	( memcmp(v->v_str,value,(size_t)(lg+1)))
				{
					*modif	= 1 ;
					memcpy	(v->v_str,value,(size_t)(lg+1));
				}
/*
printf("%s set to [%s]\n",XoGetRefAttrName(ar->ar_num),v->v_str) ;
*/
			}
			else
			{
				int	dynini	= 0 ;
				if	( !IS_PREALLOC(ar) )
				{
					if	(!v->v_dyn)
						dynini	= 1 ;
					if	( allodyna(v,lg+1) < 0 )
					{
						XOERR("",0) ;
						return	-3 ;
					}
				}
				if	( dynini ||
					memcmp(v->v_dyn,value,(size_t)(lg+1)))
				{
					*modif	= 1 ;
					memcpy	(v->v_dyn,value,(size_t)(lg+1));
				}
/*
printf("%s set to [%s] %ld\n",XoGetRefAttrName(ar->ar_num),v->v_dyn,(long)V_LG(v)) ;
*/
			}
		break ;
		case	XoBuffer :
			if	( lg <= 0 )
			{
				XOERR("",0) ;
				return	-4 ;
			}
			if	( ar->ar_lg_max != -1 && lg > ar->ar_lg_max )
			{
				XOERR("",0) ;
				return	-5 ;
			}
			if	( allodyna(v,lg) < 0 )
			{
				XOERR("",0) ;
				return	-3 ;
			}
			if	( V_LG(v) != lg 
					|| memcmp(v->v_dyn,value,(size_t)lg) )
			{
				*modif	= 1 ;
				memcpy	(v->v_dyn,value,(size_t)lg) ;
				V_LG(v)	= lg ;
			}
		break ;
		default :
			XOERR("",ar->ar_type) ;
			return	-1 ;
		break ;
	}
	return	1 ;
}


/*
 *	recuperation d'une value
 */

static	char	*get_value(register t_attribut_ref *ar, register u_val *v, int32 *lg)
{
	register	char	*value = NULL ;

	switch(ar->ar_type)
	{
		case	XoBool :
		case	XoDate :
		case	XoInt4B :
			value	= (char *)&(v->v_l) ;
			if	(lg)
				*lg	= sizeof(int32) ;
		break ;
		case	XoChar :
			value	= (char *)&(v->v_c) ;
			if	(lg)
				*lg	= sizeof(char) ;
		break ;
#if	0
		case	XoVoid :
fprintf(stderr,"get void value %10.10f\n",*(double *)&(v->v_void)) ;
fflush(stderr) ;
			value	= (char *)&(v->v_void) ;
			if	(lg)
				*lg	= sizeof(void *) ;
		break ;
#endif
		case	XoString :
			if	( IS_NICE_STR(ar) )
			{
				value	= v->v_str ;
				if	(lg)
					*lg	= strlen(value) ;
			}
			else
			{
				value	= v->v_dyn ;
				if	(lg)
					*lg	= V_LG(v)-1 ;
			}
/*
			if	(lg)
				*lg	= strlen(value) ;	????
*/
		break ;
		case	XoBuffer :
			value	= v->v_dyn ;
			if	(lg)
				*lg	= V_LG(v) ;
		break ;
		default :
			value	= NULL ;
		break ;
	}
	if	( !value )
		XOERR("",0) ;
	return	value ;
}

/*
 *	valorisation d'un attribut
 */

/*DEF*/	int32	XoSetAttr(void *obj, int32 nrefattr, void *value, int32 lg)
{
	t_obj		*o ;
	t_objet_ref	*or ;
	u_attr		*a ;
	t_attribut_ref	*ar ;

	int32		mode ;
	int32		ibasic ;
	int32		nattr ;
	int32		ret ;

	int32		modif	= 0 ;

	nattr	= translate(obj,nrefattr,&o,&or,&a,&ar,&mode) ;
	if	( nattr < 0 )
	{
		XOERR("",nattr) ;
		return	nattr ;
	}


	if	( mode == BASIC_ATTR )
	{
		if	( !a->a_ba )
		{	/*	c'est une initialisation */
/*
			ibasic		= indice_basic(or,nrefattr) ;
*/
			ibasic		= ar->ar_indice_basic ;
			if	( ibasic < 0 || ibasic >= or->or_nb_basic )
			{
				XOERR("basic",nrefattr) ;
				return	-250 ;
			}
			a->a_ba		= &(o->o_tab_ba[ibasic]) ;
			if	( IS_PREALLOC(ar) )
			{	/* prealloue dans l'objet */
				a->a_ba->ba_val.v_dyn =
					o->o_buf_str + ar->ar_dep_str ;
			}
		}
		modif	= 0 ;
		ret 	= set_value(ar,&(a->a_ba->ba_val),value,lg,&modif);
		if	( ret < 0 )
		{
			a->a_ba	= NULL ;
			XOERR("",ret) ;
			return	( ret - 500 ) ;
		}
		if	(modif)
			O_SET_MODIFIED(o) ;
		return	1 ;
	}

	if	( mode == OBJ_ATTR )
	{
		t_obj		*lko ;		/* linked objet */

		lko	= (t_obj *)value ;
		if	( !lko )
		{
			/*	suppression	*/
			a->a_ob	= NULL ;
			O_SET_MODIFIED(o) ;
			return	1 ;
		}

		if	( lko->o_magic != O_MAGIC )
		{
			XOERR("magic",0) ;
			return	-102 ;
		}
		if	( ar->ar_type != XoVoidObj )
		{	/* ce n'est pas un obj de type Void => controle	*/
			if	( lko->o_num != ar->ar_type )
			{	/* type incorrect */
				XOERR("bad type linked obj",0) ;
				return	-103 ;
			}
		}
		modif	= 0 ;
		if	(a->a_ob != value)
			modif	= 1 ;
		a->a_ob	= value ;
#ifdef	WITH_FATHER
		lko->o_father	= obj ;
#endif
		if	( modif )
			O_SET_MODIFIED(o) ;
		return	1 ;
	}

	if	( mode == LIST_BASIC_ATTR || mode == LIST_OBJ_ATTR )
	{
		register	t_list_info	*li ;

		li	= a->a_li ;
		if	( !li || !li->li_nb || !li->li_first || !li->li_last )
		{	/*	non initialise */
			XOERR("",0) ;
			return	-103 ;
		}
		if	( !li->li_curr )
			li->li_curr	= li->li_first ;

		if	( mode == LIST_BASIC_ATTR )
		{
			modif	= 0 ;
			ret	= set_value(ar,&(li->li_curr->l_val),value,
								lg,&modif) ;
			if	( ret < 0 )
			{
				XOERR("",ret) ;
				return	( ret - 500 ) ;
			}
		}
		else
		{
			t_obj		*lko ;		/* linked objet */

			lko	= (t_obj *)value ;
			if	( lko->o_magic != O_MAGIC )
			{
				XOERR("magic",0) ;
				return	-104 ;
			}
			if	( li->li_curr->l_obj != value )
				modif	= 1 ;
			li->li_curr->l_obj	= value ;
#ifdef	WITH_FATHER
			lko->o_father		= obj ;
#endif
		}
		if	(modif)
			O_SET_MODIFIED(o) ;
		return	1 ;
	}
	XOERR("",0) ;
	return	-100 ;
}


/*
 *	valorisation d'un attribut de tous les objets d'un type
 */

/*DEF*/	int32	XoSetAllAttr(int32 numobj, int32 nrefattr, void *value, int32 lg)
{
	int32	nb ;
	int32	ret ;
	void	*obj ;

	nb	= 0 ;
	obj	= XoFirstObj	(numobj) ;
	while	( obj )
	{
		ret	= XoSetAttr	(obj,nrefattr,value,lg) ;
		if	( ret < 0 )
		{
			XOERR("XoSetAllAttr()",ret) ;
			return	ret ;
		}
		nb++ ;
		obj	= XoNextObj	(numobj) ;
	}
	return	nb ;
}






/*
 *	devalorisation d'un attribut
 *	seulement sur les attributs non liste sur ceux-ci il suffit de le
 *	supprimer
 */

/*DEF*/	int32	XoUnSetAttr(void *obj, int32 nrefattr)
{
	t_obj		*o ;
	t_objet_ref	*or ;
	u_attr		*a ;
	t_attribut_ref	*ar ;

	int32		mode ;
	int32		nattr ;

	nattr	= translate(obj,nrefattr,&o,&or,&a,&ar,&mode) ;
	if	( nattr < 0 )
	{
		XOERR("",nattr) ;
		return	nattr ;
	}


	if	( mode == BASIC_ATTR )
	{
		if	( !a->a_ba )
		{
			return	0 ;
		}
		// si non prealloc() && (String || Buffer) la valeur n'est pas
		// liberee => free_value() ... 4.0.25
		free_value(&(a->a_ba->ba_val),ar);
		a->a_ba	= NULL ;
		O_SET_MODIFIED(o) ;
		return	1 ;
	}

	if	( mode == OBJ_ATTR )
	{
		if	( !a->a_ob )
		{
			return	0 ;
		}
		// si c'est un objet il n'est pas libere ???
		a->a_ob	= NULL ;
		O_SET_MODIFIED(o) ;
		return	1 ;
	}

	if	( mode == LIST_BASIC_ATTR || mode == LIST_OBJ_ATTR )
	{
		return	-11 ;
	}
	return	-100 ;
}

/*
 *	est-ce qu'un attribut est renseigne
 */

/*DEF*/	int32	XoIsSetAttr(void *obj, int32 nrefattr)
{
	t_obj		*o ;
	t_objet_ref	*or ;
	u_attr		*a ;
	t_attribut_ref	*ar ;

	int32		mode ;
	int32		nattr ;

	nattr	= translate(obj,nrefattr,&o,&or,&a,&ar,&mode) ;
	if	( nattr < 0 )
	{
		XOERR("",nattr) ;
		return	nattr ;
	}


	if	( mode == BASIC_ATTR )
	{
		if	( !a->a_ba )
		{
			return	0 ;
		}
		return	1 ;
	}

	if	( mode == OBJ_ATTR )
	{
		if	( !a->a_ob )
		{
			return	0 ;
		}
		return	1 ;
	}

	if	( mode == LIST_BASIC_ATTR || mode == LIST_OBJ_ATTR )
	{
		return	-11 ;
	}
	return	-100 ;
}

/*
 *	devalorisation d'un attribut de tous les objets d'un type
 *	seulement sur les attributs non liste
 *	supprimer
 */
/*DEF*/	int32	XoUnSetAllAttr(int32 numobj, int32 nrefattr)
{
	int32	nb ;
	int32	ret ;
	void	*obj ;

	nb	= 0 ;
	obj	= XoFirstObj	(numobj) ;
	while	( obj )
	{
		ret	= XoUnSetAttr	(obj,nrefattr) ;
		if	( ret < 0 )
		{
			XOERR("XoUnSetAllAttr()",ret) ;
			return	ret ;
		}
		nb++ ;
		obj	= XoNextObj	(numobj) ;
	}
	return	nb ;
}


/*
 *	recuperation d'un attribut
 */

/*DEF*/	void	*XoGetAttr(void *obj, int32 nrefattr, int32 *lg)
{
	t_obj		*o ;
	t_objet_ref	*or ;
	u_attr		*a ;
	t_attribut_ref	*ar ;

	int32		nattr ;
	int32		mode ;
	register	char		*value ;

	if (lg) *lg = 0 ; nattr	= translate(obj,nrefattr,&o,&or,&a,&ar,&mode) ;
	if	( nattr < 0 )
	{
		XOERR("",0) ;
		return	NULL ;
	}

	if	( mode == OBJ_ATTR )
	{
		value	= a->a_ob ;
		return	value ;
	}

	if	( mode == BASIC_ATTR )
	{
		if	( !a->a_ba )
		{	/*	non initialise */
			value	= NULL ;
		}
		else
		{
			value	= get_value(ar,&(a->a_ba->ba_val),lg);
		}
		return	value ;
	}

	if	( mode == LIST_BASIC_ATTR || mode == LIST_OBJ_ATTR )
	{
		register	t_list_info	*li ;

		li	= a->a_li ;
		if	( !li )
		{	/*	non initialise */
			return	NULL ;
		}
		if	( !li->li_nb || !li->li_first || !li->li_last )
		{
			XOERR("",0) ;
			return	NULL ;
		}
		if	( !li->li_curr )
			li->li_curr	= li->li_first ;

		if	( mode == LIST_BASIC_ATTR )
			value	= get_value(ar,&(li->li_curr->l_val),lg) ;
		else
			value	= li->li_curr->l_obj ;
		return	value ;
	}
	XOERR("",0) ;
	return	NULL ;
}

/*
 *	est-ce qu'un attribut a une valeur egale a la valeur par defaut
 */

/*DEF*/	int32	XoAttrValueIsDef(void *obj, int32 nrefattr)
{
	t_obj		*o ;
	t_objet_ref	*or ;
	u_attr		*a ;
	t_attribut_ref	*ar ;

	int32		nattr ;
	int32		mode ;

	nattr	= translate(obj,nrefattr,&o,&or,&a,&ar,&mode) ;
	if	( nattr < 0 )
	{
		XOERR("",0) ;
		return	-1 ;
	}

	if	( mode != BASIC_ATTR )
	{
		XOERR("",0) ;
		return	-2 ;
	}

	if	( a->a_ba )
	{
		t_basic_attr	*ba ;

		ba	= a->a_ba ;
		if	(xo_diff_default(&(ba->ba_val),ar))
			return	0 ;
		else
			return	1 ;
	}

	return	0 ;
}

static	int32	compare_elem(t_list *el,void *adr,int32 mode,t_attribut_ref *ar)
{
	char	*value ;
	int32	lg = 0;

	switch(mode)
	{
		case	LIST_OBJ_ATTR :
			return	( el->l_obj == (char *)adr ) ;
		break ;
		case	LIST_BASIC_ATTR :
			value	= get_value(ar,&(el->l_val),&lg) ;
			if	( !value )
				return	0 ;
			switch	(ar->ar_type)
			{
#if	0
			case	XoVoid :
				return	( (void *)adr == (void *)value) ;
			break ;
#endif
			case	XoString :
				return	( !strcmp(value,(char *)adr) ) ;
			break ;
			case	XoBuffer :
				return	( !memcmp(value,(char *)adr,lg) ) ;
			break ;
			case	XoChar :
			case	XoBool :
			case	XoInt4B :
				return	( (int32)adr == *(int32 *)value) ;
			break ;
			default :
				XOERR("",0) ;
				return	0 ;
			break ;
			}
		break ;
		default :
			XOERR("",0) ;
			return	0 ;
		break ;
	}
	return	0 ;
}

static	int32	list_get_index(t_list_info *li, void *adr, int32 mode,t_attribut_ref *ar)
{
	int32	ok	= 0 ;
	int32	idx	= 0 ;
	t_list	*el ;

	el	= li->li_first ;
	while	(el && !ok)
	{
		if	( compare_elem(el,adr,mode,ar) )
			ok	= 1 ;
		else
		{
			el	= el->l_next ;
			idx++ ;
		}
	}
	if	( !ok )
		return	-713 ;
	li->li_curr	= el ;
	return	idx ;
}

/*
 *	fonction multiple sur liste
 *		retourne le nbre d'elements dans 1 attribut liste
 *		avance , recule ...
 */

/*DEF*/	int32	XoList(void *obj, int32 nrefattr, int32 action, void **adr)
{
	t_obj		*o ;
	t_objet_ref	*or ;
	u_attr		*a ;
	t_attribut_ref	*ar ;

	int32		mode ;
	int32		nattr ;

	t_list_info	*li ;

	nattr	= translate(obj,nrefattr,&o,&or,&a,&ar,&mode) ;
	if	( nattr < 0 )
	{
		XOERR("",nattr) ;
		return	( nattr - 500 ) ;
	}

	if	( mode != LIST_BASIC_ATTR && mode != LIST_OBJ_ATTR )
	{
		XOERR("",0) ;
		return	-100 ;
	}
#if	0
	if	( action == LIST_GET_INDEX && mode != LIST_OBJ_ATTR )
	{
		XOERR("GET_INDEX sur LIST_OBJ_ATTR",0) ;
		return	-200 ;
	}
#endif

	li	= a->a_li ;
	if	( !li )
		return	0 ;

	switch(action)
	{
	case	LIST_NB :
		return	li->li_nb ;
	break ;
	case	LIST_FIRST :
		li->li_curr	= li->li_first ;
		return	1 ;
	break ;
	case	LIST_LAST :
		li->li_curr	= li->li_last ;
		return	1 ;
	break ;
	case	LIST_NEXT :
		if	( li->li_curr && li->li_curr->l_next )
		{
			li->li_curr	= li->li_curr->l_next ;
			return	1 ;
		}
		return	0 ;
	break ;
	case	LIST_PREV :
		if	( li->li_curr && li->li_curr->l_prev )
		{
			li->li_curr	= li->li_curr->l_prev ;
			return	1 ;
		}
		return	0 ;
	break ;
	case	LIST_GET_CURR :
		*adr	= (char *)li->li_curr ;
		return	1 ;
	break ;
	case	LIST_SET_CURR :
		{
		int32	ok	= 0 ;
		t_list	*el ;

		el	= li->li_first ;
		while	(el && !ok)
		{
			if	( el == (t_list *)adr )
				ok	= 1 ;
			el	= el->l_next ;
		}
		if	( !ok )
			return	-213 ;
		li->li_curr	= (t_list *)adr ;
		return	1 ;
		}
	break ;
	case	LIST_GET_INDEX :
		return	list_get_index(li,(void *)adr,mode,ar) ;
#if	0
		{
		int32	ok	= 0 ;
		int32	idx	= 0 ;
		t_list	*el ;

		el	= li->li_first ;
		while	(el && !ok)
		{
			if	( el->l_obj == (char *)adr )
				ok	= 1 ;
			else
			{
				el	= el->l_next ;
				idx++ ;
			}
		}
		if	( !ok )
			return	-713 ;
		li->li_curr	= el ;
		return	idx ;
		}
#endif
	break ;
	case	LIST_SET_INDEX :
		{
		int32	idx ;
		t_list	*el ;

		idx	= (int32)adr ;
		if	( idx < 0 || idx >= li->li_nb )
		{
			return	-310 ;
		}
		el	= li->li_first ;
		while	( idx > 0 )
		{
			if	( !el )
				return	-300 ;
			el	= el->l_next ;
			idx-- ;
		}
		li->li_curr	= el ;
		return	1 ;
		}
	break ;
	default :
		XOERR("",0) ;
		return	-200 ;
	break ;
	}
	XOERR("",0) ;
	return	-500 ;
}

/*
 *	retourne un point32eur sur l'element courant d'une liste 
 *	en vue de sa sauvegarde
 */

/*DEF*/	void	*XoGetCurr(void *obj, int32 nrefattr)
{
	void	*adr ;
	int32	ret ;

	ret	= XoList(obj,nrefattr,LIST_GET_CURR,&adr) ;
	if	( ret > 0 )
		return	adr ;
	if	( ret < 0 )
		XOERR("",0) ;
	return	NULL ;
}


/*
 *	positionne l'elem courant obtenu par XoGetCurr()
 */

/*DEF*/	int32	XoSetCurr(void *obj, int32 nrefattr, void *adr)
/*DEF*/	    	      
/*DEF*/	     	          
/*DEF*/	    	      
{
	int32	ret ;

	ret	= XoList(obj,nrefattr,LIST_SET_CURR,(void **)adr) ;
	if	( ret < 0 )
		XOERR("",ret) ;
	return	ret ;
}

/*
 *	retourne le nbre d'elem dans une liste
 */

/*DEF*/	int32	XoNbInAttr(void *obj, int32 nrefattr)
{
	int32	ret ;

	ret	= XoList(obj,nrefattr,LIST_NB,NULL) ;
	if	( ret < 0 )
		XOERR("",ret) ;
	return	ret ;
}

/*DEF*/	int32	XoFirstAttr(void *obj, int32 nrefattr)
{
	int32	ret ;

	ret	= XoList(obj,nrefattr,LIST_FIRST,NULL) ;
	if	( ret < 0 )
		XOERR("",ret) ;
	return	ret ;
}

/*DEF*/	int32	XoLastAttr(void *obj, int32 nrefattr)
{
	int32	ret ;

	ret	= XoList(obj,nrefattr,LIST_LAST,NULL) ;
	if	( ret < 0 )
		XOERR("",ret) ;
	return	ret ;
}

/*DEF*/	int32	XoNextAttr(void *obj, int32 nrefattr)
{
	int32	ret ;

	ret	= XoList(obj,nrefattr,LIST_NEXT,NULL) ;
	if	( ret < 0 )
		XOERR("",ret) ;
	return	ret ;
}

/*DEF*/	int32	XoPrevAttr(void *obj, int32 nrefattr)
{
	int32	ret ;

	ret	= XoList(obj,nrefattr,LIST_PREV,NULL) ;
	if	( ret < 0 )
		XOERR("",ret) ;
	return	ret ;
}



/*
 *
 *	Recherche un objet ds une liste et retourne son index
 *	cet elem devient l'elem courant
 *
 */

/*DEF*/	int32	XoGetIndexInAttr(void *obj, int32 nrefattr, void *elem)
{
	int32	ret ;

	ret	= XoList(obj,nrefattr,LIST_GET_INDEX,(void **)elem) ;
	if	( ret < 0 )
		XOERR("",ret) ;
	return	ret ;
}

/*
 *
 *	Selectionne le ieme objet d'une liste
 *
 */

/*DEF*/	int32	XoSetIndexInAttr(void *obj, int32 nrefattr, int32 idx)
{
	int32	ret ;

	ret	= XoList(obj,nrefattr,LIST_SET_INDEX,(void **)idx) ;
	if	( ret < 0 )
		XOERR("",ret) ;
	return	ret ;
}







/*
 *	ajout d'un element dans 1 attribut liste 
 *	a droite ou a gauche de l'elem courant
 */

static	int32	xoaddinattr(void *obj, int32 nrefattr, void *add, int32 lg, int32 leftright)
    	      
     	          
    	      	/* XoInt4B ou XoChar ou XoString ou XoBuffer ou objet */
     	    	/* doit etre renseigne si XoBuffer */
     	           	/* RIGHT_ADD ou LEFT_ADD */
{
	t_obj		*o ;
	t_objet_ref	*or ;
	u_attr		*a ;
	t_attribut_ref	*ar ;

	int32		mode ;
	int32		nattr ;

	t_obj		*lko ;		/* linked objet */
	char		*value = NULL ;
	t_list		*elem ;

	int32		ret ;
	int32		modif ;

	nattr	= translate(obj,nrefattr,&o,&or,&a,&ar,&mode) ;
	if	( nattr < 0 )
	{
		XOERR("",nattr) ;
		return	( nattr - 500 ) ;
	}

	if	( mode != LIST_BASIC_ATTR && mode != LIST_OBJ_ATTR )
	{
		XOERR("",nattr) ;
		return	-100 ;
	}

	if	( mode == LIST_OBJ_ATTR )
	{
		lko	= (t_obj *)add ;
		if	( !lko )
		{
			XOERR("null",0) ;
			return	-101 ;
		}
		if	( lko->o_magic != O_MAGIC )
		{
			XOERR("magic",0) ;
			return	-102 ;
		}
		if	( ar->ar_type != XoVoidObj )
		{	/* ce n'est pas un obj de type Void => controle	*/
			if	( lko->o_num != ar->ar_type )
			{	/* type incorrect */
				XOERR("bad type linked obj",0) ;
				return	-103 ;
			}
		}
	}
	else
		value	= add ;


	if	( !a->a_li )
	{
/*
printf("premier acces liste\n") ;
*/
		a->a_li	= (t_list_info *)malloc(sizeof(t_list_info));
		if	( !a->a_li )
		{
			XOERR("",0) ;
			return	-200 ;
		}
/*
		memset	(a->a_li,0,sizeof(t_list_info)) ;
*/
		*(a->a_li)	= ListInfoNull ;

		a->a_li->li_num		= nrefattr ;
#ifdef	LI_MODE
		a->a_li->li_mode	= mode ;
#endif
	}
	else
	{
		if	( a->a_li->li_num != nrefattr )
		{
			XOERR("",0) ;
			return	-304 ;
		}
#ifdef	LI_MODE
		if	( a->a_li->li_mode != mode )
		{
			XOERR("",0) ;
			return	-305 ;
		}
#endif
	}

	elem	= (t_list *)malloc(sizeof(t_list)) ;
	if	( !elem )
	{
		XOERR("malloc()",errno) ;
		return	-201 ;
	}
	memset	(elem,0,sizeof(t_list)) ;
	if	( mode == LIST_BASIC_ATTR )
	{
		ret	= set_value(ar,&(elem->l_val),value,lg,&modif);
		if	( ret < 0 )
		{
			free	(elem) ;
			XOERR("",ret) ;
			return	( ret - 400 ) ;
		}
	}
	if	( mode == LIST_OBJ_ATTR )
	{
		elem->l_obj	= add ;
#ifdef	WITH_FATHER
		((t_obj *)add)->o_father	= obj ;
#endif
	}

	if	( leftright == LEFT_ADD )
		ins_in_list(a->a_li,elem) ;
	else
		add_in_list(a->a_li,elem) ;
	return	( a->a_li->li_nb) ;
}

/*
 *	ajout d'un element dans 1 attribut liste a droite de l'elem courant
 */

/*DEF*/	int32	XoAddInAttr(void *obj, int32 nrefattr, void *add, int32 lg)
{
	int32	ret ;

	ret	= xoaddinattr(obj,nrefattr,add,lg,RIGHT_ADD) ;
	if	( ret < 0 )
	{
		XOERR("",ret) ;
	}
	O_SET_MODIFIED(((t_obj *)obj)) ;
	return	ret ;
}

/*
 *	ajout d'un element dans 1 attribut liste a gauche de l'elem courant
 */

/*DEF*/	int32	XoInsInAttr(void *obj, int32 nrefattr, void *add, int32 lg)
{
	int32	ret ;

	ret	= xoaddinattr(obj,nrefattr,add,lg,LEFT_ADD) ;
	if	( ret < 0 )
	{
		XOERR("",ret) ;
	}
	O_SET_MODIFIED(((t_obj *)obj)) ;
	return	ret ;
}


/*
 *	sup l'element courant  , le courant devient l'elem de gauche
 */

/*DEF*/	int32	XoSupInAttr(void *obj, int32 nrefattr)
{
	t_obj		*o ;
	t_objet_ref	*or ;
	u_attr		*a ;
	t_attribut_ref	*ar ;
	t_list		*elem ;

	int32		mode ;
	int32		nattr ;

	nattr	= translate(obj,nrefattr,&o,&or,&a,&ar,&mode) ;
	if	( nattr < 0 )
	{
		XOERR("",nattr) ;
		return	( nattr - 500 ) ;
	}

	if	( mode != LIST_BASIC_ATTR && mode != LIST_OBJ_ATTR )
	{
		XOERR("",0) ;
		return	-100 ;
	}

	if	( !a->a_li )
	{	/* non initialisee */
		return	-101 ;
	}
	if	( a->a_li->li_num != nrefattr )
	{
		XOERR("",0) ;
		return	-102 ;
	}
#ifdef	LI_MODE
	if	( a->a_li->li_mode != mode )
	{
		XOERR("",0) ;
		return	-103 ;
	}
#endif

	elem	= sup_in_list(a->a_li) ;
	if	( !elem )
	{
		XOERR("",0) ;
		return	-104 ;
	}

	if	( mode == LIST_BASIC_ATTR )
	{
		free_value(&(elem->l_val),ar);
	}
#if	0
	else
	{
		XoFree	(elem->l_ob,1) ;
	}
#endif
	free	(elem) ;
	O_SET_MODIFIED(o) ;
	if	( a->a_li->li_nb <= 0 )
	{
		free	(a->a_li) ;
		a->a_li	= NULL ;
		return	0 ;
	}
	return	( a->a_li->li_nb) ;
}



/*
 *	ajout d'un element dans 1 attribut liste a droite de l'elem current
 *	pour tous les objets du type numobj 
 */

/*DEF*/	int32	XoAddInAllAttr(int32 numobj, int32 nrefattr, char *value, int32 lg)
{
	int32	nb ;
	int32	ret ;
	void	*obj ;

	nb	= 0 ;
	obj	= XoFirstObj	(numobj) ;
	while	( obj )
	{
		ret	= XoAddInAttr	(obj,nrefattr,value,lg) ;
		if	( ret < 0 )
		{
			XOERR("XoAddInAllAttr()",ret) ;
			return	ret ;
		}
		nb++ ;
		obj	= XoNextObj	(numobj) ;
	}
	return	nb ;
}



/*
 *	copie d'une occurence d'objet reccursive
 */

static	void	*xonewcopy(void *obj, int32 reccur,int32 strict)
{
	t_obj		*o ;
	t_objet_ref	*or ;
	char		*objret ;

	t_attribut_ref	*ar ;

	int32		mode ;
	int32		nb ;
	int32		ret ;
	int32		anum ;
	int32		atype ;
	int32		i ;

	char		*value ;
	int32		lg ;

	o	= (t_obj *)obj ;

	if	( !o )
	{
		XOERR("null",0) ;
		return	NULL ;
	}

	if	( o->o_magic != O_MAGIC )
	{
		XOERR("magic",o->o_magic) ;
		return	NULL ;
	}

	or	= o->o_or ;
	if	( or->or_num != o->o_num )
	{
		XOERR("ref failure",o->o_num) ;
		return	NULL ;
	}

	objret	= XoNew(o->o_num) ;
	if	( !objret )
	{
		XOERR("XoNew() failed in xonewcopy()",o->o_num) ;
		return	NULL ;
	}

	if	( strict )
		XoClear(objret) ;

	/*	attr liste */
	ret	= 1 ;
	nb	= or->or_nb_attr ;
	for	( i = 0 ; i < nb && ret >= 0 ; i++ )
	{
		ar	= &(or->or_tab_attr[i]) ;
		mode	= ar->ar_mode ;
		anum	= ar->ar_num ;
		atype	= ar->ar_type ;
		switch	(mode)
		{
	 		case	BASIC_ATTR :
				value	= XoGetAttr(obj,anum,&lg) ;
				if	( value )
				{
					ret	= 
					presetattr(objret,anum,value,lg,atype);
					if	( ret < 0 )
						XOERR("",ret) ;
				}
				else
				{
					if	( strict )
					{
						XoUnSetAttr(objret,anum) ;
					}
				}
			break ;
			case	OBJ_ATTR :
				if	( !reccur )		continue ;
				value	= XoGetAttr(obj,anum,&lg) ;
				if	( value )
				{
					value=xonewcopy(value,reccur,strict) ;
					if	( value )
					{
						ret = 
					presetattr(objret,anum,value,lg,atype);
						if	( ret < 0 )
							XOERR("",ret) ;
					}
					else
					{
						XOERR("",0) ;
						ret	= -1001 ;
					}
				}
				else
				{
					if	( strict )
					{
						XoUnSetAttr(objret,anum) ;
					}
				}
			break ;
			case	LIST_BASIC_ATTR :
				ret 	=copy_list_basic(obj,objret,anum,atype);
				if	( ret < 0 )
				{
					XOERR("",ret) ;
				}
			break ;
			case	LIST_OBJ_ATTR :
				if	( !reccur )		continue ;
				ret=copy_list_obj(obj,objret,anum,atype,strict);
				if	( ret < 0 )
				{
					XOERR("",ret) ;
				}
			break ;
			default :
				XOERR("mode ???",mode) ;
			break ;
		}
	}

	if	( ret < 0 )
	{
		XOERR("",ret) ;
		XoFree	(objret,1) ;
		return	NULL ;
	}
#ifdef	WITH_FATHER
	((t_obj *)objret)->o_father	= o->o_father ;
#endif
	return	objret ;
}

/*
 *	copie d'une occurence d'objet reccursive
 */

/*DEF*/	void	*XoNewCopy(void *obj, int32 reccur)
{
	return	xonewcopy(obj,reccur,0) ;
}

/*
 *	copie stricte d'une occurence d'objet reccursive
 */

/*DEF*/	void	*XoNewStrictCopy(void *obj, int32 reccur)
{
	return	xonewcopy(obj,reccur,1) ;
}

int32	copy_list_basic(char *src, char *dst, int32 anum, int32 atype)
{
	int32	pos ;
	int32	ret ;
	void	*value ;
	int32	nb ;
	int32	lg ;

	nb	= 0 ;
	pos	= XoFirstAttr(src,anum) ;
	while	( pos > 0 )
	{
		value 	= XoGetAttr(src,anum,&lg) ;
		if	( value )
		{
/*
			ret	= XoAddInAttr(dst,anum,value,lg);
*/
			ret	= preaddattr(dst,anum,value,lg,atype);
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
		pos	= XoNextAttr(src,anum) ;
		nb++ ;
	}
	if	( pos < 0 )
	{
		XOERR("",0) ;
		return	-3 ;
	}
	return	nb ;
}

static	int32	copy_list_obj(char *src,char *dst,int32 anum,int32 atype,int32 strict)
{
	int32	pos ;
	int32	ret ;
	void	*value ;
	int32	nb ;
	int32	lg ;

	nb	= 0 ;
	pos	= XoFirstAttr(src,anum) ;
	while	( pos > 0 )
	{
		value 	= XoGetAttr(src,anum,&lg) ;
		if	( value )
		{
			value	= xonewcopy(value,1,strict) ;
			if	( value )
			{
/*
				ret	= XoAddInAttr(dst,anum,value,lg) ;
*/
				ret	=preaddattr(dst,anum,value,lg,atype);
				if	( ret < 0 )
				{
					XOERR("",ret) ;
					return	-1 ;
				}
			}
			else
			{
				XOERR("",0) ;
				return	-2 ;
			}
		}
		else
		{	/* postionnement correct et pas de valeur !!! */
			XOERR("",0) ;
			return	-3 ;
		}
		pos	= XoNextAttr(src,anum) ;
		nb++ ;
	}
	if	( pos < 0 )
	{
		XOERR("",0) ;
		return	-4 ;
	}
	return	nb ;
}



int32	presetattr(char *obj, int32 anum, char *value, int32 lg, int32 type)
{
	int32	ret ;
	char	c ;
	int32	l ;

	switch	(type)
	{
		case	XoChar :
			c	= *(char *)value ;
			ret	= XoSetAttr(obj,anum,(void *)(int32)c,lg) ;
		break ;
		case	XoInt4B :
		case	XoBool :
		case	XoDate :
			l	= *(int32 *)value ;
			ret	= XoSetAttr(obj,anum,(void *)l,lg) ;
		break ;
		case	XoString :
		case	XoBuffer :
#if	0
		case	XoVoid :
#endif
		default :
			ret	= XoSetAttr(obj,anum,value,lg) ;
		break ;
	}

	if	( ret < 0 )
		XOERR("",ret) ;

	return	ret ;
}



int32	preaddattr(char *obj, int32 anum, char *value, int32 lg, int32 type)
{
	int32	ret ;
	char	c ;
	int32	l ;

	switch	(type)
	{
		case	XoChar :
			c	= *(char *)value ;
			ret	= XoAddInAttr(obj,anum,(void *)(int32)c,lg) ;
		break ;
		case	XoInt4B :
		case	XoBool :
		case	XoDate :
			l	= *(int32 *)value ;
			ret	= XoAddInAttr(obj,anum,(void *)(int32)l,lg) ;
		break ;
		case	XoString :
#if	0
		case	XoVoid :
#endif
		case	XoBuffer :
		default :
			ret	= XoAddInAttr(obj,anum,value,lg) ;
		break ;
	}

	if	( ret < 0 )
		XOERR("",ret) ;

	return	ret ;
}

#ifdef	WITH_FATHER
/*
 *	retourne l'objet pere
 */

/*DEF*/	int32	XoGetFather(void *obj,void **father)
{
	t_obj	*o ;

	*father	= NULL ;
	o	= (t_obj *)obj ;
	if	( !o )
	{
		XOERR("null",0) ;
		return	-1 ;
	}

	if	( o->o_magic != O_MAGIC )
	{
		XOERR("magic",o->o_magic) ;
		return	-2 ;
	}

	*father	= o->o_father ;
	if	( *father )
		return	1 ;
	return	0 ;
}
#endif

#ifdef	WITH_FATHER
/*
 *	retourne l'objet ancetre
 */

/*DEF*/	int32	XoGetRootFather(void *obj,void **rootfather)
{
	t_obj	*o ;
	void	*father ;

	*rootfather	= NULL ;
	o	= (t_obj *)obj ;
	if	( !o )
	{
		XOERR("null",0) ;
		return	-1 ;
	}

	if	( o->o_magic != O_MAGIC )
	{
		XOERR("magic",o->o_magic) ;
		return	-2 ;
	}

	while	( XoGetFather(obj,&father)>0 )
	{
		if	( father )
		{
			*rootfather	= father ;
			obj		= father ;
		}
		else
			break ;
	}

	if	( *rootfather )
		return	1 ;
	return	0 ;
}
#endif

/*DEF*/	int32	XoIsObix(void *obj)
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
		XOERR("magic",o->o_magic) ;
		return	-2 ;
	}

	or	= o->o_or ;
	if	( or->or_num != o->o_num )
	{
		XOERR("ref failure",o->o_num) ;
		return	-3 ;
	}

	return	O_IS_OBIX(o);
}

static	int32	xomodified(void *obj, int32 reccur, int32 action)
{
	int32	ret ;
	int32	type ;
	int32	nbattr ;
	char	*nameobj ;
	int32	a ;

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
	switch	(action)
	{
		case	IS_MODIFIED :
			ret	= O_IS_MODIFIED(((t_obj *)obj)) ;
		break ;
		case	SET_MODIFIED :
			O_SET_MODIFIED(((t_obj *)obj)) ;
			ret	= 0 ;
		break ;
		case	UNSET_MODIFIED :
			O_UNSET_MODIFIED(((t_obj *)obj)) ;
			ret	= 0 ;
		break ;
		default :
			XOERR("",0) ;
			return	-4 ;
		break ;
	}

	/* non  reccur. c'est fini */
	if	( !reccur )	
		return	ret ;

	/* reccur. mais celui-la est deja trouve modifie */
	if	( ret > 0 && action == IS_MODIFIED )
		return	ret ;

	ret	= 0 ;
	for	( a = 0 ; a < nbattr ; a++ )
	{
		int32	numattr ;
		int32	mode ;
		int32	lg ;
		int32	pos ;
		void	*value ;
		void	*svPos ;

		numattr	= type + a + 1 ;
		mode	= XoModeAttr(numattr) ;
		if	( mode < 0 )
		{
			XOERR("",0) ;
			return	-5 ;
		}
		switch	(mode)
		{
			case	BASIC_ATTR :
			case	LIST_BASIC_ATTR :
				continue ;
			break ;
			case	OBJ_ATTR :
				value	= XoGetAttr	(obj,numattr,&lg) ;
				if	( !value )	continue ;
				ret	= xomodified(value,reccur,action) ;
				if	( ret < 0 )
				{
					XOERR("",ret) ;
					return	ret ;
				}
			break ;
			case	LIST_OBJ_ATTR :
				svPos	= XoGetCurr(obj,numattr) ;
				pos	= XoFirstAttr(obj,numattr) ;
				while	( pos > 0 )
				{
					value	= XoGetAttr(obj,numattr,&lg) ;
					if	( !value )
					{
						XOERR("",0) ;
						return	-6 ;
					}
					ret = xomodified(value,reccur,action) ;
					if	( ret < 0 )
					{
						XOERR("",ret) ;
						return	ret ;
					}
					if	(ret>0 && action==IS_MODIFIED)
						break ;
					pos	= XoNextAttr(obj,numattr) ;
				}
				XoSetCurr(obj,numattr,svPos) ;
			break ;
		}
		if	( ret > 0 && action == IS_MODIFIED )
			break ;
	}
	return	ret ;
}



/*DEF*/	int32	XoIsModified(void *obj, int32 reccur)
{
	int32	ret ;

	ret	= xomodified(obj,reccur,IS_MODIFIED) ;
	if	( ret < 0 )
	{
		XOERR("XoIsModified",ret) ;
	}
	return	ret ;
}

/*DEF*/	int32	XoSetModified(void *obj, int32 reccur)
{
	int32	ret ;

	ret	= xomodified(obj,reccur,SET_MODIFIED) ;
	if	( ret < 0 )
	{
		XOERR("XoSetModified",ret) ;
	}
	return	ret ;
}

/*DEF*/	int32	XoUnSetModified(void *obj, int32 reccur)
{
	int32	ret ;

	ret	= xomodified(obj,reccur,UNSET_MODIFIED) ;
	if	( ret < 0 )
	{
		XOERR("XoUnSetModified",ret) ;
	}
	return	ret ;
}


/*DEF*/	char	*XoAttrToPtrAscii(void *obj, int32 numAttr, char *str)
{
	void	*val ;
	int32	lg ;

	if(!obj)
	{
		XOERR("null obj",0);
		return	NULL ;
	}
	if(!(val=XoGetAttr(obj,numAttr,&lg)))
	{
		return	NULL ;
	}
	switch(XoTypeAttr(numAttr))
	{
	case XoInt4B:	sprintf(str,"%ld",*(long *)val);break;
	case XoString:	return	val ;			break;
	case XoBool:	sprintf(str,"%ld",*(long *)val);break;
	case XoChar:	sprintf(str,"%c",*(char *)val);	break;
	case XoBuffer:	
		{
			int	i ;
			unsigned char	*v ;
			char	*pt ;

			v	= (unsigned char *)val ;
			*str	= '\0' ;
			pt	= str ;
			for	( i = 0 ; i < lg ; i++ )
			{
				sprintf	(pt,"%02x",v[i]) ;
				pt	+= 2 ;
			}
			*pt	= '\0' ;
		}
	break;
	default:
//		XOERR("bad type",0);
		return	NULL ;
	break ;
	}
	return str;
}

static	int32	xoasciitoattr(void *obj, int32 numAttr, char *str)
{
	if(!obj)
	{
		XOERR("null obj",0);
		return	-1 ;
	}
	if( !str )
	{
		XoUnSetAttr(obj,numAttr);
		return 0;
	}
	switch(XoTypeAttr(numAttr))
	{
	case XoBool:
	case XoInt4B:
		XoSetAttr(obj,numAttr,(void *)(int32)xoatoi(str),0);
		break;
	case XoString:
		XoSetAttr(obj,numAttr,str,0);
		break;
	case XoChar:
		XoSetAttr(obj,numAttr,(void *)(int32)(*str),0);
		break;
	case XoBuffer:
		{
			unsigned	char	*pt ;
			int32		lg	= strlen(str) ;
			int		ret ;
			int XoAsc2Hex( unsigned char *src, unsigned char *dest);

			if	( !lg )	{ XOERR("",0); return	-4 ; }
			pt	= (unsigned char *)malloc(lg) ;
			if	( !pt )	{ XOERR("",0); return	-5 ; }

			if	((ret=XoAsc2Hex((unsigned char *)str,pt)) < 0 )
			{
				XOERR("asc -> hexa error format",ret) ;
				free	(pt) ;
				return	-6 ;
			}
			XoSetAttr(obj,numAttr,pt,ret);
			free	(pt) ;
			break ;
		}
	default:
		XOERR("bad type",0);
		return	-3 ;
	break ;
	}
	return 0;
}

/*DEF*/	int32	XoAsciiToAttr(void *obj, int32 numAttr, char *str)
{
	if(!obj)
	{
		XOERR("null obj",0);
		return	-1 ;
	}
	if( str && *str == 0)
	{
		str	= NULL ;
	}
	return	xoasciitoattr(obj,numAttr,str) ;
}

/*DEF*/	int32	XoAsciiToAttr2(void *obj, int32 numAttr, char *str)
{
	if(!obj)
	{
		XOERR("null obj",0);
		return	-1 ;
	}
	return	xoasciitoattr(obj,numAttr,str) ;
}
