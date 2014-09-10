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
 * id $Id: SetLevelCommand.java 8752 2014-05-21 15:21:42Z JReich $
 * author $Author: JReich $
 * version $Revision: 8752 $
 * lastrevision $Date: 2014-05-21 17:21:42 +0200 (Wed, 21 May 2014) $
 * modifiedby $LastChangedBy: JReich $
 * lastmodified $LastChangedDate: 2014-05-21 17:21:42 +0200 (Wed, 21 May 2014) $
 */

package com.actility.m2m.log.command;

import java.io.IOException;
import java.io.PrintStream;
import java.util.ArrayList;
import java.util.StringTokenizer;

import org.apache.felix.shell.Command;
import org.osgi.framework.Bundle;
import org.osgi.framework.BundleContext;
import org.osgi.framework.ServiceReference;
import org.osgi.service.cm.ConfigurationAdmin;

import com.actility.m2m.framework.resources.ResourcesAccessorService;

public final class SetLevelCommand implements Command {
    private final ResourcesAccessorService resourcesAccessorService;
    private final BundleContext bundleContext;

    public SetLevelCommand(ResourcesAccessorService resourcesAccessorService, BundleContext bundleContext) {
        this.resourcesAccessorService = resourcesAccessorService;
        this.bundleContext = bundleContext;
    }

    public String getName() {
        return "logconfig-setlevel";
    }

    public String getUsage() {
        return "logconfig-setlevel <level> [<bundle>] ...\n"
                + "<level>   The new log level (one of error,warning,info,debug or default)\n"
                + "<bundle>  The bundle(s) that the new level applies to. If no bundles are\n"
                + "          given the default level is changed. The bundle may be given as\n"
                + "          the bundle id, the file location of the bundle or the bundle's\n"
                + "          short-name. If the bundle's short-name is given then the default\n"
                + "          configuration for all bundles with the given short-name will be set.\n"
                + "          This means that if wanting to set the configuration of a specific\n"
                + "          bundle the bundle id or the bundle location has to be given\n";
    }

    public String getShortDescription() {
        return "Set log level";
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
                String level = null;
                ArrayList argsBundles = new ArrayList();
                int index = 0;
                if (st.countTokens() >= 1) {
                    while (st.hasMoreTokens()) {
                        String token = st.nextToken().trim();
                        switch (index) {
                        case 0:
                            level = LogUtil.checkLevel(token);
                            break;
                        default:
                            argsBundles.add(token);
                            break;
                        }
                        ++index;
                    }
                }

                if (level == null) {
                    out.println("No or bad level argument: " + line);
                    out.print(getUsage());
                    out.flush();
                    return;
                }

                String[] selection = null;
                if (argsBundles.size() > 0) {
                    selection = (String[]) argsBundles.toArray(new String[argsBundles.size()]);
                }
                if (selection != null) {
                    setValidBundles(config, selection, level);
                } else {
                    config.setLogLevel(level);
                }
                config.save();
            }
        } catch (IOException e) {
            out.println("Internal error: " + e.getMessage());
            out.flush();
            return;
        } finally {
            bundleContext.getServiceReference(ConfigurationAdmin.class.getName());
        }
    }

    private void setValidBundles(LogConfiguration config, String[] givenBundles, String level) {
        String symbolicName = null;
        for (int i = givenBundles.length - 1; i >= 0; i--) {
            symbolicName = givenBundles[i].trim();
            try {
                long id = Long.parseLong(symbolicName);
                Bundle bundle = bundleContext.getBundle(id);
                if (null != bundle) {
                    symbolicName = LogUtil.getSymbolicName(bundle);
                } else {
                    symbolicName = null;
                }
            } catch (NumberFormatException nfe) {
                // Ignore
            }
            if (symbolicName != null && symbolicName.length() > 0) {
                if ("default".equals(level)) {
                    config.removeLogLevelForBundle(symbolicName);
                } else {
                    config.setLogLevelForBundle(symbolicName, level);
                }
            }
        }
    }
}
