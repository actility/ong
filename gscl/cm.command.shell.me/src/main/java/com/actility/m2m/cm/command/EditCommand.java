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
import java.util.ArrayList;
import java.util.StringTokenizer;

import org.apache.felix.shell.Session;
import org.apache.felix.shell.SessionCommand;
import org.osgi.framework.BundleContext;
import org.osgi.framework.ServiceReference;
import org.osgi.service.cm.Configuration;
import org.osgi.service.cm.ConfigurationAdmin;

public class EditCommand implements SessionCommand {

    private BundleContext bundleContext;

    public EditCommand(BundleContext bundleContext) {
        this.bundleContext = bundleContext;
    }

    public String getName() {
        return "cm-edit";
    }

    public String getUsage() {
        return "cm-edit <selection>\n" + "<selection>  A pid that can contain wildcards '*',\n"
                + "             or an ldap filter, or an index in output\n"
                + "             from the latest use of the 'list' command.\n"
                + "             If the selection doesn't match exactly one\n"
                + "             configuration it will have to be refined.";
    }

    public String getShortDescription() {
        return "Edit an existing configuration.";
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
        ArrayList argsSelection = new ArrayList();
        if (st.countTokens() >= 1) {
            while (st.hasMoreTokens()) {
                argsSelection.add(st.nextToken().trim());
            }
        }
        String[] selection = (String[]) argsSelection.toArray(new String[argsSelection.size()]);

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

        session.setAttribute(Activator.CURRENT, null);
        session.setAttribute(Activator.EDITED, null);

        try {
            Configuration[] cs = Activator.getConfigurations(bundleContext, session, configAdmin, selection);
            if (cs == null || cs.length == 0) {
                throw new Exception("Selection didn't match any configurations. "
                        + "Use 'create' to create the configuration you want to edit "
                        + "if it doesnt exist, or change your selection to match " + "exactly one configuration.");
            } else if (cs.length == 1) {
                out.println("Editing " + cs[0].getPid());
                session.setAttribute(Activator.CURRENT, cs[0]);
            } else {
                throw new Exception("Selection matched " + cs.length + " configurations. "
                        + "Refine your selection to match exactly one configuration.");
            }

        } catch (Exception e) {
            out.println("Edit failed. Details:");
            String reason = e.getMessage();
            out.println(reason == null ? "<unknown>: " + e.toString() : reason);
        } finally {
            bundleContext.ungetService(sr);
        }
    }
}
