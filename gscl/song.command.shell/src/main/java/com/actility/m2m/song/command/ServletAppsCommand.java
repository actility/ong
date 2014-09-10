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
 * id $Id: ServletAppsCommand.java 6134 2013-10-17 07:44:09Z mlouiset $
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

import com.actility.m2m.servlet.ext.ExtServletContext;
import com.actility.m2m.servlet.service.ext.ExtServletService;

public class ServletAppsCommand implements Command {

    private BundleContext bundleContext;

    public ServletAppsCommand(BundleContext bundleContext) {
        this.bundleContext = bundleContext;
    }

    public String getName() {
        return "servlet-apps";
    }

    public String getUsage() {
        return "servlet-apps";
    }

    public String getShortDescription() {
        return "Get the full description of registered servlet applications";
    }

    public void execute(String line, PrintStream out, PrintStream err) {
        ServiceReference sr = bundleContext.getServiceReference(ExtServletService.class.getName());
        if (sr == null) {
            err.println("Unable to get the ExtServletService");
            return;
        }
        ExtServletService servletService = (ExtServletService) bundleContext.getService(sr);
        if (servletService == null) {
            err.println("Unable to get the ExtServletService");
            return;
        }

        Iterator it = servletService.getApplications();
        while (it.hasNext()) {
            ExtServletContext application = (ExtServletContext) it.next();
            out.println(application.getDescription());
        }

        bundleContext.ungetService(sr);
    }

}
