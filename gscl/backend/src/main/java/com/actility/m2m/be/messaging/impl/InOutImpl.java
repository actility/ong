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
 * id $Id: InOutImpl.java 6060 2013-10-14 12:13:03Z mlouiset $
 * author $Author: mlouiset $
 * version $Revision: 6060 $
 * lastrevision $Date: 2013-10-14 14:13:03 +0200 (Mon, 14 Oct 2013) $
 * modifiedby $LastChangedBy: mlouiset $
 * lastmodified $LastChangedDate: 2013-10-14 14:13:03 +0200 (Mon, 14 Oct 2013) $
 */

package com.actility.m2m.be.messaging.impl;

import com.actility.m2m.be.messaging.BackendMessage;
import com.actility.m2m.be.messaging.InOut;

public final class InOutImpl extends MessageExchangeImpl implements InOut {

    private BackendMessage inMessage;
    private BackendMessage outMessage;

    public InOutImpl(String id) {
        super(id);
    }

    public BackendMessage getInMessage() {
        return inMessage;
    }

    public BackendMessage getOutMessage() {
        return outMessage;
    }

    public void setInMessage(BackendMessage msg) {
        this.inMessage = msg;
    }

    public void setOutMessage(BackendMessage msg) {
        this.outMessage = msg;
    }

    public String getPattern() {
        return "IN-OUT";
    }

    public BackendMessage getMessage(String name) {
        if ("IN".equals(name)) {
            return inMessage;
        } else if ("OUT".equals(name)) {
            return outMessage;
        }
        return null;
    }

    public void setMessage(BackendMessage message, String name) {
        if ("IN".equals(name)) {
            inMessage = message;
        } else if ("OUT".equals(name)) {
            outMessage = message;
        }
    }
}
