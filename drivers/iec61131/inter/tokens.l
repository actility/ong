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
#include <string>
#include "node.h"
#include "parser.hpp"
#define SAVE_TOKEN yylval.string = new std::string(yytext, yyleng)
#define TOKEN(t) (yylval.token = t)
extern "C" int yywrap() { }
%}

%option case-insensitive
%x	comment

%%

[ \t\n]					;
"-"{0,1}[0-9]+\.[0-9]*		SAVE_TOKEN; return TFLOAT;
"-"{0,1}[0-9]+			SAVE_TOKEN; return TINTEGER;
"2#"[0-1_]+			SAVE_TOKEN; return TINTEGER;
"8#"[0-7]+			SAVE_TOKEN; return TINTEGER;
"16#"[0-9A-Fa-f]+		SAVE_TOKEN; return TINTEGER;
":="				return TOKEN(TEQUAL);
"="				return TOKEN(TCEQ);
"EQ"				return TOKEN(TCEQ);
"!="				return TOKEN(TCNE);
"NE"				return TOKEN(TCNE);
"<>"				return TOKEN(TCNE);
"<"				return TOKEN(TCLT);
"LT"				return TOKEN(TCLT);
"<="				return TOKEN(TCLE);
"LE"				return TOKEN(TCLE);
">"				return TOKEN(TCGT);
"GT"				return TOKEN(TCGT);
">="				return TOKEN(TCGE);
"GE"				return TOKEN(TCGE);
"("				return TOKEN(TLPAREN);
")"				return TOKEN(TRPAREN);
"{"				return TOKEN(TLBRACE);
"}"				return TOKEN(TRBRACE);
"."				return TOKEN(TDOT);
","				return TOKEN(TCOMMA);
"+"				return TOKEN(TPLUS);
"-"				return TOKEN(TMINUS);
"*"				return TOKEN(TMUL);
"/"				return TOKEN(TDIV);
"TRUE"				return TOKEN(TTRUE);
"FALSE"				return TOKEN(TFALSE);
"AND"				return TOKEN(TAND);
"OR"				return TOKEN(TOR);
"XOR"				return TOKEN(TXOR);
"NOT"				return TOKEN(TNOT);
"TIME#""-"{0,1}[0-9dhms]*	SAVE_TOKEN; return TTIME;
"T#""-"{0,1}[0-9dhms]*		SAVE_TOKEN; return TTIME;
"DATE#"[0-9\-]*			SAVE_TOKEN; return TDATE;
"D#"[0-9\-]*			SAVE_TOKEN; return TDATE;
"TIME_OF_DAY#"[0-9:.]*		SAVE_TOKEN; return TTOD;
"TOD#"[0-9:.]*			SAVE_TOKEN; return TTOD;
"DATE_AND_TIME#"[0-9:.\-]*	SAVE_TOKEN; return TDT;
"DT#"[0-9:.\-]*			SAVE_TOKEN; return TDT;

"IF"				return TOKEN(TIF);
"THEN"				return TOKEN(TTHEN);
"ELSE"				return TOKEN(TELSE);
"ELSIF"				return TOKEN(TELSIF);
"END_IF"			return TOKEN(TENDIF);

"/*" BEGIN(comment);
<comment>"*/" BEGIN(INITIAL);
<comment>.|\n

"//"[^\n]*\n

"'"[a-zA-Z0-9_. ]*"'"		SAVE_TOKEN; return TSTRING;
"\""[a-zA-Z0-9_. ]*"\""		SAVE_TOKEN; return TSTRING;

[a-zA-Z_][a-zA-Z0-9_.]*		SAVE_TOKEN; return TIDENTIFIER;
.	{return yytext[0];}

%%
