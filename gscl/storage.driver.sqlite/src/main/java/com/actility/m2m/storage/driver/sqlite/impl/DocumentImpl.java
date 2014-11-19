package com.actility.m2m.storage.driver.sqlite.impl;

import java.util.Date;
import java.util.HashMap;
import java.util.Iterator;
import java.util.List;
import java.util.Map;

import com.actility.m2m.storage.Document;

public final class DocumentImpl implements Document {
    private final String path;
    private final Object id;
    private final Map/* <String, Object> */attributes;
    private byte[] content;

    /**
     * Class constructor.
     *
     * @param id the document
     * @param path the document path
     */
    public DocumentImpl(Object id, String path) {
        this.id = id;
        this.path = path;
        this.attributes = new HashMap/* <String, Object> */();
        this.content = null;
    }

    public Object getId() {
        return id;
    }

    public String getPath() {
        return path;
    }

    public byte[] getContent() {
        return content;
    }

    public void setContent(byte[] content) {
        this.content = content;
    }

    public Object getAttribute(String name) {
        return attributes.get(name);
    }

    public Integer getIntegerAttribute(String name) {
        return (Integer) attributes.get(name);
    }

    public Date getDateAttribute(String name) {
        return (Date) attributes.get(name);
    }

    public String getStringAttribute(String name) {
        return (String) attributes.get(name);
    }

    public List getListAttribute(String name) {
        return (List) attributes.get(name);
    }

    public void setAttribute(String name, Object value) {
        attributes.put(name, value);
    }

    public Object removeAttribute(String name) {
        return attributes.remove(name);
    }

    public Iterator getAttributes() {
        return attributes.entrySet().iterator();
    }
}
