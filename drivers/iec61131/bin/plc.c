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

/* plc.c --
 *
 * This file is part of the iec61131-3 interpreter.
 **
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
#include <sys/stat.h>

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
#include "validation.h"
#include "compileST.h"
#ifdef WITH_EXI
#include "exi2xml.h"
#endif

#include "admin_cli.h"

extern int TraceLevel;
iec_global_t GLOB;

/*
void *__calloc(int line, int x, int y) {
	RTL_TRDBG (TRACE_DETAIL, "calloc %d %d %d\n", line, x, y);
	return calloc(x,y);
}
#define Calloc(X,Y) __calloc(__LINE__,X,Y);
*/
/*
#define Calloc(X,Y) calloc(X,Y);
*/

void *__IECcalloc(int line, int x, int y) {
	void *pt = calloc(x,y);
	if	(!pt) {
		GSC_log("MALLOC_FAILURE", "Memory allocation failure.", "",
			"malloc failure attempting to allocate %d bytes. Aborting IEC...", x*y);
		abort();
	}
	return pt;
}
#define Calloc(X,Y) __IECcalloc(__LINE__,X,Y)
void *__IECstrdup(int line, char *s) {
	if	(!s)
		return	NULL;
	char *pt = __IECcalloc(line, strlen(s)+1, 1);
	strcpy (pt, s);
	return pt;
}
#define Strdup(S) __IECstrdup(__LINE__,S)

extern void iec_gettimeofday(IEC_TIMESPEC *iec_time);

void *searchPou(void *project, char *name);
void Block_AddExec(iec_ctx_t *ctx, void *xo_block, char *name);
void Block_AddExec_v2(iec_ctx_t *ctx, void *xo_block);
void Block_ExecDump(iec_ctx_t *ctx);
void *Block_NextExec(iec_ctx_t *ctx);
iec_ctx_t *getContextByName(iec_program_t *program, char *name);
void freeVariables_derived(void *hash);
int runActions(iec_program_t *program, iec_step_t *step, mxml_node_t *xo_ab, mxml_node_t *xo_action);
void Step_Free(iec_step_t *step);


/**
 * @brief A version of strcmp that checks the parameters
 * @param s1 as strcmp
 * @param s2 as strcmp
 * @return Same as strcmp
 */
int protected_strcmp(char *s1, char *s2) {
	if	(!s1 || !s2)
		return	1;
	return	strcmp(s1, s2);
}

char *typeString[] = {
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

/**
 * @brief Format a parameter
 * @param p The resulting formated parameter
 * param_u_t is a simple C union of all types of IEC data
 * @param value The content to store in the parameter
 * @param type The type of the content
 * @return void
 */
void setParameter (param_u_t *p, void *value, char type) {
	switch (type) {
	case	BOOLtype : p->BOOLvar = getBoolean(value); break;
	case	DINTtype :p->DINTvar	= getInteger(value); break;
	case	TIMEtype : p->TIMEvar	= getDuration(value); break;
	case	DATEtype : p->DATEvar	= getDate(value); break;
	case	TODtype : p->TODvar	= getTimeOfDay(value); break;
	case	DTtype : p->DTvar	= getDateAndTime(value); break;
	case	REALtype : sscanf (value, "%f", &p->REALvar); break;
	case	LREALtype : sscanf (value, "%lf", &p->LREALvar); break;

	case	SINTtype :p->SINTvar	= (IEC_SINT)getInteger(value); break;
	case	INTtype :p->INTvar	= (IEC_INT)getInteger(value); break;
	case	LINTtype :p->LINTvar	= (IEC_LINT)getInteger(value); break;
	case	USINTtype :p->USINTvar	= (IEC_USINT)getInteger(value); break;
	case	UINTtype :p->UINTvar	= (IEC_UINT)getInteger(value); break;
	case	UDINTtype :p->UDINTvar	= (IEC_UDINT)getInteger(value); break;
	case	ULINTtype :p->ULINTvar	= (IEC_ULINT)getInteger(value); break;
	case	BYTEtype :p->USINTvar	= (IEC_BYTE)getInteger(value); break;
	case	WORDtype :p->WORDvar	= (IEC_WORD)getInteger(value); break;
	case	DWORDtype :p->DWORDvar	= (IEC_DWORD)getInteger(value); break;
	case	LWORDtype :p->LWORDvar	= (IEC_LWORD)getInteger(value); break;
	case	STRINGtype :
		p->STRINGvar = getString(value);
/*
		len	= strlen((char *)value);
		if	(len > STR_MAX_LEN) {
			RTL_TRDBG (TRACE_ERROR, "! String len (%d) reached STR_MAX_LEN (%d)\n", len, STR_MAX_LEN);
			len	= STR_MAX_LEN;
		}
		p->STRINGvar.len	= len;
		strncpy ((char *)p->STRINGvar.body, value, p->STRINGvar.len);
*/
		break;
	case	DERIVEDtype :
		RTL_TRDBG (TRACE_DETAIL, "setParameter DERIVEDtype %p\n", p->DERIVEDvar);
		if	(!p->DERIVEDvar)
			p->DERIVEDvar = rtl_htblCreate(32, NULL);
		break;
	}
}

 /**
 * @brief Format a return parameter
 * @param pret The resulting formated parameter
 * param_ret_t is a param_u_t + a type
 * @param value The content to store in the parameter
 * @param len ???
 * @param type The type of the content
 * @return void
 */
void setReturnParameter (param_ret_t *pret, void *value, int len, char type) {
	pret->t = type;
	setParameter (&pret->p, value, type);
}
 
/**
 * @brief Add a IEC function into the hashtable GLOB.StdFunc
 * @param info A pointer to a structure iec_fctinfos_t
 * @return 0 sucess -1 error
 */
int iec_addFunc(const iec_fctinfos_t *info) {
	if	(!GLOB.StdFunc)
		GLOB.StdFunc = rtl_htblCreate(128, NULL); // *LNI* what is 128 ??
	if	(rtl_htblGet(GLOB.StdFunc, info->name) == NULL) {
		rtl_htblInsert (GLOB.StdFunc, info->name, (void *)info);
		return	0;
	}
	else {
		RTL_TRDBG (TRACE_ERROR, "function %s duplicated\n", info->name);
		return	-1;
	}
}

/**
 * @brief Add a set of functions into the hashtable GLOB.StdFunc
 * @param info A table iec_fctinfos_t, lasting with a NULL
 * @return 0 sucess -1 error
 */
int iec_addFuncs(const iec_fctinfos_t *info) {
	int	count = 0;
	while (info->name)
		if (iec_addFunc (info++) == 0)
			count ++;
	return	count;
}

/**
 * @brief Add all standard IEC functions into the hashtable GLOB.StdFunc
 * @return void
 */
/*
iec_fctinfos_t fctTab[] = {
#include "STANDARD_FUNCTIONS.h"
	{ NULL, NULL, NULL }
};
*/

extern const iec_fctinfos_t fctTab[];

void initStdFunc() {
	if	(GLOB.StdFunc)
		return;
	GLOB.StdFunc = rtl_htblCreate(128, NULL);
	int count = iec_addFuncs (fctTab);
	RTL_TRDBG (TRACE_DETAIL, "Number of IEC Standard Functions loaded: %d\n", count);
}

/**
 * @brief Loads Function Blocks into global memory GLOB.FBD_list
 * @param fn File name
 * @return void
 */
void iec_addFBS(char *fn) {
	RTL_TRDBG (TRACE_DETAIL, "Loading %s\n", fn);
	FILE *fp = fopen (fn, "r");
	if	(!fp) {
		RTL_TRDBG (TRACE_ERROR, "Unable to open %s\n", fn);
		Board_Put (&GLOB.board_errors, "FATAL : Unable to open %s", fn);
		return;
	}
#if 0 // OBSOLETE
	mxml_node_t *new_pous = mxmlLoadFile(NULL, fp, MXML_OPAQUE_CALLBACK);
	mxmlReduce (new_pous);
#endif // OBSOLETE
	mxml_node_t *new_pous = mxmlSAXLoadFile (NULL , fp , MXML_OPAQUE_CALLBACK, iec_sax_cb, NULL);
	mxmlReduceAttrs (new_pous);
	fclose (fp);
	if	(!new_pous) {
		RTL_TRDBG (TRACE_ERROR, "Unable to load %s. Bad xml format.\n", fn);
		Board_Put (&GLOB.board_errors, "Unable to load %s. Bad xml format.", fn);
		return;
	}

	iec_fbd_t *fbd = Calloc(sizeof(iec_fbd_t), 1);
	fbd->xml	= new_pous;
	list_add_tail (&fbd->head, &GLOB.FBD_list);
}

/**
 * @brief Initialisation of the VM
 * @return void
 */
int iec_init(char *stdInclude) {
	extern void STANDARD_FBS_init ();

	INIT_LIST_HEAD (&GLOB.user_list);
	INIT_LIST_HEAD (&GLOB.FBD_list);
	Board_Init (&GLOB.board_errors);

	initStdFunc();

	char tmp[512] = "";
	if	(!stdInclude) {
		stdInclude = Operating_TemplatesDir();
	}

	sprintf (tmp, "%s/STANDARD_FBS.xml", stdInclude);
	iec_addFBS (tmp);
	STANDARD_FBS_init ();

	sprintf (tmp, "%s/ACTILITY_FBS.xml", stdInclude);
	iec_addFBS (tmp);

	//AdmTcpInit ();
	void iec_init_cli ();
	iec_init_cli ();

	iec_watchdog_start (IEC_Config_Int_Default("watchdog", 60));

	return 0;
}

/**
 * @brief Search a function in the hashtable
 * @param name A function name
 * @return A pointer to a iec_fctinfos_t
 */
iec_fctinfos_t *searchStdFunc (char *name) {
	return rtl_htblGet(GLOB.StdFunc, name);
}

/**
 * @brief Returns a human readable display of a variable
 * @param var A variable
 * @param buf A buffer containing the result
 * @return the buffer buf
 */
char *printVarValue(iec_variable_t *var, char **tmp) {
	STRING s;
	int sz;
	if	(!var)
		return "";
	switch (var->type) {
	case	BOOLtype  : iec_asprintf (tmp, "%s", var->value.BOOLvar ? "TRUE":"FALSE"); break;
	case	INTtype   : iec_asprintf (tmp, "%d", var->value.INTvar); break;
	case	DINTtype  : iec_asprintf (tmp, "%d", var->value.DINTvar); break;
	case	LINTtype  : iec_asprintf (tmp, "%lld", var->value.LINTvar); break;
	case	UINTtype  : iec_asprintf (tmp, "%d", var->value.UINTvar); break;
	case	UDINTtype : iec_asprintf (tmp, "%d", var->value.UDINTvar); break;
	case	ULINTtype : iec_asprintf (tmp, "%lld", var->value.ULINTvar); break;
	case	REALtype  : iec_asprintf (tmp, "%f", var->value.REALvar); break;
	case	LREALtype : iec_asprintf (tmp, "%f", var->value.LREALvar); break;
	case	STRINGtype:
		sz = var->value.STRINGvar.len;
		//strncpy (tmp, (char *)var->value.STRINGvar.body, sz);
		iec_asprintf (tmp, "%*.*s", sz, sz, (char *)var->value.STRINGvar.body);
		break;
	case	TIMEtype :
		s = __time_to_string(var->value.TIMEvar);
		iec_asprintf (tmp, "%ld.%ld [%s]", var->value.TIMEvar.tv_sec, var->value.TIMEvar.tv_nsec, (char *)s.body);
		break;
	case	TODtype :
		s = __tod_to_string(var->value.TODvar);
		iec_asprintf (tmp, "%ld.%ld [%s]", var->value.TIMEvar.tv_sec, var->value.TIMEvar.tv_nsec, (char *)s.body);
		break;
	case	DATEtype :
		s = __date_to_string(var->value.DATEvar);
		iec_asprintf (tmp, "%ld.%ld [%s]", var->value.TIMEvar.tv_sec, var->value.TIMEvar.tv_nsec, (char *)s.body);
		break;
	case	DTtype :
		s = __dt_to_string(var->value.DTvar);
		iec_asprintf (tmp, "%ld.%ld [%s]", var->value.TIMEvar.tv_sec, var->value.TIMEvar.tv_nsec, (char *)s.body);
		break;
	}
	return *tmp;
}

void printVariableInBuffer_WithName(iec_variable_t *var, char **buf, char *namevar, BOOL withTime) {
	if	(!var) return;
	char *tmp = NULL;
	printVarValue (var, &tmp);
	if	(!namevar)	namevar = var->name;
	iec_asprintf (buf, "%s (%p) %s %s", namevar, var, typeString[(int)var->type], tmp);
	if	(withTime && var->lastModifiedTime.tv_sec) {
		STRING s = { 0, "" };
		s = __dt_to_string(var->lastModifiedTime);
		iec_asprintf (buf, "[%s]", (char *)s.body);
	}
	free (tmp);
}

void printVariableInBuffer(iec_variable_t *var, char **buf) {
	printVariableInBuffer_WithName(var, buf, NULL, FALSE);
}

/**
 * @brief Displays a variable in the standards logs
 * @param ctx A context
 * @param var A variable
 * @return void
 */
void printVariableSet(iec_ctx_t *ctx, iec_variable_t *var) {
	if	(!var) return;
	char *tmp = NULL;
	printVariableInBuffer (var, &tmp);
	RTL_TRDBG (TRACE_SET, "VAR_TRACE_context [%s] SET_VAR %s\n", ctx->name, tmp);
	free (tmp);
}

void printVariableGet(iec_ctx_t *ctx, iec_variable_t *var) {
	if	(!var) return;
	char *tmp = NULL;
	printVariableInBuffer (var, &tmp);
	RTL_TRDBG (TRACE_GET, "VAR_TRACE_context [%s] GET_VAR %s\n", ctx->name, tmp);
	free (tmp);
}

//	VARIABLE
void setVariable (iec_variable_t *var, void *value) {
	setParameter (&var->value, value, var->type);
}

/**
 * @brief Create a variable.
 * @param name The name of the variable
 * @param type The type
 * If the type is DERIVEDtype, a hashtable is created for hosting the subfields; value must be NULL
 * @param value Content
 * @return The created variable
 */
iec_variable_t *createVariable (char *name, char type, void *value) {
	//RTL_TRDBG (TRACE_SET, "VAR_TRACE_createVariable [%s]\n", name);
	if	(strlen(name) >= IEC_VARNAME_SIZE) {
		RTL_TRDBG (TRACE_ERROR, "VAR_TRACE_createVariable [%s] size too big (%d)\n", name, IEC_VARNAME_SIZE);
		Board_Put (&GLOB.board_errors, "The variable name [%s] exceed max size of %d", name, IEC_VARNAME_SIZE);
		return NULL;
	}
	iec_variable_t *v = Calloc(sizeof(iec_variable_t), 1);
	strcpy (v->name, name);
	v->flags	|= VARFLAG_INHEAP;
	v->type	= type;
	v->refcount = 1;
	//v->valueHasChanged = TRUE;
	//iec_gettimeofday (&v->lastModifiedTime);
	if	(value)
		setVariable (v, value);
	else if	(type == DERIVEDtype)
		v->value.DERIVEDvar = rtl_htblCreate(32, NULL);
	return	v;
}

iec_variable_t *cloneVariable(iec_variable_t *var) {
	RTL_TRDBG (TRACE_SET, "VAR_TRACE_cloneVariable [%s]\n", var->name);
	iec_variable_t *v = Calloc(sizeof(iec_variable_t), 1);
	*v	= *var;
	v->flags	|= VARFLAG_INHEAP;
	v->refcount = 1;
	return	v;
}

iec_variable_t *createVariableWithParameter (char *name, char type, param_u_t *param) {
	//RTL_TRDBG (TRACE_SET, "VAR_TRACE_createVariableWithParameter [%s]\n", name);
	if	(strlen(name) >= IEC_VARNAME_SIZE) {
		RTL_TRDBG (TRACE_ERROR, "VAR_TRACE_createVariable [%s] size too big (%d)\n", name, IEC_VARNAME_SIZE);
		Board_Put (&GLOB.board_errors, "The variable name [%s] exceed max size of %d", name, IEC_VARNAME_SIZE);
		return NULL;
	}
	iec_variable_t *v = Calloc(sizeof(iec_variable_t), 1);
	strcpy (v->name, name);
	v->flags	|= VARFLAG_INHEAP;
	v->type	= type;
	v->refcount = 1;
	v->flags |= VARFLAG_CHANGED;
	//iec_gettimeofday (&v->lastModifiedTime);
	v->value = *param;
	return	v;
}

iec_variable_t *createVariableWithParameter2 (char *name, param_ret_t *param) {
	if	(strlen(name) >= IEC_VARNAME_SIZE) {
		RTL_TRDBG (TRACE_ERROR, "VAR_TRACE_createVariable [%s] size too big (%d)\n", name, IEC_VARNAME_SIZE);
		Board_Put (&GLOB.board_errors, "The variable name [%s] exceed max size of %d", name, IEC_VARNAME_SIZE);
		return NULL;
	}
	iec_variable_t *v = Calloc(sizeof(iec_variable_t), 1);
	strcpy (v->name, name);
	v->flags	|= VARFLAG_INHEAP;
	v->type = param->t;
	v->refcount = 1;
	v->flags |= VARFLAG_CHANGED;
	//iec_gettimeofday (&v->lastModifiedTime);
	v->value = param->p;
	RTL_TRDBG (TRACE_SET, "VAR_TRACE_createVariableWithParameter2 [%s] %s\n", name, typeString[(int)v->type]);
	return	v;
}

void freeVariable(iec_variable_t *var) {
	if	(!var) return;
	RTL_TRDBG (TRACE_SET, "VAR_TRACE_freeVariable (%p) [%s] %s (ref:%d %s)\n",
		var, var->name, typeString[(int)var->type], var->refcount, var->flags & VARFLAG_INHEAP ? "Heap":"");
	if	(var->type == DERIVEDtype)
		freeVariables_derived(var->value.DERIVEDvar);
	if	(var->refcount <= 0) {
		RTL_TRDBG (TRACE_ERROR, "ERROR refcount\n");
		return;
	}
	var->refcount --;
	if	(var->refcount <= 0) {
		if	(var->flags & VARFLAG_INHEAP)
			free (var);
	}
	else
		RTL_TRDBG (TRACE_SET, "VAR_TRACE_freeVariable [%s] %s differed (ref:%d)\n", var->name, typeString[(int)var->type], var->refcount);
}

/**
 * @brief Free all variables of a hash table
 * @param hash The hash table
 * @return void
 */
void freeVariables_derived(void *hash) {
	inline void fct(char *key, void *var) { freeVariable(var); }
	rtl_htblDump(hash, fct);
	rtl_htblDestroy(hash);
}

/**
 * @brief Free all variables of a context
 * @param ctx The context
 * @return void
 */
void freeVariables(iec_ctx_t *ctx) {
	inline void fct(char *key, void *var) { freeVariable(var); }
	rtl_htblDump(ctx->m_variables, fct);
	rtl_htblDestroy(ctx->m_variables);
}

/**
 * @brief Create a derived variable.
 * A derived variable is a hash table containing subfields
 * @param name The variable name
 * @return A iec_variable_t object
 */
iec_variable_t *createDerivedVariable (char *name) {
	if	(strlen(name) >= IEC_VARNAME_SIZE) {
		RTL_TRDBG (TRACE_ERROR, "VAR_TRACE_createVariable [%s] size too big (%d)\n", name, IEC_VARNAME_SIZE);
		Board_Put (&GLOB.board_errors, "The variable name [%s] exceed max size of %d", name, IEC_VARNAME_SIZE);
		return NULL;
	}
	iec_variable_t *v = Calloc(sizeof(iec_variable_t), 1);
	strcpy (v->name, name);
	v->flags	|= VARFLAG_INHEAP;
	v->type = DERIVEDtype;
	v->refcount = 1;
	v->flags |= VARFLAG_CHANGED;
	//iec_gettimeofday (&v->lastModifiedTime);
	v->value.DERIVEDvar = rtl_htblCreate(32, NULL); // *LNI* requires define
	return	v;
}

#define TIME_NOT_DEFINED(t) ((t).tv_sec+(t).tv_nsec == 0)

iec_variable_t *addVariableInHash (void *hash, iec_variable_t *var) {
	iec_variable_t *old = rtl_htblGet(hash, var->name);
	if	(old) {
		RTL_TRDBG (TRACE_DETAIL, "VAR_TRACE_REPLACE %s OLD addr %p\n", var->name, old);
		if	(old->type != var->type) {
			RTL_TRDBG (TRACE_ERROR, "\033[31;7mVAR_TRACE_ERROR setVar [%s] : bad type\033[0m\n", var->name);
			return	NULL;
		}
		if	(memcmp(&old->value, &var->value, sizeof(param_u_t)) ||
			TIME_NOT_DEFINED(old->lastModifiedTime)) {
			old->value = var->value;
			old->flags |= VARFLAG_CHANGED;
		}
		else {
			RTL_TRDBG (TRACE_DETAIL, "VAR_TRACE setVar [%s] : SAMEVALUE\n", var->name);
		}
		iec_gettimeofday (&old->lastModifiedTime);
		freeVariable (var);
		return old;
	}
	else {
		if (rtl_htblInsert (hash, var->name, var) < 0)
			RTL_TRDBG (TRACE_ERROR, "VAR_TRACE_ERROR rtl_htblInsert\n");
		var->refcount = 1;
		//var->valueHasChanged = TRUE;
		return var;
	}
}

BOOL Variable_Exists (iec_ctx_t *ctx, char *name) {
	return rtl_htblGet(ctx->m_variables, name) != NULL;
}

/**
 * @brief Tries to link a externalVar to a globalVar
 * @param ctx The local context
 * @param varname The variable name
 * @return 0 if success, <0 else
 */
int linkExternalVar(iec_ctx_t *ctx, char *varname) {
	// External variable already linked. Return quietly.
	if (rtl_htblGet(ctx->m_variables, varname))
		return 0;

	iec_resource_t *resource = ctx->program->task->resource;
	iec_variable_t *global = rtl_htblGet(resource->m_variables, varname);
	if	(!global) {
		global	= rtl_htblGet(resource->user->m_variables, varname);
		if	(!global)
			return	-1;
	}
	if (rtl_htblInsert (ctx->m_variables, varname, global) < 0)
		return	-2;
	global->refcount ++;
	return	0;
}

/**
 * @brief link a variable with another in different contexts. Handle refcount.
 */
int linkVariable(void *from, char *nameFrom, void *to, char *nameTo) {
	iec_variable_t *var = rtl_htblGet(from, nameFrom);
	if	(!var)	return -1;

	if (rtl_htblInsert (to, nameTo, var) < 0)
		return	-2;
	var->refcount ++;
	return	0;
}

int linkToUpperVar(iec_ctx_t *ctx, char *varname) {
	if (Variable_Exists (ctx, varname))
		return	-1;
	iec_ctx_t *up = getUpperContext(ctx);
	iec_variable_t *uvar = rtl_htblGet(up->m_variables, varname);
	if	(!uvar)
		return	-2;
	if (rtl_htblInsert (ctx->m_variables, varname, uvar) < 0)
		return	-3;
	uvar->refcount ++;
	return	0;
}

int updateVariableWithParameter(iec_ctx_t *ctx, char *varname, param_ret_t *param) {
	RTL_TRDBG (TRACE_SET, "updateVariableWithParameter %s\n", varname);
	iec_variable_t *var = rtl_htblGet(ctx->m_variables, varname);
	if	(!var) {
        RTL_TRDBG (TRACE_SET, "updateVariableWithParameterERROR Variable not found %s\n", varname);
		return	-1;}
	if	(var->type != param->t){
        RTL_TRDBG (TRACE_SET, "updateVariableWithParameterERROR Invalid Type %s, VarType:%d ParamType:%d\n", varname, var->type, param->t );
		return	-2;}
	if	(memcmp(&var->value, &param->p, sizeof(param_u_t)) ||
			TIME_NOT_DEFINED(var->lastModifiedTime)) {
		var->value	= param->p;
		var->flags |= VARFLAG_CHANGED;
		if	(var->flags & VARFLAG_RETAIN)
			saveRetain();
	}
	else {
		RTL_TRDBG (TRACE_DETAIL, "VAR_TRACE setVar [%s] : SAMEVALUE\n", varname);
	}
	iec_gettimeofday (&var->lastModifiedTime);
	printVariableSet(ctx, var);
	return 0;
}

/**
 * @brief Add a subfield to a derived variable
 * @param var The variable (must be derived)
 * @param field The subfield
 * @return void
 */
void addFieldToDerivedVariable (iec_variable_t *var, iec_variable_t *field) {
	addVariableInHash (var->value.DERIVEDvar, field);
}

void addFieldToDerivedParam (param_ret_t *pret, iec_variable_t *field) {
	RTL_TRDBG (TRACE_SET, "addFieldToDerivedParam %p\n", pret->p.DERIVEDvar);
	if	(!pret->p.DERIVEDvar)
		pret->p.DERIVEDvar = rtl_htblCreate(32, NULL);
	addVariableInHash (pret->p.DERIVEDvar, field);
}

void addVariableInContext (iec_ctx_t *ctx, iec_variable_t *var) {
	var = addVariableInHash (ctx->m_variables, var);
	if	(var) {
		//var->valueHasChanged = TRUE;
		printVariableSet (ctx, var);
		if	(var->flags & VARFLAG_RETAIN)
			saveRetain();
	}
}

iec_variable_t *getDerived(void *hash, char *name) {
	iec_variable_t *var;
	RTL_TRDBG (TRACE_GET, "VAR_TRACE_getDerived %s\n", name);

	char *pt = strchr(name, '.');
	if	(pt) {
		char tmp[100];
		strncpy (tmp, name, pt-name);
		tmp[pt-name] = 0;
		var = (iec_variable_t *)rtl_htblGet(hash, tmp);
		if	(!var)
			return	NULL;
		if	(var->type != DERIVEDtype)
			return	NULL;

		return	getDerived(var->value.DERIVEDvar, pt+1);
	}

	var = (iec_variable_t *)rtl_htblGet(hash, name);
	if	(var) {
		char *tmp = NULL;
		printVariableInBuffer (var, &tmp);
		RTL_TRDBG (TRACE_GET, "VAR_TRACE_Derived found %s\n", tmp);
		free (tmp);
	}
	return	var;
}

/**
 * @brief Retrieves a variable. Handles compound variables as A.B.C.D
 *
 * Deux types of compound variables :
 * 1. a variable contained in the context c1 : c1.var
 * 2. Variable of type DERIVEDtype
 *
 * @param ctx A context
 * @param var A variable.
 * @return A pointer to the variable
 */
iec_variable_t *getVariable (iec_ctx_t *ctx, char *name) {
	iec_variable_t *var;
	RTL_TRDBG (TRACE_GET, "VAR_TRACE_context [%s] getVariable %s\n", ctx->name, name);
	//	Chaine de caractères : on crée une variable à la volée
	if	(*name == '\'') {
		var = createVariable(name, STRINGtype, name);
		addVariableInContext (ctx, var);
		return var;
	}

	var = (iec_variable_t *)rtl_htblGet(ctx->m_variables, name);
	if	(var) {
		printVariableGet (ctx, var);
		return	var;
	}

	char *pt = strchr(name, '.');
	if	(pt) {
		char tmp[200];
		strncpy (tmp, name, pt-name);
		tmp[pt-name] = 0;
		iec_ctx_t *sub_ctx = getContextByName(ctx->program, tmp);
		if	(sub_ctx) {
			return	getVariable(sub_ctx, pt+1);
		}
		var = (iec_variable_t *)rtl_htblGet(ctx->m_variables, tmp);
		if	(!var)
			return	NULL;
		if	(var->type != DERIVEDtype)
			return	NULL;

		return	getDerived(var->value.DERIVEDvar, pt+1);
	}
	else {
		RTL_TRDBG (TRACE_ERROR, "\033[31;7mVAR_TRACE_ERROR GET variable %s\033[0m\n", name);
		return NULL;
	}
}

void updateVariable(iec_ctx_t *ctx, iec_variable_t *newvar) {
	RTL_TRDBG (TRACE_SET, "VAR_TRACE_updateVariable [%s]\n", newvar->name);
	addVariableInContext (ctx, newvar);
}

void printDerivedParam (param_ret_t *pret) {
	inline void fct(char *key, void *v) {
		iec_variable_t *v1 = (iec_variable_t *)v;
		RTL_TRDBG (TRACE_GET, "\tfield [%s] %s\n", v1->name, v1->type);
	}
	if	(pret->t == DERIVEDtype) {
		RTL_TRDBG (TRACE_GET, "printDerivedParam\n");
		rtl_htblDump(pret->p.DERIVEDvar, fct);
	}
}

void printDerivedVariable (iec_variable_t *var) {
	inline void fct(char *key, void *v) {
		iec_variable_t *v1 = (iec_variable_t *)v;
		RTL_TRDBG (TRACE_GET, "\tfield [%s] %s\n", v1->name, v1->type);
	}
	if	(var->type == DERIVEDtype) {
		RTL_TRDBG (TRACE_GET, "printDerivedVariable [%s] %s\n", var->name, var->type);
		rtl_htblDump(var->value.DERIVEDvar, fct);
	}
}

void copyDerivedVariable (iec_variable_t *from, iec_variable_t *to) {
	int fct (void *h, char *key, void *var, void *udata) {
		iec_variable_t *v1 = (iec_variable_t *)var;
		iec_variable_t *dest = (iec_variable_t *)udata;

		RTL_TRDBG (TRACE_SET, "\tfield [%s] %s\n", v1->name, v1->type);
		iec_variable_t *v2 = createVariable (v1->name, v1->type, NULL);
		v2 = addVariableInHash (dest->value.DERIVEDvar, v2);
		if	(!v2) return -1;
		if	(v1->type == DERIVEDtype) {
			copyDerivedVariable (v1, v2);
		}
		else {
			v2->value = v1->value;
		}
		return	0;
	}
	RTL_TRDBG (TRACE_SET, "copyDerivedVariable %p %s\n", to, to->name);
	rtl_htblWalk(from->value.DERIVEDvar, fct, to);
}

/**
 * @brief Copy a variable
 * @param from The origin context
 * @param varfrom The origin variable
 * @param to The target context
 * @param varto The target variable
 * @return void
 */
int copyVariable (iec_ctx_t *from, char *varfrom, iec_ctx_t *to, char *varto, BOOL autoCreate) {
	RTL_TRDBG (TRACE_DETAIL, "VAR_TRACE_copyVariable [%s]:%s into [%s]:%s\n", from->name, varfrom, to->name, varto);
	iec_variable_t *v1 = rtl_htblGet(from->m_variables, varfrom);
	if	(!v1) {
		RTL_TRDBG (TRACE_ERROR, "\033[31;7mVAR_TRACE_ERROR GET %s : variable not declared\033[0m\n", varfrom);
		return -1;
	}
	param_ret_t pret;
	pret.t = v1->type;
	pret.p = v1->value;
	int rc = updateVariableWithParameter(to, varto, &pret);
	if (rc < 0) {
		if	(autoCreate == FALSE) {
			RTL_TRDBG (TRACE_ERROR, "\033[31;7mVAR_TRACE_ERROR SET %s : variable not declared\033[0m\n", varto);
			return -1;
		}
		iec_variable_t *var = createVariableWithParameter2 (varto, &pret);
		addVariableInContext (to, var);
	}
	return 0;
}

void dumpVariables(iec_ctx_t *ctx) {
	inline void fct(char *key, void *var) {
		char *tmp = NULL;
		printVariableInBuffer (var, &tmp);
		RTL_TRDBG (TRACE_GET, "\t%s\n", tmp);
		free (tmp);
	}
	RTL_TRDBG (TRACE_GET, "\tVAR_TRACE_dumpVariables [%s]\n", ctx->name);
	rtl_htblDump(ctx->m_variables, fct);
}

//-------------------------------------------
//	CONTEXT
//-------------------------------------------

void dumpContexts(iec_program_t *program) {
	char *tmp = NULL;
	iec_asprintf (&tmp, "");
	struct list_head *elem;
	list_for_each (elem, &program->ctx_list) {
		iec_ctx_t *ctx	= list_entry(elem, iec_ctx_t, head);
		iec_asprintf (&tmp, "%s,", ctx->name);
	}
	RTL_TRDBG (TRACE_DETAIL, "CONTEXT_TRACE_List of contexts : %s\n", tmp);
	free (tmp);
}

/**
 * @brief Returns the full context name (with path). The buffer returned must be freed by the caller.
 */
char *getFullContextName(iec_program_t *program, char *name) {
	struct list_head *elem;
	char *tmp = NULL;
	iec_asprintf (&tmp, "%s", name);
	list_for_each (elem, &program->ctx_list) {
		iec_ctx_t *ctx	= list_entry(elem, iec_ctx_t, head);
		iec_asprintf (&tmp, ".%s", ctx->name);
	}
	return tmp;
}

iec_ctx_t *getContextByName(iec_program_t *program, char *name) {
	struct list_head *elem;
	list_for_each (elem, &program->ctx_list) {
		iec_ctx_t *ctx	= list_entry(elem, iec_ctx_t, head);
		if	(ctx && !strcmp(ctx->name, name))
			return	ctx;
	}
	return	NULL;
}


/**
 * @brief Create a context of execution.
 *
 * IMPORTANT : un contexte n'est jamais supprimé car il peut être rappelé ultérieurement
 * et doit conserver les variables locales internes.
 * C'est pourquoi lorsque l'on crée un context, on regarde d'abord s'il n'existe pas déjà.
 * Le nom du context est le chemin complet à partir de la racine des contextes.
 *
 * @param program The program into which to create the context
 * @param name Name of context
 * @param type The type of context : CTX_TYPE_CONFIGURATION, CTX_TYPE_RESOURCE, CTX_TYPE_POU, CTX_TYPE_BLOCK, CTX_TYPE_STEP
 *
 * @return A pointer to the context
 */
iec_ctx_t *Context_New (iec_program_t *program, char *name, unsigned char type) {
	RTL_TRDBG (TRACE_DETAIL, "CONTEXT_TRACE_Create new context [%s] in program [%s]\n", name, program->name);
	iec_ctx_t *ctx;
	char *fullname = NULL;

	/*
	 *	Search context in hashtable
	 */
	fullname = getFullContextName(program, name);
	ctx	= rtl_htblGet(program->m_ctx_hash, fullname);
	RTL_TRDBG (TRACE_DETAIL, "CONTEXT_TRACE_retrieveContext '%s' => %p\n", fullname, ctx);

	if	(!ctx) {
		ctx= (iec_ctx_t *)Calloc(sizeof(iec_ctx_t), 1);
		ctx->name	= strdup(name);

		//	SFC step shares there variables with the upper context
		if	(type == CTX_TYPE_STEP)
			ctx->m_variables = Context_Current(program)->m_variables;
		else
			ctx->m_variables = rtl_htblCreate(32, NULL);

		INIT_LIST_HEAD (&ctx->block_list);
		ctx->m_type	= type;
		int ret = rtl_htblInsert (program->m_ctx_hash, fullname, ctx);
		if (ret < 0)
			RTL_TRDBG (TRACE_ERROR, "CONTEXT_TRACE_ERROR rtl_htblInsert program\n");

		RTL_TRDBG (TRACE_DETAIL, "CONTEXT_TRACE_context inserted '%s' Pointer ctx (%p) ctxhtblInstertReturn=%d\n", fullname, ctx, ret);
	}
	ctx->m_state	= 0;
	ctx->m_next	= TRUE;
	ctx->program	= program;	// back ptr
	list_add (&ctx->head, &program->ctx_list);
	dumpContexts(program);
	free (fullname);
	return	ctx;
}

/**
 * @brief Free all context memory
 *
 * Really remove all variables. Warning : this function is not the recommanded one. Use Context_Pop instead.
 * This function is used when a script is unloaded from the VM.
 *
 * @param ctx The context
 * @return void
 */
void Context_Free(iec_ctx_t *ctx) {
	struct list_head *elem, *pos;
	RTL_TRDBG (TRACE_DETAIL, "CONTEXT_TRACE_Context_Free [%s]\n", ctx->name);
	list_for_each_safe (elem, pos, &ctx->block_list) {
		iec_block_t *block	= list_entry(elem, iec_block_t, head);
		free (block->name);
		free (block);
	}
	//	CTX_TYPE_STEP shares there variables
	if	(ctx->m_type != CTX_TYPE_STEP)
		freeVariables (ctx);
	free (ctx->name);
	free (ctx);
}

/**
 * @brief Get the current context (the one in the top of the ctx_list)
 *
 * @param program The program
 * @return The current context
 */
iec_ctx_t *Context_Current (iec_program_t *program) {
	//return list_entry((&program->ctx_list)->next, iec_ctx_t, head);

	struct list_head *elem;
	list_for_each (elem, &program->ctx_list) {
		iec_ctx_t *ctx	= list_entry(elem, iec_ctx_t, head);
		return	ctx;
	}
	return	NULL;
}

/**
 * @brief Pop a context
 *
 * The context is poped but not removed. It stays in m_ctx_hash. All its variables are conserved
 * for a future call to this context (see for example the TOF function block usage).
 *
 * @param program The program
 * @return The previous context
 */
iec_ctx_t *Context_Pop (iec_program_t *program) {
	iec_ctx_t *ctx = Context_Current(program);
	if	(!ctx)
		return	NULL;
	RTL_TRDBG (TRACE_DETAIL, "CONTEXT_TRACE_Pop context [%s]\n", ctx->name);

	list_del (&ctx->head);
	ctx = Context_Current(program);

	if	(ctx) {
		program->xo_pou	= ctx->xo_pou;
		program->xo_block	= ctx->xo_block;
	}
	dumpContexts(program);
//dumpVariables(ctx);
	return	ctx;
}

/**
 * @brief Retrieve the context just upper the current one.
 *
 * @param ctx Current context
 * @return The upper context
 */
iec_ctx_t *getUpperContext (iec_ctx_t *ctx) {
	if	(ctx->head.next == &ctx->program->ctx_list)
		return	NULL;
	return	list_entry(ctx->head.next, iec_ctx_t, head);
}


void Context_Rename(iec_ctx_t *ctx, char *name) {
	free (ctx->name);
	ctx->name = strdup(name);
}


//-------------------------------------------
//	BLOCK
//-------------------------------------------

/**
 * @brief Retrieve an input variable identified by refLocalId
 *
 * @param ctx Current context
 * @param xo_pou
 * @param refLocalId
 * @return The variable
 */

#if 0
iec_variable_t *searchInputVar(iec_ctx_t *ctx, void *xo_pou, char *refLocalId) {
	RTL_TRDBG (TRACE_DETAIL, "VAR_TRACE_searchInputVar %s\n", refLocalId);
	mxml_node_t *variable;
	iec_variable_t	*v;

	if	(!refLocalId)
		return NULL;

	variable = xo_pou;
	while	((variable = mxmlFindElement(variable, xo_pou, "inOutVariable", NULL, NULL, MXML_DESCEND))) {
		char *localId = (char *)mxmlElementGetAttr(variable, "localId");
		if	(localId && !strcmp(refLocalId, localId)) {
			mxml_node_t *expression = mxmlFindElement(variable, variable, "expression", NULL, NULL, MXML_DESCEND);
			char *varname = (char *)mxmlGetOpaque (expression);
			if	(!varname) continue;
			v = getVariable (getUpperContext(ctx), varname);
			return v;
		}
	}
	variable = xo_pou;
	while	((variable = mxmlFindElement(variable, xo_pou, "inVariable", NULL, NULL, MXML_DESCEND))) {
		char *localId = (char *)mxmlElementGetAttr(variable, "localId");
		if	(localId && !strcmp(refLocalId, localId)) {
			mxml_node_t *expression = mxmlFindElement(variable, variable, "expression", NULL, NULL, MXML_DESCEND);
			char *varname = (char *)mxmlGetOpaque (expression);
			if	(!varname) continue;
			v = getVariable (getUpperContext(ctx), varname);
			return v;
		}
	}
	return	NULL;
}
#endif

char *searchInputVarName(iec_ctx_t *ctx, void *xo_pou, char *refLocalId) {
	RTL_TRDBG (TRACE_DETAIL, "VAR_TRACE_searchInputVarName %s\n", refLocalId);
	mxml_node_t *variable;

	if	(!refLocalId)
		return NULL;

	variable = xo_pou;
	while	((variable = mxmlFindElement(variable, xo_pou, "inOutVariable", NULL, NULL, MXML_DESCEND))) {
		char *localId = (char *)mxmlElementGetAttr(variable, "localId");
		if	(localId && !strcmp(refLocalId, localId)) {
			mxml_node_t *expression = mxmlFindElement(variable, variable, "expression", NULL, NULL, MXML_DESCEND);
			char *varname = (char *)mxmlGetOpaque (expression);
			//char *varname = getContainedText(expression);
			if	(varname)
				return varname;
		}
	}
	variable = xo_pou;
	while	((variable = mxmlFindElement(variable, xo_pou, "inVariable", NULL, NULL, MXML_DESCEND))) {
		char *localId = (char *)mxmlElementGetAttr(variable, "localId");
		if	(localId && !strcmp(refLocalId, localId)) {
			mxml_node_t *expression = mxmlFindElement(variable, variable, "expression", NULL, NULL, MXML_DESCEND);
			char *varname = (char *)mxmlGetOpaque (expression);
			//char *varname = getContainedText(expression);
			if	(varname)
				return varname;
		}
	}
	return	NULL;
}


/**
 * @brief Retrieve an output variable identified by refLocalId and formalParameter
 *
 * @param xo_pou
 * @param refLocalId
 * @param formalParameter
 * @return The variable name
 */
char *searchOutputVar(void *xo_pou, char *localId, char *formalParameter) {
	mxml_node_t *variable;

	variable = xo_pou;
	while	((variable = mxmlFindElement(variable, xo_pou, "outVariable", NULL, NULL, MXML_DESCEND))) {

		mxml_node_t *connectionPointIn = mxmlFindElement(variable, variable, "connectionPointIn", NULL, NULL, MXML_DESCEND);
		mxml_node_t *connection = mxmlFindElement(connectionPointIn, connectionPointIn, "connection", NULL, NULL, MXML_DESCEND);
		char *refLocalId = (char *)mxmlElementGetAttr(connection, "refLocalId");
		char *param = (char *)mxmlElementGetAttr(connection, "formalParameter");

		if	(refLocalId && param && !strcmp(refLocalId, localId) && !strcmp(param, formalParameter)) {
			mxml_node_t *expression = mxmlFindElement(variable, variable, "expression", NULL, NULL, MXML_DESCEND);
			return (char *)mxmlGetOpaque (expression);
		}
	}
	return	NULL;
}

#if 0 // DEPRECATED
/**
 * @brief Search all <block> that reference an output variable. Add them in the exec list.
 *
 * @param ctx The execution context
 * @param xo_pou Xml pou object
 * @param localId
 * @param formalParameter
 * @return
 */
void searchBlocksAndAdd(iec_ctx_t *ctx, void *xo_pou, char *localId, char *formalParameter) {
	//RTL_TRDBG (TRACE_DETAIL, "searchBlocksAndAdd(%s,%s)\n", localId, formalParameter);
	//RTL_TRDBG (TRACE_DETAIL, "pou name %s\n", (char *)mxmlElementGetAttr(xo_pou, "name"));
	mxml_node_t *block = xo_pou;
	mxml_node_t *fbd = mxmlFindElement(block, xo_pou, "FBD", NULL, NULL, MXML_DESCEND);
	if	(!fbd) {
		RTL_TRDBG (TRACE_DETAIL, "No more <FBD> blocks\n");
		return;
	}
	block	= fbd;

	while	((block = mxmlFindElement(block, xo_pou, "block", NULL, NULL, MXML_DESCEND))) {
		//char *typeName = (char *)mxmlElementGetAttr(block, "typeName");
		char *instanceName = (char *)mxmlElementGetAttr(block, "instanceName");
		//RTL_TRDBG (TRACE_DETAIL, "%s\n", typeName);
		mxml_node_t *inputVariables = mxmlFindElement(block, block, "inputVariables", NULL, NULL, MXML_DESCEND);
		mxml_node_t *variable = inputVariables;

		while	((variable = mxmlFindElement(variable, inputVariables, "variable", NULL, NULL, MXML_DESCEND))) {
			mxml_node_t *connectionPointIn = mxmlFindElement(variable, variable, "connectionPointIn",
				NULL, NULL, MXML_DESCEND);
			mxml_node_t *connection = mxmlFindElement(connectionPointIn, connectionPointIn, "connection",
				NULL, NULL, MXML_DESCEND);

			char *refLocalId = (char *)mxmlElementGetAttr(connection, "refLocalId");
			char *param = (char *)mxmlElementGetAttr(connection, "formalParameter");

			if	(refLocalId && param && !strcmp(refLocalId, localId) && !strcmp(param, formalParameter)) {
				RTL_TRDBG (TRACE_DETAIL, "Found block (%s,%s,%s)\n", localId, param, instanceName);
				Block_AddExec(ctx, block, instanceName);
			}
		}
	}
}
#endif

void Block_CreateExecList(iec_ctx_t *ctx, void *xo_pou) {
	mxml_node_t *fbd = mxmlFindElement(xo_pou, xo_pou, "FBD", NULL, NULL, MXML_DESCEND);
	if	(!fbd)
		return;

	mxml_node_t *block = fbd;
	int flg = MXML_DESCEND;
	while	((block = mxmlFindElement(block, fbd, "block", NULL, NULL, flg))) {
		flg = MXML_NO_DESCEND;
		Block_AddExec_v2(ctx, block);
	}
	Block_ExecDump(ctx);
}

//	Recherche et affecte les variables linkées avec une sortie de block
void searchVarsOfType(iec_ctx_t *ctx, void *xo_pou, char *localId, char *formalParameter, char *type) {
	mxml_node_t *variable;

	variable = xo_pou;
	while	((variable = mxmlFindElement(variable, xo_pou, type, NULL, NULL, MXML_DESCEND))) {
		mxml_node_t *connectionPointIn = mxmlFindElement(variable, variable, "connectionPointIn", NULL, NULL, MXML_DESCEND);
		if	(connectionPointIn) {
			mxml_node_t *connection = mxmlFindElement(connectionPointIn, connectionPointIn, "connection", NULL, NULL, MXML_DESCEND);
			if	(connection) {
				char *refLocalId = (char *)mxmlElementGetAttr(connection, "refLocalId");
				char *fp = (char *)mxmlElementGetAttr(connection, "formalParameter");

				if	(refLocalId && fp && !strcmp(refLocalId, localId) && !strcmp(fp, formalParameter)) {
					mxml_node_t *expression = mxmlFindElement(variable, variable, "expression", NULL, NULL, MXML_DESCEND);
					char *target = (char *)mxmlGetOpaque (expression);
					if	(!target) continue;
					char *source = NULL;
					iec_asprintf (&source, "%s.%s", localId, formalParameter);
					copyVariable (ctx, source, ctx, target, TRUE);
					free (source);
				}
			}
		}
	}
}

void searchVars(iec_ctx_t *ctx, void *xo_pou, char *localId, char *formalParameter) {
	RTL_TRDBG (TRACE_DETAIL, "VAR_TRACE_searchVars linked to (%s,%s)\n", localId, formalParameter);
	searchVarsOfType (ctx, xo_pou, localId, formalParameter, "outVariable");
	searchVarsOfType (ctx, xo_pou, localId, formalParameter, "inOutVariable");
}


iec_ctx_t *_Current_Context; // functions can access vars using this
iec_ctx_t *getCurrentContext() {
	return _Current_Context;
}

/**
 * @brief Run a block
 * @param program program object
 * @return 1=program can continue, 0=program must stop
 */
int Block_Start (iec_program_t *program) {
	int rc = 1;
	void *xo_pou = program->xo_pou;
	void *xo_block = program->xo_block;
	char *localId = (char *)mxmlElementGetAttr(xo_block, "localId");

	iec_ctx_t *ctx = Context_Current(program);
	_Current_Context = ctx;

	RTL_TRDBG (TRACE_BLOCK, "[%s] startBlock %s\n", ctx->name, localId);
	//dumpVariables(ctx);

	char *fctname1 = NULL;		// simple name
	char *fctname2 = NULL;		// typed
	char *fctname3 = NULL;		// typed
	char *fctname4 = NULL;		// typed

	param_u_t callParams[IEC_MAX_CALLVARS];
	char tmp[100];

	iec_asprintf (&fctname1, "%s", (char *)mxmlElementGetAttr(xo_block, "typeName"));
	iec_asprintf (&fctname2, "%s", fctname1);
	iec_asprintf (&fctname3, "%s", fctname1);
	iec_asprintf (&fctname4, "%s", fctname1);

	memset (callParams, 0, sizeof(callParams));
	program->countParams	= 0;

RTL_TRDBG (TRACE_BLOCK, "FBD_TRACE_BlockStart inputVariables [%s]\n", ctx->name);

	// 1. Valoriser les paramètres en entrée
	mxml_node_t *inputVariables = mxmlFindElement(xo_block, xo_block, "inputVariables", NULL, NULL, MXML_DESCEND);
	mxml_node_t *variable = inputVariables;
	while	((variable = mxmlFindElement(variable, inputVariables, "variable", NULL, NULL, MXML_DESCEND))) {
		mxml_node_t *connectionPointIn = mxmlFindElement(variable, variable, "connectionPointIn", NULL, NULL, MXML_DESCEND);
		mxml_node_t *connection = mxmlFindElement(connectionPointIn, connectionPointIn, "connection", NULL, NULL, MXML_DESCEND);
		char *refLocalId = (char *)mxmlElementGetAttr(connection, "refLocalId");
		char *formalParameter = (char *)mxmlElementGetAttr(variable, "formalParameter");
		char *fm2 = (char *)mxmlElementGetAttr(connection, "formalParameter");
		char * varname = "no name";
		iec_variable_t *var = NULL;
		int literalVarCount = 0;
		BOOL isLiteral = FALSE;

		//	Parametre dans une variable
		if	(!fm2) {
			//var = searchInputVar(ctx, xo_pou, refLocalId);
			varname = searchInputVarName(ctx, xo_pou, refLocalId);
			if	(varname)
				var = getVariable (getUpperContext(ctx), varname);

			// Variable not found, can be a literal
			if	(!var) {
				RTL_TRDBG (TRACE_DETAIL, "getLiteral(%s)\n", varname);
				param_ret_t *pret = getLiteral(varname);
				if	(pret) {
					sprintf (tmp, "___LIT___%02d", literalVarCount++);
					var = createVariableWithParameter2 (tmp, pret);
					isLiteral = TRUE;
				}
			}
		}
		else {
			// Ou bien parametre comme sortie d'un autre block : <localId>.<formalParameter>, par exemple "7.OUT"
			sprintf (tmp, "%s.%s", refLocalId, fm2);
			varname = tmp;
			var = getVariable (getUpperContext(ctx), varname);
		}

		if	(!var) {
			RTL_TRDBG (TRACE_ERROR, "FBD_TRACE_BlockStart %s needs an input variable %s. This can be an Execution Order issue.\n",
				localId, varname);
			User_GSC_log(program->task->resource->user, "BLOCK_START_FAILED", "Block abort", "",
				"Block %s needs an input variable %s. This can be an Execution Order issue.",
				localId, varname);
			rc = 0; goto bye;
		}

		// On concatene le premier type
		if	(program->countParams < 1)
			iec_asprintf (&fctname2, "_%s", typeString[(int)var->type]);
		if	(program->countParams < 2)
			iec_asprintf (&fctname3, "_%s", typeString[(int)var->type]);
		iec_asprintf (&fctname4, "_%s", typeString[(int)var->type]);

		// Parameter for function block
		if	(program->countParams < IEC_MAX_CALLVARS) {
			// Parameter for Function
			callParams[program->countParams]	= var->value;

			// Parameter for Function Block
			iec_variable_t *dest;
			if	(!isLiteral)
				dest = cloneVariable(var);
			else
				dest = var;
			program->callVars[program->countParams] = dest;
			strcpy (dest->name, formalParameter);
			program->countParams++;
		}
		RTL_TRDBG (TRACE_BLOCK, "FBD_TRACE_BlockStart Context [%s] parameter [%s] => [%s.%s]\n",
			ctx->name, var->name, localId, formalParameter);
	}


/*
	TODO : idem avec inOutVariables
*/
RTL_TRDBG (TRACE_BLOCK, "FBD_TRACE_BlockStart inOutVariables [%s] ---- NOT IMPLEMENTED ----\n", ctx->name);





	//RTL_TRDBG (TRACE_BLOCK, "%s %s %s %s\n", fctname1, fctname2, fctname3, fctname4);

	iec_fctinfos_t *fctinfo = searchStdFunc (fctname4);
	if	(fctinfo) RTL_TRDBG (TRACE_DETAIL, "FBD_TRACE_BlockStart searchStdFunc(4) => [%s] %s varargs=%d\n", fctinfo->name, fctinfo->ret_type, fctinfo->varargs);

	if	(!fctinfo || !*fctinfo->ret_type) {
		fctinfo = searchStdFunc (fctname3);
		if	(fctinfo) RTL_TRDBG (TRACE_DETAIL, "FBD_TRACE_BlockStart searchStdFunc(3) => [%s] %s varargs=%d\n", fctinfo->name, fctinfo->ret_type, fctinfo->varargs);
	}
	if	(!fctinfo || !*fctinfo->ret_type) {
		fctinfo = searchStdFunc (fctname2);
		if	(fctinfo) RTL_TRDBG (TRACE_DETAIL, "FBD_TRACE_BlockStart searchStdFunc(2) => [%s] %s varargs=%d\n", fctinfo->name, fctinfo->ret_type, fctinfo->varargs);
	}
	if	(!fctinfo || !*fctinfo->ret_type) {
		fctinfo = searchStdFunc (fctname1);
		if	(fctinfo) RTL_TRDBG (TRACE_DETAIL, "FBD_TRACE_BlockStart searchStdFunc(1) => [%s] %s varargs=%d\n", fctinfo->name, fctinfo->ret_type, fctinfo->varargs);
	}

	//	2. Est-ce une fonction standard ?
	if	(fctinfo && *fctinfo->ret_type) {
		void	(*fct)() = fctinfo->fct;
		param_ret_t RET[IEC_MAX_RETVARS];
		BOOL EN=TRUE, ENO;		// TODO : EN is always TRUE. Correct it !

		RTL_TRDBG (TRACE_DETAIL, "FBD_TRACE_BlockStart Call function [%s] with %d params (varargs=%d)\n", fctinfo->name, program->countParams, fctinfo->varargs);

		memset (RET, 0, sizeof(RET));
		RET[0].t	= -1;
		(*fct)(EN, &ENO, RET, program->countParams, callParams);

		if	(ENO == FALSE) {
			RTL_TRDBG (TRACE_DETAIL, "FBD_TRACE_BlockStart ENO FALSE. Execution continues.\n");
			goto bye;
		}

		mxml_node_t *outputVariables = mxmlFindElement(xo_block, xo_block, "outputVariables", NULL, NULL, MXML_DESCEND);
		mxml_node_t *variable = outputVariables;
		param_ret_t *pret = RET;

		while	((variable = mxmlFindElement(variable, outputVariables, "variable", NULL, NULL, MXML_DESCEND))) {
			char *formalParameter = (char *)mxmlElementGetAttr(variable, "formalParameter");

			//	Les fonctions standards ne modifient pas le type de retour. On utilise le code retour définit dans le tableau de fonctions.

			if	(pret->t == -1) {
				pret->t	= typeInteger(fctinfo->ret_type);
				RTL_TRDBG (TRACE_DETAIL, "FBD_TRACE_BlockStart RET type not set. Use function return %s\n", fctinfo->ret_type);
			}

			int rc = updateVariableWithParameter(ctx, formalParameter, pret);
			if (rc < 0) {
				iec_variable_t *var = createVariableWithParameter2 (formalParameter, pret);
				addVariableInContext (ctx, var);
			}

			pret	++;
		}
		//	No more need callVars : only for FBDs
		int i;
		for	(i=0; i < program->countParams ; i++) {
			iec_variable_t *var = program->callVars[i];
			freeVariable(var);
		}
		program->countParams = 0;
	}
	else {
		// appeler le pou avec la liste des paramètres
		char *name = (char *)mxmlElementGetAttr(xo_block, "typeName");
		xo_pou	= searchPou(program->task->resource->user->project, name);
		if	(!xo_pou) {
			RTL_TRDBG (TRACE_ERROR, "FBD_TRACE_BlockStart Unable to find a function or pou having the name [%s]\n", name);
			User_GSC_log(program->task->resource->user, "POU_UNDEFINED", "A pou is undefined. Aborting.", "", "Can't find pou %s", name);
			rc = 0; goto bye;
		}
		RTL_TRDBG (TRACE_DETAIL, "FBD_TRACE_BlockStart It's a <pou>\n");
		program->xo_pou	= xo_pou;
		ctx = Context_New (program, (char *)mxmlElementGetAttr(xo_pou, "name"), CTX_TYPE_POU);
		ctx->xo_pou	= program->xo_pou;
	}
bye:
	free (fctname1);
	free (fctname2);
	free (fctname3);
	free (fctname4);
	return	rc;
}

void Block_End (iec_program_t *program, void *xo_pou, void *xo_block) {
	char tmp[100];
	iec_ctx_t *ctx = Context_Current(program);

	RTL_TRDBG (TRACE_BLOCK, "FBD_TRACE_BlockEnd endBlock [%s]\n", ctx->name);
	//dumpVariables(getUpperContext(ctx));
	// 3. Valoriser toutes les variables faisant reference à cette sortie.
	char	*localId = (char *)mxmlElementGetAttr(xo_block, "localId");
	mxml_node_t *outputVariables = mxmlFindElement(xo_block, xo_block, "outputVariables", NULL, NULL, MXML_DESCEND);

	mxml_node_t *variable = outputVariables;
	while	((variable = mxmlFindElement(variable, outputVariables, "variable", NULL, NULL, MXML_DESCEND))) {
		char *formalParameter = (char *)mxmlElementGetAttr(variable, "formalParameter");
		char *expression = searchOutputVar(xo_pou, localId, formalParameter);
		if	(expression)
			copyVariable (ctx, formalParameter, getUpperContext(ctx), expression, TRUE);

		// 3b Creation de la variable <localId>.<formalParameter>, par exemple "7.OUT"
		sprintf (tmp, "%s.%s", localId, formalParameter);
		copyVariable (ctx, formalParameter, getUpperContext(ctx), tmp, TRUE);
	}

	//	Retrieve execution info (xo_ab, xo_action and step) before Context_Pop
	//	These info are used to continue the actionBlock
	mxml_node_t *xo_ab = ctx->xo_ab;
	mxml_node_t *xo_action = ctx->xo_action;
	iec_step_t *step = ctx->step;

	// 4. On quitte le block
	//dumpVariables(getUpperContext(ctx));
	ctx	= Context_Pop (program);
	//dumpVariables(ctx);

	// 5. Rechercher les <block> qui font reference à cette sortie. Les dédoublonner et les appeler.
	outputVariables = mxmlFindElement(xo_block, xo_block, "outputVariables", NULL, NULL, MXML_DESCEND);
	variable = outputVariables;
	while	((variable = mxmlFindElement(variable, outputVariables, "variable", NULL, NULL, MXML_DESCEND))) {
		char *formalParameter = (char *)mxmlElementGetAttr(variable, "formalParameter");
#ifdef OLD
		searchBlocksAndAdd(ctx, xo_pou, localId, formalParameter);
#endif
		searchVars(ctx, xo_pou, localId, formalParameter);
	}
	ctx->m_next	= TRUE;

	//	Continue the actionBlock (if any)
	if	(xo_ab && xo_action) {
		RTL_TRDBG (TRACE_DETAIL, "FBD_TRACE_BlockEnd This FBD was executed in an <actionBlock>. Continue with next action.\n");
		runActions (program, step, xo_ab, xo_action);
	}
}

/**
 * @brief 
 * Le premier block devant etre executé est celui n'ayant aucune dépendance sur la sortie d'un autre block.
 * Donc celui dont les <inputVar> n'ont pas de formalParameter
 * @param xo_pou
 * @return Pointer to XML block
 */
void *Block_FindFirst (void *xo_pou) {
	mxml_node_t *block = xo_pou;
	mxml_node_t *fbd = mxmlFindElement(block, xo_pou, "FBD", NULL, NULL, MXML_DESCEND);
	RTL_TRDBG (TRACE_DETAIL, "FBD_TRACE_Block_FindFirst\n");

	if	(!fbd) {
		RTL_TRDBG (TRACE_ERROR, "FBD_TRACE_Block_FindFirst No <FBD> section. This library is only able to handle FBD language\n");
		return	NULL;
	}
	block	= fbd;

	while	((block = mxmlFindElement(block, xo_pou, "block", NULL, NULL, MXML_DESCEND))) {
		BOOL ok = TRUE;
		mxml_node_t *inputVariables = mxmlFindElement(block, block, "inputVariables", NULL, NULL, MXML_DESCEND);
		mxml_node_t *variable = inputVariables;
		while	((variable = mxmlFindElement(variable, inputVariables, "variable", NULL, NULL, MXML_DESCEND))) {
			mxml_node_t *connectionPointIn = mxmlFindElement(variable, variable, "connectionPointIn", NULL, NULL, MXML_DESCEND);
			mxml_node_t *connection = mxmlFindElement(connectionPointIn, connectionPointIn, "connection", NULL, NULL, MXML_DESCEND);
			char *param = (char *)mxmlElementGetAttr(connection, "formalParameter");
			if	(param) ok = FALSE;
		}
		if	(ok) return	block;
	}
	return	NULL;
}

char *_mxmlGetFirstElement(mxml_node_t *node) {
	for	(node=mxmlGetFirstChild(node); node; node=mxmlGetNextSibling(node))
		if (mxmlGetType(node) == MXML_ELEMENT)
			return (char *)mxmlGetElement(node);
	return NULL;
}

/**
 * @brief Run a block (FBD)
 * @param program User object
 * @return  void
 */
void Block_Next (iec_program_t *program) {
	iec_ctx_t *ctx = Context_Current(program);

	ctx->m_state	++;
	RTL_TRDBG (TRACE_BLOCK, "Block_Cont Context state=%d\n", ctx->m_state);

	switch (ctx->m_state) {
	case	1 : // *LNI* requires define
		//	Block_Start rend 1 si on peut passer à la suite
		if (Block_Start (program) == 1)
			ctx->m_next	= TRUE;
		break;
	case	2 : // *LNI* requires define
		Block_End (program, program->xo_pou, program->xo_block);
		break;
	}
	Program_Reschedule(program);
}

mxml_node_t *searchBlockByName(void *xo_pou, char *name) {
	mxml_node_t *block = xo_pou;

	while	((block = mxmlFindElement(block, xo_pou, "block", NULL, NULL, MXML_DESCEND))) {
		char *instanceName = (char *)mxmlElementGetAttr(block, "instanceName");
		if	(instanceName && !strcmp(instanceName, name))
			return	block;
	}
	return	NULL;
}

int runBlockByName(iec_program_t *program, char *name) {
	mxml_node_t *xo_block = searchBlockByName(program->xo_pou, name);
	if	(!xo_block)
		return -1;

	program->xo_block = xo_block;
	char *ctx_name = (char *)mxmlElementGetAttr(program->xo_block, "instanceName");
	iec_ctx_t *ctx = Context_New (program, ctx_name, CTX_TYPE_BLOCK);
	ctx->xo_block	= program->xo_block;
	ctx->xo_pou	= program->xo_pou;
	return	0;
}

#if 0 // DEPRECATED
void Block_AddExec(iec_ctx_t *ctx, void *xo_block, char *name) {
	struct list_head *elem;

	//RTL_TRDBG (TRACE_DETAIL, "Block_AddExec %s\n", name);
	//	block already exists for execution ?
	list_for_each (elem, &ctx->block_list) {
		iec_block_t *pt	= list_entry(elem, iec_block_t, head);
		if	(pt->xo_block == xo_block)
			return;
	}

	//	add block
	iec_block_t *block = Calloc(sizeof(iec_block_t), 1);
	block->xo_block	= xo_block;
	block->name = strdup(name);
	list_add_tail (&block->head, &ctx->block_list);
}
#endif

void Block_AddExec_v2(iec_ctx_t *ctx, void *xo_block) {
	char *instanceName = (char *)mxmlElementGetAttr(xo_block, "instanceName");
	if	(!instanceName)
		instanceName = (char *)mxmlElementGetAttr(xo_block, "typeName");
	char *executionOrderId = (char *)mxmlElementGetAttr(xo_block, "executionOrderId");

	//RTL_TRDBG (TRACE_DETAIL, "FBD_TRACE_Block_AddExec_v2 %s %s\n", instanceName, executionOrderId);

	iec_block_t *block = Calloc(sizeof(iec_block_t), 1);
	block->xo_block	= xo_block;
	block->name = strdup(instanceName);

	if	(!executionOrderId) {
		list_add_tail (&block->head, &ctx->block_list);
	}
	else {
		struct list_head *elem = NULL;
		struct list_head *prev = &ctx->block_list;
		iec_block_t *pt = NULL;
		char *order;
		list_for_each (elem, &ctx->block_list) {
			pt	= list_entry(elem, iec_block_t, head);
			order = (char *)mxmlElementGetAttr(pt->xo_block, "executionOrderId");
			if	(!order)
				break;
			if	(atoi(executionOrderId) < atoi(order))
				break;
			prev = elem;
		}
		if	(pt) {
			pt	= list_entry(prev, iec_block_t, head);
			list_add (&block->head, &pt->head);
			//RTL_TRDBG (TRACE_DETAIL, "add after %s\n", (char *)mxmlElementGetAttr(pt->xo_block, "instanceName"));
		}
		else {
			list_add_tail (&block->head, &ctx->block_list);
		}
	}
}

void Block_ExecDump(iec_ctx_t *ctx) {
	struct list_head *elem;
	list_for_each (elem, &ctx->block_list) {
		iec_block_t *pt	= list_entry(elem, iec_block_t, head);
		RTL_TRDBG (TRACE_DETAIL, "Block %s %s %s\n",
			(char *)mxmlElementGetAttr(pt->xo_block, "instanceName"),
			(char *)mxmlElementGetAttr(pt->xo_block, "typeName"),
			(char *)mxmlElementGetAttr(pt->xo_block, "executionOrderId"));
	}
}

void *Block_NextExec(iec_ctx_t *ctx) {
	if	(list_empty(&ctx->block_list))
		return	NULL;
	iec_block_t *block = list_entry((&ctx->block_list)->next, iec_block_t, head);
	void *xo_block = block->xo_block;
	RTL_TRDBG (TRACE_DETAIL, "Block_NextExec %s\n", block->name);
	list_del (&block->head);
	free (block->name);
	free (block);
	return	xo_block;
}


//-------------------------------------------
//	SFC
//-------------------------------------------


#if 0
/**
 * @brief Run a ST piece of code
 * @param program
 * @param A pointer to the xml representation of the code
 * @return The result of the code's evaluation, or TRUE by default
 */
IEC_BOOL runST(iec_program_t *program, mxml_node_t *ST) {
	BOOL b = FALSE;
	extern param_ret_t *do_ST_interpreter();
	ST	= mxmlWalkNext (ST, NULL, MXML_DESCEND);
	if	(ST->type == MXML_OPAQUE) {
		char *STcode = (char *)mxmlGetOpaque (ST);
		//if	(*STcode == '\n') {
		if	(STcode && !strcmp(STcode, "\n")) {
			ST	= mxmlWalkNext (ST, NULL, MXML_DESCEND);
			STcode = (char *)mxmlGetCDATA(ST);
			while	(STcode[strlen(STcode)-1] == ']')
				STcode[strlen(STcode)-1] = 0;
		}
		RTL_TRDBG (TRACE_ST, "Execute ST code : (%s)\n", STcode);
		param_ret_t *ret = do_ST_interpreter (program, STcode);
		if	(ret) {
			if	(ret->t == BOOLtype)
				b = ret->p.BOOLvar;
			free (ret);
		}
	}
	else if	(ST->type == MXML_ELEMENT) {
		char *STcode = ST->value.element.name;
		if	(!memcmp(STcode, "![CDATA[", 8))
			STcode += 8;
		while	(STcode[strlen(STcode)-1] == ']')
			STcode[strlen(STcode)-1] = 0;
		RTL_TRDBG (TRACE_ST, "Execute ST code : (%s)\n", STcode);
		param_ret_t *ret = do_ST_interpreter (program, STcode);
		if	(ret) {
			if	(ret->t == BOOLtype)
				b = ret->p.BOOLvar;
			free (ret);
		}
	}
	return	b;
}
#endif


/**
 * @brief Run a ST piece of code. Version II : this enhanced version pre-compile
 *        the code and store its syntax tree into the attribute "tree".
 *        The code is compiled only the first time.
 * @param program An execution context (to get/set variables)
 * @param ST A pointer to the xml representation of the code
 * @return The result of the code's evaluation, or TRUE by default
 */
IEC_BOOL runST(iec_program_t *program, mxml_node_t *ST) {
	BOOL b = FALSE;

	ST	= mxmlWalkNext (ST, NULL, MXML_DESCEND);

	if	(ST->type == MXML_ELEMENT) {
		void *tree = (void *)mxmlElementGetAttr (ST, "tree");
		if	(tree) {
			tree	= (void *)atol(tree);
		}
		else {
			char *STcode = ST->value.element.name;
			if	(!memcmp(STcode, "![CDATA[", 8))
				STcode += 8;
			while	(STcode[strlen(STcode)-1] == ']')
				STcode[strlen(STcode)-1] = 0;
			RTL_TRDBG (TRACE_ST, "Compile ST code : (%s)\n", STcode);
			if	(compileST (STcode) == 0) {
				RTL_TRDBG (TRACE_ST, "ST code precompiled at %p\n", programBlock);
				mxmlElementSetAttrf (ST, "tree", "%ld", (long)programBlock);
				tree = programBlock;
			}
			else {
				RTL_TRDBG (TRACE_ST, "ERROR compiling ST code : (%s)\n", STcode);
				User_GSC_log(program->task->resource->user, "ST_COMPILATION_FAILED", "ERROR compiling ST code", "", "%s", STcode);
			}
		}
		if	(tree) {
			RTL_TRDBG (TRACE_ST, "Execute ST pcode at %p\n", tree);
			param_ret_t *ret = codeGen (tree, program);
			if	(ret) {
				if	(ret->t == BOOLtype)
					b = ret->p.BOOLvar;
				free (ret);
			}
		}
	}

	return	b;
}

/**
 * @brief Search a transition by its name
 * @param program
 * @param name
 * @return XML element or NULL
 */
mxml_node_t *transitionByReference(iec_program_t *program, char *name) {
	mxml_node_t *transitions = mxmlFindElement(program->xo_pou, program->xo_pou, "transitions", NULL, NULL, MXML_DESCEND);
	mxml_node_t *xo_transition = transitions;
	int flg = MXML_DESCEND;
	while	((xo_transition = mxmlFindElement(xo_transition, transitions, "transition", NULL, NULL, flg))) {
		char *name2 = (char *)mxmlElementGetAttr(xo_transition, "name");
		if	(name2 && !strcmp(name, name2))
			return mxmlFindElement(xo_transition, xo_transition, "ST", NULL, NULL, MXML_DESCEND);
		flg = MXML_NO_DESCEND;
	}
	return	NULL;
}

/**
 * @brief Evaluate a condition.
 * @param program
 * @param xo_trans Transition xml element
 * @return The result of the code's evaluation, or TRUE by default
 */
IEC_BOOL runCondition(iec_program_t *program, mxml_node_t *xo_trans) {
	mxml_node_t *condition = mxmlFindElement(xo_trans, xo_trans, "condition", NULL, NULL, MXML_DESCEND);
	mxml_node_t *reference = mxmlFindElement(condition, condition, "reference", NULL, NULL, MXML_DESCEND);
	mxml_node_t *code = mxmlFindElement(condition, condition, "inline", NULL, NULL, MXML_DESCEND);

	mxml_node_t *ST = NULL;
	if	(code) {
		ST = mxmlFindElement(code, code, "ST", NULL, NULL, MXML_DESCEND);
		RTL_TRDBG (TRACE_TRANSITION, "SFC_TRACE_transition execute ST inline\n");
	}
	else if	(reference) {
		char *name = (char *)mxmlElementGetAttr(reference, "name");
		ST = transitionByReference(program, name);
		RTL_TRDBG (TRACE_TRANSITION, "SFC_TRACE_transition execute ST reference (name=%s)\n", name);
	}
	if	(!ST)
		return	TRUE;

	return runST (program, ST);
}

/**
 * @brief Search a transition pointing to a step (the pointer is a localId)
 * @param start The xml pointer where to start the research
 * @param localId Search criteria
 * @return A xml pointer to the transition found, or NULL
 */
mxml_node_t *searchTransition(mxml_node_t *start, char *localId) {
	mxml_node_t *xo_trans;

	xo_trans = start;
	int flg = MXML_DESCEND;
	while	((xo_trans = mxmlFindElement(xo_trans, start, "transition", NULL, NULL, flg))) {
		mxml_node_t *connectionPointIn = mxmlFindElement(xo_trans, xo_trans, "connectionPointIn", NULL, NULL, MXML_DESCEND);
		mxml_node_t *connection = mxmlFindElement(connectionPointIn, connectionPointIn, "connection", NULL, NULL, MXML_DESCEND);
		char *refLocalId = (char *)mxmlElementGetAttr(connection, "refLocalId");

		if	(refLocalId && !strcmp(refLocalId, localId)) {
			RTL_TRDBG (TRACE_TRANSITION, "SFC_TRACE_searchTransition refLocalId=%s\n", refLocalId);
			return	xo_trans;
		}
		flg = MXML_NO_DESCEND;
	}
	return	NULL;
}

/**
 * @brief Search a SelectionDivergence connected to the step
 * @param start A pointer to xml where to start the research
 * @param localId The localId of the step
 * @return A pointer to the xml element, or NULL
 */
mxml_node_t *searchSelectionDivergence(mxml_node_t *start, char *localId) {
	mxml_node_t *xo_trans;

	xo_trans = start;
	while	((xo_trans = mxmlFindElement(xo_trans, start, "selectionDivergence", NULL, NULL, MXML_DESCEND))) {
		mxml_node_t *connectionPointIn = mxmlFindElement(xo_trans, xo_trans, "connectionPointIn", NULL, NULL, MXML_DESCEND);
		mxml_node_t *connection = mxmlFindElement(connectionPointIn, connectionPointIn, "connection", NULL, NULL, MXML_DESCEND);
		char *refLocalId = (char *)mxmlElementGetAttr(connection, "refLocalId");

		if	(refLocalId && !strcmp(refLocalId, localId)) {
			RTL_TRDBG (TRACE_TRANSITION, "<selectionDivergence> refLocalId=%s\n", refLocalId);
			return	xo_trans;
		}
	}
	return	NULL;
}

void activateAllSteps(iec_program_t *program, mxml_node_t *start, char *localId);
mxml_node_t *searchStep (iec_program_t *program, mxml_node_t *start, iec_step_t *step, char *localId);

/**
 * @brief Search a step by localId
 * @param start A pointer to xml where to start the research
 * @param localId The localId of the step
 * @return A pointer to the xml element, or NULL
 */
mxml_node_t *searchStep2 (iec_program_t *program, mxml_node_t *start, iec_step_t *step, char *localId, BOOL *multiple) {
	mxml_node_t *xo_step;

	*multiple = FALSE;
	xo_step = start;
	while	((xo_step = mxmlFindElement(xo_step, start, "step", NULL, NULL, MXML_DESCEND))) {
		mxml_node_t *connectionPointIn = mxmlFindElement(xo_step, xo_step, "connectionPointIn", NULL, NULL, MXML_DESCEND);
		if	(!connectionPointIn)
			continue;
		mxml_node_t *connection = mxmlFindElement(connectionPointIn, connectionPointIn, "connection", NULL, NULL, MXML_DESCEND);
		char *refLocalId = (char *)mxmlElementGetAttr(connection, "refLocalId");

		if	(refLocalId && !strcmp(refLocalId, localId)) {
			RTL_TRDBG (TRACE_DETAIL, "SFC_TRACE_searchStep2 <step> found with localId=%s\n", localId);
			return	xo_step;
		}
	}

	xo_step = start;
	while	((xo_step = mxmlFindElement(xo_step, start, "jumpStep", NULL, NULL, MXML_DESCEND))) {
		mxml_node_t *connectionPointIn = mxmlFindElement(xo_step, xo_step, "connectionPointIn", NULL, NULL, MXML_DESCEND);
		if	(!connectionPointIn)
			continue;
		mxml_node_t *connection = mxmlFindElement(connectionPointIn, connectionPointIn, "connection", NULL, NULL, MXML_DESCEND);
		char *refLocalId = (char *)mxmlElementGetAttr(connection, "refLocalId");

		if	(refLocalId && !strcmp(refLocalId, localId)) {
			RTL_TRDBG (TRACE_DETAIL, "SFC_TRACE_searchStep2 <jumpStep> found with localId=%s\n", localId);
			char *targetName = (char *)mxmlElementGetAttr(xo_step, "targetName");
			xo_step = mxmlFindElement(start, start, "step", "name", targetName, MXML_DESCEND);
			if	(xo_step)
				RTL_TRDBG (TRACE_DETAIL, "SFC_TRACE_searchStep2 <step> found with name=%s\n", targetName);
			return	xo_step;
		}
	}

	xo_step = start;
	while	((xo_step = mxmlFindElement(xo_step, start, "selectionConvergence", NULL, NULL, MXML_DESCEND))) {
		mxml_node_t *connectionPointIn = xo_step;

		while ((connectionPointIn = mxmlFindElement(connectionPointIn, xo_step, "connectionPointIn", NULL, NULL, MXML_DESCEND))) {
			mxml_node_t *connection = mxmlFindElement(connectionPointIn, connectionPointIn, "connection", NULL, NULL, MXML_DESCEND);
			char *refLocalId = (char *)mxmlElementGetAttr(connection, "refLocalId");
			if	(refLocalId && !strcmp(refLocalId, localId)) {
				RTL_TRDBG (TRACE_DETAIL, "SFC_TRACE_searchStep2 <selectionConvergence> found with localId=%s\n", localId);

				// recursive call
				xo_step = searchStep (program, start, step, (char *)mxmlElementGetAttr(xo_step, "localId"));
				return	xo_step;
			}
		}
	}

	mxml_node_t *xo_div = start;
	while	((xo_div = mxmlFindElement(xo_div, start, "simultaneousDivergence", NULL, NULL, MXML_DESCEND))) {
		mxml_node_t *connectionPointIn = mxmlFindElement(xo_div, xo_div, "connectionPointIn", NULL, NULL, MXML_DESCEND);
		if	(!connectionPointIn)
			continue;
		mxml_node_t *connection = mxmlFindElement(connectionPointIn, connectionPointIn, "connection", NULL, NULL, MXML_DESCEND);
		if	(!connection)
			continue;
		char *refLocalId = (char *)mxmlElementGetAttr(connection, "refLocalId");
		if	(refLocalId && !strcmp(refLocalId, localId)) {
			RTL_TRDBG (TRACE_DETAIL, "SFC_TRACE_searchStep2 <simultaneousDivergence> found with localId=%s\n", localId);
			activateAllSteps (program, start, (char *)mxmlElementGetAttr(xo_div, "localId"));
			Step_Free (step);
			*multiple = TRUE;
			return	NULL;
		}
	}
	return	NULL;
}

mxml_node_t *searchStep (iec_program_t *program, mxml_node_t *start, iec_step_t *step, char *localId) {
	BOOL multiple;
	return	searchStep2(program, start, step, localId, &multiple);
}

BOOL runTransition(iec_program_t *program, mxml_node_t *start, mxml_node_t *xo_trans, iec_step_t *step, char *localId);

/**
 * @brief Execute each transition of a SelectionDivergence. Stops when a transition matches.
 * TODO : this function must sort the transition by priority
 * @param program
 * @param step
 * @param start xml pointer where to start
 * @param localId
 * @return void
 */
mxml_node_t *forAllTransitions(iec_program_t *program, iec_step_t *step, mxml_node_t *start, char *localId) {
	mxml_node_t *xo_trans;

	RTL_TRDBG (TRACE_DETAIL, "SFC_TRACE_forAllTransitions ===START\n");
	xo_trans = start;
	while	((xo_trans = mxmlFindElement(xo_trans, start, "transition", NULL, NULL, MXML_DESCEND))) {
		mxml_node_t *connectionPointIn = mxmlFindElement(xo_trans, xo_trans, "connectionPointIn", NULL, NULL, MXML_DESCEND);
		mxml_node_t *connection = mxmlFindElement(connectionPointIn, connectionPointIn, "connection", NULL, NULL, MXML_DESCEND);
		char *refLocalId = (char *)mxmlElementGetAttr(connection, "refLocalId");

		// TODO : must be sorted by priority

		if	(refLocalId && strcmp(refLocalId, localId))
			continue;

		if	(runTransition (program, program->xo_sfc, xo_trans, step, NULL))
			return	xo_trans;
		RTL_TRDBG (TRACE_DETAIL, "SFC_TRACE_forAllTransitions ======CONTINUE\n");
	}
	RTL_TRDBG (TRACE_DETAIL, "SFC_TRACE_forAllTransitions ===END\n");
	return	NULL;
}


mxml_node_t *actionByReference(iec_program_t *program, char *name) {
	mxml_node_t *actions = mxmlFindElement(program->xo_pou, program->xo_pou, "actions", NULL, NULL, MXML_DESCEND);
	mxml_node_t *xo_action = actions;
	while	((xo_action = mxmlFindElement(xo_action, actions, "action", NULL, NULL, MXML_DESCEND))) {
		char *name2 = (char *)mxmlElementGetAttr(xo_action, "name");
		if	(name2 && !strcmp(name, name2))
			return mxmlFindElement(xo_action, xo_action, "ST", NULL, NULL, MXML_DESCEND);
	}
	return	NULL;
}

/**
 * @brief Pulse action functions. 
 *        Test/Set/Reset
 */
BOOL stepActionPulseTestAndSet(iec_step_t *step, int localId) {
	unsigned int bit = localId;
	RTL_TRDBG (TRACE_DETAIL, "SFC_TRACE_stepActionPulseTestAndSet step=%p localId=%d bits=%lx\n", step, localId, step->pulses);
	if	(bit >= 32)
		return	FALSE;
	if	(step->pulses & (1 << bit))
		return	TRUE;
	step->pulses |= (1 << bit);
	return	FALSE;
}

void stepActionPulseSet(iec_step_t *step, int localId) {
	unsigned int bit = localId;
	if	(bit < 32)
		step->pulses |= (1 << bit);
	RTL_TRDBG (TRACE_DETAIL, "SFC_TRACE_stepActionPulseSet step=%p localId=%d bits=%lx\n", step, localId, step->pulses);
}

void stepActionPulseResetAll(iec_step_t *step) {
	step->pulses	= 0;
	step->attCounter = 0;
	RTL_TRDBG (TRACE_DETAIL, "SFC_TRACE_stepActionPulseResetAll step=%p bits=%lx\n", step, step->pulses);
}

/**
 * @brief Run all actions of an action block
 * An action block is composed of multi FBD and ST, each evaluated one after one
 * @param program The program
 * @param step current step
 * @param xo_ab Pointer to the xml representation of action block
 * @param xo_action Pointer to the previous action. This permits to chain actions.
 * @return void
 */
int runActions(iec_program_t *program, iec_step_t *step, mxml_node_t *xo_ab, mxml_node_t *xo_action) {
	RTL_TRDBG (TRACE_DETAIL, "SFC_TRACE_Start_runActions\n");
	if	(!xo_action)
		xo_action = xo_ab;

	while	((xo_action = mxmlFindElement(xo_action, xo_ab, "action", NULL, NULL, MXML_DESCEND))) {
		mxml_node_t *code = mxmlFindElement(xo_action, xo_action, "inline", NULL, NULL, MXML_DESCEND);
		mxml_node_t *reference = mxmlFindElement(xo_action, xo_action, "reference", NULL, NULL, MXML_DESCEND);
		//char *localId = (char *)mxmlElementGetAttr(xo_action, "localId");
		char *qualifier = (char *)mxmlElementGetAttr(xo_action, "qualifier");

		if	(qualifier && !strcmp(qualifier, "P")) {
			if	(stepActionPulseTestAndSet(step, step->attCounter++))
				continue;
		}

		if	(code) {
			mxml_node_t *ST = mxmlFindElement(code, code, "ST", NULL, NULL, MXML_DESCEND);
			if	(ST) {
				RTL_TRDBG (TRACE_ACTION, "SFC_TRACE_action ST inline\n");
				runST (program, ST);
			}
		}
		else if	(reference) {
			char *name = (char *)mxmlElementGetAttr(reference, "name");
			// search a global action
			mxml_node_t *ST = actionByReference(program, name);
			if	(ST) {
				RTL_TRDBG (TRACE_ACTION, "SFC_TRACE_action ST reference (name=%s)\n", name);
				runST (program, ST);
			}
			else if	(runBlockByName(program, name) == 0) {
				RTL_TRDBG (TRACE_ACTION, "SFC_TRACE_action FBD (name=%s)\n", name);

				//	runBlockByName has created a context. Then we store info
				//	in this context to perform next actions in actionBlock
				iec_ctx_t *ctx = Context_Current(program);
				ctx->xo_ab	= xo_ab;
				ctx->xo_action	= xo_action;
				ctx->step	= step;
				return 1;
			}
		}
	}
	RTL_TRDBG (TRACE_ACTION, "SFC_TRACE_End_runActions\n");
	return	0;
}

/**
 * @brief Run an action block when entering a step
 * @param program
 * @param step the current step
 * @param start
 * @param localId The localId of the step
 * @return void
 */
int runActionBlock(iec_program_t *program, iec_step_t *step, mxml_node_t *start, char *localId) {
	mxml_node_t *xo_ab;

	xo_ab = start;
	while	((xo_ab = mxmlFindElement(xo_ab, start, "actionBlock", NULL, NULL, MXML_DESCEND))) {
		mxml_node_t *connectionPointIn = mxmlFindElement(xo_ab, xo_ab, "connectionPointIn", NULL, NULL, MXML_DESCEND);
		mxml_node_t *connection = mxmlFindElement(connectionPointIn, connectionPointIn, "connection", NULL, NULL, MXML_DESCEND);
		if	(!connection)
			continue;
		char *refLocalId = (char *)mxmlElementGetAttr(connection, "refLocalId");

		if	(refLocalId && !strcmp(refLocalId, localId)) {
			step->attCounter = 0;
			return runActions (program, step, xo_ab, xo_ab);
		}
	}
	RTL_TRDBG (TRACE_DETAIL, "SFC_TRACE_runActionBlock No <actionBlock> for localId %s\n", localId);
	return	0;
}

/**
 * @brief Create a step. Add it to the list of active steps
 * @param program The program
 * @param name The name of this step
 * @param xo_step The xml pointer
 * @return An step object
 */
iec_step_t *Step_New(iec_program_t *program, char *name, mxml_node_t *xo_step) {
	iec_step_t *step = Calloc(sizeof(iec_step_t), 1);
	step->xo_step	= xo_step;
	step->name = strdup(name);
	list_add_tail (&step->head, &program->activeSteps_list);
	RTL_TRDBG (TRACE_DETAIL, "Added step %s to activeSteps_list\n", step->name);
	stepActionPulseResetAll(step);
	return	step;
}

/**
 * @brief Replace a step
 * @param step The step object
 * @param name The name of this step
 * @param xo_step The xml pointer
 * @return void
 */
void Step_Replace(iec_step_t *step, char *name, mxml_node_t *xo_step) {
	if	(!step) return;
	if	(!strcmp(step->name, name)) return;
	RTL_TRDBG (TRACE_DETAIL, "Replaced step %s with %s to activeSteps_list\n", step->name, name);
	free (step->name);
	step->name = strdup(name);
	step->xo_step	= xo_step;
	stepActionPulseResetAll(step);
}

void Step_Free(iec_step_t *step) {
	if	(!step) return;
	RTL_TRDBG (TRACE_DETAIL, "Remove step %s\n", step->name);
	list_del (&step->head);
	free (step->name);
	free (step);
}

/**
 * @brief In case of a simultaneous divergence, activate all steps
 * @param program
 * @param start
 * @param localId
 * @return void
 */
void activateAllSteps(iec_program_t *program, mxml_node_t *start, char *localId) {
	mxml_node_t *xo_step;

	xo_step = start;
	while	((xo_step = mxmlFindElement(xo_step, start, "step", NULL, NULL, MXML_DESCEND))) {
		mxml_node_t *connectionPointIn = mxmlFindElement(xo_step, xo_step, "connectionPointIn", NULL, NULL, MXML_DESCEND);
		if	(!connectionPointIn)
			continue;
		mxml_node_t *connection = mxmlFindElement(connectionPointIn, connectionPointIn, "connection", NULL, NULL, MXML_DESCEND);
		if	(!connection)
			continue;
		char *refLocalId = (char *)mxmlElementGetAttr(connection, "refLocalId");

		if	(refLocalId && !strcmp(refLocalId, localId)) {
			RTL_TRDBG (TRACE_DETAIL, "<step> found with localId=%s\n", localId);
			Step_New(program, (char *)mxmlElementGetAttr(xo_step, "name"), xo_step);
			Program_Reschedule(program);
		}
	}
}

/**
 * @brief Get step by its localId
 * @param program
 * @param start Start of search in the xml tree
 * @param refLocalId
 * @return An iec_step_t object or NULL
 */
mxml_node_t *getStepByLocalId (iec_program_t *program, mxml_node_t *start, char *refLocalId) {
	mxml_node_t *xo_step;
	xo_step = start;
	while	((xo_step = mxmlFindElement(xo_step, start, "step", NULL, NULL, MXML_DESCEND))) {
		char *localId = (char *)mxmlElementGetAttr(xo_step, "localId");
		if	(localId && !strcmp(refLocalId, localId)) {
			return	xo_step;
		}
	}
	return	NULL;
}

/**
 * @brief Determine if a step is in active list
 * @param program User object
 * @param xo_step xml representation of the step
 * @return An iec_step_t object or NULL
 */
iec_step_t *isStepActive(iec_program_t *program, mxml_node_t *xo_step) {
	char *name = (char *)mxmlElementGetAttr(xo_step, "name");
	struct list_head *elem;
	list_for_each (elem, &program->activeSteps_list) {
		iec_step_t *step	= list_entry(elem, iec_step_t, head);
		if	(step && !strcmp(step->name, name))
			return	step;
	}
	return	NULL;
}

/**
 * @brief Activate a simultaneousConvergence if all steps are actives
 * @param program User object
 * @param xo_step xml representation of the step
 * @return An iec_step_t object or NULL
 */
mxml_node_t *simultaneousConvergence(iec_program_t *program, mxml_node_t *start, mxml_node_t *xo_div) {
	RTL_TRDBG (TRACE_DETAIL, "Check simultaneousConvergence %s\n", (char *)mxmlElementGetAttr(xo_div, "localId"));
	mxml_node_t *connectionPointIn = xo_div;
	while	((connectionPointIn = mxmlFindElement(connectionPointIn, xo_div, "connectionPointIn", NULL, NULL, MXML_DESCEND))) {
		mxml_node_t *connection = mxmlFindElement(connectionPointIn, connectionPointIn, "connection", NULL, NULL, MXML_DESCEND);
		if	(!connection)
			continue;
		char *refLocalId = (char *)mxmlElementGetAttr(connection, "refLocalId");
		mxml_node_t *xo_step	= getStepByLocalId(program, program->xo_sfc, refLocalId);
		if	(!xo_step)
			return	NULL;
		char *name = (char *)mxmlElementGetAttr(xo_step, "name");
		if	(!isStepActive(program, xo_step)) {
			RTL_TRDBG (TRACE_DETAIL, "Step %s is not active. No simultaneousConvergence\n", name);
			return	NULL;
		}
		RTL_TRDBG (TRACE_DETAIL, "Step %s is active\n", name);
	}

	RTL_TRDBG (TRACE_DETAIL, "All steps are active. Convergence can be done.\n");

	//	Then, activate the transition
	mxml_node_t *xo_trans = searchTransition (program->xo_sfc, (char *)mxmlElementGetAttr(xo_div, "localId"));
	if	(!xo_trans) {
		RTL_TRDBG (TRACE_ERROR, "Transition not found\n");
		User_GSC_log(program->task->resource->user, "TRANSITION_NOT_FOUND", "Transition not found", "", "Transition %s", (char *)mxmlElementGetAttr(xo_div, "localId"));
		return	NULL;
	}
	if	(!runTransition (program, program->xo_sfc, xo_trans, NULL, NULL)) {
		RTL_TRDBG (TRACE_DETAIL, "Transition not ok\n");
		return	NULL;
	}

	RTL_TRDBG (TRACE_DETAIL, "Now removing all steps from active list\n");

	connectionPointIn = xo_div;
	while	((connectionPointIn = mxmlFindElement(connectionPointIn, xo_div, "connectionPointIn", NULL, NULL, MXML_DESCEND))) {
		mxml_node_t *connection = mxmlFindElement(connectionPointIn, connectionPointIn, "connection", NULL, NULL, MXML_DESCEND);
		if	(!connection)
			continue;
		char *refLocalId = (char *)mxmlElementGetAttr(connection, "refLocalId");
		mxml_node_t *xo_step	= getStepByLocalId(program, program->xo_sfc, refLocalId);
		if	(!xo_step)
			return	NULL;
		iec_step_t *step = isStepActive(program, xo_step);
		if	(step)
			Step_Free (step);
	}

	return	xo_trans;
}

/**
 * @brief Scan all simultaneousConvergences for activation
 * @param program User object
 * @param start Where to start scan (usually xo_sfc)
 * @return  void
 */
void forAllSimultaneousConvergence(iec_program_t *program, mxml_node_t *start) {
	mxml_node_t *xo_div;

	xo_div = start;
	while	((xo_div = mxmlFindElement(xo_div, start, "simultaneousConvergence", NULL, NULL, MXML_DESCEND))) {
		simultaneousConvergence(program, program->xo_sfc, xo_div);
	}
}

/**
 * @brief Run a transition : run its condition and if condition returns TRUE, activate the next step
 * @param program The program
 * @param start
 * @param xo_trans
 * @param step
 * @param localId
 * @return
 */
BOOL runTransition(iec_program_t *program, mxml_node_t *start, mxml_node_t *xo_trans, iec_step_t *step, char *localId) {
	// 2. execute the condition
	if	(xo_trans) {
		IEC_BOOL ret = runCondition(program, xo_trans);
		RTL_TRDBG (TRACE_DETAIL, "SFC_TRACE_runTransition Transition Clearing? %s\n", ret?"TRUE":"FALSE");
		if	(ret == FALSE)	return	FALSE;
		RTL_TRDBG (TRACE_TRANSITION, "SFC_TRACE_runTransition transition Cleared\n");
	}

	if	(!localId && xo_trans)
		localId	= (char *)mxmlElementGetAttr(xo_trans, "localId");

	// 3. search target step
	BOOL multiple;
	mxml_node_t *xo_step = searchStep2 (program, start, step, localId, &multiple);
	if	(!xo_step) {
		if	(multiple)
			return	TRUE;
		RTL_TRDBG (TRACE_ERROR, "SFC_TRACE_runTransition Can't select next step %s\n", localId);
		return FALSE;
	}

	char *name = (char *)mxmlElementGetAttr(xo_step, "name");
	if	(!name)
		name = (char *)mxmlElementGetAttr(xo_step, "localId");
	RTL_TRDBG (TRACE_DETAIL, "SFC_TRACE_runTransition start searching step %s\n", name);

	if	(step) {
		Step_Replace (step, name, xo_step);
		iec_ctx_t *ctx = Context_Current (program);
		Context_Rename (ctx, name);
	}
	else {
		step = Step_New(program, name, xo_step);
	}

	Program_Reschedule(program); // *LNI* allow other programs instances to execute now
	return	TRUE;
}

/**
 * @brief Run all the active steps.
 * For each active step, search a transition : this can be a single transition, a divergence or a link
 * @param program The program
 * @return void
 */
void Step_Next (iec_program_t *program) {
	iec_ctx_t *ctx = Context_Current(program);
	struct list_head *elem, *temp;

	list_for_each_safe (elem, temp, &program->activeSteps_list) {
		iec_step_t *step	= list_entry(elem, iec_step_t, head);
		if	(!step)
			break;
		RTL_TRDBG (TRACE_STEP, "SFC_TRACE_Step_Next : %s m_state=%d\n", step->name, step->m_state);

		switch (step->m_state) {
		case	0 :
			if (runActionBlock (program, step, program->xo_sfc, (char *)mxmlElementGetAttr(step->xo_step, "localId"))) {
//RTL_TRDBG (TRACE_STEP, "SFC_TRACE_Step_Next : CASE <> 0 Jul 22, 2013 Needs Reschedule\n");
				step->m_state ++;
				Program_Reschedule(program);
				return;
			}
//RTL_TRDBG (TRACE_STEP, "SFC_TRACE_Step_Next : CASE == 0 Jul 22, 2013 No Reschedule\n");
			step->m_state ++;
			Program_Reschedule(program);
			break;

		case	1 :
			{
			char *localId = (char *)mxmlElementGetAttr(step->xo_step, "localId");
			mxml_node_t *xo_trans = searchTransition (program->xo_sfc, localId);
			if	(xo_trans) {
/*
				ISSUE : this 'break' cause step->m_state = 0; never done. m_state stay to 1
				if (runTransition (program, program->xo_sfc, xo_trans, step, NULL) == FALSE)
					break;
*/
				runTransition (program, program->xo_sfc, xo_trans, step, NULL);
			} else {
				RTL_TRDBG (TRACE_DETAIL, "No transition found for %s (%s)\n", localId, step->name);
				mxml_node_t *xo_sel = searchSelectionDivergence (program->xo_sfc, localId);
				if	(xo_sel)
					forAllTransitions(program, step, program->xo_sfc, (char *)mxmlElementGetAttr(xo_sel, "localId"));
				else {
					RTL_TRDBG (TRACE_DETAIL, "Trying direct link\n");
					runTransition (program, program->xo_sfc, NULL, step, localId);
				}
				RTL_TRDBG (TRACE_DETAIL, "*** Returning to Step_Next\n");
			}
			step->m_state = 0;
			}
			break;
		}
	}

	forAllSimultaneousConvergence(program, program->xo_sfc);
	ctx->m_next	= TRUE;
}





//-------------------------------------------
//	POU
//-------------------------------------------



/**
 * @brief Get a Pou by name. Search in xml script, then in global scripts GLOB.FBD_list
 * @param project
 * @param name Pou name
 * @return Pointer to XML pou
 */
void *searchPou(void *project, char *name) {
	if	(!name)
		return	NULL;
	mxml_node_t *pous = mxmlFindElement(project, project, "pous", NULL, NULL, MXML_DESCEND);
	RTL_TRDBG (TRACE_DETAIL, "FBD_TRACE_searchPOU\n");
	if	(!pous) {
		RTL_TRDBG (TRACE_ERROR, "FBD_TRACE_Cannot find <pous> section !\n");
		return	NULL;
	}

	mxml_node_t *pou = pous;
	int flg = MXML_DESCEND;
	while ((pou=mxmlFindElement(pou, pous, "pou", NULL, NULL, flg))) {
		char *val = (char *)mxmlElementGetAttr(pou, "name");
		if	(val && !strcmp(val, name))
			return pou;
		flg = MXML_NO_DESCEND;
	}

	struct list_head *elem;
	list_for_each (elem, &GLOB.FBD_list) {
		iec_fbd_t *fbd	= list_entry(elem, iec_fbd_t, head);
		pou = fbd->xml;
		flg = MXML_DESCEND;
		while ((pou=mxmlFindElement(pou, fbd->xml, "pou", NULL, NULL, flg))) {
			char *val = (char *)mxmlElementGetAttr(pou, "name");
			if	(val && !strcmp(val, name))
				return pou;
			flg = MXML_NO_DESCEND;
		}
	}
	return	NULL;
}

/*
void dumpPous(iec_user_t *user) {
	mxml_node_t *pous = mxmlFindElement(user->project, user->project, "pous", NULL, NULL, MXML_DESCEND);
	if	(!pous) {
		RTL_TRDBG (TRACE_ERROR, "Cannot find <pous> section !\n");
		return;
	}

	mxml_node_t *pou = pous;
	int flg = MXML_DESCEND;
	while ((pou=mxmlFindElement(pou, pous, "pou", NULL, NULL, flg))) {
		char *val = (char *)mxmlElementGetAttr(pou, "name");
		RTL_TRDBG (TRACE_DETAIL, "Pou list %s\n", val);
		flg = MXML_NO_DESCEND;
	}
}
*/



/**
 * @brief Returns the initial value of a variable
 * @param variable A pointer to the xml representation of a variable
 * @return The value
 */
static char *getInitialValue(mxml_node_t *variable) {
	char	*value = NULL;
	mxml_node_t *initialValue = mxmlFindElement(variable, variable, "initialValue", NULL, NULL, MXML_DESCEND);
	if	(initialValue) {
		mxml_node_t *simpleValue = mxmlFindElement(initialValue, initialValue, "simpleValue", NULL, NULL, MXML_DESCEND);
		if	(simpleValue)
			value = (char *)mxmlElementGetAttr(simpleValue, "value");
	}
	//RTL_TRDBG (TRACE_DETAIL, "getInitialValue %s\n", value);
	return	value;
}

/**
 * @brief Initialize POU localVars
 * @param ctx Execution context
 * @param xo_pou The pou XML object
 * @return
 */
void Pou_localVars (iec_ctx_t *ctx, void *xo_pou) {
	mxml_node_t *Vars, *variable;
	RTL_TRDBG (TRACE_BLOCK, "[%s] localVars\n", ctx->name);
	iec_user_t *user = ctx->program->task->resource->user;

	int flg = MXML_DESCEND;
	Vars = xo_pou;
	while	((Vars = mxmlFindElement(Vars, xo_pou, "localVars", NULL, NULL, flg))) {
		int flg2 = MXML_DESCEND_FIRST;
		variable = Vars;
		while	((variable = mxmlFindElement(variable, Vars, "variable", NULL, NULL, flg2))) {
			char *name = (char *)mxmlElementGetAttr(variable, "name");
			mxml_node_t *node = mxmlFindElement(variable, variable, "type", NULL, NULL, MXML_DESCEND);
			char *type = _mxmlGetFirstElement(node);

			// don't erase localVars if they exist
			if (Variable_Exists (ctx, name) == FALSE) {
				iec_variable_t *var = createVariable (name, typeInteger(type), getInitialValue(variable));
				addVariableInContext (ctx, var);

				char *address = (char *)mxmlElementGetAttr(variable, "address");
				if	(address) {
					RTL_TRDBG (TRACE_SET, "linkVariable %s => %s\n", name, address);
					if (linkVariable(ctx->m_variables, name, user->m_accessVars, address) < 0)
						RTL_TRDBG (TRACE_ERROR, "linkVariable FAILED %s => %s\n", name, address);
				}
			}
			flg2 = MXML_NO_DESCEND;
		}
		flg = MXML_NO_DESCEND;
	}
}

/**
 * @brief Initialize POU externalVars
 * @param ctx Execution context
 * @param xo_pou The pou XML object
 * @return
 */
void Pou_externalVars (iec_ctx_t *ctx, void *xo_pou) {
	mxml_node_t *Vars, *variable;
	RTL_TRDBG (TRACE_BLOCK, "[%s] externalVars\n", ctx->name);

	int flg = MXML_DESCEND;
	Vars = xo_pou;
	while	((Vars = mxmlFindElement(Vars, xo_pou, "externalVars", NULL, NULL, flg))) {
		int flg2 = MXML_DESCEND_FIRST;
		variable = Vars;
		while	((variable = mxmlFindElement(variable, Vars, "variable", NULL, NULL, flg2))) {
			char *name = (char *)mxmlElementGetAttr(variable, "name");
			RTL_TRDBG (TRACE_BLOCK, "linkExternalVar %s\n", name);
			if (linkExternalVar(ctx, name) < 0) {
				RTL_TRDBG (TRACE_ERROR, "\033[31;7mERROR linking externalVar %s\033[0m\n", name);
				User_GSC_log(ctx->program->task->resource->user, "LINK_EXTVAR", "ERROR linking externalVar", "",
					"ERROR linking externalVar %s", name);
			}
			flg2 = MXML_NO_DESCEND;
		}
		flg = MXML_NO_DESCEND;
	}
}

/**
 * @brief Initialize POU inputVars
 * @param ctx Execution context
 * @param xo_pou The pou XML object
 * @return
 */
void Pou_inputVars (iec_ctx_t *ctx, void *xo_pou) {
	mxml_node_t *Vars, *variable;
	RTL_TRDBG (TRACE_BLOCK, "[%s] inputVars\n", ctx->name);

	int flg = MXML_DESCEND;
	Vars = xo_pou;
	while	((Vars = mxmlFindElement(Vars, xo_pou, "inputVars", NULL, NULL, flg))) {
		int flg2 = MXML_DESCEND_FIRST;
		variable = Vars;
		while	((variable = mxmlFindElement(variable, Vars, "variable", NULL, NULL, flg2))) {
			char *name = (char *)mxmlElementGetAttr(variable, "name");
			mxml_node_t *node = mxmlFindElement(variable, variable, "type", NULL, NULL, MXML_DESCEND);
			char *type = _mxmlGetFirstElement(node);
			iec_variable_t *var = createVariable (name, typeInteger(type), NULL);
			addVariableInContext (ctx, var);
			flg2 = MXML_NO_DESCEND;
		}
		flg = MXML_NO_DESCEND;
	}

	RTL_TRDBG (TRACE_BLOCK, "[%s] inOutVars\n", ctx->name);

	flg = MXML_DESCEND;
	Vars = xo_pou;
	while	((Vars = mxmlFindElement(Vars, xo_pou, "inOutVars", NULL, NULL, flg))) {
		int flg2 = MXML_DESCEND_FIRST;
		variable = Vars;
		while	((variable = mxmlFindElement(variable, Vars, "variable", NULL, NULL, flg2))) {
			char *name = (char *)mxmlElementGetAttr(variable, "name");
			mxml_node_t *node = mxmlFindElement(variable, variable, "type", NULL, NULL, MXML_DESCEND);
			char *type = _mxmlGetFirstElement(node);
			iec_variable_t *var = createVariable (name, typeInteger(type), NULL);
			addVariableInContext (ctx, var);
			flg2 = MXML_NO_DESCEND;
		}
		flg = MXML_NO_DESCEND;
	}
}

/**
 * @brief Initialize POU parameters
 * @param ctx Execution context
 * @param xo_pou The pou XML object
 * @return
 */
void Pou_parameters(iec_program_t *program) {
	iec_ctx_t *ctx = Context_Current(program);
	RTL_TRDBG (TRACE_BLOCK, "[%s] parameters\n", ctx->name);
	int i;
	for	(i=0; i < program->countParams ; i++) {
		iec_variable_t *var = program->callVars[i];
		addVariableInContext (ctx, var);
	}
	program->countParams = 0;
}

void Pou_Start(iec_program_t *program) {
	iec_ctx_t *ctx = Context_Current(program);
	void *xo_pou = program->xo_pou;

	RTL_TRDBG (TRACE_BLOCK, "POU_TRACE_Pou_Next m_state 1: [%s] startPou and exsisting Vars\n", ctx->name);
	//dumpVariables(ctx);

	Pou_localVars (ctx, xo_pou);

	Pou_externalVars (ctx, xo_pou);

	//Pou_inputVars (ctx, xo_pou);

	Pou_parameters (program);

	mxml_node_t *fbd = mxmlFindElement(xo_pou, xo_pou, "FBD", NULL, NULL, MXML_DESCEND);
	mxml_node_t *sfc = mxmlFindElement(xo_pou, xo_pou, "SFC", NULL, NULL, MXML_DESCEND);

	if	(fbd) {
		RTL_TRDBG (TRACE_DETAIL, "POU_TRACE_Pou_Next m_state 1: This is a FBD program\n");

		Block_CreateExecList (ctx, xo_pou);
		program->xo_block	= Block_NextExec(ctx);

#ifdef OLD
		//	first block
		program->xo_block = Block_FindFirst(xo_pou);
#endif

		if	(program->xo_block) {
			char *ctx_name = (char *)mxmlElementGetAttr(program->xo_block, "instanceName");
			if	(!ctx_name)
				ctx_name = (char *)mxmlElementGetAttr(program->xo_block, "localId");
			ctx = Context_New (program, ctx_name, CTX_TYPE_BLOCK);
			ctx->xo_block	= program->xo_block;
			ctx->xo_pou	= program->xo_pou;
		}
	}
	else if	(sfc) {
		RTL_TRDBG (TRACE_DETAIL, "POU_TRACE_Pou_Next m_state 1: This is a SFC program\n");
		//	initial step
		mxml_node_t *xo_step = mxmlFindElement(xo_pou, sfc, "step", NULL, NULL, MXML_DESCEND);
		char *name = (char *)mxmlElementGetAttr(xo_step, "name");
		if	(!name)
			name = (char *)mxmlElementGetAttr(xo_step, "localId");
		RTL_TRDBG (TRACE_DETAIL, "SFC_TRACE_Initial step %s\n", name);
		program->xo_sfc	= sfc;
		program->xo_step	= xo_step;
		ctx = Context_New (program, name, CTX_TYPE_STEP);
		ctx->xo_step	= xo_step;
		ctx->xo_pou	= program->xo_pou;

		Step_New (program, name, xo_step);
	}
	else {
		RTL_TRDBG (TRACE_ERROR, "Pou_Next m_state 1: No <FBD> nor <SFC> section. This VM is only able to handle FBD and SFC languages\n");
		User_GSC_log(program->task->resource->user, "NO_FBD_SFC_SECTION", "No <FBD> nor <SFC> section. This VM is only able to handle FBD and SFC languages", "", "");
		return;
	}
	ctx->m_next	= TRUE;
}

/**
 * @brief Initialize POU localVars
 * @param ctx Execution context
 * @param xo_pou The pou XML object
 * @return
 */
void Pou_outputVars (iec_ctx_t *ctx, void *xo_pou) {
	iec_ctx_t *upper = getUpperContext(ctx);
	if	(!upper)
		return;

	mxml_node_t *Vars, *variable;
	RTL_TRDBG (TRACE_BLOCK, "[%s] outputVars\n", ctx->name);

	int flg = MXML_DESCEND;
	Vars = xo_pou;
	while	((Vars = mxmlFindElement(Vars, xo_pou, "outputVars", NULL, NULL, flg))) {
		int flg2 = MXML_DESCEND_FIRST;
		variable = Vars;
		while	((variable = mxmlFindElement(variable, Vars, "variable", NULL, NULL, flg2))) {
			char *name = (char *)mxmlElementGetAttr(variable, "name");
			copyVariable (ctx, name, upper, name, TRUE);
			flg2 = MXML_NO_DESCEND;
		}
		flg = MXML_NO_DESCEND;
	}

	flg = MXML_DESCEND;
	Vars = xo_pou;
	while	((Vars = mxmlFindElement(Vars, xo_pou, "inOutVars", NULL, NULL, flg))) {
		int flg2 = MXML_DESCEND_FIRST;
		variable = Vars;
		while	((variable = mxmlFindElement(variable, Vars, "variable", NULL, NULL, flg2))) {
			char *name = (char *)mxmlElementGetAttr(variable, "name");
			copyVariable (ctx, name, upper, name, TRUE);
			flg2 = MXML_NO_DESCEND;
		}
		flg = MXML_NO_DESCEND;
	}
}

void Pou_End(iec_program_t *program) {
	iec_ctx_t *ctx = Context_Current(program);
	void *xo_pou = program->xo_pou;

	RTL_TRDBG (TRACE_BLOCK, "Pou_Next m_state 1: [%s] endPou\n", ctx->name);
	//dumpVariables(ctx);
	dumpContexts(program);

	Pou_outputVars(ctx, xo_pou);

	ctx = Context_Pop (program);
	if	(!ctx)
		return;

	char *localId = (char *)mxmlElementGetAttr(program->xo_block, "localId");
	RTL_TRDBG (TRACE_DETAIL, "OK for localId %s\n", localId);

#ifdef OLD
	// 5. Rechercher les <block> qui font reference à cette sortie. Les dédoublonner et les appeler.
	mxml_node_t *outputVariables = mxmlFindElement(program->xo_block, program->xo_block, "outputVariables", NULL, NULL, MXML_DESCEND);
	variable = outputVariables;
	while	((variable = mxmlFindElement(variable, outputVariables, "variable", NULL, NULL, MXML_DESCEND))) {
		char *formalParameter = (char *)mxmlElementGetAttr(variable, "formalParameter");
		searchBlocksAndAdd(ctx, program->xo_pou, localId, formalParameter);
		//searchVars(ctx, xo_pou, localId, formalParameter);
	}
#endif
	ctx->m_next	= TRUE;
}

/**
 * @brief Run a Pou. Initialize variables. If its a FBD pou, search the first block and create a context to it.
 * If its a SFC pou, locate the initial step and create a context to it.
 * @param program
 * @return void
 */
void Pou_Next (iec_program_t *program) {
	iec_ctx_t *ctx = Context_Current(program);

	ctx->m_state	++;
	//RTL_TRDBG (TRACE_BLOCK, "Pou_Cont state=%d\n", ctx->m_state);

	switch (ctx->m_state) {
	case	1 :
		Pou_Start(program);
		break;
	case	2 :
		RTL_TRDBG (TRACE_DETAIL, "Pou_Next m_state 2\n");
		// 6. Déclencher les blocks
		program->xo_block	= Block_NextExec(ctx);
		if	(program->xo_block) {
			char *ctx_name = (char *)mxmlElementGetAttr(program->xo_block, "instanceName");
			if	(!ctx_name)
				ctx_name = (char *)mxmlElementGetAttr(program->xo_block, "localId");
			ctx->m_state --;
			ctx = Context_New (program, ctx_name, CTX_TYPE_BLOCK);
			ctx->xo_block	= program->xo_block;
			ctx->xo_pou	= program->xo_pou;
		}
		else {
			Pou_End(program);
		}
		break;
	}
	Program_Reschedule(program);
}

//-------------------------------------------
//	PROGRAM / TASK / RESOURCE / USER
//-------------------------------------------

iec_program_t *Program_New(iec_task_t *task, void *node) {
	char *type = (char *)mxmlElementGetAttr(node, "typeName");
	void *pou	= searchPou(task->resource->user->project, type);
	if	(!pou) {
		RTL_TRDBG (TRACE_ERROR, "No pou named '%s' found\n", type);
		User_GSC_log(task->resource->user, "POU_NOT_FOUND", "", "", "Can't find pou %s", type);
		return NULL;
	}

	iec_program_t *program = Calloc(sizeof(iec_program_t), 1);
	INIT_LIST_HEAD (&program->ctx_list);
	program->m_ctx_hash = rtl_htblCreate(32, NULL);
	program->name = strdup(mxmlElementGetAttr(node, "name"));
	program->typeName = strdup(type);
	INIT_LIST_HEAD (&program->activeSteps_list);
	program->task = task;	// back ptr
	program->xo_pou = pou;

	program->level = task->level;
	rtl_tracelevel (program->level);
	TraceLevel = program->level;

	//iec_ctx_t *ctx = Context_New (program, type, CTX_TYPE_POU);
	iec_ctx_t *ctx = Context_New (program, program->name, CTX_TYPE_POU);
	ctx->xo_pou	= program->xo_pou;
	return	program;
}

void Program_RedoFromStart(iec_program_t *program) {
	RTL_TRDBG (TRACE_ENGINE_SCHEDULE, "==== Program_Program_RedoFromStart\n");
	//iec_ctx_t *ctx = Context_New (program, program->typeName, CTX_TYPE_POU);
	iec_ctx_t *ctx = Context_New (program, program->name, CTX_TYPE_POU);
	ctx->xo_pou	= program->xo_pou;
}

/**
 * @brief Free a program.
 * @param program
 * @return void
 */
void Program_Free(iec_program_t *program) {
	struct list_head *elem, *pos;
	list_for_each_safe (elem, pos, &program->ctx_list) {
		iec_ctx_t *ctx	= list_entry(elem, iec_ctx_t, head);
		Context_Free (ctx);
	}

	list_for_each_safe (elem, pos, &program->activeSteps_list) {
		iec_step_t *step	= list_entry(elem, iec_step_t, head);
		Step_Free (step);
	}

	rtl_htblDestroy (program->m_ctx_hash);
	free (program->name);
	free (program->typeName);
	free (program);
}

TIME getXmlDuration(char *str) {
	TIME t = { 0, 0};
	double v;

	if	(str[0] == 'P' && str[1] == 'T') {
		str += 2;
		if	(str[strlen(str)-1] == 'S')
			str[strlen(str)-1] = 0;
	}

	v	= atof(str);
	t.tv_sec = floor(v);
	t.tv_nsec = (v-floor(v))*1E9;
	return t;
}

/**
 * @brief Create a task.
 * @param xo_task A pointer to the xml representation of a task
 * @return A pointer to an allocated iec_task_t object
 */
iec_task_t *Task_New(iec_resource_t *resource, void *node) {
	if	(!node)	return	NULL;
	iec_task_t *task = Calloc(sizeof(iec_task_t), 1);
	INIT_LIST_HEAD (&task->program_list);
	task->name = strdup(mxmlElementGetAttr(node, "name"));
	char *str = (char *)mxmlElementGetAttr(node, "interval");
	if	(!str) str="T#1s";
	task->interval_initial = task->interval = getDuration(str);

	char *res = User_GetDictionnary(resource->user, "iecMinCyclicInterval");
	if	(res) {
		TIME minCyclicInterval = getXmlDuration(res);
		if	(CMP_TIMESPEC(task->interval, minCyclicInterval) < 0) {
			task->interval = minCyclicInterval;
			User_GSC_log(resource->user, "MINCYCLICINTERVAL_CONSTRAINT", "minCyclicInterval constraint failure.", "",
			"Task's interval limited to %s", res);
		}
	}

	str = (char *)mxmlElementGetAttr(node, "priority");
	task->priority = str ? atoi(str) : 0;
	task->xo_task	= node;
	task->resource = resource;
	task->level = resource->level;

	if	(!NULL_TIMESPEC(task->interval)) {
		iec_gettimeofday (&task->next);
		ADD_TIMESPEC (task->next, task->interval);
	}
	RTL_TRDBG (TRACE_ENGINE_TIMING, "TASK %s created interval %ds%dms\n",
		task->name, task->interval.tv_sec, task->interval.tv_nsec/NSEC_PER_MSEC);
	return	task;
}

/**
 * @brief Free a task.
 * @param task A pointer to the task object
 * @return void
 */
void Task_Free(iec_task_t *task) {
	struct list_head *elem, *pos;
	list_for_each_safe (elem, pos, &task->program_list) {
		iec_program_t *program	= list_entry(elem, iec_program_t, head);
		Program_Free (program);
	}
	free (task->name);
	list_del (&task->head);
	free (task);
}

/**
 * @brief Run a task.
 * @param program
 * @return void
 */
void Task_Next (iec_task_t *task) {
	TIME start, end;
	//struct rusage ru;

	task->ticks ++;

	//getrusage (RUSAGE_SELF, &ru);
	//TIMEVAL2TIMESPEC (ru.ru_utime, start);
	iec_gettimeofday (&start);

	task->force_schedule = FALSE;
	struct list_head *elem;
	list_for_each (elem, &task->program_list) {
		iec_program_t *program	= list_entry(elem, iec_program_t, head);
		Program_Next (program);

		rtl_tracelevel (task->level);
		TraceLevel = task->level;
	}

	//getrusage (RUSAGE_SELF, &ru);
	//TIMEVAL2TIMESPEC (ru.ru_utime, end);
	iec_gettimeofday (&end);

	SUB_TIMESPEC (end, start);
	ADD_TIMESPEC (task->total_time, end);
	ADD_TIMESPEC (GLOB.total_time, end);
}

/**
 * @brief Advance user. Depending of the current context type, calls Task_Next, Pou_Next, Block_Next or Step_Next
 *  If context is not runnable (m_next == FALSE), do nothing
 * @param user The user
 * @return 0 if ok, -1 is user paused
 */
int Program_Next (iec_program_t *program) {

	program->force_schedule = FALSE;

	iec_ctx_t *ctx = Context_Current(program);
#ifdef DEV_LAURENT
	RTL_TRDBG (TRACE_ENGINE_SCHEDULE, "==== Program_Next Pointer program %s (%p) ctx (%p)\n", program->name, program, ctx);
#endif
	if	(!ctx) {
		Program_RedoFromStart(program);
		return -1;
	}

	if	(ctx->m_next == FALSE)
		return -1;

	rtl_tracelevel (program->level);
	TraceLevel = program->level;

	RTL_TRDBG (TRACE_ENGINE_SCHEDULE, "================= Program_Next [%s] Type=%d\n", program->name, ctx->m_type);
	ctx->m_next	= FALSE;
	switch (ctx->m_type) {
	case	CTX_TYPE_POU:
		Pou_Next (program);
		break;
	case	CTX_TYPE_BLOCK:
		Block_Next (program);
		break;
	case	CTX_TYPE_STEP:
		Step_Next (program);
		break;
	default:
		dumpContexts(program);
		break;
	}
	return	0;
}

void checkGlitch(TIME interval) {
	static TIME previous = {0,0};
	struct list_head *elem, *elem2, *elem3;
	TIME now, offset;

	//RTL_TRDBG (TRACE_DETAIL, "checkGlitch %lds %ldns\n", interval.tv_sec, interval.tv_nsec);
	iec_gettimeofday (&now);

	//RTL_TRDBG (TRACE_ERROR, "+++ previous=%ld now=%ld\n", previous.tv_sec, now.tv_sec);
	if	(previous.tv_sec && (now.tv_sec < previous.tv_sec)) {
		ADD_TIMESPEC3 (offset, previous, interval);
		SUB_TIMESPEC (offset, now);

		RTL_TRDBG (TRACE_ERROR, "\033[31;7m+++ detecting reverse time change offset=%ld sec\033[0m\n", -offset.tv_sec);
		list_for_each (elem, &GLOB.user_list) {
			iec_user_t *user	= list_entry(elem, iec_user_t, head);
			list_for_each (elem2, &user->resource_list) {
				iec_resource_t *resource = list_entry(elem2, iec_resource_t, head);
				list_for_each (elem3, &resource->task_list) {
					iec_task_t *task = list_entry(elem3, iec_task_t, head);
					SUB_TIMESPEC (task->next, offset);
					RTL_TRDBG (TRACE_ERROR, "+++ now:%ld futur:%ld\n", now.tv_sec, task->next.tv_sec);
				}
			}
		}
	}
	if	(previous.tv_sec && (now.tv_sec > previous.tv_sec + 600)) {
		SUB_TIMESPEC3 (offset, now, interval);
		SUB_TIMESPEC (offset, previous);

		RTL_TRDBG (TRACE_ERROR, "\033[31;7m+++ detecting time change offset=%ld sec\033[0m\n", offset.tv_sec);
		list_for_each (elem, &GLOB.user_list) {
			iec_user_t *user	= list_entry(elem, iec_user_t, head);
			list_for_each (elem2, &user->resource_list) {
				iec_resource_t *resource = list_entry(elem2, iec_resource_t, head);
				list_for_each (elem3, &resource->task_list) {
					iec_task_t *task = list_entry(elem3, iec_task_t, head);
					if	(NULL_TIMESPEC(task->interval))
						continue;
					ADD_TIMESPEC (task->next, offset);
					RTL_TRDBG (TRACE_ERROR, "+++ now:%ld futur:%ld\n", now.tv_sec, task->next.tv_sec);
				}
			}
		}
	}

	previous = now;
}

void Resource_Next (iec_resource_t *resource) {
	struct list_head *elem;

	list_for_each (elem, &resource->task_list) {
		iec_task_t *task	= list_entry(elem, iec_task_t, head);

		if	(NULL_TIMESPEC(task->interval))
			continue;
		TIME now;
		iec_gettimeofday (&now);
		if	(task->force_schedule) {
			Task_Next (task);
		}
		else if	(CMP_TIMESPEC(now, task->next) >= 0) {
			ADD_TIMESPEC (task->next, task->interval);
			Task_Next (task);
		}
		else {
			struct list_head *elem;
			list_for_each (elem, &task->program_list) {
				iec_program_t *program	= list_entry(elem, iec_program_t, head);
				if	(program->force_schedule)
					Program_Next (program);
			}
		}

		rtl_tracelevel (resource->level);
		TraceLevel = resource->level;
	}
}

void User_Next(iec_user_t *user) {
	rtl_tracelevel (user->level);
	TraceLevel = user->level;

	struct list_head *elem;
	list_for_each (elem, &user->resource_list) {
		iec_resource_t *resource = list_entry(elem, iec_resource_t, head);
		Resource_Next (resource);
	}

	rtl_tracelevel (user->level);
	TraceLevel = user->level;
}

/**
 * @brief Advance each task of each user if its time
 * @return 0
 */
void Users_Next() {
	struct list_head *elem;

	iec_watchdog_reset ();	// still alive !!

	iec_check_cpu ();

	list_for_each (elem, &GLOB.user_list) {
		iec_user_t *user	= list_entry(elem, iec_user_t, head);
		User_Next (user);
	}
}

/**
 * @brief Force rescheduling a program
 * @return void
 */
void Program_Reschedule(iec_program_t *program) {
	RTL_TRDBG (TRACE_ENGINE_SCHEDULE, "================= Program_Reschedule: name=[%s] =================\n",program->name);
	//program->task->force_schedule	= TRUE; No more !!
	program->force_schedule	= TRUE;
}

/**
 * @brief Force rescheduling a task
 * @return void
 */
void Task_Reschedule(iec_task_t *task) {
	RTL_TRDBG (TRACE_ENGINE_SCHEDULE, "---------------- IEC_TASK_Reschedule ---------------\n");
	task->force_schedule	= TRUE;
}

BOOL Task_MustSchedule(iec_task_t *task) {
	if	(task->force_schedule)
		return	TRUE;

	struct list_head *elem;
	list_for_each (elem, &task->program_list) {
		iec_program_t *program	= list_entry(elem, iec_program_t, head);
		if	(program->force_schedule)
			return	TRUE;
	}
	return FALSE;
}

/**
 * @brief Force rescheduling a user
 * @return void
 */
void User_Reschedule(iec_user_t *user) {
	struct list_head *elem, *elem2;
	RTL_TRDBG (TRACE_ENGINE_SCHEDULE, "#################################### M2M_APP_Reschedule ####################################\n");
	list_for_each (elem, &user->resource_list) {
		iec_resource_t *resource = list_entry(elem, iec_resource_t, head);
		list_for_each (elem2, &resource->task_list) {
			iec_task_t *task = list_entry(elem2, iec_task_t, head);
			Task_Reschedule(task);
		}
	}
}

/**
 * @brief Cleans non used tasks
 * @param user[in]
 * @return void
 */
void User_Cleanup(iec_user_t *user) {
	struct list_head *elem, *elem2, *pos;
	list_for_each (elem, &user->resource_list) {
		iec_resource_t *resource = list_entry(elem, iec_resource_t, head);
		list_for_each_safe (elem2, pos, &resource->task_list) {
			iec_task_t *task = list_entry(elem2, iec_task_t, head);
			if	(list_empty (&task->program_list)) {
				RTL_TRDBG (TRACE_ENGINE_SCHEDULE, "%s task empty\n", task->name);
				Task_Free (task);
			}
		}
	}
}

/**
 * @brief Looking at all runable scripts, returns the minimum waiting time before doing something.
 * @param w OUT The minimum waiting time
 * @return void
 */
void iec_getMinWait(TIME *w) {
	// Protection, if nothing to do : 1 sec
	TIME MinWait = {1,0}; // needs description. Why initialized at 1 seconde ???
	TIME now;
	struct list_head *elem, *elem2, *elem3;

	if	(list_empty (&GLOB.user_list)) {
		*w	= MinWait;
		return;
	}

	iec_gettimeofday (&now);
	BOOL first = TRUE;

	list_for_each (elem, &GLOB.user_list) {
		iec_user_t *user	= list_entry(elem, iec_user_t, head);
		list_for_each (elem2, &user->resource_list) {
			iec_resource_t *resource = list_entry(elem2, iec_resource_t, head);
			list_for_each (elem3, &resource->task_list) {
				iec_task_t *task = list_entry(elem3, iec_task_t, head);

				if	(NULL_TIMESPEC(task->interval))
					continue;

				if	(Task_MustSchedule(task)) {
					MinWait	= now;
					break;
				}
				//RTL_TRDBG (TRACE_DETAIL, "+++ next %d %d\n", task->next.tv_sec, task->next.tv_nsec);
				if	(first || CMP_TIMESPEC(task->next, MinWait) < 0) {
					MinWait	= task->next;
					first	= FALSE;
				}
			}
		}
	}
	//RTL_TRDBG (TRACE_DETAIL, "--- now  %d %d\n", now.tv_sec, now.tv_nsec);
	if	(CMP_TIMESPEC(MinWait, now) >= 0) {
		SUB_TIMESPEC (MinWait, now);
	}
	else {
		MinWait.tv_sec	= 0;
		MinWait.tv_nsec	= 0;
	}
	*w	= MinWait;
	RTL_TRDBG (TRACE_ENGINE_TIMING, "iec_getMinWait %lds %ldms\n", MinWait.tv_sec, (MinWait.tv_nsec+(NSEC_PER_MSEC/2))/NSEC_PER_MSEC);
}

void loadGlobalVars (iec_user_t *user, mxml_node_t *vars, void *variables) {
	mxml_node_t *variable;
	variable = vars;
	int flg = MXML_DESCEND_FIRST;
	while	((variable = mxmlFindElement(variable, vars, "variable", NULL, NULL, flg))) {
		flg = MXML_NO_DESCEND;
		char *name = (char *)mxmlElementGetAttr(variable, "name");
		char *configuration = (char *)mxmlElementGetAttr(variable, "configuration");

		if	(configuration && strcmp(configuration, user->name))
			continue;

		mxml_node_t *node = mxmlFindElement(variable, variable, "type", NULL, NULL, MXML_DESCEND);
		char *type = _mxmlGetFirstElement(node);
		int itype = typeInteger(type);

		iec_variable_t *var = rtl_htblGet(variables, name);

		if	(var && (var->type == itype)) {
			RTL_TRDBG (TRACE_SET, "  globalVar %s=%s\n", name, getInitialValue(variable));
			setVariable (var, getInitialValue(variable));
		}
	}
}


//======================================================
//	Script loading phase : scan all configuration
//======================================================

void initVariables (iec_user_t *user, mxml_node_t *vars, void *variables, BOOL retain) {
	mxml_node_t *variable;
	variable = vars;
	int flg = MXML_DESCEND_FIRST;
	while	((variable = mxmlFindElement(variable, vars, "variable", NULL, NULL, flg))) {
		char *name = (char *)mxmlElementGetAttr(variable, "name");
		mxml_node_t *node = mxmlFindElement(variable, variable, "type", NULL, NULL, MXML_DESCEND);
		char *type = _mxmlGetFirstElement(node);

		if	(rtl_htblGet(variables, name) == NULL) {
			iec_variable_t *var = createVariable (name, typeInteger(type), getInitialValue(variable));
			RTL_TRDBG (TRACE_SET, "createVariable %s (%p)%s\n", name, var, retain ? "..retain":"");
			if	(retain)
				var->flags |= VARFLAG_RETAIN;
			addVariableInHash (variables, var);

			char *address = (char *)mxmlElementGetAttr(variable, "address");
			if	(address) {
				RTL_TRDBG (TRACE_SET, "linkVariable %s => %s\n", name, address);
				if (linkVariable(variables, name, user->m_accessVars, address) < 0)
					RTL_TRDBG (TRACE_ERROR, "linkVariable FAILED %s => %s\n", name, address);
			}
		}
		flg = MXML_NO_DESCEND;
	}
}

void scan_globalVars(iec_user_t *user, void *variables, mxml_node_t *top) {
	void *node = top;
	int flg = MXML_DESCEND_FIRST;
	while	((node = mxmlFindElement(node, top, "globalVars", NULL, NULL, flg))) {
		char *attr = (char *)mxmlElementGetAttr(node, "retain");
		BOOL retain = FALSE;
		if	(attr && !strcasecmp(attr, "TRUE"))
			retain	= TRUE;
		initVariables (user, node, variables, retain);
		flg = MXML_NO_DESCEND;
	}
}

void scan_pouInstances(iec_task_t *task, mxml_node_t *top) {
	mxml_node_t *node = top;
	int flg = MXML_DESCEND_FIRST;
	while	((node = mxmlFindElement(node, top, "pouInstance", NULL, NULL, flg))) {
		iec_program_t *program = Program_New(task, node);
		if	(program)
			list_add_tail (&program->head, &task->program_list);
		flg = MXML_NO_DESCEND;
	}
}

void scan_tasks(iec_resource_t *resource, mxml_node_t *top) {
	mxml_node_t *node = top;
	int flg = MXML_DESCEND_FIRST;
	while	((node = mxmlFindElement(node, top, "task", NULL, NULL, flg))) {
		iec_task_t *task	= Task_New (resource, node);
		list_add_tail (&task->head, &resource->task_list);
		scan_pouInstances(task, node);
		flg = MXML_NO_DESCEND;
		resource->user->countTasks ++;
		char *res = User_GetDictionnary(resource->user, "iecMaxTasks");
		int maxTasks = res ? atoi(res) : INT_MAX;
		if	(resource->user->countTasks > maxTasks) {
			User_GSC_log(resource->user, "MAXTASK_EXCEEDED", "Max number of tasks reached.", "", "");
		}
	}
}

void scan_resources(iec_user_t *user, mxml_node_t *top) {
	mxml_node_t *node = top;
	int flg = MXML_DESCEND_FIRST;
	while	((node = mxmlFindElement(node, top, "resource", NULL, NULL, flg))) {
		iec_resource_t *resource	= Resource_New (user, node);
		scan_globalVars(user, resource->m_variables, node);
		scan_tasks(resource, node);
		RTL_TRDBG (TRACE_DETAIL, "load globals for resource %s\n", resource->name);
		loadRetain (user, resource->m_variables);
		flg = MXML_NO_DESCEND;
	}
}
/*
void scan_configurations(iec_user_t *user, mxml_node_t *top) {
	mxml_node_t *node	= top;
	//	<configuration> is not directly under <project>. We use MXML_DESCEND in place of MXML_DESCEND_FIRST.
	int flg = MXML_DESCEND;
	while	((node = mxmlFindElement(node, top, "configuration", NULL, NULL, flg))) {
		scan_globalVars(user->m_variables, node);
		scan_resources(user, node);
		flg = MXML_NO_DESCEND;
	}
}
*/
int scan_configuration(iec_user_t *user, mxml_node_t *top) {
	mxml_node_t *node	= top;
	user->countTasks = 0;
	//	<configuration> is not directly under <project>. We use MXML_DESCEND in place of MXML_DESCEND_FIRST.
	int flg = MXML_DESCEND;
	node = mxmlFindElement(node, top, "configuration", NULL, NULL, flg);
	if	(!node)
		return -1;

	scan_globalVars(user, user->m_variables, node);
	scan_resources(user, node);
	flg = MXML_NO_DESCEND;
	return 0;
}

iec_resource_t *Resource_New(iec_user_t *user, void *node) {
	iec_resource_t *resource = Calloc(sizeof(iec_resource_t), 1);

	list_add_tail (&resource->head, &user->resource_list);
	INIT_LIST_HEAD (&resource->task_list);
	resource->name = strdup(mxmlElementGetAttr(node, "name"));
	resource->m_variables = rtl_htblCreate(32, NULL);
	resource->user = user;
	resource->level = user->level;

	return	resource;
}

void Resource_Free(iec_resource_t *resource) {
	struct list_head *elem, *pos;
	RTL_TRDBG (TRACE_DETAIL, "Resource_Free\n");
	list_del (&resource->head);
	list_for_each_safe (elem, pos, &resource->task_list) {
		iec_task_t *task = list_entry(elem, iec_task_t, head);
		Task_Free (task);
	}
	free (resource);
}

iec_user_t *User_GetByUri(char *uri) {
	struct list_head *elem;
	list_for_each (elem, &GLOB.user_list) {
		iec_user_t *user	= list_entry(elem, iec_user_t, head);
		if	(!user)
			return NULL;
		if	(!strcmp(uri, user->name))
			return	user;
	}
	return	NULL;
}

iec_user_t *User_GetByIndex(int idx) {
	struct list_head *elem;
	list_for_each (elem, &GLOB.user_list) {
		iec_user_t *user	= list_entry(elem, iec_user_t, head);
		if	(!user)
			return NULL;
		if	(idx-- == 0)
			return	user;
	}
	return	NULL;
}

iec_user_t *User_Alloc(char *appId) {
	iec_user_t *user = Calloc(sizeof(iec_user_t), 1);
	list_add_tail (&user->head, &GLOB.user_list);
	user->name	= strdup(appId);
	user->m_variables = rtl_htblCreate(32, NULL);
	user->m_accessVars = rtl_htblCreate(32, NULL);
	INIT_LIST_HEAD (&user->resource_list);
	return	user;
}

/**
 * @brief Create a new user. 'user' is synonym of 'script'
 * @param file The file containing the xml
 * @return A iec_user_t structure
 */
iec_user_t *User_New(char *file, int lev) {
	void *script = NULL;
	FILE *fp = fopen(file, "r");
	if	(!fp) {
		RTL_TRDBG (TRACE_ERROR, "File not found %s\n", file);
		return	NULL;
	}
#ifdef WITH_EXI
	if	(strstr(file, ".exi")) {
		struct stat st;
		if (stat(file, &st) >= 0) {
RTL_TRDBG (TRACE_DETAIL, "EXI load %d\n", st.st_size);
			char *buf	= Calloc(st.st_size, 1);
			int n = fread (buf, st.st_size, 1, fp);
			if	(n == 1)
				script = exi2xml (buf, st.st_size);
			free (buf);
		}
	} else {
#else
	{
#endif
		script = mxmlSAXLoadFile (NULL , fp , MXML_OPAQUE_CALLBACK, iec_sax_cb, NULL);
		mxmlReduceAttrs (script);
	}
	fclose (fp);

	if	(!script) {
		Board_Put (&GLOB.board_errors, "FATAL : %s not loaded (bad xml)", file);
		RTL_TRDBG (TRACE_DETAIL, "FATAL : %s not loaded (bad xml)", file);
		return	NULL;
	}

	if	(PLCvalidation(script) < 0) {
		Board_Put (&GLOB.board_errors, "FATAL %s %s", file, PLCvalidation_LastError());
		RTL_TRDBG (TRACE_DETAIL, "FATAL : %s validation FAILED\n", file);
		mxmlDelete (script);
		return NULL;
	}

	iec_user_t *user = User_Alloc(file);

	user->project = script;

	User_SetLevel (user, lev);
	if	(scan_configuration(user, script) < 0) {
		User_Free (user);
		return	NULL;
	}
	RTL_TRDBG (TRACE_DETAIL, "load globals for configuration %s\n", user->name);
	loadRetain (user, user->m_variables);

	User_Cleanup(user);

	//	Speed up startup
	User_Reschedule(user);
	return	user;
}

/**
 * @brief Create a new user from a string. 'user' is synonym of 'script'
 * A new LoadUri creates a new user, starts it and frees the old one.
 * @param uri The uri identifying the script
 * @param buf The buffer containing the document (XML or EXI)
 * @param lev Trace level
 * @return A iec_user_t structure
 */
iec_user_t *User_LoadUri(char *appId, iec_buff_t buf, int lev) {
	void *script;
	iec_user_t *old = User_GetByUri(appId);

	RTL_TRDBG (TRACE_DETAIL, "User_LoadUri %s len %d\n", buf.contentType, buf.len);

#ifdef WITH_EXI
	if	(!strcmp(buf.contentType, "application/exi")) {
		script = exi2xml (buf.data, buf.len);
	} else {
#else
	{
#endif
		script = mxmlSAXLoadString (NULL , buf.data , MXML_OPAQUE_CALLBACK, iec_sax_cb, NULL);
		mxmlReduceAttrs (script);
	}

	if	(!script) {
		Board_Put (&GLOB.board_errors, "FATAL : %s not loaded (bad xml)", appId);
		GSC_log("CONFIGURATION_BAD_XML", "Configuration Bad XML", "", "appId %s", appId);
		return	NULL;
	}

	if	(PLCvalidation(script) < 0) {
		Board_Put (&GLOB.board_errors, "FATAL %s %s", appId, PLCvalidation_LastError());
		GSC_log("CONFIGURATION_BAD_XML", "Validation FAILED", "", "appId %s %s", appId, PLCvalidation_LastError());
		mxmlDelete (script);
		return NULL;
	}

	iec_user_t *user = User_Alloc(appId);

	if	(old) {
		// IMPORTANT : All GSCL parameters are copied
		user->m_dictionnary = old->m_dictionnary;
		old->m_dictionnary = NULL;
		User_Free (old);
	}

	user->project = script;
	User_SetLevel (user, lev);
	if	(scan_configuration(user, script) < 0) {
		User_Free (user);
		return	NULL;
	}
	RTL_TRDBG (TRACE_DETAIL, "load globals for configuration %s\n", user->name);
	loadRetain (user, user->m_variables);
	//	Speed up startup
	User_Reschedule(user);
	//User_Pause(user, TRUE);

	return	user;
}

/**
 * @brief Reset configuration execution to the beginning.
 */
int User_Reset(char *appId) {
	RTL_TRDBG (TRACE_DETAIL, "User_Reset %s\n", appId);
	iec_user_t *user = User_GetByUri(appId);
	if	(!user) {
		return -1;
	}
	struct list_head *elem, *pos;
	list_for_each_safe (elem, pos, &user->resource_list) {
		iec_resource_t *resource = list_entry(elem, iec_resource_t, head);
		Resource_Free (resource);
	}

	inline void fct(char *key, void *var) { freeVariable(var); }
	rtl_htblDump(user->m_variables, fct);

	scan_configuration(user, user->project);
	loadRetain (user, user->m_variables);
	//	Speed up startup
	User_Reschedule(user);
	return 0;
}

/**
 * @brief Unload a user script. 
 * @param user The user
 * @return void
 */
void User_Free(iec_user_t *user) {
	struct list_head *elem, *pos;
	RTL_TRDBG (TRACE_DETAIL, "User_Free %s\n", user->name);
	list_del (&user->head);

	list_for_each_safe (elem, pos, &user->resource_list) {
		iec_resource_t *resource = list_entry(elem, iec_resource_t, head);
		Resource_Free (resource);
	}

//	Script_Free(user->name);
	if	(user->project)
		mxmlDelete (user->project);

	inline void fct(char *key, void *var) { freeVariable(var); }
	rtl_htblDump(user->m_variables, fct);
	rtl_htblDestroy(user->m_variables);
	rtl_htblDump(user->m_accessVars, fct);
	rtl_htblDestroy(user->m_accessVars);

	free (user->name);

	if	(user->m_dictionnary)
		rtl_htblDestroy (user->m_dictionnary);
	if	(user->content_configuration.data)
		free (user->content_configuration.data);
	if	(user->content_mapping.data)
		free (user->content_mapping.data);

	free (user);
}

void Program_SetLevel(iec_program_t *program, int lev) {
	program->level = lev;
}

void Task_SetLevel(iec_task_t *task, int lev) {
	struct list_head *elem, *pos;
	task->level = lev;
	list_for_each_safe (elem, pos, &task->program_list) {
		iec_program_t *program = list_entry(elem, iec_program_t, head);
		Program_SetLevel (program, lev);
	}
}

void Resource_SetLevel(iec_resource_t *resource, int lev) {
	struct list_head *elem, *pos;
	resource->level = lev;
	list_for_each_safe (elem, pos, &resource->task_list) {
		iec_task_t *task = list_entry(elem, iec_task_t, head);
		Task_SetLevel (task, lev);
	}
}

void User_SetLevel(iec_user_t *user, int lev) {
	struct list_head *elem, *pos;
	TraceLevel = lev;
	rtl_tracelevel (TraceLevel);
	user->level = lev;
	list_for_each_safe (elem, pos, &user->resource_list) {
		iec_resource_t *resource = list_entry(elem, iec_resource_t, head);
		Resource_SetLevel (resource, lev);
	}
}

/**
 * @brief Pause a script.
 * @param user The user
 * @return void
 */
void Program_Pause(iec_program_t *program, BOOL flagPause) {
	iec_ctx_t *ctx = Context_Current(program);
	ctx->m_next	= !flagPause;
}

void Task_Pause(iec_task_t *task, BOOL flagPause) {
	struct list_head *elem;
	list_for_each (elem, &task->program_list) {
		iec_program_t *program = list_entry(elem, iec_program_t, head);
		Program_Pause (program, flagPause);
	}
}

void Resource_Pause(iec_resource_t *resource, BOOL flagPause) {
	struct list_head *elem;
	list_for_each (elem, &resource->task_list) {
		iec_task_t *task = list_entry(elem, iec_task_t, head);
		Task_Pause (task, flagPause);
	}
}

void User_Pause(iec_user_t *user, BOOL flagPause) {
	struct list_head *elem;
	user->flagPause = flagPause;
	list_for_each (elem, &user->resource_list) {
		iec_resource_t *resource = list_entry(elem, iec_resource_t, head);
		Resource_Pause (resource, flagPause);
	}
}

void User_Resume(iec_user_t *user) {
	User_Pause(user, FALSE);
}

void User_Start(iec_user_t *user) {
	User_Resume(user);
	User_Reschedule(user);
}

/**
 * @brief Dictionnary of parameters taken from
 *  '/m2m/applications/<APP>/containers/DESCRIPTOR/contentInstances/latest/content'
 */
char *User_GetDictionnary(iec_user_t *user, char *param) {
	if	(!user || !user->m_dictionnary)
		return	NULL;
	return rtl_htblGet (user->m_dictionnary, param);
}

/**
 * @brief Save all Global variables with the retain flag
 * @return void
 */
void saveRetain() {
	iec_user_t *user;
	iec_resource_t *resource;

	RTL_TRDBG (TRACE_DETAIL, "[GLOB] save retain\n");
	FILE *fd = fopen("retain.xml", "w");
	fprintf (fd, "<globalVars retain=\"true\">\n");

	inline void fct(char *key, void *p) {
		iec_variable_t *var = (iec_variable_t *)p;
		if	(var->flags & VARFLAG_RETAIN) {
			fprintf (fd, " <variable name=\"%s\" configuration=\"%s\"", key, user->name);
			if	(resource)
				fprintf (fd, " resource=\"%s\"", resource->name);
			fprintf (fd, ">\n");
			fprintf (fd, "  <type><%s/></type>\n", typeString[(int)var->type]);
			char *tmp = NULL;
			fprintf (fd, "  <initialValue><simpleValue value=\"%s\"/></initialValue>\n", printVarValue(var, &tmp));
			fprintf (fd, " </variable>\n");
			free (tmp);
		}
	}

	struct list_head *elem, *elem2;

	list_for_each (elem, &GLOB.user_list) {
		user	= list_entry(elem, iec_user_t, head);
		resource = NULL;
		rtl_htblDump(user->m_variables, fct);

		list_for_each (elem2, &user->resource_list) {
			resource	= list_entry(elem2, iec_resource_t, head);
			rtl_htblDump(resource->m_variables, fct);
		}
	}

	fprintf (fd, "</globalVars>\n");
	fclose (fd);
}

/**
 * @brief Load all retain variables
 * @return void
 */
void loadRetain(iec_user_t *user, void *variables) {
	FILE *fp = fopen("retain.xml", "r");
	if	(!fp)
		return;
	mxml_node_t *tot = mxmlLoadFile(NULL, fp, MXML_OPAQUE_CALLBACK);
	fclose (fp);

	loadGlobalVars (user, tot, variables);
	mxmlRelease (tot);
}
