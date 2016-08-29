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

import java.util.Date;
import java.util.List;

import com.actility.m2m.storage.AttributeOperation;
import com.actility.m2m.storage.Batch;
import com.actility.m2m.storage.Condition;
import com.actility.m2m.storage.Document;
import com.actility.m2m.storage.StorageFactory;

public final class StorageFactoryImpl implements StorageFactory {

    private final DriverManager driverManager;

    public StorageFactoryImpl(DriverManager driverManager) {
        this.driverManager = driverManager;
    }

    public Condition createStringCondition(String attributeName, int operator, String operandValue) {
        return new ConditionImpl(attributeName, operator, operandValue, false);
    }

    public Condition createDateCondition(String attributeName, int operator, Date operandValue) {
        return new ConditionImpl(attributeName, operator, operandValue, false);
    }

    public Condition createIntegerCondition(String attributeName, int operator, Integer operandValue) {
        return new ConditionImpl(attributeName, operator, operandValue, false);
    }

    public Condition createStringExistsCondition(String attributeName, int operator, String operandValue) {
        return new ConditionImpl(attributeName, operator, operandValue, true);
    }

    public Condition createDateExistsCondition(String attributeName, int operator, Date operandValue) {
        return new ConditionImpl(attributeName, operator, operandValue, true);
    }

    public Condition createIntegerExistsCondition(String attributeName, int operator, Integer operandValue) {
        return new ConditionImpl(attributeName, operator, operandValue, true);
    }

    public Condition createConjunction(int operation, List/* <Condition> */ conditions) {
        return new ConditionImpl(operation, conditions);
    }

    public Document createDocument(String path) {
        return new DocumentImpl(null, path);
    }

    public Document createDocumentFromId(String id) {
        return new DocumentImpl(id, null);
    }

    public AttributeOperation createAttributeOperation(int type, String name, Object value) {
        return new AttributeOperationImpl(type, name, value);
    }

    public Batch createBatch() {
        return new BatchImpl(driverManager);
    }

}
