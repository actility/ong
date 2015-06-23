
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

#ifndef	XO_PROTO_H
#define	XO_PROTO_H

#ifdef __cplusplus
extern "C" {
#endif

/*	api.c	*/
char *XoVersion();
void xogetmalloc (int32 *nb , int32 *lg );
int32 xoatoi (char *s );
void xoerr (char *file , int32 line , char *mes , int32 par );
int32 XoSetUsr (void *obj , void *data , int32 (*destruc )());
void *XoGetUsr (void *obj );
void *XoNew (int32 nobj );
int32 XoReUse (void *obj , int32 numobj );
int32 XoFree (void *obj , int32 reccur );
int32 XoClear (void *obj );
int32 XoMove (void *objsrc , void *objdst );
int32 XoMove (void *objsrc , void *objdst );
int32 XoSetAttr (void *obj , int32 nrefattr , void *value , int32 lg );
int32 XoSetAllAttr (int32 numobj , int32 nrefattr , void *value , int32 lg );
int32 XoUnSetAttr (void *obj , int32 nrefattr );
int32 XoIsSetAttr (void *obj , int32 nrefattr );
int32 XoUnSetAllAttr (int32 numobj , int32 nrefattr );
void *XoGetAttr (void *obj , int32 nrefattr , int32 *lg );
int32 XoAttrValueIsDef (void *obj , int32 nrefattr );
int32 XoList (void *obj , int32 nrefattr , int32 action , void **adr );
void *XoGetCurr (void *obj , int32 nrefattr );
int32 XoSetCurr (void *obj , int32 nrefattr , void *adr );
int32 XoNbInAttr (void *obj , int32 nrefattr );
int32 XoFirstAttr (void *obj , int32 nrefattr );
int32 XoLastAttr (void *obj , int32 nrefattr );
int32 XoNextAttr (void *obj , int32 nrefattr );
int32 XoPrevAttr (void *obj , int32 nrefattr );
int32 XoGetIndexInAttr (void *obj , int32 nrefattr , void *elem );
int32 XoSetIndexInAttr (void *obj , int32 nrefattr , int32 idx );
int32 XoAddInAttr (void *obj , int32 nrefattr , void *add , int32 lg );
int32 XoInsInAttr (void *obj , int32 nrefattr , void *add , int32 lg );
int32 XoSupInAttr (void *obj , int32 nrefattr );
int32 XoAddInAllAttr (int32 numobj , int32 nrefattr , char *value , int32 lg );
void *XoNewCopy (void *obj , int32 reccur );
void *XoNewStrictCopy (void *obj , int32 reccur );
int32 XoGetFather (void *obj , void **father );
int32 XoGetRootFather (void *obj , void **rootfather );
int32 XoIsModified(void *obj,int32 reccur);
int32 XoSetModified(void *obj,int32 reccur);
int32 XoUnSetModified(void *obj,int32 reccur);
char *XoAttrToPtrAscii(void *obj, int32 numAttr, char *str);
int32 XoAsciiToAttr(void *obj, int32 numAttr, char *str);
int32 XoAsciiToAttr2(void *obj, int32 numAttr, char *str);
void XoSetErrorHandler(void (*errFunc)(char *file,int32 line ,char *mes,int32 par));
int32 XoIsObix(void *obj);


/* apiascii.c */
void XoStartEndFluxAscii(char *pstart,char *pend) ;
int32 XoSaveAscii (void *obj , FILE *fout );
void *XoLoadAscii (FILE *fin );
void *XoLoadAsciiMem (char *buf,int32 lg);
void *XoLoadAsciiUnComplete(FILE *fin,int32 *uncomplete);
void XoLoadAsciiSetGetc(int (*fct)(void *par));
void XoLoadAsciiSetUnGetc(int(*fct)(int c,void *par));

/* apihtml.c */
int32 XoSaveHtml (void *obj , FILE *fout );


/* apichang.c */
int32 XoFctConvType (int32 (*fct )());
int32 XoTryCopy (void *objdst , void *objsrc , int32 reccur );
int32 XoTryCopyFilter (void *objdst , void *objsrc , int32 reccur , int32 (*fctfilter )(), void *par1 , void *par2 );
int32 xotrycopy (void *objdst , void *objsrc , int32 reccur );
int32 XoChangeRef2 (void *objsrc , int32 numclasse );


/* apidump.c */
int32 XoDump (void *obj , FILE *fout , int32 reccur );
int32 XoDumpWithoutAdr(void *obj, FILE *fout, int32 reccur);
int32 XoDumpNbOcc (FILE *fout , int32 not0 , int32 notpredef );
int32 XoDumpNbOccEx(char *pFileName, int32 not0, int32 notpredef);
int32 XoDumpListObj (FILE *fout , int32 onum , int32 reccur );
int XoAsc2Hex( unsigned char *src, unsigned char *dest);


/* apiexten.c */
char *XoNmType (void *obj );
void *XoNewNest (int32 numClass );
void *XoNmNewNest (char *nameclass );
void *XoNmNew (char *nameclass );

int32 XoNmExistField (void *obj , char *nom , ...);
int32 XoNmGetIndexInAttr (void *obj , char *nomChp , void *elem , ...);
int32 XoNmSetIndexInAttr (void *obj , char *nomChp , int32 idx , ...);
int32 XoNmSupInAttr (void *obj , char *nomChp , ...);
int32 XoNmAddInAttr (void *obj , char *nomChp , void *val , int32 lg , ...);
int32 XoNmInsInAttr (void *obj , char *nomChp , void *val , int32 lg , ...);
int32 XoNmSetAttr (void *obj , char *nomChp , void *val , int32 lg , ...);
void *XoNmGetAttr (void *obj , char *nomChp , int32 *lg , ...);
int32 XoNmUnSetAttr(void *obj, char *nomChp,...);
int32 XoNmIsSetAttr(void *obj, char *nomChp,...);
int32 XoNmAttrValueIsDef(void *obj, char *nomChp,...);
int32 XoNmNbInAttr (void *obj , char *nomChp , ...);
int32 XoNmFirstAttr (void *obj , char *nomChp , ...);
int32 XoNmNextAttr (void *obj , char *nomChp , ...);
int32 XoNmPrevAttr (void *obj , char *nomChp , ...);
int32 XoNmLastAttr (void *obj , char *nomChp , ...);
int32 XoDropList (void *obj , int32 numAttr , int32 withfree , int32 reccur );
int32 XoNmDropList (void *obj , char *nomChp , int32 withfree , int32 reccur , ...);
char *XoNmAttrToAscii (void *obj , char *nom , char *bigbuf , ...);
char *XoNmAttrToPtrAscii (void *obj , char *nom , char *smallbuf , ...);
int32 XoNmAsciiToAttr (void *obj , char *nom , char *val , ...);
int32 XoNmAsciiToAttr2 (void *obj , char *nom , char *val , ...);

int32 XoExtSave (char *file , void *obj , char *info );
int32 XoExtSaveFlg(char *file, void *obj, char *info,int32 flags);
void *XoExtLoad (char *file );
int32 XoExtLoadRef (char *file );
int32 XoExtLoadRefDir(char *pathdir);
int32 XoNmDelObjRef (char *nameclass );
int32 XoChangeRef (void *obj , int32 classe , int32 reccur );
int32 XoNmChangeRef (void *obj , char *nameclass , int32 reccur );
int32 XoNmChangeRef2 (void *obj , char *nameclass );
int32 XoSprintf2 (char *buf , char *obj , ...);
int32 XoSprintf (char *buf , char *obj , char *fmt , ...);
int32 XoExtSprintf (char *buf , char *obj , char *fmt , int32 nbarg , ...);
int32 XoSaveAsciiEx( void * pObj, char * pFileName );
void *XoLoadAsciiEx(char *fileName);


/* apiinfo.c */
int32 XoAcceptDuplObjRef (void );
int32 XoNoAcceptDuplObjRef (void );
int32 XoDeamonNew (int32 (*f )());
int32 XoDeamonFree (int32 (*f )());
int32 XoEnd (void );
char *XoGetRefObjName (int32 obj );
char *XoGetRefObjNameSilent (int32 obj );
char *XoGetRefObjNameFrom(int32 obj);
char *XoGetRefObjNameAlias(int32 obj);
char *XoGetRefObjNameHasAlias(int32 obj);
int32 XoGetRefObjStExt(int32 obj);
int32 XoGetRefObjWrapHidden(int32 obj);
int32 XoGetRefObjWrapped(int32 obj);
int32 XoGetRefObjCustomAttributs(int32 obj);
int32 XoDumpRefObjFromList(int32 obj,FILE *fout);
int32 XoGetRefObjFromList(int32 obj,char *tab[],int32 lev /* =0 */);

int32 XoGetNumType (char *name );
int32 XoRenameRefObj (char *oldname , char *newname );
int32 XoNbAttr (int32 obj );
int32 XoNbOccObj (int32 obj );
void *XoFirstObj (int32 obj );
void *XoNextObj (int32 obj );
t_objet_ref *XoGetRefObj (int32 obj );
char *XoGetRefAttrName (int32 num );
int32 XoGetRefAttrLgMax (int32 num );
void *XoGetRefAttrDefault (int32 num );
void *XoGetRefAttrValue (int32 num );
void *XoGetRefAttrHelpLine (int32 num );
void *XoGetRefAttrHelpFile (int32 num );
t_attribut_ref *XoGetRefAttr (int32 num );
int32 XoTypeAttr (int32 num );
int32 XoNumObj (char *name );
int32 XoNumObjSilent (char *name );
int32 XoNumAttr (int32 numobj , char *name );
int32 XoNumAttrInObj (void *obj , char *name );
int32 XoModeAttr (int32 num );
int32 do_modulo4 (uint32 lg );
int32 XoInit (int32 trace );
int32 XoType (void *obj );
int32 XoAddObjRef (void *obj );
int32 XoDelObjRef (int32 numobj );
void *XoNewObjRef (int32 numobj );
int32 XoSaveRef (int32 fout );


/* apiload.c */
void *XoLoadMemExtDict(char *buf, int32 size, char *dico, int32 szDico);
void *XoLoadMem (char *buf , int32 size );
void *XoLoad (int32 file );
void *XoLoadRef (int32 fin );


/* apisave.c */
int32 XoSaveHeader (char *what , int32 fout );

// flags == no
int32 XoSave (void *obj , int32 fout );
int32 XoSaveMem (void *obj , char *fout );
int32 XoSaveMemAlloc(void *obj, char **fout, int32 *lgout);
int32 XoLgSave (void *obj );

// flags == user
int32 XoSaveFlg (void *obj , int32 fout, int32 flags );
int32 XoSaveMemFlg (void *obj , char *fout , int32 flags);
int32 XoSaveMemAllocFlg(void *obj, char **fout, int32 *lgout, int32 flags);
int32 XoLgSaveFlg (void *obj, int32 flags );

int32 xomodesv (t_attribut_ref *ar );
int32 XoOffsetDict(char *fout,int32 lg);
int32 XoLgDict(char *fout,int32 lg);


/* apicompr.c */
int32	XoMemCanDeflate(char *in,int32 lgin);
int32	XoMemDeflate(char *in,int32 lgin,char *out);
int32	XoMemInflate(char *in,int32 lgin,char *out);

/* apicompr.c deprecated */
int32	XoMemCompress(unsigned char *in,int32 lgin,unsigned char *out) ;
int32	XoMemUnCompress(unsigned char *in,int32 lgin,unsigned char *out) ;

/* apiwalk.c */
int32	XoWalkObjInObj ( void *obj,
	int32 (*fct)(void *o,int32 type,void *r1,void *r2,void *r3),
	void *ref1,void *ref2,void *ref3) ;
int32	XoObjDiffDef (void *obj);
int32	XoObjInObjDiffDef (void *obj,void **retObj);
int32	XoRecoverFrom (void *src,void *dst);
int32	XoUndoRecoverFrom (void *src,void *dst);

/* apixmlns.c */
char	*XoDropPrefixe(char *qname);
int32	XoAddNameSpace(char *pname,char *uri);
int32	XoLoadNameSpace(char *file);
int32	XoLoadNameSpaceDir(char *path);
void	XoDumpNameSpace(FILE *fout);
int32	XoSetNameSpace(void *obj,char *apref /*NULL for all*/);
int32	XoUnSetNameSpace(void *obj,char *apref /*NULL for all*/);

/* apixmlread.c */
void *XoReadXmlReader(/*xmlTextReaderPtr*/ void *preader,char *roottype,int flags,int *err);
void *XoReadXmlEx(char *filename,char *roottype,int flags,int *err);
void *XoReadXmlMem(char *buf,int sz,char *roottype,int flags,int *err);


/* apixmlwrit.c */
void *XoWritXmlMem(void *obj,int32 flags,char **bufout,char *defns);
void XoWritXmlFreeMem(void *buf);
void XoWritXmlFreeCtxt(void *pbuf);
int32 XoWritXmlEx(char *filename,void *obj,int32 flags,char *defns);

/* apixmlobix.c */
void *XoReadObixEx(char *filename,void *unused,int flags,int *err);
void *XoReadObixMem(char *buf,int sz,void *unused,int flags,int *err);

/* apiexiread.c */
void *XoReadExiMem(char *buf,int sz,char *roottype,int flags,int *err);
void *XoReadExiEx(char *filename,char *roottype,int flags,int *err);


/* apiexiwrit.c */
void	*XoWritExiMem(void *obj,int32 flags,char **bufout,int *len,char *schemaID, int useRootObj);
void	XoWritExiFreeMem(void *buf);
void	XoWritExiFreeCtxt(void *pbuf);
int32	XoWritExiEx(char *filename,void *obj,int32 flags,char *schemaID, int useRootObj);

/* apiexischema.c */
void XoLoadEXISchemaFiles(char** fileNames, unsigned int schemaFilesCount);
void XoLoadEXISchemas(char* str);
void XoLoadEXISchemaDir(char *path);


/* apixmlsave.c DO NOT USE tests only */
int32 XoSaveXml(void *obj, FILE *fout,int flags);
void XoStartEndFluxXml(char *pstart,char *pend);

/* apijsowrit.c */
int	XoWritJsoMem(void *obj,int32 flags,char **bufout);
int32	XoWritJsoEx(char *filename,void *obj,int32 flags);

/* apisection.c */
void	XoInitCS(void);
void	XoEnterCS(void);
void	XoLeaveCS(void);

/* apishareddico.c */
int32	XoLoadSharedDico(char *file);
int32	XoLoadSharedDicoDir(char *path);
int32	XoFindInSharedDico(char *s);
int32	XoFindInSharedDicoPrefixed(char *s,int32 *offpref);
void	XoSharedDicoFree();

/* apidir.c DO NOT USE*/
void	*xo_openDir(char *path);
char	*xo_readDir(void *dir);
char	*xo_readAbsDir(void *dir);
void	xo_closeDir(void *dir);
char	*xo_suffixname(char *fullName);

#ifdef __cplusplus
};
#endif

#endif
