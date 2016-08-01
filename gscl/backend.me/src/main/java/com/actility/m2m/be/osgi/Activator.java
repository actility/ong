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

package com.actility.m2m.be.osgi;

import java.util.Dictionary;
import java.util.Enumeration;
import java.util.HashMap;
import java.util.Hashtable;
import java.util.Map;

import org.apache.log4j.Logger;
import org.osgi.framework.BundleActivator;
import org.osgi.framework.BundleContext;
import org.osgi.framework.Constants;
import org.osgi.framework.ServiceRegistration;
import org.osgi.service.cm.ConfigurationException;
import org.osgi.service.cm.ManagedService;

import com.actility.m2m.be.BackendService;
import com.actility.m2m.be.impl.BackendServiceImpl;
import com.actility.m2m.be.log.BundleLogger;
import com.actility.m2m.util.log.OSGiLogger;

/**
 * The current node manager
 */
public final class Activator implements ManagedService, BundleActivator {
    private static final Logger LOG = OSGiLogger.getLogger(Activator.class, BundleLogger.getStaticLogger());

    private BundleContext context;
    private ServiceRegistration managedServiceRegistration;
    private BackendServiceImpl backendService;
    private ServiceRegistration backendServiceRegistration;
    private final Map defaultConfiguration;

    public Activator() {
        defaultConfiguration = new HashMap();
        defaultConfiguration.put("threadStrategy", "pool");
        defaultConfiguration.put("queueSize", new Integer(32));
        defaultConfiguration.put("minPoolThreads", new Integer(4));
        defaultConfiguration.put("maxPoolThreads", new Integer(4));
        defaultConfiguration.put("poolThreadExpiration", "PT10S");
    }

    public void start(BundleContext context) throws Exception {
        this.context = context;
        BundleLogger.getStaticLogger().init(context);
        String pid = context.getBundle().getSymbolicName() + ".config";
        if (LOG.isInfoEnabled()) {
            LOG.info("Starting bundle " + context.getBundle().getSymbolicName() + "...");
            LOG.info("Registering service " + ManagedService.class.getName() + " for pid " + pid + "...");
        }

        Dictionary dictionary = new Hashtable();
        dictionary.put(Constants.SERVICE_PID, pid);
        managedServiceRegistration = context.registerService(ManagedService.class.getName(), this, dictionary);
    }

    public void stop(BundleContext context) throws Exception {
        if (LOG.isInfoEnabled()) {
            String pid = context.getBundle().getSymbolicName() + ".config";
            LOG.info("Stoppping bundle " + context.getBundle().getSymbolicName() + "...");
            LOG.info("Unregistering service " + ManagedService.class.getName() + " for pid " + pid + "...");
        }
        managedServiceRegistration.unregister();
        stopBackendService();
        this.context = null;
        BundleLogger.getStaticLogger().init(null);
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
        stopBackendService();
        Map config = defaultConfiguration;
        if (properties != null) {
            config = new HashMap(config);
        }
        startBackendService(config);
    }

    private void startBackendService(Map config) {
        if (LOG.isInfoEnabled()) {
            LOG.info("Registering service " + BackendService.class.getName() + "...");
        }
        backendService = new BackendServiceImpl(config);
        backendServiceRegistration = context.registerService(BackendService.class.getName(), backendService, null);
    }

    private void stopBackendService() {
        if (backendService != null) {
            if (LOG.isInfoEnabled()) {
                LOG.info("Unregistering service " + BackendService.class.getName() + "...");
            }
            backendService.stop();
            backendServiceRegistration.unregister();
            backendService = null;
        }
    }

}
