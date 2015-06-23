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
 * id $Id: ArrayUtils.java 7256 2014-01-16 15:08:52Z JReich $
 * author $Author: JReich $
 * version $Revision: 7256 $
 * lastrevision $Date: 2014-01-16 16:08:52 +0100 (Thu, 16 Jan 2014) $
 * modifiedby $LastChangedBy: JReich $
 * lastmodified $LastChangedDate: 2014-01-16 16:08:52 +0100 (Thu, 16 Jan 2014) $
 */

package com.actility.m2m.util;

/**
 * Utils class to manage arrays.
 */
public final class ArrayUtils {

    private ArrayUtils() {
        // Static class
    }

    /**
     * Appends an element to an array. This function builds a new array copy the given array and append the element.
     *
     * @param array The array that is going to be merge with the given element
     * @param elem The element to add to the given array
     * @return The built array which is a copy of the given array plus the given element
     */
    public static Object[] appendArray(Object[] array, Object elem) {
        Object[] newArray = new Object[array.length + 1];
        System.arraycopy(array, 0, newArray, 0, array.length);
        newArray[array.length] = elem;
        return newArray;
    }

    /**
     * Generate a string from an Array. Useful to generate logs.
     * <p>
     * Example:
     * <p>
     * { 10, 5, 3 } with prefix "[", suffix "]" and separator "-" : [10-5-3]
     *
     * @param array The array from which to generate a string
     * @param prefix The prefix of the generated string
     * @param suffix The suffix of the generated string
     * @param separator Separator to use between each element
     * @return The generated string.
     */
    public static String toString(Object[] array, String prefix, String suffix, String separator) {
        StringBuffer buffer = new StringBuffer();
        if (prefix != null) {
            buffer.append(prefix);
        }
        for (int i = 0; i < array.length - 1; ++i) {
            buffer.append(array[i].toString());
            buffer.append(separator);
        }
        if (array.length > 0) {
            buffer.append(array[array.length - 1].toString());
        }
        if (suffix != null) {
            buffer.append(suffix);
        }
        return buffer.toString();
    }

    /**
     * Generate a string from a byte array. Useful to generate logs.
     * <p>
     * Example:
     * <p>
     * { 0x00, 0xA0, 0x10 } with prefix "[", suffix "]" and separator "-" : [10-5-3]
     *
     * @param bytes The byte array from which to generate a string
     * @param prefix The prefix of the generated string
     * @param suffix The suffix of the generated string
     * @param separator Separator to use between each element
     * @return The generated string.
     */
    public static String toString(byte[] bytes, String prefix, String suffix, String separator) {
        StringBuffer buffer = new StringBuffer(prefix);
        for (int i = 0; i < bytes.length; i++) {
            if (i != 0) {
                buffer.append(separator);
            }
            buffer.append("0x" + Integer.toString(bytes[i] & 0xFF, 16));
        }
        buffer.append(suffix);
        return buffer.toString();
    }
}
