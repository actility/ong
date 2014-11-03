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
