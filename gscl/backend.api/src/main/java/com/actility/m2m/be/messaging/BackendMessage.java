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
 * id $Id: BackendMessage.java 6058 2013-10-14 12:01:59Z mlouiset $
 * author $Author: mlouiset $
 * version $Revision: 6058 $
 * lastrevision $Date: 2013-10-14 14:01:59 +0200 (Mon, 14 Oct 2013) $
 * modifiedby $LastChangedBy: mlouiset $
 * lastmodified $LastChangedDate: 2013-10-14 14:01:59 +0200 (Mon, 14 Oct 2013) $
 */

package com.actility.m2m.be.messaging;

import java.util.Set;

/**
 * Represents a Backend Message. A normalized message consists of:
 * <ul>
 * <li>Content. The “payload” message.</li>
 * <li>Properties. Other message-related data. Properties are name/value pairs. This may be extended by components to include
 * component-specific properties.</li>
 * </ul>
 */
public interface BackendMessage {

    /**
     * Retrieve the content of the message.
     * 
     * @return message content
     */
    Object getContent();

    /**
     * Retrieve a property from the message.
     * 
     * @param name property name
     * @return property value, or <code>null</code> if the property does not exist or has no value
     */
    Object getProperty(String name);

    /**
     * Retrieve a set of property names for this message.
     * 
     * @return set of property names for this message
     */
    Set getPropertyNames();

    /**
     * Set the content of the message.
     * 
     * @param content message content
     * @throws MessagingException failed to set content
     */
    void setContent(Object content) throws MessagingException;

    /**
     * Set a property on the message.
     * 
     * @param name property name
     * @param value property value
     */
    void setProperty(String name, Object value);
}
