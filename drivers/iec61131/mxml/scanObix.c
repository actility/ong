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
#include <stddef.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <mxml.h>
#include <rtlbase.h>


void scan_obix(mxml_node_t *node) {
	char *obix_type = node->value.element.name;
	char *obix_val = (char *)mxmlElementGetAttr(node, "val");
	printf ("Obix : %s / %s\n", obix_type, obix_val);
}

char *getContainedText(mxml_node_t *node) {
	char *content = NULL;
	node	= mxmlWalkNext (node, NULL, MXML_DESCEND);
	if	(node && node->type == MXML_OPAQUE) {
		content = (char *)mxmlGetOpaque (node);
		if	(content && !strcmp(content, "\n")) {
			node	= mxmlWalkNext (node, NULL, MXML_DESCEND);
			content = (char *)mxmlGetCDATA(node);
		}
	}
	return content;
}

char *base64_decode_alloc(char *from, unsigned int len) {
	int target_size = (double)len*3.0/4.0 + 10;
	char *out = malloc(target_size);
	long sz = rtl_base64_decode (out, from, len);
	if	(sz < 0) {
		free (out);
		return NULL;
	}
	return out;
}

int scan_notify(mxml_node_t *top) {
	//IEC_DT	lastModifiedTime;

	//	Level 1 decoding
	mxml_node_t *node = mxmlFindElement(top, top, "subscriptionReference", NULL, NULL, MXML_DESCEND);
	if	(!node) {
		//RTL_TRDBG (TRACE_ERROR, "subscriptionReference failed\n");
		return -1;
	}
/*
	char *uri = getContainedText(node);
	printf ("URI : [%s]\n", uri);
*/
	//char *path = pathFromUri(uri);

	//	Level 2 decoding
	node = mxmlFindElement(top, top, "representation", NULL, NULL, MXML_DESCEND);
	if	(!node) {
		//RTL_TRDBG (TRACE_ERROR, "representation failed\n");
		return -1;
	}
	char *content = getContainedText(node);
	char *out = base64_decode_alloc(content, strlen(content));
	if	(!out) {
		//RTL_TRDBG (TRACE_ERROR, "representation base64 error\n");
		return -1;
	}

	mxml_node_t *doc = mxmlLoadString(NULL, out, MXML_OPAQUE_CALLBACK);
	if	(!doc) {
		//RTL_TRDBG (TRACE_ERROR, "representation xml error\n");
		free (out);
		return -1;
	}

	//	Level 3 decoding
/*
	node = mxmlFindElement(doc, doc, "lastModifiedTime", NULL, NULL, MXML_DESCEND);
	if	(node) {
		iso8601_to_IECdate (getContainedText(node), &lastModifiedTime);
	}
*/
	node = mxmlFindElement(doc, doc, "m2m:content", NULL, NULL, MXML_DESCEND);
	if	(!node) {
		//RTL_TRDBG (TRACE_ERROR, "m2m:content failed\n");
		mxmlDelete (doc);
		free (out);
		return -1;
	}

	content = getContainedText(node);

	char *out2 = base64_decode_alloc(content, strlen(content));
	if	(!out2) {
		//RTL_TRDBG (TRACE_ERROR, "m2m:content base64 error\n");
		mxmlDelete (doc);
		free (out);
		return -1;
	}

	mxml_node_t *doc2 = mxmlLoadString(NULL, out2, MXML_OPAQUE_CALLBACK);
	if	(!doc2) {
		//RTL_TRDBG (TRACE_ERROR, "m2m:content xml error\n");
		free (out2);
		mxmlDelete (doc);
		free (out);
		return -1;
	}

//<real xmlns="http://obix.org/ns/schema/1.1" val="24.00" x-template="dat_inst_0402_0000_0.xml" x-date="2013-06-19T06:37:43.764-07:00"/>
/*
	node = mxmlFindElement(doc2, doc2, "obix:obj", NULL, NULL, MXML_DESCEND);
	if	(!node) {
		RTL_TRDBG (TRACE_ERROR, "obix:obj failed\n");
		mxmlDelete (doc2);
		return -1;
	}
*/
	node = mxmlFindElement(doc2, doc2, NULL, NULL, NULL, MXML_DESCEND);
	if	(!node) {
		//RTL_TRDBG (TRACE_ERROR, "obix:obj failed\n");
		mxmlDelete (doc2);
		free (out2);
		mxmlDelete (doc);
		free (out);
		return -1;
	}

	char *obix_type = node->value.element.name;
	char *obix_val = (char *)mxmlElementGetAttr(node, "val");
	char *x_date = (char *)mxmlElementGetAttr(node, "x-date");
	//iso8601_to_IECdate (x_date, &lastModifiedTime);

	printf ("type=%s val=%s date=%s\n", obix_type, obix_val, x_date);

	mxmlDelete (doc2);
	free (out2);
	mxmlDelete (doc);
	free (out);
	return 0;
}

void scan_doc(mxml_node_t *top) {
	mxml_node_t *node = mxmlFindElement(top, top, NULL, NULL, NULL, MXML_DESCEND);

	if (!node || node->type != MXML_ELEMENT)
		return;

	//printf ("%s\n", node->value.element.name);
	if	(!strcasecmp(node->value.element.name, "m2m:notify"))
		scan_notify(node);
	else
		scan_obix(node);
}

/*
<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n<real val=3.5 xmlns=\"http://obix.org/ns/schema/1.1\"/>
*/

int main(int ac, char **av) {
/*
	FILE *fp = fopen(av[1], "r");
	if	(!fp)
		return	1;
	mxml_node_t *script = mxmlLoadFile(NULL, fp, MXML_OPAQUE_CALLBACK);
	fclose (fp);
*/
	int fd = open(av[1], 0);
	char tmp[100*1024];
	int n = read (fd, tmp, sizeof(tmp));
	tmp[n] = 0;
	close (fd);

	mxml_node_t *script = mxmlLoadString(NULL, tmp, MXML_OPAQUE_CALLBACK);

	scan_doc(script);
	mxmlDelete (script);

	return	0;
}
