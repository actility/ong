/*	attention ce fichier est genere par MKXO	*/
/*	il est vivement deconseille de le modifier	*/
#include	<stdio.h>
#include	<string.h>
#include	"xoapipr.h"
static	int32	GenXo_o:t_xml_obix_attr(void)
{
	int32	ret ;
	int32	i ;
	void	*or ;
	void	*ar[3] ;

	memset(ar,0,sizeof(ar));
	ret = -1 ;
	if (!(or=XoNew(XoObjRef))) goto end ;
	ret = -2 ;
	for (i=0;i<2;i++)
		if (!(ar[i]=XoNew(XoAttrRef))) goto end ;
	ret = -3 ;
	XoSetAttr(or,XoObjRefType,(void *)1,0) ;
	XoSetAttr(or,XoObjRefName,"o:t_xml_obix_attr",0) ;
	XoSetAttr(or,XoObjRefRoot,(void *)0,0) ;

	XoSetAttr(ar[0],XoAttrRefName,"name",0) ;
	XoSetAttr(ar[0],XoAttrRefTypeName,"XoString",0) ;
	XoSetAttr(ar[0],XoAttrRefList,(void *)0,0) ;
	XoSetAttr(ar[0],XoAttrRefLgMax,(void *)-1,0) ;
	XoSetAttr(ar[0],XoAttrRefNoSave,(void *)0,0) ;
	XoSetAttr(ar[0],XoAttrRefSaveUnset,(void *)0,0) ;
	XoSetAttr(ar[0],XoAttrRefEmbAttr,(void *)0,0) ;
	XoSetAttr(ar[0],XoAttrRefWrapHidden,(void *)0,0) ;
	XoSetAttr(ar[0],XoAttrRefShDico,(void *)0,0) ;
	XoAddInAttr(or,XoObjRefListAttr,ar[0],0) ;

	XoSetAttr(ar[1],XoAttrRefName,"value",0) ;
	XoSetAttr(ar[1],XoAttrRefTypeName,"XoString",0) ;
	XoSetAttr(ar[1],XoAttrRefList,(void *)0,0) ;
	XoSetAttr(ar[1],XoAttrRefLgMax,(void *)-1,0) ;
	XoSetAttr(ar[1],XoAttrRefNoSave,(void *)0,0) ;
	XoSetAttr(ar[1],XoAttrRefSaveUnset,(void *)0,0) ;
	XoSetAttr(ar[1],XoAttrRefEmbAttr,(void *)0,0) ;
	XoSetAttr(ar[1],XoAttrRefWrapHidden,(void *)0,0) ;
	XoSetAttr(ar[1],XoAttrRefShDico,(void *)0,0) ;
	XoAddInAttr(or,XoObjRefListAttr,ar[1],0) ;

	XoSetAttr(or,XoObjRefNbAttr,(void *)2,0) ;
	ret = -100 ;
	if (XoAddObjRef(or)<0) goto end ;
	ret = 0 ;
	end :
	for (i=0;i<2;i++)
		if (ar[i]) XoFree(ar[i],0) ;
	if (or) XoFree(or,0) ;
	return	ret ;
}
static	int32	GenXo_o:t_xml_obix_obj(void)
{
	int32	ret ;
	int32	i ;
	void	*or ;
	void	*ar[4] ;

	memset(ar,0,sizeof(ar));
	ret = -1 ;
	if (!(or=XoNew(XoObjRef))) goto end ;
	ret = -2 ;
	for (i=0;i<3;i++)
		if (!(ar[i]=XoNew(XoAttrRef))) goto end ;
	ret = -3 ;
	XoSetAttr(or,XoObjRefType,(void *)2,0) ;
	XoSetAttr(or,XoObjRefName,"o:t_xml_obix_obj",0) ;
	XoSetAttr(or,XoObjRefRoot,(void *)0,0) ;

	XoSetAttr(ar[0],XoAttrRefName,"name$",0) ;
	XoSetAttr(ar[0],XoAttrRefTypeName,"XoString",0) ;
	XoSetAttr(ar[0],XoAttrRefList,(void *)0,0) ;
	XoSetAttr(ar[0],XoAttrRefLgMax,(void *)-1,0) ;
	XoSetAttr(ar[0],XoAttrRefNoSave,(void *)0,0) ;
	XoSetAttr(ar[0],XoAttrRefSaveUnset,(void *)0,0) ;
	XoSetAttr(ar[0],XoAttrRefEmbAttr,(void *)0,0) ;
	XoSetAttr(ar[0],XoAttrRefWrapHidden,(void *)0,0) ;
	XoSetAttr(ar[0],XoAttrRefShDico,(void *)0,0) ;
	XoAddInAttr(or,XoObjRefListAttr,ar[0],0) ;

	XoSetAttr(ar[1],XoAttrRefName,"xmlns",0) ;
	XoSetAttr(ar[1],XoAttrRefTypeName,"XoString",0) ;
	XoSetAttr(ar[1],XoAttrRefList,(void *)0,0) ;
	XoSetAttr(ar[1],XoAttrRefLgMax,(void *)-1,0) ;
	XoSetAttr(ar[1],XoAttrRefNoSave,(void *)0,0) ;
	XoSetAttr(ar[1],XoAttrRefSaveUnset,(void *)0,0) ;
	XoSetAttr(ar[1],XoAttrRefEmbAttr,(void *)1,0) ;
	XoSetAttr(ar[1],XoAttrRefWrapHidden,(void *)0,0) ;
	XoSetAttr(ar[1],XoAttrRefShDico,(void *)1,0) ;
	XoAddInAttr(or,XoObjRefListAttr,ar[1],0) ;

	XoSetAttr(ar[2],XoAttrRefName,"custom$",0) ;
	XoSetAttr(ar[2],XoAttrRefTypeName,"o:t_xml_obix_attr",0) ;
	XoSetAttr(ar[2],XoAttrRefList,(void *)1,0) ;
	XoSetAttr(ar[2],XoAttrRefLgMax,(void *)-1,0) ;
	XoSetAttr(ar[2],XoAttrRefNoSave,(void *)0,0) ;
	XoSetAttr(ar[2],XoAttrRefSaveUnset,(void *)0,0) ;
	XoSetAttr(ar[2],XoAttrRefEmbAttr,(void *)1,0) ;
	XoSetAttr(ar[2],XoAttrRefWrapHidden,(void *)0,0) ;
	XoSetAttr(ar[2],XoAttrRefShDico,(void *)0,0) ;
	XoAddInAttr(or,XoObjRefListAttr,ar[2],0) ;

	XoSetAttr(or,XoObjRefNbAttr,(void *)3,0) ;
	ret = -100 ;
	if (XoAddObjRef(or)<0) goto end ;
	ret = 0 ;
	end :
	for (i=0;i<3;i++)
		if (ar[i]) XoFree(ar[i],0) ;
	if (or) XoFree(or,0) ;
	return	ret ;
}
static	int32	GenXo_o:obj(void)
{
	int32	ret ;
	int32	i ;
	void	*or ;
	void	*ar[11] ;

	memset(ar,0,sizeof(ar));
	ret = -1 ;
	if (!(or=XoNew(XoObjRef))) goto end ;
	ret = -2 ;
	for (i=0;i<10;i++)
		if (!(ar[i]=XoNew(XoAttrRef))) goto end ;
	ret = -3 ;
	XoSetAttr(or,XoObjRefType,(void *)1,0) ;
	XoSetAttr(or,XoObjRefName,"o:obj",0) ;
	XoSetAttr(or,XoObjRefRoot,(void *)0,0) ;

	XoSetAttr(ar[0],XoAttrRefName,"name$",0) ;
	XoSetAttr(ar[0],XoAttrRefTypeName,"XoString",0) ;
	XoSetAttr(ar[0],XoAttrRefList,(void *)0,0) ;
	XoSetAttr(ar[0],XoAttrRefLgMax,(void *)-1,0) ;
	XoSetAttr(ar[0],XoAttrRefNoSave,(void *)0,0) ;
	XoSetAttr(ar[0],XoAttrRefSaveUnset,(void *)0,0) ;
	XoSetAttr(ar[0],XoAttrRefEmbAttr,(void *)0,0) ;
	XoSetAttr(ar[0],XoAttrRefWrapHidden,(void *)0,0) ;
	XoSetAttr(ar[0],XoAttrRefShDico,(void *)0,0) ;
	XoAddInAttr(or,XoObjRefListAttr,ar[0],0) ;

	XoSetAttr(ar[1],XoAttrRefName,"xmlns",0) ;
	XoSetAttr(ar[1],XoAttrRefTypeName,"XoString",0) ;
	XoSetAttr(ar[1],XoAttrRefList,(void *)0,0) ;
	XoSetAttr(ar[1],XoAttrRefLgMax,(void *)-1,0) ;
	XoSetAttr(ar[1],XoAttrRefNoSave,(void *)0,0) ;
	XoSetAttr(ar[1],XoAttrRefSaveUnset,(void *)0,0) ;
	XoSetAttr(ar[1],XoAttrRefEmbAttr,(void *)1,0) ;
	XoSetAttr(ar[1],XoAttrRefWrapHidden,(void *)0,0) ;
	XoSetAttr(ar[1],XoAttrRefShDico,(void *)1,0) ;
	XoAddInAttr(or,XoObjRefListAttr,ar[1],0) ;

	XoSetAttr(ar[2],XoAttrRefName,"custom$",0) ;
	XoSetAttr(ar[2],XoAttrRefTypeName,"o:t_xml_obix_attr",0) ;
	XoSetAttr(ar[2],XoAttrRefList,(void *)1,0) ;
	XoSetAttr(ar[2],XoAttrRefLgMax,(void *)-1,0) ;
	XoSetAttr(ar[2],XoAttrRefNoSave,(void *)0,0) ;
	XoSetAttr(ar[2],XoAttrRefSaveUnset,(void *)0,0) ;
	XoSetAttr(ar[2],XoAttrRefEmbAttr,(void *)1,0) ;
	XoSetAttr(ar[2],XoAttrRefWrapHidden,(void *)0,0) ;
	XoSetAttr(ar[2],XoAttrRefShDico,(void *)0,0) ;
	XoAddInAttr(or,XoObjRefListAttr,ar[2],0) ;

	XoSetAttr(ar[3],XoAttrRefName,"name",0) ;
	XoSetAttr(ar[3],XoAttrRefTypeName,"XoString",0) ;
	XoSetAttr(ar[3],XoAttrRefList,(void *)0,0) ;
	XoSetAttr(ar[3],XoAttrRefLgMax,(void *)-1,0) ;
	XoSetAttr(ar[3],XoAttrRefNoSave,(void *)0,0) ;
	XoSetAttr(ar[3],XoAttrRefSaveUnset,(void *)0,0) ;
	XoSetAttr(ar[3],XoAttrRefEmbAttr,(void *)1,0) ;
	XoSetAttr(ar[3],XoAttrRefWrapHidden,(void *)0,0) ;
	XoSetAttr(ar[3],XoAttrRefShDico,(void *)0,0) ;
	XoAddInAttr(or,XoObjRefListAttr,ar[3],0) ;

	XoSetAttr(ar[4],XoAttrRefName,"href",0) ;
	XoSetAttr(ar[4],XoAttrRefTypeName,"XoString",0) ;
	XoSetAttr(ar[4],XoAttrRefList,(void *)0,0) ;
	XoSetAttr(ar[4],XoAttrRefLgMax,(void *)-1,0) ;
	XoSetAttr(ar[4],XoAttrRefNoSave,(void *)0,0) ;
	XoSetAttr(ar[4],XoAttrRefSaveUnset,(void *)0,0) ;
	XoSetAttr(ar[4],XoAttrRefEmbAttr,(void *)1,0) ;
	XoSetAttr(ar[4],XoAttrRefWrapHidden,(void *)0,0) ;
	XoSetAttr(ar[4],XoAttrRefShDico,(void *)0,0) ;
	XoAddInAttr(or,XoObjRefListAttr,ar[4],0) ;

	XoSetAttr(ar[5],XoAttrRefName,"is",0) ;
	XoSetAttr(ar[5],XoAttrRefTypeName,"XoString",0) ;
	XoSetAttr(ar[5],XoAttrRefList,(void *)0,0) ;
	XoSetAttr(ar[5],XoAttrRefLgMax,(void *)-1,0) ;
	XoSetAttr(ar[5],XoAttrRefNoSave,(void *)0,0) ;
	XoSetAttr(ar[5],XoAttrRefSaveUnset,(void *)0,0) ;
	XoSetAttr(ar[5],XoAttrRefEmbAttr,(void *)1,0) ;
	XoSetAttr(ar[5],XoAttrRefWrapHidden,(void *)0,0) ;
	XoSetAttr(ar[5],XoAttrRefShDico,(void *)0,0) ;
	XoAddInAttr(or,XoObjRefListAttr,ar[5],0) ;

	XoSetAttr(ar[6],XoAttrRefName,"null",0) ;
	XoSetAttr(ar[6],XoAttrRefTypeName,"XoString",0) ;
	XoSetAttr(ar[6],XoAttrRefList,(void *)0,0) ;
	XoSetAttr(ar[6],XoAttrRefLgMax,(void *)5,0) ;
	XoSetAttr(ar[6],XoAttrRefNoSave,(void *)0,0) ;
	XoSetAttr(ar[6],XoAttrRefSaveUnset,(void *)0,0) ;
	XoSetAttr(ar[6],XoAttrRefEmbAttr,(void *)1,0) ;
	XoSetAttr(ar[6],XoAttrRefWrapHidden,(void *)0,0) ;
	XoSetAttr(ar[6],XoAttrRefShDico,(void *)0,0) ;
	XoAddInAttr(or,XoObjRefListAttr,ar[6],0) ;

	XoSetAttr(ar[7],XoAttrRefName,"icon",0) ;
	XoSetAttr(ar[7],XoAttrRefTypeName,"XoString",0) ;
	XoSetAttr(ar[7],XoAttrRefList,(void *)0,0) ;
	XoSetAttr(ar[7],XoAttrRefLgMax,(void *)-1,0) ;
	XoSetAttr(ar[7],XoAttrRefNoSave,(void *)0,0) ;
	XoSetAttr(ar[7],XoAttrRefSaveUnset,(void *)0,0) ;
	XoSetAttr(ar[7],XoAttrRefEmbAttr,(void *)1,0) ;
	XoSetAttr(ar[7],XoAttrRefWrapHidden,(void *)0,0) ;
	XoSetAttr(ar[7],XoAttrRefShDico,(void *)0,0) ;
	XoAddInAttr(or,XoObjRefListAttr,ar[7],0) ;

	XoSetAttr(ar[8],XoAttrRefName,"displayName",0) ;
	XoSetAttr(ar[8],XoAttrRefTypeName,"XoString",0) ;
	XoSetAttr(ar[8],XoAttrRefList,(void *)0,0) ;
	XoSetAttr(ar[8],XoAttrRefLgMax,(void *)-1,0) ;
	XoSetAttr(ar[8],XoAttrRefNoSave,(void *)0,0) ;
	XoSetAttr(ar[8],XoAttrRefSaveUnset,(void *)0,0) ;
	XoSetAttr(ar[8],XoAttrRefEmbAttr,(void *)1,0) ;
	XoSetAttr(ar[8],XoAttrRefWrapHidden,(void *)0,0) ;
	XoSetAttr(ar[8],XoAttrRefShDico,(void *)0,0) ;
	XoAddInAttr(or,XoObjRefListAttr,ar[8],0) ;

	XoSetAttr(ar[9],XoAttrRefName,"display",0) ;
	XoSetAttr(ar[9],XoAttrRefTypeName,"XoString",0) ;
	XoSetAttr(ar[9],XoAttrRefList,(void *)0,0) ;
	XoSetAttr(ar[9],XoAttrRefLgMax,(void *)-1,0) ;
	XoSetAttr(ar[9],XoAttrRefNoSave,(void *)0,0) ;
	XoSetAttr(ar[9],XoAttrRefSaveUnset,(void *)0,0) ;
	XoSetAttr(ar[9],XoAttrRefEmbAttr,(void *)1,0) ;
	XoSetAttr(ar[9],XoAttrRefWrapHidden,(void *)0,0) ;
	XoSetAttr(ar[9],XoAttrRefShDico,(void *)0,0) ;
	XoAddInAttr(or,XoObjRefListAttr,ar[9],0) ;

	XoSetAttr(ar[10],XoAttrRefName,"writable",0) ;
	XoSetAttr(ar[10],XoAttrRefTypeName,"XoString",0) ;
	XoSetAttr(ar[10],XoAttrRefList,(void *)0,0) ;
	XoSetAttr(ar[10],XoAttrRefLgMax,(void *)5,0) ;
	XoSetAttr(ar[10],XoAttrRefNoSave,(void *)0,0) ;
	XoSetAttr(ar[10],XoAttrRefSaveUnset,(void *)0,0) ;
	XoSetAttr(ar[10],XoAttrRefEmbAttr,(void *)1,0) ;
	XoSetAttr(ar[10],XoAttrRefWrapHidden,(void *)0,0) ;
	XoSetAttr(ar[10],XoAttrRefShDico,(void *)0,0) ;
	XoAddInAttr(or,XoObjRefListAttr,ar[10],0) ;

	XoSetAttr(ar[11],XoAttrRefName,"status",0) ;
	XoSetAttr(ar[11],XoAttrRefTypeName,"XoString",0) ;
	XoSetAttr(ar[11],XoAttrRefList,(void *)0,0) ;
	XoSetAttr(ar[11],XoAttrRefLgMax,(void *)-1,0) ;
	XoSetAttr(ar[11],XoAttrRefNoSave,(void *)0,0) ;
	XoSetAttr(ar[11],XoAttrRefSaveUnset,(void *)0,0) ;
	XoSetAttr(ar[11],XoAttrRefEmbAttr,(void *)1,0) ;
	XoSetAttr(ar[11],XoAttrRefWrapHidden,(void *)0,0) ;
	XoSetAttr(ar[11],XoAttrRefShDico,(void *)0,0) ;
	XoAddInAttr(or,XoObjRefListAttr,ar[11],0) ;

	XoSetAttr(ar[12],XoAttrRefName,"wrapper$",0) ;
	XoSetAttr(ar[12],XoAttrRefTypeName,"XoVoidObj",0) ;
	XoSetAttr(ar[12],XoAttrRefList,(void *)1,0) ;
	XoSetAttr(ar[12],XoAttrRefLgMax,(void *)-1,0) ;
	XoSetAttr(ar[12],XoAttrRefNoSave,(void *)0,0) ;
	XoSetAttr(ar[12],XoAttrRefSaveUnset,(void *)0,0) ;
	XoSetAttr(ar[12],XoAttrRefEmbAttr,(void *)0,0) ;
	XoSetAttr(ar[12],XoAttrRefWrapHidden,(void *)0,0) ;
	XoSetAttr(ar[12],XoAttrRefShDico,(void *)0,0) ;
	XoAddInAttr(or,XoObjRefListAttr,ar[12],0) ;

	XoSetAttr(or,XoObjRefNbAttr,(void *)13,0) ;
	ret = -100 ;
	if (XoAddObjRef(or)<0) goto end ;
	ret = 0 ;
	end :
	for (i=0;i<13;i++)
		if (ar[i]) XoFree(ar[i],0) ;
	if (or) XoFree(or,0) ;
	return	ret ;
}
static	int32	GenXo_o:list(void)
{
	int32	ret ;
	int32	i ;
	void	*or ;
	void	*ar[3] ;

	memset(ar,0,sizeof(ar));
	ret = -1 ;
	if (!(or=XoNew(XoObjRef))) goto end ;
	ret = -2 ;
	for (i=0;i<2;i++)
		if (!(ar[i]=XoNew(XoAttrRef))) goto end ;
	ret = -3 ;
	XoSetAttr(or,XoObjRefType,(void *)1,0) ;
	XoSetAttr(or,XoObjRefName,"o:list",0) ;
	XoSetAttr(or,XoObjRefRoot,(void *)0,0) ;

	XoSetAttr(ar[0],XoAttrRefName,"name$",0) ;
	XoSetAttr(ar[0],XoAttrRefTypeName,"XoString",0) ;
	XoSetAttr(ar[0],XoAttrRefList,(void *)0,0) ;
	XoSetAttr(ar[0],XoAttrRefLgMax,(void *)-1,0) ;
	XoSetAttr(ar[0],XoAttrRefNoSave,(void *)0,0) ;
	XoSetAttr(ar[0],XoAttrRefSaveUnset,(void *)0,0) ;
	XoSetAttr(ar[0],XoAttrRefEmbAttr,(void *)0,0) ;
	XoSetAttr(ar[0],XoAttrRefWrapHidden,(void *)0,0) ;
	XoSetAttr(ar[0],XoAttrRefShDico,(void *)0,0) ;
	XoAddInAttr(or,XoObjRefListAttr,ar[0],0) ;

	XoSetAttr(ar[1],XoAttrRefName,"xmlns",0) ;
	XoSetAttr(ar[1],XoAttrRefTypeName,"XoString",0) ;
	XoSetAttr(ar[1],XoAttrRefList,(void *)0,0) ;
	XoSetAttr(ar[1],XoAttrRefLgMax,(void *)-1,0) ;
	XoSetAttr(ar[1],XoAttrRefNoSave,(void *)0,0) ;
	XoSetAttr(ar[1],XoAttrRefSaveUnset,(void *)0,0) ;
	XoSetAttr(ar[1],XoAttrRefEmbAttr,(void *)1,0) ;
	XoSetAttr(ar[1],XoAttrRefWrapHidden,(void *)0,0) ;
	XoSetAttr(ar[1],XoAttrRefShDico,(void *)1,0) ;
	XoAddInAttr(or,XoObjRefListAttr,ar[1],0) ;

	XoSetAttr(ar[2],XoAttrRefName,"custom$",0) ;
	XoSetAttr(ar[2],XoAttrRefTypeName,"o:t_xml_obix_attr",0) ;
	XoSetAttr(ar[2],XoAttrRefList,(void *)1,0) ;
	XoSetAttr(ar[2],XoAttrRefLgMax,(void *)-1,0) ;
	XoSetAttr(ar[2],XoAttrRefNoSave,(void *)0,0) ;
	XoSetAttr(ar[2],XoAttrRefSaveUnset,(void *)0,0) ;
	XoSetAttr(ar[2],XoAttrRefEmbAttr,(void *)1,0) ;
	XoSetAttr(ar[2],XoAttrRefWrapHidden,(void *)0,0) ;
	XoSetAttr(ar[2],XoAttrRefShDico,(void *)0,0) ;
	XoAddInAttr(or,XoObjRefListAttr,ar[2],0) ;

	XoSetAttr(ar[3],XoAttrRefName,"name",0) ;
	XoSetAttr(ar[3],XoAttrRefTypeName,"XoString",0) ;
	XoSetAttr(ar[3],XoAttrRefList,(void *)0,0) ;
	XoSetAttr(ar[3],XoAttrRefLgMax,(void *)-1,0) ;
	XoSetAttr(ar[3],XoAttrRefNoSave,(void *)0,0) ;
	XoSetAttr(ar[3],XoAttrRefSaveUnset,(void *)0,0) ;
	XoSetAttr(ar[3],XoAttrRefEmbAttr,(void *)1,0) ;
	XoSetAttr(ar[3],XoAttrRefWrapHidden,(void *)0,0) ;
	XoSetAttr(ar[3],XoAttrRefShDico,(void *)0,0) ;
	XoAddInAttr(or,XoObjRefListAttr,ar[3],0) ;

	XoSetAttr(ar[4],XoAttrRefName,"href",0) ;
	XoSetAttr(ar[4],XoAttrRefTypeName,"XoString",0) ;
	XoSetAttr(ar[4],XoAttrRefList,(void *)0,0) ;
	XoSetAttr(ar[4],XoAttrRefLgMax,(void *)-1,0) ;
	XoSetAttr(ar[4],XoAttrRefNoSave,(void *)0,0) ;
	XoSetAttr(ar[4],XoAttrRefSaveUnset,(void *)0,0) ;
	XoSetAttr(ar[4],XoAttrRefEmbAttr,(void *)1,0) ;
	XoSetAttr(ar[4],XoAttrRefWrapHidden,(void *)0,0) ;
	XoSetAttr(ar[4],XoAttrRefShDico,(void *)0,0) ;
	XoAddInAttr(or,XoObjRefListAttr,ar[4],0) ;

	XoSetAttr(ar[5],XoAttrRefName,"is",0) ;
	XoSetAttr(ar[5],XoAttrRefTypeName,"XoString",0) ;
	XoSetAttr(ar[5],XoAttrRefList,(void *)0,0) ;
	XoSetAttr(ar[5],XoAttrRefLgMax,(void *)-1,0) ;
	XoSetAttr(ar[5],XoAttrRefNoSave,(void *)0,0) ;
	XoSetAttr(ar[5],XoAttrRefSaveUnset,(void *)0,0) ;
	XoSetAttr(ar[5],XoAttrRefEmbAttr,(void *)1,0) ;
	XoSetAttr(ar[5],XoAttrRefWrapHidden,(void *)0,0) ;
	XoSetAttr(ar[5],XoAttrRefShDico,(void *)0,0) ;
	XoAddInAttr(or,XoObjRefListAttr,ar[5],0) ;

	XoSetAttr(ar[6],XoAttrRefName,"null",0) ;
	XoSetAttr(ar[6],XoAttrRefTypeName,"XoString",0) ;
	XoSetAttr(ar[6],XoAttrRefList,(void *)0,0) ;
	XoSetAttr(ar[6],XoAttrRefLgMax,(void *)5,0) ;
	XoSetAttr(ar[6],XoAttrRefNoSave,(void *)0,0) ;
	XoSetAttr(ar[6],XoAttrRefSaveUnset,(void *)0,0) ;
	XoSetAttr(ar[6],XoAttrRefEmbAttr,(void *)1,0) ;
	XoSetAttr(ar[6],XoAttrRefWrapHidden,(void *)0,0) ;
	XoSetAttr(ar[6],XoAttrRefShDico,(void *)0,0) ;
	XoAddInAttr(or,XoObjRefListAttr,ar[6],0) ;

	XoSetAttr(ar[7],XoAttrRefName,"icon",0) ;
	XoSetAttr(ar[7],XoAttrRefTypeName,"XoString",0) ;
	XoSetAttr(ar[7],XoAttrRefList,(void *)0,0) ;
	XoSetAttr(ar[7],XoAttrRefLgMax,(void *)-1,0) ;
	XoSetAttr(ar[7],XoAttrRefNoSave,(void *)0,0) ;
	XoSetAttr(ar[7],XoAttrRefSaveUnset,(void *)0,0) ;
	XoSetAttr(ar[7],XoAttrRefEmbAttr,(void *)1,0) ;
	XoSetAttr(ar[7],XoAttrRefWrapHidden,(void *)0,0) ;
	XoSetAttr(ar[7],XoAttrRefShDico,(void *)0,0) ;
	XoAddInAttr(or,XoObjRefListAttr,ar[7],0) ;

	XoSetAttr(ar[8],XoAttrRefName,"displayName",0) ;
	XoSetAttr(ar[8],XoAttrRefTypeName,"XoString",0) ;
	XoSetAttr(ar[8],XoAttrRefList,(void *)0,0) ;
	XoSetAttr(ar[8],XoAttrRefLgMax,(void *)-1,0) ;
	XoSetAttr(ar[8],XoAttrRefNoSave,(void *)0,0) ;
	XoSetAttr(ar[8],XoAttrRefSaveUnset,(void *)0,0) ;
	XoSetAttr(ar[8],XoAttrRefEmbAttr,(void *)1,0) ;
	XoSetAttr(ar[8],XoAttrRefWrapHidden,(void *)0,0) ;
	XoSetAttr(ar[8],XoAttrRefShDico,(void *)0,0) ;
	XoAddInAttr(or,XoObjRefListAttr,ar[8],0) ;

	XoSetAttr(ar[9],XoAttrRefName,"display",0) ;
	XoSetAttr(ar[9],XoAttrRefTypeName,"XoString",0) ;
	XoSetAttr(ar[9],XoAttrRefList,(void *)0,0) ;
	XoSetAttr(ar[9],XoAttrRefLgMax,(void *)-1,0) ;
	XoSetAttr(ar[9],XoAttrRefNoSave,(void *)0,0) ;
	XoSetAttr(ar[9],XoAttrRefSaveUnset,(void *)0,0) ;
	XoSetAttr(ar[9],XoAttrRefEmbAttr,(void *)1,0) ;
	XoSetAttr(ar[9],XoAttrRefWrapHidden,(void *)0,0) ;
	XoSetAttr(ar[9],XoAttrRefShDico,(void *)0,0) ;
	XoAddInAttr(or,XoObjRefListAttr,ar[9],0) ;

	XoSetAttr(ar[10],XoAttrRefName,"writable",0) ;
	XoSetAttr(ar[10],XoAttrRefTypeName,"XoString",0) ;
	XoSetAttr(ar[10],XoAttrRefList,(void *)0,0) ;
	XoSetAttr(ar[10],XoAttrRefLgMax,(void *)5,0) ;
	XoSetAttr(ar[10],XoAttrRefNoSave,(void *)0,0) ;
	XoSetAttr(ar[10],XoAttrRefSaveUnset,(void *)0,0) ;
	XoSetAttr(ar[10],XoAttrRefEmbAttr,(void *)1,0) ;
	XoSetAttr(ar[10],XoAttrRefWrapHidden,(void *)0,0) ;
	XoSetAttr(ar[10],XoAttrRefShDico,(void *)0,0) ;
	XoAddInAttr(or,XoObjRefListAttr,ar[10],0) ;

	XoSetAttr(ar[11],XoAttrRefName,"status",0) ;
	XoSetAttr(ar[11],XoAttrRefTypeName,"XoString",0) ;
	XoSetAttr(ar[11],XoAttrRefList,(void *)0,0) ;
	XoSetAttr(ar[11],XoAttrRefLgMax,(void *)-1,0) ;
	XoSetAttr(ar[11],XoAttrRefNoSave,(void *)0,0) ;
	XoSetAttr(ar[11],XoAttrRefSaveUnset,(void *)0,0) ;
	XoSetAttr(ar[11],XoAttrRefEmbAttr,(void *)1,0) ;
	XoSetAttr(ar[11],XoAttrRefWrapHidden,(void *)0,0) ;
	XoSetAttr(ar[11],XoAttrRefShDico,(void *)0,0) ;
	XoAddInAttr(or,XoObjRefListAttr,ar[11],0) ;

	XoSetAttr(ar[12],XoAttrRefName,"of",0) ;
	XoSetAttr(ar[12],XoAttrRefTypeName,"XoString",0) ;
	XoSetAttr(ar[12],XoAttrRefList,(void *)0,0) ;
	XoSetAttr(ar[12],XoAttrRefLgMax,(void *)-1,0) ;
	XoSetAttr(ar[12],XoAttrRefNoSave,(void *)0,0) ;
	XoSetAttr(ar[12],XoAttrRefSaveUnset,(void *)0,0) ;
	XoSetAttr(ar[12],XoAttrRefEmbAttr,(void *)1,0) ;
	XoSetAttr(ar[12],XoAttrRefWrapHidden,(void *)0,0) ;
	XoSetAttr(ar[12],XoAttrRefShDico,(void *)0,0) ;
	XoAddInAttr(or,XoObjRefListAttr,ar[12],0) ;

	XoSetAttr(ar[13],XoAttrRefName,"wrapper$",0) ;
	XoSetAttr(ar[13],XoAttrRefTypeName,"XoVoidObj",0) ;
	XoSetAttr(ar[13],XoAttrRefList,(void *)1,0) ;
	XoSetAttr(ar[13],XoAttrRefLgMax,(void *)-1,0) ;
	XoSetAttr(ar[13],XoAttrRefNoSave,(void *)0,0) ;
	XoSetAttr(ar[13],XoAttrRefSaveUnset,(void *)0,0) ;
	XoSetAttr(ar[13],XoAttrRefEmbAttr,(void *)0,0) ;
	XoSetAttr(ar[13],XoAttrRefWrapHidden,(void *)0,0) ;
	XoSetAttr(ar[13],XoAttrRefShDico,(void *)0,0) ;
	XoAddInAttr(or,XoObjRefListAttr,ar[13],0) ;

	XoSetAttr(or,XoObjRefNbAttr,(void *)14,0) ;
	ret = -100 ;
	if (XoAddObjRef(or)<0) goto end ;
	ret = 0 ;
	end :
	for (i=0;i<14;i++)
		if (ar[i]) XoFree(ar[i],0) ;
	if (or) XoFree(or,0) ;
	return	ret ;
}
static	int32	GenXo_o:op(void)
{
	int32	ret ;
	int32	i ;
	void	*or ;
	void	*ar[3] ;

	memset(ar,0,sizeof(ar));
	ret = -1 ;
	if (!(or=XoNew(XoObjRef))) goto end ;
	ret = -2 ;
	for (i=0;i<2;i++)
		if (!(ar[i]=XoNew(XoAttrRef))) goto end ;
	ret = -3 ;
	XoSetAttr(or,XoObjRefType,(void *)1,0) ;
	XoSetAttr(or,XoObjRefName,"o:op",0) ;
	XoSetAttr(or,XoObjRefRoot,(void *)0,0) ;

	XoSetAttr(ar[0],XoAttrRefName,"name$",0) ;
	XoSetAttr(ar[0],XoAttrRefTypeName,"XoString",0) ;
	XoSetAttr(ar[0],XoAttrRefList,(void *)0,0) ;
	XoSetAttr(ar[0],XoAttrRefLgMax,(void *)-1,0) ;
	XoSetAttr(ar[0],XoAttrRefNoSave,(void *)0,0) ;
	XoSetAttr(ar[0],XoAttrRefSaveUnset,(void *)0,0) ;
	XoSetAttr(ar[0],XoAttrRefEmbAttr,(void *)0,0) ;
	XoSetAttr(ar[0],XoAttrRefWrapHidden,(void *)0,0) ;
	XoSetAttr(ar[0],XoAttrRefShDico,(void *)0,0) ;
	XoAddInAttr(or,XoObjRefListAttr,ar[0],0) ;

	XoSetAttr(ar[1],XoAttrRefName,"xmlns",0) ;
	XoSetAttr(ar[1],XoAttrRefTypeName,"XoString",0) ;
	XoSetAttr(ar[1],XoAttrRefList,(void *)0,0) ;
	XoSetAttr(ar[1],XoAttrRefLgMax,(void *)-1,0) ;
	XoSetAttr(ar[1],XoAttrRefNoSave,(void *)0,0) ;
	XoSetAttr(ar[1],XoAttrRefSaveUnset,(void *)0,0) ;
	XoSetAttr(ar[1],XoAttrRefEmbAttr,(void *)1,0) ;
	XoSetAttr(ar[1],XoAttrRefWrapHidden,(void *)0,0) ;
	XoSetAttr(ar[1],XoAttrRefShDico,(void *)1,0) ;
	XoAddInAttr(or,XoObjRefListAttr,ar[1],0) ;

	XoSetAttr(ar[2],XoAttrRefName,"custom$",0) ;
	XoSetAttr(ar[2],XoAttrRefTypeName,"o:t_xml_obix_attr",0) ;
	XoSetAttr(ar[2],XoAttrRefList,(void *)1,0) ;
	XoSetAttr(ar[2],XoAttrRefLgMax,(void *)-1,0) ;
	XoSetAttr(ar[2],XoAttrRefNoSave,(void *)0,0) ;
	XoSetAttr(ar[2],XoAttrRefSaveUnset,(void *)0,0) ;
	XoSetAttr(ar[2],XoAttrRefEmbAttr,(void *)1,0) ;
	XoSetAttr(ar[2],XoAttrRefWrapHidden,(void *)0,0) ;
	XoSetAttr(ar[2],XoAttrRefShDico,(void *)0,0) ;
	XoAddInAttr(or,XoObjRefListAttr,ar[2],0) ;

	XoSetAttr(ar[3],XoAttrRefName,"name",0) ;
	XoSetAttr(ar[3],XoAttrRefTypeName,"XoString",0) ;
	XoSetAttr(ar[3],XoAttrRefList,(void *)0,0) ;
	XoSetAttr(ar[3],XoAttrRefLgMax,(void *)-1,0) ;
	XoSetAttr(ar[3],XoAttrRefNoSave,(void *)0,0) ;
	XoSetAttr(ar[3],XoAttrRefSaveUnset,(void *)0,0) ;
	XoSetAttr(ar[3],XoAttrRefEmbAttr,(void *)1,0) ;
	XoSetAttr(ar[3],XoAttrRefWrapHidden,(void *)0,0) ;
	XoSetAttr(ar[3],XoAttrRefShDico,(void *)0,0) ;
	XoAddInAttr(or,XoObjRefListAttr,ar[3],0) ;

	XoSetAttr(ar[4],XoAttrRefName,"href",0) ;
	XoSetAttr(ar[4],XoAttrRefTypeName,"XoString",0) ;
	XoSetAttr(ar[4],XoAttrRefList,(void *)0,0) ;
	XoSetAttr(ar[4],XoAttrRefLgMax,(void *)-1,0) ;
	XoSetAttr(ar[4],XoAttrRefNoSave,(void *)0,0) ;
	XoSetAttr(ar[4],XoAttrRefSaveUnset,(void *)0,0) ;
	XoSetAttr(ar[4],XoAttrRefEmbAttr,(void *)1,0) ;
	XoSetAttr(ar[4],XoAttrRefWrapHidden,(void *)0,0) ;
	XoSetAttr(ar[4],XoAttrRefShDico,(void *)0,0) ;
	XoAddInAttr(or,XoObjRefListAttr,ar[4],0) ;

	XoSetAttr(ar[5],XoAttrRefName,"is",0) ;
	XoSetAttr(ar[5],XoAttrRefTypeName,"XoString",0) ;
	XoSetAttr(ar[5],XoAttrRefList,(void *)0,0) ;
	XoSetAttr(ar[5],XoAttrRefLgMax,(void *)-1,0) ;
	XoSetAttr(ar[5],XoAttrRefNoSave,(void *)0,0) ;
	XoSetAttr(ar[5],XoAttrRefSaveUnset,(void *)0,0) ;
	XoSetAttr(ar[5],XoAttrRefEmbAttr,(void *)1,0) ;
	XoSetAttr(ar[5],XoAttrRefWrapHidden,(void *)0,0) ;
	XoSetAttr(ar[5],XoAttrRefShDico,(void *)0,0) ;
	XoAddInAttr(or,XoObjRefListAttr,ar[5],0) ;

	XoSetAttr(ar[6],XoAttrRefName,"null",0) ;
	XoSetAttr(ar[6],XoAttrRefTypeName,"XoString",0) ;
	XoSetAttr(ar[6],XoAttrRefList,(void *)0,0) ;
	XoSetAttr(ar[6],XoAttrRefLgMax,(void *)5,0) ;
	XoSetAttr(ar[6],XoAttrRefNoSave,(void *)0,0) ;
	XoSetAttr(ar[6],XoAttrRefSaveUnset,(void *)0,0) ;
	XoSetAttr(ar[6],XoAttrRefEmbAttr,(void *)1,0) ;
	XoSetAttr(ar[6],XoAttrRefWrapHidden,(void *)0,0) ;
	XoSetAttr(ar[6],XoAttrRefShDico,(void *)0,0) ;
	XoAddInAttr(or,XoObjRefListAttr,ar[6],0) ;

	XoSetAttr(ar[7],XoAttrRefName,"icon",0) ;
	XoSetAttr(ar[7],XoAttrRefTypeName,"XoString",0) ;
	XoSetAttr(ar[7],XoAttrRefList,(void *)0,0) ;
	XoSetAttr(ar[7],XoAttrRefLgMax,(void *)-1,0) ;
	XoSetAttr(ar[7],XoAttrRefNoSave,(void *)0,0) ;
	XoSetAttr(ar[7],XoAttrRefSaveUnset,(void *)0,0) ;
	XoSetAttr(ar[7],XoAttrRefEmbAttr,(void *)1,0) ;
	XoSetAttr(ar[7],XoAttrRefWrapHidden,(void *)0,0) ;
	XoSetAttr(ar[7],XoAttrRefShDico,(void *)0,0) ;
	XoAddInAttr(or,XoObjRefListAttr,ar[7],0) ;

	XoSetAttr(ar[8],XoAttrRefName,"displayName",0) ;
	XoSetAttr(ar[8],XoAttrRefTypeName,"XoString",0) ;
	XoSetAttr(ar[8],XoAttrRefList,(void *)0,0) ;
	XoSetAttr(ar[8],XoAttrRefLgMax,(void *)-1,0) ;
	XoSetAttr(ar[8],XoAttrRefNoSave,(void *)0,0) ;
	XoSetAttr(ar[8],XoAttrRefSaveUnset,(void *)0,0) ;
	XoSetAttr(ar[8],XoAttrRefEmbAttr,(void *)1,0) ;
	XoSetAttr(ar[8],XoAttrRefWrapHidden,(void *)0,0) ;
	XoSetAttr(ar[8],XoAttrRefShDico,(void *)0,0) ;
	XoAddInAttr(or,XoObjRefListAttr,ar[8],0) ;

	XoSetAttr(ar[9],XoAttrRefName,"display",0) ;
	XoSetAttr(ar[9],XoAttrRefTypeName,"XoString",0) ;
	XoSetAttr(ar[9],XoAttrRefList,(void *)0,0) ;
	XoSetAttr(ar[9],XoAttrRefLgMax,(void *)-1,0) ;
	XoSetAttr(ar[9],XoAttrRefNoSave,(void *)0,0) ;
	XoSetAttr(ar[9],XoAttrRefSaveUnset,(void *)0,0) ;
	XoSetAttr(ar[9],XoAttrRefEmbAttr,(void *)1,0) ;
	XoSetAttr(ar[9],XoAttrRefWrapHidden,(void *)0,0) ;
	XoSetAttr(ar[9],XoAttrRefShDico,(void *)0,0) ;
	XoAddInAttr(or,XoObjRefListAttr,ar[9],0) ;

	XoSetAttr(ar[10],XoAttrRefName,"writable",0) ;
	XoSetAttr(ar[10],XoAttrRefTypeName,"XoString",0) ;
	XoSetAttr(ar[10],XoAttrRefList,(void *)0,0) ;
	XoSetAttr(ar[10],XoAttrRefLgMax,(void *)5,0) ;
	XoSetAttr(ar[10],XoAttrRefNoSave,(void *)0,0) ;
	XoSetAttr(ar[10],XoAttrRefSaveUnset,(void *)0,0) ;
	XoSetAttr(ar[10],XoAttrRefEmbAttr,(void *)1,0) ;
	XoSetAttr(ar[10],XoAttrRefWrapHidden,(void *)0,0) ;
	XoSetAttr(ar[10],XoAttrRefShDico,(void *)0,0) ;
	XoAddInAttr(or,XoObjRefListAttr,ar[10],0) ;

	XoSetAttr(ar[11],XoAttrRefName,"status",0) ;
	XoSetAttr(ar[11],XoAttrRefTypeName,"XoString",0) ;
	XoSetAttr(ar[11],XoAttrRefList,(void *)0,0) ;
	XoSetAttr(ar[11],XoAttrRefLgMax,(void *)-1,0) ;
	XoSetAttr(ar[11],XoAttrRefNoSave,(void *)0,0) ;
	XoSetAttr(ar[11],XoAttrRefSaveUnset,(void *)0,0) ;
	XoSetAttr(ar[11],XoAttrRefEmbAttr,(void *)1,0) ;
	XoSetAttr(ar[11],XoAttrRefWrapHidden,(void *)0,0) ;
	XoSetAttr(ar[11],XoAttrRefShDico,(void *)0,0) ;
	XoAddInAttr(or,XoObjRefListAttr,ar[11],0) ;

	XoSetAttr(ar[12],XoAttrRefName,"wrapper$",0) ;
	XoSetAttr(ar[12],XoAttrRefTypeName,"XoVoidObj",0) ;
	XoSetAttr(ar[12],XoAttrRefList,(void *)1,0) ;
	XoSetAttr(ar[12],XoAttrRefLgMax,(void *)-1,0) ;
	XoSetAttr(ar[12],XoAttrRefNoSave,(void *)0,0) ;
	XoSetAttr(ar[12],XoAttrRefSaveUnset,(void *)0,0) ;
	XoSetAttr(ar[12],XoAttrRefEmbAttr,(void *)0,0) ;
	XoSetAttr(ar[12],XoAttrRefWrapHidden,(void *)0,0) ;
	XoSetAttr(ar[12],XoAttrRefShDico,(void *)0,0) ;
	XoAddInAttr(or,XoObjRefListAttr,ar[12],0) ;

	XoSetAttr(ar[13],XoAttrRefName,"in",0) ;
	XoSetAttr(ar[13],XoAttrRefTypeName,"XoString",0) ;
	XoSetAttr(ar[13],XoAttrRefList,(void *)0,0) ;
	XoSetAttr(ar[13],XoAttrRefLgMax,(void *)-1,0) ;
	XoSetAttr(ar[13],XoAttrRefNoSave,(void *)0,0) ;
	XoSetAttr(ar[13],XoAttrRefSaveUnset,(void *)0,0) ;
	XoSetAttr(ar[13],XoAttrRefEmbAttr,(void *)1,0) ;
	XoSetAttr(ar[13],XoAttrRefWrapHidden,(void *)0,0) ;
	XoSetAttr(ar[13],XoAttrRefShDico,(void *)0,0) ;
	XoAddInAttr(or,XoObjRefListAttr,ar[13],0) ;

	XoSetAttr(ar[14],XoAttrRefName,"out",0) ;
	XoSetAttr(ar[14],XoAttrRefTypeName,"XoString",0) ;
	XoSetAttr(ar[14],XoAttrRefList,(void *)0,0) ;
	XoSetAttr(ar[14],XoAttrRefLgMax,(void *)-1,0) ;
	XoSetAttr(ar[14],XoAttrRefNoSave,(void *)0,0) ;
	XoSetAttr(ar[14],XoAttrRefSaveUnset,(void *)0,0) ;
	XoSetAttr(ar[14],XoAttrRefEmbAttr,(void *)1,0) ;
	XoSetAttr(ar[14],XoAttrRefWrapHidden,(void *)0,0) ;
	XoSetAttr(ar[14],XoAttrRefShDico,(void *)0,0) ;
	XoAddInAttr(or,XoObjRefListAttr,ar[14],0) ;

	XoSetAttr(or,XoObjRefNbAttr,(void *)15,0) ;
	ret = -100 ;
	if (XoAddObjRef(or)<0) goto end ;
	ret = 0 ;
	end :
	for (i=0;i<15;i++)
		if (ar[i]) XoFree(ar[i],0) ;
	if (or) XoFree(or,0) ;
	return	ret ;
}
static	int32	GenXo_o:feed(void)
{
	int32	ret ;
	int32	i ;
	void	*or ;
	void	*ar[3] ;

	memset(ar,0,sizeof(ar));
	ret = -1 ;
	if (!(or=XoNew(XoObjRef))) goto end ;
	ret = -2 ;
	for (i=0;i<2;i++)
		if (!(ar[i]=XoNew(XoAttrRef))) goto end ;
	ret = -3 ;
	XoSetAttr(or,XoObjRefType,(void *)1,0) ;
	XoSetAttr(or,XoObjRefName,"o:feed",0) ;
	XoSetAttr(or,XoObjRefRoot,(void *)0,0) ;

	XoSetAttr(ar[0],XoAttrRefName,"name$",0) ;
	XoSetAttr(ar[0],XoAttrRefTypeName,"XoString",0) ;
	XoSetAttr(ar[0],XoAttrRefList,(void *)0,0) ;
	XoSetAttr(ar[0],XoAttrRefLgMax,(void *)-1,0) ;
	XoSetAttr(ar[0],XoAttrRefNoSave,(void *)0,0) ;
	XoSetAttr(ar[0],XoAttrRefSaveUnset,(void *)0,0) ;
	XoSetAttr(ar[0],XoAttrRefEmbAttr,(void *)0,0) ;
	XoSetAttr(ar[0],XoAttrRefWrapHidden,(void *)0,0) ;
	XoSetAttr(ar[0],XoAttrRefShDico,(void *)0,0) ;
	XoAddInAttr(or,XoObjRefListAttr,ar[0],0) ;

	XoSetAttr(ar[1],XoAttrRefName,"xmlns",0) ;
	XoSetAttr(ar[1],XoAttrRefTypeName,"XoString",0) ;
	XoSetAttr(ar[1],XoAttrRefList,(void *)0,0) ;
	XoSetAttr(ar[1],XoAttrRefLgMax,(void *)-1,0) ;
	XoSetAttr(ar[1],XoAttrRefNoSave,(void *)0,0) ;
	XoSetAttr(ar[1],XoAttrRefSaveUnset,(void *)0,0) ;
	XoSetAttr(ar[1],XoAttrRefEmbAttr,(void *)1,0) ;
	XoSetAttr(ar[1],XoAttrRefWrapHidden,(void *)0,0) ;
	XoSetAttr(ar[1],XoAttrRefShDico,(void *)1,0) ;
	XoAddInAttr(or,XoObjRefListAttr,ar[1],0) ;

	XoSetAttr(ar[2],XoAttrRefName,"custom$",0) ;
	XoSetAttr(ar[2],XoAttrRefTypeName,"o:t_xml_obix_attr",0) ;
	XoSetAttr(ar[2],XoAttrRefList,(void *)1,0) ;
	XoSetAttr(ar[2],XoAttrRefLgMax,(void *)-1,0) ;
	XoSetAttr(ar[2],XoAttrRefNoSave,(void *)0,0) ;
	XoSetAttr(ar[2],XoAttrRefSaveUnset,(void *)0,0) ;
	XoSetAttr(ar[2],XoAttrRefEmbAttr,(void *)1,0) ;
	XoSetAttr(ar[2],XoAttrRefWrapHidden,(void *)0,0) ;
	XoSetAttr(ar[2],XoAttrRefShDico,(void *)0,0) ;
	XoAddInAttr(or,XoObjRefListAttr,ar[2],0) ;

	XoSetAttr(ar[3],XoAttrRefName,"name",0) ;
	XoSetAttr(ar[3],XoAttrRefTypeName,"XoString",0) ;
	XoSetAttr(ar[3],XoAttrRefList,(void *)0,0) ;
	XoSetAttr(ar[3],XoAttrRefLgMax,(void *)-1,0) ;
	XoSetAttr(ar[3],XoAttrRefNoSave,(void *)0,0) ;
	XoSetAttr(ar[3],XoAttrRefSaveUnset,(void *)0,0) ;
	XoSetAttr(ar[3],XoAttrRefEmbAttr,(void *)1,0) ;
	XoSetAttr(ar[3],XoAttrRefWrapHidden,(void *)0,0) ;
	XoSetAttr(ar[3],XoAttrRefShDico,(void *)0,0) ;
	XoAddInAttr(or,XoObjRefListAttr,ar[3],0) ;

	XoSetAttr(ar[4],XoAttrRefName,"href",0) ;
	XoSetAttr(ar[4],XoAttrRefTypeName,"XoString",0) ;
	XoSetAttr(ar[4],XoAttrRefList,(void *)0,0) ;
	XoSetAttr(ar[4],XoAttrRefLgMax,(void *)-1,0) ;
	XoSetAttr(ar[4],XoAttrRefNoSave,(void *)0,0) ;
	XoSetAttr(ar[4],XoAttrRefSaveUnset,(void *)0,0) ;
	XoSetAttr(ar[4],XoAttrRefEmbAttr,(void *)1,0) ;
	XoSetAttr(ar[4],XoAttrRefWrapHidden,(void *)0,0) ;
	XoSetAttr(ar[4],XoAttrRefShDico,(void *)0,0) ;
	XoAddInAttr(or,XoObjRefListAttr,ar[4],0) ;

	XoSetAttr(ar[5],XoAttrRefName,"is",0) ;
	XoSetAttr(ar[5],XoAttrRefTypeName,"XoString",0) ;
	XoSetAttr(ar[5],XoAttrRefList,(void *)0,0) ;
	XoSetAttr(ar[5],XoAttrRefLgMax,(void *)-1,0) ;
	XoSetAttr(ar[5],XoAttrRefNoSave,(void *)0,0) ;
	XoSetAttr(ar[5],XoAttrRefSaveUnset,(void *)0,0) ;
	XoSetAttr(ar[5],XoAttrRefEmbAttr,(void *)1,0) ;
	XoSetAttr(ar[5],XoAttrRefWrapHidden,(void *)0,0) ;
	XoSetAttr(ar[5],XoAttrRefShDico,(void *)0,0) ;
	XoAddInAttr(or,XoObjRefListAttr,ar[5],0) ;

	XoSetAttr(ar[6],XoAttrRefName,"null",0) ;
	XoSetAttr(ar[6],XoAttrRefTypeName,"XoString",0) ;
	XoSetAttr(ar[6],XoAttrRefList,(void *)0,0) ;
	XoSetAttr(ar[6],XoAttrRefLgMax,(void *)5,0) ;
	XoSetAttr(ar[6],XoAttrRefNoSave,(void *)0,0) ;
	XoSetAttr(ar[6],XoAttrRefSaveUnset,(void *)0,0) ;
	XoSetAttr(ar[6],XoAttrRefEmbAttr,(void *)1,0) ;
	XoSetAttr(ar[6],XoAttrRefWrapHidden,(void *)0,0) ;
	XoSetAttr(ar[6],XoAttrRefShDico,(void *)0,0) ;
	XoAddInAttr(or,XoObjRefListAttr,ar[6],0) ;

	XoSetAttr(ar[7],XoAttrRefName,"icon",0) ;
	XoSetAttr(ar[7],XoAttrRefTypeName,"XoString",0) ;
	XoSetAttr(ar[7],XoAttrRefList,(void *)0,0) ;
	XoSetAttr(ar[7],XoAttrRefLgMax,(void *)-1,0) ;
	XoSetAttr(ar[7],XoAttrRefNoSave,(void *)0,0) ;
	XoSetAttr(ar[7],XoAttrRefSaveUnset,(void *)0,0) ;
	XoSetAttr(ar[7],XoAttrRefEmbAttr,(void *)1,0) ;
	XoSetAttr(ar[7],XoAttrRefWrapHidden,(void *)0,0) ;
	XoSetAttr(ar[7],XoAttrRefShDico,(void *)0,0) ;
	XoAddInAttr(or,XoObjRefListAttr,ar[7],0) ;

	XoSetAttr(ar[8],XoAttrRefName,"displayName",0) ;
	XoSetAttr(ar[8],XoAttrRefTypeName,"XoString",0) ;
	XoSetAttr(ar[8],XoAttrRefList,(void *)0,0) ;
	XoSetAttr(ar[8],XoAttrRefLgMax,(void *)-1,0) ;
	XoSetAttr(ar[8],XoAttrRefNoSave,(void *)0,0) ;
	XoSetAttr(ar[8],XoAttrRefSaveUnset,(void *)0,0) ;
	XoSetAttr(ar[8],XoAttrRefEmbAttr,(void *)1,0) ;
	XoSetAttr(ar[8],XoAttrRefWrapHidden,(void *)0,0) ;
	XoSetAttr(ar[8],XoAttrRefShDico,(void *)0,0) ;
	XoAddInAttr(or,XoObjRefListAttr,ar[8],0) ;

	XoSetAttr(ar[9],XoAttrRefName,"display",0) ;
	XoSetAttr(ar[9],XoAttrRefTypeName,"XoString",0) ;
	XoSetAttr(ar[9],XoAttrRefList,(void *)0,0) ;
	XoSetAttr(ar[9],XoAttrRefLgMax,(void *)-1,0) ;
	XoSetAttr(ar[9],XoAttrRefNoSave,(void *)0,0) ;
	XoSetAttr(ar[9],XoAttrRefSaveUnset,(void *)0,0) ;
	XoSetAttr(ar[9],XoAttrRefEmbAttr,(void *)1,0) ;
	XoSetAttr(ar[9],XoAttrRefWrapHidden,(void *)0,0) ;
	XoSetAttr(ar[9],XoAttrRefShDico,(void *)0,0) ;
	XoAddInAttr(or,XoObjRefListAttr,ar[9],0) ;

	XoSetAttr(ar[10],XoAttrRefName,"writable",0) ;
	XoSetAttr(ar[10],XoAttrRefTypeName,"XoString",0) ;
	XoSetAttr(ar[10],XoAttrRefList,(void *)0,0) ;
	XoSetAttr(ar[10],XoAttrRefLgMax,(void *)5,0) ;
	XoSetAttr(ar[10],XoAttrRefNoSave,(void *)0,0) ;
	XoSetAttr(ar[10],XoAttrRefSaveUnset,(void *)0,0) ;
	XoSetAttr(ar[10],XoAttrRefEmbAttr,(void *)1,0) ;
	XoSetAttr(ar[10],XoAttrRefWrapHidden,(void *)0,0) ;
	XoSetAttr(ar[10],XoAttrRefShDico,(void *)0,0) ;
	XoAddInAttr(or,XoObjRefListAttr,ar[10],0) ;

	XoSetAttr(ar[11],XoAttrRefName,"status",0) ;
	XoSetAttr(ar[11],XoAttrRefTypeName,"XoString",0) ;
	XoSetAttr(ar[11],XoAttrRefList,(void *)0,0) ;
	XoSetAttr(ar[11],XoAttrRefLgMax,(void *)-1,0) ;
	XoSetAttr(ar[11],XoAttrRefNoSave,(void *)0,0) ;
	XoSetAttr(ar[11],XoAttrRefSaveUnset,(void *)0,0) ;
	XoSetAttr(ar[11],XoAttrRefEmbAttr,(void *)1,0) ;
	XoSetAttr(ar[11],XoAttrRefWrapHidden,(void *)0,0) ;
	XoSetAttr(ar[11],XoAttrRefShDico,(void *)0,0) ;
	XoAddInAttr(or,XoObjRefListAttr,ar[11],0) ;

	XoSetAttr(ar[12],XoAttrRefName,"wrapper$",0) ;
	XoSetAttr(ar[12],XoAttrRefTypeName,"XoVoidObj",0) ;
	XoSetAttr(ar[12],XoAttrRefList,(void *)1,0) ;
	XoSetAttr(ar[12],XoAttrRefLgMax,(void *)-1,0) ;
	XoSetAttr(ar[12],XoAttrRefNoSave,(void *)0,0) ;
	XoSetAttr(ar[12],XoAttrRefSaveUnset,(void *)0,0) ;
	XoSetAttr(ar[12],XoAttrRefEmbAttr,(void *)0,0) ;
	XoSetAttr(ar[12],XoAttrRefWrapHidden,(void *)0,0) ;
	XoSetAttr(ar[12],XoAttrRefShDico,(void *)0,0) ;
	XoAddInAttr(or,XoObjRefListAttr,ar[12],0) ;

	XoSetAttr(ar[13],XoAttrRefName,"in",0) ;
	XoSetAttr(ar[13],XoAttrRefTypeName,"XoString",0) ;
	XoSetAttr(ar[13],XoAttrRefList,(void *)0,0) ;
	XoSetAttr(ar[13],XoAttrRefLgMax,(void *)-1,0) ;
	XoSetAttr(ar[13],XoAttrRefNoSave,(void *)0,0) ;
	XoSetAttr(ar[13],XoAttrRefSaveUnset,(void *)0,0) ;
	XoSetAttr(ar[13],XoAttrRefEmbAttr,(void *)1,0) ;
	XoSetAttr(ar[13],XoAttrRefWrapHidden,(void *)0,0) ;
	XoSetAttr(ar[13],XoAttrRefShDico,(void *)0,0) ;
	XoAddInAttr(or,XoObjRefListAttr,ar[13],0) ;

	XoSetAttr(ar[14],XoAttrRefName,"of",0) ;
	XoSetAttr(ar[14],XoAttrRefTypeName,"XoString",0) ;
	XoSetAttr(ar[14],XoAttrRefList,(void *)0,0) ;
	XoSetAttr(ar[14],XoAttrRefLgMax,(void *)-1,0) ;
	XoSetAttr(ar[14],XoAttrRefNoSave,(void *)0,0) ;
	XoSetAttr(ar[14],XoAttrRefSaveUnset,(void *)0,0) ;
	XoSetAttr(ar[14],XoAttrRefEmbAttr,(void *)1,0) ;
	XoSetAttr(ar[14],XoAttrRefWrapHidden,(void *)0,0) ;
	XoSetAttr(ar[14],XoAttrRefShDico,(void *)0,0) ;
	XoAddInAttr(or,XoObjRefListAttr,ar[14],0) ;

	XoSetAttr(or,XoObjRefNbAttr,(void *)15,0) ;
	ret = -100 ;
	if (XoAddObjRef(or)<0) goto end ;
	ret = 0 ;
	end :
	for (i=0;i<15;i++)
		if (ar[i]) XoFree(ar[i],0) ;
	if (or) XoFree(or,0) ;
	return	ret ;
}
static	int32	GenXo_o:ref(void)
{
	int32	ret ;
	int32	i ;
	void	*or ;
	void	*ar[1] ;

	memset(ar,0,sizeof(ar));
	ret = -1 ;
	if (!(or=XoNew(XoObjRef))) goto end ;
	ret = -2 ;
	for (i=0;i<0;i++)
		if (!(ar[i]=XoNew(XoAttrRef))) goto end ;
	ret = -3 ;
	XoSetAttr(or,XoObjRefType,(void *)1,0) ;
	XoSetAttr(or,XoObjRefName,"o:ref",0) ;
	XoSetAttr(or,XoObjRefRoot,(void *)0,0) ;

	XoSetAttr(ar[0],XoAttrRefName,"name$",0) ;
	XoSetAttr(ar[0],XoAttrRefTypeName,"XoString",0) ;
	XoSetAttr(ar[0],XoAttrRefList,(void *)0,0) ;
	XoSetAttr(ar[0],XoAttrRefLgMax,(void *)-1,0) ;
	XoSetAttr(ar[0],XoAttrRefNoSave,(void *)0,0) ;
	XoSetAttr(ar[0],XoAttrRefSaveUnset,(void *)0,0) ;
	XoSetAttr(ar[0],XoAttrRefEmbAttr,(void *)0,0) ;
	XoSetAttr(ar[0],XoAttrRefWrapHidden,(void *)0,0) ;
	XoSetAttr(ar[0],XoAttrRefShDico,(void *)0,0) ;
	XoAddInAttr(or,XoObjRefListAttr,ar[0],0) ;

	XoSetAttr(ar[1],XoAttrRefName,"xmlns",0) ;
	XoSetAttr(ar[1],XoAttrRefTypeName,"XoString",0) ;
	XoSetAttr(ar[1],XoAttrRefList,(void *)0,0) ;
	XoSetAttr(ar[1],XoAttrRefLgMax,(void *)-1,0) ;
	XoSetAttr(ar[1],XoAttrRefNoSave,(void *)0,0) ;
	XoSetAttr(ar[1],XoAttrRefSaveUnset,(void *)0,0) ;
	XoSetAttr(ar[1],XoAttrRefEmbAttr,(void *)1,0) ;
	XoSetAttr(ar[1],XoAttrRefWrapHidden,(void *)0,0) ;
	XoSetAttr(ar[1],XoAttrRefShDico,(void *)1,0) ;
	XoAddInAttr(or,XoObjRefListAttr,ar[1],0) ;

	XoSetAttr(ar[2],XoAttrRefName,"custom$",0) ;
	XoSetAttr(ar[2],XoAttrRefTypeName,"o:t_xml_obix_attr",0) ;
	XoSetAttr(ar[2],XoAttrRefList,(void *)1,0) ;
	XoSetAttr(ar[2],XoAttrRefLgMax,(void *)-1,0) ;
	XoSetAttr(ar[2],XoAttrRefNoSave,(void *)0,0) ;
	XoSetAttr(ar[2],XoAttrRefSaveUnset,(void *)0,0) ;
	XoSetAttr(ar[2],XoAttrRefEmbAttr,(void *)1,0) ;
	XoSetAttr(ar[2],XoAttrRefWrapHidden,(void *)0,0) ;
	XoSetAttr(ar[2],XoAttrRefShDico,(void *)0,0) ;
	XoAddInAttr(or,XoObjRefListAttr,ar[2],0) ;

	XoSetAttr(ar[3],XoAttrRefName,"name",0) ;
	XoSetAttr(ar[3],XoAttrRefTypeName,"XoString",0) ;
	XoSetAttr(ar[3],XoAttrRefList,(void *)0,0) ;
	XoSetAttr(ar[3],XoAttrRefLgMax,(void *)-1,0) ;
	XoSetAttr(ar[3],XoAttrRefNoSave,(void *)0,0) ;
	XoSetAttr(ar[3],XoAttrRefSaveUnset,(void *)0,0) ;
	XoSetAttr(ar[3],XoAttrRefEmbAttr,(void *)1,0) ;
	XoSetAttr(ar[3],XoAttrRefWrapHidden,(void *)0,0) ;
	XoSetAttr(ar[3],XoAttrRefShDico,(void *)0,0) ;
	XoAddInAttr(or,XoObjRefListAttr,ar[3],0) ;

	XoSetAttr(ar[4],XoAttrRefName,"href",0) ;
	XoSetAttr(ar[4],XoAttrRefTypeName,"XoString",0) ;
	XoSetAttr(ar[4],XoAttrRefList,(void *)0,0) ;
	XoSetAttr(ar[4],XoAttrRefLgMax,(void *)-1,0) ;
	XoSetAttr(ar[4],XoAttrRefNoSave,(void *)0,0) ;
	XoSetAttr(ar[4],XoAttrRefSaveUnset,(void *)0,0) ;
	XoSetAttr(ar[4],XoAttrRefEmbAttr,(void *)1,0) ;
	XoSetAttr(ar[4],XoAttrRefWrapHidden,(void *)0,0) ;
	XoSetAttr(ar[4],XoAttrRefShDico,(void *)0,0) ;
	XoAddInAttr(or,XoObjRefListAttr,ar[4],0) ;

	XoSetAttr(ar[5],XoAttrRefName,"is",0) ;
	XoSetAttr(ar[5],XoAttrRefTypeName,"XoString",0) ;
	XoSetAttr(ar[5],XoAttrRefList,(void *)0,0) ;
	XoSetAttr(ar[5],XoAttrRefLgMax,(void *)-1,0) ;
	XoSetAttr(ar[5],XoAttrRefNoSave,(void *)0,0) ;
	XoSetAttr(ar[5],XoAttrRefSaveUnset,(void *)0,0) ;
	XoSetAttr(ar[5],XoAttrRefEmbAttr,(void *)1,0) ;
	XoSetAttr(ar[5],XoAttrRefWrapHidden,(void *)0,0) ;
	XoSetAttr(ar[5],XoAttrRefShDico,(void *)0,0) ;
	XoAddInAttr(or,XoObjRefListAttr,ar[5],0) ;

	XoSetAttr(ar[6],XoAttrRefName,"null",0) ;
	XoSetAttr(ar[6],XoAttrRefTypeName,"XoString",0) ;
	XoSetAttr(ar[6],XoAttrRefList,(void *)0,0) ;
	XoSetAttr(ar[6],XoAttrRefLgMax,(void *)5,0) ;
	XoSetAttr(ar[6],XoAttrRefNoSave,(void *)0,0) ;
	XoSetAttr(ar[6],XoAttrRefSaveUnset,(void *)0,0) ;
	XoSetAttr(ar[6],XoAttrRefEmbAttr,(void *)1,0) ;
	XoSetAttr(ar[6],XoAttrRefWrapHidden,(void *)0,0) ;
	XoSetAttr(ar[6],XoAttrRefShDico,(void *)0,0) ;
	XoAddInAttr(or,XoObjRefListAttr,ar[6],0) ;

	XoSetAttr(ar[7],XoAttrRefName,"icon",0) ;
	XoSetAttr(ar[7],XoAttrRefTypeName,"XoString",0) ;
	XoSetAttr(ar[7],XoAttrRefList,(void *)0,0) ;
	XoSetAttr(ar[7],XoAttrRefLgMax,(void *)-1,0) ;
	XoSetAttr(ar[7],XoAttrRefNoSave,(void *)0,0) ;
	XoSetAttr(ar[7],XoAttrRefSaveUnset,(void *)0,0) ;
	XoSetAttr(ar[7],XoAttrRefEmbAttr,(void *)1,0) ;
	XoSetAttr(ar[7],XoAttrRefWrapHidden,(void *)0,0) ;
	XoSetAttr(ar[7],XoAttrRefShDico,(void *)0,0) ;
	XoAddInAttr(or,XoObjRefListAttr,ar[7],0) ;

	XoSetAttr(ar[8],XoAttrRefName,"displayName",0) ;
	XoSetAttr(ar[8],XoAttrRefTypeName,"XoString",0) ;
	XoSetAttr(ar[8],XoAttrRefList,(void *)0,0) ;
	XoSetAttr(ar[8],XoAttrRefLgMax,(void *)-1,0) ;
	XoSetAttr(ar[8],XoAttrRefNoSave,(void *)0,0) ;
	XoSetAttr(ar[8],XoAttrRefSaveUnset,(void *)0,0) ;
	XoSetAttr(ar[8],XoAttrRefEmbAttr,(void *)1,0) ;
	XoSetAttr(ar[8],XoAttrRefWrapHidden,(void *)0,0) ;
	XoSetAttr(ar[8],XoAttrRefShDico,(void *)0,0) ;
	XoAddInAttr(or,XoObjRefListAttr,ar[8],0) ;

	XoSetAttr(ar[9],XoAttrRefName,"display",0) ;
	XoSetAttr(ar[9],XoAttrRefTypeName,"XoString",0) ;
	XoSetAttr(ar[9],XoAttrRefList,(void *)0,0) ;
	XoSetAttr(ar[9],XoAttrRefLgMax,(void *)-1,0) ;
	XoSetAttr(ar[9],XoAttrRefNoSave,(void *)0,0) ;
	XoSetAttr(ar[9],XoAttrRefSaveUnset,(void *)0,0) ;
	XoSetAttr(ar[9],XoAttrRefEmbAttr,(void *)1,0) ;
	XoSetAttr(ar[9],XoAttrRefWrapHidden,(void *)0,0) ;
	XoSetAttr(ar[9],XoAttrRefShDico,(void *)0,0) ;
	XoAddInAttr(or,XoObjRefListAttr,ar[9],0) ;

	XoSetAttr(ar[10],XoAttrRefName,"writable",0) ;
	XoSetAttr(ar[10],XoAttrRefTypeName,"XoString",0) ;
	XoSetAttr(ar[10],XoAttrRefList,(void *)0,0) ;
	XoSetAttr(ar[10],XoAttrRefLgMax,(void *)5,0) ;
	XoSetAttr(ar[10],XoAttrRefNoSave,(void *)0,0) ;
	XoSetAttr(ar[10],XoAttrRefSaveUnset,(void *)0,0) ;
	XoSetAttr(ar[10],XoAttrRefEmbAttr,(void *)1,0) ;
	XoSetAttr(ar[10],XoAttrRefWrapHidden,(void *)0,0) ;
	XoSetAttr(ar[10],XoAttrRefShDico,(void *)0,0) ;
	XoAddInAttr(or,XoObjRefListAttr,ar[10],0) ;

	XoSetAttr(ar[11],XoAttrRefName,"status",0) ;
	XoSetAttr(ar[11],XoAttrRefTypeName,"XoString",0) ;
	XoSetAttr(ar[11],XoAttrRefList,(void *)0,0) ;
	XoSetAttr(ar[11],XoAttrRefLgMax,(void *)-1,0) ;
	XoSetAttr(ar[11],XoAttrRefNoSave,(void *)0,0) ;
	XoSetAttr(ar[11],XoAttrRefSaveUnset,(void *)0,0) ;
	XoSetAttr(ar[11],XoAttrRefEmbAttr,(void *)1,0) ;
	XoSetAttr(ar[11],XoAttrRefWrapHidden,(void *)0,0) ;
	XoSetAttr(ar[11],XoAttrRefShDico,(void *)0,0) ;
	XoAddInAttr(or,XoObjRefListAttr,ar[11],0) ;

	XoSetAttr(ar[12],XoAttrRefName,"wrapper$",0) ;
	XoSetAttr(ar[12],XoAttrRefTypeName,"XoVoidObj",0) ;
	XoSetAttr(ar[12],XoAttrRefList,(void *)1,0) ;
	XoSetAttr(ar[12],XoAttrRefLgMax,(void *)-1,0) ;
	XoSetAttr(ar[12],XoAttrRefNoSave,(void *)0,0) ;
	XoSetAttr(ar[12],XoAttrRefSaveUnset,(void *)0,0) ;
	XoSetAttr(ar[12],XoAttrRefEmbAttr,(void *)0,0) ;
	XoSetAttr(ar[12],XoAttrRefWrapHidden,(void *)0,0) ;
	XoSetAttr(ar[12],XoAttrRefShDico,(void *)0,0) ;
	XoAddInAttr(or,XoObjRefListAttr,ar[12],0) ;

	XoSetAttr(or,XoObjRefNbAttr,(void *)13,0) ;
	ret = -100 ;
	if (XoAddObjRef(or)<0) goto end ;
	ret = 0 ;
	end :
	for (i=0;i<13;i++)
		if (ar[i]) XoFree(ar[i],0) ;
	if (or) XoFree(or,0) ;
	return	ret ;
}
static	int32	GenXo_o:err(void)
{
	int32	ret ;
	int32	i ;
	void	*or ;
	void	*ar[1] ;

	memset(ar,0,sizeof(ar));
	ret = -1 ;
	if (!(or=XoNew(XoObjRef))) goto end ;
	ret = -2 ;
	for (i=0;i<0;i++)
		if (!(ar[i]=XoNew(XoAttrRef))) goto end ;
	ret = -3 ;
	XoSetAttr(or,XoObjRefType,(void *)1,0) ;
	XoSetAttr(or,XoObjRefName,"o:err",0) ;
	XoSetAttr(or,XoObjRefRoot,(void *)0,0) ;

	XoSetAttr(ar[0],XoAttrRefName,"name$",0) ;
	XoSetAttr(ar[0],XoAttrRefTypeName,"XoString",0) ;
	XoSetAttr(ar[0],XoAttrRefList,(void *)0,0) ;
	XoSetAttr(ar[0],XoAttrRefLgMax,(void *)-1,0) ;
	XoSetAttr(ar[0],XoAttrRefNoSave,(void *)0,0) ;
	XoSetAttr(ar[0],XoAttrRefSaveUnset,(void *)0,0) ;
	XoSetAttr(ar[0],XoAttrRefEmbAttr,(void *)0,0) ;
	XoSetAttr(ar[0],XoAttrRefWrapHidden,(void *)0,0) ;
	XoSetAttr(ar[0],XoAttrRefShDico,(void *)0,0) ;
	XoAddInAttr(or,XoObjRefListAttr,ar[0],0) ;

	XoSetAttr(ar[1],XoAttrRefName,"xmlns",0) ;
	XoSetAttr(ar[1],XoAttrRefTypeName,"XoString",0) ;
	XoSetAttr(ar[1],XoAttrRefList,(void *)0,0) ;
	XoSetAttr(ar[1],XoAttrRefLgMax,(void *)-1,0) ;
	XoSetAttr(ar[1],XoAttrRefNoSave,(void *)0,0) ;
	XoSetAttr(ar[1],XoAttrRefSaveUnset,(void *)0,0) ;
	XoSetAttr(ar[1],XoAttrRefEmbAttr,(void *)1,0) ;
	XoSetAttr(ar[1],XoAttrRefWrapHidden,(void *)0,0) ;
	XoSetAttr(ar[1],XoAttrRefShDico,(void *)1,0) ;
	XoAddInAttr(or,XoObjRefListAttr,ar[1],0) ;

	XoSetAttr(ar[2],XoAttrRefName,"custom$",0) ;
	XoSetAttr(ar[2],XoAttrRefTypeName,"o:t_xml_obix_attr",0) ;
	XoSetAttr(ar[2],XoAttrRefList,(void *)1,0) ;
	XoSetAttr(ar[2],XoAttrRefLgMax,(void *)-1,0) ;
	XoSetAttr(ar[2],XoAttrRefNoSave,(void *)0,0) ;
	XoSetAttr(ar[2],XoAttrRefSaveUnset,(void *)0,0) ;
	XoSetAttr(ar[2],XoAttrRefEmbAttr,(void *)1,0) ;
	XoSetAttr(ar[2],XoAttrRefWrapHidden,(void *)0,0) ;
	XoSetAttr(ar[2],XoAttrRefShDico,(void *)0,0) ;
	XoAddInAttr(or,XoObjRefListAttr,ar[2],0) ;

	XoSetAttr(ar[3],XoAttrRefName,"name",0) ;
	XoSetAttr(ar[3],XoAttrRefTypeName,"XoString",0) ;
	XoSetAttr(ar[3],XoAttrRefList,(void *)0,0) ;
	XoSetAttr(ar[3],XoAttrRefLgMax,(void *)-1,0) ;
	XoSetAttr(ar[3],XoAttrRefNoSave,(void *)0,0) ;
	XoSetAttr(ar[3],XoAttrRefSaveUnset,(void *)0,0) ;
	XoSetAttr(ar[3],XoAttrRefEmbAttr,(void *)1,0) ;
	XoSetAttr(ar[3],XoAttrRefWrapHidden,(void *)0,0) ;
	XoSetAttr(ar[3],XoAttrRefShDico,(void *)0,0) ;
	XoAddInAttr(or,XoObjRefListAttr,ar[3],0) ;

	XoSetAttr(ar[4],XoAttrRefName,"href",0) ;
	XoSetAttr(ar[4],XoAttrRefTypeName,"XoString",0) ;
	XoSetAttr(ar[4],XoAttrRefList,(void *)0,0) ;
	XoSetAttr(ar[4],XoAttrRefLgMax,(void *)-1,0) ;
	XoSetAttr(ar[4],XoAttrRefNoSave,(void *)0,0) ;
	XoSetAttr(ar[4],XoAttrRefSaveUnset,(void *)0,0) ;
	XoSetAttr(ar[4],XoAttrRefEmbAttr,(void *)1,0) ;
	XoSetAttr(ar[4],XoAttrRefWrapHidden,(void *)0,0) ;
	XoSetAttr(ar[4],XoAttrRefShDico,(void *)0,0) ;
	XoAddInAttr(or,XoObjRefListAttr,ar[4],0) ;

	XoSetAttr(ar[5],XoAttrRefName,"is",0) ;
	XoSetAttr(ar[5],XoAttrRefTypeName,"XoString",0) ;
	XoSetAttr(ar[5],XoAttrRefList,(void *)0,0) ;
	XoSetAttr(ar[5],XoAttrRefLgMax,(void *)-1,0) ;
	XoSetAttr(ar[5],XoAttrRefNoSave,(void *)0,0) ;
	XoSetAttr(ar[5],XoAttrRefSaveUnset,(void *)0,0) ;
	XoSetAttr(ar[5],XoAttrRefEmbAttr,(void *)1,0) ;
	XoSetAttr(ar[5],XoAttrRefWrapHidden,(void *)0,0) ;
	XoSetAttr(ar[5],XoAttrRefShDico,(void *)0,0) ;
	XoAddInAttr(or,XoObjRefListAttr,ar[5],0) ;

	XoSetAttr(ar[6],XoAttrRefName,"null",0) ;
	XoSetAttr(ar[6],XoAttrRefTypeName,"XoString",0) ;
	XoSetAttr(ar[6],XoAttrRefList,(void *)0,0) ;
	XoSetAttr(ar[6],XoAttrRefLgMax,(void *)5,0) ;
	XoSetAttr(ar[6],XoAttrRefNoSave,(void *)0,0) ;
	XoSetAttr(ar[6],XoAttrRefSaveUnset,(void *)0,0) ;
	XoSetAttr(ar[6],XoAttrRefEmbAttr,(void *)1,0) ;
	XoSetAttr(ar[6],XoAttrRefWrapHidden,(void *)0,0) ;
	XoSetAttr(ar[6],XoAttrRefShDico,(void *)0,0) ;
	XoAddInAttr(or,XoObjRefListAttr,ar[6],0) ;

	XoSetAttr(ar[7],XoAttrRefName,"icon",0) ;
	XoSetAttr(ar[7],XoAttrRefTypeName,"XoString",0) ;
	XoSetAttr(ar[7],XoAttrRefList,(void *)0,0) ;
	XoSetAttr(ar[7],XoAttrRefLgMax,(void *)-1,0) ;
	XoSetAttr(ar[7],XoAttrRefNoSave,(void *)0,0) ;
	XoSetAttr(ar[7],XoAttrRefSaveUnset,(void *)0,0) ;
	XoSetAttr(ar[7],XoAttrRefEmbAttr,(void *)1,0) ;
	XoSetAttr(ar[7],XoAttrRefWrapHidden,(void *)0,0) ;
	XoSetAttr(ar[7],XoAttrRefShDico,(void *)0,0) ;
	XoAddInAttr(or,XoObjRefListAttr,ar[7],0) ;

	XoSetAttr(ar[8],XoAttrRefName,"displayName",0) ;
	XoSetAttr(ar[8],XoAttrRefTypeName,"XoString",0) ;
	XoSetAttr(ar[8],XoAttrRefList,(void *)0,0) ;
	XoSetAttr(ar[8],XoAttrRefLgMax,(void *)-1,0) ;
	XoSetAttr(ar[8],XoAttrRefNoSave,(void *)0,0) ;
	XoSetAttr(ar[8],XoAttrRefSaveUnset,(void *)0,0) ;
	XoSetAttr(ar[8],XoAttrRefEmbAttr,(void *)1,0) ;
	XoSetAttr(ar[8],XoAttrRefWrapHidden,(void *)0,0) ;
	XoSetAttr(ar[8],XoAttrRefShDico,(void *)0,0) ;
	XoAddInAttr(or,XoObjRefListAttr,ar[8],0) ;

	XoSetAttr(ar[9],XoAttrRefName,"display",0) ;
	XoSetAttr(ar[9],XoAttrRefTypeName,"XoString",0) ;
	XoSetAttr(ar[9],XoAttrRefList,(void *)0,0) ;
	XoSetAttr(ar[9],XoAttrRefLgMax,(void *)-1,0) ;
	XoSetAttr(ar[9],XoAttrRefNoSave,(void *)0,0) ;
	XoSetAttr(ar[9],XoAttrRefSaveUnset,(void *)0,0) ;
	XoSetAttr(ar[9],XoAttrRefEmbAttr,(void *)1,0) ;
	XoSetAttr(ar[9],XoAttrRefWrapHidden,(void *)0,0) ;
	XoSetAttr(ar[9],XoAttrRefShDico,(void *)0,0) ;
	XoAddInAttr(or,XoObjRefListAttr,ar[9],0) ;

	XoSetAttr(ar[10],XoAttrRefName,"writable",0) ;
	XoSetAttr(ar[10],XoAttrRefTypeName,"XoString",0) ;
	XoSetAttr(ar[10],XoAttrRefList,(void *)0,0) ;
	XoSetAttr(ar[10],XoAttrRefLgMax,(void *)5,0) ;
	XoSetAttr(ar[10],XoAttrRefNoSave,(void *)0,0) ;
	XoSetAttr(ar[10],XoAttrRefSaveUnset,(void *)0,0) ;
	XoSetAttr(ar[10],XoAttrRefEmbAttr,(void *)1,0) ;
	XoSetAttr(ar[10],XoAttrRefWrapHidden,(void *)0,0) ;
	XoSetAttr(ar[10],XoAttrRefShDico,(void *)0,0) ;
	XoAddInAttr(or,XoObjRefListAttr,ar[10],0) ;

	XoSetAttr(ar[11],XoAttrRefName,"status",0) ;
	XoSetAttr(ar[11],XoAttrRefTypeName,"XoString",0) ;
	XoSetAttr(ar[11],XoAttrRefList,(void *)0,0) ;
	XoSetAttr(ar[11],XoAttrRefLgMax,(void *)-1,0) ;
	XoSetAttr(ar[11],XoAttrRefNoSave,(void *)0,0) ;
	XoSetAttr(ar[11],XoAttrRefSaveUnset,(void *)0,0) ;
	XoSetAttr(ar[11],XoAttrRefEmbAttr,(void *)1,0) ;
	XoSetAttr(ar[11],XoAttrRefWrapHidden,(void *)0,0) ;
	XoSetAttr(ar[11],XoAttrRefShDico,(void *)0,0) ;
	XoAddInAttr(or,XoObjRefListAttr,ar[11],0) ;

	XoSetAttr(ar[12],XoAttrRefName,"wrapper$",0) ;
	XoSetAttr(ar[12],XoAttrRefTypeName,"XoVoidObj",0) ;
	XoSetAttr(ar[12],XoAttrRefList,(void *)1,0) ;
	XoSetAttr(ar[12],XoAttrRefLgMax,(void *)-1,0) ;
	XoSetAttr(ar[12],XoAttrRefNoSave,(void *)0,0) ;
	XoSetAttr(ar[12],XoAttrRefSaveUnset,(void *)0,0) ;
	XoSetAttr(ar[12],XoAttrRefEmbAttr,(void *)0,0) ;
	XoSetAttr(ar[12],XoAttrRefWrapHidden,(void *)0,0) ;
	XoSetAttr(ar[12],XoAttrRefShDico,(void *)0,0) ;
	XoAddInAttr(or,XoObjRefListAttr,ar[12],0) ;

	XoSetAttr(or,XoObjRefNbAttr,(void *)13,0) ;
	ret = -100 ;
	if (XoAddObjRef(or)<0) goto end ;
	ret = 0 ;
	end :
	for (i=0;i<13;i++)
		if (ar[i]) XoFree(ar[i],0) ;
	if (or) XoFree(or,0) ;
	return	ret ;
}
static	int32	GenXo_o:bool(void)
{
	int32	ret ;
	int32	i ;
	void	*or ;
	void	*ar[2] ;

	memset(ar,0,sizeof(ar));
	ret = -1 ;
	if (!(or=XoNew(XoObjRef))) goto end ;
	ret = -2 ;
	for (i=0;i<1;i++)
		if (!(ar[i]=XoNew(XoAttrRef))) goto end ;
	ret = -3 ;
	XoSetAttr(or,XoObjRefType,(void *)1,0) ;
	XoSetAttr(or,XoObjRefName,"o:bool",0) ;
	XoSetAttr(or,XoObjRefRoot,(void *)0,0) ;

	XoSetAttr(ar[0],XoAttrRefName,"name$",0) ;
	XoSetAttr(ar[0],XoAttrRefTypeName,"XoString",0) ;
	XoSetAttr(ar[0],XoAttrRefList,(void *)0,0) ;
	XoSetAttr(ar[0],XoAttrRefLgMax,(void *)-1,0) ;
	XoSetAttr(ar[0],XoAttrRefNoSave,(void *)0,0) ;
	XoSetAttr(ar[0],XoAttrRefSaveUnset,(void *)0,0) ;
	XoSetAttr(ar[0],XoAttrRefEmbAttr,(void *)0,0) ;
	XoSetAttr(ar[0],XoAttrRefWrapHidden,(void *)0,0) ;
	XoSetAttr(ar[0],XoAttrRefShDico,(void *)0,0) ;
	XoAddInAttr(or,XoObjRefListAttr,ar[0],0) ;

	XoSetAttr(ar[1],XoAttrRefName,"xmlns",0) ;
	XoSetAttr(ar[1],XoAttrRefTypeName,"XoString",0) ;
	XoSetAttr(ar[1],XoAttrRefList,(void *)0,0) ;
	XoSetAttr(ar[1],XoAttrRefLgMax,(void *)-1,0) ;
	XoSetAttr(ar[1],XoAttrRefNoSave,(void *)0,0) ;
	XoSetAttr(ar[1],XoAttrRefSaveUnset,(void *)0,0) ;
	XoSetAttr(ar[1],XoAttrRefEmbAttr,(void *)1,0) ;
	XoSetAttr(ar[1],XoAttrRefWrapHidden,(void *)0,0) ;
	XoSetAttr(ar[1],XoAttrRefShDico,(void *)1,0) ;
	XoAddInAttr(or,XoObjRefListAttr,ar[1],0) ;

	XoSetAttr(ar[2],XoAttrRefName,"custom$",0) ;
	XoSetAttr(ar[2],XoAttrRefTypeName,"o:t_xml_obix_attr",0) ;
	XoSetAttr(ar[2],XoAttrRefList,(void *)1,0) ;
	XoSetAttr(ar[2],XoAttrRefLgMax,(void *)-1,0) ;
	XoSetAttr(ar[2],XoAttrRefNoSave,(void *)0,0) ;
	XoSetAttr(ar[2],XoAttrRefSaveUnset,(void *)0,0) ;
	XoSetAttr(ar[2],XoAttrRefEmbAttr,(void *)1,0) ;
	XoSetAttr(ar[2],XoAttrRefWrapHidden,(void *)0,0) ;
	XoSetAttr(ar[2],XoAttrRefShDico,(void *)0,0) ;
	XoAddInAttr(or,XoObjRefListAttr,ar[2],0) ;

	XoSetAttr(ar[3],XoAttrRefName,"name",0) ;
	XoSetAttr(ar[3],XoAttrRefTypeName,"XoString",0) ;
	XoSetAttr(ar[3],XoAttrRefList,(void *)0,0) ;
	XoSetAttr(ar[3],XoAttrRefLgMax,(void *)-1,0) ;
	XoSetAttr(ar[3],XoAttrRefNoSave,(void *)0,0) ;
	XoSetAttr(ar[3],XoAttrRefSaveUnset,(void *)0,0) ;
	XoSetAttr(ar[3],XoAttrRefEmbAttr,(void *)1,0) ;
	XoSetAttr(ar[3],XoAttrRefWrapHidden,(void *)0,0) ;
	XoSetAttr(ar[3],XoAttrRefShDico,(void *)0,0) ;
	XoAddInAttr(or,XoObjRefListAttr,ar[3],0) ;

	XoSetAttr(ar[4],XoAttrRefName,"href",0) ;
	XoSetAttr(ar[4],XoAttrRefTypeName,"XoString",0) ;
	XoSetAttr(ar[4],XoAttrRefList,(void *)0,0) ;
	XoSetAttr(ar[4],XoAttrRefLgMax,(void *)-1,0) ;
	XoSetAttr(ar[4],XoAttrRefNoSave,(void *)0,0) ;
	XoSetAttr(ar[4],XoAttrRefSaveUnset,(void *)0,0) ;
	XoSetAttr(ar[4],XoAttrRefEmbAttr,(void *)1,0) ;
	XoSetAttr(ar[4],XoAttrRefWrapHidden,(void *)0,0) ;
	XoSetAttr(ar[4],XoAttrRefShDico,(void *)0,0) ;
	XoAddInAttr(or,XoObjRefListAttr,ar[4],0) ;

	XoSetAttr(ar[5],XoAttrRefName,"is",0) ;
	XoSetAttr(ar[5],XoAttrRefTypeName,"XoString",0) ;
	XoSetAttr(ar[5],XoAttrRefList,(void *)0,0) ;
	XoSetAttr(ar[5],XoAttrRefLgMax,(void *)-1,0) ;
	XoSetAttr(ar[5],XoAttrRefNoSave,(void *)0,0) ;
	XoSetAttr(ar[5],XoAttrRefSaveUnset,(void *)0,0) ;
	XoSetAttr(ar[5],XoAttrRefEmbAttr,(void *)1,0) ;
	XoSetAttr(ar[5],XoAttrRefWrapHidden,(void *)0,0) ;
	XoSetAttr(ar[5],XoAttrRefShDico,(void *)0,0) ;
	XoAddInAttr(or,XoObjRefListAttr,ar[5],0) ;

	XoSetAttr(ar[6],XoAttrRefName,"null",0) ;
	XoSetAttr(ar[6],XoAttrRefTypeName,"XoString",0) ;
	XoSetAttr(ar[6],XoAttrRefList,(void *)0,0) ;
	XoSetAttr(ar[6],XoAttrRefLgMax,(void *)5,0) ;
	XoSetAttr(ar[6],XoAttrRefNoSave,(void *)0,0) ;
	XoSetAttr(ar[6],XoAttrRefSaveUnset,(void *)0,0) ;
	XoSetAttr(ar[6],XoAttrRefEmbAttr,(void *)1,0) ;
	XoSetAttr(ar[6],XoAttrRefWrapHidden,(void *)0,0) ;
	XoSetAttr(ar[6],XoAttrRefShDico,(void *)0,0) ;
	XoAddInAttr(or,XoObjRefListAttr,ar[6],0) ;

	XoSetAttr(ar[7],XoAttrRefName,"icon",0) ;
	XoSetAttr(ar[7],XoAttrRefTypeName,"XoString",0) ;
	XoSetAttr(ar[7],XoAttrRefList,(void *)0,0) ;
	XoSetAttr(ar[7],XoAttrRefLgMax,(void *)-1,0) ;
	XoSetAttr(ar[7],XoAttrRefNoSave,(void *)0,0) ;
	XoSetAttr(ar[7],XoAttrRefSaveUnset,(void *)0,0) ;
	XoSetAttr(ar[7],XoAttrRefEmbAttr,(void *)1,0) ;
	XoSetAttr(ar[7],XoAttrRefWrapHidden,(void *)0,0) ;
	XoSetAttr(ar[7],XoAttrRefShDico,(void *)0,0) ;
	XoAddInAttr(or,XoObjRefListAttr,ar[7],0) ;

	XoSetAttr(ar[8],XoAttrRefName,"displayName",0) ;
	XoSetAttr(ar[8],XoAttrRefTypeName,"XoString",0) ;
	XoSetAttr(ar[8],XoAttrRefList,(void *)0,0) ;
	XoSetAttr(ar[8],XoAttrRefLgMax,(void *)-1,0) ;
	XoSetAttr(ar[8],XoAttrRefNoSave,(void *)0,0) ;
	XoSetAttr(ar[8],XoAttrRefSaveUnset,(void *)0,0) ;
	XoSetAttr(ar[8],XoAttrRefEmbAttr,(void *)1,0) ;
	XoSetAttr(ar[8],XoAttrRefWrapHidden,(void *)0,0) ;
	XoSetAttr(ar[8],XoAttrRefShDico,(void *)0,0) ;
	XoAddInAttr(or,XoObjRefListAttr,ar[8],0) ;

	XoSetAttr(ar[9],XoAttrRefName,"display",0) ;
	XoSetAttr(ar[9],XoAttrRefTypeName,"XoString",0) ;
	XoSetAttr(ar[9],XoAttrRefList,(void *)0,0) ;
	XoSetAttr(ar[9],XoAttrRefLgMax,(void *)-1,0) ;
	XoSetAttr(ar[9],XoAttrRefNoSave,(void *)0,0) ;
	XoSetAttr(ar[9],XoAttrRefSaveUnset,(void *)0,0) ;
	XoSetAttr(ar[9],XoAttrRefEmbAttr,(void *)1,0) ;
	XoSetAttr(ar[9],XoAttrRefWrapHidden,(void *)0,0) ;
	XoSetAttr(ar[9],XoAttrRefShDico,(void *)0,0) ;
	XoAddInAttr(or,XoObjRefListAttr,ar[9],0) ;

	XoSetAttr(ar[10],XoAttrRefName,"writable",0) ;
	XoSetAttr(ar[10],XoAttrRefTypeName,"XoString",0) ;
	XoSetAttr(ar[10],XoAttrRefList,(void *)0,0) ;
	XoSetAttr(ar[10],XoAttrRefLgMax,(void *)5,0) ;
	XoSetAttr(ar[10],XoAttrRefNoSave,(void *)0,0) ;
	XoSetAttr(ar[10],XoAttrRefSaveUnset,(void *)0,0) ;
	XoSetAttr(ar[10],XoAttrRefEmbAttr,(void *)1,0) ;
	XoSetAttr(ar[10],XoAttrRefWrapHidden,(void *)0,0) ;
	XoSetAttr(ar[10],XoAttrRefShDico,(void *)0,0) ;
	XoAddInAttr(or,XoObjRefListAttr,ar[10],0) ;

	XoSetAttr(ar[11],XoAttrRefName,"status",0) ;
	XoSetAttr(ar[11],XoAttrRefTypeName,"XoString",0) ;
	XoSetAttr(ar[11],XoAttrRefList,(void *)0,0) ;
	XoSetAttr(ar[11],XoAttrRefLgMax,(void *)-1,0) ;
	XoSetAttr(ar[11],XoAttrRefNoSave,(void *)0,0) ;
	XoSetAttr(ar[11],XoAttrRefSaveUnset,(void *)0,0) ;
	XoSetAttr(ar[11],XoAttrRefEmbAttr,(void *)1,0) ;
	XoSetAttr(ar[11],XoAttrRefWrapHidden,(void *)0,0) ;
	XoSetAttr(ar[11],XoAttrRefShDico,(void *)0,0) ;
	XoAddInAttr(or,XoObjRefListAttr,ar[11],0) ;

	XoSetAttr(ar[12],XoAttrRefName,"wrapper$",0) ;
	XoSetAttr(ar[12],XoAttrRefTypeName,"XoVoidObj",0) ;
	XoSetAttr(ar[12],XoAttrRefList,(void *)1,0) ;
	XoSetAttr(ar[12],XoAttrRefLgMax,(void *)-1,0) ;
	XoSetAttr(ar[12],XoAttrRefNoSave,(void *)0,0) ;
	XoSetAttr(ar[12],XoAttrRefSaveUnset,(void *)0,0) ;
	XoSetAttr(ar[12],XoAttrRefEmbAttr,(void *)0,0) ;
	XoSetAttr(ar[12],XoAttrRefWrapHidden,(void *)0,0) ;
	XoSetAttr(ar[12],XoAttrRefShDico,(void *)0,0) ;
	XoAddInAttr(or,XoObjRefListAttr,ar[12],0) ;

	XoSetAttr(ar[13],XoAttrRefName,"val",0) ;
	XoSetAttr(ar[13],XoAttrRefTypeName,"XoString",0) ;
	XoSetAttr(ar[13],XoAttrRefList,(void *)0,0) ;
	XoSetAttr(ar[13],XoAttrRefLgMax,(void *)5,0) ;
	XoSetAttr(ar[13],XoAttrRefNoSave,(void *)0,0) ;
	XoSetAttr(ar[13],XoAttrRefSaveUnset,(void *)0,0) ;
	XoSetAttr(ar[13],XoAttrRefEmbAttr,(void *)1,0) ;
	XoSetAttr(ar[13],XoAttrRefWrapHidden,(void *)0,0) ;
	XoSetAttr(ar[13],XoAttrRefShDico,(void *)0,0) ;
	XoAddInAttr(or,XoObjRefListAttr,ar[13],0) ;

	XoSetAttr(or,XoObjRefNbAttr,(void *)14,0) ;
	ret = -100 ;
	if (XoAddObjRef(or)<0) goto end ;
	ret = 0 ;
	end :
	for (i=0;i<14;i++)
		if (ar[i]) XoFree(ar[i],0) ;
	if (or) XoFree(or,0) ;
	return	ret ;
}
static	int32	GenXo_o:int(void)
{
	int32	ret ;
	int32	i ;
	void	*or ;
	void	*ar[5] ;

	memset(ar,0,sizeof(ar));
	ret = -1 ;
	if (!(or=XoNew(XoObjRef))) goto end ;
	ret = -2 ;
	for (i=0;i<4;i++)
		if (!(ar[i]=XoNew(XoAttrRef))) goto end ;
	ret = -3 ;
	XoSetAttr(or,XoObjRefType,(void *)1,0) ;
	XoSetAttr(or,XoObjRefName,"o:int",0) ;
	XoSetAttr(or,XoObjRefRoot,(void *)0,0) ;

	XoSetAttr(ar[0],XoAttrRefName,"name$",0) ;
	XoSetAttr(ar[0],XoAttrRefTypeName,"XoString",0) ;
	XoSetAttr(ar[0],XoAttrRefList,(void *)0,0) ;
	XoSetAttr(ar[0],XoAttrRefLgMax,(void *)-1,0) ;
	XoSetAttr(ar[0],XoAttrRefNoSave,(void *)0,0) ;
	XoSetAttr(ar[0],XoAttrRefSaveUnset,(void *)0,0) ;
	XoSetAttr(ar[0],XoAttrRefEmbAttr,(void *)0,0) ;
	XoSetAttr(ar[0],XoAttrRefWrapHidden,(void *)0,0) ;
	XoSetAttr(ar[0],XoAttrRefShDico,(void *)0,0) ;
	XoAddInAttr(or,XoObjRefListAttr,ar[0],0) ;

	XoSetAttr(ar[1],XoAttrRefName,"xmlns",0) ;
	XoSetAttr(ar[1],XoAttrRefTypeName,"XoString",0) ;
	XoSetAttr(ar[1],XoAttrRefList,(void *)0,0) ;
	XoSetAttr(ar[1],XoAttrRefLgMax,(void *)-1,0) ;
	XoSetAttr(ar[1],XoAttrRefNoSave,(void *)0,0) ;
	XoSetAttr(ar[1],XoAttrRefSaveUnset,(void *)0,0) ;
	XoSetAttr(ar[1],XoAttrRefEmbAttr,(void *)1,0) ;
	XoSetAttr(ar[1],XoAttrRefWrapHidden,(void *)0,0) ;
	XoSetAttr(ar[1],XoAttrRefShDico,(void *)1,0) ;
	XoAddInAttr(or,XoObjRefListAttr,ar[1],0) ;

	XoSetAttr(ar[2],XoAttrRefName,"custom$",0) ;
	XoSetAttr(ar[2],XoAttrRefTypeName,"o:t_xml_obix_attr",0) ;
	XoSetAttr(ar[2],XoAttrRefList,(void *)1,0) ;
	XoSetAttr(ar[2],XoAttrRefLgMax,(void *)-1,0) ;
	XoSetAttr(ar[2],XoAttrRefNoSave,(void *)0,0) ;
	XoSetAttr(ar[2],XoAttrRefSaveUnset,(void *)0,0) ;
	XoSetAttr(ar[2],XoAttrRefEmbAttr,(void *)1,0) ;
	XoSetAttr(ar[2],XoAttrRefWrapHidden,(void *)0,0) ;
	XoSetAttr(ar[2],XoAttrRefShDico,(void *)0,0) ;
	XoAddInAttr(or,XoObjRefListAttr,ar[2],0) ;

	XoSetAttr(ar[3],XoAttrRefName,"name",0) ;
	XoSetAttr(ar[3],XoAttrRefTypeName,"XoString",0) ;
	XoSetAttr(ar[3],XoAttrRefList,(void *)0,0) ;
	XoSetAttr(ar[3],XoAttrRefLgMax,(void *)-1,0) ;
	XoSetAttr(ar[3],XoAttrRefNoSave,(void *)0,0) ;
	XoSetAttr(ar[3],XoAttrRefSaveUnset,(void *)0,0) ;
	XoSetAttr(ar[3],XoAttrRefEmbAttr,(void *)1,0) ;
	XoSetAttr(ar[3],XoAttrRefWrapHidden,(void *)0,0) ;
	XoSetAttr(ar[3],XoAttrRefShDico,(void *)0,0) ;
	XoAddInAttr(or,XoObjRefListAttr,ar[3],0) ;

	XoSetAttr(ar[4],XoAttrRefName,"href",0) ;
	XoSetAttr(ar[4],XoAttrRefTypeName,"XoString",0) ;
	XoSetAttr(ar[4],XoAttrRefList,(void *)0,0) ;
	XoSetAttr(ar[4],XoAttrRefLgMax,(void *)-1,0) ;
	XoSetAttr(ar[4],XoAttrRefNoSave,(void *)0,0) ;
	XoSetAttr(ar[4],XoAttrRefSaveUnset,(void *)0,0) ;
	XoSetAttr(ar[4],XoAttrRefEmbAttr,(void *)1,0) ;
	XoSetAttr(ar[4],XoAttrRefWrapHidden,(void *)0,0) ;
	XoSetAttr(ar[4],XoAttrRefShDico,(void *)0,0) ;
	XoAddInAttr(or,XoObjRefListAttr,ar[4],0) ;

	XoSetAttr(ar[5],XoAttrRefName,"is",0) ;
	XoSetAttr(ar[5],XoAttrRefTypeName,"XoString",0) ;
	XoSetAttr(ar[5],XoAttrRefList,(void *)0,0) ;
	XoSetAttr(ar[5],XoAttrRefLgMax,(void *)-1,0) ;
	XoSetAttr(ar[5],XoAttrRefNoSave,(void *)0,0) ;
	XoSetAttr(ar[5],XoAttrRefSaveUnset,(void *)0,0) ;
	XoSetAttr(ar[5],XoAttrRefEmbAttr,(void *)1,0) ;
	XoSetAttr(ar[5],XoAttrRefWrapHidden,(void *)0,0) ;
	XoSetAttr(ar[5],XoAttrRefShDico,(void *)0,0) ;
	XoAddInAttr(or,XoObjRefListAttr,ar[5],0) ;

	XoSetAttr(ar[6],XoAttrRefName,"null",0) ;
	XoSetAttr(ar[6],XoAttrRefTypeName,"XoString",0) ;
	XoSetAttr(ar[6],XoAttrRefList,(void *)0,0) ;
	XoSetAttr(ar[6],XoAttrRefLgMax,(void *)5,0) ;
	XoSetAttr(ar[6],XoAttrRefNoSave,(void *)0,0) ;
	XoSetAttr(ar[6],XoAttrRefSaveUnset,(void *)0,0) ;
	XoSetAttr(ar[6],XoAttrRefEmbAttr,(void *)1,0) ;
	XoSetAttr(ar[6],XoAttrRefWrapHidden,(void *)0,0) ;
	XoSetAttr(ar[6],XoAttrRefShDico,(void *)0,0) ;
	XoAddInAttr(or,XoObjRefListAttr,ar[6],0) ;

	XoSetAttr(ar[7],XoAttrRefName,"icon",0) ;
	XoSetAttr(ar[7],XoAttrRefTypeName,"XoString",0) ;
	XoSetAttr(ar[7],XoAttrRefList,(void *)0,0) ;
	XoSetAttr(ar[7],XoAttrRefLgMax,(void *)-1,0) ;
	XoSetAttr(ar[7],XoAttrRefNoSave,(void *)0,0) ;
	XoSetAttr(ar[7],XoAttrRefSaveUnset,(void *)0,0) ;
	XoSetAttr(ar[7],XoAttrRefEmbAttr,(void *)1,0) ;
	XoSetAttr(ar[7],XoAttrRefWrapHidden,(void *)0,0) ;
	XoSetAttr(ar[7],XoAttrRefShDico,(void *)0,0) ;
	XoAddInAttr(or,XoObjRefListAttr,ar[7],0) ;

	XoSetAttr(ar[8],XoAttrRefName,"displayName",0) ;
	XoSetAttr(ar[8],XoAttrRefTypeName,"XoString",0) ;
	XoSetAttr(ar[8],XoAttrRefList,(void *)0,0) ;
	XoSetAttr(ar[8],XoAttrRefLgMax,(void *)-1,0) ;
	XoSetAttr(ar[8],XoAttrRefNoSave,(void *)0,0) ;
	XoSetAttr(ar[8],XoAttrRefSaveUnset,(void *)0,0) ;
	XoSetAttr(ar[8],XoAttrRefEmbAttr,(void *)1,0) ;
	XoSetAttr(ar[8],XoAttrRefWrapHidden,(void *)0,0) ;
	XoSetAttr(ar[8],XoAttrRefShDico,(void *)0,0) ;
	XoAddInAttr(or,XoObjRefListAttr,ar[8],0) ;

	XoSetAttr(ar[9],XoAttrRefName,"display",0) ;
	XoSetAttr(ar[9],XoAttrRefTypeName,"XoString",0) ;
	XoSetAttr(ar[9],XoAttrRefList,(void *)0,0) ;
	XoSetAttr(ar[9],XoAttrRefLgMax,(void *)-1,0) ;
	XoSetAttr(ar[9],XoAttrRefNoSave,(void *)0,0) ;
	XoSetAttr(ar[9],XoAttrRefSaveUnset,(void *)0,0) ;
	XoSetAttr(ar[9],XoAttrRefEmbAttr,(void *)1,0) ;
	XoSetAttr(ar[9],XoAttrRefWrapHidden,(void *)0,0) ;
	XoSetAttr(ar[9],XoAttrRefShDico,(void *)0,0) ;
	XoAddInAttr(or,XoObjRefListAttr,ar[9],0) ;

	XoSetAttr(ar[10],XoAttrRefName,"writable",0) ;
	XoSetAttr(ar[10],XoAttrRefTypeName,"XoString",0) ;
	XoSetAttr(ar[10],XoAttrRefList,(void *)0,0) ;
	XoSetAttr(ar[10],XoAttrRefLgMax,(void *)5,0) ;
	XoSetAttr(ar[10],XoAttrRefNoSave,(void *)0,0) ;
	XoSetAttr(ar[10],XoAttrRefSaveUnset,(void *)0,0) ;
	XoSetAttr(ar[10],XoAttrRefEmbAttr,(void *)1,0) ;
	XoSetAttr(ar[10],XoAttrRefWrapHidden,(void *)0,0) ;
	XoSetAttr(ar[10],XoAttrRefShDico,(void *)0,0) ;
	XoAddInAttr(or,XoObjRefListAttr,ar[10],0) ;

	XoSetAttr(ar[11],XoAttrRefName,"status",0) ;
	XoSetAttr(ar[11],XoAttrRefTypeName,"XoString",0) ;
	XoSetAttr(ar[11],XoAttrRefList,(void *)0,0) ;
	XoSetAttr(ar[11],XoAttrRefLgMax,(void *)-1,0) ;
	XoSetAttr(ar[11],XoAttrRefNoSave,(void *)0,0) ;
	XoSetAttr(ar[11],XoAttrRefSaveUnset,(void *)0,0) ;
	XoSetAttr(ar[11],XoAttrRefEmbAttr,(void *)1,0) ;
	XoSetAttr(ar[11],XoAttrRefWrapHidden,(void *)0,0) ;
	XoSetAttr(ar[11],XoAttrRefShDico,(void *)0,0) ;
	XoAddInAttr(or,XoObjRefListAttr,ar[11],0) ;

	XoSetAttr(ar[12],XoAttrRefName,"wrapper$",0) ;
	XoSetAttr(ar[12],XoAttrRefTypeName,"XoVoidObj",0) ;
	XoSetAttr(ar[12],XoAttrRefList,(void *)1,0) ;
	XoSetAttr(ar[12],XoAttrRefLgMax,(void *)-1,0) ;
	XoSetAttr(ar[12],XoAttrRefNoSave,(void *)0,0) ;
	XoSetAttr(ar[12],XoAttrRefSaveUnset,(void *)0,0) ;
	XoSetAttr(ar[12],XoAttrRefEmbAttr,(void *)0,0) ;
	XoSetAttr(ar[12],XoAttrRefWrapHidden,(void *)0,0) ;
	XoSetAttr(ar[12],XoAttrRefShDico,(void *)0,0) ;
	XoAddInAttr(or,XoObjRefListAttr,ar[12],0) ;

	XoSetAttr(ar[13],XoAttrRefName,"val",0) ;
	XoSetAttr(ar[13],XoAttrRefTypeName,"XoString",0) ;
	XoSetAttr(ar[13],XoAttrRefList,(void *)0,0) ;
	XoSetAttr(ar[13],XoAttrRefLgMax,(void *)-1,0) ;
	XoSetAttr(ar[13],XoAttrRefNoSave,(void *)0,0) ;
	XoSetAttr(ar[13],XoAttrRefSaveUnset,(void *)0,0) ;
	XoSetAttr(ar[13],XoAttrRefEmbAttr,(void *)1,0) ;
	XoSetAttr(ar[13],XoAttrRefWrapHidden,(void *)0,0) ;
	XoSetAttr(ar[13],XoAttrRefShDico,(void *)0,0) ;
	XoAddInAttr(or,XoObjRefListAttr,ar[13],0) ;

	XoSetAttr(ar[14],XoAttrRefName,"min",0) ;
	XoSetAttr(ar[14],XoAttrRefTypeName,"XoString",0) ;
	XoSetAttr(ar[14],XoAttrRefList,(void *)0,0) ;
	XoSetAttr(ar[14],XoAttrRefLgMax,(void *)-1,0) ;
	XoSetAttr(ar[14],XoAttrRefNoSave,(void *)0,0) ;
	XoSetAttr(ar[14],XoAttrRefSaveUnset,(void *)0,0) ;
	XoSetAttr(ar[14],XoAttrRefEmbAttr,(void *)1,0) ;
	XoSetAttr(ar[14],XoAttrRefWrapHidden,(void *)0,0) ;
	XoSetAttr(ar[14],XoAttrRefShDico,(void *)0,0) ;
	XoAddInAttr(or,XoObjRefListAttr,ar[14],0) ;

	XoSetAttr(ar[15],XoAttrRefName,"max",0) ;
	XoSetAttr(ar[15],XoAttrRefTypeName,"XoString",0) ;
	XoSetAttr(ar[15],XoAttrRefList,(void *)0,0) ;
	XoSetAttr(ar[15],XoAttrRefLgMax,(void *)-1,0) ;
	XoSetAttr(ar[15],XoAttrRefNoSave,(void *)0,0) ;
	XoSetAttr(ar[15],XoAttrRefSaveUnset,(void *)0,0) ;
	XoSetAttr(ar[15],XoAttrRefEmbAttr,(void *)1,0) ;
	XoSetAttr(ar[15],XoAttrRefWrapHidden,(void *)0,0) ;
	XoSetAttr(ar[15],XoAttrRefShDico,(void *)0,0) ;
	XoAddInAttr(or,XoObjRefListAttr,ar[15],0) ;

	XoSetAttr(ar[16],XoAttrRefName,"unit",0) ;
	XoSetAttr(ar[16],XoAttrRefTypeName,"XoString",0) ;
	XoSetAttr(ar[16],XoAttrRefList,(void *)0,0) ;
	XoSetAttr(ar[16],XoAttrRefLgMax,(void *)-1,0) ;
	XoSetAttr(ar[16],XoAttrRefNoSave,(void *)0,0) ;
	XoSetAttr(ar[16],XoAttrRefSaveUnset,(void *)0,0) ;
	XoSetAttr(ar[16],XoAttrRefEmbAttr,(void *)1,0) ;
	XoSetAttr(ar[16],XoAttrRefWrapHidden,(void *)0,0) ;
	XoSetAttr(ar[16],XoAttrRefShDico,(void *)0,0) ;
	XoAddInAttr(or,XoObjRefListAttr,ar[16],0) ;

	XoSetAttr(or,XoObjRefNbAttr,(void *)17,0) ;
	ret = -100 ;
	if (XoAddObjRef(or)<0) goto end ;
	ret = 0 ;
	end :
	for (i=0;i<17;i++)
		if (ar[i]) XoFree(ar[i],0) ;
	if (or) XoFree(or,0) ;
	return	ret ;
}
static	int32	GenXo_o:real(void)
{
	int32	ret ;
	int32	i ;
	void	*or ;
	void	*ar[6] ;

	memset(ar,0,sizeof(ar));
	ret = -1 ;
	if (!(or=XoNew(XoObjRef))) goto end ;
	ret = -2 ;
	for (i=0;i<5;i++)
		if (!(ar[i]=XoNew(XoAttrRef))) goto end ;
	ret = -3 ;
	XoSetAttr(or,XoObjRefType,(void *)1,0) ;
	XoSetAttr(or,XoObjRefName,"o:real",0) ;
	XoSetAttr(or,XoObjRefRoot,(void *)0,0) ;

	XoSetAttr(ar[0],XoAttrRefName,"name$",0) ;
	XoSetAttr(ar[0],XoAttrRefTypeName,"XoString",0) ;
	XoSetAttr(ar[0],XoAttrRefList,(void *)0,0) ;
	XoSetAttr(ar[0],XoAttrRefLgMax,(void *)-1,0) ;
	XoSetAttr(ar[0],XoAttrRefNoSave,(void *)0,0) ;
	XoSetAttr(ar[0],XoAttrRefSaveUnset,(void *)0,0) ;
	XoSetAttr(ar[0],XoAttrRefEmbAttr,(void *)0,0) ;
	XoSetAttr(ar[0],XoAttrRefWrapHidden,(void *)0,0) ;
	XoSetAttr(ar[0],XoAttrRefShDico,(void *)0,0) ;
	XoAddInAttr(or,XoObjRefListAttr,ar[0],0) ;

	XoSetAttr(ar[1],XoAttrRefName,"xmlns",0) ;
	XoSetAttr(ar[1],XoAttrRefTypeName,"XoString",0) ;
	XoSetAttr(ar[1],XoAttrRefList,(void *)0,0) ;
	XoSetAttr(ar[1],XoAttrRefLgMax,(void *)-1,0) ;
	XoSetAttr(ar[1],XoAttrRefNoSave,(void *)0,0) ;
	XoSetAttr(ar[1],XoAttrRefSaveUnset,(void *)0,0) ;
	XoSetAttr(ar[1],XoAttrRefEmbAttr,(void *)1,0) ;
	XoSetAttr(ar[1],XoAttrRefWrapHidden,(void *)0,0) ;
	XoSetAttr(ar[1],XoAttrRefShDico,(void *)1,0) ;
	XoAddInAttr(or,XoObjRefListAttr,ar[1],0) ;

	XoSetAttr(ar[2],XoAttrRefName,"custom$",0) ;
	XoSetAttr(ar[2],XoAttrRefTypeName,"o:t_xml_obix_attr",0) ;
	XoSetAttr(ar[2],XoAttrRefList,(void *)1,0) ;
	XoSetAttr(ar[2],XoAttrRefLgMax,(void *)-1,0) ;
	XoSetAttr(ar[2],XoAttrRefNoSave,(void *)0,0) ;
	XoSetAttr(ar[2],XoAttrRefSaveUnset,(void *)0,0) ;
	XoSetAttr(ar[2],XoAttrRefEmbAttr,(void *)1,0) ;
	XoSetAttr(ar[2],XoAttrRefWrapHidden,(void *)0,0) ;
	XoSetAttr(ar[2],XoAttrRefShDico,(void *)0,0) ;
	XoAddInAttr(or,XoObjRefListAttr,ar[2],0) ;

	XoSetAttr(ar[3],XoAttrRefName,"name",0) ;
	XoSetAttr(ar[3],XoAttrRefTypeName,"XoString",0) ;
	XoSetAttr(ar[3],XoAttrRefList,(void *)0,0) ;
	XoSetAttr(ar[3],XoAttrRefLgMax,(void *)-1,0) ;
	XoSetAttr(ar[3],XoAttrRefNoSave,(void *)0,0) ;
	XoSetAttr(ar[3],XoAttrRefSaveUnset,(void *)0,0) ;
	XoSetAttr(ar[3],XoAttrRefEmbAttr,(void *)1,0) ;
	XoSetAttr(ar[3],XoAttrRefWrapHidden,(void *)0,0) ;
	XoSetAttr(ar[3],XoAttrRefShDico,(void *)0,0) ;
	XoAddInAttr(or,XoObjRefListAttr,ar[3],0) ;

	XoSetAttr(ar[4],XoAttrRefName,"href",0) ;
	XoSetAttr(ar[4],XoAttrRefTypeName,"XoString",0) ;
	XoSetAttr(ar[4],XoAttrRefList,(void *)0,0) ;
	XoSetAttr(ar[4],XoAttrRefLgMax,(void *)-1,0) ;
	XoSetAttr(ar[4],XoAttrRefNoSave,(void *)0,0) ;
	XoSetAttr(ar[4],XoAttrRefSaveUnset,(void *)0,0) ;
	XoSetAttr(ar[4],XoAttrRefEmbAttr,(void *)1,0) ;
	XoSetAttr(ar[4],XoAttrRefWrapHidden,(void *)0,0) ;
	XoSetAttr(ar[4],XoAttrRefShDico,(void *)0,0) ;
	XoAddInAttr(or,XoObjRefListAttr,ar[4],0) ;

	XoSetAttr(ar[5],XoAttrRefName,"is",0) ;
	XoSetAttr(ar[5],XoAttrRefTypeName,"XoString",0) ;
	XoSetAttr(ar[5],XoAttrRefList,(void *)0,0) ;
	XoSetAttr(ar[5],XoAttrRefLgMax,(void *)-1,0) ;
	XoSetAttr(ar[5],XoAttrRefNoSave,(void *)0,0) ;
	XoSetAttr(ar[5],XoAttrRefSaveUnset,(void *)0,0) ;
	XoSetAttr(ar[5],XoAttrRefEmbAttr,(void *)1,0) ;
	XoSetAttr(ar[5],XoAttrRefWrapHidden,(void *)0,0) ;
	XoSetAttr(ar[5],XoAttrRefShDico,(void *)0,0) ;
	XoAddInAttr(or,XoObjRefListAttr,ar[5],0) ;

	XoSetAttr(ar[6],XoAttrRefName,"null",0) ;
	XoSetAttr(ar[6],XoAttrRefTypeName,"XoString",0) ;
	XoSetAttr(ar[6],XoAttrRefList,(void *)0,0) ;
	XoSetAttr(ar[6],XoAttrRefLgMax,(void *)5,0) ;
	XoSetAttr(ar[6],XoAttrRefNoSave,(void *)0,0) ;
	XoSetAttr(ar[6],XoAttrRefSaveUnset,(void *)0,0) ;
	XoSetAttr(ar[6],XoAttrRefEmbAttr,(void *)1,0) ;
	XoSetAttr(ar[6],XoAttrRefWrapHidden,(void *)0,0) ;
	XoSetAttr(ar[6],XoAttrRefShDico,(void *)0,0) ;
	XoAddInAttr(or,XoObjRefListAttr,ar[6],0) ;

	XoSetAttr(ar[7],XoAttrRefName,"icon",0) ;
	XoSetAttr(ar[7],XoAttrRefTypeName,"XoString",0) ;
	XoSetAttr(ar[7],XoAttrRefList,(void *)0,0) ;
	XoSetAttr(ar[7],XoAttrRefLgMax,(void *)-1,0) ;
	XoSetAttr(ar[7],XoAttrRefNoSave,(void *)0,0) ;
	XoSetAttr(ar[7],XoAttrRefSaveUnset,(void *)0,0) ;
	XoSetAttr(ar[7],XoAttrRefEmbAttr,(void *)1,0) ;
	XoSetAttr(ar[7],XoAttrRefWrapHidden,(void *)0,0) ;
	XoSetAttr(ar[7],XoAttrRefShDico,(void *)0,0) ;
	XoAddInAttr(or,XoObjRefListAttr,ar[7],0) ;

	XoSetAttr(ar[8],XoAttrRefName,"displayName",0) ;
	XoSetAttr(ar[8],XoAttrRefTypeName,"XoString",0) ;
	XoSetAttr(ar[8],XoAttrRefList,(void *)0,0) ;
	XoSetAttr(ar[8],XoAttrRefLgMax,(void *)-1,0) ;
	XoSetAttr(ar[8],XoAttrRefNoSave,(void *)0,0) ;
	XoSetAttr(ar[8],XoAttrRefSaveUnset,(void *)0,0) ;
	XoSetAttr(ar[8],XoAttrRefEmbAttr,(void *)1,0) ;
	XoSetAttr(ar[8],XoAttrRefWrapHidden,(void *)0,0) ;
	XoSetAttr(ar[8],XoAttrRefShDico,(void *)0,0) ;
	XoAddInAttr(or,XoObjRefListAttr,ar[8],0) ;

	XoSetAttr(ar[9],XoAttrRefName,"display",0) ;
	XoSetAttr(ar[9],XoAttrRefTypeName,"XoString",0) ;
	XoSetAttr(ar[9],XoAttrRefList,(void *)0,0) ;
	XoSetAttr(ar[9],XoAttrRefLgMax,(void *)-1,0) ;
	XoSetAttr(ar[9],XoAttrRefNoSave,(void *)0,0) ;
	XoSetAttr(ar[9],XoAttrRefSaveUnset,(void *)0,0) ;
	XoSetAttr(ar[9],XoAttrRefEmbAttr,(void *)1,0) ;
	XoSetAttr(ar[9],XoAttrRefWrapHidden,(void *)0,0) ;
	XoSetAttr(ar[9],XoAttrRefShDico,(void *)0,0) ;
	XoAddInAttr(or,XoObjRefListAttr,ar[9],0) ;

	XoSetAttr(ar[10],XoAttrRefName,"writable",0) ;
	XoSetAttr(ar[10],XoAttrRefTypeName,"XoString",0) ;
	XoSetAttr(ar[10],XoAttrRefList,(void *)0,0) ;
	XoSetAttr(ar[10],XoAttrRefLgMax,(void *)5,0) ;
	XoSetAttr(ar[10],XoAttrRefNoSave,(void *)0,0) ;
	XoSetAttr(ar[10],XoAttrRefSaveUnset,(void *)0,0) ;
	XoSetAttr(ar[10],XoAttrRefEmbAttr,(void *)1,0) ;
	XoSetAttr(ar[10],XoAttrRefWrapHidden,(void *)0,0) ;
	XoSetAttr(ar[10],XoAttrRefShDico,(void *)0,0) ;
	XoAddInAttr(or,XoObjRefListAttr,ar[10],0) ;

	XoSetAttr(ar[11],XoAttrRefName,"status",0) ;
	XoSetAttr(ar[11],XoAttrRefTypeName,"XoString",0) ;
	XoSetAttr(ar[11],XoAttrRefList,(void *)0,0) ;
	XoSetAttr(ar[11],XoAttrRefLgMax,(void *)-1,0) ;
	XoSetAttr(ar[11],XoAttrRefNoSave,(void *)0,0) ;
	XoSetAttr(ar[11],XoAttrRefSaveUnset,(void *)0,0) ;
	XoSetAttr(ar[11],XoAttrRefEmbAttr,(void *)1,0) ;
	XoSetAttr(ar[11],XoAttrRefWrapHidden,(void *)0,0) ;
	XoSetAttr(ar[11],XoAttrRefShDico,(void *)0,0) ;
	XoAddInAttr(or,XoObjRefListAttr,ar[11],0) ;

	XoSetAttr(ar[12],XoAttrRefName,"wrapper$",0) ;
	XoSetAttr(ar[12],XoAttrRefTypeName,"XoVoidObj",0) ;
	XoSetAttr(ar[12],XoAttrRefList,(void *)1,0) ;
	XoSetAttr(ar[12],XoAttrRefLgMax,(void *)-1,0) ;
	XoSetAttr(ar[12],XoAttrRefNoSave,(void *)0,0) ;
	XoSetAttr(ar[12],XoAttrRefSaveUnset,(void *)0,0) ;
	XoSetAttr(ar[12],XoAttrRefEmbAttr,(void *)0,0) ;
	XoSetAttr(ar[12],XoAttrRefWrapHidden,(void *)0,0) ;
	XoSetAttr(ar[12],XoAttrRefShDico,(void *)0,0) ;
	XoAddInAttr(or,XoObjRefListAttr,ar[12],0) ;

	XoSetAttr(ar[13],XoAttrRefName,"val",0) ;
	XoSetAttr(ar[13],XoAttrRefTypeName,"XoString",0) ;
	XoSetAttr(ar[13],XoAttrRefList,(void *)0,0) ;
	XoSetAttr(ar[13],XoAttrRefLgMax,(void *)-1,0) ;
	XoSetAttr(ar[13],XoAttrRefNoSave,(void *)0,0) ;
	XoSetAttr(ar[13],XoAttrRefSaveUnset,(void *)0,0) ;
	XoSetAttr(ar[13],XoAttrRefEmbAttr,(void *)1,0) ;
	XoSetAttr(ar[13],XoAttrRefWrapHidden,(void *)0,0) ;
	XoSetAttr(ar[13],XoAttrRefShDico,(void *)0,0) ;
	XoAddInAttr(or,XoObjRefListAttr,ar[13],0) ;

	XoSetAttr(ar[14],XoAttrRefName,"min",0) ;
	XoSetAttr(ar[14],XoAttrRefTypeName,"XoString",0) ;
	XoSetAttr(ar[14],XoAttrRefList,(void *)0,0) ;
	XoSetAttr(ar[14],XoAttrRefLgMax,(void *)-1,0) ;
	XoSetAttr(ar[14],XoAttrRefNoSave,(void *)0,0) ;
	XoSetAttr(ar[14],XoAttrRefSaveUnset,(void *)0,0) ;
	XoSetAttr(ar[14],XoAttrRefEmbAttr,(void *)1,0) ;
	XoSetAttr(ar[14],XoAttrRefWrapHidden,(void *)0,0) ;
	XoSetAttr(ar[14],XoAttrRefShDico,(void *)0,0) ;
	XoAddInAttr(or,XoObjRefListAttr,ar[14],0) ;

	XoSetAttr(ar[15],XoAttrRefName,"max",0) ;
	XoSetAttr(ar[15],XoAttrRefTypeName,"XoString",0) ;
	XoSetAttr(ar[15],XoAttrRefList,(void *)0,0) ;
	XoSetAttr(ar[15],XoAttrRefLgMax,(void *)-1,0) ;
	XoSetAttr(ar[15],XoAttrRefNoSave,(void *)0,0) ;
	XoSetAttr(ar[15],XoAttrRefSaveUnset,(void *)0,0) ;
	XoSetAttr(ar[15],XoAttrRefEmbAttr,(void *)1,0) ;
	XoSetAttr(ar[15],XoAttrRefWrapHidden,(void *)0,0) ;
	XoSetAttr(ar[15],XoAttrRefShDico,(void *)0,0) ;
	XoAddInAttr(or,XoObjRefListAttr,ar[15],0) ;

	XoSetAttr(ar[16],XoAttrRefName,"unit",0) ;
	XoSetAttr(ar[16],XoAttrRefTypeName,"XoString",0) ;
	XoSetAttr(ar[16],XoAttrRefList,(void *)0,0) ;
	XoSetAttr(ar[16],XoAttrRefLgMax,(void *)-1,0) ;
	XoSetAttr(ar[16],XoAttrRefNoSave,(void *)0,0) ;
	XoSetAttr(ar[16],XoAttrRefSaveUnset,(void *)0,0) ;
	XoSetAttr(ar[16],XoAttrRefEmbAttr,(void *)1,0) ;
	XoSetAttr(ar[16],XoAttrRefWrapHidden,(void *)0,0) ;
	XoSetAttr(ar[16],XoAttrRefShDico,(void *)0,0) ;
	XoAddInAttr(or,XoObjRefListAttr,ar[16],0) ;

	XoSetAttr(ar[17],XoAttrRefName,"precision",0) ;
	XoSetAttr(ar[17],XoAttrRefTypeName,"XoString",0) ;
	XoSetAttr(ar[17],XoAttrRefList,(void *)0,0) ;
	XoSetAttr(ar[17],XoAttrRefLgMax,(void *)-1,0) ;
	XoSetAttr(ar[17],XoAttrRefNoSave,(void *)0,0) ;
	XoSetAttr(ar[17],XoAttrRefSaveUnset,(void *)0,0) ;
	XoSetAttr(ar[17],XoAttrRefEmbAttr,(void *)1,0) ;
	XoSetAttr(ar[17],XoAttrRefWrapHidden,(void *)0,0) ;
	XoSetAttr(ar[17],XoAttrRefShDico,(void *)0,0) ;
	XoAddInAttr(or,XoObjRefListAttr,ar[17],0) ;

	XoSetAttr(or,XoObjRefNbAttr,(void *)18,0) ;
	ret = -100 ;
	if (XoAddObjRef(or)<0) goto end ;
	ret = 0 ;
	end :
	for (i=0;i<18;i++)
		if (ar[i]) XoFree(ar[i],0) ;
	if (or) XoFree(or,0) ;
	return	ret ;
}
static	int32	GenXo_o:str(void)
{
	int32	ret ;
	int32	i ;
	void	*or ;
	void	*ar[4] ;

	memset(ar,0,sizeof(ar));
	ret = -1 ;
	if (!(or=XoNew(XoObjRef))) goto end ;
	ret = -2 ;
	for (i=0;i<3;i++)
		if (!(ar[i]=XoNew(XoAttrRef))) goto end ;
	ret = -3 ;
	XoSetAttr(or,XoObjRefType,(void *)1,0) ;
	XoSetAttr(or,XoObjRefName,"o:str",0) ;
	XoSetAttr(or,XoObjRefRoot,(void *)0,0) ;

	XoSetAttr(ar[0],XoAttrRefName,"name$",0) ;
	XoSetAttr(ar[0],XoAttrRefTypeName,"XoString",0) ;
	XoSetAttr(ar[0],XoAttrRefList,(void *)0,0) ;
	XoSetAttr(ar[0],XoAttrRefLgMax,(void *)-1,0) ;
	XoSetAttr(ar[0],XoAttrRefNoSave,(void *)0,0) ;
	XoSetAttr(ar[0],XoAttrRefSaveUnset,(void *)0,0) ;
	XoSetAttr(ar[0],XoAttrRefEmbAttr,(void *)0,0) ;
	XoSetAttr(ar[0],XoAttrRefWrapHidden,(void *)0,0) ;
	XoSetAttr(ar[0],XoAttrRefShDico,(void *)0,0) ;
	XoAddInAttr(or,XoObjRefListAttr,ar[0],0) ;

	XoSetAttr(ar[1],XoAttrRefName,"xmlns",0) ;
	XoSetAttr(ar[1],XoAttrRefTypeName,"XoString",0) ;
	XoSetAttr(ar[1],XoAttrRefList,(void *)0,0) ;
	XoSetAttr(ar[1],XoAttrRefLgMax,(void *)-1,0) ;
	XoSetAttr(ar[1],XoAttrRefNoSave,(void *)0,0) ;
	XoSetAttr(ar[1],XoAttrRefSaveUnset,(void *)0,0) ;
	XoSetAttr(ar[1],XoAttrRefEmbAttr,(void *)1,0) ;
	XoSetAttr(ar[1],XoAttrRefWrapHidden,(void *)0,0) ;
	XoSetAttr(ar[1],XoAttrRefShDico,(void *)1,0) ;
	XoAddInAttr(or,XoObjRefListAttr,ar[1],0) ;

	XoSetAttr(ar[2],XoAttrRefName,"custom$",0) ;
	XoSetAttr(ar[2],XoAttrRefTypeName,"o:t_xml_obix_attr",0) ;
	XoSetAttr(ar[2],XoAttrRefList,(void *)1,0) ;
	XoSetAttr(ar[2],XoAttrRefLgMax,(void *)-1,0) ;
	XoSetAttr(ar[2],XoAttrRefNoSave,(void *)0,0) ;
	XoSetAttr(ar[2],XoAttrRefSaveUnset,(void *)0,0) ;
	XoSetAttr(ar[2],XoAttrRefEmbAttr,(void *)1,0) ;
	XoSetAttr(ar[2],XoAttrRefWrapHidden,(void *)0,0) ;
	XoSetAttr(ar[2],XoAttrRefShDico,(void *)0,0) ;
	XoAddInAttr(or,XoObjRefListAttr,ar[2],0) ;

	XoSetAttr(ar[3],XoAttrRefName,"name",0) ;
	XoSetAttr(ar[3],XoAttrRefTypeName,"XoString",0) ;
	XoSetAttr(ar[3],XoAttrRefList,(void *)0,0) ;
	XoSetAttr(ar[3],XoAttrRefLgMax,(void *)-1,0) ;
	XoSetAttr(ar[3],XoAttrRefNoSave,(void *)0,0) ;
	XoSetAttr(ar[3],XoAttrRefSaveUnset,(void *)0,0) ;
	XoSetAttr(ar[3],XoAttrRefEmbAttr,(void *)1,0) ;
	XoSetAttr(ar[3],XoAttrRefWrapHidden,(void *)0,0) ;
	XoSetAttr(ar[3],XoAttrRefShDico,(void *)0,0) ;
	XoAddInAttr(or,XoObjRefListAttr,ar[3],0) ;

	XoSetAttr(ar[4],XoAttrRefName,"href",0) ;
	XoSetAttr(ar[4],XoAttrRefTypeName,"XoString",0) ;
	XoSetAttr(ar[4],XoAttrRefList,(void *)0,0) ;
	XoSetAttr(ar[4],XoAttrRefLgMax,(void *)-1,0) ;
	XoSetAttr(ar[4],XoAttrRefNoSave,(void *)0,0) ;
	XoSetAttr(ar[4],XoAttrRefSaveUnset,(void *)0,0) ;
	XoSetAttr(ar[4],XoAttrRefEmbAttr,(void *)1,0) ;
	XoSetAttr(ar[4],XoAttrRefWrapHidden,(void *)0,0) ;
	XoSetAttr(ar[4],XoAttrRefShDico,(void *)0,0) ;
	XoAddInAttr(or,XoObjRefListAttr,ar[4],0) ;

	XoSetAttr(ar[5],XoAttrRefName,"is",0) ;
	XoSetAttr(ar[5],XoAttrRefTypeName,"XoString",0) ;
	XoSetAttr(ar[5],XoAttrRefList,(void *)0,0) ;
	XoSetAttr(ar[5],XoAttrRefLgMax,(void *)-1,0) ;
	XoSetAttr(ar[5],XoAttrRefNoSave,(void *)0,0) ;
	XoSetAttr(ar[5],XoAttrRefSaveUnset,(void *)0,0) ;
	XoSetAttr(ar[5],XoAttrRefEmbAttr,(void *)1,0) ;
	XoSetAttr(ar[5],XoAttrRefWrapHidden,(void *)0,0) ;
	XoSetAttr(ar[5],XoAttrRefShDico,(void *)0,0) ;
	XoAddInAttr(or,XoObjRefListAttr,ar[5],0) ;

	XoSetAttr(ar[6],XoAttrRefName,"null",0) ;
	XoSetAttr(ar[6],XoAttrRefTypeName,"XoString",0) ;
	XoSetAttr(ar[6],XoAttrRefList,(void *)0,0) ;
	XoSetAttr(ar[6],XoAttrRefLgMax,(void *)5,0) ;
	XoSetAttr(ar[6],XoAttrRefNoSave,(void *)0,0) ;
	XoSetAttr(ar[6],XoAttrRefSaveUnset,(void *)0,0) ;
	XoSetAttr(ar[6],XoAttrRefEmbAttr,(void *)1,0) ;
	XoSetAttr(ar[6],XoAttrRefWrapHidden,(void *)0,0) ;
	XoSetAttr(ar[6],XoAttrRefShDico,(void *)0,0) ;
	XoAddInAttr(or,XoObjRefListAttr,ar[6],0) ;

	XoSetAttr(ar[7],XoAttrRefName,"icon",0) ;
	XoSetAttr(ar[7],XoAttrRefTypeName,"XoString",0) ;
	XoSetAttr(ar[7],XoAttrRefList,(void *)0,0) ;
	XoSetAttr(ar[7],XoAttrRefLgMax,(void *)-1,0) ;
	XoSetAttr(ar[7],XoAttrRefNoSave,(void *)0,0) ;
	XoSetAttr(ar[7],XoAttrRefSaveUnset,(void *)0,0) ;
	XoSetAttr(ar[7],XoAttrRefEmbAttr,(void *)1,0) ;
	XoSetAttr(ar[7],XoAttrRefWrapHidden,(void *)0,0) ;
	XoSetAttr(ar[7],XoAttrRefShDico,(void *)0,0) ;
	XoAddInAttr(or,XoObjRefListAttr,ar[7],0) ;

	XoSetAttr(ar[8],XoAttrRefName,"displayName",0) ;
	XoSetAttr(ar[8],XoAttrRefTypeName,"XoString",0) ;
	XoSetAttr(ar[8],XoAttrRefList,(void *)0,0) ;
	XoSetAttr(ar[8],XoAttrRefLgMax,(void *)-1,0) ;
	XoSetAttr(ar[8],XoAttrRefNoSave,(void *)0,0) ;
	XoSetAttr(ar[8],XoAttrRefSaveUnset,(void *)0,0) ;
	XoSetAttr(ar[8],XoAttrRefEmbAttr,(void *)1,0) ;
	XoSetAttr(ar[8],XoAttrRefWrapHidden,(void *)0,0) ;
	XoSetAttr(ar[8],XoAttrRefShDico,(void *)0,0) ;
	XoAddInAttr(or,XoObjRefListAttr,ar[8],0) ;

	XoSetAttr(ar[9],XoAttrRefName,"display",0) ;
	XoSetAttr(ar[9],XoAttrRefTypeName,"XoString",0) ;
	XoSetAttr(ar[9],XoAttrRefList,(void *)0,0) ;
	XoSetAttr(ar[9],XoAttrRefLgMax,(void *)-1,0) ;
	XoSetAttr(ar[9],XoAttrRefNoSave,(void *)0,0) ;
	XoSetAttr(ar[9],XoAttrRefSaveUnset,(void *)0,0) ;
	XoSetAttr(ar[9],XoAttrRefEmbAttr,(void *)1,0) ;
	XoSetAttr(ar[9],XoAttrRefWrapHidden,(void *)0,0) ;
	XoSetAttr(ar[9],XoAttrRefShDico,(void *)0,0) ;
	XoAddInAttr(or,XoObjRefListAttr,ar[9],0) ;

	XoSetAttr(ar[10],XoAttrRefName,"writable",0) ;
	XoSetAttr(ar[10],XoAttrRefTypeName,"XoString",0) ;
	XoSetAttr(ar[10],XoAttrRefList,(void *)0,0) ;
	XoSetAttr(ar[10],XoAttrRefLgMax,(void *)5,0) ;
	XoSetAttr(ar[10],XoAttrRefNoSave,(void *)0,0) ;
	XoSetAttr(ar[10],XoAttrRefSaveUnset,(void *)0,0) ;
	XoSetAttr(ar[10],XoAttrRefEmbAttr,(void *)1,0) ;
	XoSetAttr(ar[10],XoAttrRefWrapHidden,(void *)0,0) ;
	XoSetAttr(ar[10],XoAttrRefShDico,(void *)0,0) ;
	XoAddInAttr(or,XoObjRefListAttr,ar[10],0) ;

	XoSetAttr(ar[11],XoAttrRefName,"status",0) ;
	XoSetAttr(ar[11],XoAttrRefTypeName,"XoString",0) ;
	XoSetAttr(ar[11],XoAttrRefList,(void *)0,0) ;
	XoSetAttr(ar[11],XoAttrRefLgMax,(void *)-1,0) ;
	XoSetAttr(ar[11],XoAttrRefNoSave,(void *)0,0) ;
	XoSetAttr(ar[11],XoAttrRefSaveUnset,(void *)0,0) ;
	XoSetAttr(ar[11],XoAttrRefEmbAttr,(void *)1,0) ;
	XoSetAttr(ar[11],XoAttrRefWrapHidden,(void *)0,0) ;
	XoSetAttr(ar[11],XoAttrRefShDico,(void *)0,0) ;
	XoAddInAttr(or,XoObjRefListAttr,ar[11],0) ;

	XoSetAttr(ar[12],XoAttrRefName,"wrapper$",0) ;
	XoSetAttr(ar[12],XoAttrRefTypeName,"XoVoidObj",0) ;
	XoSetAttr(ar[12],XoAttrRefList,(void *)1,0) ;
	XoSetAttr(ar[12],XoAttrRefLgMax,(void *)-1,0) ;
	XoSetAttr(ar[12],XoAttrRefNoSave,(void *)0,0) ;
	XoSetAttr(ar[12],XoAttrRefSaveUnset,(void *)0,0) ;
	XoSetAttr(ar[12],XoAttrRefEmbAttr,(void *)0,0) ;
	XoSetAttr(ar[12],XoAttrRefWrapHidden,(void *)0,0) ;
	XoSetAttr(ar[12],XoAttrRefShDico,(void *)0,0) ;
	XoAddInAttr(or,XoObjRefListAttr,ar[12],0) ;

	XoSetAttr(ar[13],XoAttrRefName,"val",0) ;
	XoSetAttr(ar[13],XoAttrRefTypeName,"XoString",0) ;
	XoSetAttr(ar[13],XoAttrRefList,(void *)0,0) ;
	XoSetAttr(ar[13],XoAttrRefLgMax,(void *)-1,0) ;
	XoSetAttr(ar[13],XoAttrRefNoSave,(void *)0,0) ;
	XoSetAttr(ar[13],XoAttrRefSaveUnset,(void *)0,0) ;
	XoSetAttr(ar[13],XoAttrRefEmbAttr,(void *)1,0) ;
	XoSetAttr(ar[13],XoAttrRefWrapHidden,(void *)0,0) ;
	XoSetAttr(ar[13],XoAttrRefShDico,(void *)0,0) ;
	XoAddInAttr(or,XoObjRefListAttr,ar[13],0) ;

	XoSetAttr(ar[14],XoAttrRefName,"min",0) ;
	XoSetAttr(ar[14],XoAttrRefTypeName,"XoString",0) ;
	XoSetAttr(ar[14],XoAttrRefList,(void *)0,0) ;
	XoSetAttr(ar[14],XoAttrRefLgMax,(void *)-1,0) ;
	XoSetAttr(ar[14],XoAttrRefNoSave,(void *)0,0) ;
	XoSetAttr(ar[14],XoAttrRefSaveUnset,(void *)0,0) ;
	XoSetAttr(ar[14],XoAttrRefEmbAttr,(void *)1,0) ;
	XoSetAttr(ar[14],XoAttrRefWrapHidden,(void *)0,0) ;
	XoSetAttr(ar[14],XoAttrRefShDico,(void *)0,0) ;
	XoAddInAttr(or,XoObjRefListAttr,ar[14],0) ;

	XoSetAttr(ar[15],XoAttrRefName,"max",0) ;
	XoSetAttr(ar[15],XoAttrRefTypeName,"XoString",0) ;
	XoSetAttr(ar[15],XoAttrRefList,(void *)0,0) ;
	XoSetAttr(ar[15],XoAttrRefLgMax,(void *)-1,0) ;
	XoSetAttr(ar[15],XoAttrRefNoSave,(void *)0,0) ;
	XoSetAttr(ar[15],XoAttrRefSaveUnset,(void *)0,0) ;
	XoSetAttr(ar[15],XoAttrRefEmbAttr,(void *)1,0) ;
	XoSetAttr(ar[15],XoAttrRefWrapHidden,(void *)0,0) ;
	XoSetAttr(ar[15],XoAttrRefShDico,(void *)0,0) ;
	XoAddInAttr(or,XoObjRefListAttr,ar[15],0) ;

	XoSetAttr(or,XoObjRefNbAttr,(void *)16,0) ;
	ret = -100 ;
	if (XoAddObjRef(or)<0) goto end ;
	ret = 0 ;
	end :
	for (i=0;i<16;i++)
		if (ar[i]) XoFree(ar[i],0) ;
	if (or) XoFree(or,0) ;
	return	ret ;
}
static	int32	GenXo_o:enum(void)
{
	int32	ret ;
	int32	i ;
	void	*or ;
	void	*ar[3] ;

	memset(ar,0,sizeof(ar));
	ret = -1 ;
	if (!(or=XoNew(XoObjRef))) goto end ;
	ret = -2 ;
	for (i=0;i<2;i++)
		if (!(ar[i]=XoNew(XoAttrRef))) goto end ;
	ret = -3 ;
	XoSetAttr(or,XoObjRefType,(void *)1,0) ;
	XoSetAttr(or,XoObjRefName,"o:enum",0) ;
	XoSetAttr(or,XoObjRefRoot,(void *)0,0) ;

	XoSetAttr(ar[0],XoAttrRefName,"name$",0) ;
	XoSetAttr(ar[0],XoAttrRefTypeName,"XoString",0) ;
	XoSetAttr(ar[0],XoAttrRefList,(void *)0,0) ;
	XoSetAttr(ar[0],XoAttrRefLgMax,(void *)-1,0) ;
	XoSetAttr(ar[0],XoAttrRefNoSave,(void *)0,0) ;
	XoSetAttr(ar[0],XoAttrRefSaveUnset,(void *)0,0) ;
	XoSetAttr(ar[0],XoAttrRefEmbAttr,(void *)0,0) ;
	XoSetAttr(ar[0],XoAttrRefWrapHidden,(void *)0,0) ;
	XoSetAttr(ar[0],XoAttrRefShDico,(void *)0,0) ;
	XoAddInAttr(or,XoObjRefListAttr,ar[0],0) ;

	XoSetAttr(ar[1],XoAttrRefName,"xmlns",0) ;
	XoSetAttr(ar[1],XoAttrRefTypeName,"XoString",0) ;
	XoSetAttr(ar[1],XoAttrRefList,(void *)0,0) ;
	XoSetAttr(ar[1],XoAttrRefLgMax,(void *)-1,0) ;
	XoSetAttr(ar[1],XoAttrRefNoSave,(void *)0,0) ;
	XoSetAttr(ar[1],XoAttrRefSaveUnset,(void *)0,0) ;
	XoSetAttr(ar[1],XoAttrRefEmbAttr,(void *)1,0) ;
	XoSetAttr(ar[1],XoAttrRefWrapHidden,(void *)0,0) ;
	XoSetAttr(ar[1],XoAttrRefShDico,(void *)1,0) ;
	XoAddInAttr(or,XoObjRefListAttr,ar[1],0) ;

	XoSetAttr(ar[2],XoAttrRefName,"custom$",0) ;
	XoSetAttr(ar[2],XoAttrRefTypeName,"o:t_xml_obix_attr",0) ;
	XoSetAttr(ar[2],XoAttrRefList,(void *)1,0) ;
	XoSetAttr(ar[2],XoAttrRefLgMax,(void *)-1,0) ;
	XoSetAttr(ar[2],XoAttrRefNoSave,(void *)0,0) ;
	XoSetAttr(ar[2],XoAttrRefSaveUnset,(void *)0,0) ;
	XoSetAttr(ar[2],XoAttrRefEmbAttr,(void *)1,0) ;
	XoSetAttr(ar[2],XoAttrRefWrapHidden,(void *)0,0) ;
	XoSetAttr(ar[2],XoAttrRefShDico,(void *)0,0) ;
	XoAddInAttr(or,XoObjRefListAttr,ar[2],0) ;

	XoSetAttr(ar[3],XoAttrRefName,"name",0) ;
	XoSetAttr(ar[3],XoAttrRefTypeName,"XoString",0) ;
	XoSetAttr(ar[3],XoAttrRefList,(void *)0,0) ;
	XoSetAttr(ar[3],XoAttrRefLgMax,(void *)-1,0) ;
	XoSetAttr(ar[3],XoAttrRefNoSave,(void *)0,0) ;
	XoSetAttr(ar[3],XoAttrRefSaveUnset,(void *)0,0) ;
	XoSetAttr(ar[3],XoAttrRefEmbAttr,(void *)1,0) ;
	XoSetAttr(ar[3],XoAttrRefWrapHidden,(void *)0,0) ;
	XoSetAttr(ar[3],XoAttrRefShDico,(void *)0,0) ;
	XoAddInAttr(or,XoObjRefListAttr,ar[3],0) ;

	XoSetAttr(ar[4],XoAttrRefName,"href",0) ;
	XoSetAttr(ar[4],XoAttrRefTypeName,"XoString",0) ;
	XoSetAttr(ar[4],XoAttrRefList,(void *)0,0) ;
	XoSetAttr(ar[4],XoAttrRefLgMax,(void *)-1,0) ;
	XoSetAttr(ar[4],XoAttrRefNoSave,(void *)0,0) ;
	XoSetAttr(ar[4],XoAttrRefSaveUnset,(void *)0,0) ;
	XoSetAttr(ar[4],XoAttrRefEmbAttr,(void *)1,0) ;
	XoSetAttr(ar[4],XoAttrRefWrapHidden,(void *)0,0) ;
	XoSetAttr(ar[4],XoAttrRefShDico,(void *)0,0) ;
	XoAddInAttr(or,XoObjRefListAttr,ar[4],0) ;

	XoSetAttr(ar[5],XoAttrRefName,"is",0) ;
	XoSetAttr(ar[5],XoAttrRefTypeName,"XoString",0) ;
	XoSetAttr(ar[5],XoAttrRefList,(void *)0,0) ;
	XoSetAttr(ar[5],XoAttrRefLgMax,(void *)-1,0) ;
	XoSetAttr(ar[5],XoAttrRefNoSave,(void *)0,0) ;
	XoSetAttr(ar[5],XoAttrRefSaveUnset,(void *)0,0) ;
	XoSetAttr(ar[5],XoAttrRefEmbAttr,(void *)1,0) ;
	XoSetAttr(ar[5],XoAttrRefWrapHidden,(void *)0,0) ;
	XoSetAttr(ar[5],XoAttrRefShDico,(void *)0,0) ;
	XoAddInAttr(or,XoObjRefListAttr,ar[5],0) ;

	XoSetAttr(ar[6],XoAttrRefName,"null",0) ;
	XoSetAttr(ar[6],XoAttrRefTypeName,"XoString",0) ;
	XoSetAttr(ar[6],XoAttrRefList,(void *)0,0) ;
	XoSetAttr(ar[6],XoAttrRefLgMax,(void *)5,0) ;
	XoSetAttr(ar[6],XoAttrRefNoSave,(void *)0,0) ;
	XoSetAttr(ar[6],XoAttrRefSaveUnset,(void *)0,0) ;
	XoSetAttr(ar[6],XoAttrRefEmbAttr,(void *)1,0) ;
	XoSetAttr(ar[6],XoAttrRefWrapHidden,(void *)0,0) ;
	XoSetAttr(ar[6],XoAttrRefShDico,(void *)0,0) ;
	XoAddInAttr(or,XoObjRefListAttr,ar[6],0) ;

	XoSetAttr(ar[7],XoAttrRefName,"icon",0) ;
	XoSetAttr(ar[7],XoAttrRefTypeName,"XoString",0) ;
	XoSetAttr(ar[7],XoAttrRefList,(void *)0,0) ;
	XoSetAttr(ar[7],XoAttrRefLgMax,(void *)-1,0) ;
	XoSetAttr(ar[7],XoAttrRefNoSave,(void *)0,0) ;
	XoSetAttr(ar[7],XoAttrRefSaveUnset,(void *)0,0) ;
	XoSetAttr(ar[7],XoAttrRefEmbAttr,(void *)1,0) ;
	XoSetAttr(ar[7],XoAttrRefWrapHidden,(void *)0,0) ;
	XoSetAttr(ar[7],XoAttrRefShDico,(void *)0,0) ;
	XoAddInAttr(or,XoObjRefListAttr,ar[7],0) ;

	XoSetAttr(ar[8],XoAttrRefName,"displayName",0) ;
	XoSetAttr(ar[8],XoAttrRefTypeName,"XoString",0) ;
	XoSetAttr(ar[8],XoAttrRefList,(void *)0,0) ;
	XoSetAttr(ar[8],XoAttrRefLgMax,(void *)-1,0) ;
	XoSetAttr(ar[8],XoAttrRefNoSave,(void *)0,0) ;
	XoSetAttr(ar[8],XoAttrRefSaveUnset,(void *)0,0) ;
	XoSetAttr(ar[8],XoAttrRefEmbAttr,(void *)1,0) ;
	XoSetAttr(ar[8],XoAttrRefWrapHidden,(void *)0,0) ;
	XoSetAttr(ar[8],XoAttrRefShDico,(void *)0,0) ;
	XoAddInAttr(or,XoObjRefListAttr,ar[8],0) ;

	XoSetAttr(ar[9],XoAttrRefName,"display",0) ;
	XoSetAttr(ar[9],XoAttrRefTypeName,"XoString",0) ;
	XoSetAttr(ar[9],XoAttrRefList,(void *)0,0) ;
	XoSetAttr(ar[9],XoAttrRefLgMax,(void *)-1,0) ;
	XoSetAttr(ar[9],XoAttrRefNoSave,(void *)0,0) ;
	XoSetAttr(ar[9],XoAttrRefSaveUnset,(void *)0,0) ;
	XoSetAttr(ar[9],XoAttrRefEmbAttr,(void *)1,0) ;
	XoSetAttr(ar[9],XoAttrRefWrapHidden,(void *)0,0) ;
	XoSetAttr(ar[9],XoAttrRefShDico,(void *)0,0) ;
	XoAddInAttr(or,XoObjRefListAttr,ar[9],0) ;

	XoSetAttr(ar[10],XoAttrRefName,"writable",0) ;
	XoSetAttr(ar[10],XoAttrRefTypeName,"XoString",0) ;
	XoSetAttr(ar[10],XoAttrRefList,(void *)0,0) ;
	XoSetAttr(ar[10],XoAttrRefLgMax,(void *)5,0) ;
	XoSetAttr(ar[10],XoAttrRefNoSave,(void *)0,0) ;
	XoSetAttr(ar[10],XoAttrRefSaveUnset,(void *)0,0) ;
	XoSetAttr(ar[10],XoAttrRefEmbAttr,(void *)1,0) ;
	XoSetAttr(ar[10],XoAttrRefWrapHidden,(void *)0,0) ;
	XoSetAttr(ar[10],XoAttrRefShDico,(void *)0,0) ;
	XoAddInAttr(or,XoObjRefListAttr,ar[10],0) ;

	XoSetAttr(ar[11],XoAttrRefName,"status",0) ;
	XoSetAttr(ar[11],XoAttrRefTypeName,"XoString",0) ;
	XoSetAttr(ar[11],XoAttrRefList,(void *)0,0) ;
	XoSetAttr(ar[11],XoAttrRefLgMax,(void *)-1,0) ;
	XoSetAttr(ar[11],XoAttrRefNoSave,(void *)0,0) ;
	XoSetAttr(ar[11],XoAttrRefSaveUnset,(void *)0,0) ;
	XoSetAttr(ar[11],XoAttrRefEmbAttr,(void *)1,0) ;
	XoSetAttr(ar[11],XoAttrRefWrapHidden,(void *)0,0) ;
	XoSetAttr(ar[11],XoAttrRefShDico,(void *)0,0) ;
	XoAddInAttr(or,XoObjRefListAttr,ar[11],0) ;

	XoSetAttr(ar[12],XoAttrRefName,"wrapper$",0) ;
	XoSetAttr(ar[12],XoAttrRefTypeName,"XoVoidObj",0) ;
	XoSetAttr(ar[12],XoAttrRefList,(void *)1,0) ;
	XoSetAttr(ar[12],XoAttrRefLgMax,(void *)-1,0) ;
	XoSetAttr(ar[12],XoAttrRefNoSave,(void *)0,0) ;
	XoSetAttr(ar[12],XoAttrRefSaveUnset,(void *)0,0) ;
	XoSetAttr(ar[12],XoAttrRefEmbAttr,(void *)0,0) ;
	XoSetAttr(ar[12],XoAttrRefWrapHidden,(void *)0,0) ;
	XoSetAttr(ar[12],XoAttrRefShDico,(void *)0,0) ;
	XoAddInAttr(or,XoObjRefListAttr,ar[12],0) ;

	XoSetAttr(ar[13],XoAttrRefName,"val",0) ;
	XoSetAttr(ar[13],XoAttrRefTypeName,"XoString",0) ;
	XoSetAttr(ar[13],XoAttrRefList,(void *)0,0) ;
	XoSetAttr(ar[13],XoAttrRefLgMax,(void *)-1,0) ;
	XoSetAttr(ar[13],XoAttrRefNoSave,(void *)0,0) ;
	XoSetAttr(ar[13],XoAttrRefSaveUnset,(void *)0,0) ;
	XoSetAttr(ar[13],XoAttrRefEmbAttr,(void *)1,0) ;
	XoSetAttr(ar[13],XoAttrRefWrapHidden,(void *)0,0) ;
	XoSetAttr(ar[13],XoAttrRefShDico,(void *)0,0) ;
	XoAddInAttr(or,XoObjRefListAttr,ar[13],0) ;

	XoSetAttr(ar[14],XoAttrRefName,"range",0) ;
	XoSetAttr(ar[14],XoAttrRefTypeName,"XoString",0) ;
	XoSetAttr(ar[14],XoAttrRefList,(void *)0,0) ;
	XoSetAttr(ar[14],XoAttrRefLgMax,(void *)-1,0) ;
	XoSetAttr(ar[14],XoAttrRefNoSave,(void *)0,0) ;
	XoSetAttr(ar[14],XoAttrRefSaveUnset,(void *)0,0) ;
	XoSetAttr(ar[14],XoAttrRefEmbAttr,(void *)1,0) ;
	XoSetAttr(ar[14],XoAttrRefWrapHidden,(void *)0,0) ;
	XoSetAttr(ar[14],XoAttrRefShDico,(void *)0,0) ;
	XoAddInAttr(or,XoObjRefListAttr,ar[14],0) ;

	XoSetAttr(or,XoObjRefNbAttr,(void *)15,0) ;
	ret = -100 ;
	if (XoAddObjRef(or)<0) goto end ;
	ret = 0 ;
	end :
	for (i=0;i<15;i++)
		if (ar[i]) XoFree(ar[i],0) ;
	if (or) XoFree(or,0) ;
	return	ret ;
}
static	int32	GenXo_o:uri(void)
{
	int32	ret ;
	int32	i ;
	void	*or ;
	void	*ar[2] ;

	memset(ar,0,sizeof(ar));
	ret = -1 ;
	if (!(or=XoNew(XoObjRef))) goto end ;
	ret = -2 ;
	for (i=0;i<1;i++)
		if (!(ar[i]=XoNew(XoAttrRef))) goto end ;
	ret = -3 ;
	XoSetAttr(or,XoObjRefType,(void *)1,0) ;
	XoSetAttr(or,XoObjRefName,"o:uri",0) ;
	XoSetAttr(or,XoObjRefRoot,(void *)0,0) ;

	XoSetAttr(ar[0],XoAttrRefName,"name$",0) ;
	XoSetAttr(ar[0],XoAttrRefTypeName,"XoString",0) ;
	XoSetAttr(ar[0],XoAttrRefList,(void *)0,0) ;
	XoSetAttr(ar[0],XoAttrRefLgMax,(void *)-1,0) ;
	XoSetAttr(ar[0],XoAttrRefNoSave,(void *)0,0) ;
	XoSetAttr(ar[0],XoAttrRefSaveUnset,(void *)0,0) ;
	XoSetAttr(ar[0],XoAttrRefEmbAttr,(void *)0,0) ;
	XoSetAttr(ar[0],XoAttrRefWrapHidden,(void *)0,0) ;
	XoSetAttr(ar[0],XoAttrRefShDico,(void *)0,0) ;
	XoAddInAttr(or,XoObjRefListAttr,ar[0],0) ;

	XoSetAttr(ar[1],XoAttrRefName,"xmlns",0) ;
	XoSetAttr(ar[1],XoAttrRefTypeName,"XoString",0) ;
	XoSetAttr(ar[1],XoAttrRefList,(void *)0,0) ;
	XoSetAttr(ar[1],XoAttrRefLgMax,(void *)-1,0) ;
	XoSetAttr(ar[1],XoAttrRefNoSave,(void *)0,0) ;
	XoSetAttr(ar[1],XoAttrRefSaveUnset,(void *)0,0) ;
	XoSetAttr(ar[1],XoAttrRefEmbAttr,(void *)1,0) ;
	XoSetAttr(ar[1],XoAttrRefWrapHidden,(void *)0,0) ;
	XoSetAttr(ar[1],XoAttrRefShDico,(void *)1,0) ;
	XoAddInAttr(or,XoObjRefListAttr,ar[1],0) ;

	XoSetAttr(ar[2],XoAttrRefName,"custom$",0) ;
	XoSetAttr(ar[2],XoAttrRefTypeName,"o:t_xml_obix_attr",0) ;
	XoSetAttr(ar[2],XoAttrRefList,(void *)1,0) ;
	XoSetAttr(ar[2],XoAttrRefLgMax,(void *)-1,0) ;
	XoSetAttr(ar[2],XoAttrRefNoSave,(void *)0,0) ;
	XoSetAttr(ar[2],XoAttrRefSaveUnset,(void *)0,0) ;
	XoSetAttr(ar[2],XoAttrRefEmbAttr,(void *)1,0) ;
	XoSetAttr(ar[2],XoAttrRefWrapHidden,(void *)0,0) ;
	XoSetAttr(ar[2],XoAttrRefShDico,(void *)0,0) ;
	XoAddInAttr(or,XoObjRefListAttr,ar[2],0) ;

	XoSetAttr(ar[3],XoAttrRefName,"name",0) ;
	XoSetAttr(ar[3],XoAttrRefTypeName,"XoString",0) ;
	XoSetAttr(ar[3],XoAttrRefList,(void *)0,0) ;
	XoSetAttr(ar[3],XoAttrRefLgMax,(void *)-1,0) ;
	XoSetAttr(ar[3],XoAttrRefNoSave,(void *)0,0) ;
	XoSetAttr(ar[3],XoAttrRefSaveUnset,(void *)0,0) ;
	XoSetAttr(ar[3],XoAttrRefEmbAttr,(void *)1,0) ;
	XoSetAttr(ar[3],XoAttrRefWrapHidden,(void *)0,0) ;
	XoSetAttr(ar[3],XoAttrRefShDico,(void *)0,0) ;
	XoAddInAttr(or,XoObjRefListAttr,ar[3],0) ;

	XoSetAttr(ar[4],XoAttrRefName,"href",0) ;
	XoSetAttr(ar[4],XoAttrRefTypeName,"XoString",0) ;
	XoSetAttr(ar[4],XoAttrRefList,(void *)0,0) ;
	XoSetAttr(ar[4],XoAttrRefLgMax,(void *)-1,0) ;
	XoSetAttr(ar[4],XoAttrRefNoSave,(void *)0,0) ;
	XoSetAttr(ar[4],XoAttrRefSaveUnset,(void *)0,0) ;
	XoSetAttr(ar[4],XoAttrRefEmbAttr,(void *)1,0) ;
	XoSetAttr(ar[4],XoAttrRefWrapHidden,(void *)0,0) ;
	XoSetAttr(ar[4],XoAttrRefShDico,(void *)0,0) ;
	XoAddInAttr(or,XoObjRefListAttr,ar[4],0) ;

	XoSetAttr(ar[5],XoAttrRefName,"is",0) ;
	XoSetAttr(ar[5],XoAttrRefTypeName,"XoString",0) ;
	XoSetAttr(ar[5],XoAttrRefList,(void *)0,0) ;
	XoSetAttr(ar[5],XoAttrRefLgMax,(void *)-1,0) ;
	XoSetAttr(ar[5],XoAttrRefNoSave,(void *)0,0) ;
	XoSetAttr(ar[5],XoAttrRefSaveUnset,(void *)0,0) ;
	XoSetAttr(ar[5],XoAttrRefEmbAttr,(void *)1,0) ;
	XoSetAttr(ar[5],XoAttrRefWrapHidden,(void *)0,0) ;
	XoSetAttr(ar[5],XoAttrRefShDico,(void *)0,0) ;
	XoAddInAttr(or,XoObjRefListAttr,ar[5],0) ;

	XoSetAttr(ar[6],XoAttrRefName,"null",0) ;
	XoSetAttr(ar[6],XoAttrRefTypeName,"XoString",0) ;
	XoSetAttr(ar[6],XoAttrRefList,(void *)0,0) ;
	XoSetAttr(ar[6],XoAttrRefLgMax,(void *)5,0) ;
	XoSetAttr(ar[6],XoAttrRefNoSave,(void *)0,0) ;
	XoSetAttr(ar[6],XoAttrRefSaveUnset,(void *)0,0) ;
	XoSetAttr(ar[6],XoAttrRefEmbAttr,(void *)1,0) ;
	XoSetAttr(ar[6],XoAttrRefWrapHidden,(void *)0,0) ;
	XoSetAttr(ar[6],XoAttrRefShDico,(void *)0,0) ;
	XoAddInAttr(or,XoObjRefListAttr,ar[6],0) ;

	XoSetAttr(ar[7],XoAttrRefName,"icon",0) ;
	XoSetAttr(ar[7],XoAttrRefTypeName,"XoString",0) ;
	XoSetAttr(ar[7],XoAttrRefList,(void *)0,0) ;
	XoSetAttr(ar[7],XoAttrRefLgMax,(void *)-1,0) ;
	XoSetAttr(ar[7],XoAttrRefNoSave,(void *)0,0) ;
	XoSetAttr(ar[7],XoAttrRefSaveUnset,(void *)0,0) ;
	XoSetAttr(ar[7],XoAttrRefEmbAttr,(void *)1,0) ;
	XoSetAttr(ar[7],XoAttrRefWrapHidden,(void *)0,0) ;
	XoSetAttr(ar[7],XoAttrRefShDico,(void *)0,0) ;
	XoAddInAttr(or,XoObjRefListAttr,ar[7],0) ;

	XoSetAttr(ar[8],XoAttrRefName,"displayName",0) ;
	XoSetAttr(ar[8],XoAttrRefTypeName,"XoString",0) ;
	XoSetAttr(ar[8],XoAttrRefList,(void *)0,0) ;
	XoSetAttr(ar[8],XoAttrRefLgMax,(void *)-1,0) ;
	XoSetAttr(ar[8],XoAttrRefNoSave,(void *)0,0) ;
	XoSetAttr(ar[8],XoAttrRefSaveUnset,(void *)0,0) ;
	XoSetAttr(ar[8],XoAttrRefEmbAttr,(void *)1,0) ;
	XoSetAttr(ar[8],XoAttrRefWrapHidden,(void *)0,0) ;
	XoSetAttr(ar[8],XoAttrRefShDico,(void *)0,0) ;
	XoAddInAttr(or,XoObjRefListAttr,ar[8],0) ;

	XoSetAttr(ar[9],XoAttrRefName,"display",0) ;
	XoSetAttr(ar[9],XoAttrRefTypeName,"XoString",0) ;
	XoSetAttr(ar[9],XoAttrRefList,(void *)0,0) ;
	XoSetAttr(ar[9],XoAttrRefLgMax,(void *)-1,0) ;
	XoSetAttr(ar[9],XoAttrRefNoSave,(void *)0,0) ;
	XoSetAttr(ar[9],XoAttrRefSaveUnset,(void *)0,0) ;
	XoSetAttr(ar[9],XoAttrRefEmbAttr,(void *)1,0) ;
	XoSetAttr(ar[9],XoAttrRefWrapHidden,(void *)0,0) ;
	XoSetAttr(ar[9],XoAttrRefShDico,(void *)0,0) ;
	XoAddInAttr(or,XoObjRefListAttr,ar[9],0) ;

	XoSetAttr(ar[10],XoAttrRefName,"writable",0) ;
	XoSetAttr(ar[10],XoAttrRefTypeName,"XoString",0) ;
	XoSetAttr(ar[10],XoAttrRefList,(void *)0,0) ;
	XoSetAttr(ar[10],XoAttrRefLgMax,(void *)5,0) ;
	XoSetAttr(ar[10],XoAttrRefNoSave,(void *)0,0) ;
	XoSetAttr(ar[10],XoAttrRefSaveUnset,(void *)0,0) ;
	XoSetAttr(ar[10],XoAttrRefEmbAttr,(void *)1,0) ;
	XoSetAttr(ar[10],XoAttrRefWrapHidden,(void *)0,0) ;
	XoSetAttr(ar[10],XoAttrRefShDico,(void *)0,0) ;
	XoAddInAttr(or,XoObjRefListAttr,ar[10],0) ;

	XoSetAttr(ar[11],XoAttrRefName,"status",0) ;
	XoSetAttr(ar[11],XoAttrRefTypeName,"XoString",0) ;
	XoSetAttr(ar[11],XoAttrRefList,(void *)0,0) ;
	XoSetAttr(ar[11],XoAttrRefLgMax,(void *)-1,0) ;
	XoSetAttr(ar[11],XoAttrRefNoSave,(void *)0,0) ;
	XoSetAttr(ar[11],XoAttrRefSaveUnset,(void *)0,0) ;
	XoSetAttr(ar[11],XoAttrRefEmbAttr,(void *)1,0) ;
	XoSetAttr(ar[11],XoAttrRefWrapHidden,(void *)0,0) ;
	XoSetAttr(ar[11],XoAttrRefShDico,(void *)0,0) ;
	XoAddInAttr(or,XoObjRefListAttr,ar[11],0) ;

	XoSetAttr(ar[12],XoAttrRefName,"wrapper$",0) ;
	XoSetAttr(ar[12],XoAttrRefTypeName,"XoVoidObj",0) ;
	XoSetAttr(ar[12],XoAttrRefList,(void *)1,0) ;
	XoSetAttr(ar[12],XoAttrRefLgMax,(void *)-1,0) ;
	XoSetAttr(ar[12],XoAttrRefNoSave,(void *)0,0) ;
	XoSetAttr(ar[12],XoAttrRefSaveUnset,(void *)0,0) ;
	XoSetAttr(ar[12],XoAttrRefEmbAttr,(void *)0,0) ;
	XoSetAttr(ar[12],XoAttrRefWrapHidden,(void *)0,0) ;
	XoSetAttr(ar[12],XoAttrRefShDico,(void *)0,0) ;
	XoAddInAttr(or,XoObjRefListAttr,ar[12],0) ;

	XoSetAttr(ar[13],XoAttrRefName,"val",0) ;
	XoSetAttr(ar[13],XoAttrRefTypeName,"XoString",0) ;
	XoSetAttr(ar[13],XoAttrRefList,(void *)0,0) ;
	XoSetAttr(ar[13],XoAttrRefLgMax,(void *)-1,0) ;
	XoSetAttr(ar[13],XoAttrRefNoSave,(void *)0,0) ;
	XoSetAttr(ar[13],XoAttrRefSaveUnset,(void *)0,0) ;
	XoSetAttr(ar[13],XoAttrRefEmbAttr,(void *)1,0) ;
	XoSetAttr(ar[13],XoAttrRefWrapHidden,(void *)0,0) ;
	XoSetAttr(ar[13],XoAttrRefShDico,(void *)0,0) ;
	XoAddInAttr(or,XoObjRefListAttr,ar[13],0) ;

	XoSetAttr(or,XoObjRefNbAttr,(void *)14,0) ;
	ret = -100 ;
	if (XoAddObjRef(or)<0) goto end ;
	ret = 0 ;
	end :
	for (i=0;i<14;i++)
		if (ar[i]) XoFree(ar[i],0) ;
	if (or) XoFree(or,0) ;
	return	ret ;
}
static	int32	GenXo_o:abstime(void)
{
	int32	ret ;
	int32	i ;
	void	*or ;
	void	*ar[5] ;

	memset(ar,0,sizeof(ar));
	ret = -1 ;
	if (!(or=XoNew(XoObjRef))) goto end ;
	ret = -2 ;
	for (i=0;i<4;i++)
		if (!(ar[i]=XoNew(XoAttrRef))) goto end ;
	ret = -3 ;
	XoSetAttr(or,XoObjRefType,(void *)1,0) ;
	XoSetAttr(or,XoObjRefName,"o:abstime",0) ;
	XoSetAttr(or,XoObjRefRoot,(void *)0,0) ;

	XoSetAttr(ar[0],XoAttrRefName,"name$",0) ;
	XoSetAttr(ar[0],XoAttrRefTypeName,"XoString",0) ;
	XoSetAttr(ar[0],XoAttrRefList,(void *)0,0) ;
	XoSetAttr(ar[0],XoAttrRefLgMax,(void *)-1,0) ;
	XoSetAttr(ar[0],XoAttrRefNoSave,(void *)0,0) ;
	XoSetAttr(ar[0],XoAttrRefSaveUnset,(void *)0,0) ;
	XoSetAttr(ar[0],XoAttrRefEmbAttr,(void *)0,0) ;
	XoSetAttr(ar[0],XoAttrRefWrapHidden,(void *)0,0) ;
	XoSetAttr(ar[0],XoAttrRefShDico,(void *)0,0) ;
	XoAddInAttr(or,XoObjRefListAttr,ar[0],0) ;

	XoSetAttr(ar[1],XoAttrRefName,"xmlns",0) ;
	XoSetAttr(ar[1],XoAttrRefTypeName,"XoString",0) ;
	XoSetAttr(ar[1],XoAttrRefList,(void *)0,0) ;
	XoSetAttr(ar[1],XoAttrRefLgMax,(void *)-1,0) ;
	XoSetAttr(ar[1],XoAttrRefNoSave,(void *)0,0) ;
	XoSetAttr(ar[1],XoAttrRefSaveUnset,(void *)0,0) ;
	XoSetAttr(ar[1],XoAttrRefEmbAttr,(void *)1,0) ;
	XoSetAttr(ar[1],XoAttrRefWrapHidden,(void *)0,0) ;
	XoSetAttr(ar[1],XoAttrRefShDico,(void *)1,0) ;
	XoAddInAttr(or,XoObjRefListAttr,ar[1],0) ;

	XoSetAttr(ar[2],XoAttrRefName,"custom$",0) ;
	XoSetAttr(ar[2],XoAttrRefTypeName,"o:t_xml_obix_attr",0) ;
	XoSetAttr(ar[2],XoAttrRefList,(void *)1,0) ;
	XoSetAttr(ar[2],XoAttrRefLgMax,(void *)-1,0) ;
	XoSetAttr(ar[2],XoAttrRefNoSave,(void *)0,0) ;
	XoSetAttr(ar[2],XoAttrRefSaveUnset,(void *)0,0) ;
	XoSetAttr(ar[2],XoAttrRefEmbAttr,(void *)1,0) ;
	XoSetAttr(ar[2],XoAttrRefWrapHidden,(void *)0,0) ;
	XoSetAttr(ar[2],XoAttrRefShDico,(void *)0,0) ;
	XoAddInAttr(or,XoObjRefListAttr,ar[2],0) ;

	XoSetAttr(ar[3],XoAttrRefName,"name",0) ;
	XoSetAttr(ar[3],XoAttrRefTypeName,"XoString",0) ;
	XoSetAttr(ar[3],XoAttrRefList,(void *)0,0) ;
	XoSetAttr(ar[3],XoAttrRefLgMax,(void *)-1,0) ;
	XoSetAttr(ar[3],XoAttrRefNoSave,(void *)0,0) ;
	XoSetAttr(ar[3],XoAttrRefSaveUnset,(void *)0,0) ;
	XoSetAttr(ar[3],XoAttrRefEmbAttr,(void *)1,0) ;
	XoSetAttr(ar[3],XoAttrRefWrapHidden,(void *)0,0) ;
	XoSetAttr(ar[3],XoAttrRefShDico,(void *)0,0) ;
	XoAddInAttr(or,XoObjRefListAttr,ar[3],0) ;

	XoSetAttr(ar[4],XoAttrRefName,"href",0) ;
	XoSetAttr(ar[4],XoAttrRefTypeName,"XoString",0) ;
	XoSetAttr(ar[4],XoAttrRefList,(void *)0,0) ;
	XoSetAttr(ar[4],XoAttrRefLgMax,(void *)-1,0) ;
	XoSetAttr(ar[4],XoAttrRefNoSave,(void *)0,0) ;
	XoSetAttr(ar[4],XoAttrRefSaveUnset,(void *)0,0) ;
	XoSetAttr(ar[4],XoAttrRefEmbAttr,(void *)1,0) ;
	XoSetAttr(ar[4],XoAttrRefWrapHidden,(void *)0,0) ;
	XoSetAttr(ar[4],XoAttrRefShDico,(void *)0,0) ;
	XoAddInAttr(or,XoObjRefListAttr,ar[4],0) ;

	XoSetAttr(ar[5],XoAttrRefName,"is",0) ;
	XoSetAttr(ar[5],XoAttrRefTypeName,"XoString",0) ;
	XoSetAttr(ar[5],XoAttrRefList,(void *)0,0) ;
	XoSetAttr(ar[5],XoAttrRefLgMax,(void *)-1,0) ;
	XoSetAttr(ar[5],XoAttrRefNoSave,(void *)0,0) ;
	XoSetAttr(ar[5],XoAttrRefSaveUnset,(void *)0,0) ;
	XoSetAttr(ar[5],XoAttrRefEmbAttr,(void *)1,0) ;
	XoSetAttr(ar[5],XoAttrRefWrapHidden,(void *)0,0) ;
	XoSetAttr(ar[5],XoAttrRefShDico,(void *)0,0) ;
	XoAddInAttr(or,XoObjRefListAttr,ar[5],0) ;

	XoSetAttr(ar[6],XoAttrRefName,"null",0) ;
	XoSetAttr(ar[6],XoAttrRefTypeName,"XoString",0) ;
	XoSetAttr(ar[6],XoAttrRefList,(void *)0,0) ;
	XoSetAttr(ar[6],XoAttrRefLgMax,(void *)5,0) ;
	XoSetAttr(ar[6],XoAttrRefNoSave,(void *)0,0) ;
	XoSetAttr(ar[6],XoAttrRefSaveUnset,(void *)0,0) ;
	XoSetAttr(ar[6],XoAttrRefEmbAttr,(void *)1,0) ;
	XoSetAttr(ar[6],XoAttrRefWrapHidden,(void *)0,0) ;
	XoSetAttr(ar[6],XoAttrRefShDico,(void *)0,0) ;
	XoAddInAttr(or,XoObjRefListAttr,ar[6],0) ;

	XoSetAttr(ar[7],XoAttrRefName,"icon",0) ;
	XoSetAttr(ar[7],XoAttrRefTypeName,"XoString",0) ;
	XoSetAttr(ar[7],XoAttrRefList,(void *)0,0) ;
	XoSetAttr(ar[7],XoAttrRefLgMax,(void *)-1,0) ;
	XoSetAttr(ar[7],XoAttrRefNoSave,(void *)0,0) ;
	XoSetAttr(ar[7],XoAttrRefSaveUnset,(void *)0,0) ;
	XoSetAttr(ar[7],XoAttrRefEmbAttr,(void *)1,0) ;
	XoSetAttr(ar[7],XoAttrRefWrapHidden,(void *)0,0) ;
	XoSetAttr(ar[7],XoAttrRefShDico,(void *)0,0) ;
	XoAddInAttr(or,XoObjRefListAttr,ar[7],0) ;

	XoSetAttr(ar[8],XoAttrRefName,"displayName",0) ;
	XoSetAttr(ar[8],XoAttrRefTypeName,"XoString",0) ;
	XoSetAttr(ar[8],XoAttrRefList,(void *)0,0) ;
	XoSetAttr(ar[8],XoAttrRefLgMax,(void *)-1,0) ;
	XoSetAttr(ar[8],XoAttrRefNoSave,(void *)0,0) ;
	XoSetAttr(ar[8],XoAttrRefSaveUnset,(void *)0,0) ;
	XoSetAttr(ar[8],XoAttrRefEmbAttr,(void *)1,0) ;
	XoSetAttr(ar[8],XoAttrRefWrapHidden,(void *)0,0) ;
	XoSetAttr(ar[8],XoAttrRefShDico,(void *)0,0) ;
	XoAddInAttr(or,XoObjRefListAttr,ar[8],0) ;

	XoSetAttr(ar[9],XoAttrRefName,"display",0) ;
	XoSetAttr(ar[9],XoAttrRefTypeName,"XoString",0) ;
	XoSetAttr(ar[9],XoAttrRefList,(void *)0,0) ;
	XoSetAttr(ar[9],XoAttrRefLgMax,(void *)-1,0) ;
	XoSetAttr(ar[9],XoAttrRefNoSave,(void *)0,0) ;
	XoSetAttr(ar[9],XoAttrRefSaveUnset,(void *)0,0) ;
	XoSetAttr(ar[9],XoAttrRefEmbAttr,(void *)1,0) ;
	XoSetAttr(ar[9],XoAttrRefWrapHidden,(void *)0,0) ;
	XoSetAttr(ar[9],XoAttrRefShDico,(void *)0,0) ;
	XoAddInAttr(or,XoObjRefListAttr,ar[9],0) ;

	XoSetAttr(ar[10],XoAttrRefName,"writable",0) ;
	XoSetAttr(ar[10],XoAttrRefTypeName,"XoString",0) ;
	XoSetAttr(ar[10],XoAttrRefList,(void *)0,0) ;
	XoSetAttr(ar[10],XoAttrRefLgMax,(void *)5,0) ;
	XoSetAttr(ar[10],XoAttrRefNoSave,(void *)0,0) ;
	XoSetAttr(ar[10],XoAttrRefSaveUnset,(void *)0,0) ;
	XoSetAttr(ar[10],XoAttrRefEmbAttr,(void *)1,0) ;
	XoSetAttr(ar[10],XoAttrRefWrapHidden,(void *)0,0) ;
	XoSetAttr(ar[10],XoAttrRefShDico,(void *)0,0) ;
	XoAddInAttr(or,XoObjRefListAttr,ar[10],0) ;

	XoSetAttr(ar[11],XoAttrRefName,"status",0) ;
	XoSetAttr(ar[11],XoAttrRefTypeName,"XoString",0) ;
	XoSetAttr(ar[11],XoAttrRefList,(void *)0,0) ;
	XoSetAttr(ar[11],XoAttrRefLgMax,(void *)-1,0) ;
	XoSetAttr(ar[11],XoAttrRefNoSave,(void *)0,0) ;
	XoSetAttr(ar[11],XoAttrRefSaveUnset,(void *)0,0) ;
	XoSetAttr(ar[11],XoAttrRefEmbAttr,(void *)1,0) ;
	XoSetAttr(ar[11],XoAttrRefWrapHidden,(void *)0,0) ;
	XoSetAttr(ar[11],XoAttrRefShDico,(void *)0,0) ;
	XoAddInAttr(or,XoObjRefListAttr,ar[11],0) ;

	XoSetAttr(ar[12],XoAttrRefName,"wrapper$",0) ;
	XoSetAttr(ar[12],XoAttrRefTypeName,"XoVoidObj",0) ;
	XoSetAttr(ar[12],XoAttrRefList,(void *)1,0) ;
	XoSetAttr(ar[12],XoAttrRefLgMax,(void *)-1,0) ;
	XoSetAttr(ar[12],XoAttrRefNoSave,(void *)0,0) ;
	XoSetAttr(ar[12],XoAttrRefSaveUnset,(void *)0,0) ;
	XoSetAttr(ar[12],XoAttrRefEmbAttr,(void *)0,0) ;
	XoSetAttr(ar[12],XoAttrRefWrapHidden,(void *)0,0) ;
	XoSetAttr(ar[12],XoAttrRefShDico,(void *)0,0) ;
	XoAddInAttr(or,XoObjRefListAttr,ar[12],0) ;

	XoSetAttr(ar[13],XoAttrRefName,"val",0) ;
	XoSetAttr(ar[13],XoAttrRefTypeName,"XoString",0) ;
	XoSetAttr(ar[13],XoAttrRefList,(void *)0,0) ;
	XoSetAttr(ar[13],XoAttrRefLgMax,(void *)-1,0) ;
	XoSetAttr(ar[13],XoAttrRefNoSave,(void *)0,0) ;
	XoSetAttr(ar[13],XoAttrRefSaveUnset,(void *)0,0) ;
	XoSetAttr(ar[13],XoAttrRefEmbAttr,(void *)1,0) ;
	XoSetAttr(ar[13],XoAttrRefWrapHidden,(void *)0,0) ;
	XoSetAttr(ar[13],XoAttrRefShDico,(void *)0,0) ;
	XoAddInAttr(or,XoObjRefListAttr,ar[13],0) ;

	XoSetAttr(ar[14],XoAttrRefName,"min",0) ;
	XoSetAttr(ar[14],XoAttrRefTypeName,"XoString",0) ;
	XoSetAttr(ar[14],XoAttrRefList,(void *)0,0) ;
	XoSetAttr(ar[14],XoAttrRefLgMax,(void *)-1,0) ;
	XoSetAttr(ar[14],XoAttrRefNoSave,(void *)0,0) ;
	XoSetAttr(ar[14],XoAttrRefSaveUnset,(void *)0,0) ;
	XoSetAttr(ar[14],XoAttrRefEmbAttr,(void *)1,0) ;
	XoSetAttr(ar[14],XoAttrRefWrapHidden,(void *)0,0) ;
	XoSetAttr(ar[14],XoAttrRefShDico,(void *)0,0) ;
	XoAddInAttr(or,XoObjRefListAttr,ar[14],0) ;

	XoSetAttr(ar[15],XoAttrRefName,"max",0) ;
	XoSetAttr(ar[15],XoAttrRefTypeName,"XoString",0) ;
	XoSetAttr(ar[15],XoAttrRefList,(void *)0,0) ;
	XoSetAttr(ar[15],XoAttrRefLgMax,(void *)-1,0) ;
	XoSetAttr(ar[15],XoAttrRefNoSave,(void *)0,0) ;
	XoSetAttr(ar[15],XoAttrRefSaveUnset,(void *)0,0) ;
	XoSetAttr(ar[15],XoAttrRefEmbAttr,(void *)1,0) ;
	XoSetAttr(ar[15],XoAttrRefWrapHidden,(void *)0,0) ;
	XoSetAttr(ar[15],XoAttrRefShDico,(void *)0,0) ;
	XoAddInAttr(or,XoObjRefListAttr,ar[15],0) ;

	XoSetAttr(ar[16],XoAttrRefName,"tz",0) ;
	XoSetAttr(ar[16],XoAttrRefTypeName,"XoString",0) ;
	XoSetAttr(ar[16],XoAttrRefList,(void *)0,0) ;
	XoSetAttr(ar[16],XoAttrRefLgMax,(void *)-1,0) ;
	XoSetAttr(ar[16],XoAttrRefNoSave,(void *)0,0) ;
	XoSetAttr(ar[16],XoAttrRefSaveUnset,(void *)0,0) ;
	XoSetAttr(ar[16],XoAttrRefEmbAttr,(void *)1,0) ;
	XoSetAttr(ar[16],XoAttrRefWrapHidden,(void *)0,0) ;
	XoSetAttr(ar[16],XoAttrRefShDico,(void *)0,0) ;
	XoAddInAttr(or,XoObjRefListAttr,ar[16],0) ;

	XoSetAttr(or,XoObjRefNbAttr,(void *)17,0) ;
	ret = -100 ;
	if (XoAddObjRef(or)<0) goto end ;
	ret = 0 ;
	end :
	for (i=0;i<17;i++)
		if (ar[i]) XoFree(ar[i],0) ;
	if (or) XoFree(or,0) ;
	return	ret ;
}
static	int32	GenXo_o:reltime(void)
{
	int32	ret ;
	int32	i ;
	void	*or ;
	void	*ar[4] ;

	memset(ar,0,sizeof(ar));
	ret = -1 ;
	if (!(or=XoNew(XoObjRef))) goto end ;
	ret = -2 ;
	for (i=0;i<3;i++)
		if (!(ar[i]=XoNew(XoAttrRef))) goto end ;
	ret = -3 ;
	XoSetAttr(or,XoObjRefType,(void *)1,0) ;
	XoSetAttr(or,XoObjRefName,"o:reltime",0) ;
	XoSetAttr(or,XoObjRefRoot,(void *)0,0) ;

	XoSetAttr(ar[0],XoAttrRefName,"name$",0) ;
	XoSetAttr(ar[0],XoAttrRefTypeName,"XoString",0) ;
	XoSetAttr(ar[0],XoAttrRefList,(void *)0,0) ;
	XoSetAttr(ar[0],XoAttrRefLgMax,(void *)-1,0) ;
	XoSetAttr(ar[0],XoAttrRefNoSave,(void *)0,0) ;
	XoSetAttr(ar[0],XoAttrRefSaveUnset,(void *)0,0) ;
	XoSetAttr(ar[0],XoAttrRefEmbAttr,(void *)0,0) ;
	XoSetAttr(ar[0],XoAttrRefWrapHidden,(void *)0,0) ;
	XoSetAttr(ar[0],XoAttrRefShDico,(void *)0,0) ;
	XoAddInAttr(or,XoObjRefListAttr,ar[0],0) ;

	XoSetAttr(ar[1],XoAttrRefName,"xmlns",0) ;
	XoSetAttr(ar[1],XoAttrRefTypeName,"XoString",0) ;
	XoSetAttr(ar[1],XoAttrRefList,(void *)0,0) ;
	XoSetAttr(ar[1],XoAttrRefLgMax,(void *)-1,0) ;
	XoSetAttr(ar[1],XoAttrRefNoSave,(void *)0,0) ;
	XoSetAttr(ar[1],XoAttrRefSaveUnset,(void *)0,0) ;
	XoSetAttr(ar[1],XoAttrRefEmbAttr,(void *)1,0) ;
	XoSetAttr(ar[1],XoAttrRefWrapHidden,(void *)0,0) ;
	XoSetAttr(ar[1],XoAttrRefShDico,(void *)1,0) ;
	XoAddInAttr(or,XoObjRefListAttr,ar[1],0) ;

	XoSetAttr(ar[2],XoAttrRefName,"custom$",0) ;
	XoSetAttr(ar[2],XoAttrRefTypeName,"o:t_xml_obix_attr",0) ;
	XoSetAttr(ar[2],XoAttrRefList,(void *)1,0) ;
	XoSetAttr(ar[2],XoAttrRefLgMax,(void *)-1,0) ;
	XoSetAttr(ar[2],XoAttrRefNoSave,(void *)0,0) ;
	XoSetAttr(ar[2],XoAttrRefSaveUnset,(void *)0,0) ;
	XoSetAttr(ar[2],XoAttrRefEmbAttr,(void *)1,0) ;
	XoSetAttr(ar[2],XoAttrRefWrapHidden,(void *)0,0) ;
	XoSetAttr(ar[2],XoAttrRefShDico,(void *)0,0) ;
	XoAddInAttr(or,XoObjRefListAttr,ar[2],0) ;

	XoSetAttr(ar[3],XoAttrRefName,"name",0) ;
	XoSetAttr(ar[3],XoAttrRefTypeName,"XoString",0) ;
	XoSetAttr(ar[3],XoAttrRefList,(void *)0,0) ;
	XoSetAttr(ar[3],XoAttrRefLgMax,(void *)-1,0) ;
	XoSetAttr(ar[3],XoAttrRefNoSave,(void *)0,0) ;
	XoSetAttr(ar[3],XoAttrRefSaveUnset,(void *)0,0) ;
	XoSetAttr(ar[3],XoAttrRefEmbAttr,(void *)1,0) ;
	XoSetAttr(ar[3],XoAttrRefWrapHidden,(void *)0,0) ;
	XoSetAttr(ar[3],XoAttrRefShDico,(void *)0,0) ;
	XoAddInAttr(or,XoObjRefListAttr,ar[3],0) ;

	XoSetAttr(ar[4],XoAttrRefName,"href",0) ;
	XoSetAttr(ar[4],XoAttrRefTypeName,"XoString",0) ;
	XoSetAttr(ar[4],XoAttrRefList,(void *)0,0) ;
	XoSetAttr(ar[4],XoAttrRefLgMax,(void *)-1,0) ;
	XoSetAttr(ar[4],XoAttrRefNoSave,(void *)0,0) ;
	XoSetAttr(ar[4],XoAttrRefSaveUnset,(void *)0,0) ;
	XoSetAttr(ar[4],XoAttrRefEmbAttr,(void *)1,0) ;
	XoSetAttr(ar[4],XoAttrRefWrapHidden,(void *)0,0) ;
	XoSetAttr(ar[4],XoAttrRefShDico,(void *)0,0) ;
	XoAddInAttr(or,XoObjRefListAttr,ar[4],0) ;

	XoSetAttr(ar[5],XoAttrRefName,"is",0) ;
	XoSetAttr(ar[5],XoAttrRefTypeName,"XoString",0) ;
	XoSetAttr(ar[5],XoAttrRefList,(void *)0,0) ;
	XoSetAttr(ar[5],XoAttrRefLgMax,(void *)-1,0) ;
	XoSetAttr(ar[5],XoAttrRefNoSave,(void *)0,0) ;
	XoSetAttr(ar[5],XoAttrRefSaveUnset,(void *)0,0) ;
	XoSetAttr(ar[5],XoAttrRefEmbAttr,(void *)1,0) ;
	XoSetAttr(ar[5],XoAttrRefWrapHidden,(void *)0,0) ;
	XoSetAttr(ar[5],XoAttrRefShDico,(void *)0,0) ;
	XoAddInAttr(or,XoObjRefListAttr,ar[5],0) ;

	XoSetAttr(ar[6],XoAttrRefName,"null",0) ;
	XoSetAttr(ar[6],XoAttrRefTypeName,"XoString",0) ;
	XoSetAttr(ar[6],XoAttrRefList,(void *)0,0) ;
	XoSetAttr(ar[6],XoAttrRefLgMax,(void *)5,0) ;
	XoSetAttr(ar[6],XoAttrRefNoSave,(void *)0,0) ;
	XoSetAttr(ar[6],XoAttrRefSaveUnset,(void *)0,0) ;
	XoSetAttr(ar[6],XoAttrRefEmbAttr,(void *)1,0) ;
	XoSetAttr(ar[6],XoAttrRefWrapHidden,(void *)0,0) ;
	XoSetAttr(ar[6],XoAttrRefShDico,(void *)0,0) ;
	XoAddInAttr(or,XoObjRefListAttr,ar[6],0) ;

	XoSetAttr(ar[7],XoAttrRefName,"icon",0) ;
	XoSetAttr(ar[7],XoAttrRefTypeName,"XoString",0) ;
	XoSetAttr(ar[7],XoAttrRefList,(void *)0,0) ;
	XoSetAttr(ar[7],XoAttrRefLgMax,(void *)-1,0) ;
	XoSetAttr(ar[7],XoAttrRefNoSave,(void *)0,0) ;
	XoSetAttr(ar[7],XoAttrRefSaveUnset,(void *)0,0) ;
	XoSetAttr(ar[7],XoAttrRefEmbAttr,(void *)1,0) ;
	XoSetAttr(ar[7],XoAttrRefWrapHidden,(void *)0,0) ;
	XoSetAttr(ar[7],XoAttrRefShDico,(void *)0,0) ;
	XoAddInAttr(or,XoObjRefListAttr,ar[7],0) ;

	XoSetAttr(ar[8],XoAttrRefName,"displayName",0) ;
	XoSetAttr(ar[8],XoAttrRefTypeName,"XoString",0) ;
	XoSetAttr(ar[8],XoAttrRefList,(void *)0,0) ;
	XoSetAttr(ar[8],XoAttrRefLgMax,(void *)-1,0) ;
	XoSetAttr(ar[8],XoAttrRefNoSave,(void *)0,0) ;
	XoSetAttr(ar[8],XoAttrRefSaveUnset,(void *)0,0) ;
	XoSetAttr(ar[8],XoAttrRefEmbAttr,(void *)1,0) ;
	XoSetAttr(ar[8],XoAttrRefWrapHidden,(void *)0,0) ;
	XoSetAttr(ar[8],XoAttrRefShDico,(void *)0,0) ;
	XoAddInAttr(or,XoObjRefListAttr,ar[8],0) ;

	XoSetAttr(ar[9],XoAttrRefName,"display",0) ;
	XoSetAttr(ar[9],XoAttrRefTypeName,"XoString",0) ;
	XoSetAttr(ar[9],XoAttrRefList,(void *)0,0) ;
	XoSetAttr(ar[9],XoAttrRefLgMax,(void *)-1,0) ;
	XoSetAttr(ar[9],XoAttrRefNoSave,(void *)0,0) ;
	XoSetAttr(ar[9],XoAttrRefSaveUnset,(void *)0,0) ;
	XoSetAttr(ar[9],XoAttrRefEmbAttr,(void *)1,0) ;
	XoSetAttr(ar[9],XoAttrRefWrapHidden,(void *)0,0) ;
	XoSetAttr(ar[9],XoAttrRefShDico,(void *)0,0) ;
	XoAddInAttr(or,XoObjRefListAttr,ar[9],0) ;

	XoSetAttr(ar[10],XoAttrRefName,"writable",0) ;
	XoSetAttr(ar[10],XoAttrRefTypeName,"XoString",0) ;
	XoSetAttr(ar[10],XoAttrRefList,(void *)0,0) ;
	XoSetAttr(ar[10],XoAttrRefLgMax,(void *)5,0) ;
	XoSetAttr(ar[10],XoAttrRefNoSave,(void *)0,0) ;
	XoSetAttr(ar[10],XoAttrRefSaveUnset,(void *)0,0) ;
	XoSetAttr(ar[10],XoAttrRefEmbAttr,(void *)1,0) ;
	XoSetAttr(ar[10],XoAttrRefWrapHidden,(void *)0,0) ;
	XoSetAttr(ar[10],XoAttrRefShDico,(void *)0,0) ;
	XoAddInAttr(or,XoObjRefListAttr,ar[10],0) ;

	XoSetAttr(ar[11],XoAttrRefName,"status",0) ;
	XoSetAttr(ar[11],XoAttrRefTypeName,"XoString",0) ;
	XoSetAttr(ar[11],XoAttrRefList,(void *)0,0) ;
	XoSetAttr(ar[11],XoAttrRefLgMax,(void *)-1,0) ;
	XoSetAttr(ar[11],XoAttrRefNoSave,(void *)0,0) ;
	XoSetAttr(ar[11],XoAttrRefSaveUnset,(void *)0,0) ;
	XoSetAttr(ar[11],XoAttrRefEmbAttr,(void *)1,0) ;
	XoSetAttr(ar[11],XoAttrRefWrapHidden,(void *)0,0) ;
	XoSetAttr(ar[11],XoAttrRefShDico,(void *)0,0) ;
	XoAddInAttr(or,XoObjRefListAttr,ar[11],0) ;

	XoSetAttr(ar[12],XoAttrRefName,"wrapper$",0) ;
	XoSetAttr(ar[12],XoAttrRefTypeName,"XoVoidObj",0) ;
	XoSetAttr(ar[12],XoAttrRefList,(void *)1,0) ;
	XoSetAttr(ar[12],XoAttrRefLgMax,(void *)-1,0) ;
	XoSetAttr(ar[12],XoAttrRefNoSave,(void *)0,0) ;
	XoSetAttr(ar[12],XoAttrRefSaveUnset,(void *)0,0) ;
	XoSetAttr(ar[12],XoAttrRefEmbAttr,(void *)0,0) ;
	XoSetAttr(ar[12],XoAttrRefWrapHidden,(void *)0,0) ;
	XoSetAttr(ar[12],XoAttrRefShDico,(void *)0,0) ;
	XoAddInAttr(or,XoObjRefListAttr,ar[12],0) ;

	XoSetAttr(ar[13],XoAttrRefName,"val",0) ;
	XoSetAttr(ar[13],XoAttrRefTypeName,"XoString",0) ;
	XoSetAttr(ar[13],XoAttrRefList,(void *)0,0) ;
	XoSetAttr(ar[13],XoAttrRefLgMax,(void *)-1,0) ;
	XoSetAttr(ar[13],XoAttrRefNoSave,(void *)0,0) ;
	XoSetAttr(ar[13],XoAttrRefSaveUnset,(void *)0,0) ;
	XoSetAttr(ar[13],XoAttrRefEmbAttr,(void *)1,0) ;
	XoSetAttr(ar[13],XoAttrRefWrapHidden,(void *)0,0) ;
	XoSetAttr(ar[13],XoAttrRefShDico,(void *)0,0) ;
	XoAddInAttr(or,XoObjRefListAttr,ar[13],0) ;

	XoSetAttr(ar[14],XoAttrRefName,"min",0) ;
	XoSetAttr(ar[14],XoAttrRefTypeName,"XoString",0) ;
	XoSetAttr(ar[14],XoAttrRefList,(void *)0,0) ;
	XoSetAttr(ar[14],XoAttrRefLgMax,(void *)-1,0) ;
	XoSetAttr(ar[14],XoAttrRefNoSave,(void *)0,0) ;
	XoSetAttr(ar[14],XoAttrRefSaveUnset,(void *)0,0) ;
	XoSetAttr(ar[14],XoAttrRefEmbAttr,(void *)1,0) ;
	XoSetAttr(ar[14],XoAttrRefWrapHidden,(void *)0,0) ;
	XoSetAttr(ar[14],XoAttrRefShDico,(void *)0,0) ;
	XoAddInAttr(or,XoObjRefListAttr,ar[14],0) ;

	XoSetAttr(ar[15],XoAttrRefName,"max",0) ;
	XoSetAttr(ar[15],XoAttrRefTypeName,"XoString",0) ;
	XoSetAttr(ar[15],XoAttrRefList,(void *)0,0) ;
	XoSetAttr(ar[15],XoAttrRefLgMax,(void *)-1,0) ;
	XoSetAttr(ar[15],XoAttrRefNoSave,(void *)0,0) ;
	XoSetAttr(ar[15],XoAttrRefSaveUnset,(void *)0,0) ;
	XoSetAttr(ar[15],XoAttrRefEmbAttr,(void *)1,0) ;
	XoSetAttr(ar[15],XoAttrRefWrapHidden,(void *)0,0) ;
	XoSetAttr(ar[15],XoAttrRefShDico,(void *)0,0) ;
	XoAddInAttr(or,XoObjRefListAttr,ar[15],0) ;

	XoSetAttr(or,XoObjRefNbAttr,(void *)16,0) ;
	ret = -100 ;
	if (XoAddObjRef(or)<0) goto end ;
	ret = 0 ;
	end :
	for (i=0;i<16;i++)
		if (ar[i]) XoFree(ar[i],0) ;
	if (or) XoFree(or,0) ;
	return	ret ;
}
static	int32	GenXo_o:date(void)
{
	int32	ret ;
	int32	i ;
	void	*or ;
	void	*ar[5] ;

	memset(ar,0,sizeof(ar));
	ret = -1 ;
	if (!(or=XoNew(XoObjRef))) goto end ;
	ret = -2 ;
	for (i=0;i<4;i++)
		if (!(ar[i]=XoNew(XoAttrRef))) goto end ;
	ret = -3 ;
	XoSetAttr(or,XoObjRefType,(void *)1,0) ;
	XoSetAttr(or,XoObjRefName,"o:date",0) ;
	XoSetAttr(or,XoObjRefRoot,(void *)0,0) ;

	XoSetAttr(ar[0],XoAttrRefName,"name$",0) ;
	XoSetAttr(ar[0],XoAttrRefTypeName,"XoString",0) ;
	XoSetAttr(ar[0],XoAttrRefList,(void *)0,0) ;
	XoSetAttr(ar[0],XoAttrRefLgMax,(void *)-1,0) ;
	XoSetAttr(ar[0],XoAttrRefNoSave,(void *)0,0) ;
	XoSetAttr(ar[0],XoAttrRefSaveUnset,(void *)0,0) ;
	XoSetAttr(ar[0],XoAttrRefEmbAttr,(void *)0,0) ;
	XoSetAttr(ar[0],XoAttrRefWrapHidden,(void *)0,0) ;
	XoSetAttr(ar[0],XoAttrRefShDico,(void *)0,0) ;
	XoAddInAttr(or,XoObjRefListAttr,ar[0],0) ;

	XoSetAttr(ar[1],XoAttrRefName,"xmlns",0) ;
	XoSetAttr(ar[1],XoAttrRefTypeName,"XoString",0) ;
	XoSetAttr(ar[1],XoAttrRefList,(void *)0,0) ;
	XoSetAttr(ar[1],XoAttrRefLgMax,(void *)-1,0) ;
	XoSetAttr(ar[1],XoAttrRefNoSave,(void *)0,0) ;
	XoSetAttr(ar[1],XoAttrRefSaveUnset,(void *)0,0) ;
	XoSetAttr(ar[1],XoAttrRefEmbAttr,(void *)1,0) ;
	XoSetAttr(ar[1],XoAttrRefWrapHidden,(void *)0,0) ;
	XoSetAttr(ar[1],XoAttrRefShDico,(void *)1,0) ;
	XoAddInAttr(or,XoObjRefListAttr,ar[1],0) ;

	XoSetAttr(ar[2],XoAttrRefName,"custom$",0) ;
	XoSetAttr(ar[2],XoAttrRefTypeName,"o:t_xml_obix_attr",0) ;
	XoSetAttr(ar[2],XoAttrRefList,(void *)1,0) ;
	XoSetAttr(ar[2],XoAttrRefLgMax,(void *)-1,0) ;
	XoSetAttr(ar[2],XoAttrRefNoSave,(void *)0,0) ;
	XoSetAttr(ar[2],XoAttrRefSaveUnset,(void *)0,0) ;
	XoSetAttr(ar[2],XoAttrRefEmbAttr,(void *)1,0) ;
	XoSetAttr(ar[2],XoAttrRefWrapHidden,(void *)0,0) ;
	XoSetAttr(ar[2],XoAttrRefShDico,(void *)0,0) ;
	XoAddInAttr(or,XoObjRefListAttr,ar[2],0) ;

	XoSetAttr(ar[3],XoAttrRefName,"name",0) ;
	XoSetAttr(ar[3],XoAttrRefTypeName,"XoString",0) ;
	XoSetAttr(ar[3],XoAttrRefList,(void *)0,0) ;
	XoSetAttr(ar[3],XoAttrRefLgMax,(void *)-1,0) ;
	XoSetAttr(ar[3],XoAttrRefNoSave,(void *)0,0) ;
	XoSetAttr(ar[3],XoAttrRefSaveUnset,(void *)0,0) ;
	XoSetAttr(ar[3],XoAttrRefEmbAttr,(void *)1,0) ;
	XoSetAttr(ar[3],XoAttrRefWrapHidden,(void *)0,0) ;
	XoSetAttr(ar[3],XoAttrRefShDico,(void *)0,0) ;
	XoAddInAttr(or,XoObjRefListAttr,ar[3],0) ;

	XoSetAttr(ar[4],XoAttrRefName,"href",0) ;
	XoSetAttr(ar[4],XoAttrRefTypeName,"XoString",0) ;
	XoSetAttr(ar[4],XoAttrRefList,(void *)0,0) ;
	XoSetAttr(ar[4],XoAttrRefLgMax,(void *)-1,0) ;
	XoSetAttr(ar[4],XoAttrRefNoSave,(void *)0,0) ;
	XoSetAttr(ar[4],XoAttrRefSaveUnset,(void *)0,0) ;
	XoSetAttr(ar[4],XoAttrRefEmbAttr,(void *)1,0) ;
	XoSetAttr(ar[4],XoAttrRefWrapHidden,(void *)0,0) ;
	XoSetAttr(ar[4],XoAttrRefShDico,(void *)0,0) ;
	XoAddInAttr(or,XoObjRefListAttr,ar[4],0) ;

	XoSetAttr(ar[5],XoAttrRefName,"is",0) ;
	XoSetAttr(ar[5],XoAttrRefTypeName,"XoString",0) ;
	XoSetAttr(ar[5],XoAttrRefList,(void *)0,0) ;
	XoSetAttr(ar[5],XoAttrRefLgMax,(void *)-1,0) ;
	XoSetAttr(ar[5],XoAttrRefNoSave,(void *)0,0) ;
	XoSetAttr(ar[5],XoAttrRefSaveUnset,(void *)0,0) ;
	XoSetAttr(ar[5],XoAttrRefEmbAttr,(void *)1,0) ;
	XoSetAttr(ar[5],XoAttrRefWrapHidden,(void *)0,0) ;
	XoSetAttr(ar[5],XoAttrRefShDico,(void *)0,0) ;
	XoAddInAttr(or,XoObjRefListAttr,ar[5],0) ;

	XoSetAttr(ar[6],XoAttrRefName,"null",0) ;
	XoSetAttr(ar[6],XoAttrRefTypeName,"XoString",0) ;
	XoSetAttr(ar[6],XoAttrRefList,(void *)0,0) ;
	XoSetAttr(ar[6],XoAttrRefLgMax,(void *)5,0) ;
	XoSetAttr(ar[6],XoAttrRefNoSave,(void *)0,0) ;
	XoSetAttr(ar[6],XoAttrRefSaveUnset,(void *)0,0) ;
	XoSetAttr(ar[6],XoAttrRefEmbAttr,(void *)1,0) ;
	XoSetAttr(ar[6],XoAttrRefWrapHidden,(void *)0,0) ;
	XoSetAttr(ar[6],XoAttrRefShDico,(void *)0,0) ;
	XoAddInAttr(or,XoObjRefListAttr,ar[6],0) ;

	XoSetAttr(ar[7],XoAttrRefName,"icon",0) ;
	XoSetAttr(ar[7],XoAttrRefTypeName,"XoString",0) ;
	XoSetAttr(ar[7],XoAttrRefList,(void *)0,0) ;
	XoSetAttr(ar[7],XoAttrRefLgMax,(void *)-1,0) ;
	XoSetAttr(ar[7],XoAttrRefNoSave,(void *)0,0) ;
	XoSetAttr(ar[7],XoAttrRefSaveUnset,(void *)0,0) ;
	XoSetAttr(ar[7],XoAttrRefEmbAttr,(void *)1,0) ;
	XoSetAttr(ar[7],XoAttrRefWrapHidden,(void *)0,0) ;
	XoSetAttr(ar[7],XoAttrRefShDico,(void *)0,0) ;
	XoAddInAttr(or,XoObjRefListAttr,ar[7],0) ;

	XoSetAttr(ar[8],XoAttrRefName,"displayName",0) ;
	XoSetAttr(ar[8],XoAttrRefTypeName,"XoString",0) ;
	XoSetAttr(ar[8],XoAttrRefList,(void *)0,0) ;
	XoSetAttr(ar[8],XoAttrRefLgMax,(void *)-1,0) ;
	XoSetAttr(ar[8],XoAttrRefNoSave,(void *)0,0) ;
	XoSetAttr(ar[8],XoAttrRefSaveUnset,(void *)0,0) ;
	XoSetAttr(ar[8],XoAttrRefEmbAttr,(void *)1,0) ;
	XoSetAttr(ar[8],XoAttrRefWrapHidden,(void *)0,0) ;
	XoSetAttr(ar[8],XoAttrRefShDico,(void *)0,0) ;
	XoAddInAttr(or,XoObjRefListAttr,ar[8],0) ;

	XoSetAttr(ar[9],XoAttrRefName,"display",0) ;
	XoSetAttr(ar[9],XoAttrRefTypeName,"XoString",0) ;
	XoSetAttr(ar[9],XoAttrRefList,(void *)0,0) ;
	XoSetAttr(ar[9],XoAttrRefLgMax,(void *)-1,0) ;
	XoSetAttr(ar[9],XoAttrRefNoSave,(void *)0,0) ;
	XoSetAttr(ar[9],XoAttrRefSaveUnset,(void *)0,0) ;
	XoSetAttr(ar[9],XoAttrRefEmbAttr,(void *)1,0) ;
	XoSetAttr(ar[9],XoAttrRefWrapHidden,(void *)0,0) ;
	XoSetAttr(ar[9],XoAttrRefShDico,(void *)0,0) ;
	XoAddInAttr(or,XoObjRefListAttr,ar[9],0) ;

	XoSetAttr(ar[10],XoAttrRefName,"writable",0) ;
	XoSetAttr(ar[10],XoAttrRefTypeName,"XoString",0) ;
	XoSetAttr(ar[10],XoAttrRefList,(void *)0,0) ;
	XoSetAttr(ar[10],XoAttrRefLgMax,(void *)5,0) ;
	XoSetAttr(ar[10],XoAttrRefNoSave,(void *)0,0) ;
	XoSetAttr(ar[10],XoAttrRefSaveUnset,(void *)0,0) ;
	XoSetAttr(ar[10],XoAttrRefEmbAttr,(void *)1,0) ;
	XoSetAttr(ar[10],XoAttrRefWrapHidden,(void *)0,0) ;
	XoSetAttr(ar[10],XoAttrRefShDico,(void *)0,0) ;
	XoAddInAttr(or,XoObjRefListAttr,ar[10],0) ;

	XoSetAttr(ar[11],XoAttrRefName,"status",0) ;
	XoSetAttr(ar[11],XoAttrRefTypeName,"XoString",0) ;
	XoSetAttr(ar[11],XoAttrRefList,(void *)0,0) ;
	XoSetAttr(ar[11],XoAttrRefLgMax,(void *)-1,0) ;
	XoSetAttr(ar[11],XoAttrRefNoSave,(void *)0,0) ;
	XoSetAttr(ar[11],XoAttrRefSaveUnset,(void *)0,0) ;
	XoSetAttr(ar[11],XoAttrRefEmbAttr,(void *)1,0) ;
	XoSetAttr(ar[11],XoAttrRefWrapHidden,(void *)0,0) ;
	XoSetAttr(ar[11],XoAttrRefShDico,(void *)0,0) ;
	XoAddInAttr(or,XoObjRefListAttr,ar[11],0) ;

	XoSetAttr(ar[12],XoAttrRefName,"wrapper$",0) ;
	XoSetAttr(ar[12],XoAttrRefTypeName,"XoVoidObj",0) ;
	XoSetAttr(ar[12],XoAttrRefList,(void *)1,0) ;
	XoSetAttr(ar[12],XoAttrRefLgMax,(void *)-1,0) ;
	XoSetAttr(ar[12],XoAttrRefNoSave,(void *)0,0) ;
	XoSetAttr(ar[12],XoAttrRefSaveUnset,(void *)0,0) ;
	XoSetAttr(ar[12],XoAttrRefEmbAttr,(void *)0,0) ;
	XoSetAttr(ar[12],XoAttrRefWrapHidden,(void *)0,0) ;
	XoSetAttr(ar[12],XoAttrRefShDico,(void *)0,0) ;
	XoAddInAttr(or,XoObjRefListAttr,ar[12],0) ;

	XoSetAttr(ar[13],XoAttrRefName,"val",0) ;
	XoSetAttr(ar[13],XoAttrRefTypeName,"XoString",0) ;
	XoSetAttr(ar[13],XoAttrRefList,(void *)0,0) ;
	XoSetAttr(ar[13],XoAttrRefLgMax,(void *)-1,0) ;
	XoSetAttr(ar[13],XoAttrRefNoSave,(void *)0,0) ;
	XoSetAttr(ar[13],XoAttrRefSaveUnset,(void *)0,0) ;
	XoSetAttr(ar[13],XoAttrRefEmbAttr,(void *)1,0) ;
	XoSetAttr(ar[13],XoAttrRefWrapHidden,(void *)0,0) ;
	XoSetAttr(ar[13],XoAttrRefShDico,(void *)0,0) ;
	XoAddInAttr(or,XoObjRefListAttr,ar[13],0) ;

	XoSetAttr(ar[14],XoAttrRefName,"min",0) ;
	XoSetAttr(ar[14],XoAttrRefTypeName,"XoString",0) ;
	XoSetAttr(ar[14],XoAttrRefList,(void *)0,0) ;
	XoSetAttr(ar[14],XoAttrRefLgMax,(void *)-1,0) ;
	XoSetAttr(ar[14],XoAttrRefNoSave,(void *)0,0) ;
	XoSetAttr(ar[14],XoAttrRefSaveUnset,(void *)0,0) ;
	XoSetAttr(ar[14],XoAttrRefEmbAttr,(void *)1,0) ;
	XoSetAttr(ar[14],XoAttrRefWrapHidden,(void *)0,0) ;
	XoSetAttr(ar[14],XoAttrRefShDico,(void *)0,0) ;
	XoAddInAttr(or,XoObjRefListAttr,ar[14],0) ;

	XoSetAttr(ar[15],XoAttrRefName,"max",0) ;
	XoSetAttr(ar[15],XoAttrRefTypeName,"XoString",0) ;
	XoSetAttr(ar[15],XoAttrRefList,(void *)0,0) ;
	XoSetAttr(ar[15],XoAttrRefLgMax,(void *)-1,0) ;
	XoSetAttr(ar[15],XoAttrRefNoSave,(void *)0,0) ;
	XoSetAttr(ar[15],XoAttrRefSaveUnset,(void *)0,0) ;
	XoSetAttr(ar[15],XoAttrRefEmbAttr,(void *)1,0) ;
	XoSetAttr(ar[15],XoAttrRefWrapHidden,(void *)0,0) ;
	XoSetAttr(ar[15],XoAttrRefShDico,(void *)0,0) ;
	XoAddInAttr(or,XoObjRefListAttr,ar[15],0) ;

	XoSetAttr(ar[16],XoAttrRefName,"tz",0) ;
	XoSetAttr(ar[16],XoAttrRefTypeName,"XoString",0) ;
	XoSetAttr(ar[16],XoAttrRefList,(void *)0,0) ;
	XoSetAttr(ar[16],XoAttrRefLgMax,(void *)-1,0) ;
	XoSetAttr(ar[16],XoAttrRefNoSave,(void *)0,0) ;
	XoSetAttr(ar[16],XoAttrRefSaveUnset,(void *)0,0) ;
	XoSetAttr(ar[16],XoAttrRefEmbAttr,(void *)1,0) ;
	XoSetAttr(ar[16],XoAttrRefWrapHidden,(void *)0,0) ;
	XoSetAttr(ar[16],XoAttrRefShDico,(void *)0,0) ;
	XoAddInAttr(or,XoObjRefListAttr,ar[16],0) ;

	XoSetAttr(or,XoObjRefNbAttr,(void *)17,0) ;
	ret = -100 ;
	if (XoAddObjRef(or)<0) goto end ;
	ret = 0 ;
	end :
	for (i=0;i<17;i++)
		if (ar[i]) XoFree(ar[i],0) ;
	if (or) XoFree(or,0) ;
	return	ret ;
}
static	int32	GenXo_o:time(void)
{
	int32	ret ;
	int32	i ;
	void	*or ;
	void	*ar[5] ;

	memset(ar,0,sizeof(ar));
	ret = -1 ;
	if (!(or=XoNew(XoObjRef))) goto end ;
	ret = -2 ;
	for (i=0;i<4;i++)
		if (!(ar[i]=XoNew(XoAttrRef))) goto end ;
	ret = -3 ;
	XoSetAttr(or,XoObjRefType,(void *)1,0) ;
	XoSetAttr(or,XoObjRefName,"o:time",0) ;
	XoSetAttr(or,XoObjRefRoot,(void *)0,0) ;

	XoSetAttr(ar[0],XoAttrRefName,"name$",0) ;
	XoSetAttr(ar[0],XoAttrRefTypeName,"XoString",0) ;
	XoSetAttr(ar[0],XoAttrRefList,(void *)0,0) ;
	XoSetAttr(ar[0],XoAttrRefLgMax,(void *)-1,0) ;
	XoSetAttr(ar[0],XoAttrRefNoSave,(void *)0,0) ;
	XoSetAttr(ar[0],XoAttrRefSaveUnset,(void *)0,0) ;
	XoSetAttr(ar[0],XoAttrRefEmbAttr,(void *)0,0) ;
	XoSetAttr(ar[0],XoAttrRefWrapHidden,(void *)0,0) ;
	XoSetAttr(ar[0],XoAttrRefShDico,(void *)0,0) ;
	XoAddInAttr(or,XoObjRefListAttr,ar[0],0) ;

	XoSetAttr(ar[1],XoAttrRefName,"xmlns",0) ;
	XoSetAttr(ar[1],XoAttrRefTypeName,"XoString",0) ;
	XoSetAttr(ar[1],XoAttrRefList,(void *)0,0) ;
	XoSetAttr(ar[1],XoAttrRefLgMax,(void *)-1,0) ;
	XoSetAttr(ar[1],XoAttrRefNoSave,(void *)0,0) ;
	XoSetAttr(ar[1],XoAttrRefSaveUnset,(void *)0,0) ;
	XoSetAttr(ar[1],XoAttrRefEmbAttr,(void *)1,0) ;
	XoSetAttr(ar[1],XoAttrRefWrapHidden,(void *)0,0) ;
	XoSetAttr(ar[1],XoAttrRefShDico,(void *)1,0) ;
	XoAddInAttr(or,XoObjRefListAttr,ar[1],0) ;

	XoSetAttr(ar[2],XoAttrRefName,"custom$",0) ;
	XoSetAttr(ar[2],XoAttrRefTypeName,"o:t_xml_obix_attr",0) ;
	XoSetAttr(ar[2],XoAttrRefList,(void *)1,0) ;
	XoSetAttr(ar[2],XoAttrRefLgMax,(void *)-1,0) ;
	XoSetAttr(ar[2],XoAttrRefNoSave,(void *)0,0) ;
	XoSetAttr(ar[2],XoAttrRefSaveUnset,(void *)0,0) ;
	XoSetAttr(ar[2],XoAttrRefEmbAttr,(void *)1,0) ;
	XoSetAttr(ar[2],XoAttrRefWrapHidden,(void *)0,0) ;
	XoSetAttr(ar[2],XoAttrRefShDico,(void *)0,0) ;
	XoAddInAttr(or,XoObjRefListAttr,ar[2],0) ;

	XoSetAttr(ar[3],XoAttrRefName,"name",0) ;
	XoSetAttr(ar[3],XoAttrRefTypeName,"XoString",0) ;
	XoSetAttr(ar[3],XoAttrRefList,(void *)0,0) ;
	XoSetAttr(ar[3],XoAttrRefLgMax,(void *)-1,0) ;
	XoSetAttr(ar[3],XoAttrRefNoSave,(void *)0,0) ;
	XoSetAttr(ar[3],XoAttrRefSaveUnset,(void *)0,0) ;
	XoSetAttr(ar[3],XoAttrRefEmbAttr,(void *)1,0) ;
	XoSetAttr(ar[3],XoAttrRefWrapHidden,(void *)0,0) ;
	XoSetAttr(ar[3],XoAttrRefShDico,(void *)0,0) ;
	XoAddInAttr(or,XoObjRefListAttr,ar[3],0) ;

	XoSetAttr(ar[4],XoAttrRefName,"href",0) ;
	XoSetAttr(ar[4],XoAttrRefTypeName,"XoString",0) ;
	XoSetAttr(ar[4],XoAttrRefList,(void *)0,0) ;
	XoSetAttr(ar[4],XoAttrRefLgMax,(void *)-1,0) ;
	XoSetAttr(ar[4],XoAttrRefNoSave,(void *)0,0) ;
	XoSetAttr(ar[4],XoAttrRefSaveUnset,(void *)0,0) ;
	XoSetAttr(ar[4],XoAttrRefEmbAttr,(void *)1,0) ;
	XoSetAttr(ar[4],XoAttrRefWrapHidden,(void *)0,0) ;
	XoSetAttr(ar[4],XoAttrRefShDico,(void *)0,0) ;
	XoAddInAttr(or,XoObjRefListAttr,ar[4],0) ;

	XoSetAttr(ar[5],XoAttrRefName,"is",0) ;
	XoSetAttr(ar[5],XoAttrRefTypeName,"XoString",0) ;
	XoSetAttr(ar[5],XoAttrRefList,(void *)0,0) ;
	XoSetAttr(ar[5],XoAttrRefLgMax,(void *)-1,0) ;
	XoSetAttr(ar[5],XoAttrRefNoSave,(void *)0,0) ;
	XoSetAttr(ar[5],XoAttrRefSaveUnset,(void *)0,0) ;
	XoSetAttr(ar[5],XoAttrRefEmbAttr,(void *)1,0) ;
	XoSetAttr(ar[5],XoAttrRefWrapHidden,(void *)0,0) ;
	XoSetAttr(ar[5],XoAttrRefShDico,(void *)0,0) ;
	XoAddInAttr(or,XoObjRefListAttr,ar[5],0) ;

	XoSetAttr(ar[6],XoAttrRefName,"null",0) ;
	XoSetAttr(ar[6],XoAttrRefTypeName,"XoString",0) ;
	XoSetAttr(ar[6],XoAttrRefList,(void *)0,0) ;
	XoSetAttr(ar[6],XoAttrRefLgMax,(void *)5,0) ;
	XoSetAttr(ar[6],XoAttrRefNoSave,(void *)0,0) ;
	XoSetAttr(ar[6],XoAttrRefSaveUnset,(void *)0,0) ;
	XoSetAttr(ar[6],XoAttrRefEmbAttr,(void *)1,0) ;
	XoSetAttr(ar[6],XoAttrRefWrapHidden,(void *)0,0) ;
	XoSetAttr(ar[6],XoAttrRefShDico,(void *)0,0) ;
	XoAddInAttr(or,XoObjRefListAttr,ar[6],0) ;

	XoSetAttr(ar[7],XoAttrRefName,"icon",0) ;
	XoSetAttr(ar[7],XoAttrRefTypeName,"XoString",0) ;
	XoSetAttr(ar[7],XoAttrRefList,(void *)0,0) ;
	XoSetAttr(ar[7],XoAttrRefLgMax,(void *)-1,0) ;
	XoSetAttr(ar[7],XoAttrRefNoSave,(void *)0,0) ;
	XoSetAttr(ar[7],XoAttrRefSaveUnset,(void *)0,0) ;
	XoSetAttr(ar[7],XoAttrRefEmbAttr,(void *)1,0) ;
	XoSetAttr(ar[7],XoAttrRefWrapHidden,(void *)0,0) ;
	XoSetAttr(ar[7],XoAttrRefShDico,(void *)0,0) ;
	XoAddInAttr(or,XoObjRefListAttr,ar[7],0) ;

	XoSetAttr(ar[8],XoAttrRefName,"displayName",0) ;
	XoSetAttr(ar[8],XoAttrRefTypeName,"XoString",0) ;
	XoSetAttr(ar[8],XoAttrRefList,(void *)0,0) ;
	XoSetAttr(ar[8],XoAttrRefLgMax,(void *)-1,0) ;
	XoSetAttr(ar[8],XoAttrRefNoSave,(void *)0,0) ;
	XoSetAttr(ar[8],XoAttrRefSaveUnset,(void *)0,0) ;
	XoSetAttr(ar[8],XoAttrRefEmbAttr,(void *)1,0) ;
	XoSetAttr(ar[8],XoAttrRefWrapHidden,(void *)0,0) ;
	XoSetAttr(ar[8],XoAttrRefShDico,(void *)0,0) ;
	XoAddInAttr(or,XoObjRefListAttr,ar[8],0) ;

	XoSetAttr(ar[9],XoAttrRefName,"display",0) ;
	XoSetAttr(ar[9],XoAttrRefTypeName,"XoString",0) ;
	XoSetAttr(ar[9],XoAttrRefList,(void *)0,0) ;
	XoSetAttr(ar[9],XoAttrRefLgMax,(void *)-1,0) ;
	XoSetAttr(ar[9],XoAttrRefNoSave,(void *)0,0) ;
	XoSetAttr(ar[9],XoAttrRefSaveUnset,(void *)0,0) ;
	XoSetAttr(ar[9],XoAttrRefEmbAttr,(void *)1,0) ;
	XoSetAttr(ar[9],XoAttrRefWrapHidden,(void *)0,0) ;
	XoSetAttr(ar[9],XoAttrRefShDico,(void *)0,0) ;
	XoAddInAttr(or,XoObjRefListAttr,ar[9],0) ;

	XoSetAttr(ar[10],XoAttrRefName,"writable",0) ;
	XoSetAttr(ar[10],XoAttrRefTypeName,"XoString",0) ;
	XoSetAttr(ar[10],XoAttrRefList,(void *)0,0) ;
	XoSetAttr(ar[10],XoAttrRefLgMax,(void *)5,0) ;
	XoSetAttr(ar[10],XoAttrRefNoSave,(void *)0,0) ;
	XoSetAttr(ar[10],XoAttrRefSaveUnset,(void *)0,0) ;
	XoSetAttr(ar[10],XoAttrRefEmbAttr,(void *)1,0) ;
	XoSetAttr(ar[10],XoAttrRefWrapHidden,(void *)0,0) ;
	XoSetAttr(ar[10],XoAttrRefShDico,(void *)0,0) ;
	XoAddInAttr(or,XoObjRefListAttr,ar[10],0) ;

	XoSetAttr(ar[11],XoAttrRefName,"status",0) ;
	XoSetAttr(ar[11],XoAttrRefTypeName,"XoString",0) ;
	XoSetAttr(ar[11],XoAttrRefList,(void *)0,0) ;
	XoSetAttr(ar[11],XoAttrRefLgMax,(void *)-1,0) ;
	XoSetAttr(ar[11],XoAttrRefNoSave,(void *)0,0) ;
	XoSetAttr(ar[11],XoAttrRefSaveUnset,(void *)0,0) ;
	XoSetAttr(ar[11],XoAttrRefEmbAttr,(void *)1,0) ;
	XoSetAttr(ar[11],XoAttrRefWrapHidden,(void *)0,0) ;
	XoSetAttr(ar[11],XoAttrRefShDico,(void *)0,0) ;
	XoAddInAttr(or,XoObjRefListAttr,ar[11],0) ;

	XoSetAttr(ar[12],XoAttrRefName,"wrapper$",0) ;
	XoSetAttr(ar[12],XoAttrRefTypeName,"XoVoidObj",0) ;
	XoSetAttr(ar[12],XoAttrRefList,(void *)1,0) ;
	XoSetAttr(ar[12],XoAttrRefLgMax,(void *)-1,0) ;
	XoSetAttr(ar[12],XoAttrRefNoSave,(void *)0,0) ;
	XoSetAttr(ar[12],XoAttrRefSaveUnset,(void *)0,0) ;
	XoSetAttr(ar[12],XoAttrRefEmbAttr,(void *)0,0) ;
	XoSetAttr(ar[12],XoAttrRefWrapHidden,(void *)0,0) ;
	XoSetAttr(ar[12],XoAttrRefShDico,(void *)0,0) ;
	XoAddInAttr(or,XoObjRefListAttr,ar[12],0) ;

	XoSetAttr(ar[13],XoAttrRefName,"val",0) ;
	XoSetAttr(ar[13],XoAttrRefTypeName,"XoString",0) ;
	XoSetAttr(ar[13],XoAttrRefList,(void *)0,0) ;
	XoSetAttr(ar[13],XoAttrRefLgMax,(void *)-1,0) ;
	XoSetAttr(ar[13],XoAttrRefNoSave,(void *)0,0) ;
	XoSetAttr(ar[13],XoAttrRefSaveUnset,(void *)0,0) ;
	XoSetAttr(ar[13],XoAttrRefEmbAttr,(void *)1,0) ;
	XoSetAttr(ar[13],XoAttrRefWrapHidden,(void *)0,0) ;
	XoSetAttr(ar[13],XoAttrRefShDico,(void *)0,0) ;
	XoAddInAttr(or,XoObjRefListAttr,ar[13],0) ;

	XoSetAttr(ar[14],XoAttrRefName,"min",0) ;
	XoSetAttr(ar[14],XoAttrRefTypeName,"XoString",0) ;
	XoSetAttr(ar[14],XoAttrRefList,(void *)0,0) ;
	XoSetAttr(ar[14],XoAttrRefLgMax,(void *)-1,0) ;
	XoSetAttr(ar[14],XoAttrRefNoSave,(void *)0,0) ;
	XoSetAttr(ar[14],XoAttrRefSaveUnset,(void *)0,0) ;
	XoSetAttr(ar[14],XoAttrRefEmbAttr,(void *)1,0) ;
	XoSetAttr(ar[14],XoAttrRefWrapHidden,(void *)0,0) ;
	XoSetAttr(ar[14],XoAttrRefShDico,(void *)0,0) ;
	XoAddInAttr(or,XoObjRefListAttr,ar[14],0) ;

	XoSetAttr(ar[15],XoAttrRefName,"max",0) ;
	XoSetAttr(ar[15],XoAttrRefTypeName,"XoString",0) ;
	XoSetAttr(ar[15],XoAttrRefList,(void *)0,0) ;
	XoSetAttr(ar[15],XoAttrRefLgMax,(void *)-1,0) ;
	XoSetAttr(ar[15],XoAttrRefNoSave,(void *)0,0) ;
	XoSetAttr(ar[15],XoAttrRefSaveUnset,(void *)0,0) ;
	XoSetAttr(ar[15],XoAttrRefEmbAttr,(void *)1,0) ;
	XoSetAttr(ar[15],XoAttrRefWrapHidden,(void *)0,0) ;
	XoSetAttr(ar[15],XoAttrRefShDico,(void *)0,0) ;
	XoAddInAttr(or,XoObjRefListAttr,ar[15],0) ;

	XoSetAttr(ar[16],XoAttrRefName,"tz",0) ;
	XoSetAttr(ar[16],XoAttrRefTypeName,"XoString",0) ;
	XoSetAttr(ar[16],XoAttrRefList,(void *)0,0) ;
	XoSetAttr(ar[16],XoAttrRefLgMax,(void *)-1,0) ;
	XoSetAttr(ar[16],XoAttrRefNoSave,(void *)0,0) ;
	XoSetAttr(ar[16],XoAttrRefSaveUnset,(void *)0,0) ;
	XoSetAttr(ar[16],XoAttrRefEmbAttr,(void *)1,0) ;
	XoSetAttr(ar[16],XoAttrRefWrapHidden,(void *)0,0) ;
	XoSetAttr(ar[16],XoAttrRefShDico,(void *)0,0) ;
	XoAddInAttr(or,XoObjRefListAttr,ar[16],0) ;

	XoSetAttr(or,XoObjRefNbAttr,(void *)17,0) ;
	ret = -100 ;
	if (XoAddObjRef(or)<0) goto end ;
	ret = 0 ;
	end :
	for (i=0;i<17;i++)
		if (ar[i]) XoFree(ar[i],0) ;
	if (or) XoFree(or,0) ;
	return	ret ;
}
int32	GenRefXo_xobix(void)
{
	if (GenXo_o:t_xml_obix_attr() < 0 ) return -1 ;
	if (GenXo_o:t_xml_obix_obj() < 0 ) return -2 ;
	if (GenXo_o:obj() < 0 ) return -3 ;
	if (GenXo_o:list() < 0 ) return -4 ;
	if (GenXo_o:op() < 0 ) return -5 ;
	if (GenXo_o:feed() < 0 ) return -6 ;
	if (GenXo_o:ref() < 0 ) return -7 ;
	if (GenXo_o:err() < 0 ) return -8 ;
	if (GenXo_o:bool() < 0 ) return -9 ;
	if (GenXo_o:int() < 0 ) return -10 ;
	if (GenXo_o:real() < 0 ) return -11 ;
	if (GenXo_o:str() < 0 ) return -12 ;
	if (GenXo_o:enum() < 0 ) return -13 ;
	if (GenXo_o:uri() < 0 ) return -14 ;
	if (GenXo_o:abstime() < 0 ) return -15 ;
	if (GenXo_o:reltime() < 0 ) return -16 ;
	if (GenXo_o:date() < 0 ) return -17 ;
	if (GenXo_o:time() < 0 ) return -18 ;
	return 0 ;
}
