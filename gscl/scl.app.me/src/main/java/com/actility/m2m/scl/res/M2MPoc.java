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
 * id $Id: M2MPoc.java 3302 2012-10-10 14:19:16Z JReich $
 * author $Author: JReich $
 * version $Revision: 3302 $
 * lastrevision $Date: 2012-10-10 16:19:16 +0200 (Wed, 10 Oct 2012) $
 * modifiedby $LastChangedBy: JReich $
 * lastmodified $LastChangedDate: 2012-10-10 16:19:16 +0200 (Wed, 10 Oct 2012) $
 */

package com.actility.m2m.scl.res;

import java.io.IOException;
import java.io.Serializable;
import java.io.UnsupportedEncodingException;
import java.net.URI;
import java.text.ParseException;
import java.util.ArrayList;
import java.util.Collection;
import java.util.Date;

import org.apache.log4j.Logger;

import com.actility.m2m.m2m.Indication;
import com.actility.m2m.m2m.M2MConstants;
import com.actility.m2m.m2m.M2MException;
import com.actility.m2m.m2m.M2MSession;
import com.actility.m2m.scl.log.BundleLogger;
import com.actility.m2m.scl.mem.ExpirationTimerDeleteOp;
import com.actility.m2m.scl.mem.ExpirationTimerUpdateOp;
import com.actility.m2m.scl.mem.M2MPocOp;
import com.actility.m2m.scl.mem.M2MPocsManager;
import com.actility.m2m.scl.model.Constants;
import com.actility.m2m.scl.model.SclManager;
import com.actility.m2m.scl.model.SclTransaction;
import com.actility.m2m.scl.model.VolatileResource;
import com.actility.m2m.storage.StorageException;
import com.actility.m2m.util.FormatUtils;
import com.actility.m2m.util.Pair;
import com.actility.m2m.util.log.OSGiLogger;
import com.actility.m2m.xo.XoException;
import com.actility.m2m.xo.XoObject;

public final class M2MPoc extends SclResource implements VolatileResource {
    private static final Logger LOG = OSGiLogger.getLogger(M2MPoc.class, BundleLogger.getStaticLogger());

    private static final M2MPoc INSTANCE = new M2MPoc();

    public static M2MPoc getInstance() {
        return INSTANCE;
    }

    private M2MPoc() {
        // Cannot be instantiated
        super(false, "m2mPocRetrieveRequestIndication", "m2mPocRetrieveResponseConfirm", "m2mPocUpdateRequestIndication",
                "m2mPocUpdateResponseConfirm", "m2mPocDeleteRequestIndication", "m2mPocDeleteResponseConfirm",
                Constants.ID_NO_FILTER_CRITERIA_MODE, true, false, M2MPocs.getInstance(), false, false);
    }

    // m2m:M2MPoc from ong:t_xml_obj
    // {
    // m2m:id XoString { embattr } // (optional) (xmlType: xsd:anyURI)
    // m2m:contactInfo m2m:ContactInfo { } // (optional)
    // m2m:expirationTime XoString { } // (optional) (xmlType: xsd:dateTime)
    // m2m:onlineStatus XoString { } // (optional) (xmlType: m2m:OnlineStatus) (enum: ONLINE OFFLINE NOT_REACHABLE )
    // m2m:creationTime XoString { } // (optional) (xmlType: xsd:dateTime)
    // m2m:lastModifiedTime XoString { } // (optional) (xmlType: xsd:dateTime)
    // }
    // alias m2m:M2MPoc with m2m:m2mPoc

    public void reload(SclManager manager, String path, XoObject resource, SclTransaction transaction) throws IOException,
            ParseException, StorageException, XoException, M2MException {
        if (LOG.isInfoEnabled()) {
            LOG.info("Reload volatile <m2mPoc>: " + path);
        }
        long now = System.currentTimeMillis();
        Date expirationTime = getAndCheckDateTime(resource, M2MConstants.TAG_M2M_EXPIRATION_TIME, Constants.ID_MODE_OPTIONAL,
                -1L);
        if (expirationTime != null && expirationTime.getTime() > 0) {
            if (expirationTime.getTime() > now) {
                if (expirationTime.getTime() < FormatUtils.MAX_DATE) {
                    transaction.addTransientOp(new ExpirationTimerUpdateOp(manager, path, Constants.ID_RES_M2M_POC,
                            expirationTime.getTime() - now));
                }
                // else infinite timer
            } else {
                // delete resource, it is already expired
                deleteResource("restore", manager, path, transaction);
                // TODO remove the return
                return;
            }
        }

        String m2mPocsPath = getParentPath(path);
        M2MPocsManager pocsManager = (M2MPocsManager) manager.getM2MContext().getAttribute(m2mPocsPath);
        if (pocsManager == null) {
            String parentPath = getParentPath(m2mPocsPath);
            pocsManager = new M2MPocsManager(manager, parentPath);
            manager.getM2MContext().setAttribute(m2mPocsPath, pocsManager);
        }
        int newOnlineStatus = getAndCheckOnlineStatus(resource, M2MConstants.TAG_M2M_ONLINE_STATUS, Constants.ID_MODE_REQUIRED);
        pocsManager.m2mPocOnlineStatusChanged(Constants.ID_ENUM_UNDEFINED_VALUE, newOnlineStatus, transaction);
    }

    public void checkRights(String logId, SclManager manager, String path, XoObject resource, URI requestingEntity, String flag)
            throws M2MException {
        checkDefaultAccessRight(logId, manager, path, requestingEntity);
    }

    public boolean createResource(SclManager manager, String path, XoObject resource, Indication indication, String parentPath,
            String id, Date creationDate, String creationTime, XoObject representation, SclTransaction transaction)
            throws XoException, M2MException {
        boolean modified = false;
        // id: O (response M*)
        // contactInfo: M (response M)
        // expirationTime: O (response M*)
        // onlineStatus: O (response M*)
        // creationTime: NP (response M)
        // lastModifiedTime: NP (response M)

        // Check representation
        checkRepresentation(representation, M2MConstants.TAG_M2M_M2M_POC);
        getAndCheckContactInfo(representation, M2MConstants.TAG_M2M_CONTACT_INFO, Constants.ID_MODE_REQUIRED);
        Date recvExpirationTime = getAndCheckDateTime(representation, M2MConstants.TAG_M2M_EXPIRATION_TIME,
                Constants.ID_MODE_OPTIONAL, creationDate.getTime());
        int newOnlineStatus = getAndCheckOnlineStatus(representation, M2MConstants.TAG_M2M_ONLINE_STATUS,
                Constants.ID_MODE_OPTIONAL);
        if (newOnlineStatus == Constants.ID_ENUM_UNDEFINED_VALUE) {
            newOnlineStatus = Constants.ID_ONLINE_STATUS_ONLINE;
        }
        getAndCheckStringMode(representation, M2MConstants.TAG_M2M_CREATION_TIME, Constants.ID_MODE_FORBIDDEN);

        // Update resource
        resource.setNameSpace(M2MConstants.PREFIX_M2M);
        resource.setStringAttribute(M2MConstants.ATTR_M2M_ID, id);
        createXoObjectMandatory(manager, resource, representation, M2MConstants.TAG_M2M_CONTACT_INFO);
        Date expirationTime = setExpirationTime(manager, resource, creationDate.getTime(), recvExpirationTime);
        if (recvExpirationTime != expirationTime) {
            modified = true;
        }
        // TODO M* ?
        createStringMandatory(resource, representation, M2MConstants.TAG_M2M_ONLINE_STATUS, M2MConstants.ONLINE_STATUS_ONLINE);
        resource.setStringAttribute(M2MConstants.TAG_M2M_CREATION_TIME, creationTime);
        resource.setStringAttribute(M2MConstants.TAG_M2M_LAST_MODIFIED_TIME, creationTime);

        // Save resource
        Collection searchAttributes = new ArrayList();
        searchAttributes.add(new Pair(M2MConstants.ATTR_ONLINE_STATUS, new Integer(newOnlineStatus)));
        searchAttributes.add(new Pair(Constants.ATTR_TYPE, Constants.TYPE_M2M_POC));
        searchAttributes.add(new Pair(M2MConstants.ATTR_CREATION_TIME, creationDate));
        searchAttributes.add(new Pair(M2MConstants.ATTR_LAST_MODIFIED_TIME, creationDate));
        transaction.createResource(path, resource, searchAttributes);
        transaction.addTransientOp(new M2MPocOp(indication.getTransactionId(), manager, parentPath,
                Constants.ID_ENUM_UNDEFINED_VALUE, newOnlineStatus, transaction));
        transaction.addTransientOp(new ExpirationTimerUpdateOp(manager, path, Constants.ID_RES_M2M_POC, expirationTime
                .getTime() - creationDate.getTime()));

        return modified;
    }

    public boolean updateResource(String logId, SclManager manager, String path, XoObject resource, Indication indication,
            XoObject representation, Date now) throws ParseException, UnsupportedEncodingException, XoException,
            StorageException, M2MException {
        boolean modified = false;
        // id: NP (response M*)
        // contactInfo: M (response M)
        // expirationTime: O (response M*)
        // onlineStatus: O (response M*)
        // creationTime: NP (response M)
        // lastModifiedTime: NP (response M)

        // Check representation
        checkRepresentation(representation, M2MConstants.TAG_M2M_M2M_POC);
        getAndCheckStringMode(representation, M2MConstants.ATTR_M2M_ID, Constants.ID_MODE_FORBIDDEN);
        getAndCheckContactInfo(representation, M2MConstants.TAG_M2M_CONTACT_INFO, Constants.ID_MODE_REQUIRED);
        Date recvExpirationTime = getAndCheckDateTime(representation, M2MConstants.TAG_M2M_EXPIRATION_TIME,
                Constants.ID_MODE_OPTIONAL, now.getTime());
        int newOnlineStatus = getAndCheckOnlineStatus(representation, M2MConstants.TAG_M2M_ONLINE_STATUS,
                Constants.ID_MODE_OPTIONAL);
        if (newOnlineStatus == Constants.ID_ENUM_UNDEFINED_VALUE) {
            newOnlineStatus = Constants.ID_ONLINE_STATUS_ONLINE;
        }
        getAndCheckStringMode(representation, M2MConstants.TAG_M2M_CREATION_TIME, Constants.ID_MODE_FORBIDDEN);

        // Update resource
        updateXoObjectMandatory(manager, resource, representation, M2MConstants.TAG_M2M_CONTACT_INFO);
        Date expirationTime = setExpirationTime(manager, resource, now.getTime(), recvExpirationTime);
        if (recvExpirationTime != expirationTime) {
            modified = true;
        }
        // TODO M* ?
        int oldOnlineStatus = getAndCheckOnlineStatus(resource, M2MConstants.TAG_M2M_ONLINE_STATUS, Constants.ID_MODE_REQUIRED);
        updateStringMandatory(resource, representation, M2MConstants.TAG_M2M_ONLINE_STATUS, M2MConstants.ONLINE_STATUS_ONLINE);
        updateLastModifiedTime(manager, resource, now);

        // Save resource
        SclTransaction transaction = new SclTransaction(manager.getStorageContext());
        Collection searchAttributes = new ArrayList();
        searchAttributes.add(new Pair(M2MConstants.ATTR_ONLINE_STATUS, new Integer(newOnlineStatus)));
        searchAttributes.add(new Pair(Constants.ATTR_TYPE, Constants.TYPE_M2M_POC));
        transaction.updateResource(path, resource, searchAttributes);
        transaction.addTransientOp(new M2MPocOp(indication.getTransactionId(), manager, getParentPath(path), oldOnlineStatus,
                newOnlineStatus, transaction));
        transaction.addTransientOp(new ExpirationTimerUpdateOp(manager, path, Constants.ID_RES_M2M_POC, expirationTime
                .getTime() - now.getTime()));
        transaction.execute();

        return modified;
    }

    public void deleteResource(String logId, SclManager manager, String path, SclTransaction transaction)
            throws UnsupportedEncodingException, XoException, StorageException, M2MException {
        if (LOG.isInfoEnabled()) {
            LOG.info("Delete <m2mPoc> resource on path: " + path);
        }
        XoObject resource = manager.getXoResource(path);
        try {
            deleteResource(logId, manager, path, resource, transaction);
        } finally {
            resource.free(true);
        }
    }

    public void deleteResource(String logId, SclManager manager, String path, XoObject resource, SclTransaction transaction)
            throws M2MException {
        transaction.deleteResource(path);
        int oldOnlineStatus = getAndCheckOnlineStatus(resource, M2MConstants.TAG_M2M_ONLINE_STATUS, Constants.ID_MODE_REQUIRED);
        transaction.addTransientOp(new M2MPocOp(logId, manager, getParentPath(path), oldOnlineStatus,
                Constants.ID_ENUM_UNDEFINED_VALUE, transaction));
        transaction.addTransientOp(new ExpirationTimerDeleteOp(manager, path));
    }

    public void deleteChildResource(String logId, SclManager manager, String path, XoObject resource, XoObject childResource,
            Date now, SclTransaction transaction) {
        throw new UnsupportedOperationException();
    }

    public void timeout(SclManager manager, String path, String timerId, M2MSession session, Serializable info)
            throws ParseException, IOException, StorageException, XoException, M2MException {
        SclTransaction transaction = new SclTransaction(manager.getStorageContext());
        XoObject resource = manager.getXoResource(path);
        try {
            deleteResource("expiration", manager, path, resource, transaction);
            transaction.execute();
        } finally {
            resource.free(true);
        }
    }
}
