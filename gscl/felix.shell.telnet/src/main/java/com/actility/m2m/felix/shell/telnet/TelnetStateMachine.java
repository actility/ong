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
 * id $Id: TelnetStateMachine.java 9060 2014-07-09 15:43:26Z JReich $
 * author $Author: JReich $
 * version $Revision: 9060 $
 * lastrevision $Date: 2014-07-09 17:43:26 +0200 (Wed, 09 Jul 2014) $
 * modifiedby $LastChangedBy: JReich $
 * lastmodified $LastChangedDate: 2014-07-09 17:43:26 +0200 (Wed, 09 Jul 2014) $
 */

package com.actility.m2m.felix.shell.telnet;

import java.util.Vector;

public class TelnetStateMachine {

    private TelnetSession telnetSession; // callback reference

    private int state = 0; // initial state

    private Vector subNegBuffer; // Telnet command buffer

    private int subCode; // sub command code

    public TelnetStateMachine(TelnetSession telnetSession) {
        state = 0;
        subCode = 0;
        subNegBuffer = new Vector();
        this.telnetSession = telnetSession;
    }

    /** Return state of command state machine */
    public int getState() {
        return state;
    }

    /** Set state of command state machine */
    public void setState(int s) {
        state = s;
    }

    /**
     * * Telnet parser, a state machine * to extract telnet commands from an input stream.
     */

    public int nextState(int state, int code) {
        int newState = 0;

        switch (state) {
        case 0: // data mode, look for IAC only
            switch (code) {
            case TelnetCommandCodes.IAC:
                newState = 1;
                break;
            default:
                newState = 0;
                break;
            }
            break;

        case 1: // command mode
            switch (code) {
            case TelnetCommandCodes.DONT: // One option byte to read
                newState = 20;
                break;
            case TelnetCommandCodes.DO: // One option byte to read
                newState = 21;
                break;
            case TelnetCommandCodes.WONT: // One option byte to read
                newState = 22;
                break;
            case TelnetCommandCodes.WILL: // One option byte to read
                newState = 23;
                break;
            case TelnetCommandCodes.SB: // Sub negotiation start
                subNegBuffer.removeAllElements();
                newState = 30;
                break;
            case TelnetCommandCodes.GA:
                telnetSession.execGA();
                newState = 0;
                break;
            case TelnetCommandCodes.EL:
                telnetSession.execEL();
                newState = 0;
                break;
            case TelnetCommandCodes.EC:
                telnetSession.execEC();
                newState = 0;
                break;
            case TelnetCommandCodes.AYT:
                telnetSession.execAYT();
                newState = 0;
                break;
            case TelnetCommandCodes.AO:
                telnetSession.execAO();
                newState = 0;
                break;
            case TelnetCommandCodes.IP:
                telnetSession.execIP();
                newState = 0;
                break;
            case TelnetCommandCodes.BRK:
                telnetSession.execBRK();
                newState = 0;
                break;
            case TelnetCommandCodes.DM:
                telnetSession.execDM();
                newState = 0;
                break;
            case TelnetCommandCodes.NOP:
                telnetSession.execNOP();
                newState = 0;
                break;
            case TelnetCommandCodes.SE: // Sub negotiation end
                // convert the vector to a byte array
                byte[] subNegArray = new byte[subNegBuffer.size() - 1];
                for (int i = 0; i < subNegBuffer.size() - 1; i++) {
                    Byte b = (Byte) subNegBuffer.elementAt(i);
                    subNegArray[i] = b.byteValue();
                }
                telnetSession.execSE(subCode, subNegArray);
                newState = 0;
                break;
            default:
                break;
            }
            break;

        case 20: // DONT command option code
            telnetSession.execDONT(code);
            newState = 0;
            break;

        case 21: // DO command option code
            telnetSession.execDO(code);
            newState = 0;
            break;

        case 22: // WONT command option code
            telnetSession.execWONT(code);
            newState = 0;
            break;

        case 23: // WILL command option code
            telnetSession.execWILL(code);
            newState = 0;
            break;

        case 30: // Sub negotiation sub command code
            switch (code) {
            default:
                subCode = code;
                newState = 31;
                break;
            }
            break;

        case 31: // Sub negotiation parameter collection mode end ?
            switch (code) {
            case TelnetCommandCodes.IAC:
                subNegBuffer.addElement(new Byte((byte) code));
                newState = 32;
                break;
            default:
                subNegBuffer.addElement(new Byte((byte) code));
                newState = 31;
                break;
            }
            break;

        case 32: // Sub negotiation parameter collection mode end ?
            switch (code) {
            case TelnetCommandCodes.IAC: // Double written IAC, skip this
                newState = 31;
                break;
            case TelnetCommandCodes.SE:
                // convert the vector to a byte array
                byte[] subNegArray = new byte[subNegBuffer.size() - 1];
                for (int i = 0; i < subNegBuffer.size() - 1; i++) {
                    Byte b = (Byte) subNegBuffer.elementAt(i);
                    subNegArray[i] = b.byteValue();
                }
                telnetSession.execSE(subCode, subNegArray);
                newState = 0;
                break;
            }
        default:
            break;
        }

        setState(newState);
        return state;
    }

} // TelnetStateMachine
