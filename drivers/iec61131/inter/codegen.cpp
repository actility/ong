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
#include <typeinfo>

#include "node.h"
#include "codegen.h"
#include "parser.hpp"
#include "cb.h"

// From ../bin
#include <param_u.h>
#include "timeoper.h"

using namespace std;

/* Compile the AST into a module */
IECParam* CodeGenContext::generateCode(NBlock& root)
{
	return root.codeGen(*this);
}

/* -- Code Generation -- */

extern "C" IEC_DINT getInteger(char *str);

void NInteger::undo() {
#ifdef MAIN
	std::cout << "undo NInteger '" << value << "'" << endl;
#endif
	delete this;
}
IECParam* NInteger::codeGen(CodeGenContext& context)
{
#ifdef MAIN
	std::cout << "codeGen NInteger '" << value << "'" << endl;
#endif
	IECParam	*pret = new IECParam();
	pret->iec.t	= DINTtype;
	pret->iec.p.DINTvar	= getInteger((char *)value.c_str());
	return	pret;
}

extern "C" REAL getReal(char *str);

void NFloat::undo() { delete this; }
IECParam* NFloat::codeGen(CodeGenContext& context)
{
	IECParam	*pret = new IECParam();
	pret->iec.t	= REALtype;
	pret->iec.p.REALvar	= getReal((char *)value.c_str());
	return	pret;
}

extern "C" LREAL getLReal(char *str);

void NDouble::undo() { delete this; }
IECParam* NDouble::codeGen(CodeGenContext& context)
{
	IECParam	*pret = new IECParam();
	pret->iec.t	= LREALtype;
	pret->iec.p.LREALvar	= getLReal((char *)value.c_str());
	return	pret;
}

void NBoolean::undo() {
#ifdef MAIN
	std::cout << "undo NBoolean '" << value << "'" << endl;
#endif
	delete this;
}
IECParam* NBoolean::codeGen(CodeGenContext& context)
{
#ifdef MAIN
	std::cout << "codeGen NBoolean '" << value << "'" << endl;
#endif
	IECParam	*pret = new IECParam();
	pret->iec.t	= BOOLtype;
	pret->iec.p.BOOLvar	= value;
	return	pret;
}

void NIdentifier::undo() {
#ifdef MAIN
	std::cout << "undo NIdentifier '" << name << "'" << endl;
#endif
	delete this;
}
IECParam* NIdentifier::codeGen(CodeGenContext& context)
{
#ifdef MAIN
	std::cout << "codeGen NIdentifier '" << name << "'" << endl;
#endif
	IECParam *pret = new IECParam();
	cb_getVar (context.userctx, name.c_str(), &pret->iec);
	return	pret;
}

void NExpression::undo() {
#ifdef MAIN
	std::cout << "undo NExpression" << endl;
#endif
	delete this;
}

void NStatement::undo() {
#ifdef MAIN
	std::cout << "undo NStatement" << endl;
#endif
	delete this;
}

void NMethodCall::undo() {
#ifdef MAIN
	std::cout << "undo NMethodCall " << id.name << endl;
#endif
	ExpressionList::const_iterator it;
	for (it = arguments.begin(); it != arguments.end(); it++) {
		(**it).undo();
	}
	id.undo();
	delete this;
}
IECParam* NMethodCall::codeGen(CodeGenContext& context)
{
#ifdef MAIN
	std::cout << "codeGen NMethodCall " << id.name << endl;
#endif
	int	argc=0;
	IECParam	*argv[20];
	param_ret_t	*parg[20];
	IECParam *pret = NULL;

	ExpressionList::const_iterator it;
	for (it = arguments.begin(); (it != arguments.end()) && (argc < 20); it++,argc++) {
		argv[argc]	= (**it).codeGen(context);
		if	(!argv[argc])
			goto cleanup;
		parg[argc]	= &argv[argc]->iec;
	}

	pret = new IECParam();
	cb_callFunc (context.userctx, id.name.c_str(), argc, parg, &pret->iec);
cleanup:
	int i;
	for	(i=0; i<argc; i++)
		delete argv[i];
	return	pret;
}

BOOL timeFamily(int t) {
	return (t == TIMEtype || t == DATEtype || t == DTtype || t == TODtype);
}

void NBinaryOperator::undo() {
#ifdef MAIN
	std::cout << "undo NBinaryOperator " << op << endl;
#endif
	lhs.undo(); rhs.undo();
	delete this;
}
IECParam* NBinaryOperator::codeGen(CodeGenContext& context)
{
#ifdef MAIN
	std::cout << "codeGen NBinaryOperator " << op << endl;
#endif

	IECParam *left = lhs.codeGen(context);
	//std::cout << "Continuing binary operation " << op << endl;
	if	(!left) {
		std::cout << "Left part is null " << op << endl;
		return	NULL;
	}
	IECParam *right = rhs.codeGen(context);
	//std::cout << "Continuing binary operation " << op << endl;
	if	(!right) {
		std::cout << "Right part is null " << op << endl;
		delete left;
		return	NULL;
	}
	//std::cout << "Continuing binary operation " << op << endl;
	if	(left->iec.t != right->iec.t) {
		if (!timeFamily(left->iec.t) || !timeFamily(right->iec.t))
		{
			fprintf (stderr, "ST Error : types are differents %d %d\n", left->iec.t, right->iec.t);
			return	NULL;
		}
	}
	IECParam	*pret = new IECParam();

	pret->iec.t	= left->iec.t;

	switch	(pret->iec.t) {
	case	DINTtype :
		switch (op) {
		case TPLUS:	pret->iec.p.DINTvar	= left->iec.p.DINTvar + right->iec.p.DINTvar; break;
		case TMINUS:	pret->iec.p.DINTvar	= left->iec.p.DINTvar - right->iec.p.DINTvar; break;
		case TMUL:	pret->iec.p.DINTvar	= left->iec.p.DINTvar * right->iec.p.DINTvar; break;
		case TDIV:
/* XXX float
			if (right->iec.p.DINTvar)
				pret->iec.p.REALvar	= (float)left->iec.p.DINTvar / (float)right->iec.p.DINTvar;
			else
				pret->iec.p.REALvar	= 0;
			pret->iec.t	= REALtype;
*/
			if (right->iec.p.DINTvar)
				pret->iec.p.DINTvar	= left->iec.p.DINTvar / right->iec.p.DINTvar;
			else
				pret->iec.p.DINTvar	= 0;
			break;
		case TCEQ:
			pret->iec.p.BOOLvar	= (left->iec.p.DINTvar == right->iec.p.DINTvar);
			pret->iec.t	= BOOLtype;
			break;
		case TCNE:
			pret->iec.p.BOOLvar	= (left->iec.p.DINTvar != right->iec.p.DINTvar);
			pret->iec.t	= BOOLtype;
			break;
		case TCLT:
			pret->iec.p.BOOLvar	= (left->iec.p.DINTvar < right->iec.p.DINTvar);
			pret->iec.t	= BOOLtype;
			break;
		case TCLE:
			pret->iec.p.BOOLvar	= (left->iec.p.DINTvar <= right->iec.p.DINTvar);
			pret->iec.t	= BOOLtype;
			break;
		case TCGT:
			pret->iec.p.BOOLvar	= (left->iec.p.DINTvar > right->iec.p.DINTvar);
			pret->iec.t	= BOOLtype;
			break;
		case TCGE:
			pret->iec.p.BOOLvar	= (left->iec.p.DINTvar >= right->iec.p.DINTvar);
			pret->iec.t	= BOOLtype;
			break;
		}
		break;
	case	LREALtype :
		switch (op) {
		case TPLUS:	pret->iec.p.LREALvar	= left->iec.p.LREALvar + right->iec.p.LREALvar; break;
		case TMINUS:	pret->iec.p.LREALvar	= left->iec.p.LREALvar - right->iec.p.LREALvar; break;
		case TMUL:	pret->iec.p.LREALvar	= left->iec.p.LREALvar * right->iec.p.LREALvar; break;
		case TDIV:
			if (right->iec.p.LREALvar)
				pret->iec.p.LREALvar	= left->iec.p.LREALvar / right->iec.p.LREALvar;
			else
				pret->iec.p.LREALvar	= 0;
			break;
		case TCEQ:
			pret->iec.p.BOOLvar	= (left->iec.p.LREALvar == right->iec.p.LREALvar);
			pret->iec.t	= BOOLtype;
			break;
		case TCNE:
			pret->iec.p.BOOLvar	= (left->iec.p.LREALvar != right->iec.p.LREALvar);
			pret->iec.t	= BOOLtype;
			break;
		case TCLT:
			pret->iec.p.BOOLvar	= (left->iec.p.LREALvar < right->iec.p.LREALvar);
			pret->iec.t	= BOOLtype;
			break;
		case TCLE:
			pret->iec.p.BOOLvar	= (left->iec.p.LREALvar <= right->iec.p.LREALvar);
			pret->iec.t	= BOOLtype;
			break;
		case TCGT:
			pret->iec.p.BOOLvar	= (left->iec.p.LREALvar > right->iec.p.LREALvar);
			pret->iec.t	= BOOLtype;
			break;
		case TCGE:
			pret->iec.p.BOOLvar	= (left->iec.p.LREALvar >= right->iec.p.LREALvar);
			pret->iec.t	= BOOLtype;
			break;
		}
		break;
	case	REALtype :
		switch (op) {
		case TPLUS:	pret->iec.p.REALvar	= left->iec.p.REALvar + right->iec.p.REALvar; break;
		case TMINUS:	pret->iec.p.REALvar	= left->iec.p.REALvar - right->iec.p.REALvar; break;
		case TMUL:	pret->iec.p.REALvar	= left->iec.p.REALvar * right->iec.p.REALvar; break;
		case TDIV:
			if (right->iec.p.REALvar)
				pret->iec.p.REALvar	= left->iec.p.REALvar / right->iec.p.REALvar;
			else
				pret->iec.p.REALvar	= 0;
			break;
		case TCEQ:
			pret->iec.p.BOOLvar	= (left->iec.p.REALvar == right->iec.p.REALvar);
			pret->iec.t	= BOOLtype;
			break;
		case TCNE:
			pret->iec.p.BOOLvar	= (left->iec.p.REALvar != right->iec.p.REALvar);
			pret->iec.t	= BOOLtype;
			break;
		case TCLT:
			pret->iec.p.BOOLvar	= (left->iec.p.REALvar < right->iec.p.REALvar);
			pret->iec.t	= BOOLtype;
			break;
		case TCLE:
			pret->iec.p.BOOLvar	= (left->iec.p.REALvar <= right->iec.p.REALvar);
			pret->iec.t	= BOOLtype;
			break;
		case TCGT:
			pret->iec.p.BOOLvar	= (left->iec.p.REALvar > right->iec.p.REALvar);
			pret->iec.t	= BOOLtype;
			break;
		case TCGE:
			pret->iec.p.BOOLvar	= (left->iec.p.REALvar >= right->iec.p.REALvar);
			pret->iec.t	= BOOLtype;
			break;
		}
		break;
	case	BOOLtype :
		pret->iec.t	= BOOLtype;
		switch (op) {
		case TAND:	pret->iec.p.BOOLvar	= left->iec.p.BOOLvar & right->iec.p.BOOLvar; break;
		case TOR:	pret->iec.p.BOOLvar	= left->iec.p.BOOLvar | right->iec.p.BOOLvar; break;
		case TXOR:	pret->iec.p.BOOLvar	= left->iec.p.BOOLvar ^ right->iec.p.BOOLvar; break;
		case TCEQ: pret->iec.p.BOOLvar	= left->iec.p.BOOLvar == right->iec.p.BOOLvar; break;
		}
		break;
	case	TIMEtype :
		switch (op) {
		case TPLUS:
			ADD_TIMESPEC3(pret->iec.p.TIMEvar, left->iec.p.TIMEvar, right->iec.p.TIMEvar);
			break;
		case TMINUS:
			SUB_TIMESPEC3(pret->iec.p.TIMEvar, left->iec.p.TIMEvar, right->iec.p.TIMEvar);
			break;
		case TCEQ:
			pret->iec.p.BOOLvar = (CMP_TIMESPEC(left->iec.p.TIMEvar, right->iec.p.TIMEvar) == 0);
			pret->iec.t	= BOOLtype;
			break;
		case TCNE:
			pret->iec.p.BOOLvar = (CMP_TIMESPEC(left->iec.p.TIMEvar, right->iec.p.TIMEvar) != 0);
			pret->iec.t	= BOOLtype;
			break;
		case TCLT:
			pret->iec.p.BOOLvar = (CMP_TIMESPEC(left->iec.p.TIMEvar, right->iec.p.TIMEvar) < 0);
			pret->iec.t	= BOOLtype;
			break;
		case TCLE:
			pret->iec.p.BOOLvar = (CMP_TIMESPEC(left->iec.p.TIMEvar, right->iec.p.TIMEvar) <= 0);
			pret->iec.t	= BOOLtype;
			break;
		case TCGT:
			pret->iec.p.BOOLvar = (CMP_TIMESPEC(left->iec.p.TIMEvar, right->iec.p.TIMEvar) > 0);
			pret->iec.t	= BOOLtype;
			break;
		case TCGE:
			pret->iec.p.BOOLvar = (CMP_TIMESPEC(left->iec.p.TIMEvar, right->iec.p.TIMEvar) >= 0);
			pret->iec.t	= BOOLtype;
			break;
		}
		break;
	case	DATEtype :
		switch (op) {
		case TPLUS:
			ADD_TIMESPEC3(pret->iec.p.DATEvar, left->iec.p.DATEvar, right->iec.p.DATEvar);
			break;
		case TMINUS:
			SUB_TIMESPEC3(pret->iec.p.DATEvar, left->iec.p.DATEvar, right->iec.p.DATEvar);
			break;
		case TCEQ:
			pret->iec.p.BOOLvar = (CMP_TIMESPEC(left->iec.p.DATEvar, right->iec.p.DATEvar) == 0);
			pret->iec.t	= BOOLtype;
			break;
		case TCNE:
			pret->iec.p.BOOLvar = (CMP_TIMESPEC(left->iec.p.DATEvar, right->iec.p.DATEvar) != 0);
			pret->iec.t	= BOOLtype;
			break;
		case TCLT:
			pret->iec.p.BOOLvar = (CMP_TIMESPEC(left->iec.p.DATEvar, right->iec.p.DATEvar) < 0);
			pret->iec.t	= BOOLtype;
			break;
		case TCLE:
			pret->iec.p.BOOLvar = (CMP_TIMESPEC(left->iec.p.DATEvar, right->iec.p.DATEvar) <= 0);
			pret->iec.t	= BOOLtype;
			break;
		case TCGT:
			pret->iec.p.BOOLvar = (CMP_TIMESPEC(left->iec.p.DATEvar, right->iec.p.DATEvar) > 0);
			pret->iec.t	= BOOLtype;
			break;
		case TCGE:
			pret->iec.p.BOOLvar = (CMP_TIMESPEC(left->iec.p.DATEvar, right->iec.p.DATEvar) >= 0);
			pret->iec.t	= BOOLtype;
			break;
		}
		break;
	case	TODtype :
		switch (op) {
		case TPLUS:
			ADD_TIMESPEC3(pret->iec.p.TODvar, left->iec.p.TODvar, right->iec.p.TODvar);
			break;
		case TMINUS:
			SUB_TIMESPEC3(pret->iec.p.TODvar, left->iec.p.TODvar, right->iec.p.TODvar);
			break;
		case TCEQ:
			pret->iec.p.BOOLvar = (CMP_TIMESPEC(left->iec.p.TODvar, right->iec.p.TODvar) == 0);
			pret->iec.t	= BOOLtype;
			break;
		case TCNE:
			pret->iec.p.BOOLvar = (CMP_TIMESPEC(left->iec.p.TODvar, right->iec.p.TODvar) != 0);
			pret->iec.t	= BOOLtype;
			break;
		case TCLT:
			pret->iec.p.BOOLvar = (CMP_TIMESPEC(left->iec.p.TODvar, right->iec.p.TODvar) < 0);
			pret->iec.t	= BOOLtype;
			break;
		case TCLE:
			pret->iec.p.BOOLvar = (CMP_TIMESPEC(left->iec.p.TODvar, right->iec.p.TODvar) <= 0);
			pret->iec.t	= BOOLtype;
			break;
		case TCGT:
			pret->iec.p.BOOLvar = (CMP_TIMESPEC(left->iec.p.TODvar, right->iec.p.TODvar) > 0);
			pret->iec.t	= BOOLtype;
			break;
		case TCGE:
			pret->iec.p.BOOLvar = (CMP_TIMESPEC(left->iec.p.TODvar, right->iec.p.TODvar) >= 0);
			pret->iec.t	= BOOLtype;
			break;
		}
		break;
	case	DTtype :
		switch (op) {
		case TPLUS:
			ADD_TIMESPEC3(pret->iec.p.DTvar, left->iec.p.DTvar, right->iec.p.DTvar);
			break;
		case TMINUS:
			SUB_TIMESPEC3(pret->iec.p.DTvar, left->iec.p.DTvar, right->iec.p.DTvar);
			break;
		case TCEQ:
			pret->iec.p.BOOLvar = (CMP_TIMESPEC(left->iec.p.DTvar, right->iec.p.DTvar) == 0);
			pret->iec.t	= BOOLtype;
			break;
		case TCNE:
			pret->iec.p.BOOLvar = (CMP_TIMESPEC(left->iec.p.DTvar, right->iec.p.DTvar) != 0);
			pret->iec.t	= BOOLtype;
			break;
		case TCLT:
			pret->iec.p.BOOLvar = (CMP_TIMESPEC(left->iec.p.DTvar, right->iec.p.DTvar) < 0);
			pret->iec.t	= BOOLtype;
			break;
		case TCLE:
			pret->iec.p.BOOLvar = (CMP_TIMESPEC(left->iec.p.DTvar, right->iec.p.DTvar) <= 0);
			pret->iec.t	= BOOLtype;
			break;
		case TCGT:
			pret->iec.p.BOOLvar = (CMP_TIMESPEC(left->iec.p.DTvar, right->iec.p.DTvar) > 0);
			pret->iec.t	= BOOLtype;
			break;
		case TCGE:
			pret->iec.p.BOOLvar = (CMP_TIMESPEC(left->iec.p.DTvar, right->iec.p.DTvar) >= 0);
			pret->iec.t	= BOOLtype;
			break;
		}
		break;
	}
	delete left;
	delete right;
	//std::cout << "End binary operation " << op << endl;
	return	pret;
}

void NUnaryOperator::undo() {
#ifdef MAIN
	std::cout << "undo NUnaryOperator " << op << endl;
#endif
	rhs.undo();
	delete this;
}
IECParam* NUnaryOperator::codeGen(CodeGenContext& context)
{
#ifdef MAIN
	std::cout << "codeGen NUnaryOperator " << op << endl;
#endif
	IECParam *right = rhs.codeGen(context);
	if	(!right) {
		//std::cout << "Right part is null " << op << endl;
		return	NULL;
	}

	IECParam	*pret = new IECParam();
	pret->iec.t	= right->iec.t;

	switch	(pret->iec.t) {
	case	BOOLtype :
		switch (op) {
		case TNOT:	pret->iec.p.BOOLvar	= !right->iec.p.BOOLvar; break;
		}
		break;
	}
	delete right;
	return	pret;
}

void NAssignment::undo() {
#ifdef MAIN
	std::cout << "undo NAssignment" << endl;
#endif
	lhs.undo(); rhs.undo();
	delete this;
}
IECParam* NAssignment::codeGen(CodeGenContext& context)
{
#ifdef MAIN
	std::cout << "codeGen NAssignment " << lhs.name << endl;
#endif
	//lhs.codeGen(context);
	IECParam *right = rhs.codeGen(context);
	if	(right) {
/*
		int t = cb_getType (context.userctx, lhs.name.c_str());
		if	(t >= 0) {
			if	((t == REALtype) && right->iec.t == DINTtype) {
				//printf ("*** dynamic type change\n");
				right->iec.p.REALvar	= (float)right->iec.p.DINTvar;
				right->iec.t	= REALtype;
			}
			cb_setVar (context.userctx, lhs.name.c_str(), &right->iec);
		}
*/
		cb_setVar (context.userctx, lhs.name.c_str(), &right->iec);
		delete right;
	}
	return	NULL;
}

void NBlock::undo() {
#ifdef MAIN
	std::cout << "undo NBlock" << std::endl;
#endif
	StatementList::const_iterator it;
	for (it = statements.begin(); it != statements.end(); it++) {
		(**it).undo();
	}
	delete this;
}
IECParam* NBlock::codeGen(CodeGenContext& context)
{
#ifdef MAIN
	std::cout << "codeGen NBlock" << std::endl;
#endif
	IECParam *last = NULL;
	StatementList::const_iterator it;
	for (it = statements.begin(); it != statements.end(); it++) {
		if	(last)
			delete last;
		//std::cout << "codeGen " << typeid(**it).name() << std::endl;
		last = (**it).codeGen(context);
	}
	return	last;
}

void NExpressionStatement::undo() {
#ifdef MAIN
	std::cout << "undo ExpressionStatement" << endl;
#endif
	expression.undo();
	delete this;
}
IECParam* NExpressionStatement::codeGen(CodeGenContext& context)
{
#ifdef MAIN
	std::cout << "codeGen NExpressionStatement " << endl;
#endif
	return expression.codeGen(context);
}

void NVariableDeclaration::undo() { delete this; }
IECParam* NVariableDeclaration::codeGen(CodeGenContext& context)
{
	//std::cout << "Creating variable declaration " << type.name << " " << id.name << endl;
	return	NULL;
}

void NFunctionDeclaration::undo() { delete this; }
IECParam* NFunctionDeclaration::codeGen(CodeGenContext& context)
{
	//std::cout << "Creating function: " << id.name << endl;
	return	NULL;
}

extern "C" TIME getDuration(char *str);

void NTime::undo() { delete this; }
IECParam* NTime::codeGen(CodeGenContext& context)
{
	//std::cout << "Creating IEC_TIME: " << value << endl;
	IECParam	*pret = new IECParam();
	pret->iec.t	= TIMEtype;
	pret->iec.p.TIMEvar	= getDuration((char *)value.c_str());
	return	pret;
}

extern "C" DATE getDate(char *str);

void NDate::undo() { delete this; }
IECParam* NDate::codeGen(CodeGenContext& context)
{
	//std::cout << "Creating IEC_DATE: " << value << endl;
	IECParam	*pret = new IECParam();
	pret->iec.t	= DATEtype;
	pret->iec.p.DATEvar	= getDate((char *)value.c_str());
	return	pret;
}

extern "C" TOD getTimeOfDay(char *str);

void NTod::undo() { delete this; }
IECParam* NTod::codeGen(CodeGenContext& context)
{
	//std::cout << "Creating IEC_TOD: " << value << endl;
	IECParam	*pret = new IECParam();
	pret->iec.t	= TODtype;
	pret->iec.p.TODvar	= getTimeOfDay((char *)value.c_str());
	return	pret;
}

extern "C" DT getDateAndTime(char *str);

void NDateTime::undo() { delete this; }
IECParam* NDateTime::codeGen(CodeGenContext& context)
{
	//std::cout << "Creating IEC_DT: " << value << endl;
	IECParam	*pret = new IECParam();
	pret->iec.t	= DTtype;
	pret->iec.p.DTvar	= getDateAndTime((char *)value.c_str());
	return	pret;
}

extern "C" void setReturnParameter (param_ret_t *pret, void *value, int len, char type);

void NString::undo() { delete this; }
IECParam* NString::codeGen(CodeGenContext& context)
{
	//std::cout << "Creating IEC_STRING: " << value << endl;
	IECParam	*pret = new IECParam();

	//	Removing quotes
	char *pt = strdup((char *)value.c_str());
	pt[strlen(pt)-1] = 0;
	setReturnParameter (&pret->iec, pt+1, 0, STRINGtype);
	free (pt);
	return	pret;
}

void NElsif::undo() {
#ifdef MAIN
	std::cout << "undo NElsif" << endl;
#endif
	expr.undo();
	block.undo();
	delete this;
}
IECParam* NElsif::codeGen(CodeGenContext& context)
{
#ifdef MAIN
	std::cout << "codeGen NElsif" << endl;
#endif
	IECParam	*pret = expr.codeGen(context);
	if	(pret->iec.t != BOOLtype)
		return	NULL;

	if	(pret->iec.p.BOOLvar == TRUE)
		block.codeGen (context);
	return	pret;
}

void NIf::undo() {
#ifdef MAIN
	std::cout << "undo If" << endl;
#endif
	expr.undo();
	block.undo();
	if	(elsiflist) {
		ElsifList::const_iterator it;
		for (it = elsiflist->begin(); it != elsiflist->end(); it++) {
			(**it).undo();
		}
	}
	if	(elseblock)
		elseblock->undo();
	delete this;
}
IECParam* NIf::codeGen(CodeGenContext& context)
{
#ifdef MAIN
	std::cout << "codeGen If" << endl;
#endif

	IECParam	*pret = expr.codeGen(context);
	if	(pret->iec.t != BOOLtype) {
		delete pret;
		return	NULL;
	}

	if	(pret->iec.p.BOOLvar == TRUE) {
		block.codeGen (context);
		delete pret;
		return	NULL;
	}

	delete pret;

	if	(elsiflist) {
		ElsifList::const_iterator it;
//printf ("*** elsiflist %d\n", elsiflist->size());
		for (it = elsiflist->begin(); it != elsiflist->end(); it++) {
			pret = (**it).codeGen(context);
			if	(pret && (pret->iec.p.BOOLvar == TRUE)) {
				delete pret;
				return	NULL;
			}
			delete pret;
		}
	}

	if	(elseblock) {
//printf (" *** ELSE\n");
		elseblock->codeGen (context);
		return	NULL;
	}
	return	NULL;
}
