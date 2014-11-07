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

package com.actility.m2m.system.version;

import java.io.Serializable;

import org.apache.log4j.Logger;

import com.actility.m2m.servlet.ServletTimer;
import com.actility.m2m.servlet.TimerListener;
import com.actility.m2m.system.log.BundleLogger;
import com.actility.m2m.system.monitoring.MonitoringManager;
import com.actility.m2m.util.log.OSGiLogger;

public final class SystemTimerListener implements TimerListener {
    private static final Logger LOG = OSGiLogger.getLogger(SystemTimerListener.class, BundleLogger.LOG);

    /**
     * This call back method is invoked by the servlet engine on timeout, for any timer of SystemVersion servlet.
     *
     * @param timer data associated to the timer.
     */
    public void timeout(ServletTimer timer) {
        Serializable info = timer.getInfo();
        if (info instanceof TimerListener) {

            if (LOG.isDebugEnabled()) {
                LOG.debug("timeout " + info);
            }

            ((TimerListener) info).timeout(timer);
        } else if (info instanceof ResourcesManager) {
            if (LOG.isDebugEnabled()) {
                LOG.debug("Timeout: SystemResourceManager to restart");
            }
            ResourcesManager resourcesManager = (ResourcesManager) info;
            if (!resourcesManager.isInitialized()) {
                resourcesManager.generate();
            }
        } else if (info instanceof MonitoringManager) {
            ((MonitoringManager) info).onRefresh();
        } else {
            LOG.error("No action associated with the timer");
        }
    }

}
