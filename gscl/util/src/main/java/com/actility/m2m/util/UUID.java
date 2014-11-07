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
 * id $Id: UUID.java 9313 2014-08-21 10:19:57Z JReich $
 * author $Author: JReich $
 * version $Revision: 9313 $
 * lastrevision $Date: 2014-08-21 12:19:57 +0200 (Thu, 21 Aug 2014) $
 * modifiedby $LastChangedBy: JReich $
 * lastmodified $LastChangedDate: 2014-08-21 12:19:57 +0200 (Thu, 21 Aug 2014) $
 */

package com.actility.m2m.util;

import java.util.Random;

/**
 * Utils class to generate unique identifiers.
 */
public final class UUID {

    private UUID() {
        // Static class
    }

    private static final char[] AUTHORIZED_DIGITS = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b', 'c', 'd',
            'e', 'f' };

    private static final Random RANDOM = new Random(System.currentTimeMillis());

    /**
     * Generates a unique identifier of the given length.
     * <p>
     * Maximum authorized value for length is 40.
     *
     * @param length The requested identifier length
     * @return The generated unique identifier
     */
    public static String randomUUID(int length) {
        char[] result = new char[length];
        for (int i = 0; i < length; ++i) {
            result[i] = AUTHORIZED_DIGITS[RANDOM.nextInt(AUTHORIZED_DIGITS.length)];
        }
        return new String(result);
    }
}
