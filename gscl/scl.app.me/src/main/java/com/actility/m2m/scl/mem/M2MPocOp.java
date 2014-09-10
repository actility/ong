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
 * id $Id: M2MPocOp.java 3302 2012-10-10 14:19:16Z JReich $
 * author $Author: JReich $
 * version $Revision: 3302 $
 * lastrevision $Date: 2012-10-10 16:19:16 +0200 (Wed, 10 Oct 2012) $
 * modifiedby $LastChangedBy: JReich $
 * lastmodified $LastChangedDate: 2012-10-10 16:19:16 +0200 (Wed, 10 Oct 2012) $
 */

package com.actility.m2m.scl.mem;

import java.text.ParseException;

import org.apache.log4j.Logger;

import com.actility.m2m.scl.log.BundleLogger;
import com.actility.m2m.scl.model.SclManager;
import com.actility.m2m.scl.model.SclTransaction;
import com.actility.m2m.scl.model.TransientOp;
import com.actility.m2m.storage.StorageException;
import com.actility.m2m.util.log.OSGiLogger;
import com.actility.m2m.xo.XoException;

public final class M2MPocOp implements TransientOp {
    private static final Logger LOG = OSGiLogger.getLogger(M2MPocOp.class, BundleLogger.getStaticLogger());
    private static final String ERROR_UNABLE_TO_ROLLBACK = ": Unable to rollback m2mPoc online status";

    private final String logId;
    private final SclManager manager;
    private final String m2mPocsPath;
    private final int oldOnlineStatus;
    private final int newOnlineStatus;
    private final SclTransaction transaction;
    private M2MPocsManager pocsManager;

    public M2MPocOp(String logId, SclManager manager, String m2mPocsPath, int oldOnlineStatus, int newOnlineStatus,
            SclTransaction transaction) {
        this.logId = logId;
        this.manager = manager;
        this.m2mPocsPath = m2mPocsPath;
        this.oldOnlineStatus = oldOnlineStatus;
        this.newOnlineStatus = newOnlineStatus;
        this.transaction = transaction;
    }

    public void prepare() throws ParseException, XoException, StorageException {
        pocsManager = (M2MPocsManager) manager.getM2MContext().getAttribute(m2mPocsPath);
        if (pocsManager == null) {
            String parentPath = m2mPocsPath.substring(0, m2mPocsPath.lastIndexOf('/', m2mPocsPath.length() - 1));
            pocsManager = new M2MPocsManager(manager, parentPath);
            manager.getM2MContext().setAttribute(m2mPocsPath, pocsManager);
        }
        pocsManager.m2mPocOnlineStatusChanged(oldOnlineStatus, newOnlineStatus, transaction);
    }

    public void rollback() {
        if (pocsManager != null) {
            try {
                pocsManager.m2mPocOnlineStatusChanged(newOnlineStatus, oldOnlineStatus, null);
            } catch (ParseException e) {
                LOG.error(logId + ERROR_UNABLE_TO_ROLLBACK, e);
            } catch (XoException e) {
                LOG.error(logId + ERROR_UNABLE_TO_ROLLBACK, e);
            } catch (StorageException e) {
                LOG.error(logId + ERROR_UNABLE_TO_ROLLBACK, e);
            }
            if (pocsManager.isEmpty()) {
                manager.getM2MContext().removeAttribute(m2mPocsPath);
            }
        }
    }

    public void postCommit() {
        if (pocsManager.isEmpty()) {
            manager.getM2MContext().removeAttribute(m2mPocsPath);
        }
    }
}
