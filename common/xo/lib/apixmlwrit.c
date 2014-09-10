
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

#include <libxml/xmlwriter.h>

#include "xoapipr.h"

#include "xmlstr.h"

static	char	*xo_file	= __FILE__;


#define	XCAST(x)	(const xmlChar *)((x))


typedef	struct
{
	int	passe;
	int	flags;
	int	nsdone;
	void	*root;
	char	*defuri;
	char	defpref[128];
	char	rootpref[128];
}	t_wctxt;

static	int32	xowritobj(xmlTextWriterPtr writer,t_wctxt *ctxt,void *obj,int32 level);



static	char	*ObjPrefixe(void *obj,char *dst)
{
	char	*nameobj;
	char	*sep;
	int	lg;

	strcpy	(dst,"");
	nameobj	= XoGetRefObjName(XoType(obj));
	sep	= strchr(nameobj,':');
	if (!sep) 
		return	dst;
	lg	= sep - nameobj;
	if (lg <= 0)
		return	dst;

	strncpy(dst,nameobj,lg);
	dst[lg]	= '\0';
//fprintf(stderr,"obj prefixe '%s'\n",dst);
	return	dst;
}

static	char	*DoName(t_wctxt *ctxt,void *obj,char *pref,char *nameattr,char *dst)
{
	char	*p;
	char	*def;

	if (C_PARSE_NATIVENS(ctxt))
	{	// as is
		sprintf	(dst,"%s",nameattr);
		return	dst;
	}

	if	(strchr(nameattr,':') == NULL || XoIsObix(obj) > 0)
	{
		sprintf	(dst,"%s",nameattr);
		return	dst;
	}

	def	= &(ctxt->defpref[0]);
	p	= pref;	// le prefixe de la classe
	if	(!p || !*p || XoIsObix(obj) > 0)
	{
		sprintf	(dst,"%s",nameattr);
		return	dst;
	}
	if	(p && *p && def && *def && strcmp(p,def) == 0
			&& strncmp(p,nameattr,strlen(p)) == 0)
	{
		sprintf	(dst,"%s",XoDropPrefixe(nameattr));
		return	dst;
	} 
	if	(strchr(nameattr,':') != NULL)
	{
		sprintf	(dst,"%s",nameattr);
		return	nameattr;
	}

	sprintf	(dst,"%s:%s",p,nameattr);
	return	dst;
}

static	int32	xowritbasicattr(xmlTextWriterPtr writer,t_wctxt *ctxt,char *nameattr,void *value,int32 lg,t_attribut_ref *ar,int32 level)
{
	char		tmp[128];
	int32		l ;
	unsigned char	c ;

//printf("n=%s l=%d\n",nameattr,lg);

	switch	(ar->ar_type)
	{
	case	XoBool :
	case	XoInt4B:
	case	XoDate :
		l	= *(int32 *)value ;
		sprintf(tmp,"%d",(int32)l) ;
		value	= tmp;
	break ;
	case	XoChar :
		c	= *(char *)value ;
		sprintf(tmp,"%c",(int32)c) ;
		value	= tmp;
	break ;
	case	XoString :
	break ;
	case	XoBuffer :
	break ;
	default :
		XOERR("xowritbasicattr",0) ;
		return	-1;
	break ;
	}
	if	(xmlTextWriterStartElement(writer,XCAST(nameattr)) < 0)
	{
		XOERR("startelement",0) ;
		return	-2;
	}
	if	(ar->ar_type == XoBuffer)
	{
		if	(xmlTextWriterWriteBase64(writer,value,0,lg) < 0)
		{
			XOERR("writebase64",0) ;
			return	-3;
		}
	}
	else
	{
		if	(xmlTextWriterWriteString(writer,value) < 0)
		{
			XOERR("writestring",0) ;
			return	-4;
		}
	}
	if	(xmlTextWriterEndElement(writer) < 0)
	{
		XOERR("endelement",0) ;
		return	-5;
	}

	return	1;
}

int32	xowritattr(xmlTextWriterPtr writer,t_wctxt *ctxt,void *obj,int32 numattr,int32 level)
{
	char	buffpref[XOML_TAG_LG];
	char	buffattr[XOML_TAG_LG*2];
	t_attribut_ref	*ar ;
	int32	type ;
	int32	mode ;
	char	*nameattr ;

	int32	lg ;
	int32	nb ;
	void	*value ;

	int32	pos ;

	nameattr	= XoGetRefAttrName(numattr) ;
	if	( !nameattr )
	{
		XOERR("",0) ;
		return	-1 ;
	}
	ar	= XoGetRefAttr(numattr) ;
	if	( !ar )
	{
		XOERR("",0) ;
		return	-11 ;
	}

	if	( IS_NO_SAVE(ar) )
		return	0 ;

	mode	= XoModeAttr(numattr) ;
	if	( mode < 0 )
	{
		XOERR("",0) ;
		return	-2 ;
	}

	type	= XoTypeAttr(numattr) ;
	if	( type < 0 )
	{
		XOERR("",0) ;
		return	-3 ;
	}

	if	(mode==BASIC_ATTR && IS_SAVE_UNSET(ar) && 
					XoIsSetAttr(obj,numattr)==0)
	{
		/* attribut non valorise et flag saveUnset */

		return	1 ;
	}


	ObjPrefixe(obj,buffpref);
	switch	(mode)
	{
	case	BASIC_ATTR :
		value	= XoGetAttr	(obj,numattr,&lg) ;

		if	( !value )
			break;
		if	(strcmp(nameattr,XOML_PSEUDO_ATTR) == 0)
		{
			// TODO
			break;
		}
		nameattr = DoName(ctxt,obj,buffpref,nameattr,buffattr);
		if (xowritbasicattr(writer,ctxt,nameattr,value,lg,ar,level)<0)
		{
			XOERR("xowritbasicattr",0) ;
			return	-4 ;
		}
	break ;
	case	LIST_BASIC_ATTR :
		nb	= XoNbInAttr(obj,numattr) ;
		if	( nb <= 0 )
			break ;

		nameattr = DoName(ctxt,obj,buffpref,nameattr,buffattr);
		nb	= 0;
		pos	= XoFirstAttr(obj,numattr) ;
		while	( pos > 0 )
		{
			value	= XoGetAttr	(obj,numattr,&lg) ;
			if	( value )
			{
				if (xowritbasicattr(writer,ctxt,nameattr,value,lg,ar,level)<0)
				{
					XOERR("xowritbasicattr",0) ;
					return	-6 ;
				}
			}
			pos	= XoNextAttr(obj,numattr) ;
		}
	break ;
	case	OBJ_ATTR :
		value	= XoGetAttr	(obj,numattr,&lg) ;
		if	( value )
		{
			if (xowritobj(writer,ctxt,value,level+1) < 0)
			{
				XOERR("xowritobj",0) ;
				return	-5 ;
			}
		}
	break ;
	case	LIST_OBJ_ATTR :
		nb	= XoNbInAttr(obj,numattr) ;
		if	( nb <= 0 )
			break ;

		if	(!IS_WRAPHIDDEN(ar) && strcmp(nameattr,XOML_WRAPPE_ATTR) != 0)
		{
			nameattr = DoName(ctxt,obj,buffpref,nameattr,buffattr);
			if (xmlTextWriterStartElement(writer,XCAST(nameattr)) < 0)
			{
				XOERR("StartElement",0) ;
				return	-6 ;
			}
		}

		nb	= 0;
		pos	= XoFirstAttr(obj,numattr) ;
		while	( pos > 0 )
		{
			value	= XoGetAttr	(obj,numattr,&lg) ;
			if	( value )
			{
				if (xowritobj(writer,ctxt,value,level+1) < 0)
				{
					XOERR("xowritobj",0) ;
					return	-7 ;
				}
			}
			pos	= XoNextAttr(obj,numattr) ;
		}

		if	(!IS_WRAPHIDDEN(ar) && strcmp(nameattr,XOML_WRAPPE_ATTR) != 0)
		{
			if	(xmlTextWriterEndElement(writer) < 0)
			{
				XOERR("EndElement",0) ;
				return	-8 ;
			}
		}

	break ;
	default	:
		XOERR("",0) ;
		return	-10 ;
	break ;
	}

	return	1 ;

}

static	int32	xowritobj(xmlTextWriterPtr writer,t_wctxt *ctxt,void *obj,int32 level)
{
	char	buffpref[XOML_TAG_LG];
	char	buffattr[XOML_TAG_LG*2];
	t_attribut_ref	*ar ;
	int32	type ;
	int32	nbattr ;
	char	*nameobj ;
	int32	a ;
	char	*nameattr;
	int32	numattr ;
	int32	numbalise = -1 ;
	int32	mode ;
	int32	lg ;
	char	*tag;
	int	elem	= 0;
	int	dodef	= 0;
	int	savebalise	= 1;

	type	= XoType(obj) ;
	if	( type < 0 )
	{
		XOERR("",0) ;
		return	-10 ;
	}
	nbattr	= XoNbAttr(type) ;
	if	( nbattr < 0 )
	{
		XOERR("",0) ;
		return	-20 ;
	}
	nameobj	= XoGetRefObjName(type) ;
	if	( !nameobj )
	{
		XOERR("",0) ;
		return	-30 ;
	}


	if (C_PARSE_TARGET(ctxt) && level == 0)
	{
		savebalise	= 0;
	}


	ObjPrefixe(obj,buffpref);
	tag		= NULL;
	numbalise	= XoNumAttrInObj(obj,XOML_BALISE_ATTR);
	if	(numbalise > 0)
		tag	= XoGetAttr(obj,numbalise,&lg);


	if	(!tag)
		tag	= nameobj;

//fprintf(stderr,"tag='%s'\n",tag);

	if	(savebalise && tag && *tag != '<')
	{
		if	(XoIsObix(obj)>0)
		{
				tag	= XoDropPrefixe(tag);
		}
		else
		{
			char	*def;

			def	= &(ctxt->defpref[0]);
			if	(def && *def && strcmp(buffpref,def) == 0)
			{
				tag	= XoDropPrefixe(tag);
				dodef	= 1;
			} 
		}
		if	(xmlTextWriterStartElement(writer,XCAST(tag)) < 0)
		{
			XOERR("StartElement",0) ;
			return	-40 ;
		}
		elem	= 1;
	}

	if	(level == 0 && elem && dodef && XoIsObix(obj) <= 0)
	{
		char	*uri;

		uri	= _XoFindUriNameSpace(buffpref);
		if	(!uri)
		{
			char	tmperr[256];
			sprintf(tmperr,"URI not found for '%s'",buffpref) ;
			XOERR(tmperr,0) ;
			return	-45 ;
		}
		ctxt->defuri	= uri;
		if (xmlTextWriterWriteAttribute(writer,
					XCAST("xmlns"),XCAST(uri)) < 0)
		{
			char	tmperr[256];
			sprintf(tmperr,"Attribute '%s'","xmlns") ;
			XOERR(tmperr,0) ;
			return	-46 ;
		}
	}

	if	(level == 0 && XoIsObix(obj) > 0)
	{	// OBIX => on force xmlns=URI(o:)
		char	*uri;

		uri	= _XoFindUriNameSpace(buffpref);
		if	(!uri)
		{
			XOERR("xmlns for obix error",0) ;
			return	-50 ;
		}
		if (xmlTextWriterWriteAttribute(writer,
					XCAST("xmlns"),XCAST(uri)) < 0)
		{
			char	tmperr[256];
			sprintf(tmperr,"Attribute '%s'","xmlns") ;
			XOERR(tmperr,0) ;
			return	-55 ;
		}
	}

	// embeded attr declares
	for	( a = 0 ; a < nbattr ; a++ )
	{
		char	*value;

		numattr	= type + a + 1 ;
		if	(numbalise > 0 && numattr == numbalise)
			continue;
		mode	= XoModeAttr(numattr) ;
		ar	= XoGetRefAttr(numattr) ;
		if	(mode == BASIC_ATTR && IS_EMBATTR(ar))
		{
			value	= XoGetAttr(obj,numattr,&lg) ;
			if 	(strcmp(ar->ar_name,"xmlns") == 0)
			{
				char	*pref;

				if	(XoIsObix(obj) > 0)
					continue;

				pref	= _XoFindPrfNameSpace(value);
				if	(!pref)
					continue;
#if	0
				if	(strcmp(ctxt->defpref,pref) == 0)
					continue; 
#endif
				sprintf(buffattr,"xmlns:%s",pref);
				nameattr = buffattr;
			}
			else
			{
				nameattr = DoName(ctxt,obj,buffpref,
							ar->ar_name,buffattr);
			}
//fprintf(stderr,"attr='%s' ar_name='%s' ar_value='%s'\n",nameattr,ar->ar_name,value);

			if (elem && value && xmlTextWriterWriteAttribute(writer, 
				XCAST(nameattr),XCAST(value)) < 0)
			{
				char	tmperr[256];
				sprintf(tmperr,"Attribute '%s' '%s'",
						ar->ar_name,nameattr) ;
				XOERR(tmperr,0) ;
				return	-60 ;
			}
		}
	}


	// custom embeded attr for OBIX object
	if	(XoIsObix(obj) > 0 && XoGetRefObjCustomAttributs(type) > 0)
	{
		int32	posn;
		int32	lg;
		char	*name;
		char	*value;

		numattr = XoNumAttrInObj(obj,XOML_CUSTOM_ATTR);
		if (numattr < 0)
		{
			XOERR("Custom Attribute",0) ;
			return	-70 ;
		}

		posn	= XoFirstAttr(obj,numattr) ;
		while	( posn > 0 )
		{
			void	*sub;

			sub	= XoGetAttr	(obj,numattr,&lg);
			if (!sub)
				break;
			name	= XoNmGetAttr	(sub,XOML_CUSTOM_ATTR_NAME,0) ;
			if	(strcmp(name,"xmlns") == 0)
			// OBIX on a deja force xmlns=URI(o:)
				goto	dropattr;
			value	= XoNmGetAttr	(sub,XOML_CUSTOM_ATTR_VALUE,0) ;
			if	( !name || !*name || !value || !*value )
				break;
			if (xmlTextWriterWriteAttribute(writer, 
				XCAST(name),XCAST(value)) < 0)
			{
				XOERR("Custom Attribute",0) ;
				return	-80 ;
			}
dropattr :
			posn	= XoNextAttr(obj,numattr) ;
		}
	}

	// simple type extension => "value$"
	if	(XoGetRefObjStExt(type) > 0)
	{
		char	*value;

		value	= XoNmGetAttr(obj,XOML_PSEUDO_ATTR,&lg) ;
		if	(!value || !*value)
		{
			if	(xmlTextWriterEndElement(writer) < 0)
			{
				XOERR("EndElement",0) ;
				return	-90 ;
			}
			return	1;
		}
		numattr = XoNumAttrInObj(obj,XOML_PSEUDO_ATTR);
		if (numattr < 0)
		{
			XOERR("Pseudo Attribute",0) ;
			return	-91 ;
		}
		ar	= XoGetRefAttr(numattr) ;
#if	0
		if (xmlTextWriterWriteString(writer,XCAST(value)) < 0)
		{
			XOERR("Content STEXT",0) ;
			return	-90 ;
		}
#endif
		if	(ar->ar_type == XoBuffer)
		{
			if	(xmlTextWriterWriteBase64(writer,value,0,lg) < 0)
			{
				XOERR("writebase64",lg) ;
				return	-92;
			}
		}
		else
		{
			if	(xmlTextWriterWriteString(writer,(const xmlChar *)value) < 0)
			{
				XOERR("writestring",0) ;
				return	-93;
			}
		}
		if	(xmlTextWriterEndElement(writer) < 0)
		{
			XOERR("EndElement",0) ;
			return	-100 ;
		}
		return	1;
	}


	for	( a = 0 ; a < nbattr ; a++ )
	{
		numattr	= type + a + 1 ;
		if	(numbalise > 0 && numattr == numbalise)
			continue;
		mode	= XoModeAttr(numattr) ;
		ar	= XoGetRefAttr(numattr) ;
		if	( IS_EMBATTR(ar) )
			continue;

		switch	(mode)
		{	// do not save empty list
			case	LIST_BASIC_ATTR :
			case	LIST_OBJ_ATTR :
				if	(XoNbInAttr(obj,numattr) <= 0)
				{
					continue;
				}
			break;
		}
		switch	(mode)
		{
			case	BASIC_ATTR :
			case	LIST_BASIC_ATTR :
				if(xowritattr(writer,ctxt,obj,numattr,level) < 0)
				{
					XOERR("xowritattr",0) ;
					return	-110 ;
				}
			break;
			case	OBJ_ATTR :
			case	LIST_OBJ_ATTR :
				if(xowritattr(writer,ctxt,obj,numattr,level+1) < 0)
				{
					XOERR("xowritattr",0) ;
					return	-120 ;
				}
			break;
		}
	}
	
	if	(elem)
	{
		if	(xmlTextWriterEndElement(writer) < 0)
		{
			XOERR("EndElement",0) ;
			return	-130 ;
		}
	}

	return	1 ;
}

/*DEF*/	void	*XoWritXmlMem(void *obj,int32 flags,char **bufout,char *defns)
{
	xmlBufferPtr	buf;
	xmlTextWriterPtr writer;
	t_wctxt		ctxt;
	int32		ret ;

	memset	(&ctxt,0,sizeof(t_wctxt));
	ctxt.flags	= flags;
	ctxt.passe	= 0;
	ctxt.root	= obj;
	ObjPrefixe(obj,ctxt.rootpref);

	if	(defns && *defns)
		strcpy(ctxt.defpref,defns);

#if	0
	ret	= xowritobj(NULL,&ctxt,obj,0) ;
	if	(ret < 0)
	{
		return	NULL;
	}
	ctxt.passe++;
#endif


	*bufout	= NULL;
	buf	= xmlBufferCreate();
	if	(!buf)
		return	NULL;
	writer	= xmlNewTextWriterMemory(buf, 0);
	if	(!writer)
	{
		xmlBufferFree(buf);
		return	NULL;
	}
	ret	 = xmlTextWriterStartDocument(writer, NULL, "UTF-8", NULL);
	if	(ret < 0)
	{
		xmlFreeTextWriter(writer);
		xmlBufferFree(buf);
		return	NULL;
	}


	ret	= xowritobj(writer,&ctxt,obj,0) ;
	if	(ret < 0)
	{
		xmlFreeTextWriter(writer);
		xmlBufferFree(buf);
		return	NULL;
	}

	ret	 = xmlTextWriterEndDocument(writer);
	if	(ret < 0)
	{
		xmlFreeTextWriter(writer);
		xmlBufferFree(buf);
		return	NULL;
	}

	xmlFreeTextWriter(writer);

	*bufout	= (char *)buf->content;

	return	(char *)buf ;
}

/*DEF*/	void	XoWritXmlFreeMem(void *buf)
{
	if (buf)
		xmlBufferFree((xmlBufferPtr)buf);
}

/*DEF*/	void	XoWritXmlFreeCtxt(void *pbuf)
{
	xmlBufferPtr	buf	= (xmlBufferPtr)pbuf;
	if (buf)
	{
		buf->content	= NULL;
		xmlBufferFree(buf);
	}
}

/*DEF*/	int32	XoWritXmlEx(char *filename,void *obj,int32 flags,char *defns)
{
	xmlTextWriterPtr writer;
	t_wctxt		ctxt;
	int32	ret ;

	memset	(&ctxt,0,sizeof(t_wctxt));
	ctxt.flags	= flags;
	ctxt.passe	= 0;
	ctxt.root	= obj;
	ObjPrefixe(obj,ctxt.rootpref);

	if	(defns && *defns)
		strcpy(ctxt.defpref,defns);

#if	0
	ret	= xowritobj(NULL,&ctxt,obj,0) ;
	if	(ret < 0)
	{
		return	-11;
	}
	ctxt.passe++;
#endif

	writer	= xmlNewTextWriterFilename(filename, 0);
	if	(!writer)
	{
		return	-1;
	}
	xmlTextWriterSetIndent(writer,1);
	ret	 = xmlTextWriterStartDocument(writer, NULL, "UTF-8", NULL);
	if	(ret < 0)
	{
		xmlFreeTextWriter(writer);
		return	-2;
	}


	ret	= xowritobj(writer,&ctxt,obj,0) ;
	if	(ret < 0)
	{
		xmlFreeTextWriter(writer);
		return	-3;
	}

	ret	 = xmlTextWriterEndDocument(writer);
	if	(ret < 0)
	{
		xmlFreeTextWriter(writer);
		return	-4;
	}

	xmlFreeTextWriter(writer);
	return	1 ;
}

