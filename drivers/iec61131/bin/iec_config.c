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
#include <sys/time.h>
#include <sys/stat.h>
#include <mxml.h>
#include <rtlbase.h>
#include <rtlhtbl.h>

#include "iec_debug.h"
#include "plc.h"
#include "operating.h"



#define	IEC_LOADCONFIG_TIME 10
char IEC_CONFIG_FILE[256] = "";

extern iec_global_t GLOB;


/**
 * @brief Load configuration file into a hashtable (param_name, value)
 * Return : -1 if error, 1 if something has changed, else 0
 */
int IEC_loadConfig() {
	static int _inited = 0;
	static time_t next = 0;
	static time_t prev_load_ok = 0;
	time_t now = time(0);

	if	(!_inited) {
		sprintf (IEC_CONFIG_FILE, "%s/spvconfig.xml", Operating_ConfigDir());
		next	= now + IEC_LOADCONFIG_TIME;
		_inited = 1;
	}
	else {
		if	(now < next)
			return 0;
		next	+= IEC_LOADCONFIG_TIME;
		struct stat st;
		if	(stat(IEC_CONFIG_FILE, &st) < 0)
			return -1;
		if	(st.st_mtime < prev_load_ok)
			return 0;
	}

	FILE *fp = fopen(IEC_CONFIG_FILE, "r");
	if	(!fp) {
		RTL_TRDBG (TRACE_ERROR, "ERROR open %s\n", IEC_CONFIG_FILE);
		return -1;
	}
	mxml_node_t *doc = mxmlLoadFile(NULL, fp, MXML_OPAQUE_CALLBACK);
	fclose (fp);
	if	(!doc) {
		RTL_TRDBG (TRACE_ERROR, "ERROR xml %s\n", IEC_CONFIG_FILE);
		return -1;
	}

	prev_load_ok = now;

	if	(GLOB.config) {
/* no more because of HTBL_FREE_DATA
		inline void _fct(char *key, void *var) { free(var); }
		rtl_htblDump(GLOB.config, _fct);
*/
		rtl_htblDestroy(GLOB.config);
	}

	//GLOB.config = rtl_htblCreate(32, NULL);
	GLOB.config = rtl_htblCreateSpec(32, NULL, HTBL_KEY_STRING|HTBL_FREE_DATA);

	mxml_node_t *node = doc;

	while (1) {
		node = mxmlFindElement(node, doc, NULL, NULL, NULL, MXML_DESCEND);
		if	(!node)
			break;
		if (node->type == MXML_ELEMENT) {
			char *val = (char *)mxmlElementGetAttr(node, "val");
			if	(val) {
				char *name = (char *)mxmlElementGetAttr(node, "name");
				//printf ("%s %s %s\n", node->value.element.name, name, val);
				rtl_htblInsert (GLOB.config, name, strdup(val));
			}
		}
	}
	mxmlRelease (doc);
	return	1;
}

char *IEC_Config (char *param) {
	if	(!GLOB.config)
		return NULL;
	return rtl_htblGet(GLOB.config, param);
}

char *IEC_Config_Default (char *param, char *def) {
	if	(!GLOB.config)
		return def;
	char *res = rtl_htblGet(GLOB.config, param);
	if	(!res)
		res = def;
	return res;
}

int IEC_Config_Int_Default (char *param, int def) {
	if	(!GLOB.config)
		return def;
	char *res = rtl_htblGet(GLOB.config, param);
	if	(!res)
		return def;
	return atoi(res);
}

void IEC_AddConfig (char *param, char *value) {
	if	(GLOB.config) {
		rtl_htblRemove (GLOB.config, param);
		rtl_htblInsert (GLOB.config, param, strdup(value));
	}
}
