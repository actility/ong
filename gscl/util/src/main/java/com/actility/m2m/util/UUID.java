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
 * id $Id: UUID.java 7256 2014-01-16 15:08:52Z JReich $
 * author $Author: JReich $
 * version $Revision: 7256 $
 * lastrevision $Date: 2014-01-16 16:08:52 +0100 (Thu, 16 Jan 2014) $
 * modifiedby $LastChangedBy: JReich $
 * lastmodified $LastChangedDate: 2014-01-16 16:08:52 +0100 (Thu, 16 Jan 2014) $
 */

package com.actility.m2m.util;

// PORTAGE Random
//import java.security.MessageDigest;
//import java.security.NoSuchAlgorithmException;
//import java.security.SecureRandom;
import java.util.Random;

/**
 * Utils class to generate unique identifiers.
 */
public final class UUID {

    private UUID() {
        // Static class
    }

    // private static final int HIGH_HALF_BYTE_FILTER = 0xf0;
    // private static final int LOW_HALF_BYTE_FILTER = 0x0f;
    // private static final int HALF_BYTE_LENGTH = 4;
    private static final char[] AUTHORIZED_DIGITS = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b', 'c', 'd',
            'e', 'f' };

    // PORTAGE Random
    // private static SecureRandom PRNG;
    // private static MessageDigest SHA;
    //
    // static {
    // try {
    // PRNG = SecureRandom.getInstance("SHA1PRNG");
    // SHA = MessageDigest.getInstance("SHA-1");
    // } catch (NoSuchAlgorithmException e) {
    // // Oups init failed
    // }
    // }
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
     // PORTAGE Random
        // String randomNum = Integer.toString(PRNG.nextInt());
        // byte[] randomBytes = randomNum.getBytes();
        // try {
        // byte[] seed = SHA.digest(randomBytes);
        // StringBuffer result = new StringBuffer();
        // int realLength = (length >> 1);
        // byte b = 0;
        // if (realLength > seed.length) {
        // realLength = seed.length;
        // }
        // for (int idx = 0; idx < realLength; ++idx) {
        // b = seed[idx];
        // result.append(AUTHORIZED_DIGITS[(b & HIGH_HALF_BYTE_FILTER) >> HALF_BYTE_LENGTH]);
        // result.append(AUTHORIZED_DIGITS[b & LOW_HALF_BYTE_FILTER]);
        // }
        // return result.toString();
        // } catch (RuntimeException e) {
        // // Prevent error from JDK 1.7
        // return randomNum;
        // }
        char[] result = new char[length];
        for (int i = 0; i < length; ++i) {
            result[i] = AUTHORIZED_DIGITS[RANDOM.nextInt(AUTHORIZED_DIGITS.length)];
        }
        return new String(result);
    }
}
