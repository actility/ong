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

import java.util.LinkedList;
import java.util.SortedMap;
import java.util.TreeMap;

public final class VT100CommandCodes {
    public static final int ARROW_UP = 1024;
    public static final int ARROW_DOWN = 1025;
    public static final int ARROW_LEFT = 1026;
    public static final int ARROW_RIGHT = 1027;
    public static final int REMOVE = 1028;
    public static final int EOF = 1029;
    public static final int BOF = 1030;

    private static final TreeMap COMMANDS_CODE = new TreeMap();

    static {
        COMMANDS_CODE.put(TelnetCommandCodes.ESC_string + "[A", new Integer(ARROW_UP));
        COMMANDS_CODE.put(TelnetCommandCodes.ESC_string + "OA", new Integer(ARROW_UP));
        COMMANDS_CODE.put(TelnetCommandCodes.ESC_string + "[B", new Integer(ARROW_DOWN));
        COMMANDS_CODE.put(TelnetCommandCodes.ESC_string + "OB", new Integer(ARROW_DOWN));
        COMMANDS_CODE.put(TelnetCommandCodes.ESC_string + "[D", new Integer(ARROW_LEFT));
        COMMANDS_CODE.put(TelnetCommandCodes.ESC_string + "OD", new Integer(ARROW_LEFT));
        COMMANDS_CODE.put(TelnetCommandCodes.ESC_string + "[C", new Integer(ARROW_RIGHT));
        COMMANDS_CODE.put(TelnetCommandCodes.ESC_string + "OC", new Integer(ARROW_RIGHT));
        COMMANDS_CODE.put(TelnetCommandCodes.ESC_string + "[3~", new Integer(REMOVE));
        COMMANDS_CODE.put(TelnetCommandCodes.ESC_string + "OS", new Integer(REMOVE));
        COMMANDS_CODE.put(TelnetCommandCodes.ESC_string + "OF", new Integer(EOF));
        COMMANDS_CODE.put(TelnetCommandCodes.ESC_string + "OH", new Integer(BOF));
    }

    public static int readVT100CommandCode(String code) {
        SortedMap possibleMap = COMMANDS_CODE.tailMap(code);
        if (!possibleMap.isEmpty()) {
            String possibleKey = (String) possibleMap.firstKey();
            if (possibleKey.equals(code)) {
                return ((Integer) COMMANDS_CODE.get(code)).intValue();
            } else if (possibleKey.startsWith(code)) {
                return 0;
            }
        }
        return -1;
    }

    public static int executeCommandCode(String code, int commandCode, TelnetSession telnetSession) {
        switch (commandCode) {
        case ARROW_UP:
            int currentPosition = telnetSession.getLinePosition();
            LinkedList currentLine = telnetSession.getCurrentLine();
            LinkedList previousCommand = telnetSession.getPreviousCommand();
            // Go to begin of line
            currentPosition = telnetSession.getLinePosition();
            telnetSession.resetLinePosition();
            for (int j = 0; j < currentPosition; ++j) {
                telnetSession.echoChar(TelnetCommandCodes.BS);
            }
            // Write new command
            for (int i = 0; i < previousCommand.size(); ++i) {
                telnetSession.echoChar(((Character) previousCommand.get(i)).charValue());
            }
            if (previousCommand.size() < currentLine.size()) {
                for (int i = previousCommand.size(); i < currentLine.size(); ++i) {
                    telnetSession.echoChar(' ');
                }
                for (int i = previousCommand.size(); i < currentLine.size(); ++i) {
                    telnetSession.echoChar(TelnetCommandCodes.BS);
                }
            }
            // Buffer new command
            telnetSession.setLinePosition(previousCommand.size());
            currentLine.clear();
            for (int i = 0; i < previousCommand.size(); ++i) {
                currentLine.add(new Character(((Character) previousCommand.get(i)).charValue()));
            }
            break;
        case ARROW_DOWN:
            currentPosition = telnetSession.getLinePosition();
            currentLine = telnetSession.getCurrentLine();
            LinkedList nextCommand = telnetSession.getNextCommand();
            // Go to begin of line
            currentPosition = telnetSession.getLinePosition();
            telnetSession.resetLinePosition();
            for (int j = 0; j < currentPosition; ++j) {
                telnetSession.echoChar(TelnetCommandCodes.BS);
            }
            // Write new command
            for (int i = 0; i < nextCommand.size(); ++i) {
                telnetSession.echoChar(((Character) nextCommand.get(i)).charValue());
            }
            if (nextCommand.size() < currentLine.size()) {
                for (int i = nextCommand.size(); i < currentLine.size(); ++i) {
                    telnetSession.echoChar(' ');
                }
                for (int i = nextCommand.size(); i < currentLine.size(); ++i) {
                    telnetSession.echoChar(TelnetCommandCodes.BS);
                }
            }
            // Buffer new command
            telnetSession.setLinePosition(nextCommand.size());
            currentLine.clear();
            for (int i = 0; i < nextCommand.size(); ++i) {
                currentLine.add(new Character(((Character) nextCommand.get(i)).charValue()));
            }
            break;
        case ARROW_LEFT:
            if (telnetSession.decLinePosition()) {
                telnetSession.echoChar(TelnetCommandCodes.BS);
            }
            break;
        case ARROW_RIGHT:
            if (telnetSession.incLinePosition()) {
                for (int i = 0; i < code.length(); ++i) {
                    telnetSession.echoChar(code.charAt(i));
                }
            }
            break;
        case REMOVE:
            currentPosition = telnetSession.getLinePosition();
            currentLine = telnetSession.getCurrentLine();
            for (int i = currentPosition + 1; i < currentLine.size(); ++i) {
                telnetSession.echoChar(((Character) currentLine.get(i)).charValue());
            }
            telnetSession.echoChar(' ');
            currentLine.remove(currentPosition);
            for (int i = currentPosition; i < currentLine.size() + 1; ++i) {
                telnetSession.echoChar(TelnetCommandCodes.BS);
            }
            break;
        case EOF:
            currentPosition = telnetSession.getLinePosition();
            telnetSession.endLinePosition();
            String subCode = TelnetCommandCodes.ESC_string + "[C";
            for (int j = currentPosition; j < telnetSession.getCurrentLine().size(); ++j) {
                for (int i = 0; i < subCode.length(); ++i) {
                    telnetSession.echoChar(subCode.charAt(i));
                }
            }
            break;
        case BOF:
            currentPosition = telnetSession.getLinePosition();
            telnetSession.resetLinePosition();
            for (int j = 0; j < currentPosition; ++j) {
                telnetSession.echoChar(TelnetCommandCodes.BS);
            }
            break;
        }
        return 0;
    }
}
