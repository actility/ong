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
 * id $Id: SongTraceStartCommand.java 6135 2013-10-17 07:52:20Z mlouiset $
 * author $Author: mlouiset $
 * version $Revision: 6135 $
 * lastrevision $Date: 2013-10-17 09:52:20 +0200 (Thu, 17 Oct 2013) $
 * modifiedby $LastChangedBy: mlouiset $
 * lastmodified $LastChangedDate: 2013-10-17 09:52:20 +0200 (Thu, 17 Oct 2013) $
 */

package com.actility.m2m.song.trace.command;

import java.io.IOException;
import java.io.PrintStream;
import java.util.StringTokenizer;

import org.apache.felix.shell.Command;
import org.osgi.framework.BundleContext;
import org.osgi.framework.ServiceReference;

import com.actility.m2m.be.BackendException;
import com.actility.m2m.be.BackendService;

public class SongTraceStartCommand implements Command {
    private BundleContext bundleContext;

    public SongTraceStartCommand(BundleContext bundleContext) {
        this.bundleContext = bundleContext;
    }

    public String getName() {
        return "songtrace-start";
    }

    public String getUsage() {
        return "songtrace-start <file>\n" + "<file>   Filename where to log song exchanges";
    }

    public String getShortDescription() {
        return "Start tracing of song exchanges in a file";
    }

    public void execute(String line, PrintStream out, PrintStream err) {
        // Parse command line.
        StringTokenizer st = new StringTokenizer(line, " ");

        // Ignore the command name.
        st.nextToken();

        // Check for optional argument.
        String file = null;
        if (st.countTokens() >= 1) {
            file = st.nextToken().trim();
        }

        if (file != null) {
            ServiceReference sr = bundleContext.getServiceReference(BackendService.class.getName());
            if (sr == null) {
                err.println("Unable to get the BackendService");
                return;
            }
            BackendService backendService = (BackendService) bundleContext.getService(sr);
            if (backendService == null) {
                err.println("Unable to get the BackendService");
                return;
            }

            try {
                new SongBackendHook(backendService, file);
            } catch (IOException e) {
                err.println("Cannot open the specified file for writing");
                return;
            } catch (BackendException e) {
                err.println("A hook is already registered");
                return;
            }
            bundleContext.ungetService(sr);
        } else {
            err.println("File must be provided");
            return;
        }

        out.println("Trace started. Please stop tracing when finished -> songtrace stop");
    }

}
