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
 * id $Id: SongTraceStopCommand.java 6135 2013-10-17 07:52:20Z mlouiset $
 * author $Author: mlouiset $
 * version $Revision: 6135 $
 * lastrevision $Date: 2013-10-17 09:52:20 +0200 (Thu, 17 Oct 2013) $
 * modifiedby $LastChangedBy: mlouiset $
 * lastmodified $LastChangedDate: 2013-10-17 09:52:20 +0200 (Thu, 17 Oct 2013) $
 */

package com.actility.m2m.song.trace.command;

import java.io.IOException;
import java.io.PrintStream;

import org.apache.felix.shell.Command;
import org.osgi.framework.BundleContext;
import org.osgi.framework.ServiceReference;

import com.actility.m2m.be.BackendService;

public class SongTraceStopCommand implements Command {
    private BundleContext bundleContext;

    public SongTraceStopCommand(BundleContext bundleContext) {
        this.bundleContext = bundleContext;
    }

    public String getName() {
        return "songtrace-stop";
    }

    public String getUsage() {
        return "songtrace-stop";
    }

    public String getShortDescription() {
        return "Stop tracing of song exchanges in a file";
    }

    public void execute(String line, PrintStream out, PrintStream err) {
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

        Object objectHook = backendService.getHook();
        if (objectHook == null) {
            err.println("Trace is not started");
            return;
        }
        if (!(objectHook instanceof SongBackendHook)) {
            err.println("An unknwon hook is registered, cannot stop it");
            return;
        }
        SongBackendHook hook = (SongBackendHook) objectHook;

        try {
            hook.close();
            hook = null;
            out.println("Trace stopped");
        } catch (IOException e) {
            err.println("Problem while closing the trace file");
            return;
        }

        bundleContext.ungetService(sr);
    }
}
