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
 * id $Id: SclConfig.java 3644 2012-11-27 16:26:04Z JReich $
 * author $Author: JReich $
 * version $Revision: 3644 $
 * lastrevision $Date: 2012-11-27 17:26:04 +0100 (Tue, 27 Nov 2012) $
 * modifiedby $LastChangedBy: JReich $
 * lastmodified $LastChangedDate: 2012-11-27 17:26:04 +0100 (Tue, 27 Nov 2012) $
 */

package com.actility.m2m.scl.model;

import java.util.Map;

import org.apache.log4j.Logger;

import com.actility.m2m.scl.log.BundleLogger;
import com.actility.m2m.util.EqualsUtils;
import com.actility.m2m.util.log.OSGiLogger;

/**
 * Configuration holder for the Service Capability Layer.
 */
public final class SclConfig {
    private static final Logger LOG = OSGiLogger.getLogger(SclConfig.class, BundleLogger.getStaticLogger());

    private int maxSubscriptions;
    private int maxSubscriptionsPerResource;
    private volatile long notifRepresentationTimer;
    private volatile long notifRequestTimeout;
    private int maxWatchQueueSize;
    private int maxInitWatches;

    private volatile long defaultExpirationDuration;
    private volatile long minExpirationDuration;
    private volatile long maxExpirationDuration;

    private volatile long defaultMaxByteSize;
    private volatile long minMaxByteSize;
    private volatile long maxMaxByteSize;

    private volatile long defaultMaxInstanceAge;
    private volatile long minMaxInstanceAge;
    private volatile long maxMaxInstanceAge;

    private volatile long defaultMaxNrOfInstances;
    private volatile long minMaxNrOfInstances;
    private volatile long maxMaxNrOfInstances;

    private volatile long registrationExpirationDuration;
    private volatile long registrationRetryDuration;

    private boolean contentInstanceCreationOnNotify;

    private final boolean gscl;
    private int aPoCHandling;

    private String discoveryAccessRightID;

    private Map reqEntityToIps;

    public SclConfig(boolean gscl, int maxSubscriptions, int maxSubscriptionsPerResource, long defaultExpirationDuration,
            long minExpirationDuration, long maxExpirationDuration, long defaultMaxByteSize, long minMaxByteSize,
            long maxMaxByteSize, long defaultMaxInstanceAge, long minMaxInstanceAge, long maxMaxInstanceAge,
            long defaultMaxNrOfInstances, long minMaxNrOfInstances, long maxMaxNrOfInstances, int maxWatchQueueSize,
            long notifRequestTimeout, long notifRepresentationTimer, int maxInitWatches, long registrationExpirationDuration,
            long registrationRetryDuration, boolean contentInstanceCreationOnNotify, String discoveryAccessRightID,
            Map reqEntityToIps) {
        this.gscl = gscl;
        this.maxSubscriptions = maxSubscriptions;
        this.maxSubscriptionsPerResource = maxSubscriptionsPerResource;
        this.defaultExpirationDuration = defaultExpirationDuration;
        this.minExpirationDuration = minExpirationDuration;
        this.maxExpirationDuration = maxExpirationDuration;
        this.defaultMaxByteSize = defaultMaxByteSize;
        this.minMaxByteSize = minMaxByteSize;
        this.maxMaxByteSize = maxMaxByteSize;
        this.defaultMaxInstanceAge = defaultMaxInstanceAge;
        this.minMaxInstanceAge = minMaxInstanceAge;
        this.maxMaxInstanceAge = maxMaxInstanceAge;
        this.defaultMaxNrOfInstances = defaultMaxNrOfInstances;
        this.maxMaxNrOfInstances = maxMaxNrOfInstances;
        this.minMaxNrOfInstances = minMaxNrOfInstances;
        this.maxWatchQueueSize = maxWatchQueueSize;
        this.notifRequestTimeout = notifRequestTimeout;
        this.notifRepresentationTimer = notifRepresentationTimer;
        this.maxInitWatches = maxInitWatches;
        this.registrationExpirationDuration = registrationExpirationDuration;
        this.registrationRetryDuration = registrationRetryDuration;
        this.contentInstanceCreationOnNotify = contentInstanceCreationOnNotify;
        this.aPoCHandling = Constants.ID_APOC_HANDLING_SHALLOW;
        this.discoveryAccessRightID = discoveryAccessRightID;
        this.reqEntityToIps = reqEntityToIps;
    }

    public boolean isGscl() {
        return gscl;
    }

    public int getMaxSubscriptions() {
        return maxSubscriptions;
    }

    public void setMaxSubscriptions(int maxSubscriptions) {
        if (maxSubscriptions != this.maxSubscriptions) {
            LOG.info("Modifying maxSubscriptions from " + this.maxSubscriptions + " to " + maxSubscriptions);
            this.maxSubscriptions = maxSubscriptions;
        }
    }

    public int getMaxSubscriptionsPerResource() {
        return maxSubscriptionsPerResource;
    }

    public void setMaxSubscriptionsPerResource(int maxSubscriptionsPerResource) {
        if (maxSubscriptionsPerResource != this.maxSubscriptionsPerResource) {
            LOG.info("Modifying maxSubscriptionsPerResource from " + this.maxSubscriptionsPerResource + " to "
                    + maxSubscriptionsPerResource);
            this.maxSubscriptionsPerResource = maxSubscriptionsPerResource;
        }
    }

    public long getDefaultExpirationDuration() {
        return defaultExpirationDuration;
    }

    public void setDefaultExpirationDuration(long defaultExpirationDuration) {
        if (defaultExpirationDuration != this.defaultExpirationDuration) {
            LOG.info("Modifying defaultExpirationDuration from " + this.defaultExpirationDuration + " to "
                    + defaultExpirationDuration);
            this.defaultExpirationDuration = defaultExpirationDuration;
        }
    }

    public long getMinExpirationDuration() {
        return minExpirationDuration;
    }

    public void setMinExpirationDuration(long minExpirationDuration) {
        if (minExpirationDuration != this.minExpirationDuration) {
            LOG.info("Modifying minExpirationDuration from " + this.minExpirationDuration + " to " + minExpirationDuration);
            this.minExpirationDuration = minExpirationDuration;
        }
    }

    public long getMaxExpirationDuration() {
        return maxExpirationDuration;
    }

    public void setMaxExpirationDuration(long maxExpirationDuration) {
        if (maxExpirationDuration != this.maxExpirationDuration) {
            LOG.info("Modifying maxExpirationDuration from " + this.maxExpirationDuration + " to " + maxExpirationDuration);
            this.maxExpirationDuration = maxExpirationDuration;
        }
    }

    public long getDefaultMaxByteSize() {
        return defaultMaxByteSize;
    }

    public void setDefaultMaxByteSize(long defaultMaxByteSize) {
        if (defaultMaxByteSize != this.defaultMaxByteSize) {
            LOG.info("Modifying defaultMaxByteSize from " + this.defaultMaxByteSize + " to " + defaultExpirationDuration);
            this.defaultMaxByteSize = defaultMaxByteSize;
        }
    }

    public long getMinMaxByteSize() {
        return minMaxByteSize;
    }

    public void setMinMaxByteSize(long minMaxByteSize) {
        if (minMaxByteSize != this.minMaxByteSize) {
            LOG.info("Modifying minMaxByteSize from " + this.minMaxByteSize + " to " + minMaxByteSize);
            this.minMaxByteSize = minMaxByteSize;
        }
    }

    public long getMaxMaxByteSize() {
        return maxMaxByteSize;
    }

    public void setMaxMaxByteSize(long maxMaxByteSize) {
        if (maxMaxByteSize != this.maxMaxByteSize) {
            LOG.info("Modifying maxMaxByteSize from " + this.maxMaxByteSize + " to " + maxMaxByteSize);
            this.maxMaxByteSize = maxMaxByteSize;
        }
    }

    public long getDefaultMaxInstanceAge() {
        return defaultMaxInstanceAge;
    }

    public void setDefaultMaxInstanceAge(long defaultMaxInstanceAge) {
        if (defaultMaxInstanceAge != this.defaultMaxInstanceAge) {
            LOG.info("Modifying defaultMaxInstanceAge from " + this.defaultMaxInstanceAge + " to " + defaultMaxInstanceAge);
            this.defaultMaxInstanceAge = defaultMaxInstanceAge;
        }
    }

    public long getMinMaxInstanceAge() {
        return minMaxInstanceAge;
    }

    public void setMinMaxInstanceAge(long minMaxInstanceAge) {
        if (minMaxInstanceAge != this.minMaxInstanceAge) {
            LOG.info("Modifying minMaxInstanceAge from " + this.minMaxInstanceAge + " to " + minMaxInstanceAge);
            this.minMaxInstanceAge = minMaxInstanceAge;
        }
    }

    public long getMaxMaxInstanceAge() {
        return maxMaxInstanceAge;
    }

    public void setMaxMaxInstanceAge(long maxMaxInstanceAge) {
        if (maxMaxInstanceAge != this.maxMaxInstanceAge) {
            LOG.info("Modifying maxMaxInstanceAge from " + this.maxMaxInstanceAge + " to " + maxMaxInstanceAge);
            this.maxMaxInstanceAge = maxMaxInstanceAge;
        }
    }

    public long getDefaultMaxNrOfInstances() {
        return defaultMaxNrOfInstances;
    }

    public void setDefaultMaxNrOfInstances(long defaultMaxNrOfInstances) {
        if (defaultMaxNrOfInstances != this.defaultMaxNrOfInstances) {
            LOG.info("Modifying defaultMaxNrOfInstances from " + this.defaultMaxNrOfInstances + " to "
                    + defaultMaxNrOfInstances);
            this.defaultMaxNrOfInstances = defaultMaxNrOfInstances;
        }
    }

    public long getMinMaxNrOfInstances() {
        return minMaxNrOfInstances;
    }

    public void setMinMaxNrOfInstances(long minMaxNrOfInstances) {
        if (minMaxNrOfInstances != this.minMaxNrOfInstances) {
            LOG.info("Modifying minMaxNrOfInstances from " + this.minMaxNrOfInstances + " to " + minMaxNrOfInstances);
            this.minMaxNrOfInstances = minMaxNrOfInstances;
        }
    }

    public long getMaxMaxNrOfInstances() {
        return maxMaxNrOfInstances;
    }

    public void setMaxMaxNrOfInstances(long maxMaxNrOfInstances) {
        if (maxMaxNrOfInstances != this.maxMaxNrOfInstances) {
            LOG.info("Modifying maxMaxNrOfInstances from " + this.maxMaxNrOfInstances + " to " + maxMaxNrOfInstances);
            this.maxMaxNrOfInstances = maxMaxNrOfInstances;
        }
    }

    public long getNotifRepresentationTimer() {
        return notifRepresentationTimer;
    }

    public void setNotifRepresentationTimer(long notifRepresentationTimer) {
        if (notifRepresentationTimer != this.notifRepresentationTimer) {
            LOG.info("Modifying notifRepresentationTimer from " + this.notifRepresentationTimer + " to "
                    + notifRepresentationTimer);
            this.notifRepresentationTimer = notifRepresentationTimer;
        }
    }

    public long getNotifRequestTimeout() {
        return notifRequestTimeout;
    }

    public void setNotifRequestTimeout(long notifRequestTimeout) {
        if (notifRequestTimeout != this.notifRequestTimeout) {
            LOG.info("Modifying notifRequestTimeout from " + this.notifRequestTimeout + " to " + notifRequestTimeout);
            this.notifRequestTimeout = notifRequestTimeout;
        }
    }

    public int getMaxWatchQueueSize() {
        return maxWatchQueueSize;
    }

    public void setMaxWatchQueueSize(int maxWatchQueueSize) {
        if (maxWatchQueueSize != this.maxWatchQueueSize) {
            LOG.info("Modifying maxWatchQueueSize from " + this.maxWatchQueueSize + " to " + maxWatchQueueSize);
            this.maxWatchQueueSize = maxWatchQueueSize;
        }
    }

    public int getMaxInitWatches() {
        return maxInitWatches;
    }

    public void setMaxInitWatches(int maxInitWatches) {
        if (maxInitWatches != this.maxInitWatches) {
            LOG.info("Modifying maxInitWatches from " + this.maxInitWatches + " to " + maxInitWatches);
            this.maxInitWatches = maxInitWatches;
        }
    }

    public int getAPoCHandling() {
        return aPoCHandling;
    }

    public void setAPoCHandling(int aPoCHandling) {
        if (aPoCHandling != this.aPoCHandling) {
            LOG.info("Modifying aPoCHandling from " + this.aPoCHandling + " to " + aPoCHandling);
            this.aPoCHandling = aPoCHandling;
        }
    }

    public long getRegistrationExpirationDuration() {
        return registrationExpirationDuration;
    }

    public void setRegistrationExpirationDuration(long registrationExpirationDuration) {
        if (registrationExpirationDuration != this.registrationExpirationDuration) {
            LOG.info("Modifying registrationExpirationDuration from " + this.registrationExpirationDuration + " to "
                    + registrationExpirationDuration);
            this.registrationExpirationDuration = registrationExpirationDuration;
        }
    }

    public long getRegistrationRetryDuration() {
        return registrationRetryDuration;
    }

    public void setRegistrationRetryDuration(long registrationRetryDuration) {
        if (registrationRetryDuration != this.registrationRetryDuration) {
            LOG.info("Modifying registrationRetryDuration from " + this.registrationRetryDuration + " to "
                    + registrationRetryDuration);
            this.registrationRetryDuration = registrationRetryDuration;
        }
    }

    public boolean isContentInstanceCreationOnNotify() {
        return contentInstanceCreationOnNotify;
    }

    public void setContentInstanceCreationOnNotify(boolean contentInstanceCreationOnNotify) {
        if (contentInstanceCreationOnNotify != this.contentInstanceCreationOnNotify) {
            LOG.info("Modifying contentInstanceCreationOnNotify from " + this.contentInstanceCreationOnNotify + " to "
                    + contentInstanceCreationOnNotify);
            this.contentInstanceCreationOnNotify = contentInstanceCreationOnNotify;
        }
    }

    public String getDiscoveryAccessRightID() {
        return discoveryAccessRightID;
    }

    public void setDiscoveryAccessRightID(String discoveryAccessRightID) {
        if (EqualsUtils.isNotEqual(discoveryAccessRightID, this.discoveryAccessRightID)) {
            LOG.info("Modifying discoveryAccessRightID from " + this.discoveryAccessRightID + " to " + discoveryAccessRightID);
            this.discoveryAccessRightID = discoveryAccessRightID;
        }
    }

    public Map getReqEntityToIps() {
        return reqEntityToIps;
    }

    public void setReqEntityToIps(Map reqEntityToIps) {
        if (EqualsUtils.isNotEqual(reqEntityToIps, this.reqEntityToIps)) {
            LOG.info("Modifying reqEntityToIps from " + this.reqEntityToIps + " to " + reqEntityToIps);
            this.reqEntityToIps = reqEntityToIps;
        }
    }
}
