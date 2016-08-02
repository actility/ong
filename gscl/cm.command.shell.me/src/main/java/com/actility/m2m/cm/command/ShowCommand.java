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
import java.util.Dictionary;
import java.util.Enumeration;
import java.util.StringTokenizer;
import java.util.Vector;

import org.apache.felix.shell.Session;
import org.apache.felix.shell.SessionCommand;
import org.osgi.framework.BundleContext;
import org.osgi.framework.ServiceReference;
import org.osgi.service.cm.Configuration;
import org.osgi.service.cm.ConfigurationAdmin;

public class ShowCommand implements SessionCommand {

    private BundleContext bundleContext;

    public ShowCommand(BundleContext bundleContext) {
        this.bundleContext = bundleContext;
    }

    public String getName() {
        return "cm-show";
    }

    public String getUsage() {
        return "cm-show [<selection>] ...\n" + "<selection>  A pid that can contain wildcards '*',\n"
                + "             or an ldap filter, or an index in output\n"
                + "             from the latest use of the 'list' command.\n"
                + "             If no selection is given all configurations\n" + "             will be shown.\n"
                + "             Use 'current' command to see the properties\n"
                + "             of the currently edited configuration.";
    }

    public String getShortDescription() {
        return "Show the saved versions of configurations.";
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
        String[] selection = null;
        if (argsSelection.size() > 0) {
            selection = (String[]) argsSelection.toArray(new String[argsSelection.size()]);
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
            Configuration[] cs = Activator.getConfigurations(bundleContext, session, configAdmin, selection);
            if (cs == null || cs.length == 0) {
                throw new Exception("No matching configurations for selection.");
            }
            for (int i = 0; i < cs.length; ++i) {
                if (i > 0) {
                    out.println();
                }
                Dictionary d = cs[i].getProperties();
                if (d == null) {
                    out.println("No properties set in " + cs[i].getPid());
                } else {
                    out.println("Properties for " + cs[i].getPid());
                    printDictionary(out, d);
                }
            }
        } catch (Exception e) {
            out.println("Show failed. Details:");
            String reason = e.getMessage();
            out.println(reason == null ? "<unknown>: " + e.toString() : reason);
        } finally {
            bundleContext.ungetService(sr);
        }
    }

    private void printDictionary(PrintStream out, Dictionary d) {
        String[] keyNames = new String[d.size()];
        int i = 0;
        for (Enumeration keys = d.keys(); keys.hasMoreElements();) {
            keyNames[i++] = (String) keys.nextElement();
        }
        for (i = 0; i < keyNames.length; i++) {
            out.print(" ");
            out.print(keyNames[i]);
            out.print(": ");
            printValue(out, d.get(keyNames[i]));
            out.println();
        }
    }

    private void printValue(PrintStream out, Object val) {
        if (val instanceof Vector) {
            Vector v = (Vector) val;
            out.print("{");
            for (int i = 0; i < v.size(); i++) {
                if (i > 0) {
                    out.print(", ");
                }
                printValue(out, v.elementAt(i));
            }
            out.print("}");
        } else if (val instanceof char[]) {
            char[] array = (char[]) val;
            out.print("[");
            for (int i = 0; i < array.length; i++) {
                if (i > 0) {
                    out.print(", ");
                }
                out.print(array[i]);
            }
            out.print("]");
        } else if (val instanceof byte[]) {
            byte[] array = (byte[]) val;
            out.print("[");
            for (int i = 0; i < array.length; i++) {
                if (i > 0) {
                    out.print(", ");
                }
                out.print(array[i]);
            }
            out.print("]");
        } else if (val instanceof short[]) {
            short[] array = (short[]) val;
            out.print("[");
            for (int i = 0; i < array.length; i++) {
                if (i > 0) {
                    out.print(", ");
                }
                out.print(array[i]);
            }
            out.print("]");
        } else if (val instanceof int[]) {
            int[] array = (int[]) val;
            out.print("[");
            for (int i = 0; i < array.length; i++) {
                if (i > 0) {
                    out.print(", ");
                }
                out.print(array[i]);
            }
            out.print("]");
        } else if (val instanceof long[]) {
            long[] array = (long[]) val;
            out.print("[");
            for (int i = 0; i < array.length; i++) {
                if (i > 0) {
                    out.print(", ");
                }
                out.print(array[i]);
            }
            out.print("]");
        } else if (val instanceof float[]) {
            float[] array = (float[]) val;
            out.print("[");
            for (int i = 0; i < array.length; i++) {
                if (i > 0) {
                    out.print(", ");
                }
                out.print(array[i]);
            }
            out.print("]");
        } else if (val instanceof double[]) {
            double[] array = (double[]) val;
            out.print("[");
            for (int i = 0; i < array.length; i++) {
                if (i > 0) {
                    out.print(", ");
                }
                out.print(array[i]);
            }
            out.print("]");
        } else if (val instanceof boolean[]) {
            boolean[] array = (boolean[]) val;
            out.print("[");
            for (int i = 0; i < array.length; i++) {
                if (i > 0) {
                    out.print(", ");
                }
                out.print(array[i]);
            }
            out.print("]");
        } else if (val.getClass().isArray()) {
            Object[] array = (Object[]) val;
            out.print("[");
            for (int i = 0; i < array.length; i++) {
                if (i > 0) {
                    out.print(", ");
                }
                out.print(array[i]);
            }
            out.print("]");
        } else {
            out.print(val.toString());
        }
    }
}
