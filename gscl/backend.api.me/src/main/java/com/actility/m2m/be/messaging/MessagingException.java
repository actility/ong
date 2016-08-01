/*******************************************************************************
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
 *******************************************************************************/

package com.actility.m2m.be.messaging;

/**
 * Generic exception used to report messaging related errors in the Backend.
 */
public class MessagingException extends Exception {

    /**
     *
     */
    private static final long serialVersionUID = -8379480795117364260L;

    /**
     * Tells whether this exception is temporary and if the message could be resent later.
     * <p>
     * For example: queue is full.
     */
    private boolean temporary;

    /**
     * Create a new MessagingException.
     *
     * @param msg error detail
     */
    public MessagingException(String msg) {
        super(msg);
    }

    /**
     * Create a new MessagingException with the specified cause and error text.
     *
     * @param msg error detail
     * @param cause underlying error
     */
    public MessagingException(String msg, Throwable cause) {
        super(msg, cause);
    }

    /**
     * Create a new MessagingException with the specified cause.
     *
     * @param cause underlying error
     */
    public MessagingException(Throwable cause) {
        super(cause);
    }

    /**
     * Create a new MessagingException.
     *
     * @param msg error detail
     * @param temporary whether the error is temporary and the message can be retried later
     */
    public MessagingException(String msg, boolean temporary) {
        super(msg);
        this.temporary = temporary;
    }

    /**
     * Create a new MessagingException with the specified cause and error text.
     *
     * @param msg error detail
     * @param cause underlying error
     * @param temporary whether the error is temporary and the message can be retried later
     */
    public MessagingException(String msg, Throwable cause, boolean temporary) {
        super(msg, cause);
        this.temporary = temporary;
    }

    /**
     * Create a new MessagingException with the specified cause.
     *
     * @param cause underlying error
     * @param temporary whether the error is temporary and the message can be retried later
     */
    public MessagingException(Throwable cause, boolean temporary) {
        super(cause);
        this.temporary = temporary;
    }

    /**
     * Whether the error is temporary and the message can be retried later
     *
     * @return Whether the error is temporary
     */
    public boolean isTemporary() {
        return temporary;
    }
}
