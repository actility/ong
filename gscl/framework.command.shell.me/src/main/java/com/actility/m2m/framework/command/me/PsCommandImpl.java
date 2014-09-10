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

package com.actility.m2m.framework.command.me;

import java.io.PrintStream;
import java.util.StringTokenizer;

import org.apache.felix.shell.Command;
import org.osgi.framework.Bundle;
import org.osgi.framework.BundleContext;
import org.osgi.framework.Constants;
import org.osgi.framework.ServiceReference;
import org.osgi.service.startlevel.StartLevel;

public class PsCommandImpl implements Command {
    private static final String LOCATION_SWITCH = "-l";
    private static final String SYMBOLIC_NAME_SWITCH = "-s";
    private static final String UPDATE_LOCATION_SWITCH = "-u";

    protected final BundleContext m_context;

    public PsCommandImpl(BundleContext context) {
        m_context = context;
    }

    public String getName() {
        return "ps";
    }

    public String getUsage() {
        return "ps [" + LOCATION_SWITCH + " | " + SYMBOLIC_NAME_SWITCH + " | " + UPDATE_LOCATION_SWITCH + "]";
    }

    public String getShortDescription() {
        return "list installed bundles.";
    }

    public void execute(String s, PrintStream out, PrintStream err) {
        // Get start level service.
        ServiceReference ref = m_context.getServiceReference(org.osgi.service.startlevel.StartLevel.class.getName());
        StartLevel sl = null;
        if (ref != null) {
            sl = (StartLevel) m_context.getService(ref);
        }

        if (sl == null) {
            out.println("StartLevel service is unavailable.");
        }

        // Parse command line.
        StringTokenizer st = new StringTokenizer(s, " ");

        // Ignore the command name.
        st.nextToken();

        // Check for optional argument.
        boolean showLoc = false;
        boolean showSymbolic = false;
        boolean showUpdate = false;
        if (st.countTokens() >= 1) {
            while (st.hasMoreTokens()) {
                String token = st.nextToken().trim();
                if (token.equals(LOCATION_SWITCH)) {
                    showLoc = true;
                } else if (token.equals(SYMBOLIC_NAME_SWITCH)) {
                    showSymbolic = true;
                } else if (token.equals(UPDATE_LOCATION_SWITCH)) {
                    showUpdate = true;
                }
            }
        }
        Bundle[] bundles = m_context.getBundles();
        if (bundles != null) {
            sortBundlesId(bundles);
            printBundleList(bundles, sl, out, showLoc, showSymbolic, showUpdate);
        } else {
            out.println("There are no installed bundles.");
        }
    }

    protected void printBundleList(Bundle[] bundles, StartLevel startLevel, PrintStream out, boolean showLoc,
            boolean showSymbolic, boolean showUpdate) {
        // Display active start level.
        if (startLevel != null) {
            out.println("START LEVEL " + startLevel.getStartLevel());
        }

        // Print column headers.
        String msg = " Name";
        if (showLoc) {
            msg = " Location";
        } else if (showSymbolic) {
            msg = " Symbolic name";
        } else if (showUpdate) {
            msg = " Update location";
        }
        String level = (startLevel == null) ? "" : "  Level ";
        out.println("   ID " + "  State       " + level + msg);
        for (int i = 0; i < bundles.length; i++) {
            // Get the bundle name or location.
            String name = (String) bundles[i].getHeaders().get(Constants.BUNDLE_NAME);
            // If there is no name, then default to symbolic name.
            name = (name == null) ? bundles[i].getSymbolicName() : name;
            // If there is no symbolic name, resort to location.
            name = (name == null) ? bundles[i].getLocation() : name;

            // Overwrite the default value is the user specifically
            // requested to display one or the other.
            if (showLoc) {
                name = bundles[i].getLocation();
            } else if (showSymbolic) {
                name = bundles[i].getSymbolicName();
                name = (name == null) ? "<no symbolic name>" : name;
            } else if (showUpdate) {
                name = (String) bundles[i].getHeaders().get(Constants.BUNDLE_UPDATELOCATION);
                name = (name == null) ? bundles[i].getLocation() : name;
            }
            // Show bundle version if not showing location.
            String version = (String) bundles[i].getHeaders().get(Constants.BUNDLE_VERSION);
            name = (!showLoc && !showUpdate && (version != null)) ? name + " (" + version + ")" : name;
            long l = bundles[i].getBundleId();
            String id = String.valueOf(l);
            if (startLevel == null) {
                level = "1";
            } else {
                level = String.valueOf(startLevel.getBundleStartLevel(bundles[i]));
            }
            while (level.length() < 5) {
                level = " " + level;
            }
            while (id.length() < 4) {
                id = " " + id;
            }
            out.println("[" + id + "] [" + getStateString(bundles[i].getState()) + "] [" + level + "] " + name);
        }
    }

    public String getStateString(int i) {
        if (i == Bundle.ACTIVE) {
            return "Active     ";
        } else if (i == Bundle.INSTALLED) {
            return "Installed  ";
        } else if (i == Bundle.RESOLVED) {
            return "Resolved   ";
        } else if (i == Bundle.STARTING) {
            return "Starting   ";
        } else if (i == Bundle.STOPPING) {
            return "Stopping   ";
        }
        return "Unknown    ";
    }

    private void sortBundlesId(Bundle[] bundles) {
        int x = bundles.length;

        for (int l = x; x > 0;) {
            x = 0;
            long p = bundles[0] != null ? bundles[0].getBundleId() : Long.MAX_VALUE;
            for (int i = 1; i < l; i++) {
                long n = bundles[i] != null ? bundles[i].getBundleId() : Long.MAX_VALUE;
                if (p > n) {
                    x = i - 1;
                    Bundle t = bundles[x];
                    bundles[x] = bundles[i];
                    bundles[i] = t;
                } else {
                    p = n;
                }
            }
        }
    }

}