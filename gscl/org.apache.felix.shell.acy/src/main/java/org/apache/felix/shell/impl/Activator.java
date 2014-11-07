/*
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements.  See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership.  The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License.  You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied.  See the License for the
 * specific language governing permissions and limitations
 * under the License.
 */
package org.apache.felix.shell.impl;

import java.io.PrintStream;
import java.util.HashSet;
import java.util.Set;

import org.apache.felix.shell.Command;
import org.apache.felix.shell.Session;
import org.apache.felix.shell.SessionCommand;
import org.osgi.framework.BundleActivator;
import org.osgi.framework.BundleContext;
import org.osgi.framework.FrameworkUtil;
import org.osgi.framework.InvalidSyntaxException;
import org.osgi.framework.ServiceReference;
import org.osgi.util.tracker.ServiceTracker;
import org.osgi.util.tracker.ServiceTrackerCustomizer;

import com.actility.m2m.framework.resources.ResourcesAccessorService;

public class Activator implements BundleActivator {
    private BundleContext context;
    private ShellServiceImpl shellService;
    private ServiceTracker commandTracker;
    private ServiceTracker resourcesAccessorServiceTracker;
    private ResourcesAccessorService resourcesAccessorService;

    public void start(BundleContext context) {
        this.context = context;

        // init and start ServiceTracker to tracks elements
        resourcesAccessorServiceTracker = new ServiceTracker(context, ResourcesAccessorService.class.getName(),
                new ResourcesAccessorServiceCustomizer());
        resourcesAccessorServiceTracker.open();
    }

    public void stop(BundleContext context) {
        resourcesAccessorServiceTracker.close();

        stopFelixShell();

        resourcesAccessorServiceTracker = null;
        resourcesAccessorService = null;
        this.context = null;
    }

    private synchronized void startFelixShell() {
        if (resourcesAccessorService != null) {
            // Register impl service implementation.
            String[] classes = { org.apache.felix.shell.ShellService.class.getName(),
                    org.ungoverned.osgi.service.shell.ShellService.class.getName() };
            context.registerService(classes, shellService = new ShellServiceImpl(), null);

            // Create a service tracker to listen for command services.
            String filter = "(|(objectClass=" + org.apache.felix.shell.Command.class.getName() + ")(objectClass="
                    + org.ungoverned.osgi.service.shell.Command.class.getName() + "))";
            try {
                commandTracker = new ServiceTracker(context, FrameworkUtil.createFilter(filter), commandTracker);
            } catch (InvalidSyntaxException ex) {
                // This should never happen.
            }

            commandTracker.open();
        }
    }

    private synchronized void stopFelixShell() {
        if (shellService != null) {
            commandTracker.close();
            shellService.close();

            commandTracker = null;
            shellService = null;
        }
    }

    private class ShellServiceImpl implements org.apache.felix.shell.ShellService,
            org.ungoverned.osgi.service.shell.ShellService {
        private Set m_sessionSet = new HashSet();

        public String[] getCommands() {
            Object[] commands = commandTracker.getServices();
            String[] names = (commands == null) ? new String[0] : new String[commands.length];
            for (int i = 0; i < names.length; i++) {
                names[i] = ((Command) commands[i]).getName();
            }
            return names;
        }

        public String getCommandUsage(String name) {
            Object[] commands = commandTracker.getServices();
            for (int i = 0; (commands != null) && (i < commands.length); i++) {
                Command command = (Command) commands[i];
                if (command.getName().equals(name)) {
                    return command.getUsage();
                }
            }
            return null;
        }

        public String getCommandDescription(String name) {
            Object[] commands = commandTracker.getServices();
            for (int i = 0; (commands != null) && (i < commands.length); i++) {
                Command command = (Command) commands[i];
                if (command.getName().equals(name)) {
                    return command.getShortDescription();
                }
            }
            return null;
        }

        public ServiceReference getCommandReference(String name) {
            ServiceReference[] refs = commandTracker.getServiceReferences();
            for (int i = 0; (refs != null) && (i < refs.length); i++) {
                Command command = (Command) commandTracker.getService(refs[i]);
                if ((command != null) && command.getName().equals(name)) {
                    return refs[i];
                }
            }
            return null;
        }

        public synchronized Session createSession() {
            Session session = new SessionImpl();
            m_sessionSet.add(session);
            return session;
        }

        public synchronized void deleteSession(Session session) {
            if (session instanceof SessionImpl) {
                m_sessionSet.remove(session);
                ((SessionImpl) session).close();
            }
        }

        public void executeCommand(Session session, String commandLine, PrintStream out, PrintStream err) throws Exception {
            commandLine = commandLine.trim();
            String commandName = (commandLine.indexOf(' ') >= 0) ? commandLine.substring(0, commandLine.indexOf(' '))
                    : commandLine;
            Command command = getCommand(commandName);
            if (command != null) {
                try {
                    if (command instanceof SessionCommand) {
                        ((SessionCommand) command).execute(session, commandLine, out, err);
                    } else {
                        command.execute(commandLine, out, err);
                    }
                } catch (Throwable ex) {
                    err.println("Unable to execute command: " + ex.toString());
                }
            } else {
                err.println("Command not found.");
            }
        }

        public void close() {
            m_sessionSet.clear();
        }

        Command getCommand(String name) {
            Object[] commands = commandTracker.getServices();
            for (int i = 0; (commands != null) && (i < commands.length); i++) {
                Command command = (Command) commands[i];
                if (command.getName().equals(name)) {
                    return command;
                }
            }
            return null;
        }
    }

    private static class OldCommandWrapper implements Command {
        private org.ungoverned.osgi.service.shell.Command m_oldCommand = null;

        public OldCommandWrapper(org.ungoverned.osgi.service.shell.Command oldCommand) {
            m_oldCommand = oldCommand;
        }

        public String getName() {
            return m_oldCommand.getName();
        }

        public String getUsage() {
            return m_oldCommand.getUsage();
        }

        public String getShortDescription() {
            return m_oldCommand.getShortDescription();
        }

        public void execute(String line, PrintStream out, PrintStream err) {
            m_oldCommand.execute(line, out, err);
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
            startFelixShell();
            // Return the service to track it
            return resourcesAccessorService;
        }

        public void modifiedService(ServiceReference reference, Object service) {
        }

        public void removedService(ServiceReference reference, Object service) {
            if (service == resourcesAccessorService) {
                stopFelixShell();
                resourcesAccessorService = null;
            }
        }
    }
}
