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
 * id $Id: ExtProtocolContainer.java 6081 2013-10-15 13:33:50Z mlouiset $
 * author $Author: mlouiset $
 * version $Revision: 6081 $
 * lastrevision $Date: 2013-10-15 15:33:50 +0200 (Tue, 15 Oct 2013) $
 * modifiedby $LastChangedBy: mlouiset $
 * lastmodified $LastChangedDate: 2013-10-15 15:33:50 +0200 (Tue, 15 Oct 2013) $
 */

package com.actility.m2m.servlet.ext;

import javax.servlet.RequestDispatcher;
import javax.servlet.Servlet;

/**
 * Interface that reference the minimum contract that a protocol must follow in order to be integrated in the servlet container.
 */
public interface ExtProtocolContainer {

    /**
     * Gets the protocol name (for example HTTP).
     *
     * @return The protocol name
     */
    String getProtocolName();

    /**
     * Gets a {@link RequestDispatcher} to the given {@link Servlet}.
     *
     * @param servlet The servlet to which the request dispatcher will point to.
     * @return The built request dispatcher
     */
    RequestDispatcher getRequestDispatcher(ExtProtocolServlet servlet);

    /**
     * Gets a human readable description of the protocol container for the given servlet.
     *
     * @param buf The buffer to which the protocol description will be added.
     *
     * @param servlet The servlet to get the description from.
     */
    void getDescription(StringBuffer buf, ExtProtocolServlet servlet);

    /**
     * Destroys the protocol context.
     */
    void destroy();
}
