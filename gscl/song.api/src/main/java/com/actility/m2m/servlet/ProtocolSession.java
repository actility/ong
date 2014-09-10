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
 * id $Id: ProtocolSession.java 6081 2013-10-15 13:33:50Z mlouiset $
 * author $Author: mlouiset $
 * version $Revision: 6081 $
 * lastrevision $Date: 2013-10-15 15:33:50 +0200 (Tue, 15 Oct 2013) $
 * modifiedby $LastChangedBy: mlouiset $
 * lastmodified $LastChangedDate: 2013-10-15 15:33:50 +0200 (Tue, 15 Oct 2013) $
 */

package com.actility.m2m.servlet;

import java.util.Iterator;

import javax.servlet.ServletContext;

/**
 * A generic session related to a protocol.
 */
public interface ProtocolSession {
    /**
     * Gets the application session to which the current session is bound.
     *
     * @return The application session
     */
    ApplicationSession getApplicationSession();

    /**
     * Gets an attribute bound to the protocol session given its name.
     *
     * @param name The name of the parameter
     * @return The value of the parameter given its name or null if it does not exist
     */
    Object getAttribute(String name);

    /**
     * Gets an iterator on the list of available attributes bound to the protocol session.
     *
     * @return An iterator on the list of available attributes
     */
    Iterator getAttributeNames();

    /**
     * Gets the creation time of the protocol session in milliseconds since EPOCH.
     *
     * @return The creation time of the protocol session
     */
    long getCreationTime();

    /**
     * Gets a unique identifier of the protocol session.
     *
     * @return The unique identifier of the protocol session
     */
    String getId();

    /**
     * Gets the invalidate when ready flag. If this flag is true, the protocol session is automatically {@link #invalidate()}
     * when it is ready to.
     *
     * @return The invalidate when ready flag
     */
    boolean getInvalidateWhenReady();

    /**
     * Gets the last accessed time to the protocol session in millisecond since EPOCH.
     *
     * @return The last accessed time to the protocol session
     */
    long getLastAccessedTime();

    /**
     * Gets the application context related to that protocol session.
     *
     * @return The application context related to that protocol session
     */
    ServletContext getServletContext();

    /**
     * Explicitly invalidates the protocol session.
     */
    void invalidate();

    /**
     * Checks whether the protocol session is valid.
     *
     * @return Whether the protocol session is valid.
     */
    boolean isValid();

    /**
     * Checks whether the protocol session is ready to invalidate.
     *
     * @return Whether the protocol session is ready to invalidate
     */
    boolean isReadyToInvalidate();

    /**
     * Removes an attribute bound to the protocol session given its name.
     *
     * @param name The name of the attribute to remove
     */
    void removeAttribute(String name);

    /**
     * Sets an attribute to the protocol session given its name and value. Any previous value of the attribute will be
     * overridden.
     *
     * @param name The name of the attribute
     * @param attribute The attribute to set
     */
    void setAttribute(String name, Object attribute);

    /**
     * Sets the invalidate when ready flags.
     *
     * @param invalidateWhenReady The invalidate when ready flag to set
     * @see #getInvalidateWhenReady()
     */
    void setInvalidateWhenReady(boolean invalidateWhenReady);
}
