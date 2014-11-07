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
 * id $Id: Subscriptions.java 4140 2013-01-15 14:08:57Z JReich $
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
import java.util.Date;
import java.util.HashSet;
import java.util.Iterator;
import java.util.List;
import java.util.Set;

import org.apache.log4j.Logger;

import com.actility.m2m.m2m.FilterCriteria;
import com.actility.m2m.m2m.Indication;
import com.actility.m2m.m2m.M2MConstants;
import com.actility.m2m.m2m.M2MException;
import com.actility.m2m.m2m.StatusCode;
import com.actility.m2m.scl.log.BundleLogger;
import com.actility.m2m.scl.mem.SubsDeleteOp;
import com.actility.m2m.scl.model.Constants;
import com.actility.m2m.scl.model.SclLogger;
import com.actility.m2m.scl.model.SclManager;
import com.actility.m2m.scl.model.SclTransaction;
import com.actility.m2m.scl.model.SubscribedResource;
import com.actility.m2m.storage.Condition;
import com.actility.m2m.storage.Document;
import com.actility.m2m.storage.SearchResult;
import com.actility.m2m.storage.StorageException;
import com.actility.m2m.storage.StorageRequestExecutor;
import com.actility.m2m.util.EmptyUtils;
import com.actility.m2m.util.FormatUtils;
import com.actility.m2m.util.Profiler;
import com.actility.m2m.util.URIUtils;
import com.actility.m2m.util.UUID;
import com.actility.m2m.util.UtilConstants;
import com.actility.m2m.util.log.OSGiLogger;
import com.actility.m2m.xo.XoException;
import com.actility.m2m.xo.XoObject;

/**
 * Collection of M2M Subscriptions.
 *
 * <pre>
 * m2m:Subscriptions from ong:t_xml_obj
 * {
 *     m2m:subscriptionCollection    m2m:NamedReferenceCollection    { } // (optional)
 * }
 * alias m2m:Subscriptions with m2m:subscriptions
 * </pre>
 */
public final class Subscriptions extends SclResource {
    private static final Logger LOG = OSGiLogger.getLogger(Subscriptions.class, BundleLogger.getStaticLogger());

    private static final Subscriptions INSTANCE = new Subscriptions();

    public static Subscriptions getInstance() {
        return INSTANCE;
    }

    private final Set reservedKeywords = new HashSet();

    private Subscriptions() {
        // Cannot be instantiated
        super(true, "subscriptionsRetrieveRequestIndication", "subscriptionsRetrieveResponseConfirm", null, null, null, null,
                Constants.ID_NO_FILTER_CRITERIA_MODE, false, false, null, false, false);
        reservedKeywords.add(M2MConstants.ATTR_SUBSCRIPTION_COLLECTION);
    }

    public void checkRights(String logId, SclManager manager, String path, XoObject resource, URI requestingEntity, String flag)
            throws UnsupportedEncodingException, StorageException, XoException, M2MException {
        String realFlag = M2MConstants.FLAG_READ;
        if (M2MConstants.FLAG_READ.equals(flag)) {
            realFlag = M2MConstants.FLAG_DELETE;
        }
        XoObject parentResource = null;
        String parentPath = getParentPath(path);
        try {
            Object[] controllerAndResource = manager.getControllerAndResource(parentPath);
            SclResource parentController = (SclResource) controllerAndResource[0];
            parentResource = (XoObject) controllerAndResource[1];
            parentController.checkRights(logId, manager, parentPath, parentResource, requestingEntity, realFlag);
        } finally {
            if (parentResource != null) {
                parentResource.free(true);
            }
        }
    }

    public void createResource(SclManager manager, String path, SclTransaction transaction) throws XoException {
        XoObject resource = null;
        try {
            resource = manager.getXoService().newXmlXoObject(M2MConstants.TAG_M2M_SUBSCRIPTIONS);

            // Update resource
            resource.setNameSpace(M2MConstants.PREFIX_M2M);

            // Save resource
            Document document = manager.getStorageContext().getStorageFactory().createDocument(path);
            document.setContent(resource.saveBinary());
            transaction.createResource(document);
        } finally {
            if (resource != null) {
                resource.free(true);
            }
        }
    }

    public boolean updateResource(String logId, SclManager manager, String path, XoObject resource, Indication indication,
            XoObject representation, Date now) {
        throw new UnsupportedOperationException();
    }

    public void deleteResource(String logId, SclManager manager, String path, SclTransaction transaction)
            throws StorageException {
        if (LOG.isInfoEnabled()) {
            LOG.info("Delete subscriptions resource on path: " + path);
        }
        SearchResult searchResult = manager.getStorageContext().search(null, path, StorageRequestExecutor.SCOPE_ONE_LEVEL,
                null, StorageRequestExecutor.ORDER_UNKNOWN, -1, true, EmptyUtils.EMPTY_LIST);
        Iterator it = searchResult.getResults();
        Document document = null;
        String subPath = null;
        Subscription subscription = Subscription.getInstance();
        while (it.hasNext()) {
            document = (Document) it.next();
            subPath = document.getPath();
            if (manager.getDefaultResourceId(subPath) == Constants.ID_RES_SUBSCRIPTION) {
                subscription.deleteResource(logId, manager, subPath, transaction);
            }
        }

        transaction.deleteResource(path);
        transaction.addTransientOp(new SubsDeleteOp(manager, path));
        transaction.addTransientOp(new SubsDeleteOp(manager, path + M2MConstants.URI_SEP + M2MConstants.ATTR_LATEST));
    }

    public void deleteResource(String logId, SclManager manager, String path, XoObject resource, SclTransaction transaction)
            throws StorageException {
        deleteResource(logId, manager, path, transaction);
    }

    public void deleteChildResource(String logId, SclManager manager, String path, XoObject resource, XoObject childResource,
            Date now, SclTransaction transaction) {
        throw new UnsupportedOperationException();
    }

    public void prepareResourceForResponse(String logId, SclManager manager, URI requestingEntity, String path,
            XoObject resource, FilterCriteria filterCriteria, Set supported) throws UnsupportedEncodingException, XoException,
            StorageException {
        Condition condition = manager.getStorageContext().getStorageFactory()
                .createStringCondition(Constants.ATTR_TYPE, Condition.ATTR_OP_EQUAL, Constants.TYPE_SUBSCRIPTION);
        SearchResult searchResult = manager.getStorageContext().search(null, path, StorageRequestExecutor.SCOPE_ONE_LEVEL,
                condition, StorageRequestExecutor.ORDER_UNKNOWN, -1, true, EmptyUtils.EMPTY_LIST);
        Iterator children = searchResult.getResults();
        generateNamedReferenceCollection(logId, manager, Subscription.getInstance(), requestingEntity, path, resource,
                children, M2MConstants.TAG_M2M_SUBSCRIPTION_COLLECTION);
    }

    public void doCreateIndication(SclManager manager, String path, XoObject resource, Indication indication)
            throws ParseException, IOException, StorageException, XoException, M2MException {
        XoObject parentResource = null;
        XoObject representation = null;
        XoObject childResource = null;
        String parentPath = getParentPath(path);
        try {
            if (Profiler.getInstance().isTraceEnabled()) {
                Profiler.getInstance().trace(
                        indication.getTransactionId() + ": Check " + M2MConstants.FLAG_CREATE + " right on " + path);
            }
            Object[] controllerAndResource = manager.getControllerAndResource(parentPath);
            SclResource parentController = (SclResource) controllerAndResource[0];
            parentResource = (XoObject) controllerAndResource[1];

            parentController.checkRights(indication.getTransactionId(), manager, parentPath, parentResource,
                    indication.getRequestingEntity(), M2MConstants.FLAG_READ);
            if (Profiler.getInstance().isTraceEnabled()) {
                Profiler.getInstance().trace(
                        indication.getTransactionId() + ": " + M2MConstants.FLAG_CREATE + " right granted on " + path);
            }
            boolean sendRepresentation = false;
            representation = indication.getRepresentation(null);
            checkRepresentationNotNull(representation);

            // Check id
            String id = getAndCheckNmToken(representation, M2MConstants.ATTR_M2M_ID, Constants.ID_MODE_OPTIONAL);
            if (id == null) {
                sendRepresentation = true;
                id = UUID.randomUUID(16);
            }
            String childPath = path + M2MConstants.URI_SEP + id;

            SclLogger.logRequestIndication("subscriptionCreateRequestIndication", "subscriptionCreateResponseConfirm",
                    indication, null, Constants.ID_LOG_REPRESENTATION);

            if (reservedKeywords.contains(id)) {
                throw new M2MException(id + " is a reserved keywork in subscriptions resource", StatusCode.STATUS_BAD_REQUEST);
            }

            Document testChildDocument = manager.getStorageContext().retrieve(null, childPath, null);
            if (testChildDocument != null) {
                sendUnsuccessResponse(manager, indication, StatusCode.STATUS_CONFLICT,
                        "A Subscription resource already exists with the id " + id);
            } else {
                Subscription childController = Subscription.getInstance();
                URI requestingEntity = indication.getRequestingEntity();
                Date now = new Date();
                String creationTime = FormatUtils.formatDateTime(now, UtilConstants.LOCAL_TIMEZONE);
                SclTransaction transaction = new SclTransaction(manager.getStorageContext());

                childResource = manager.getXoService().newXmlXoObject(M2MConstants.TAG_M2M_SUBSCRIPTION);
                sendRepresentation = childController.createResource(manager, childPath, childResource, path, requestingEntity,
                        id, now, creationTime, indication.getRawContentType(), representation, parentPath,
                        (SubscribedResource) parentController, transaction)
                        || sendRepresentation;

                transaction.execute();

                childController.sendCreatedSuccessResponse(manager, childPath, childResource, indication, sendRepresentation);
            }
        } finally {
            if (parentResource != null) {
                parentResource.free(true);
            }
            if (representation != null) {
                representation.free(true);
            }
            if (childResource != null) {
                childResource.free(true);
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
