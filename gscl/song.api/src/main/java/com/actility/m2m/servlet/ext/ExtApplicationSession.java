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
 * id $Id: ExtApplicationSession.java 6081 2013-10-15 13:33:50Z mlouiset $
 * author $Author: mlouiset $
 * version $Revision: 6081 $
 * lastrevision $Date: 2013-10-15 15:33:50 +0200 (Tue, 15 Oct 2013) $
 * modifiedby $LastChangedBy: mlouiset $
 * lastmodified $LastChangedDate: 2013-10-15 15:33:50 +0200 (Tue, 15 Oct 2013) $
 */

package com.actility.m2m.servlet.ext;

import javax.servlet.ServletException;

import com.actility.m2m.servlet.ApplicationSession;

/**
 * Extension of the {@link ApplicationSession} interface in order to add internal API exposed to servlet bindings.
 */
public interface ExtApplicationSession extends ApplicationSession {

    /**
     * Gets the servlet context associated to the application session.
     *
     * @return The servlet context
     */
    ExtServletContext getServletContext();

    /**
     * Adds a {@link ExtProtocolSession protocol session} to the application session.
     *
     * @param session The protocol session to add
     * @throws ServletException If the session is already known
     */
    void addProtocolSession(ExtProtocolSession session) throws ServletException;

    /**
     * Removes a {@link ExtProtocolSession protocol session} from the application session.
     *
     * @param session The session to remove
     */
    void removeProtocolSession(ExtProtocolSession session);

    /**
     * Informs the application session that the given protocol session is ready to invalidate.
     *
     * @param session The protocol session that is ready to invalidate
     */
    void protocolSessionReadyToInvalidate(ExtProtocolSession session);

    /**
     * Gets number of running timers.
     *
     * @return The number of running timers
     */
    int getTimersSize();

    /**
     * Gets number of valid protocol sessions.
     *
     * @return The number of valid protocol sessions
     */
    int getProtocolSessionsSize();

    /**
     * Update the accessed time information for this session. This method should be called by the context when a request comes
     * in for a particular session, even if the application does not reference it. The getLastAccessedTime method of the
     * ApplicationSession interfaces allows a servlet to determine the last time a session was accessed before the current
     * message was handled. A session is considered to be accessed when a message that is part of the session is handled by the
     * servlet container. The last accessed time of a ApplicationSession will thus always be the most recent last accessed time
     * of any of its contained protocol sessions. Whenever the last accessed time for a ApplicationSession is updated, it is
     * considered refreshed i.e., the expiry timer for that ApplicationSession starts anew.
     */
    void access();

    /**
     * Gets the request handler associated with the application session for the given protocol.
     *
     * @param protocol The protocol which searches its handler
     * @return The request handler associated with the application session
     */
    ExtProtocolServlet getHandler(ExtProtocolContainer protocol);
}
