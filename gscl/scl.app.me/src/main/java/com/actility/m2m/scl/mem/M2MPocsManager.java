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
 * id $Id: M2MPocsManager.java 3302 2012-10-10 14:19:16Z JReich $
 * author $Author: JReich $
 * version $Revision: 3302 $
 * lastrevision $Date: 2012-10-10 16:19:16 +0200 (Wed, 10 Oct 2012) $
 * modifiedby $LastChangedBy: JReich $
 * lastmodified $LastChangedDate: 2012-10-10 16:19:16 +0200 (Wed, 10 Oct 2012) $
 */

package com.actility.m2m.scl.mem;

import java.text.ParseException;

import com.actility.m2m.m2m.M2MConstants;
import com.actility.m2m.scl.model.Constants;
import com.actility.m2m.scl.model.SclManager;
import com.actility.m2m.scl.model.SclTransaction;
import com.actility.m2m.scl.res.Scl;
import com.actility.m2m.storage.StorageException;
import com.actility.m2m.xo.XoException;
import com.actility.m2m.xo.XoObject;

public final class M2MPocsManager {
    private int nbOnline;
    private int nbOffline;
    private int nbNotReachable;
    private int sclOnlineStatus;

    private final SclManager manager;
    private final String sclPath;

    public M2MPocsManager(SclManager manager, String sclPath) {
        this.manager = manager;
        this.sclPath = sclPath;
        this.sclOnlineStatus = Constants.ID_ONLINE_STATUS_OFFLINE;
        this.nbOnline = 0;
        this.nbOffline = 0;
        this.nbNotReachable = 0;
    }

    public boolean isEmpty() {
        return (nbOnline + nbOffline + nbNotReachable) == 0;
    }

    public void m2mPocOnlineStatusChanged(int previousState, int newState, SclTransaction transaction) throws ParseException,
            StorageException, XoException {
        switch (previousState) {
        case Constants.ID_ONLINE_STATUS_ONLINE:
            --nbOnline;
            break;
        case Constants.ID_ONLINE_STATUS_OFFLINE:
            --nbOffline;
            break;
        case Constants.ID_ONLINE_STATUS_NOT_REACHABLE:
            --nbNotReachable;
            break;
        default:
            // No previous state (new m2mPoc)
            break;
        }
        switch (newState) {
        case Constants.ID_ONLINE_STATUS_ONLINE:
            ++nbOnline;
            break;
        case Constants.ID_ONLINE_STATUS_OFFLINE:
            ++nbOffline;
            break;
        case Constants.ID_ONLINE_STATUS_NOT_REACHABLE:
            ++nbNotReachable;
            break;
        default:
            // No new state (m2mPoc deleted)
            break;
        }
        String onlineStatus = null;
        String serverCapable = null;
        if (nbOnline > 0) {
            if (sclOnlineStatus != Constants.ID_ONLINE_STATUS_ONLINE) {
                sclOnlineStatus = Constants.ID_ONLINE_STATUS_ONLINE;
                onlineStatus = M2MConstants.ONLINE_STATUS_ONLINE;
                serverCapable = "true";
            }
        } else if (nbNotReachable > 0 && nbOffline == 0) {
            if (sclOnlineStatus != Constants.ID_ONLINE_STATUS_NOT_REACHABLE) {
                sclOnlineStatus = Constants.ID_ONLINE_STATUS_NOT_REACHABLE;
                onlineStatus = M2MConstants.ONLINE_STATUS_NOT_REACHABLE;
                serverCapable = "true";
            }
        } else {
            if (sclOnlineStatus != Constants.ID_ONLINE_STATUS_OFFLINE) {
                sclOnlineStatus = Constants.ID_ONLINE_STATUS_OFFLINE;
                onlineStatus = M2MConstants.ONLINE_STATUS_OFFLINE;
                serverCapable = (nbNotReachable == 0 && nbOffline == 0) ? "false" : "true";
            }
        }
        if (onlineStatus != null && transaction != null) {
            XoObject scl = null;
            try {
                scl = manager.getXoResource(sclPath);
                Scl.getInstance().updateFromM2MPocs(manager, sclPath, scl, onlineStatus, serverCapable, transaction);
            } finally {
                if (scl != null) {
                    scl.free(true);
                }
            }
        }
    }
}
