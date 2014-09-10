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
* Please contact Actility, SA.,	4, rue Ampere 22300 LANNION FRANCE
* or visit www.actility.com if you need additional
* information or have any questions.
*/


#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <unistd.h>
#include <malloc.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "procTypes.h"
#include "EXISerializer.h"
#include "EXIParser.h"
#include "stringManipulate.h"
#include "grammarGenerator.h"

#include <mxml.h>

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



struct element {
	struct element* next;
	char* name;
	mxml_node_t *node;
};

static void push(struct element** stack, struct element* el);
static struct element* pop(struct element** stack);
static struct element* createElement(char* name, mxml_node_t *node);
static void destroyElement(struct element* el);

typedef struct {
	char *prefix;
	char *ns;
} ns_t;

#define NSTABLE_SIZE 20
struct appData {
	boolean expectAttributeData;
	char nameBuf[200];             // needed for the OUT_XML Output Format
	struct element* stack;         // needed for the OUT_XML Output Format
	mxml_node_t *root;
};

ns_t NSTable[NSTABLE_SIZE];
int NSTable_size = 0;

int addNameSpace(char *prefix, char *ns) {
	int i;
	ns_t *pt;
	for	(i=0; i<NSTABLE_SIZE; i++) {
		pt = &NSTable[i];
		if	(!pt->prefix) {
			pt->prefix = strdup(prefix);
			pt->ns = strdup(ns);
			NSTable_size++;
			return 0;
		}
		// Prefix already declared
		if	(!strcmp(prefix, pt->prefix))
			return -1;
	}
	return -1;
}

char *getPrefixByNs_String(const String *ns) {
	int i;
	ns_t *pt;
	for	(i=0; i<NSTABLE_SIZE; i++) {
		pt = &NSTable[i];
		if	(!pt->prefix)
			return NULL;
		if (stringEqualToAscii(*ns, pt->ns))
			return pt->prefix;
	}
	return NULL;
}


 /**
 * @brief Convert a EXI formated document into its mxml equivalent
 * @param buf Input buffer
 * @param len Length of input buffer
 * @return mxml_node_t pointer
 */
mxml_node_t *exi2xml(char *data, int len) {
	static boolean _inited = FALSE;

	errorCode tmp_err_code = 0;
	BinaryBuffer buffer;
	struct appData userData;
	Parser testParser;

	buffer.buf = data;
	buffer.bufContent = len;
	buffer.bufLen = len;
	buffer.ioStrm.readWriteToStream = NULL;
	buffer.ioStrm.stream = NULL;

	if	(_inited == FALSE) {
		addNameSpace ("xsi", "http://www.w3.org/2001/XMLSchema-instance");
		//addNameSpace ("", "http://www.plcopen.org/xml/tc6.xsd");
		_inited = TRUE;
	}

	memset (&userData, 0, sizeof(userData));
	userData.expectAttributeData = FALSE;

	tmp_err_code = initParser(&testParser, buffer, &userData);
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

	tmp_err_code = parseHeader(&testParser, FALSE);

	EXIPSchema *schemaPtr = NULL;
	setSchema(&testParser, schemaPtr);

	while(tmp_err_code == ERR_OK)
		tmp_err_code = parseNext(&testParser);
	destroyParser(&testParser);
	return	userData.root;
}

void string2Ascii(const String *s, char *str) {
	memcpy (str, s->str, s->length);
	str[s->length] = 0;
}

static errorCode sample_fatalError(const errorCode code, const char* msg, void* app_data) {
	struct appData* appD = (struct appData*) app_data;
	printf("\n%d : FATAL ERROR: %s\n", code, msg);
	mxmlDelete (appD->root);
	appD->root = NULL;
	return EXIP_HANDLER_STOP;
}

static errorCode sample_startDocument(void* app_data) {
	struct appData* appD = (struct appData*) app_data;
	appD->root = mxmlNewXML ("1.0");
	return ERR_OK;
}

static errorCode sample_endDocument(void* app_data) {
	return ERR_OK;
}

static errorCode sample_startElement(QName qname, void* app_data) {
	struct appData* appD = (struct appData*) app_data;
	char *pfx;
	char tmp[10];
	boolean newNS = FALSE;
	*appD->nameBuf = 0;
	boolean firstElement = appD->stack ? FALSE : TRUE;

	if(!isStringEmpty(qname.uri)) {
		pfx = getPrefixByNs_String (qname.uri);
		if	(!pfx) {
			//	Assume the first namespace (after xsi) is the default, ie pfx = ""
			if	(NSTable_size == 1)
				pfx = "";
			else {
				sprintf(tmp, "p%d", NSTable_size);
				pfx = tmp;
			}
			string2Ascii (qname.uri, appD->nameBuf);
			addNameSpace (pfx, appD->nameBuf);
			*appD->nameBuf = 0;
			newNS = TRUE;
		}
		if	(pfx && *pfx)
			sprintf(appD->nameBuf, "%s:", pfx);
	}

	string2Ascii (qname.localName, appD->nameBuf+strlen(appD->nameBuf));

	mxml_node_t *parent = firstElement ? appD->root : appD->stack->node;

	mxml_node_t *node = mxmlNewElement (parent, appD->nameBuf);
	push(&(appD->stack), createElement(appD->nameBuf, node));

	// First element : add all xmlns
	if	(firstElement) {
		int i;
		ns_t *pt;
		for	(i=0; i<NSTable_size; i++) {
			pt = &NSTable[i];

			strcpy(appD->nameBuf, "xmlns");
			if	(pt->prefix && *pt->prefix) {
				strcat (appD->nameBuf, ":");
				strcat (appD->nameBuf, pt->prefix);
			}
			mxmlElementSetAttr (node, appD->nameBuf, pt->ns);
		}
	}
	else if (newNS) {
		char uri[100];
		strcpy(appD->nameBuf, "xmlns");
		if	(pfx && *pfx) {
			strcat (appD->nameBuf, ":");
			strcat (appD->nameBuf, pfx);
		}
		string2Ascii (qname.uri, uri);
		mxmlElementSetAttr (node, appD->nameBuf, uri);
	}
	return ERR_OK;
}

static errorCode sample_endElement(void* app_data) {
	struct appData* appD = (struct appData*) app_data;
	struct element* el;
	el = pop(&(appD->stack));
	destroyElement(el);
	return ERR_OK;
}

static errorCode sample_attribute(QName qname, void* app_data) {
	struct appData* appD = (struct appData*) app_data;
	*appD->nameBuf = 0;
	if(!isStringEmpty(qname.uri)) {
		char *pfx = getPrefixByNs_String (qname.uri);
		if	(pfx && *pfx)
			sprintf (appD->nameBuf, "%s:", pfx);
	}
	string2Ascii (qname.localName, appD->nameBuf+strlen(appD->nameBuf));
	appD->expectAttributeData = TRUE;
	return ERR_OK;
}

static errorCode sample_stringData(const String value, void* app_data) {
	struct appData* appD = (struct appData*) app_data;
	char str[300];
	string2Ascii (&value, str);
	if(appD->expectAttributeData) {
		mxmlElementSetAttr (appD->stack->node, appD->nameBuf, str);
		appD->expectAttributeData = FALSE;
	}
	else {
		//mxmlNewText (appD->stack->node, 0, str);
		mxmlNewOpaque (appD->stack->node, str);
	}
	return ERR_OK;
}

static errorCode sample_decimalData(Decimal value, void* app_data) {
	return ERR_OK;
}

static errorCode sample_intData(Integer int_val, void* app_data) {
	return ERR_OK;
}

static errorCode sample_booleanData(boolean bool_val, void* app_data) {
	return ERR_OK;
}

static errorCode sample_floatData(Float fl_val, void* app_data) {
	return ERR_OK;
}

static errorCode sample_dateTimeData(EXIPDateTime dt_val, void* app_data) {
	return ERR_OK;
}

static errorCode sample_binaryData(const char* binary_val, Index nbytes, void* app_data) {
	return ERR_OK;
}

static errorCode sample_qnameData(const QName qname, void* app_data) {
	return ERR_OK;
}

//-------- Simple stack to store context
static void push(struct element** stack, struct element* el) {
	if(*stack == NULL)
		*stack = el;
	else
	{
		el->next = *stack;
		*stack = el;
	}
}

static struct element* pop(struct element** stack) {
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

static struct element* createElement(char* name, mxml_node_t *node) {
	struct element* el;
	el = malloc(sizeof(struct element));
	if(el == NULL)
		exit(1);
	el->next = NULL;
	el->name = strdup(name);
	el->node = node;
	return el;
}

static void destroyElement(struct element* el) {
	free(el->name);
	free(el);
}
