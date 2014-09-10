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

package com.actility.m2m.shutdown.hook.jni.osgi;

import org.apache.log4j.Logger;
import org.osgi.framework.BundleActivator;
import org.osgi.framework.BundleContext;
import org.osgi.framework.ServiceRegistration;

import com.actility.m2m.shutdown.hook.jni.impl.JniShutdownHookServiceImpl;
import com.actility.m2m.shutdown.hook.jni.log.BundleLogger;
import com.actility.m2m.shutdown.hook.ni.NiShutdownHookService;
import com.actility.m2m.util.log.OSGiLogger;

public class Activator implements BundleActivator {
    private static final Logger LOG = OSGiLogger.getLogger(Activator.class, BundleLogger.LOG);

    private ServiceRegistration niShudownHookServiceRegistration;

    public void start(BundleContext context) throws Exception {
        BundleLogger.LOG.init(context);

        if (LOG.isInfoEnabled()) {
            LOG.info("Starting bundle " + context.getBundle().getSymbolicName() + "...");
            LOG.info("Registering service " + NiShutdownHookService.class.getName() + "...");
        }
        niShudownHookServiceRegistration = context.registerService(NiShutdownHookService.class.getName(),
                new JniShutdownHookServiceImpl(), null);
    }

    public void stop(BundleContext context) throws Exception {
        if (LOG.isInfoEnabled()) {
            LOG.info("Stopping bundle " + context.getBundle().getSymbolicName() + "...");
            LOG.info("Unregistering service " + NiShutdownHookService.class.getName() + "...");
        }
        niShudownHookServiceRegistration.unregister();
        niShudownHookServiceRegistration = null;

        BundleLogger.LOG.init(null);
    }

}
