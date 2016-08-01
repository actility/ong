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

import com.actility.m2m.be.messaging.InOnly;
import com.actility.m2m.be.messaging.InOut;
import com.actility.m2m.be.messaging.MessageExchange;
import com.actility.m2m.be.messaging.MessageExchangeFactory;
import com.actility.m2m.be.messaging.MessagingException;

public final class MessageExchangeFactoryImpl implements MessageExchangeFactory {

    private int id = 0;

    public MessageExchange createExchange(String pattern) throws MessagingException {
        if ("InOut".equals(pattern)) {
            return createInOutExchange();
        } else if ("InOnly".equals(pattern)) {
            return createInOnlyExchange();
        } else {
            throw new MessagingException("Unable to create the exchange. Unknown pattern: " + pattern);
        }
    }

    public MessageExchange createExchange(String id, String pattern) throws MessagingException {
        if ("InOut".equals(pattern)) {
            return createInOutExchange(id);
        } else if ("InOnly".equals(pattern)) {
            return createInOnlyExchange(id);
        } else {
            throw new MessagingException("Unable to create the exchange. Unknown pattern: " + pattern);
        }
    }

    public synchronized InOnly createInOnlyExchange() {
        ++id;
        return new InOnlyImpl(String.valueOf(id));
    }

    public InOnly createInOnlyExchange(String id) {
        return new InOnlyImpl(id);
    }

    public synchronized InOut createInOutExchange() {
        ++id;
        return new InOutImpl(String.valueOf(id));
    }

    public InOut createInOutExchange(String id) {
        return new InOutImpl(id);
    }
}
