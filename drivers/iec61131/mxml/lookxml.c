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

#include "unistd.h"
#include "malloc.h"
#include "mxml.h"

void
disp_mallinfo() {
	struct mallinfo info = mallinfo();
	printf ("### mallinfo : total allocated space:  %d bytes\n", info.uordblks);
}

int isEmpty(char *pt) {
	while	(*pt) {
		if	(*pt != '\n' && *pt != '\r' && *pt != ' ' && *pt != '\t')
			return 0;
		pt++;
	}
	return 1;
}

int main (int argc, char **argv ) {
	FILE *fp  = NULL;
	int k = 0;
	mxml_node_t * tree = NULL;
	mxml_node_t * node  = NULL;

	if (argc < 2){
		perror("Argument Required XML File ");
		exit(1);
	}
	fp = fopen (argv[1], "r");
	if	(!fp) {
		perror("Could Not Open the File Provided");
		exit(1);
	}

printf ("mxml_node_t = %d\n", sizeof(mxml_node_t));
printf ("mxml_value_t = %d\n", sizeof(mxml_value_t));

disp_mallinfo();
	tree = mxmlLoadFile (NULL , fp , MXML_OPAQUE_CALLBACK);
disp_mallinfo();
	//tree = mxmlLoadFile (NULL , fp , MXML_TEXT_CALLBACK);

	if	(!tree)
		exit (1);

	int count = 0;
	for (node = mxmlFindElement(tree, tree,NULL,NULL, NULL,MXML_DESCEND);
		node != NULL; )
		//node=mxmlWalkNext (node, NULL, MXML_DESCEND)
		//node = mxmlFindElement(node, tree, NULL,NULL,NULL,MXML_DESCEND)
	{
		if (node->type == MXML_ELEMENT) {
			if	(!strcmp(node->value.element.name, "position")
				|| !strcmp(node->value.element.name, "relPosition")) {
				mxml_node_t *n2 = mxmlWalkNext (node, NULL, MXML_DESCEND);
				mxmlDelete (node);
				node = n2;
				continue;
			}
			printf("MXML_ELEMENT Node %s:%d \n", node->value.element.name, node->value.element.num_attrs);
			mxmlElementDeleteAttr (node, "width");
			mxmlElementDeleteAttr (node, "height");
			for (k = 0; k < node->value.element.num_attrs; k++){
				if (node->value.element.attrs ){
					printf ("  Attr Name  : %s \n", node->value.element.attrs[k].name);
					printf ("  Attr Value : %s \n", node->value.element.attrs[k].value);
				}
			}
		}
		else if (node->type == MXML_REAL)
			printf("MXML_REAL Node is %s\n", node->value.element.name);
		else if(node->type == MXML_OPAQUE) {
			char *pt = node->value.element.name;
			if (isEmpty(pt)) {
/*
				mxml_node_t *parent = node->parent;
				mxmlDelete (node);
				node = parent;
*/
				mxml_node_t *n2 = mxmlWalkNext (node, NULL, MXML_DESCEND);
				mxmlDelete (node);
				node = n2;
				continue;
			}
			printf("MXML_OPAQUE Node is %s\n", pt);
		}
		else if(node->type == MXML_INTEGER)
			printf("MXML_INTEGER Node is %s\n", node->value.element.name);
		else if(node->type == MXML_TEXT)
			printf("MXML_TEXT Node is %s\n", node->value.element.name);
		else if(node->type == MXML_IGNORE)
			printf("MXML_IGNORE Node is %s\n", node->value.element.name);
		else if(node->type == MXML_CUSTOM)
			printf("MXML_IGNORE Node is %s\n", node->value.element.name);
		else
			printf("Type Default Node is %s\n", node->value.element.name);
		count ++;
		node=mxmlWalkNext (node, NULL, MXML_DESCEND);
	}
	printf ("### Elements count %d\n", count);
disp_mallinfo();

	mxmlDelete(tree);
disp_mallinfo();
	fclose (fp);
disp_mallinfo();
	return 0;
}
