package com.actility.m2m.storage.driver.sqlite.impl;

import com.actility.m2m.storage.AttributeOperation;

public class AttributeOperationImpl implements AttributeOperation {
    private int type = 0;
    private String name = null;
    private Object value = null;
    public AttributeOperationImpl(int type, String name, Object value){
        this.type = type;
        this.name = name;
        this.value = value;
    }
    public AttributeOperationImpl(String name, int type, Object value){
        this.type = type;
        this.name = name;
        this.value = value;
    }
    public int getType() {
        // TODO Auto-generated method stub
        return type;
    }

    public String getName() {
        // TODO Auto-generated method stub
        return name;
    }

    public Object getValue() {
        // TODO Auto-generated method stub
        return value;
    }

}
