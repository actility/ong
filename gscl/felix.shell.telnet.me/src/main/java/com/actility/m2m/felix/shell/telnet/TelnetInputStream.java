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

package com.actility.m2m.felix.shell.telnet;

import java.io.FilterInputStream;
import java.io.IOException;
import java.io.InputStream;

/**
 * Reads an input stream and extracts telnet commands. When a command is found a callback is made to the telnet session.
 *
 * <p>
 * The following transformations are made in the data part of the input stream:<br>
 * <table>
 * <caption>Transformation mappings</caption>
 * <tr>
 * <th>From</th>
 * <th>To</th>
 * </tr>
 * <tr>
 * <td>CR null</td>
 * <td>CR</td>
 * </tr>
 * <tr>
 * <td>CR LF</td>
 * <td>CR</td>
 * </tr>
 * <tr>
 * <td>CR n</td>
 * <td>where n is not null or LF is discarded</td>
 * </tr>
 * <tr>
 * <td>IAC IAC</td>
 * <td>IAC</td>
 * </tr>
 * </table>
 *
 * While parsing telnet commands no transforms except IAC IAC to IAC are made.
 *
 * A state machine is used to parse the telnet commands.
 */

public class TelnetInputStream extends FilterInputStream {
    private TelnetStateMachine tsm;

    private int prevChar;

    private int thisChar;

    /*
     * public TelnetInputStream (InputStream is) { super(is); }
     */

    public TelnetInputStream(InputStream is, TelnetSession tels) {
        super(is);
        tsm = new TelnetStateMachine(tels);
        prevChar = -1;
    }

    public int available() throws IOException {
        return in.available();
    }

    public void close() throws IOException {
        in.close();
    }

    public void mark(int readlimit) {
        in.mark(readlimit);
    }

    public boolean markSupported() {
        return in.markSupported();
    }

    public int read() throws IOException {
        scanStream: while (true) {
            prevChar = thisChar;
            thisChar = in.read();

            if (thisChar == -1) {
                break scanStream;
            }
            if (prevChar == TelnetCommandCodes.IAC && thisChar == TelnetCommandCodes.IAC) {
                // Strip one double written IAC from input stream
                continue scanStream;
            }
            if (tsm.getState() == 0 && thisChar == TelnetCommandCodes.IAC) {
                // Detect command start
                tsm.nextState(tsm.getState(), thisChar);
                continue scanStream;
            }
            if (tsm.getState() != 0) {
                // Continue commad extraction
                tsm.nextState(tsm.getState(), thisChar);
                continue scanStream;
            }
            break scanStream;
        }
        return thisChar;
    }

    public int read(byte[] b) throws IOException {
        return read(b, 0, b.length);
    }

    public int read(byte[] b, int off, int len) throws IOException {
        int count = 0;
        int character = 0;
        while (count < (len - off) && in.available() > 0) {
            character = read();
            if (character == -1) {
                if (count != 0) {
                    return count;
                }
                return -1;
            }
            b[off + count] = (byte) character;
            count++;
        }
        return count;
    }

    public void reset() throws IOException {
        in.reset();
    }

    public long skip(long n) throws IOException {
        return in.skip(n);
    }
}
