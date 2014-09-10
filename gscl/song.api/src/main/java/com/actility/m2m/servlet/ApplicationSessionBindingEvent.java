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
 * id $Id: ApplicationSessionBindingEvent.java 6081 2013-10-15 13:33:50Z mlouiset $
 * author $Author: mlouiset $
 * version $Revision: 6081 $
 * lastrevision $Date: 2013-10-15 15:33:50 +0200 (Tue, 15 Oct 2013) $
 * modifiedby $LastChangedBy: mlouiset $
 * lastmodified $LastChangedDate: 2013-10-15 15:33:50 +0200 (Tue, 15 Oct 2013) $
 */

package com.actility.m2m.servlet;

/**
 * Events of this type are either sent to an object that implements ApplicationSessionBindingListener when it is bound or
 * unbound from an application session, or to a ApplicationSessionAttributeListener that has been configured in the deployment
 * descriptor when any attribute is bound, unbound or replaced in an application session. The session binds the object by a call
 * to ApplicationSession.setAttribute(String, Object) and unbinds the object by a call to
 * ApplicationSession.removeAttribute(String).
 * 
 * @see ApplicationSession ApplicationSessionBindingListener ApplicationSessionAttributeListener
 */
public class ApplicationSessionBindingEvent {
    private String name = null;
    private ApplicationSession appSession;

    /**
     * Constructs an event that notifies an object that it has been bound to or unbound from an application session. To receive
     * the event, the object must implement ApplicationSessionBindingListener.
     * 
     * @param session The application session to which the object is bound or unbound
     * @param name the name with which the object is bound or unbound
     */
    public ApplicationSessionBindingEvent(ApplicationSession session, String name) {
        this.appSession = session;
        this.name = name;
    }

    /**
     * Returns the application session to or from which the object is bound or unbound.
     * 
     * @return The application session
     */
    public ApplicationSession getApplicationSession() {
        return appSession;
    }

    /**
     * Returns the name with which the object is bound to or unbound from the application session.
     * 
     * @return The name with which the object is bound
     */
    public String getName() {
        return name;
    }
}
