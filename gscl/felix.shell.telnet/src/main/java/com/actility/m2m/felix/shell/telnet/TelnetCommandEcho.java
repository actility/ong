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
 * id $Id: TelnetCommandEcho.java 6116 2013-10-16 12:27:48Z mlouiset $
 * author $Author: mlouiset $
 * version $Revision: 6116 $
 * lastrevision $Date: 2013-10-16 14:27:48 +0200 (Wed, 16 Oct 2013) $
 * modifiedby $LastChangedBy: mlouiset $
 * lastmodified $LastChangedDate: 2013-10-16 14:27:48 +0200 (Wed, 16 Oct 2013) $
 */

package com.actility.m2m.felix.shell.telnet;

/**
 * * The echo command has no sub negotiable parameters
 */

public class TelnetCommandEcho extends TelnetCommand {
    public TelnetCommandEcho(TelnetSession ts, int commandCode, boolean doStatus, boolean show) {
        super(ts, commandCode, doStatus, show);
    }

    /**
     * * Option negotiation and execution mechanism * * To follow the intentions of RFC 854, a change in status * is always
     * followed by a response but if trying to enter a mode * that we are already in, no response is returned. * * This is
     * essential to prevent negotiation loops. * *
     *
     * @param action, one of the telnet protocol basic actions * DO, DONT, WILL, WONT or SE *
     * @param optionCode, the option code *
     * @param parameters, a byte array with optional parameters to the option code. * *
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

        // handle the case of WONT, which means that
        // the other end wont echo but this end will still echo
        case TelnetCommandCodes.WONT:
            if (doStatus == true) {
                // no appropriate answer to send
                doStatus = true;
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

            } else { // not in right state
                sb.append(getDONT());
            }
            break;

        default:
            break;
        }
        return sb.toString();
    }
}
