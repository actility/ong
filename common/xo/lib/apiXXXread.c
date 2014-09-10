
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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <libxml/xmlreader.h>
#include "xoapipr.h"

#define	XO_XML_READER_XML
#include "xmlstr.h"


static	char	*xo_file = __FILE__;

/*
 * this file is used by apixmlread.c and apiexiread.c
 */

/*
 * from libxml2-2.7.8:xmlreader.c because #ifdef NOT_USED_YET
 */
static int
__xmlBase64Decode(const unsigned char *in, unsigned long *inlen,
                unsigned char *to, unsigned long *tolen)
{
    unsigned long incur;        /* current index in in[] */

    unsigned long inblk;        /* last block index in in[] */

    unsigned long outcur;       /* current index in out[] */

    unsigned long inmax;        /* size of in[] */

    unsigned long outmax;       /* size of out[] */

    unsigned char cur;          /* the current value read from in[] */

    unsigned char intmp[4], outtmp[4];  /* temporary buffers for the convert */

    int nbintmp;                /* number of byte in intmp[] */

    int is_ignore;              /* cur should be ignored */

    int is_end = 0;             /* the end of the base64 was found */

    int retval = 1;

    int i;

    if ((in == NULL) || (inlen == NULL) || (to == NULL) || (tolen == NULL))
        return (-1);

    incur = 0;
    inblk = 0;
    outcur = 0;
    inmax = *inlen;
    outmax = *tolen;
    nbintmp = 0;

    while (1) {
        if (incur >= inmax)
            break;
        cur = in[incur++];
        is_ignore = 0;
        if ((cur >= 'A') && (cur <= 'Z'))
            cur = cur - 'A';
        else if ((cur >= 'a') && (cur <= 'z'))
            cur = cur - 'a' + 26;
        else if ((cur >= '0') && (cur <= '9'))
            cur = cur - '0' + 52;
        else if (cur == '+')
            cur = 62;
        else if (cur == '/')
            cur = 63;
        else if (cur == '.')
            cur = 0;
        else if (cur == '=')    /*no op , end of the base64 stream */
            is_end = 1;
        else {
            is_ignore = 1;
            if (nbintmp == 0)
                inblk = incur;
        }

        if (!is_ignore) {
            int nbouttmp = 3;

            int is_break = 0;

            if (is_end) {
                if (nbintmp == 0)
                    break;
                if ((nbintmp == 1) || (nbintmp == 2))
                    nbouttmp = 1;
                else
                    nbouttmp = 2;
                nbintmp = 3;
                is_break = 1;
            }
            intmp[nbintmp++] = cur;
            /*
             * if intmp is full, push the 4byte sequence as a 3 byte
             * sequence out
             */
            if (nbintmp == 4) {
                nbintmp = 0;
                outtmp[0] = (intmp[0] << 2) | ((intmp[1] & 0x30) >> 4);
                outtmp[1] =
                    ((intmp[1] & 0x0F) << 4) | ((intmp[2] & 0x3C) >> 2);
                outtmp[2] = ((intmp[2] & 0x03) << 6) | (intmp[3] & 0x3F);
                if (outcur + 3 >= outmax) {
                    retval = 2;
                    break;
                }

                for (i = 0; i < nbouttmp; i++)
                    to[outcur++] = outtmp[i];
                inblk = incur;
            }

            if (is_break) {
                retval = 0;
                break;
            }
        }
    }

    *tolen = outcur;
    *inlen = inblk;
    return (retval);
}

static	int32	CmpPrefixe(char *nameobj,char *nameattr)
{
	char	*sep;
	int	lg;

	sep	= strchr(nameobj,':');
	if (!sep) 
		return	1;
	lg	= sep - nameobj;
	if (lg <= 0)
		return	1;

//fprintf(stderr,"compare prefixe '%s' '%s' lg=%d\n",nameobj,nameattr,lg+1);

	return	strncmp(nameobj,nameattr,lg+1);
}

// si avalue != NULL c'est un xmlns

static	char	*FindAttrInObj(t_ctxt *ctxt,void *curobj,char *nameobj,
		char *qname,char *avalue,int32 *numattr)
{
	char	*nameattr;
	int	retry = 0;
	int	common = 0;
	char	buffattr[XOML_TAG_LG*2]; // result of xodoname

	if (C_PARSE_NATIVENS(ctxt))
		nameattr	= qname;
	else
		nameattr= _XoDoName(&(ctxt->ns),(char *)qname,avalue,buffattr);
	if (C_PARSE_OBIX(ctxt) && !avalue && CmpPrefixe(nameobj,nameattr) == 0)
	{ // prefixe commun, on suppose que l'attribut ne reprend pas le prefixe
		nameattr= XoDropPrefixe((char *)qname);
		common	= 1;
	}
	retry	= 0;
retryattr:
if (C_PARSE_TRACE(ctxt))
XOML_TRACE("xo search attr='%s'/'%s' in obj '%s' retry=%d\n",
				nameattr,qname,nameobj,retry);

	*numattr	= XoNumAttrInObj(curobj,nameattr);
	if	(*numattr < 0)
	{

		if (C_PARSE_OBIX(ctxt) && !avalue && retry == 0 && common)
		{ // nouvelle recherche avec le prefixe commun
//			nameattr= _XoDoName(&(ctxt->ns),(char *)qname,NULL,buffattr);
			nameattr= &buffattr[0];
//fprintf(stderr,"retry case1 '%s'\n",nameattr);
			retry	= 1;
			goto	retryattr;
		}
		else
		{
			if	(C_PARSE_RETRYATTR(ctxt) && retry == 0)
			{
				nameattr	= qname;
//fprintf(stderr,"retry case2 '%s'\n",nameattr);
				retry	= 1;
				goto	retryattr;
			}
if (C_PARSE_TRACE(ctxt))
XOML_TRACE("xo NOT found attr='%s'/'%s' in obj '%s' retry=%d\n",
				nameattr,qname,nameobj,retry);
		}
	}
	else
	{
if (C_PARSE_TRACE(ctxt))
XOML_TRACE("xo found attr='%s'/'%s' in obj '%s' retry=%d num=%d\n",
				nameattr,qname,nameobj,retry,*numattr);
	}

	return	nameattr;
}

static	char	*FindAutoWrapper(t_ctxt *ctxt,void *curobj,char *nameobj,
		char *qname,int32 *numattr)
{
	char	*nametype;
	char	bufftype[XOML_TAG_LG*2]; // result of xodoname
	int32	type ;
	int32	mode ;
	int32	nbattr ;
	int32	a ;
	t_attribut_ref	*ar ;

	if (C_PARSE_NATIVENS(ctxt))
		nametype	= qname;
	else
		nametype= _XoDoName(&(ctxt->ns),(char *)qname,NULL,bufftype);

if (C_PARSE_TRACE(ctxt))
XOML_TRACE("auto wrapp search an attribut list of type='%s' in obj='%s'\n",
			nametype,nameobj);

	*numattr	= -1;
	type	= XoType(curobj) ;
	if	( type < 0 )
		return	NULL;
	nbattr	= XoNbAttr(type) ;
	if	( nbattr < 0 )
		return	NULL;

	for	( a = 0 ; a < nbattr ; a++ )
	{
		*numattr	= type + a + 1 ;
//		mode	= XoModeAttr(*numattr) ;
		ar	= XoGetRefAttr(*numattr) ;
		mode	= ar->ar_mode;

		if	(mode != LIST_OBJ_ATTR)
			continue;
		if	(!ar->ar_type_name)
			continue;
		if	(strcmp(ar->ar_type_name,nametype) != 0)
			continue;
if (C_PARSE_TRACE(ctxt))
XOML_TRACE("auto wrapp found attribut list '%s' of type='%s' in obj='%s'\n",
			ar->ar_name,nametype,nameobj);


		return	ar->ar_name;
	}

#if	0	// ca pose pb pour retrouver le type de l'obj a allouer
	for	( a = 0 ; a < nbattr ; a++ )
	{
		*numattr	= type + a + 1 ;
		mode	= XoModeAttr(*numattr) ;
		ar	= XoGetRefAttr(*numattr) ;

		if	(mode != LIST_OBJ_ATTR)
			continue;
		if	(!ar->ar_type_name)
			continue;
		if	(ar->ar_type != XoVoidObj)
			continue;
if (C_PARSE_TRACE(ctxt))
XOML_TRACE("auto wrapp found attribut list '%s' of type='%s' in obj='%s'\n",
			ar->ar_name,"XoVoidObj",nameobj);
		//
		// TODO a revoir on change une donnee commune
		ar->ar_default	= nametype;

		return	ar->ar_name;
	}
#endif

	*numattr	= -1;
	return	NULL;
}


static	char	*WaitTypeTxt(int type)
{
	char	*pt;
	switch	(type)
	{
	case	END_ROOT : pt = "END_ROOT" ; break;
	case	END_STEXT : pt = "END_STEXT" ; break;
	case	END_OBJECT : pt = "END_OBJECT" ; break;
	case	END_LIST : pt = "END_LIST" ; break;
	case	END_ATTR : pt = "END_ATTR" ; break;
	case	END_WRAPPER : pt = "END_WRAPPER" ; break;
	default : pt = "???" ; break;
	}

	return	pt;
}

void	_XoDumpStack(t_ctxt *ctxt)
{
	int	i;

printf("++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++\n");
	for	( i = ctxt->level-1 ; i >= 0 ; i--)
	{
		char	*typeobj;
		char	*typecnt;
#ifdef PARSE_DEBUG
		char	*balise	= ctxt->waitstr[i];
#else
		char	*balise	= "<nodbg>";
#endif
		typecnt	=
			(ctxt->cnt[i]?XoGetRefObjName(XoType(ctxt->cnt[i])):"");
		typeobj	=
			(ctxt->obj[i]?XoGetRefObjName(XoType(ctxt->obj[i])):"");

	printf	("[%02d] [%s] balise=[%s] cnt=%lx/%s ar=%lx/%s cur=%lx/%s\n",i,
		WaitTypeTxt(ctxt->waittype[i]),balise,
		(unsigned long)ctxt->cnt[i],typecnt,
		(unsigned long)ctxt->ar[i],
				ctxt->ar[i]?ctxt->ar[i]->ar_type_name:"",
		(unsigned long)ctxt->obj[i],typeobj);
	}
printf("++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++\n");
}


void	_XoAllocCtxt(t_ctxt *ctxt)
{
	_XoAllocNameSpace(&(ctxt->ns),XOML_MAX_NAMESPACE);
}
void	_XoFreeCtxt(t_ctxt *ctxt)
{
	_XoFreeNameSpace(&(ctxt->ns));
}

// ici le type de l'objet n'a pas encore ete determine
static	int	LoadAttributesAndNS(xmlTextReaderPtr reader,int attrCount,
	t_ctxt *ctxt)
{
	int		i;
	int		ret;

	for (i = 0; i < attrCount && i < XOML_MAX_ATTR ; i++) 
	{
		char	*aqname	= ctxt->atname[i];
		char	*avalue	= ctxt->atvalue[i];

//fprintf(stderr,"atname='%s' atvalue='%s'\n",aqname,avalue);
		if	(strncmp((char *)aqname,"xmlns",5) == 0)
		{
			ret = _XoAddNameSpace(NULL,0,&(ctxt->ns),(char *)aqname,
								(char *)avalue);
			if	(ret <= 0)
				return	-1;
		}
	}
	return	1;
}

void	_XoFreeAttributesWith(t_ctxt *ctxt,void (*fct)(void *))
{
	int		i;
	if	(!fct)
	{
		fct	= free;
	}

	for (i = 0; i < ctxt->atcount && i < XOML_MAX_ATTR ; i++) 
	{
		if (ctxt->atname[i]) 
		{
			(*fct)(ctxt->atname[i]);
			ctxt->atname[i]	= NULL;
		}
		if (ctxt->atvalue[i]) 
		{
			(*fct)(ctxt->atvalue[i]);
			ctxt->atvalue[i]	= NULL;
		}
	}
	ctxt->atcount	= 0;
}

void	_XoFreeAttributes(t_ctxt *ctxt)
{
	_XoFreeAttributesWith(ctxt,NULL);	// free(3C)
}


static	void	StoreAttributes(xmlTextReaderPtr reader,int attrCount,
	t_ctxt *ctxt,void *obj)
{
	int		i;
	int32		custattr = -1;
	char		*custtype = NULL;
	char		*nameobj;
	t_attribut_ref	*attr	= NULL;

	nameobj	= XoGetRefObjName(XoType(obj));

	if (C_PARSE_OBIX(ctxt) && XoGetRefObjCustomAttributs(XoType(obj)) > 0)
	{
		custattr	= XoNumAttrInObj(obj,XOML_CUSTOM_ATTR);
		if (custattr > 0)
		{
			attr	= XoGetRefAttr(custattr);
			if (attr)
				custtype	= attr->ar_type_name;
		}
	}

	for (i = 0; i < ctxt->atcount && i < XOML_MAX_ATTR; i++) 
	{
		char	*nameattr;
		int32	numattr;
		char	*aqname;
		char	*avalue;

		aqname	= ctxt->atname[i];
		avalue	= ctxt->atvalue[i];

if (C_PARSE_TRACE(ctxt))
XOML_TRACE("xo store attr[%d] name='%s' value='%s' num=%d\n", 
						i,aqname,avalue,numattr);

		if	(strncmp(aqname,"xmlns",5) != 0)
		{
			nameattr= FindAttrInObj(ctxt,obj,nameobj,aqname,NULL,
								&numattr);
		}
		else
		{
			nameattr= FindAttrInObj(ctxt,obj,nameobj,aqname,avalue,
								&numattr);
		}
		if (numattr > 0)
		{
			XoSetAttr(obj,numattr,avalue,0);
			continue;
		}
		// l'attribut n'est pas defini => added to custom$
		if (custtype && *custtype)
		{
			void *sub;

			sub	= XoNmNew(custtype);
			if (sub)
			{
				XoNmSetAttr(sub,XOML_CUSTOM_ATTR_NAME,
								aqname,0);
				XoNmSetAttr(sub,XOML_CUSTOM_ATTR_VALUE,
								avalue,0);
				XoAddInAttr(obj,custattr,sub,0);
			}
		}
	}
}

// cherche name == "xxx:zzzz" dans les attributs

static	char	*FindLocalAttribute(xmlTextReaderPtr reader,int attrCount,
		t_ctxt *ctxt,char *name,char *dst)
{
	int	i;
	char	*found	= NULL;
	char	buffattr[XOML_TAG_LG*2]; // result of xodoname

	for (i = 0; !found && i < ctxt->atcount && i < XOML_MAX_ATTR; i++) 
	{
		char	*extname;
		char	*aqname;
		char	*avalue;

		aqname	= ctxt->atname[i];
		avalue	= ctxt->atvalue[i];

		if	(strncmp(aqname,"xmlns",5) == 0)
			continue;

		if (C_PARSE_NATIVENS(ctxt))
			extname	= aqname;
		else
		{
			extname	= _XoDoName(&(ctxt->ns),aqname,NULL,buffattr);
			if (!extname || !*extname)
				continue;
		}
/*
fprintf(stderr,"nm='%s' aq='%s' ex='%s' def='%s'\n",name,aqname,extname,ctxt->ns.nsdefault);
fflush(stderr);
*/


		if (strcmp(extname,name) != 0)
		{	// xxx:yyyy != zzz:name
			continue;
		}

		strcpy	(dst,avalue);
		found	= dst;
if (C_PARSE_TRACE(ctxt))
XOML_TRACE("--------------------- found attribut '%s'/'%s' value='%s'\n",
				name,aqname,found);
	}
if (!found && C_PARSE_TRACE(ctxt))
XOML_TRACE("--------------------- attribut not found '%s'\n",name);


	return	found;
}


// search xsi:type
static	char	*SearchForNewType(xmlTextReaderPtr reader,int attrCount,
		t_ctxt *ctxt,char *def,char *dst,int *found)
{
	char	*type;

	*found	= 0;
	*dst	= '\0';
	if (def)
		strcpy	(dst,def);
	type	= FindLocalAttribute(reader,attrCount,ctxt,TYVOID_ATTR,dst);
	if	(type)
		*found	= 1;
	return	dst;
}

void	_XoSetElementName(void *obj,char *balise)
{
	int32		numattr;

	numattr	= XoNumAttrInObj(obj,BALISE_ATTR);
	if	(numattr <= 0)
	{
		return;
	}
	XoSetAttr(obj,numattr,balise,0);
}

static	void	DoElementName(t_ctxt *ctxt,void *obj,char *nameobj,char *balise)
{
	char	buff[XOML_TAG_LG*2];
	char	*ret;

	if (C_PARSE_NATIVENS(ctxt))
	{
		_XoSetElementName(obj,balise);
		return;
	}

	ret	= _XoDoName(&(ctxt->ns),balise,NULL,buff);
	_XoSetElementName(obj,ret);

if (C_PARSE_TRACE(ctxt))
XOML_TRACE("balise='%s' => name$='%s'\n",balise,buff);
	return;

#if	0
	piv	= strchr(nameobj,':');
	if	(!piv)
	{
		_XoSetElementName(obj,balise);
		return;
	}
	strcpy	(nobj,nameobj);
	piv	= strchr(nobj,':');
	if	(!piv)	// ???
		return;
	*piv	= '\0';
	sprintf	(buff,"%s:%s",nobj,XoDropPrefixe(balise));
	_XoSetElementName(obj,buff);
#endif
#if	0
	fprintf	(stderr,"nameobj='%s' balise='%s' name='%s'\n",nameobj,balise,buff);
#endif

}


void *_XoRootPush(/*xmlTextReaderPtr*/void *reader,t_ctxt *ctxt,char *nametype,char *balise,int *err)
{
	void	*curObj;
	char	*roottype;

	// param reader can be NULL

	*err	= 1;
	ctxt->level	= 0;
	if	(ctxt->roottype)
		roottype	= ctxt->roottype;
	else
		roottype	= nametype;

	if	(0 && C_PARSE_OBIX(ctxt) && CmpPrefixe(roottype,"o:") != 0)
	{	// en parse OBIX on ne peut allouer que des objets OBIX
		*err	= -100;
		return	NULL;
	}

	curObj	= XoNmNew(roottype);
	if (!curObj)
	{
		*err	= -105;
		return	NULL;
	}
	if	(!C_PARSE_OBIX(ctxt) && XoIsObix(curObj) > 0)
	{
		ctxt->flags	= ctxt->flags | XOML_PARSE_OBIX;
if (C_PARSE_TRACE(ctxt))
		XOML_TRACE("--------------------- SWITCH to OBIX at root level\n");

	}
	if	(C_PARSE_OBIX(ctxt) && XoIsObix(curObj) <= 0)
	{	// en parse OBIX on ne peut allouer que des objets OBIX
		XoFree(curObj,1);
		*err	= -110;
		return	NULL;
	}

	ctxt->cnt[ctxt->level] 		= NULL;
	ctxt->obj[ctxt->level]		= curObj;
	ctxt->waittype[ctxt->level]	= END_ROOT;
#ifdef PARSE_DEBUG
	strcpy(ctxt->waitstr[ctxt->level],balise);
#endif

if (C_PARSE_TRACE(ctxt))
XOML_TRACE("--------------------- CREATE obj root '%s'/'%s'\n",roottype,balise);

	ctxt->level++;
if (C_PARSE_TRACE(ctxt))
XOML_TRACE("--------------------- PUSH root obj %d\n",ctxt->level);

	return	curObj;

}

// allocation d'1 nouvel objet (dans une liste ou non)
// determination du type si XoVoidObj en cherchant dans les attributs xml
// sauvegarde des attributs xml (embattr)
// sauvegarde du nom de l'objet 'name$'
// positionnement sur l'attribut 'value$' si simple type extension, sinon reset
// de l'attribut en cours
static	void	*AllocNewObject(xmlTextReaderPtr reader,int attrCount,t_ctxt *ctxt,t_attribut_ref **curAtr,char *aqname,int *ret)
{
	char	*newType;
	void	*newObj;
	char	tmp[512];
	int	found = 0;
	int	xsitype = 0;
	char	*nameobj;
	char	buffattr[XOML_TAG_LG*2]; // result of xodoname

	*ret	= 1;

	// par default le type d'objet est donne par la definition de l'attribut
	// mais pas tjrs vrai si le type de l'attribut est XoVoidObj
	newType	= (*curAtr)->ar_type_name;	

	// en XML le type de l'objet peut venir de xsi:type
	if	(!C_PARSE_OBIX(ctxt) && (*curAtr)->ar_type == XoVoidObj)
	{
		newType	= SearchForNewType(reader,attrCount,ctxt,
					(*curAtr)->ar_default,tmp,&found);
		if (found)
			xsitype	= 1;


		if (!found 
		&& (!(*curAtr)->ar_default || !strlen((*curAtr)->ar_default)))
		{ // xml attr "type" pas trouve et pas de type par default
		  // => on se sert de la balise ...
			newType	= aqname;
			found = 1;
		}
		else
		if (!newType || !*newType)
		{
			*ret	= -200;
			return	NULL;
		}

		if (!C_PARSE_NATIVENS(ctxt))
			newType	= _XoDoName(&(ctxt->ns),newType,NULL,buffattr);

if (C_PARSE_TRACE(ctxt))
XOML_TRACE("--------------------- XML DYNAMIC typing '%s'/'%s' found=%d xsi=%d\n",newType,aqname,found,xsitype);

		goto	tryalloc;

	}
	// en OBIX le type de l'objet vient forcement de la balise
	if	(C_PARSE_OBIX(ctxt) && (*curAtr)->ar_type == XoVoidObj)
	{
		newType	= aqname;
		if (!C_PARSE_NATIVENS(ctxt))
			newType	= _XoDoName(&(ctxt->ns),newType,NULL,buffattr);

if (C_PARSE_TRACE(ctxt))
XOML_TRACE("--------------------- OBIX DYNAMIC typing '%s'/'%s'\n",newType,aqname);

		goto	tryalloc;
	}


tryalloc :

	*curAtr	= NULL;
	if	(0 && C_PARSE_OBIX(ctxt) && CmpPrefixe(newType,"o:") != 0)
	{	// en parse OBIX on ne peut allouer que des objets OBIX
		*ret	= -205;
		return	NULL;
	}
	newObj	= XoNmNew(newType);
	if	(!newObj)
	{
		*ret	= -210;
		return	NULL;
	}
	if	(C_PARSE_OBIX(ctxt) && XoIsObix(newObj) <= 0)
	{	// en parse OBIX on ne peut allouer que des objets OBIX
		XoFree(newObj,1);
		*ret	= -215;
		return	NULL;
	}

	nameobj	= XoGetRefObjName(XoType(newObj));
	DoElementName(ctxt,newObj,nameobj,aqname);

	if	(xsitype)
	{
//fprintf(stderr,"set special xsi:type attribut '%s'\n",newType);
		XoNmSetAttr(newObj,TYVOID_ATTR,newType,0);
	}

	if	(XoGetRefObjStExt(XoType(newObj)) > 0)
	{
		*curAtr	= XoGetRefAttr(XoNumAttrInObj(newObj,PSEUDO_ATTR));
		if	(!*curAtr)
		{
			XoFree(newObj,1);
			*ret	= -220;
			return	NULL;
		}
if (C_PARSE_TRACE(ctxt))
XOML_TRACE("--------------------- NEW for obj STEXT '%s'\n",nameobj);
	}
	else
	{
if (C_PARSE_TRACE(ctxt))
XOML_TRACE("--------------------- NEW for obj '%s'\n",nameobj);
	}

	StoreAttributes(reader,attrCount,ctxt,newObj);

	return	newObj;
}

// sauvegarde du contenu >...< pour un attribut simple dans une liste ou non
int32	_XoSetBasicValue(void *curObj,t_attribut_ref *curAtr,char **value,int plg)
{
	int	lg	= plg;
	int32	valint32;
	void	*valvoid;

	switch	(curAtr->ar_type)
	{
	case	XoInt4B:
	case	XoBool :
	case	XoDate :
		valint32= xoatoi((char *)*value);
		valvoid	= (void *)valint32;
	break;
	case	XoString:
		valvoid	= (void *)*value;
#if 0	// TODO : keep memory allocated by xml (see lib/api.c set_value())
	// ca ne gagne pas grand chose 5%
		lg	= -9999;
#endif
	break;
	case	XoBuffer:
//		lg	= strlen((char *)*value)+1;
		valvoid	= (void *)*value;
	break;
	default:
		return	-1;
	break;
	}

	switch	(curAtr->ar_mode)
	{
	case	BASIC_ATTR :
		if (XoSetAttr(curObj,curAtr->ar_num,valvoid,lg) < 0)
			return	-2;
#if 0
		if (lg == -9999)	// dont xmlfree this value
			*value	= NULL;
#endif
		return	0;
	break;
	case	LIST_BASIC_ATTR :
		if (XoAddInAttr(curObj,curAtr->ar_num,valvoid,lg) < 0)
			return	-3;
#if 0
		if (lg == -9999)	// dont xmlfree this value
			*value	= NULL;
#endif
		return	0;
	break;
	}

	return	-4;
}

static	void	PushWrapper(t_ctxt *ctxt,void *curObj,t_attribut_ref *curAtr,int waittype)
{
	ctxt->ar[ctxt->level-1]		= curAtr;
	ctxt->ar[ctxt->level]		= curAtr;
	ctxt->cnt[ctxt->level]		= curObj;
	ctxt->obj[ctxt->level]		= NULL;
	ctxt->waittype[ctxt->level]	= waittype;
#ifdef PARSE_DEBUG
	strcpy(ctxt->waitstr[ctxt->level],WRAPPE_ATTR);
#endif
	ctxt->level++;
}

static	int	NeedPushWrapper(xmlTextReaderPtr reader,t_ctxt *ctxt,char *balise)
{
	void		*curObj	= NULL;
	t_attribut_ref	*curAtr	= NULL;
	int32		numattr;

	curObj	= ctxt->obj[ctxt->level-1];
	if (!curObj)
		return	-300;
	if	( C_PARSE_OBIX(ctxt) )
	{
		if	(XoIsObix(curObj) <= 0)
			return	-305;
		numattr	= XoNumAttrInObj(curObj,WRAPPE_ATTR);
		if	(numattr <= 0)
			return	-310;
		curAtr	= XoGetRefAttr(numattr);
		if	(!curAtr)
			return	-315;
if (C_PARSE_TRACE(ctxt))
XOML_TRACE("--------------------- PUSH OBIX WRAPPER for list/obj %d\n",ctxt->level);
		PushWrapper(ctxt,curObj,curAtr,END_WRAPPER);
		return	1;
	}

	numattr	= XoGetRefObjWrapHidden(XoType(curObj));
	if	(numattr > 0)
	{
		curAtr	= XoGetRefAttr(numattr);
		if	(!curAtr)
			return	-320;
if (C_PARSE_TRACE(ctxt))
XOML_TRACE("--------------------- PUSH HIDDEN WRAPPER for list/obj %d\n",ctxt->level);
		PushWrapper(ctxt,curObj,curAtr,END_WRAPPER);
		return	1;
	}

	return	0;
}

static	int	AutoPushWrapper(xmlTextReaderPtr reader,t_ctxt *ctxt,int32 numattr)
{
	void		*curObj	= NULL;
	t_attribut_ref	*curAtr	= NULL;

	curObj	= ctxt->obj[ctxt->level-1];
	if (!curObj)
		return	-400;
	curAtr	= XoGetRefAttr(numattr);
	if	(!curAtr)
		return	-415;

if (C_PARSE_TRACE(ctxt))
XOML_TRACE("--------------------- PUSH AUTO WRAPPER for list/obj %d\n",ctxt->level);
	PushWrapper(ctxt,curObj,curAtr,END_WRAPPER);
	return	1;
}

static	char	*TypeNodeTxt(int type)
{
	switch(type)
	{
		case	XML_READER_TYPE_NONE : 
				return	"none" ; break;
		case	XML_READER_TYPE_ELEMENT : 
				return	"begin_element" ; break;
		case	XML_READER_TYPE_ATTRIBUTE : 
				return	"attribut" ; break;
		case	XML_READER_TYPE_TEXT : 
				return	"text" ; break;
		case	XML_READER_TYPE_CDATA : 
				return	"cdata" ; break;
		case	XML_READER_TYPE_ENTITY_REFERENCE : 
				return	"entity_ref" ; break;
		case	XML_READER_TYPE_ENTITY : 
				return	"entity" ; break;
		case	XML_READER_TYPE_PROCESSING_INSTRUCTION : 
				return	"instruction" ; break;
		case	XML_READER_TYPE_COMMENT : 
				return	"comment" ; break;
		case	XML_READER_TYPE_DOCUMENT : 
				return	"document" ; break;
		case	XML_READER_TYPE_DOCUMENT_TYPE : 
				return	"document_type" ; break;
		case	XML_READER_TYPE_DOCUMENT_FRAGMENT : 
				return	"document_frag" ; break;
		case	XML_READER_TYPE_NOTATION : 
				return	"notation" ; break;
		case	XML_READER_TYPE_WHITESPACE : 
				return	"whitespace" ; break;
		case	XML_READER_TYPE_SIGNIFICANT_WHITESPACE : 
				return	"sig_whitespace" ; break;
		case	XML_READER_TYPE_END_ELEMENT : 
				return	"end_element" ; break;
		case	XML_READER_TYPE_END_ENTITY : 
				return	"end_entity" ; break;
		case	XML_READER_TYPE_XML_DECLARATION : 
				return	"declaration" ; break;
		default :
		break;
	}
	return	"type_error";
}

// seuls 3 node types nous interessent :
// - debut d'element <balise> :
// 	- on cree l'objet root
// 	- on cree les objets attributs de type liste ou non
// 	- on se prepare a lire des attributs simples
// 	- push
// - contenu d'element <balise>...<balise>
// - fin d'element <balise>
// 	- pop
// 	- rien a faire d'autre si attributs simples
// 	- on recupere les objets crees
int	_XoProcessNode(/*xmlTextReaderPtr*/void *reader,t_ctxt *ctxt)
{
	int	ret = 1;
	int	depth;
	int	nodeType;
	int	isEmpty		= 1;
	int	attrCount	= 0;
	char	*qname	= NULL;
	char	*value	= NULL;

	char		*myqname;
	void		*curObj	= NULL;
	void		*newObj	= NULL;
	t_attribut_ref	*curAtr	= NULL;
	t_attribut_ref	*svAtr	= NULL;
	int32		numattr;
	char		*nameattr;
	char		*nameobj;
	char		*waitendof;
	int		lg	= 0;
	char		buffattr[XOML_TAG_LG*2]; // result of xodoname


//	depth		= xmlTextReaderDepth(reader);
	depth		= ctxt->depth;
	if	(depth >= XOML_MAX_LEVEL-1 || ctxt->level >= XOML_MAX_LEVEL-1)
	{
		return	-1000;
	}

//	nodeType	= xmlTextReaderNodeType(reader);
	nodeType	= ctxt->nodetype;
//	qname		= xmlTextReaderName(reader);
	qname		= ctxt->qname;



	switch(nodeType)
	{
	case	XML_READER_TYPE_ELEMENT :	// start elem
	{
		char	*balise	= NULL;

//		isEmpty		= xmlTextReaderIsEmptyElement(reader);
		isEmpty		= ctxt->isempty;
//		attrCount	= xmlTextReaderAttributeCount(reader);
		attrCount	= ctxt->atcount;

if (C_PARSE_TRACE(ctxt))
XOML_TRACE("nodeType=%02d '%s' qname='%s' attr=%d depth=%d level=%d isEmpty=%d\n",
nodeType,TypeNodeTxt(nodeType),qname,attrCount,depth,ctxt->level,isEmpty);
		if (C_PARSE_ONLY(ctxt))
		{
			goto	endnode;
		}

		if (ctxt->skipdepth && depth > ctxt->skipdepth)
		{
if (C_PARSE_TRACE(ctxt))
XOML_TRACE("--------------------- SKIP %d>%d\n",depth,ctxt->skipdepth);
			goto	endnode;
		}

if (C_PARSE_TRACE(ctxt) && ctxt->skipdepth)
XOML_TRACE("--------------------- RESET SKIP mode %d\n",depth);

		ctxt->skipdepth	= 0;

#if	0	// now done before call _XoProcessNode()
		FetchAttributes(reader,ctxt);
#endif
		if	(LoadAttributesAndNS(reader,attrCount,ctxt) <= 0)
		{
			ret	= -5;
			goto	endnode;
		}

		if	(depth <= 0 && ctxt->level <= 0)
		{
			balise	= (char *)qname;
			if (C_PARSE_NATIVENS(ctxt))
			nameattr= balise;
			else
			nameattr= _XoDoName(&(ctxt->ns),balise,NULL,buffattr);
			curObj = _XoRootPush(reader,ctxt,nameattr,balise,&ret);
			if	(!curObj)
			{
				goto	endnode;
			}
			nameobj	= XoGetRefObjName(XoType(curObj));
			DoElementName(ctxt,curObj,nameobj,balise);
			StoreAttributes(reader,attrCount,ctxt,curObj);
			NeedPushWrapper(reader,ctxt,balise);
			goto	endnode;
		}

		curObj	= ctxt->obj[ctxt->level-1];
		if	(!curObj && ctxt->waittype[ctxt->level-1] != END_LIST)
		{
			if	(ctxt->waittype[ctxt->level-1] != END_WRAPPER)
			{
				ret	= -20;
				goto	endnode;
			}
		}
		myqname	= (char *)qname;
autowrapper :
		if	(!curObj)
		{	// normal si liste d'objets
			curAtr	= ctxt->ar[ctxt->level-2];
			svAtr	= curAtr;
			if	(!curAtr)
			{
				ret	= -21;
				goto	endnode;
			}
			newObj	= AllocNewObject(reader,attrCount,ctxt,&curAtr,
						(char *)myqname,&ret);
			if	(!newObj)
			{
				goto	endnode;
			}
			if (isEmpty) 
			{
if (C_PARSE_TRACE(ctxt))
XOML_TRACE("--------------------- NO PUSH for empty obj in list\n");
				curObj	=	ctxt->cnt[ctxt->level-1];
				XoAddInAttr(curObj,svAtr->ar_num,newObj,lg) ;
				goto	endnode;
			}
if (C_PARSE_TRACE(ctxt))
XOML_TRACE("--------------------- PUSH for obj in list %d\n",ctxt->level);
			ctxt->ar[ctxt->level]		= curAtr;
			ctxt->cnt[ctxt->level]		= 
						ctxt->cnt[ctxt->level-1];
			ctxt->obj[ctxt->level]		= newObj;
			ctxt->waittype[ctxt->level]	= END_OBJECT;
#ifdef PARSE_DEBUG
			strcpy(ctxt->waitstr[ctxt->level],balise);
#endif
			ctxt->level++;
			NeedPushWrapper(reader,ctxt,balise);

			goto	endnode;
		}



		nameobj	= XoGetRefObjName(XoType(curObj));

		nameattr= FindAttrInObj(ctxt,curObj,nameobj,(char *)myqname,NULL,
						&numattr);
		balise	= nameattr;
		if	(numattr < 0)
		{
if (C_PARSE_TRACE(ctxt))
XOML_TRACE("xo start attr='%s' in obj '%s' does not exist\n", nameattr,nameobj);

			if (C_PARSE_AUTOWRAPP(ctxt))
			{
				nameattr= FindAutoWrapper(ctxt,curObj,
					nameobj,(char *)myqname,&numattr);
				if (nameattr && numattr > 0)
				{
					AutoPushWrapper(reader,ctxt,numattr);
					myqname	= nameattr;
					curObj	= NULL;
					goto	autowrapper;
				}
			}
if (C_PARSE_TRACE(ctxt))
XOML_TRACE("--------------------- SET SKIP mode %d\n",depth);
			ctxt->skipdepth	= depth;
			goto	endnode;
		}

		curAtr	= XoGetRefAttr (numattr);
		if	(!curAtr)
		{
			ret	= -31;
			goto	endnode;
		}
		ctxt->ar[ctxt->level-1]	= curAtr;

if (C_PARSE_TRACE(ctxt))
XOML_TRACE("xo start attr='%s' in obj '%s' has num %d mode=%d type='%s'\n",nameattr,
				nameobj,numattr,
				curAtr->ar_mode,curAtr->ar_type_name);

		switch	(curAtr->ar_mode)
		{
		case	BASIC_ATTR :
if (C_PARSE_TRACE(ctxt))
XOML_TRACE("--------------------- ==== for basic attr %d\n",ctxt->level);
			if (isEmpty && curAtr->ar_type == XoString)
			{
				XoSetAttr(curObj,curAtr->ar_num,"",0) ;
			}
		case	LIST_BASIC_ATTR :	
			if ( ctxt->level == 1  && C_PARSE_TARGET(ctxt) )
			{
				StoreAttributes(reader,attrCount,ctxt,curObj);
			}
if (C_PARSE_TRACE(ctxt))
XOML_TRACE("--------------------- PUSH for list/basic attr %d\n",ctxt->level);
			ctxt->ar[ctxt->level]		= curAtr;
			ctxt->cnt[ctxt->level]		= curObj;
			ctxt->obj[ctxt->level]		= curObj;
			ctxt->waittype[ctxt->level]	= END_ATTR;
#ifdef PARSE_DEBUG
			strcpy(ctxt->waitstr[ctxt->level],balise);
#endif
			ctxt->level++;
		break;
		case	OBJ_ATTR :
			svAtr	= curAtr;
			newObj	= AllocNewObject(reader,attrCount,ctxt,&curAtr,
						(char *)myqname,&ret);
			if	(!newObj)
			{
				goto	endnode;
			}
			if (isEmpty) 
			{
if (C_PARSE_TRACE(ctxt))
XOML_TRACE("--------------------- NO PUSH for empty obj\n");
				XoSetAttr(curObj,svAtr->ar_num,newObj,0) ;
				goto	endnode;
			}
if (C_PARSE_TRACE(ctxt))
XOML_TRACE("--------------------- PUSH for obj %d\n",ctxt->level);
			ctxt->ar[ctxt->level]		= curAtr;
			ctxt->cnt[ctxt->level]		= curObj;
			ctxt->obj[ctxt->level]		= newObj;
			ctxt->waittype[ctxt->level]	= END_OBJECT;
#ifdef PARSE_DEBUG
			strcpy(ctxt->waitstr[ctxt->level],balise);
#endif
			ctxt->level++;
			NeedPushWrapper(reader,ctxt,balise);
		break;
		case	LIST_OBJ_ATTR :
if (C_PARSE_TRACE(ctxt))
XOML_TRACE("--------------------- PUSH for list/obj %d\n",ctxt->level);
			ctxt->ar[ctxt->level]		= curAtr;
			ctxt->cnt[ctxt->level]		= curObj;
			ctxt->obj[ctxt->level]		= NULL;
			ctxt->waittype[ctxt->level]	= END_LIST;
#ifdef PARSE_DEBUG
			strcpy(ctxt->waitstr[ctxt->level],balise);
#endif
			ctxt->level++;
		break;
		default:
			ret	= -400;
			goto	endnode;
		break;
		}

	}
	break;

	case	XML_READER_TYPE_TEXT :
	{
if (C_PARSE_TRACE(ctxt))
XOML_TRACE("nodeType=%02d '%s' qname='%s' depth=%d level=%d\n",
	nodeType,TypeNodeTxt(nodeType),qname,depth,ctxt->level);

		if (C_PARSE_ONLY(ctxt))
		{
			goto	endnode;
		}
		if (ctxt->skipdepth && depth >= ctxt->skipdepth)
		{
if (C_PARSE_TRACE(ctxt))
XOML_TRACE("--------------------- SKIP %d>=%d\n",depth,ctxt->skipdepth);
			goto	endnode;
		}
		curObj	= ctxt->obj[ctxt->level-1];
		if	(curObj == NULL)
		{
			ret	= -50;
			goto	endnode;
		}
		curAtr	= ctxt->ar[ctxt->level-1];
		if	(curAtr == NULL)
		{
			ret	= -51;
			goto	endnode;
		}

		nameobj	= XoGetRefObjName(XoType(curObj));
//		value	= xmlTextReaderValue(reader);
		value	= ctxt->value;

if (C_PARSE_TRACE(ctxt))
XOML_TRACE("xo value attr='%s' in obj '%s' mode=%d type=%s\n",
			curAtr->ar_name,nameobj,
			curAtr->ar_mode,curAtr->ar_type_name);
if (C_PARSE_TRACE(ctxt))
XOML_TRACE("	value='%s'\n",value);

		if (!value)
		{
			ret	= -55;
			goto	endnode;
		}
		if (curAtr->ar_type == XoBuffer)
		{
			unsigned char	*out;
			unsigned long	outlen;
			unsigned long	inlen;
			char	*svvalue;
			int	ret;

			inlen = (unsigned long)strlen((char*)value);
			if (inlen <= 0)
				break;
#if	0
			if (strcmp(nameobj,"xmime:base64Binary"))
			{
				ret	= -56;
				goto	endnode;
			}
#endif
			outlen	= inlen;
			out	= (unsigned char *)malloc(outlen);
			if (!out)
			{
				ret	= -57;
				goto	endnode;
			}
			ret	= __xmlBase64Decode((unsigned char *)value,&inlen,out,&outlen);
//printf("xxxx %d '%s' '%s' lgin=%d lgout=%d ret=%d\n",__LINE__,nameobj,curAtr->ar_name,inlen,outlen,ret);
			if (ret < 0)
			{
				ret	= -58;
				goto	endnode;
			}
//			strcpy	(out,"bonjour");	// Ym9uam91cg==
			svvalue	= (char *)value;	// because of XmlFree
			value	= (char *)out;
			_XoSetBasicValue(curObj,curAtr,(char **)&value,outlen);
			value	= svvalue;	
			free	(out);
		}
		else
			_XoSetBasicValue(curObj,curAtr,(char **)&value,0);
	}
	break;

	case	XML_READER_TYPE_END_ELEMENT :
	{
		waitendof	= "<nodbg>";
#ifdef PARSE_DEBUG
		if (ctxt->level >= 1)
			waitendof	= ctxt->waitstr[ctxt->level-1];
		else
			waitendof	= "???";
		waitendof	= XoDropPrefixe(waitendof);
#endif

if (C_PARSE_TRACE(ctxt))
XOML_TRACE("nodeType=%02d '%s' qname='%s' depth=%d level=%d wait=%s waittype='%s'\n",
		nodeType,TypeNodeTxt(nodeType),qname,
		depth,ctxt->level,waitendof,
		WaitTypeTxt(ctxt->waittype[ctxt->level-1]));

		if (C_PARSE_ONLY(ctxt))
		{
			goto	endnode;
		}
		if (ctxt->skipdepth && depth >= ctxt->skipdepth)
		{
if (C_PARSE_TRACE(ctxt))
XOML_TRACE("--------------------- SKIP %d>=%d\n",depth,ctxt->skipdepth);
			goto	endnode;
		}
if (C_PARSE_TRACE(ctxt) && ctxt->skipdepth)
XOML_TRACE("--------------------- RESET SKIP mode %d\n",depth);


		ctxt->skipdepth	= 0;

		nameattr= XoDropPrefixe((char *)qname);
#ifdef PARSE_DEBUG
		if (strcmp(waitendof,nameattr))
		{
if (C_PARSE_TRACE(ctxt))
XOML_TRACE("wait for '%s' and '%s' received => error \n",waitendof,nameattr);
			ret	= -60;
			goto	endnode;
		}
if (C_PARSE_TRACE(ctxt))
XOML_TRACE("wait for '%s' and '%s' received => synchro \n",waitendof,nameattr);
#endif


		if	(ctxt->level <= 1 
				&& ctxt->waittype[ctxt->level-1] != END_ROOT)
		{
			ret	= -70;
			goto	endnode;
		}

		// rien a faire pop only
		switch	(ctxt->waittype[ctxt->level-1])
		{
		case	END_ROOT :
			ctxt->level--;
if (C_PARSE_TRACE(ctxt))
XOML_TRACE("--------------------- POP for ROOT %d\n",ctxt->level);
			goto	endnode;
		break;
		case	END_ATTR :
			ctxt->level--;
if (C_PARSE_TRACE(ctxt))
XOML_TRACE("--------------------- POP for list/basic attr %d\n",ctxt->level);
			goto	endnode;
		break;
		case	END_LIST :
			ctxt->level--;
if (C_PARSE_TRACE(ctxt))
XOML_TRACE("--------------------- POP for empty/end list/obj %d\n",ctxt->level);
			goto	endnode;
		break;
		case	END_WRAPPER :
			ctxt->level--;
if (C_PARSE_TRACE(ctxt))
XOML_TRACE("--------------------- POP for WRAPPER list/obj %d\n",ctxt->level);
		break;
		}


		// recupere l'objet qui vient d'etre parse
		switch	(ctxt->waittype[ctxt->level-1])
		{
		case	END_ROOT :
			ctxt->level--;
if (C_PARSE_TRACE(ctxt))
XOML_TRACE("--------------------- POP for ROOT %d\n",ctxt->level);
			goto	endnode;
		break;
		case	END_OBJECT :
			ctxt->level--;
if (C_PARSE_TRACE(ctxt))
XOML_TRACE("--------------------- POP for obj attr %d\n",ctxt->level);
		break;
		default :
			ret	= -71;
			goto	endnode;
		break;
		}



		curAtr	= ctxt->ar[ctxt->level-1];
		if	(curAtr == NULL)
		{
			ret	= -72;
			goto	endnode;
		}
		if	(ctxt->cnt[ctxt->level-1] == NULL)
		{
			nameobj	= "???";
		}
		else
			nameobj	= XoGetRefObjName(XoType(ctxt->cnt[ctxt->level-1]));

if (C_PARSE_TRACE(ctxt))
XOML_TRACE("xo end obj='%s' in obj '%s'/'%s' mode=%d type=%s waittype='%s'\n",
			nameattr,nameobj,curAtr->ar_name,
			curAtr->ar_mode,curAtr->ar_type_name,
			WaitTypeTxt(ctxt->waittype[ctxt->level-1]));

		switch	(curAtr->ar_mode)
		{
		case	OBJ_ATTR :
			curObj	= ctxt->obj[ctxt->level-1];
			if	(curObj == NULL)
			{
				ret	= -75;
				goto	endnode;
			}

			newObj	= ctxt->obj[ctxt->level];
			curObj	= ctxt->obj[ctxt->level-1];
			curAtr	= ctxt->ar[ctxt->level-1];
			XoSetAttr(curObj,curAtr->ar_num,newObj,0) ;
if (C_PARSE_TRACE(ctxt))
XOML_TRACE("--------------------- STORE obj\n");
		break;

		case	LIST_OBJ_ATTR :
			newObj	= ctxt->obj[ctxt->level];
			if	(newObj == NULL)
			{
				ret	= -80;
				goto	endnode;
			}
			curObj	= ctxt->cnt[ctxt->level-1];
			curAtr	= ctxt->ar[ctxt->level-1];
			XoAddInAttr(curObj,curAtr->ar_num,newObj,lg) ;

if (C_PARSE_TRACE(ctxt))
XOML_TRACE("--------------------- STORE obj + SHIFT %d\n",ctxt->level);
			ctxt->obj[ctxt->level]	= NULL;

		break;

		}

	}
	break;

	default :
if (C_PARSE_TRACE(ctxt))
XOML_TRACE("nodeType=%02d '%s'\n",nodeType,TypeNodeTxt(nodeType));
	break;
	}

endnode :

#if	0	// now done after call _XoProcessNode()
	if (ctxt->atcount)
		FreeAttributes(ctxt);
#endif

	if ( C_PARSE_INTERACTIF(ctxt) ) 
	{
		switch(nodeType)
		{
		case	XML_READER_TYPE_ELEMENT :	// start elem
		case	XML_READER_TYPE_TEXT :
		case	XML_READER_TYPE_END_ELEMENT :
			ctxt->prevnode	= nodeType;
		break;
		default :
			ctxt->prevnode	= nodeType;
		break;
		}
	}

#if	0	// now done after call _XoProcessNode()
	if (qname)
		xmlFree(qname);
	if (value)
		xmlFree(value);
#endif

	return	ret;
}
