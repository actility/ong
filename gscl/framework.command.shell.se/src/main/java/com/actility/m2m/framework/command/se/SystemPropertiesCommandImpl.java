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
import java.util.Iterator;
import java.util.StringTokenizer;

import org.apache.felix.shell.Command;

/**
 * Command to display, set and modify system properties Usage: sysprop -&gt; displays all the system properties sysprop [key] -&gt;
 * displays the [key] property sysprop -r [key] -&gt; removes the [key] property sysprop [key] [value] -&gt; set the property [key] to
 * [value]
 */
public class SystemPropertiesCommandImpl implements Command {
    private static final String REMOVE_PROP_SWITCH = "-r";

    public void execute(String line, PrintStream out, PrintStream err) {
        StringTokenizer st = new StringTokenizer(line);
        int tokens = st.countTokens();

        if (tokens == 1) {
            printAll(out);
        } else {
            st.nextToken();
            String secondArgument = st.nextToken();

            if (tokens == 2) {
                out.println(secondArgument + "=" + System.getProperty(secondArgument));
            } else if (tokens == 3) {
                if (REMOVE_PROP_SWITCH.equals(secondArgument)) {
                    removeProperty(st.nextToken());
                } else {
                    String value = st.nextToken();
                    System.setProperty(secondArgument, value);
                    out.println("Set " + secondArgument + "=" + value);
                }
            }
        }
    }

    private void printAll(PrintStream out) {
        out.println("-------System properties------");
        for (Iterator keyIterator = System.getProperties().keySet().iterator(); keyIterator.hasNext();) {
            Object key = keyIterator.next();
            out.println(key.toString() + "=" + System.getProperty(key.toString()));
        }
    }

    private void removeProperty(String key) {
        System.getProperties().remove(key);
    }

    public String getName() {
        return "sysprop";
    }

    public String getShortDescription() {
        return "Display, set, modify and remove system properties";
    }

    public String getUsage() {
        return "sysprop [-r] [<key>] [<value>]";
    }
}
