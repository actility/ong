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

#ifndef _XPATH_H_
#define _XPATH_H_

typedef struct {
	struct list_head head;
	char *targetID;
	char *xpath_expr;
	char *iec_input_var;
	char *value;
	char *subscription_ref;
	time_t nextSubscription;
	char *msg;
	int tid;
} iec_xpath_t;

void User_SetXpathVar(iec_user_t *user, char *iecVariable, char *obix_type, char *obix_val, IEC_DT lastModifiedTime);
char *XpathExpression(mxml_node_t *top, char *expr);
void XpathAdd (iec_user_t *user, char *targetID, char *xpath_expr, char *iec_input_var, char *value);
void XpathSetReference(int tid, char *ref, char *msg);
void XpathUnsetReference(char *ref);
iec_xpath_t *XpathGetWithInputVar (iec_user_t *user, char *input_var);
void XpathAnalyzeInput(iec_user_t *user, char *uri_str, mxml_node_t *node, IEC_DT lastModifiedTime);


#endif // _XPATH_H_
