/*******************************************************************************
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
 *******************************************************************************/
package com.actility.m2m.storage.impl;

import java.util.List;

import com.actility.m2m.storage.Condition;

public class ConditionImpl implements Condition {

    private final String attributeName;
    private final int operator;
    private final Object operand;
    private final List/* <Condition> */conditions;
    private final boolean exists;

    public ConditionImpl(String attributeName, int operator, Object operand, boolean exists) {
        this.attributeName = attributeName;
        this.operator = operator;
        this.operand = operand;
        this.conditions = null;
        this.exists = exists;
    }

    public ConditionImpl(int operator, List/* <Condition> */conditions) {
        this.attributeName = null;
        this.operator = operator;
        this.operand = null;
        this.conditions = conditions;
        this.exists = false;
    }

    public String getAttributeName() {
        return attributeName;
    }

    public int getOperator() {
        return operator;
    }

    public Object getOperand() {
        return operand;
    }

    public List/*<Condition>*/ getConditions() {
        return conditions;
    }

    public boolean isExists() {
        return exists;
    }
}
