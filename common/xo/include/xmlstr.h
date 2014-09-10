
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


#if 0
#define PARSE_DEBUG
#endif

/*
	reader (pull parser) :

	node types :
		XML_READER_TYPE_ELEMENT
		XML_READER_TYPE_END_ELEMENT
		XML_READER_TYPE_TEXT

	functions :
		xmlNewTextReaderFilename
		xmlReaderForMemory
		xmlFreeTextReader

		xmlTextReaderRead	// pull node
		xmlTextReaderNodeType
		xmlTextReaderName
		xmlTextReaderValue
		xmlTextReaderGetParserLineNumber
		xmlTextReaderDepth
		xmlTextReaderHasValue
		xmlTextReaderIsEmptyElement
		xmlTextReaderAttributeCount
		xmlTextReaderMoveToAttributeNo
		xmlTextReaderSetErrorHandler

		xmlFree


	writer :
		functions :
		xmlNewTextWriterFilename
		xmlNewTextWriterMemory
		xmlFreeTextWriter

		xmlTextWriterStartDocument
		xmlTextWriterEndDocument
		xmlTextWriterStartElement
		xmlTextWriterEndElement
		xmlTextWriterWriteElement
		xmlTextWriterWriteAttribute
		xmlTextWriterWriteString

		xmlBufferCreate
		xmlBufferFree



 */

typedef	struct
{
	char	*ns_userprf;	// user prefixe
	char	*ns_pivotprf;	// prefixe pivot
	char	*ns_uri;	// uri value for namespace declaration
}	t_ns;

typedef	struct
{
	int		flags;
	t_ns		*ns;	// ns[]
	int		nbns;
	char		*nsdefault;
	int		nsnumdef;
}	t_nsctxt;


typedef	struct
{
	int		flags;
	char		*roottype;	// may be NULL
	int		prevnode;
	int		level;
	int		skipdepth;
	int		line;
	void		*cnt[XOML_MAX_LEVEL+1];
	void		*obj[XOML_MAX_LEVEL+1];
	t_attribut_ref	*ar[XOML_MAX_LEVEL+1];
	char		waittype[XOML_MAX_LEVEL+1];
	int		atcount;
	char		*atname[XOML_MAX_ATTR];
	char		*atvalue[XOML_MAX_ATTR];

	// store xmlTextReaderXXXX() results
	int		nodetype;
	int		depth;
	int		isempty;
	char		*qname;
	char		*value;
#ifdef PARSE_DEBUG
	char		waitstr[XOML_MAX_LEVEL+1][XOML_TAG_LG];
#endif

	t_nsctxt	ns;
}	t_ctxt;

#ifdef	XO_XML_READER_EXI
/*
 * EXI does not define node types but xo parser needs them.
 * We define the same types as Xml Text Reader
 */
typedef enum {
    XML_READER_TYPE_NONE = 0,
    XML_READER_TYPE_ELEMENT = 1,
    XML_READER_TYPE_ATTRIBUTE = 2,
    XML_READER_TYPE_TEXT = 3,
    XML_READER_TYPE_CDATA = 4,
    XML_READER_TYPE_ENTITY_REFERENCE = 5,
    XML_READER_TYPE_ENTITY = 6,
    XML_READER_TYPE_PROCESSING_INSTRUCTION = 7,
    XML_READER_TYPE_COMMENT = 8,
    XML_READER_TYPE_DOCUMENT = 9,
    XML_READER_TYPE_DOCUMENT_TYPE = 10,
    XML_READER_TYPE_DOCUMENT_FRAGMENT = 11,
    XML_READER_TYPE_NOTATION = 12,
    XML_READER_TYPE_WHITESPACE = 13,
    XML_READER_TYPE_SIGNIFICANT_WHITESPACE = 14,
    XML_READER_TYPE_END_ELEMENT = 15,
    XML_READER_TYPE_END_ENTITY = 16,
    XML_READER_TYPE_XML_DECLARATION = 17
} xmlReaderTypes;
#endif

#define	BALISE_ATTR	XOML_BALISE_ATTR	// "name$"
#define	PSEUDO_ATTR	XOML_PSEUDO_ATTR	// "value$"
#define	TYVOID_ATTR	XOML_TYVOID_ATTR	// type
#define	WRAPPE_ATTR	XOML_WRAPPE_ATTR	// "wrapper$" for obix objects	

#define	END_ROOT	1
#define	END_STEXT	2 // single type extenion END_OBJECT like, not used
#define	END_OBJECT	3
#define	END_LIST	4
#define	END_ATTR	5
#define	END_WRAPPER	6

#define	PARSE_ONLY		XOML_PARSE_ONLY
#define	PARSE_INTERACTIF	XOML_PARSE_INTERACTIF
#define	PARSE_TRACE		XOML_PARSE_TRACE
#define	PARSE_TARGET		XOML_PARSE_TARGET
#define	PARSE_OBIX		XOML_PARSE_OBIX
#define	PARSE_NATIVENS		XOML_PARSE_NATIVENS
#define	PARSE_RETRYATTR		XOML_PARSE_RETRYATTR
#define	PARSE_AUTOWRAPP		XOML_PARSE_AUTOWRAPP

#define	C_PARSE_ONLY(ctxt)		(((ctxt)->flags & PARSE_ONLY) == \
						PARSE_ONLY)
#define	C_PARSE_INTERACTIF(ctxt)	(((ctxt)->flags & PARSE_INTERACTIF) == \
						PARSE_INTERACTIF)
#define	C_PARSE_TRACE(ctxt)		(((ctxt)->flags & PARSE_TRACE) == \
						PARSE_TRACE)
#define	C_PARSE_TARGET(ctxt)		(((ctxt)->flags & PARSE_TARGET) == \
						PARSE_TARGET)

#define	C_PARSE_OBIX(ctxt)		(((ctxt)->flags & PARSE_OBIX) == \
						PARSE_OBIX)

#define	C_PARSE_NATIVENS(ctxt)		(((ctxt)->flags & PARSE_NATIVENS) == \
						PARSE_NATIVENS)
#define	C_PARSE_RETRYATTR(ctxt)		(((ctxt)->flags & PARSE_RETRYATTR) == \
						PARSE_RETRYATTR)

#define	C_PARSE_AUTOWRAPP(ctxt)		(((ctxt)->flags & PARSE_AUTOWRAPP) == \
						PARSE_AUTOWRAPP)

#define	XOML_TRACE(...) \
{\
	fprintf(stderr,__VA_ARGS__);\
	fflush(stderr);\
}


void	*_XoAllocNameSpace(t_nsctxt *nsctxt,int nb);
void	_XoFreeNameSpace(t_nsctxt *nsctxt);
int	_XoFindCfgNameSpace(char *uri);
char	*_XoFindUriNameSpace(char *pref);
char	*_XoFindPrfNameSpace(char *uri);
int32	_XoAddNameSpace(void *reader,int attrCount,t_nsctxt *nsctxt,char *pname,char *uri);
char	*_XoDoName(t_nsctxt *nsctxt,char *aqname,char *avalue,char *dst);



void	_XoAllocCtxt(t_ctxt *ctxt);
void	_XoFreeCtxt(t_ctxt *ctxt);
void	_XoDumpStack(t_ctxt *ctxt);
void *_XoRootPush(void *reader,t_ctxt *ctxt,char *nametype,char *balise,int *err);
void	_XoFreeAttributesWith(t_ctxt *ctxt,void (*fct)(void *));
void	_XoFreeAttributes(t_ctxt *ctxt);
int32	_XoSetBasicValue(void *curObj,t_attribut_ref *curAtr,char **value,int plg);
void	_XoSetElementName(void *obj,char *balise);
int	_XoProcessNode(void *reader,t_ctxt *ctxt);
