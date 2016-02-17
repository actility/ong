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
 * id $Id: $
 * author $Author: $
 * version $Revision: $
 * lastrevision $Date: $
 * modifiedby $LastChangedBy: $
 * lastmodified $LastChangedDate: $
 */

package com.actility.m2m.log.impl;

import org.osgi.service.log.LogService;

/**
 * Utility class for the LogService interface.
 *
 * It exports methods that translates between the numeric values of the severity level constants and human readable strings.
 */
public class LogUtils {
    private LogUtils() {
        // Static class
    }

    /**
     * Converts from a numeric log severity level to a left justified string of at least the given length.
     *
     * @param sb The string buffer in which to append the log
     * @param level is the log severity level.
     * @param padding Padding that determine the final level length
     */
    public static void fromLevel(StringBuffer sb, int level, String padding) {
        switch (level) {
        case LogService.LOG_INFO:
            sb.append("info");
            break;
        case LogService.LOG_DEBUG:
            sb.append("debug");
            break;
        case LogService.LOG_WARNING:
            sb.append("Warning");
            break;
        case LogService.LOG_ERROR:
            sb.append("ERROR");
            break;
        case 0:
            sb.append("DEFAULT");
            break;
        default:
            sb.append('[');
            sb.append(level);
            sb.append(']');
        }
        sb.append(padding);
        sb.setLength(padding.length());
    }

    /**
     * Converts a string representing a log severity level to an int.
     *
     * @param level The string to convert.
     * @param def Default value to use if the string is not recognized as a log level.
     * @return the log level, or the default value if the string can not be recognized.
     */
    public static int toLevel(String level, int def) {
        if (level.equalsIgnoreCase("INFO")) {
            return LogService.LOG_INFO;
        } else if (level.equalsIgnoreCase("DEBUG")) {
            return LogService.LOG_DEBUG;
        } else if (level.equalsIgnoreCase("WARNING")) {
            return LogService.LOG_WARNING;
        } else if (level.equalsIgnoreCase("ERROR")) {
            return LogService.LOG_ERROR;
        } else if (level.equalsIgnoreCase("DEFAULT")) {
            return 0;
        }
        return def;
    }

}
