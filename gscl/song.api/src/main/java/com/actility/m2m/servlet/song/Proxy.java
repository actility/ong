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
 * id $Id: Proxy.java 8026 2014-03-07 17:26:54Z JReich $
 * author $Author: JReich $
 * version $Revision: 8026 $
 * lastrevision $Date: 2014-03-07 18:26:54 +0100 (Fri, 07 Mar 2014) $
 * modifiedby $LastChangedBy: JReich $
 * lastmodified $LastChangedDate: 2014-03-07 18:26:54 +0100 (Fri, 07 Mar 2014) $
 */

package com.actility.m2m.servlet.song;

/**
 * Represents the operation of proxying a SONG request.
 * <p>
 * A number of parameters control how proxying is carried out:
 *
 * <ul>
 * <li><b>supervised</b>: Whether the application will be invoked on incoming responses related to this proxying. Default value
 * is <code>false</code></li>
 * </ul>
 */
public interface Proxy {

    /**
     * Returns true if the controlling servlet will be invoked on incoming responses for this proxying operation, and false
     * otherwise.
     *
     * @return If true, the servlet invoked to handle the request originally received will be notified when a response is
     *         received.
     */
    boolean getSupervised();

    /**
     * Specifies whether the controlling servlet is to be invoked for incoming responses relating to this proxying.
     *
     * @param supervised If true, the servlet invoked to handle the request originally received will be notified when a response
     *            is received.
     */
    void setSupervised(boolean supervised);

    /**
     * Gets the proxy the current request to the given proxy URI.
     *
     * @return The proxy URI to which the current request must be sent
     */
    SongURI getProxyTo();

    /**
     * Sets the proxy the current request to the given proxy URI.
     *
     * @param proxy The proxy URI to which the current request must be sent
     */
    void setProxyTo(SongURI proxy);
}
