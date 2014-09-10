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

package com.actility.m2m.inspector.osgi;

import javax.servlet.ServletContext;
import javax.servlet.ServletException;

import org.apache.felix.shell.Command;
import org.osgi.framework.BundleActivator;
import org.osgi.framework.BundleContext;
import org.osgi.framework.ServiceReference;
import org.osgi.framework.ServiceRegistration;
import org.osgi.util.tracker.ServiceTracker;
import org.osgi.util.tracker.ServiceTrackerCustomizer;

import com.actility.m2m.inspector.InspectorServlet;
import com.actility.m2m.inspector.RetrieveCommand;
import com.actility.m2m.servlet.song.service.SongService;

public class InspectorActivator implements BundleActivator {
    private BundleContext context;
    private ServiceTracker songServiceTracker;
    private ServiceRegistration retrieveRegistration;

    private SongService songService;

    private InspectorServlet inspector;
    private String bindingPath;
    private ServletContext applicationContext;

    public void start(BundleContext context) throws Exception {
        this.context = context;

        // init and start ServiceTracker to tracks elements
        songServiceTracker = new ServiceTracker(context, SongService.class.getName(), new SongServiceServiceCustomizer());
        songServiceTracker.open();
    }

    public void stop(BundleContext context) throws Exception {
        songServiceTracker.close();
        songServiceTracker = null;

        stopInspector();
    }

    private void startInspector() {
        boolean error = false;
        try {
            bindingPath = "/inpector";
            applicationContext = songService.createApplication(bindingPath, "INSPECTOR", null, null, null);

            inspector = new InspectorServlet();

            songService.registerServiceServlet(applicationContext, "Servlet", inspector, null, null);

            // Start telnet commands
            retrieveRegistration = context.registerService(Command.class.getName(),
                    new RetrieveCommand(inspector.getSongFactory()), null);
        } catch (ServletException e) {
            error = true;
        } catch (Throwable e) {
            error = true;
        } finally {
            if (error) {
                stopInspector();
            }
        }
    }

    private void stopInspector() {
        if (applicationContext != null) {
            try {
                songService.unregisterApplication(applicationContext);
            } catch (Exception e) {
                // Ignore
            }
            applicationContext = null;
        }
        if (retrieveRegistration != null) {
            retrieveRegistration.unregister();
            retrieveRegistration = null;
        }
    }

    class SongServiceServiceCustomizer implements ServiceTrackerCustomizer {

        public Object addingService(ServiceReference reference) {
            if (songService != null) {
                return null;
            }
            songService = (SongService) context.getService(reference);

            // Do something with the service
            if (songService != null) {
                startInspector();
            }
            // Return the service to track it
            return songService;
        }

        public void modifiedService(ServiceReference reference, Object service) {
        }

        public void removedService(ServiceReference reference, Object service) {
            if (service == songService) {
                if (songService != null) {
                    stopInspector();
                }
                songService = null;
            }
        }

    }

}
