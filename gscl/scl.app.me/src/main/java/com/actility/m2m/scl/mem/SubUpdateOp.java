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
 * id $Id: SubUpdateOp.java 4140 2013-01-15 14:08:57Z JReich $
 * author $Author: JReich $
 * version $Revision: 4140 $
 * lastrevision $Date: 2013-01-15 15:08:57 +0100 (Tue, 15 Jan 2013) $
 * modifiedby $LastChangedBy: JReich $
 * lastmodified $LastChangedDate: 2013-01-15 15:08:57 +0100 (Tue, 15 Jan 2013) $
 */

package com.actility.m2m.scl.mem;

import com.actility.m2m.m2m.M2MConstants;
import com.actility.m2m.scl.model.SclManager;
import com.actility.m2m.scl.model.TransientOp;
import com.actility.m2m.xo.XoObject;

public final class SubUpdateOp implements TransientOp {

    private final SclManager manager;
    private final String subsPath;
    private final XoObject subscription;
    private final String mediaType;

    private SubManager subManager;

    public SubUpdateOp(SclManager manager, String subsPath, XoObject subscription, String mediaType) {
        this.manager = manager;
        this.subsPath = subsPath;
        this.subscription = subscription;
        this.mediaType = mediaType;
    }

    public void prepare() {
        String subId = subscription.getStringAttribute(M2MConstants.ATTR_M2M_ID);
        subManager = ((SubsManager) manager.getM2MContext().getAttribute(subsPath)).getSubscription(subId);
    }

    public void rollback() {
        // Ignore

    }

    public void postCommit() {
        subManager.setMediaType(mediaType);
        subManager.subscriptionUpdated(subscription);
        subManager.startExpirationTimer();
    }
}
