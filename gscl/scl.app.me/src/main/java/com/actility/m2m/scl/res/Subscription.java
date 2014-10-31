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
 * id $Id: Subscription.java 4140 2013-01-15 14:08:57Z JReich $
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
import java.util.List;
import java.util.Set;

import org.apache.log4j.Logger;

import com.actility.m2m.m2m.FilterCriteria;
import com.actility.m2m.m2m.Indication;
import com.actility.m2m.m2m.M2MConstants;
import com.actility.m2m.m2m.M2MException;
import com.actility.m2m.m2m.StatusCode;
import com.actility.m2m.scl.log.BundleLogger;
import com.actility.m2m.scl.mem.SubCreateOp;
import com.actility.m2m.scl.mem.SubDeleteOp;
import com.actility.m2m.scl.mem.SubManager;
import com.actility.m2m.scl.mem.SubUpdateOp;
import com.actility.m2m.scl.mem.SubsManager;
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
 * M2M Subscription. Defines a Subscription on a resource and its behavior.
 *
 * <pre>
 * m2m:Subscription from ong:t_xml_obj
 * {
 *     m2m:id    XoString    { embattr } // (optional) (xmlType: xsd:NMTOKEN)
 *     m2m:expirationTime    XoString    { } // (optional) (xmlType: xsd:dateTime)
 *     m2m:minimalTimeBetweenNotifications    XoString    { } // (xmlType: xsd:long)
 *     m2m:delayTolerance    XoString    { } // (xmlType: xsd:dateTime)
 *     m2m:creationTime    XoString    { } // (optional) (xmlType: xsd:dateTime)
 *     m2m:lastModifiedTime    XoString    { } // (optional) (xmlType: xsd:dateTime)
 *     m2m:filterCriteria    XoVoidObj    { default=m2m:FilterCriteriaType } // (optional)
 *     m2m:subscriptionType    XoString    { } // (optional) (xmlType: m2m:SubscriptionType) (enum: ASYNCHRONOUS SYNCHRONOUS )
 *     m2m:contact    XoString    { } // (optional) (xmlType: xsd:anyURI)
 *     m2m:aggregateURI    XoString    { } // (optional) (xmlType: xsd:anyURI)
 *     m2m:timeoutReason    XoString    { } // (optional) (xmlType: xsd:string)
 *     m2m:noRepresentation    XoString    { } // (optional) (xmlType: xsd:boolean)
 *     m2m:subscriberId    XoString    { } // (optional) (xmlType: xsd:anyURI)
 *     m2m:maxAccuCreates    XoString    { } // (optional) (xmlType: xsd:long)
 *     m2m:currentAccuCreates    XoString    { } // (optional) (xmlType: xsd:long)
 * }
 * alias m2m:Subscription with m2m:subscription
 * </pre>
 */
public final class Subscription extends SclResource implements VolatileResource {
    private static final Logger LOG = OSGiLogger.getLogger(Subscription.class, BundleLogger.getStaticLogger());

    private static final Subscription INSTANCE = new Subscription();

    public static Subscription getInstance() {
        return INSTANCE;
    }

    private Subscription() {
        // Cannot be instantiated
        super(false, "subscriptionRetrieveRequestIndication", "subscriptionRetrieveResponseConfirm",
                "subscriptionUpdateRequestIndication", "subscriptionUpdateResponseConfirm",
                "subscriptionDeleteRequestIndication", "subscriptionDeleteResponseConfirm",
                Constants.ID_NO_FILTER_CRITERIA_MODE, true, false, null, false, false);
    }

    private String getSubsManager(SclManager manager, String path, XoObject resource) {
        // Subscription manager
        String subsPath = getParentPath(path);
        XoObject filterCriteria = resource.getXoObjectAttribute(M2MConstants.TAG_M2M_FILTER_CRITERIA);
        if (filterCriteria != null) {
            String attributeAccessor = filterCriteria.getStringAttribute(M2MConstants.TAG_ATTRIBUTE_ACCESSOR);
            if (SubCreateOp.LATEST_URI.equals(attributeAccessor) || M2MConstants.ATTR_LATEST.equals(attributeAccessor)) {
                subsPath += M2MConstants.URI_SEP + M2MConstants.ATTR_LATEST;
            }
        }
        return subsPath;
    }

    public void reload(SclManager manager, String path, XoObject resource, SclTransaction transaction) throws XoException,
            StorageException, M2MException {
        if (LOG.isInfoEnabled()) {
            LOG.info("Reload volatile <subscription>: " + path);
        }

        XoObject subscribedResource = null;
        try {
            String subsPath = getParentPath(path);
            String subscribedPath = getParentPath(subsPath);
            Object[] controllerAndResource = manager.getControllerAndResource(subscribedPath);
            SubscribedResource subscribedController = (SubscribedResource) controllerAndResource[0];
            subscribedResource = (XoObject) controllerAndResource[1];

            FilterCriteria filterCriteria = null;
            if (subscribedController.getClass() == ContentInstances.class) {
                filterCriteria = getAndCheckContentInstanceFilterCriteriaType(resource, M2MConstants.TAG_M2M_FILTER_CRITERIA,
                        Constants.ID_MODE_OPTIONAL);
            } else {
                filterCriteria = getAndCheckFilterCriteriaType(resource, M2MConstants.TAG_M2M_FILTER_CRITERIA,
                        Constants.ID_MODE_OPTIONAL);
            }

            if (subscribedController.getClass() == ContentInstances.class
                    && filterCriteria != null
                    && (SubCreateOp.LATEST_URI.equals(filterCriteria.getAttributeAccessor()) || M2MConstants.ATTR_LATEST
                            .equals(filterCriteria.getAttributeAccessor()))) {
                subsPath += M2MConstants.URI_SEP + M2MConstants.ATTR_LATEST;
                subscribedController = Latest.getInstance();
            }

            SubsManager subsManager = (SubsManager) manager.getM2MContext().getAttribute(subsPath);

            // Subscription manager
            if (subsManager == null) {
                subsManager = new SubsManager(manager, subscribedController, subsPath, subscribedPath);
                manager.getM2MContext().setAttribute(subsPath, subsManager);
            }
            String subId = resource.getStringAttribute(M2MConstants.ATTR_M2M_ID);
            // TODO cannot restore mediaType
            SubManager subManager = new SubManager(subsManager, manager, path, subId, subscribedController, subscribedPath,
                    resource, filterCriteria, M2MConstants.MT_APPLICATION_XML);
            if (subsManager.addSubscription(subId, subManager)) {
                subManager.startExpirationTimer();
            } else {
                if (LOG.isInfoEnabled()) {
                    LOG.info("Too much pending subscriptions, delete: " + path);
                }
                subManager.subscriptionDeleted();
            }
        } finally {
            if (subscribedResource != null) {
                subscribedResource.free(true);
            }
        }
    }

    public void checkRights(String logId, SclManager manager, String path, XoObject resource, URI requestingEntity, String flag)
            throws UnsupportedEncodingException, StorageException, XoException, M2MException {
        if (!requestingEntity.toString().equals(resource.getStringAttribute(M2MConstants.TAG_M2M_SUBSCRIBER_ID))) {
            if (M2MConstants.FLAG_READ.equals(flag)) {
                XoObject grandParentResource = null;
                String grandParentPath = getGrandParentPath(path);
                try {
                    Object[] controllerAndResource = manager.getControllerAndResource(grandParentPath);
                    SclResource grandParentController = (SclResource) controllerAndResource[0];
                    grandParentResource = (XoObject) controllerAndResource[1];
                    grandParentController.checkRights(logId, manager, grandParentPath, grandParentResource, requestingEntity,
                            M2MConstants.FLAG_DELETE);
                } finally {
                    if (grandParentResource != null) {
                        grandParentResource.free(true);
                    }
                }
            } else {
                throw new M2MException("Resource does not exist", StatusCode.STATUS_NOT_FOUND);
            }
        }
    }

    /**
     * <pre>
     * expirationTime:                  O  (response M*)
     * minimalTimeBetweenNotifications: O  (response O)
     * delayTolerance:                  O  (response O)
     * creationTime:                    NP (response M)
     * lastModifiedTime:                NP (response M)
     * filterCriteria:                  O  (response O)
     * subscriptionType:                NP (response M)
     * contact:                         M  (response M)
     * id:                              O  (response M*)
     * aggregateURI                     O  (response O)
     * timeoutReason                    O  (response O)
     * noRepresentation                 O  (response O)
     * subscriberId                     NP (response NP)
     * maxAccuCreates                   O  (response O)
     * currentAccuCreates               NP (response O)
     * </pre>
     *
     * @param manager
     * @param subscription
     * @param subsPath
     * @param requestingEntity
     * @param id
     * @param creationDate
     * @param representation
     * @param transaction
     * @throws XoException
     * @throws M2MException
     */
    public boolean createResource(SclManager manager, String path, XoObject resource, String subsPath, URI requestingEntity,
            String id, Date creationDate, String creationTime, String contentType, XoObject representation,
            String subscribedPath, SubscribedResource subscribedResource, SclTransaction transaction) throws XoException,
            M2MException {
        boolean modified = false;
        FilterCriteria filterCriteria = null;

        // Check representation
        checkRepresentation(representation, M2MConstants.TAG_M2M_SUBSCRIPTION);
        Date recvExpirationTime = getAndCheckDateTime(representation, M2MConstants.TAG_M2M_EXPIRATION_TIME,
                Constants.ID_MODE_OPTIONAL, creationDate.getTime());
        if (getAndCheckLong(representation, M2MConstants.TAG_M2M_MINIMAL_TIME_BETWEEN_NOTIFICATIONS, Constants.ID_MODE_OPTIONAL) >= 0
                && getAndCheckDateTime(representation, M2MConstants.TAG_M2M_DELAY_TOLERANCE, Constants.ID_MODE_OPTIONAL, -1L) != null) {
            throw new M2MException(M2MConstants.TAG_M2M_MINIMAL_TIME_BETWEEN_NOTIFICATIONS + " and "
                    + M2MConstants.TAG_M2M_DELAY_TOLERANCE + " cannot be used simultaneously in a subscription",
                    StatusCode.STATUS_BAD_REQUEST);
        }
        getAndCheckStringMode(representation, M2MConstants.TAG_M2M_CREATION_TIME, Constants.ID_MODE_FORBIDDEN);
        getAndCheckStringMode(representation, M2MConstants.TAG_M2M_LAST_MODIFIED_TIME, Constants.ID_MODE_FORBIDDEN);
        if (manager.getDefaultResourceId(getGrandParentPath(path)) == Constants.ID_RES_CONTENT_INSTANCES) {
            filterCriteria = getAndCheckContentInstanceFilterCriteriaType(representation, M2MConstants.TAG_M2M_FILTER_CRITERIA,
                    Constants.ID_MODE_OPTIONAL);
        } else {
            filterCriteria = getAndCheckFilterCriteriaType(representation, M2MConstants.TAG_M2M_FILTER_CRITERIA,
                    Constants.ID_MODE_OPTIONAL);
        }
        getAndCheckStringMode(representation, M2MConstants.TAG_M2M_SUBSCRIPTION_TYPE, Constants.ID_MODE_FORBIDDEN);
        URI contactUri = getAndCheckReachableURI(representation, M2MConstants.TAG_M2M_CONTACT, Constants.ID_MODE_REQUIRED);
        getAndCheckURI(representation, M2MConstants.TAG_M2M_AGGREGATE_U_R_I, Constants.ID_MODE_OPTIONAL);
        getAndCheckBoolean(representation, M2MConstants.TAG_M2M_NO_REPRESENTATION, Constants.ID_MODE_OPTIONAL);
        getAndCheckStringMode(representation, M2MConstants.TAG_M2M_SUBSCRIBER_ID, Constants.ID_MODE_FORBIDDEN);
        getAndCheckLong(representation, M2MConstants.TAG_M2M_MAX_ACCU_CREATES, Constants.ID_MODE_OPTIONAL);
        getAndCheckLong(representation, M2MConstants.TAG_M2M_CURRENT_ACCU_CREATES, Constants.ID_MODE_FORBIDDEN);

        // Update resource
        resource.setNameSpace(M2MConstants.PREFIX_M2M);
        resource.setNameSpace(M2MConstants.PREFIX_XSI);
        resource.setStringAttribute(M2MConstants.ATTR_M2M_ID, id);
        Date expirationTime = setExpirationTime(manager, resource, creationDate.getTime(), recvExpirationTime);
        if (recvExpirationTime != expirationTime) {
            modified = true;
        }
        createStringOptional(resource, representation, M2MConstants.TAG_M2M_MINIMAL_TIME_BETWEEN_NOTIFICATIONS);
        createStringOptional(resource, representation, M2MConstants.TAG_M2M_DELAY_TOLERANCE);
        resource.setStringAttribute(M2MConstants.TAG_M2M_CREATION_TIME, creationTime);
        resource.setStringAttribute(M2MConstants.TAG_M2M_LAST_MODIFIED_TIME, creationTime);
        createXoObjectOptional(resource, representation, M2MConstants.TAG_M2M_FILTER_CRITERIA);

        String baseContactUri = manager.createPocUriFromServerRoot(contactUri, "/http/contact/").toString();
        String contactUriStr = contactUri.normalize().toString();
        String contactLocalPath = null;
        if (contactUriStr.startsWith(baseContactUri)) {
            contactLocalPath = contactUriStr.substring(baseContactUri.length());
        }
        if (contactLocalPath != null && manager.getDefaultResourceId(contactLocalPath) == Constants.ID_RES_NOTIFICATION_CHANNEL) {
            resource.setStringAttribute(M2MConstants.TAG_M2M_SUBSCRIPTION_TYPE, M2MConstants.SUBSCRIPTION_TYPE_SYNCHRONOUS);
        } else {
            resource.setStringAttribute(M2MConstants.TAG_M2M_SUBSCRIPTION_TYPE, M2MConstants.SUBSCRIPTION_TYPE_ASYNCHRONOUS);
        }

        createStringMandatory(resource, representation, M2MConstants.TAG_M2M_CONTACT, null);
        createStringOptional(resource, representation, M2MConstants.TAG_M2M_AGGREGATE_U_R_I);
        createStringOptional(resource, representation, M2MConstants.TAG_M2M_TIMEOUT_REASON);
        createStringOptional(resource, representation, M2MConstants.TAG_M2M_NO_REPRESENTATION);
        resource.setStringAttribute(M2MConstants.TAG_M2M_SUBSCRIBER_ID, requestingEntity.toString());
        createStringOptional(resource, representation, M2MConstants.TAG_M2M_MAX_ACCU_CREATES);
        if (resource.containsAttribute(M2MConstants.TAG_M2M_MAX_ACCU_CREATES)) {
            resource.setStringAttribute(M2MConstants.TAG_M2M_CURRENT_ACCU_CREATES, "0");
        }

        // Save resource
        String mediaType = M2MConstants.MT_APPLICATION_XML;
        if (M2MConstants.MT_APPLICATION_EXI.equals(contentType)) {
            mediaType = M2MConstants.MT_APPLICATION_EXI;
        }
        Document document = manager.getStorageContext().getStorageFactory().createDocument(path);
        document.setAttribute(Constants.ATTR_TYPE, Constants.TYPE_SUBSCRIPTION);
        document.setAttribute(M2MConstants.ATTR_CREATION_TIME, creationDate);
        document.setAttribute(M2MConstants.ATTR_LAST_MODIFIED_TIME, creationDate);
        document.setAttribute(Constants.ATTR_MEDIA_TYPE, mediaType);
        document.setContent(resource.saveBinary());
        transaction.createResource(document);
        transaction.addTransientOp(new SubCreateOp(manager, subsPath, path, id, resource, subscribedPath, subscribedResource,
                filterCriteria, mediaType));

        return modified;
    }

    /**
     * <pre>
     * expirationTime:                  O  (response M*)
     * minimalTimeBetweenNotifications: O  (response O)
     * delayTolerance:                  O  (response O)
     * creationTime:                    NP (response M)
     * lastModifiedTime:                NP (response M)
     * filterCriteria:                  NP (response O)
     * subscriptionType:                NP (response M)
     * contact:                         NP (response M)
     * id:                              NP (response M*)
     * aggregateURI                     O  (response O)
     * timeoutReason                    O  (response O)
     * noRepresentation                 O  (response O)
     * subscriberId                     NP (response NP)
     * maxAccuCreates                   O  (response O)
     * currentAccuCreates               NP (response O)
     * </pre>
     */
    public boolean updateResource(String logId, SclManager manager, String path, XoObject resource, Indication indication,
            XoObject representation, Date now) throws ParseException, UnsupportedEncodingException, XoException,
            StorageException, M2MException {
        boolean modified = false;

        // Check representation
        checkRepresentation(representation, M2MConstants.TAG_M2M_SUBSCRIPTION);
        getAndCheckStringMode(representation, M2MConstants.ATTR_M2M_ID, Constants.ID_MODE_FORBIDDEN);
        Date recvExpirationTime = getAndCheckDateTime(representation, M2MConstants.TAG_M2M_EXPIRATION_TIME,
                Constants.ID_MODE_OPTIONAL, now.getTime());
        if (getAndCheckLong(representation, M2MConstants.TAG_M2M_MINIMAL_TIME_BETWEEN_NOTIFICATIONS, Constants.ID_MODE_OPTIONAL) >= 0
                && getAndCheckDateTime(representation, M2MConstants.TAG_M2M_DELAY_TOLERANCE, Constants.ID_MODE_OPTIONAL, -1L) != null) {
            throw new M2MException(M2MConstants.TAG_M2M_MINIMAL_TIME_BETWEEN_NOTIFICATIONS + " and "
                    + M2MConstants.TAG_M2M_DELAY_TOLERANCE + " cannot be used simultaneously in a subscription",
                    StatusCode.STATUS_BAD_REQUEST);
        }
        getAndCheckStringMode(representation, M2MConstants.TAG_M2M_CREATION_TIME, Constants.ID_MODE_FORBIDDEN);
        getAndCheckStringMode(representation, M2MConstants.TAG_M2M_LAST_MODIFIED_TIME, Constants.ID_MODE_FORBIDDEN);
        getAndCheckXoObjectMode(representation, M2MConstants.TAG_M2M_FILTER_CRITERIA, Constants.ID_MODE_FORBIDDEN);
        getAndCheckStringMode(representation, M2MConstants.TAG_M2M_SUBSCRIPTION_TYPE, Constants.ID_MODE_FORBIDDEN);
        getAndCheckStringMode(representation, M2MConstants.TAG_M2M_CONTACT, Constants.ID_MODE_FORBIDDEN);
        getAndCheckURI(representation, M2MConstants.TAG_M2M_AGGREGATE_U_R_I, Constants.ID_MODE_OPTIONAL);
        getAndCheckBoolean(representation, M2MConstants.TAG_M2M_NO_REPRESENTATION, Constants.ID_MODE_OPTIONAL);
        getAndCheckStringMode(representation, M2MConstants.TAG_M2M_SUBSCRIBER_ID, Constants.ID_MODE_FORBIDDEN);
        getAndCheckLong(representation, M2MConstants.TAG_M2M_MAX_ACCU_CREATES, Constants.ID_MODE_OPTIONAL);
        getAndCheckLong(representation, M2MConstants.TAG_M2M_CURRENT_ACCU_CREATES, Constants.ID_MODE_FORBIDDEN);

        // Update resource
        Date expirationTime = setExpirationTime(manager, resource, now.getTime(), recvExpirationTime);
        if (recvExpirationTime != expirationTime) {
            modified = true;
        }
        updateStringOptional(resource, representation, M2MConstants.TAG_M2M_MINIMAL_TIME_BETWEEN_NOTIFICATIONS);
        updateStringOptional(resource, representation, M2MConstants.TAG_M2M_DELAY_TOLERANCE);
        updateLastModifiedTime(manager, resource, now);
        createStringOptional(resource, representation, M2MConstants.TAG_M2M_AGGREGATE_U_R_I);
        createStringOptional(resource, representation, M2MConstants.TAG_M2M_TIMEOUT_REASON);
        createStringOptional(resource, representation, M2MConstants.TAG_M2M_NO_REPRESENTATION);
        createStringOptional(resource, representation, M2MConstants.TAG_M2M_MAX_ACCU_CREATES);

        // Save resource
        SclTransaction transaction = new SclTransaction(manager.getStorageContext());

        String mediaType = M2MConstants.MT_APPLICATION_XML;
        if (M2MConstants.MT_APPLICATION_EXI.equals(indication.getRawContentType())) {
            mediaType = M2MConstants.MT_APPLICATION_EXI;
        }
        Document document = manager.getStorageContext().getStorageFactory().createDocument(path);
        document.setAttribute(Constants.ATTR_TYPE, Constants.TYPE_SUBSCRIPTION);
        document.setAttribute(M2MConstants.ATTR_CREATION_TIME,
                FormatUtils.parseDateTime(resource.getStringAttribute(M2MConstants.TAG_M2M_CREATION_TIME)));
        document.setAttribute(M2MConstants.ATTR_LAST_MODIFIED_TIME, now);
        document.setAttribute(Constants.ATTR_MEDIA_TYPE, mediaType);
        document.setContent(resource.saveBinary());
        transaction.updateResource(document);
        transaction.addTransientOp(new SubUpdateOp(manager, getSubsManager(manager, path, resource), resource, mediaType));
        transaction.execute();

        return modified;
    }

    public void deleteResource(String logId, SclManager manager, String path, SclTransaction transaction) {
        if (LOG.isInfoEnabled()) {
            LOG.info("Delete <subscription> resource on path: " + path);
        }
        transaction.deleteResource(path);
    }

    public void deleteResource(String logId, SclManager manager, String path, XoObject resource, SclTransaction transaction) {
        String subId = resource.getStringAttribute(M2MConstants.ATTR_M2M_ID);

        transaction.deleteResource(path);
        transaction.addTransientOp(new SubDeleteOp(manager, getSubsManager(manager, path, resource), subId));
    }

    public void deleteChildResource(String logId, SclManager manager, String path, XoObject resource, XoObject childResource,
            Date now, SclTransaction transaction) {
        throw new UnsupportedOperationException();
    }

    public void prepareResourceForResponse(String logId, SclManager manager, URI requestingEntity, String path,
            XoObject resource, FilterCriteria filterCriteria, Set supported) {
        resource.setStringAttribute(M2MConstants.TAG_M2M_SUBSCRIBER_ID, null);
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
