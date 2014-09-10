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
 * id $Id: TelnetReader.java 6382 2013-11-07 08:50:55Z mlouiset $
 * author $Author: mlouiset $
 * version $Revision: 6382 $
 * lastrevision $Date: 2013-11-07 09:50:55 +0100 (Thu, 07 Nov 2013) $
 * modifiedby $LastChangedBy: mlouiset $
 * lastmodified $LastChangedDate: 2013-11-07 09:50:55 +0100 (Thu, 07 Nov 2013) $
 */

package com.actility.m2m.felix.shell.telnet;

import java.io.IOException;
import java.io.InputStream;
import java.io.Reader;
import java.util.Vector;

/**
 * Reads an input stream and extracts telnet TVM character.
 *
 * This class provides a very limited line editing capability.
 */
public class TelnetReader extends Reader {
    // private InputStreamReader ir;
    private TelnetSession tels;

    private Vector lineBuf; // buffer while a line is being edited

    private char[] readyLine; // buffer for a that has been terminated by CR
                              // or LF

    private int rp; // pointer in read buffer line

    private InputStream is;

    private boolean busyWait;

    private boolean skipLF = false; // Skip next char if LF

    public TelnetReader(InputStream is, TelnetSession tels, boolean busyWait) {
        this.tels = tels;
        this.is = is;
        this.busyWait = busyWait;

        // ir = new InputStreamReader(is);
        lineBuf = new Vector();
        readyLine = new char[0];
        rp = 0;
    }

    public void close() throws IOException {
        is.close();
    }

    public boolean ready() throws IOException {
        // TODO, we should check skipLF
        return is.available() > 0;
    }

    /**
     * Block here until end of line (CR) and then return
     *
     * This method provides very limited line editing functionality
     *
     * CR -&lt; return from read BS -&lt; back one step in buffer
     *
     */
    public int read(char[] buf, int off, int len) throws IOException {
        int count = 0;
        int character;
        Integer tmp;

        // System.out.println("TelnetReader.read buf=" + buf + ", off=" + off +
        // ", len=" + len);

        // 1. Check if there still are characters in readyLine ?

        count = chkRdyLine(buf, off, len);

        if (count > 0) {
            return count;
        }
        boolean w1 = true;
        while (w1) {
            character = readChar();

            if (character != -1) {

                // System.out.println("Char " + String.valueOf(character));

                switch (character) {

                case TelnetCommandCodes.BS:
                    // System.out.println("BS");
                    if (lineBuf.size() > 0) {
                        lineBuf.removeElementAt(lineBuf.size() - 1);
                        // tels.echoChar(character);
                        tels.echoChar(' ');
                        tels.echoChar(character);
                    }
                    break;

                case TelnetCommandCodes.CR:
                case TelnetCommandCodes.LF:
                    // System.out.println("CR");
                    lineBuf.addElement(new Integer(character));
                    // tels.echoChar(character);

                    // transfer from lineBuf to readyLine

                    readyLine = new char[lineBuf.size()];
                    for (int i = 0; i < lineBuf.size(); i++) {
                        tmp = (Integer) lineBuf.elementAt(i);
                        character = tmp.intValue();
                        readyLine[i] = (char) character;
                    }
                    rp = 0;

                    lineBuf.removeAllElements();
                    count = chkRdyLine(buf, off, len);
                    return count;

                default:
                    // System.out.println("char=" + character);
                    lineBuf.addElement(new Integer(character));
                    //tels.echoChar(character);
                }
            } else {
                // The input stream is closed; ignore the buffered data
                // and return -1 to to tell next layer that the reader
                // is closed.
                return -1;
            }
        }
        return count;
    }

    private int readChar() throws IOException {
        // System.out.println("TelnetReader.readChar()");
        int c;
        while (true) {
            if (busyWait) {
                while (is.available() < 1) {
                    try {
                        Thread.sleep(20);
                    } catch (Exception e) {
                    }
                }
            }
            c = is.read();
            if (c != 0) {
                if (skipLF) {
                    skipLF = false;
                    if (c == TelnetCommandCodes.LF) {
                        continue;
                    }
                }
                break;
            }
        }

        tels.echoChar(c);

        if (c == TelnetCommandCodes.CR) {
            skipLF = true;
        }
        return c;
    }

    private int chkRdyLine(char[] buf, int off, int len) {
        int count = 0;
        if (rp < readyLine.length) {
            while (count < (len - off)) {
                buf[off + count] = readyLine[rp++];
                count++;
            }
        }
        return count;
    }

    /**
     * * Read input data until CR or LF found;
     */

    public String readLine() throws IOException {
        Vector buf = new Vector();
        int character = -2;

        // System.out.println("TelnetReader.readLine()");

        try {
            while (true) {
                character = readChar();
                if (character == TelnetCommandCodes.CR) {
                    break;
                }

                if (-1 == character) {
                    // The input stream is closed; ignore the buffered
                    // data and return null to to tell next layer that
                    // the reader is closed.
                    return null;
                }
                // System.out.println("TelnetReader.readLine() add char " +
                // character);
                buf.addElement(new Character((char) character));
            }

            // Convert to string

            char[] cbuf = new char[buf.size()];
            for (int i = 0; i < buf.size(); i++) {
                Character c1 = (Character) buf.elementAt(i);
                cbuf[i] = c1.charValue();
            }
            String s1 = new String(cbuf);
            return s1;
        } catch (IOException e) {
            e.printStackTrace();
            throw e;
        }
    }
}
