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
 * id $Id: Activator.java 8765 2014-05-21 15:37:33Z JReich $
 * author $Author: JReich $
 * version $Revision: 8765 $
 * lastrevision $Date: 2014-05-21 17:37:33 +0200 (Wed, 21 May 2014) $
 * modifiedby $LastChangedBy: JReich $
 * lastmodified $LastChangedDate: 2014-05-21 17:37:33 +0200 (Wed, 21 May 2014) $
 */

package com.actility.m2m.shutdown.osgi;

import org.apache.log4j.Logger;
import org.osgi.framework.BundleActivator;
import org.osgi.framework.BundleContext;
import org.osgi.framework.ServiceReference;
import org.osgi.util.tracker.ServiceTracker;
import org.osgi.util.tracker.ServiceTrackerCustomizer;

import com.actility.m2m.framework.resources.ResourcesAccessorService;
import com.actility.m2m.shutdown.ShutdownSocketListener;
import com.actility.m2m.shutdown.hook.ni.NiShutdownHookService;
import com.actility.m2m.shutdown.log.BundleLogger;
import com.actility.m2m.util.log.OSGiLogger;

/**
 * @author mlouiset
 *
 */
public class Activator implements BundleActivator {
    private static final Logger LOG = OSGiLogger.getLogger(Activator.class, BundleLogger.getStaticLogger());

    private static final int DEFAULT_SERVER_PORT = 4444;

    private Thread socketListenerThread;
    private ShutdownSocketListener socketListener;
    private ServiceTracker resourcesAccessorServiceTracker;
    private ResourcesAccessorService resourcesAccessorService;
    private ServiceTracker niShutdownHookServiceTracker;
    private NiShutdownHookService niShutdownHookService;
    private BundleContext context;

    private int getServerPort(BundleContext context) {
        int portValue = DEFAULT_SERVER_PORT;
        String defaultServerPort = resourcesAccessorService.getProperty(context, context.getBundle().getSymbolicName()
                + ".config.serverPort");
        if (defaultServerPort != null) {
            defaultServerPort = defaultServerPort.trim();
            if (defaultServerPort.length() != 0) {
                try {
                    portValue = Integer.parseInt(defaultServerPort);
                } catch (NumberFormatException e) {
                    LOG.error("Framework property " + context.getBundle().getSymbolicName()
                            + ".config.serverPort is not a valid number: " + defaultServerPort + " use default value "
                            + portValue + " instead");
                }
            }
        }
        return portValue;
    }

    /**
     * Called to start the current bundle.
     *
     * @param context The bundle context associated to this bundle activation
     */
    public void start(BundleContext context) throws Exception {
        // Initialize log mechanism
        this.context = context;
        BundleLogger.getStaticLogger().init(context);

        if (LOG.isInfoEnabled()) {
            LOG.info("Starting bundle " + context.getBundle().getSymbolicName() + "...");
            LOG.info("Starting tracker for " + NiShutdownHookService.class.getName() + " service...");
        }

        // init and start ServiceTracker to tracks elements
        niShutdownHookServiceTracker = new ServiceTracker(context, NiShutdownHookService.class.getName(),
                new ShutdownHookServiceCustomizer());
        niShutdownHookServiceTracker.open();

        if (LOG.isInfoEnabled()) {
            LOG.info("Starting tracker for " + ResourcesAccessorService.class.getName() + " service...");
        }

        // init and start ServiceTracker to tracks elements
        resourcesAccessorServiceTracker = new ServiceTracker(context, ResourcesAccessorService.class.getName(),
                new ResourcesAccessorServiceTracker());
        resourcesAccessorServiceTracker.open();
    }

    /**
     * Called to deactivate the current bundle.
     */
    public void stop(BundleContext context) throws Exception {
        if (LOG.isInfoEnabled()) {
            LOG.info("Stopping bundle " + context.getBundle().getSymbolicName() + "...");
            LOG.info("Stopping tracker for " + NiShutdownHookService.class.getName() + " service...");
        }

        niShutdownHookServiceTracker.close();
        niShutdownHookServiceTracker = null;

        if (LOG.isInfoEnabled()) {
            LOG.info("Stopping tracker for " + ResourcesAccessorService.class.getName() + " service...");
        }

        // init and start ServiceTracker to tracks elements
        resourcesAccessorServiceTracker.close();
        resourcesAccessorServiceTracker = null;

        stopShutdownHook();

        this.context = null;
        BundleLogger.getStaticLogger().init(null);
    }

    private void startShutdownHook() {
        if (niShutdownHookService != null && resourcesAccessorService != null) {
            LOG.info("Starting shutdown hook...");
            int serverPort = getServerPort(context);
            niShutdownHookService.setSignalHandler(serverPort);

            socketListener = new ShutdownSocketListener(context, serverPort);
            socketListenerThread = new Thread(socketListener, "Shutdown-hook thread");
            socketListenerThread.start();
        }
    }

    private void stopShutdownHook() {
        if (socketListener != null) {
            LOG.info("Stopping shutdown hook...");
            socketListener.stop();
            socketListenerThread.interrupt();
            try {
                socketListenerThread.join();
            } catch (InterruptedException e) {
                LOG.error("Unable to join with the socket listener thread", e);
            }
            socketListener = null;
        }
    }

    // customizer that handles tracked service
    // registration/modification/unregistration events
    private class ShutdownHookServiceCustomizer implements ServiceTrackerCustomizer {

        public Object addingService(ServiceReference reference) {
            if (LOG.isInfoEnabled()) {
                LOG.info("Adding service " + NiShutdownHookService.class.getName() + "...");
            }
            if (niShutdownHookService != null) {
                return null;
            }
            niShutdownHookService = (NiShutdownHookService) context.getService(reference);

            // Do something with the service
            startShutdownHook();
            // Return the service to track it
            return niShutdownHookService;
        }

        public void modifiedService(ServiceReference reference, Object service) {
        }

        public void removedService(ServiceReference reference, Object service) {
            if (LOG.isInfoEnabled()) {
                LOG.info("Removing service " + NiShutdownHookService.class.getName() + "...");
            }
            if (service == niShutdownHookService) {
                stopShutdownHook();
                niShutdownHookService = null;
            }
        }
    }

    // customizer that handles tracked service
    // registration/modification/unregistration events
    private class ResourcesAccessorServiceTracker implements ServiceTrackerCustomizer {

        public Object addingService(ServiceReference reference) {
            if (LOG.isInfoEnabled()) {
                LOG.info("Adding service " + ResourcesAccessorService.class.getName() + "...");
            }
            if (resourcesAccessorService != null) {
                return null;
            }
            resourcesAccessorService = (ResourcesAccessorService) context.getService(reference);

            // Do something with the service
            startShutdownHook();
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
                stopShutdownHook();
                resourcesAccessorService = null;
            }
        }
    }

}
