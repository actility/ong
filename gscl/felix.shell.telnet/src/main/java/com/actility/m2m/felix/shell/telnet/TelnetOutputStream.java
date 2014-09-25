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
 * id $Id: TelnetOutputStream.java 9060 2014-07-09 15:43:26Z JReich $
 * author $Author: JReich $
 * version $Revision: 9060 $
 * lastrevision $Date: 2014-07-09 17:43:26 +0200 (Wed, 09 Jul 2014) $
 * modifiedby $LastChangedBy: JReich $
 * lastmodified $LastChangedDate: 2014-07-09 17:43:26 +0200 (Wed, 09 Jul 2014) $
 */

package com.actility.m2m.felix.shell.telnet;

import java.io.FilterOutputStream;
import java.io.IOException;
import java.io.OutputStream;

/**
 * This extension of FilterOutputStream makes the following translations on data in the output stream
 *
 * All characters in the output buffer are ended with a mask from the session. This mask is either 0x7F (7 bit) or 0xFF (8 bit)
 *
 * It also has a clear method to empty the output buffer and a writeCommand method to send telnet commands.
 *
 */
public class TelnetOutputStream extends FilterOutputStream {
    private TelnetSession ts;

    public TelnetOutputStream(OutputStream os, TelnetSession ts) {
        super(os);
        this.ts = ts;
    }

    public void write(byte[] b) throws IOException {
        write(b, 0, b.length);
    }

    public void write(byte[] b, int off, int len) throws IOException {
        byte mask = (byte) ts.getMask();
        byte[] filter = new byte[b.length + 1];
        int j = 0;
        for (int i = off; i < len; ++i) {
            b[i] = (byte) (b[i] & mask);
            if (b[i] == TelnetCommandCodes.CR) {
                if (i == (b.length - 1) || b[i + 1] != TelnetCommandCodes.LF) {
                    filter[j] = TelnetCommandCodes.CR;
                    ++j;
                    filter[j] = TelnetCommandCodes.LF;
                    ++j;
                    super.out.write(filter, 0, j);
                    j = 0;
                } else {
                    filter[j] = TelnetCommandCodes.CR;
                    ++j;
                    filter[j] = TelnetCommandCodes.LF;
                    ++j;
                    ++i;
                }
            } else if (b[i] == TelnetCommandCodes.LF) {
                filter[j] = TelnetCommandCodes.CR;
                ++j;
                filter[j] = TelnetCommandCodes.LF;
                ++j;
                super.out.write(filter, 0, j);
                j = 0;
            } else {
                filter[j] = (byte) (b[i] & mask);
                ++j;
            }
        }
        if (j != 0) {
            super.out.write(filter, 0, j);
        }
    }

    public void write(int b) throws IOException {
        byte mask = (byte) ts.getMask();
        super.write(b & mask);
    }

    /**
     * * Write a telnet command in the output stream. * *
     *
     * @parameter tc, command string to write
     */
    public synchronized void writeCommand(String tc) throws IOException {
        if (tc != null) {
            byte[] bytes = new byte[tc.length()];
            for (int i = 0; i < tc.length(); ++i) {
                bytes[i] = (byte) tc.charAt(i);
            }
            // Must be out to avoid telnet session filter on bytes
            super.out.write(bytes);
            flush();
        }
    }
}
