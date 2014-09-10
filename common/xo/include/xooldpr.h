
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
 *	ne pas modifier les proto dans ce fichier
 *	ce fichier est inclus par les sources ecrits avec
 *	les versions de xo anterieures strictement a la version 2.1
 */


#if	defined(__STDC__) && !defined(NO_PROTOTYPE)
int	XoInit(int trace);
int	XoEnd();
int	XoFree(char *obj , int reccur);
int	XoType(char *obj);
int	XoSetAttr(char *obj,int	numattr,char *value,int lgval);
int	XoUnSetAttr(char *obj,int numattr);
int	XoAddInAttr(char *obj,int numattr,char *elem,int lgval);
int	XoInsInAttr(char *obj,int numattr,char *elem,int lgval);
int	XoSupInAttr(char *obj,int numattr);
int	XoNbInAttr(char *obj,int numattr);
int	XoFirstAttr(char *obj,int numattr);
int	XoLastAttr(char *obj,int numattr);
int	XoNextAttr(char *obj,int numattr);
int	XoPrevAttr(char *obj,int numattr);
int	XoSetCurr(char *obj,int  numattr,char *elem);
int	XoNbOccObj(int onum);
int	XoSetAllAttr(int numobj,int numattr,char *value,int lgval);
int	XoAddInAllAttr(int numobj,int numattr,char *value,int lgval);
int	XoUnSetAllAttr(int numobj,int numattr);
int	XoNumObj(char *name);
int	XoGetNumType(char *nameobj);
int	XoNbAttr(int onum);
int	XoNumAttr(int numobj,char *name);
int	XoNumAttrInObj(char *obj,char *name);
int	XoTypeAttr(int anum);
int	XoGetRefAttrLgMax(int numattr);
int	XoModeAttr(int numattr);
int	XoAddObjRef(char *obj);
int	XoDelObjRef(int onum);
int	XoRenameRefObj(char *oldname,char *newname);
int	XoSaveHeader(char *description , int fout);
int	XoSave(char *rootobj,int fout);
int	XoSaveMem(char *rootobj,char *buf);
int	XoLgSave(char *rootobj);
int	XoSaveRef(int fout);
int	XoSetUsr(char *obj,char *data,int (*destructeur)());
char	*XoNew(int numobj);
char	*XoNewCopy(char	*obj,int reccur);
char	*XoGetAttr(char	*obj,int numattr,int *lgval);
char	*XoGetCurr(char *obj,int  numattr);
char	*XoFirstObj(int onum);
char	*XoNextObj(int onum);
char	*XoGetRefObjName(int numobj);
char	*XoGetRefObjNameSilent(int numobj);
char	*XoGetRefAttrName(int numattr);
char	*XoGetRefAttrDefault(int numattr);
char	*XoGetRefAttrValue(int numattr);
char	*XoGetRefAttrHelpLine(int numattr);
char	*XoGetRefAttrHelpFile(int numattr);
char	*XoNewObjRef(int numobj);
char	*XoLoad(int fin);
char	*XoLoadRef(int fin);
char	*XoLoadMem(char *buf,int size);
char	*XoLoadAscii(FILE *fin);
char	*XoGetUsr(char *obj);
int	XoGetIndexInAttr(char *obj,int numAttr,char *elem);
int	XoSetIndexInAttr(char *obj,int numAttr,int idx);
int	XoDropList(char *ojb,int numAttr,int withfree,int reccur);
int	XoExtSave(char *file,char *obj,char *info);
char	*XoExtLoad(char *file);
int	XoExtLoadRef(char *file);
char	*XoNewNest(int numClass);
int	XoTryCopy(char *objdst,char *objsrc,int reccur);
int	XoTryCopyFilter(char *d,char *s,int rec,int (*f)(),void *p1,void *p2);
int	XoMove(char *objsrc,char *objdst);
int	XoClear(char *obj);
int	XoDump(char *obj,FILE *fout,int	reccur);
int	XoDumpWithoutAdr(char *obj, FILE *fout, int reccur);
int	XoDumpNbOcc(FILE *fout,int not0,int notpredef);
int	XoDumpListObj(FILE *fout,int onum,int reccur);
int	XoChangeRef(char *obj,int numobj,int reccur);
int	XoChangeRef2(char *obj,int numobj);
char	*XoNmType(char *obj);
int	XoNmDelObjRef(char *nameClass);
int	XoNmChangeRef(char *obj,char *nameClass,int reccur);
int	XoNmChangeRef2(char *obj,char *nameClass);
char	*XoNmNew(char *nameClass);
char	*XoNmNewNest(char *nameClass);

int	XoNmExistField(char *obj,char *path,...);
int	XoNmGetIndexInAttr(char *obj,char *path,char *elem,...);
int	XoNmSetIndexInAttr(char *obj,char *path,int idx,...);
int	XoNmSupInAttr(char *obj,char *nomChp,...);
int	XoNmAddInAttr(char *obj,char *nomChp,void *val,int lg,...);
int	XoNmInsInAttr(char *obj,char *nomChp,void *val,int lg,...);
int	XoNmSetAttr(char *obj,char *nomChp,void *val,int lg,...);
char	*XoNmGetAttr(char *obj,char *nomChp,int *lg,...);
int	XoNmNbInAttr(char *obj,char *nomChp,...);
int	XoNmFirstAttr(char *obj,char *nomChp,...);
int	XoNmNextAttr(char *obj,char *nomChp,...);
int	XoNmPrevAttr(char *obj,char *nomChp,...);
int	XoNmLastAttr(char *obj,char *nomChp,...);
int	XoNmDropList(char *ojb,char *nomChp,int withfree,int reccur,...);
char	*XoNmAttrToAscii(char *obj,char *nom,char *str,...);
char	*XoNmAttrToPtrAscii(char *obj,char *nom,char *str,...);
int	XoNmAsciiToAttr(char *obj,char *nom,char *val,...);

int	XoSprintf(char *buf,char *obj,char *fmt,...) ;
int	XoSprintf2(char *buf,char *obj,...) ;
int	XoExtSprintf(char *buf,char *obj,char *fmt,int nbarg,...) ;
int	XoMemCompress(unsigned char *in,int lgin,unsigned char *out) ;
int	XoMemUnCompress(unsigned char *in,int lgin,unsigned char *out) ;

#else
char	*XoNew() ;
char	*XoNewCopy() ;
char	*XoGetAttr() ;
char	*XoGetCurr() ;
char	*XoFirstObj() ;
char	*XoNextObj() ;
char	*XoGetRefObjName() ;
char	*XoGetRefAttrName() ;
char	*XoGetRefAttrDefault() ;
char	*XoGetRefAttrValue() ;
char	*XoGetRefAttrHelpLine() ;
char	*XoGetRefAttrHelpFile() ;
char	*XoNewObjRef() ;
char	*XoLoad() ;
char	*XoLoadRef() ;
char	*XoGetUsr() ;
#endif
