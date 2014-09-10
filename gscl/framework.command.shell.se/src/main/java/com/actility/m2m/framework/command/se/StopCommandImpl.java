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
import java.util.ArrayList;
import java.util.List;
import java.util.StringTokenizer;

import org.apache.felix.shell.Command;
import org.osgi.framework.Bundle;
import org.osgi.framework.BundleContext;
import org.osgi.framework.BundleException;

public class StopCommandImpl implements Command {
    private static final String TRANSIENT_SWITCH = "-t";

    private BundleContext m_context = null;

    public StopCommandImpl(BundleContext context) {
        m_context = context;
    }

    public String getName() {
        return "stop";
    }

    public String getUsage() {
        return "stop [-t] <id> [<id> ...]";
    }

    public String getShortDescription() {
        return "stop bundle(s).";
    }

    public void execute(String s, PrintStream out, PrintStream err) {
        StringTokenizer st = new StringTokenizer(s, " ");

        // Ignore the command name.
        st.nextToken();

        // Put the remaining tokens into a list.
        List tokens = new ArrayList();
        for (int i = 0; st.hasMoreTokens(); i++) {
            tokens.add(st.nextToken());
        }

        // There should be at least one bundle id.
        if (tokens.size() >= 1) {
            while (tokens.size() > 0) {
                String id = ((String) tokens.remove(0)).trim();

                try {
                    long l = Long.parseLong(id);
                    Bundle bundle = m_context.getBundle(l);
                    if (bundle != null) {
                        bundle.stop();
                    } else {
                        err.println("Bundle ID " + id + " is invalid.");
                    }
                } catch (NumberFormatException ex) {
                    err.println("Unable to parse id '" + id + "'.");
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
        } else {
            err.println("Incorrect number of arguments");
        }
    }
}