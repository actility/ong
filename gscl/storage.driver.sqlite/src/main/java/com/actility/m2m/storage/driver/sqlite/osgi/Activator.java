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
import org.osgi.framework.ServiceReference;
import org.osgi.framework.ServiceRegistration;
import org.osgi.util.tracker.ServiceTracker;
import org.osgi.util.tracker.ServiceTrackerCustomizer;

import com.actility.m2m.storage.driver.StorageRequestDriverExecutor;
import com.actility.m2m.storage.driver.sqlite.impl.BackupCommand;
import com.actility.m2m.storage.driver.sqlite.impl.StorageRequestDriverExecutorImpl;
import com.actility.m2m.storage.driver.sqlite.log.BundleLogger;
import com.actility.m2m.storage.driver.sqlite.ni.SqliteDriverNIService;
import com.actility.m2m.util.log.OSGiLogger;

public class Activator implements BundleActivator {

    private static final Logger LOG = OSGiLogger.getLogger(Activator.class, BundleLogger.getStaticLogger());

    private static final String DEFAULT_DRIVER_NAME = "sqlite";

    private BundleContext context;

    private ServiceRegistration storageRequestDriverExecutorRegistration;
    private ServiceRegistration commandRegistration;

    private ServiceTracker sqliteDriverNIServiceTracker;
    private SqliteDriverNIService sqliteDriverNIService;

    /**
     * Creates a new database if the existing doesn't exists and checks the database version of the database.
     *
     * @param bundleContext
     * @param config
     */
    public void start(BundleContext context) throws Exception {
        this.context = context;
        // Initialize log mechanism
        BundleLogger.getStaticLogger().init(context);
        if (LOG.isInfoEnabled()) {
            LOG.info("Starting bundle " + context.getBundle().getSymbolicName() + "...");
            LOG.info("Starting tracker for " + SqliteDriverNIService.class.getName() + " service...");
        }

        // for ResourcesAccessorService
        sqliteDriverNIServiceTracker = new ServiceTracker(context, SqliteDriverNIService.class.getName(),
                new SqliteDriverServiceCustomizer());
        sqliteDriverNIServiceTracker.open();
    }

    public void stop(BundleContext context) throws Exception {
        if (LOG.isInfoEnabled()) {
            LOG.info("Stopping bundle " + context.getBundle().getSymbolicName() + "...");
            LOG.info("Stopping tracker for " + SqliteDriverNIService.class.getName() + " service...");
        }

        sqliteDriverNIServiceTracker.close();

        stopSqliteDriverService();

        sqliteDriverNIServiceTracker = null;
        sqliteDriverNIService = null;
        this.context = null;
        BundleLogger.getStaticLogger().init(null);
    }

    private synchronized void startSqliteDriverService() {
        if ((sqliteDriverNIService != null)) {
            // Create configuration
            Dictionary props = new Hashtable();
            props.put(StorageRequestDriverExecutor.PARAM_DRIVER_NAME, DEFAULT_DRIVER_NAME);

            StorageRequestDriverExecutorImpl storageRequestDriverExecutor = new StorageRequestDriverExecutorImpl(
                    sqliteDriverNIService);

            if (LOG.isInfoEnabled()) {
                LOG.info("Registering service " + org.apache.felix.shell.Command.class.getName() + " for backup command...");
            }
            // Register the backup command service.
            commandRegistration = context.registerService(org.apache.felix.shell.Command.class.getName(), new BackupCommand(
                    sqliteDriverNIService), null);

            if (LOG.isInfoEnabled()) {
                LOG.info("Registering service " + StorageRequestDriverExecutor.class.getName() + "...");
            }
            storageRequestDriverExecutorRegistration = context.registerService(StorageRequestDriverExecutor.class.getName(),
                    storageRequestDriverExecutor, props);
        }
    }

    /**
     * Everything necessary when the driver is closing
     *
     */
    private synchronized void stopSqliteDriverService() {
        if (storageRequestDriverExecutorRegistration != null) {
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
        }
    }

    // customizer that handles tracked service
    // registration/modification/unregistration events
    private class SqliteDriverServiceCustomizer implements ServiceTrackerCustomizer {

        public Object addingService(ServiceReference reference) {
            if (LOG.isInfoEnabled()) {
                LOG.info("Adding service " + SqliteDriverNIService.class.getName() + "...");
            }
            if (sqliteDriverNIService != null) {
                return null;
            }
            sqliteDriverNIService = (SqliteDriverNIService) context.getService(reference);

            startSqliteDriverService();
            // Return the service to track it
            return sqliteDriverNIService;
        }

        public void modifiedService(ServiceReference reference, Object service) {
        }

        public void removedService(ServiceReference reference, Object service) {
            if (LOG.isInfoEnabled()) {
                LOG.info("Removing service " + SqliteDriverNIService.class.getName() + "...");
            }
            if (service == sqliteDriverNIService) {
                stopSqliteDriverService();
                sqliteDriverNIService = null;
            }
        }
    }
}
