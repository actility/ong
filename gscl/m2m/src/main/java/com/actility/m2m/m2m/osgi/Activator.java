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
 * id $Id: Activator.java 8760 2014-05-21 15:31:19Z JReich $
 * author $Author: JReich $
 * version $Revision: 8760 $
 * lastrevision $Date: 2014-05-21 17:31:19 +0200 (Wed, 21 May 2014) $
 * modifiedby $LastChangedBy: JReich $
 * lastmodified $LastChangedDate: 2014-05-21 17:31:19 +0200 (Wed, 21 May 2014) $
 */

package com.actility.m2m.m2m.osgi;

import org.apache.log4j.Logger;
import org.osgi.framework.BundleActivator;
import org.osgi.framework.BundleContext;
import org.osgi.framework.ServiceReference;
import org.osgi.framework.ServiceRegistration;
import org.osgi.util.tracker.ServiceTracker;
import org.osgi.util.tracker.ServiceTrackerCustomizer;

import com.actility.m2m.m2m.M2MService;
import com.actility.m2m.m2m.impl.M2MServiceImpl;
import com.actility.m2m.m2m.log.BundleLogger;
import com.actility.m2m.servlet.song.service.SongService;
import com.actility.m2m.util.log.OSGiLogger;
import com.actility.m2m.xo.XoService;

public final class Activator implements BundleActivator {
    private static final Logger LOG = OSGiLogger.getLogger(Activator.class, BundleLogger.getStaticLogger());

    private M2MServiceImpl m2mService;
    private ServiceRegistration m2mServiceRegistration;
    private XoService xoService;
    private SongService songService;

    // BundleContext
    private BundleContext context;
    // ServiceTracker for ConfigurationAdmin
    private ServiceTracker xoServiceTracker;
    private ServiceTracker songServiceTracker;

    public void start(BundleContext context) throws Exception {
        BundleLogger.getStaticLogger().init(context);
        if (LOG.isInfoEnabled()) {
            LOG.info("Starting bundle " + context.getBundle().getSymbolicName() + "...");
        }
        this.context = context;

        if (LOG.isInfoEnabled()) {
            LOG.info("Starting tracker for " + XoService.class.getName() + " service...");
        }
        // init and start ServiceTracker to tracks elements
        xoServiceTracker = new ServiceTracker(context, XoService.class.getName(), new XoServiceCustomizer());
        xoServiceTracker.open();

        if (LOG.isInfoEnabled()) {
            LOG.info("Starting tracker for " + SongService.class.getName() + " service...");
        }
        songServiceTracker = new ServiceTracker(context, SongService.class.getName(), new SongServiceCustomizer());
        songServiceTracker.open();

    }

    public void stop(BundleContext context) throws Exception {
        if (LOG.isInfoEnabled()) {
            LOG.info("Stopping bundle " + context.getBundle().getSymbolicName() + "...");
            LOG.info("Stopping tracker for " + XoService.class.getName() + " service...");
        }
        xoServiceTracker.close();
        if (LOG.isInfoEnabled()) {
            LOG.info("Stopping tracker for " + SongService.class.getName() + " service...");
        }
        songServiceTracker.close();
        xoService = null;
        songService = null;
        xoServiceTracker = null;
        songServiceTracker = null;
        stopM2MService();
        BundleLogger.getStaticLogger().init(null);
    }

    private synchronized void startM2MService() {
        if (m2mService == null && songService != null && xoService != null) {
            if (LOG.isInfoEnabled()) {
                LOG.info("Registering service " + M2MService.class.getName() + "...");
            }
            m2mService = new M2MServiceImpl(songService, xoService);
            m2mServiceRegistration = context.registerService(M2MService.class.getName(), m2mService, null);
        }
    }

    private synchronized void stopM2MService() {
        if (m2mService != null) {
            if (LOG.isInfoEnabled()) {
                LOG.info("Unregistering service " + M2MService.class.getName() + "...");
            }
            // TODO : stop ne fait rien dans M2MServiceImpl
            // m2mService.stop();
            m2mServiceRegistration.unregister();
            m2mServiceRegistration = null;
            m2mService = null;
        }
    }

    // customizer that handles tracked service
    // registration/modification/unregistration events
    private class XoServiceCustomizer implements ServiceTrackerCustomizer {

        public Object addingService(ServiceReference reference) {
            if (LOG.isInfoEnabled()) {
                LOG.info("Adding service " + XoService.class.getName() + "...");
            }
            if (xoService != null) {
                return null;
            }
            xoService = (XoService) context.getService(reference);

            // Do something with the service
            startM2MService();
            // Return the service to track it
            return xoService;
        }

        public void modifiedService(ServiceReference reference, Object service) {
        }

        public void removedService(ServiceReference reference, Object service) {
            if (LOG.isInfoEnabled()) {
                LOG.info("Removing service " + XoService.class.getName() + "...");
            }
            if (service == xoService) {
                stopM2MService();
                xoService = null;
            }
        }
    }

    // customizer that handles tracked service
    // registration/modification/unregistration events
    private class SongServiceCustomizer implements ServiceTrackerCustomizer {

        public Object addingService(ServiceReference reference) {
            if (LOG.isInfoEnabled()) {
                LOG.info("Adding service " + SongService.class.getName() + "...");
            }
            if (songService != null) {
                return null;
            }
            songService = (SongService) context.getService(reference);

            // Do something with the service
            startM2MService();
            // Return the service to track it
            return songService;
        }

        public void modifiedService(ServiceReference reference, Object service) {
        }

        public void removedService(ServiceReference reference, Object service) {
            if (LOG.isInfoEnabled()) {
                LOG.info("Removing service " + SongService.class.getName() + "...");
            }
            if (service == songService) {
                stopM2MService();
                songService = null;
            }
        }
    }

}
