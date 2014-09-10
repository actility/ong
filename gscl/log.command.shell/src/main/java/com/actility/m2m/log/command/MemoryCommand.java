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
 * id $Id: MemoryCommand.java 6795 2013-12-10 10:29:44Z JReich $
 * author $Author: JReich $
 * version $Revision: 6795 $
 * lastrevision $Date: 2013-12-10 11:29:44 +0100 (Tue, 10 Dec 2013) $
 * modifiedby $LastChangedBy: JReich $
 * lastmodified $LastChangedDate: 2013-12-10 11:29:44 +0100 (Tue, 10 Dec 2013) $
 */

package com.actility.m2m.log.command;

import java.io.IOException;
import java.io.PrintStream;
import java.util.StringTokenizer;

import org.apache.felix.shell.Command;
import org.osgi.framework.BundleContext;
import org.osgi.framework.ServiceReference;
import org.osgi.service.cm.ConfigurationAdmin;

import com.actility.m2m.framework.resources.ResourcesAccessorService;

public final class MemoryCommand implements Command {
    private final ResourcesAccessorService resourcesAccessorService;
    private final BundleContext bundleContext;

    public MemoryCommand(ResourcesAccessorService resourcesAccessorService, BundleContext bundleContext) {
        this.resourcesAccessorService = resourcesAccessorService;
        this.bundleContext = bundleContext;
    }

    public String getName() {
        return "logconfig-memory";
    }

    public String getUsage() {
        return "logconfig-memory [<int>]\n" + "<int>   The new number of log entries to keep\n";
    }

    public String getShortDescription() {
        return "Number of log entries to keep in memory.";
    }

    public void execute(String line, PrintStream out, PrintStream err) {
        ServiceReference serviceReference = bundleContext.getServiceReference(ConfigurationAdmin.class.getName());
        ConfigurationAdmin configAdmin = (ConfigurationAdmin) bundleContext.getService(serviceReference);
        try {
            if (configAdmin != null) {
                LogConfiguration config = new LogConfiguration(resourcesAccessorService, bundleContext, configAdmin);

                // Parse command line.
                StringTokenizer st = new StringTokenizer(line, " ");

                // Ignore the command name.
                st.nextToken();

                // Check for optional argument.
                int newSize = -1;
                if (st.countTokens() >= 1) {
                    while (st.hasMoreTokens()) {
                        String token = st.nextToken().trim();
                        newSize = Integer.parseInt(st.nextToken());
                    }
                }

                if (newSize >= 0) {
                    config.setMaxNbOfLogsInMemory(newSize);
                } else {
                    out.println("  log memory size: " + config.getMaxNbOfLogsInMemory());
                }
            }
        } catch (NumberFormatException e) {
            out.println("Bad integer argument: " + e.getMessage());
            out.print(getUsage());
            out.flush();
            return;
        } catch (IOException e) {
            out.println("Internal error: " + e.getMessage());
            out.flush();
            return;
        } finally {
            bundleContext.getServiceReference(ConfigurationAdmin.class.getName());
        }
    }
}
