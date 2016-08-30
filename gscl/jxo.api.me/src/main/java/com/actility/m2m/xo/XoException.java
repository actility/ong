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

package com.actility.m2m.xo;

/**
 * Generic exception in the Xo service.
 */
public class XoException extends Exception {

    /**
     *
     */
    private static final long serialVersionUID = 4837367095530875564L;

    /**
     * Builds an Xo exception with the given message.
     *
     * @param aMessage The message of the exception
     */
    public XoException(String aMessage) {
        super(aMessage);
    }

    /**
     * Builds a Xo exception with the given message and {@link Throwable} root cause.
     *
     * @param aMessage The message of the exception
     * @param aCause The root cause of the exception
     */
    public XoException(String aMessage, Throwable aCause) {
        super(aMessage + " " + aCause.toString());
    }

    /**
     * Builds a Xo exception with the given root cause.
     *
     * @param aCause The root cause of the exception
     */
    public XoException(Throwable aCause) {
        super(aCause.toString());
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
