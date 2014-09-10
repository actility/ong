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
#include <strings.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>
#include <poll.h>
#include <sys/resource.h>

#include <mxml.h>

#include <rtlbase.h>
#include <rtllist.h>
#include <rtlhtbl.h>

#include "plc.h"
#include "iec_std_lib.h"
#include "iec_debug.h"
#include "literals.h"
#include "timeoper.h"
#include "mxml-reduce.h"
#include "vasprintf.h"
#include "watchdog.h"
#include "iec_config.h"
#include "cpu.h"
#include "operating.h"

#include "admin_cli.h"

extern int compileST(char *code);
extern void *programBlock;

static char _lastError[128];

int
PLCvalidation_precompileST(mxml_node_t *pou, mxml_node_t *ST) {
	ST	= mxmlWalkNext (ST, NULL, MXML_DESCEND);

	if	(ST->type != MXML_ELEMENT) {
		RTL_TRDBG (TRACE_ST, "precompile ST code : not an MXML_ELEMENT (%d)\n", ST->type);
		return 0;
	}

	char *STcode = ST->value.element.name;
	if	(!memcmp(STcode, "![CDATA[", 8))
		STcode += 8;
	while	(STcode[strlen(STcode)-1] == ']')
		STcode[strlen(STcode)-1] = 0;

	RTL_TRDBG (TRACE_ST, "precompile ST code : (%s)\n", STcode);

	if	(compileST (STcode) != 0) {
		char *name = (char *)mxmlElementGetAttr(pou, "name");
		sprintf (_lastError, "pou '%s' compilation failed '%20.20s'", name, STcode);
		return -1;
	}
	mxmlElementSetAttrf (ST, "tree", "%ld", programBlock);
	RTL_TRDBG (TRACE_ST, "ST code precompiled at %p\n", programBlock);
	return	0;
}

int
PLCvalidation_ST(mxml_node_t *pou) {
	mxml_node_t *ST = pou;
	int flg = MXML_DESCEND;
	while ((ST=mxmlFindElement(ST, pou, "ST", NULL, NULL, flg))) {
		if	(PLCvalidation_precompileST (pou, ST) < 0)
			return -1;
	}
	return	0;
}

int
PLCvalidation_pou(mxml_node_t *pou) {
	mxml_node_t *block = pou;
	int flg = MXML_DESCEND;

	while ((block=mxmlFindElement(block, pou, "block", NULL, NULL, flg))) {
		char *executionOrderId = (char *)mxmlElementGetAttr(block, "executionOrderId");
		if	(!executionOrderId) {
			char *name = (char *)mxmlElementGetAttr(pou, "name");
			char *localId = (char *)mxmlElementGetAttr(block, "localId");
			RTL_TRDBG (TRACE_DETAIL, "PLCvalidation No executionOrderId found for block localId %s\n", localId);
			sprintf (_lastError, "pou '%s' no executionOrderId for block '%s'", name, localId);
			return -1;
		}
		flg = MXML_NO_DESCEND;
	}
	if	(PLCvalidation_ST(pou) < 0)
		return -1;
	return	0;
}

int
PLCvalidation(mxml_node_t *project) {
	mxml_node_t *pous = mxmlFindElement(project, project, "pous", NULL, NULL, MXML_DESCEND);
	if	(!pous) {
		strcpy (_lastError, "No <pous> element in XML");
		return	-1;
	}

	mxml_node_t *pou = pous;
	int flg = MXML_DESCEND;
	while ((pou=mxmlFindElement(pou, pous, "pou", NULL, NULL, flg))) {
		int rc = PLCvalidation_pou(pou);
		if	(rc < 0) {
			char *name = (char *)mxmlElementGetAttr(pou, "name");
			RTL_TRDBG (TRACE_DETAIL, "PLCvalidation FAILED pou %s\n", name);
			return rc;
		}
		flg = MXML_NO_DESCEND;
	}
	return	0;
}

char *PLCvalidation_LastError() {
	return _lastError;
}
