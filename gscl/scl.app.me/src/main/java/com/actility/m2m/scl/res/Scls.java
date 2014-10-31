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
 * id $Id: Scls.java 4140 2013-01-15 14:08:57Z JReich $
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
import com.actility.m2m.scl.mem.SubsManager;
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
import com.actility.m2m.util.UtilConstants;
import com.actility.m2m.util.log.OSGiLogger;
import com.actility.m2m.xo.XoException;
import com.actility.m2m.xo.XoObject;

/**
 * Collection of M2M Remote Service Capability Layers.
 * <p>
 * This resource can be subscribed.
 *
 * <pre>
 * m2m:Scls from ong:t_xml_obj
 * {
 *     m2m:accessRightID    XoString    { shdico } // (optional) (xmlType: xsd:anyURI)
 *     m2m:creationTime    XoString    { } // (optional) (xmlType: xsd:dateTime)
 *     m2m:lastModifiedTime    XoString    { } // (optional) (xmlType: xsd:dateTime)
 *     m2m:sclCollection    m2m:NamedReferenceCollection    { } // (optional)
 *     m2m:subscriptionsReference    XoString    { } // (optional) (xmlType: xsd:anyURI)
 *     m2m:mgmtObjsReference    XoString    { } // (optional) (xmlType: xsd:anyURI)
 * }
 * alias m2m:Scls with m2m:scls
 * </pre>
 */
public final class Scls extends SclResource implements SubscribedResource {
    private static final Logger LOG = OSGiLogger.getLogger(Scls.class, BundleLogger.getStaticLogger());

    private static final Scls INSTANCE = new Scls();

    public static Scls getInstance() {
        return INSTANCE;
    }

    private final Set reservedKeywords = new HashSet();

    private Scls() {
        // Cannot be instantiated
        super(true, "sclsRetrieveRequestIndication", "sclsRetrieveResponseConfirm", "sclsUpdateRequestIndication",
                "sclsUpdateResponseConfirm", null, null, Constants.ID_NO_FILTER_CRITERIA_MODE, true, true, null, false, false);
        reservedKeywords.add(M2MConstants.ATTR_ACCESS_RIGHT_I_D);
        reservedKeywords.add(M2MConstants.ATTR_CREATION_TIME);
        reservedKeywords.add(M2MConstants.ATTR_LAST_MODIFIED_TIME);
        reservedKeywords.add(M2MConstants.ATTR_SCL_COLLECTION);
        reservedKeywords.add(M2MConstants.ATTR_SUBSCRIPTIONS_REFERENCE);
        reservedKeywords.add(M2MConstants.RES_SUBSCRIPTIONS);
        reservedKeywords.add(M2MConstants.ATTR_MGMT_OBJS_REFERENCE);
        reservedKeywords.add(M2MConstants.RES_MGMT_OBJS);
    }

    public void checkRights(String logId, SclManager manager, String path, XoObject resource, URI requestingEntity, String flag)
            throws UnsupportedEncodingException, StorageException, XoException, M2MException {
        if (!M2MConstants.FLAG_CREATE.equals(flag)) {
            getAndCheckAccessRight(logId, manager, path, resource, requestingEntity, flag);
        }
    }

    public void createResource(SclManager manager, String path, Date creationDate, String creationTime, String accessRightID,
            SclTransaction transaction) throws XoException {
        XoObject resource = null;
        try {
            resource = manager.getXoService().newXmlXoObject(M2MConstants.TAG_M2M_SCLS);

            // Update resource
            resource.setNameSpace(M2MConstants.PREFIX_M2M);
            resource.setStringAttribute(M2MConstants.TAG_M2M_ACCESS_RIGHT_I_D, accessRightID);
            resource.setStringAttribute(M2MConstants.TAG_M2M_CREATION_TIME, creationTime);
            resource.setStringAttribute(M2MConstants.TAG_M2M_LAST_MODIFIED_TIME, creationTime);

            // Create sub-resources
            Subscriptions.getInstance().createResource(manager, path + M2MConstants.URI_SEP + M2MConstants.RES_SUBSCRIPTIONS,
                    transaction);

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

    public void createDefaultChild(SclManager manager, String path, XoObject resource, String sclId, URI link, boolean force)
            throws ParseException, UnsupportedEncodingException, StorageException, XoException, M2MException {
        // Check sclId
        String childPath = path + M2MConstants.URI_SEP + sclId;

        Document testChildDocument = manager.getStorageContext().retrieve(null, childPath, null);
        if (testChildDocument != null) {
            if (!force) {
                throw new M2MException("A Scl resource already exists with the sclId " + sclId,
                        StatusCode.STATUS_INTERNAL_SERVER_ERROR);
            }
            manager.getStorageContext().delete(null, manager.getStorageContext().getStorageFactory().createDocument(childPath),
                    null);
        }
        Date now = new Date();
        String creationTime = FormatUtils.formatDateTime(now, UtilConstants.LOCAL_TIMEZONE);
        SclTransaction transaction = new SclTransaction(manager.getStorageContext());
        Scl.getInstance().createDefaultResource(manager, childPath, sclId, now, creationTime, link, transaction);

        resource.setStringAttribute(M2MConstants.TAG_M2M_LAST_MODIFIED_TIME, creationTime);

        Document document = manager.getStorageContext().getStorageFactory().createDocument(path);
        document.setAttribute(M2MConstants.ATTR_CREATION_TIME,
                FormatUtils.parseDateTime(resource.getStringAttribute(M2MConstants.TAG_M2M_CREATION_TIME)));
        document.setAttribute(M2MConstants.ATTR_LAST_MODIFIED_TIME, now);
        document.setContent(resource.saveBinary());
        transaction.updateResource(document);
        transaction.execute();
    }

    public boolean updateResource(String logId, SclManager manager, String path, XoObject resource, Indication indication,
            XoObject representation, Date now) throws ParseException, XoException, StorageException, M2MException {
        // accessRightID: O (response O)
        // creationTime: NP (response M)
        // lastModifiedTime: NP (response M)
        // sclCollection: NP (response M)
        // subscriptionsReference: NP (response M)
        // mgmtObjsReference: NP (response O)

        // Check representation
        checkRepresentation(representation, M2MConstants.TAG_M2M_SCLS);
        getAndCheckXoObjectMode(representation, M2MConstants.TAG_M2M_SCL_COLLECTION, Constants.ID_MODE_FORBIDDEN);
        getAndCheckStringMode(representation, M2MConstants.TAG_M2M_SUBSCRIPTIONS_REFERENCE, Constants.ID_MODE_FORBIDDEN);
        getAndCheckStringMode(representation, M2MConstants.TAG_M2M_MGMT_OBJS_REFERENCE, Constants.ID_MODE_FORBIDDEN);
        getAndCheckAccessRightID(manager, representation, M2MConstants.TAG_M2M_ACCESS_RIGHT_I_D, Constants.ID_MODE_OPTIONAL,
                indication.getTargetPath());
        getAndCheckStringMode(representation, M2MConstants.TAG_M2M_CREATION_TIME, Constants.ID_MODE_FORBIDDEN);

        // Update resource
        updateStringOptional(resource, representation, M2MConstants.TAG_M2M_ACCESS_RIGHT_I_D);
        updateLastModifiedTime(manager, resource, new Date());

        // Save resource
        Document document = manager.getStorageContext().getStorageFactory().createDocument(path);
        document.setAttribute(M2MConstants.ATTR_CREATION_TIME,
                FormatUtils.parseDateTime(resource.getStringAttribute(M2MConstants.TAG_M2M_CREATION_TIME)));
        document.setAttribute(M2MConstants.ATTR_LAST_MODIFIED_TIME, now);
        document.setContent(resource.saveBinary());
        manager.getStorageContext().update(null, document, null);

        return false;
    }

    public void deleteResource(String logId, SclManager manager, String path, XoObject resource, SclTransaction transaction) {
        throw new UnsupportedOperationException();
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
            XoObject resource, FilterCriteria filterCriteria, Set supported) throws UnsupportedEncodingException,
            StorageException, XoException {
        String appPath = manager.getM2MContext().getApplicationPath();
        String accessRight = resource.getStringAttribute(M2MConstants.TAG_M2M_ACCESS_RIGHT_I_D);
        if (accessRight != null) {
            resource.setStringAttribute(M2MConstants.TAG_M2M_ACCESS_RIGHT_I_D, appPath + URIUtils.encodePath(accessRight)
                    + M2MConstants.URI_SEP);
        }
        resource.setStringAttribute(M2MConstants.TAG_M2M_SUBSCRIPTIONS_REFERENCE, appPath + URIUtils.encodePath(path)
                + M2MConstants.URI_SEP + M2MConstants.RES_SUBSCRIPTIONS + M2MConstants.URI_SEP);
        // Add scls children
        Condition condition = manager.getStorageContext().getStorageFactory()
                .createStringCondition(Constants.ATTR_TYPE, Condition.ATTR_OP_EQUAL, Constants.TYPE_SCL);
        SearchResult searchResult = manager.getStorageContext().search(null, path, StorageRequestExecutor.SCOPE_ONE_LEVEL,
                condition, StorageRequestExecutor.ORDER_UNKNOWN, -1, true, EmptyUtils.EMPTY_LIST);
        Iterator children = searchResult.getResults();
        generateNamedReferenceCollection(logId, manager, Scl.getInstance(), requestingEntity, path, resource, children,
                M2MConstants.TAG_M2M_SCL_COLLECTION);
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
            representation = indication.getRepresentation(null);
            checkRepresentationNotNull(representation);

            // Check sclId
            String sclId = representation.getStringAttribute(M2MConstants.ATTR_SCL_ID);
            if (sclId == null) {
                sendUnsuccessResponse(manager, indication, StatusCode.STATUS_BAD_REQUEST,
                        "Missing mandatory sclId attribute in <scl> representation");
            } else {
                String childPath = path + M2MConstants.URI_SEP + sclId;

                SclLogger.logRequestIndication("sclCreateRequestIndication", "sclCreateResponseConfirm", indication, null,
                        Constants.ID_LOG_REPRESENTATION);

                if (sclId.length() == 0) {
                    throw new M2MException("sclId cannot be empty", StatusCode.STATUS_BAD_REQUEST);
                } else if (reservedKeywords.contains(sclId)) {
                    throw new M2MException(sclId + " is a reserved keywork in scls resource", StatusCode.STATUS_BAD_REQUEST);
                } else if (sclId.indexOf('/') != -1) {
                    throw new M2MException("sclId cannot contains a '/' character: " + sclId, StatusCode.STATUS_BAD_REQUEST);
                }

                Document testChildDocument = manager.getStorageContext().retrieve(null, childPath, null);
                if (testChildDocument != null) {
                    sendUnsuccessResponse(manager, indication, StatusCode.STATUS_CONFLICT,
                            "A Scl resource already exists with the sclId " + sclId);
                } else {
                    Scl childController = Scl.getInstance();
                    Date now = new Date();
                    String creationTime = FormatUtils.formatDateTime(now, UtilConstants.LOCAL_TIMEZONE);
                    SclTransaction transaction = new SclTransaction(manager.getStorageContext());
                    childResource = manager.getXoService().newXmlXoObject(M2MConstants.TAG_M2M_SCL);
                    boolean sendRepresentation = childController.createResource(manager, childPath, childResource,
                            indication.getTargetPath(), now, creationTime, representation, transaction);

                    resource.setStringAttribute(M2MConstants.TAG_M2M_LAST_MODIFIED_TIME, creationTime);

                    Document document = manager.getStorageContext().getStorageFactory().createDocument(path);
                    document.setAttribute(M2MConstants.ATTR_CREATION_TIME,
                            FormatUtils.parseDateTime(resource.getStringAttribute(M2MConstants.TAG_M2M_CREATION_TIME)));
                    document.setAttribute(M2MConstants.ATTR_LAST_MODIFIED_TIME, now);
                    document.setContent(resource.saveBinary());
                    transaction.updateResource(document);
                    transaction.execute();

                    childController.sendCreatedSuccessResponse(manager, childPath, childResource, indication,
                            sendRepresentation);

                    SubsManager subscriptions = (SubsManager) manager.getM2MContext().getAttribute(
                            path + M2MConstants.URI_SEP + M2MConstants.RES_SUBSCRIPTIONS);
                    if (subscriptions != null) {
                        subscriptions.resourceUpdated(resource, false, now.getTime());
                    }
                }
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
