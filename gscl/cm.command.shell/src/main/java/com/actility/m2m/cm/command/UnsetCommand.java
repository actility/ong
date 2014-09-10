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
 * id $Id: UnsetCommand.java 7124 2014-01-04 13:15:00Z JReich $
 * author $Author: JReich $
 * version $Revision: 7124 $
 * lastrevision $Date: 2014-01-04 14:15:00 +0100 (Sat, 04 Jan 2014) $
 * modifiedby $LastChangedBy: JReich $
 * lastmodified $LastChangedDate: 2014-01-04 14:15:00 +0100 (Sat, 04 Jan 2014) $
 */

package com.actility.m2m.cm.command;

import java.io.PrintStream;
import java.util.Dictionary;
import java.util.StringTokenizer;

import org.apache.felix.shell.Session;
import org.apache.felix.shell.SessionCommand;
import org.osgi.framework.BundleContext;
import org.osgi.service.cm.Configuration;

public class UnsetCommand implements SessionCommand {

    private BundleContext bundleContext;

    public UnsetCommand(BundleContext bundleContext) {
        this.bundleContext = bundleContext;
    }

    public String getName() {
        return "cm-unset";
    }

    public String getUsage() {
        return "cm-unset <property>\n" + "<property> Name of property to remove from the configuration.";
    }

    public String getShortDescription() {
        return "Remove a property from the currently open configuration.";
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
        String property = null;
        if (st.countTokens() >= 1) {
            property = st.nextToken().trim();
        }

        try {
            if (session.getAttribute(Activator.CURRENT) == null) {
                throw new Exception("No configuration open currently");
            }
            Dictionary dict = (Dictionary) session.getAttribute(Activator.EDITED);
            if (dict == null) {
                Configuration cfg = (Configuration) session.getAttribute(Activator.CURRENT);
                dict = cfg.getProperties();
                session.setAttribute(Activator.EDITED, dict);
            }
            Object oldValue = dict.remove(property);
            if (oldValue == null) {
                throw new Exception("No property named " + property + " in current configuration.");
            }
        } catch (Exception e) {
            out.println("Unset failed. Details:");
            String reason = e.getMessage();
            out.println(reason == null ? "<unknown>: " + e.toString() : reason);
        }
    }

}
