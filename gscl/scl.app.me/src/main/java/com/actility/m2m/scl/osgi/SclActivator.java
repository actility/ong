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
 * id $Id: SclActivator.java 4208 2013-01-24 08:50:51Z JReich $
 * author $Author: JReich $
 * version $Revision: 4208 $
 * lastrevision $Date: 2013-01-24 09:50:51 +0100 (Thu, 24 Jan 2013) $
 * modifiedby $LastChangedBy: JReich $
 * lastmodified $LastChangedDate: 2013-01-24 09:50:51 +0100 (Thu, 24 Jan 2013) $
 */

package com.actility.m2m.scl.osgi;

import java.io.File;
import java.io.IOException;
import java.net.URI;
import java.net.URISyntaxException;
import java.text.ParseException;
import java.util.Arrays;
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
import org.osgi.framework.ServiceReference;
import org.osgi.framework.ServiceRegistration;
import org.osgi.service.cm.ConfigurationException;
import org.osgi.service.cm.ManagedService;
import org.osgi.util.tracker.ServiceTracker;
import org.osgi.util.tracker.ServiceTrackerCustomizer;

import com.actility.m2m.framework.resources.ResourcesAccessorService;
import com.actility.m2m.m2m.M2MContext;
import com.actility.m2m.m2m.M2MException;
import com.actility.m2m.m2m.M2MService;
import com.actility.m2m.scl.impl.SclManagerImpl;
import com.actility.m2m.scl.log.BundleLogger;
import com.actility.m2m.scl.model.SclConfig;
import com.actility.m2m.storage.StorageException;
import com.actility.m2m.storage.StorageRequestExecutor;
import com.actility.m2m.util.FileUtils;
import com.actility.m2m.util.FormatUtils;
import com.actility.m2m.util.StringUtils;
import com.actility.m2m.util.UtilConstants;
import com.actility.m2m.util.log.OSGiLogger;
import com.actility.m2m.xo.XoException;
import com.actility.m2m.xo.XoService;

/**
 * OSGi Activator for the SCL bundle.
 *
 */
public final class SclActivator implements BundleActivator, ManagedService {
    private static final Logger LOG = OSGiLogger.getLogger(SclActivator.class, BundleLogger.getStaticLogger());

    private static final Integer DEFAULT_MAX_WATCH_QUEUE_SIZE = new Integer(50);
    private static final Integer DEFAULT_MAX_INIT_WATCHES = new Integer(50);
    private static final Integer DEFAULT_QUEUE_SIZE = new Integer(32);
    private static final Integer DEFAULT_MAX_SUBSCRIPTIONS = new Integer(100);
    private static final Integer DEFAULT_MAX_SUBSCRIPTIONS_PER_RESOURCE = new Integer(10);
    private static final Long DEFAULT_DEFAULT_EXPIRATION_DURATION = new Long(0L); // infinity
    private static final Long DEFAULT_MIN_EXPIRATION_DURATION = new Long(60000L); // PT1M
    private static final Long DEFAULT_MAX_EXPIRATION_DURATION = new Long(2592000000L); // P30D
    private static final Long DEFAULT_DEFAULT_MAX_BYTE_SIZE = new Long(20480L); // ~20Ko
    private static final Long DEFAULT_MIN_MAX_BYTE_SIZE = new Long(10L);
    private static final Long DEFAULT_MAX_MAX_BYTE_SIZE = new Long(1024000L); // 1 Mo
    private static final Long DEFAULT_DEFAULT_MAX_INSTANCE_AGE = new Long(0L); // infinity
    private static final Long DEFAULT_MIN_MAX_INSTANCE_AGE = new Long(86400000L); // P1D
    private static final Long DEFAULT_MAX_MAX_INSTANCE_AGE = new Long(31536000000L); // P1Y
    private static final Long DEFAULT_DEFAULT_MAX_NR_OF_INSTANCES = new Long(20L);
    private static final Long DEFAULT_MIN_MAX_NR_OF_INSTANCES = new Long(1L);
    private static final Long DEFAULT_MAX_MAX_NR_OF_INSTANCES = new Long(200L);
    private static final Long DEFAULT_NOTIF_REQUEST_TIMEOUT = new Long(10000L);
    private static final Long DEFAULT_NOTIF_REPRESENTATION_TIMER = new Long(600000L);
    private static final Long DEFAULT_REGISTRATION_EXPIRATION_DURATION = new Long(1800000L); // PT30M
    private static final Long DEFAULT_REGISTRATION_RETRY_DURATION = new Long(5000L); // PT5S
    private static final Boolean DEFAULT_CONTENT_INSTANCE_CREATION_ON_NOTIFY = Boolean.TRUE;
    private static final int[] DEFAULT_NOTIF_BACKTRACKABLE_ERROR_CODES = new int[] { 503 };

    private BundleContext context;

    private ServiceRegistration managedServiceRegistration;

    private ServiceTracker xoServiceTracker = null;
    private XoService xoService = null;

    private ServiceTracker m2mServiceTracker = null;
    private M2MService m2mService = null;

    private ServiceTracker storageRequestExecutorTracker = null;
    private StorageRequestExecutor storageContext = null;

    private ServiceTracker resourcesAccessorServiceTracker = null;
    private ResourcesAccessorService resourcesAccessorService = null;

    private SclManagerImpl sclManager;
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

    private URI getNsclUri(BundleContext bundleContext, Dictionary config) {
        String nsclUriStr = (String) checkWithDefault(config, "nsclUri", String.class, null);
        URI nsclUri = null;
        try {
            nsclUri = (!StringUtils.isEmpty(nsclUriStr)) ? new URI(nsclUriStr) : null;
        } catch (URISyntaxException e) {
            LOG.error("Cannot decode the configuration parameter nsclUri as an URI: " + nsclUriStr, e);
        }
        if (nsclUri == null) {
            String propertyNsclUri = resourcesAccessorService.getProperty(bundleContext, bundleContext.getBundle()
                    .getSymbolicName() + ".config.nsclUri");
            if (!StringUtils.isEmpty(propertyNsclUri)) {
                try {
                    nsclUri = new URI(propertyNsclUri);
                } catch (URISyntaxException e) {
                    LOG.error("Cannot decode the property " + bundleContext.getBundle().getSymbolicName()
                            + ".config.nsclUri as an URI: " + propertyNsclUri, e);
                }
            }
        }
        return nsclUri;
    }

    private URI getTpkDevUri(BundleContext bundleContext, Dictionary config) {
        String tpkDevUriStr = (String) checkWithDefault(config, "tpkDevUri", String.class, null);
        URI tpkDevUri = null;
        try {
            tpkDevUri = (!StringUtils.isEmpty(tpkDevUriStr)) ? new URI(tpkDevUriStr) : null;
        } catch (URISyntaxException e) {
            LOG.error("Cannot decode the configuration parameter tpkDevUri as an URI: " + tpkDevUriStr, e);
        }
        if (tpkDevUri == null) {
            String propertyTpkDevUri = resourcesAccessorService.getProperty(bundleContext, bundleContext.getBundle()
                    .getSymbolicName() + ".config.tpkDevUri");
            if (!StringUtils.isEmpty(propertyTpkDevUri)) {
                try {
                    tpkDevUri = new URI(propertyTpkDevUri);
                } catch (URISyntaxException e) {
                    LOG.error("Cannot decode the property " + bundleContext.getBundle().getSymbolicName()
                            + ".config.tpkDevUri as an URI: " + propertyTpkDevUri, e);
                }
            }
        }
        return tpkDevUri;
    }

    private File getDefaultResourcesFolder(BundleContext bundleContext, File rootact) {
        File defaultResourcesFolder = null;
        String propertyDefaultResourcesFolder = resourcesAccessorService.getProperty(bundleContext, bundleContext.getBundle()
                .getSymbolicName() + ".config.defaultResourcesFolder");
        if (!StringUtils.isEmpty(propertyDefaultResourcesFolder)) {
            defaultResourcesFolder = FileUtils.normalizeFile(rootact, propertyDefaultResourcesFolder);
            if (!defaultResourcesFolder.exists()) {
                LOG.error("Configured default resources folder does not exists '" + propertyDefaultResourcesFolder + "'");
                defaultResourcesFolder = null;
            } else if (!defaultResourcesFolder.isDirectory()) {
                LOG.error("Configured default resources folders is not a folder '" + propertyDefaultResourcesFolder + "'");
                defaultResourcesFolder = null;
            }
        }
        return defaultResourcesFolder;
    }

    private int getMaxWatchQueueSize(Dictionary config) {
        return ((Integer) checkWithDefault(config, ".maxWatchQueueSize", Integer.class, DEFAULT_MAX_WATCH_QUEUE_SIZE))
                .intValue();
    }

    private int getMaxInitWatches(Dictionary config) {
        return ((Integer) checkWithDefault(config, ".maxInitWatches", Integer.class, DEFAULT_MAX_INIT_WATCHES)).intValue();
    }

    private int getQueueSize(Dictionary config) {
        return ((Integer) checkWithDefault(config, ".queueSize", Integer.class, DEFAULT_QUEUE_SIZE)).intValue();
    }

    private int getMaxSubscriptions(Dictionary config) {
        return ((Integer) checkWithDefault(config, ".maxSubscriptions", Integer.class, DEFAULT_MAX_SUBSCRIPTIONS)).intValue();
    }

    private int getMaxSubscriptionsPerResource(Dictionary config) {
        return ((Integer) checkWithDefault(config, ".maxSubscriptionsPerResource", Integer.class,
                DEFAULT_MAX_SUBSCRIPTIONS_PER_RESOURCE)).intValue();
    }

    private long getDefaultExpirationDuration(Dictionary config) {
        String defaultStr = FormatUtils.formatDuration(DEFAULT_DEFAULT_EXPIRATION_DURATION.longValue());
        String duration = (String) checkWithDefault(config, ".defaultExpirationDuration", String.class, defaultStr);
        try {
            return FormatUtils.parseDuration(duration);
        } catch (ParseException e) {
            LOG.error("Configuration property .defaultExpirationDuration cannot be decoded as a duration, use default value "
                    + defaultStr, e);
            return DEFAULT_MIN_EXPIRATION_DURATION.longValue();
        }
    }

    private long getMinExpirationDuration(Dictionary config) {
        String defaultStr = FormatUtils.formatDuration(DEFAULT_MIN_EXPIRATION_DURATION.longValue());
        String duration = (String) checkWithDefault(config, ".minExpirationDuration", String.class, defaultStr);
        try {
            return FormatUtils.parseDuration(duration);
        } catch (ParseException e) {
            LOG.error("Configuration property .minExpirationDuration cannot be decoded as a duration, use default value "
                    + defaultStr, e);
            return DEFAULT_MIN_EXPIRATION_DURATION.longValue();
        }
    }

    private long getMaxExpirationDuration(Dictionary config) {
        String defaultStr = FormatUtils.formatDuration(DEFAULT_MAX_EXPIRATION_DURATION.longValue());
        String duration = (String) checkWithDefault(config, ".maxExpirationDuration", String.class, defaultStr);
        try {
            return FormatUtils.parseDuration(duration);
        } catch (ParseException e) {
            LOG.error("Configuration property .maxExpirationDuration cannot be decoded as a duration, use default value "
                    + defaultStr, e);
            return DEFAULT_MAX_EXPIRATION_DURATION.longValue();
        }
    }

    private long getDefaultMaxByteSize(Dictionary config) {
        return ((Long) checkWithDefault(config, ".defaultMaxByteSize", Long.class, DEFAULT_DEFAULT_MAX_BYTE_SIZE)).longValue();
    }

    private long getMinMaxByteSize(Dictionary config) {
        return ((Long) checkWithDefault(config, ".minMaxByteSize", Long.class, DEFAULT_MIN_MAX_BYTE_SIZE)).longValue();
    }

    private long getMaxMaxByteSize(Dictionary config) {
        return ((Long) checkWithDefault(config, ".maxMaxByteSize", Long.class, DEFAULT_MAX_MAX_BYTE_SIZE)).longValue();
    }

    private long getDefaultMaxInstanceAge(Dictionary config) {
        String defaultStr = FormatUtils.formatDuration(DEFAULT_DEFAULT_MAX_INSTANCE_AGE.longValue());
        String duration = (String) checkWithDefault(config, ".defaultMaxInstanceAge", String.class, defaultStr);
        try {
            return FormatUtils.parseDuration(duration);
        } catch (ParseException e) {
            LOG.error("Configuration property .defaultMaxInstanceAge cannot be decoded as a duration, use default value "
                    + defaultStr, e);
            return DEFAULT_DEFAULT_MAX_INSTANCE_AGE.longValue();
        }
    }

    private long getMinMaxInstanceAge(Dictionary config) {
        String defaultStr = FormatUtils.formatDuration(DEFAULT_MIN_MAX_INSTANCE_AGE.longValue());
        String duration = (String) checkWithDefault(config, ".minMaxInstanceAge", String.class, defaultStr);
        try {
            return FormatUtils.parseDuration(duration);
        } catch (ParseException e) {
            LOG.error("Configuration property .minMaxInstanceAge cannot be decoded as a duration, use default value "
                    + defaultStr, e);
            return DEFAULT_MIN_MAX_INSTANCE_AGE.longValue();
        }
    }

    private long getMaxMaxInstanceAge(Dictionary config) {
        String defaultStr = FormatUtils.formatDuration(DEFAULT_MAX_MAX_INSTANCE_AGE.longValue());
        String duration = (String) checkWithDefault(config, ".maxMaxInstanceAge", String.class, defaultStr);
        try {
            return FormatUtils.parseDuration(duration);
        } catch (ParseException e) {
            LOG.error("Configuration property .maxMaxInstanceAge cannot be decoded as a duration, use default value "
                    + defaultStr, e);
            return DEFAULT_MAX_MAX_INSTANCE_AGE.longValue();
        }
    }

    private long getDefaultMaxNrOfInstances(Dictionary config) {
        return ((Long) checkWithDefault(config, ".defaultMaxNrOfInstances", Long.class, DEFAULT_DEFAULT_MAX_NR_OF_INSTANCES))
                .longValue();
    }

    private long getMinMaxNrOfInstances(Dictionary config) {
        return ((Long) checkWithDefault(config, ".minMaxNrOfInstances", Long.class, DEFAULT_MIN_MAX_NR_OF_INSTANCES))
                .longValue();
    }

    private long getMaxMaxNrOfInstances(Dictionary config) {
        return ((Long) checkWithDefault(config, ".maxMaxNrOfInstances", Long.class, DEFAULT_MAX_MAX_NR_OF_INSTANCES))
                .longValue();
    }

    private long getNotifRequestTimeout(Dictionary config) {
        String defaultStr = FormatUtils.formatDuration(DEFAULT_NOTIF_REQUEST_TIMEOUT.longValue());
        String duration = (String) checkWithDefault(config, ".notifRequestTimeout", String.class, defaultStr);
        try {
            return FormatUtils.parseDuration(duration);
        } catch (ParseException e) {
            LOG.error("Configuration property .notifRequestTimeout cannot be decoded as a duration, use default value "
                    + defaultStr, e);
            return DEFAULT_NOTIF_REQUEST_TIMEOUT.longValue();
        }
    }

    private long getNotifRepresentationTimer(Dictionary config) {
        String defaultStr = FormatUtils.formatDuration(DEFAULT_NOTIF_REPRESENTATION_TIMER.longValue());
        String duration = (String) checkWithDefault(config, ".notifRepresentationTimer", String.class, defaultStr);
        try {
            return FormatUtils.parseDuration(duration);
        } catch (ParseException e) {
            LOG.error("Configuration property .notifRepresentationTimer cannot be decoded as a duration, use default value "
                    + defaultStr, e);
            return DEFAULT_NOTIF_REPRESENTATION_TIMER.longValue();
        }
    }

    private long getRegistrationExpirationDuration(Dictionary config) {
        String defaultStr = FormatUtils.formatDuration(DEFAULT_REGISTRATION_EXPIRATION_DURATION.longValue());
        String duration = (String) checkWithDefault(config, ".registrationExpirationDuration", String.class, defaultStr);
        try {
            return FormatUtils.parseDuration(duration);
        } catch (ParseException e) {
            LOG.error(
                    "Configuration property .registrationExpirationDuration cannot be decoded as a duration, use default value "
                            + defaultStr, e);
            return DEFAULT_REGISTRATION_EXPIRATION_DURATION.longValue();
        }
    }

    private long getRegistrationRetryDuration(Dictionary config) {
        String defaultStr = FormatUtils.formatDuration(DEFAULT_REGISTRATION_RETRY_DURATION.longValue());
        String duration = (String) checkWithDefault(config, ".registrationRetryDuration", String.class, defaultStr);
        try {
            return FormatUtils.parseDuration(duration);
        } catch (ParseException e) {
            LOG.error("Configuration property .registrationRetryDuration cannot be decoded as a duration, use default value "
                    + defaultStr, e);
            return DEFAULT_REGISTRATION_RETRY_DURATION.longValue();
        }
    }

    private boolean getContentInstanceCreationOnNotify(Dictionary config) {
        return ((Boolean) checkWithDefault(config, ".contentInstanceCreationOnNotify", Boolean.class,
                DEFAULT_CONTENT_INSTANCE_CREATION_ON_NOTIFY)).booleanValue();
    }

    private String getDiscoveryAccessRightID(Dictionary config) {
        return ((String) checkWithDefault(config, "discoveryAccessRightID", String.class, null));
    }

    private Map getReqEntityToIps(Dictionary config) {
        String[] reqEntityToIps = (String[]) checkWithDefault(config, "reqEntityToIps", String[].class, null);
        Map reqEntityToIpsMap = null;
        if (reqEntityToIps != null) {
            reqEntityToIpsMap = new HashMap();
            String reqEntityToIpsToken = null;
            String[] ips = null;
            int startParen = 0;
            int endParen = 0;
            for (int i = 0; i < reqEntityToIps.length; ++i) {
                ips = null;
                try {
                    reqEntityToIpsToken = reqEntityToIps[i];
                    startParen = reqEntityToIpsToken.indexOf('(');
                    if (startParen != -1) {
                        endParen = reqEntityToIpsToken.indexOf(')', startParen);
                        if (endParen != -1) {
                            ips = StringUtils.split(reqEntityToIpsToken.substring(startParen + 1, endParen), ';', false);
                            Arrays.sort(ips);
                            reqEntityToIpsMap.put(new URI(reqEntityToIpsToken.substring(0, startParen)), ips);
                        }
                    }
                    if (ips == null) {
                        LOG.error("Cannot add a mapping of a requesting entity to a list of IPs because entry is badly formatted: "
                                + reqEntityToIpsToken);
                    }
                } catch (URISyntaxException e) {
                    LOG.error("Cannot add a mapping of a requesting entity to a list of IPs because the URI is invalid: "
                            + reqEntityToIpsToken, e);
                } catch (RuntimeException e) {
                    LOG.error("Cannot add a mapping of a requesting entity to a list of IPs because the URI is invalid: "
                            + reqEntityToIpsToken, e);
                }
            }
        }
        return reqEntityToIpsMap;
    }

    private int[] getNotifBacktrackableErrorCodes(Dictionary config) {
        return (int[]) checkWithDefault(config, ".notifBacktrackableErrorCodes", int[].class,
                DEFAULT_NOTIF_BACKTRACKABLE_ERROR_CODES);
    }

    /**
     * Called to activate the current bundle.
     *
     * @param context The bundle context
     */
    public void start(BundleContext context) throws ParseException, StorageException, XoException, M2MException, IOException {
        this.context = context;
        BundleLogger.getStaticLogger().init(context);
        if (LOG.isInfoEnabled()) {
            LOG.info("Starting bundle " + context.getBundle().getSymbolicName() + "...");
            LOG.info("Starting tracker for " + XoService.class.getName() + " service...");
        }

        // init and start ServiceTrackers to tracks elements
        // for XoService
        xoServiceTracker = new ServiceTracker(context, XoService.class.getName(), new XoServiceCustomizer());
        xoServiceTracker.open();

        if (LOG.isInfoEnabled()) {
            LOG.info("Starting tracker for " + StorageRequestExecutor.class.getName() + " service...");
        }

        // for StorageRequestExecutor
        storageRequestExecutorTracker = new ServiceTracker(context, StorageRequestExecutor.class.getName(),
                new StorageContextCustomizer());
        storageRequestExecutorTracker.open();

        if (LOG.isInfoEnabled()) {
            LOG.info("Starting tracker for " + M2MService.class.getName() + " service...");
        }

        // for M2MService
        m2mServiceTracker = new ServiceTracker(context, M2MService.class.getName(), new M2MServiceCustomizer());
        m2mServiceTracker.open();

        if (LOG.isInfoEnabled()) {
            LOG.info("Starting tracker for " + ResourcesAccessorService.class.getName() + " service...");
        }

        // for ResourcesAccessorService
        resourcesAccessorServiceTracker = new ServiceTracker(context, ResourcesAccessorService.class.getName(),
                new ResourcesAccessorServiceTracker());
        resourcesAccessorServiceTracker.open();

        config = null;
        String pid = context.getBundle().getSymbolicName() + ".config";
        if (LOG.isInfoEnabled()) {
            LOG.info("Registering service " + ManagedService.class.getName() + " for pid " + pid + "...");
        }
        Dictionary props = new Hashtable();
        props.put(Constants.SERVICE_PID, pid);
        managedServiceRegistration = context.registerService(ManagedService.class.getName(), this, props);
    }

    /**
     * Called to deactivate the current bundle.
     */
    public void stop(BundleContext context) throws Exception {
        if (LOG.isInfoEnabled()) {
            LOG.info("Stopping bundle " + context.getBundle().getSymbolicName() + "...");
            LOG.info("Stopping tracker for " + XoService.class.getName() + " service...");
        }

        xoServiceTracker.close();
        xoServiceTracker = null;

        if (LOG.isInfoEnabled()) {
            LOG.info("Stopping tracker for " + M2MService.class.getName() + " service...");
        }

        m2mServiceTracker.close();
        m2mServiceTracker = null;

        if (LOG.isInfoEnabled()) {
            LOG.info("Stopping tracker for " + StorageRequestExecutor.class.getName() + " service...");
        }

        storageRequestExecutorTracker.close();
        storageRequestExecutorTracker = null;

        if (LOG.isInfoEnabled()) {
            LOG.info("Stopping tracker for " + ResourcesAccessorService.class.getName() + " service...");
        }

        resourcesAccessorServiceTracker.close();
        resourcesAccessorService = null;

        if (LOG.isInfoEnabled()) {
            String pid = context.getBundle().getSymbolicName() + ".config";
            LOG.info("Unregistering service " + ManagedService.class.getName() + " for pid " + pid + "...");
        }
        managedServiceRegistration.unregister();

        stopSclApp();

        config = null;
        this.context = null;
        BundleLogger.getStaticLogger().init(null);
    }

    private synchronized void startSclApp() {
        if ((xoService != null) && (storageContext != null) && (m2mService != null) && (resourcesAccessorService != null)
                && (config != null)) {
            M2MContext m2mContext = null;
            try {
                String contextPath = (String) config.get("contextPath");
                if (contextPath == null || contextPath.length() == 0) {
                    contextPath = resourcesAccessorService.getProperty(context, context.getBundle().getSymbolicName()
                            + ".config.contextPath");
                    if (contextPath == null || contextPath.length() == 0) {
                        contextPath = "/m2m";
                    }
                }

                LOG.info("Initializing SCL manager...");

                URI nsclUri = getNsclUri(context, config);
                URI tpkDevUri = getTpkDevUri(context, config);
                SclConfig sclConfig = new SclConfig(nsclUri != null, getMaxSubscriptions(config),
                        getMaxSubscriptionsPerResource(config), getDefaultExpirationDuration(config),
                        getMinExpirationDuration(config), getMaxExpirationDuration(config), getDefaultMaxByteSize(config),
                        getMinMaxByteSize(config), getMaxMaxByteSize(config), getDefaultMaxInstanceAge(config),
                        getMinMaxInstanceAge(config), getMaxMaxInstanceAge(config), getDefaultMaxNrOfInstances(config),
                        getMinMaxNrOfInstances(config), getMaxMaxNrOfInstances(config), getMaxWatchQueueSize(config),
                        getNotifRequestTimeout(config), getNotifRepresentationTimer(config), getMaxInitWatches(config),
                        getRegistrationExpirationDuration(config), getRegistrationRetryDuration(config),
                        getContentInstanceCreationOnNotify(config), getDiscoveryAccessRightID(config),
                        getReqEntityToIps(config));

                LOG.info("Creating SCL manager...");
                sclManager = new SclManagerImpl(sclConfig, xoService, storageContext);

                LOG.info("Registering SCL to M2M Service...");
                Map backendConfig = new HashMap();
                backendConfig.put("threadStrategy", "distinct");
                backendConfig.put("queueSize", new Integer(getQueueSize(config)));
                m2mContext = m2mService.registerScl("SCL", contextPath, sclManager, sclManager, backendConfig);
                m2mContext.setBacktrackableErrorCodes(getNotifBacktrackableErrorCodes(config));
                sclManager.initRegistration(nsclUri, tpkDevUri);

                LOG.info("Starting SCL manager...");
                File rootact = new File(resourcesAccessorService.getProperty(context, UtilConstants.FW_PROP_ROOTACT));
                sclManager.start(getDefaultResourcesFolder(context, rootact));
                LOG.info("SCL is ready");
                System.out.println("SCL is ready");
            } catch (ParseException e) {
                LOG.error("ParseException while starting SCL APP", e);
                // throw e;
                e.printStackTrace();
            } catch (IOException e) {
                LOG.error("IOException while starting SCL APP", e);
                // throw e;
                e.printStackTrace();
            } catch (StorageException e) {
                LOG.error("StorageException while starting SCL APP", e);
                // throw e;
                e.printStackTrace();
            } catch (XoException e) {
                LOG.error("XoException while starting SCL APP", e);
                // throw e;
                e.printStackTrace();
            } catch (M2MException e) {
                LOG.error("M2MException while starting SCL APP", e);
                // throw e;
                e.printStackTrace();
            } catch (RuntimeException e) {
                LOG.error("Problem while starting the SCL bundle", e);
                if (m2mContext != null) {
                    m2mService.unregisterContext(m2mContext);
                }
                throw e;
            }
        }
    }

    /**
     * Reset the scl.app bundle memory space, ready to re-start if ever needed.
     */
    private synchronized void stopSclApp() {
        if (sclManager != null) {
            LOG.info("Stopping SCL manager...");
            sclManager.stop();
            try {
                m2mService.unregisterContext(sclManager.getM2MContext());
            } catch (RuntimeException e) {
                LOG.error("Problem while unregistering SCL service", e);
            }
            sclManager = null;
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
        if (sclManager != null) {
            SclConfig sclConfig = sclManager.getSclConfig();
            sclConfig.setMaxSubscriptions(getMaxSubscriptions(config));
            sclConfig.setMaxSubscriptionsPerResource(getMaxSubscriptionsPerResource(config));
            sclConfig.setDefaultExpirationDuration(getDefaultExpirationDuration(config));
            sclConfig.setMinExpirationDuration(getMinExpirationDuration(config));
            sclConfig.setMaxExpirationDuration(getMaxExpirationDuration(config));
            sclConfig.setDefaultMaxByteSize(getDefaultMaxByteSize(config));
            sclConfig.setMinMaxByteSize(getMinMaxByteSize(config));
            sclConfig.setMaxMaxByteSize(getMaxMaxByteSize(config));
            sclConfig.setDefaultMaxInstanceAge(getDefaultMaxInstanceAge(config));
            sclConfig.setMinMaxInstanceAge(getMinMaxInstanceAge(config));
            sclConfig.setMaxMaxInstanceAge(getMaxMaxInstanceAge(config));
            sclConfig.setDefaultMaxNrOfInstances(getDefaultMaxNrOfInstances(config));
            sclConfig.setMinMaxNrOfInstances(getMinMaxNrOfInstances(config));
            sclConfig.setMaxMaxNrOfInstances(getMaxMaxNrOfInstances(config));
            sclConfig.setMaxWatchQueueSize(getMaxWatchQueueSize(config));
            sclConfig.setNotifRequestTimeout(getNotifRequestTimeout(config));
            sclConfig.setNotifRepresentationTimer(getNotifRepresentationTimer(config));
            sclConfig.setMaxInitWatches(getMaxInitWatches(config));
            sclConfig.setRegistrationExpirationDuration(getRegistrationExpirationDuration(config));
            sclConfig.setRegistrationRetryDuration(getRegistrationRetryDuration(config));
            sclConfig.setContentInstanceCreationOnNotify(getContentInstanceCreationOnNotify(config));
            sclConfig.setDiscoveryAccessRightID(getDiscoveryAccessRightID(config));
            sclConfig.setReqEntityToIps(getReqEntityToIps(config));
            sclManager.getM2MContext().setBacktrackableErrorCodes(getNotifBacktrackableErrorCodes(config));
        } else {
            startSclApp();
        }
    }

    // customizer that handles registration/modification/unregistration events for XoService
    private class XoServiceCustomizer implements ServiceTrackerCustomizer {

        public Object addingService(ServiceReference reference) {
            if (LOG.isInfoEnabled()) {
                LOG.info("Adding service " + XoService.class.getName() + "...");
            }
            if (xoService != null) {
                return null;
            }
            xoService = (XoService) context.getService(reference);

            startSclApp();
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
                stopSclApp();
                xoService = null;
            }
        }
    }

    // customizer that handles registration/modification/unregistration events for M2MService
    private class M2MServiceCustomizer implements ServiceTrackerCustomizer {

        public Object addingService(ServiceReference reference) {
            if (LOG.isInfoEnabled()) {
                LOG.info("Adding service " + M2MService.class.getName() + "...");
            }
            if (m2mService != null) {
                return null;
            }
            m2mService = (M2MService) context.getService(reference);

            startSclApp();
            // Return the service to track it
            return m2mService;
        }

        public void modifiedService(ServiceReference reference, Object service) {
        }

        public void removedService(ServiceReference reference, Object service) {
            if (LOG.isInfoEnabled()) {
                LOG.info("Removing service " + M2MService.class.getName() + "...");
            }
            if (service == m2mService) {
                stopSclApp();
                m2mService = null;
            }
        }
    }

    // customizer that handles registration/modification/unregistration events for ConfigurationAdmin
    private class StorageContextCustomizer implements ServiceTrackerCustomizer {

        public Object addingService(ServiceReference reference) {
            if (LOG.isInfoEnabled()) {
                LOG.info("Adding service " + StorageRequestExecutor.class.getName() + "...");
            }
            if (storageContext != null) {
                return null;
            }
            storageContext = (StorageRequestExecutor) context.getService(reference);

            startSclApp();
            // Return the service to track it
            return storageContext;
        }

        public void modifiedService(ServiceReference reference, Object service) {
        }

        public void removedService(ServiceReference reference, Object service) {
            if (LOG.isInfoEnabled()) {
                LOG.info("Removing service " + StorageRequestExecutor.class.getName() + "...");
            }
            if (service == storageContext) {
                stopSclApp();
                storageContext = null;
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
            startSclApp();
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
                stopSclApp();
                resourcesAccessorService = null;
            }
        }
    }
}
