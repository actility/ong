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
 * id $Id: $
 * author $Author: $
 * version $Revision: $
 * lastrevision $Date: $
 * modifiedby $LastChangedBy: $
 * lastmodified $LastChangedDate: $
 */

package com.actility.m2m.system.sshtunnel;

import java.io.IOException;
import java.io.InputStreamReader;
import java.io.OutputStreamWriter;
import java.io.Serializable;
import java.text.ParseException;

import org.apache.log4j.Logger;

import com.actility.m2m.servlet.ServletTimer;
import com.actility.m2m.servlet.TimerListener;
import com.actility.m2m.servlet.song.SongServletRequest;
import com.actility.m2m.servlet.song.SongServletResponse;
import com.actility.m2m.system.log.BundleLogger;
import com.actility.m2m.util.FormatUtils;
import com.actility.m2m.util.StringUtils;
import com.actility.m2m.util.log.OSGiLogger;

public class SshTunnel implements Serializable, TimerListener {

    /**
     * generated serial UID for SshTunnel
     */
    private static final long serialVersionUID = 799846393902132742L;

    private static final Logger LOG = OSGiLogger.getLogger(SshTunnel.class, BundleLogger.LOG);

    private static final long DEFAULT_TUNNEL_MAPPED_PORT = 2000;
    private static final long DEFAULT_TUNNEL_CONNECTION_TIMEOUT_VALUE = 60 * 60 * 1000;

    private static final String MAGIC_TOKEN = "[[--&&MAGIC-TOKEN&&--))";

    private static SshTunnel instance = new SshTunnel(); // the singleton instance

    private String[] shellCmd;
    private String sshTargetHostname;
    private int sshTargetPort;
    private String sshTargetLogin;
    private String sshTargetPassword;
    private String sshConnectOptions;

    private Process sshTunnelProcess;

    private InputStreamReader inProcessStream = null;
    private InputStreamReader errProcessStream = null;
    private OutputStreamWriter outProcessStream = null;
    private StringBuffer bufferIn = new StringBuffer(256);
    private StringBuffer bufferErr = new StringBuffer(256);

    /**
     * Get the singleton instance.
     *
     * @return the singleton instance.
     */
    public static SshTunnel getInstance() {
        return instance;
    }

    /**
     * @param shellCmd the shellCmd to set
     */
    public void setShellCmd(String shellCmd) {
        this.shellCmd = StringUtils.split(shellCmd, ' ', true);
    }

    /**
     * @param sshTargetHostname the sshTargetHostname to set
     */
    public void setSshTargetHostname(String sshTargetHostname) {
        this.sshTargetHostname = sshTargetHostname;
    }

    /**
     * @param sshTargetPort the sshTargetPort to set
     */
    public void setSshTargetPort(int sshTargetPort) {
        this.sshTargetPort = sshTargetPort;
    }

    /**
     * @param sshTargetLogin the sshTargetLogin to set
     */
    public void setSshTargetLogin(String sshTargetLogin) {
        this.sshTargetLogin = sshTargetLogin;
    }

    /**
     * @param sshTargetPassword the sshTargetPassword to set
     */
    public void setSshTargetPassword(String sshTargetPassword) {
        this.sshTargetPassword = sshTargetPassword;
    }

    /**
     * @param sshConnectOptions the sshConnectOptions to set
     */
    public void setSshConnectOptions(String sshConnectOptions) {
        this.sshConnectOptions = sshConnectOptions;
    }

    /**
     * Read input on std output or std error output of the exec process, and only return when the given string is found at the
     * beginning of the line
     *
     * @param inCmd
     * @param errCmd
     * @return
     * @throws IOException
     */
    private boolean waitStartLine(InputStreamReader inCmd, InputStreamReader errCmd, String startLine) throws IOException {
        int maxLines = 10;
        int numRead = -1;
        StringBuffer buf;
        boolean res = false;
        while (maxLines > 0) {
            if (inCmd.ready()) {
                numRead = inCmd.read();
                buf = bufferIn;
            } else if (errCmd.ready()) {
                numRead = errCmd.read();
                buf = bufferErr;
            } else {
                numRead = -1;
                buf = null;
            }

            if (buf != null) {
                if (numRead != '\n') {
                    buf.append((char) numRead);
                    if (buf.length() == startLine.length() && startLine.equals(buf.toString())) {
                        res = true;
                        maxLines = 0; // force loop end
                    }
                } else {
                    if (LOG.isInfoEnabled()) {
                        LOG.info("Read more data: (" + buf.toString() + ")");
                    }
                    // reset the buffer string
                    --maxLines;
                    buf.setLength(0);
                }
            } else if (numRead != -1) {
                maxLines = 0; // force loop end
            } else {
                try {
                    Thread.sleep(100);
                } catch (InterruptedException e) {
                    // nothing to do
                }
            }
        }

        return res;
    }

    /**
     * Read input on std output or std error output of the exec process, and return when a line was read.
     *
     * @param inCmd
     * @param errCmd
     * @return
     * @throws IOException
     */
    private String readLine(InputStreamReader inCmd, InputStreamReader errCmd) throws IOException {
        int readRetries = 10;
        int numRead = -1;
        StringBuffer buf;
        String res = null;
        while (readRetries > 0) {
            if (inCmd.ready()) {
                numRead = inCmd.read();
                buf = bufferIn;
            } else if (errCmd.ready()) {
                numRead = errCmd.read();
                buf = bufferErr;
            } else {
                numRead = -1;
                buf = null;
            }

            if (buf != null) {
                if (numRead != '\n') {
                    buf.append((char) numRead);
                } else {
                    // reset the buffer string
                    res = buf.toString();
                    buf.delete(0, 255);

                    readRetries = 0; // force loop end
                }
            } else if (numRead != -1) {
                readRetries = 0; // force loop end
            } else {
                readRetries--;
                try {
                    Thread.sleep(100);
                } catch (InterruptedException e) {
                    // nothing to do
                }
            }
        }

        // SSH password invite has no trailing '\n'... So if we have already wait 5s and no '\n' was detected yet, take a look
        // at buffered string
        if (res == null) {
            if (bufferErr.length() != 0) {
                res = bufferErr.toString();
                bufferErr.delete(0, 255);
            } else if (bufferIn.length() != 0) {
                res = bufferIn.toString();
                bufferIn.delete(0, 255);
            }
        }

        if (LOG.isInfoEnabled()) {
            LOG.info("Read: " + res);
        }
        return res;
    }

    /**
     * Close a process' stream reader.
     *
     * @param reader the reader to be closed
     */
    private void closeReader(InputStreamReader reader) {
        if (reader != null) {
            try {
                reader.close();
            } catch (IOException e) {
                LOG.error("Unable to close process' stream reader", e);
            }
        }
    }

    /**
     * Close a process' stream reader.
     *
     * @param reader the reader to be closed
     */
    private void closeWriter(OutputStreamWriter writer) {
        if (writer != null) {
            try {
                writer.close();
            } catch (IOException e) {
                LOG.error("Unable to close process' stream writer", e);
            }
        }
    }

    private void closeTunnel() {
        closeWriter(outProcessStream);
        closeReader(inProcessStream);
        closeReader(errProcessStream);
        sshTunnelProcess.destroy();
        sshTunnelProcess = null;
    }

    /**
     * Do process openSshTunnel operation: if the SSH tunnel resource is not connected, try to open it.
     *
     * @param request the received SONG request.
     * @return 0 if SSH tunnel cannot be opened, or the maximum duration allowed for the created SSH connection.
     * @throws IOException If any IO problem occurs while opening the SSH tunnel
     */
    public long invokeOpenSshTunnel(SongServletRequest request) throws IOException {

        long sshConnectionMaxDuration = 0;
        SongServletResponse response = null;

        LOG.info("Establishing SSH tunnel with " + sshTargetHostname);

        if (sshTunnelProcess != null) {
            // tunnel is already available
            response = request.createResponse(SongServletResponse.SC_FORBIDDEN);
        } else {

            long tunnelMappedPort = DEFAULT_TUNNEL_MAPPED_PORT;
            long tunnelConnectionTimeoutValue = DEFAULT_TUNNEL_CONNECTION_TIMEOUT_VALUE;

            // First, parse the request body to retrieve tunnel parameters
            if (0 != request.getContentLength()) {
                // <?xml version='1.0' encoding='UTF-8'?>
                // <obj xmlns="http://obix.org/ns/schema/1.1"/>
                // <int name="remotePort" val="2222"/>
                // <reltime name="maxDuration" val="PT1H"/>
                // </obj>

                String content = new String(request.getRawContent(), "UTF-8");
                int index = content.indexOf("\"remotePort\"");
                if (index != -1) {
                    index = content.indexOf("val=\"", index);
                    if (index != -1) {
                        index += 5;
                        int startIndex = index;
                        index = content.indexOf("\"", index);
                        if (index != -1) {
                            tunnelMappedPort = Long.parseLong(content.substring(startIndex, index));
                        }
                    }
                }
                index = content.indexOf("\"maxDuration\"");
                if (index != -1) {
                    index = content.indexOf("val=\"", index);
                    if (index != -1) {
                        index += 5;
                        int startIndex = index;
                        index = content.indexOf("\"", index);
                        if (index != -1) {
                            try {
                                tunnelConnectionTimeoutValue = FormatUtils.parseDuration(content.substring(startIndex, index));
                            } catch (ParseException e) {
                                LOG.error("Cannot parse maxDuration", e);
                            }
                        }
                    }
                }
            }

            StringBuffer sshCommand = new StringBuffer();
            sshCommand.append("trans_pty ssh -p ").append(sshTargetPort).append(" ").append(sshConnectOptions).append(" -R ")
                    .append(tunnelMappedPort).append(":localhost:22 ").append(sshTargetLogin).append("@")
                    .append(sshTargetHostname);
            // sshCommand[shellCmd.length] = "ssh " + sshTargetLogin + "@" + sshTargetHostname;
            // Built the SSH command
            String[] execCommand = new String[shellCmd.length + 1];
            System.arraycopy(shellCmd, 0, execCommand, 0, shellCmd.length);
            execCommand[shellCmd.length] = sshCommand.toString();

            try {
                // Now try to establish the SSH connection with the centralized server
                sshTunnelProcess = Runtime.getRuntime().exec(execCommand);

                inProcessStream = new InputStreamReader(sshTunnelProcess.getInputStream());
                errProcessStream = new InputStreamReader(sshTunnelProcess.getErrorStream());
                outProcessStream = new OutputStreamWriter(sshTunnelProcess.getOutputStream());

                int maxRetries = 1;
                String readLine = null;
                if (waitStartLine(inProcessStream, errProcessStream, sshTargetLogin + "@" + sshTargetHostname + "'s password:")) {
                    LOG.info("Entering password");
                    outProcessStream.write(sshTargetPassword);
                    outProcessStream.write("\n");
                    outProcessStream.flush();

                    Thread.sleep(1000);

                    // from this point, nothing should be read on input except whitespace... Otherwise it indicates an error
                    // happened.
                    LOG.info("Watching for connection result");
                    maxRetries = 50;
                    boolean logged = false;
                    String errorMsg = null;
                    while (maxRetries > 0) {
                        outProcessStream.write("echo \"" + MAGIC_TOKEN + "\"");
                        outProcessStream.write("\n");
                        outProcessStream.flush();
                        readLine = readLine(inProcessStream, errProcessStream);
                        if (readLine != null) {
                            if (LOG.isInfoEnabled()) {
                                LOG.info("Read more data: (" + readLine + ")");
                            }
                            if (readLine.startsWith(MAGIC_TOKEN)) {
                                // prompt detected
                                logged = true; // error flag is reset (previous potential errors were actually not
                                               // errors)
                                maxRetries = 0; // force loop end
                                // ////////////////////////////////////////////////////////////////////////////////////////////
                                // error cases
                            } else if (readLine.endsWith("assword: ")) {
                                errorMsg = "Failed to establish SSH connection - wrong password";
                                maxRetries = 0; // no need to wait more, force loop end
                            }
                        }
                        maxRetries--;
                    }

                    if (!logged) {
                        if (errorMsg != null) {
                            throw new IllegalArgumentException(errorMsg);
                        } else {
                            throw new IllegalArgumentException("Failed to establish SSH connection (last msg:" + readLine + ")");
                        }
                    }

                    LOG.info("SSH tunnel created");
                    response = request.createResponse(SongServletResponse.SC_ACCEPTED);

                    sshConnectionMaxDuration = tunnelConnectionTimeoutValue;

                } else {

                    LOG.error("SSH connection failed (cmd:" + execCommand[shellCmd.length] + ")");
                    maxRetries = 1;
                    while (maxRetries > 0) {
                        readLine = readLine(inProcessStream, errProcessStream);
                        if (readLine != null) {
                            LOG.error("SSH connection failure (read:" + readLine + ")");
                        } else {
                            maxRetries--;
                        }
                    }
                    if (sshTunnelProcess != null) {
                        closeTunnel();
                    }
                    response = request.createResponse(SongServletResponse.SC_BAD_GATEWAY);
                }

            } catch (IOException e) {
                // SSH tunnel creation failed, send back error message
                LOG.error("Failed to execute shell command for SSH tunnel creation (cmd:" + execCommand[shellCmd.length] + ")",
                        e);
                if (sshTunnelProcess != null) {
                    closeTunnel();
                }
                response = request.createResponse(SongServletResponse.SC_BAD_GATEWAY);
            } catch (Exception e) {
                LOG.error("Failed to retrieve request parameters from content", e);
                if (sshTunnelProcess != null) {
                    closeTunnel();
                }
                response = request.createResponse(SongServletResponse.SC_BAD_REQUEST);
            } finally {

                // one must not close outProcessStream here as we will need it to close SSH tunnel later
            }
        }

        try {
            response.send();
        } catch (IOException e) {
            LOG.error("Failed to send SONG response back", e);
        }

        return sshConnectionMaxDuration;
    }

    /**
     * Close the SSH tunnel
     *
     * @return true if SSH tunnel existed and was closed, false otherwise.
     */
    public boolean closeSshTunnel() {
        boolean res = false;
        if (sshTunnelProcess != null) {

            try {
                // from this point, nothing should be read on input except whitespace... Otherwise it indicates an error
                // happened.
                LOG.info("Watching for connection result");
                int maxRetries = 10;
                String readLine = null;
                while (maxRetries > 0) {
                    outProcessStream.write("exit\n");
                    outProcessStream.flush();
                    readLine = readLine(inProcessStream, errProcessStream);
                    if (readLine != null) {
                        if (LOG.isInfoEnabled()) {
                            LOG.info("Read more data: (" + readLine + ")");
                        }
                        if (readLine.startsWith("exit")) {
                            // exit detected
                            maxRetries = 0; // force loop end
                        }
                    }
                    maxRetries--;
                }
            } catch (IOException e) {
                LOG.error("Failed to close SSH tunnel", e);
            } finally {
                closeTunnel();
            }
            res = true;
        }
        return res;
    }

    /**
     * Do process closeSshTunnel operation: if the SSH tunnel resource is connected, try to close it.
     *
     * @param request the received SONG request.
     */
    public void invokeCloseSshTunnel(SongServletRequest request) {
        SongServletResponse response = null;

        LOG.info("Closing SSH tunnel with " + sshTargetHostname);

        if (closeSshTunnel()) {
            response = request.createResponse(SongServletResponse.SC_ACCEPTED);
        } else {
            response = request.createResponse(SongServletResponse.SC_NOT_FOUND);
        }

        try {
            response.send();
        } catch (IOException e) {
            LOG.error("Failed to send SONG response back", e);
        }

    }

    /**
     * The SSH connection is now established for too long duration, close it.
     *
     * @param timer the expired timer handler
     */
    public void timeout(ServletTimer timer) {
        closeSshTunnel();
    }

}
