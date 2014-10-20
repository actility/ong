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
 * id $Id: AccessRights.java 4140 2013-01-15 14:08:57Z JReich $
 * author $Author: JReich $
 * version $Revision: 4140 $
 * lastrevision $Date: 2013-01-15 15:08:57 +0100 (Tue, 15 Jan 2013) $
 * modifiedby $LastChangedBy: JReich $
 * lastmodified $LastChangedDate: 2013-01-15 15:08:57 +0100 (Tue, 15 Jan 2013) $
 */

package com.actility.m2m.scl.res;

import java.io.IOException;
import java.io.UnsupportedEncodingException;
import java.net.URI;
import java.text.ParseException;
import java.util.ArrayList;
import java.util.Collection;
import java.util.Date;
import java.util.HashSet;
import java.util.Iterator;
import java.util.List;
import java.util.Map;
import java.util.Map.Entry;
import java.util.Set;

import org.apache.log4j.Logger;

import com.actility.m2m.m2m.FilterCriteria;
import com.actility.m2m.m2m.Indication;
import com.actility.m2m.m2m.M2MConstants;
import com.actility.m2m.m2m.M2MException;
import com.actility.m2m.m2m.StatusCode;
import com.actility.m2m.scl.log.BundleLogger;
import com.actility.m2m.scl.mem.SubsManager;
import com.actility.m2m.scl.model.Constants;
import com.actility.m2m.scl.model.SclLogger;
import com.actility.m2m.scl.model.SclManager;
import com.actility.m2m.scl.model.SclTransaction;
import com.actility.m2m.scl.model.SubscribedResource;
import com.actility.m2m.storage.Condition;
import com.actility.m2m.storage.ConditionBuilder;
import com.actility.m2m.storage.SearchResult;
import com.actility.m2m.storage.StorageException;
import com.actility.m2m.storage.StorageRequestExecutor;
import com.actility.m2m.util.EmptyUtils;
import com.actility.m2m.util.FormatUtils;
import com.actility.m2m.util.Pair;
import com.actility.m2m.util.Profiler;
import com.actility.m2m.util.URIUtils;
import com.actility.m2m.util.UUID;
import com.actility.m2m.util.log.OSGiLogger;
import com.actility.m2m.xo.XoException;
import com.actility.m2m.xo.XoObject;

/**
 * Collection of M2M Access Control Rights.
 */
public final class AccessRights extends SclResource implements SubscribedResource {
    private static final Logger LOG = OSGiLogger.getLogger(AccessRights.class, BundleLogger.getStaticLogger());

    private static final AccessRights INSTANCE = new AccessRights();

    public static AccessRights getInstance() {
        return INSTANCE;
    }

    private final Set reservedKeywords = new HashSet();

    private AccessRights() {
        // Cannot be instantiated
        super(true, "accessRightsRetrieveRequestIndication", "accessRightsRetrieveResponseConfirm",
                "accessRightsUpdateRequestIndication", "accessRightsUpdateResponseConfirm", null, null,
                Constants.ID_NO_FILTER_CRITERIA_MODE, true, true, null, false, false);
        reservedKeywords.add(M2MConstants.ATTR_ACCESS_RIGHT_I_D);
        reservedKeywords.add(M2MConstants.ATTR_CREATION_TIME);
        reservedKeywords.add(M2MConstants.ATTR_LAST_MODIFIED_TIME);
        reservedKeywords.add(M2MConstants.ATTR_ACCESS_RIGHT_COLLECTION);
        reservedKeywords.add(M2MConstants.ATTR_ACCESS_RIGHT_ANNC_COLLECTION);
        reservedKeywords.add(M2MConstants.ATTR_SUBSCRIPTIONS_REFERENCE);
        reservedKeywords.add(M2MConstants.RES_SUBSCRIPTIONS);
    }

    // m2m:AccessRights from ong:t_xml_obj
    // {
    // m2m:accessRightID XoString { } // (optional) (xmlType: xsd:anyURI)
    // m2m:creationTime XoString { } // (optional) (xmlType: xsd:dateTime)
    // m2m:lastModifiedTime XoString { } // (optional) (xmlType: xsd:dateTime)
    // m2m:accessRightCollection m2m:NamedReferenceCollection { } // (optional)
    // m2m:accessRightAnncCollection m2m:NamedReferenceCollection { } // (optional)
    // m2m:subscriptionsReference XoString { } // (optional) (xmlType: xsd:anyURI)
    // }
    // alias m2m:AccessRights with m2m:accessRights

    public void checkRights(String logId, SclManager manager, String path, XoObject resource, URI requestingEntity, String flag)
            throws UnsupportedEncodingException, StorageException, XoException, M2MException {
        getAndCheckAccessRight(logId, manager, path, resource, requestingEntity, flag);
    }

    public void createResource(SclManager manager, String path, Date creationDate, String creationTime, String accessRightID,
            SclTransaction transaction) throws XoException {
        XoObject resource = null;
        try {
            resource = manager.getXoService().newXmlXoObject(M2MConstants.TAG_M2M_ACCESS_RIGHTS);

            // Update resource
            resource.setNameSpace(M2MConstants.PREFIX_M2M);
            resource.setStringAttribute(M2MConstants.TAG_M2M_ACCESS_RIGHT_I_D, accessRightID);
            resource.setStringAttribute(M2MConstants.TAG_M2M_CREATION_TIME, creationTime);
            resource.setStringAttribute(M2MConstants.TAG_M2M_LAST_MODIFIED_TIME, creationTime);

            // Create sub-resources
            Subscriptions.getInstance().createResource(manager, path + M2MConstants.URI_SEP + M2MConstants.RES_SUBSCRIPTIONS,
                    transaction);

            // Save resource
            Collection searchAttributes = new ArrayList();
            searchAttributes.add(new Pair(M2MConstants.ATTR_CREATION_TIME, creationDate));
            searchAttributes.add(new Pair(M2MConstants.ATTR_LAST_MODIFIED_TIME, creationDate));
            transaction.createResource(path, resource, searchAttributes);
        } finally {
            if (resource != null) {
                resource.free(true);
            }
        }
    }

    public boolean updateResource(String logId, SclManager manager, String path, XoObject resource, Indication indication,
            XoObject representation, Date now) throws ParseException, XoException, StorageException, M2MException {
        // accessRightID: O (response O)
        // creationTime: NP (response M)
        // lastModifiedTime: NP (response M)
        // accessRightCollection: NP (response M)
        // accessRightAnncCollection: NP (response M)
        // subscriptionsReference: NP (response M)

        // Check representation
        checkRepresentation(representation, M2MConstants.TAG_M2M_ACCESS_RIGHTS);
        getAndCheckAccessRightID(manager, representation, M2MConstants.TAG_M2M_ACCESS_RIGHT_I_D, Constants.ID_MODE_OPTIONAL,
                indication.getTargetPath());
        getAndCheckStringMode(representation, M2MConstants.TAG_M2M_CREATION_TIME, Constants.ID_MODE_FORBIDDEN);
        getAndCheckXoObjectMode(representation, M2MConstants.TAG_M2M_ACCESS_RIGHT_COLLECTION, Constants.ID_MODE_FORBIDDEN);
        getAndCheckXoObjectMode(representation, M2MConstants.TAG_M2M_ACCESS_RIGHT_ANNC_COLLECTION, Constants.ID_MODE_FORBIDDEN);
        getAndCheckStringMode(representation, M2MConstants.TAG_M2M_SUBSCRIPTIONS_REFERENCE, Constants.ID_MODE_FORBIDDEN);

        // Update resource
        updateStringOptional(resource, representation, M2MConstants.TAG_M2M_ACCESS_RIGHT_I_D);
        updateLastModifiedTime(manager, resource, now);

        // Save resource
        Collection searchAttributes = new ArrayList();
        searchAttributes.add(new Pair(M2MConstants.ATTR_CREATION_TIME, FormatUtils.parseDateTime(resource
                .getStringAttribute(M2MConstants.TAG_M2M_CREATION_TIME))));
        searchAttributes.add(new Pair(M2MConstants.ATTR_LAST_MODIFIED_TIME, now));
        manager.getStorageContext().update(path, resource.saveBinary(), searchAttributes);

        return false;
    }

    public void deleteResource(String logId, SclManager manager, String path, SclTransaction transaction)
            throws StorageException {
        if (LOG.isInfoEnabled()) {
            LOG.info("Delete accessRights resource on path: " + path);
        }
        SearchResult searchResult = manager.getStorageContext().search(path, StorageRequestExecutor.SCOPE_EXACT, null);
        Map children = searchResult.getResults();
        Iterator it = children.entrySet().iterator();
        Entry entry = null;
        String subPath = null;
        AccessRight accessRight = AccessRight.getInstance();
        while (it.hasNext()) {
            entry = (Entry) it.next();
            subPath = (String) entry.getKey();
            if (manager.getDefaultResourceId(subPath) == Constants.ID_RES_ACCESS_RIGHT) {
                accessRight.deleteResource(logId, manager, subPath, transaction);
            }
        }

        Subscriptions.getInstance().deleteResource(logId, manager,
                path + M2MConstants.URI_SEP + M2MConstants.RES_SUBSCRIPTIONS, transaction);

        transaction.deleteResource(path);
    }

    public void deleteResource(String logId, SclManager manager, String path, XoObject resource, SclTransaction transaction)
            throws StorageException {
        deleteResource(logId, manager, path, transaction);
    }

    public void deleteChildResource(String logId, SclManager manager, String path, XoObject resource, XoObject childResource,
            Date now, SclTransaction transaction) throws ParseException, XoException {
        resource.setStringAttribute(M2MConstants.TAG_M2M_LAST_MODIFIED_TIME,
                FormatUtils.formatDateTime(now, manager.getTimeZone()));

        Collection searchAttributes = new ArrayList();
        searchAttributes.add(new Pair(M2MConstants.ATTR_CREATION_TIME, FormatUtils.parseDateTime(resource
                .getStringAttribute(M2MConstants.TAG_M2M_CREATION_TIME))));
        searchAttributes.add(new Pair(M2MConstants.ATTR_LAST_MODIFIED_TIME, now));
        transaction.updateResource(path, resource, searchAttributes);
    }

    public void prepareResourceForResponse(String logId, SclManager manager, String path, XoObject resource,
            URI requestingEntity, FilterCriteria filterCriteria, Set supported) throws UnsupportedEncodingException,
            StorageException, XoException {
        String appPath = manager.getM2MContext().getApplicationPath();
        String accessRight = resource.getStringAttribute(M2MConstants.TAG_M2M_ACCESS_RIGHT_I_D);
        if (accessRight != null) {
            resource.setStringAttribute(M2MConstants.TAG_M2M_ACCESS_RIGHT_I_D, appPath + URIUtils.encodePath(accessRight)
                    + M2MConstants.URI_SEP);
        }
        resource.setStringAttribute(M2MConstants.TAG_M2M_SUBSCRIPTIONS_REFERENCE, appPath + URIUtils.encodePath(path)
                + M2MConstants.URI_SEP + M2MConstants.RES_SUBSCRIPTIONS);
        // Add accessRights children
        Condition condition = manager.getConditionBuilder().createStringCondition(Constants.ATTR_TYPE,
                ConditionBuilder.OPERATOR_EQUAL, Constants.TYPE_ACCESS_RIGHT);
        SearchResult searchResultAccessRight = manager.getStorageContext().search(path, StorageRequestExecutor.SCOPE_EXACT,
                condition);
        Map childrenAccessRight = searchResultAccessRight.getResults();
        generateNamedReferenceCollection(logId, manager, AccessRight.getInstance(), requestingEntity, path, resource,
                childrenAccessRight, M2MConstants.TAG_M2M_ACCESS_RIGHT_COLLECTION);
        generateNamedReferenceCollection(logId, manager, null, requestingEntity, path, resource, EmptyUtils.EMPTY_MAP,
                M2MConstants.TAG_M2M_ACCESS_RIGHT_ANNC_COLLECTION);
    }

    public void doCreateIndication(SclManager manager, String path, XoObject resource, Indication indication)
            throws ParseException, IOException, StorageException, XoException, M2MException {
        if (Profiler.getInstance().isTraceEnabled()) {
            Profiler.getInstance().trace(
                    indication.getTransactionId() + ": Check " + M2MConstants.FLAG_CREATE + " right on " + path);
        }
        checkRights(indication.getTransactionId(), manager, path, resource, indication.getRequestingEntity(),
                M2MConstants.FLAG_CREATE);
        if (Profiler.getInstance().isTraceEnabled()) {
            Profiler.getInstance().trace(
                    indication.getTransactionId() + ": " + M2MConstants.FLAG_CREATE + " right granted on " + path);
        }

        XoObject representation = null;
        XoObject childResource = null;
        try {
            boolean sendRepresentation = false;
            representation = indication.getRepresentation(null);
            checkRepresentationNotNull(representation);

            // Check id
            String id = representation.getStringAttribute(M2MConstants.ATTR_M2M_ID);
            if (id == null) {
                sendRepresentation = true;
                id = UUID.randomUUID(16);
            }
            String childPath = path + M2MConstants.URI_SEP + id;

            SclLogger.logRequestIndication(Constants.PT_ACCESS_RIGHT_CREATE_REQUEST, Constants.PT_ACCESS_RIGHT_CREATE_RESPONSE,
                    indication, null, Constants.ID_LOG_REPRESENTATION);

            if (id.length() == 0) {
                throw new M2MException("id cannot be empty", StatusCode.STATUS_BAD_REQUEST);
            } else if (reservedKeywords.contains(id)) {
                throw new M2MException(id + " is a reserved keywork in accessRights resource", StatusCode.STATUS_BAD_REQUEST);
            } else if (id.indexOf('/') != -1) {
                throw new M2MException("id cannot contains a '/' character: " + id, StatusCode.STATUS_BAD_REQUEST);
            }
            byte[] testChildPath = manager.getStorageContext().retrieve(childPath);
            if (testChildPath != null) {
                sendUnsuccessResponse(manager, indication, StatusCode.STATUS_CONFLICT,
                        "An Access Right resource already exists with the id " + id);
            } else {
                AccessRight childController = AccessRight.getInstance();
                Date now = new Date();
                String creationTime = FormatUtils.formatDateTime(now, manager.getTimeZone());
                SclTransaction transaction = new SclTransaction(manager.getStorageContext());
                childResource = manager.getXoService().newXmlXoObject(representation.getName());
                sendRepresentation = childController.createResource(manager, childPath, childResource,
                        indication.getRequestingEntity(), id, now, creationTime, representation, transaction)
                        || sendRepresentation;

                resource.setStringAttribute(M2MConstants.TAG_M2M_LAST_MODIFIED_TIME, creationTime);

                Collection searchAttributes = new ArrayList();
                searchAttributes.add(new Pair(M2MConstants.ATTR_CREATION_TIME, FormatUtils.parseDateTime(resource
                        .getStringAttribute(M2MConstants.TAG_M2M_CREATION_TIME))));
                searchAttributes.add(new Pair(M2MConstants.ATTR_LAST_MODIFIED_TIME, now));
                transaction.updateResource(path, resource, searchAttributes);
                transaction.execute();

                childController.sendCreatedSuccessResponse(manager, childPath, childResource, indication, sendRepresentation);

                SubsManager subscriptions = (SubsManager) manager.getM2MContext().getAttribute(
                        path + M2MConstants.URI_SEP + M2MConstants.RES_SUBSCRIPTIONS);
                if (subscriptions != null) {
                    subscriptions.resourceUpdated(resource, false, now.getTime());
                }
            }
        } finally {
            if (childResource != null) {
                childResource.free(true);
            }
            if (representation != null) {
                representation.free(true);
            }
        }
    }

    public int appendDiscoveryURIs(String logId, SclManager manager, String path, XoObject resource, URI requestingEntity,
            URI targetID, String appPath, String[] searchStrings, List discoveryURIs, int remainingURIs) throws IOException,
            StorageException, XoException {
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
