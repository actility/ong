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

package com.actility.m2m.song.binding.coap.jni.osgi;

import java.io.File;
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
import com.actility.m2m.song.binding.coap.jni.impl.JniSongBindingCoapServiceImpl;
import com.actility.m2m.song.binding.coap.jni.log.BundleLogger;
import com.actility.m2m.song.binding.coap.ni.api.NiSongBindingCoapService;
import com.actility.m2m.util.FileUtils;
import com.actility.m2m.util.UtilConstants;
import com.actility.m2m.util.log.OSGiLogger;

public class Activator implements BundleActivator, ManagedService {
    private static final Logger LOG = OSGiLogger.getLogger(Activator.class, BundleLogger.LOG);

    private static final String DEFAULT_DIA_LOG_FILE = "../../../var/log/gscl/DiaJni.log";
    private static final Integer DEFAULT_DIA_TRACE_LEVEL = new Integer(-1);
    private static final Integer DEFAULT_DIA_MAX_LOG_FILE_SIZE = new Integer(0);
    private static final String DEFAULT_SERVER_ADDRESS = "::";
    private static final Integer DEFAULT_SERVER_PORT = new Integer(5683);

    private ServiceTracker resourcesAccessorServiceTracker;
    private ResourcesAccessorService resourcesAccessorService;

    private BundleContext context;
    private JniSongBindingCoapServiceImpl jniSongBindingCoap;
    private ServiceRegistration jniSongBindingCoapServiceRegistration;
    private Dictionary config;
    private ServiceRegistration managedServiceRegistration;

    public void start(BundleContext context) throws Exception {
        this.context = context;
        BundleLogger.LOG.init(context);

        if (LOG.isInfoEnabled()) {
            LOG.info("Starting bundle " + context.getBundle().getSymbolicName() + "...");
            LOG.info("Starting tracker for " + ResourcesAccessorService.class.getName() + " service...");
        }
        // init and start ServiceTrackers to tracks elements
        // for ResourcesAccessorService
        resourcesAccessorServiceTracker = new ServiceTracker(context, ResourcesAccessorService.class.getName(),
                new ResourcesAccessorServiceCustomizer());
        resourcesAccessorServiceTracker.open();

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
            LOG.info("Stopping bundle " + context.getBundle().getSymbolicName() + "...");
            LOG.info("Stopping tracker for " + ResourcesAccessorService.class.getName() + " service...");
        }
        resourcesAccessorServiceTracker.close();
        resourcesAccessorServiceTracker = null;

        if (LOG.isInfoEnabled()) {
            String pid = context.getBundle().getSymbolicName() + ".config";
            LOG.info("Unregistering service " + ManagedService.class.getName() + " for pid " + pid + "...");
        }
        managedServiceRegistration.unregister();
        managedServiceRegistration = null;

        stopJniSongBindingCoap();
        this.context = null;
        resourcesAccessorService = null;
        config = null;
        BundleLogger.LOG.init(null);
    }

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

    private String getDiaLogFile(Dictionary config, File rootact) {
        return FileUtils.normalizeFile(rootact,
                (String) checkWithDefault(config, "dIaLogFile", String.class, DEFAULT_DIA_LOG_FILE)).getAbsolutePath();
    }

    private int getDiaTraceLevel(Dictionary config) {
        return ((Integer) checkWithDefault(config, "dIaTraceLevel", Integer.class, DEFAULT_DIA_TRACE_LEVEL)).intValue();
    }

    private int getDiaMaxLogFileSize(Dictionary config) {
        return ((Integer) checkWithDefault(config, "dIaMaxLogFileSize", Integer.class, DEFAULT_DIA_MAX_LOG_FILE_SIZE))
                .intValue();
    }

    private String getServerAddress(Dictionary config) {
        return (String) checkWithDefault(config, "serverAddress", String.class, DEFAULT_SERVER_ADDRESS);
    }

    private int getServerPort(Dictionary config) {
        return ((Integer) checkWithDefault(config, "serverPort", Integer.class, DEFAULT_SERVER_PORT)).intValue();
    }

    private synchronized void startJniSongBindingCoap() {
        if (config != null && resourcesAccessorService != null) {
            boolean error = false;
            try {
                File rootact = new File(resourcesAccessorService.getProperty(context, UtilConstants.FW_PROP_ROOTACT));
                String serverAddress = getServerAddress(config);
                int serverPort = getServerPort(config);
                String diaLogFile = getDiaLogFile(config, rootact);
                int diaTraceLevel = getDiaTraceLevel(config);
                int diaMaxLogFileSize = getDiaMaxLogFileSize(config);

                jniSongBindingCoap = new JniSongBindingCoapServiceImpl(serverAddress, String.valueOf(serverPort),
                        diaTraceLevel, diaLogFile, diaMaxLogFileSize);

                jniSongBindingCoap.diaStart();

                if (LOG.isInfoEnabled()) {
                    LOG.info("Registering service " + NiSongBindingCoapService.class.getName() + "...");
                }
                Dictionary properties = new Hashtable();
                properties.put("serverAddress", serverAddress);
                properties.put("serverPort", new Integer(serverPort));
                jniSongBindingCoapServiceRegistration = context.registerService(NiSongBindingCoapService.class.getName(),
                        jniSongBindingCoap, properties);
            } catch (Throwable t) {
                LOG.error("Unexpected exception", t);
                error = true;
            } finally {
                if (error) {
                    stopJniSongBindingCoap();
                }
            }
        }
    }

    private synchronized void stopJniSongBindingCoap() {
        if (jniSongBindingCoap != null) {
            if (LOG.isInfoEnabled()) {
                LOG.info("Unregistering service " + NiSongBindingCoapService.class.getName() + "...");
            }
            jniSongBindingCoapServiceRegistration.unregister();
            jniSongBindingCoap.diaStop();
            jniSongBindingCoap = null;
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
        stopJniSongBindingCoap();
        startJniSongBindingCoap();
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

            startJniSongBindingCoap();
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
                stopJniSongBindingCoap();
                resourcesAccessorService = null;
            }
        }
    }
}
