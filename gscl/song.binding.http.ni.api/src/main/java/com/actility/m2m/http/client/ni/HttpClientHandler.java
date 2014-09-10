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
 * id $Id: HttpClientHandler.java 5992 2013-10-07 14:32:35Z JReich $
 * author $Author: JReich $
 * version $Revision: 5992 $
 * lastrevision $Date: 2013-10-07 16:32:35 +0200 (Mon, 07 Oct 2013) $
 * modifiedby $LastChangedBy: JReich $
 * lastmodified $LastChangedDate: 2013-10-07 16:32:35 +0200 (Mon, 07 Oct 2013) $
 */
package com.actility.m2m.http.client.ni;

import java.io.IOException;

/**
 * Handler that treats HTTP client responses when received from the network
 */
public interface HttpClientHandler {
    /**
     * Callback to notify the Handler that an HTTP response has been received on the network
     *
     * @param transaction The HTTP response embedded in the HTTP transaction (request + response)
     * @throws IOException If any I/O problem occurs while treating this HTTP response
     */
    void doClientResponse(HttpClientTransaction transaction) throws IOException;
}
