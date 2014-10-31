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
 * id $Id: NotificationChannels.java 4140 2013-01-15 14:08:57Z JReich $
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
import com.actility.m2m.scl.model.Constants;
import com.actility.m2m.scl.model.SclLogger;
import com.actility.m2m.scl.model.SclManager;
import com.actility.m2m.scl.model.SclTransaction;
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
 * Collection of M2M Channels.
 *
 * <pre>
 * m2m:NotificationChannels from ong:t_xml_obj
 * {
 *     m2m:creationTime    XoString    { } // (optional) (xmlType: xsd:dateTime)
 *     m2m:lastModifiedTime    XoString    { } // (optional) (xmlType: xsd:dateTime)
 *     m2m:notificationChannelCollection    m2m:NamedReferenceCollection    { } // (optional)
 * }
 * alias m2m:NotificationChannels with m2m:notificationChannels
 * </pre>
 */
public final class NotificationChannels extends SclResource {
    private static final Logger LOG = OSGiLogger.getLogger(NotificationChannels.class, BundleLogger.getStaticLogger());

    private static final NotificationChannels INSTANCE = new NotificationChannels();

    public static NotificationChannels getInstance() {
        return INSTANCE;
    }

    private final Set reservedKeywords = new HashSet();

    private NotificationChannels() {
        // Cannot be instantiated
        super(true, "notificationChannelsRetrieveRequestIndication", "notificationChannelsRetrieveResponseConfirm", null, null,
                null, null, Constants.ID_NO_FILTER_CRITERIA_MODE, true, false, null, false, false);
        reservedKeywords.add(M2MConstants.ATTR_CREATION_TIME);
        reservedKeywords.add(M2MConstants.ATTR_LAST_MODIFIED_TIME);
        reservedKeywords.add(M2MConstants.ATTR_NOTIFICATION_CHANNEL_COLLECTION);
    }

    public void checkRights(String logId, SclManager manager, String path, XoObject resource, URI requestingEntity, String flag)
            throws M2MException {
        checkDefaultAccessRight(logId, manager, path, requestingEntity);
    }

    public void createResource(SclManager manager, String path, Date creationDate, String creationTime,
            SclTransaction transaction) throws XoException {
        XoObject resource = null;
        try {
            resource = manager.getXoService().newXmlXoObject(M2MConstants.TAG_M2M_NOTIFICATION_CHANNELS);

            // Update resource
            resource.setNameSpace(M2MConstants.PREFIX_M2M);
            resource.setStringAttribute(M2MConstants.TAG_M2M_CREATION_TIME, creationTime);
            resource.setStringAttribute(M2MConstants.TAG_M2M_LAST_MODIFIED_TIME, creationTime);

            // Save resource
            Document document = manager.getStorageContext().getStorageFactory().createDocument(path);
            document.setAttribute(M2MConstants.ATTR_CREATION_TIME, creationDate);
            document.setAttribute(M2MConstants.ATTR_LAST_MODIFIED_TIME, creationDate);
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
            throws UnsupportedEncodingException, StorageException, XoException, M2MException {
        if (LOG.isInfoEnabled()) {
            LOG.info("Delete notificationChannels resource on path: " + path);
        }
        SearchResult searchResult = manager.getStorageContext().search(null, path, StorageRequestExecutor.SCOPE_ONE_LEVEL,
                null, StorageRequestExecutor.ORDER_UNKNOWN, -1, true, EmptyUtils.EMPTY_LIST);
        Iterator it = searchResult.getResults();
        Document document = null;
        String subPath = null;
        NotificationChannel notificationChannel = NotificationChannel.getInstance();
        while (it.hasNext()) {
            document = (Document) it.next();
            subPath = document.getPath();
            if (manager.getDefaultResourceId(subPath) == Constants.ID_RES_NOTIFICATION_CHANNEL) {
                notificationChannel.deleteResource(logId, manager, subPath, transaction);
            }
        }

        transaction.deleteResource(path);
    }

    public void deleteResource(String logId, SclManager manager, String path, XoObject resource, SclTransaction transaction)
            throws UnsupportedEncodingException, StorageException, XoException, M2MException {
        deleteResource(logId, manager, path, transaction);
    }

    public void deleteChildResource(String logId, SclManager manager, String path, XoObject resource, XoObject childResource,
            Date now, SclTransaction transaction) throws ParseException, XoException {
        resource.setStringAttribute(M2MConstants.TAG_M2M_LAST_MODIFIED_TIME,
                FormatUtils.formatDateTime(now, UtilConstants.LOCAL_TIMEZONE));

        Document document = manager.getStorageContext().getStorageFactory().createDocument(path);
        document.setAttribute(M2MConstants.ATTR_CREATION_TIME,
                FormatUtils.parseDateTime(resource.getStringAttribute(M2MConstants.TAG_M2M_CREATION_TIME)));
        document.setAttribute(M2MConstants.ATTR_LAST_MODIFIED_TIME, now);
        document.setContent(resource.saveBinary());
        transaction.updateResource(document);
    }

    public void prepareResourceForResponse(String logId, SclManager manager, URI requestingEntity, String path,
            XoObject resource, FilterCriteria filterCriteria, Set supported) throws UnsupportedEncodingException, XoException,
            StorageException {
        Condition condition = manager.getStorageContext().getStorageFactory()
                .createStringCondition(Constants.ATTR_TYPE, Condition.ATTR_OP_EQUAL, Constants.TYPE_NOTIFICATION_CHANNEL);
        SearchResult searchResult = manager.getStorageContext().search(null, path, StorageRequestExecutor.SCOPE_ONE_LEVEL,
                condition, StorageRequestExecutor.ORDER_UNKNOWN, -1, true, EmptyUtils.EMPTY_LIST);
        Iterator children = searchResult.getResults();
        generateNamedReferenceCollection(logId, manager, NotificationChannel.getInstance(), requestingEntity, path, resource,
                children, M2MConstants.TAG_M2M_NOTIFICATION_CHANNEL_COLLECTION);
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
            String id = (representation != null) ? getAndCheckNmToken(representation, M2MConstants.ATTR_M2M_ID,
                    Constants.ID_MODE_OPTIONAL) : null;
            if (id == null) {
                sendRepresentation = true;
                id = UUID.randomUUID(16);
            }
            String childPath = path + M2MConstants.URI_SEP + id;

            SclLogger.logRequestIndication("notificationChannelCreateRequestIndication",
                    "notificationChannelCreateResponseConfirm", indication, null, Constants.ID_LOG_REPRESENTATION);

            if (reservedKeywords.contains(id)) {
                throw new M2MException(id + " is a reserved keywork in notificationChannels resource",
                        StatusCode.STATUS_BAD_REQUEST);
            }

            Document testChildDocument = manager.getStorageContext().retrieve(null, childPath, null);
            if (testChildDocument != null) {
                sendUnsuccessResponse(manager, indication, StatusCode.STATUS_CONFLICT,
                        "A Notification Channel resource already exists with the id " + id);
            } else {
                NotificationChannel childController = NotificationChannel.getInstance();
                Date now = new Date();
                String creationTime = FormatUtils.formatDateTime(now, UtilConstants.LOCAL_TIMEZONE);
                SclTransaction transaction = new SclTransaction(manager.getStorageContext());
                childResource = manager.getXoService().newXmlXoObject(M2MConstants.TAG_M2M_NOTIFICATION_CHANNEL);
                sendRepresentation = childController.createResource(indication.getTransactionId(), manager, childPath,
                        childResource, id, now, creationTime, indication.getTargetID(), representation, transaction)
                        || sendRepresentation;

                resource.setStringAttribute(M2MConstants.TAG_M2M_LAST_MODIFIED_TIME, creationTime);

                Document document = manager.getStorageContext().getStorageFactory().createDocument(path);
                document.setAttribute(M2MConstants.ATTR_CREATION_TIME,
                        FormatUtils.parseDateTime(resource.getStringAttribute(M2MConstants.TAG_M2M_CREATION_TIME)));
                document.setAttribute(M2MConstants.ATTR_LAST_MODIFIED_TIME, now);
                document.setContent(resource.saveBinary());
                transaction.updateResource(document);
                transaction.execute();

                childController.sendCreatedSuccessResponse(manager, childPath, childResource, indication, sendRepresentation);
            }
        } finally {
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
