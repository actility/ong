
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


#ifndef		XOAPI_H
#define		XOAPI_H
#define		XO_2

#ifndef		DOS
#include	<stdio.h>
#endif

#include	"rtlbase.h"

#define		XOENTERCS()	XoEnterCS(__FILE__,__LINE__)
#define		XOLEAVECS()	XoLeaveCS(__FILE__,__LINE__)


#ifdef		DOS
#include	<io.h>
#define		XO_FIC_BINARY	O_BINARY
#define		XO_CAR_SPEC	'\\'
#else
#define		XO_FIC_BINARY	0x0
#define		XO_CAR_SPEC	'/'
#endif

#define		XO_FMAGIC	(0xDAFADAFA)
#define		XO_VERSION0	(0x43)
#define		XO_VERSION	(XO_VERSION0+1)

#define		XO_FMT_STD_XML	(0)
#define		XO_FMT_STD_BIN	(1)

#define		XO_HEADER_SIZE	12	// magic+(flags+version+format)+length


#define		XOERR(s,p)	xoerr(xo_file,__LINE__,(s),(p))

#define		IO_FILE		1
#define		IO_MEMORY	2
#define		IO_SIZE		3
#define		IO_ALLOC	4

#define		XO_SHARED_DICO		1
#define		XO_COMPRESS_0		2
#define		XO_COMPRESS_LZ77	4

#define		XOML_MAX_NAMESPACE	32
#define		XOML_MAX_LEVEL		32
#define		XOML_MAX_ATTR		32
#define		XOML_TAG_LG		64

#define		XOML_BALISE_ATTR	"name$"		// element name
#define		XOML_PSEUDO_ATTR	"value$"	// xml content
#define		XOML_TYVOID_ATTR	"xsi:type"	// dynamic typing

#define		XOML_WRAPPE_ATTR	"wrapper$"	// for obix objects
#define		XOML_CUSTOM_ATTR	"custom$"	// for obix objects
#define		XOML_CUSTOM_ATTR_NAME	"name"		// for obix objects
#define		XOML_CUSTOM_ATTR_VALUE	"value"		// for obix objects

#define		XOML_PARSE_ONLY		1
#define		XOML_PARSE_INTERACTIF	2
#define		XOML_PARSE_TRACE	4
#define		XOML_PARSE_TARGET	8
#define		XOML_PARSE_OBIX		16
#define		XOML_PARSE_NATIVENS	32	// native namespaces assumed
#define		XOML_PARSE_RETRYATTR	64
#define		XOML_PARSE_AUTOWRAPP	128

#define		XO_XOR_EXT		".xor"
#define		XO_XNS_EXT		".xns"
#define		XO_DIC_EXT		".xdi"


#ifdef		XO_AVANT_2_1
#include	"xooldpr.h"
#endif

/*
 *	type d'attribut pour les sauvegarde
 *	ne jamais modifier l'ordre de cet enum
 */

enum
{
	CFG_OBJ,
	CFG_INT,
	CFG_BUF,
	CFG_LSTOBJ,
	CFG_LSTINT,
	CFG_LSTBUF,
	CFG_INT_UNSET,
	CFG_BUF_UNSET
};

/* 
 *	acces au mode de sauvegarde
 */

#define	GET_MDSV_TYPE(mode)	(0x0000FFFF&mode)

#define	IS_MDSV_SAVE_UNSET(md)	((md)==CFG_INT_UNSET||(md)==CFG_BUF_UNSET)


#define		BASIC_OBJ	0
#define		SINGLE_OBJ	1
#define		STRUCT_OBJ	2

#define		BASIC_ATTR		1
#define		LIST_BASIC_ATTR		2
#define		OBJ_ATTR		3
#define		LIST_OBJ_ATTR		4


#define	AR_FLIST		1
#define	AR_FPREALLOC		2
#define	AR_FNICE_STR		4
#define	AR_FNO_SAVE		8
#define	AR_FSAVE_UNSET		16
#define	AR_FEMBATTR		32	// xml attr embeded
#define	AR_FWRAPHIDDEN		64	// xml attr wrapped
#define	AR_FSHDICO		128	// attr shidco

#define	IS_A_LIST(ar)	 ( ((ar)->ar_flag&AR_FLIST) == AR_FLIST )
#define	IS_PREALLOC(ar)	 ( ((ar)->ar_flag&AR_FPREALLOC) == AR_FPREALLOC)
#define	IS_NICE_STR(ar)	 ( ((ar)->ar_flag&AR_FNICE_STR) == AR_FNICE_STR)
#define	IS_NO_SAVE(ar)	 ( ((ar)->ar_flag&AR_FNO_SAVE) == AR_FNO_SAVE)
#define	IS_SAVE_UNSET(ar) ( ((ar)->ar_flag&AR_FSAVE_UNSET) == AR_FSAVE_UNSET)
#define	IS_EMBATTR(ar) ( ((ar)->ar_flag&AR_FEMBATTR) == AR_FEMBATTR)
#define	IS_WRAPHIDDEN(ar) ( ((ar)->ar_flag&AR_FWRAPHIDDEN) == AR_FWRAPHIDDEN)
#define	IS_SHDICO(ar) ( ((ar)->ar_flag&AR_FSHDICO) == AR_FSHDICO)

#define		O_FMODIFIED		1
#define		O_FOBIX			2

#define		BaseObj		0x10000

#define		NUM_OBJ_TO_INDICE(n)	( ((n)>>16) - 1 )

#define		NUM_ATTR_TO_INDICE(n)	( ((n)&0x0000FFFF) - 1 )

#define		NUM_ATTR_TO_NUM_OBJ(n)	( (n)&0xFFFF0000 )

#define		IS_NUM_OBJ(n)		( ((n)&0x0000FFFF) == 0 )


#define		BaseNumObj	50

#define		BaseUserObj(n)	( BaseObj * ( BaseNumObj + (n) ) )

/*
 *	liste des objets internes
 */

#define		XoInt4B		( BaseObj	*	1 )
#define		XoChar		( BaseObj	*	2 )
#define		XoString	( BaseObj	*	3 )
#define		XoBuffer	( BaseObj	*	4 )

#define		XoHeader	( BaseObj	*	5 )

#define		XoAttrRef	( BaseObj	*	6 )
#define		XoObjRef	( BaseObj	*	7 )
#define		XoCfg		( BaseObj	*	8 )


#define		XoDate		( BaseObj	*	9 )
#define		XoBool		( BaseObj	*	10 )
#define		XoVoidObj	( BaseObj	*	11 )
#define		XoReserved12	( BaseObj	*	12 )

#define		XoReserved13	( BaseObj	*	13 )
#define		XoReserved14	( BaseObj	*	14 )
#define		XoReserved15	( BaseObj	*	15 )
#define		XoReserved16	( BaseObj	*	16 )
#define		XoReserved17	( BaseObj	*	17 )
#define		XoReserved18	( BaseObj	*	18 )
#define		XoReserved19	( BaseObj	*	19 )
#define		XoReserved20	( BaseObj	*	20 )
#define		XoReserved21	( BaseObj	*	21 )
#define		XoReserved22	( BaseObj	*	22 )
#define		XoReserved23	( BaseObj	*	23 )
#define		XoReserved24	( BaseObj	*	24 )
#define		XoReserved25	( BaseObj	*	25 )
#define		XoReserved26	( BaseObj	*	26 )
#define		XoReserved27	( BaseObj	*	27 )
#define		XoReserved28	( BaseObj	*	28 )
#define		XoReserved29	( BaseObj	*	29 )

#define		XoReserved30	( BaseObj	*	30 )
#define		XoReserved31	( BaseObj	*	31 )
#define		XoReserved32	( BaseObj	*	32 )
#define		XoReserved33	( BaseObj	*	33 )
#define		XoReserved34	( BaseObj	*	34 )
#define		XoReserved35	( BaseObj	*	35 )
#define		XoReserved36	( BaseObj	*	36 )
#define		XoReserved37	( BaseObj	*	37 )
#define		XoReserved38	( BaseObj	*	38 )
#define		XoReserved39	( BaseObj	*	39 )
#define		XoReserved40	( BaseObj	*	40 )
#define		XoReserved41	( BaseObj	*	41 )
#define		XoReserved42	( BaseObj	*	42 )
#define		XoReserved43	( BaseObj	*	43 )
#define		XoReserved44	( BaseObj	*	44 )
#define		XoReserved45	( BaseObj	*	45 )
#define		XoReserved46	( BaseObj	*	46 )
#define		XoReserved47	( BaseObj	*	47 )
#define		XoReserved48	( BaseObj	*	48 )
#define		XoReserved49	( BaseObj	*	49 )


/*
 *	attributs de l'objet Header
 */

#define		XoHeaderMagic		( XoHeader	+	0 + 1 )
#define		XoHeaderVersion		( XoHeader	+	1 + 1 )
#define		XoHeaderDate		( XoHeader	+	2 + 1 )
#define		XoHeaderDesc		( XoHeader	+	3 + 1 )

/*
 *	attributs de l'objet AttrRef	= t_attribut_ref
 */

#define		XoAttrRefNum		( XoAttrRef	+	0 + 1 )
#define		XoAttrRefType		( XoAttrRef	+	0 + 2 )
#define		XoAttrRefLgMax		( XoAttrRef	+	0 + 3 )
#define		XoAttrRefList		( XoAttrRef	+	0 + 4 )
#define		XoAttrRefName		( XoAttrRef	+	0 + 5 )
#define		XoAttrRefTypeName	( XoAttrRef	+	0 + 6 )
#define		XoAttrRefValue		( XoAttrRef	+	0 + 7 )
#define		XoAttrRefDefault	( XoAttrRef	+	0 + 8 )
#define		XoAttrRefHelpLine	( XoAttrRef	+	0 + 9 )
#define		XoAttrRefHelpFile	( XoAttrRef	+	0 + 10 )
#define		XoAttrRefNoSave		( XoAttrRef	+	0 + 11 )
#define		XoAttrRefSaveUnset	( XoAttrRef	+	0 + 12 )
#define		XoAttrRefEmbAttr	( XoAttrRef	+	0 + 13 )
#define		XoAttrRefWrapHidden	( XoAttrRef	+	0 + 14 )
#define		XoAttrRefShDico		( XoAttrRef	+	0 + 15 )

/*
 *	attributs de l'objet ObjRef	= t_objet_ref
 */

#define		XoObjRefNum		( XoObjRef	+	0 + 1 )
#define		XoObjRefType		( XoObjRef	+	0 + 2 )
#define		XoObjRefNbBasic		( XoObjRef	+	0 + 3 )
#define		XoObjRefNbObj		( XoObjRef	+	0 + 4 )
#define		XoObjRefNbListBas	( XoObjRef	+	0 + 5 )
#define		XoObjRefNbListObj	( XoObjRef	+	0 + 6 )
#define		XoObjRefListAttr	( XoObjRef	+	0 + 7 )
#define		XoObjRefNbAttr		( XoObjRef	+	0 + 8 )
#define		XoObjRefName		( XoObjRef	+	0 + 9 )
#define		XoObjRefRoot		( XoObjRef	+	0 + 10 )
#define		XoObjRefStExt		( XoObjRef	+	0 + 11 )
#define		XoObjRefFrom		( XoObjRef	+	0 + 12 )
#define		XoObjRefAlias		( XoObjRef	+	0 + 13 )
#define		XoObjRefAliasHas	( XoObjRef	+	0 + 14 )

/*
 *	attributs de l'objet ObjCfg
 */

#define		XoCfgListObjRef		( XoCfg	+	0 + 1 )

/*
 *	ne jamais modifier l'ordre des champs dans cette structure
 *	les 6 premiers champs sont initialises dans apitab.c
 *	dans le cas des objets utilisateurs ar_type est calcule
 *	a partir de ar_type_name
 */

typedef	struct
{
	/* 	initialises dans apitab.c */
	int32			ar_num ;
	int32			ar_type ;
	int32			ar_lg_max ;
	int32			ar_flag ;
	char			*ar_name ;
	char			*ar_type_name ;


	char			*ar_value ;
	char			*ar_default ;
	char			*ar_help_line ;
	char			*ar_help_file ;

	/*
	 *		calcules
	 */
	int32			ar_mode ;
	int32			ar_dep_str ;
	int32			ar_indice_basic ;
	int32			ar_lg_name ;
	int32			ar_mode_sv ;
}	t_attribut_ref ;

/*
 *	ne jamais modifier l'ordre des champs dans cette structure
 *
 *	les 5 premiers champs sont initialises en C dans un tableau
 *	des structures .
 */

typedef	struct
{
	/* 	initialises dans apitab.c */
	int32			or_num ;
	int32			or_type ;
	char			*or_name ;
	t_attribut_ref		*or_tab_attr ;
	int32			or_nb_attr ;

	/* 	calcules */
	int32			or_nb_basic ;
	int32			or_nb_objet ;
	int32			or_nb_list_basic ;
	int32			or_nb_list_objet ;
	int32			or_sz_str ;
	int32			or_lg_name ;

	int32			or_dep_attr ;	/* offset pour tab_attr	*/
	int32			or_dep_basic ;	/* offset pour tab_basic*/
	int32			or_dep_str ;	/* offset pour buf_str	*/
	int32			or_sz_tot ;	/* taille totale objet	*/


	/*	flags & autres */
	int32			or_root ;	/* objet racine ?	*/
	int32			or_stext ;	/* simple type extension*/
	int32			or_wrapped ;	/* attributs xml wrapped=>obix*/
	int32			or_custom ;	/* attributs xml custom=>obix*/
	int32			or_wraphidden ;	/* only one attribut wrap*/
	char			*or_from;	/* derive from		*/
	char			*or_alias;	/* alias of		*/
	char			*or_alias_has;	/* alias defined for obj*/

	/* 	chainage des objets du type */
	char			*or_first ;	/* 1er objet du type */
	char			*or_last ;	/* dernier objet du type */
	char			*or_curr ;	/* objet courant du type */
	int32			or_nb_obj ;	/* nbre d'objets du type */

	/* 	calcules */
	t_attribut_ref		**or_stab_attr;	/* attributs tries	*/
}	t_objet_ref ;


#ifdef __cplusplus
extern "C" {
#endif

t_objet_ref	*XoGetRefObj(int32 obj) ;
t_attribut_ref	*XoGetRefAttr(int32 num) ;

#ifdef __cplusplus
};
#endif

#ifdef		XO_APRES_2_1
#include	"xopr.h"
#endif

#endif
