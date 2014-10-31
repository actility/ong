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
 * id $Id: CisChildCreateOp.java 3302 2012-10-10 14:19:16Z JReich $
 * author $Author: JReich $
 * version $Revision: 3302 $
 * lastrevision $Date: 2012-10-10 16:19:16 +0200 (Wed, 10 Oct 2012) $
 * modifiedby $LastChangedBy: JReich $
 * lastmodified $LastChangedDate: 2012-10-10 16:19:16 +0200 (Wed, 10 Oct 2012) $
 */

package com.actility.m2m.scl.mem;

import java.util.Date;

import com.actility.m2m.m2m.M2MException;
import com.actility.m2m.scl.model.Constants;
import com.actility.m2m.scl.model.SclManager;
import com.actility.m2m.scl.model.TransientOp;

public class CisChildCreateOp implements TransientOp {

    private final SclManager manager;
    private final String path;
    private final Date now;
    private final long maxInstanceAge;
    private final Date lastCiCreationDate;
    private final String lastCiId;
    private String timerId;

    public CisChildCreateOp(SclManager manager, String path, Date lastCiCreationDate, String lastCiId, Date now,
            long maxInstanceAge) throws M2MException {
        this.manager = manager;
        this.path = path;
        this.lastCiCreationDate = lastCiCreationDate;
        this.lastCiId = lastCiId;
        this.now = now;
        this.maxInstanceAge = maxInstanceAge;
    }

    public void prepare() throws M2MException {
        String oldTimerId = (String) manager.getM2MContext().getAttribute(path + "/instanceAgeTimer");
        // TODO Should buffer next expiration for rollback !
        if (oldTimerId != null) {
            manager.getM2MContext().cancelTimer(oldTimerId);
        }
        timerId = manager.startResourceTimer(lastCiCreationDate.getTime() + maxInstanceAge - now.getTime(), path,
                Constants.ID_RES_CONTENT_INSTANCES, null);
        manager.getM2MContext().setAttribute(path + "/instanceAgeTimerId", timerId);
        manager.getM2MContext().setAttribute(path + "/instanceAgeId", lastCiId);
    }

    public void rollback() {
        if (timerId != null) {
            // Subscription manager
            manager.getM2MContext().cancelTimer(timerId);
        }
    }

    public void postCommit() {
        // Nothing to do
    }

}
