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
#include <string.h>

#include <rtlbase.h>

#include <param_u.h>
#include <iec_debug.h>


void *g_vars = NULL;

/*
 *	THIS CODE IS NOT PART OF PLC.X
 *	THIS CODE IS MADE FOR UNIT TESTS OF THE ST INTERPRETER, AS A STANDALONE EXECUTABLE
 *
 *	PLC.X HAS ITS OWN VERSION, MORE UPTODATE AND CHECKED, OF THIS CODE
 */

#include <rtlhtbl.h>
#include <iec_std_lib.h>

const char *typeString[] IN_TEXT_SEGMENT = {
	"BOOL", "SINT", "INT", "DINT", "LINT", "USINT", "UINT", "UDINT",
	"ULINT", "BYTE", "WORD", "DWORD", "LWORD", "REAL", "LREAL",
	"TIME", "DATE", "DT", "TOD", "STRING", "derived", NULL
};

int typeInteger(char *t) {
	int i;
	for	(i=0; typeString[i]; i++)
		if	(!strcasecmp(typeString[i], t))
			return	i;
	return	-1;
}

void freeVariable(iec_variable_t *var) {
	free (var);
}

void addVariableInHash (void *hash, iec_variable_t *var) {
	iec_variable_t *old = rtl_htblGet(hash, var->name);
	if	(old) {
		rtl_htblRemove (hash, old->name);
		freeVariable (old);
	}
	rtl_htblInsert (hash, var->name, var);
}

iec_variable_t *createVariableWithParameter2 (char *name, param_ret_t *param) {
	iec_variable_t *v = calloc(sizeof(iec_variable_t), 1);
	strcpy (v->name, name);
	v->type	= param->t;
	v->value = param->p;
	return	v;
}

iec_variable_t *getVariable (void *hash, char *name) {
	return (iec_variable_t *)rtl_htblGet(hash, name);
}

char *printVariable(iec_variable_t *var) {
	static char tmp[1024];
	if	(!var)
		return tmp;
	strcpy (tmp, var->name);
	strcat (tmp, "[");
	strcat (tmp, typeString[(int)var->type]);
	strcat (tmp, "] = ");

	switch (var->type) {
	case BOOLtype : strcat (tmp, var->value.BOOLvar ? "TRUE":"FALSE"); break;
	case INTtype : sprintf (tmp+strlen(tmp), "%d", var->value.INTvar); break;
	case DINTtype : sprintf (tmp+strlen(tmp), "%d", var->value.DINTvar); break;
	case REALtype : sprintf (tmp+strlen(tmp), "%f", var->value.REALvar); break;
	case LREALtype : sprintf (tmp+strlen(tmp), "%f", var->value.LREALvar); break;
	case TIMEtype : sprintf (tmp+strlen(tmp), "tv_sec=%ld tv_nsec=%ld", var->value.TIMEvar.tv_sec, var->value.TIMEvar.tv_nsec); break;
	case DATEtype : sprintf (tmp+strlen(tmp), "tv_sec=%ld tv_nsec=%ld", var->value.DATEvar.tv_sec, var->value.DATEvar.tv_nsec); break;
	case TODtype : sprintf (tmp+strlen(tmp), "tv_sec=%ld tv_nsec=%ld", var->value.TODvar.tv_sec, var->value.TODvar.tv_nsec); break;
	case DTtype : sprintf (tmp+strlen(tmp), "tv_sec=%ld tv_nsec=%ld", var->value.DTvar.tv_sec, var->value.DTvar.tv_nsec); break;
	case STRINGtype :
		strncat (tmp, (char *)var->value.STRINGvar.body, var->value.STRINGvar.len);
		sprintf (tmp+strlen(tmp), "(%d)", var->value.STRINGvar.len); break;
		break;
	default : strcat (tmp, "???"); break;
	}
	return	tmp;
}

//	STD FUNCTIONS
typedef struct {
	char *name;
	void *fct;
	char *ret_type;
	BOOL varargs;
} iec_fctinfos_t;

void *g_StdFunc = NULL;

/*
iec_fctinfos_t fctTab[] = {
#include <STANDARD_FUNCTIONS.h>
	{ NULL, NULL, NULL }
};
*/

extern iec_fctinfos_t fctTab[];

int iec_addFunc(iec_fctinfos_t *info) {
	if	(!g_StdFunc)
		g_StdFunc = rtl_htblCreate(128, NULL);
	if	(rtl_htblGet(g_StdFunc, info->name) == NULL) {
		rtl_htblInsert (g_StdFunc, info->name, info);
		return	0;
	}
	else {
		return	-1;
	}
}

int iec_addFuncs(iec_fctinfos_t *info) {
	int	count = 0;
	while (info->name)
		if (iec_addFunc (info++) == 0)
			count ++;
	return	count;
}

extern void STANDARD_FBS_init ();
extern void Actility_init();

void initStdFunc() {
	if	(g_StdFunc)
		return;
	g_StdFunc = rtl_htblCreate(128, NULL);
	iec_addFuncs (fctTab);
	STANDARD_FBS_init ();
	Actility_init ();
}

iec_fctinfos_t *searchStdFunc (const char *name) {
	return rtl_htblGet(g_StdFunc, (char *)name);
}

//	PARAMETERS
void setParameter (param_u_t *p, void *value, char type) {
	int	len;
	switch (type) {
	case	BOOLtype :
		if	(!strcasecmp(value, "TRUE"))
			p->BOOLvar	= TRUE;
		else
			p->BOOLvar	= (IEC_BOOL)atoi(value);
		break;
	case	INTtype :p->INTvar	= (IEC_INT)atoi(value); break;
	case	DINTtype :p->DINTvar	= (IEC_DINT)atol(value); break;
	case	REALtype : p->REALvar	= (IEC_REAL)atof(value); break;
	case	LREALtype : p->LREALvar	= (IEC_LREAL)atof(value); break;
	case	STRINGtype :
		len	= strlen((char *)value);
		if	(len > STR_MAX_LEN) {
			printf ("! String len (%d) reached STR_MAX_LEN (%d)\n", len, STR_MAX_LEN);
			len	= STR_MAX_LEN;
		}
		p->STRINGvar.len	= len;
		strncpy ((char *)p->STRINGvar.body, value, p->STRINGvar.len);
		break;
	case	DERIVEDtype :
		printf ("setReturnParameter DERIVEDtype %p\n", p->DERIVEDvar);
		if	(!p->DERIVEDvar)
			p->DERIVEDvar = rtl_htblCreate(128, NULL);
		break;
	}
}

void setReturnParameter (param_ret_t *pret, void *value, int len, char type) {
	pret->t	= type;
	setParameter (&pret->p, value, type);
}

//=======================================================


int cb_getVar (void *userctx, const char *name, param_ret_t *pret) {
	if	(!g_vars)
		g_vars = rtl_htblCreate(128, NULL);
	iec_variable_t *var = getVariable (g_vars, (char *)name);
	if	(!var) {
		pret->t	= DINTtype;
		pret->p.DINTvar	= 0;
/*
		var = createVariableWithParameter2 ((char *)name, pret);
		addVariableInHash (g_vars, var);
*/
		return	0;
	}
	pret->t	= var->type;
	pret->p	= var->value;
	RTL_TRDBG (TRACE_DETAIL, "getVar %s\n", printVariable(var));
	return	0;
}

int cb_getType(void *userctx, const char *name) {
	if	(!g_vars)
		g_vars = rtl_htblCreate(128, NULL);
	iec_variable_t *var = getVariable (g_vars, (char *)name);
	if	(!var)
		return	-1;
	return	var->type;
}

int cb_setVar (void *userctx, const char *name, param_ret_t *pret) {
	if	(!g_vars)
		g_vars = rtl_htblCreate(128, NULL);

	iec_variable_t *var = createVariableWithParameter2 ((char *)name, pret);
	addVariableInHash (g_vars, var);
	RTL_TRDBG (TRACE_DETAIL, "setVar %s\n", printVariable(var));
	return	0;
}

int cb_callFunc (void *userctx, const char *name, int argc, param_ret_t **argv, param_ret_t *pret) {
	char	fctname[300];
	param_u_t	callParams[20];
	int	i;

	initStdFunc();

	strcpy (fctname, name);
	for	(i=0; i<argc; i++) {
		strcat (fctname, "_");
		strcat (fctname, typeString[(int)argv[i]->t]);
	}
	RTL_TRDBG (TRACE_DETAIL, "callFunc %s argc=%d\n", fctname, argc);

	iec_fctinfos_t *fctinfo = searchStdFunc (fctname);

	if	(!fctinfo) {
		fctinfo = searchStdFunc (name);
		RTL_TRDBG (TRACE_DETAIL, "callFunc %s argc=%d\n", name, argc);
	}

	if	(fctinfo && *fctinfo->ret_type) {
		void	(*fct)() = fctinfo->fct;
		BOOL EN=TRUE, ENO;		// TODO : EN is always TRUE. Correct it !
		int	i;
		for	(i=0; (i<argc) && (i<20); i++) {
			callParams[i]	= argv[i]->p;
		}
		(*fct)(EN, &ENO, pret, argc, callParams);
		//pret->t	= argv[0]->t;
		RTL_TRDBG (TRACE_DETAIL, "RET is of type %s\n", typeString[(int)pret->t]);
	}
	return	0;
}

void *getCurrentContext() {
	return NULL;
}
