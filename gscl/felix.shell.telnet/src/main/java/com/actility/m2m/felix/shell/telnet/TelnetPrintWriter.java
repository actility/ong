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
 * id $Id: TelnetPrintWriter.java 8131 2014-03-19 16:35:37Z JReich $
 * author $Author: JReich $
 * version $Revision: 8131 $
 * lastrevision $Date: 2014-03-19 17:35:37 +0100 (Wed, 19 Mar 2014) $
 * modifiedby $LastChangedBy: JReich $
 * lastmodified $LastChangedDate: 2014-03-19 17:35:37 +0100 (Wed, 19 Mar 2014) $
 */

package com.actility.m2m.felix.shell.telnet;

import java.io.CharArrayWriter;
import java.io.IOException;
import java.io.OutputStream;
import java.io.PrintWriter;
import java.io.Writer;

/**
 * * This extension of a PrintWriter makes the following * translations on data in the output stream * * CR LF -> no change * CR
 * <n>, when n != LF -> CR NULL <n> * <n> LF -> <n> CR LF * * All characters in the output buffer are * anded with a mask from
 * the session. * This mask is either 0x7F (7 bit) or 0xFF (8 bit) * * It also has a clear method to empty the output buffer *
 * and a writeCommand method to send telnet commands. *
 */

public class TelnetPrintWriter extends PrintWriter {
    private TelnetSession ts;

    private CharArrayWriter caw; // Intermediate array writer

    private PrintWriter pw2; // Private PrintWriter

    private boolean prevWasCR = false;

    public TelnetPrintWriter(OutputStream os, TelnetStateMachine tsm, TelnetSession ts) {
        super(os);
        this.ts = ts;

        Writer w1 = super.out; // get the underlaying writer
        caw = new CharArrayWriter(); // intermediate array writer
        super.out = caw; // replace underlying Writer with a CharArrayWriter
        pw2 = new PrintWriter(w1); // a private PrintWriter
    }

    /**
     * * Override flusg and close methods,
     */

    public void flush() {
        caw.flush();
        pribuf();
        pw2.flush();
    }

    public void close() {
        flush();
        caw.close();
        pw2.close();
    }

    /**
     * * Override all println methods, add CRLF as end of line
     */

    public synchronized void println() {
        write(TelnetCommandCodes.CR);
        write(TelnetCommandCodes.LF);
        flush();
    }

    public synchronized void println(boolean b) {
        super.print(b);
        this.println();
    }

    public synchronized void println(char c) {
        synchronized (this) {
            super.print(c);
            this.println();
        }
    }

    public synchronized void println(int i) {
        super.print(i);
        this.println();
    }

    public synchronized void println(long l) {
        super.print(l);
        this.println();
    }

    public synchronized void println(float f) {
        super.print(f);
        this.println();
    }

    public synchronized void println(double d) {
        super.print(d);
        this.println();
    }

    public synchronized void println(char[] c) {
        super.print(c);
        this.println();
    }

    public synchronized void println(String s) {
        synchronized (this) {
            super.print(s);
            this.println();
        }
    }

    public synchronized void println(Object o) {
        super.print(o);
        this.println();
    }

    /**
     * * Override all write methods, write to internal buffer first
     */
    public synchronized void write(char[] buf) {
        caw.write(buf, 0, buf.length);
    }

    public synchronized void write(char[] buf, int off, int len) {
        caw.write(buf, off, len);
    }

    public synchronized void write(String s) {
        synchronized (this) {
            try {
                caw.write(s);
            } catch (IOException iox) {
                System.out.println("Exception in TelnetPrintWriter write (String) " + iox.toString());
            }
        }
    }

    public synchronized void write(int c) {
        caw.write(c);
    }

    public synchronized void write(String s, int off, int len) {
        caw.write(s, off, len);
    }

    /**
     * * Scan the output buffer for CR<n> where n != LF, * which is translated it to CR NULL n
     */
    private synchronized void pribuf() {
        char[] buf = caw.toCharArray();
        caw.reset();
        char mask = ts.getMask();

        for (int i = 0; i < buf.length; i++) {
            if (prevWasCR == true) {
                if (buf[i] != TelnetCommandCodes.LF_char) { // add a NULL
                    pw2.write(TelnetCommandCodes.NULL_char);
                    pw2.write(buf[i] & mask);
                } else {
                    pw2.write(buf[i] & mask);
                }
            } else {
                if (buf[i] == TelnetCommandCodes.LF_char) { // a single LF -> CRLF
                    pw2.write(TelnetCommandCodes.CR_char);
                    pw2.write(TelnetCommandCodes.LF_char);
                } else {
                    pw2.write(buf[i] & mask);
                }
            }
            prevWasCR = (buf[i] == TelnetCommandCodes.CR_char) ? true : false;
        }
        // pw2.flush();
    }

    public synchronized void reset() {
        caw.reset();
    }

    /**
     * * Write a telnet command in the output stream. * *
     *
     * @parameter tc, command string to write
     */
    public synchronized void writeCommand(String tc) {
        if (tc != null) {
            pw2.print(tc);
            pw2.flush();
        }
    }
}
