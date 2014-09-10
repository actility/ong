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
 * id $Id: BackendException.java 7604 2014-02-06 13:01:34Z JReich $
 * author $Author: JReich $
 * version $Revision: 7604 $
 * lastrevision $Date: 2014-02-06 14:01:34 +0100 (Thu, 06 Feb 2014) $
 * modifiedby $LastChangedBy: JReich $
 * lastmodified $LastChangedDate: 2014-02-06 14:01:34 +0100 (Thu, 06 Feb 2014) $
 */

package com.actility.m2m.be;

/**
 * Generic exception in the Backend service.
 */
public class BackendException extends Exception {

    /**
     * The exception serial version UID
     */
    private static final long serialVersionUID = -7126588632475028812L;

    /**
     * Builds a backend exception with the given message.
     *
     * @param aMessage The message of the exception
     */
    public BackendException(String aMessage) {
        super(aMessage);
    }

    /**
     * Builds a backend exception with the given message and {@link Throwable} root cause.
     *
     * @param aMessage The message of the exception
     * @param aCause The root cause of the exception
     */
    public BackendException(String aMessage, Throwable aCause) {
        super(aMessage, aCause);
    }

    /**
     * Builds a backend exception with the given root cause.
     *
     * @param aCause The root cause of the excetion
     */
    public BackendException(Throwable aCause) {
        super(aCause);
    }
}
