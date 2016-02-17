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
 * id $Id: TelnetSession.java 9060 2014-07-09 15:43:26Z JReich $
 * author $Author: JReich $
 * version $Revision: 9060 $
 * lastrevision $Date: 2014-07-09 17:43:26 +0200 (Wed, 09 Jul 2014) $
 * modifiedby $LastChangedBy: JReich $
 * lastmodified $LastChangedDate: 2014-07-09 17:43:26 +0200 (Wed, 09 Jul 2014) $
 */

package com.actility.m2m.felix.shell.telnet;

import java.io.BufferedInputStream;
import java.io.BufferedOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.io.PrintStream;
import java.net.Socket;
import java.util.LinkedList;

import org.apache.felix.shell.Session;
import org.apache.felix.shell.ShellService;
import org.apache.log4j.Logger;
import org.osgi.framework.BundleContext;
import org.osgi.framework.ServiceReference;
import org.osgi.util.tracker.ServiceTracker;
import org.osgi.util.tracker.ServiceTrackerCustomizer;

import com.actility.m2m.felix.shell.telnet.log.BundleLogger;
import com.actility.m2m.util.log.OSGiLogger;

/**
 * This is the session object that binds the socket, the input and the output streams together. Here is where the telnet
 * commands are executed and where they affect the ConsoleService session.
 */

public class TelnetSession implements Runnable, ServiceTrackerCustomizer {
    private static final Logger LOG = OSGiLogger.getLogger(TelnetSession.class, BundleLogger.getStaticLogger());
    private static final String SHELL_SERVICE_NAME = ShellService.class.getName();

    private Socket socket;

    private TelnetConfig telnetConfig;

    private ServiceTracker consoleTracker;

    private ShellService shellService;

    private Session session;

    private TelnetServer telnetServer;

    private TelnetCommand[] telnetCommands = new TelnetCommand[256];

    private InputStream is;

    private OutputStream os;

    private TelnetInputStream telnetInputStream;

    private TelnetOutputStream telnetOutputStream;

    private TelnetReader reader;

    private PrintStream printStream;

    private char mask = '\177'; // Normal 7 bit mask

    private boolean enableEcho = true;

    private BundleContext bc;

    private boolean end = false;

    private int linePosition;

    private LinkedList lastEditingLine;
    private LinkedList[] editingHistory;
    private int editingHistoryIndex;
    private String[] history;
    private int historyIndex;

    private LinkedList currentLine;

    public TelnetSession(Socket socket, TelnetConfig telnetConfig, BundleContext bc, TelnetServer telnetServer) {
        this.telnetConfig = telnetConfig;
        this.socket = socket;
        this.bc = bc;
        this.telnetServer = telnetServer;

        if (LOG.isInfoEnabled()) {
            LOG.info("Connection request from " + socket.getInetAddress());
        }

        // Set up service tracker for the console service.
        consoleTracker = new ServiceTracker(bc, SHELL_SERVICE_NAME, this);
        consoleTracker.open();

        try {
            is = socket.getInputStream();
            os = socket.getOutputStream();
        } catch (IOException iox) {
            LOG.error("Session socket opening exception " + iox.toString(), iox);
        }

        telnetInputStream = new TelnetInputStream(new BufferedInputStream(is), this);
        telnetOutputStream = new TelnetOutputStream(new BufferedOutputStream(os), this);

        printStream = new PrintStream(telnetOutputStream);
        reader = new TelnetReader(telnetInputStream, this, telnetConfig.getBusyWait());

        // Instantiate the supported options, with default state
        // ts code, do show
        telnetCommands[TelnetCommandCodes.ECHO] = new TelnetCommandEcho(this, TelnetCommandCodes.ECHO, false, false);

        telnetCommands[TelnetCommandCodes.SUPGA] = new TelnetCommandSupga(this, TelnetCommandCodes.SUPGA, false, true);

        telnetCommands[TelnetCommandCodes.STATUS] = new TelnetCommandStatus(this, TelnetCommandCodes.STATUS, false, true);

        currentLine = new LinkedList();
        history = new String[32];
        historyIndex = 0;
        lastEditingLine = null;
        editingHistory = new LinkedList[32];
        editingHistoryIndex = 0;
    }

    public void run() {
        // Telnet initial option negotiaion
        initialNegotiation(telnetOutputStream);

        // Platform login processing

        printStream.println("Felix Shell console");

        try {
            if (login(reader, printStream, telnetOutputStream)) {
                if (null == shellService) {
                    printStream.println("Shell service not available, closing telnet session.");
                    LOG.info("Telnet session logged in, but was logged out since no shell service is available.");
                } else {
                    printStream.println("'quit' to end session");
                    LOG.info("Telnet session logged in");
                    session = shellService.createSession();
                    String cmdline = null;
                    // boolean eol = false;
                    // int current = 0;
                    while (!end) {
                        printStream.print("-> ");

                        end = printStream.checkError();
                        // eol = end;

                        editingHistoryIndex = historyIndex;
                        for (int i = 0; i < editingHistory.length; ++i) {
                            editingHistory[i] = null;
                        }
                        cmdline = reader.readLine();
                        if (cmdline == null) {
                            throw new IOException("Socket is closed");
                        }

                        if (!end && cmdline.length() > 0) {
                            try {
                                String command = cmdline.trim();
                                // Add command to history
                                int previousIndex = (historyIndex - 1 + history.length) % history.length;
                                if (command.length() > 0
                                        && (history[previousIndex] == null || !history[previousIndex].equals(command))) {
                                    history[historyIndex] = command;
                                    historyIndex = (historyIndex + 1) % history.length;
                                }
                                if ("quit".equalsIgnoreCase(command)) {
                                    end = true;
                                } else {
                                    shellService.executeCommand(session, command, printStream, printStream);
                                }
                            } catch (Exception e) {
                                printStream.println("ERROR: " + e.getMessage());
                                e.printStackTrace(printStream);
                                LOG.error(e.getMessage(), e);
                            }
                        }
                    }
                }
            } else {
                printStream.println("Bad login or password");
                LOG.info("Bad login or password");
            }
        } catch (IOException e) {
            printStream.println("ERROR: " + e.getMessage());
            e.printStackTrace(printStream);
            LOG.error(e.getMessage(), e);
        } catch (RuntimeException e) {

            printStream.println("Telnet session stopped by an exception");
            e.printStackTrace(printStream);
            LOG.error(e.getMessage(), e);
        } finally {
            sessionEnd(true);
        }
    }

    public void close() {
        sessionEnd(false);
    }

    synchronized private void sessionEnd(boolean removeSession) {
        try {
            printStream.close();
            reader.close();
            telnetInputStream.close();
            telnetOutputStream.close();
            is.close();
            os.close();
            socket.close();
            if (LOG.isInfoEnabled()) {
                LOG.info("Telnet session logged out");
            }
            if (session != null) {
                shellService.deleteSession(session);
            }
            if (removeSession) {
                telnetServer.removeSession(this);
            }
            session = null;
        } catch (Exception iox) {
            LOG.error("Session end exception " + iox.toString(), iox);
        }
        consoleTracker.close();
    }

    /**
     * Get the character mask. This is to support binary mode, that is 7 or 8 bit data in the output data stream
     *
     * @return The mask character
     */
    public char getMask() {
        return mask;
    }

    /**
     * Get all instantiated commands in this session
     *
     * @return The array of telnet commands in this session
     */
    public TelnetCommand[] getCommands() {
        return telnetCommands;
    }

    /**
     * Get the TelnetOutputStream
     *
     * @return The telnet ouput stream
     */
    public TelnetOutputStream getTelnetOutputStream() {
        return telnetOutputStream;
    }

    /**
     * Method to do echo to the output stream This also looks at the enableEcho flag
     *
     * @param character The character to write in the telnet output stream
     */
    public void echoChar(int character) {
        TelnetCommand tc = telnetCommands[TelnetCommandCodes.ECHO];

        if (tc != null) {
            if (tc.getDoStatus() && enableEcho) {
                printStream.print((char) character);
            }
        }
    }

    public void echoFlush() {
        TelnetCommand tc = telnetCommands[TelnetCommandCodes.ECHO];

        if (tc != null) {
            if (tc.getDoStatus() && enableEcho) {
                printStream.flush();
            }
        }
    }

    public void execGA() {
        // Not implemented
    }

    public void execEL() {
        // Not implemented
    }

    public void execEC() {
        // Not implemented
    }

    public void execAYT() {
        try {
            telnetOutputStream.writeCommand("[" + socket.getLocalAddress().getHostName() + ": yes]");
        } catch (IOException ex) {
            LOG.error("Command AYT exception " + ex.toString(), ex);
        }
    }

    public void execAO() {
        sessionEnd(true);
    }

    public void execIP() {
        sessionEnd(true);
    }

    public void execBRK() {
        sessionEnd(true);
    }

    public void execDM() {
        // Not implemented
    }

    public void execNOP() {
        // Not implemented
    }

    /**
     * DONT
     *
     * @param code the optional command code
     */
    public void execDONT(int code) {
        String response = execCommand(TelnetCommandCodes.DONT, code, null);
        try {
            telnetOutputStream.writeCommand(response);
        } catch (IOException ex) {
            LOG.error("Command DONT exception " + ex.toString(), ex);
        }
    }

    /**
     * DO
     *
     * @param code the optional command code
     */
    public void execDO(int code) {
        String response = execCommand(TelnetCommandCodes.DO, code, null);
        try {
            telnetOutputStream.writeCommand(response);
        } catch (IOException ex) {
            LOG.error("Command DO exception " + ex.toString(), ex);
        }
    }

    /**
     * WONT
     *
     * @param code the optional command code
     */
    public void execWONT(int code) {
        String response = execCommand(TelnetCommandCodes.WONT, code, null);
        try {
            telnetOutputStream.writeCommand(response);
        } catch (IOException ex) {
            LOG.error("Command WONT exception " + ex.toString(), ex);
        }
    }

    /**
     * WILL
     *
     * @param code the optional command code
     */
    public void execWILL(int code) {
        String response = execCommand(TelnetCommandCodes.WILL, code, null);
        try {
            telnetOutputStream.writeCommand(response);
        } catch (IOException ex) {
            LOG.error("Command WILL exception " + ex.toString(), ex);
        }
    }

    /**
     * On SE command, execute optional sub negotiated command.
     *
     * @param code the optional command code.
     * @param param the optional parameters.
     */
    public void execSE(int code, byte[] param) {
        String response = execCommand(TelnetCommandCodes.SE, code, param);
        try {
            telnetOutputStream.writeCommand(response);
        } catch (IOException ex) {
            LOG.error("Command SE exception " + ex.toString(), ex);
        }
    }

    // Individual command execution code

    /**
     * Execute optional sub command. In the case that there is no support for the option, the WILL and DO will be responded with
     * DONT and WONT respectively, to inform the requestor that the option is not supported.
     *
     * @param action the negotiation code
     * @param code the optional command code
     * @param params the optional parameters
     */
    private String execCommand(int action, int code, byte[] params) {
        String response = null;
        TelnetCommand tc = telnetCommands[code];

        if (tc != null) {
            response = tc.execute(action, code, params);
        } else { // Refuse unknown options
            if (action == TelnetCommandCodes.WILL) {
                response = TelnetCommandCodes.IAC_string + TelnetCommandCodes.DONT_string + String.valueOf((char) code);
            }

            if (action == TelnetCommandCodes.DO) {
                response = TelnetCommandCodes.IAC_string + TelnetCommandCodes.WONT_string + String.valueOf((char) code);
            }
        }
        return response;
    }

    /**
     * A login procedure (primitive and ugly).
     *
     * @param in Telnetreader
     * @param out Printwriter
     * @return a TelnetLogin object with the result of the login process.
     */

    private boolean login(TelnetReader in, PrintStream out, TelnetOutputStream tos) {
        String userName = null;
        String password = null;
        boolean loginResult = false;
        TelnetCommand echo = telnetCommands[TelnetCommandCodes.ECHO];

        try {
            out.print("login: ");
            out.flush();

            tos.writeCommand(echo.getWILL());
            tos.writeCommand(echo.getDO());

            if (echo.getDoStatus() == false) {
                echo.setDoStatus(true);
            }

            enableEcho = true;

            userName = in.readLine();

            out.print("password: ");
            out.flush();

            enableEcho = false;

            password = in.readLine();

            out.println();

            enableEcho = true;

            loginResult = telnetConfig.getUser().equals(userName) && telnetConfig.getPassword().equals(password);
        } catch (Exception e) {
            LOG.error("Login error", e);
        }
        return loginResult;
    }

    /**
     * Initial option setup
     *
     * @param tos The telnet output stream
     */
    private void initialNegotiation(TelnetOutputStream tos) {
        try {
            Thread.sleep(20);
        } catch (Exception ex) {
            LOG.error("Fail during Thread sleep" + ex.toString(), ex);
        }

        // Offer all telnet options that should be shown.
        for (int i = 0; i < telnetCommands.length; i++) {
            TelnetCommand tc = telnetCommands[i];
            if (tc != null && tc.getShow()) {
                try {
                    tos.writeCommand(tc.getWILL());
                } catch (IOException ex) {
                    LOG.error("Fail during initial option negotiation" + ex.toString(), ex);
                }
            }
        }
    }

    public int getLinePosition() {
        return linePosition;
    }

    public boolean incLinePosition() {
        if (linePosition < currentLine.size()) {
            this.linePosition++;
            return true;
        }
        return false;
    }

    public boolean decLinePosition() {
        if (linePosition > 0) {
            linePosition--;
            return true;
        }
        return false;
    }

    public void endLinePosition() {
        linePosition = currentLine.size();
    }

    public void resetLinePosition() {
        linePosition = 0;
    }

    public void setLinePosition(int linePosition) {
        this.linePosition = linePosition;
    }

    public LinkedList getCurrentLine() {
        return currentLine;
    }

    public LinkedList getPreviousCommand() {
        if (editingHistoryIndex == historyIndex) {
            lastEditingLine = new LinkedList(currentLine);
        } else {
            editingHistory[editingHistoryIndex] = new LinkedList(currentLine);
        }
        int previousEditingHistoryIndex = (editingHistoryIndex - 1 + editingHistory.length) % editingHistory.length;
        if (previousEditingHistoryIndex != historyIndex && history[previousEditingHistoryIndex] != null) {
            editingHistoryIndex = previousEditingHistoryIndex;
            LinkedList previousCommandList = editingHistory[editingHistoryIndex];
            if (previousCommandList == null) {
                String previousCommand = history[editingHistoryIndex];
                previousCommandList = new LinkedList();
                for (int i = 0; i < previousCommand.length(); ++i) {
                    previousCommandList.add(new Character(previousCommand.charAt(i)));
                }
            }
            return previousCommandList;
        }
        return new LinkedList(currentLine);
    }

    public LinkedList getNextCommand() {
        if (editingHistoryIndex != historyIndex) {
            editingHistory[editingHistoryIndex] = new LinkedList(currentLine);

            editingHistoryIndex = (editingHistoryIndex + 1) % editingHistory.length;

            LinkedList nextCommandList = null;
            if (editingHistoryIndex == historyIndex) {
                nextCommandList = lastEditingLine;
            } else {
                nextCommandList = editingHistory[editingHistoryIndex];
                if (nextCommandList == null) {
                    String nextCommand = history[editingHistoryIndex];
                    nextCommandList = new LinkedList();
                    for (int i = 0; i < nextCommand.length(); ++i) {
                        nextCommandList.add(new Character(nextCommand.charAt(i)));
                    }
                }
            }
            return nextCommandList;
        }
        return new LinkedList(currentLine);
    }

    public Object addingService(ServiceReference reference) {
        if (null == shellService) {
            shellService = (ShellService) bc.getService(reference);
            return shellService;
        }
        return null;
    }

    public void modifiedService(ServiceReference reference, Object service) {
        // Ignored
    }

    public void removedService(ServiceReference reference, Object service) {
        if (shellService == service) {
            LOG.info("Shell service closed.");
            printStream.println("Shell service closed, terminating telnet session.");
            sessionEnd(true);
            shellService = null;
        }
    }
}
