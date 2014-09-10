/*
 * Copyright   Actility, SA. All Rights Reserved.
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
 *
 * id $Id: ConditionImpl.java 8036 2014-03-07 17:34:56Z JReich $
 * author $Author: JReich $
 * version $Revision: 8036 $
 * lastrevision $Date: 2014-03-07 18:34:56 +0100 (Fri, 07 Mar 2014) $
 * modifiedby $LastChangedBy: JReich $
 * lastmodified $LastChangedDate: 2014-03-07 18:34:56 +0100 (Fri, 07 Mar 2014) $
 */

package com.actility.m2m.storage.impl;

import com.actility.m2m.storage.Condition;

public class ConditionImpl implements Condition {

    private String attributeName;
    private int type;
    private int operator;
    private Object operand;
    private Condition condition1;
    private Condition condition2;

    public ConditionImpl(String attributeName, int type, int operator, Object operand) {
        super();
        this.attributeName = attributeName;
        this.type = type;
        this.operator = operator;
        this.operand = operand;
    }

    public ConditionImpl(String attributeName, int operator, Object operand) {
        super();
        this.attributeName = attributeName;
        this.operator = operator;
        this.operand = operand;
    }

    public ConditionImpl(Condition condition1, int operator, Condition condition2) {
        super();
        this.condition1 = condition1;
        this.operator = operator;
        this.condition2 = condition2;
    }

    public ConditionImpl() {
        // empty
    }

    public String getAttributeName() {
        return attributeName;
    }

    public void setAttributeName(String attributeName) {
        this.attributeName = attributeName;
    }

    public int getType() {
        return type;
    }

    public void setType(int type) {
        this.type = type;
    }

    public int getOperator() {
        return operator;
    }

    public void setOperator(int operator) {
        this.operator = operator;
    }

    public Object getOperand() {
        return operand;
    }

    public void setOperand(Object operand) {
        this.operand = operand;
    }

    public Condition getCondition1() {
        return condition1;
    }

    public void setCondition1(Condition condition1) {
        this.condition1 = condition1;
    }

    public Condition getCondition2() {
        return condition2;
    }

    public void setCondition2(Condition condition2) {
        this.condition2 = condition2;
    }

}
