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

package com.actility.m2m.be.messaging.impl;

import java.util.Collections;
import java.util.HashMap;
import java.util.Map;
import java.util.Set;

import com.actility.m2m.be.messaging.BackendMessage;
import com.actility.m2m.be.messaging.MessageExchange;

public abstract class MessageExchangeImpl implements MessageExchange {

    private Map properties;
    private int source;
    private int destination;
    private final String id;

    protected MessageExchangeImpl(String id) {
        this.id = id;
    }

    public final BackendMessage createMessage() {
        return new BackendMessageImpl();
    }

    public final String getExchangeId() {
        return id;
    }

    public final Object getProperty(String name) {
        return (properties != null) ? properties.get(name) : null;
    }

    public final Set getPropertyNames() {
        return (properties == null) ? Collections.EMPTY_SET : properties.keySet();
    }

    public final void setProperty(String name, Object obj) {
        if (properties == null) {
            properties = new HashMap();
        }
        properties.put(name, obj);
    }

    public final void setDestination(int destination) {
        this.destination = destination;
    }

    public final void setSource(int source) {
        this.source = source;
    }

    public final int getDestination() {
        return destination;
    }

    public final int getSource() {
        return source;
    }
}
