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
 * id $Id: ShowCommand.java 8752 2014-05-21 15:21:42Z JReich $
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
import java.util.List;
import java.util.Map;
import java.util.StringTokenizer;

import org.apache.felix.shell.Command;
import org.osgi.framework.Bundle;
import org.osgi.framework.BundleContext;
import org.osgi.framework.ServiceReference;
import org.osgi.service.cm.ConfigurationAdmin;

import com.actility.m2m.framework.resources.ResourcesAccessorService;

public final class ShowCommand implements Command {
    private final ResourcesAccessorService resourcesAccessorService;
    private final BundleContext bundleContext;

    public ShowCommand(ResourcesAccessorService resourcesAccessorService, BundleContext bundleContext) {
        this.resourcesAccessorService = resourcesAccessorService;
        this.bundleContext = bundleContext;
    }

    public String getName() {
        return "logconfig-show";
    }

    public String getUsage() {
        return "logconfig-show [<bundle>] ...\n"
                + "When called without an argument, all bundles with a log level configuration\n"
                + "will be listed followed by all configurations currently not matching a\n" + "bundle\n"
                + "<bundle>     Show level for the specified bundles only. The bundle\n"
                + "             may be given as the bundle id, bundle's short-name,\n"
                + "             bundles symbolic name or the bundle location. If the bundle\n"
                + "             uses the default log level its line will end with the text \"(default)\"\n";
    }

    public String getShortDescription() {
        return "Show current log levels for bundles.";
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
                List argsBundles = new ArrayList();
                if (st.countTokens() >= 1) {
                    while (st.hasMoreTokens()) {
                        String token = st.nextToken().trim();
                        argsBundles.add(token);
                    }
                }

                Bundle[] bundles = bundleContext.getBundles();

                Map allBundlesLevel = config.getLogLevelPerBundle();
                String[] selections = null;
                if (argsBundles.size() > 0) {
                    selections = (String[]) argsBundles.toArray(new String[argsBundles.size()]);
                } else {
                    selections = (String[]) allBundlesLevel.keySet().toArray(new String[allBundlesLevel.size()]);
                }

                // Print the default filter level.
                out.println("    *  " + LogUtil.formatLevel(config.getLogLevel(), 8) + "(default)");

                for (int i = 0; i < selections.length; ++i) {
                    Bundle bundle = getMatchingBundle(bundles, selections[i]);
                    if (bundle == null) {
                        out.println("    -  " + selections[i] + " (Unknown bundle)");
                    } else {
                        String symbolicName = LogUtil.getSymbolicName(bundle);
                        String level = (String) allBundlesLevel.get(symbolicName);
                        if (level == null) {
                            out.println("    -  " + LogUtil.formatLevel(config.getLogLevel(), 8) + " " + getShortName(bundle)
                                    + " (default)");
                        } else {
                            out.println("    -  " + LogUtil.formatLevel(level, 8) + " " + getShortName(bundle));
                        }
                    }
                }
            }
        } catch (IOException e) {
            out.println("Internal error: " + e.getMessage());
            out.flush();
            return;
        } finally {
            bundleContext.getServiceReference(ConfigurationAdmin.class.getName());
        }
    }

    private Bundle getMatchingBundle(Bundle[] bundles, String selection) {
        for (int i = 0; i < bundles.length; i++) {
            String shortName = getShortName(bundles[i]);
            String symbolicName = LogUtil.getSymbolicName(bundles[i]); // May be null!
            String location = bundles[i].getLocation();

            try {
                long id = Long.parseLong(selection);
                if (bundles[i].getBundleId() == id) {
                    return bundles[i];
                }
            } catch (NumberFormatException e) {
                // Ignored
            }
            if (shortName.equals(selection) || location.equals(selection) || symbolicName.equals(selection)) {
                return bundles[i];
            }
            if (selection.startsWith("*")) {
                String s = selection.substring(1);
                if (shortName.endsWith(s) || location.endsWith(s) || symbolicName.endsWith(s)) {
                    return bundles[i];
                }
            } else if (selection.endsWith("*")) {
                String s = selection.substring(0, selection.length() - 1);
                if (shortName.startsWith(s) || symbolicName.startsWith(s)) {
                    return bundles[i];
                }
                if (s.indexOf(':') != -1 && location.startsWith(s)) {
                    return bundles[i];
                }
            } else {
                if (location.endsWith("/" + selection + ".jar") || location.endsWith("\\" + selection + ".jar")) {
                    return bundles[i];
                }
            }
        }
        return null;
    }

    /**
     * Get short name of specified bundle. First, try to get the BUNDLE-NAME header. If it fails use the location of the bundle
     * with all characters upto and including the last '/' or '\' and any trailing ".jar" stripped off.
     *
     * @param bundle the bundle
     * @return The bundles shortname or null if input was null
     */
    private String getShortName(Bundle bundle) {
        String n = (String) bundle.getHeaders().get("Bundle-Name");
        if (n == null) {
            n = bundle.getLocation();
            int x = n.lastIndexOf('/');
            int y = n.lastIndexOf('\\');
            if (y > x) {
                x = y;
            }
            if (x != -1) {
                n = n.substring(x + 1);
            }
            if (n.endsWith(".jar")) {
                n = n.substring(0, n.length() - 4);
            }
        }
        return n;
    }
}
