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
 * id $Id: SongRouteCommand.java 9043 2014-07-03 15:49:18Z JReich $
 * author $Author: JReich $
 * version $Revision: 9043 $
 * lastrevision $Date: 2014-07-03 17:49:18 +0200 (Thu, 03 Jul 2014) $
 * modifiedby $LastChangedBy: JReich $
 * lastmodified $LastChangedDate: 2014-07-03 17:49:18 +0200 (Thu, 03 Jul 2014) $
 */

package com.actility.m2m.song.command;

import java.io.PrintStream;
import java.util.Iterator;
import java.util.Map;

import org.apache.felix.shell.Command;
import org.osgi.framework.BundleContext;
import org.osgi.framework.ServiceReference;

import com.actility.m2m.servlet.song.binding.service.ext.ExtSongBindingService;
import com.actility.m2m.servlet.song.ext.SongNode;
import com.actility.m2m.servlet.song.service.ext.ExtSongService;

public class SongRouteCommand implements Command {

    private BundleContext bundleContext;

    public SongRouteCommand(BundleContext bundleContext) {
        this.bundleContext = bundleContext;
    }

    public String getName() {
        return "song-route";
    }

    public String getUsage() {
        return "song-route";
    }

    public String getShortDescription() {
        return "Get the route table of the song container";
    }

    public void execute(String line, PrintStream out, PrintStream err) {
        ServiceReference sr1 = bundleContext.getServiceReference(ExtSongService.class.getName());
        if (sr1 == null) {
            out.println("Unable to get the ExtSongService");
            return;
        }
        ServiceReference sr2 = bundleContext.getServiceReference(ExtSongBindingService.class.getName());
        if (sr2 == null) {
            out.println("Unable to get the ExtSongBindingService");
            return;
        }
        ExtSongService songService = (ExtSongService) bundleContext.getService(sr1);
        if (songService == null) {
            out.println("Unable to get the ExtSongService");
            return;
        }
        ExtSongBindingService songBindingService = (ExtSongBindingService) bundleContext.getService(sr2);
        if (songBindingService == null) {
            out.println("Unable to get the ExtSongBindingService");
            return;
        }

        String[] routesConfiguration = songService.getRoutesConfiguration();
        out.println("Route configuration:");
        for (int i = 0; i < routesConfiguration.length; ++i) {
            out.println("  " + routesConfiguration[i]);
        }
        out.println();

        SongNode proxyNode = songService.getProxyNode();
        if (proxyNode != null) {
            out.println("Proxy Node -> " + proxyNode.getTarget());
            out.println();
        }

        Iterator it = songBindingService.getSchemeRoutes();
        out.println("Schemes route table:");
        while (it.hasNext()) {
            Map.Entry entry = (Map.Entry) it.next();
            String scheme = (String) entry.getKey();
            SongNode endpoint = (SongNode) entry.getValue();
            out.println("  " + scheme + " -> " + endpoint.getTarget());
        }
        out.println();

        it = songService.getPathRoutes();
        out.println("Paths route table:");
        while (it.hasNext()) {
            Map.Entry entry = (Map.Entry) it.next();
            String path = (String) entry.getKey();
            SongNode endpoint = (SongNode) entry.getValue();
            out.println("  " + path + " -> " + endpoint.getTarget());
        }
        out.println();

        it = songService.getPrivatePathRoutes();
        out.println("Private paths route table:");
        while (it.hasNext()) {
            Map.Entry entry = (Map.Entry) it.next();
            String path = (String) entry.getKey();
            SongNode endpoint = (SongNode) entry.getValue();
            out.println("  " + path + " -> " + endpoint.getTarget());
        }
        out.println();

        it = songService.getExternalAliasRoutes();
        out.println("External aliases route table:");
        while (it.hasNext()) {
            Map.Entry entry = (Map.Entry) it.next();
            String externalAlias = (String) entry.getKey();
            String contextPath = (String) entry.getValue();
            out.println("  " + externalAlias + " -> " + contextPath);
        }
        out.println();

        bundleContext.ungetService(sr1);
        bundleContext.ungetService(sr2);
    }
}
