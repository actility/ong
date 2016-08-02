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

package com.actility.m2m.cm.command;

import java.io.PrintStream;
import java.util.Dictionary;
import java.util.StringTokenizer;

import org.apache.felix.shell.Session;
import org.apache.felix.shell.SessionCommand;
import org.osgi.framework.BundleContext;
import org.osgi.framework.ServiceReference;
import org.osgi.service.cm.Configuration;
import org.osgi.service.cm.ConfigurationAdmin;

public class SaveCommand implements SessionCommand {

    private BundleContext bundleContext;

    public SaveCommand(BundleContext bundleContext) {
        this.bundleContext = bundleContext;
    }

    public String getName() {
        return "cm-save";
    }

    public String getUsage() {
        return "cm-save [-force]\n" + "-force   Force the save";
    }

    public String getShortDescription() {
        return "Save the currently open configuration in the CM.";
    }

    public void execute(String line, PrintStream out, PrintStream err) {
        // Ignore
    }

    public void execute(Session session, String line, PrintStream out, PrintStream err) {
        // Parse command line.
        StringTokenizer st = new StringTokenizer(line, " ");

        // Ignore the command name.
        st.nextToken();

        // Check for optional argument.
        String pid = null;
        boolean forceOptionNotSpecified = false;
        if (st.countTokens() >= 1) {
            while (st.hasMoreTokens()) {
                String token = st.nextToken().trim();
                if (token.equals("-force")) {
                    forceOptionNotSpecified = true;
                }
            }
        }

        ServiceReference sr = bundleContext.getServiceReference(ConfigurationAdmin.class.getName());
        if (sr == null) {
            err.println("Unable to get the ConfigurationAdmin");
            return;
        }
        ConfigurationAdmin configAdmin = (ConfigurationAdmin) bundleContext.getService(sr);
        if (configAdmin == null) {
            err.println("Unable to get the ConfigurationAdmin");
            return;
        }

        try {
            Configuration cfg = (Configuration) session.getAttribute(Activator.CURRENT);
            if (cfg == null) {
                throw new Exception("No configuration open currently");
            }

            if (forceOptionNotSpecified) {
                throw new Exception("The configuration has changed in CM since it was opened."
                        + "Use -force option if you want to force saving of your changes.");
            }

            if (session.getAttribute(Activator.EDITED) != null) {
                Dictionary dict = (Dictionary) session.getAttribute(Activator.EDITED);
                cfg.update(dict);
                session.setAttribute(Activator.EDITED, null);
            } else {
                throw new Exception("No changes to save");
            }
        } catch (Exception e) {
            out.println("Save failed. Details:");
            String reason = e.getMessage();
            out.println(reason == null ? "<unknown>: " + e.toString() : reason);
        } finally {
            bundleContext.ungetService(sr);
        }
    }
}
