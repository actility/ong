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
 * id $Id: HttpClientException.java 5992 2013-10-07 14:32:35Z JReich $
 * author $Author: JReich $
 * version $Revision: 5992 $
 * lastrevision $Date: 2013-10-07 16:32:35 +0200 (Mon, 07 Oct 2013) $
 * modifiedby $LastChangedBy: JReich $
 * lastmodified $LastChangedDate: 2013-10-07 16:32:35 +0200 (Mon, 07 Oct 2013) $
 */
package com.actility.m2m.http.client.ni;

/**
 * A generic exception which reflects an error on HTTP client stack.
 */
public class HttpClientException extends Exception {

    /**
     *
     */
    private static final long serialVersionUID = -691246443087249733L;

    /**
     * Construct a <code>HttpClientException</code> object with a detail message and a nested exception.
     *
     * @param message The detail message.
     */
    public HttpClientException(String message) {
        super(message);
    }

    /**
     * Construct a <code>HttpClientException</code> object with a detail message and a nested exception.
     *
     * @param message The detail message.
     * @param cause The nested exception
     */
    public HttpClientException(String message, Throwable cause) {
        super(message, cause);
    }

    /**
     * The cause of this exception can only be set when constructed.
     *
     * @param cause Cause of the exception.
     * @return This object.
     * @throws java.lang.IllegalStateException This method will always throw an <code>IllegalStateException</code> since the
     *             cause of this exception can only be set when constructed.
     */
    public synchronized Throwable initCause(Throwable cause) {
        throw new IllegalStateException();
    }
}
