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
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <malloc.h>

#include <mxml.h>

#include <rtlbase.h>
#include <rtllist.h>
#include <rtlhtbl.h>

#include "plc.h"
#include "admin_cli.h"
#include "_version.h"
#include "board.h"
#include "tools.h"

#include "xpath.h"

extern iec_global_t GLOB;
int gMode = 0;
extern int TraceLevel;

char *ctxString[] = { "CONFIGURATION", "RESOURCE", "TASK", "POU", "BLOCK", "STEP" };

void printJson(t_cli *cl, iec_variable_t *var) {
	char tmp[1024];
	int sz;
	tmp[0] = 0;
	if	(!var)
		return;
	switch (var->type) {
	case	BOOLtype : strcpy (tmp, var->value.BOOLvar ? "TRUE":"FALSE"); break;
	case	INTtype : sprintf (tmp, "%d", var->value.INTvar); break;
	case	DINTtype : sprintf (tmp, "%d", var->value.DINTvar); break;
	case	LINTtype : sprintf (tmp, "%lld", var->value.LINTvar); break;
	case	UINTtype : sprintf (tmp, "%d", var->value.UINTvar); break;
	case	UDINTtype : sprintf (tmp, "%d", var->value.UDINTvar); break;
	case	ULINTtype : sprintf (tmp, "%lld", var->value.ULINTvar); break;
	case	REALtype : sprintf (tmp, "%f", var->value.REALvar); break;
	case	LREALtype : sprintf (tmp, "%f", var->value.LREALvar); break;
	case	STRINGtype :
		sz = var->value.STRINGvar.len;
		strncpy (tmp, (char *)var->value.STRINGvar.body, sz);
		tmp[sz] = 0;
		break;
	case	TIMEtype :
	case	DATEtype :
	case	DTtype :
	case	TODtype :
		sprintf (tmp, "%ld.%ld", var->value.TIMEvar.tv_sec, var->value.TIMEvar.tv_nsec);
		break;
	}
	extern char *typeString[];
	AdmPrint (cl, "{ name='%s', type='%s', value='%s' },\n", var->name, typeString[(int)var->type], tmp);
}


static iec_resource_t *_getResource(int c, int r) {
	struct list_head *elem;

	iec_user_t *user = User_GetByIndex(c);
	if	(!user)	return NULL;

	list_for_each (elem, &user->resource_list) {
		if	(!elem)
			break;
		iec_resource_t *resource	= list_entry(elem, iec_resource_t, head);
		if	(r-- == 0) {
			return resource;
		}
	}
	return NULL;
}

static iec_task_t *_getTask(int c, int r, int t) {
	struct list_head *elem;
	iec_resource_t *resource = _getResource(c, r);
	if	(!resource) return NULL;

	list_for_each (elem, &resource->task_list) {
		if	(!elem)
			break;
		iec_task_t *task	= list_entry(elem, iec_task_t, head);
		if	(t-- == 0) {
			return task;
		}
	}
	return	NULL;
}

static iec_program_t *_getProgram(int c, int r, int t, int p) {
	struct list_head *elem;
	iec_task_t *task = _getTask(c, r, t);
	if	(!task)	return	NULL;

	list_for_each (elem, &task->program_list) {
		if	(!elem)
			break;
		iec_program_t *program	= list_entry(elem, iec_program_t, head);
		if	(p-- == 0) {
			return program;
		}
	}
	return	NULL;
}


static void dumpVariables(t_cli *cl, void *m_variables) {
	inline void fct(char *key, void *var) {
		if	(gMode == 0) {
			char *tmp = NULL;
			printVariableInBuffer_WithName (var, &tmp, key, TRUE);
			AdmPrint (cl, "\t%s\n", tmp);
			free (tmp);
		}
		else if	(gMode == 1)
			printJson (cl, var);
	}
	rtl_htblDump(m_variables, fct);
}

void Context_Display(t_cli *cl, iec_program_t *program) {
	struct list_head *elem;
	list_for_each (elem, &program->ctx_list) {
		if	(!elem)
			break;
		iec_ctx_t *ctx	= list_entry(elem, iec_ctx_t, head);

		if	(gMode == 0)
			AdmPrint (cl, "   stack %s [%s]\n", ctx->name, ctxString[ctx->m_type]);
		else if	(gMode == 1)
			AdmPrint (cl, "{ name=%s, type=%s, state=%d, variables=[\n", ctx->name, ctxString[ctx->m_type], ctx->m_state);

		if	(ctx->m_type != CTX_TYPE_STEP)
			dumpVariables (cl, ctx->m_variables);

		if	(gMode == 1)
			AdmPrint (cl, "]\n},\n");
	}
	if	(gMode == 1)
		AdmPrint (cl, "]\n}\n");
}

void Program_Display(t_cli *cl, iec_task_t *task) {
	struct list_head *elem;
	int	i = 0;
	list_for_each (elem, &task->program_list) {
		if	(!elem)
			break;
		iec_program_t *program	= list_entry(elem, iec_program_t, head);
		AdmPrint (cl, "  program %d :%s [%s] lev:%d\n", i, program->name, program->typeName, program->level);
		i++;
		Context_Display(cl, program);
	}
}

void Task_Display(t_cli *cl, iec_resource_t *resource) {
	struct list_head *elem;
	int	i = 0;
	list_for_each (elem, &resource->task_list) {
		if	(!elem)
			break;
		iec_task_t *task	= list_entry(elem, iec_task_t, head);
		long a, b, pcent;
		a = task->total_time.tv_sec*1000000 + task->total_time.tv_nsec/1000;
		b = GLOB.total_time.tv_sec*1000000 + GLOB.total_time.tv_nsec/1000;
		pcent = (a*100)/b;
		AdmPrint (cl, " [32;7m[task %d :%s %ds:%dms lev:%d ticks:%ld total_time=%ds:%dus CPU=%ld%%[0m\n",
			i, task->name,
			task->interval.tv_sec, task->interval.tv_nsec/1000000,
			task->level, task->ticks,
			task->total_time.tv_sec, task->total_time.tv_nsec/1000, pcent);
		i++;
		Program_Display(cl, task);
	}
}

void Resource_Display(t_cli *cl, iec_user_t *user) {
	struct list_head *elem;
	int	i = 0;
	list_for_each (elem, &user->resource_list) {
		if	(!elem)
			break;
		iec_resource_t *resource	= list_entry(elem, iec_resource_t, head);
		AdmPrint (cl, " resource %d :%s level=%d\n", i, resource->name, resource->level);
		dumpVariables (cl, resource->m_variables);
		i++;
		Task_Display (cl, resource);
	}
}

void dumpMapping(t_cli *cl, iec_user_t *user) {
	struct list_head *elem;
	list_for_each (elem, &user->xpath_list) {
		if	(!elem)
			break;
		iec_xpath_t *elt = list_entry(elem, iec_xpath_t, head);
		AdmPrint (cl, "\t%s %s\n", elt->iec_input_var, elt->targetID);
	}
}

int ls_fct(t_cli *cl, char *args[], int nbargs) {
	struct list_head *elem;
	int	i = 0;
/*
	AdmPrint (cl, "Access Variables :\n");
	dumpVariables (cl, GLOB.m_variables);
*/
	list_for_each (elem, &GLOB.user_list) {
		if	(!elem)
			break;
		iec_user_t *user	= list_entry(elem, iec_user_t, head);
		AdmPrint (cl, "script %d :%s level=%d\n", i, user->name, user->level);

		AdmPrint (cl, "Access Variables :\n");
		dumpVariables (cl, user->m_accessVars);

		AdmPrint (cl, "Mapping :\n");
		dumpMapping (cl, user);

		dumpVariables (cl, user->m_variables);
		i++;
		Resource_Display (cl, user);
		AdmPrint (cl, "------------------------------------------------------\n");
	}
	return	0;
}

int err_fct(t_cli *cl, char *args[], int nbargs) {
	inline void fct(char *msg) {
		AdmPrint (cl, "%s\n", msg);
	}
	AdmPrint (cl, "### List of last errors ###\n");
	Board_Dump (&GLOB.board_errors, fct);
	AdmPrint (cl, "### End  of last errors ###\n");
	return 0;
}

int run_fct(t_cli *cl, char *args[], int nbargs) {
	iec_user_t *user = User_New(args[0], atoi(args[1]));
	if	(!user)
		return	-1;
	return	0;
}

int pause_fct(t_cli *cl, char *args[], int nbargs) {
	int	n = nbargs ? atoi(args[0]) : 0;
	iec_user_t *user = User_GetByIndex(n);
	if	(user)
		User_Pause (user, TRUE);
	return	0;
}

int resume_fct(t_cli *cl, char *args[], int nbargs) {
	int	n = nbargs ? atoi(args[0]) : 0;
	iec_user_t *user = User_GetByIndex(n);
	if	(user)
		User_Pause (user, FALSE);
	return	0;
}

int unload_fct(t_cli *cl, char *args[], int nbargs) {
	int	n = nbargs ? atoi(args[0]) : 0;
	iec_user_t *user = User_GetByIndex(n);
	if	(user)
		User_Free(user);
	return	0;
}

int reset_fct(t_cli *cl, char *args[], int nbargs) {
	int	n = nbargs ? atoi(args[0]) : 0;
	iec_user_t *user = User_GetByIndex(n);
	if	(user)
		User_Reset(user->name);
	return	0;
}

int trace_fct(t_cli *cl, char *args[], int nbargs) {
	if	(nbargs < 2) return -1;

	if	(!strcmp(args[0], "m2m")) {
		GLOB.level	= atoi(args[1]);
		AdmPrint (cl, "m2m log level set to %d\n", GLOB.level);
		return 0;
	}

	char *parts[5];
	int x = my_strcut (args[0], ".", parts, 5);
	iec_user_t *user;
	iec_resource_t *resource;
	iec_task_t *task;
	iec_program_t *program;

	switch (x) {
	case 1:
		user = User_GetByIndex(atoi(parts[0]));
		TraceLevel = atoi(args[1]);
		rtl_tracelevel (TraceLevel);
		if	(user)
			User_SetLevel (user, TraceLevel);
		break;
	case 2:
		resource = _getResource(atoi(parts[0]), atoi(parts[1]));
		if	(resource)
			Resource_SetLevel (resource, atoi(args[1]));
		break;
	case 3:
		task = _getTask(atoi(parts[0]), atoi(parts[1]), atoi(parts[2]));
		if	(task)
			Task_SetLevel (task, atoi(args[1]));
		break;
	case 4:
		program = _getProgram(atoi(parts[0]), atoi(parts[1]), atoi(parts[2]), atoi(parts[3]));
		if	(program)
			Program_SetLevel (program, atoi(args[1]));
		break;
	}
	return	0;
}

extern void *g_initialHeap;
extern char etext, edata, end; /* The symbols must have some type, or "gcc -Wall" complains */

int info_fct(t_cli *cl, char *args[], int nbargs) {
	time_t t;
	AdmPrint (cl, "Version   : %s\n", iec61131_whatStr);
	AdmPrint (cl, "Pid       : %d	Vsize : %ld	Cpu : %f\n", getpid(), rtl_vsize(getpid()),
		(double)(rtl_cpuloadavg()/100.0));
	time (&t);
	AdmPrint (cl, "LocalTime : %s", asctime(localtime(&t)));
	AdmPrint (cl, "GmtTime   : %s", asctime(gmtime(&t)));
	AdmPrint (cl, "Heap addr :%p size:%d\n", sbrk(0), sbrk(0)-g_initialHeap);
	AdmPrint (cl, "     etext:%p [%d] edata:%p end:%p\n", &etext, &etext - 0x8048000, &edata, &end);

	struct mallinfo info = mallinfo();

	AdmPrint (cl, "mallinfo  :\n total allocated space:  %d bytes\n", info.uordblks);
	return	0;
}


int ctx_fct(t_cli *cl, char *args[], int nbargs) {
/*
	int	n = nbargs ? atoi(args[0]) : 0;
	iec_user_t *user = User_GetByIndex(n);
	if	(!user)
		return	-1;

	if	(gMode == 1)
		AdmPrint (cl, "{ name=\"%s\", num=%d, contexts=[\n", user->name, n);

	dumpVariables (cl, GLOB.m_variables);
	struct list_head *elem;
	list_for_each (elem, &user->task->ctx_list) {
		iec_ctx_t *ctx	= list_entry(elem, iec_ctx_t, head);

		if	(gMode == 0)
			AdmPrint (cl, "name=%s type=%s state=%d\n", ctx->name, ctxString[ctx->m_type], ctx->m_state);
		else if	(gMode == 1)
			AdmPrint (cl, "{ name=%s, type=%s, state=%d, variables=[\n", ctx->name, ctxString[ctx->m_type], ctx->m_state);

		dumpVariables (cl, ctx->m_variables);

		if	(gMode == 1)
			AdmPrint (cl, "]\n},\n");
	}
	if	(gMode == 1)
		AdmPrint (cl, "]\n}\n");
*/
	return	0;
}

int mode_fct(t_cli *cl, char *args[], int nbargs) {
	if	(!strcmp(args[0], "text"))
		gMode	= 0;
	else if	(!strcmp(args[0], "json"))
		gMode	= 1;
	else if	(!strcmp(args[0], "xml"))
		gMode	= 2;
	return	0;
}

extern param_ret_t *do_ST_interpreter();

int inter_fct(t_cli *cl, char *args[], int nbargs) {
	char *parts[5];
	int n = my_strcut (args[0], ".", parts, 5);

	if (n != 4)
		return -1;
	iec_program_t *program = _getProgram(atoi(parts[0]), atoi(parts[1]), atoi(parts[2]), atoi(parts[3]));
	if	(program) {
		param_ret_t *pret = do_ST_interpreter (program, args[1]);
		free (pret);
	}
	return	0;
}

t_cli *watch_cl = NULL;

static char watch_vars[10][64];
int watch_fct(t_cli *cl, char *args[], int nbargs) {
	int i;
	for	(i=0; i<nbargs; i++) {
		strcpy (watch_vars[i], args[i]);
	}
	watch_cl = cl;
	AdmPrint (cl, "  watch activated\n");
	return	0;
}

int unwatch_fct(t_cli *cl, char *args[], int nbargs) {
	watch_cl = NULL;
	AdmPrint (cl, "  watch deactivated\n");
	return	0;
}

int m2mget_fct(t_cli *cl, char *args[], int nbargs) {
	retrieve_IEC_APPLICATION (args[0]);
	return	0;
}

extern iec_xpath_t *XpathGetWithInputVar (iec_user_t *user, char *input_var);

int xpath_fct(t_cli *cl, char *args[], int nbargs) {
	iec_user_t *user;
	struct list_head *elem;

	inline void fct(char *key, void *content) {
		// Only % variables. Others are inter-script globals
		if	(*key != '%')
			return;
		iec_xpath_t *elt = XpathGetWithInputVar (user, key);
		if	(!elt)
			return;

		if	(elt->iec_input_var)
			AdmPrint (cl, "PLC var : %s\n", elt->iec_input_var);
		AdmPrint (cl, "  targetID : %s\n", elt->targetID);
		if	(elt->xpath_expr)
			AdmPrint (cl, "  xpath_expr : %s\n", elt->xpath_expr);
		if	(elt->value)
			AdmPrint (cl, "  value : %s\n", elt->value);
		AdmPrint (cl, "  subscription_ref : %s\n", elt->subscription_ref ?
			elt->subscription_ref : "(No Reference Has Been Received)");
		AdmPrint (cl, "  tid : %d\n", elt->tid);

		char isoTimeBuf[100];
		extern void Unixtime_to_iso8601(time_t t, char *buf);
		if	(elt->nextSubscription != 0)
			Unixtime_to_iso8601 (elt->nextSubscription, isoTimeBuf);
		else
			strcpy (isoTimeBuf, "ASAP");
		AdmPrint (cl, "  nextSubscription : %s\n", isoTimeBuf);
		if	(elt->msg)
			AdmPrint (cl, "  msg : %s\n", elt->msg);
	}

	list_for_each (elem, &GLOB.user_list) {
		if	(!elem)
			break;
		user	= list_entry(elem, iec_user_t, head);
		rtl_htblDump(user->m_accessVars, fct);
	}

	return	0;
}

//	Called from m2mAccess.c
void CLI_valueHasChanged(iec_variable_t *var) {
	if	(!watch_cl)
		return;
	char *tmp = NULL;
	printVariableInBuffer_WithName (var, &tmp, var->name, TRUE);
	AdmPrint (watch_cl, "\t%s\n", tmp);
	free (tmp);
}

int log_fct(t_cli *cl, char *args[], int nbargs) {
	int	n = nbargs ? atoi(args[0]) : 0;
	iec_user_t *user = User_GetByIndex(n);
	if	(user) {
		User_GSC_log(user, "LOG_FROM_CLI", "This log is generated by the Command Line Interface", "", "");
		AdmPrint (cl, "Log generated in '%s' container\n", user->name);
	}
	else {
		GSC_log("LOG_FROM_CLI", "This log is generated by the Command Line Interface", "", "");
		AdmPrint (cl, "Log generated in VM container\n");
	}
	return	0;
}

static cli_cmd_t ls = { {NULL,NULL}, "ls", "Lists scripts", 0, ls_fct };
static cli_cmd_t err = { {NULL,NULL}, "err", "Fatal errors display", 0, err_fct };
static cli_cmd_t run = { {NULL,NULL}, "run", "run <xmlfile> <level> : run a script", 2, run_fct };
static cli_cmd_t _pause = { {NULL,NULL}, "pause", "pause <script#>", 1, pause_fct };
static cli_cmd_t resume = { {NULL,NULL}, "resume", "resume <script#>", 1, resume_fct };
static cli_cmd_t unload = { {NULL,NULL}, "unload", "unload <script#>", 1, unload_fct };
static cli_cmd_t reset = { {NULL,NULL}, "reset", "reset <script#>", 1, reset_fct };
static cli_cmd_t trace = { {NULL,NULL}, "trace", "trace <script>.<resource>.<task>.<instance> <level>\n  OR trace m2m <level>",
	2, trace_fct };
static cli_cmd_t info = { {NULL,NULL}, "info", "info", 0, info_fct };
//static cli_cmd_t ctx = { {NULL,NULL}, "ctx", "ctx <script#>. Displays the script's execution contexts", 1, ctx_fct };
//static cli_cmd_t mode = { {NULL,NULL}, "mode", "mode text|json", 1, mode_fct };
static cli_cmd_t inter_cmd = { {NULL,NULL}, "inter", "inter <s.r.t.p> <code>", 2, inter_fct };
static cli_cmd_t watch_cmd = { {NULL,NULL}, "watch", "watch", 0, watch_fct };
static cli_cmd_t unwatch_cmd = { {NULL,NULL}, "unwatch", "unwatch", 0, unwatch_fct };
//static cli_cmd_t m2mget_cmd = { {NULL,NULL}, "m2mget", "m2mget <appId>", 1, m2mget_fct };
static cli_cmd_t xpath_cmd = { {NULL,NULL}, "sub", "Display all subscriptions", 0, xpath_fct };
static cli_cmd_t log_cmd = { {NULL,NULL}, "log", "log <script#>", 1, log_fct };

void iec_init_cli () {
	CLI_register (&ls);
	CLI_register (&err);
	CLI_register (&run);
	CLI_register (&_pause);
	CLI_register (&resume);
	CLI_register (&unload);
	CLI_register (&reset);
	CLI_register (&trace);
	CLI_register (&info);
	//CLI_register (&ctx);
	//CLI_register (&mode);
	CLI_register (&inter_cmd);
	CLI_register (&watch_cmd);
	CLI_register (&unwatch_cmd);
	//CLI_register (&m2mget_cmd);
	CLI_register (&xpath_cmd);
	CLI_register (&log_cmd);
}
