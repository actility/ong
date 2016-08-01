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

package com.actility.m2m.be.command;

import java.io.File;
import java.io.PrintStream;
import java.util.StringTokenizer;

import org.apache.felix.shell.Command;

import com.actility.m2m.util.Profiler;

public final class BackendTracesCommand implements Command {

    public void execute(String line, PrintStream out, PrintStream err) {
        // Parse command line.
        StringTokenizer st = new StringTokenizer(line, " ");

        // Ignore the command name.
        st.nextToken();

        // Check for optional argument.
        boolean noArgs = true;
        boolean enable = false;
        File file = null;
        int index = 0;
        if (st.countTokens() >= 1) {
            noArgs = false;
            while (st.hasMoreTokens()) {
                String token = st.nextToken().trim();
                switch (index) {
                case 0:
                    enable = Boolean.valueOf(token).booleanValue();
                    break;
                case 1:
                    file = new File(token);
                    break;
                }
                ++index;
            }
        }

        if (noArgs) {
            out.println("Traces are " + ((Profiler.getInstance().isTraceEnabled()) ? "enabled " : "disabled"));
            out.println("Output file is: " + Profiler.getInstance().getTraceOutputFile().getPath());
        } else {
            if (enable) {
                if (file != null) {
                    Profiler.getInstance().setTraceOutputFile(file);
                }
                Profiler.getInstance().setTraceEnabled(true);
            } else {
                Profiler.getInstance().setTraceEnabled(false);
                if (file != null) {
                    Profiler.getInstance().setTraceOutputFile(file);
                }
            }
        }
    }

    public String getName() {
        return "backend-traces";
    }

    public String getShortDescription() {
        return "Configure traces to detect performances isssues";
    }

    public String getUsage() {
        return "backend-traces [<enable> [<file_path>]]\n"
                + "If no argument are given, prints the actual traces configuration (if it is enabled and where traces are logged)\n"
                + "Otherwise, configures traces:\n"
                + "  - enable: Whether to enable or disable traces (respectively 'true' or 'false')\n"
                + "  - file_path: Where to log traces on the file system";
    }
}
