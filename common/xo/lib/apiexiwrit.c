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

/**
 * @file apiexiwrit.c
 * @brief EXI encoder for XO
 */


#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

//#include <libxml/xmlwriter.h>

#include "xoapipr.h"

#include "xmlstr.h"
#include <rtlhtbl.h>
#include <rtllist.h>

#include <EXISerializer.h>
#include <stringManipulate.h>
#include <grammarGenerator.h>
#include "procTypes.h"
#include "grammars.h"
#include "sTables.h"

static int ExiSerialize(EXIStream *strm, QName qname, char *value, boolean isAttr, EXITypeClass t);

long base64_encode (char *to, const char *from, unsigned int len);
long base64_decode (char *to, const char *from, unsigned int len);
//EXIType getEXIType(QName qname);

typedef EXIStream *xmlTextWriterPtr;
typedef BinaryBuffer *xmlBufferPtr;
typedef void *xmlChar;
#define XCAST(x)	(const xmlChar *)((x))

const String NS_EMPTY_STR = {NULL, 0};

EXIPSchema *XoExiLoadSchema(char *schemaID);

//#define DEBUG

#define FWRITE fwrite


static char    *xo_file        = __FILE__;

#define PRINT(...) { \
  char msg[256]; \
  snprintf(msg, 256, __VA_ARGS__); \
  xoerr(xo_file, __LINE__, msg, 0); \
}


#define EXIP_VERSION 294


#if	0	// PCA
static char *CorrespTab[][2] = {
	{ "m2m",     "http://uri.etsi.org/m2m" },
	{ "o",       "http://obix.org/ns/schema/1.1" },
	{ "ong",     "http://uri.actility.com/m2m/ong" },
	{ "acy",     "http://uri.actility.com/m2m" },
	{ "xmime",   "http://www.w3.org/2005/05/xmlmime" },
	{ "xsi",     "http://www.w3.org/2001/XMLSchema-instance" },
	{ "xsd",     "http://www.w3.org/2001/XMLSchema" },
	{ NULL, NULL }
};
#endif

char *getNamespaceByPrefix(char *pfx) {
#if	0	// PCA
	int	i;
	for	(i=0; CorrespTab[i][0]; i++)
		if	(!strcmp(pfx, CorrespTab[i][0]))
			return	CorrespTab[i][1];
#endif
	char	*u;

	u	= _XoFindUriNameSpace(pfx);
	if	(u && *u)
		return	u;
	return	pfx;
}

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

static void writeString(String str) {
	PRINT("%s", str.str);
	// FWRITE(str.str, sizeof(CharType), str.length, stdout);
}

static void writeQName(QName qname) {
	PRINT("QName=(");
	writeString (*qname.uri);
	PRINT(":");
	writeString (*qname.localName);
	PRINT(")");
}

/*
static void copyString(String str, char *out) {
	memcpy (out, str.str, str.length);
	out[str.length] = 0;
}
*/


typedef struct {
	char	*data;
	int	len;
} memoryStream_t;

static size_t writeMemoryOutputStream(void* buf, size_t readSize, void* stream) {
	PRINT("writeMemoryOutputStream %d\n", readSize);
	memoryStream_t *ms = (memoryStream_t *)stream;
	if	(!ms->data)
		ms->data	= malloc(readSize + 1);
	else
		ms->data	= realloc(ms->data, ms->len + readSize + 1);
	memcpy (ms->data + ms->len, buf, readSize);
	ms->len	+= readSize;
	ms->data[ms->len] = 0;
	return	readSize;
}

static size_t writeFileOutputStream(void* buf, size_t writeSize, void* stream) {
	FILE *outfile = (FILE*) stream;
	PRINT("writeFileOutputStream %d\n", writeSize);
	return fwrite(buf, 1, writeSize, outfile);
}


#define OUTPUT_BUFFER_SIZE 1024

/**
 * @brief Create a buffer (libxml2 compatibility function)
 * @return A buffer
 */
static xmlBufferPtr xmlBufferCreate() {
	BinaryBuffer	*buffer	= calloc(sizeof(BinaryBuffer), 1);
	buffer->buf = malloc(OUTPUT_BUFFER_SIZE);
	buffer->bufLen = OUTPUT_BUFFER_SIZE;
	buffer->bufContent = 0;
	return	buffer;
}

/**
 * @brief Free a buffer (libxml2 compatibility function)
 * @param buf A buffer 
 * @return void
 */
static void xmlBufferFree(xmlBufferPtr buf) {
	if	(buf->ioStrm.readWriteToStream == writeMemoryOutputStream) {
		memoryStream_t *outMemory = (memoryStream_t *)buf->ioStrm.stream;
		if	(outMemory->data)
			free (outMemory->data);
		free (outMemory);
	}
	free (buf->buf);
	free (buf);
}

/**
 * @brief From apiexischema.c
 */
extern EXIPSchema *g_schemaPtr;

/**
 * @brief Create a Memory Writer (libxml2 compatibility function)
 * @param buf A buffer 
 * @param compression Unused
 * @return A xmlTextWriterPtr object
 */
static xmlTextWriterPtr xmlNewTextWriterMemory(xmlBufferPtr buf, int compression, char *schemaID) {
	EXIStream	*stream;
	memoryStream_t	*outMemory;

	stream	= calloc(sizeof(EXIStream), 1);
	outMemory = calloc(sizeof(memoryStream_t), 1);

	// I: First initialize the header of the stream
	serialize.initHeader(stream);

	// II: Set any options in the header, if different from the defaults
	//stream->header.has_cookie = TRUE;
	stream->header.has_options = TRUE;
	//stream->header.opts.valueMaxLength = 300;
	//stream->header.opts.valuePartitionCapacity = 50;

//	SET_STRICT(stream->header.opts.enumOpt);
	SET_PRESERVED(stream->header.opts.preserve, PRESERVE_PREFIXES);
//	SET_ALIGNMENT(stream->header.opts.enumOpt, BYTE_ALIGNMENT);

	// III: Define an external stream for the output if any
	buf->ioStrm.readWriteToStream = writeMemoryOutputStream;
	buf->ioStrm.stream = outMemory;

	// IV: Initialize the stream
#if EXIP_VERSION==276
	if	(schemaID && *schemaID) {
		XoExiLoadSchema (schemaID);
		String s;
		asciiToString(schemaID, &s, &stream->memList, FALSE);
		serialize.initStream(stream, *buf, g_schemaPtr, SCHEMA_ID_SET, &s);
	}
	else {
		serialize.initStream(stream, *buf, g_schemaPtr, SCHEMA_ID_ABSENT, NULL);
	}
#endif
#if EXIP_VERSION>=289
	if	(schemaID && *schemaID) {
		XoExiLoadSchema (schemaID);
		asciiToString(schemaID, &stream->header.opts.schemaID, &stream->memList, FALSE);
		stream->header.opts.schemaIDMode = SCHEMA_ID_SET;
	}
	else {
		stream->header.opts.schemaIDMode = SCHEMA_ID_ABSENT;
	}
	serialize.initStream(stream, *buf, g_schemaPtr);
#endif

	// V: Start building the stream step by step: header, document, element etc...
	serialize.exiHeader(stream);

	return	(xmlTextWriterPtr)stream;
}

/**
 * @brief Create a File Writer (libxml2 compatibility function)
 * @param uri Filename
 * @param compression Unused
 * @return A xmlTextWriterPtr object
 */
static xmlTextWriterPtr xmlNewTextWriterFilename(const char *uri, int compression, char *schemaID) {
	EXIStream	*stream;
	xmlBufferPtr buf = xmlBufferCreate();

	FILE *outfile = fopen(uri, "wb" );
	if(!outfile)
		return	NULL;

	stream	= calloc(sizeof(EXIStream), 1);

	// I: First initialize the header of the stream
	serialize.initHeader(stream);

	// II: Set any options in the header, if different from the defaults
	//stream->header.has_cookie = TRUE;
	stream->header.has_options = TRUE;
	//stream->header.opts.valueMaxLength = 300;
	//stream->header.opts.valuePartitionCapacity = 50;

//	SET_STRICT(stream->header.opts.enumOpt);
	SET_PRESERVED(stream->header.opts.preserve, PRESERVE_PREFIXES);
//	SET_ALIGNMENT(stream->header.opts.enumOpt, BYTE_ALIGNMENT);

	// III: Define an external stream for the output if any
	buf->ioStrm.readWriteToStream = writeFileOutputStream;
	buf->ioStrm.stream = outfile;

	// IV: Initialize the stream
#if EXIP_VERSION==276
	if	(schemaID && *schemaID) {
		XoExiLoadSchema (schemaID);
		String s;
		asciiToString(schemaID, &s, &stream->memList, FALSE);
		serialize.initStream(stream, *buf, g_schemaPtr, SCHEMA_ID_SET, &s);
	}
	else {
		serialize.initStream(stream, *buf, g_schemaPtr, SCHEMA_ID_ABSENT, NULL);
	}
#endif
#if EXIP_VERSION>=289
	if	(schemaID && *schemaID) {
		XoExiLoadSchema (schemaID);
		asciiToString(schemaID, &stream->header.opts.schemaID, &stream->memList, FALSE);
		stream->header.opts.schemaIDMode = SCHEMA_ID_SET;
	}
	else {
		stream->header.opts.schemaIDMode = SCHEMA_ID_ABSENT;
	}
	serialize.initStream(stream, *buf, g_schemaPtr);
#endif

	// V: Start building the stream step by step: header, document, element etc...
	serialize.exiHeader(stream);

	return	(xmlTextWriterPtr)stream;
}

static void xmlFreeTextWriter(xmlTextWriterPtr writer) {
/*
	if	(writer->buffer.ioStrm.readWriteToStream == writeMemoryOutputStream) {
		memoryStream_t *outMemory = (memoryStream_t *)writer->buffer.ioStrm.stream;
		if	(outMemory->data)
			free (outMemory->data);
		free (outMemory);
	}
	else if	(writer->buffer.ioStrm.readWriteToStream == writeFileOutputStream) {
		fclose ((FILE *)writer->buffer.ioStrm.stream);
		xmlBufferFree(&writer->buffer);
	}
*/
	if	(writer->buffer.ioStrm.readWriteToStream == writeFileOutputStream)
		fclose ((FILE *)writer->buffer.ioStrm.stream);
	free (writer);
}

/**
 * @brief Contexts pile. At every SE, pushes its name and type.
 * At every EE, pops.
 */
typedef struct {
	struct list_head head;
	char SE[200];
	EXITypeClass valueType;
} t_elt;

struct list_head pile;

static void pile_init() {
	INIT_LIST_HEAD (&pile);
}

static void push (char *SE, EXITypeClass t) {
	t_elt *e = calloc(sizeof(t_elt), 1);
	if	(e) {
		strcpy (e->SE, SE);
		e->valueType	= t;
		list_add(&e->head, &pile);
	}
}

static void pop() {
	t_elt *e = list_entry(pile.next, t_elt, head);
	if	(e) {
		list_del(&e->head);
		free (e);
	}
}

static t_elt *currentContext() {
	return list_entry(pile.next, t_elt, head);
}

/**
 * @brief Starts a document (libxml2 compatibility function)
 * @param writer EXI stream returned by xmlNewTextWriterMemory or xmlNewTextWriterFilename
 * @param version unused
 * @param encoding unused
 * @param standalone unused
 * @return 0 if success, -1 else
 */
static int xmlTextWriterStartDocument(EXIStream *stream, char *p1, char *p2, char *p3) {
	pile_init ();

	PRINT("serialize.startDocument\n");
	if	(serialize.startDocument(stream))
		return	-1;

	if	(g_schemaPtr) {
		//printf ("SD schemaPtr\n");
/*
		String ns = { "http://uri.etsi.org/m2m", 23 };
		String pfx = { "m2m", 3 };
		errorCode err = serialize.namespaceDeclaration(stream, ns, pfx, 1);
		printf ("namespaceDeclaration ret=%d\n", err);
*/
	}
	return	0;
}

/**
 * @brief Ends a document (libxml2 compatibility function)
 * @param writer EXI stream
 * @return 0 if success, -1 else
 */
static int xmlTextWriterEndDocument(xmlTextWriterPtr writer) {
	PRINT("serialize.endDocument\n");
	if	(serialize.endDocument(writer))
		return	-1;
	if	(serialize.closeEXIStream(writer))
		return	-1;
	//dumpSchema (g_schemaPtr);
	return	0;
}

static int xmlTextWriterSetIndent(xmlTextWriterPtr writer, int indent) {
	return	0;
}

char g_NsDefault[100] = "";

/**
 * @brief Starts an EXI element (libxml2 compatibility function)
 * @param writer An EXIStream
 * @param elem An element name
 * @return 0 if success, -1 if error
 */
static int xmlTextWriterStartElement(xmlTextWriterPtr writer, char *elem) {
	String uri = {NULL,0};
	String localName = {NULL,0};
	QName qname = {&uri, &localName, NULL };
	int	ret = 0;

	//printf ("SE %s\n", elem);

	char *work	= strdup(elem);
	char *pfx;
	char *lname = strchr(work, ':');
	if	(lname) {
		pfx	= work;
		*lname++	= 0;
		pfx	= getNamespaceByPrefix(pfx);
		uri.str	= pfx;
		uri.length = strlen(pfx);
	}
	else {
		//pfx	= "";
/* XXX
		uri.str	= g_NsDefault;
		uri.length = strlen(g_NsDefault);
*/
		lname	= work;
	}

	localName.str	= lname;
	localName.length = strlen(lname);

	PRINT("serialize.startElement ");
	writeQName(qname);
	PRINT("\n");

	EXITypeClass valueType;
	ret = serialize.startElement(writer, qname, &valueType);
	push (elem, valueType);
	free (work);

	if	(ret) {
		printf ("serialize.startElement ret=%d '%s' '%s'\n", ret,elem,lname);
		return	-1;
	}
	return	0;
}

/**
 * @brief Ends an EXI element (libxml2 compatibility function)
 * @param writer An EXIStream
 * @return 0 if success, -1 if error
 */
static int xmlTextWriterEndElement(xmlTextWriterPtr writer) {
	pop ();
	PRINT("serialize.endElement\n");
	if (serialize.endElement(writer))
		return	-1;
	return	0;
}

void cutQName(char *str, char *uri, char *ln) {
	char *pt = strchr(str, ':');
	if (pt) {
		int sz = pt - str;
		strncpy (uri, str, sz);
		uri[sz] = 0;
		strcpy (ln, pt+1);
	}
	else {
		*uri = 0;
		strcpy (ln, str);
	}

	char *ns = getNamespaceByPrefix (uri);
	if	(ns)
		strcpy (uri, ns);
}

void serializeXSITYPE(EXIStream *strm, char *value) {
	char	s_uri[100], s_ln[100];
	String	uri;
	String	ln;
	QName	qxsi	= {&uri,&ln};

	PRINT("serialize.qnameData %s\n", value);

	cutQName (value, s_uri, s_ln);
	asciiToString(s_uri, &uri, &strm->memList, FALSE);
        asciiToString(s_ln, &ln, &strm->memList, FALSE);
	serialize.qnameData(strm, qxsi);
}

/**
 * @brief Writes an attribute (libxml2 compatibility function)
 * @param[in] writer An EXIStream
 * @param[in] name Attribute name
 * @param[in] value Attribute value
 * @return 0 if success, -1 if error
 */
static int xmlTextWriterWriteAttribute(xmlTextWriterPtr writer, char*name, char *value) {
	String uri = {NULL,0};
	String localName = {NULL,0};
	QName qname = {&uri, &localName, NULL };
	String chVal;
	int	ret = 0;
	int	type = 0;

	PRINT("AT %s=%s\n", name, value);

	if	(strcmp(name,"xsi:type") == 0)
	{
		type	= VALUE_TYPE_QNAME_CLASS;
	}
	else if (g_schemaPtr && strstr(name, "xmlns"))
		return 0;

	name	= strdup(name);
	char *pfx;
	char *lname = strchr(name, ':');
	if	(lname) {
		pfx	= name;
		*lname++	= 0;
		uri.str	= getNamespaceByPrefix(pfx);
		uri.length = strlen(uri.str);
	}
	else {
		pfx	= "";
/* XXX
		uri.str	= g_NsDefault;
		uri.length = strlen(g_NsDefault);
*/
		lname	= name;
	}

	localName.str	= lname;
	localName.length = strlen(lname);

	if	(g_schemaPtr) {
		if	(!strcmp(lname, "xmlns")) {
			strcpy (g_NsDefault, value);
/*
			String ns = { value, strlen(value) };
			errorCode err = serialize.namespaceDeclaration(writer, ns, localName, 0);
			printf ("serialize.namespaceDeclaration(%s , %s)\n", value, lname);
*/
			//XoAddNameSpace(lname, value);
		}
		else {
			ret = ExiSerialize(writer, qname, value, TRUE, type);
		}
	}
	else {
		if      (!strcmp(pfx, "xmlns")) {
			PRINT("serialize.namespaceDeclaration ");
			writeQName(qname);
                        PRINT("\n");

                        String ns = { value, strlen(value) };
			ret += serialize.namespaceDeclaration(writer, ns, localName, TRUE);
                        if (ret) {
                                free(name);return -1;
                        }
		}
		else {
			EXITypeClass valueType;

			PRINT("serialize.attribute ");
			writeQName(qname);
			PRINT("\n");

			ret += serialize.attribute(writer, qname, TRUE, &valueType);
			if (ret) {
				free(name);return -1;
			}

			// xsi:type
			if	(type == VALUE_TYPE_QNAME_CLASS) {
				serializeXSITYPE(writer, value);
			}
			else {
				ret += asciiToString((char *)value, &chVal, &writer->memList, FALSE);
				PRINT("serialize.stringData (%s)\n", value);
				ret += serialize.stringData(writer, chVal);
			}
		}
	}
	free (name);
	if	(ret) return	-1;
	return	0;
}

/**
 * @brief Writes a string as an element's content (libxml2 compatibility function)
 * @param writer An EXIStream
 * @param content String
 * @return 0 if success, -1 if error
 */
static int xmlTextWriterWriteString(xmlTextWriterPtr writer, const xmlChar *content) {
	int	ret = 0;
	PRINT("CH [%s] (%d)\n", (char *)content, strlen((char *)content));

	if	(g_schemaPtr) {
		String uri = {NULL,0};
		String localName = {NULL,0};
		QName qname = {&uri, &localName, NULL };

		ret = ExiSerialize(writer, qname, (char *)content, FALSE, currentContext()->valueType);
	}
	else
	{
		String chVal;
		ret += asciiToString((char *)content, &chVal, &writer->memList, FALSE);
		PRINT("serialize.stringData (%s)\n", (char *)content);
		ret += serialize.stringData(writer, chVal);
	}
	if	(ret)	return -1;
	return	0;
}

/**
 * @brief Writes a buffer in base64, as an element's content (libxml2 compatibility function)
 * @param writer An EXIStream
 * @param data binary data
 * @param start the position within the data of the first byte to encode
 * @param len the number of bytes to encode
 * @return 0 if success, -1 if error
 */
static int xmlTextWriterWriteBase64(xmlTextWriterPtr writer,
	const char *data, int start, int len) {
/*
	char *to = malloc(len*2);
	base64_encode (to, data+start, len);
	int rc = xmlTextWriterWriteString(writer, XCAST(to));
	free (to);
*/
	PRINT("serialize.binaryData\n");
	serialize.binaryData(writer, data+start, len);
	return	0;
}

/**
 * @brief Converts a value into its mantissa and base10 exponent
 * @param[in] val The value to convert
 * @param[out] exponent The base10 exponent
 * @return The mantissa
 */
static int64_t normalize_float(char *val, int16_t *exponent) {
	char *pt;
	*exponent = 0;

	// 1. Removes zeroes at left
	for (pt=val; *pt; pt++) {
		if	(*pt != '0')
			break;
	}
	val	= pt;

	// 2. Removes zeroes at right
	for (pt = val+strlen(val)-1; pt >= val; pt--) {
		if	(*pt != '0')
			break;
		*pt	= 0;
	}

	// 3. Counts digits at right
	pt	= strchr(val, '.');
	if	(pt) {
		int cnt = 1;
		while	(*pt) {
			*pt	= *(pt+1);
			pt++;
			cnt --;
		}
		*exponent = cnt;
	}

	return	atoll(val);
}

#include <math.h>

/**
 * @brief Converts an ISO date like 2012-12-14T15:00:17.717Z
 *                                  012345678901234567890123
 * @param iso An ISO date string
 * @param dt The resulting IEC DT
 * @return void
 */

#define BASE_YEAR 2000

void iso8601_to_EXIP(char *iso, EXIPDateTime *dt) {
	if	(!iso || strlen(iso) < 17)
		return;

	memset (dt, 0, sizeof(EXIPDateTime));

	dt->presenceMask = 0;
	dt->dateTime.tm_year	= (iso[0]-'0')*1000 + (iso[1]-'0')*100 + (iso[2]-'0')*10 + (iso[3]-'0') - BASE_YEAR;
	dt->dateTime.tm_mon	= (iso[5]-'0')*10 + (iso[6]-'0') - 1;
	dt->dateTime.tm_mday	= (iso[8]-'0')*10 + (iso[9]-'0');
	dt->dateTime.tm_hour	= (iso[11]-'0')*10 + (iso[12]-'0');
	dt->dateTime.tm_min	= (iso[14]-'0')*10 + (iso[15]-'0');
	dt->dateTime.tm_sec	= (iso[17]-'0')*10 + (iso[18]-'0');

	int type;
	char *pt = &iso[19];
	if	(!*pt)
		return;

	if	(*pt == '.') {
		char *frac = ++pt;
		while (isdigit(*pt)) {
			pt ++;
		}
		type = *pt;
		*pt = 0;
		pt ++;
		dt->fSecs.value = atoi(frac);
		dt->fSecs.offset = strlen(frac) - 1;
		dt->presenceMask |= FRACT_PRESENCE;
	}
	else {
		type = *pt++;
	}

	if	(type == '+' || type == '-') {
		int offset;
		int sign = (type == '-') ? -1 : 1;

		dt->TimeZone	= (pt[0]-'0')*10 + (pt[1]-'0');
		dt->TimeZone	<<= 6;
		dt->TimeZone	+= (pt[3]-'0')*10 + (pt[4]-'0');
		dt->TimeZone	*= sign;
		dt->presenceMask |= TZONE_PRESENCE;
	}
}


void EXIP_to_iso8601(EXIPDateTime *dt, char *avalue) {
	*avalue = 0;
	sprintf (avalue+strlen(avalue), "%04d-%02d-%02d",
		dt->dateTime.tm_year + BASE_YEAR,
		dt->dateTime.tm_mon + 1,
		dt->dateTime.tm_mday);
	
	if	(dt->presenceMask & FRACT_PRESENCE) {
		double sec = dt->dateTime.tm_sec;
		sec += (double)dt->fSecs.value * pow(10, -(dt->fSecs.offset + 1));
		sprintf (avalue+strlen(avalue), "T%02d:%02d:%05.3f",
			dt->dateTime.tm_hour,
			dt->dateTime.tm_min, sec);
	}
	else {
		sprintf (avalue+strlen(avalue), "T%02d:%02d:%02d",
			dt->dateTime.tm_hour,
			dt->dateTime.tm_min, dt->dateTime.tm_sec);
	}
	
	if	(dt->presenceMask & TZONE_PRESENCE) {
		int h, m, tz;
		char sign;

		sign	= dt->TimeZone >= 0 ? '+' : '-';
		tz	= abs(dt->TimeZone);
		h	= tz >> 6;
		m	= tz & 0x3f;
		sprintf (avalue+strlen(avalue), "%c%02d:%02d", sign, h, m);
	}
	else {
		strcat (avalue, "Z");
	}
}


static int ExiSerialize(EXIStream *strm, QName qname, char *value, boolean isAttr, EXITypeClass t) {
	EXITypeClass valueType = VALUE_TYPE_STRING_CLASS;
	errorCode tmp_err_code = EXIP_OK;
	String chVal;

	if	(isAttr) {
		PRINT("serialize.attribute ");
		writeQName(qname);
		PRINT("\n");
		tmp_err_code = serialize.attribute(strm, qname, TRUE, &valueType);
		if (tmp_err_code)
			return -1;

		// xsi:type
		if	(t == VALUE_TYPE_QNAME_CLASS) {
			serializeXSITYPE(strm, value);
			return 0;
		}
	}
	else {
		valueType	= t;
	}

	switch (valueType) {
	case VALUE_TYPE_UNTYPED_CLASS :
	case VALUE_TYPE_STRING_CLASS :
	case VALUE_TYPE_NONE_CLASS :
		tmp_err_code += asciiToString(value, &chVal, &strm->memList, FALSE);
		PRINT("serialize.stringData (%s)\n", value);
		tmp_err_code += serialize.stringData(strm, chVal);
		break;
	case VALUE_TYPE_INTEGER_CLASS :
		PRINT("serialize.intData\n");
		tmp_err_code += serialize.intData(strm, atoll(value));
		break;
	case VALUE_TYPE_BOOLEAN_CLASS :
		{
		boolean b = FALSE;
		if	(!strcasecmp(value, "true") || !strcmp(value, "1"))
			b	= TRUE;
		PRINT("serialize.booleanData\n");
		tmp_err_code += serialize.booleanData(strm, b);
		break;
		}
	case VALUE_TYPE_DECIMAL_CLASS :
#if	0
		PRINT("serialize.decimalData\n");
		tmp_err_code += serialize.decimalData(strm, (Decimal)atof(value));
#else
		printf("XO/EXI Type Decimal not supported '%s':%d\n",
					__FILE__,__LINE__);
#endif
		break;
	case VALUE_TYPE_FLOAT_CLASS :
		{
		Float	fl_val;
		fl_val.mantissa = normalize_float(value, &fl_val.exponent);
		PRINT("serialize.floatData\n");
		tmp_err_code += serialize.floatData(strm, fl_val);
		}
		break;
	case VALUE_TYPE_BINARY_CLASS :
//		tmp_err_code += serialize.binaryData(&testStrm, SOME_BINARY_DATA, 10);
		break;
	case VALUE_TYPE_DATE_TIME_CLASS :
		{
		char *temp = strdup(value);
		EXIPDateTime dt;
		iso8601_to_EXIP(temp, &dt);
		free (temp);
		PRINT("serialize.dateTimeData %s\n", value);
		tmp_err_code += serialize.dateTimeData(strm, dt);
		}
		break;
	case VALUE_TYPE_LIST_CLASS :
		break;
	case VALUE_TYPE_QNAME_CLASS :
		break;
	default :
		break;
	}
	if (tmp_err_code)
		return -1;
	return 0;
}

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
	if	(xmlTextWriterStartElement(writer,nameattr) < 0)
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

static int32	xowritattr(xmlTextWriterPtr writer,t_wctxt *ctxt,void *obj,int32 numattr,int32 level)
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
			if (xmlTextWriterStartElement(writer,nameattr) < 0)
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
//printf ("%s %d / %s %s %s\n", __FILE__, __LINE__, __FUNCTION__, nameobj, tag);


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

		if	(xmlTextWriterStartElement(writer,tag) < 0)
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

		if (xmlTextWriterWriteAttribute(writer, "xmlns",uri) < 0)
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
					"xmlns",uri) < 0)
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
				nameattr,value) < 0)
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
				name,value) < 0)
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
			if	(xmlTextWriterWriteString(writer,XCAST(value)) < 0)
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

/**
 * @brief Encode a XO object in memory, using EXI
 * @param obj The XO object
 * @param flags
 * @param bufout output buffer
 * @param len output buffer length
 * @param schemaID Force schemaID
 * @param useRootObj Use the name of the rootobj as the schemaID
 * @return A pointer to the allocated memory structure to be freed
 */
/*DEF*/	void	*XoWritExiMem(void *obj,int32 flags,char **bufout,int *len,char *schemaID, int useRootObj)
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
/*
	if	(defns && *defns)
		strcpy(ctxt.defpref,defns);
*/
#if	0
	ret	= xowritobj(NULL,&ctxt,obj,0) ;
	if	(ret < 0)
	{
		return	NULL;
	}
	ctxt.passe++;
#endif

	if	(useRootObj) {
		int numbalise	= XoNumAttrInObj(obj,XOML_BALISE_ATTR);
		int lg;
		schemaID	= XoGetAttr(obj,numbalise,&lg);

		char *pt = strchr(schemaID, ':');
		if	(pt)
			schemaID	= pt+1;
/*
		if	(!strcmp(schemaID, "obj"))
			schemaID	= "xobix";
*/
	}

	if	((g_schemaPtr || useRootObj) && XoIsObix(obj) > 0) {
		strcpy (g_NsDefault, "http://obix.org/ns/schema/1.1");
		schemaID	= "xobix";
	}

	*bufout	= NULL;
	buf	= xmlBufferCreate();
	if	(!buf)
		return	NULL;
	writer	= xmlNewTextWriterMemory(buf, 0, schemaID);
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

	if	(g_schemaPtr && XoIsObix(obj) > 0) {
		strcpy (g_NsDefault, "http://obix.org/ns/schema/1.1");
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

	memoryStream_t *outMemory = (memoryStream_t *)writer->buffer.ioStrm.stream;
//printf ("outMemory-> %d\n", outMemory->len);
	*len = outMemory->len;
	*bufout	= (char *)outMemory->data;

	return	(char *)buf ;
}

/*DEF*/	void	XoWritExiFreeMem(void *buf)
{
	if (buf)
		xmlBufferFree((xmlBufferPtr)buf);
}

/*DEF*/	void	XoWritExiFreeCtxt(void *pbuf)
{
	xmlBufferPtr	buf	= (xmlBufferPtr)pbuf;
	if (buf)
	{
		//buf->content	= NULL;
		xmlBufferFree(buf);
	}
}

/**
 * @brief Encode a XO object in a file, using EXI
 * @param obj The XO object
 * @param flags
 * @param bufout Null terminated output buffer
 * @param schemaID Force schemaID
 * @param useRootObj Use the name of the rootobj as the schemaID
 * @return A pointer to the allocated memory structure to be freed
 */
/*DEF*/	int32	XoWritExiEx(char *filename,void *obj,int32 flags,char *schemaID, int useRootObj)
{
	xmlTextWriterPtr writer;
	t_wctxt		ctxt;
	int32	ret ;

	memset	(&ctxt,0,sizeof(t_wctxt));
	ctxt.flags	= flags;
	ctxt.passe	= 0;
	ctxt.root	= obj;
	ObjPrefixe(obj,ctxt.rootpref);
/*
	if	(defns && *defns)
		strcpy(ctxt.defpref,defns);
*/
#if	0
	ret	= xowritobj(NULL,&ctxt,obj,0) ;
	if	(ret < 0)
	{
		return	-11;
	}
	ctxt.passe++;
#endif

	if	(useRootObj) {
		int numbalise	= XoNumAttrInObj(obj,XOML_BALISE_ATTR);
		int lg;
		schemaID	= XoGetAttr(obj,numbalise,&lg);

		char *pt = strchr(schemaID, ':');
		if	(pt)
			schemaID	= pt+1;
/*
		if	(!strcmp(schemaID, "obj"))
			schemaID	= "xobix";
*/
	}

	if	((g_schemaPtr || useRootObj) && XoIsObix(obj) > 0) {
		strcpy (g_NsDefault, "http://obix.org/ns/schema/1.1");
		schemaID	= "xobix";
	}

	writer	= xmlNewTextWriterFilename(filename, 0, schemaID);
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



/* ------------------- base64 ------------------ */

#include <string.h>

char b64string[] =
	"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

long base64_encode (char *to, const char *from, unsigned int len) {
	const char *fromp = from;
	char *top = to;
	unsigned char cbyte;
	unsigned char obyte;
	char end[3];

	for (; len >= 3; len -= 3) {
		cbyte = *fromp++;
		*top++ = b64string[(int)(cbyte >> 2)];
		obyte = (cbyte << 4) & 0x30;		/* 0011 0000 */

		cbyte = *fromp++;
		obyte |= (cbyte >> 4);			/* 0000 1111 */
		*top++ = b64string[(int)obyte];
		obyte = (cbyte << 2) & 0x3C;		/* 0011 1100 */

		cbyte = *fromp++;
		obyte |= (cbyte >> 6);			/* 0000 0011 */
		*top++ = b64string[(int)obyte];
		*top++ = b64string[(int)(cbyte & 0x3F)];/* 0011 1111 */
	}

	if (len) {
		end[0] = *fromp++;
		if (--len) end[1] = *fromp++; else end[1] = 0;
		end[2] = 0;

		cbyte = end[0];
		*top++ = b64string[(int)(cbyte >> 2)];
		obyte = (cbyte << 4) & 0x30;		/* 0011 0000 */

		cbyte = end[1];
		obyte |= (cbyte >> 4);
		*top++ = b64string[(int)obyte];
		obyte = (cbyte << 2) & 0x3C;		/* 0011 1100 */

		if (len) *top++ = b64string[(int)obyte];
		else *top++ = '=';
		*top++ = '=';
	}
	*top = 0;
	return top - to;
}

/* badchar(): check if c is decent; puts either the */
/* location of c or null into p.                  */
#define badchar(c,p) (!(p = memchr(b64string, c, 64)))

long base64_decode (char *to, const char *from, unsigned int len) {
	const char *fromp = from;
	char *top = to;
	char *p;
	unsigned char cbyte;
	unsigned char obyte;
	int padding = 0;

	for (; len >= 4; len -= 4) {
		if ((cbyte = *fromp++) == '=') cbyte = 0;
		else {
			if (badchar(cbyte, p)) return -1;
			cbyte = (p - b64string);
		}
		obyte = cbyte << 2;		/* 1111 1100 */

		if ((cbyte = *fromp++) == '=') cbyte = 0;
		else {
			if (badchar(cbyte, p)) return -1;
			cbyte = p - b64string;
		}
		obyte |= cbyte >> 4;		/* 0000 0011 */
		*top++ = obyte;

		obyte = cbyte << 4;		/* 1111 0000 */
		if ((cbyte = *fromp++) == '=') { cbyte = 0; padding++; }
		else {
			padding = 0;
			if (badchar (cbyte, p)) return -1;
			cbyte = p - b64string;
		}
		obyte |= cbyte >> 2;		/* 0000 1111 */
		*top++ = obyte;

		obyte = cbyte << 6;		/* 1100 0000 */
		if ((cbyte = *fromp++) == '=') { cbyte = 0; padding++; }
		else {
			padding = 0;
			if (badchar (cbyte, p)) return -1;
			cbyte = p - b64string;
		}
		obyte |= cbyte;			/* 0011 1111 */
		*top++ = obyte;
	}

	*top = 0;
	if (len) return -1;
	return (top - to) - padding;
}

