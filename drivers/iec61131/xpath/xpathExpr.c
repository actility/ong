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
#include <mxml.h>

#ifdef MAIN
#include <malloc.h>
#include <unitTest.h>

void disp_mallinfo() {
	struct mallinfo info = mallinfo();
	printf ("mallinfo : total allocated space:  %d bytes\n", info.uordblks);
}

/**
 * @brief Retrieve the text contained in a xml token, eg <tok>content</tok>
 * @param node The xml node
 * @return The contained text
 */
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
#endif

char *XpathExpression(mxml_node_t *top, char *expr) {
	char *res = NULL;
	char *attr = NULL;
	char *val = NULL;

	if	(*expr == '/')
		expr ++;
	char *pt = strchr(expr, '/');
	if	(pt)
		*pt = 0;
	else {
		pt = strstr(expr, "[@");
		if	(pt) {
			*pt = 0;
			attr = pt+2;
			val = strchr(attr, '=');
			pt = strchr(attr, ']');
			if	(!pt)
				return NULL;
			*pt = 0;
			pt = NULL;
			if (val) {
				*val++ = 0;
				if	(*val == '\'')
					val ++;
				if	(val[strlen(val)-1] == '\'')
					val[strlen(val)-1] = 0;
			}
		//printf ("attr (%s) val (%s)\n", attr, val);
		}
	}
	//printf ("expr=%s attr=%s val=%s\n", expr, expr, val);

	mxml_node_t *node = mxmlFindElement(top, top, expr, attr, val, MXML_DESCEND);
	if	(node) {
		if	(pt) {
			res = XpathExpression (node, pt+1);
		}
		else if (attr) {
			res = (char *)mxmlElementGetAttr(node, attr);
		}
		else {
			res = getContainedText(node);
			//printf ("getContainedText = (%s)\n", res);
		}
	}
	else {
		//printf ("### FAILED mxmlFindElement(%s)\n", expr);
	}
	return res;
}

#ifdef MAIN

void
do_test(char *fn, char *expr, char *expected, char *detail) {
	FILE *fp;
	mxml_node_t *script;
	char *res;

	fp = fopen(fn, "r");
	script = mxmlLoadFile(NULL, fp, MXML_OPAQUE_CALLBACK);
	expr = strdup(expr);
	res = XpathExpression(script, expr);
	//printf ("[32;7m%s[0m\n", res);
	STAssertStringEquals (res, expected, detail);
	fclose (fp);
	mxmlDelete (script);
	free (expr);
}

int
do_all() {
	do_test ("sample1.xml", "/foo/bar", "Message", "Test #1 : expression /foo/bar");
	do_test ("sample2.xml", "/foo/bar[@attr]", "Hello", "Test #2 : expression /foo/bar[@attr]");
	do_test ("sample2.xml", "/foo/bar[@attr='Hello']", "Hello", "Test #3 : expression /foo/bar[@attr='Hello']");
	do_test ("sample2.xml", "/foo/bar[@attr='World']", NULL, "Test #4 : expression /foo/bar[@attr='World']");

	STStatistics();
	if	(STErrorCount() != 0)
		return 1;
	return	0;
}

int
main(int ac, char **av) {
	if	(!strcmp(av[1], "-test")) {
		int rc = do_all();
		exit (rc);
	}

	FILE *fp = fopen(av[1], "r");
	if	(!fp)
		return	1;
	//disp_mallinfo();
	mxml_node_t *script = mxmlLoadFile(NULL, fp, MXML_OPAQUE_CALLBACK);
	//disp_mallinfo();
	fclose (fp);

	char *res = XpathExpression(script, av[2]);
	printf ("[32;7m%s[0m\n", res);

	mxmlDelete (script);
	return	0;
}
#endif
