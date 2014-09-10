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
 * id $Id: TelnetOutputStream.java 8130 2014-03-19 16:33:07Z JReich $
 * author $Author: JReich $
 * version $Revision: 8130 $
 * lastrevision $Date: 2014-03-19 17:33:07 +0100 (Wed, 19 Mar 2014) $
 * modifiedby $LastChangedBy: JReich $
 * lastmodified $LastChangedDate: 2014-03-19 17:33:07 +0100 (Wed, 19 Mar 2014) $
 */

package com.actility.m2m.felix.shell.telnet;

import java.io.ByteArrayOutputStream;
import java.io.DataOutputStream;
import java.io.FilterOutputStream;
import java.io.IOException;
import java.io.OutputStream;

/**
 * * This extension of FilterOutputStream makes the following * translations on data in the output stream * * CR LF to no change
 * * CR n, when n != LF to CR NULL n * n LF to n CR LF * * All characters in the output buffer are * anded with a mask from the
 * session. * This mask is either 0x7F (7 bit) or 0xFF (8 bit) * * It also has a clear method to empty the output buffer * and a
 * writeCommand method to send telnet commands. *
 */

public class TelnetOutputStream extends FilterOutputStream {
    private TelnetSession ts;

    private ByteArrayOutputStream baos; // Intermediate byte array stream

    private DataOutputStream dos2; // Internal DataOutputStream

    private boolean prevWasCR = false; // True if previous char was CR

    public TelnetOutputStream(OutputStream os, TelnetSession ts) {
        super(os);
        this.ts = ts;
        OutputStream os1 = super.out; // get the underlaying stream
        baos = new ByteArrayOutputStream(); // intermediate array stream
        super.out = baos; // replace underlying Stream with a
                          // ByteArrayOutputStream
        dos2 = new DataOutputStream(os1); // a new OutputStream that uses the
        // original output stream
    }

    /**
     * * Override flush and close methods,
     */

    public void flush() throws IOException {
        scanBuf();
        dos2.flush();
    }

    public void close() throws IOException {
        flush();
        baos.close();
        dos2.close();
    }

    /**
     * * Override all write methods, add CRLF when appropriate
     */

    public void write(byte[] b) throws IOException {
        baos.write(b);
    }

    public void write(byte[] b, int off, int len) {
        baos.write(b, off, len);
    }

    public void write(int b) {
        baos.write(b);
    }

    /**
     * * Scan the output buffer for: * * CR LF to no change * CR n, when n != LF to CR NULL n * n LF to n CR LF *
     */

    private synchronized void scanBuf() throws IOException {
        byte[] buf = baos.toByteArray();
        baos.reset();
        byte mask = (byte) ts.getMask();

        for (int i = 0; i < buf.length; i++) {
            if (prevWasCR == true) {
                if (buf[i] != (byte) TelnetCommandCodes.LF) { // add a LF
                    dos2.write((byte) TelnetCommandCodes.LF);
                    dos2.write(buf[i] & mask);
                } else {
                    dos2.write(buf[i] & mask);
                }
            } else {
                if (buf[i] == (byte) TelnetCommandCodes.LF) { // a single LF -> CRLF
                    dos2.write((byte) TelnetCommandCodes.CR);
                    dos2.write((byte) TelnetCommandCodes.LF);
                } else {
                    dos2.write(buf[i] & mask);
                }
            }
            prevWasCR = (buf[i] == (byte) TelnetCommandCodes.CR) ? true : false;
        }
    }

    public synchronized void reset() {
        baos.reset();
    }

    /**
     * * Write a telnet command in the output stream. * *
     *
     * @parameter tc, command string to write
     */

    public synchronized void writeCommand(String tc) throws IOException {
        if (tc != null) {
            int len = tc.length();
            for (int i = 0 ; i < len ; i++) {
                dos2.writeByte((byte)tc.charAt(i));
            }

            dos2.flush();
        }
    }
}
