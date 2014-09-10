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
 * id $Id: TelnetCommand.java 6116 2013-10-16 12:27:48Z mlouiset $
 * author $Author: mlouiset $
 * version $Revision: 6116 $
 * lastrevision $Date: 2013-10-16 14:27:48 +0200 (Wed, 16 Oct 2013) $
 * modifiedby $LastChangedBy: mlouiset $
 * lastmodified $LastChangedDate: 2013-10-16 14:27:48 +0200 (Wed, 16 Oct 2013) $
 */

package com.actility.m2m.felix.shell.telnet;

/**
 * This abstract class is designed to make it simpler to create and add new telnet commands in this implementation. Each command
 * is a TelnetCommand object, has a callback mechanism to the session it is instantiated in and has one execute method.
 */

public abstract class TelnetCommand {
    public TelnetSession ts; // the instantiating telnet session

    protected int commandCode; // the command code of this command

    protected boolean doStatus; // true if the server is ready to execute the

    // command

    protected boolean show; // if the command will be shown at initial

    // command negotiation

    public TelnetCommand() {
    } // parameterless dummy constructor

    /**
     * Parameters in the constructor:
     *
     * @parameter ts TelnetSession, to provide callback to the telnet session
     * @parameter int commandCode
     * @parameter boolean doStatus, if true the command has been asked to be active
     * @parameter boolean show, if true the commands is shown
     */
    public TelnetCommand(TelnetSession ts, int commandCode, boolean doStatus, boolean show) {
        this.ts = ts;
        this.commandCode = commandCode;
        this.doStatus = doStatus;
        this.show = show;
    }

    /** WILL string of command */

    public String getWILL() {
        return TelnetCommandCodes.IAC_string + TelnetCommandCodes.WILL_string + String.valueOf((char) commandCode);
    }

    /** WONT string of command */

    public String getWONT() {
        return TelnetCommandCodes.IAC_string + TelnetCommandCodes.WONT_string + String.valueOf((char) commandCode);
    }

    /** DONT string of command */

    public String getDONT() {
        return TelnetCommandCodes.IAC_string + TelnetCommandCodes.DONT_string + String.valueOf((char) commandCode);
    }

    /** DO string of command */

    public String getDO() {
        return TelnetCommandCodes.IAC_string + TelnetCommandCodes.DO_string + String.valueOf((char) commandCode);
    }

    public void setDoStatus(boolean state) {
        doStatus = state;
    }

    public boolean getDoStatus() {
        return doStatus;
    }

    public boolean getShow() {
        return show;
    }

    /**
     * * Get all registered commands from the session
     */

    public TelnetCommand[] getCommands() {
        return ts.getCommands();
    }

    /**
     * * Debug printout
     */

    void printCommand(int action, int optionCode, byte[] parameters) {
        System.out.print("Telnet Command code: " + String.valueOf(action) + " option: " + String.valueOf(optionCode)
                + " status now: " + String.valueOf(doStatus));
        if (parameters != null) {
            System.out.print(" parameters:");
            for (int i = 0; i < parameters.length; i++) {
                System.out.print(" " + String.valueOf(parameters[i]));
            }
            System.out.println();
        } else {
            // System.out.println(" no parameters");
            System.out.println();
        }
    }

    /**
     * Option negotiation and execution mechanism. To follow the intentions of RFC 854, a change in status is always followed by
     * a response but if trying to enter a mode that we are already in, no response is returned. This is essential to prevent
     * negotiation loops.
     *
     * @parameter action, one of the telnet protocol basic actions DO, DONT, WILL, WONT or SE
     * @parameter optionCode, the option code
     * @parameter parameters, a byte array with optional parameters, addition data to the option command.
     *
     * @return a String with the response of the command.
     */

    public abstract String execute(int action, int optionCode, byte[] parameters);
}
