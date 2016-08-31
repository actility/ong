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

package com.actility.m2m.xo.jni.osgi;

import java.io.File;

import org.apache.log4j.Logger;
import org.osgi.framework.BundleActivator;
import org.osgi.framework.BundleContext;
import org.osgi.framework.ServiceReference;
import org.osgi.framework.ServiceRegistration;
import org.osgi.util.tracker.ServiceTracker;
import org.osgi.util.tracker.ServiceTrackerCustomizer;

import com.actility.m2m.framework.resources.ResourcesAccessorService;
import com.actility.m2m.util.FileUtils;
import com.actility.m2m.util.UtilConstants;
import com.actility.m2m.util.log.OSGiLogger;
import com.actility.m2m.xo.jni.JniXo;
import com.actility.m2m.xo.jni.JniXoServiceImpl;
import com.actility.m2m.xo.jni.log.BundleLogger;
import com.actility.m2m.xo.ni.NiXoService;

public final class Activator implements BundleActivator {
    private static final Logger LOG = OSGiLogger.getLogger(Activator.class, BundleLogger.LOG);

    private BundleContext context;
    private ServiceRegistration sniXoNativeServiceRegistration;
    private ServiceTracker resourcesAccessorServiceTracker;
    private ResourcesAccessorService resourcesAccesorService;

    public void start(BundleContext context) throws Exception {
        BundleLogger.LOG.init(context);
        this.context = context;
        if (LOG.isInfoEnabled()) {
            LOG.info("Starting bundle " + context.getBundle().getSymbolicName() + "...");
            LOG.info("Starting tracker for " + ResourcesAccessorService.class.getName() + " service...");
        }

        // init and start ServiceTracker to tracks elements
        resourcesAccessorServiceTracker = new ServiceTracker(context, ResourcesAccessorService.class.getName(),
                new ResourcesAccessorServiceTracker());
        resourcesAccessorServiceTracker.open();
    }

    public void stop(BundleContext context) throws Exception {
        if (LOG.isInfoEnabled()) {
            LOG.info("Stoppping bundle " + context.getBundle().getSymbolicName() + "...");
            LOG.info("Stoppping tracker for " + ResourcesAccessorService.class.getName() + " service...");
        }
        resourcesAccessorServiceTracker.close();
        resourcesAccessorServiceTracker = null;
        resourcesAccesorService = null;

        stopJniXoService();
        this.context = null;
        BundleLogger.LOG.init(null);
    }

    private synchronized void startJniXoService() {
        if (resourcesAccesorService != null) {
            if (LOG.isInfoEnabled()) {
                LOG.info("Registering service " + NiXoService.class.getName() + "...");
            }
            String symbolicName = context.getBundle().getSymbolicName();
            File rootact = new File(resourcesAccesorService.getProperty(context, UtilConstants.FW_PROP_ROOTACT));
            String namespaces = resourcesAccesorService.getProperty(context, symbolicName + ".config.namespacesFile");
            String refs = resourcesAccesorService.getProperty(context, symbolicName + ".config.refsFile");
            String dict = resourcesAccesorService.getProperty(context, symbolicName + ".config.dictFile");
            File namespacesFile = null;
            File refsFile = null;
            File dictFile = null;
            if (namespaces == null || !(namespacesFile = FileUtils.normalizeFile(rootact, namespaces)).isFile()) {
                String error = "Cannot start Xo native service because the namespaces file is not set or does not exists: "
                        + namespacesFile;
                LOG.error(error);
                throw new IllegalStateException(error);
            }
            if (refs == null || !(refsFile = FileUtils.normalizeFile(rootact, refs)).isFile()) {
                String error = "Cannot start Xo native service because the refs file is not set or does not exists: "
                        + refsFile;
                LOG.error(error);
                throw new IllegalStateException(error);
            }
            if (dict == null || !(dictFile = FileUtils.normalizeFile(rootact, dict)).isFile()) {
                String error = "Cannot start Xo native service because the dictionary file is not set or does not exists: "
                        + dictFile;
                LOG.error(error);
                throw new IllegalStateException(error);
            }
            if (!JniXo.start(namespacesFile.getAbsolutePath(), refsFile.getAbsolutePath(),
                    dictFile.getAbsolutePath())) {
                String error = "Cannot start Xo JNI stack";
                LOG.error(error);
                throw new IllegalStateException(error);
            }
            sniXoNativeServiceRegistration = context.registerService(NiXoService.class.getName(), new JniXoServiceImpl(), null);
        }
    }

    private synchronized void stopJniXoService() {
        if (sniXoNativeServiceRegistration != null) {
            if (LOG.isInfoEnabled()) {
                LOG.info("Unregistering service " + NiXoService.class.getName() + "...");
            }
            sniXoNativeServiceRegistration.unregister();
            sniXoNativeServiceRegistration = null;
            JniXo.stop();
        }
    }

    // customizer that handles tracked service
    // registration/modification/unregistration events
    private class ResourcesAccessorServiceTracker implements ServiceTrackerCustomizer {

        public Object addingService(ServiceReference reference) {
            if (LOG.isInfoEnabled()) {
                LOG.info("Adding service " + ResourcesAccessorService.class.getName() + "...");
            }
            if (resourcesAccesorService != null) {
                return null;
            }
            resourcesAccesorService = (ResourcesAccessorService) context.getService(reference);

            // Do something with the service
            startJniXoService();
            // Return the service to track it
            return resourcesAccesorService;
        }

        public void modifiedService(ServiceReference reference, Object service) {
        }

        public void removedService(ServiceReference reference, Object service) {
            if (LOG.isInfoEnabled()) {
                LOG.info("Removing service " + ResourcesAccessorService.class.getName() + "...");
            }
            if (service == resourcesAccesorService) {
                stopJniXoService();
                resourcesAccesorService = null;
            }
        }
    }
}
