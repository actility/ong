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

%{
	#include <stdio.h>
	#include <rtlbase.h>
	#include <iec_debug.h>
	#include "codegen.h"
	inter_node_t *programBlock; /* the top level root node of our final AST */

	extern int yylex();
	void yyerror(const char *s) { RTL_TRDBG (TRACE_ERROR, "ST Error: %s\n", s); }
%}

/* Represents the many different ways we can access our data */
%union {
	inter_node_t *node;
	inter_node_t *block;
	inter_node_t *stmt;
	inter_node_t *expr;
	char *string;
	inter_node_t *ident;
	struct list_head *call_args;
	struct list_head *elsif_stmts;
	int token;
}

/* Define our terminal symbols (tokens). This should
   match our tokens.l lex file. We also define the node type
   they represent.
 */
%token <string> TIDENTIFIER TINTEGER TDOUBLE TFLOAT TTIME TDATE TTOD TDT TSTRING
%token <token> TCEQ TCNE TCLT TCLE TCGT TCGE TEQUAL
%token <token> TLPAREN TRPAREN TLBRACE TRBRACE TCOMMA TDOT
%token <token> TPLUS TMINUS TMUL TDIV TTRUE TFALSE
%token <token> TAND TOR TXOR TNOT
%token <token> TIF TTHEN TELSE TELSIF TENDIF

/* Define the type of node our nonterminal symbols represent.
   The types refer to the %union declaration above. Ex: when
   we call an ident (defined by union type ident) we are really
   calling an (NIdentifier*). It makes the compiler happy.
 */
%type <ident> ident
%type <expr> numeric expr 
%type <call_args> call_args
%type <block> program stmts elsif_stmt
%type <stmt> stmt if_stmt
%type <token> comparison
%type <elsif_stmts> elsif_stmts

/* Operator precedence for mathematical operators */
%left TAND TOR TXOR
%left TCEQ TCNE TCLT TCLE TCGT TCGE
%left TPLUS TMINUS
%left TMUL TDIV

%start program

%%

program : { programBlock = NULL; } | stmts { programBlock = $1; }
	;

stmts : stmt { $$ = New_Block(); Add_Statement($$, $1); }
	| stmts stmt { Add_Statement($1, $2); }
	;

stmt : expr ';' { $$ = New_ExpressionStatement($1); }
	| expr { $$ = New_ExpressionStatement($1); }
	| if_stmt
	;

if_stmt : TIF expr TTHEN stmts TENDIF { $$ = New_If($2, $4, NULL, NULL); }
	| TIF expr TTHEN stmts TELSE stmts TENDIF { $$ = New_If($2, $4, $6, NULL); }
	| TIF expr TTHEN stmts elsif_stmts TENDIF { $$ = New_If($2, $4, NULL, $5); }
	| TIF expr TTHEN stmts elsif_stmts TELSE stmts TENDIF { $$ = New_If($2, $4, $7, $5); }
	;

elsif_stmts : elsif_stmt { $$ = New_ElsifList(); Add_Elsif($$, $1); }
	| elsif_stmts elsif_stmt { Add_Elsif($1, $2); }
	;

elsif_stmt : TELSIF expr TTHEN stmts { $$ = New_Elsif($2, $4); }
	;

ident : TIDENTIFIER { $$ = New_Identifier($1); }
	;

numeric : TINTEGER	{ $$ = New_Integer($1); }
	| TFLOAT	{ $$ = New_Float($1); }
	| TDOUBLE	{ $$ = New_Double($1); }
	| TTRUE		{ $$ = New_Boolean(TRUE); }
	| TFALSE	{ $$ = New_Boolean(FALSE); }
	| TTIME		{ $$ = New_Time($1); }
	| TDATE		{ $$ = New_Date($1); }
	| TTOD		{ $$ = New_ToD($1); }
	| TDT		{ $$ = New_DateTime($1); }
	| TSTRING	{ $$ = New_String($1); }
	;

expr :	ident TEQUAL expr { $$ = New_Assignment($1, $3); }
	| ident TLPAREN call_args TRPAREN { $$ = New_FunctionCall($1, $3); }
	| ident { $<ident>$ = $1; }
	| numeric
	| expr comparison expr { $$ = New_BinaryOp($1, $2, $3); }
	| TNOT expr { $$ = New_UnaryOp($1, $2); }
	| TLPAREN expr TRPAREN { $$ = $2; }
	;

call_args : /*blank*/  { $$ = New_ExpressionList(); }
	| expr { $$ = New_ExpressionList(); Add_ExpressionList($$, $1); }
	| call_args TCOMMA expr  { Add_ExpressionList($1, $3); }
	;

comparison : TCEQ | TCNE | TCLT | TCLE | TCGT | TCGE 
	| TPLUS | TMINUS | TMUL | TDIV | TAND | TOR | TXOR
	;

%%
