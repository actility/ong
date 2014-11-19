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

package com.actility.m2m.storage.driver.sqlite.osgi;

import java.util.Dictionary;
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

import com.actility.m2m.storage.DeletionHandler;
import com.actility.m2m.storage.StorageException;
import com.actility.m2m.storage.StorageRequestExecutor;
import com.actility.m2m.storage.driver.StorageRequestDriverExecutor;
import com.actility.m2m.storage.driver.sqlite.impl.BackupCommand;
import com.actility.m2m.storage.driver.sqlite.impl.StorageRequestDriverExecutorImpl;
import com.actility.m2m.storage.driver.sqlite.log.BundleLogger;
import com.actility.m2m.util.log.OSGiLogger;

public final class Activator implements BundleActivator, ManagedService {
    private static final Logger LOG = OSGiLogger.getLogger(Activator.class, BundleLogger.LOG);
    private static final String DEFAULT_DRIVER_NAME = "sqlite";

    private BundleContext context;
    private StorageRequestDriverExecutorImpl storageRequestDriverExecutor;
    private ServiceRegistration storageRequestDriverExecutorRegistration;
    private ServiceRegistration commandRegistration;
    private ServiceRegistration managedServiceRegistration;
    private Dictionary config;
    private DeletionHandler deletionhandler;
    private ServiceTracker deletionHandlerTracker;

    /**
     * Creates a new database if the existing doesn't exists and checks the database version of the database.
     *
     * @param bundleContext
     * @param config
     */
    public void start(BundleContext context) throws Exception {
        this.context = context;
        // Initialize log mechanism
        BundleLogger.LOG.init(context);

        if (LOG.isInfoEnabled()) {
            LOG.info("Starting tracker for " + DeletionHandler.class.getName() + " service...");
        }

        // for ResourcesAccessorService
        deletionHandlerTracker = new ServiceTracker(context, DeletionHandler.class.getName(),
                new DeletionHandlerTracker());
        deletionHandlerTracker.open();

        String pid = context.getBundle().getSymbolicName() + ".config";
        if (LOG.isInfoEnabled()) {
            LOG.info("Registering service " + ManagedService.class.getName() + " for pid " + pid + "...");
        }
        config = null;
        Dictionary props = new Hashtable();
        props.put(Constants.SERVICE_PID, pid);
        managedServiceRegistration = context.registerService(ManagedService.class.getName(), this, props);
    }

    public void stop(BundleContext context) throws Exception {
        if (LOG.isInfoEnabled()) {
            LOG.info("Stopping tracker for " + DeletionHandler.class.getName() + " service...");
        }

        deletionHandlerTracker.close();
        deletionhandler = null;

        if (LOG.isInfoEnabled()) {
            String pid = context.getBundle().getSymbolicName() + ".config";
            LOG.info("Unregistering service " + ManagedService.class.getName() + " for pid " + pid + "...");
        }
        managedServiceRegistration.unregister();
        managedServiceRegistration = null;

        stopSqliteDriverService();

        this.context = null;
        BundleLogger.LOG.init(null);
    }

    private synchronized void startSqliteDriverService() {
        if (config != null) {
            try {
                // Create configuration
                Dictionary props = new Hashtable();
                props.put(StorageRequestExecutor.CONFIG_DRIVER_NAME, DEFAULT_DRIVER_NAME);

                storageRequestDriverExecutor = new StorageRequestDriverExecutorImpl(config);
                if (deletionhandler != null) {
                    storageRequestDriverExecutor.bindDeletionHandler(deletionhandler);
                }

                if (LOG.isInfoEnabled()) {
                    LOG.info("Registering service " + org.apache.felix.shell.Command.class.getName() + " for backup command...");
                }

                // Register the backup command service.
                commandRegistration = context.registerService(org.apache.felix.shell.Command.class.getName(),
                        new BackupCommand(storageRequestDriverExecutor.getSQLiteRequestExecutor()), null);

                if (LOG.isInfoEnabled()) {
                    LOG.info("Registering service " + StorageRequestDriverExecutor.class.getName() + "...");
                }
                storageRequestDriverExecutorRegistration = context.registerService(
                        StorageRequestDriverExecutor.class.getName(), storageRequestDriverExecutor, props);
            } catch (StorageException e) {
                LOG.error("Cannot start SQLite storage", e);
            }
        }
    }

    /**
     * Everything necessary when the driver is closing
     *
     */
    private synchronized void stopSqliteDriverService() {
        if (storageRequestDriverExecutor != null) {
            if (LOG.isInfoEnabled()) {
                LOG.info("Unregistering service " + org.apache.felix.shell.Command.class.getName() + " for backup command...");
            }
            commandRegistration.unregister();

            if (LOG.isInfoEnabled()) {
                LOG.info("Unregistering service " + StorageRequestDriverExecutor.class.getName() + "...");
            }
            storageRequestDriverExecutorRegistration.unregister();

            commandRegistration = null;
            storageRequestDriverExecutorRegistration = null;
            storageRequestDriverExecutor = null;
        }
    }

    public void updated(Dictionary properties) throws ConfigurationException {
        LOG.info("Configuration updated...");

        config = properties;
        if (config == null) {
            config = new Hashtable();
        }
        if (storageRequestDriverExecutor != null) {
            stopSqliteDriverService();
        }
        startSqliteDriverService();
    }

    // customizer that handles tracked service
    // registration/modification/unregistration events
    private class DeletionHandlerTracker implements ServiceTrackerCustomizer {

        public Object addingService(ServiceReference reference) {
            if (LOG.isInfoEnabled()) {
                LOG.info("Adding service " + DeletionHandler.class.getName() + "...");
            }
            synchronized (Activator.this) {
                if (deletionhandler != null) {
                    return null;
                }
                deletionhandler = (DeletionHandler) context.getService(reference);

                if (storageRequestDriverExecutor != null) {
                    storageRequestDriverExecutor.bindDeletionHandler(deletionhandler);
                }
                // Return the service to track it
                return deletionhandler;
            }
        }

        public void modifiedService(ServiceReference reference, Object service) {
        }

        public void removedService(ServiceReference reference, Object service) {
            if (LOG.isInfoEnabled()) {
                LOG.info("Removing service " + DeletionHandler.class.getName() + "...");
            }
            synchronized (Activator.this) {
                if (service == deletionhandler) {
                    if (storageRequestDriverExecutor != null) {
                        storageRequestDriverExecutor.unbindDeletionHandler(deletionhandler);
                    }
                    deletionhandler = null;
                }
            }
        }
    }
}
