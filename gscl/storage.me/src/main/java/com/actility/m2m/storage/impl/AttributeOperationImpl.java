package com.actility.m2m.storage.impl;

import com.actility.m2m.storage.AttributeOperation;

public final class AttributeOperationImpl implements AttributeOperation {

    private final int type;
    private final String name;
    private final Object value;

    public AttributeOperationImpl(int type, String name, Object value) {
        this.type = type;
        this.name = name;
        this.value = value;
    }

    public int getType() {
        return type;
    }

    public String getName() {
        return name;
    }

    public Object getValue() {
        return value;
    }

}
