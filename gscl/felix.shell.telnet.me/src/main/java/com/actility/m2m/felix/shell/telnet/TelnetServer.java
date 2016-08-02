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
import java.io.InterruptedIOException;
import java.net.InetAddress;
import java.net.ServerSocket;
import java.net.Socket;
import java.net.SocketException;
import java.util.Dictionary;
import java.util.Enumeration;
import java.util.Hashtable;

import org.apache.log4j.Logger;
import org.osgi.framework.BundleActivator;
import org.osgi.framework.BundleContext;
import org.osgi.framework.Constants;
import org.osgi.framework.ServiceReference;
import org.osgi.framework.ServiceRegistration;
import org.osgi.service.cm.ConfigurationException;
import org.osgi.service.cm.ManagedService;
import org.osgi.util.tracker.ServiceTracker;
import org.osgi.util.tracker.ServiceTrackerCustomizer;

import com.actility.m2m.felix.shell.telnet.log.BundleLogger;
import com.actility.m2m.framework.resources.ResourcesAccessorService;
import com.actility.m2m.util.log.OSGiLogger;

/**
 * The telnet console server listens to a port for connections and upon accept, creates a telnet session to handle that
 * connection.
 */
public class TelnetServer implements BundleActivator, Runnable, ManagedService {
    private static final Logger LOG = OSGiLogger.getLogger(TelnetServer.class, BundleLogger.getStaticLogger());

    private TelnetConfig telnetConfig = null;

    private ServiceRegistration managedServiceRegistration = null;

    private Thread telnetServerThread;

    private Hashtable telnetSessions = null;

    private TelnetSession telnetSession = null;

    private boolean accept = true; // control of main loop running

    private boolean updated = true; // control of main loop server update

    private boolean first = true; // first time server start

    private ServerSocket serverSocket;

    private BundleContext context;
    private Dictionary config;

    private ServiceTracker resourcesAccessorServiceTracker;
    private ResourcesAccessorService resourcesAccessorService;

    public void start(BundleContext context) {
        BundleLogger.getStaticLogger().init(context);
        this.context = context;
        if (LOG.isInfoEnabled()) {
            LOG.info("Starting bundle " + context.getBundle().getSymbolicName() + "...");
            LOG.info("Starting tracker for " + ResourcesAccessorService.class.getName() + " service...");
        }

        // init and start ServiceTracker to tracks elements
        resourcesAccessorServiceTracker = new ServiceTracker(context, ResourcesAccessorService.class.getName(),
                new ResourcesAccessorServiceCustomizer());
        resourcesAccessorServiceTracker.open();

        config = null;
        Dictionary props = new Hashtable();
        String pid = context.getBundle().getSymbolicName() + ".config";
        props.put(Constants.SERVICE_PID, pid);

        if (LOG.isInfoEnabled()) {
            LOG.info("Registering service " + ManagedService.class.getName() + " for pid " + pid + "...");
        }
        managedServiceRegistration = context.registerService(ManagedService.class.getName(), this, props);
    }

    public void stop(BundleContext bc) {
        if (LOG.isInfoEnabled()) {
            LOG.info("Stopping bundle " + context.getBundle().getSymbolicName() + "...");
            LOG.info("Stopping tracker for " + ResourcesAccessorService.class.getName() + " service...");
        }
        resourcesAccessorServiceTracker.close();

        if (LOG.isInfoEnabled()) {
            String pid = context.getBundle().getSymbolicName() + ".config";
            LOG.info("Unregistering service " + ManagedService.class.getName() + " for pid " + pid + "...");
        }
        managedServiceRegistration.unregister();

        stopTelnetServer();

        resourcesAccessorServiceTracker = null;
        resourcesAccessorService = null;
        managedServiceRegistration = null;
        config = null;
        this.context = null;
        BundleLogger.getStaticLogger().init(null);
    }

    private void startTelnetServer() {
        if (resourcesAccessorService != null) {
            LOG.info("Starting Telnet Server...");
            telnetSessions = new Hashtable();

            try {
                telnetConfig = new TelnetConfig(resourcesAccessorService, context, config);
            } catch (ConfigurationException cexp) {
                LOG.error("Consoletelnet configuration error " + cexp.toString());
            }

            telnetServerThread = new Thread(this, "Felix Shell Telnet Server");
            telnetServerThread.start();
        }
    }

    private void stopTelnetServer() {
        if (telnetServerThread != null) {
            LOG.info("Stopping Telnet Server...");
            // Stop accepting new connections
            accept = false;
            if (serverSocket != null) {
                try {
                    serverSocket.close();
                } catch (IOException e) {
                    LOG.error("IOException while stopping telnet server socket", e);
                }
            }
            telnetServerThread.interrupt();
            try {
                telnetServerThread.join();
            } catch (InterruptedException e) {
                LOG.error("Interrupted while waiting the end of the telnet server thread", e);
            }

            // Close all pending sessions

            Enumeration e = telnetSessions.keys();
            while (e.hasMoreElements()) {
                telnetSession = (TelnetSession) e.nextElement();
                telnetSession.close();
            }
            telnetSessions.clear();

            telnetServerThread = null;
            serverSocket = null;
            telnetSessions = null;
            telnetConfig = null;
        }
    }

    // Telnet server main loop, listen for connections and
    // also look for configuration updates.

    public void run() {
        // int socketTimeout = 10000;
        // int backlog = 100;

        boolean bFailed = false;

        while (accept && !bFailed) {
            if (updated) {
                LOG.info("updating in server main loop");
                if (serverSocket != null) {
                    try {
                        serverSocket.close();
                    } catch (IOException e) {
                        LOG.error("Server socket exception ", e);
                        LOG.error("Port: " + telnetConfig.getPort());
                        bFailed = true;
                    }
                }
                try {
                    // serverSocket = new ServerSocket(telnetConfig.getPort());
                    serverSocket = new ServerSocket(telnetConfig.getPort(), telnetConfig.getBacklog(),
                            telnetConfig.getAddress());

                    LOG.info("listening on port " + telnetConfig.getPort());

                    updated = false;
                } catch (IOException iox) {
                    LOG.error("Server socket exception", iox);
                    updated = true;
                    bFailed = true;
                }
            } else {
                // System.out.print(":");
                try {
                    serverSocket.setSoTimeout(telnetConfig.getSocketTimeout());
                    Socket socket = serverSocket.accept();

                    TelnetSession telnetSession = new TelnetSession(socket, telnetConfig, context, this);
                    Thread telnetSessionThread = new Thread(telnetSession, "TelnetConsoleSession");
                    telnetSessions.put(telnetSession, telnetSessionThread);
                    telnetSessionThread.start();
                } catch (SocketException e) {
                    // Socket is closed
                    accept = false;
                } catch (InterruptedIOException iox) { // Timeout
                    // iox.printStackTrace();
                } catch (IOException iox) {
                    // iox.printStackTrace();
                    LOG.error("Server exception", iox);
                }
            }
        } // end while (accept)

        // Accept no connections, shut down server socket

        if (serverSocket != null) {
            try {
                serverSocket.close();
            } catch (IOException e) {
                LOG.error("Server socket exception " + e);
                LOG.error("Port: " + telnetConfig.getPort());
            }
        }
    } // end run

    public void removeSession(TelnetSession tels) {
        telnetSessions.remove(tels);
    }

    public TelnetConfig getTelnetConfig() {
        return telnetConfig;
    }

    public void updated(Dictionary dict) {
        // Get port and host of present config
        // If they differ from the new configuration,
        // set semaphore updated = true
        // so that the main loop may detect the change and
        // close and reopen the server socket

        config = dict;
        if (telnetServerThread != null) {
            try {
                boolean portupdated;
                boolean hostupdated = false;

                int oldport = telnetConfig.getPort();
                InetAddress oldinet = telnetConfig.getAddress();

                telnetConfig.updated(dict);

                int port = telnetConfig.getPort();
                InetAddress inet = telnetConfig.getAddress();

                // port change check
                if (oldport != port) {
                    portupdated = true;
                } else {
                    portupdated = false;
                }

                // inet address change check
                if (oldinet == null && inet == null) {
                    hostupdated = false;
                } else if (oldinet == null && inet != null) {
                    hostupdated = true;
                } else if (oldinet != null && inet == null) {
                    hostupdated = true;
                } else if (oldinet != null && inet != null) {
                    if (oldinet.equals(inet)) {
                        hostupdated = false;
                    } else {
                        hostupdated = true;
                    }
                }
                updated = portupdated || hostupdated || first;
                first = false;

                if (LOG.isInfoEnabled()) {
                    LOG.info("p " + portupdated);
                    LOG.info("h " + hostupdated);
                    LOG.info("upda " + updated);
                }
            } catch (Exception ex) {
                LOG.error("Consoltelnet config exception " + ex);
                ex.printStackTrace();
            }
        }
    }

    // customizer that handles tracked service
    // registration/modification/unregistration events
    private class ResourcesAccessorServiceCustomizer implements ServiceTrackerCustomizer {

        public Object addingService(ServiceReference reference) {
            if (LOG.isInfoEnabled()) {
                LOG.info("Adding service " + ResourcesAccessorService.class.getName() + "...");
            }
            if (resourcesAccessorService != null) {
                return null;
            }
            resourcesAccessorService = (ResourcesAccessorService) context.getService(reference);

            // Do something with the service
            startTelnetServer();
            // Return the service to track it
            return resourcesAccessorService;
        }

        public void modifiedService(ServiceReference reference, Object service) {
        }

        public void removedService(ServiceReference reference, Object service) {
            if (LOG.isInfoEnabled()) {
                LOG.info("Removing service " + ResourcesAccessorService.class.getName() + "...");
            }
            if (service == resourcesAccessorService) {
                stopTelnetServer();
                resourcesAccessorService = null;
            }
        }
    }
}
