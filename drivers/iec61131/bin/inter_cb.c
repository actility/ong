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

/*
	There are 3 callbacks from the ST interpreter :
	- cb_getVar to retrieve a variable
	- cb_setVar to set a variable
	- cb_callFunc to call a function
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <rtlbase.h>

#include "param_u.h"
#include "plc.h"
#include "iec_debug.h"
#include "vasprintf.h"

int cb_getVar (iec_program_t *program, const char *name, param_ret_t *pret) {
	//RTL_TRDBG (TRACE_DETAIL, "cb_getVar %s\n", name);
	iec_ctx_t *ctx = Context_Current(program);
	//ctx = getUpperContext (ctx);

	iec_variable_t *var = getVariable (ctx, (char *)name);
	if	(!var) {
/*
		pret->t	= DINTtype;
		pret->p.DINTvar	= 0;
		var = createVariableWithParameter2 ((char *)name, pret);
		addVariableInContext (ctx, var);
*/
		RTL_TRDBG (TRACE_ERROR, "\033[31;7mERROR setVar %s : variable not declared\033[0m\n", name);
		return -1;
	}
	pret->t	= var->type;
	pret->p	= var->value;
	return	0;
}

int cb_setVar (iec_program_t *program, const char *name, param_ret_t *pret) {
	iec_ctx_t *ctx = Context_Current(program);
	int rc = updateVariableWithParameter(ctx, (char *)name, pret);
	if	(rc == -1) {
		RTL_TRDBG (TRACE_ERROR, "\033[31;7mERROR setVar %s : variable not declared\033[0m\n", name);
		return -1;
	}
	if	(rc == -2) {
		RTL_TRDBG (TRACE_ERROR, "\033[31;7mERROR setVar %s : incorrect Data Type\033[0m\n", name);
		return -1;
	}
	if	(rc <0) {
		return -1;
	}
	return	0;
}

int cb_getType(iec_program_t *program, const char *name) {
	//RTL_TRDBG (TRACE_DETAIL, "cb_getType %s\n", name);
	iec_ctx_t *ctx = Context_Current(program);
	//ctx = getUpperContext (ctx);

	iec_variable_t *var = getVariable (ctx, (char *)name);
	if	(!var)
		return	-1;
	return	var->type;
}

int cb_callFunc (void *program, const char *name, int argc, param_ret_t **argv, param_ret_t *pret) {
	char *fctname = NULL;
	param_u_t	callParams[20];
	extern char *typeString[];
	int i;

	iec_asprintf (&fctname, "%s", name);
	for	(i=0; i<argc; i++) {
		iec_asprintf (&fctname, "_%s", typeString[(int)argv[i]->t]);
	}

	RTL_TRDBG (TRACE_DETAIL, "Looking for %s\n", fctname);
	iec_fctinfos_t *fctinfo = searchStdFunc (fctname);
	if	(!fctinfo) {
		free (fctname);
		fctname = NULL;
		iec_asprintf (&fctname, "%s", name);
		RTL_TRDBG (TRACE_DETAIL, "Looking for %s\n", fctname);
		fctinfo = searchStdFunc (fctname);
	}

	if	(fctinfo && *fctinfo->ret_type) {
		RTL_TRDBG (TRACE_DETAIL, "*** cb_callFunc %s argc=%d\n", fctname, argc);
		void	(*fct)() = fctinfo->fct;
		BOOL EN=TRUE, ENO;		// TODO : EN is always TRUE. Correct it !
		int	i;
		for	(i=0; (i<argc) && (i<20); i++) {
			callParams[i]	= argv[i]->p;
		}
		(*fct)(EN, &ENO, pret, argc, callParams);
		//pret->t	= argv[0]->t; ????
	}
	else {
		RTL_TRDBG (TRACE_ERROR, "Function %s doesn't exist\n", name);
	}

	free (fctname);

	return	0;
}
