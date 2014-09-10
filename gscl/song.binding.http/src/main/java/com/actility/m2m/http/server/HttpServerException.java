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
 * id $Id: HttpServerException.java 8521 2014-04-14 09:05:59Z JReich $
 * author $Author: JReich $
 * version $Revision: 8521 $
 * lastrevision $Date: 2014-04-14 11:05:59 +0200 (Mon, 14 Apr 2014) $
 * modifiedby $LastChangedBy: JReich $
 * lastmodified $LastChangedDate: 2014-04-14 11:05:59 +0200 (Mon, 14 Apr 2014) $
 */

package com.actility.m2m.http.server;

import com.actility.m2m.m2m.StatusCode;


/**
 * A generic exception which reflects to a network error.
 */
public final class HttpServerException extends Exception {

    /**
     *
     */
    private static final long serialVersionUID = -278205927877333032L;

    /**
     * Status code of the exception
     */
    private int status;

    /**
     * The M2M status
     */
    private StatusCode m2mStatus;

    /**
     * Reason phrase of the exception
     */
    private final String reasonPhrase;

    /**
     * Construct a <code>HttpException</code> object with a detail message and a nested exception.
     *
     * @param message The detail message.
     * @param m2mStatus The M2M status
     * @param status The status code of exception
     * @param reasonPhrase The reason phrase of the exception
     */
    public HttpServerException(String message, StatusCode m2mStatus, int status, String reasonPhrase) {
        super(message);
        this.m2mStatus = m2mStatus;
        this.status = status;
        this.reasonPhrase = reasonPhrase;
    }

    /**
     * Construct a <code>HttpException</code> object with a detail message and a nested exception.
     *
     * @param message The detail message.
     * @param m2mStatus The M2M status
     * @param status The status code of exception
     * @param reasonPhrase The reason phrase of the exception
     * @param cause The nested exception
     */
    public HttpServerException(String message, StatusCode m2mStatus, int status, String reasonPhrase, Throwable cause) {
        super(message, cause);
        this.m2mStatus = m2mStatus;
        this.status = status;
        this.reasonPhrase = reasonPhrase;
    }

    /**
     * Gets the status code from this exception.
     *
     * @return The status code
     */
    public int getStatus() {
        return status;
    }

    /**
     * Gets the M2M status from this exception.
     *
     * @return The M2M status
     */
    public StatusCode getM2MStatus() {
        return m2mStatus;
    }

    /**
     * Gets the reason phrase from this exception.
     *
     * @return The reason phrase
     */
    public String getReasonPhrase() {
        return reasonPhrase;
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
