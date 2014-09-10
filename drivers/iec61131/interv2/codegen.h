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
#include <string.h>
#include <stdlib.h>

#include <rtllist.h>

// From ../bin
#include <param_u.h>
#include "timeoper.h"

typedef enum {
	T_INTEGER=0,
	T_FLOAT,
	T_DOUBLE,
	T_BOOLEAN,
	T_IDENTIFIER,
	T_BLOCK,
	T_FUNCTION_CALL,
	T_BINARY_OP,
	T_UNARY_OP,
	T_ASSIGNMENT,
	T_EXPRESSION_STATEMENT,
	T_TIME,
	T_DATE,
	T_TOD,
	T_DATETIME,
	T_STRING,
	T_IF,
	T_ELSIF,
} inter_type_t;


typedef struct {
	IEC_DINT value;
} inter_integer_t;

typedef struct {
	IEC_REAL value;
} inter_float_t;

typedef struct {
	IEC_LREAL value;
} inter_double_t;

typedef struct {
	char *value;
} inter_string_t;

typedef struct {
	BOOL value;
} inter_boolean_t;

typedef struct {
	char *name;
} inter_identifier_t;

typedef struct {
	struct inter_node_s *identifier;
	struct list_head *arguments;
} inter_function_call_t;

typedef struct {
	struct inter_node_s *lhs;
	int	op;
	struct inter_node_s *rhs;
} inter_binary_op_t;

typedef struct {
	int op;
	struct inter_node_s *rhs;
} inter_unary_op_t;

typedef struct {
	struct inter_node_s *lhs;
	struct inter_node_s *rhs;
} inter_assignment_t;

typedef struct {
	struct list_head statements;
} inter_block_t;

typedef struct {
	struct inter_node_s *expression;
} inter_expressionStatement_t;

typedef struct {
	IEC_DATE value;
} inter_date_t;

typedef struct {
	IEC_TIME value;
} inter_time_t;

typedef struct {
	IEC_TOD value;
} inter_tod_t;

typedef struct {
	IEC_DT value;
} inter_dt_t;

typedef struct {
	struct inter_node_s *expr;
	struct inter_node_s *block;
	struct inter_node_s *elseblock;
	struct list_head *elsiflist;
} inter_if_t;

typedef struct {
	struct inter_node_s *expr;
	struct inter_node_s *block;
} inter_elsif_t;

typedef struct inter_node_s {
	inter_type_t	type;
	struct list_head m_list;
	union {
		inter_identifier_t identifier;
		inter_integer_t integer;
		inter_float_t _float;
		inter_double_t _double;
		inter_boolean_t boolean;
		inter_function_call_t function_call;
		inter_binary_op_t binary_op;
		inter_unary_op_t unary_op;
		inter_assignment_t assignment;
		inter_block_t block;
		inter_expressionStatement_t expressionStatement;
		inter_date_t date;
		inter_time_t time;
		inter_tod_t tod;
		inter_dt_t datetime;
		inter_string_t string;
		inter_if_t _if;
		inter_elsif_t elsif;
	} u;
} inter_node_t ;


inter_node_t *New_Node(inter_type_t type);
inter_node_t *New_Identifier(char *name);
inter_node_t *New_Integer(char *value);
inter_node_t *New_Float(char *value);
inter_node_t *New_Double(char *value);
inter_node_t *New_Boolean(BOOL value);
inter_node_t *New_Time(char *value);
inter_node_t *New_Date(char *value);
inter_node_t *New_ToD(char *value);
inter_node_t *New_DateTime(char *value);
inter_node_t *New_String(char *value);
inter_node_t *New_FunctionCall(inter_node_t *identifier, struct list_head *arguments);
inter_node_t *New_BinaryOp(inter_node_t *lhs, int op, inter_node_t *rhs);
inter_node_t *New_UnaryOp(int op, inter_node_t *rhs);
inter_node_t *New_Assignment(inter_node_t *lhs, inter_node_t *rhs);
inter_node_t *New_Block();
void Add_Statement(inter_node_t *block, inter_node_t *statement);
inter_node_t *New_ExpressionStatement(inter_node_t *expression);
struct list_head *New_ElsifList();
void Add_Elsif(struct list_head *ElsifList, inter_node_t *elsif);
struct list_head *New_ExpressionList();
void Add_ExpressionList(struct list_head *list, inter_node_t *node);
inter_node_t *New_If(inter_node_t *expr, inter_node_t *block, inter_node_t *elseblock, struct list_head *elsiflist);
inter_node_t *New_Elsif(inter_node_t *expr, inter_node_t *block);
param_ret_t *codeGen(inter_node_t *node, void *userctx);
void cleanUp(inter_node_t *node);

