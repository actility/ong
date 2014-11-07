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
 * Please contact Actility, SA.,  4, rue Ampère 22300 LANNION FRANCE
 * or visit www.actility.com if you need additional
 * information or have any questions.
 *
 * id $Id: $
 * author $Author: $
 * version $Revision: $
 * lastrevision $Date: $
 * modifiedby $LastChangedBy: $
 * lastmodified $LastChangedDate: $
 */

package com.actility.m2m.system.monitoring.alarm;

/**
 * This exception is released when an error occurred during alarm execution
 *
 * @author qdesrame
 *
 */
public final class AlarmExecutionException extends Exception {
    public static final int LEVEL_ERROR = 0;
    public static final int LEVEL_CLEAR = 1;
    /**
     *
     */
    private static final long serialVersionUID = -6688984152720760955L;

    private int level = 0;

    // public AlarmExecutionException(String message) {
    // super(message);
    // level = 0;
    // }
    public AlarmExecutionException(String message, int level) {
        super(message);
        this.level = level;
    }

    // public AlarmExecutionException(Throwable cause) {
    // super(cause);
    // this.level = 0;
    // }

    // public AlarmExecutionException(String message, Throwable cause) {
    // super(message, cause);
    // this.level = 0;
    // }
    public AlarmExecutionException(Throwable cause, int level) {
        super(cause);
        this.level = level;
    }

    public AlarmExecutionException(String message, Throwable cause, int level) {
        super(message, cause);
        this.level = level;
    }

    public int getLevel() {
        return this.level;
    }
}
