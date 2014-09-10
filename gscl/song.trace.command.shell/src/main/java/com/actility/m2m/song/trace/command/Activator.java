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
 * id $Id: Activator.java 6135 2013-10-17 07:52:20Z mlouiset $
 * author $Author: mlouiset $
 * version $Revision: 6135 $
 * lastrevision $Date: 2013-10-17 09:52:20 +0200 (Thu, 17 Oct 2013) $
 * modifiedby $LastChangedBy: mlouiset $
 * lastmodified $LastChangedDate: 2013-10-17 09:52:20 +0200 (Thu, 17 Oct 2013) $
 */

package com.actility.m2m.song.trace.command;

import org.apache.felix.shell.Command;
import org.osgi.framework.BundleActivator;
import org.osgi.framework.BundleContext;
import org.osgi.framework.ServiceRegistration;

public class Activator implements BundleActivator {
    private BundleContext bundleContext;
    private ServiceRegistration songTraceStartRegistration;
    private ServiceRegistration songTraceStopRegistration;

    public void start(BundleContext context) throws Exception {
        bundleContext = context;
        songTraceStartRegistration = context.registerService(Command.class.getName(), new SongTraceStartCommand(bundleContext),
                null);
        songTraceStopRegistration = context.registerService(Command.class.getName(), new SongTraceStopCommand(bundleContext),
                null);
    }

    public void stop(BundleContext context) throws Exception {
        songTraceStartRegistration.unregister();
        songTraceStopRegistration.unregister();
    }
}
