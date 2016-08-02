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

/**
 * The STATUS command has one sub negotiable parameter that may be IS or SEND.
 */

public class TelnetCommandStatus extends TelnetCommand {

    public TelnetCommandStatus(TelnetSession ts, int commandCode, boolean doStatus, boolean show) {
        super(ts, commandCode, doStatus, show);
    }

    /**
     * Option negotiation and execution mechanism. To follow the intentions of RFC 854, a change in status is always followed by
     * a response but if trying to enter a mode that we are already in, no response is returned. This is essential to prevent
     * negotiation loops.
     *
     * @param action one of the telnet protocol basic actions DO, DONT, WILL, WONT or SE
     * @param optionCode the option code
     * @param parameters a string with optional parameters to the option code.
     * @return a String with the response to the command.
     */
    public String execute(int action, int optionCode, byte[] parameters) {
        // printCommand(action, optionCode, parameters);
        StringBuffer sb = new StringBuffer();

        switch (action) {
        case TelnetCommandCodes.DO:
            if (doStatus == true) {
                // willing and ready, send no resonse,
                // to prevent creation of negotiation loop
            } else {
                doStatus = true;
                sb.append(getWILL());
            }
            break;

        case TelnetCommandCodes.WILL:
            if (doStatus == true) {
                // willing and ready, send no resonse,
                // to prevent creation of negotiation loop
            } else {
                doStatus = true;
                sb.append(getDO());
            }
            break;

        case TelnetCommandCodes.DONT:
            if (doStatus == true) {
                sb.append(getWONT());
                doStatus = false;
                // now not willing, send no resonse,
                // to prevent creation of negotiation loop
            } else {
                doStatus = false;
            }
            break;

        case TelnetCommandCodes.WONT:
            if (doStatus == true) {
                // no appropriate answer to send
                doStatus = false;
                // now not willing, send no resonse,
                // to prevent creation of negotiation loop
            } else {
                doStatus = false;
            }
            break;

        // SB .... SE, command execution, when negotiations
        // are finished and both parties have agreed

        case TelnetCommandCodes.SE:
            if (doStatus == true) {
                sb.append(doCommand(action, optionCode, parameters));
            } else { // not in right state
                sb.append(getDONT());
            }
            break;

        default:
            break;
        }
        return sb.toString();
    }

    public String doCommand(int action, int optionCode, byte[] parameters) {
        // printCommand( action, optionCode, parameters);

        StringBuffer sb = new StringBuffer();
        if (parameters != null && (parameters[0] == (byte) TelnetCommandCodes.SEND)) {
            // assume SEND
            sb.append(TelnetCommandCodes.IAC_string + TelnetCommandCodes.SB_string + String.valueOf((char) optionCode)
                    + TelnetCommandCodes.IS_string);

            TelnetCommand[] tcs = getCommands();
            for (int i = 0; i < tcs.length; i++) {
                TelnetCommand tc = tcs[i];
                if (tc != null) {
                    sb.append(TelnetCommandCodes.WILL_string);
                    sb.append(String.valueOf((char) i));

                    if (tc.getDoStatus() == true) {
                        sb.append(TelnetCommandCodes.DO_string);
                        sb.append(String.valueOf((char) i));
                    } else {
                        sb.append(TelnetCommandCodes.DONT_string);
                        sb.append(String.valueOf((char) i));
                    }
                }
            }
            sb.append(TelnetCommandCodes.IAC_string + TelnetCommandCodes.SE_string);
        } else if (parameters != null && (parameters[0] == (byte) TelnetCommandCodes.IS)) {

        }
        return sb.toString();
    }
}
