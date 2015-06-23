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
 * id $Id: CisChildDeleteOp.java 3302 2012-10-10 14:19:16Z JReich $
 * author $Author: JReich $
 * version $Revision: 3302 $
 * lastrevision $Date: 2012-10-10 16:19:16 +0200 (Wed, 10 Oct 2012) $
 * modifiedby $LastChangedBy: JReich $
 * lastmodified $LastChangedDate: 2012-10-10 16:19:16 +0200 (Wed, 10 Oct 2012) $
 */

package com.actility.m2m.scl.mem;

import java.util.ArrayList;
import java.util.Date;
import java.util.Iterator;
import java.util.List;

import com.actility.m2m.m2m.M2MConstants;
import com.actility.m2m.m2m.M2MException;
import com.actility.m2m.scl.model.Constants;
import com.actility.m2m.scl.model.SclManager;
import com.actility.m2m.scl.model.TransientOp;
import com.actility.m2m.scl.res.SclResource;
import com.actility.m2m.storage.Condition;
import com.actility.m2m.storage.Document;
import com.actility.m2m.storage.SearchResult;
import com.actility.m2m.storage.StorageException;
import com.actility.m2m.storage.StorageFactory;
import com.actility.m2m.storage.StorageRequestExecutor;
import com.actility.m2m.util.EmptyUtils;
import com.actility.m2m.xo.XoException;
import com.actility.m2m.xo.XoObject;

public class CisChildDeleteOp implements TransientOp {

    private final SclManager manager;
    private final XoObject childResource;
    private final String path;
    private final Date now;
    private String timerId;

    public CisChildDeleteOp(SclManager manager, String path, XoObject childResource, Date now) {
        this.manager = manager;
        this.path = path;
        this.childResource = childResource;
        this.now = now;
    }

    public void prepare() throws StorageException, XoException, M2MException {
        String lastCiId = (String) manager.getM2MContext().getAttribute(path + Constants.AT_INSTANCE_AGE_ID_SUFFIX);
        if (lastCiId != null && lastCiId.equals(childResource.getStringAttribute(M2MConstants.ATTR_M2M_ID))) {
            String oldTimerId = (String) manager.getM2MContext().getAttribute(path + Constants.AT_INSTANCE_AGE_TIMER_ID_SUFFIX);
            manager.getM2MContext().cancelTimer(oldTimerId);
            Date childCreationTime = SclResource.getAndCheckDateTime(childResource, M2MConstants.TAG_M2M_CREATION_TIME,
                    Constants.ID_MODE_REQUIRED, -1L);
            XoObject parentResource = null;
            XoObject lastCi = null;
            StorageFactory storageFactory = manager.getStorageContext().getStorageFactory();
            List/* <Condition> */conditions = new ArrayList/* <Condition> */(2);
            conditions.add(storageFactory.createStringCondition(Constants.ATTR_TYPE, Condition.ATTR_OP_EQUAL,
                    Constants.TYPE_CONTENT_INSTANCE));
            conditions.add(storageFactory.createDateCondition(M2MConstants.ATTR_CREATION_TIME,
                    Condition.ATTR_OP_STRICTLY_GREATER, childCreationTime));
            Condition condition = storageFactory.createConjunction(Condition.COND_OP_AND, conditions);
            SearchResult searchResult = manager.getStorageContext().search(null, path, StorageRequestExecutor.SCOPE_ONE_LEVEL,
                    condition, StorageRequestExecutor.ORDER_ASC, 1, true, EmptyUtils.EMPTY_LIST);
            Iterator it = searchResult.getResults();
            Document document = null;
            if (it.hasNext()) {
                try {
                    document = (Document) it.next();
                    lastCi = manager.getXoService().readBinaryXmlXoObject(document.getContent());
                    parentResource = manager.getXoResource(SclResource.getParentPath(path));

                    // Check conditions
                    long maxInstanceAge = SclResource.getAndCheckDuration(parentResource,
                            M2MConstants.TAG_M2M_MAX_INSTANCE_AGE, Constants.ID_MODE_OPTIONAL);
                    Date lastCiCreationTime = SclResource.getAndCheckDateTime(lastCi, M2MConstants.TAG_M2M_CREATION_TIME,
                            Constants.ID_MODE_REQUIRED, -1L);
                    timerId = manager.startResourceTimer(lastCiCreationTime.getTime() + maxInstanceAge - now.getTime(), path,
                            Constants.ID_RES_CONTENT_INSTANCES, null);
                    manager.getM2MContext().setAttribute(path + Constants.AT_INSTANCE_AGE_TIMER_ID_SUFFIX, timerId);
                    manager.getM2MContext().setAttribute(path + Constants.AT_INSTANCE_AGE_ID_SUFFIX,
                            lastCi.getStringAttribute(M2MConstants.ATTR_M2M_ID));
                } finally {
                    if (lastCi != null) {
                        lastCi.free(true);
                    }
                    if (parentResource != null) {
                        parentResource.free(true);
                    }
                }
            }
        }
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
