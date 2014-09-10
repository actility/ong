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
	#include "node.h"
        #include <cstdio>
        #include <cstdlib>
	NBlock *programBlock; /* the top level root node of our final AST */

	extern int yylex();
	//void yyerror(const char *s) { std::printf("Error: %s\n", s);std::exit(1); }
	void yyerror(const char *s) { std::printf("Error: %s\n", s); }
%}

/* Represents the many different ways we can access our data */
%union {
	Node *node;
	NBlock *block;
	NExpression *expr;
	NStatement *stmt;
	NIdentifier *ident;
	NVariableDeclaration *var_decl;
	NElsif *elsif_decl;
	std::vector<NVariableDeclaration*> *varvec;
	std::vector<NExpression*> *exprvec;
	std::vector<NElsif*> *elsifvec;
	std::string *string;
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
%type <varvec> func_decl_args
%type <exprvec> call_args
%type <block> program stmts block
%type <stmt> stmt var_decl func_decl if_stmt
%type <token> comparison
%type <elsif_decl> elsif_stmt
%type <elsifvec> elsif_stmts

/* Operator precedence for mathematical operators */
%left TAND TOR TXOR
%left TCEQ TCNE TCLT TCLE TCGT TCGE
%left TPLUS TMINUS
%left TMUL TDIV

%start program

%%

program : stmts { programBlock = $1; }
		;
		
stmts : stmt { $$ = new NBlock(); $$->statements.push_back($1); }
	| stmts stmt { $1->statements.push_back($2); }
	;
/*
	| stmts stmt { $1->statements.push_back($<stmt>2); }
*/

stmt : var_decl ';' | func_decl
	| expr ';' { $$ = new NExpressionStatement(*$1); }
	| expr { $$ = new NExpressionStatement(*$1); }
	| if_stmt
	;

if_stmt : TIF expr TTHEN stmts TENDIF { $$ = new NIf(*$2, *$4); }
	| TIF expr TTHEN stmts TELSE stmts TENDIF { $$ = new NIf(*$2, *$4, $6); }
	| TIF expr TTHEN stmts elsif_stmts TENDIF { $$ = new NIf(*$2, *$4, NULL, $5); }
	| TIF expr TTHEN stmts elsif_stmts TELSE stmts TENDIF { $$ = new NIf(*$2, *$4, $7, $5); }
	;

//elsif_stmts : /* empty */ { $$ = new ElsifList(); }

elsif_stmts : elsif_stmt { $$ = new ElsifList(); $$->push_back($1); }
	| elsif_stmts elsif_stmt { $1->push_back($2); }
	;

elsif_stmt : TELSIF expr TTHEN stmts { $$ = new NElsif(*$2, *$4); }
	;

block : TLBRACE stmts TRBRACE { $$ = $2; }
	  | TLBRACE TRBRACE { $$ = new NBlock(); }
	  ;

var_decl : ident ident { $$ = new NVariableDeclaration(*$1, *$2); }
	| ident ident TEQUAL expr { $$ = new NVariableDeclaration(*$1, *$2, $4); }
	;

func_decl : ident ident TLPAREN func_decl_args TRPAREN block 
			{ $$ = new NFunctionDeclaration(*$1, *$2, *$4, *$6); delete $4; }
		  ;
	
func_decl_args : /*blank*/  { $$ = new VariableList(); }
		  | var_decl { $$ = new VariableList(); $$->push_back($<var_decl>1); }
		  | func_decl_args TCOMMA var_decl { $1->push_back($<var_decl>3); }
		  ;

ident : TIDENTIFIER { $$ = new NIdentifier(*$1); delete $1; }
	  ;

numeric : TINTEGER { $$ = new NInteger(*$1); delete $1; }
		| TDOUBLE { $$ = new NDouble(*$1); delete $1; }
		| TFLOAT { $$ = new NFloat(*$1); delete $1; }
		| TTRUE { $$ = new NBoolean(TRUE); }
		| TFALSE { $$ = new NBoolean(FALSE); }
		| TTIME { $$ = new NTime(*$1); delete $1; }
		| TDATE { $$ = new NDate(*$1); delete $1; }
		| TTOD { $$ = new NTod(*$1); delete $1; }
		| TDT { $$ = new NDateTime(*$1); delete $1; }
		| TSTRING { $$ = new NString(*$1); delete $1; }
		;

expr : ident TEQUAL expr { $$ = new NAssignment(*$<ident>1, *$3); }
	 | ident TLPAREN call_args TRPAREN { $$ = new NMethodCall(*$1, *$3); delete $3; }
	 | ident { $<ident>$ = $1; }
	 | numeric
 	 | expr comparison expr { $$ = new NBinaryOperator(*$1, $2, *$3); }
 	 | TNOT expr { $$ = new NUnaryOperator($1, *$2); }
     | TLPAREN expr TRPAREN { $$ = $2; }
	 ;
	
call_args : /*blank*/  { $$ = new ExpressionList(); }
		  | expr { $$ = new ExpressionList(); $$->push_back($1); }
		  | call_args TCOMMA expr  { $1->push_back($3); }
		  ;

comparison : TCEQ | TCNE | TCLT | TCLE | TCGT | TCGE 
		   | TPLUS | TMINUS | TMUL | TDIV | TAND | TOR | TXOR
		   ;

%%
