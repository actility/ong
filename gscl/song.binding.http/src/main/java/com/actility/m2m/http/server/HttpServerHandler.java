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
 * id $Id: HttpServerHandler.java 8521 2014-04-14 09:05:59Z JReich $
 * author $Author: JReich $
 * version $Revision: 8521 $
 * lastrevision $Date: 2014-04-14 11:05:59 +0200 (Mon, 14 Apr 2014) $
 * modifiedby $LastChangedBy: JReich $
 * lastmodified $LastChangedDate: 2014-04-14 11:05:59 +0200 (Mon, 14 Apr 2014) $
 */

package com.actility.m2m.http.server;

import java.io.IOException;

/**
 * Handler that treats HTTP server requests when received from the network
 */
public interface HttpServerHandler {
    /**
     * Callback to notify the handler that a HTTP request that could not be properly decoded has been received from the network
     *
     * @param e The exception that represents the problem
     * @param transaction The HTTP server transaction that caused this error
     */
    void doServerError(HttpServerException e, HttpServerTransaction transaction);

    /**
     * Callback to notify the handler that an HTTP request has been received on the network
     *
     * @param transaction The HTTP request embedded in the HTTP server transaction (request + response)
     * @throws IOException If any I/O problem occurs while treating this HTTP request
     */
    void doServerRequest(HttpServerTransaction transaction) throws IOException;
}
