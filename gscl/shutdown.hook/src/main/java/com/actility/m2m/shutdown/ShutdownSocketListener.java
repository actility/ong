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
 * id $Id: ShutdownSocketListener.java 8765 2014-05-21 15:37:33Z JReich $
 * author $Author: JReich $
 * version $Revision: 8765 $
 * lastrevision $Date: 2014-05-21 17:37:33 +0200 (Wed, 21 May 2014) $
 * modifiedby $LastChangedBy: JReich $
 * lastmodified $LastChangedDate: 2014-05-21 17:37:33 +0200 (Wed, 21 May 2014) $
 */

package com.actility.m2m.shutdown;

import java.io.IOException;
import java.io.InputStreamReader;
import java.net.ServerSocket;
import java.net.Socket;
import java.net.SocketException;

import org.apache.log4j.Logger;
import org.osgi.framework.Bundle;
import org.osgi.framework.BundleContext;
import org.osgi.framework.BundleException;
import org.osgi.framework.FrameworkEvent;
import org.osgi.framework.FrameworkListener;

import com.actility.m2m.shutdown.log.BundleLogger;
import com.actility.m2m.util.log.OSGiLogger;

/**
 * @author mlouiset
 *
 */
public class ShutdownSocketListener implements Runnable, FrameworkListener {

    private static final Logger LOG = OSGiLogger.getLogger(ShutdownSocketListener.class, BundleLogger.getStaticLogger());
    private final BundleContext bundleContext;
    private final int serverPort;
    private ServerSocket serverSocket = null;

    /**
     * Constructor
     *
     * @param bundleContext reference on System Bundle
     */
    public ShutdownSocketListener(BundleContext bundleContext, int serverPort) {
        this.bundleContext = bundleContext;
        this.serverPort = serverPort;
    }

    /**
     *
     */
    public void stop() {
        try {
            serverSocket.close();
        } catch (IOException e) {
            LOG.error("Unable to close the socket", e);
        }
    }

    public void run() {

        try {
            serverSocket = new ServerSocket(serverPort);
            Socket clientSocket = null;
            try {
                boolean shutdown = false;
                while (!shutdown) {
                    clientSocket = serverSocket.accept();
                    LOG.info("connection established");
                    InputStreamReader in = new InputStreamReader(clientSocket.getInputStream());
                    int input;
                    char[] readChars = new char[255];
                    int i = 0;

                    while ((!shutdown) && (input = in.read()) != -1) {
                        if (i < 255) {

                            if (input == '\n') {
                                String readLine = new String(readChars, 0, i);
                                if (readLine.equals("SHUTDOWN")) {
                                    LOG.info("Ask for framework shut down and exit");
                                    shutdown = true;
                                }
                                LOG.info("read (" + readLine + ")");
                                i = 0;
                                readChars = new char[255];
                            } else {
                                readChars[i] = (char) input;
                                i++;
                            }
                        }
                    }

                    if (shutdown) {
                        if (bundleContext.getBundle(0).getState() == Bundle.STARTING) {
                            bundleContext.addFrameworkListener(this);
                        } else {
                            bundleContext.getBundle(0).stop();
                        }
                    }

                    in.close();
                    clientSocket.close();
                }
            } catch (SocketException e) {
                LOG.info("Socket is closed (?)", e);
            } catch (IOException e) {
                LOG.error("Unable to read the socket", e);
            } catch (BundleException e) {
                LOG.error("Unable to stop the framework", e);
            } finally {
                try {
                    serverSocket.close();
                } catch (IOException e) {
                    LOG.error("Unable to close server socket", e);
                }
            }
        } catch (IOException e) {
            LOG.fatal("Could not listen on port: 4444.", e);
        }

    }

    public void frameworkEvent(FrameworkEvent event) {
        if (event.getType() == FrameworkEvent.STARTED) {
            // We were waiting for shutting down the framework, and we've just been notified of framework ACTIVE state,
            // so now we can do the job
            try {
                bundleContext.removeFrameworkListener(this);
                bundleContext.getBundle(0).stop();
            } catch (BundleException e) {
                LOG.error("Unable to stop the framework", e);
            }
        }
    }
}
