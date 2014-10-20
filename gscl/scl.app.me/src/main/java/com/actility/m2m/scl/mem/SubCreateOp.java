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
 * id $Id: SubCreateOp.java 4140 2013-01-15 14:08:57Z JReich $
 * author $Author: JReich $
 * version $Revision: 4140 $
 * lastrevision $Date: 2013-01-15 15:08:57 +0100 (Tue, 15 Jan 2013) $
 * modifiedby $LastChangedBy: JReich $
 * lastmodified $LastChangedDate: 2013-01-15 15:08:57 +0100 (Tue, 15 Jan 2013) $
 */

package com.actility.m2m.scl.mem;

import java.net.URI;

import com.actility.m2m.m2m.FilterCriteria;
import com.actility.m2m.m2m.M2MConstants;
import com.actility.m2m.m2m.M2MException;
import com.actility.m2m.m2m.StatusCode;
import com.actility.m2m.scl.model.SclManager;
import com.actility.m2m.scl.model.SubscribedResource;
import com.actility.m2m.scl.model.TransientOp;
import com.actility.m2m.scl.res.ContentInstances;
import com.actility.m2m.scl.res.Latest;
import com.actility.m2m.xo.XoObject;

public final class SubCreateOp implements TransientOp {
    public static final String LATEST_URI = M2MConstants.ATTR_LATEST + M2MConstants.URI_SEP;

    private final SclManager manager;
    private final String subsPath;
    private final String subPath;
    private final String subId;
    private final XoObject subResource;
    private final String subscribedPath;
    private final SubscribedResource subscribedResource;
    private final URI requestingEntity;
    private final FilterCriteria filterCriteria;
    private final String mediaType;

    private SubManager subManager;
    private SubsManager subsManager;

    public SubCreateOp(SclManager manager, String subsPath, String subPath, String subId, XoObject subResource,
            String subscribedPath, SubscribedResource subscribedResource, URI requestingEntity, FilterCriteria filterCriteria,
            String mediaType) {
        this.manager = manager;
        String subsPathTmp = subsPath;
        SubscribedResource subscribedResourceTmp = subscribedResource;
        if (subscribedResource.getClass() == ContentInstances.class
                && filterCriteria != null
                && (LATEST_URI.equals(filterCriteria.getAttributeAccessor()) || M2MConstants.ATTR_LATEST.equals(filterCriteria
                        .getAttributeAccessor()))) {
            subsPathTmp += M2MConstants.URI_SEP + M2MConstants.ATTR_LATEST;
            subscribedResourceTmp = Latest.getInstance();
        }
        this.subsPath = subsPathTmp;
        this.subPath = subPath;
        this.subId = subId;
        this.subResource = subResource;
        this.subscribedPath = subscribedPath;
        this.subscribedResource = subscribedResourceTmp;
        this.requestingEntity = requestingEntity;
        this.filterCriteria = filterCriteria;
        this.mediaType = mediaType;
    }

    public void prepare() throws M2MException {
        subsManager = (SubsManager) manager.getM2MContext().getAttribute(subsPath);
        // Subscription manager
        if (subsManager == null) {
            subsManager = new SubsManager(manager, subscribedResource, subsPath, subscribedPath);
            manager.getM2MContext().setAttribute(subsPath, subsManager);
        }
        subManager = new SubManager(subsManager, manager, subPath, subId, subscribedResource, subscribedPath, subResource,
                requestingEntity, filterCriteria, mediaType);
        if (!subsManager.addSubscription(subId, subManager)) {
            throw new M2MException("Too much pending subscriptions", StatusCode.STATUS_SERVICE_UNAVAILABLE);
        }
    }

    public void rollback() {
        if (subsManager != null) {
            // Subscription manager
            subsManager.removeSubscription(subId);
            if (subsManager.isEmpty()) {
                manager.getM2MContext().removeAttribute(subsPath);
            }
        }
    }

    public void postCommit() {
        if (!subManager.startExpirationTimer()) {
            subManager.retrieveAndSendNotify();
        }
    }
}
