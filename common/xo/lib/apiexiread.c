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
 * @file apiexiread.c
 * @brief EXI handler
 */

#include <stdio.h>
#include <string.h>
#define _GNU_SOURCE
#include <math.h>

#include <EXIParser.h>
#include <stringManipulate.h>
#include <grammarGenerator.h>

#include "xoapipr.h"

#define	XO_XML_READER_EXI
#include "xmlstr.h"

//#define DEBUG

#ifdef DEBUG
#define PRINT	printf
#define FWRITE	fwrite
#else
#define PRINT _XX
#define FWRITE _XX
static inline void _XX() { }
#endif


#define INPUT_BUFFER_SIZE 1024
#define MAX_PREFIXES 20

struct appData {
	unsigned char expectAttributeData;
	char nameBuf[200];             // needed for the OUT_XML Output Format
	struct element* stack;         // needed for the OUT_XML Output Format
	unsigned char unclosedElement; 	 // needed for the OUT_XML Output Format
	char prefixes[MAX_PREFIXES][200]; // needed for the OUT_XML Output Format
	unsigned char prefixesCount; 	 // needed for the OUT_XML Output Format
	char	*attribute;
	t_ctxt ctxt;
	void	*xo_obj;
	int	depth;
	Parser	*parser;
};



// Stuff needed for the OUT_XML Output Format
// ******************************************
struct element {
	struct element* next;
	char* name;
};

static void push(struct element** stack, struct element* el);
static struct element* pop(struct element** stack);
static struct element* createElement(char* name);
static void destroyElement(struct element* el);
static char lookupPrefix(struct appData* aData, String ns, unsigned char* prxHit, unsigned char* prefixIndex);

static	int cb_startElement (struct appData* appD, int EEflag);
static	int cb_endElement (struct appData* appD);
static	int cb_text (struct appData* appD, char *value);

// ******************************************

// Content Handler API
#if EXIP_VERSION==276
typedef char errorCode;
#endif
static errorCode sample_fatalError(const errorCode code, const char* msg, void* app_data);
static errorCode sample_startDocument(void* app_data);
static errorCode sample_endDocument(void* app_data);
static errorCode sample_startElement(QName qname, void* app_data);
static errorCode sample_endElement(void* app_data);
static errorCode sample_attribute(QName qname, void* app_data);
static errorCode sample_stringData(const String value, void* app_data);
static errorCode sample_decimalData(Decimal value, void* app_data);
static errorCode sample_intData(Integer int_val, void* app_data);
static errorCode sample_floatData(Float fl_val, void* app_data);
static errorCode sample_booleanData(boolean bool_val, void* app_data);
static errorCode sample_dateTimeData(EXIPDateTime dt_val, void* app_data);
static errorCode sample_binaryData(const char* binary_val, Index nbytes, void* app_data);
static errorCode sample_qnameData(const QName qname, void* app_data);
static EXIPSchema *sample_retrieveSchema(void* app_data);

EXIPSchema *XoExiLoadSchema(char *schemaID);

char *getPrefixByNamespace(char *ns) {
	char	*p;

	if	(!ns)	return "";
	p	= _XoFindPrfNameSpace(ns);
	if	(p && *p)
		return	p;
	return	ns;
}

size_t readFileInputStream(void* buf, size_t readSize, void* stream) {
	FILE *infile = (FILE*) stream;
	return fread(buf, 1, readSize, infile);
}

typedef struct {
	char	*data;
	int	len;
	int	current;
} memoryStream_t;

size_t readMemInputStream(void* buf, size_t readSize, void* stream) {
	memoryStream_t *ms = (memoryStream_t *)stream;
	if	(!ms->data)
		return	0;
	int how	= ms->len - ms->current;
	if	(how > readSize)
		how	= readSize;
	PRINT ("readMemInputStream %d how=%d\n", readSize, how);
	memcpy (buf, ms->data+ms->current, how);
	ms->current	+= how;
	return	how;
}

static void writeString(String str) {
	FWRITE(str.str, sizeof(CharType), str.length, stdout);
}

static void copyString(String str, char *out) {
	memcpy (out, str.str, str.length);
	out[str.length] = 0;
}




/**
 * @brief From apiexischema.c
 */
extern EXIPSchema *g_schemaPtr;

static	void _lookupSchema() {
	int	i, j;
	EXIGrammar *gr;
	GrammarRule *rule;
	extern errorCode printGrammarRule(SmallIndex nonTermID, GrammarRule* rule, EXIPSchema *schema);

	printf ("*** DocGrammar\n");
	gr=&g_schemaPtr->docGrammar;
	for	(j=0, rule=gr->rule; j<gr->count; j++, rule++) {
		printGrammarRule(j, rule, g_schemaPtr);
	}

	printf ("  grammarTable %d\n", g_schemaPtr->grammarTable.count);
	for	(i=0, gr=g_schemaPtr->grammarTable.grammar; i<g_schemaPtr->grammarTable.count; i++, gr++) {
		printf ("*** Grammar #%d : %d rules\n", i, gr->count);
		for	(j=0, rule=gr->rule; j<gr->count; j++, rule++) {
			printGrammarRule(j, rule, g_schemaPtr);
		}
	}
}

static void *XoReadExiCommon(struct ioStream *ioStr, char *roottype,int flags,int *err) {
	Parser testParser;
	struct appData parsingData;
	char buf[INPUT_BUFFER_SIZE];
	BinaryBuffer buffer;
	errorCode tmp_err_code = UNEXPECTED_ERROR;

	memset	(&parsingData,0,sizeof(parsingData));
	parsingData.ctxt.roottype	= roottype;
	parsingData.ctxt.flags	= flags;
	parsingData.ctxt.ns.flags	= flags;
	parsingData.parser	= &testParser;
	_XoAllocCtxt(&parsingData.ctxt);

	buffer.buf = buf;
	buffer.bufLen = INPUT_BUFFER_SIZE;
	buffer.bufContent = 0;
	// Parsing steps:

	// I: First, define an external stream for the input to the parser if any
	buffer.ioStrm	= *ioStr;

	// II: Second, initialize the parser object
#if EXIP_VERSION==276
	tmp_err_code = initParser(&testParser, buffer, g_schemaPtr, &parsingData);
#endif
#if EXIP_VERSION>=289
	tmp_err_code = initParser(&testParser, buffer, &parsingData);
#endif
	if(tmp_err_code != ERR_OK) {
		_XoFreeCtxt(&parsingData.ctxt);
		return NULL;
	}

	// III: Initialize the parsing data and hook the callback handlers to the parser object

	parsingData.expectAttributeData = 0;
	parsingData.stack = NULL;
	parsingData.unclosedElement = 0;
	parsingData.prefixesCount = 0;

	testParser.handler.fatalError = sample_fatalError;
	testParser.handler.error = sample_fatalError;
	testParser.handler.startDocument = sample_startDocument;
	testParser.handler.endDocument = sample_endDocument;
	testParser.handler.startElement = sample_startElement;
	testParser.handler.attribute = sample_attribute;
	testParser.handler.stringData = sample_stringData;
	testParser.handler.endElement = sample_endElement;
	testParser.handler.decimalData = sample_decimalData;
	testParser.handler.intData = sample_intData;
	testParser.handler.floatData = sample_floatData;
	testParser.handler.booleanData = sample_booleanData;
	testParser.handler.dateTimeData = sample_dateTimeData;
	testParser.handler.binaryData = sample_binaryData;
	testParser.handler.qnameData = sample_qnameData;
#if EXIP_VERSION==276
	testParser.handler.retrieveSchema = sample_retrieveSchema;
#endif

	// IV: Parse the header of the stream

	tmp_err_code = parseHeader(&testParser, FALSE);
#if EXIP_VERSION>=289
	// IV.1: Set the schema to be used for parsing.
	// The schemaID mode and schemaID field can be read at
	// parser.strm.header.opts.schemaIDMode and
	// parser.strm.header.opts.schemaID respectively
	// If schemaless mode, use setSchema(&parser, NULL);

	EXIPSchema *schemaPtr = sample_retrieveSchema(&parsingData);
	setSchema(&testParser, schemaPtr);
#endif

	//_lookupSchema();

	// V: Parse the body of the EXI stream

	while(tmp_err_code == ERR_OK)
	{
		tmp_err_code = parseNext(&testParser);
		PRINT ("parseNext ret=%d\n", tmp_err_code);
	}

	// PCA
	parsingData.xo_obj	= parsingData.ctxt.obj[0];

	// VI: Free the memory allocated by the parser and schema object

	destroyParser(&testParser);

	_XoFreeCtxt(&parsingData.ctxt);
	PRINT ("End parsing xo_obj=%p\n", parsingData.xo_obj);
	return	parsingData.xo_obj;
}

/*DEF*/void *XoReadExiMem(char *buf,int sz,char *roottype,int flags,int *err)
{
	memoryStream_t	ms;
	struct ioStream ioStrm;

	ms.data	= buf;
	ms.len	= sz;
	ms.current	= 0;

	ioStrm.readWriteToStream = readMemInputStream;
	ioStrm.stream = &ms;

	return XoReadExiCommon(&ioStrm, roottype, flags, err);
}

/*DEF*/void *XoReadExiEx(char *filename,char *roottype,int flags,int *err)
{
	FILE *infile;
	struct ioStream ioStrm;

	infile = fopen(filename, "rb" );
	if(!infile)
		return NULL;

	ioStrm.readWriteToStream = readFileInputStream;
	ioStrm.stream = infile;

	void *ret = XoReadExiCommon(&ioStrm, roottype, flags, err);
	fclose (infile);
	return	ret;
}

/**
 * @brief Calls cb_startElement() only when all attributes are read
 * @param app_data Pointer to the struct appData passed to initParser()
 * @return void
 */
static void checkCallback(struct appData* appD, int EEflag) {
	//	Are we decoding attributes ?
	if	(appD->unclosedElement) {
		cb_startElement (appD, EEflag);
		appD->depth	++;
		appD->unclosedElement = 0;
		_XoFreeAttributesWith (&appD->ctxt,free);
	}
}

/**
 * @brief Convert an EXIP String into a C char *. Allocate memory like strdup().
 * @param str A EXIP String
 * @return
 */
char *StringStrdup(const String *str) {
	char	*c = malloc(str->length+1);
	memcpy (c, str->str, str->length);
	c[str->length] = 0;
	return	c;
}

/**
 * @brief Convert an EXIP QName into a C char *. Allocate memory like strdup().
 * @param str A EXIP QName
 * @return
 */
char *QNameStrdup(const QName *qname) {
	char *uri = StringStrdup(qname->uri);
	char *pfx = getPrefixByNamespace(uri);
	char *lname = StringStrdup(qname->localName);
	char *str = malloc(strlen(pfx)+strlen(lname)+2);
	*str = 0;
	if	(pfx && *pfx) {
		strcpy (str, pfx);
		strcat (str, ":");
	}
	strcat (str, lname);
	return str;
}

/**
 * @brief QName trace function
 * @param qname
 * @param out
 * @return none
 */
static void writeQName(QName qname) {
	PRINT ("QName=(");
	writeString (*qname.uri);
	PRINT (":");
	writeString (*qname.localName);
	PRINT (")");
}

/**
 * @brief Callback called when the EXIP library has found an error
 * @param app_data Pointer to the struct appData passed to initParser()
 * @return EXIP_HANDLER_OK if success, or EXIP_HANDLER_STOP if need to stop the parser
 */
static errorCode sample_fatalError(const errorCode code, const char* msg, void* app_data) {
	printf("\n[%s:%d] %d : FATAL ERROR: %s\n", __FILE__, __LINE__, code, msg);
	return EXIP_HANDLER_STOP;
}

/**
 * @brief Callback called when the EXIP library has parsed the header.
 * This is the place to check for schemaID and load schemas files if any.
 *
 * @param app_data Pointer to the struct appData passed to initParser()
 * @return An EXIPSchema or NULL
 */
static EXIPSchema *sample_retrieveSchema(void* app_data) {
	struct appData* appD = (struct appData*) app_data;
	EXIheader *hdr = &appD->parser->strm.header;

	if	(!hdr->has_options)
		return	g_schemaPtr;

	if	(isStringEmpty(&hdr->opts.schemaID))
		return	g_schemaPtr;

	PRINT ("sample_retrieveSchema\n");
	PRINT ("schemaID = ");
	writeString (hdr->opts.schemaID);
	PRINT ("\n");

	char	sch[100];
	copyString(hdr->opts.schemaID, sch);

	//	Obix specific : simulate xmlns attribute
	if	(!strcmp(sch, "xobix")) {
		t_ctxt	*ctxt = &appD->ctxt;
		ctxt->atname[ctxt->atcount]	= strdup("xmlns");
		ctxt->atvalue[ctxt->atcount]	= strdup("http://obix.org/ns/schema/1.1");
		ctxt->atcount++;
	}

	XoExiLoadSchema (sch);
	return g_schemaPtr;
}

/**
 * @brief Callback called when the EXIP library has found the start of the document
 * @param app_data Pointer to the struct appData passed to initParser()
 * @return EXIP_HANDLER_OK if success, or EXIP_HANDLER_STOP if need to stop the parser
 */
static errorCode sample_startDocument(void* app_data) {
	PRINT ("SD\n");
	return ERR_OK;
}

/**
 * @brief Callback called when the EXIP library has found the end of the document
 * @param app_data Pointer to the struct appData passed to initParser()
 * @return EXIP_HANDLER_OK if success, or EXIP_HANDLER_STOP if need to stop the parser
 */
static errorCode sample_endDocument(void* app_data) {
	PRINT ("ED\n");
	return ERR_OK;
}

/**
 * @brief Callback called when the EXIP library has found the start of an element
 * @param qname The qualified name of the element
 * @param app_data Pointer to the struct appData passed to initParser()
 * @return EXIP_HANDLER_OK if success, or EXIP_HANDLER_STOP if need to stop the parser
 */
static errorCode sample_startElement(QName qname, void* app_data) {
	struct appData* appD = (struct appData*) app_data;
	char error = 0;
	unsigned char prefixIndex = 0;
	unsigned char prxHit = 1;
	int t = 0;

	PRINT ("SE ");
	writeQName(qname);
	PRINT ("\n");

	//	Check if we where in another element. If yes, calls cb_startElement
	checkCallback (appD, 0);

	if(!isStringEmpty(qname.uri)) {
		char uri[200];
		char pfx[30];

		error = lookupPrefix(appD, *qname.uri, &prxHit, &prefixIndex);
		if	(error) {
			fprintf (stderr, "Prefix not found\n");
			return EXIP_HANDLER_STOP;
		}

		copyString(*qname.uri, uri);
		strcpy (pfx, getPrefixByNamespace(uri));
		strcpy (appD->nameBuf, pfx);
		strcat (appD->nameBuf, ":");
		t = strlen(appD->nameBuf);

		if(prxHit == 0) {
			// Create xmlns: attribute
			t_ctxt	*ctxt = &appD->ctxt;
			char tmp[30];
			strcpy (tmp, "xmlns:");
			strcat (tmp, pfx);
			ctxt->atname[ctxt->atcount]	= strdup(tmp);
			ctxt->atvalue[ctxt->atcount]	= StringStrdup(qname.uri);
			ctxt->atcount++;
		}
	}

	memcpy(appD->nameBuf + t, qname.localName->str, qname.localName->length);
	appD->nameBuf[t + qname.localName->length] = '\0';

	push(&(appD->stack), createElement(appD->nameBuf));

	appD->unclosedElement = 1;
	return ERR_OK;
}

/**
 * @brief Callback called when the EXIP library has found the end of an element
 * @param app_data Pointer to the struct appData passed to initParser()
 * @return EXIP_HANDLER_OK if success, or EXIP_HANDLER_STOP if need to stop the parser
 */
static errorCode sample_endElement(void* app_data) {
	struct appData* appD = (struct appData*) app_data;
	struct element* el;

	PRINT ("EE\n");
	int save = appD->unclosedElement;
	checkCallback (appD, 1);

	//	libxml2 compatibility : don't call cb_endElement when it is empty
	if	(!save)
		cb_endElement (appD);
	appD->depth	--;

	el = pop(&(appD->stack));
	destroyElement(el);
	_XoFreeAttributesWith (&appD->ctxt,free);
	return ERR_OK;
}

static void putValue(struct appData* appD, char *avalue) {
	t_ctxt	*ctxt = &appD->ctxt;
//printf ("putValue %s\n", avalue);
	//	This is the attribute value
	if(appD->expectAttributeData) {
		char	*aqname = appD->attribute;
		//printf ("AT(#%d) %s=%s\n", ctxt->atcount, aqname, avalue);
		ctxt->atname[ctxt->atcount]	= aqname;
		ctxt->atvalue[ctxt->atcount]	= strdup(avalue);
		ctxt->atcount++;

		appD->expectAttributeData = 0;
		appD->attribute	= NULL;
	}
	else {
		//printf ("CH %s\n", avalue);
		checkCallback (appD, 0);
		cb_text (appD, avalue);
	}
}

/**
 * @brief Callback called when the EXIP library has found an attribute
 * @param qname The qualified name of the attribute
 * @param app_data Pointer to the struct appData passed to initParser()
 * @return EXIP_HANDLER_OK if success, or EXIP_HANDLER_STOP if need to stop the parser
 */
static errorCode sample_attribute(QName qname, void* app_data) {
	struct appData* appD = (struct appData*) app_data;
	PRINT ("AT ");
	writeQName(qname);
	PRINT ("\n");
	char *attr = QNameStrdup(&qname);
	appD->attribute = attr;
	appD->expectAttributeData = 1;
	return ERR_OK;
}

static errorCode sample_qnameData(const QName qname, void* app_data) {
	PRINT ("### QNAME ");
	writeQName (qname);
	PRINT ("\n");
	char *avalue = QNameStrdup(&qname);
	putValue ((struct appData*) app_data, avalue);
	free (avalue);
	return ERR_OK;
}

/**
 * @brief Callback called when the EXIP library has found some string data
 *	Two cases : if we where decoding an attribute, this data its the value;
 *	else this is text data into an element
 * @param value The string data
 * @param app_data Pointer to the struct appData passed to initParser()
 * @return EXIP_HANDLER_OK if success, or EXIP_HANDLER_STOP if need to stop the parser
 */
static errorCode sample_stringData(const String value, void* app_data) {
	char	*avalue = StringStrdup(&value);
	putValue ((struct appData*) app_data, avalue);
	free (avalue);
	return ERR_OK;
}

static errorCode sample_decimalData(Decimal value, void* app_data) {
	char	avalue[40];
#if	0
	sprintf (avalue, "%g", (double)value);
	putValue ((struct appData*) app_data, avalue);
#else
	printf("XO/EXI Type Decimal not supported '%s':%d\n",
				__FILE__,__LINE__);
#endif
	return ERR_OK;
}

static errorCode sample_intData(Integer int_val, void* app_data) {
	char	avalue[40];
	sprintf (avalue, "%lld", int_val);
	putValue ((struct appData*) app_data, avalue);
	return ERR_OK;
}

static errorCode sample_booleanData(boolean bool_val, void* app_data) {
	putValue ((struct appData*) app_data, bool_val ? "true":"false");
	return ERR_OK;
}

static errorCode sample_floatData(Float fl_val, void* app_data) {
	char	avalue[40];
	sprintf(avalue, "%lldE%d", fl_val.mantissa, fl_val.exponent);
	putValue ((struct appData*) app_data, avalue);
	return ERR_OK;
}

extern void EXIP_to_iso8601 (EXIPDateTime *dt, char *str);

static errorCode sample_dateTimeData(EXIPDateTime dt_val, void* app_data) {
	char	avalue[200];
	EXIP_to_iso8601 (&dt_val, avalue);
	putValue ((struct appData*) app_data, avalue);
	return ERR_OK;
}

static errorCode sample_binaryData(const char* binary_val, Index nbytes, void* app_data) {
	struct appData* appD = (struct appData*)app_data;
	t_ctxt *ctxt = &appD->ctxt;
	void		*curObj	= ctxt->obj[ctxt->level-1];
	t_attribut_ref	*curAtr	= ctxt->ar[ctxt->level-1];

	if	(!curObj || !curAtr) {
		fprintf (stderr, "binaryData failure\n");
		return	EXIP_HANDLER_STOP;
	}

	if (curAtr->ar_type == XoBuffer)
		_XoSetBasicValue(curObj, curAtr, (char **)&binary_val, nbytes);
	return ERR_OK;
}

static void push(struct element** stack, struct element* el)
{
	if(*stack == NULL)
		*stack = el;
	else
	{
		el->next = *stack;
		*stack = el;
	}
}

static struct element* pop(struct element** stack)
{
	if(*stack == NULL)
		return NULL;
	else
	{
		struct element* result;
		result = *stack;
		*stack = (*stack)->next;
		return result;
	}
}

static struct element* createElement(char* name)
{
	struct element* el;
	el = malloc(sizeof(struct element));
	if(el == NULL)
		exit(1);
	el->next = NULL;
	el->name = malloc(strlen(name)+1);
	if(el->name == NULL)
		exit(1);
	strcpy(el->name, name);
	return el;
}

static void destroyElement(struct element* el) {
	free(el->name);
	free(el);
}

static char lookupPrefix(struct appData* aData, String ns, unsigned char* prxHit, unsigned char* prefixIndex)
{
	int i;
	for(i = 0; i < aData->prefixesCount; i++)
	{
		if(stringEqualToAscii(ns, aData->prefixes[i]))
		{
			*prefixIndex = i;
			*prxHit = 1;
			return 0;
		}
	}

	if(aData->prefixesCount == MAX_PREFIXES)
		return 1;
	else
	{
		memcpy(aData->prefixes[aData->prefixesCount], ns.str, ns.length);
		aData->prefixes[aData->prefixesCount][ns.length] = '\0';
		*prefixIndex = aData->prefixesCount;
		aData->prefixesCount += 1;
		*prxHit = 0;
		return 0;
	}
}







/* ================== XO legacy part ===================== */



static	int cb_startElement (struct appData* appD, int EEflag)
{
	t_ctxt *ctxt = &appD->ctxt;

	appD->ctxt.depth	= appD->depth;
	appD->ctxt.nodetype	= XML_READER_TYPE_ELEMENT;
	appD->ctxt.qname	= appD->nameBuf;
	appD->ctxt.value	= NULL;
	appD->ctxt.isempty	= EEflag;

	return	_XoProcessNode(appD,ctxt);
}

static	int cb_endElement (struct appData* appD)
{
	t_ctxt *ctxt = &appD->ctxt;

	appD->ctxt.depth	= appD->depth;
	appD->ctxt.nodetype	= XML_READER_TYPE_END_ELEMENT;
	appD->ctxt.qname	= appD->nameBuf;
	appD->ctxt.value	= NULL;
	appD->ctxt.isempty	= 0;

	return	_XoProcessNode(appD,ctxt);
}

static	int cb_text (struct appData* appD, char *value)
{
	t_ctxt *ctxt = &appD->ctxt;

	appD->ctxt.depth	= appD->depth;
	appD->ctxt.nodetype	= XML_READER_TYPE_TEXT;
	appD->ctxt.qname	= appD->nameBuf;
	appD->ctxt.value	= value;
	appD->ctxt.isempty	= 0;

	return	_XoProcessNode(appD,ctxt);
}
