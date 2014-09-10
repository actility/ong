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
 * id $Id: BackendEndpointsCommand.java 6112 2013-10-15 15:51:58Z mlouiset $
 * author $Author: mlouiset $
 * version $Revision: 6112 $
 * lastrevision $Date: 2013-10-15 17:51:58 +0200 (Tue, 15 Oct 2013) $
 * modifiedby $LastChangedBy: mlouiset $
 * lastmodified $LastChangedDate: 2013-10-15 17:51:58 +0200 (Tue, 15 Oct 2013) $
 */

package com.actility.m2m.be.command;

import java.io.PrintStream;
import java.util.Iterator;

import org.apache.felix.shell.Command;
import org.osgi.framework.BundleContext;
import org.osgi.framework.ServiceReference;

import com.actility.m2m.be.BackendEndpointContext;
import com.actility.m2m.be.BackendService;

public final class BackendEndpointsCommand implements Command {

    private final BundleContext bundleContext;

    public BackendEndpointsCommand(BundleContext bundleContext) {
        this.bundleContext = bundleContext;
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

        Iterator it = backendService.getEndpoints();
        while (it.hasNext()) {
            BackendEndpointContext endpoint = (BackendEndpointContext) it.next();
            out.println("ID:" + endpoint.getEndpoint().getContext().getEndpointId() + " -> "
                    + endpoint.getEndpoint().toString());
        }

        bundleContext.ungetService(sr);
    }

    public String getName() {
        return "backend-endpoints";
    }

    public String getShortDescription() {
        return "Gets the list of registered backend endpoints";
    }

    public String getUsage() {
        return "backend-endpoints";
    }

}
