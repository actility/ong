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
 * id $Id: InOnly.java 6058 2013-10-14 12:01:59Z mlouiset $
 * author $Author: mlouiset $
 * version $Revision: 6058 $
 * lastrevision $Date: 2013-10-14 14:01:59 +0200 (Mon, 14 Oct 2013) $
 * modifiedby $LastChangedBy: mlouiset $
 * lastmodified $LastChangedDate: 2013-10-14 14:01:59 +0200 (Mon, 14 Oct 2013) $
 */

package com.actility.m2m.be.messaging;

/**
 * Supports operations used to process an In Only MEP to completion.
 */
public interface InOnly extends MessageExchange {

    /**
     * Retrieves the <i>in</i> BackendMessage from this exchange.
     * 
     * @return in message
     */
    BackendMessage getInMessage();

    /**
     * Sets the <i>in</i> BackendMessage for this exchange.
     * 
     * @param msg in message
     * @throws MessagingException unable to set in message
     */
    void setInMessage(BackendMessage msg) throws MessagingException;

}
