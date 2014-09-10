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

import java.io.IOException;
import java.io.InputStream;
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

public class UpdateCommandImpl implements Command {
    private BundleContext m_context = null;

    public UpdateCommandImpl(BundleContext context) {
        m_context = context;
    }

    public String getName() {
        return "update";
    }

    public String getUsage() {
        return "update <id> [<URL>]";
    }

    public String getShortDescription() {
        return "update bundle.";
    }

    public void execute(String s, PrintStream out, PrintStream err) {
        StringTokenizer st = new StringTokenizer(s, " ");

        // Ignore the command name.
        st.nextToken();

        // There should be at least a bundle ID, but there may
        // also be a URL.
        if ((st.countTokens() == 1) || (st.countTokens() == 2)) {
            String id = st.nextToken().trim();
            String location = st.countTokens() == 0 ? null : st.nextToken().trim();

            if (location != null) {
                location = absoluteLocation(location);

                if (location == null) {
                    err.println("Malformed location: " + location);
                }
            }

            try {
                // Get the bundle id.
                long l = Long.parseLong(id);

                // Get the bundle.
                Bundle bundle = m_context.getBundle(l);
                if (bundle != null) {
                    // Create input stream from location if present
                    // and use it to update, otherwise just update.
                    if (location != null) {
                        InputStream is = new URL(location).openStream();
                        bundle.update(is);
                    } else {
                        bundle.update();
                    }
                } else {
                    err.println("Bundle ID " + id + " is invalid.");
                }
            } catch (NumberFormatException ex) {
                err.println("Unable to parse id '" + id + "'.");
            } catch (MalformedURLException ex) {
                err.println("Unable to parse URL.");
            } catch (IOException ex) {
                err.println("Unable to open input stream: " + ex);
            } catch (BundleException ex) {
                if (ex.getNestedException() != null) {
                    err.println(ex.getNestedException().toString());
                } else {
                    err.println(ex.toString());
                }
            } catch (Exception ex) {
                err.println(ex.toString());
            }
        } else {
            err.println("Incorrect number of arguments");
        }
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