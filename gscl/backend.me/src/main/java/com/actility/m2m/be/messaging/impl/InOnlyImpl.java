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

import com.actility.m2m.be.messaging.BackendMessage;
import com.actility.m2m.be.messaging.InOnly;

public final class InOnlyImpl extends MessageExchangeImpl implements InOnly {
    private BackendMessage inMessage;

    public InOnlyImpl(String id) {
        super(id);
    }

    public BackendMessage getInMessage() {
        return inMessage;
    }

    public void setInMessage(BackendMessage msg) {
        this.inMessage = msg;
    }

    public String getPattern() {
        return "IN";
    }

    public BackendMessage getMessage(String name) {
        if ("IN".equals(name)) {
            return inMessage;
        }
        return null;
    }

    public void setMessage(BackendMessage message, String name) {
        if ("IN".equals(name)) {
            inMessage = message;
        }
    }
}
