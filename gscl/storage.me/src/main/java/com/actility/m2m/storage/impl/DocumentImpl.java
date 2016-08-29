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
import java.util.HashMap;
import java.util.Iterator;
import java.util.List;
import java.util.Map;

import com.actility.m2m.storage.Document;

public final class DocumentImpl implements Document {

    private final String id;
    private final String path;
    private final Map/* <String, Object> */ attributes;
    private byte[] content;

    public DocumentImpl(String id, String path) {
        this.id = id;
        this.path = path;
        this.attributes = new HashMap/* <String, Object> */();
    }

    public String getId() {
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
