/*
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
 *
 * id $Id: ModificationImpl.java 8036 2014-03-07 17:34:56Z JReich $
 * author $Author: JReich $
 * version $Revision: 8036 $
 * lastrevision $Date: 2014-03-07 18:34:56 +0100 (Fri, 07 Mar 2014) $
 * modifiedby $LastChangedBy: JReich $
 * lastmodified $LastChangedDate: 2014-03-07 18:34:56 +0100 (Fri, 07 Mar 2014) $
 */

package com.actility.m2m.storage.impl;

import java.util.Collection;
import java.util.Map;

import com.actility.m2m.storage.Modification;

public class ModificationImpl implements Modification {

    private int type;
    private String fullPath;
    private byte[] rawDocument;
    private Collection searchAttributes;
    private Map configuration;
    private int scope;

    public ModificationImpl(int type, String fullPath, byte[] rawDocument, Collection searchAttributes,
            Map configuration) {
        super();
        this.type = type;
        this.fullPath = fullPath;
        this.rawDocument = rawDocument;
        this.searchAttributes = searchAttributes;
        this.configuration = configuration;
        this.scope = -1;
    }

    public ModificationImpl(int type, String fullPath, byte[] rawDocument, Collection searchAttributes) {
        super();
        this.type = type;
        this.fullPath = fullPath;
        this.rawDocument = rawDocument;
        this.searchAttributes = searchAttributes;
        this.scope = -1;
    }

    public ModificationImpl(int type, String fullPath, byte[] rawDocument, Collection searchAttributes,
    		Map configuration, int scope) {
        super();
        this.type = type;
        this.fullPath = fullPath;
        this.rawDocument = rawDocument;
        this.searchAttributes = searchAttributes;
        this.configuration = configuration;
        this.scope = scope;
    }

    public ModificationImpl(int type, String fullPath, byte[] rawDocument, Collection searchAttributes,
            int scope) {
        super();
        this.type = type;
        this.fullPath = fullPath;
        this.rawDocument = rawDocument;
        this.searchAttributes = searchAttributes;
        this.scope = scope;
    }

    public int getType() {
        return this.type;
    }

    public String getFullPath() {
        return this.fullPath;
    }

    public byte[] getRawDocument() {
        return this.rawDocument;
    }

    public Map getConfiguration() {
        return this.configuration;
    }

    public Collection getSearchAttributes() {
        return this.searchAttributes;
    }

    public void setType(int type) {
        this.type = type;
    }

    public void setFullPath(String fullPath) {
        this.fullPath = fullPath;
    }

    public void setRawDocument(byte[] rawDocument) {
        this.rawDocument = rawDocument;
    }

    public void setSearchAttributes(Collection searchAttributes) {
        this.searchAttributes = searchAttributes;
    }

    public void setConfiguration(Map configuration) {
        this.configuration = configuration;
    }

    public int getScope() {
        return scope;
    }

    public void setScope(int scope) {
        this.scope = scope;
    }
}
