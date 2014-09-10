
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


/*
    ident	%W% %E%
*/

/*  RTL Technologies */
/*  Keyxord             : %W% */
/*  Current module name : %M% */
/*  SCCS file name      : %F% */
/*  SID                 : %I% */
/*  Delta Date          : %E% */
/*  Current date/time   : %D%,%T% */




/*
 *	si on met define D_LG on peut gerer le type XoBuffer
 *	sinon on gagne 10% de place memoire au prix de qques strlen()
 */
#define	D_LG

/*
 *	si on met define LI_MODE on dispose d'un controle supplementaire
 *	sinon on gagne presque rien
 */
#define	LI_MODE

/*
char		*xomalloc() ;
#define		malloc(lg)	xomalloc((lg))
*/


t_objet_ref	*XoGetRefObj(int32 obj) ;
t_attribut_ref	*XoGetRefAttr(int32 num) ;


#define		O_MAGIC		(141263)

/*
 *	requetes d'acces aux listes
 */
#define		LIST_NB 	0
#define		LIST_FIRST 	1
#define		LIST_LAST 	2
#define		LIST_NEXT 	3
#define		LIST_PREV 	4
#define		LIST_GET_CURR 	5
#define		LIST_SET_CURR 	6
#define		LIST_GET_INDEX 	7
#define		LIST_SET_INDEX 	8

/*
 *	t_dyn peut contenir :
 *	1)	des chaines de taille non limitee d_pt point32e dans malloc
 *	2)	des buffers de taille non limitee d_pt point32e dans malloc
 *	3)	des chaines de taille limtee d_pt point32e dans l'obj auquel
 *		il ne faut pas faire de free !!!
 */

typedef	struct				/* 8 octets			*/
{
	char	*d_pt ;			/* pointeur sur une zone malloc */
#ifdef	D_LG
	int32	d_lg ;			/* longueur de d_pt		*/
#endif
}	t_dyn ;


#define		LG_MAX_ATTR		(sizeof(t_dyn)-1)


/*
 *	u_val contient tous les types de valeurs possibles
 *		int32			v_l
 *		char			v_c
 *		petite chaine		v_str	< LG_MAX_ATTR
 *		les autres valeurs	v_sdyn
 */

typedef	union				/* 8 octets			*/
{
	int32		v_l ;		/* c'est un int32		*/
	unsigned	char	v_c ;	/* c'est un char		*/
	char	v_str[LG_MAX_ATTR+1] ;	/* c'est un char * de moins 7	*/
	t_dyn		v_sdyn ;	/* char * , ou buffer		*/
}	u_val ;

#define	v_dyn	v_sdyn.d_pt		/* raccourci			*/
#define	v_void	v_sdyn.d_pt		/* raccourci			*/

#ifdef	D_LG
#define	V_LG(v)	( (v)->v_lg )
#define	v_lg	v_sdyn.d_lg		/* raccourci			*/
#else
#define	V_LG(v)	( strlen((v)->v_dyn) + 1 )
#endif



typedef	struct				/* 8 octets			*/
{
	u_val	ba_val ;		/* sa valeur			*/
}	t_basic_attr ;



/*
 *	un element de liste peut etre :
 *	1)	un attribut simple		=> l_val
 *	2)	un point32eur sur un objet	=> l_obj
 */

typedef	struct	s_list			/* 16 octets			*/
{
	struct	s_list	*l_next ;
	struct	s_list	*l_prev ;
	u_val		l_val ;
}	t_list ;

#define			l_obj	l_val.v_dyn

typedef	struct				/* 24 octets			*/
{
	t_list	*li_first ;
	t_list	*li_last ;
	t_list	*li_curr ;		/* current objet		*/
	int32	li_nb ;			/* nbre dans la liste		*/
	int32	li_num ;		/* num reference d'attribut	*/
#ifdef	LI_MODE
	int32	li_mode ;		/* son mode			*/
#endif
}	t_list_info ;

typedef	union				/* 4 octets			*/
{
	t_basic_attr	*a_ba ;		/* alloue dans o_tab_ba		*/
	char		*a_ob ;		/* (t_obj *) c'est un objet	*/
	t_list_info	*a_li ;		/* c'est une liste		*/
}	u_attr ;

typedef	struct	s_obj			/* 44 octets			*/
{
	int32		o_magic ;
	int32		o_num ;		/* num reference d'objet	*/
	t_objet_ref	*o_or ;		/* pointeur sur obj reference	*/
	char		*o_buf_str ;	/* buffer pour chaine a taille fixe */
	t_basic_attr	*o_tab_ba ;	/* alloue en 1 fois nbre de basic attr*/
	u_attr		*o_tab_attr ;	/* table des attributs		*/
	struct s_obj	*o_prev ;	/* objet precedent		*/
	struct s_obj	*o_next ;	/* objet suivant		*/
	char		*o_usr_ptr ;	/* pt sur donnee utilisateur	*/
	int32		(*o_free_usr)();/* destructeur usr_ptr		*/
#ifdef	WITH_FATHER
	char		*o_father ;	/* objet pere ou NULL		*/
#endif
	char		*o_buffer;	/* contient o_buf,o_tabba,o_tab_attr*/
	unsigned char	o_flag ;
}	t_obj ;


#define		O_IS_MODIFIED(o)	(((o)->o_flag&O_FMODIFIED)==O_FMODIFIED)
#define		O_SET_MODIFIED(o)	((o)->o_flag |= O_FMODIFIED)
#define		O_UNSET_MODIFIED(o)	((o)->o_flag &= ~O_FMODIFIED)
#define		O_IS_OBIX(o)		(((o)->o_flag&O_FOBIX)==O_FOBIX)
#define		O_SET_OBIX(o)		((o)->o_flag |= O_FOBIX)


/*
 *	attribut de taille fixe prealloue dans l'objet pere
 */
#define		AR_PRE_ALLOC(ar) 	( \
					   ((ar)->ar_type == XoString \
					|| (ar)->ar_type == XoBuffer) \
					&& !IS_A_LIST((ar)) \
					&& (ar)->ar_lg_max != -1 \
					&& (ar)->ar_lg_max > LG_MAX_ATTR\
					&& (ar)->ar_lg_max <= 512 )

/*
 *	petite chaine qui tient dans une u_val ie NICE_STRING
 */
#define		AR_NICE_STRING(ar)	( (ar)->ar_type == XoString \
					&& (ar)->ar_lg_max != -1 \
					&& (ar)->ar_lg_max <= LG_MAX_ATTR )
