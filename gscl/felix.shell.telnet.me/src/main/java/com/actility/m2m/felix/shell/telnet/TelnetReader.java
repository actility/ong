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

import java.io.IOException;
import java.io.InputStream;
import java.util.LinkedList;

/**
 * Reads an input stream and extracts telnet TVM character.
 *
 * This class provides a very limited line editing capability.
 */
public class TelnetReader {
    // private InputStreamReader ir;
    private TelnetSession telnetSession;
    private InputStream is;
    private boolean busyWait;
    private boolean skipLF = false; // Skip next char if LF

    public TelnetReader(InputStream is, TelnetSession telnetSession, boolean busyWait) {
        this.telnetSession = telnetSession;
        this.is = is;
        this.busyWait = busyWait;
    }

    public void close() throws IOException {
        is.close();
    }

    public boolean ready() throws IOException {
        return is.available() > 0;
    }

    private int readChar() throws IOException {
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

        if (c != TelnetCommandCodes.ESC && c != TelnetCommandCodes.DEL && c != TelnetCommandCodes.BS) {
            telnetSession.echoChar(c);
            telnetSession.echoFlush();
        }

        if (c == TelnetCommandCodes.CR) {
            skipLF = true;
        }
        return c;
    }

    /**
     * Read input data until CR or LF found;
     *
     * @return The complete line read from telnet session
     * @throws IOException If any IO problem occurs
     */
    public String readLine() throws IOException {
        LinkedList buffer = telnetSession.getCurrentLine();
        buffer.clear();
        int character = 0;
        telnetSession.resetLinePosition();

        while ((character = readChar()) != -1) {
            switch (character) {
            case TelnetCommandCodes.ESC:
                // Read VT100
                StringBuffer tmp = new StringBuffer();
                tmp.append((char) character);
                while ((character = is.read()) != 1) {
                    tmp.append((char) character);
                    int command = VT100CommandCodes.readVT100CommandCode(tmp.toString());
                    if (command == -1) {
                        // Unknown VT100 character code. Echo all characters
                        for (int i = 0; i < tmp.length(); ++i) {
                            telnetSession.echoChar(tmp.charAt(i));
                        }
                        telnetSession.echoFlush();
                        break;
                    } else if (command > 0) {
                        // Execute command code
                        VT100CommandCodes.executeCommandCode(tmp.toString(), command, telnetSession);
                        break;
                    }
                }
                if (character == -1) {
                    return null;
                }
                break;
            case TelnetCommandCodes.BS:
            case TelnetCommandCodes.DEL:
                if (buffer.size() > 0 && telnetSession.getLinePosition() > 0) {
                    buffer.remove(telnetSession.getLinePosition() - 1);
                    telnetSession.decLinePosition();
                    telnetSession.echoChar(TelnetCommandCodes.BS);
                    for (int i = telnetSession.getLinePosition(); i < buffer.size(); ++i) {
                        telnetSession.echoChar(((Character) buffer.get(i)).charValue());
                    }
                    telnetSession.echoChar(' ');
                    telnetSession.echoChar(TelnetCommandCodes.BS);
                    for (int i = telnetSession.getLinePosition(); i < buffer.size(); ++i) {
                        telnetSession.echoChar(TelnetCommandCodes.BS);
                    }
                }
                break;
            case TelnetCommandCodes.CR:
            case TelnetCommandCodes.LF:
                StringBuffer realLine = new StringBuffer();
                for (int i = 0; i < buffer.size(); i++) {
                    realLine.append(buffer.get(i));
                }
                return realLine.toString();
            default:
                buffer.add(telnetSession.getLinePosition(), new Character((char) character));
                telnetSession.incLinePosition();
                for (int i = telnetSession.getLinePosition(); i < buffer.size(); ++i) {
                    telnetSession.echoChar(((Character) buffer.get(i)).charValue());
                }
                for (int i = telnetSession.getLinePosition(); i < buffer.size(); ++i) {
                    telnetSession.echoChar(TelnetCommandCodes.BS);
                }
            }
            telnetSession.echoFlush();
        }
        return null;
    }
}
