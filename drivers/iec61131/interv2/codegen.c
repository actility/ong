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
Original work by
https://github.com/lsegal/my_toy_compiler
*/

#include <stdio.h>
#include <stddef.h> // important for list_for_each

#include <rtlbase.h>
#include <rtllist.h>

// From ../bin
#include <param_u.h>
#include "timeoper.h"
#include "iec_debug.h"

#include "codegen.h"
#include "parser.h"
#include "cb.h"



extern IEC_DINT getInteger(char *str);
extern IEC_REAL getReal(char *str);
extern IEC_LREAL getLReal(char *str);
extern IEC_TIME getDuration(char *str);
extern IEC_DATE getDate(char *str);
extern IEC_TOD getTimeOfDay(char *str);
extern IEC_DT getDateAndTime(char *str);
extern void setReturnParameter (param_ret_t *pret, void *value, int len, char type);

inter_node_t *New_Node(inter_type_t type) {
	inter_node_t *node = malloc(sizeof(inter_node_t));
	if	(!node)
		return	NULL;
	node->type	= type;
	return	node;
}

inter_node_t *New_Identifier(char *value) {
	RTL_TRDBG (TRACE_DETAIL, "T_IDENTIFIER %s\n", value);
	inter_node_t *node = New_Node(T_IDENTIFIER);
	node->u.identifier.name = strdup(value);
	return node;
}

inter_node_t *New_Integer(char *value) {
	RTL_TRDBG (TRACE_DETAIL, "T_INTEGER %s\n", value);
	inter_node_t *node = New_Node(T_INTEGER);
	node->u.integer.value = getInteger(value);
	return node;
}

inter_node_t *New_Float(char *value) {
	RTL_TRDBG (TRACE_DETAIL, "T_FLOAT %s\n", value);
	inter_node_t *node = New_Node(T_FLOAT);
	node->u._float.value = getReal(value);
	return node;
}

inter_node_t *New_Double(char *value) {
	RTL_TRDBG (TRACE_DETAIL, "T_DOUBLE %s\n", value);
	inter_node_t *node = New_Node(T_DOUBLE);
	node->u._double.value = getLReal(value);
	return node;
}

inter_node_t *New_Boolean(BOOL value) {
	RTL_TRDBG (TRACE_DETAIL, "T_BOOLEAN %d\n", value);
	inter_node_t *node = New_Node(T_BOOLEAN);
	node->u.boolean.value = value;
	return node;
}

inter_node_t *New_Time(char *value) {
	RTL_TRDBG (TRACE_DETAIL, "T_TIME %s\n", value);
	inter_node_t *node = New_Node(T_TIME);
	node->u.time.value = getDuration(value);
	return node;
}

inter_node_t *New_Date(char *value) {
	RTL_TRDBG (TRACE_DETAIL, "T_DATE %s\n", value);
	inter_node_t *node = New_Node(T_DATE);
	node->u.date.value = getDate(value);
	return node;
}

inter_node_t *New_ToD(char *value) {
	RTL_TRDBG (TRACE_DETAIL, "T_TOD %s\n", value);
	inter_node_t *node = New_Node(T_TOD);
	node->u.tod.value = getTimeOfDay(value);
	return node;
}

inter_node_t *New_DateTime(char *value) {
	RTL_TRDBG (TRACE_DETAIL, "T_DATETIME %s\n", value);
	inter_node_t *node = New_Node(T_DATETIME);
	node->u.datetime.value = getDateAndTime(value);
	return node;
}

inter_node_t *New_String(char *value) {
	RTL_TRDBG (TRACE_DETAIL, "T_STRING %s\n", value);
	inter_node_t *node = New_Node(T_STRING);
	//	Removing quotes
	char *pt = strdup(value+1);
	pt[strlen(pt)-1] = 0;
	node->u.string.value = pt;
	return node;
}

inter_node_t *New_FunctionCall(inter_node_t *identifier, struct list_head *arguments) {
	RTL_TRDBG (TRACE_DETAIL, "T_FUNCTION_CALL %s\n", identifier->u.identifier.name);
	inter_node_t *node = New_Node(T_FUNCTION_CALL);
	node->u.function_call.identifier = identifier;
	node->u.function_call.arguments = arguments;
	return node;
}

char *opDisplay(int op) {
	switch (op) {
	case TPLUS:	return "+";
	case TMINUS:	return "-";
	case TMUL:	return "*";
	case TDIV:	return "/";
	case TCEQ:	return "EQ";
	case TCNE:	return "NE";
	case TCLT:	return "LT";
	case TCLE:	return "LE";
	case TCGT:	return "GT";
	case TCGE:	return "GE";
	case TNOT:	return "NOT";
	case TAND:	return "AND";
	case TOR:	return "OR";
	case TXOR:	return "XOR";
	}
	return "?";
}

inter_node_t *New_BinaryOp(inter_node_t *lhs, int op, inter_node_t *rhs) {
	RTL_TRDBG (TRACE_DETAIL, "T_BINARY_OP %d (%s)\n", op, opDisplay(op));
	inter_node_t *node = New_Node(T_BINARY_OP);
	node->u.binary_op.lhs = lhs;
	node->u.binary_op.op = op;
	node->u.binary_op.rhs = rhs;
	return node;
}

inter_node_t *New_UnaryOp(int op, inter_node_t *rhs) {
	RTL_TRDBG (TRACE_DETAIL, "T_UNARY_OP %d (%s)\n", op, opDisplay(op));
	inter_node_t *node = New_Node(T_UNARY_OP);
	node->u.unary_op.op = op;
	node->u.unary_op.rhs = rhs;
	return node;
}

inter_node_t *New_Assignment(inter_node_t *lhs, inter_node_t *rhs) {
	RTL_TRDBG (TRACE_DETAIL, "T_ASSIGNMENT\n");
	inter_node_t *node = New_Node(T_ASSIGNMENT);
	node->u.assignment.lhs = lhs;
	node->u.assignment.rhs = rhs;
	return node;
}

inter_node_t *New_Block() {
	RTL_TRDBG (TRACE_DETAIL, "T_BLOCK\n");
	inter_node_t *node = New_Node(T_BLOCK);
	INIT_LIST_HEAD (&node->u.block.statements);
	return node;
}

void Add_Statement(inter_node_t *block, inter_node_t *statement) {
	list_add_tail (&statement->m_list, &block->u.block.statements);
}

inter_node_t *New_ExpressionStatement(inter_node_t *expression) {
	RTL_TRDBG (TRACE_DETAIL, "T_EXPRESSION_STATEMENT\n");
	inter_node_t *node = New_Node(T_EXPRESSION_STATEMENT);
	node->u.expressionStatement.expression = expression;
	return node;
}

struct list_head *New_ElsifList() {
	struct list_head *pt = (struct list_head *)malloc(sizeof(struct list_head));
	INIT_LIST_HEAD (pt);
	return pt;
}

void Add_Elsif(struct list_head *ElsifList, inter_node_t *elsif) {
	list_add_tail (&elsif->m_list, ElsifList);
}

struct list_head *New_ExpressionList() {
	struct list_head *pt = (struct list_head *)malloc(sizeof(struct list_head));
	INIT_LIST_HEAD (pt);
	return pt;
}

void Add_ExpressionList(struct list_head *list, inter_node_t *node) {
	list_add_tail (&node->m_list, list);
}

inter_node_t *New_If(inter_node_t *expr, inter_node_t *block, inter_node_t *elseblock, struct list_head *elsiflist) {
	RTL_TRDBG (TRACE_DETAIL, "T_IF\n");
	inter_node_t *node = New_Node(T_IF);
	node->u._if.expr = expr;
	node->u._if.block = block;
	node->u._if.elseblock = elseblock;
	node->u._if.elsiflist = elsiflist;
	return node;
}

inter_node_t *New_Elsif(inter_node_t *expr, inter_node_t *block) {
	RTL_TRDBG (TRACE_DETAIL, "T_ELSIF\n");
	inter_node_t *node = New_Node(T_ELSIF);
	node->u.elsif.expr = expr;
	node->u.elsif.block = block;
	return node;
}







static BOOL timeFamily(int t) {
	return (t == TIMEtype || t == DATEtype || t == DTtype || t == TODtype);
}

param_ret_t *binaryOp_codeGen(inter_node_t *node, void *context);

param_ret_t *codeGen(inter_node_t *node, void *userctx) {
	if	(!node) {
		return NULL;
	}
	param_ret_t *pret = NULL;

	switch (node->type) {
	case	T_INTEGER :
		pret = malloc(sizeof(param_ret_t));
		pret->t	= DINTtype;
		pret->p.DINTvar	= node->u.integer.value;
		break;
	case	T_FLOAT :
		pret = malloc(sizeof(param_ret_t));
		pret->t	= REALtype;
		pret->p.REALvar	= node->u._float.value;
		break;
	case	T_DOUBLE :
		pret = malloc(sizeof(param_ret_t));
		pret->t	= LREALtype;
		pret->p.LREALvar = node->u._double.value;
		break;
	case	T_BOOLEAN :
		pret = malloc(sizeof(param_ret_t));
		pret->t	= BOOLtype;
		pret->p.BOOLvar	= node->u.boolean.value;
		break;
	case	T_STRING :
		pret = malloc(sizeof(param_ret_t));
		setReturnParameter (pret, node->u.string.value, 0, STRINGtype);
		break;
	case	T_TIME :
		pret = malloc(sizeof(param_ret_t));
		pret->t	= TIMEtype;
		pret->p.DATEvar	= node->u.time.value;
		break;
	case	T_DATE :
		pret = malloc(sizeof(param_ret_t));
		pret->t	= DATEtype;
		pret->p.DATEvar	= node->u.date.value;
		break;
	case	T_TOD :
		pret = malloc(sizeof(param_ret_t));
		pret->t	= TODtype;
		pret->p.DATEvar	= node->u.tod.value;
		break;
	case	T_DATETIME :
		pret = malloc(sizeof(param_ret_t));
		pret->t	= DTtype;
		pret->p.DATEvar	= node->u.datetime.value;
		break;
	case	T_IDENTIFIER :
		pret = malloc(sizeof(param_ret_t));
		cb_getVar (userctx, node->u.identifier.name, pret);
		break;
	case	T_FUNCTION_CALL :
		{
		int	argc=0, i;
		param_ret_t	*argv[20];
		inter_node_t *subnode;
		list_for_each_entry (subnode, node->u.function_call.arguments, m_list) {
			argv[argc++] = codeGen(subnode, userctx);
		}
		pret = malloc(sizeof(param_ret_t));

		inter_node_t *identifier = node->u.function_call.identifier;
		cb_callFunc (userctx, identifier->u.identifier.name, argc, argv, pret);

		for	(i=0; i<argc; i++)
			free(argv[i]);
		}
		break;
	case	T_BINARY_OP :
		pret = binaryOp_codeGen (node, userctx);
		break;
	case	T_UNARY_OP :
		pret = codeGen(node->u.unary_op.rhs, userctx);
		if	(!pret) break;
		if	(pret->t == BOOLtype && node->u.unary_op.op == TNOT)
			pret->p.BOOLvar	= !pret->p.BOOLvar; break;
		break;
	case	T_ASSIGNMENT :
		pret = codeGen(node->u.assignment.rhs, userctx);
		if	(pret) {
			inter_node_t *node2 = node->u.assignment.lhs;
			cb_setVar (userctx, node2->u.identifier.name, pret);
			free (pret);
			return NULL;
		}
		break;
	case	T_BLOCK :
		{
		inter_node_t *subnode;
		list_for_each_entry (subnode, &node->u.block.statements, m_list) {
			// Free previous pret
			if	(pret)
				free (pret);
			pret = codeGen(subnode, userctx);
		}
		}
		break;
	case	T_EXPRESSION_STATEMENT :
		pret = codeGen(node->u.expressionStatement.expression, userctx);
		break;
	case	T_IF :
		{
		inter_node_t *subnode;
		pret = codeGen(node->u._if.expr, userctx);
		if	(pret->t != BOOLtype) {
			free (pret);
			return NULL;
		}
		if	(pret->p.BOOLvar == TRUE) {
			codeGen (node->u._if.block, userctx);
			free (pret);
			return NULL;
		}

		free (pret);

		if	(node->u._if.elsiflist) {
			list_for_each_entry (subnode, node->u._if.elsiflist, m_list) {
				pret = codeGen(subnode, userctx);
				if	(pret && (pret->p.BOOLvar == TRUE)) {
					free (pret);
					return	NULL;
				}
				free (pret);
			}
		}

		if	(node->u._if.elseblock) {
			codeGen (node->u._if.elseblock, userctx);
			return	NULL;
		}
		}
		break;
	case	T_ELSIF :
		pret = codeGen (node->u.elsif.expr, userctx);
		if	(!pret)
			return NULL;
		if	(pret->t != BOOLtype) {
			free (pret);
			return	NULL;
		}
		if	(pret->p.BOOLvar == TRUE)
			codeGen (node->u.elsif.block, userctx);
		break;
	}
	return	pret;
}

extern const char *typeString[];

param_ret_t *binaryOp_codeGen(inter_node_t *node, void *userctx) {
	int op = node->u.binary_op.op;

	param_ret_t *left = codeGen(node->u.binary_op.lhs, userctx);
	if	(!left) {
		RTL_TRDBG (TRACE_ERROR, "ST: BINARY_OP %d (%s): left value is NULL\n", op, opDisplay(op));
		return	NULL;
	}
	param_ret_t *right = codeGen(node->u.binary_op.rhs, userctx);
	if	(!right) {
		RTL_TRDBG (TRACE_ERROR, "ST: BINARY_OP %d (%s): right value is NULL\n", op, opDisplay(op));
		free (left);
		return	NULL;
	}
	if	(left->t != right->t) {
		if (!timeFamily(left->t) || !timeFamily(right->t))
		{
			RTL_TRDBG (TRACE_ERROR, "ST: BINARY_OP %d (%s): types are differents left:%s right:%s\n",
				op, opDisplay(op), typeString[(int)left->t], typeString[(int)right->t]);
			return	NULL;
		}
	}

	param_ret_t *pret = malloc(sizeof(param_ret_t));
	pret->t	= left->t;

	switch	(pret->t) {
	case	DINTtype :
		switch (node->u.binary_op.op) {
		case TPLUS:	pret->p.DINTvar	= left->p.DINTvar + right->p.DINTvar; break;
		case TMINUS:	pret->p.DINTvar	= left->p.DINTvar - right->p.DINTvar; break;
		case TMUL:	pret->p.DINTvar	= left->p.DINTvar * right->p.DINTvar; break;
		case TDIV:
			if (right->p.DINTvar)
				pret->p.DINTvar	= left->p.DINTvar / right->p.DINTvar;
			else
				pret->p.DINTvar	= 0;
			break;
		case TCEQ:
			pret->p.BOOLvar	= (left->p.DINTvar == right->p.DINTvar);
			pret->t	= BOOLtype;
			break;
		case TCNE:
			pret->p.BOOLvar	= (left->p.DINTvar != right->p.DINTvar);
			pret->t	= BOOLtype;
			break;
		case TCLT:
			pret->p.BOOLvar	= (left->p.DINTvar < right->p.DINTvar);
			pret->t	= BOOLtype;
			break;
		case TCLE:
			pret->p.BOOLvar	= (left->p.DINTvar <= right->p.DINTvar);
			pret->t	= BOOLtype;
			break;
		case TCGT:
			pret->p.BOOLvar	= (left->p.DINTvar > right->p.DINTvar);
			pret->t	= BOOLtype;
			break;
		case TCGE:
			pret->p.BOOLvar	= (left->p.DINTvar >= right->p.DINTvar);
			pret->t	= BOOLtype;
			break;
		}
		break;
	case	LREALtype :
		switch (node->u.binary_op.op) {
		case TPLUS:	pret->p.LREALvar	= left->p.LREALvar + right->p.LREALvar; break;
		case TMINUS:	pret->p.LREALvar	= left->p.LREALvar - right->p.LREALvar; break;
		case TMUL:	pret->p.LREALvar	= left->p.LREALvar * right->p.LREALvar; break;
		case TDIV:
			if (right->p.LREALvar)
				pret->p.LREALvar	= left->p.LREALvar / right->p.LREALvar;
			else
				pret->p.LREALvar	= 0;
			break;
		case TCEQ:
			pret->p.BOOLvar	= (left->p.LREALvar == right->p.LREALvar);
			pret->t	= BOOLtype;
			break;
		case TCNE:
			pret->p.BOOLvar	= (left->p.LREALvar != right->p.LREALvar);
			pret->t	= BOOLtype;
			break;
		case TCLT:
			pret->p.BOOLvar	= (left->p.LREALvar < right->p.LREALvar);
			pret->t	= BOOLtype;
			break;
		case TCLE:
			pret->p.BOOLvar	= (left->p.LREALvar <= right->p.LREALvar);
			pret->t	= BOOLtype;
			break;
		case TCGT:
			pret->p.BOOLvar	= (left->p.LREALvar > right->p.LREALvar);
			pret->t	= BOOLtype;
			break;
		case TCGE:
			pret->p.BOOLvar	= (left->p.LREALvar >= right->p.LREALvar);
			pret->t	= BOOLtype;
			break;
		}
		break;
	case	REALtype :
		switch (node->u.binary_op.op) {
		case TPLUS:	pret->p.REALvar	= left->p.REALvar + right->p.REALvar; break;
		case TMINUS:	pret->p.REALvar	= left->p.REALvar - right->p.REALvar; break;
		case TMUL:	pret->p.REALvar	= left->p.REALvar * right->p.REALvar; break;
		case TDIV:
			if (right->p.REALvar)
				pret->p.REALvar	= left->p.REALvar / right->p.REALvar;
			else
				pret->p.REALvar	= 0;
			break;
		case TCEQ:
			pret->p.BOOLvar	= (left->p.REALvar == right->p.REALvar);
			pret->t	= BOOLtype;
			break;
		case TCNE:
			pret->p.BOOLvar	= (left->p.REALvar != right->p.REALvar);
			pret->t	= BOOLtype;
			break;
		case TCLT:
			pret->p.BOOLvar	= (left->p.REALvar < right->p.REALvar);
			pret->t	= BOOLtype;
			break;
		case TCLE:
			pret->p.BOOLvar	= (left->p.REALvar <= right->p.REALvar);
			pret->t	= BOOLtype;
			break;
		case TCGT:
			pret->p.BOOLvar	= (left->p.REALvar > right->p.REALvar);
			pret->t	= BOOLtype;
			break;
		case TCGE:
			pret->p.BOOLvar	= (left->p.REALvar >= right->p.REALvar);
			pret->t	= BOOLtype;
			break;
		}
		break;
	case	BOOLtype :
		pret->t	= BOOLtype;
		switch (node->u.binary_op.op) {
		case TAND:	pret->p.BOOLvar	= left->p.BOOLvar & right->p.BOOLvar; break;
		case TOR:	pret->p.BOOLvar	= left->p.BOOLvar | right->p.BOOLvar; break;
		case TXOR:	pret->p.BOOLvar	= left->p.BOOLvar ^ right->p.BOOLvar; break;
		case TCEQ: pret->p.BOOLvar	= left->p.BOOLvar == right->p.BOOLvar; break;
		}
		break;
	case	TIMEtype :
		switch (node->u.binary_op.op) {
		case TPLUS:
			ADD_TIMESPEC3(pret->p.TIMEvar, left->p.TIMEvar, right->p.TIMEvar);
			break;
		case TMINUS:
			SUB_TIMESPEC3(pret->p.TIMEvar, left->p.TIMEvar, right->p.TIMEvar);
			break;
		case TCEQ:
			pret->p.BOOLvar = (CMP_TIMESPEC(left->p.TIMEvar, right->p.TIMEvar) == 0);
			pret->t	= BOOLtype;
			break;
		case TCNE:
			pret->p.BOOLvar = (CMP_TIMESPEC(left->p.TIMEvar, right->p.TIMEvar) != 0);
			pret->t	= BOOLtype;
			break;
		case TCLT:
			pret->p.BOOLvar = (CMP_TIMESPEC(left->p.TIMEvar, right->p.TIMEvar) < 0);
			pret->t	= BOOLtype;
			break;
		case TCLE:
			pret->p.BOOLvar = (CMP_TIMESPEC(left->p.TIMEvar, right->p.TIMEvar) <= 0);
			pret->t	= BOOLtype;
			break;
		case TCGT:
			pret->p.BOOLvar = (CMP_TIMESPEC(left->p.TIMEvar, right->p.TIMEvar) > 0);
			pret->t	= BOOLtype;
			break;
		case TCGE:
			pret->p.BOOLvar = (CMP_TIMESPEC(left->p.TIMEvar, right->p.TIMEvar) >= 0);
			pret->t	= BOOLtype;
			break;
		}
		break;
	case	DATEtype :
		switch (node->u.binary_op.op) {
		case TPLUS:
			ADD_TIMESPEC3(pret->p.DATEvar, left->p.DATEvar, right->p.DATEvar);
			break;
		case TMINUS:
			SUB_TIMESPEC3(pret->p.DATEvar, left->p.DATEvar, right->p.DATEvar);
			break;
		case TCEQ:
			pret->p.BOOLvar = (CMP_TIMESPEC(left->p.DATEvar, right->p.DATEvar) == 0);
			pret->t	= BOOLtype;
			break;
		case TCNE:
			pret->p.BOOLvar = (CMP_TIMESPEC(left->p.DATEvar, right->p.DATEvar) != 0);
			pret->t	= BOOLtype;
			break;
		case TCLT:
			pret->p.BOOLvar = (CMP_TIMESPEC(left->p.DATEvar, right->p.DATEvar) < 0);
			pret->t	= BOOLtype;
			break;
		case TCLE:
			pret->p.BOOLvar = (CMP_TIMESPEC(left->p.DATEvar, right->p.DATEvar) <= 0);
			pret->t	= BOOLtype;
			break;
		case TCGT:
			pret->p.BOOLvar = (CMP_TIMESPEC(left->p.DATEvar, right->p.DATEvar) > 0);
			pret->t	= BOOLtype;
			break;
		case TCGE:
			pret->p.BOOLvar = (CMP_TIMESPEC(left->p.DATEvar, right->p.DATEvar) >= 0);
			pret->t	= BOOLtype;
			break;
		}
		break;
	case	TODtype :
		switch (node->u.binary_op.op) {
		case TPLUS:
			ADD_TIMESPEC3(pret->p.TODvar, left->p.TODvar, right->p.TODvar);
			break;
		case TMINUS:
			SUB_TIMESPEC3(pret->p.TODvar, left->p.TODvar, right->p.TODvar);
			break;
		case TCEQ:
			pret->p.BOOLvar = (CMP_TIMESPEC(left->p.TODvar, right->p.TODvar) == 0);
			pret->t	= BOOLtype;
			break;
		case TCNE:
			pret->p.BOOLvar = (CMP_TIMESPEC(left->p.TODvar, right->p.TODvar) != 0);
			pret->t	= BOOLtype;
			break;
		case TCLT:
			pret->p.BOOLvar = (CMP_TIMESPEC(left->p.TODvar, right->p.TODvar) < 0);
			pret->t	= BOOLtype;
			break;
		case TCLE:
			pret->p.BOOLvar = (CMP_TIMESPEC(left->p.TODvar, right->p.TODvar) <= 0);
			pret->t	= BOOLtype;
			break;
		case TCGT:
			pret->p.BOOLvar = (CMP_TIMESPEC(left->p.TODvar, right->p.TODvar) > 0);
			pret->t	= BOOLtype;
			break;
		case TCGE:
			pret->p.BOOLvar = (CMP_TIMESPEC(left->p.TODvar, right->p.TODvar) >= 0);
			pret->t	= BOOLtype;
			break;
		}
		break;
	case	DTtype :
		switch (node->u.binary_op.op) {
		case TPLUS:
			ADD_TIMESPEC3(pret->p.DTvar, left->p.DTvar, right->p.DTvar);
			break;
		case TMINUS:
			SUB_TIMESPEC3(pret->p.DTvar, left->p.DTvar, right->p.DTvar);
			break;
		case TCEQ:
			pret->p.BOOLvar = (CMP_TIMESPEC(left->p.DTvar, right->p.DTvar) == 0);
			pret->t	= BOOLtype;
			break;
		case TCNE:
			pret->p.BOOLvar = (CMP_TIMESPEC(left->p.DTvar, right->p.DTvar) != 0);
			pret->t	= BOOLtype;
			break;
		case TCLT:
			pret->p.BOOLvar = (CMP_TIMESPEC(left->p.DTvar, right->p.DTvar) < 0);
			pret->t	= BOOLtype;
			break;
		case TCLE:
			pret->p.BOOLvar = (CMP_TIMESPEC(left->p.DTvar, right->p.DTvar) <= 0);
			pret->t	= BOOLtype;
			break;
		case TCGT:
			pret->p.BOOLvar = (CMP_TIMESPEC(left->p.DTvar, right->p.DTvar) > 0);
			pret->t	= BOOLtype;
			break;
		case TCGE:
			pret->p.BOOLvar = (CMP_TIMESPEC(left->p.DTvar, right->p.DTvar) >= 0);
			pret->t	= BOOLtype;
			break;
		}
		break;
	}
	free (left);
	free (right);
	return	pret;
}

void cleanUp(inter_node_t *node) {
	if	(!node) return;

	switch (node->type) {
	case T_INTEGER :
	case T_FLOAT :
	case T_DOUBLE :
	case T_BOOLEAN :
	case T_TIME :
	case T_DATE :
	case T_TOD :
	case T_DATETIME :
		break;
	case T_STRING :
		free (node->u.string.value);
		break;
	case T_IDENTIFIER :
		free (node->u.identifier.name);
		break;
	case T_FUNCTION_CALL :
		cleanUp (node->u.function_call.identifier);
		{
		inter_node_t *subnode;
		list_for_each_entry (subnode, node->u.function_call.arguments, m_list)
			cleanUp (subnode);
		}
		free (node->u.function_call.arguments);
		break;
	case T_BINARY_OP :
		cleanUp (node->u.binary_op.lhs);
		cleanUp (node->u.binary_op.rhs);
		break;
	case T_UNARY_OP :
		cleanUp (node->u.unary_op.rhs);
		break;
	case T_ASSIGNMENT :
		cleanUp (node->u.assignment.lhs);
		cleanUp (node->u.assignment.rhs);
		break;
	case T_BLOCK :
		{
		inter_node_t *subnode;
		list_for_each_entry (subnode, &node->u.block.statements, m_list)
			cleanUp (subnode);
		}
		break;
	case T_EXPRESSION_STATEMENT :
		cleanUp (node->u.expressionStatement.expression);
		break;
	case T_IF :
		{
		inter_node_t *subnode;
		if	(node->u._if.expr)
			cleanUp (node->u._if.expr);
		if	(node->u._if.block)
			cleanUp (node->u._if.block);

		if	(node->u._if.elsiflist) {
			list_for_each_entry (subnode, node->u._if.elsiflist, m_list)
				cleanUp (subnode);
			free (node->u._if.elsiflist);
		}

		if	(node->u._if.elseblock)
			cleanUp (node->u._if.elseblock);
		}
		break;
	case	T_ELSIF :
		if	(node->u.elsif.expr)
			cleanUp (node->u.elsif.expr);
		if	(node->u.elsif.block)
			cleanUp (node->u.elsif.block);
		break;
	}
	free (node);
}
