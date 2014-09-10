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
 * id $Id: BackendMessageImpl.java 6060 2013-10-14 12:13:03Z mlouiset $
 * author $Author: mlouiset $
 * version $Revision: 6060 $
 * lastrevision $Date: 2013-10-14 14:13:03 +0200 (Mon, 14 Oct 2013) $
 * modifiedby $LastChangedBy: mlouiset $
 * lastmodified $LastChangedDate: 2013-10-14 14:13:03 +0200 (Mon, 14 Oct 2013) $
 */

package com.actility.m2m.be.messaging.impl;

import java.util.Collections;
import java.util.HashMap;
import java.util.Map;
import java.util.Set;

import com.actility.m2m.be.messaging.BackendMessage;

public final class BackendMessageImpl implements BackendMessage {
    private Object content;
    private Map properties;

    public Object getContent() {
        return content;
    }

    public void setContent(Object content) {
        this.content = content;
    }

    public Object getProperty(String name) {
        return (properties != null) ? properties.get(name) : null;
    }

    public Set getPropertyNames() {
        return (properties != null) ? properties.keySet() : Collections.EMPTY_SET;
    }

    public void setProperty(String name, Object value) {
        if (properties == null) {
            properties = new HashMap();
        }
        properties.put(name, value);
    }
}
