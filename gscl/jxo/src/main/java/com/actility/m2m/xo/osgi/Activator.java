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
 * id $Id: Activator.java 8758 2014-05-21 15:28:56Z JReich $
 * author $Author: JReich $
 * version $Revision: 8758 $
 * lastrevision $Date: 2014-05-21 17:28:56 +0200 (Wed, 21 May 2014) $
 * modifiedby $LastChangedBy: JReich $
 * lastmodified $LastChangedDate: 2014-05-21 17:28:56 +0200 (Wed, 21 May 2014) $
 */

package com.actility.m2m.xo.osgi;

import org.apache.log4j.Logger;
import org.osgi.framework.BundleActivator;
import org.osgi.framework.BundleContext;
import org.osgi.framework.ServiceReference;
import org.osgi.framework.ServiceRegistration;
import org.osgi.util.tracker.ServiceTracker;
import org.osgi.util.tracker.ServiceTrackerCustomizer;

import com.actility.m2m.util.log.OSGiLogger;
import com.actility.m2m.xo.XoService;
import com.actility.m2m.xo.impl.XoServiceImpl;
import com.actility.m2m.xo.log.BundleLogger;
import com.actility.m2m.xo.ni.NiXoService;

public final class Activator implements BundleActivator {

    private static final Logger LOG = OSGiLogger.getLogger(Activator.class, BundleLogger.getStaticLogger());

    private ServiceRegistration xoServiceRegistration;
    private ServiceTracker niXoServiceTracker;
    private NiXoService niXoService;
    private BundleContext context;

    public void start(BundleContext context) throws Exception {
        BundleLogger.getStaticLogger().init(context);
        this.context = context;
        if (LOG.isInfoEnabled()) {
            LOG.info("Starting bundle " + context.getBundle().getSymbolicName() + "...");
            LOG.info("Starting tracker for " + NiXoService.class.getName() + " service...");
        }

        // init and start ServiceTracker to tracks elements
        niXoServiceTracker = new ServiceTracker(context, NiXoService.class.getName(),
                new XoNativeServiceCustomizer());
        niXoServiceTracker.open();
    }

    public void stop(BundleContext context) throws Exception {
        if (LOG.isInfoEnabled()) {
            LOG.info("Stopping bundle " + context.getBundle().getSymbolicName() + "...");
            LOG.info("Stopping tracker for " + NiXoService.class.getName() + " service...");
        }
        niXoServiceTracker.close();
        niXoServiceTracker = null;

        stopXoService();
        BundleLogger.getStaticLogger().init(null);
    }

    private synchronized void startXoService() {
        if (niXoService != null) {
            if (LOG.isInfoEnabled()) {
                LOG.info("Registering service " + XoService.class.getName() + "...");
            }
            xoServiceRegistration = context.registerService(XoService.class.getName(), new XoServiceImpl(niXoService), null);
        }
    }

    private synchronized void stopXoService() {
        if (xoServiceRegistration != null) {
            if (LOG.isInfoEnabled()) {
                LOG.info("Unregistering service " + XoService.class.getName() + "...");
            }
            xoServiceRegistration.unregister();
            xoServiceRegistration = null;
        }
    }

    // customizer that handles tracked service
    // registration/modification/unregistration events
    private class XoNativeServiceCustomizer implements ServiceTrackerCustomizer {

        public Object addingService(ServiceReference reference) {
            if (LOG.isInfoEnabled()) {
                LOG.info("Adding service " + NiXoService.class.getName() + "...");
            }
            if (niXoService != null) {
                return null;
            }
            niXoService = (NiXoService) context.getService(reference);

            // Do something with the service
            startXoService();
            // Return the service to track it
            return niXoService;
        }

        public void modifiedService(ServiceReference reference, Object service) {
        }

        public void removedService(ServiceReference reference, Object service) {
            if (LOG.isInfoEnabled()) {
                LOG.info("Removing service " + NiXoService.class.getName() + "...");
            }
            if (service == niXoService) {
                stopXoService();
                niXoService = null;
            }
        }
    }
}
