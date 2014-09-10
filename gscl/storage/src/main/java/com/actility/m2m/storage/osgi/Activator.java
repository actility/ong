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

package com.actility.m2m.storage.osgi;

import org.apache.log4j.Logger;
import org.osgi.framework.BundleActivator;
import org.osgi.framework.BundleContext;
import org.osgi.framework.ServiceReference;
import org.osgi.framework.ServiceRegistration;
import org.osgi.util.tracker.ServiceTracker;
import org.osgi.util.tracker.ServiceTrackerCustomizer;

import com.actility.m2m.framework.resources.ResourcesAccessorService;
import com.actility.m2m.storage.ConditionBuilder;
import com.actility.m2m.storage.StorageRequestExecutor;
import com.actility.m2m.storage.impl.ConditionBuilderImpl;
import com.actility.m2m.storage.impl.StorageRequestExecutorImpl;
import com.actility.m2m.storage.log.BundleLogger;
import com.actility.m2m.util.log.OSGiLogger;

public class Activator implements BundleActivator {

    private static final Logger LOG = OSGiLogger.getLogger(Activator.class, BundleLogger.getStaticLogger());

    private BundleContext context;
    private StorageRequestExecutorImpl storageRequestExecutorImpl;
    private ServiceRegistration storageRequestExecutorRegistration;
    private ConditionBuilderImpl conditionBuilderImpl;
    private ServiceRegistration conditionBuilderRegistration;

    // ServiceTracker for ConfigAdmin
    private ServiceTracker resourcesAccessorServiceTracker;
    private ResourcesAccessorService resourcesAccessorService;

    public void start(BundleContext context) throws Exception {
        this.context = context;
        BundleLogger.getStaticLogger().init(context);
        if (LOG.isInfoEnabled()) {
            LOG.info("Starting bundle " + context.getBundle().getSymbolicName() + "...");
            LOG.info("Starting tracker for " + ResourcesAccessorService.class.getName() + " service...");
        }

        resourcesAccessorServiceTracker = new ServiceTracker(context, ResourcesAccessorService.class.getName(),
                new ResourcesAccessorServiceCustomizer());
        resourcesAccessorServiceTracker.open();
    }

    public void stop(BundleContext context) throws Exception {
        if (LOG.isInfoEnabled()) {
            LOG.info("Stopping bundle " + context.getBundle().getSymbolicName() + "...");
            LOG.info("Stopping tracker for " + ResourcesAccessorService.class.getName() + " service...");
        }
        resourcesAccessorServiceTracker.close();

        stopStorage();
        resourcesAccessorServiceTracker = null;
        resourcesAccessorService = null;
        this.context = null;
        BundleLogger.getStaticLogger().init(null);
    }

    private void startStorage() {
        if (resourcesAccessorService != null) {
            String defaultDriverName = resourcesAccessorService.getProperty(context, context.getBundle().getSymbolicName()
                    + ".config.defaultDriverName");

            if (LOG.isInfoEnabled()) {
                LOG.info("Registering service " + StorageRequestExecutor.class.getName() + "...");
            }
            storageRequestExecutorImpl = new StorageRequestExecutorImpl(defaultDriverName, context);
            storageRequestExecutorRegistration = context.registerService(StorageRequestExecutor.class.getName(),
                    storageRequestExecutorImpl, null);

            if (LOG.isInfoEnabled()) {
                LOG.info("Registering service " + ConditionBuilder.class.getName() + "...");
            }
            conditionBuilderImpl = new ConditionBuilderImpl();
            conditionBuilderRegistration = context
                    .registerService(ConditionBuilder.class.getName(), conditionBuilderImpl, null);
        }
    }

    private void stopStorage() {
        if (storageRequestExecutorRegistration != null) {
            if (LOG.isInfoEnabled()) {
                LOG.info("Unregistering service " + StorageRequestExecutor.class.getName() + "...");
            }
            storageRequestExecutorRegistration.unregister();
            storageRequestExecutorRegistration = null;
            storageRequestExecutorImpl = null;

            if (LOG.isInfoEnabled()) {
                LOG.info("Unregistering service " + ConditionBuilder.class.getName() + "...");
            }
            conditionBuilderRegistration.unregister();
            conditionBuilderRegistration = null;
            conditionBuilderImpl = null;
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
            startStorage();
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
                stopStorage();
                resourcesAccessorService = null;
            }
        }
    }
}
