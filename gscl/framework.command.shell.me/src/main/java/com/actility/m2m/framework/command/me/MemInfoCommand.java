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
 * id $Id: MemInfoCommand.java 7137 2014-01-07 08:45:18Z JReich $
 * author $Author: JReich $
 * version $Revision: 7137 $
 * lastrevision $Date: 2014-01-07 09:45:18 +0100 (Tue, 07 Jan 2014) $
 * modifiedby $LastChangedBy: JReich $
 * lastmodified $LastChangedDate: 2014-01-07 09:45:18 +0100 (Tue, 07 Jan 2014) $
 */

package com.actility.m2m.framework.command.me;

import java.io.PrintStream;
import java.util.StringTokenizer;

import org.apache.felix.shell.Command;

public final class MemInfoCommand implements Command {

    public String getName() {
        return "meminfo";
    }

    public String getUsage() {
        return "meminfo [-gc] [-b | -m]\n" + "-gc  Run garbage collector first\n" + "-b   Display using bytes\n"
                + "-m   Display using megabytes\n";
    }

    public String getShortDescription() {
        return "Display java memory information, in kilobytes";
    }

    public void execute(String line, PrintStream out, PrintStream err) {
        // Parse command line.
        StringTokenizer st = new StringTokenizer(line, " ");

        // Ignore the command name.
        st.nextToken();

        // Check for optional argument.
        int unit = 1024;
        String unitStr = "kB";
        if (st.countTokens() >= 1) {
            while (st.hasMoreTokens()) {
                String token = st.nextToken().trim();
                if (token.equals("-gc")) {
                    System.gc();
                } else if (token.equals("-b")) {
                    unit = 1;
                    unitStr = "bytes";
                } else if (token.equals("-m")) {
                    unit = 1024 * 1024;
                    unitStr = "MB";
                }
            }
        }

        Runtime r = Runtime.getRuntime();
        // PORTAGE Runtime.maxMemory unavailable
        // out.println("Total: " + (r.totalMemory() + unit / 2) / unit + unitStr + "  Free: " + (r.freeMemory() + unit / 2) /
        // unit
        // + unitStr + "  Max: " + (r.maxMemory() + unit / 2) / unit + unitStr);
        out.println("Total: " + (r.totalMemory() + unit / 2) / unit + unitStr + "  Free: " + (r.freeMemory() + unit / 2) / unit
                + unitStr);
    }
}
