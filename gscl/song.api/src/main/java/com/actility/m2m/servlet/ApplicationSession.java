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
 * id $Id: ApplicationSession.java 6081 2013-10-15 13:33:50Z mlouiset $
 * author $Author: mlouiset $
 * version $Revision: 6081 $
 * lastrevision $Date: 2013-10-15 15:33:50 +0200 (Tue, 15 Oct 2013) $
 * modifiedby $LastChangedBy: mlouiset $
 * lastmodified $LastChangedDate: 2013-10-15 15:33:50 +0200 (Tue, 15 Oct 2013) $
 */

package com.actility.m2m.servlet;

import java.util.Collection;
import java.util.Iterator;

import javax.servlet.ServletContext;

/**
 * An applicative session related to a servlet application.
 * <p>
 * This session is used to gather several {@link ProtocolSession protocol sessions} and / or start {@link ServletTimer timers}.
 */
public interface ApplicationSession {

    /**
     * Gets the name of the application.
     * <p>
     *
     * @see ServletContext#getServletContextName()
     *
     * @return The name of the application
     */
    String getApplicationName();

    /**
     * Gets an attribute bound to the application session given its name.
     *
     * @param name The name of the parameter
     * @return The value of the parameter given its name or null if it does not exist
     */
    Object getAttribute(String name);

    /**
     * Gets an iterator on the list of available attributes bound to the application session.
     *
     * @return An iterator on the list of available attributes
     */
    Iterator getAttributeNames();

    /**
     * Gets the creation time of the application session in milliseconds since EPOCH.
     *
     * @return The creation time of the application session
     */
    long getCreationTime();

    /**
     * Gets the expiration time of the application session in milliseconds since EPOCH.
     *
     * @return The expiration time of the application session
     */
    long getExpirationTime();

    /**
     * Gets a unique identifier of the application session.
     *
     * @return The unique identifier of the application session
     */
    String getId();

    /**
     * Gets the invalidate when ready flag. If this flag is true, the application session is automatically {@link #invalidate()}
     * when it is ready (all protocol sessions bound to the application session are invalidated or ready to invalidate and no
     * timers are active)
     *
     * @return The invalidate when ready flag
     */
    boolean getInvalidateWhenReady();

    /**
     * Gets the last accessed time to the application session in millisecond since EPOCH.
     *
     * @return The last accessed time to the application session
     */
    long getLastAccessedTime();

    /**
     * Gets an iterator on all {@link ProtocolSession protocol sessions} bound to the application session.
     *
     * @return An iterator on all protocol sessions
     */
    Iterator getSessions();

    /**
     * Gets an iterator on all {@link ProtocolSession protocol sessions} related to the given protocol name and bound to the
     * application session.
     *
     * @param protocol The protocol name for which we search the protocol sessions
     * @return The protocol sessions bound to the given protocol
     */
    Iterator getSessions(String protocol);

    /**
     * Gets a {@link ProtocolSession protocol session} given its protocol name and unique identifier.
     *
     * @param protocol The protocol name
     * @param id The unique identifier of the protocol session
     * @return The protocol session bound to the given protocol name and identifier or null if it does not exist
     */
    ProtocolSession getSession(String protocol, String id);

    /**
     * Gets a {@link ServletTimer timer} given its unique identifier.
     *
     * @param id The unique identifier of the ServletTimer
     * @return The ServletTimer or null if it does not exist
     */
    ServletTimer getTimer(String id);

    /**
     * Gets a collection of all active {@link ServletTimer timers} bound to the application session.
     *
     * @return The collection of active timers
     */
    Collection getTimers();

    /**
     * Explicitly invalidates the application session. This stops all active {@link ServletTimer timers} and invalidates all
     * valid {@link ProtocolSession protocol sessions}.
     */
    void invalidate();

    /**
     * Checks whether the application session is ready to invalidate (e.g. all protocol sessions bound to the application
     * session are invalidated or ready to invalidate and no timers are active)
     *
     * @return Whether the application session is ready to invalidate
     */
    boolean isReadyToInvalidate();

    /**
     * Checks whether the application session is valid.
     *
     * @return Whether the application session is valid.
     */
    boolean isValid();

    /**
     * Removes an attribute bound to the application session given its name.
     *
     * @param name The name of the attribute to remove
     */
    void removeAttribute(String name);

    /**
     * Sets an attribute to the application session given its name and value. Any previous value of the attribute will be
     * overridden.
     *
     * @param name The name of the attribute
     * @param attribute The attribute to set
     */
    void setAttribute(String name, Object attribute);

    /**
     * Sets the number of minutes in which the application session will be expired.
     *
     * @param deltaMinutes The number of minutes in which the application session will be expired
     * @return The real number of minutes in which the application session will be expired. Could be different from the given
     *         value
     */
    int setExpires(int deltaMinutes);

    /**
     * Sets the number of milliseconds in which the application session will be expired.
     *
     * @param deltaMilliseconds The number of milliseconds in which the application session will be expired
     * @return The real number of milliseconds in which the application session will be expired. Could be different from the
     *         given value
     */
    long setExpiresMillis(long deltaMilliseconds);

    /**
     * Sets the invalidate when ready flags.
     *
     * @param invalidateWhenReady The invalidate when ready flag to set
     * @see #getInvalidateWhenReady()
     */
    void setInvalidateWhenReady(boolean invalidateWhenReady);
}
