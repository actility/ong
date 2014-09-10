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
 * id $Id: Activator.java 7137 2014-01-07 08:45:18Z JReich $
 * author $Author: JReich $
 * version $Revision: 7137 $
 * lastrevision $Date: 2014-01-07 09:45:18 +0100 (Tue, 07 Jan 2014) $
 * modifiedby $LastChangedBy: JReich $
 * lastmodified $LastChangedDate: 2014-01-07 09:45:18 +0100 (Tue, 07 Jan 2014) $
 */

package com.actility.m2m.framework.command.me;

import org.apache.felix.shell.CdCommand;
import org.apache.felix.shell.Command;
import org.osgi.framework.BundleActivator;
import org.osgi.framework.BundleContext;
import org.osgi.framework.ServiceReference;
import org.osgi.framework.ServiceRegistration;
import org.osgi.util.tracker.ServiceTracker;
import org.osgi.util.tracker.ServiceTrackerCustomizer;

import com.actility.m2m.framework.resources.ResourcesAccessorService;

public class Activator implements BundleActivator {
    private BundleContext context;
    private ServiceTracker resourcesAccessorServiceTracker;
    private ResourcesAccessorService resourcesAccessorService;

    private ServiceRegistration memInfoRegistration;
    private ServiceRegistration servicesRegistration;
    private ServiceRegistration bundleLevelRegistration;
    private ServiceRegistration cdRegistration;
    private ServiceRegistration findRegistration;
    private ServiceRegistration headersRegistration;
    private ServiceRegistration helpRegistration;
    private ServiceRegistration logRegistration;
    private ServiceRegistration psRegistration;
    private ServiceRegistration startLevelRegistration;
    private ServiceRegistration versionRegistration;

    public void start(BundleContext context) throws Exception {
        this.context = context;

        // init and start ServiceTracker to tracks elements
        resourcesAccessorServiceTracker = new ServiceTracker(context, ResourcesAccessorService.class.getName(),
                new ResourcesAccessorServiceCustomizer());
        resourcesAccessorServiceTracker.open();
    }

    public void stop(BundleContext context) throws Exception {
        memInfoRegistration.unregister();
        servicesRegistration.unregister();
    }

    private void startFrameworkCommand() {
        if (resourcesAccessorService != null) {
            memInfoRegistration = context.registerService(Command.class.getName(), new MemInfoCommand(), null);
            servicesRegistration = context.registerService(Command.class.getName(), new ServicesCommand(context), null);

            // Register "bundlelevel" command service.
            bundleLevelRegistration = context.registerService(Command.class.getName(),
                    new BundleLevelCommandImpl(context), null);

            // Register "cd" command service.
            String[] classes = new String[2];
            classes[0] = Command.class.getName();
            classes[1] = CdCommand.class.getName();
            cdRegistration = context.registerService(classes, new CdCommandImpl(resourcesAccessorService, context), null);

            // Register "find" command service.
            findRegistration = context.registerService(Command.class.getName(), new FindCommandImpl(context), null);

            // Register "headers" command service.
            headersRegistration = context.registerService(Command.class.getName(), new HeadersCommandImpl(context), null);

            // Register "help" command service.
            helpRegistration = context.registerService(Command.class.getName(), new HelpCommandImpl(context), null);

            // Register "log" command service.
            logRegistration = context.registerService(Command.class.getName(), new LogCommandImpl(context), null);

            // Register "ps" command service.
            psRegistration = context.registerService(Command.class.getName(), new PsCommandImpl(context), null);

            // Register "startlevel" command service.
            startLevelRegistration = context.registerService(Command.class.getName(), new StartLevelCommandImpl(context), null);

            // Register "version" command service.
            versionRegistration = context.registerService(Command.class.getName(), new VersionCommandImpl(context), null);
        }
    }

    private void stopFrameworkCommand() {
        if (memInfoRegistration != null) {
            memInfoRegistration.unregister();
            servicesRegistration.unregister();
            bundleLevelRegistration.unregister();
            cdRegistration.unregister();
            findRegistration.unregister();
            headersRegistration.unregister();
            helpRegistration.unregister();
            logRegistration.unregister();
            psRegistration.unregister();
            startLevelRegistration.unregister();
            versionRegistration.unregister();

            memInfoRegistration = null;
            servicesRegistration = null;
            bundleLevelRegistration = null;
            cdRegistration = null;
            findRegistration = null;
            headersRegistration = null;
            helpRegistration = null;
            logRegistration = null;
            psRegistration = null;
            startLevelRegistration = null;
            versionRegistration = null;
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
            startFrameworkCommand();
            // Return the service to track it
            return resourcesAccessorService;
        }

        public void modifiedService(ServiceReference reference, Object service) {
        }

        public void removedService(ServiceReference reference, Object service) {
            if (service == resourcesAccessorService) {
                stopFrameworkCommand();
                resourcesAccessorService = null;
            }
        }
    }
}
