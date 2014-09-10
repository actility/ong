
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

static	char	*xo_file = __FILE__;

/*DEF*/void *XoReadObixEx(char *filename,void *unused,int flags,int *err)
{
	xmlTextReaderPtr reader	= NULL;
	void		 *obj;

	flags	= flags | XOML_PARSE_OBIX;
	*err	= 0;
	reader = xmlNewTextReaderFilename(filename);
	if (reader == NULL) 
	{
		char	tmperr[256];
		sprintf(tmperr,"XoReadObixEx(%s):cannot create reader file\n",
								filename);
		XOERR(tmperr,0);
		return	NULL;
	}
	obj	= XoReadXmlReader(reader,NULL,flags,err);
	xmlFreeTextReader(reader);
	return	obj;
}

/*DEF*/void *XoReadObixMem(char *buf,int sz,void *unused,int flags,int *err)
{
	xmlTextReaderPtr reader	= NULL;
	void		 *obj;

	flags	= flags | XOML_PARSE_OBIX;
	*err	= 0;
	reader = xmlReaderForMemory(buf,sz,"",NULL,0);
	if (reader == NULL) 
	{
		char	tmperr[256];
		sprintf(tmperr,"XoReadObixMem():cannot create reader memory\n");
		XOERR(tmperr,0);
		return	NULL;
	}
	obj	= XoReadXmlReader(reader,NULL,flags,err);
	xmlFreeTextReader(reader);
	return	obj;
}

