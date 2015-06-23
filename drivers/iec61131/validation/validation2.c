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
#include <mxml.h>
#include <rtlbase.h>

#include "iec_debug.h"
#include "mxml-reduce.h"
#include "validation.h"

int TraceDebug=1;
int TraceLevel=9;

int main(int ac, char **av) {
	int err=0;

	rtl_tracelevel(TraceLevel);

	while	(--ac) {
		char *file = *++av;
		RTL_TRDBG (TRACE_DETAIL, "Validation %s\n", file);

		FILE *fp = fopen(file, "r");
		void *script = mxmlSAXLoadFile (NULL , fp , MXML_OPAQUE_CALLBACK, iec_sax_cb, NULL);
		mxmlReduceAttrs (script);
		fclose (fp);

		int rc;
		if	(script) {
			rc = PLCvalidation(script);
		}
		else	rc = -1;

		if	(rc < 0) {
			RTL_TRDBG (TRACE_DETAIL, "\033[31;7m%s validation FAILED\033[0m\n", file);
			err ++;
		}
		else
			RTL_TRDBG (TRACE_DETAIL, "\033[32;7m%s validation SUCCESS\033[0m\n", file);
	}
	exit (err);
}
