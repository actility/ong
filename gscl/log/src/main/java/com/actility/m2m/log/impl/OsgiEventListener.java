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

package com.actility.m2m.log.impl;

import org.osgi.framework.Bundle;
import org.osgi.framework.BundleEvent;
import org.osgi.framework.BundleListener;
import org.osgi.framework.FrameworkEvent;
import org.osgi.framework.FrameworkListener;
import org.osgi.framework.ServiceEvent;
import org.osgi.framework.ServiceListener;
import org.osgi.framework.ServiceReference;
import org.osgi.service.log.LogService;

public class OsgiEventListener implements FrameworkListener, BundleListener, ServiceListener {
    private final LogReaderServiceImpl logReaderService;

    public OsgiEventListener(LogReaderServiceImpl logReaderService) {
        this.logReaderService = logReaderService;
    }

    /**
     * The framework event callback method inserts all framework events into the log. Events of type <code>error</code> are
     * logged at the error level other event types are logged on the info level.
     * <p/>
     * FrameworkListener callback.
     *
     * @param fe the framework event that has occurred.
     */
    public void frameworkEvent(FrameworkEvent fe) {
        int level = LogService.LOG_INFO;
        String msg = null;
        // We always include the Exception even though
        // the specification says differently
        Throwable thr = fe.getThrowable();
        switch (fe.getType()) {
        case FrameworkEvent.ERROR:
            msg = "FrameworkEvent ERROR";
            level = LogService.LOG_ERROR;
            break;
        case FrameworkEvent.STARTED:
            msg = "FrameworkEvent STARTED";
            level = LogService.LOG_INFO;
            break;
        case FrameworkEvent.STARTLEVEL_CHANGED:
            msg = "FrameworkEvent STARTLEVEL_CHANGED";
            level = LogService.LOG_INFO;
            break;
        case FrameworkEvent.PACKAGES_REFRESHED:
            msg = "FrameworkEvent PACKAGES_REFRESHED";
            level = LogService.LOG_INFO;
            break;
        case FrameworkEvent.WARNING:
            msg = "FrameworkEvent WARNING";
            level = LogService.LOG_INFO; // sic! According to spec.
            break;
        case FrameworkEvent.INFO:
            msg = "FrameworkEvent INFO";
            level = LogService.LOG_INFO;
            break;
        default:
            msg = "FrameworkEvent <" + fe.getType() + ">";
            level = LogService.LOG_WARNING;
            break;
        }
        logReaderService.log(new LogEntryImpl(fe.getBundle(), level, msg, thr));
    }

    /**
     * * The bundle event callback method inserts all bundle events * into the log. * * Events are all assinged the log level
     * info, *
     *
     * @param be the bundle event that has occured.
     */
    public void bundleChanged(BundleEvent be) {
        String msg = null;
        switch (be.getType()) {
        case BundleEvent.INSTALLED:
            msg = "BundleEvent INSTALLED";
            break;
        case BundleEvent.STARTED:
            msg = "BundleEvent STARTED";
            break;
        case BundleEvent.STOPPED:
            msg = "BundleEvent STOPPED";
            break;
        case BundleEvent.UPDATED:
            msg = "BundleEvent UPDATED";
            break;
        case BundleEvent.UNINSTALLED:
            msg = "BundleEvent UNINSTALLED";
            break;
        case BundleEvent.RESOLVED:
            msg = "BundleEvent RESOLVED";
            break;
        case BundleEvent.UNRESOLVED:
            msg = "BundleEvent UNRESOLVED";
            break;
        case BundleEvent.STARTING:
            msg = "BundleEvent STARTING";
            break;
        case BundleEvent.STOPPING:
            msg = "BundleEvent STOPPING";
            break;
        default:
            msg = "BundleEvent OTHER: " + be.getType();
            break;
        /*
         * case BundleEvent.STARTING: msg = "BundleEvent STARTING"; break; case BundleEvent.STOPPING: msg =
         * "BundleEvent STOPPING"; break;
         */
        }
        logReaderService.log(new LogEntryImpl(be.getBundle(), LogService.LOG_INFO, msg));
    }

    /**
     * * The service event callback method inserts all service events * into the log. * * Event of types REGISTERED,
     * UNREGISTERED are assinged the log level info. Events of type MODIFIED are assigned the log level DEBUG. *
     *
     * @param se the service event that has occured.
     */
    public void serviceChanged(ServiceEvent se) {
        ServiceReference sr = se.getServiceReference();
        Bundle bundle = sr.getBundle();
        String msg = null;
        int level = LogService.LOG_INFO;
        switch (se.getType()) {
        case ServiceEvent.REGISTERED:
            msg = "ServiceEvent REGISTERED";
            break;
        case ServiceEvent.UNREGISTERING:
            msg = "ServiceEvent UNREGISTERING";
            break;
        case ServiceEvent.MODIFIED:
            msg = "ServiceEvent MODIFIED";
            level = LogService.LOG_DEBUG;
            break;
        }
        logReaderService.log(new LogEntryImpl(bundle, sr, level, msg));
    }

}
