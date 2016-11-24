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
 */

package com.actility.m2m.m2m;

/**
 * A generic exception which reflects to a network error.
 */
public final class M2MException extends Exception {

    /**
     *
     */
    private static final long serialVersionUID = -5335021583066991906L;

    /**
     * Status code of the exception
     */
    private final StatusCode status;

    /**
     * Construct a <code>ResourceException</code> object with a detail message.
     *
     * @param message the detail message
     * @param status The status code of the exception
     */
    public M2MException(String message, StatusCode status) {
        super(message);
        this.status = status;
    }

    /**
     * Construct a <code>ResourceException</code> object with a detail message and a nested exception.
     *
     * @param message The detail message.
     * @param status The status code of exception
     * @param cause The nested exception
     */
    public M2MException(String message, StatusCode status, Throwable cause) {
        super(message, cause);
        this.status = status;
    }

    /**
     * Gets the status code from this exception.
     *
     * @return The status code
     */
    public StatusCode getStatusCode() {
        return status;
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
