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

#ifndef _PLC_H_
#define _PLC_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <sys/time.h>

#include "iec_types_all.h"

#include "param_u.h"
#include "board.h"

#ifdef TEST_EN
#undef TEST_EN
#endif

#define TEST_EN\
  if (!EN) {\
    if (ENO != NULL)\
      *ENO = FALSE;\
    return;\
  }\
  else if (ENO != NULL)\
    *ENO = TRUE;


#define CTX_TYPE_CONFIGURATION 0
#define CTX_TYPE_RESOURCE 1
#define CTX_TYPE_TASK 2
#define CTX_TYPE_POU 3
#define CTX_TYPE_BLOCK 4
#define CTX_TYPE_STEP 5

typedef struct {
	struct list_head head;
	void	*xml;
} iec_fbd_t;

typedef struct {
	//unsigned int rate;
	double rate;
	unsigned int size;
	double count;
	struct timeval last;
} iec_bucket_t;

typedef struct {
	char *data;
	int len;
	char *contentType;
} iec_buff_t;

typedef struct iec_user_s {
	struct list_head head;		// for user_list
	char	*name;
	struct list_head resource_list;
	void	*project;
	void	*m_inputTable;
	void	*m_outputTable;
	struct list_head xpath_list;
	int	level;
	void	*m_variables;		// Configuration Global Vars
	void	*m_accessVars;		// %Ixxx and %Oyyy
	void	*m_dictionnary;
	iec_buff_t	content_configuration;
	iec_buff_t	content_mapping;
	iec_bucket_t m_bucket;
	int	countTasks;
	BOOL	flagPause;
} iec_user_t;

typedef struct iec_resource_s {
	struct list_head head;		// for resource_list
	struct list_head task_list;
	char	*name;
	int	level;
	void	*m_variables;		// Resource Global Vars
	iec_user_t	*user;		// back pt
} iec_resource_t;

typedef struct {
	struct list_head head;
	struct list_head ctx_list;
	char	*name;
	char	*typeName;
	void	*m_ctx_hash;
	void	*xo_pou;
	void	*xo_block;
	void	*xo_sfc;
	void	*xo_step;
	struct list_head activeSteps_list;
#define IEC_MAX_CALLVARS 20
#define IEC_MAX_RETVARS 10
	iec_variable_t *callVars[IEC_MAX_CALLVARS];
	int	countParams;
	int	level;
	struct iec_task_s	*task;	// back ptr
	BOOL	force_schedule;
} iec_program_t;

typedef struct iec_task_s {
	struct list_head head;
	char	*name;
	TIME	interval;
	TIME	interval_initial;
	TIME	next;
	int	priority;
	char	*var_interrupt;
	void	*xo_task;
	struct list_head program_list;
	unsigned long	ticks; // *LNI* we need to discuss the size of that variable
	TIME	total_time;
	int	level;
	iec_resource_t *resource;
	BOOL	force_schedule;
	char	type;		// 0:cyclic 1:interrupt
} iec_task_t;

typedef struct {
	struct list_head head;
	void *xo_step;
	char *name;
	unsigned long pulses;	// bitmask
	unsigned char m_state;
	unsigned char attCounter;
} iec_step_t;

typedef struct {
	struct list_head head;
	char	*name;
	void	*m_variables;
	struct list_head block_list;
	void *xo_pou;
	void *xo_block;
	void *xo_step;

	void *xo_ab;
	void *xo_action;
	iec_step_t *step;
	iec_program_t	*program;	// back ptr
	BOOL	m_next;
	unsigned char m_state;
	unsigned char m_type;
} iec_ctx_t;

typedef struct {
	struct list_head head;
	void *xo_block;
	char *name;
} iec_block_t;

typedef struct {
	char *name;
	void *fct;
	char *ret_type;
	BOOL varargs;
} iec_fctinfos_t;

typedef struct {
	struct list_head user_list;
	struct list_head FBD_list;
	void	*StdFunc;
	void	*config;
	TIME	total_time;
	board_t board_errors;
	struct list_head dIa_list;
	int	level;
} iec_global_t;


extern iec_task_t *Task_New(iec_resource_t *resource, void *node);
extern void Task_Next (iec_task_t *task);
void iec_getMinWait(TIME *w);
void checkGlitch(TIME interval);
void Users_Next();
int Program_Next (iec_program_t *program);
void Program_Reschedule(iec_program_t *program);
void User_Free(iec_user_t *user);
extern iec_user_t *User_New(char *file, int lev);
iec_user_t *User_LoadUri(char *uri, iec_buff_t buf, int lev);
iec_user_t *User_GetByUri(char *uri);
iec_user_t *User_GetByIndex(int idx);
int User_LoadMapping(iec_user_t *user, char *string);
iec_resource_t *Resource_New(iec_user_t *user, void *node);
void User_Pause(iec_user_t *user, BOOL flagPause);
void User_Resume(iec_user_t *user);
void User_Start(iec_user_t *user);
extern int iec_addFunc(const iec_fctinfos_t *info);
extern int iec_addFuncs(const iec_fctinfos_t *info);
extern int iec_init();
void setReturnParameter (param_ret_t *pret, void *value, int len, char type);
iec_variable_t *createVariable (char *name, char type, void *value);
void addFieldToDerivedVariable (iec_variable_t *var, iec_variable_t *field);
void addFieldToDerivedParam (param_ret_t *pret, iec_variable_t *field);
iec_variable_t *getVariable (iec_ctx_t *ctx, char *name);
iec_ctx_t *Context_Current(iec_program_t *program);
iec_variable_t *createVariableWithParameter2 (char *name, param_ret_t *param);
int updateVariableWithParameter(iec_ctx_t *ctx, char *varname, param_ret_t *param);
iec_variable_t *addVariableInHash (void *hash, iec_variable_t *var);
iec_fctinfos_t *searchStdFunc (char *name);
iec_ctx_t *getUpperContext (iec_ctx_t *ctx);
void addVariableInContext (iec_ctx_t *ctx, iec_variable_t *var);
void setParameter (param_u_t *p, void *value, char type);
void Program_SetLevel(iec_program_t *program, int lev);
void Task_SetLevel(iec_task_t *task, int lev);
void User_SetLevel(iec_user_t *user, int lev);
iec_user_t *User_Alloc(char *appId);
int User_Reset(char *appId);
void Resource_SetLevel(iec_resource_t *resource, int lev);
void loadRetain(iec_user_t *user, void *variables);
void saveRetain();
char *printVarValue(iec_variable_t *var, char **tmp);
void GSC_log(char *error, char *message, char *ref, char *detail, ...);
void User_GSC_log(iec_user_t *user, char *error, char *message, char *ref, char *detail, ...);
int retrieve_IEC_APPLICATION(char *appId);
char *User_GetDictionnary(iec_user_t *user, char *param);
int User_LoadFileMapping(iec_user_t *user, char *fn);
int iec_m2mInit();
void iec_m2m_nextTimer(struct timeval *tv);
int iec_m2mPoll();
void printVariableInBuffer_WithName(iec_variable_t *var, char **buf, char *namevar, BOOL withTime);

#endif /* _PLC_H_ */

#ifdef __cplusplus
}
#endif
