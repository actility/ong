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

package com.actility.m2m.cm;

import org.osgi.framework.BundleContext;
import org.osgi.framework.ServiceReference;
import org.osgi.service.cm.ConfigurationEvent;
import org.osgi.service.cm.ConfigurationListener;

public final class ListenerEvent implements Runnable {
    /**
     * The bundle context
     */
    private final BundleContext context;
    private final ServiceReference[] serviceReferences;
    private final ConfigurationEvent event;

    /**
     * Construct an UpdateQueue given a BundleContext.
     *
     * @param context The BundleContext to use.
     */
    public ListenerEvent(BundleContext context, ServiceReference[] serviceReferences, ConfigurationEvent event) {
        this.context = context;
        this.serviceReferences = serviceReferences;
        this.event = event;
    }

    /**
     * Override of Thread.run().
     */
    public void run() {
        for (int i = 0; i < serviceReferences.length; ++i) {
            ConfigurationListener configurationListener = (ConfigurationListener) context.getService(serviceReferences[i]);
            if (configurationListener != null) {
                configurationListener.configurationEvent(event);
            }
            context.ungetService(serviceReferences[i]);
        }
    }
}
