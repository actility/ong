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

package com.actility.m2m.servlet.osgi;

import java.net.UnknownHostException;
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

import com.actility.m2m.be.BackendException;
import com.actility.m2m.be.BackendService;
import com.actility.m2m.framework.resources.ResourcesAccessorService;
import com.actility.m2m.servlet.log.BundleLogger;
import com.actility.m2m.servlet.service.ServletService;
import com.actility.m2m.servlet.service.ext.ExtServletService;
import com.actility.m2m.servlet.service.impl.ServletServiceImpl;
import com.actility.m2m.servlet.song.binding.service.SongBindingService;
import com.actility.m2m.servlet.song.binding.service.ext.ExtSongBindingService;
import com.actility.m2m.servlet.song.service.SongService;
import com.actility.m2m.servlet.song.service.ext.ExtSongService;
import com.actility.m2m.servlet.song.service.impl.SongServiceImpl;
import com.actility.m2m.transport.logger.TransportLoggerService;
import com.actility.m2m.util.StringUtils;
import com.actility.m2m.util.log.OSGiLogger;
import com.actility.m2m.xo.XoService;

public final class SongActivator implements BundleActivator, ManagedService {
    public static final Logger LOG = OSGiLogger.getLogger(SongActivator.class, BundleLogger.getStaticLogger());

    private static final String[] DEFAULT_ROUTES_CONFIGURATION = { "0.0.0.0/0:false" };
    private static final Long DEFAULT_MAX_REMOTE_REQUESTS = new Long(100L);
    private static final String DEFAULT_SERVER_INFO = "";

    private BundleContext context;

    // object for ServletService, ExtServletService
    private ServiceRegistration servletServiceRegistration;
    private ServletServiceImpl servletServiceImpl;

    // object for ExtSongService, ExtSongBindingService, SongService, SongBindingService
    private SongServiceImpl songService;
    private ServiceRegistration songServiceRegistration;

    private ServiceRegistration managedServiceRegistration;

    // ServiceTracker for BackendService
    private ServiceTracker backendServiceTracker;
    private BackendService backendService;

    // ServiceTracker for ConfigAdmin
    private ServiceTracker resourcesAccessorServiceTracker;
    private ResourcesAccessorService resourcesAccessorService;

    // ServiceTracker for XoService
    private ServiceTracker xoServiceTracker;
    private XoService xoService;

    // ServiceTracker for TransportLoggerService
    private ServiceTracker transportLoggerServiceTracker;
    private TransportLoggerService transportLoggerService;

    private Dictionary config;

    public void start(BundleContext context) throws Exception {
        this.context = context;
        BundleLogger.getStaticLogger().init(context);
        if (LOG.isInfoEnabled()) {
            LOG.info("Starting bundle " + context.getBundle().getSymbolicName() + "...");
            LOG.info("Starting tracker for " + BackendService.class.getName() + " service...");
        }

        backendServiceTracker = new ServiceTracker(context, BackendService.class.getName(), new BackendServiceCustomizer());
        backendServiceTracker.open();

        if (LOG.isInfoEnabled()) {
            LOG.info("Starting tracker for " + ResourcesAccessorService.class.getName() + " service...");
        }

        resourcesAccessorServiceTracker = new ServiceTracker(context, ResourcesAccessorService.class.getName(),
                new ResourcesAccessorServiceCustomizer());
        resourcesAccessorServiceTracker.open();

        if (LOG.isInfoEnabled()) {
            LOG.info("Starting tracker for " + XoService.class.getName() + " service...");
        }

        xoServiceTracker = new ServiceTracker(context, XoService.class.getName(), new XoServiceCustomizer());
        xoServiceTracker.open();

        if (LOG.isInfoEnabled()) {
            LOG.info("Starting tracker for " + TransportLoggerService.class.getName() + " service...");
        }

        transportLoggerServiceTracker = new ServiceTracker(context, TransportLoggerService.class.getName(),
                new TransportLoggerServiceCustomizer());
        transportLoggerServiceTracker.open();

        String pid = context.getBundle().getSymbolicName() + ".config";
        if (LOG.isInfoEnabled()) {
            LOG.info("Registering service " + ManagedService.class.getName() + " for pid " + pid + "...");
        }

        // register the ManagedService for handling CM events
        config = null;
        Dictionary props = new Hashtable();
        props.put(Constants.SERVICE_PID, pid);
        managedServiceRegistration = context.registerService(ManagedService.class.getName(), this, props);
    }

    public void stop(BundleContext context) throws Exception {
        if (LOG.isInfoEnabled()) {
            LOG.info("Stopping bundle " + context.getBundle().getSymbolicName() + "...");
            LOG.info("Stopping tracker for " + BackendService.class.getName() + " service...");
        }
        // stop ServiceTracker to track BackendService
        backendServiceTracker.close();

        if (LOG.isInfoEnabled()) {
            LOG.info("Stopping tracker for " + ResourcesAccessorService.class.getName() + " service...");
        }

        resourcesAccessorServiceTracker.close();

        if (LOG.isInfoEnabled()) {
            LOG.info("Stopping tracker for " + XoService.class.getName() + " service...");
        }

        xoServiceTracker.close();

        if (LOG.isInfoEnabled()) {
            LOG.info("Stopping tracker for " + TransportLoggerService.class.getName() + " service...");
        }

        transportLoggerServiceTracker.close();

        if (LOG.isInfoEnabled()) {
            String pid = context.getBundle().getSymbolicName() + ".config";
            LOG.info("Unregistering service " + ManagedService.class.getName() + " for pid " + pid + "...");
        }
        managedServiceRegistration.unregister();

        stopSong();

        servletServiceImpl = null;
        backendService = null;
        backendServiceTracker = null;
        resourcesAccessorServiceTracker = null;
        resourcesAccessorService = null;
        xoServiceTracker = null;
        xoService = null;
        transportLoggerServiceTracker = null;
        transportLoggerService = null;
        managedServiceRegistration = null;
        config = null;
        this.context = null;
        BundleLogger.getStaticLogger().init(null);
    }

    private String getServerInfo(BundleContext bundleContext, Dictionary config) {
        String serverInfo = (String) checkWithDefault(config, "serverInfo", String.class, DEFAULT_SERVER_INFO);
        if (StringUtils.isEmpty(serverInfo)) {
            String propertyServerInfo = resourcesAccessorService.getProperty(bundleContext, bundleContext.getBundle()
                    .getSymbolicName() + ".config.serverInfo");
            if (!StringUtils.isEmpty(propertyServerInfo)) {
                serverInfo = propertyServerInfo;
            } else {
                serverInfo = "ONG";
            }
        }
        return serverInfo;
    }

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

    private String[] getRoutesConfiguration(Dictionary config) {
        return (String[]) checkWithDefault(config, "routesConfiguration", String[].class, DEFAULT_ROUTES_CONFIGURATION);
    }

    private long getMaxRemoteRequests(Dictionary config) {
        return ((Long) checkWithDefault(config, "maxRemoteRequests", Long.class, DEFAULT_MAX_REMOTE_REQUESTS)).longValue();
    }

    private synchronized void startSong() {
        if ((resourcesAccessorService != null) && (backendService != null) && (transportLoggerService != null)
                && (xoService != null) && (config != null)) {
            String hostName = resourcesAccessorService.getProperty(context, context.getBundle().getSymbolicName()
                    + ".config.hostName");
            String domainName = resourcesAccessorService.getProperty(context, context.getBundle().getSymbolicName()
                    + ".config.domainName");

            servletServiceImpl = new ServletServiceImpl(resourcesAccessorService, getServerInfo(context, config));

            if (LOG.isInfoEnabled()) {
                LOG.info("Registering service [" + ExtServletService.class.getName() + ", " + ServletService.class.getName()
                        + "]...");
            }
            servletServiceRegistration = context.registerService(new String[] { ExtServletService.class.getName(),
                    ServletService.class.getName() }, servletServiceImpl, null);

            try {
                songService = new SongServiceImpl(getRoutesConfiguration(config), servletServiceImpl, backendService,
                        xoService, resourcesAccessorService, transportLoggerService, hostName, domainName,
                        getMaxRemoteRequests(config));

                if (LOG.isInfoEnabled()) {
                    LOG.info("Registering service [" + SongService.class.getName() + ", " + ExtSongService.class.getName()
                            + ", " + SongBindingService.class.getName() + ", " + ExtSongBindingService.class.getName() + "]...");
                }
                songServiceRegistration = context.registerService(
                        new String[] { SongService.class.getName(), ExtSongService.class.getName(),
                                SongBindingService.class.getName(), ExtSongBindingService.class.getName() }, songService, null);

            } catch (UnknownHostException e) {
                LOG.error("Unable to resolve route config (routes:" + getRoutesConfiguration(config) + ")", e);
                e.printStackTrace();
            } catch (BackendException e) {
                LOG.error("Song service fails to start", e);
                e.printStackTrace();
            }
        }
    }

    private synchronized void stopSong() {
        // desactivation du context pour ServletService et ExtServletService
        if (songService != null) {
            if (LOG.isInfoEnabled()) {
                LOG.info("Unregistering service [" + ExtServletService.class.getName() + ", " + ServletService.class.getName()
                        + "]...");
            }
            servletServiceRegistration.unregister();
            servletServiceRegistration = null;
            servletServiceImpl.stop();
            servletServiceImpl = null;

            if (LOG.isInfoEnabled()) {
                LOG.info("Unregistering service [" + SongService.class.getName() + ", " + ExtSongService.class.getName() + ", "
                        + SongBindingService.class.getName() + ", " + ExtSongBindingService.class.getName() + "]...");
            }
            songServiceRegistration.unregister();
            songServiceRegistration = null;
            songService.stop();
            songService = null;
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
        if (songService != null) {
            songService.setRoutesConfiguration(getRoutesConfiguration(config));
            songService.setMaxRemoteRequests(getMaxRemoteRequests(config));
            servletServiceImpl.setServerInfo(getServerInfo(context, config));
        } else {
            startSong();
        }
    }

    // customizer that handles tracked service
    // registration/modification/unregistration events
    private class BackendServiceCustomizer implements ServiceTrackerCustomizer {

        public Object addingService(ServiceReference reference) {
            if (LOG.isInfoEnabled()) {
                LOG.info("Adding service " + BackendService.class.getName() + "...");
            }
            if (backendService != null) {
                return backendService;
            }
            backendService = (BackendService) context.getService(reference);

            startSong();

            // Return the service to track it
            return backendService;
        }

        public void modifiedService(ServiceReference reference, Object service) {
        }

        public void removedService(ServiceReference reference, Object service) {
            if (LOG.isInfoEnabled()) {
                LOG.info("Removing service " + BackendService.class.getName() + "...");
            }
            if (service == backendService) {
                backendService = null;
                stopSong();
            }
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
            startSong();
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
                stopSong();
                resourcesAccessorService = null;
            }
        }
    }

    // customizer that handles tracked service
    // registration/modification/unregistration events
    private class XoServiceCustomizer implements ServiceTrackerCustomizer {

        public Object addingService(ServiceReference reference) {
            if (LOG.isInfoEnabled()) {
                LOG.info("Adding service " + XoService.class.getName() + "...");
            }
            if (xoService != null) {
                return null;
            }
            xoService = (XoService) context.getService(reference);

            // Do something with the service
            startSong();
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
                stopSong();
                xoService = null;
            }
        }
    }

    // customizer that handles tracked service
    // registration/modification/unregistration events
    private class TransportLoggerServiceCustomizer implements ServiceTrackerCustomizer {

        public Object addingService(ServiceReference reference) {
            if (LOG.isInfoEnabled()) {
                LOG.info("Adding service " + TransportLoggerService.class.getName() + "...");
            }
            if (transportLoggerService != null) {
                return null;
            }
            transportLoggerService = (TransportLoggerService) context.getService(reference);

            // Do something with the service
            startSong();
            // Return the service to track it
            return transportLoggerService;
        }

        public void modifiedService(ServiceReference reference, Object service) {
        }

        public void removedService(ServiceReference reference, Object service) {
            if (LOG.isInfoEnabled()) {
                LOG.info("Removing service " + TransportLoggerService.class.getName() + "...");
            }
            if (service == transportLoggerService) {
                stopSong();
                transportLoggerService = null;
            }
        }
    }
}
