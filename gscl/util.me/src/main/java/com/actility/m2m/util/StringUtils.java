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

package com.actility.m2m.util;

import java.io.UnsupportedEncodingException;
import java.util.ArrayList;
import java.util.List;

/**
 * Utils class for String.
 */
public final class StringUtils {

    private StringUtils() {
        // Static class
    }

    /**
     * Checks whether the given String is empty (null or "").
     *
     * @param value THe String to check
     * @return Whether the given String is empty
     */
    public static boolean isEmpty(String value) {
        return value == null || "".equals(value);
    }

    /**
     * Splits the given String each time the given separator is found.
     *
     * @param values The list where to add values
     * @param value The value to split must not be <code>null</code>
     * @param separator The list separator
     * @param ignoreEmpty Whether to ignore empty strings in the list
     */
    public static void listSplit(List values, String value, char separator, boolean ignoreEmpty) {

        int firstNotSepIndex = 0;
        int sepIndex = 0;
        while (firstNotSepIndex < value.length() && value.charAt(firstNotSepIndex) == separator) {
            if (!ignoreEmpty) {
                values.add("");
            }
            ++firstNotSepIndex;
        }
        sepIndex = value.indexOf(separator, firstNotSepIndex);
        while (sepIndex != -1) {
            values.add(value.substring(firstNotSepIndex, sepIndex));
            firstNotSepIndex = sepIndex + 1;
            while (firstNotSepIndex < value.length() && value.charAt(firstNotSepIndex) == separator) {
                if (!ignoreEmpty) {
                    values.add("");
                }
                ++firstNotSepIndex;
            }
            sepIndex = value.indexOf(separator, firstNotSepIndex);
        }
        if (firstNotSepIndex < value.length() || (!ignoreEmpty && firstNotSepIndex == value.length())) {
            values.add(value.substring(firstNotSepIndex));
        }
    }

    /**
     * Splits the given String each time the given separator is found.
     *
     * @param value The value to split must not be <code>null</code>
     * @param separator The list separator
     * @param ignoreEmpty Whether to ignore empty strings in the list
     * @return The splitted String as an array
     */
    public static String[] split(String value, char separator, boolean ignoreEmpty) {
        List values = new ArrayList();
        listSplit(values, value, separator, ignoreEmpty);
        return (String[]) values.toArray(new String[values.size()]);
    }

    private static void appendEscape(StringBuffer sb, byte b) {
        sb.append('%');
        sb.append(CharacterUtils.HEX_DIGITS[(b >> 4) & 0x0f]);
        sb.append(CharacterUtils.HEX_DIGITS[(b >> 0) & 0x0f]);
    }

    private static void appendEncoded(StringBuffer sb, char c) {
        try {
            String str = "" + c;
            byte bbuf[] = str.getBytes("UTF-8");
            for (int i = 0; i < bbuf.length; i++) {
                int b = bbuf[i] & 0xff;
                if (b >= 0x80) {
                    appendEscape(sb, (byte) b);
                } else {
                    sb.append((char) b);
                }
            }
        } catch (UnsupportedEncodingException ex) {
            IllegalArgumentException e = new IllegalArgumentException();
            e.initCause(ex);
            throw e;
        }
    }

    /**
     * Encodes string characters that do not match the given authorized character set
     *
     * @param s The string to encode
     * @param lowMask The low bitmask of authorized ASCII mask
     * @param highMask The high bitmask of authorized ASCII mask
     * @return The encoded string
     */
    public static String quote(String s, long lowMask, long highMask) {
        int n = s.length();
        StringBuffer sb = null;
        boolean allowNonASCII = ((lowMask & CharacterUtils.L_ESCAPED) != 0);
        for (int i = 0; i < n; i++) {
            char c = s.charAt(i);
            if (c < '\u0080') {
                if (!CharacterUtils.match(c, lowMask, highMask)) {
                    if (sb == null) {
                        sb = new StringBuffer();
                        sb.append(s.substring(0, i));
                    }
                    appendEscape(sb, (byte) c);
                } else {
                    if (sb != null) {
                        sb.append(c);
                    }
                }
            } else if (allowNonASCII && (CharacterUtils.isSpaceChar(c) || CharacterUtils.isISOControl(c))) {
                if (sb == null) {
                    sb = new StringBuffer();
                    sb.append(s.substring(0, i));
                }
                appendEncoded(sb, c);
            } else {
                if (sb != null) {
                    sb.append(c);
                }
            }
        }
        return (sb == null) ? s : sb.toString();
    }
}
