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
#include <malloc.h>
#include <mxml.h>
#include <rtlbase.h>
#include "iec_debug.h"

void disp_mallinfo() {
	struct mallinfo info = mallinfo();
	RTL_TRDBG (TRACE_DETAIL, "### mallinfo : total allocated space:  %d bytes\n", info.uordblks);
}

int isEmpty(char *pt) {
	if	(!pt)	return 1;
	while	(*pt) {
		if	(*pt != '\n' && *pt != '\r' && *pt != ' ' && *pt != '\t')
			return 0;
		pt++;
	}
	return 1;
}

#if 0 // OBSOLETE
void mxmlReduce (mxml_node_t *tree) {
	mxml_node_t * node;

	if	(!tree) return;

	disp_mallinfo();
	int initial = 0;
	for (node = mxmlFindElement(tree, tree,NULL,NULL, NULL,MXML_DESCEND); node; initial++)
		node=mxmlWalkNext (node, NULL, MXML_DESCEND);

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

	int final = 0;
	for (node = mxmlFindElement(tree, tree,NULL,NULL, NULL,MXML_DESCEND); node; final++)
		node=mxmlWalkNext (node, NULL, MXML_DESCEND);

	RTL_TRDBG (TRACE_DETAIL, "Elements count before reduce %d, after reduce %d\n", initial, final);
	disp_mallinfo();
}
#endif // OBSOLETE

/*
 * Reduce the size by removing some attributes.
 */
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

/*
 * SAX callback. Usage :
        mxml_node_t *new_pous = mxmlSAXLoadFile (NULL , fp , MXML_OPAQUE_CALLBACK, iec_sax_cb, NULL);
        mxmlReduceAttrs (new_pous);
 */
void iec_sax_cb(mxml_node_t *node, mxml_sax_event_t event, void *data) {
	const char *name;
	if ((node->type == MXML_OPAQUE) && isEmpty(node->value.element.name))
		return;

	switch (event) {
	case MXML_SAX_ELEMENT_OPEN :
/* due to a bug in mxml library, moved in mxmlReduceAttrs()
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

