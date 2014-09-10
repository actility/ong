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

#include <unistd.h>
#include <malloc.h>
#include <mxml.h>

#include <rtlhtbl.h>

int g_withDict = 0;


void disp_mallinfo() {
	struct mallinfo info = mallinfo();
	fprintf (stderr, "### mallinfo : total allocated space:  %d bytes\n", info.uordblks);
}

int isEmpty(const char *pt) {
	if	(!pt)
		return 0;
	while	(*pt) {
		if	(*pt != '\n' && *pt != '\r' && *pt != ' ' && *pt != '\t')
			return 0;
		pt++;
	}
	return 1;
}

static int ini = 0;
static int fini = 0;
void countIni(mxml_node_t *tree) {
	mxml_node_t *node;
	for (node = mxmlFindElement(tree, tree,NULL,NULL, NULL,MXML_DESCEND); node; ini++)
		node=mxmlWalkNext (node, NULL, MXML_DESCEND);
}

void countFini(mxml_node_t *tree) {
	mxml_node_t *node;
	for (node = mxmlFindElement(tree, tree,NULL,NULL, NULL,MXML_DESCEND); node; fini++)
		node=mxmlWalkNext (node, NULL, MXML_DESCEND);
	fprintf (stderr, "### Elements count %d/%d\n", fini, ini);
}


void mxmlReduce (mxml_node_t *tree) {
	mxml_node_t * node;

	if	(!tree) return;

	for (node = mxmlFindElement(tree, tree,NULL,NULL, NULL,MXML_DESCEND); node; ) {
		switch (node->type) {
		case MXML_ELEMENT :
			if	(!strcmp(node->value.element.name, "position")
				|| !strcmp(node->value.element.name, "relPosition")
				|| !strcmp(node->value.element.name, "connectionPointOut")
				|| !strcmp(node->value.element.name, "connectionPointOutAction")
				|| !strcmp(node->value.element.name, "fileHeader")
				|| !strcmp(node->value.element.name, "contentHeader")
				|| !strcmp(node->value.element.name, "comment")
				|| !strcmp(node->value.element.name, "documentation")
				)
			{
				mxml_node_t *parent = node->parent;
				mxmlDelete (node);
				node = parent;
				continue;
			}
			mxmlElementDeleteAttr (node, "width");
			mxmlElementDeleteAttr (node, "height");
			mxmlElementDeleteAttr (node, "x");
			mxmlElementDeleteAttr (node, "y");
			break;
		case MXML_OPAQUE :
			if (isEmpty(node->value.element.name)) {
				mxml_node_t *parent = node->parent;
				mxmlDelete (node);
				node = parent;
				continue;
			}
			break;
		default :
			break;
		}
		node=mxmlWalkNext (node, NULL, MXML_DESCEND);
	}

/*
	FILE *fout = fopen("/tmp/out.xml", "a");
	mxmlSaveFile (tree, fout, NULL);
	fclose (fout);
*/
}

mxml_type_t				/* O - Node type */
mxml_filter_cb(mxml_node_t *node)	/* I - Current node */
{
	//printf ("filter_cb %s\n", node->value.element.name);

	mxmlElementDeleteAttr (node, "width");
	mxmlElementDeleteAttr (node, "height");
	mxmlElementDeleteAttr (node, "x");
	mxmlElementDeleteAttr (node, "y");

	if	(!strcmp(node->value.element.name, "position")
		|| !strcmp(node->value.element.name, "relPosition")
		|| !strcmp(node->value.element.name, "connectionPointOut")
		|| !strcmp(node->value.element.name, "connectionPointOutAction")
		|| !strcmp(node->value.element.name, "fileHeader")
		|| !strcmp(node->value.element.name, "contentHeader")
		|| !strcmp(node->value.element.name, "comment")
		)
	{
		//printf ("  MXML_IGNORE\n");
		return MXML_IGNORE;
	}
/*
	else if(node->type == MXML_OPAQUE) {
		if (isEmpty(node->value.element.name)) {
			return MXML_IGNORE;
		}
	}
*/
	return MXML_OPAQUE;
}

// Replaces element's name by a short code taken from the dictionnary
void mxml_Dictionnary_Element (mxml_node_t *node) {
	static void *g_Dict = NULL;
	static int g_Code = 0;
	const char *name = mxmlGetElement(node);

	if	(!name) return;

	if	(!g_Dict)
		g_Dict = rtl_htblCreate(32, NULL);

	char *code = rtl_htblGet(g_Dict, (char *)name);
	if	(!code) {
		char tmp[20];
		sprintf (tmp, "e%d", g_Code++);
		code = strdup(tmp);
		rtl_htblInsert (g_Dict, strdup(name), code);
	}
	mxmlSetElement (node, code);

	int i;
	mxml_attr_t *attr;
	for (i = node->value.element.num_attrs, attr = node->value.element.attrs; i > 0; i --, attr ++) {
		char *code = rtl_htblGet(g_Dict, (char *)attr->name);
		if	(!code) {
			char tmp[20];
			sprintf (tmp, "a%d", g_Code++);
			code = strdup(tmp);
			rtl_htblInsert (g_Dict, strdup(attr->name), code);
		}
		free (attr->name);
		attr->name	= strdup(code);
	}
}

void mxml_Dictionnary (mxml_node_t *tree) {
	mxml_node_t *node = mxmlFindElement(tree, tree,NULL,NULL, NULL,MXML_DESCEND);
	while (node) {
		mxml_Dictionnary_Element (node);
		node=mxmlWalkNext (node, NULL, MXML_DESCEND);
	}
}

void mxmlReduceAttrs (mxml_node_t *tree) {
	mxml_node_t * node;

	if	(!tree) return;

	for (node = mxmlFindElement(tree, tree,NULL,NULL, NULL,MXML_DESCEND); node; ) {
		switch (node->type) {
		case MXML_ELEMENT :
			mxmlElementDeleteAttr (node, "width");
			mxmlElementDeleteAttr (node, "height");
			mxmlElementDeleteAttr (node, "x");
			mxmlElementDeleteAttr (node, "y");
			break;
		default :
			break;
		}
		node=mxmlWalkNext (node, NULL, MXML_DESCEND);
	}
}

void iec_sax_cb(mxml_node_t *node, mxml_sax_event_t event, void *data) {
	const char *name;
	if ((node->type == MXML_OPAQUE) && isEmpty(node->value.element.name))
		return;

	switch (event) {
	case MXML_SAX_ELEMENT_OPEN :
/*
		mxmlElementDeleteAttr (node, "width");
		mxmlElementDeleteAttr (node, "height");
		mxmlElementDeleteAttr (node, "x");
		mxmlElementDeleteAttr (node, "y");
*/
		name = mxmlGetElement(node);
		if	(!strcmp(name, "position")
			|| !strcmp(name, "relPosition")
			|| !strcmp(name, "connectionPointOut")
			|| !strcmp(name, "connectionPointOutAction")
			|| !strcmp(name, "fileHeader")
			|| !strcmp(name, "contentHeader")
			|| !strcmp(name, "comment")
			|| !strcmp(name, "documentation")
			)
		{
			// don't retain
		}
		else {
			mxmlRetain(node);
		}
		break;
	case MXML_SAX_DATA :
	case MXML_SAX_CDATA :
		if (mxmlGetRefCount(mxmlGetParent(node)) > 1) {
			//If the parent was retained, then retain this data node as well.
			mxmlRetain(node);
		}
		break;
	case MXML_SAX_DIRECTIVE :
		mxmlRetain(node);
		break;
	case MXML_SAX_COMMENT :
	case MXML_SAX_ELEMENT_CLOSE :
		// don't retain
		break;
	}
}

int main (int argc, char **argv ) {
	FILE *fp;
	mxml_node_t * tree = NULL;
	int	opt;
	char	*file = NULL;
	char	*type = "reduce";
	int	wflag = 0;

	while ((opt = getopt(argc, argv, "f:wt:d")) != -1) {
		switch (opt) {
		case 'f' :
			file	= optarg;
			break;
		case 'w' :
			wflag	= 1;
			break;
		case 't' :
			type	= optarg;
			break;
		case 'd' :
			g_withDict = 1;
			break;
		}
	}

	if (!file) {
		perror("Argument Required XML File ");
		exit(1);
	}
	fp = fopen (file, "r");
	if	(!fp) {
		perror("Could Not Open the File Provided");
		exit(1);
	}

	if	(!strcmp(type, "cb")) {
		disp_mallinfo();
		tree = mxmlLoadFile (NULL , fp , mxml_filter_cb);
		fclose (fp);
		disp_mallinfo();
		if	(!tree)
			exit (1);
	}
	else if	(!strcmp(type, "reduce")) {
		disp_mallinfo();
		tree = mxmlLoadFile (NULL , fp , MXML_OPAQUE_CALLBACK);
		disp_mallinfo();
		if	(!tree)
			exit (1);
		mxmlReduce(tree);
		disp_mallinfo();
		if	(g_withDict) {
			mxml_Dictionnary (tree);
			disp_mallinfo();
		}
	}
	else if	(!strcmp(type, "sax")) {
		disp_mallinfo();
		tree = mxmlSAXLoadFile (NULL , fp , MXML_OPAQUE_CALLBACK, iec_sax_cb, NULL);
		mxmlReduceAttrs (tree);
		fclose (fp);
		disp_mallinfo();
		if	(!tree)
			exit (1);
		if	(g_withDict) {
			mxml_Dictionnary (tree);
			disp_mallinfo();
		}
	}

	countFini (tree);
	if	(wflag) {
		mxmlSaveFile (tree, stdout, MXML_NO_CALLBACK);
	}

	mxmlDelete(tree);
	return 0;
}
