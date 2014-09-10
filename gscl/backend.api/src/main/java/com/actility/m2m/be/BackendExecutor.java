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
 * id $Id: BackendExecutor.java 6058 2013-10-14 12:01:59Z mlouiset $
 * author $Author: mlouiset $
 * version $Revision: 6058 $
 * lastrevision $Date: 2013-10-14 14:01:59 +0200 (Mon, 14 Oct 2013) $
 * modifiedby $LastChangedBy: mlouiset $
 * lastmodified $LastChangedDate: 2013-10-14 14:01:59 +0200 (Mon, 14 Oct 2013) $
 */

package com.actility.m2m.be;

import com.actility.m2m.be.messaging.MessageExchange;
import com.actility.m2m.be.messaging.MessagingException;

/**
 * The class that concretely executes the process method of the {@link BackendEndpoint endpoint} with the given
 * {@link MessageExchange exchange}.
 */
public interface BackendExecutor {

    /**
     * Sends an exchange to the destination endpoint. This is an asynchonous call.
     *
     * @param exchange The exchange to send
     * @param destination The destination endpoint to reach
     * @param priority exchange priority
     * @throws MessagingException If any problem occurs during the transfer
     */
    void send(MessageExchange exchange, BackendEndpoint destination, int priority) throws MessagingException;

    /**
     * Destroys the backend executor and its associated context and threads
     */
    void destroy();
}
