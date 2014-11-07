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
 * id $Id: Container.java 3302 2012-10-10 14:19:16Z JReich $
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
import java.util.Date;
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
import com.actility.m2m.scl.model.SclManager;
import com.actility.m2m.scl.model.SclTransaction;
import com.actility.m2m.scl.model.SubscribedResource;
import com.actility.m2m.scl.model.VolatileResource;
import com.actility.m2m.storage.Document;
import com.actility.m2m.storage.StorageException;
import com.actility.m2m.util.FormatUtils;
import com.actility.m2m.util.URIUtils;
import com.actility.m2m.util.log.OSGiLogger;
import com.actility.m2m.xo.XoException;
import com.actility.m2m.xo.XoObject;

/**
 * M2M Container. Placeholder to store Collections of Content Instances.
 * <p>
 * This resource can be subscribed.
 *
 * <pre>
 * m2m:Container from ong:t_xml_obj
 * {
 *     m2m:id    XoString    { embattr } // (optional) (xmlType: xsd:NMTOKEN)
 *     m2m:expirationTime    XoString    { } // (optional) (xmlType: xsd:dateTime)
 *     m2m:accessRightID    XoString    { shdico } // (optional) (xmlType: xsd:anyURI)
 *     m2m:searchStrings    m2m:SearchStrings    { } // (optional)
 *     m2m:creationTime    XoString    { } // (optional) (xmlType: xsd:dateTime)
 *     m2m:lastModifiedTime    XoString    { } // (optional) (xmlType: xsd:dateTime)
 *     m2m:announceTo    m2m:AnnounceTo    { } // (optional)
 *     m2m:maxNrOfInstances    XoString    { } // (optional) (xmlType: xsd:long)
 *     m2m:maxByteSize    XoString    { } // (optional) (xmlType: xsd:long)
 *     m2m:maxInstanceAge    XoString    { } // (optional) (xmlType: xsd:duration)
 *     acy:storageConfiguration    acy:StorageConfiguration    { } // (optional)
 *     m2m:contentInstancesReference    XoString    { } // (optional) (xmlType: xsd:anyURI)
 *     m2m:subcontainersReference    XoString    { } // (optional) (xmlType: xsd:anyURI)
 *     m2m:subscriptionsReference    XoString    { } // (optional) (xmlType: xsd:anyURI)
 * }
 * alias m2m:Container with m2m:container
 * </pre>
 */
public final class Container extends SclResource implements VolatileResource, SubscribedResource {
    private static final Logger LOG = OSGiLogger.getLogger(Container.class, BundleLogger.getStaticLogger());

    private static final Container INSTANCE = new Container();

    public static Container getInstance() {
        return INSTANCE;
    }

    private Container() {
        // Cannot be instantiated
        super(false, "containerRetrieveRequestIndication", "containerRetrieveResponseConfirm",
                "containerUpdateRequestIndication", "containerUpdateResponseConfirm", "containerDeleteRequestIndication",
                "containerDeleteResponseConfirm", Constants.ID_NO_FILTER_CRITERIA_MODE, true, true, Containers.getInstance(),
                true, true);
    }

    public void reload(SclManager manager, String path, XoObject resource, SclTransaction transaction) throws IOException,
            M2MException, StorageException, XoException {
        if (LOG.isInfoEnabled()) {
            LOG.info("Reload volatile <container>: " + path);
        }
        long now = System.currentTimeMillis();
        Date expirationTime = getAndCheckDateTime(resource, M2MConstants.TAG_M2M_EXPIRATION_TIME, Constants.ID_MODE_OPTIONAL,
                -1L);
        if (expirationTime != null && expirationTime.getTime() > 0) {
            if (expirationTime.getTime() > now) {
                if (expirationTime.getTime() < FormatUtils.MAX_DATE) {
                    transaction.addTransientOp(new ExpirationTimerUpdateOp(manager, path, Constants.ID_RES_CONTAINER,
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
            throws UnsupportedEncodingException, StorageException, XoException, M2MException {
        getAndCheckAccessRight(logId, manager, path, resource, requestingEntity, flag);
    }

    public boolean createResource(SclManager manager, String path, XoObject resource, String targetPath, String id,
            Date creationDate, String creationTime, XoObject representation, SclTransaction transaction) throws XoException,
            M2MException {
        boolean modified = false;
        // id: O (response M*)
        // expirationTime: O (response M*)
        // accessRightID: O (response O)
        // searchStrings: O (response M)
        // creationTime: NP (response M)
        // lastModifiedTime: NP (response M)
        // announceTo: O (response M*)
        // maxNrOfInstances: O (response M*)
        // maxByteSize: O (response M*)
        // maxInstanceAge: O (response M*)
        // storageConfiguration: O (response M)
        // contentInstancesReference: NP (response M)
        // subscriptionsReference: NP (response M)

        // Check representation
        checkRepresentation(representation, M2MConstants.TAG_M2M_CONTAINER);
        Date recvExpirationTime = getAndCheckDateTime(representation, M2MConstants.TAG_M2M_EXPIRATION_TIME,
                Constants.ID_MODE_OPTIONAL, creationDate.getTime());
        String accessRightId = getAndCheckAccessRightID(manager, representation, M2MConstants.TAG_M2M_ACCESS_RIGHT_I_D,
                Constants.ID_MODE_OPTIONAL, targetPath);
        getAndCheckStringMode(representation, M2MConstants.TAG_M2M_CREATION_TIME, Constants.ID_MODE_FORBIDDEN);
        long maxNrOfInstances = getAndCheckLong(representation, M2MConstants.TAG_M2M_MAX_NR_OF_INSTANCES,
                Constants.ID_MODE_OPTIONAL);
        long maxByteSize = getAndCheckLong(representation, M2MConstants.TAG_M2M_MAX_BYTE_SIZE, Constants.ID_MODE_OPTIONAL);
        long maxInstanceAge = getAndCheckDuration(representation, M2MConstants.TAG_M2M_MAX_INSTANCE_AGE,
                Constants.ID_MODE_OPTIONAL);
        getAndCheckStorageConfiguration(representation, M2MConstants.TAG_ACY_STORAGE_CONFIGURATION, Constants.ID_MODE_OPTIONAL);
        getAndCheckStringMode(representation, M2MConstants.TAG_M2M_CONTENT_INSTANCES_REFERENCE, Constants.ID_MODE_FORBIDDEN);
        getAndCheckStringMode(representation, M2MConstants.TAG_M2M_SUBSCRIPTIONS_REFERENCE, Constants.ID_MODE_FORBIDDEN);

        // Update resource
        resource.setNameSpace(M2MConstants.PREFIX_M2M);
        resource.setStringAttribute(M2MConstants.ATTR_M2M_ID, id);
        Date expirationTime = setExpirationTime(manager, resource, creationDate.getTime(), recvExpirationTime);
        if (recvExpirationTime != expirationTime) {
            modified = true;
        }
        createStringOptional(resource, representation, M2MConstants.TAG_M2M_ACCESS_RIGHT_I_D);
        createXoObjectMandatory(manager, resource, representation, M2MConstants.TAG_M2M_SEARCH_STRINGS);
        resource.setStringAttribute(M2MConstants.TAG_M2M_CREATION_TIME, creationTime);
        resource.setStringAttribute(M2MConstants.TAG_M2M_LAST_MODIFIED_TIME, creationTime);
        if (representation.getXoObjectAttribute(M2MConstants.TAG_M2M_ANNOUNCE_TO) != null) {
            modified = true;
        }
        resource.setXoObjectAttribute(M2MConstants.TAG_M2M_ANNOUNCE_TO,
                manager.getXoService().newXmlXoObject(M2MConstants.TAG_M2M_ANNOUNCE_TO));
        modified = setMaxNrOfInstances(manager, resource, maxNrOfInstances) || modified;
        modified = setMaxByteSize(manager, resource, maxByteSize) || modified;
        modified = setMaxInstanceAge(manager, resource, maxInstanceAge) || modified;
        createXoObjectMandatory(manager, resource, representation, M2MConstants.TAG_ACY_STORAGE_CONFIGURATION);

        // Create sub-resources
        ContentInstances.getInstance().createResource(manager,
                path + M2MConstants.URI_SEP + M2MConstants.RES_CONTENT_INSTANCES, creationDate, creationTime, transaction);
        Subcontainers.getInstance().createResource(manager, path + M2MConstants.URI_SEP + M2MConstants.RES_SUBCONTAINERS,
                creationDate, creationTime, accessRightId, transaction);
        Subscriptions.getInstance().createResource(manager, path + M2MConstants.URI_SEP + M2MConstants.RES_SUBSCRIPTIONS,
                transaction);

        // Save resource
        Document document = manager.getStorageContext().getStorageFactory().createDocument(path);
        document.setAttribute(Constants.ATTR_TYPE, Constants.TYPE_CONTAINER);
        XoObject searchStrings = resource.getXoObjectAttribute(M2MConstants.TAG_M2M_SEARCH_STRINGS);
        List searchStringList = searchStrings.getStringListAttribute(M2MConstants.TAG_M2M_SEARCH_STRING);
        document.setAttribute(M2MConstants.ATTR_SEARCH_STRING, new ArrayList(searchStringList));
        document.setAttribute(M2MConstants.ATTR_CREATION_TIME, creationDate);
        document.setAttribute(M2MConstants.ATTR_LAST_MODIFIED_TIME, creationDate);
        document.setContent(resource.saveBinary());
        transaction.createResource(document);

        transaction.addTransientOp(new ExpirationTimerUpdateOp(manager, path, Constants.ID_RES_CONTAINER, expirationTime
                .getTime() - creationDate.getTime()));

        return modified;
    }

    public boolean updateResource(String logId, SclManager manager, String path, XoObject resource, Indication indication,
            XoObject representation, Date now) throws ParseException, XoException, StorageException, M2MException {
        boolean modified = false;
        // id: NP (response M*)
        // expirationTime: O (response M*)
        // accessRightID: O (response O)
        // searchStrings: O (response M)
        // creationTime: NP (response M)
        // lastModifiedTime: NP (response M)
        // announceTo: O (response M*)
        // maxNrOfInstances: O (response M*)
        // maxByteSize: O (response M*)
        // maxInstanceAge: O (response M*)
        // / storageConfiguration: NP (response M)
        // contentInstancesReference: NP (response M)
        // subscriptionsReference: NP (response M)

        // Check representation
        checkRepresentation(representation, M2MConstants.TAG_M2M_CONTAINER);
        getAndCheckStringMode(representation, M2MConstants.ATTR_M2M_ID, Constants.ID_MODE_FORBIDDEN);
        Date recvExpirationTime = getAndCheckDateTime(representation, M2MConstants.TAG_M2M_EXPIRATION_TIME,
                Constants.ID_MODE_OPTIONAL, now.getTime());
        getAndCheckAccessRightID(manager, representation, M2MConstants.TAG_M2M_ACCESS_RIGHT_I_D, Constants.ID_MODE_OPTIONAL,
                indication.getTargetPath());
        getAndCheckStringMode(representation, M2MConstants.TAG_M2M_CREATION_TIME, Constants.ID_MODE_FORBIDDEN);
        long maxNrOfInstances = getAndCheckLong(representation, M2MConstants.TAG_M2M_MAX_NR_OF_INSTANCES,
                Constants.ID_MODE_OPTIONAL);
        long maxByteSize = getAndCheckLong(representation, M2MConstants.TAG_M2M_MAX_BYTE_SIZE, Constants.ID_MODE_OPTIONAL);
        long maxInstanceAge = getAndCheckDuration(representation, M2MConstants.TAG_M2M_MAX_INSTANCE_AGE,
                Constants.ID_MODE_OPTIONAL);
        getAndCheckXoObjectMode(representation, M2MConstants.TAG_ACY_STORAGE_CONFIGURATION, Constants.ID_MODE_FORBIDDEN);
        getAndCheckStringMode(representation, M2MConstants.TAG_M2M_CONTENT_INSTANCES_REFERENCE, Constants.ID_MODE_FORBIDDEN);
        getAndCheckStringMode(representation, M2MConstants.TAG_M2M_SUBSCRIPTIONS_REFERENCE, Constants.ID_MODE_FORBIDDEN);

        // Update resource
        Date expirationTime = setExpirationTime(manager, resource, now.getTime(), recvExpirationTime);
        if (recvExpirationTime != expirationTime) {
            modified = true;
        }
        updateStringOptional(resource, representation, M2MConstants.TAG_M2M_ACCESS_RIGHT_I_D);
        updateXoObjectMandatory(manager, resource, representation, M2MConstants.TAG_M2M_SEARCH_STRINGS);
        updateLastModifiedTime(manager, resource, now);
        if (representation.getXoObjectAttribute(M2MConstants.TAG_M2M_ANNOUNCE_TO) != null) {
            modified = true;
        }
        modified = setMaxNrOfInstances(manager, resource, maxNrOfInstances) || modified;
        modified = setMaxByteSize(manager, resource, maxByteSize) || modified;
        modified = setMaxInstanceAge(manager, resource, maxInstanceAge) || modified;

        // Save resource
        Document document = manager.getStorageContext().getStorageFactory().createDocument(path);
        document.setAttribute(Constants.ATTR_TYPE, Constants.TYPE_CONTAINER);
        XoObject searchStrings = resource.getXoObjectAttribute(M2MConstants.TAG_M2M_SEARCH_STRINGS);
        List searchStringList = searchStrings.getStringListAttribute(M2MConstants.TAG_M2M_SEARCH_STRING);
        document.setAttribute(M2MConstants.ATTR_SEARCH_STRING, new ArrayList(searchStringList));
        document.setAttribute(M2MConstants.ATTR_CREATION_TIME,
                FormatUtils.parseDateTime(resource.getStringAttribute(M2MConstants.TAG_M2M_CREATION_TIME)));
        document.setAttribute(M2MConstants.ATTR_LAST_MODIFIED_TIME, now);
        document.setContent(resource.saveBinary());
        manager.getStorageContext().update(null, document, null);

        new ExpirationTimerUpdateOp(manager, path, Constants.ID_RES_CONTAINER, expirationTime.getTime() - now.getTime())
                .postCommit();

        return modified;
    }

    public void deleteResource(String logId, SclManager manager, String path, SclTransaction transaction)
            throws StorageException, XoException, M2MException {
        if (LOG.isInfoEnabled()) {
            LOG.info("Delete <container> resource on path: " + path);
        }
        ContentInstances.getInstance().deleteResource(logId, manager,
                path + M2MConstants.URI_SEP + M2MConstants.RES_CONTENT_INSTANCES, transaction);
        Subcontainers.getInstance().deleteResource(logId, manager,
                path + M2MConstants.URI_SEP + M2MConstants.RES_SUBCONTAINERS, transaction);
        Subscriptions.getInstance().deleteResource(logId, manager,
                path + M2MConstants.URI_SEP + M2MConstants.RES_SUBSCRIPTIONS, transaction);

        transaction.deleteResource(path);
    }

    public void deleteResource(String logId, SclManager manager, String path, XoObject resource, SclTransaction transaction)
            throws StorageException, XoException, M2MException {
        deleteResource(logId, manager, path, transaction);
        transaction.addTransientOp(new ExpirationTimerDeleteOp(manager, path));
    }

    public void deleteChildResource(String logId, SclManager manager, String path, XoObject resource, XoObject childResource,
            Date now, SclTransaction transaction) {
        throw new UnsupportedOperationException();
    }

    public void prepareResourceForResponse(String logId, SclManager manager, URI requestingEntity, String path,
            XoObject resource, FilterCriteria filterCriteria, Set supported) {
        String appPath = manager.getM2MContext().getApplicationPath();
        String accessRight = resource.getStringAttribute(M2MConstants.TAG_M2M_ACCESS_RIGHT_I_D);
        if (accessRight != null) {
            resource.setStringAttribute(M2MConstants.TAG_M2M_ACCESS_RIGHT_I_D, appPath + URIUtils.encodePath(accessRight)
                    + M2MConstants.URI_SEP);
        }
        String encodedPath = URIUtils.encodePath(path);
        resource.setStringAttribute(M2MConstants.TAG_M2M_CONTENT_INSTANCES_REFERENCE, appPath + encodedPath
                + M2MConstants.URI_SEP + M2MConstants.RES_CONTENT_INSTANCES + M2MConstants.URI_SEP);
        resource.setStringAttribute(M2MConstants.TAG_M2M_SUBCONTAINERS_REFERENCE, appPath + encodedPath + M2MConstants.URI_SEP
                + M2MConstants.RES_SUBCONTAINERS + M2MConstants.URI_SEP);
        resource.setStringAttribute(M2MConstants.TAG_M2M_SUBSCRIPTIONS_REFERENCE, appPath + encodedPath + M2MConstants.URI_SEP
                + M2MConstants.RES_SUBSCRIPTIONS + M2MConstants.URI_SEP);
        if (supported == null || !supported.contains(Constants.SP_STORAGE_CONF)) {
            resource.getXoObjectAttribute(M2MConstants.TAG_ACY_STORAGE_CONFIGURATION).free(true);
            resource.setXoObjectAttribute(M2MConstants.TAG_ACY_STORAGE_CONFIGURATION, null);
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
