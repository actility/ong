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


/*
	This program uses mxml/sax to read a document, encodes it in EXI,
	then decodes it.
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

#define OUTPUT_BUFFER_SIZE 64*1024
#define check(str)	if (tmp_err_code != ERR_OK) { printf ("	=====> Err line %d (%s) code:%d\n", __LINE__, str, tmp_err_code); exit(0); }

#define OUT_EXI 0
#define OUT_XML 1

// Stuff needed for the OUT_XML Output Format
// ******************************************
struct element {
	struct element* next;
	char* name;
	mxml_node_t *node;
};

static void push(struct element** stack, struct element* el);
static struct element* pop(struct element** stack);
static struct element* createElement(char* name, mxml_node_t *node);
static void destroyElement(struct element* el);


static int parseSchema(char* xsdList, EXIPSchema* schema);

typedef struct {
	char *prefix;
	char *ns;
} ns_t;

#define NSTABLE_SIZE 20
struct appData {
	unsigned char outputFormat;
	boolean expectAttributeData;
	char nameBuf[200];             // needed for the OUT_XML Output Format
	struct element* stack;         // needed for the OUT_XML Output Format
};

ns_t NSTable[NSTABLE_SIZE];
int NSTable_size = 0;

int addNameSpace(char *prefix, char *ns) {
	int i;
	ns_t *pt;
printf ("### addNameSpace %s %s\n", prefix, ns);
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

char *getNsByPrefix(char *prefix) {
	int i;
	ns_t *pt;
	for	(i=0; i<NSTable_size; i++) {
		pt = &NSTable[i];
		if	(!pt->prefix)
			return NULL;
		if	(!strcmp(prefix, pt->prefix))
			return pt->ns;
	}
	return NULL;
}

char *getPrefixByNs(char *ns) {
	int i;
	ns_t *pt;
	for	(i=0; i<NSTable_size; i++) {
		pt = &NSTable[i];
		if	(!pt->prefix)
			return NULL;
		if	(!strcmp(ns, pt->ns))
			return pt->prefix;
	}
	return NULL;
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

void info() {
	struct mallinfo info = mallinfo();
	printf ("### mallinfo : total allocated space:  %d bytes\n", info.uordblks);
}

void greenMsg (char *fmt, ...) {
	va_list ap;
	va_start(ap, fmt);

	printf ("\033[32;7m");
	vprintf (fmt, ap);
	printf ("\033[0m\n");
	va_end(ap);
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

	char *ns = getNsByPrefix (uri);
	if	(ns)
		strcpy (uri, ns);
}


void string2Ascii(const String *s, char *str) {
	memcpy (str, s->str, s->length);
	str[s->length] = 0;
}

/**
 * @brief Retrieve the text contained in a xml token, eg <tok>content</tok>
 * @param node The xml node
 * @return The contained text
 */
char *getContainedText(mxml_node_t *node) {
	char *content = NULL;
	if (node && node->type == MXML_OPAQUE) {
		content = (char *)mxmlGetOpaque (node);
		if (content && !strcmp(content, "\n")) {
			node	= mxmlWalkNext (node, NULL, MXML_DESCEND);
			content = (char *)mxmlGetCDATA(node);
		}
	}
	return content;
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

	dt->presenceMask = YEAR_PRESENCE | MON_PRESENCE | MDAY_PRESENCE | HOUR_PRESENCE | MIN_PRESENCE | SEC_PRESENCE;
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
	if	(dt->presenceMask & YEAR_PRESENCE) {
		sprintf (avalue+strlen(avalue), "%04d-%02d-%02d",
			dt->dateTime.tm_year + BASE_YEAR,
			dt->dateTime.tm_mon + 1,
			dt->dateTime.tm_mday);
	}
	if	(dt->presenceMask & HOUR_PRESENCE) {
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

void saveToFile (char *file, unsigned char *buf, int sz) {
	int fd = open(file, O_RDWR|O_CREAT|O_TRUNC, 0644);
	if	(fd < 0)
		return;
	write (fd, buf, sz);
	close(fd);
}

EXITypeClass valueType;

/**
 * @brief SAX callback
 * @param[in] node Current node
 * @param[in] event SAX event
 * @param[in] data User data
 * @return void
 */
void sax_cb(mxml_node_t *node, mxml_sax_event_t event, void *data) {
	errorCode tmp_err_code = 0;
	EXIStream *strm = (EXIStream *)data;
	String uri;
	String ln;
	QName qname= {&uri, &ln};
	char	s_uri[100], s_ln[100];
	String chVal;
	char *name, *value;
	int i;
	mxml_attr_t *attr;

	switch (event) {
	case	MXML_SAX_ELEMENT_OPEN:

		// Handles xmlns attributes first
		for (i = node->value.element.num_attrs, attr = node->value.element.attrs; i > 0; i --, attr ++) {
			cutQName (attr->name, s_uri, s_ln);

			if (!strcmp(s_uri, "xmlns")) {
				addNameSpace (s_ln, attr->value);
				continue;
			}
			if (!strcmp(s_ln, "xmlns")) {
				addNameSpace ("", attr->value);
				continue;
			}
		}

		name = node->value.element.name;
		cutQName (name, s_uri, s_ln);

		printf ("### startElement %s:%s\n", s_uri, s_ln);
		tmp_err_code += asciiToString(s_uri, &uri, &strm->memList, FALSE); check("")
		tmp_err_code += asciiToString(s_ln, &ln, &strm->memList, FALSE); check("")
		tmp_err_code += serialize.startElement(strm, qname, &valueType); check(name)
		printf ("                 valueType=%d\n", valueType);

		for (i = node->value.element.num_attrs, attr = node->value.element.attrs; i > 0; i --, attr ++) {
			EXITypeClass vt;
			cutQName (attr->name, s_uri, s_ln);

			if (!strcmp(s_uri, "xmlns")) {
				continue;
			}
			if (!strcmp(s_ln, "xmlns")) {
				continue;
			}

			printf ("### attribute %s:%s\n", s_uri, s_ln);
			tmp_err_code += asciiToString(s_uri, &uri, &strm->memList, FALSE); check("")
			tmp_err_code += asciiToString(s_ln, &ln, &strm->memList, FALSE); check("")
			tmp_err_code += serialize.attribute(strm, qname, TRUE, &vt); check(attr->name)

			if (!strcmp(attr->name, "xsi:type")) {
				cutQName (attr->value, s_uri, s_ln);
				printf ("### qnameData %s:%s\n", s_uri, s_ln);
				tmp_err_code += asciiToString(s_uri, &uri, &strm->memList, FALSE); check("")
				tmp_err_code += asciiToString(s_ln, &ln, &strm->memList, FALSE); check("")
				tmp_err_code += serialize.qnameData(strm, qname); check(attr->value);
			}
			else {
				printf ("### stringData %s\n", attr->value);
				tmp_err_code += asciiToString(attr->value, &chVal, &strm->memList, FALSE);
				tmp_err_code += serialize.stringData(strm, chVal); check("CH")
			}
		}
		break;
	case	MXML_SAX_ELEMENT_CLOSE:
		printf ("### endElement\n");
		tmp_err_code += serialize.endElement(strm); check("EE")
		break;
	case	MXML_SAX_DATA:
		value	= getContainedText(node);

		if (value && (*value != '\n')) {
			// mxml sends some empty DATA. These DATA are out of schema and the function
			// serialize.stringData returns an error. This error is *not* checked.
			switch (valueType) {
			case VALUE_TYPE_STRING_CLASS :
			case VALUE_TYPE_UNTYPED_CLASS :
			case VALUE_TYPE_NONE_CLASS :
			default :
				printf ("### stringData %s\n", value);
				tmp_err_code += asciiToString(value, &chVal, &strm->memList, FALSE);
				tmp_err_code += serialize.stringData(strm, chVal);
				break;
			case VALUE_TYPE_BOOLEAN_CLASS :
				{
				boolean b = FALSE;
				if	(!strcasecmp(value, "true") || !strcmp(value, "1"))
					b	= TRUE;
				printf ("### booleanData\n");
				tmp_err_code += serialize.booleanData(strm, b);
				break;
				}
			case VALUE_TYPE_DECIMAL_CLASS :
				printf ("### decimalData %s\n", value);
				tmp_err_code += serialize.decimalData(strm, (Decimal)atof(value));
				break;
			case VALUE_TYPE_INTEGER_CLASS :
				printf ("### intData %s\n", value);
				tmp_err_code += serialize.intData(strm, atoll(value));
				break;
			case VALUE_TYPE_FLOAT_CLASS :
				{
				Float	fl_val;
				fl_val.mantissa = normalize_float(value, &fl_val.exponent);
				printf ("floatData %s\n", value);
				tmp_err_code += serialize.floatData(strm, fl_val);
				}
				break;
			case VALUE_TYPE_BINARY_CLASS :
				//tmp_err_code += serialize.binaryData(&testStrm, SOME_BINARY_DATA, 10);
				break;
			case VALUE_TYPE_DATE_TIME_CLASS :
				{
				char *temp = strdup(value);
				EXIPDateTime dt;
				iso8601_to_EXIP	(temp, &dt);
				free (temp);
				printf ("### dateTimeData %s\n", value);
				tmp_err_code += serialize.dateTimeData(strm, dt);
				}
				break;
			}
		}
		break;
	case	MXML_SAX_CDATA :
		value = node->value.element.name;
		printf ("CDATA (%s)\n", value);
		tmp_err_code += asciiToString(value, &chVal, &strm->memList, FALSE);
		tmp_err_code += serialize.stringData(strm, chVal);
		break;
	case	MXML_SAX_DIRECTIVE :
		//printf ("DIRECTIVE\n");
		break;
	case	MXML_SAX_COMMENT :
		printf ("COMMENT\n");
		break;
	default :
		printf ("SAX event %d not handled\n", event);
		break;
	}
}

void usage(char *prog) {
	printf ("%s -f <xmlfile> [-s <comma separated list of exs files>] [-I schemaID]\n", prog);
	printf ("%s -d : demo mode\n", prog);
	printf ("%s -s <comma separated list of exs files> -v : validate exs\n", prog);
	printf ("%s -f <exifile> : parses .exi file\n", prog);
}

char *outfile = NULL;
boolean exi2xml = FALSE;

int main(int argc, char **argv) {
	EXIStream testStrm;
	EXIPSchema schema, *schemaPtr = NULL;
	String uri;
	String ln;
	QName qname= {&uri, &ln};
	String chVal;
	char buf[OUTPUT_BUFFER_SIZE];
	errorCode tmp_err_code = 0;
	BinaryBuffer buffer;
	int	opt;
	boolean validate = FALSE;
	char *schemaID = NULL;
	struct appData parsingData;

	buffer.buf = buf;
	buffer.bufContent = 0;
	buffer.bufLen = OUTPUT_BUFFER_SIZE;
	buffer.ioStrm.readWriteToStream = NULL;
	buffer.ioStrm.stream = NULL;

	char files[1024] = "";
	char *xmlfile = NULL;

	while ((opt = getopt(argc, argv, "f:s:hdvI:o:")) != -1) {
		switch (opt) {
		case 'f' :
			xmlfile = optarg;
			break;
		case 's' :
			strcpy (files, optarg);
			break;
		case 'd' :
			strcpy (files, "common.exs,application.exs,contentInstances.exs");
			xmlfile = "test.xml";
			schemaID = "application";
			printf ("./sax.x -s %s -f %s -I %s\n", files, xmlfile, schemaID);
			break;
		case 'v' :
			validate = TRUE;
			break;
		case 'I' :
			schemaID = optarg;
			break;
		case 'o' :
			outfile = optarg;
			break;
		case 'h' :
		default:
			usage(argv[0]);
			exit( 1 );
		}
	}

info();
	if (*files && parseSchema(files, &schema) == 0) {
		schemaPtr	= &schema;
	}

	if	(validate)
		exit (0);

	if	(!xmlfile) {
		usage(argv[0]);
		exit( 1 );
	}

	if	(strstr(xmlfile, ".exi")) {
		if	(outfile) {
			exi2xml = TRUE;
			addNameSpace ("xsi", "http://www.w3.org/2001/XMLSchema-instance");
			//addNameSpace ("", "http://www.plcopen.org/xml/tc6.xsd");
		}
		int fd = open(xmlfile, 0);
		buffer.bufContent = read (fd, buffer.buf, OUTPUT_BUFFER_SIZE);
		close (fd);
		goto parsing;
	}

info();
	serialize.initHeader(&testStrm);

	testStrm.header.has_options = TRUE;

	if	(schemaPtr && schemaID) {
		tmp_err_code = asciiToString(schemaID, &testStrm.header.opts.schemaID, &testStrm.memList, FALSE); check("")
		printf ("### schemaID %s\n", schemaID);
		testStrm.header.opts.schemaIDMode = SCHEMA_ID_SET;
	}
	else {
		testStrm.header.opts.schemaIDMode = SCHEMA_ID_EMPTY;
	}

	printf ("### initStream\n");
	tmp_err_code = serialize.initStream(&testStrm, buffer, schemaPtr); check("")
	printf ("### exiHeader\n");
	tmp_err_code = serialize.exiHeader(&testStrm); check("")
	printf ("### startDocument\n");
	tmp_err_code = serialize.startDocument(&testStrm); check("SD")

info();
	FILE *fp = fopen(xmlfile, "r");
	if (!fp)
		exit(1);
	mxml_node_t *top = mxmlSAXLoadFile(NULL, fp, MXML_OPAQUE_CALLBACK, sax_cb, &testStrm);
	//mxmlDelete (top);

info();
	tmp_err_code += serialize.endDocument(&testStrm); check("ED")

	// V: Free the memory allocated by the EXI stream object
	tmp_err_code = serialize.closeEXIStream(&testStrm); check("")

	if (tmp_err_code == ERR_OK)
		greenMsg ("### ENCODING SUCCESS sz=%d", testStrm.context.bufferIndx + 1);

	if	(outfile)
		saveToFile (outfile, buffer.buf, testStrm.context.bufferIndx + 1);

info();
	printf ("\n### START PARSING\n");

	//	DECODING

	Parser testParser;

	//buffer.bufContent = OUTPUT_BUFFER_SIZE;
	buffer.bufContent = testStrm.context.bufferIndx + 1;
parsing :
	parsingData.expectAttributeData = FALSE;
	parsingData.stack = NULL;
	if	(exi2xml)
		parsingData.outputFormat = OUT_XML;
	tmp_err_code = initParser(&testParser, buffer, &parsingData); check("")

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

	tmp_err_code = parseHeader(&testParser, FALSE); check("")

	// IV.1: Set the schema to be used for parsing.
	// The schemaID mode and schemaID field can be read at
	// parser.strm.header.opts.schemaIDMode and
	// parser.strm.header.opts.schemaID respectively
	// If schemaless mode, use setSchema(&parser, NULL);

	if	(testParser.strm.header.opts.schemaIDMode == SCHEMA_ID_SET) {
		printf ("### schemaID : ");
		printString (&testParser.strm.header.opts.schemaID);
		printf ("\n");
	}

	TRY(setSchema(&testParser, schemaPtr));

	while(tmp_err_code == ERR_OK)
	{
		tmp_err_code = parseNext(&testParser);
	}

	destroyParser(&testParser);

	if (tmp_err_code == PARSING_COMPLETE)
		printf ("### PARSING SUCCESS\n");
	else
		printf ("### tmp_err_code = %d\n", tmp_err_code);

	return	0;
}

static void printURI(const String *str) {
	char *prefix = getPrefixByNs_String(str);
	if	(prefix)
		printf ("%s", prefix);
	else
		fwrite (str->str, sizeof(CharType), str->length, stdout);
}

void printQName(const QName qname) {
	printURI (qname.uri);
	printf(":");
	printString(qname.localName);
}



mxml_node_t *root;

static errorCode sample_fatalError(const errorCode code, const char* msg, void* app_data)
{
	printf("\n%d : FATAL ERROR: %s\n", code, msg);
	mxmlDelete (root);
	return EXIP_HANDLER_STOP;
}

static errorCode sample_startDocument(void* app_data) {
	struct appData* appD = (struct appData*) app_data;
	if(appD->outputFormat == OUT_XML) {
		root = mxmlNewXML ("1.0");
	}
	else
		printf ("SD\n");
	return ERR_OK;
}

static errorCode sample_endDocument(void* app_data) {
	struct appData* appD = (struct appData*) app_data;
	if(appD->outputFormat == OUT_XML) {
		int fd = open(outfile, O_RDWR|O_CREAT|O_TRUNC, 0644);
		mxmlSaveFd (root, fd, MXML_NO_CALLBACK);
		close(fd);
	}
	else
		printf ("ED\n");
	return ERR_OK;
}

static errorCode sample_startElement(QName qname, void* app_data) {
	struct appData* appD = (struct appData*) app_data;
	if(appD->outputFormat == OUT_XML) {
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

		mxml_node_t *parent = firstElement ? root : appD->stack->node;

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
	}
	else {
		printf("### SE ");
		printQName (qname);
		printf("\n");
	}

	return ERR_OK;
}

static errorCode sample_endElement(void* app_data) {
	struct appData* appD = (struct appData*) app_data;
	if(appD->outputFormat == OUT_XML) {
		struct element* el;
		el = pop(&(appD->stack));
		destroyElement(el);
	}
	else
		printf ("EE\n");
	return ERR_OK;
}

static errorCode sample_attribute(QName qname, void* app_data) {
	struct appData* appD = (struct appData*) app_data;
	if(appD->outputFormat == OUT_XML) {
		*appD->nameBuf = 0;

		if(!isStringEmpty(qname.uri)) {
			char *pfx = getPrefixByNs_String (qname.uri);
			if	(pfx && *pfx)
				sprintf (appD->nameBuf, "%s:", pfx);
		}
		string2Ascii (qname.localName, appD->nameBuf+strlen(appD->nameBuf));
	}
	else {
		printf("### AT ");
		printQName (qname);
		printf("\n");
	}
	appD->expectAttributeData = TRUE;
	return ERR_OK;
}

static errorCode sample_stringData(const String value, void* app_data) {
	struct appData* appD = (struct appData*) app_data;
	if(appD->outputFormat == OUT_XML) {
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
	}
	else {
		printf("### CH ");
		printString(&value);
		printf("\n");
	}
	return ERR_OK;
}

static errorCode sample_decimalData(Decimal value, void* app_data) {
	printf("### decimalData %g\n", (double)value);
	return ERR_OK;
}

static errorCode sample_intData(Integer int_val, void* app_data) {
	printf("### intData %lld\n", (long long int) int_val);
	return ERR_OK;
}

static errorCode sample_booleanData(boolean bool_val, void* app_data) {
	printf("### booleanData %s\n", bool_val ? "true":"false");
	return ERR_OK;
}

static errorCode sample_floatData(Float fl_val, void* app_data) {
	printf("### floatData %lldE%d\n", fl_val.mantissa, fl_val.exponent);
	return ERR_OK;
}

static errorCode sample_dateTimeData(EXIPDateTime dt_val, void* app_data) {
	char	avalue[200];
	EXIP_to_iso8601 (&dt_val, avalue);
	printf ("### dateTimeData %s\n", avalue);
	return ERR_OK;
}

static errorCode sample_binaryData(const char* binary_val, Index nbytes, void* app_data) {
	printf ("### binaryData\n");
	return ERR_OK;
}

static errorCode sample_qnameData(const QName qname, void* app_data) {
	printf ("### qnameData ");
	printQName (qname);
	printf("\n");
	return ERR_OK;
}


#define MAX_XSD_FILES_COUNT 10 // up to 10 XSD files

/**
 * @brief Parses schema files.
 * @param[in] xsdList List of schema files
 * @param[out] schema EXI schema
 * @return 0 if success, -1 if error
 */
static int parseSchema(char* xsdList, EXIPSchema* schema) {
	errorCode tmp_err_code = UNEXPECTED_ERROR;
	FILE *schemaFile;
	BinaryBuffer buffer[MAX_XSD_FILES_COUNT];
	char schemaFileName[50];
	unsigned int schemaFilesCount = 0;
	char *token;

	for (token = strtok(xsdList, "=,") ; token != NULL; token = strtok(NULL, "=,"))
	{
		printf ("%d %s\n", schemaFilesCount, token);
		if(schemaFilesCount > MAX_XSD_FILES_COUNT)
		{
			fprintf(stderr, "Too many xsd files given as an input: %d\n", schemaFilesCount);
			return 0;
		}

		strcpy(schemaFileName, token);
		schemaFile = fopen(schemaFileName, "rb" );
		if(!schemaFile)
		{
			fprintf(stderr, "Unable to open file %s\n", schemaFileName);
			continue;
		}

		//Get file length
		fseek(schemaFile, 0, SEEK_END);
		buffer[schemaFilesCount].bufLen = ftell(schemaFile) + 1;
		fseek(schemaFile, 0, SEEK_SET);

		//Allocate memory
		buffer[schemaFilesCount].buf = (char *) malloc(buffer[schemaFilesCount].bufLen);
		if (!buffer[schemaFilesCount].buf)
		{
			fprintf(stderr, "Memory allocation error!\n");
			fclose(schemaFile);
			return -1;
		}

		//Read file contents into buffer
		fread(buffer[schemaFilesCount].buf, buffer[schemaFilesCount].bufLen, 1, schemaFile);
		fclose(schemaFile);

		buffer[schemaFilesCount].bufContent = buffer[schemaFilesCount].bufLen;
		buffer[schemaFilesCount].ioStrm.readWriteToStream = NULL;
		buffer[schemaFilesCount].ioStrm.stream = NULL;
		schemaFilesCount++;
	}

	if	(!schemaFilesCount)
		return -1;

	// Generate the EXI grammars based on the schema information
	tmp_err_code = generateSchemaInformedGrammars(buffer, schemaFilesCount, SCHEMA_FORMAT_XSD_EXI, NULL, schema);

	int i;
	for(i = 0; i < schemaFilesCount; i++)
	{
		free(buffer[i].buf);
	}

	if(tmp_err_code != ERR_OK)
		printf("Grammar generation error occurred: %d\n", tmp_err_code);
	else
		printf("Grammar generation SUCCESS\n");
	return 0;
}


// Stuff needed for the OUT_XML Output Format
// ******************************************
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

static struct element* createElement(char* name, mxml_node_t *node)
{
	struct element* el;
	el = malloc(sizeof(struct element));
	if(el == NULL)
		exit(1);
	el->next = NULL;
	el->name = strdup(name);
	el->node = node;
	return el;
}

static void destroyElement(struct element* el)
{
	free(el->name);
	free(el);
}
