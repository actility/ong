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
 * id $Id: ParameterableImpl.java 6085 2013-10-15 14:08:29Z mlouiset $
 * author $Author: mlouiset $
 * version $Revision: 6085 $
 * lastrevision $Date: 2013-10-15 16:08:29 +0200 (Tue, 15 Oct 2013) $
 * modifiedby $LastChangedBy: mlouiset $
 * lastmodified $LastChangedDate: 2013-10-15 16:08:29 +0200 (Tue, 15 Oct 2013) $
 */

package com.actility.m2m.servlet.song.message;

import java.util.Iterator;
import java.util.Map;
import java.util.Set;

import com.actility.m2m.servlet.song.Parameterable;

public final class ParameterableImpl implements Parameterable {

    private final Map paramMap;
    private String value;
    private boolean modifiable;

    public ParameterableImpl(String headerValue, Map paramMap, boolean modifiable) {
        this.value = headerValue;
        this.paramMap = paramMap;
        this.modifiable = modifiable;
    }

    private void checkModifiable() {
        if (!modifiable) {
            throw new IllegalStateException("The Parameterable is not modifiable");
        }
    }

    public String getParameter(String key) {
        return (String) paramMap.get(key);
    }

    public Iterator getParameterNames() {
        return paramMap.keySet().iterator();
    }

    public Set getParameters() {
        return paramMap.entrySet();
    }

    public String getValue() {
        return value;
    }

    public void removeParameter(String name) {
        checkModifiable();
        if (name == null) {
            throw new IllegalArgumentException("name is null");
        }
        paramMap.remove(name);
    }

    public void setParameter(String name, String value) {
        checkModifiable();
        if (name == null) {
            throw new IllegalArgumentException("name is null");
        }
        if (value == null) {
            paramMap.remove(name);
        } else {
            paramMap.put(name, value);
        }
    }

    public void setValue(String value) {
        checkModifiable();
        if (value == null) {
            throw new IllegalArgumentException("value is null");
        }
        this.value = value;
    }

    public String toString() {
        StringBuffer result = new StringBuffer();
        result.append(value);
        Iterator it = paramMap.entrySet().iterator();
        Map.Entry entry = null;
        String paramKey = null;
        String paramValue = null;
        while (it.hasNext()) {
            entry = (Map.Entry) it.next();
            result.append(';');
            paramKey = (String) entry.getKey();
            paramValue = (String) entry.getValue();
            result.append(paramKey);
            if (paramValue.length() != 0) {
                result.append('=');
                result.append(paramValue);
            }
        }
        return result.toString();
    }

}
