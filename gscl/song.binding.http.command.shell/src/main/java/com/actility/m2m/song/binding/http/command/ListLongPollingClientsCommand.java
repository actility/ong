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

package com.actility.m2m.song.binding.http.command;

import java.io.PrintStream;
import java.util.Iterator;
import java.util.List;

import org.apache.felix.shell.Command;
import org.osgi.framework.BundleContext;
import org.osgi.framework.ServiceReference;

import com.actility.m2m.song.binding.http.SongHttpBindingStatsService;

public final class ListLongPollingClientsCommand implements Command {

    private final BundleContext context;

    public ListLongPollingClientsCommand(BundleContext context) {
        this.context = context;
    }

    public String getName() {
        return "http-lslpclients";
    }

    public String getUsage() {
        return "http-lslpclients";
    }

    public String getShortDescription() {
        return "Gets the list of long polling client ids";
    }

    public void execute(String line, PrintStream out, PrintStream err) {
        ServiceReference sr = context.getServiceReference(SongHttpBindingStatsService.class.getName());
        if (sr == null) {
            err.println("Unable to get the SongHttpBindingStatsService");
            return;
        }
        SongHttpBindingStatsService songHttpBindingStatsService = (SongHttpBindingStatsService) context.getService(sr);
        if (songHttpBindingStatsService == null) {
            err.println("Unable to get the SongHttpBindingStatsService");
            return;
        }

        List ids = songHttpBindingStatsService.listLongPollingClients();
        Iterator it = ids.iterator();
        while (it.hasNext()) {
            out.println(it.next());
        }

        context.ungetService(sr);
    }

}
