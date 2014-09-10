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
 * id $Id: XoParseException.java 6065 2013-10-14 12:36:17Z mlouiset $
 * author $Author: mlouiset $
 * version $Revision: 6065 $
 * lastrevision $Date: 2013-10-14 14:36:17 +0200 (Mon, 14 Oct 2013) $
 * modifiedby $LastChangedBy: mlouiset $
 * lastmodified $LastChangedDate: 2013-10-14 14:36:17 +0200 (Mon, 14 Oct 2013) $
 */

package com.actility.m2m.xo;

/**
 * Xo exception to declare a parsing exception while reading a Binary, XML, oBIX...
 */
public final class XoParseException extends XoException {

    /**
     *
     */
    private static final long serialVersionUID = 2549531277851331235L;

    /**
     * Builds an Xo exception with the given message.
     *
     * @param aMessage The message of the exception
     */
    public XoParseException(String aMessage) {
        super(aMessage);
    }

    /**
     * Builds a Xo exception with the given message and {@link Throwable} root cause.
     *
     * @param aMessage The message of the exception
     * @param aCause The root cause of the exception
     */
    public XoParseException(String aMessage, Throwable aCause) {
        super(aMessage, aCause);
    }

    /**
     * Builds a Xo exception with the given root cause.
     *
     * @param aCause The root cause of the exception
     */
    public XoParseException(Throwable aCause) {
        super(aCause);
    }
}
