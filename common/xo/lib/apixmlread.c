
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

static	void	FetchAttributes(xmlTextReaderPtr reader,t_ctxt *ctxt)
{
	int	i;
	int	attrCount = xmlTextReaderAttributeCount(reader);

//printf("fetch xml attributes %d\n",attrCount);
	ctxt->atcount	= 0;
	for (i = 0; i < attrCount && i < XOML_MAX_ATTR ; i++) 
	{
		xmlChar	*aqname	= NULL;
		xmlChar	*avalue	= NULL;

		xmlTextReaderMoveToAttributeNo(reader,i);
		aqname	= xmlTextReaderName(reader);
		avalue	= xmlTextReaderValue(reader);

		ctxt->atname[ctxt->atcount]	= (char *)aqname;
		ctxt->atvalue[ctxt->atcount]	= (char *)avalue;
		ctxt->atcount++;
	}
}

static	void	IgnoreTrace()
{
}


/*DEF*/void *XoReadXmlReader(/*xmlTextReaderPtr*/ void *preader,char *roottype,int flags,int *err)
{
	xmlTextReaderPtr	reader = (xmlTextReaderPtr)preader;
	int 		next;
	t_ctxt		ctxt;
	int		ret;

	*err	= 0;
	memset	(&ctxt,0,sizeof(ctxt));
	ctxt.roottype	= roottype;
	ctxt.flags	= flags;
	ctxt.ns.flags	= flags;
	_XoAllocCtxt(&ctxt);

	if ( C_PARSE_TARGET(&ctxt) )
	{
		void	*r;
		char	*tag	= "<dont_save_this_tag>";

		r	= _XoRootPush(NULL,&ctxt,NULL,tag,err);
		if (!r) 
		{
			_XoFreeCtxt(&ctxt);
			return	NULL;
		}
		_XoSetElementName(r,tag);
		*err	= 0;
	}

	if ( !C_PARSE_TRACE(&ctxt) )
		xmlTextReaderSetErrorHandler(reader,IgnoreTrace,NULL);
	ret	= 1;
	next = xmlTextReaderRead(reader);
	while (next == 1 && ret > 0) 
	{
		ctxt.depth	= xmlTextReaderDepth(reader);
		ctxt.nodetype	= xmlTextReaderNodeType(reader);
		ctxt.qname	= (char *)xmlTextReaderName(reader);
		ctxt.value	= (char *)xmlTextReaderValue(reader);
		switch	(ctxt.nodetype)
		{
		case	XML_READER_TYPE_ELEMENT :
			ctxt.isempty	= xmlTextReaderIsEmptyElement(reader);
			ctxt.atcount	= xmlTextReaderAttributeCount(reader);
			FetchAttributes(reader,&ctxt);
		break;
		case	XML_READER_TYPE_TEXT :
		break;
		}

		ret	= _XoProcessNode(reader,&ctxt);

		if	(ctxt.atcount)
			_XoFreeAttributesWith(&ctxt,xmlFree);
		if (ctxt.qname)
			xmlFree(ctxt.qname);
		if (ctxt.value)
			xmlFree(ctxt.value);

		if (ret < 0) 
		{
			ctxt.line = xmlTextReaderGetParserLineNumber(reader);
			goto errload;
		}
		next = xmlTextReaderRead(reader);

		if ( C_PARSE_INTERACTIF(&ctxt) ) 
		{
			int	c;

			switch(ctxt.prevnode)
			{
			case	XML_READER_TYPE_ELEMENT :
			case	XML_READER_TYPE_TEXT :
			case	XML_READER_TYPE_END_ELEMENT :
				_XoDumpStack(&ctxt);
				fflush(stdin);
				c = getchar();
				if	(c == 'p')
				{
					void	*o;
					o = ctxt.obj[ctxt.level-1];
					XoSaveAscii(o,stdout);
				}
			break;
			}
		}
	}
	if (next != 0)	// parsing error, get line number
	{
		*err	= xmlTextReaderGetParserLineNumber(reader);
	}

	if (next != 0) 
	{
		if ( C_PARSE_TRACE(&ctxt) ) 
		XOML_TRACE("XoReadXmlReader() : failed to parse next=%d ret=%d line=%d\n", 
				next,ret,*err);
		if	(ctxt.obj[0])
			XoFree(ctxt.obj[0],1);
		_XoFreeCtxt(&ctxt);
		return	NULL;
	}
errload	:
	if (ret < 0) 
	{
		*err	= ret;
		if ( C_PARSE_TRACE(&ctxt) ) 
		XOML_TRACE("XoReadXmlReader() : failed to load next=%d ret=%d line=%d\n", 
				next,ret,ctxt.line);
		if	(ctxt.obj[0])
			XoFree(ctxt.obj[0],1);
		_XoFreeCtxt(&ctxt);
		return	NULL;
	}

	_XoFreeCtxt(&ctxt);
	return	ctxt.obj[0];
}

/*DEF*/void *XoReadXmlEx(char *filename,char *roottype,int flags,int *err)
{
	xmlTextReaderPtr reader	= NULL;
	void		 *obj;

	*err	= 0;
	reader = xmlNewTextReaderFilename(filename);
	if (reader == NULL) 
	{
		char	tmperr[256];
		sprintf(tmperr,"XoReadXmlEx(%s) : cannot create reader file\n", 
								filename);
		XOERR(tmperr,0);
		return	NULL;
	}
	obj	= XoReadXmlReader(reader,roottype,flags,err);
	xmlFreeTextReader(reader);
	return	obj;
}

/*DEF*/void *XoReadXmlMem(char *buf,int sz,char *roottype,int flags,int *err)
{
	xmlTextReaderPtr reader	= NULL;
	void		 *obj;

	*err	= 0;
	reader = xmlReaderForMemory(buf,sz,"",NULL,0);
	if (reader == NULL) 
	{
		char	tmperr[256];
		sprintf(tmperr,"XoReadXmlMem():cannot create reader memory\n");
		XOERR(tmperr,0);
	}
	obj	= XoReadXmlReader(reader,roottype,flags,err);
	xmlFreeTextReader(reader);
	return	obj;
}
