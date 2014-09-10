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

import java.io.PrintStream;
import java.net.MalformedURLException;
import java.net.URL;
import java.util.StringTokenizer;

import org.apache.felix.shell.CdCommand;
import org.apache.felix.shell.Command;
import org.osgi.framework.Bundle;
import org.osgi.framework.BundleContext;
import org.osgi.framework.BundleException;
import org.osgi.framework.ServiceReference;

public class InstallCommandImpl implements Command {
    private BundleContext m_context = null;

    public InstallCommandImpl(BundleContext context) {
        m_context = context;
    }

    public String getName() {
        return "install";
    }

    public String getUsage() {
        return "install <URL> [<URL> ...]";
    }

    public String getShortDescription() {
        return "install bundle(s).";
    }

    public void execute(String s, PrintStream out, PrintStream err) {
        StringTokenizer st = new StringTokenizer(s, " ");

        // Ignore the command name.
        st.nextToken();

        // There should be at least one URL.
        if (st.countTokens() >= 1) {
            StringBuffer sb = new StringBuffer();
            while (st.hasMoreTokens()) {
                String location = st.nextToken().trim();
                Bundle bundle = install(location, out, err);
                if (bundle != null) {
                    if (sb.length() > 0) {
                        sb.append(", ");
                    }
                    sb.append(bundle.getBundleId());
                }
            }
            if (sb.toString().indexOf(',') > 0) {
                out.println("Bundle IDs: " + sb.toString());
            } else if (sb.length() > 0) {
                out.println("Bundle ID: " + sb.toString());
            }
        } else {
            err.println("Incorrect number of arguments");
        }
    }

    protected Bundle install(String location, PrintStream out, PrintStream err) {
        String abs = absoluteLocation(location);
        if (abs == null) {
            err.println("Malformed location: " + location);
        } else {
            try {
                return m_context.installBundle(abs, null);
            } catch (IllegalStateException ex) {
                err.println(ex.toString());
            } catch (BundleException ex) {
                if (ex.getNestedException() != null) {
                    err.println(ex.getNestedException().toString());
                } else {
                    err.println(ex.toString());
                }
            } catch (Exception ex) {
                err.println(ex.toString());
            }
        }
        return null;
    }

    private String absoluteLocation(String location) {
        try {
            new URL(location);
        } catch (MalformedURLException ex) {
            // Try to create a valid URL using the base URL
            // contained in the "cd" command service.
            String baseURL = "";

            try {
                // Get a reference to the "cd" command service.
                ServiceReference ref = m_context.getServiceReference(org.apache.felix.shell.CdCommand.class.getName());

                if (ref != null) {
                    CdCommand cd = (CdCommand) m_context.getService(ref);
                    baseURL = cd.getBaseURL();
                    baseURL = (baseURL == null) ? "" : baseURL;
                    m_context.ungetService(ref);
                }

                String theURL = baseURL + location;
                new URL(theURL);
                location = theURL;
            } catch (Exception ex2) {
                // Just fall through and return the original location.
            }
        }
        return location;
    }
}