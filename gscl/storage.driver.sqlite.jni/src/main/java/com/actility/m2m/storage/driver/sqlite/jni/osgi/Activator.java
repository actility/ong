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

package com.actility.m2m.storage.driver.sqlite.jni.osgi;

import java.io.BufferedReader;
import java.io.File;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.io.Reader;
import java.util.Dictionary;
import java.util.Enumeration;
import java.util.Hashtable;

import org.apache.log4j.Level;
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

import com.actility.m2m.framework.resources.ResourcesAccessorService;
import com.actility.m2m.storage.driver.sqlite.jni.impl.SqliteDriverJNIService;
import com.actility.m2m.storage.driver.sqlite.jni.log.BundleLogger;
import com.actility.m2m.storage.driver.sqlite.ni.SqliteDriverNIService;
import com.actility.m2m.util.FileUtils;
import com.actility.m2m.util.UtilConstants;
import com.actility.m2m.util.log.OSGiLogger;

public class Activator implements BundleActivator, ManagedService {
    private static final Logger LOG = OSGiLogger.getLogger(Activator.class, BundleLogger.LOG);

    private static final String DEFAULT_DB_FILE_NAME = "resources/storage.db";
    private static final Integer DEFAULT_MAX_DATABASE_SIZE_KB = new Integer(6144);
    private static final Integer DEFAULT_MIN_DURATION_BETWEEN_VACUUMS = new Integer(60000);
    private static final Integer DEFAULT_MAX_OPS_BEFORE_VACUUM = new Integer(500);
    private static final Integer DEFAULT_MAX_DURATION_BEFORE_VACUUM = new Integer(86400000);
    private static final Integer DEFAULT_MAX_DB_USAGE_BEFORE_VACUUM = new Integer(90);
    private static final Integer DEFAULT_MIN_DURATION_BETWEEN_DB_USAGE_COMPUTATION = new Integer(60000);

    private final static int MAJOR_VERSION = 3;
    private final static int LAST_MINOR_VERSION = 0;

    private BundleContext context;

    private ServiceRegistration managedServiceRegistration;

    private ServiceTracker resourcesAccessorServiceTracker;
    private ResourcesAccessorService resourcesAccessorService;

    private SqliteDriverJNIService sqliteDriverJniService;
    private ServiceRegistration sqliteDriverJniServiceRegistration;
    private Dictionary config;

    private Object checkWithDefault(Dictionary config, String name, Class type, Object defaultValue) {
        Object result = defaultValue;
        Object value = null;
        if (config != null) {
            value = config.get(name);
        }
        if (value == null) {
            if (LOG.isEnabledFor(Level.WARN)) {
                LOG.warn("Configuration property " + name + " is null -> use default value instead " + defaultValue);
            }
        } else if (!type.equals(value.getClass())) {
            LOG.error("Configuration property " + name + " must be of type " + type.toString() + " while it is of type "
                    + value.getClass() + " -> use default value instead " + defaultValue);
        } else {
            result = value;
        }
        return result;
    }

    private Integer getMaxDatabaseSizeKb(Dictionary config) {
        return (Integer) checkWithDefault(config, "maxDatabaseSizeKb", Integer.class, DEFAULT_MAX_DATABASE_SIZE_KB);
    }

    private Integer getMinDurationBetweenVacuums(Dictionary config) {
        return (Integer) checkWithDefault(config, "minDurationBetweenVacuums", Integer.class,
                DEFAULT_MIN_DURATION_BETWEEN_VACUUMS);
    }

    private Integer getMaxOpsBeforeVacuum(Dictionary config) {
        return (Integer) checkWithDefault(config, "maxOpsBeforeVacuum", Integer.class, DEFAULT_MAX_OPS_BEFORE_VACUUM);
    }

    private Integer getMaxDurationBeforeVacuum(Dictionary config) {
        return (Integer) checkWithDefault(config, "maxDurationBeforeVacuum", Integer.class, DEFAULT_MAX_DURATION_BEFORE_VACUUM);
    }

    private Integer getMaxDBUsagePourcentageBeforeVacuum(Dictionary config) {
        return (Integer) checkWithDefault(config, "maxDBUsagePourcentageBeforeVacuum", Integer.class,
                DEFAULT_MAX_DB_USAGE_BEFORE_VACUUM);
    }

    private Integer getMinDurationBetweenDBUsageComputation(Dictionary config) {
        return (Integer) checkWithDefault(config, "minDurationBetweenDBUsageComputation", Integer.class,
                DEFAULT_MIN_DURATION_BETWEEN_DB_USAGE_COMPUTATION);
    }

    public void start(BundleContext context) throws Exception {
        this.context = context;
        // Initialize log mechanism
        BundleLogger.LOG.init(context);

        String pid = context.getBundle().getSymbolicName() + ".config";
        if (LOG.isInfoEnabled()) {
            LOG.info("Starting bundle " + context.getBundle().getSymbolicName() + "...");
            LOG.info("Starting tracker for " + ResourcesAccessorService.class.getName() + " service...");
        }
        // init and start ServiceTrackers to tracks elements
        // for ResourcesAccessorService
        resourcesAccessorServiceTracker = new ServiceTracker(context, ResourcesAccessorService.class.getName(),
                new ResourcesAccessorServiceCustomizer());
        resourcesAccessorServiceTracker.open();

        if (LOG.isInfoEnabled()) {
            LOG.info("Registering service " + ManagedService.class.getName() + " for pid " + pid + "...");
        }

        // for ConfigurationAdmin
        config = null;
        Dictionary props = new Hashtable();
        props.put(Constants.SERVICE_PID, pid);
        managedServiceRegistration = context.registerService(ManagedService.class.getName(), this, props);
    }

    public void stop(BundleContext context) throws Exception {
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

        stopSqliteDriverService();

        resourcesAccessorServiceTracker = null;
        resourcesAccessorService = null;
        managedServiceRegistration = null;
        config = null;
        this.context = null;
        BundleLogger.LOG.init(null);
    }

    private synchronized void startSqliteDriverService() {
        if ((config != null) && (resourcesAccessorService != null)) {
            sqliteDriverJniService = new SqliteDriverJNIService();

            File rootact = new File(resourcesAccessorService.getProperty(context, UtilConstants.FW_PROP_ROOTACT));
            boolean dbExists = false;
            String actualDBFileName = DEFAULT_DB_FILE_NAME;
            Object fileNameConfig = config.get("fileName");
            if (fileNameConfig != null && fileNameConfig instanceof String) {
                actualDBFileName = (String) fileNameConfig;
            } else {
                LOG.warn("Using default db file name");
            }
            // Test if file exists
            File dbFile = FileUtils.normalizeFile(rootact, actualDBFileName);

            dbExists = dbFile.exists();
            if (LOG.isInfoEnabled()) {
                LOG.info("dbFile: " + dbFile.getAbsolutePath());
            }
            if (!dbExists) {
                LOG.warn("Creating database " + dbFile.getAbsolutePath());
            }

            sqliteDriverJniService.sqliteOpen(dbFile.getAbsolutePath());

            LOG.warn("Set log level");
            Object logLevelConfig = config.get("logLevel");
            if (logLevelConfig != null && logLevelConfig instanceof Integer) {
                if (LOG.isInfoEnabled()) {
                    LOG.info("Enable log level " + logLevelConfig + " on native sqlite driver");
                }
                Integer logLevel = (Integer) logLevelConfig;
                sqliteDriverJniService.setLogLevel(logLevel.intValue());
            }

            try {
                // read and execute script
                if (!dbExists) {
                    // make a copy of the database model and execute the sql script of creation
                    LOG.warn("Execute creation script");
                    readSqlScript();
                }
            } catch (IOException e) {
                e.printStackTrace();
            }
            LOG.warn("Check DB version");
            // Check database version
            sqliteDriverJniService.checkDBVersion(MAJOR_VERSION, LAST_MINOR_VERSION);

            LOG.warn("Activate JNI service");
            // turn on auto_vacuum incremental
            sqliteDriverJniService.activate();

            LOG.warn("Set config");
            sqliteDriverJniService.setMaxDatabaseSizeKb(getMaxDatabaseSizeKb(config).intValue());
            sqliteDriverJniService.setMinDurationBetweenVacuums(getMinDurationBetweenVacuums(config).intValue());
            sqliteDriverJniService.setMaxOpsBeforeVacuum(getMaxOpsBeforeVacuum(config).intValue());
            sqliteDriverJniService.setMaxDurationBeforeVacuum(getMaxDurationBeforeVacuum(config).intValue());
            sqliteDriverJniService
                    .setMaxDBUsagePourcentageBeforeVacuum(getMaxDBUsagePourcentageBeforeVacuum(config).intValue());
            sqliteDriverJniService.setMinDurationBetweenDBUsageComputation(getMinDurationBetweenDBUsageComputation(config)
                    .intValue());

            if (LOG.isInfoEnabled()) {
                LOG.info("Registering service " + SqliteDriverNIService.class.getName() + "...");
            }
            sqliteDriverJniServiceRegistration = context.registerService(SqliteDriverNIService.class.getName(),
                    sqliteDriverJniService, null);
        }
    }

    private void readSqlScript() throws IOException {
        LOG.debug("Executing db creation script");

        InputStream in = resourcesAccessorService.getResourceAsStream(context.getBundle(), "/sql/createGscl.sql");
        Reader otherReader = new InputStreamReader(in);
        BufferedReader br = new BufferedReader(otherReader);
        StringBuffer statement = new StringBuffer();
        String line = br.readLine();
        while (line != null) {
            LOG.debug(line);
            if (!line.startsWith("--")) {
                statement.append(line);
                if (line.endsWith(";")) {
                    sqliteDriverJniService.executeSqlStatementWithOutParameters(statement.toString());
                    statement = new StringBuffer();
                }
            }
            line = br.readLine();
        }
    }

    /**
     * Everything necessary when the driver is closing
     *
     */
    private synchronized void stopSqliteDriverService() {
        if (sqliteDriverJniService != null) {
            if (LOG.isInfoEnabled()) {
                LOG.info("Unregistering service " + SqliteDriverNIService.class.getName() + "...");
            }
            sqliteDriverJniServiceRegistration.unregister();
            sqliteDriverJniService.sqliteClose();

            sqliteDriverJniServiceRegistration = null;
            sqliteDriverJniService = null;
        }
    }

    public synchronized void updated(Dictionary properties) throws ConfigurationException {
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
        stopSqliteDriverService();
        startSqliteDriverService();
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

            startSqliteDriverService();
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
                stopSqliteDriverService();
                resourcesAccessorService = null;
            }
        }
    }
}
