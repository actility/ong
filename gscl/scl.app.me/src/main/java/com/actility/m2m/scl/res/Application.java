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
 * id $Id: Application.java 3302 2012-10-10 14:19:16Z JReich $
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
import java.util.HashSet;
import java.util.Iterator;
import java.util.List;
import java.util.Set;

import org.apache.log4j.Logger;

import com.actility.m2m.m2m.FilterCriteria;
import com.actility.m2m.m2m.Indication;
import com.actility.m2m.m2m.M2MConstants;
import com.actility.m2m.m2m.M2MContext;
import com.actility.m2m.m2m.M2MException;
import com.actility.m2m.m2m.M2MSession;
import com.actility.m2m.m2m.Request;
import com.actility.m2m.m2m.StatusCode;
import com.actility.m2m.scl.log.BundleLogger;
import com.actility.m2m.scl.mem.ExpirationTimerDeleteOp;
import com.actility.m2m.scl.mem.ExpirationTimerUpdateOp;
import com.actility.m2m.scl.model.Constants;
import com.actility.m2m.scl.model.PartialAccessor;
import com.actility.m2m.scl.model.SclLogger;
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
 * M2M Application. Defines an application on the M2M network, it could be anything like a device, a logic application, a
 * service...
 * <p>
 * This resource can be subscribed.
 *
 * <pre>
 * m2m:Application from ong:t_xml_obj
 * {
 *     appId    XoString    { embattr } // (optional) (xmlType: xsd:anyURI)
 *     m2m:expirationTime    XoString    { } // (optional) (xmlType: xsd:dateTime)
 *     m2m:accessRightID    XoString    { shdico } // (optional) (xmlType: xsd:anyURI)
 *     m2m:searchStrings    m2m:SearchStrings    { } // (optional)
 *     m2m:creationTime    XoString    { } // (optional) (xmlType: xsd:dateTime)
 *     m2m:lastModifiedTime    XoString    { } // (optional) (xmlType: xsd:dateTime)
 *     m2m:announceTo    m2m:AnnounceTo    { } // (optional)
 *     m2m:aPoC    XoString    { } // (optional) (xmlType: xsd:anyURI)
 *     m2m:aPoCPaths    m2m:APoCPaths    { } // (optional)
 *     m2m:locRequestor    XoString    { } // (optional) (xmlType: xsd:anyType)
 *     m2m:referencePoint    XoString    { } // (optional) (xmlType: m2m:ReferencePoint) (enum: MIA_REFERENCE_POINT DIA_REFERENCE_POINT )
 *     m2m:poc    XoString    { } // (optional) (xmlType: xsd:anyURI)
 *     m2m:containersReference    XoString    { } // (optional) (xmlType: xsd:anyURI)
 *     m2m:groupsReference    XoString    { } // (optional) (xmlType: xsd:anyURI)
 *     m2m:accessRightsReference    XoString    { } // (optional) (xmlType: xsd:anyURI)
 *     m2m:subscriptionsReference    XoString    { } // (optional) (xmlType: xsd:anyURI)
 *     m2m:notificationChannelsReference    XoString    { } // (optional) (xmlType: xsd:anyURI)
 * }
 * alias m2m:Application with m2m:application
 * </pre>
 */
public final class Application extends SclResource implements VolatileResource, SubscribedResource {
    private static final Logger LOG = OSGiLogger.getLogger(Application.class, BundleLogger.getStaticLogger());

    private static final Application INSTANCE = new Application();

    public static Application getInstance() {
        return INSTANCE;
    }

    private final Set reservedKeywords = new HashSet();

    private Application() {
        // Cannot be instantiated
        super(false, "applicationRetrieveRequestIndication", "applicationRetrieveResponseConfirm",
                "applicationUpdateRequestIndication", "applicationUpdateResponseConfirm", "applicationDeleteRequestIndication",
                "applicationDeleteResponseConfirm", Constants.ID_NO_FILTER_CRITERIA_MODE, true, true, Applications
                        .getInstance(), true, false);
        reservedKeywords.add(M2MConstants.ATTR_APP_ID);
        reservedKeywords.add(M2MConstants.ATTR_EXPIRATION_TIME);
        reservedKeywords.add(M2MConstants.ATTR_ACCESS_RIGHT_I_D);
        reservedKeywords.add(M2MConstants.ATTR_SEARCH_STRINGS);
        reservedKeywords.add(M2MConstants.ATTR_CREATION_TIME);
        reservedKeywords.add(M2MConstants.ATTR_LAST_MODIFIED_TIME);
        reservedKeywords.add(M2MConstants.ATTR_ANNOUNCE_TO);
        reservedKeywords.add(M2MConstants.ATTR_A_PO_C);
        reservedKeywords.add(M2MConstants.ATTR_A_PO_C_PATHS);
        reservedKeywords.add(M2MConstants.ATTR_LOC_REQUESTOR);
        reservedKeywords.add(M2MConstants.ATTR_REFERENCE_POINT);
        reservedKeywords.add(M2MConstants.ATTR_POC);
        reservedKeywords.add(M2MConstants.ATTR_CONTAINERS_REFERENCE);
        reservedKeywords.add(M2MConstants.RES_CONTAINERS);
        reservedKeywords.add(M2MConstants.ATTR_GROUPS_REFERENCE);
        reservedKeywords.add(M2MConstants.RES_GROUPS);
        reservedKeywords.add(M2MConstants.ATTR_ACCESS_RIGHTS_REFERENCE);
        reservedKeywords.add(M2MConstants.RES_ACCESS_RIGHTS);
        reservedKeywords.add(M2MConstants.ATTR_SUBSCRIPTIONS_REFERENCE);
        reservedKeywords.add(M2MConstants.RES_SUBSCRIPTIONS);
        reservedKeywords.add(M2MConstants.ATTR_NOTIFICATION_CHANNELS_REFERENCE);
        reservedKeywords.add(M2MConstants.RES_NOTIFICATION_CHANNELS);
    }

    public void reload(SclManager manager, String path, XoObject resource, SclTransaction transaction) throws IOException,
            M2MException, StorageException, XoException {
        if (LOG.isInfoEnabled()) {
            LOG.info("Reload volatile <application>: " + path);
        }
        long now = System.currentTimeMillis();
        Date expirationTime = getAndCheckDateTime(resource, M2MConstants.TAG_M2M_EXPIRATION_TIME, Constants.ID_MODE_OPTIONAL,
                -1L);
        if (expirationTime != null && expirationTime.getTime() > 0) {
            if (expirationTime.getTime() > now) {
                if (expirationTime.getTime() < FormatUtils.MAX_DATE) {
                    transaction.addTransientOp(new ExpirationTimerUpdateOp(manager, path, Constants.ID_RES_APPLICATION,
                            expirationTime.getTime() - now));
                }
                // else infinite timer
            } else {
                // delete resource, it is already expired
                deleteResource("restore", manager, path, transaction);
            }
        }
    }

    public void retargetIndication(SclManager manager, String path, XoObject resource, Indication indication, String flag)
            throws IOException, StorageException, XoException, M2MException {
        String aPoc = resource.getStringAttribute(M2MConstants.TAG_M2M_A_PO_C);
        XoObject aPoCPaths = resource.getXoObjectAttribute(M2MConstants.TAG_M2M_A_PO_C_PATHS);
        if (aPoc == null) {
            sendUnsuccessResponse(manager, indication, StatusCode.STATUS_NOT_FOUND,
                    "Application does not define an aPoC attribute");
        } else if (aPoCPaths == null) {
            sendUnsuccessResponse(manager, indication, StatusCode.STATUS_NOT_FOUND,
                    "Application does not define an aPoCPaths attribute");
        } else {
            XoObject bestAPoCPath = null;
            int currentPathLength = path.length() + 1;
            String targetPath = indication.getTargetPath();
            List aPoCPathList = aPoCPaths.getXoObjectListAttribute(M2MConstants.TAG_M2M_A_PO_C_PATH);
            Iterator it = aPoCPathList.iterator();
            boolean deepApocHandling = (manager.getSclConfig().getAPoCHandling() == Constants.ID_APOC_HANDLING_DEEP);

            XoObject aPoCPath = null;
            int slashOffset = 0;
            String bestPath = null;
            String tmpPath = null;
            while (it.hasNext()) {
                aPoCPath = (XoObject) it.next();
                tmpPath = aPoCPath.getStringAttribute(M2MConstants.TAG_M2M_PATH);
                if (bestPath == null || bestPath.length() < tmpPath.length()) {
                    if (deepApocHandling) {
                        if (targetPath.regionMatches(true, currentPathLength, tmpPath, 0, tmpPath.length())) {
                            slashOffset = currentPathLength + tmpPath.length();
                            if (tmpPath.charAt(tmpPath.length() - 1) == '/'
                                    || (targetPath.length() > slashOffset && targetPath.charAt(slashOffset) == '/')) {
                                bestPath = tmpPath;
                                bestAPoCPath = aPoCPath;
                            }
                        }
                    } else if (tmpPath.charAt(tmpPath.length() - 1) == '/') {
                        if (targetPath.regionMatches(true, currentPathLength, tmpPath, 0, tmpPath.length())) {
                            slashOffset = targetPath.indexOf('/', currentPathLength + tmpPath.length());
                            if (slashOffset == -1 || slashOffset == (targetPath.length() - 1)) {
                                bestPath = tmpPath;
                                bestAPoCPath = aPoCPath;
                            }
                        }
                    } else {
                        if (targetPath.regionMatches(true, currentPathLength, tmpPath, 0, targetPath.length()
                                - currentPathLength)) {
                            bestPath = tmpPath;
                            bestAPoCPath = aPoCPath;
                        }
                    }
                    if (targetPath.regionMatches(true, currentPathLength, tmpPath, 0, tmpPath.length())) {
                        slashOffset = currentPathLength + tmpPath.length();
                        if (tmpPath.charAt(tmpPath.length() - 1) == '/'
                                || (targetPath.length() > slashOffset && targetPath.charAt(slashOffset) == '/')) {
                            bestPath = tmpPath;
                            bestAPoCPath = aPoCPath;
                        }
                    }
                }
            }
            if (bestAPoCPath != null) {
                String accessRightID = bestAPoCPath.getStringAttribute(M2MConstants.TAG_M2M_ACCESS_RIGHT_I_D);
                if (accessRightID != null) {
                    retrieveAndCheckPermissionFromAccessRightID(indication.getTransactionId(), manager, path,
                            indication.getRequestingEntity(), flag, accessRightID);
                } else {
                    getAndCheckAccessRight(indication.getTransactionId(), manager, path, resource,
                            indication.getRequestingEntity(), flag);
                }

                // Build retargetted URI
                int retargettedPathOffset = currentPathLength;
                if (aPoc.charAt(aPoc.length() - 1) != '/') {
                    --retargettedPathOffset;
                }

                URI retargettedUri = URI.create(aPoc + targetPath.substring(retargettedPathOffset));
                M2MContext m2mContext = manager.getM2MContext();
                URI requestingEntity = m2mContext.createLocalUri(retargettedUri, path);
                Request request = indication.createRequest(requestingEntity, retargettedUri);
                request.setAttribute(Constants.AT_INDICATION, indication);
                if (LOG.isInfoEnabled()) {
                    LOG.info(indication.getTransactionId() + ": Retargeting indication to " + retargettedUri.toString()
                            + " with new request " + request.getTransactionId());
                }
                request.send();
            } else {
                sendUnsuccessResponse(manager, indication, StatusCode.STATUS_NOT_FOUND, "No aPoCPath matches the targetID");
            }
        }
    }

    public void checkRights(String logId, SclManager manager, String path, XoObject resource, URI requestingEntity, String flag)
            throws UnsupportedEncodingException, StorageException, XoException, M2MException {
        getAndCheckAccessRight(logId, manager, path, resource, requestingEntity, flag);
    }

    public boolean createResource(SclManager manager, String path, XoObject resource, String targetPath, String appId,
            Date creationDate, String creationTime, XoObject representation, SclTransaction transaction) throws XoException,
            M2MException {
        boolean modified = false;

        // appId: O (response M)
        // expirationTime: O (response M*)
        // accessRightID: O (response O)
        // searchStrings: O (response M)
        // creationTime: NP (response M)
        // lastModifiedTime: NP (response M)
        // announceTo: O (response M*)
        // aPoC: O (response O)
        // aPoCPaths: O (response O)
        // locindicationor: O (response O)
        // containersReference: NP (response M)
        // groupsReference: NP (response M)
        // accessRightsReference: NP (response M)
        // subscriptionsReference: NP (response M)
        // notificationChannelsReference: NP (response M)

        // Check representation
        checkRepresentation(representation, M2MConstants.TAG_M2M_APPLICATION);
        Date recvExpirationTime = getAndCheckDateTime(representation, M2MConstants.TAG_M2M_EXPIRATION_TIME,
                Constants.ID_MODE_OPTIONAL, creationDate.getTime());
        String accessRightID = getAndCheckAccessRightID(manager, representation, M2MConstants.TAG_M2M_ACCESS_RIGHT_I_D,
                Constants.ID_MODE_OPTIONAL, targetPath);
        getAndCheckStringMode(representation, M2MConstants.TAG_M2M_CREATION_TIME, Constants.ID_MODE_FORBIDDEN);
        getAndCheckURI(representation, M2MConstants.TAG_M2M_A_PO_C, Constants.ID_MODE_OPTIONAL);
        Set subPaths = getAndCheckAPoCPaths(manager, path, representation, M2MConstants.TAG_M2M_A_PO_C_PATHS,
                Constants.ID_MODE_OPTIONAL, path + M2MConstants.URI_SEP);
        getAndCheckStringMode(representation, M2MConstants.TAG_M2M_REFERENCE_POINT, Constants.ID_MODE_FORBIDDEN);
        getAndCheckStringMode(representation, M2MConstants.TAG_M2M_POC, Constants.ID_MODE_OPTIONAL);
        getAndCheckStringMode(representation, M2MConstants.TAG_M2M_CONTAINERS_REFERENCE, Constants.ID_MODE_FORBIDDEN);
        getAndCheckStringMode(representation, M2MConstants.TAG_M2M_GROUPS_REFERENCE, Constants.ID_MODE_FORBIDDEN);
        getAndCheckStringMode(representation, M2MConstants.TAG_M2M_ACCESS_RIGHTS_REFERENCE, Constants.ID_MODE_FORBIDDEN);
        getAndCheckStringMode(representation, M2MConstants.TAG_M2M_SUBSCRIPTIONS_REFERENCE, Constants.ID_MODE_FORBIDDEN);
        getAndCheckStringMode(representation, M2MConstants.TAG_M2M_NOTIFICATION_CHANNELS_REFERENCE, Constants.ID_MODE_FORBIDDEN);

        if (subPaths != null) {
            Iterator it = subPaths.iterator();
            while (it.hasNext()) {
                String subPath = (String) it.next();
                if (reservedKeywords.contains(subPath)) {
                    throw new M2MException(subPath
                            + " is a reserved keywork in <application> resource. Cannot use it in an aPoCPath",
                            StatusCode.STATUS_BAD_REQUEST);
                }
            }
        }

        // Update resource
        resource.setNameSpace(M2MConstants.PREFIX_M2M);
        resource.setStringAttribute(M2MConstants.ATTR_APP_ID, appId);
        Date expirationTime = setExpirationTime(manager, resource, creationDate.getTime(), recvExpirationTime);
        if (recvExpirationTime != expirationTime) {
            modified = true;
        }
        createStringOptional(resource, representation, M2MConstants.TAG_M2M_ACCESS_RIGHT_I_D);
        createXoObjectMandatory(manager, resource, representation, M2MConstants.TAG_M2M_SEARCH_STRINGS);
        resource.setStringAttribute(M2MConstants.TAG_M2M_CREATION_TIME, creationTime);
        resource.setStringAttribute(M2MConstants.TAG_M2M_LAST_MODIFIED_TIME, creationTime);
        if (resource.containsAttribute(M2MConstants.TAG_M2M_ANNOUNCE_TO)) {
            modified = true;
        }
        resource.setXoObjectAttribute(M2MConstants.TAG_M2M_ANNOUNCE_TO,
                manager.getXoService().newXmlXoObject(M2MConstants.TAG_M2M_ANNOUNCE_TO));
        createStringOptional(resource, representation, M2MConstants.TAG_M2M_A_PO_C);
        createXoObjectOptional(resource, representation, M2MConstants.TAG_M2M_A_PO_C_PATHS);
        createStringOptional(resource, representation, M2MConstants.TAG_M2M_LOC_REQUESTOR);
        resource.setStringAttribute(M2MConstants.TAG_M2M_REFERENCE_POINT, M2MConstants.REFERENCE_POINT_DIA);

        // Create sub-resources
        Containers.getInstance().createResource(manager, path + M2MConstants.URI_SEP + M2MConstants.RES_CONTAINERS,
                creationDate, creationTime, accessRightID, transaction);
        AccessRights.getInstance().createResource(manager, path + M2MConstants.URI_SEP + M2MConstants.RES_ACCESS_RIGHTS,
                creationDate, creationTime, accessRightID, transaction);
        Subscriptions.getInstance().createResource(manager, path + M2MConstants.URI_SEP + M2MConstants.RES_SUBSCRIPTIONS,
                transaction);
        NotificationChannels.getInstance().createResource(manager,
                path + M2MConstants.URI_SEP + M2MConstants.RES_NOTIFICATION_CHANNELS, creationDate, creationTime, transaction);

        // Save resource
        Document document = manager.getStorageContext().getStorageFactory().createDocument(path);
        document.setAttribute(Constants.ATTR_TYPE, Constants.TYPE_APPLICATION);
        XoObject searchStrings = resource.getXoObjectAttribute(M2MConstants.TAG_M2M_SEARCH_STRINGS);
        List searchStringList = searchStrings.getStringListAttribute(M2MConstants.TAG_M2M_SEARCH_STRING);
        document.setAttribute(M2MConstants.ATTR_SEARCH_STRING, new ArrayList(searchStringList));
        document.setAttribute(M2MConstants.ATTR_CREATION_TIME, creationDate);
        document.setAttribute(M2MConstants.ATTR_LAST_MODIFIED_TIME, creationDate);
        document.setContent(resource.saveBinary());
        transaction.createResource(document);

        transaction.addTransientOp(new ExpirationTimerUpdateOp(manager, path, Constants.ID_RES_APPLICATION, expirationTime
                .getTime() - creationDate.getTime()));

        return modified;
    }

    public boolean updateResource(String logId, SclManager manager, String path, XoObject resource, Indication indication,
            XoObject representation, Date now) throws ParseException, XoException, StorageException, M2MException {
        boolean modified = false;
        // appId: NP (response M)
        // expirationTime: O (response M*)
        // accessRightID: O (response O)
        // searchStrings: O (response M)
        // creationTime: NP (response M)
        // lastModifiedTime: NP (response M)
        // announceTo: O (response M*)
        // aPoC: O (response O)
        // aPoCPaths: O (response O)
        // locindicationor: O (response O)
        // containersReference: NP (response M)
        // groupsReference: NP (response M)
        // accessRightsReference: NP (response M)
        // subscriptionsReference: NP (response M)
        // notificationChannelsReference: NP (response M)

        // Check representation
        checkRepresentation(representation, M2MConstants.TAG_M2M_APPLICATION);
        getAndCheckStringMode(representation, M2MConstants.ATTR_APP_ID, Constants.ID_MODE_FORBIDDEN);
        Date recvExpirationTime = getAndCheckDateTime(representation, M2MConstants.TAG_M2M_EXPIRATION_TIME,
                Constants.ID_MODE_OPTIONAL, now.getTime());
        getAndCheckAccessRightID(manager, representation, M2MConstants.TAG_M2M_ACCESS_RIGHT_I_D, Constants.ID_MODE_OPTIONAL,
                indication.getTargetPath());
        getAndCheckStringMode(representation, M2MConstants.TAG_M2M_CREATION_TIME, Constants.ID_MODE_FORBIDDEN);
        getAndCheckURI(representation, M2MConstants.TAG_M2M_A_PO_C, Constants.ID_MODE_OPTIONAL);
        Set subPaths = getAndCheckAPoCPaths(manager, path, representation, M2MConstants.TAG_M2M_A_PO_C_PATHS,
                Constants.ID_MODE_OPTIONAL, path + M2MConstants.URI_SEP);
        getAndCheckStringMode(representation, M2MConstants.TAG_M2M_CONTAINERS_REFERENCE, Constants.ID_MODE_FORBIDDEN);
        getAndCheckStringMode(representation, M2MConstants.TAG_M2M_GROUPS_REFERENCE, Constants.ID_MODE_FORBIDDEN);
        getAndCheckStringMode(representation, M2MConstants.TAG_M2M_ACCESS_RIGHTS_REFERENCE, Constants.ID_MODE_FORBIDDEN);
        getAndCheckStringMode(representation, M2MConstants.TAG_M2M_SUBSCRIPTIONS_REFERENCE, Constants.ID_MODE_FORBIDDEN);
        getAndCheckStringMode(representation, M2MConstants.TAG_M2M_NOTIFICATION_CHANNELS_REFERENCE, Constants.ID_MODE_FORBIDDEN);

        if (subPaths != null) {
            Iterator it = subPaths.iterator();
            while (it.hasNext()) {
                String subPath = (String) it.next();
                if (reservedKeywords.contains(subPath)) {
                    throw new M2MException(subPath
                            + " is a reserved keywork in <application> resource. Cannot use it in an aPoCPath",
                            StatusCode.STATUS_BAD_REQUEST);
                }
            }
        }

        // Update resource
        Date expirationTime = setExpirationTime(manager, resource, now.getTime(), recvExpirationTime);
        if (recvExpirationTime != expirationTime) {
            modified = true;
        }
        updateStringOptional(resource, representation, M2MConstants.TAG_M2M_ACCESS_RIGHT_I_D);
        updateXoObjectMandatory(manager, resource, representation, M2MConstants.TAG_M2M_SEARCH_STRINGS);
        updateLastModifiedTime(manager, resource, now);
        if (representation.containsAttribute(M2MConstants.TAG_M2M_ANNOUNCE_TO)) {
            modified = true;
        }
        updateStringOptional(resource, representation, M2MConstants.TAG_M2M_A_PO_C);
        updateXoObjectOptional(resource, representation, M2MConstants.TAG_M2M_A_PO_C_PATHS);
        updateStringOptional(resource, representation, M2MConstants.TAG_M2M_LOC_REQUESTOR);

        // Save resource
        Document document = manager.getStorageContext().getStorageFactory().createDocument(path);
        document.setAttribute(Constants.ATTR_TYPE, Constants.TYPE_APPLICATION);
        XoObject searchStrings = resource.getXoObjectAttribute(M2MConstants.TAG_M2M_SEARCH_STRINGS);
        List searchStringList = searchStrings.getStringListAttribute(M2MConstants.TAG_M2M_SEARCH_STRING);
        document.setAttribute(M2MConstants.ATTR_SEARCH_STRING, new ArrayList(searchStringList));
        document.setAttribute(M2MConstants.ATTR_CREATION_TIME, FormatUtils.parseDateTime(resource
                .getStringAttribute(M2MConstants.TAG_M2M_CREATION_TIME)));
        document.setAttribute(M2MConstants.ATTR_LAST_MODIFIED_TIME, now);
        document.setContent(resource.saveBinary());
        manager.getStorageContext().update(null, document, null);

        new ExpirationTimerUpdateOp(manager, path, Constants.ID_RES_APPLICATION, expirationTime.getTime() - now.getTime())
                .postCommit();

        return modified;
    }

    public void deleteResource(String logId, SclManager manager, String path, SclTransaction transaction)
            throws UnsupportedEncodingException, StorageException, XoException, M2MException {
        if (LOG.isInfoEnabled()) {
            LOG.info("Delete <application> resource on path: " + path);
        }
        Containers.getInstance().deleteResource(logId, manager, path + M2MConstants.URI_SEP + M2MConstants.RES_CONTAINERS,
                transaction);
        AccessRights.getInstance().deleteResource(logId, manager, path + M2MConstants.URI_SEP + M2MConstants.RES_ACCESS_RIGHTS,
                transaction);
        Subscriptions.getInstance().deleteResource(logId, manager,
                path + M2MConstants.URI_SEP + M2MConstants.RES_SUBSCRIPTIONS, transaction);
        NotificationChannels.getInstance().deleteResource(logId, manager,
                path + M2MConstants.URI_SEP + M2MConstants.RES_NOTIFICATION_CHANNELS, transaction);

        transaction.deleteResource(path);
    }

    public void deleteResource(String logId, SclManager manager, String path, XoObject resource, SclTransaction transaction)
            throws UnsupportedEncodingException, StorageException, XoException, M2MException {
        deleteResource(logId, manager, path, transaction);
        transaction.addTransientOp(new ExpirationTimerDeleteOp(manager, path));
    }

    public void deleteChildResource(String logId, SclManager manager, String path, XoObject resource, XoObject childResource,
            Date now, SclTransaction transaction) {
        throw new UnsupportedOperationException();
    }

    public void prepareResourceForResponse(String logId, SclManager manager, URI requestingEntity, String path, XoObject resource,
            FilterCriteria filterCriteria, Set supported) throws M2MException {
        String appPath = manager.getM2MContext().getApplicationPath();
        String accessRight = resource.getStringAttribute(M2MConstants.TAG_M2M_ACCESS_RIGHT_I_D);
        if (accessRight != null) {
            resource.setStringAttribute(M2MConstants.TAG_M2M_ACCESS_RIGHT_I_D, appPath + URIUtils.encodePath(accessRight)
                    + M2MConstants.URI_SEP);
        }
        getAndFixAPoCPaths(manager, path, resource, M2MConstants.TAG_M2M_A_PO_C_PATHS);

        // APoc
        String requestingEntityStr = requestingEntity.toString();
        String localSclBase = null;
        if (requestingEntityStr.charAt(requestingEntityStr.length() - 1) == '/') {
            localSclBase = manager.getM2MContext().createLocalUri(requestingEntity, M2MConstants.URI_SEP).toString();
        } else {
            localSclBase = manager.getM2MContext().createLocalUri(requestingEntity, Constants.PATH_ROOT).toString();
        }
        if (!localSclBase.equals(requestingEntityStr)) {
            String localResourceUri = manager.getM2MContext().createLocalUri(requestingEntity, path).toString();
            if (!localResourceUri.equals(requestingEntityStr)) {
                // Remove aPoc
                resource.setStringAttribute(M2MConstants.TAG_M2M_A_PO_C, null);
            }
        }

        String encodedPath = URIUtils.encodePath(path);
        resource.setStringAttribute(M2MConstants.TAG_M2M_CONTAINERS_REFERENCE, appPath + encodedPath + M2MConstants.URI_SEP
                + M2MConstants.RES_CONTAINERS + M2MConstants.URI_SEP);
        resource.setStringAttribute(M2MConstants.TAG_M2M_GROUPS_REFERENCE, appPath + encodedPath + M2MConstants.URI_SEP
                + M2MConstants.RES_GROUPS + M2MConstants.URI_SEP);
        resource.setStringAttribute(M2MConstants.TAG_M2M_ACCESS_RIGHTS_REFERENCE, appPath + encodedPath + M2MConstants.URI_SEP
                + M2MConstants.RES_ACCESS_RIGHTS + M2MConstants.URI_SEP);
        resource.setStringAttribute(M2MConstants.TAG_M2M_SUBSCRIPTIONS_REFERENCE, appPath + encodedPath + M2MConstants.URI_SEP
                + M2MConstants.RES_SUBSCRIPTIONS + M2MConstants.URI_SEP);
        resource.setStringAttribute(M2MConstants.TAG_M2M_NOTIFICATION_CHANNELS_REFERENCE, appPath + encodedPath
                + M2MConstants.URI_SEP + M2MConstants.RES_NOTIFICATION_CHANNELS + M2MConstants.URI_SEP);
    }

    public void doCreateIndication(SclManager manager, String path, XoObject resource, Indication indication,
            PartialAccessor partialAccessor) throws ParseException, IOException, StorageException, XoException, M2MException {
        if (partialAccessor.getAttributeId() > 0) {
            super.doCreateIndication(manager, path, resource, indication, partialAccessor);
        } else {
            // Re-targetting
            SclLogger.logRequestIndication("applicationUpdateRequestIndication", "applicationUpdateResponseConfirm",
                    indication, null, Constants.ID_LOG_RAW_REPRESENTATION);
            retargetIndication(manager, path, resource, indication, M2MConstants.FLAG_WRITE);
        }
    }

    public void doRetrieveIndication(SclManager manager, String path, XoObject resource, Indication indication,
            PartialAccessor partialAccessor) throws ParseException, IOException, StorageException, XoException, M2MException {
        if (partialAccessor.getAttributeId() > 0) {
            super.doRetrieveIndication(manager, path, resource, indication, partialAccessor);
        } else {
            // Re-targetting
            SclLogger.logRequestIndication("applicationRetrieveRequestIndication", "applicationRetrieveResponseConfirm",
                    indication, null, 0);
            retargetIndication(manager, path, resource, indication, M2MConstants.FLAG_READ);
        }
    }

    public void doUpdateIndication(SclManager manager, String path, XoObject resource, Indication indication,
            PartialAccessor partialAccessor) throws ParseException, IOException, StorageException, XoException, M2MException {
        if (partialAccessor.getAttributeId() > 0) {
            super.doUpdateIndication(manager, path, resource, indication, partialAccessor);
        } else {
            // Re-targetting
            SclLogger.logRequestIndication("applicationUpdateRequestIndication", "applicationUpdateResponseConfirm",
                    indication, null, Constants.ID_LOG_RAW_REPRESENTATION);
            retargetIndication(manager, path, resource, indication, M2MConstants.FLAG_WRITE);
        }
    }

    public void doDeleteIndication(SclManager manager, String path, XoObject resource, Indication indication,
            PartialAccessor partialAccessor) throws ParseException, IOException, StorageException, XoException, M2MException {
        if (partialAccessor.getAttributeId() > 0) {
            super.doDeleteIndication(manager, path, resource, indication, partialAccessor);
        } else {
            // Re-targetting
            SclLogger.logRequestIndication("applicationUpdateRequestIndication", "applicationUpdateResponseConfirm",
                    indication, null, Constants.ID_LOG_RAW_REPRESENTATION);
            retargetIndication(manager, path, resource, indication, M2MConstants.FLAG_WRITE);
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

            // APoCPaths handling
            XoObject aPoCPaths = resource.getXoObjectAttribute(M2MConstants.TAG_M2M_A_PO_C_PATHS);
            if (aPoCPaths != null) {
                List aPoCPathList = aPoCPaths.getXoObjectListAttribute(M2MConstants.TAG_M2M_A_PO_C_PATH);

                String aPoCPathPath = null;
                String aPoCPathAccessRightID = null;
                XoObject aPoCPathSearchStrings = null;
                List aPoCPathSearchString = null;
                boolean match = true;
                Iterator it = aPoCPathList.iterator();
                XoObject aPoCPath = null;
                while (it.hasNext()) {
                    aPoCPath = (XoObject) it.next();
                    try {
                        aPoCPathPath = aPoCPath.getStringAttribute(M2MConstants.TAG_M2M_PATH);
                        // Check rights
                        aPoCPathAccessRightID = aPoCPath.getStringAttribute(M2MConstants.TAG_M2M_ACCESS_RIGHT_I_D);
                        if (aPoCPathAccessRightID != null) {
                            retrieveAndCheckPermissionFromAccessRightID(logId, manager, path, requestingEntity,
                                    M2MConstants.FLAG_DISCOVER, aPoCPathAccessRightID);
                        }
                        match = true;
                        if (searchStrings != null) {
                            // Check searchStrings
                            aPoCPathSearchStrings = aPoCPath.getXoObjectAttribute(M2MConstants.TAG_M2M_SEARCH_STRINGS);
                            aPoCPathSearchString = null;
                            if (aPoCPathSearchStrings != null) {
                                aPoCPathSearchString = aPoCPathSearchStrings
                                        .getStringListAttribute(M2MConstants.TAG_M2M_SEARCH_STRING);
                            }
                            if (aPoCPathSearchString != null) {
                                for (int i = 0; i < searchStrings.length; ++i) {
                                    if (!aPoCPathSearchString.contains(searchStrings[i])) {
                                        match = false;
                                        break;
                                    }
                                }
                            }
                        }
                        if (match) {
                            if (urisCount > 0) {
                                discoveryURIs.add(appPath + URIUtils.encodePath(path) + M2MConstants.URI_SEP + aPoCPathPath);
                            }
                            --urisCount;
                        }
                    } catch (M2MException e) {
                        // Right is not granted
                    }
                }
            }
        } catch (M2MException e) {
            // Right is not granted
        }
        return urisCount;
    }
}
