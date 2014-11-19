package com.actility.m2m.storage.driver.sqlite.impl;

import java.util.List;

import com.actility.m2m.storage.Condition;

public class ConditionImpl implements Condition {

    private final String attributeName;
    private final int operator;
    private final Object operand;
    private final List/*<Condition>*/ conditions;
    private final boolean exist;
    public ConditionImpl(String attributeName, int operator, Object operand, boolean exist) {
        this.attributeName = attributeName;
        this.operator = operator;
        this.operand = operand;
        this.conditions = null;
        this.exist = exist;
    }
    public ConditionImpl(String attributeName, int operator, Object operand) {
        this.attributeName = attributeName;
        this.operator = operator;
        this.operand = operand;
        this.conditions = null;
        this.exist = false;
    }
    public ConditionImpl(int operator, List/*<Condition>*/ conditions, boolean exist) {
        this.attributeName = null;
        this.operator = operator;
        this.operand = null;
        this.conditions = conditions;
        this.exist = exist;
    }
    public ConditionImpl(int operator, List/*<Condition>*/ conditions) {
        this.attributeName = null;
        this.operator = operator;
        this.operand = null;
        this.conditions = conditions;
        this.exist = false;
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
        return exist;
    }

}
