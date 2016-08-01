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

package com.actility.m2m.cm.osgi;

import java.io.File;
import java.io.IOException;
import java.util.Dictionary;

import org.apache.log4j.Level;
import org.apache.log4j.Logger;
import org.osgi.framework.Bundle;
import org.osgi.framework.BundleActivator;
import org.osgi.framework.BundleContext;
import org.osgi.framework.BundleEvent;
import org.osgi.framework.BundleListener;
import org.osgi.framework.InvalidSyntaxException;
import org.osgi.framework.ServiceEvent;
import org.osgi.framework.ServiceFactory;
import org.osgi.framework.ServiceListener;
import org.osgi.framework.ServiceReference;
import org.osgi.framework.ServiceRegistration;
import org.osgi.service.cm.ConfigurationAdmin;
import org.osgi.service.cm.ConfigurationPlugin;
import org.osgi.service.cm.ManagedService;
import org.osgi.service.cm.ManagedServiceFactory;
import org.osgi.util.tracker.ServiceTracker;
import org.osgi.util.tracker.ServiceTrackerCustomizer;

import com.actility.m2m.cm.ConfigurationAdminImpl;
import com.actility.m2m.cm.ConfigurationAdminManager;
import com.actility.m2m.cm.log.BundleLogger;
import com.actility.m2m.framework.resources.ResourcesAccessorService;
import com.actility.m2m.util.FileUtils;
import com.actility.m2m.util.StringUtils;
import com.actility.m2m.util.UtilConstants;
import com.actility.m2m.util.log.OSGiLogger;

/**
 * CM bundle activator implementation
 */

public final class Activator implements BundleActivator, ServiceFactory, ServiceListener, BundleListener {
    private static final Logger LOG = OSGiLogger.getLogger(Activator.class, BundleLogger.getStaticLogger());

    private static final String ETC_CONFIG_DIR = ".config.etcConfigDir";
    private static final String USR_CONFIG_DIR = ".config.usrConfigDir";

    private static final File DEFAULT_ETC_CONFIG_DIR = new File("etc/cm");
    private static final File DEFAULT_USR_CONFIG_DIR = new File("usr/cm");

    private BundleContext context;
    private ServiceTracker resourcesAccessorServiceTracker;
    private ResourcesAccessorService resourcesAccessorService;
    private ServiceRegistration serviceRegistration;

    private ConfigurationAdminManager manager;

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

    private File getEtcStoreDir(BundleContext context, File rootact) {
        String storeEtcName = resourcesAccessorService.getProperty(context, context.getBundle().getSymbolicName()
                + ETC_CONFIG_DIR);
        File storeEtcDir = null;
        if (StringUtils.isEmpty(storeEtcName)) {
            storeEtcDir = DEFAULT_ETC_CONFIG_DIR;
        } else {
            storeEtcDir = new File(storeEtcName);
        }
        storeEtcDir = FileUtils.normalizeFile(rootact, storeEtcDir.getPath());
        return storeEtcDir;
    }

    private File getUsrStoreDir(BundleContext context, File rootact) {
        String storeUsrName = resourcesAccessorService.getProperty(context, context.getBundle().getSymbolicName()
                + USR_CONFIG_DIR);
        File storeUsrDir = null;
        if (StringUtils.isEmpty(storeUsrName)) {
            storeUsrDir = DEFAULT_USR_CONFIG_DIR;
        } else {
            storeUsrDir = new File(storeUsrName);
        }
        storeUsrDir = FileUtils.normalizeFile(rootact, storeUsrDir.getPath());
        return storeUsrDir;
    }

    private void serviceChanged(ServiceReference sr, int eventType, String objectClass) {
        if (ManagedServiceFactory.class.getName().equals(objectClass)) {
            managedServiceFactoryChanged(sr, eventType);
        } else if (ManagedService.class.getName().equals(objectClass)) {
            managedServiceChanged(sr, eventType);
        } else if (ConfigurationPlugin.class.getName().equals(objectClass)) {
            configurationPluginChanged(sr, eventType);
        }
    }

    private void managedServiceFactoryChanged(ServiceReference sr, int eventType) {
        switch (eventType) {
        case ServiceEvent.REGISTERED:
            manager.registeredManagedServiceFactory(sr);
            break;
        case ServiceEvent.MODIFIED:
            break;
        case ServiceEvent.UNREGISTERING:
            manager.unregisteredManagedServiceFactory(sr);
            break;
        }
    }

    private void managedServiceChanged(ServiceReference sr, int eventType) {
        switch (eventType) {
        case ServiceEvent.REGISTERED:
            manager.registeredManagedService(sr);
            break;
        case ServiceEvent.MODIFIED:
            break;
        case ServiceEvent.UNREGISTERING:
            manager.unregisteredManagedService(sr);
            break;
        }
    }

    private void configurationPluginChanged(ServiceReference sr, int eventType) {
        switch (eventType) {
        case ServiceEvent.REGISTERED:
            manager.registeredConfigurationPlugin(sr);
            break;
        case ServiceEvent.MODIFIED:
            manager.modifiedConfigurationPlugin(sr);
            break;
        case ServiceEvent.UNREGISTERING:
            manager.unregisteredConfigurationPlugin(sr);
            break;
        }
    }

    private void addRegisteredServices(BundleContext context, Class c) {
        ServiceReference[] srs = null;
        try {
            srs = context.getServiceReferences(c.getName(), null);
        } catch (InvalidSyntaxException ignored) {
        }
        if (srs == null) {
            return;
        }
        for (int i = 0; i < srs.length; ++i) {
            serviceChanged(srs[i], ServiceEvent.REGISTERED, c.getName());
        }
    }

    private void addInstalledBundles(BundleContext context) {
        Bundle[] bs = context.getBundles();
        for (int i = 0; bs != null && i < bs.length; ++i) {
            manager.addedBundle(bs[i].getLocation());
        }
    }

    public void start(BundleContext context) throws IOException {
        this.context = context;
        BundleLogger.getStaticLogger().init(context);
        if (LOG.isInfoEnabled()) {
            LOG.info("Starting bundle " + context.getBundle().getSymbolicName() + "...");
            LOG.info("Starting tracker for " + ResourcesAccessorService.class.getName() + " service...");
        }

        // init and start ServiceTracker to tracks elements
        resourcesAccessorServiceTracker = new ServiceTracker(context, ResourcesAccessorService.class.getName(),
                new ResourcesAccessorServiceCustomizer());
        resourcesAccessorServiceTracker.open();
    }

    public void stop(BundleContext context) {
        if (LOG.isInfoEnabled()) {
            LOG.info("Stopping bundle " + context.getBundle().getSymbolicName() + "...");
            LOG.info("Stopping tracker for " + ResourcesAccessorService.class.getName() + " service...");
        }
        resourcesAccessorServiceTracker.close();

        try {
            stopConfigAdminService();
        } finally {
            resourcesAccessorServiceTracker = null;
            resourcesAccessorService = null;
            serviceRegistration = null;
            manager = null;
            this.context = null;

            BundleLogger.getStaticLogger().init(null);
        }
    }

    public void bundleChanged(BundleEvent event) {
        if (event.getType() == BundleEvent.UNINSTALLED) {
            manager.removedBundle(event.getBundle().getLocation());
        } else if (event.getType() == BundleEvent.INSTALLED) {
            manager.addedBundle(event.getBundle().getLocation());
        }
    }

    public void serviceChanged(ServiceEvent event) {
        ServiceReference sr = event.getServiceReference();
        int eventType = event.getType();
        String[] objectClasses = (String[]) sr.getProperty("objectClass");
        for (int i = 0; i < objectClasses.length; ++i) {
            serviceChanged(sr, eventType, objectClasses[i]);
        }
    }

    public Object getService(Bundle bundle, ServiceRegistration registration) {
        // For now we don't keep track of the services returned internally
        return new ConfigurationAdminImpl(manager, bundle.getLocation());
    }

    public void ungetService(Bundle bundle, ServiceRegistration registration, Object service) {
        // For now we do nothing here
    }

    private synchronized void startConfigAdminService() {
        if (resourcesAccessorService != null) {
            try {
                File rootact = new File(resourcesAccessorService.getProperty(context, UtilConstants.FW_PROP_ROOTACT));
                manager = new ConfigurationAdminManager(context, getEtcStoreDir(context, rootact), getUsrStoreDir(context,
                        rootact));

                addInstalledBundles(context);

                String filter = "(|(objectClass=" + ManagedServiceFactory.class.getName() + ")" + "(objectClass="
                        + ManagedService.class.getName() + ")" + "(objectClass=" + ConfigurationPlugin.class.getName() + "))";
                try {
                    context.addServiceListener(this, filter);
                    context.addBundleListener(this);
                } catch (InvalidSyntaxException e) {
                    LOG.error("Invalid syntax exception", e);
                }

                addRegisteredServices(context, ConfigurationPlugin.class);
                addRegisteredServices(context, ManagedServiceFactory.class);
                addRegisteredServices(context, ManagedService.class);

                if (LOG.isInfoEnabled()) {
                    LOG.info("Registering service " + ConfigurationAdmin.class.getName() + "...");
                }
                serviceRegistration = context.registerService(ConfigurationAdmin.class.getName(), this, null);
                manager.setCmRef(serviceRegistration.getReference());
            } catch (IOException e) {
                LOG.error("Cannot start " + ConfigurationAdmin.class.getName(), e);
            }
        }
    }

    private synchronized void stopConfigAdminService() {
        if (manager != null) {
            if (serviceRegistration != null) {
                if (LOG.isInfoEnabled()) {
                    LOG.info("Unregistering service " + ConfigurationAdmin.class.getName() + "...");
                }
                serviceRegistration.unregister();
            }
            context.removeServiceListener(this);
            context.removeBundleListener(this);
            manager.stop();
            serviceRegistration = null;
            manager = null;
        }
    }

    // customizer that handles tracked service
    // registration/modification/unregistration events
    private class ResourcesAccessorServiceCustomizer implements ServiceTrackerCustomizer {

        public Object addingService(ServiceReference reference) {
            if (resourcesAccessorService != null) {
                return null;
            }
            resourcesAccessorService = (ResourcesAccessorService) context.getService(reference);

            // Do something with the service
            startConfigAdminService();
            // Return the service to track it
            return resourcesAccessorService;
        }

        public void modifiedService(ServiceReference reference, Object service) {
        }

        public void removedService(ServiceReference reference, Object service) {
            if (service == resourcesAccessorService) {
                stopConfigAdminService();
                resourcesAccessorService = null;
            }
        }
    }
}
