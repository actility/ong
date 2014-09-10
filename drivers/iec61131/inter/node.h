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

#ifndef _NODE_H
#define _NODE_H

#include <iostream>
#include <vector>
#include <typeinfo>

#include <param_u.h>

class CodeGenContext;
class NStatement;
class NExpression;
class NVariableDeclaration;
class NElsif;

typedef std::vector<NStatement*> StatementList;
typedef std::vector<NExpression*> ExpressionList;
typedef std::vector<NVariableDeclaration*> VariableList;
typedef std::vector<NElsif*> ElsifList;

class IECParam {
public:
	param_ret_t	iec;
};

class Node {
public:
	virtual IECParam* codeGen(CodeGenContext& context) { }
	virtual void undo() { }
};

class NExpression : public Node {
public:
	virtual void undo();
};

class NStatement : public Node {
public:
	virtual void undo();
};

class NInteger : public NExpression {
public:
	std::string value;
	NInteger(const std::string& value) : value(value) { }
	virtual IECParam* codeGen(CodeGenContext& context);
	virtual void undo();
};

class NFloat : public NExpression {
public:
	std::string value;
	NFloat(const std::string& value) : value(value) { }
	virtual IECParam* codeGen(CodeGenContext& context);
	virtual void undo();
};

class NDouble : public NExpression {
public:
	std::string value;
	NDouble(const std::string& value) : value(value) { }
	virtual IECParam* codeGen(CodeGenContext& context);
	virtual void undo();
};

class NBoolean : public NExpression {
public:
	IEC_BOOL value;
	NBoolean(BOOL value) : value(value) { }
	virtual IECParam* codeGen(CodeGenContext& context);
	virtual void undo();
};

class NIdentifier : public NExpression {
public:
	std::string name;
	NIdentifier(const std::string& name) : name(name) { }
	virtual IECParam* codeGen(CodeGenContext& context);
	virtual void undo();
};

class NMethodCall : public NExpression {
public:
	NIdentifier& id;
	ExpressionList arguments;
	NMethodCall(NIdentifier& id, ExpressionList& arguments) :
		id(id), arguments(arguments) { }
	NMethodCall(NIdentifier& id) : id(id) { }
	virtual IECParam* codeGen(CodeGenContext& context);
	virtual void undo();
};

class NBinaryOperator : public NExpression {
public:
	int op;
	NExpression& lhs;
	NExpression& rhs;
	NBinaryOperator(NExpression& lhs, int op, NExpression& rhs) :
		lhs(lhs), rhs(rhs), op(op) { }
	virtual IECParam* codeGen(CodeGenContext& context);
	virtual void undo();
};

class NUnaryOperator : public NExpression {
public:
	int op;
	NExpression& rhs;
	NUnaryOperator(int op, NExpression& rhs) :
		rhs(rhs), op(op) { }
	virtual IECParam* codeGen(CodeGenContext& context);
	virtual void undo();
};

class NAssignment : public NExpression {
public:
	NIdentifier& lhs;
	NExpression& rhs;
	NAssignment(NIdentifier& lhs, NExpression& rhs) : 
		lhs(lhs), rhs(rhs) { }
	virtual IECParam* codeGen(CodeGenContext& context);
	virtual void undo();
};

class NBlock : public NExpression {
public:
	StatementList statements;
	NBlock() { }
	virtual IECParam* codeGen(CodeGenContext& context);
	virtual void undo();
};

class NExpressionStatement : public NStatement {
public:
	NExpression& expression;
	NExpressionStatement(NExpression& expression) : 
		expression(expression) { }
	virtual IECParam* codeGen(CodeGenContext& context);
	virtual void undo();
};

class NVariableDeclaration : public NStatement {
public:
	NIdentifier& type;
	NIdentifier& id;
	NExpression *assignmentExpr;
	NVariableDeclaration(NIdentifier& type, NIdentifier& id) :
		type(type), id(id) { }
	NVariableDeclaration(NIdentifier& type, NIdentifier& id, NExpression *assignmentExpr) :
		type(type), id(id), assignmentExpr(assignmentExpr) { }
	virtual IECParam* codeGen(CodeGenContext& context);
	virtual void undo();
};

class NFunctionDeclaration : public NStatement {
public:
	NIdentifier& type;
	NIdentifier& id;
	VariableList arguments;
	NBlock& block;
	NFunctionDeclaration(NIdentifier& type, NIdentifier& id, 
			VariableList& arguments, NBlock& block) :
		type(type), id(id), arguments(arguments), block(block) { }
	virtual IECParam* codeGen(CodeGenContext& context);
	virtual void undo();
};

class NTime : public NExpression {
public:
	std::string value;
	NTime(const std::string& value) : value(value) { }
	virtual IECParam* codeGen(CodeGenContext& context);
	virtual void undo();
};

class NDate : public NExpression {
public:
	std::string value;
	NDate(const std::string& value) : value(value) { }
	virtual IECParam* codeGen(CodeGenContext& context);
	virtual void undo();
};

class NTod : public NExpression {
public:
	std::string value;
	NTod(const std::string& value) : value(value) { }
	virtual IECParam* codeGen(CodeGenContext& context);
	virtual void undo();
};

class NDateTime : public NExpression {
public:
	std::string value;
	NDateTime(const std::string& value) : value(value) { }
	virtual IECParam* codeGen(CodeGenContext& context);
	virtual void undo();
};

class NString : public NExpression {
public:
	std::string value;
	NString(const std::string& value) : value(value) { }
	virtual IECParam* codeGen(CodeGenContext& context);
	virtual void undo();
};

class NElsif : public NStatement {
public:
	NExpression& expr;
	NBlock& block;
	NElsif (NExpression& expr, NBlock& block) : expr(expr), block(block) { }
	virtual IECParam* codeGen(CodeGenContext& context);
	virtual void undo();
};

class NIf : public NStatement {
public:
	NExpression& expr;
	NBlock& block;
	NBlock *elseblock;
	ElsifList *elsiflist;
	NIf (NExpression& expr, NBlock& block) : expr(expr), block(block) { elseblock=NULL; elsiflist=NULL; }
	NIf (NExpression& expr, NBlock& block, NBlock *elseblock) : expr(expr), block(block), elseblock(elseblock) { elsiflist=NULL; }
	NIf (NExpression& expr, NBlock& block, NBlock *elseblock, ElsifList *elsiflist) : expr(expr), block(block), elseblock(elseblock), elsiflist(elsiflist) { }
	virtual IECParam* codeGen(CodeGenContext& context);
	virtual void undo();
};

#endif /*_NODE_H*/
