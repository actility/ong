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

public final class StatusCode {
    private static final int TYPE_OK = 0;
    private static final int TYPE_CLIENT_ERROR = 1;
    private static final int TYPE_SERVER_ERROR = 2;

    public static final StatusCode STATUS_OK = new StatusCode(0, TYPE_OK, "STATUS_OK");
    public static final StatusCode STATUS_CREATED = new StatusCode(1, TYPE_OK, "STATUS_CREATED");
    public static final StatusCode STATUS_ACCEPTED = new StatusCode(2, TYPE_OK, "STATUS_ACCEPTED");
    public static final StatusCode STATUS_BAD_REQUEST = new StatusCode(3, TYPE_CLIENT_ERROR, "STATUS_BAD_REQUEST");
    public static final StatusCode STATUS_PERMISSION_DENIED = new StatusCode(4, TYPE_CLIENT_ERROR, "STATUS_PERMISSION_DENIED");
    public static final StatusCode STATUS_FORBIDDEN = new StatusCode(5, TYPE_CLIENT_ERROR, "STATUS_FORBIDDEN");
    public static final StatusCode STATUS_NOT_FOUND = new StatusCode(6, TYPE_CLIENT_ERROR, "STATUS_NOT_FOUND");
    public static final StatusCode STATUS_METHOD_NOT_ALLOWED = new StatusCode(7, TYPE_CLIENT_ERROR, "STATUS_METHOD_NOT_ALLOWED");
    public static final StatusCode STATUS_NOT_ACCEPTABLE = new StatusCode(8, TYPE_CLIENT_ERROR, "STATUS_NOT_ACCEPTABLE");
    public static final StatusCode STATUS_REQUEST_TIMEOUT = new StatusCode(9, TYPE_CLIENT_ERROR, "STATUS_REQUEST_TIMEOUT");
    public static final StatusCode STATUS_CONFLICT = new StatusCode(10, TYPE_CLIENT_ERROR, "STATUS_CONFLICT");
    public static final StatusCode STATUS_UNSUPPORTED_MEDIA_TYPE = new StatusCode(11, TYPE_CLIENT_ERROR,
            "STATUS_UNSUPPORTED_MEDIA_TYPE");
    public static final StatusCode STATUS_INTERNAL_SERVER_ERROR = new StatusCode(12, TYPE_SERVER_ERROR,
            "STATUS_INTERNAL_SERVER_ERROR");
    public static final StatusCode STATUS_NOT_IMPLEMENTED = new StatusCode(13, TYPE_SERVER_ERROR, "STATUS_NOT_IMPLEMENTED");
    public static final StatusCode STATUS_BAD_GATEWAY = new StatusCode(14, TYPE_SERVER_ERROR, "STATUS_BAD_GATEWAY");
    public static final StatusCode STATUS_SERVICE_UNAVAILABLE = new StatusCode(15, TYPE_SERVER_ERROR,
            "STATUS_SERVICE_UNAVAILABLE");
    public static final StatusCode STATUS_GATEWAY_TIMEOUT = new StatusCode(16, TYPE_SERVER_ERROR, "STATUS_GATEWAY_TIMEOUT");
    public static final StatusCode STATUS_DELETED = new StatusCode(17, TYPE_CLIENT_ERROR, "STATUS_DELETED");
    public static final StatusCode STATUS_EXPIRED = new StatusCode(18, TYPE_CLIENT_ERROR, "STATUS_EXPIRED");

    private final int ordinal;
    private final int type;
    private final String name;

    /**
     * Builds a status code object from the given string and ordinal identifier.
     *
     * @param ordinal The ordinal identifier of the status code
     * @param name The string representation of the status code
     */
    private StatusCode(int ordinal, int type, String name) {
        this.ordinal = ordinal;
        this.type = type;
        this.name = name;
    }

    /**
     * Returns whether the status code represents a normal response
     *
     * @return Whether the status code represents a normal response
     */
    public boolean isOk() {
        return type == TYPE_OK;
    }

    /**
     * Returns whether the status code represents a client error
     *
     * @return Whether the status code represents a client error
     */
    public boolean isClientError() {
        return type == TYPE_CLIENT_ERROR;
    }

    /**
     * Returns whether the status code represents a server error
     *
     * @return Whether the status code represents a server error
     */
    public boolean isServerError() {
        return type == TYPE_SERVER_ERROR;
    }

    /**
     * Gets the name of this status code.
     *
     * @return The name of the status code
     */
    public String name() {
        return name;
    }

    /**
     * Gets the ordinal identifier of this status code.
     *
     * @return The ordinal identifier of the status code
     */
    public int ordinal() {
        return ordinal;
    }

    public boolean equals(Object value) {
        if (this == value) {
            return true;
        }
        if (value == null || StatusCode.class != value.getClass()) {
            return false;
        }
        return ordinal == ((StatusCode) value).ordinal;
    }

    public int hashCode() {
        return name.hashCode();
    }

    public String toString() {
        return name;
    }
}
