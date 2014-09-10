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

package com.actility.m2m.util;

/**
 * Useful methods to match characters
 */
public final class CharacterUtils {

    private CharacterUtils() {
        // Static class
    }

    /**
     * Low bitmask for escaped characters:
     * <p>
     * The zero'th bit is used to indicate that escape pairs and non-US-ASCII characters are allowed; this is handled by the
     * scanEscape method below.
     */
    public static final long L_ESCAPED = 1L;
    /**
     * High bitmask for escaped characters:
     * <p>
     * The zero'th bit is used to indicate that escape pairs and non-US-ASCII characters are allowed; this is handled by the
     * scanEscape method below.
     */
    public static final long H_ESCAPED = 0L;

    /**
     * Low bitmask for digits characters:
     * <p>
     * digit = "0" | "1" | "2" | "3" | "4" | "5" | "6" | "7" | "8" | "9"
     */
    public static final long L_DIGIT = lowMask('0', '9');
    /**
     * High bitmask for digits characters:
     * <p>
     * digit = "0" | "1" | "2" | "3" | "4" | "5" | "6" | "7" | "8" | "9"
     */
    public static final long H_DIGIT = 0L;

    /**
     * Low bitmask for uppercase alpha characters:
     * <p>
     * upalpha = "A" | "B" | "C" | "D" | "E" | "F" | "G" | "H" | "I" | "J" | "K" | "L" | "M" | "N" | "O" | "P" | "Q" | "R" | "S"
     * | "T" | "U" | "V" | "W" | "X" | "Y" | "Z"
     */
    public static final long L_UPALPHA = 0L;
    /**
     * High bitmask for uppercase alpha characters:
     * <p>
     * upalpha = "A" | "B" | "C" | "D" | "E" | "F" | "G" | "H" | "I" | "J" | "K" | "L" | "M" | "N" | "O" | "P" | "Q" | "R" | "S"
     * | "T" | "U" | "V" | "W" | "X" | "Y" | "Z"
     */
    public static final long H_UPALPHA = highMask('A', 'Z');

    /**
     * Low bitmask for lowercase alpha characters:
     * <p>
     * lowalpha = "a" | "b" | "c" | "d" | "e" | "f" | "g" | "h" | "i" | "j" | "k" | "l" | "m" | "n" | "o" | "p" | "q" | "r" |
     * "s" | "t" | "u" | "v" | "w" | "x" | "y" | "z"
     */
    public static final long L_LOWALPHA = 0L;
    /**
     * High bitmask for lowercase alpha characters:
     * <p>
     * lowalpha = "a" | "b" | "c" | "d" | "e" | "f" | "g" | "h" | "i" | "j" | "k" | "l" | "m" | "n" | "o" | "p" | "q" | "r" |
     * "s" | "t" | "u" | "v" | "w" | "x" | "y" | "z"
     */
    public static final long H_LOWALPHA = highMask('a', 'z');

    /**
     * Low bitmask for alpha characters:
     * <p>
     * alpha = upalpha | lowalpha
     */
    public static final long L_ALPHA = L_LOWALPHA | L_UPALPHA;
    /**
     * High bitmask for alpha characters:
     * <p>
     * alpha = upalpha | lowalpha
     */
    public static final long H_ALPHA = H_LOWALPHA | H_UPALPHA;

    /**
     * Low bitmask for alpha-numeric characters:
     * <p>
     * alphanum = alpha | digit
     */
    public static final long L_ALPHANUM = L_DIGIT | L_ALPHA;
    /**
     * High bitmask for alpha-numeric characters:
     * <p>
     * alphanum = alpha | digit
     */
    public static final long H_ALPHANUM = H_DIGIT | H_ALPHA;

    /**
     * Low bitmask for hexadecimal characters:
     * <p>
     * hex = digit | "A" | "B" | "C" | "D" | "E" | "F" | "a" | "b" | "c" | "d" | "e" | "f"
     */
    public static final long L_HEX = L_DIGIT;
    /**
     * High bitmask for hexadecimal characters:
     * <p>
     * hex = digit | "A" | "B" | "C" | "D" | "E" | "F" | "a" | "b" | "c" | "d" | "e" | "f"
     */
    public static final long H_HEX = highMask('A', 'F') | highMask('a', 'f');

    /**
     * Array of hexadecimal characters
     */
    public final static char[] HEX_DIGITS = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F' };

    /**
     * Determines if the referenced character (Unicode code point) is an ISO control character. A character is considered to be
     * an ISO control character if its code is in the range <code>'&#92;u0000'</code> through <code>'&#92;u001F'</code> or in
     * the range <code>'&#92;u007F'</code> through <code>'&#92;u009F'</code>. Extract from jdk 1.5 sources
     *
     * @param character the character (Unicode code point) to be tested.
     * @return <code>true</code> if the character is an ISO control character; <code>false</code> otherwise.
     */
    public static boolean isISOControl(char character) {
        int codePoint = character;
        return (codePoint >= 0x0000 && codePoint <= 0x001F) || (codePoint >= 0x007F && codePoint <= 0x009F);
    }

    /**
     * Determines if the specified character is ISO-LATIN-1 white space. This method returns <code>true</code> for the following
     * five characters only:
     * <table>
     * <tr>
     * <td><code>'\t'</code></td>
     * <td><code>'&#92;u0009'</code></td>
     * <td><code>HORIZONTAL TABULATION</code></td>
     * </tr>
     * <tr>
     * <td><code>'\n'</code></td>
     * <td><code>'&#92;u000A'</code></td>
     * <td><code>NEW LINE</code></td>
     * </tr>
     * <tr>
     * <td><code>'\f'</code></td>
     * <td><code>'&#92;u000C'</code></td>
     * <td><code>FORM FEED</code></td>
     * </tr>
     * <tr>
     * <td><code>'\r'</code></td>
     * <td><code>'&#92;u000D'</code></td>
     * <td><code>CARRIAGE RETURN</code></td>
     * </tr>
     * <tr>
     * <td><code>'&nbsp;'</code></td>
     * <td><code>'&#92;u0020'</code></td>
     * <td><code>SPACE</code></td>
     * </tr>
     * </table>
     * Extract from jdk 1.5 sources ("isSpace" method with is deprecated since 1.5)
     *
     * @param character the character to be tested.
     * @return <code>true</code> if the character is ISO-LATIN-1 white space; <code>false</code> otherwise.
     * @see java.lang.Character#isSpaceChar(char)
     * @see java.lang.Character#isWhitespace(char)
     */
    public static boolean isSpaceChar(char character) {
        return (character <= 0x0020)
                && (((((1L << 0x0009) | (1L << 0x000A) | (1L << 0x000C) | (1L << 0x000D) | (1L << 0x0020)) >> character) & 1L) != 0);
    }

    /**
     * Builds the low bitmask from the given list of ASCII characters.
     * <p>
     * The bitmask is decomposed into two parts of 64 bits (long) to express all kind of ASCII characters
     *
     * @param chars The list of ASCII characters
     * @return The low bitmask
     */
    public static long lowMask(String chars) {
        int n = chars.length();
        long m = 0;
        for (int i = 0; i < n; i++) {
            char c = chars.charAt(i);
            if (c < 64) {
                m |= (1L << c);
            }
        }
        return m;
    }

    /**
     * Builds the high bitmask from the given list of ASCII characters.
     * <p>
     * The bitmask is decomposed into two parts of 64 bits (long) to express all kind of ASCII characters
     *
     * @param chars The list of ASCII characters
     * @return The high bitmask
     */
    public static long highMask(String chars) {
        int n = chars.length();
        long m = 0;
        for (int i = 0; i < n; i++) {
            char c = chars.charAt(i);
            if ((c >= 64) && (c < 128)) {
                m |= (1L << (c - 64));
            }
        }
        return m;
    }

    /**
     * Builds the low bitmask from the given inclusive range of ASCII characters.
     * <p>
     * The bitmask is decomposed into two parts of 64 bits (long) to express all kind of ASCII characters
     *
     * @param first The lower bound of the inclusive range of ASCII characters
     * @param last The upper bound of the inclusive range of ASCII characters
     * @return The low bitmask
     */
    public static long lowMask(char first, char last) {
        long m = 0;
        int f = Math.max(Math.min(first, 63), 0);
        int l = Math.max(Math.min(last, 63), 0);
        for (int i = f; i <= l; i++) {
            m |= 1L << i;
        }
        return m;
    }

    /**
     * Builds the high bitmask from the given inclusive range of ASCII characters.
     * <p>
     * The bitmask is decomposed into two parts of 64 bits (long) to express all kind of ASCII characters
     *
     * @param first The lower bound of the inclusive range of ASCII characters
     * @param last The upper bound of the inclusive range of ASCII characters
     * @return The high bitmask
     */
    public static long highMask(char first, char last) {
        long m = 0;
        int f = Math.max(Math.min(first, 127), 64) - 64;
        int l = Math.max(Math.min(last, 127), 64) - 64;
        for (int i = f; i <= l; i++) {
            m |= 1L << i;
        }
        return m;
    }

    /**
     * Checks whether the given char matches the given character set
     *
     * @param c The character to match
     * @param lowMask The low bitmask of authorized ASCII characters
     * @param highMask The high bitmask of authorized ASCII characters
     * @return Whether the character matches
     */
    public static boolean match(char c, long lowMask, long highMask) {
        if (c < 64) {
            return ((1L << c) & lowMask) != 0;
        }
        if (c < 128) {
            return ((1L << (c - 64)) & highMask) != 0;
        }
        return false;
    }
}
