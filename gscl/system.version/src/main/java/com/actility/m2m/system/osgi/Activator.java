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
 * Please contact Actility, SA.,  4, rue Ampère 22300 LANNION FRANCE
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

package com.actility.m2m.system.osgi;

import java.util.Dictionary;
import java.util.Enumeration;
import java.util.Hashtable;

import javax.servlet.ServletContext;
import javax.servlet.ServletException;

import org.apache.log4j.Logger;
import org.osgi.framework.BundleActivator;
import org.osgi.framework.BundleContext;
import org.osgi.framework.Constants;
import org.osgi.framework.ServiceReference;
import org.osgi.service.cm.ConfigurationAdmin;
import org.osgi.service.cm.ConfigurationException;
import org.osgi.service.cm.ManagedService;
import org.osgi.util.tracker.ServiceTracker;
import org.osgi.util.tracker.ServiceTrackerCustomizer;

import com.actility.m2m.framework.resources.ResourcesAccessorService;
import com.actility.m2m.servlet.song.service.SongService;
import com.actility.m2m.system.config.Configuration;
import com.actility.m2m.system.log.BundleLogger;
import com.actility.m2m.system.version.SystemTimerListener;
import com.actility.m2m.system.version.SystemVersionServlet;
import com.actility.m2m.util.log.OSGiLogger;
import com.actility.m2m.xo.XoService;

/**
 * @author mlouiset
 *
 */
public class Activator implements BundleActivator, ManagedService {

    private static final Logger LOG = OSGiLogger.getLogger(Activator.class, BundleLogger.LOG);
    private Configuration configuration;
    private ServiceTracker songServiceServiceTracker;
    private ServiceTracker xoServiceServiceTracker;
    private ServiceTracker configAdminServiceTracker;
    private SongService songService;
    private XoService xoService;
    private ConfigurationAdmin configAdmin;
    private ServiceTracker resourcesAccessorServiceTracker;
    private ResourcesAccessorService resourcesAccessorService;

    private ServletContext applicationContext;
    private SystemVersionServlet systemVersionServlet;

    private BundleContext context;

//    private Dictionary config;
//    private ServiceRegistration managedServiceRegistration;

    /**
     * Called to activate the current bundle.
     *
     * @param context The bundle context
     */
    public void start(BundleContext context) throws Exception {
        this.context = context;
        // Initialize log mechanism
        BundleLogger.LOG.init(context);
        configuration = new Configuration(context.getBundle().getSymbolicName());

        if (LOG.isInfoEnabled()) {
            LOG.info("Starting bundle " + context.getBundle().getSymbolicName() + "...");
            LOG.info("Starting tracker for " + SongService.class.getName() + " service...");
        }

        // init and start ServiceTrackers to tracks elements
        // for SongService and for XoService
        songServiceServiceTracker = new ServiceTracker(context, SongService.class.getName(), new SongServiceCustomizer());
        songServiceServiceTracker.open();

        if (LOG.isInfoEnabled()) {
            LOG.info("Starting tracker for " + XoService.class.getName() + " service...");
        }

        xoServiceServiceTracker = new ServiceTracker(context, XoService.class.getName(), new XoServiceCustomizer());
        xoServiceServiceTracker.open();

        if (LOG.isInfoEnabled()) {
            LOG.info("Starting tracker for " + ConfigurationAdmin.class.getName() + " service...");
        }

        configAdminServiceTracker = new ServiceTracker(context, ConfigurationAdmin.class.getName(), new ConfigAdminServiceCustomizer());
        configAdminServiceTracker.open();

        if (LOG.isInfoEnabled()) {
            LOG.info("Starting tracker for " + ResourcesAccessorService.class.getName() + " service...");
        }

        // for ResourcesAccessorService
        resourcesAccessorServiceTracker = new ServiceTracker(context, ResourcesAccessorService.class.getName(),
                new ResourcesAccessorServiceCustomizer());
        resourcesAccessorServiceTracker.open();


        String pid = context.getBundle().getSymbolicName() + ".config";
        if (LOG.isInfoEnabled()) {
            LOG.info("Registering service " + ManagedService.class.getName() + " for pid " + pid + "...");
        }
        Dictionary props = new Hashtable();
        props.put(Constants.SERVICE_PID, pid);
        /*managedServiceRegistration = */context.registerService(ManagedService.class.getName(), this, props);
    }

    /**
     * Called to deactivate the current bundle.
     */
    public void stop(BundleContext context) throws Exception {
        if (LOG.isInfoEnabled()) {
            LOG.info("Stopping bundle " + context.getBundle().getSymbolicName() + "...");
            LOG.info("Stopping tracker for " + SongService.class.getName() + " service...");
        }
        songServiceServiceTracker.close();

        if (LOG.isInfoEnabled()) {
            LOG.info("Stopping tracker for " + XoService.class.getName() + " service...");
        }
        xoServiceServiceTracker.close();

        if (LOG.isInfoEnabled()) {
            LOG.info("Stopping tracker for " + ResourcesAccessorService.class.getName() + " service...");
        }
        resourcesAccessorServiceTracker.close();

        stopSystemVersionApp();

        songServiceServiceTracker = null;
        songService = null;
        xoServiceServiceTracker = null;
        xoService = null;
        resourcesAccessorServiceTracker = null;
        resourcesAccessorService = null;
        this.context = null;
        BundleLogger.LOG.init(null);

    }

    /**
     * Actually starts the application
     */
    private synchronized void startSystemVersionApp() {
        if ((songService != null) && (resourcesAccessorService != null) && (xoService != null) && (configAdmin != null)) {
            try {
                LOG.info("Starting System Application...");
                configuration.setConfigAdminService(configAdmin);
                // create the servlet
                systemVersionServlet = new SystemVersionServlet(context, resourcesAccessorService, xoService, configuration);

                // create System application instance and provide it as new created application
                applicationContext = songService.createApplication("/SYSTEM", "SYSTEM", null, new SystemTimerListener(), null);
                if (LOG.isDebugEnabled()) {
                    LOG.debug("Context path: " + applicationContext.getContextPath());
                }

                // register the servlet
                songService.registerServiceServlet(applicationContext, "/", "SystemVersionServlet", systemVersionServlet, null,
                        null);
            } catch (ServletException e) {
                LOG.error("Can't initialize servlet", e);
            } catch (Exception e) {
                LOG.error("Problem with the contract registration", e);
                e.printStackTrace();
            }
        }
    }

    /**
     * Actually stops the application
     */
    private synchronized void stopSystemVersionApp() {
        if (systemVersionServlet != null) {
            LOG.info("Stopping System Application...");
            songService.unregisterApplication(applicationContext);

            systemVersionServlet.uninit();
            systemVersionServlet = null;
            applicationContext = null;
        }
    }

    // customizer that handles tracked service
    // registration/modification/unregistration events
    private class SongServiceCustomizer implements ServiceTrackerCustomizer {

        public Object addingService(ServiceReference reference) {
            if (LOG.isInfoEnabled()) {
                LOG.info("Adding service " + SongService.class.getName() + "...");
            }
            if (songService != null) {
                return null;
            }
            songService = (SongService) context.getService(reference);

            startSystemVersionApp();
            // Return the service to track it
            return songService;
        }

        public void modifiedService(ServiceReference reference, Object service) {
        }

        public void removedService(ServiceReference reference, Object service) {
            if (LOG.isInfoEnabled()) {
                LOG.info("Removing service " + SongService.class.getName() + "...");
            }
            if (service == songService) {
                stopSystemVersionApp();
                songService = null;
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

            startSystemVersionApp();
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
                stopSystemVersionApp();
                xoService = null;
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

            startSystemVersionApp();
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
                stopSystemVersionApp();
                resourcesAccessorService = null;
            }
        }
    }
    private class ConfigAdminServiceCustomizer implements ServiceTrackerCustomizer {

        public Object addingService(ServiceReference reference) {
            if (LOG.isInfoEnabled()) {
                LOG.info("Adding service " + ConfigurationAdmin.class.getName() + "...");
            }
            if (configAdmin != null) {
                return null;
            }
            configAdmin = (ConfigurationAdmin) context.getService(reference);
            
            startSystemVersionApp();
            // Return the service to track it
            return configAdmin;
        }

        public void modifiedService(ServiceReference reference, Object service) {
            
        }

        public void removedService(ServiceReference reference, Object service) {
            if (LOG.isInfoEnabled()) {
                LOG.info("Removing service " + ConfigurationAdmin.class.getName() + "...");
            }
            if (service == configAdmin) {
                stopSystemVersionApp();
                configAdmin = null;
            }
        }
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
        configuration.update(properties);
//        startSystemVersionApp();
    }

}
