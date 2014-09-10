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

package com.actility.m2m.framework.command.se;

import org.apache.felix.shell.Command;
import org.osgi.framework.BundleActivator;
import org.osgi.framework.BundleContext;
import org.osgi.framework.ServiceRegistration;

public class Activator implements BundleActivator {
    private BundleContext context = null;
    private ServiceRegistration installRegistration;
    private ServiceRegistration startRegistration;
    private ServiceRegistration stopRegistration;
    private ServiceRegistration syspropRegistration;
    private ServiceRegistration refreshRegistration;
    private ServiceRegistration resolveRegistration;
    private ServiceRegistration shutdownRegistration;
    private ServiceRegistration uninstallRegistration;
    private ServiceRegistration updateRegistration;
    private ServiceRegistration inspectRegistration;
    private ServiceRegistration threadsRegistration;

    public void start(BundleContext context) {
        this.context = context;

        // Register "install" command service.
        installRegistration = context.registerService(Command.class.getName(), new InstallCommandImpl(context), null);

        // Register "start" command service.
        startRegistration = context.registerService(Command.class.getName(), new StartCommandImpl(context), null);

        // Register "stop" command service.
        stopRegistration = context.registerService(Command.class.getName(), new StopCommandImpl(context), null);

        // Register "sysprop" command service.
        syspropRegistration = context.registerService(Command.class.getName(), new SystemPropertiesCommandImpl(), null);

        // Register "refresh" command service.
        refreshRegistration = context.registerService(Command.class.getName(), new RefreshCommandImpl(context), null);

        // Register "resolve" command service.
        resolveRegistration = context.registerService(Command.class.getName(), new ResolveCommandImpl(context), null);

        // Register "shutdown" command service.
        shutdownRegistration = context.registerService(Command.class.getName(), new ShutdownCommandImpl(context), null);

        // Register "uninstall" command service.
        uninstallRegistration = context.registerService(Command.class.getName(), new UninstallCommandImpl(context), null);

        // Register "update" command service.
        updateRegistration = context.registerService(Command.class.getName(), new UpdateCommandImpl(context), null);

        // Register "inspect" command service.
        inspectRegistration = context.registerService(Command.class.getName(), new InspectCommandImpl(context), null);

        // Register "threads" command service.
        threadsRegistration = context.registerService(Command.class.getName(), new ThreadsCommand(), null);
    }

    public void stop(BundleContext context) {
        installRegistration.unregister();
        startRegistration.unregister();
        stopRegistration.unregister();
        syspropRegistration.unregister();
        refreshRegistration.unregister();
        resolveRegistration.unregister();
        shutdownRegistration.unregister();
        uninstallRegistration.unregister();
        updateRegistration.unregister();
        inspectRegistration.unregister();
        threadsRegistration.unregister();
    }
}