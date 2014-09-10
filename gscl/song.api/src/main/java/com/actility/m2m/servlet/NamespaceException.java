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
 * id $Id: NamespaceException.java 6081 2013-10-15 13:33:50Z mlouiset $
 * author $Author: mlouiset $
 * version $Revision: 6081 $
 * lastrevision $Date: 2013-10-15 15:33:50 +0200 (Tue, 15 Oct 2013) $
 * modifiedby $LastChangedBy: mlouiset $
 * lastmodified $LastChangedDate: 2013-10-15 15:33:50 +0200 (Tue, 15 Oct 2013) $
 */

package com.actility.m2m.servlet;

/**
 * A NamespaceException is thrown to indicate a name conflict for the servlet registered in the SONG container.
 */
public class NamespaceException extends Exception {
    static final long serialVersionUID = 7235606031147877747L;
    /**
     * Nested exception.
     */
    private Throwable cause;

    /**
     * Construct a <code>NamespaceException</code> object with a detail message.
     *
     * @param message the detail message
     */
    public NamespaceException(String message) {
        super(message);
        cause = null;
    }

    /**
     * Construct a <code>NamespaceException</code> object with a detail message and a nested exception.
     *
     * @param message The detail message.
     * @param cause The nested exception.
     */
    public NamespaceException(String message, Throwable cause) {
        super(message);
        this.cause = cause;
    }

    /**
     * Returns the nested exception.
     *
     * <p>
     * This method predates the general purpose exception chaining mechanism. The {@link #getCause()} method is now the
     * preferred means of obtaining this information.
     *
     * @return the nested exception or <code>null</code> if there is no nested exception.
     */
    public Throwable getException() {
        return cause;
    }

    /**
     * Returns the cause of this exception or <code>null</code> if no cause was specified when this exception was created.
     *
     * @return The cause of this exception or <code>null</code> if no cause was specified.
     */
    public Throwable getCause() {
        return cause;
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
