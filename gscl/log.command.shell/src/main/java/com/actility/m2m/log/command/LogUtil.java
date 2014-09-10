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
 * id $Id: CmLoader.java 6387 2013-11-07 13:37:35Z mlouiset $
 * author $Author: mlouiset $
 * version $Revision: 6387 $
 * lastrevision $Date: 2013-11-07 14:37:35 +0100 (Thu, 07 Nov 2013) $
 * modifiedby $LastChangedBy: mlouiset $
 * lastmodified $LastChangedDate: 2013-11-07 14:37:35 +0100 (Thu, 07 Nov 2013) $
 */

package com.actility.m2m.log.command;

import java.util.Dictionary;

import org.osgi.framework.Bundle;


/**
 * Utility class for the LogService interface.
 *
 * It exports methods that translates between the numeric values of the severity level constants and human readable strings.
 */
public final class LogUtil {

    /**
     * Converts from a numeric log severity level to a left justified string of at least the given length.
     *
     * @param level is the log severity level.
     * @param length the minimum length of the resulting string.
     * @return left justified string representation of a numeric log level.
     */
    public static String formatLevel(String level, int length) {
        StringBuffer sb = new StringBuffer(length > 7 ? length : 7);
        sb.append(level);
        for (int i = sb.length(); i < length; i++) {
            sb.append(" ");
        }
        return sb.toString();
    }

    public static String checkLevel(String level) {
        if (level.equalsIgnoreCase("info")) {
            return "info";
        } else if (level.equalsIgnoreCase("debug")) {
            return "debug";
        } else if (level.equalsIgnoreCase("warning")) {
            return "warning";
        } else if (level.equalsIgnoreCase("error")) {
            return "error";
        } else if (level.equalsIgnoreCase("default")) {
            return "default";
        }
        return null;
    }

    /**
     * Get the symbolic name of the specified bundle. All directives and parameters attached to the symbolic name attribute will
     * be stripped.
     *
     * @param bundle the bundle
     * @return The bundles symbolic name or null if not specified.
     */
    public static String getSymbolicName(Bundle bundle) {
        Dictionary d = bundle.getHeaders();
        String bsn = (String) d.get("Bundle-SymbolicName");
        if (bsn != null && bsn.length() > 0) {
            // Remove parameters and directives from the value
            int semiPos = bsn.indexOf(';');
            if (-1 < semiPos) {
                bsn = bsn.substring(0, semiPos).trim();
            }
        } else {
            bsn = bundle.getLocation();
        }
        return bsn;
    }
}
