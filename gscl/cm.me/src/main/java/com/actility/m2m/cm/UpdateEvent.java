/*******************************************************************************
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
 *******************************************************************************/

package com.actility.m2m.cm;

import java.util.Dictionary;

import org.apache.log4j.Logger;
import org.osgi.framework.BundleContext;
import org.osgi.framework.ServiceReference;
import org.osgi.service.cm.ConfigurationException;
import org.osgi.service.cm.ManagedService;
import org.osgi.service.cm.ManagedServiceFactory;

import com.actility.m2m.cm.log.BundleLogger;
import com.actility.m2m.util.log.OSGiLogger;

public final class UpdateEvent implements Runnable {
    private static final Logger LOG = OSGiLogger.getLogger(UpdateEvent.class, BundleLogger.getStaticLogger());

    private final ConfigurationAdminManager manager;
    private final BundleContext context;
    private final ServiceReference serviceReference;
    private final String servicePid;
    private final String factoryPid;
    private final ConfigurationDictionary configuration;

    public UpdateEvent(ConfigurationAdminManager manager, BundleContext context, ServiceReference serviceReference,
            String servicePid, String factoryPid, ConfigurationDictionary configuration) {
        this.manager = manager;
        this.context = context;
        this.serviceReference = serviceReference;
        this.servicePid = servicePid;
        this.factoryPid = factoryPid;
        this.configuration = configuration;
    }

    public void run() {
        try {
            Object targetService = context.getService(serviceReference);
            if (targetService != null) {
                ConfigurationDictionary processedConfiguration = manager.callPlugins(serviceReference, servicePid,
                        configuration);
                if (factoryPid == null) {
                    update((ManagedService) targetService, processedConfiguration);
                } else {
                    update((ManagedServiceFactory) targetService, processedConfiguration);
                }
            }
            context.ungetService(serviceReference);
        } catch (ConfigurationException e) {
            LOG.error("Cannot update configuration of target service", e);
        }
    }

    private void update(ManagedService targetService, Dictionary dictionary) throws ConfigurationException {
        targetService.updated(dictionary);
    }

    private void update(ManagedServiceFactory targetService, Dictionary dictionary) throws ConfigurationException {
        if (configuration == null) {
            targetService.deleted(servicePid);
        } else if (dictionary == null) {
            targetService.deleted(servicePid);
        } else {
            targetService.updated(servicePid, configuration);
        }
    }
}
