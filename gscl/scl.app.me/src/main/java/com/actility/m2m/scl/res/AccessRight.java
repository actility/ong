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
 * id $Id: AccessRight.java 3302 2012-10-10 14:19:16Z JReich $
 * author $Author: JReich $
 * version $Revision: 3302 $
 * lastrevision $Date: 2012-10-10 16:19:16 +0200 (Wed, 10 Oct 2012) $
 * modifiedby $LastChangedBy: JReich $
 * lastmodified $LastChangedDate: 2012-10-10 16:19:16 +0200 (Wed, 10 Oct 2012) $
 */

package com.actility.m2m.scl.res;

import java.io.IOException;
import java.io.Serializable;
import java.net.URI;
import java.text.ParseException;
import java.util.ArrayList;
import java.util.Collection;
import java.util.Date;
import java.util.Iterator;
import java.util.List;
import java.util.Set;

import org.apache.log4j.Logger;

import com.actility.m2m.m2m.FilterCriteria;
import com.actility.m2m.m2m.Indication;
import com.actility.m2m.m2m.M2MConstants;
import com.actility.m2m.m2m.M2MException;
import com.actility.m2m.m2m.M2MSession;
import com.actility.m2m.scl.log.BundleLogger;
import com.actility.m2m.scl.mem.ExpirationTimerDeleteOp;
import com.actility.m2m.scl.mem.ExpirationTimerUpdateOp;
import com.actility.m2m.scl.model.Constants;
import com.actility.m2m.scl.model.PartialAccessor;
import com.actility.m2m.scl.model.SclManager;
import com.actility.m2m.scl.model.SclTransaction;
import com.actility.m2m.scl.model.SubscribedResource;
import com.actility.m2m.scl.model.VolatileResource;
import com.actility.m2m.storage.StorageException;
import com.actility.m2m.util.EmptyUtils;
import com.actility.m2m.util.FormatUtils;
import com.actility.m2m.util.Pair;
import com.actility.m2m.util.URIUtils;
import com.actility.m2m.util.log.OSGiLogger;
import com.actility.m2m.xo.XoException;
import com.actility.m2m.xo.XoObject;

/**
 * M2M Access Right. Defines the permissions applied to a resource.
 */
public final class AccessRight extends SclResource implements VolatileResource, SubscribedResource {
    private static final Logger LOG = OSGiLogger.getLogger(AccessRight.class, BundleLogger.getStaticLogger());

    private static final AccessRight INSTANCE = new AccessRight();

    public static AccessRight getInstance() {
        return INSTANCE;
    }

    private AccessRight() {
        // Cannot be instantiated
        super(false, "accessRightRetrieveRequestIndication", "accessRightRetrieveResponseConfirm",
                "accessRightUpdateRequestIndication", "accessRightUpdateResponseConfirm", "accessRightDeleteRequestIndication",
                "accessRightDeleteResponseConfirm", Constants.ID_NO_FILTER_CRITERIA_MODE, true, true, AccessRights
                        .getInstance(), true, false);
    }

    // m2m:AccessRight from ong:t_xml_obj
    // {
    // m2m:id XoString { embattr } // (optional) (xmlType: xsd:anyURI)
    // m2m:expirationTime XoString { } // (optional) (xmlType: xsd:dateTime)
    // m2m:searchStrings m2m:SearchStrings { } // (optional)
    // m2m:creationTime XoString { } // (optional) (xmlType: xsd:dateTime)
    // m2m:lastModifiedTime XoString { } // (optional) (xmlType: xsd:dateTime)
    // m2m:announceTo m2m:AnnounceTo { } // (optional)
    // m2m:permissions m2m:PermissionListType { } // (optional)
    // m2m:selfPermissions m2m:PermissionListType { }
    // m2m:subscriptionsReference XoString { } // (optional) (xmlType: xsd:anyURI)
    // }
    // alias m2m:AccessRight with m2m:accessRight

    public void reload(SclManager manager, String path, XoObject resource, SclTransaction transaction) throws IOException,
            M2MException, StorageException, XoException {
        if (LOG.isInfoEnabled()) {
            LOG.info("Reload volatile <accessRight>: " + path);
        }
        long now = System.currentTimeMillis();
        Date expirationTime = getAndCheckDateTime(resource, M2MConstants.TAG_M2M_EXPIRATION_TIME, Constants.ID_MODE_OPTIONAL,
                -1L);
        if (expirationTime != null && expirationTime.getTime() > 0) {
            if (expirationTime.getTime() > now) {
                if (expirationTime.getTime() < FormatUtils.MAX_DATE) {
                    transaction.addTransientOp(new ExpirationTimerUpdateOp(manager, path, Constants.ID_RES_ACCESS_RIGHT,
                            expirationTime.getTime() - now));
                }
                // else infinite timer
            } else {
                // delete resource, it is already expired
                deleteResource("restore", manager, path, transaction);
            }
        }
    }

    public void checkRights(String logId, SclManager manager, String path, XoObject resource, URI requestingEntity, String flag)
            throws M2MException {
        checkPermissions(logId, manager, requestingEntity, flag,
                resource.getXoObjectAttribute(M2MConstants.TAG_M2M_SELF_PERMISSIONS));
    }

    public boolean createResource(SclManager manager, String path, XoObject resource, URI requestingEntity, String id,
            Date creationDate, String creationTime, XoObject representation, SclTransaction transaction) throws XoException,
            M2MException {
        // id: O (response M*)
        // expirationTime: O (response M*)
        // searchStrings: O (response M)
        // creationTime: NP (response M)
        // lastModifiedTime: NP (response M)
        // announceTo: O (response M*)
        // permissions; O (response M)
        // selfPermissions: O (response M)
        // subscriptionsReference: NP (response M)

        // Check representation
        checkRepresentation(representation, M2MConstants.TAG_M2M_ACCESS_RIGHT);
        Date recvExpirationTime = getAndCheckDateTime(representation, M2MConstants.TAG_M2M_EXPIRATION_TIME,
                Constants.ID_MODE_OPTIONAL, creationDate.getTime());
        getAndCheckStringMode(representation, M2MConstants.TAG_M2M_CREATION_TIME, Constants.ID_MODE_FORBIDDEN);
        getAndCheckPermissionListType(representation, M2MConstants.TAG_M2M_PERMISSIONS, Constants.ID_MODE_OPTIONAL);
        getAndCheckPermissionListType(representation, M2MConstants.TAG_M2M_SELF_PERMISSIONS, Constants.ID_MODE_OPTIONAL);
        getAndCheckStringMode(representation, M2MConstants.TAG_M2M_SUBSCRIPTIONS_REFERENCE, Constants.ID_MODE_FORBIDDEN);

        boolean modified = false;

        // Update resource
        resource.setNameSpace(M2MConstants.PREFIX_M2M);
        resource.setStringAttribute(M2MConstants.ATTR_M2M_ID, id);
        Date expirationTime = setExpirationTime(manager, resource, creationDate.getTime(), recvExpirationTime);
        if (recvExpirationTime != expirationTime) {
            modified = true;
        }
        createXoObjectMandatory(manager, resource, representation, M2MConstants.TAG_M2M_SEARCH_STRINGS);
        resource.setStringAttribute(M2MConstants.TAG_M2M_CREATION_TIME, creationTime);
        resource.setStringAttribute(M2MConstants.TAG_M2M_LAST_MODIFIED_TIME, creationTime);
        if (representation.getXoObjectAttribute(M2MConstants.TAG_M2M_ANNOUNCE_TO) != null) {
            modified = true;
        }
        createXoObjectMandatory(manager, resource, representation, M2MConstants.TAG_M2M_PERMISSIONS);
        createXoObjectMandatory(manager, resource, representation, M2MConstants.TAG_M2M_SELF_PERMISSIONS);

        // Create sub-resources
        Subscriptions.getInstance().createResource(manager, path + M2MConstants.URI_SEP + M2MConstants.RES_SUBSCRIPTIONS,
                transaction);

        // Save resource
        Collection searchAttributes = new ArrayList();
        searchAttributes.add(new Pair(Constants.ATTR_TYPE, Constants.TYPE_ACCESS_RIGHT));
        XoObject searchStrings = resource.getXoObjectAttribute(M2MConstants.TAG_M2M_SEARCH_STRINGS);
        List searchStringList = searchStrings.getStringListAttribute(M2MConstants.TAG_M2M_SEARCH_STRING);
        Iterator it = searchStringList.iterator();
        while (it.hasNext()) {
            searchAttributes.add(new Pair(M2MConstants.ATTR_SEARCH_STRING, it.next()));
        }
        searchAttributes.add(new Pair(M2MConstants.ATTR_CREATION_TIME, creationDate));
        searchAttributes.add(new Pair(M2MConstants.ATTR_LAST_MODIFIED_TIME, creationDate));
        transaction.createResource(path, resource, searchAttributes);

        transaction.addTransientOp(new ExpirationTimerUpdateOp(manager, path, Constants.ID_RES_ACCESS_RIGHT, expirationTime
                .getTime() - creationDate.getTime()));

        return modified;
    }

    public boolean updateResource(String logId, SclManager manager, String path, XoObject resource, Indication indication,
            XoObject representation, Date now) throws ParseException, XoException, StorageException, M2MException {
        boolean modified = false;
        // id: NP (response M*)
        // expirationTime: O (response M*)
        // searchStrings: O (response M)
        // creationTime: NP (response M)
        // lastModifiedTime: NP (response M)
        // announceTo: O (response M*)
        // permissions; O (response M)
        // selfPermissions: O (response M)
        // subscriptionsReference: NP (response M)

        // Check representation
        checkRepresentation(representation, M2MConstants.TAG_M2M_ACCESS_RIGHT);
        getAndCheckStringMode(representation, M2MConstants.ATTR_M2M_ID, Constants.ID_MODE_FORBIDDEN);
        Date recvExpirationTime = getAndCheckDateTime(representation, M2MConstants.TAG_M2M_EXPIRATION_TIME,
                Constants.ID_MODE_OPTIONAL, now.getTime());
        getAndCheckStringMode(representation, M2MConstants.TAG_M2M_CREATION_TIME, Constants.ID_MODE_FORBIDDEN);
        getAndCheckPermissionListType(representation, M2MConstants.TAG_M2M_PERMISSIONS, Constants.ID_MODE_OPTIONAL);
        getAndCheckPermissionListType(representation, M2MConstants.TAG_M2M_SELF_PERMISSIONS, Constants.ID_MODE_OPTIONAL);
        getAndCheckStringMode(representation, M2MConstants.TAG_M2M_SUBSCRIPTIONS_REFERENCE, Constants.ID_MODE_FORBIDDEN);

        // Update resource
        Date expirationTime = setExpirationTime(manager, resource, now.getTime(), recvExpirationTime);
        if (recvExpirationTime != expirationTime) {
            modified = true;
        }
        updateXoObjectMandatory(manager, resource, representation, M2MConstants.TAG_M2M_SEARCH_STRINGS);
        updateLastModifiedTime(manager, resource, now);
        if (representation.getXoObjectAttribute(M2MConstants.TAG_M2M_ANNOUNCE_TO) != null) {
            modified = true;
        }
        updateXoObjectMandatory(manager, resource, representation, M2MConstants.TAG_M2M_PERMISSIONS);
        updateXoObjectMandatory(manager, resource, representation, M2MConstants.TAG_M2M_SELF_PERMISSIONS);

        // Save resource
        Collection searchAttributes = new ArrayList();
        searchAttributes.add(new Pair(Constants.ATTR_TYPE, Constants.TYPE_ACCESS_RIGHT));
        XoObject searchStrings = resource.getXoObjectAttribute(M2MConstants.TAG_M2M_SEARCH_STRINGS);
        List searchStringList = searchStrings.getStringListAttribute(M2MConstants.TAG_M2M_SEARCH_STRING);
        Iterator it = searchStringList.iterator();
        while (it.hasNext()) {
            searchAttributes.add(new Pair(M2MConstants.ATTR_SEARCH_STRING, it.next()));
        }
        searchAttributes.add(new Pair(M2MConstants.ATTR_CREATION_TIME, FormatUtils.parseDateTime(resource
                .getStringAttribute(M2MConstants.TAG_M2M_CREATION_TIME))));
        searchAttributes.add(new Pair(M2MConstants.ATTR_LAST_MODIFIED_TIME, now));
        manager.getStorageContext().update(path, resource.saveBinary(), searchAttributes);

        new ExpirationTimerUpdateOp(manager, path, Constants.ID_RES_ACCESS_RIGHT, expirationTime.getTime() - now.getTime())
                .postCommit();

        return modified;
    }

    public void deleteResource(String logId, SclManager manager, String path, SclTransaction transaction)
            throws StorageException {
        if (LOG.isInfoEnabled()) {
            LOG.info("Delete <accessRight> resource on path: " + path);
        }
        Subscriptions.getInstance().deleteResource(logId, manager,
                path + M2MConstants.URI_SEP + M2MConstants.RES_SUBSCRIPTIONS, transaction);

        transaction.deleteResource(path);
    }

    public void deleteResource(String logId, SclManager manager, String path, XoObject resource, SclTransaction transaction)
            throws StorageException {
        deleteResource(logId, manager, path, transaction);
        transaction.addTransientOp(new ExpirationTimerDeleteOp(manager, path));
    }

    public void deleteChildResource(String logId, SclManager manager, String path, XoObject resource, XoObject childResource,
            Date now, SclTransaction transaction) {
        throw new UnsupportedOperationException();
    }

    public void prepareResourceForResponse(String logId, SclManager manager, String path, XoObject resource,
            URI requestingEntity, FilterCriteria filterCriteria, Set supported) {
        String appPath = manager.getM2MContext().getApplicationPath();
        resource.setStringAttribute(M2MConstants.TAG_M2M_SUBSCRIPTIONS_REFERENCE, appPath + URIUtils.encodePath(path)
                + M2MConstants.URI_SEP + M2MConstants.RES_SUBSCRIPTIONS + M2MConstants.URI_SEP);
    }

    public void doCreateIndication(SclManager manager, String path, XoObject resource, Indication indication,
            PartialAccessor partialAccessor) throws ParseException, IOException, StorageException, XoException, M2MException {
        switch (partialAccessor.getAttributeId()) {
        case Constants.ID_ATTR_EXPIRATION_TIME:
            super.doCreateIndication(manager, path, resource, indication, partialAccessor);
            break;
        case Constants.ID_ATTR_SEARCH_STRINGS:
            super.doCreateIndication(manager, path, resource, indication, partialAccessor);
            break;
        case Constants.ID_ATTR_CREATION_TIME:
            super.doCreateIndication(manager, path, resource, indication, partialAccessor);
            break;
        case Constants.ID_ATTR_LAST_MODIFIED_TIME:
            super.doCreateIndication(manager, path, resource, indication, partialAccessor);
            break;
        case Constants.ID_ATTR_ANNOUNCE_TO:
            super.doCreateIndication(manager, path, resource, indication, partialAccessor);
            break;
        case Constants.ID_ATTR_PERMISSIONS:
            List subPath = partialAccessor.getSubPath();
            Iterator it = (subPath != null) ? subPath.iterator() : EmptyUtils.EMPTY_ITERATOR;
            getAndUpdatePermissionListType(manager, path, resource, resource, indication, partialAccessor, it,
                    Constants.ID_MD_CREATE, true, M2MConstants.TAG_M2M_PERMISSIONS, Constants.ID_MODE_REQUIRED);
            break;
        case Constants.ID_ATTR_SELF_PERMISSIONS:
            break;
        case Constants.ID_ATTR_SUBSCRIPTIONS_REFERENCE:
            super.doCreateIndication(manager, path, resource, indication, partialAccessor);
            break;
        default:
            // TODO should respond 404 Not Found
            super.doCreateIndication(manager, path, resource, indication, partialAccessor);
            break;
        }
    }

    public void doDeleteIndication(SclManager manager, String path, XoObject resource, Indication indication,
            PartialAccessor partialAccessor) throws ParseException, IOException, StorageException, XoException, M2MException {
        switch (partialAccessor.getAttributeId()) {
        case Constants.ID_ATTR_EXPIRATION_TIME:
            super.doDeleteIndication(manager, path, resource, indication, partialAccessor);
            break;
        case Constants.ID_ATTR_SEARCH_STRINGS:
            super.doDeleteIndication(manager, path, resource, indication, partialAccessor);
            break;
        case Constants.ID_ATTR_CREATION_TIME:
            super.doDeleteIndication(manager, path, resource, indication, partialAccessor);
            break;
        case Constants.ID_ATTR_LAST_MODIFIED_TIME:
            super.doDeleteIndication(manager, path, resource, indication, partialAccessor);
            break;
        case Constants.ID_ATTR_ANNOUNCE_TO:
            super.doDeleteIndication(manager, path, resource, indication, partialAccessor);
            break;
        case Constants.ID_ATTR_PERMISSIONS:
            List subPath = partialAccessor.getSubPath();
            Iterator it = (subPath != null) ? subPath.iterator() : EmptyUtils.EMPTY_ITERATOR;
            getAndUpdatePermissionListType(manager, path, resource, resource, indication, partialAccessor, it,
                    Constants.ID_MD_DELETE, true, M2MConstants.TAG_M2M_PERMISSIONS, Constants.ID_MODE_REQUIRED);
            break;
        case Constants.ID_ATTR_SELF_PERMISSIONS:
            break;
        case Constants.ID_ATTR_SUBSCRIPTIONS_REFERENCE:
            super.doDeleteIndication(manager, path, resource, indication, partialAccessor);
            break;
        default:
            // TODO should respond 404 Not Found
            super.doDeleteIndication(manager, path, resource, indication, partialAccessor);
            break;
        }
    }

    public void timeout(SclManager manager, String path, String timerId, M2MSession session, Serializable info)
            throws ParseException, IOException, StorageException, XoException, M2MException {
        SclTransaction transaction = new SclTransaction(manager.getStorageContext());
        deleteResource("expiration", manager, path, transaction);
        transaction.addTransientOp(new ExpirationTimerDeleteOp(manager, path));
        transaction.execute();
    }

    public int appendDiscoveryURIs(String logId, SclManager manager, String path, XoObject resource, URI requestingEntity,
            URI targetID, String appPath, String[] searchStrings, List discoveryURIs, int remainingURIs)
            throws StorageException, XoException {
        int urisCount = remainingURIs;
        try {
            checkRights(logId, manager, path, resource, requestingEntity, M2MConstants.FLAG_DISCOVER);
            if (urisCount > 0) {
                discoveryURIs.add(appPath + URIUtils.encodePath(path));
            }
            --urisCount;
        } catch (M2MException e) {
            // Right is not granted
        }
        return urisCount;
    }
}
