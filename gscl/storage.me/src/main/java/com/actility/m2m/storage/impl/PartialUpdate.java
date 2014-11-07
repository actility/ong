package com.actility.m2m.storage.impl;

import java.util.List;
import java.util.Map;

import com.actility.m2m.storage.Condition;
import com.actility.m2m.storage.Document;

public class PartialUpdate extends Modification {

    private final byte[] content;
    private final List attrOps;

    public PartialUpdate(Map configuration, Document document, byte[] content, List attrOps, Condition condition) {
        super(configuration, document, condition);
        this.content = content;
        this.attrOps = attrOps;
    }

    public byte[] getContent() {
        return content;
    }

    public List getAttrOps() {
        return attrOps;
    }
}
