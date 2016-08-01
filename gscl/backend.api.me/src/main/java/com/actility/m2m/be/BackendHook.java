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

package com.actility.m2m.be;

import com.actility.m2m.be.messaging.MessageExchange;

/**
 * A hook that receives all exchanges that go through the backend bus.
 */
public interface BackendHook {

    /**
     * An exchange has been received on the bus and the destination is known.
     * 
     * @param exchange The exchange that has been received
     * @param from The origin of the exchange
     * @param to The destination of the exchange
     */
    void resolvedExchange(MessageExchange exchange, BackendEndpoint from, BackendEndpoint to);

    /**
     * An exchange has been received on the bus and the destination is unknown.
     * 
     * @param exchange The exchange that has been received
     * @param from The origin of the exchange
     */
    void unresolvedExchange(MessageExchange exchange, BackendEndpoint from);
}
