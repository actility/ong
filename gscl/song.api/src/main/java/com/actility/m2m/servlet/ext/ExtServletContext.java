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
 * id $Id: ExtServletContext.java 6081 2013-10-15 13:33:50Z mlouiset $
 * author $Author: mlouiset $
 * version $Revision: 6081 $
 * lastrevision $Date: 2013-10-15 15:33:50 +0200 (Tue, 15 Oct 2013) $
 * modifiedby $LastChangedBy: mlouiset $
 * lastmodified $LastChangedDate: 2013-10-15 15:33:50 +0200 (Tue, 15 Oct 2013) $
 */

package com.actility.m2m.servlet.ext;

import java.util.ConcurrentModificationException;
import java.util.Iterator;
import java.util.Map;

import javax.servlet.ServletContext;
import javax.servlet.ServletException;
import javax.servlet.ServletRequestAttributeListener;
import javax.servlet.ServletRequestListener;

import com.actility.m2m.servlet.ProtocolSessionAttributeListener;
import com.actility.m2m.servlet.ProtocolSessionListener;

/**
 * Extensions to the {@link ServletContext} which is used internally between the servlet container and a protocol container
 * 
 */
public interface ExtServletContext extends ServletContext {
    /**
     * Builds an internal {@link ExtApplicationSession}.
     * 
     * @return The built {@link ExtApplicationSession}
     */
    ExtApplicationSession createApplicationSession();

    /**
     * Gets an iterator on known {@link ExtApplicationSession}.
     * <p>
     * Warning: The iterator is not thread safe and a {@link ConcurrentModificationException} can be raised while reading it
     * 
     * @return An iterator on {@link ExtApplicationSession}
     */
    Iterator getApplicationSessions();

    /**
     * Informs the servlet container that a servlet has been added to the given protocol container.
     * 
     * @param servletName The servlet name
     * @param servlet The servlet to add
     * @param initParams The initial parameters of the servlet used to configure the servlet during its initialization
     * @throws ServletException If a problem occurs while registering this event
     */
    void addServlet(String servletName, ExtProtocolServlet servlet, Map initParams) throws ServletException;

    /**
     * Informs the servlet container that a servlet has been removed from the given protocol container.
     * 
     * @param servletName The name of the servlet to remove
     */
    void removeServlet(String servletName);

    /**
     * Gets a human readable description of the servlet context.
     * 
     * @return The description of the servlet context
     */
    String getDescription();

    /**
     * Gets an iterator on {@link ProtocolSessionAttributeListener}.
     * 
     * @return An iterator on {@link ProtocolSessionAttributeListener}
     */
    Iterator getProtocolSessionAttributeListeners();

    /**
     * Gets an iterator on {@link ProtocolSessionListener}.
     * 
     * @return An iterator on {@link ProtocolSessionListener}
     */
    Iterator getProtocolSessionListeners();

    /**
     * Gets an iterator on {@link ServletRequestListener}.
     * 
     * @return An iterator on {@link ServletRequestListener}
     */
    Iterator getServletRequestListeners();

    /**
     * Gets an iterator on {@link ServletRequestAttributeListener}.
     * 
     * @return An iterator on {@link ServletRequestAttributeListener}
     */
    Iterator getServletRequestAttributeListeners();

    /**
     * Gets the default servlet for the current application.
     * 
     * @param protocol The protocol which asks for the default servlet
     * @return The default servlet for the given protocol
     */
    ExtProtocolServlet getDefaultServlet(ExtProtocolContainer protocol);

    /**
     * Sets the context current request handler for the given thread.
     * <p>
     * This is local to a thread
     * 
     * @param protocol The protocol container for which the current request handler must be set
     * @param servlet The servlet to set as the current request handler
     * @return The previous request handler
     */
    ExtProtocolServlet setCurrentRequestHandler(ExtProtocolContainer protocol, ExtProtocolServlet servlet);

    /**
     * Gets the current request handler in the current thread for the given protocol.
     * <p>
     * This is local to a thread
     * 
     * @param protocol The protocol which asks for the default servlet
     * @return The current request handler
     */
    ExtProtocolServlet getCurrentRequestHandler(ExtProtocolContainer protocol);
}
