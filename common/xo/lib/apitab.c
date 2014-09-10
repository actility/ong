
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

#ifdef		DOS
#include	<stdio.h>
#endif
#include	"xoapipr.h"
#ifdef	RTL_MBR
#include	"mbr/include/mbr.h"
#endif

static	char	*xo_file = __FILE__ ;

/*
 *	definitions internes ne pas modifier
 */

static	t_attribut_ref	TabAttrHeader[]	=
{
	{
	XoHeaderMagic	, XoInt4B,-1,0,	"XoHeaderMagic"	, "XoInt4B"
	} ,
	{
	XoHeaderVersion	, XoInt4B,-1,0,	"XoHeaderVersion", "XoInt4B"
	} ,
	{
	XoHeaderDate	, XoDate,-1,0,	"XoHeaderDate" , "XoDate"
	} ,
	{
	XoHeaderDesc	, XoString,-1,0,"XoHeaderDesc" , "XoString"
	}
} ;


static	t_attribut_ref	TabAttrRef[] =
{
	{
	XoAttrRefNum	, XoInt4B,-1,0,	"XoAttrRefNum"	, "XoInt4B"
	} ,
	{
	XoAttrRefType	, XoInt4B,-1,0,	"XoAttrRefType"	, "XoInt4B"
	} ,
	{
	XoAttrRefLgMax	, XoInt4B,-1,0,	"XoAttrRefLgMax"	, "XoInt4B"
	} ,
	{
	XoAttrRefList	, XoInt4B,-1,0,	"XoAttrRefList"	, "XoInt4B"
	} ,
	{
	XoAttrRefName	, XoString,-1,0,"XoAttrRefName" , "XoString"
	} ,
	{
	XoAttrRefTypeName, XoString,-1,0,"XoAttrRefTypeName" , "XoString"
	} ,
	{
	XoAttrRefValue	, XoString,-1,0,"XoAttrRefValue" , "XoString"
	} ,
	{
	XoAttrRefDefault, XoString,-1,0,"XoAttrRefDefault" , "XoString"
	} ,
	{
	XoAttrRefHelpLine, XoString,-1,0,"XoAttrRefHelpLine" , "XoString"
	} ,
	{
	XoAttrRefHelpFile, XoString,-1,0,"XoAttrRefHelpFile" , "XoString"
	} ,
	{
	XoAttrRefNoSave	, XoBool,-1,0,	"XoAttrRefNoSave" , "XoBool"
	} ,
	{
	XoAttrRefSaveUnset	, XoBool,-1,0,	"XoAttrRefSaveUnset" , "XoBool"
	} ,
	{
	XoAttrRefEmbAttr	, XoBool,-1,0,	"XoAttrRefEmbAttr" , "XoBool"
	} ,
	{
	XoAttrRefWrapHidden	, XoBool,-1,0,	"XoAttrRefWrapHidden" , "XoBool"
	} ,
	{
	XoAttrRefShDico		, XoBool,-1,0,	"XoAttrRefShDico" , "XoBool"
	} ,
} ;

static	t_attribut_ref	TabObjRef[] =
{
	{
	XoObjRefNum	, XoInt4B,-1,0,	"XoObjRefNum" , "XoInt4B"
	} ,
	{
	XoObjRefType	, XoInt4B,-1,0,	"XoObjRefType" , "XoInt4B"
	} ,
	{
	XoObjRefNbBasic	, XoInt4B,-1,0,	"XoObjRefNbBasic" , "XoInt4B"
	} ,
	{
	XoObjRefNbObj	, XoInt4B,-1,0,	"XoObjRefNbObj" , "XoInt4B"
	} ,
	{
	XoObjRefNbListBas	, XoInt4B,-1,0,	"XoObjRefNbListBas" , "XoInt4B"
	} ,
	{
	XoObjRefNbListObj	, XoInt4B,-1,0,	"XoObjRefNbListObj" , "XoInt4B"
	} ,
	{
	XoObjRefListAttr , XoAttrRef,-1,AR_FLIST,"XoObjRefListAttr", "XoAttrRef"
	} ,
	{
	XoObjRefNbAttr	, XoInt4B,-1,0,	"XoObjRefNbAttr" , "XoInt4B"
	} ,
	{
	XoObjRefName	, XoString,-1,0,"XoObjRefName" , "XoString"
	} ,
	{
	XoObjRefRoot	, XoInt4B,-1,0,	"XoObjRefRoot" , "XoInt4B"
	} ,
	{
	XoObjRefStExt	, XoInt4B,-1,0,	"XoObjRefStExt" , "XoInt4B"
	} ,
	{
	XoObjRefFrom	, XoString,-1,0,"XoObjRefFrom" , "XoString"
	} ,
	{
	XoObjRefAlias	, XoString,-1,0,"XoObjRefAlias" , "XoString"
	} ,
	{
	XoObjRefAliasHas, XoString,-1,0,"XoObjRefAliasHas" , "XoString"
	} ,
} ;

static	t_attribut_ref	TabObjCfg[] =
{
{
XoCfgListObjRef,XoObjRef,-1,AR_FLIST,	"XoCfgListObjRef" , "XoObjRef"
}
} ;



t_objet_ref	XoTabObjInt[]	= 
{
/*
 *	definitions internes ne pas modifier
 */
 	{ XoInt4B	,BASIC_OBJ, "XoInt4B" } ,
	{ XoChar	,BASIC_OBJ, "XoChar" } ,
	{ XoString	,BASIC_OBJ, "XoString" } ,
	{ XoBuffer	,BASIC_OBJ, "XoBuffer" } ,
	{
		XoHeader,SINGLE_OBJ, "XoHeader"	 ,
		TabAttrHeader,sizeof(TabAttrHeader)/sizeof(t_attribut_ref),
	} ,
	{
		XoAttrRef,SINGLE_OBJ, "XoAttrRef" ,
		TabAttrRef,sizeof(TabAttrRef)/sizeof(t_attribut_ref),
	} ,
	{
		XoObjRef,STRUCT_OBJ, "XoObjRef"	,
		TabObjRef,sizeof(TabObjRef)/sizeof(t_attribut_ref),
	} ,
	{
		XoCfg,STRUCT_OBJ, "XoCfg"	,
		TabObjCfg,sizeof(TabObjCfg)/sizeof(t_attribut_ref),
	} ,
	{ XoDate     	,BASIC_OBJ, "XoDate" } ,
	{ XoBool	,BASIC_OBJ, "XoBool" } ,
	{ XoVoidObj	,STRUCT_OBJ, "XoVoidObj" , 0 , 0 } ,


	{ XoReserved12,BASIC_OBJ, "XoReserved12" } ,
	{ XoReserved13,BASIC_OBJ, "XoReserved13" } ,
	{ XoReserved14,BASIC_OBJ, "XoReserved14" } ,
	{ XoReserved15,BASIC_OBJ, "XoReserved15" } ,
	{ XoReserved16,BASIC_OBJ, "XoReserved16" } ,
	{ XoReserved17,BASIC_OBJ, "XoReserved17" } ,
	{ XoReserved18,BASIC_OBJ, "XoReserved18" } ,
	{ XoReserved19,BASIC_OBJ, "XoReserved19" } ,
	{ XoReserved20,BASIC_OBJ, "XoReserved20" } ,
	{ XoReserved21,BASIC_OBJ, "XoReserved21" } ,
	{ XoReserved22,BASIC_OBJ, "XoReserved22" } ,
	{ XoReserved23,BASIC_OBJ, "XoReserved23" } ,
	{ XoReserved24,BASIC_OBJ, "XoReserved24" } ,
	{ XoReserved25,BASIC_OBJ, "XoReserved25" } ,
	{ XoReserved26,BASIC_OBJ, "XoReserved26" } ,
	{ XoReserved27,BASIC_OBJ, "XoReserved27" } ,
	{ XoReserved28,BASIC_OBJ, "XoReserved28" } ,
	{ XoReserved29,BASIC_OBJ, "XoReserved29" } ,

	{ XoReserved30,BASIC_OBJ, "XoReserved30" } ,
	{ XoReserved31,BASIC_OBJ, "XoReserved31" } ,
	{ XoReserved32,BASIC_OBJ, "XoReserved32" } ,
	{ XoReserved33,BASIC_OBJ, "XoReserved33" } ,
	{ XoReserved34,BASIC_OBJ, "XoReserved34" } ,
	{ XoReserved35,BASIC_OBJ, "XoReserved35" } ,
	{ XoReserved36,BASIC_OBJ, "XoReserved36" } ,
	{ XoReserved37,BASIC_OBJ, "XoReserved37" } ,
	{ XoReserved38,BASIC_OBJ, "XoReserved38" } ,
	{ XoReserved39,BASIC_OBJ, "XoReserved39" } ,
	{ XoReserved40,BASIC_OBJ, "XoReserved40" } ,
	{ XoReserved41,BASIC_OBJ, "XoReserved41" } ,
	{ XoReserved42,BASIC_OBJ, "XoReserved42" } ,
	{ XoReserved43,BASIC_OBJ, "XoReserved43" } ,
	{ XoReserved44,BASIC_OBJ, "XoReserved44" } ,
	{ XoReserved45,BASIC_OBJ, "XoReserved45" } ,
	{ XoReserved46,BASIC_OBJ, "XoReserved46" } ,
	{ XoReserved47,BASIC_OBJ, "XoReserved47" } ,
	{ XoReserved48,BASIC_OBJ, "XoReserved48" } ,
	{ XoReserved49,BASIC_OBJ, "XoReserved49" } 
} ;


int32	XoNbObjInt  = sizeof(XoTabObjInt)/sizeof(t_objet_ref);
