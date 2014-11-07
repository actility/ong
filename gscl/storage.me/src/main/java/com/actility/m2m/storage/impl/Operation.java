package com.actility.m2m.storage.impl;

import java.util.Map;

import com.actility.m2m.storage.Condition;
import com.actility.m2m.storage.Document;

public final class Operation extends Modification {
    public static final int TYPE_CREATE = 0;
    public static final int TYPE_UPDATE = 1;
    public static final int TYPE_DELETE = 2;

    private final int type;

    public Operation(int type, Map configuration, Document document, Condition condition) {
        super(configuration, document, condition);
        this.type = type;
    }

    public int getType() {
        return this.type;
    }
}
