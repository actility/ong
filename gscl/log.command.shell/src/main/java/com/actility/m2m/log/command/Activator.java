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
 * id $Id: Activator.java 6795 2013-12-10 10:29:44Z JReich $
 * author $Author: JReich $
 * version $Revision: 6795 $
 * lastrevision $Date: 2013-12-10 11:29:44 +0100 (Tue, 10 Dec 2013) $
 * modifiedby $LastChangedBy: JReich $
 * lastmodified $LastChangedDate: 2013-12-10 11:29:44 +0100 (Tue, 10 Dec 2013) $
 */

package com.actility.m2m.log.command;

import org.apache.felix.shell.Command;
import org.osgi.framework.BundleActivator;
import org.osgi.framework.BundleContext;
import org.osgi.framework.ServiceReference;
import org.osgi.framework.ServiceRegistration;
import org.osgi.util.tracker.ServiceTracker;
import org.osgi.util.tracker.ServiceTrackerCustomizer;

import com.actility.m2m.framework.resources.ResourcesAccessorService;

public final class Activator implements BundleActivator {
    private BundleContext context;
    private ServiceRegistration fileRegistration;
    private ServiceRegistration memRegistration;
    private ServiceRegistration outRegistration;
    private ServiceRegistration setLevelRegistration;
    private ServiceRegistration showRegistration;

    private ServiceTracker resourcesAccessorServiceTracker;
    private ResourcesAccessorService resourcesAccessorService;

    public void start(BundleContext context) throws Exception {
        this.context = context;

        // init and start ServiceTracker to tracks elements
        resourcesAccessorServiceTracker = new ServiceTracker(context, ResourcesAccessorService.class.getName(),
                new ResourcesAccessorServiceCustomizer());
        resourcesAccessorServiceTracker.open();
    }

    public void stop(BundleContext context) throws Exception {
        resourcesAccessorServiceTracker.close();

        stopLogCommands();

        resourcesAccessorServiceTracker = null;
        resourcesAccessorService = null;
        this.context = null;
    }

    private void startLogCommands() {
        if (resourcesAccessorService != null) {
            fileRegistration = context.registerService(Command.class.getName(), new FileCommand(resourcesAccessorService,
                    context), null);
            memRegistration = context.registerService(Command.class.getName(), new MemoryCommand(resourcesAccessorService,
                    context), null);
            outRegistration = context.registerService(Command.class.getName(),
                    new OutCommand(resourcesAccessorService, context), null);
            setLevelRegistration = context.registerService(Command.class.getName(), new SetLevelCommand(
                    resourcesAccessorService, context), null);
            showRegistration = context.registerService(Command.class.getName(), new ShowCommand(resourcesAccessorService,
                    context), null);
        }
    }

    private void stopLogCommands() {
        if (fileRegistration != null) {
            fileRegistration.unregister();
            memRegistration.unregister();
            outRegistration.unregister();
            setLevelRegistration.unregister();
            showRegistration.unregister();

            fileRegistration = null;
            memRegistration = null;
            outRegistration = null;
            setLevelRegistration = null;
            showRegistration = null;
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
            startLogCommands();
            // Return the service to track it
            return resourcesAccessorService;
        }

        public void modifiedService(ServiceReference reference, Object service) {
        }

        public void removedService(ServiceReference reference, Object service) {
            if (service == resourcesAccessorService) {
                stopLogCommands();
                resourcesAccessorService = null;
            }
        }
    }
}
