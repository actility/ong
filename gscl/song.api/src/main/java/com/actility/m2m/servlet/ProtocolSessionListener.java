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
 * id $Id: ProtocolSessionListener.java 6081 2013-10-15 13:33:50Z mlouiset $
 * author $Author: mlouiset $
 * version $Revision: 6081 $
 * lastrevision $Date: 2013-10-15 15:33:50 +0200 (Tue, 15 Oct 2013) $
 * modifiedby $LastChangedBy: mlouiset $
 * lastmodified $LastChangedDate: 2013-10-15 15:33:50 +0200 (Tue, 15 Oct 2013) $
 */

package com.actility.m2m.servlet;

import java.util.EventListener;

/**
 * Implementations of this interface are notified of changes to the list of active ProtocolSessions in a servlet application. To
 * receive notification events, the implementation class must be configured in the deployment descriptor for the application.
 */
public interface ProtocolSessionListener extends EventListener {
    /**
     * Notification that a SongSession was created.
     * 
     * @param se The notification event
     */
    void sessionCreated(ProtocolSessionEvent se);

    /**
     * Notification that a SongSession was destroyed.
     * 
     * @param se The notification event
     */
    void sessionDestroyed(ProtocolSessionEvent se);

    /**
     * Notification that a ProtocolSession is in the ready-to-invalidate state. The container will invalidate this session upon
     * completion of this callback unless the listener implementation calls
     * {@link ProtocolSession#setInvalidateWhenReady(boolean)} with a false value.
     * 
     * @param se the notification event
     */
    void sessionReadyToInvalidate(ProtocolSessionEvent se);
}
