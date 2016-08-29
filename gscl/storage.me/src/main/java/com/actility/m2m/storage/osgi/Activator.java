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

package com.actility.m2m.storage.osgi;

import java.util.Dictionary;
import java.util.Enumeration;
import java.util.HashMap;
import java.util.Hashtable;
import java.util.Map;

import org.apache.log4j.Level;
import org.apache.log4j.Logger;
import org.osgi.framework.BundleActivator;
import org.osgi.framework.BundleContext;
import org.osgi.framework.Constants;
import org.osgi.framework.InvalidSyntaxException;
import org.osgi.framework.ServiceReference;
import org.osgi.framework.ServiceRegistration;
import org.osgi.service.cm.ConfigurationException;
import org.osgi.service.cm.ManagedService;
import org.osgi.util.tracker.ServiceTracker;
import org.osgi.util.tracker.ServiceTrackerCustomizer;

import com.actility.m2m.storage.StorageRequestExecutor;
import com.actility.m2m.storage.driver.StorageRequestDriverExecutor;
import com.actility.m2m.storage.impl.StorageRequestExecutorImpl;
import com.actility.m2m.storage.log.BundleLogger;
import com.actility.m2m.util.log.OSGiLogger;

public class Activator implements BundleActivator, ManagedService {
    private static final Logger LOG = OSGiLogger.getLogger(Activator.class, BundleLogger.LOG);

    private final static String DEFAULT_DRIVER_NAME = "mongodb";

    private BundleContext context;
    private StorageRequestExecutorImpl storageRequestExecutorImpl;
    private ServiceRegistration storageRequestExecutorRegistration;
    private ServiceRegistration managedServiceRegistration;
    private ServiceTracker storageRequestDriverExecutorServiceTracker;
    private StorageRequestDriverExecutor defaultStorageRequestDriverExecutor;
    private String defaultDriverName;
    private Dictionary config;

    private Object checkWithDefault(Dictionary config, String name, Class type, Object defaultValue) {
        Object result = defaultValue;
        Object value = config.get(name);
        if (value == null) {
            if (LOG.isEnabledFor(Level.WARN)) {
                LOG.warn("Configuration property " + name + " is null -> use default value instead " + defaultValue);
            }
        } else if (!type.equals(value.getClass())) {
            LOG.error("Configuration property " + name + " must be of type " + type.toString() + " while it is of type "
                    + value.getClass() + " -> use default value instead " + defaultValue);
        } else if (type != String.class || ((String) value).length() != 0) {
            result = value;
        }
        return result;
    }

    private String getDefaultDriverName(Dictionary config) {
        return ((String) checkWithDefault(config, "defaultDriverName", String.class, null));
    }

    public void start(BundleContext context) throws Exception {
        this.context = context;
        BundleLogger.LOG.init(context);
        if (LOG.isInfoEnabled()) {
            LOG.info("Starting bundle " + context.getBundle().getSymbolicName() + "...");
            LOG.info("Starting service " + context.getBundle().getSymbolicName() + "...");
        }

        config = null;
        String pid = context.getBundle().getSymbolicName() + ".config";
        if (LOG.isInfoEnabled()) {
            LOG.info("Registering service " + ManagedService.class.getName() + " for pid " + pid + "...");
        }
        Dictionary props = new Hashtable();
        props.put(Constants.SERVICE_PID, pid);
        managedServiceRegistration = context.registerService(ManagedService.class.getName(), this, props);
    }

    public void stop(BundleContext context) throws Exception {
        if (LOG.isInfoEnabled()) {
            LOG.info("Stopping bundle " + context.getBundle().getSymbolicName() + "...");
        }
        stopStorage();

        if (LOG.isInfoEnabled()) {
            String pid = context.getBundle().getSymbolicName() + ".config";
            LOG.info("Unregistering service " + ManagedService.class.getName() + " for pid " + pid + "...");
        }
        managedServiceRegistration.unregister();

        config = null;
        this.context = null;
        BundleLogger.LOG.init(null);
    }

    public void updated(Dictionary properties) throws ConfigurationException {
        if (LOG.isInfoEnabled()) {
            if (properties != null) {
                LOG.info("Received a configuration for pid " + context.getBundle().getSymbolicName() + ".config");
                Enumeration e = properties.keys();
                while (e.hasMoreElements()) {
                    String key = (String) e.nextElement();
                    Object value = properties.get(key);
                    LOG.info("  " + key + " -> " + value + " (" + value.getClass() + ")");
                }
            } else {
                LOG.info("Received a null configuration for pid " + context.getBundle().getSymbolicName() + ".config");
            }
        }
        config = properties;
        if (config == null) {
            config = new Hashtable();
        }
        if (storageRequestExecutorImpl != null) {
            stopStorage();
        }
        defaultDriverName = getDefaultDriverName(config);
        if (defaultDriverName == null) {
            defaultDriverName = context.getProperty(context.getBundle().getSymbolicName() + ".config.defaultDriverName");
        }
        if (defaultDriverName == null) {
            defaultDriverName = DEFAULT_DRIVER_NAME;
        }
        defaultStorageRequestDriverExecutor = null;
        startStorage();
    }

    private void startStorage() {
        if (config != null) {
            if (storageRequestDriverExecutorServiceTracker != null) {
                if (defaultStorageRequestDriverExecutor != null) {
                    try {
                        if (LOG.isInfoEnabled()) {
                            LOG.info("Registering service " + StorageRequestExecutor.class.getName() + "...");
                        }
                        Map/* <String, StorageRequestDriverExecutor> */drivers = new HashMap/*
                                                                                             * <String,
                                                                                             * StorageRequestDriverExecutor>
                                                                                             */();
                        ServiceReference[] references = context.getAllServiceReferences(
                                StorageRequestDriverExecutor.class.getName(), null);
                        if (references != null) {
                            for (int i = 0; i < references.length; ++i) {
                                ServiceReference reference = references[i];
                                LOG.info("Put " + (String) reference.getProperty(StorageRequestExecutor.CONFIG_DRIVER_NAME));
                                drivers.put(reference.getProperty(StorageRequestExecutor.CONFIG_DRIVER_NAME),
                                        context.getService(reference));
                            }
                        }
                        storageRequestExecutorImpl = new StorageRequestExecutorImpl(defaultDriverName, drivers);
                        storageRequestExecutorRegistration = context.registerService(StorageRequestExecutor.class.getName(),
                                storageRequestExecutorImpl, null);
                    } catch (InvalidSyntaxException e) {
                        LOG.error("Cannot create storage service", e);
                    }
                }
            } else {
                if (LOG.isInfoEnabled()) {
                    LOG.info("Starting tracker for " + StorageRequestDriverExecutor.class.getName() + " service...");
                }

                // init and start ServiceTrackers to tracks elements
                // for XoService
                storageRequestDriverExecutorServiceTracker = new ServiceTracker(context,
                        StorageRequestDriverExecutor.class.getName(), new StorageRequestDriverExecutorCustomizer());
                storageRequestDriverExecutorServiceTracker.open();
            }
        }
    }

    private void stopStorage() {
        if (storageRequestDriverExecutorServiceTracker != null) {
            if (LOG.isInfoEnabled()) {
                LOG.info("Stopping tracker for " + StorageRequestDriverExecutor.class.getName() + " service...");
            }
            storageRequestDriverExecutorServiceTracker.close();
            storageRequestDriverExecutorServiceTracker = null;
        }
        if (storageRequestExecutorImpl != null) {
            if (LOG.isInfoEnabled()) {
                LOG.info("Unregistering service " + StorageRequestExecutor.class.getName() + "...");
            }
            storageRequestExecutorRegistration.unregister();
            storageRequestExecutorRegistration = null;
            storageRequestExecutorImpl = null;
        }
    }

    // customizer that handles registration/modification/unregistration events for XoService
    private class StorageRequestDriverExecutorCustomizer implements ServiceTrackerCustomizer {

        public Object addingService(ServiceReference reference) {
            if (LOG.isInfoEnabled()) {
                LOG.info("Adding service " + StorageRequestDriverExecutor.class.getName() + "...");
            }
            StorageRequestDriverExecutor executor = (StorageRequestDriverExecutor) context.getService(reference);
            String driverName = (String) reference.getProperty(StorageRequestExecutor.CONFIG_DRIVER_NAME);
            if (storageRequestExecutorImpl != null) {
                storageRequestExecutorImpl.addDriver(executor, driverName);
            } else if (defaultDriverName.equals(defaultDriverName)) {
                defaultStorageRequestDriverExecutor = executor;
                startStorage();
            }

            // Return the service to track it
            return executor;
        }

        public void modifiedService(ServiceReference reference, Object service) {
        }

        public void removedService(ServiceReference reference, Object service) {
            if (LOG.isInfoEnabled()) {
                LOG.info("Removing service " + StorageRequestDriverExecutor.class.getName() + "...");
            }
            StorageRequestDriverExecutor executor = (StorageRequestDriverExecutor) context.getService(reference);
            if (storageRequestExecutorImpl != null) {
                String driverName = (String) reference.getProperty(StorageRequestExecutor.CONFIG_DRIVER_NAME);
                if (defaultDriverName.equals(driverName)) {
                    stopStorage();
                } else {
                    storageRequestExecutorImpl.removeDriver(executor, driverName);
                }
            }
        }
    }
}
