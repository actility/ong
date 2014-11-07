package com.actility.m2m.storage.impl;

import java.util.Map;

import com.actility.m2m.storage.Condition;
import com.actility.m2m.storage.Document;

public class Modification {
    private final Map configuration;
    private final Document document;
    private final Condition condition;

    public Modification(Map configuration, Document document, Condition condition) {
        this.configuration = configuration;
        this.document = document;
        this.condition = condition;
    }

    public Map getConfiguration() {
        return configuration;
    }

    public Document getDocument() {
        return document;
    }

    public Condition getCondition() {
        return condition;
    }

}
