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

package com.actility.m2m.log.osgi;

import java.util.Dictionary;
import java.util.Hashtable;

import org.osgi.framework.BundleActivator;
import org.osgi.framework.BundleContext;
import org.osgi.framework.Constants;
import org.osgi.framework.ServiceReference;
import org.osgi.framework.ServiceRegistration;
import org.osgi.service.cm.ConfigurationException;
import org.osgi.service.cm.ManagedService;
import org.osgi.service.log.LogReaderService;
import org.osgi.service.log.LogService;
import org.osgi.util.tracker.ServiceTracker;
import org.osgi.util.tracker.ServiceTrackerCustomizer;

import com.actility.m2m.framework.resources.ResourcesAccessorService;
import com.actility.m2m.log.impl.LogConfiguration;
import com.actility.m2m.log.impl.LogEntryImpl;
import com.actility.m2m.log.impl.LogReaderServiceImpl;
import com.actility.m2m.log.impl.LogServiceFactory;
import com.actility.m2m.log.impl.OsgiEventListener;

public final class Activator implements BundleActivator, ManagedService {
    private BundleContext context;
    private LogServiceFactory logServiceFactory;
    private LogReaderServiceImpl logReaderServiceFactory;
    private LogConfiguration logConfiguration;
    private OsgiEventListener osgiEventListener;
    private ServiceRegistration logReaderServiceRegistration;
    private ServiceRegistration logServiceRegistration;
    private ServiceRegistration managedServiceRegistration;
    private ServiceTracker resourcesAccessorServiceTracker;
    private ResourcesAccessorService resourcesAccessorService;
    private Dictionary config;

    // BundleActivator callback.
    public void start(BundleContext context) {
        this.context = context;
        // init and start ServiceTracker to tracks elements
        resourcesAccessorServiceTracker = new ServiceTracker(context, ResourcesAccessorService.class.getName(),
                new ResourcesAccessorServiceTracker());
        resourcesAccessorServiceTracker.open();

        config = null;
        Dictionary configProps = new Hashtable();
        configProps.put(Constants.SERVICE_PID, context.getBundle().getSymbolicName() + ".config");
        managedServiceRegistration = context.registerService(ManagedService.class.getName(), this, configProps);
    }

    // BundleActivator callback.
    public void stop(BundleContext context) {
        resourcesAccessorServiceTracker.close();
        stopLogService();
        config = null;
        resourcesAccessorServiceTracker = null;
        resourcesAccessorService = null;
        this.context = null;
    }

    private synchronized void startLogService() {
        if (resourcesAccessorService != null  && config != null) {
            logConfiguration = new LogConfiguration(context, resourcesAccessorService, config);
            logReaderServiceFactory = new LogReaderServiceImpl(context, logConfiguration);
            logServiceFactory = new LogServiceFactory(logReaderServiceFactory);

            context.addBundleListener(logConfiguration);

            // Catch all framework error and place in the log.
            osgiEventListener = new OsgiEventListener(logReaderServiceFactory);
            context.addFrameworkListener(osgiEventListener);
            context.addBundleListener(osgiEventListener);
            context.addServiceListener(osgiEventListener);

            // Register our services
            logReaderServiceRegistration = context.registerService(LogReaderService.class.getName(), logReaderServiceFactory,
                    null);
            logServiceRegistration = context.registerService(new String[] { LogService.class.getName(),
                    com.actility.m2m.log.LogService.class.getName() }, logServiceFactory, null);
        }
    }

    private synchronized void stopLogService() {
        if (logConfiguration != null) {
            context.removeBundleListener(logConfiguration);
            context.removeFrameworkListener(osgiEventListener);
            context.removeBundleListener(osgiEventListener);
            context.removeServiceListener(osgiEventListener);

            logReaderServiceRegistration.unregister();
            logServiceRegistration.unregister();
            managedServiceRegistration.unregister();

            logReaderServiceFactory.log(new LogEntryImpl(context.getBundle(), LogService.LOG_INFO, "Log stopped."));
            logReaderServiceFactory.stop();

            logServiceFactory = null;
            logReaderServiceFactory = null;
            logConfiguration = null;
            osgiEventListener = null;
            logReaderServiceRegistration = null;
            logServiceRegistration = null;
            managedServiceRegistration = null;
        }
    }

    public synchronized void updated(Dictionary properties) throws ConfigurationException {
        config = properties;
        if (config == null) {
            config = new Hashtable();
        }
        if (logConfiguration == null) {
            startLogService();
        } else {
            logConfiguration.updated(config);
        }
    }

    // customizer that handles tracked service
    // registration/modification/unregistration events
    private class ResourcesAccessorServiceTracker implements ServiceTrackerCustomizer {

        public Object addingService(ServiceReference reference) {
            if (resourcesAccessorService != null) {
                return null;
            }
            resourcesAccessorService = (ResourcesAccessorService) context.getService(reference);

            // Do something with the service
            startLogService();
            // Return the service to track it
            return resourcesAccessorService;
        }

        public void modifiedService(ServiceReference reference, Object service) {
        }

        public void removedService(ServiceReference reference, Object service) {
            if (service == resourcesAccessorService) {
                stopLogService();
                resourcesAccessorService = null;
            }
        }
    }
}
