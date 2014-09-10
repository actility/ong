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
#include <strings.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <poll.h>
#include <math.h>

#include <mxml.h>
#include <rtlbase.h>
#include <rtllist.h>
#include <rtlhtbl.h>
#include <dIa.h>

#include "iec_debug.h"
#include "iec_config.h"
#include "plc.h"
#include "literals.h"
#include "timeoper.h"
#include "iec_std_lib.h"
#include "vasprintf.h"
#include "tools.h"
#include "operating.h"
#include "xpath.h"
#include "cli.h"


extern void setVariable (iec_variable_t *var, void *value);
char *getContainedText(mxml_node_t *node);
int obix_to_iec_type(char *obix_type);

extern char *typeString[];
extern iec_global_t GLOB;
extern char *M2M_applications;
extern char *M2M_subscriptions;

void User_SetXpathVar(iec_user_t *user, char *iecVariable, char *obix_type, char *obix_val, IEC_DT lastModifiedTime) {
	RTL_TRDBG (TRACE_DETAIL, "User_SetXpathVar type=%s val=%s iecVariable=%s\n", obix_type, obix_val, iecVariable);
	iec_variable_t *var = rtl_htblGet(user->m_accessVars, iecVariable);
	if	(!var || !obix_val)
		return;

	//	Type match
	if	(obix_type) {
		int iec_type = obix_to_iec_type(obix_type);
		if	((iec_type < 0) || (iec_type != var->type)) {
			RTL_TRDBG (TRACE_ERROR, "obix type '%s' and iec variable %s of type '%s' don't match\n",
				obix_type, iecVariable, typeString[(int)var->type]);
			User_GSC_log(user, "OBIX_TYPE_MISMATCH", "The obix type received in a notify doesn't match the IEC variable type",
				"obix type '%s' and iec variable %s of type '%s' don't match\n",
				obix_type, iecVariable, typeString[(int)var->type]);
			return;
		}
	}
	setVariable (var, obix_val);
	var->lastModifiedTime	= lastModifiedTime;
	CLI_valueHasChanged(var);

	RTL_TRDBG (TRACE_DETAIL, "SUCCESS M2M variable %s=%s\n", iecVariable, obix_val);
}

#if 0
char *XpathExpression(mxml_node_t *top, char *expr) {
	char *res = NULL;
	char *attr = NULL;
	char *val = NULL;

	if	(*expr == '/')
		expr ++;
	char *pt = strchr(expr, '/');
	if	(pt)
		*pt = 0;
	else {
		pt = strstr(expr, "[@");
		if	(pt) {
			*pt = 0;
			attr = pt+2;
			val = strchr(attr, '=');
			pt = strchr(attr, ']');
			if	(!pt)
				return NULL;
			*pt = 0;
			pt = NULL;
			if (val) {
				*val++ = 0;
				if	(*val == '\'')
					val ++;
				if	(val[strlen(val)-1] == '\'')
					val[strlen(val)-1] = 0;
			}
		//printf ("attr (%s) val (%s)\n", attr, val);
		}
	}
	//printf ("expr=%s attr=%s val=%s\n", expr, expr, val);

	mxml_node_t *node = mxmlFindElement(top, top, expr, attr, val, MXML_DESCEND);
	if	(node) {
		if	(pt) {
			res = XpathExpression (node, pt+1);
		}
		else if (attr) {
			res = (char *)mxmlElementGetAttr(node, attr);
		}
		else {
			res = getContainedText(node);
			//printf ("getContainedText = (%s)\n", res);
		}
	}
	else {
		//printf ("### FAILED mxmlFindElement(%s)\n", expr);
	}
	return res;
}
#endif

#include "../xpath/xpathExpr.c"

void XpathAdd (iec_user_t *user, char *targetID, char *xpath_expr, char *iec_input_var, char *value) {
	RTL_TRDBG (TRACE_DETAIL, "XpathAdd targetID=%s xpath=%s iec_input_var=%s value=%s\n",
		targetID, xpath_expr, iec_input_var, value);
	iec_xpath_t *xpath = Calloc(sizeof(iec_xpath_t), 1);
	xpath->targetID	= Strdup(targetID);
	xpath->xpath_expr	= Strdup(xpath_expr);
	xpath->iec_input_var	= Strdup(iec_input_var);
	xpath->value	= Strdup(value);
	xpath->subscription_ref = NULL;
	xpath->msg = NULL;
	xpath->nextSubscription = 0;
	list_add_tail (&xpath->head, &user->xpath_list);
}

void XpathSetReference(int tid, char *ref, char *msg) {
	iec_user_t *user;
	struct list_head *elem;
	list_for_each (elem, &GLOB.user_list) {
		user	= list_entry(elem, iec_user_t, head);

		struct list_head *elem2;
		list_for_each (elem2, &user->xpath_list) {
			if	(!elem2)
				break;
			iec_xpath_t *elt = list_entry(elem2, iec_xpath_t, head);
			if	(elt->tid == tid) {
				RTL_TRDBG (TRACE_DETAIL, "XpathSetReference tid=%d ref=%s %s\n", tid, ref, elt->targetID);
				Free (elt->subscription_ref);
				if	(ref)
					elt->subscription_ref = Strdup(ref);
				else {
					elt->nextSubscription = 0;
					elt->subscription_ref = NULL;
				}
				Free (elt->msg);
				elt->msg	= Strdup(msg);
				return;
			}
		}
	}
	RTL_TRDBG (TRACE_DETAIL, "XpathSetReference failed tid=%d ref=%s\n", tid, ref);
}

void XpathUnsetReference(char *ref) {
	iec_user_t *user;
	struct list_head *elem;
	RTL_TRDBG (TRACE_DETAIL, "XpathUnsetReference ref=%s\n", ref);
	list_for_each (elem, &GLOB.user_list) {
		user	= list_entry(elem, iec_user_t, head);

		struct list_head *elem2;
		list_for_each (elem2, &user->xpath_list) {
			if	(!elem2)
				break;
			iec_xpath_t *elt = list_entry(elem2, iec_xpath_t, head);
			if	(elt->subscription_ref && !strcmp(ref, elt->subscription_ref)) {
				RTL_TRDBG (TRACE_DETAIL, "XpathUnsetReference ref=%s\n", ref);
				Free (elt->subscription_ref);
				elt->nextSubscription = 0;
				return;
			}
		}
	}
}


iec_xpath_t *XpathGetWithInputVar (iec_user_t *user, char *input_var) {
	struct list_head *elem;
	//RTL_TRDBG (TRACE_DETAIL, "XpathGetWithInputVar input_var=%s\n", input_var);
	list_for_each (elem, &user->xpath_list) {
		if	(!elem)
			break;
		iec_xpath_t *elt = list_entry(elem, iec_xpath_t, head);
		if	(elt && !strcmp(input_var, elt->iec_input_var))
			return elt;
	}
	return NULL;
}

void XpathAnalyzeInput(iec_user_t *user, char *uri_str, mxml_node_t *node, IEC_DT lastModifiedTime) {
	struct list_head *elem;

	uri_str	= Strdup(uri_str);

	char *subscription_ref = strstr(uri_str, M2M_subscriptions);
	char *path = strstr(uri_str, M2M_applications);
	if	(!subscription_ref)
		goto end;
	*subscription_ref = 0;
	subscription_ref += strlen(M2M_subscriptions)+1;

	RTL_TRDBG (TRACE_DETAIL, "XpathAnalyzeInput path=%s subscription_ref=%s\n", path, subscription_ref);

	list_for_each (elem, &user->xpath_list) {
		if	(!elem)
			break;
		iec_xpath_t *elt = list_entry(elem, iec_xpath_t, head);

		RTL_TRDBG (TRACE_DETAIL, "  XpathAnalyzeInput elt->subscription_ref=%s targetID=%s\n", elt->subscription_ref, elt->targetID);
		if	(!elt->subscription_ref || strcmp(subscription_ref, elt->subscription_ref)) {
			if	(strcmp(path, elt->targetID)) {
				continue;
			}
		}

		if	(!elt->xpath_expr) {
			char *obix_type = node->value.element.name;
			char *obix_val = (char *)mxmlElementGetAttr(node, "val");

			RTL_TRDBG (TRACE_DETAIL, "XpathAnalyzeInput OK case 1 : iec_input_var=%s type=%s val=%s\n",
				elt->iec_input_var, obix_type, obix_val);
			User_SetXpathVar (user, elt->iec_input_var, obix_type, obix_val, lastModifiedTime);
			goto end;
		}

		char *res = XpathExpression(node, elt->xpath_expr);
		if	(res) {
			RTL_TRDBG (TRACE_DETAIL, "XpathAnalyzeInput OK case 2 : iec_input_var=%s res=%s val=%s\n",
				elt->iec_input_var, res, elt->value);
			User_SetXpathVar (user, elt->iec_input_var, NULL, elt->value ? elt->value : res, lastModifiedTime);
			goto end;
		}
	}
end:
	Free (uri_str);
}

