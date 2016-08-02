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
import java.util.Enumeration;
import java.util.Vector;

import org.apache.felix.shell.Session;
import org.apache.felix.shell.SessionCommand;
import org.osgi.framework.BundleContext;
import org.osgi.framework.ServiceReference;
import org.osgi.service.cm.Configuration;
import org.osgi.service.cm.ConfigurationAdmin;

public class CurrentCommand implements SessionCommand {
    private BundleContext bundleContext;

    public CurrentCommand(BundleContext bundleContext) {
        this.bundleContext = bundleContext;
    }

    public String getName() {
        return "cm-current";
    }

    public String getUsage() {
        return "cm-current";
    }

    public String getShortDescription() {
        return "Show the currently open configuration.";
    }

    public void execute(String line, PrintStream out, PrintStream err) {
        // Ignore
    }

    public void execute(Session session, String line, PrintStream out, PrintStream err) {
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

        Configuration cfg = (Configuration) session.getAttribute(Activator.CURRENT);
        if (cfg == null) {
            out.println("No configuration open currently");
        } else {
            if (session.getAttribute(Activator.EDITED) != null) {
                printDictionary(out, (Dictionary) session.getAttribute(Activator.EDITED));
            } else {
                Dictionary dict = cfg.getProperties();
                if (dict == null) {
                    out.println("No properties set in current configuration");
                } else {
                    printDictionary(out, dict);
                }
            }
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

    public void printValue(PrintStream out, Object val) {
      out.print(val.toString());
    }

    public void printValue(PrintStream out, Vector val) {
      out.print("{");
      for (int i = 0; i < val.size(); i++) {
        if (i > 0) {
          out.print(", ");
        }
        printValue(out, val.elementAt(i));
      }
      out.print("}");
    }

    public void printValue(PrintStream out, Object[] val) {
      int length = val.length;
      out.print("[");
      for (int i = 0; i < length; i++) {
        if (i > 0) {
          out.print(", ");
        }
        printValue(out, val[i]);
      }
      out.print("]");
    }

}
