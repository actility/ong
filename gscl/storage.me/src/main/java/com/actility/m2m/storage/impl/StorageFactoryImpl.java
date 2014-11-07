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

    public Condition createConjunction(int operation, List/* <Condition> */conditions) {
        return new ConditionImpl(operation, conditions);
    }

    public Document createDocument(String path) {
        return new DocumentImpl(path);
    }

    public AttributeOperation createAttributeOperation(int type, String name, Object value) {
        return new AttributeOperationImpl(type, name, value);
    }

    public Batch createBatch() {
        return new BatchImpl(driverManager);
    }

}
