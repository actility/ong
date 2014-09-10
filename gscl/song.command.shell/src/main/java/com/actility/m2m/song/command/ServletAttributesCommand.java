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
 * id $Id: ServletAttributesCommand.java 6134 2013-10-17 07:44:09Z mlouiset $
 * author $Author: mlouiset $
 * version $Revision: 6134 $
 * lastrevision $Date: 2013-10-17 09:44:09 +0200 (Thu, 17 Oct 2013) $
 * modifiedby $LastChangedBy: mlouiset $
 * lastmodified $LastChangedDate: 2013-10-17 09:44:09 +0200 (Thu, 17 Oct 2013) $
 */

package com.actility.m2m.song.command;

import java.io.PrintStream;
import java.util.Enumeration;
import java.util.Iterator;
import java.util.StringTokenizer;

import org.apache.felix.shell.Command;
import org.osgi.framework.BundleContext;
import org.osgi.framework.ServiceReference;

import com.actility.m2m.servlet.ext.ExtApplicationSession;
import com.actility.m2m.servlet.ext.ExtServletContext;
import com.actility.m2m.servlet.service.ext.ExtServletService;

public class ServletAttributesCommand implements Command {

    private BundleContext bundleContext;

    public ServletAttributesCommand(BundleContext bundleContext) {
        this.bundleContext = bundleContext;
    }

    public String getName() {
        return "servlet-attributes";
    }

    public String getUsage() {
        return "servlet-attributes [<id>]\n" + "<id>   Id of the servlet context or application session to print.\n"
                + "If no id is provided then all attributes from all\n"
                + " servlet contexts and application sessions are printed\n";
    }

    public String getShortDescription() {
        return "Get all attributes of the given application session";
    }

    public void execute(String line, PrintStream out, PrintStream err) {
        // Parse command line.
        StringTokenizer st = new StringTokenizer(line, " ");

        // Ignore the command name.
        st.nextToken();

        // Check for optional argument.
        String id = null;
        if (st.countTokens() >= 1) {
            id = st.nextToken().trim();
        }

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
            if (id == null || application.getServletContextName().equals(id)) {
                out.print("Servlet context: ");
                out.println(application.getServletContextName());
                Enumeration enumAttrNames = application.getAttributeNames();
                while (enumAttrNames.hasMoreElements()) {
                    String attrName = (String) enumAttrNames.nextElement();
                    out.print(" ");
                    out.print(attrName);
                    out.print(" -> ");
                    out.println(application.getAttribute(attrName));
                }
                out.println();
            }

            Iterator nIt = application.getApplicationSessions();

            while (nIt.hasNext()) {
                ExtApplicationSession appSession = (ExtApplicationSession) nIt.next();
                if (id == null || appSession.getId().equals(id)) {
                    out.print("Application session: ");
                    out.println(appSession.getId());
                    Iterator attrNames = appSession.getAttributeNames();
                    while (attrNames.hasNext()) {
                        String attrName = (String) attrNames.next();
                        out.print(" ");
                        out.print(attrName);
                        out.print(" -> ");
                        out.println(appSession.getAttribute(attrName));
                    }
                    out.println();
                }
            }
        }

        bundleContext.ungetService(sr);
    }
}
