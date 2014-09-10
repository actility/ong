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
 * version $Revision: 17 $
 * lastrevision $Date: 2011-03-04 15:12:54 +0100 (ven., 04 mars 2011) $
 * modifiedby $LastChangedBy: $
 * lastmodified $LastChangedDate: 2011-03-04 15:12:54 +0100 (ven., 04 mars 2011) $
 */

package com.actility.maven.plugin.cocoon;

import java.text.ParseException;

/**
 * Utils class to parse a String object.
 */
public final class StringReader {
    private final String message;
    private final int length;
    private int offset;

    /**
     * Builds a StringReader from the given String to parse.
     *
     * @param message The String to parse
     */
    public StringReader(String message) {
        this.offset = 0;
        this.length = message.length();
        this.message = message;
    }

    /**
     * Skip the given offset of characters.
     *
     * @param offset The offset of characters to skip
     * @throws ParseException If the end of the String is reached
     */
    public void skipOffset(int offset) throws ParseException {
        if ((this.offset + offset) > length) {
            throw new ParseException(message, length);
        }
        this.offset += offset;
    }

    /**
     * Checks whether the end of String is reached.
     *
     * @return Whether the end of String is reached
     */
    public boolean isTerminated() {
        return offset == length;
    }

    /**
     * Returns the current character.
     *
     * @return The character at the current position
     * @throws ParseException If end of String is reached
     */
    public char readCurrent() throws ParseException {
        if (offset < length) {
            return message.charAt(offset);
        }
        throw new ParseException(message, offset);
    }

    /**
     * Returns the current character and go to the next one.
     *
     * @return The character at the current position
     * @throws ParseException If end of String is reached
     */
    public char readCurrentAndSkip() throws ParseException {
        if (offset < length) {
            char current = message.charAt(offset);
            ++offset;
            return current;
        }
        throw new ParseException(message, offset);
    }

    /**
     * Gets the String that is currently parsed.
     *
     * @return The parsed String
     */
    public String getMessage() {
        return message;
    }

    /**
     * Gets the current position in the String.
     *
     * @return The current position
     */
    public int getOffset() {
        return offset;
    }

    /**
     * Checks whether the end of String is reached and throw ParseException otherwise.
     *
     * @throws ParseException If the end of String is not reached
     */
    public void checkTerminated() throws ParseException {
        if (offset < length) {
            throw new ParseException(message, offset);
        }
    }

    /**
     * Checks whether the character at the current position is equal to the given character.
     *
     * @param expected The expected character
     * @throws ParseException If the end of String is reached or the character at the curret position is not equal to the given
     *             character
     */
    public void checkCurrent(char expected) throws ParseException {
        if (offset >= length || message.charAt(offset) != expected) {
            throw new ParseException(message, offset);
        }
    }

    /**
     * Checks whether the character at the current position is equal to one of the given characters.
     *
     * @param expected The expected possible characters
     * @throws ParseException If the end of String is reached or the character at the curret position is not equal to any of the
     *             given characters
     */
    public void checkCurrent(char[] expected) throws ParseException {
        if (offset < length) {
            for (int i = 0; i < expected.length; ++i) {
                if (message.charAt(offset) == expected[i]) {
                    return;
                }
            }
        }
        throw new ParseException(message, offset);
    }

    /**
     * Checks whether the character at the current position is equal to the given character and go to the next character.
     *
     * @param expected The expected character
     * @throws ParseException If the end of String is reached or the character at the curret position is not equal to the given
     *             character
     */
    public void checkCurrentAndSkip(char expected) throws ParseException {
        if (offset >= length || message.charAt(offset) != expected) {
            throw new ParseException(message, offset);
        }
        ++offset;
    }

    /**
     * Checks whether the character at the current position is equal to one of the given characters and go to the next
     * character.
     *
     * @param expected The expected possible characters
     * @throws ParseException If the end of String is reached or the character at the curret position is not equal to any of the
     *             given characters
     */
    public void checkCurrentAndSkip(char[] expected) throws ParseException {
        if (offset < length) {
            for (int i = 0; i < expected.length; ++i) {
                if (message.charAt(offset) == expected[i]) {
                    ++offset;
                    return;
                }
            }
        }
        throw new ParseException(message, offset);
    }

    /**
     * Skips a specific char.
     *
     * @param c The character to skip
     * @return The last seen character
     */
    public char skip(char c) {
        char current = 0;
        while (offset < length && (current = message.charAt(offset)) == c) {
            ++offset;
        }
        if (offset < length) {
            return current;
        }
        return 0;
    }

    /**
     * Skips until the char is found.
     *
     * @param c The character to find
     * @return the last seen character
     */
    public char skipUntil(char c) {
        char current = 0;
        while (offset < length && (current = message.charAt(offset)) != c) {
            ++offset;
        }
        if (offset < length) {
            return current;
        }
        return 0;
    }

    /**
     * Reads until the char is found.
     *
     * @param c The character to find
     * @return The read String
     */
    public String readUntil(char c) {
        int currentOffset = offset;
        while (offset < length && message.charAt(offset) != c) {
            ++offset;
        }
        return message.substring(currentOffset, offset);
    }

    /**
     * Reads until the char is found.
     *
     * @param buffer Buffer where to copy the read String
     * @param c The character to find
     * @return The length of the read String
     */
    public int readUntil(StringBuffer buffer, char c) {
        int currentOffset = offset;
        while (offset < length && message.charAt(offset) != c) {
            ++offset;
        }
        buffer.append(message.substring(currentOffset, offset));
        return offset - currentOffset;
    }

    /**
     * Skips the list of authorized characters.
     *
     * @param authorized The list of authorized characters
     * @return The last seen character
     */
    public char skip(char[] authorized) {
        boolean skip = true;
        char current = 0;
        while (offset < length && skip) {
            skip = false;
            current = message.charAt(offset);
            for (int k = 0; k < authorized.length; ++k) {
                if (authorized[k] == current) {
                    skip = true;
                    ++offset;
                    break;
                }
            }
        }
        if (offset < length) {
            return message.charAt(offset);
        }
        return 0;
    }

    /**
     * Skips the list of authorized characters.
     *
     * @param authorized The list of authorized characters as a String
     * @return The last seen character
     */
    public char skip(String authorized) {
        boolean skip = true;
        char current = 0;
        while (offset < length && skip) {
            skip = false;
            current = message.charAt(offset);
            for (int k = 0; k < authorized.length(); ++k) {
                if (authorized.charAt(k) == current) {
                    skip = true;
                    ++offset;
                    break;
                }
            }
        }
        if (offset < length) {
            return message.charAt(offset);
        }
        return 0;
    }

    /**
     * Skips until an unauthorized character is found.
     *
     * @param unauthorized THe list of unauthorized characters
     * @return The last seen character
     */
    public char skipUntil(char[] unauthorized) {
        boolean skip = true;
        char current = 0;
        while (offset < length && skip) {
            current = message.charAt(offset);
            for (int k = 0; k < unauthorized.length; ++k) {
                if (unauthorized[k] == current) {
                    skip = false;
                    break;
                }
            }
            if (skip) {
                ++offset;
            }
        }
        if (offset < length) {
            return current;
        }
        return 0;
    }

    /**
     * Skips until an unauthorized character is found.
     *
     * @param unauthorized The list of unauthorized characters as a String
     * @return The last seen character
     */
    public char skipUntil(String unauthorized) {
        boolean skip = true;
        char current = 0;
        while (offset < length && skip) {
            current = message.charAt(offset);
            for (int k = 0; k < unauthorized.length(); ++k) {
                if (unauthorized.charAt(k) == current) {
                    skip = false;
                    break;
                }
            }
            if (skip) {
                ++offset;
            }
        }
        if (offset < length) {
            return current;
        }
        return 0;
    }

    /**
     * Reads the list of authorized characters.
     *
     * @param authorized The list of authorized characters
     * @return The read string
     */
    public String read(char[] authorized) {
        int currentOffset = offset;
        boolean read = true;
        char current = 0;
        while (offset < length && read) {
            read = false;
            current = message.charAt(offset);
            for (int k = 0; k < authorized.length; ++k) {
                if (authorized[k] == current) {
                    read = true;
                    ++offset;
                    break;
                }
            }
        }
        return message.substring(currentOffset, offset);
    }

    /**
     * Reads the list of authorized characters.
     *
     * @param buffer Buffer where to copy the read String
     * @param authorized The list of authorized characters
     * @return THe length of the read String
     */
    public int read(StringBuffer buffer, char[] authorized) {
        int currentOffset = offset;
        boolean read = true;
        char current = 0;
        while (offset < length && read) {
            read = false;
            current = message.charAt(offset);
            for (int k = 0; k < authorized.length; ++k) {
                if (authorized[k] == current) {
                    read = true;
                    ++offset;
                    break;
                }
            }
        }
        buffer.append(message.substring(currentOffset, offset));
        return offset - currentOffset;
    }

    /**
     * Reads the list of authorized characters.
     *
     * @param authorized The list of authorized characters as a String
     * @return The read string
     */
    public String read(String authorized) {
        int currentOffset = offset;
        boolean read = true;
        char current = 0;
        while (offset < length && read) {
            read = false;
            current = message.charAt(offset);
            for (int k = 0; k < authorized.length(); ++k) {
                if (authorized.charAt(k) == current) {
                    read = true;
                    ++offset;
                    break;
                }
            }
        }
        return message.substring(currentOffset, offset);
    }

    /**
     * Reads the list of authorized characters.
     *
     * @param buffer Buffer where to copy the read String
     * @param authorized The list of authorized characters as a String
     * @return The length of the read String
     */
    public int read(StringBuffer buffer, String authorized) {
        int currentOffset = offset;
        boolean read = true;
        char current = 0;
        while (offset < length && read) {
            read = false;
            current = message.charAt(offset);
            for (int k = 0; k < authorized.length(); ++k) {
                if (authorized.charAt(k) == current) {
                    read = true;
                    ++offset;
                    break;
                }
            }
        }
        buffer.append(message.substring(currentOffset, offset));
        return offset - currentOffset;
    }

    /**
     * Reads until an unauthorized character is found.
     *
     * @param unauthorized THe list of unauthorized characters
     * @return The read String
     */
    public String readUntil(char[] unauthorized) {
        int currentOffset = offset;
        boolean read = true;
        char current = 0;
        while (offset < length && read) {
            current = message.charAt(offset);
            for (int k = 0; k < unauthorized.length; ++k) {
                if (unauthorized[k] == current) {
                    read = false;
                    break;
                }
            }
            if (read) {
                ++offset;
            }
        }
        return message.substring(currentOffset, offset);
    }

    /**
     * Reads until an unauthorized character is found.
     *
     * @param buffer Buffer where to copy the read String
     * @param unauthorized The list of unauthorized characters
     * @return The length of the read String
     */
    public int readUntil(StringBuffer buffer, char[] unauthorized) {
        int currentOffset = offset;
        boolean read = true;
        char current = 0;
        while (offset < length && read) {
            current = message.charAt(offset);
            for (int k = 0; k < unauthorized.length; ++k) {
                if (unauthorized[k] == current) {
                    read = false;
                    break;
                }
            }
            if (read) {
                ++offset;
            }
        }
        buffer.append(message.substring(currentOffset, offset));
        return offset - currentOffset;
    }

    /**
     * Reads until an unauthorized character is found
     *
     * @param unauthorized The list of unauthorized characters as a String
     * @return The read String
     */
    public String readUntil(String unauthorized) {
        int currentOffset = offset;
        boolean read = true;
        char current = 0;
        while (offset < length && read) {
            current = message.charAt(offset);
            for (int k = 0; k < unauthorized.length(); ++k) {
                if (unauthorized.charAt(k) == current) {
                    read = false;
                    break;
                }
            }
            if (read) {
                ++offset;
            }
        }
        return message.substring(currentOffset, offset);
    }

    /**
     * Reads until an unauthorized character is found
     *
     * @param buffer Buffer where to copy the read String
     * @param unauthorized The list of unauthorized characters
     * @return The length of the read String
     */
    public int readUntil(StringBuffer buffer, String unauthorized) {
        int currentOffset = offset;
        boolean read = true;
        char current = 0;
        while (offset < length && read) {
            current = message.charAt(offset);
            for (int k = 0; k < unauthorized.length(); ++k) {
                if (unauthorized.charAt(k) == current) {
                    read = false;
                    break;
                }
            }
            if (read) {
                ++offset;
            }
        }
        buffer.append(message.substring(currentOffset, offset));
        return offset - currentOffset;
    }

    /**
     * Reads a long value from the current position and until a none digit character is found.
     *
     * @return The read long value
     * @throws ParseException If the end of String is reached
     */
    public long readLong() throws ParseException {
        if (isTerminated()) {
            throw new ParseException(message, offset);
        }
        long result = 0;
        while (offset < length && message.charAt(offset) >= '0' && message.charAt(offset) <= '9') {
            result *= 10;
            result += message.charAt(offset) - '0';
            ++offset;
        }
        return result;
    }

    /**
     * Reads a long value from the given position and until a none digit character is found, and check the long value length (in
     * number of characters).
     *
     * @param length The expected length for the long value
     * @return The read long value
     * @throws ParseException If the end of String is reached or if the long value does not have the expected length
     */
    public long readLongCheckLength(int length) throws ParseException {
        if (isTerminated()) {
            throw new ParseException(message, offset);
        }
        long result = 0;
        int beginOffset = offset;
        while (offset < this.length && message.charAt(offset) >= '0' && message.charAt(offset) <= '9') {
            result *= 10;
            result += message.charAt(offset) - '0';
            ++offset;
        }
        if ((offset - beginOffset) != length) {
            throw new ParseException(message, offset);
        }
        return result;
    }

    /**
     * Reads an int value from the current position and until a none digit character is found.
     *
     * @return The read int value
     * @throws ParseException If the end of String is reached
     */
    public int readInt() throws ParseException {
        if (isTerminated()) {
            throw new ParseException(message, offset);
        }
        int result = 0;
        while (offset < length && message.charAt(offset) >= '0' && message.charAt(offset) <= '9') {
            result *= 10;
            result += message.charAt(offset) - '0';
            ++offset;
        }
        return result;
    }

    /**
     * Reads a int value from the given position and until a none digit character is found, and check the int value length (in
     * number of characters).
     *
     * @param length The expected length for the int value
     * @return The read int value
     * @throws ParseException If the end of String is reached or if the int value does not have the expected length
     */
    public int readIntAndCheckLength(int length) throws ParseException {
        if (isTerminated()) {
            throw new ParseException(message, offset);
        }
        int result = 0;
        int beginOffset = offset;
        while (offset < this.length && message.charAt(offset) >= '0' && message.charAt(offset) <= '9') {
            result *= 10;
            result += message.charAt(offset) - '0';
            ++offset;
        }
        if ((offset - beginOffset) != length) {
            throw new ParseException(message, offset);
        }
        return result;
    }
}
