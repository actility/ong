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
 * id $Id: ServletParseException.java 6081 2013-10-15 13:33:50Z mlouiset $
 * author $Author: mlouiset $
 * version $Revision: 6081 $
 * lastrevision $Date: 2013-10-15 15:33:50 +0200 (Tue, 15 Oct 2013) $
 * modifiedby $LastChangedBy: mlouiset $
 * lastmodified $LastChangedDate: 2013-10-15 15:33:50 +0200 (Tue, 15 Oct 2013) $
 */

package com.actility.m2m.servlet.song;

/**
 * Thrown by the container when an application attempts to parse a malformed header or addressing structure. See Also:Serialized
 * Form
 */
public class ServletParseException extends javax.servlet.ServletException {
    private static final long serialVersionUID = -8754215391929713818L;

    /**
     * Constructs a new parse exception, without any message.
     */
    public ServletParseException() {
        super();
    }

    /**
     * Constructs a new parse exception with the specified message.
     * 
     * @param msg A String specifying the text of the exception message
     */
    public ServletParseException(final java.lang.String msg) {
        super(msg);
    }

    /**
     * Constructs a new parse exception with the specified detail message and cause. Note that the detail message associated
     * with cause is not automatically incorporated in this exception's detail message.
     * 
     * @param message The detail message (which is saved for later retrieval by the {@link Throwable#getMessage()} method)
     * @param cause The cause (which is saved for later retrieval by the {@link Throwable#getCause()} method). (A null value is
     *            permitted, and indicates that the cause is nonexistent or unknown.)
     */
    public ServletParseException(final java.lang.String message, final java.lang.Throwable cause) {
        super(message, cause);
    }

    /**
     * Constructs a new parse exception with the specified cause and a detail message of (cause==null ? null : cause.toString())
     * (which typically contains the class and detail message of cause). This constructor is useful for exceptions that are
     * little more than wrappers for other throwables.
     * 
     * @param cause the cause (which is saved for later retrieval by the Throwable.getCause() method). (A null value is
     *            permitted, and indicates that the cause is nonexistent or unknown.)
     */
    public ServletParseException(final java.lang.Throwable cause) {
        super(cause);
    }

}
