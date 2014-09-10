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
 * id $Id: SongCoAPBindingActivator.java 8769 2014-05-21 15:43:39Z JReich $
 * author $Author: JReich $
 * version $Revision: 8769 $
 * lastrevision $Date: 2014-05-21 17:43:39 +0200 (Wed, 21 May 2014) $
 * modifiedby $LastChangedBy: JReich $
 * lastmodified $LastChangedDate: 2014-05-21 17:43:39 +0200 (Wed, 21 May 2014) $
 */

package com.actility.m2m.servlet.song.coap.osgi;

import java.net.InetAddress;
import java.net.UnknownHostException;

import javax.servlet.ServletContext;
import javax.servlet.ServletException;

import org.apache.log4j.Logger;
import org.osgi.framework.BundleActivator;
import org.osgi.framework.BundleContext;
import org.osgi.framework.ServiceReference;
import org.osgi.util.tracker.ServiceTracker;
import org.osgi.util.tracker.ServiceTrackerCustomizer;

import com.actility.m2m.servlet.NamespaceException;
import com.actility.m2m.servlet.song.SongBindingFacade;
import com.actility.m2m.servlet.song.coap.CoAPBindingFacade;
import com.actility.m2m.servlet.song.coap.SongCoAPBinding;
import com.actility.m2m.servlet.song.coap.log.BundleLogger;
import com.actility.m2m.servlet.song.service.SongBindingService;
import com.actility.m2m.song.binding.coap.ni.api.NiSongBindingCoapService;
import com.actility.m2m.util.log.OSGiLogger;

/**
 * OSGi activator for the SONG HTTP Binding
 */
public final class SongCoAPBindingActivator implements BundleActivator {
    private static final Logger LOG = OSGiLogger.getLogger(SongCoAPBindingActivator.class, BundleLogger.getStaticLogger());

    private static final String[] PROTOCOLS = new String[] { "coap", "coaps" };

    private String bindingPath;
    private ServletContext applicationContext;
    private SongBindingService songBindingService;
    private NiSongBindingCoapService niSongBindingCoapService;
    private String serverAddress;
    private int serverPort;
    private SongCoAPBinding songCoAPBinding;

    // BundleContext
    private BundleContext context;
    // ServiceTracker for dIa native service (NiSongBindingCoapService)
    private ServiceTracker songBindingServiceTracker;
    private ServiceTracker niSongBindingCoapServiceTracker;

    /**
     * Activates the SONG CoAP Binding.
     *
     * @throws UnknownHostException In case the local host name could be determined
     * @throws ServletException In case a problem occurs while registering servlets
     * @throws NamespaceException In case a conflict of paths occurs while registering a SONG servlet
     */
    public void start(BundleContext context) throws Exception {
        this.context = context;
        BundleLogger.getStaticLogger().init(context);
        if (LOG.isInfoEnabled()) {
            LOG.info("Starting bundle " + context.getBundle().getSymbolicName() + "...");
            LOG.info("Starting tracker for " + SongBindingService.class.getName() + " service...");
        }
        // init and start ServiceTracker to track PreferencesService
        songBindingServiceTracker = new ServiceTracker(context, SongBindingService.class.getName(),
                new SongBindingServiceCustomizer());
        songBindingServiceTracker.open();

        if (LOG.isInfoEnabled()) {
            LOG.info("Starting tracker for " + NiSongBindingCoapService.class.getName() + " service...");
        }
        niSongBindingCoapServiceTracker = new ServiceTracker(context, NiSongBindingCoapService.class.getName(),
                new NiSongBindingCoapServiceCustomizer());
        niSongBindingCoapServiceTracker.open();
    }

    /**
     * Deactivates the SONG HTTP Binding.
     */
    public void stop(BundleContext context) throws Exception {
        if (LOG.isInfoEnabled()) {
            LOG.info("Stopping bundle " + context.getBundle().getSymbolicName() + "...");
            LOG.info("Stopping tracker for " + SongBindingService.class.getName() + " service...");
        }
        songBindingServiceTracker.close();
        songBindingServiceTracker = null;

        if (LOG.isInfoEnabled()) {
            LOG.info("Stopping tracker for " + NiSongBindingCoapService.class.getName() + " service...");
        }
        niSongBindingCoapServiceTracker.close();
        niSongBindingCoapServiceTracker = null;

        stopSongBindingCoap();

        this.context = null;
        BundleLogger.getStaticLogger().init(null);
    }

    private void stopSongBindingCoap() {
        if (songCoAPBinding != null) {
            LOG.info("Starting song binding coap...");
            try {
                if (songBindingService != null) {
                    songBindingService.unregisterApplication(applicationContext);
                }
            } catch (RuntimeException e) {
                LOG.error("Problem while unregistering CoAP SONG Binding", e);
            } finally {
                songBindingService = null;
            }

            try {
                if (songCoAPBinding != null) {
                    songCoAPBinding.destroy();
                }
            } catch (RuntimeException e) {
                LOG.error("Problem while deleting CoAP SONG binding", e);
            } finally {
                songCoAPBinding = null;
            }
        }
    }

    private void startSongBindingCoap() {
        if ((songCoAPBinding == null) && (niSongBindingCoapService != null) && (songBindingService != null)) {

            LOG.info("Starting song binding coap...");
            boolean error = false;
            // Do something with the service
            try {
                bindingPath = "/coap";

                applicationContext = songBindingService.createApplication(bindingPath, "CoAP", null, null, null);

                songCoAPBinding = new SongCoAPBinding(niSongBindingCoapService, serverAddress, serverPort);
                SongBindingFacade bindingFacade = new CoAPBindingFacade();

                // TODO bug IPv6 not supported
                InetAddress serverInetAddress = InetAddress.getLocalHost();
                songBindingService.registerBindingServlet(applicationContext, "SONGBinding", songCoAPBinding, null,
                        bindingFacade, "coap", PROTOCOLS, true, SongCoAPBinding.DEFAULT_PROTOCOL,
                        serverInetAddress, serverPort, null);

                songCoAPBinding.startDia();
            } catch (UnknownHostException e) {
                LOG.error("Can't get the localhost address", e);
                error = true;
                // throw e;
            } catch (ServletException e) {
                if (applicationContext != null) {
                    songBindingService.unregisterApplication(applicationContext);
                }
                error = true;
                LOG.error("Can't initialize servlet", e);
                // throw e;
            } catch (NamespaceException e) {
                songBindingService.unregisterApplication(applicationContext);
                LOG.error("Problem with the name of the SONG Binding Servlet", e);
                error = true;
                // throw e;
            } catch (Throwable t) {
                songBindingService.unregisterApplication(applicationContext);
                LOG.error("Unexpected exception", t);
                error = true;
            } finally {
                if (error) {
                    stopSongBindingCoap();
                }
            }
        }
    }

    // customizer that handles tracked service
    // registration/modification/unregistration events
    private class SongBindingServiceCustomizer implements ServiceTrackerCustomizer {

        public Object addingService(ServiceReference reference) {
            if (LOG.isInfoEnabled()) {
                LOG.info("Adding service " + SongBindingService.class.getName() + "...");
            }
            if (songBindingService != null) {
                return null;
            }
            songBindingService = (SongBindingService) context.getService(reference);

            startSongBindingCoap();
            // Return the service to track it
            return songBindingService;
        }

        public void modifiedService(ServiceReference reference, Object service) {
        }

        public void removedService(ServiceReference reference, Object service) {
            if (LOG.isInfoEnabled()) {
                LOG.info("Removing service " + SongBindingService.class.getName() + "...");
            }
            if (service == songBindingService) {
                stopSongBindingCoap();
                songBindingService = null;
            }
        }
    }

    private class NiSongBindingCoapServiceCustomizer implements ServiceTrackerCustomizer {

        public Object addingService(ServiceReference reference) {
            if (LOG.isInfoEnabled()) {
                LOG.info("Adding service " + NiSongBindingCoapService.class.getName() + "...");
            }
            if (niSongBindingCoapService != null) {
                return null;
            }
            niSongBindingCoapService = (NiSongBindingCoapService) context.getService(reference);
            serverAddress = (String) reference.getProperty("serverAddress");
            serverPort = ((Integer) reference.getProperty("serverPort")).intValue();

            startSongBindingCoap();
            // Return the service to track it
            return niSongBindingCoapService;
        }

        public void modifiedService(ServiceReference reference, Object service) {
        }

        public void removedService(ServiceReference reference, Object service) {
            if (LOG.isInfoEnabled()) {
                LOG.info("Removing service " + NiSongBindingCoapService.class.getName() + "...");
            }
            if (service == niSongBindingCoapService) {
                stopSongBindingCoap();
                niSongBindingCoapService = null;
            }
        }
    }
}
