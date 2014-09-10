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
 * id $Id: ServletAppSessionsCommand.java 6134 2013-10-17 07:44:09Z mlouiset $
 * author $Author: mlouiset $
 * version $Revision: 6134 $
 * lastrevision $Date: 2013-10-17 09:44:09 +0200 (Thu, 17 Oct 2013) $
 * modifiedby $LastChangedBy: mlouiset $
 * lastmodified $LastChangedDate: 2013-10-17 09:44:09 +0200 (Thu, 17 Oct 2013) $
 */

package com.actility.m2m.song.command;

import java.io.PrintStream;
import java.util.Iterator;

import org.apache.felix.shell.Command;
import org.osgi.framework.BundleContext;
import org.osgi.framework.ServiceReference;

import com.actility.m2m.servlet.ext.ExtApplicationSession;
import com.actility.m2m.servlet.ext.ExtServletContext;
import com.actility.m2m.servlet.service.ext.ExtServletService;

public class ServletAppSessionsCommand implements Command {

    private BundleContext bundleContext;

    public ServletAppSessionsCommand(BundleContext bundleContext) {
        this.bundleContext = bundleContext;
    }

    public String getName() {
        return "servlet-appsessions";
    }

    public String getUsage() {
        return "servlet-appsessions";
    }

    public String getShortDescription() {
        return "Get the all active application sessions";
    }

    public void execute(String line, PrintStream out, PrintStream err) {
        ServiceReference sr = bundleContext.getServiceReference(ExtServletService.class.getName());
        if (sr == null) {
            out.println("Unable to get the ExtServletService");
            return;
        }
        ExtServletService servletService = (ExtServletService) bundleContext.getService(sr);
        if (servletService == null) {
            out.println("Unable to get the ExtServletService");
            return;
        }

        Iterator it = servletService.getApplications();
        long now = System.currentTimeMillis();
        while (it.hasNext()) {
            ExtServletContext application = (ExtServletContext) it.next();
            Iterator nIt = application.getApplicationSessions();

            while (nIt.hasNext()) {
                ExtApplicationSession appSession = (ExtApplicationSession) nIt.next();
                out.print(appSession.getId());
                out.print(" (age: ");
                out.print(now - appSession.getCreationTime());
                out.print("ms)");
                long expirationTime = appSession.getExpirationTime();
                if (expirationTime < 0) {
                    out.print(" (never expires)");
                } else {
                    out.print(" (expires in: ");
                    out.print(appSession.getExpirationTime() - now);
                    out.print("ms)");
                }
                out.print(" (invalidate when ready: ");
                out.print(appSession.getInvalidateWhenReady());
                out.print(") (timers: ");
                out.print(appSession.getTimersSize());
                out.print(") (sessions: ");
                out.print(appSession.getProtocolSessionsSize());
                out.println(")");
            }
        }

        bundleContext.ungetService(sr);
    }
}
