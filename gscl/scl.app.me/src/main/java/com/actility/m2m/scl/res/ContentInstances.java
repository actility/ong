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
 * id $Id: ContentInstances.java 4306 2013-02-18 10:22:20Z JReich $
 * author $Author: JReich $
 * version $Revision: 4306 $
 * lastrevision $Date: 2013-02-18 11:22:20 +0100 (Mon, 18 Feb 2013) $
 * modifiedby $LastChangedBy: JReich $
 * lastmodified $LastChangedDate: 2013-02-18 11:22:20 +0100 (Mon, 18 Feb 2013) $
 */

package com.actility.m2m.scl.res;

import java.io.IOException;
import java.io.Serializable;
import java.io.UnsupportedEncodingException;
import java.net.URI;
import java.text.ParseException;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.Collection;
import java.util.Date;
import java.util.HashSet;
import java.util.Iterator;
import java.util.List;
import java.util.Map;
import java.util.Map.Entry;
import java.util.Set;
import java.util.TimeZone;

import org.apache.log4j.Logger;

import com.actility.m2m.m2m.ContentInstanceFilterCriteria;
import com.actility.m2m.m2m.FilterCriteria;
import com.actility.m2m.m2m.Indication;
import com.actility.m2m.m2m.M2MConstants;
import com.actility.m2m.m2m.M2MException;
import com.actility.m2m.m2m.M2MSession;
import com.actility.m2m.m2m.StatusCode;
import com.actility.m2m.scl.log.BundleLogger;
import com.actility.m2m.scl.mem.CisChildCreateOp;
import com.actility.m2m.scl.mem.CisChildDeleteOp;
import com.actility.m2m.scl.mem.SubsManager;
import com.actility.m2m.scl.model.Constants;
import com.actility.m2m.scl.model.SclLogger;
import com.actility.m2m.scl.model.SclManager;
import com.actility.m2m.scl.model.SclTransaction;
import com.actility.m2m.scl.model.SubscribedResource;
import com.actility.m2m.scl.model.VolatileResource;
import com.actility.m2m.storage.Condition;
import com.actility.m2m.storage.ConditionBuilder;
import com.actility.m2m.storage.SearchResult;
import com.actility.m2m.storage.StorageException;
import com.actility.m2m.storage.StorageRequestExecutor;
import com.actility.m2m.util.FormatUtils;
import com.actility.m2m.util.Pair;
import com.actility.m2m.util.Profiler;
import com.actility.m2m.util.URIUtils;
import com.actility.m2m.util.log.OSGiLogger;
import com.actility.m2m.xo.XoException;
import com.actility.m2m.xo.XoObject;
import com.actility.m2m.xo.XoService;

/**
 * Collection of M2M Content Instances.
 * <p>
 * This resource can be subscribed.
 */
public final class ContentInstances extends SclResource implements SubscribedResource, VolatileResource {
    private static final Logger LOG = OSGiLogger.getLogger(ContentInstances.class, BundleLogger.getStaticLogger());

    private static final ContentInstances INSTANCE = new ContentInstances();

    private static final TimeZone GMT = TimeZone.getTimeZone("GMT");

    public static ContentInstances getInstance() {
        return INSTANCE;
    }

    private final Set reservedKeywords = new HashSet();

    private ContentInstances() {
        // Cannot be instantiated
        super(true, "contentInstancesRetrieveRequestIndication", "contentInstancesRetrieveResponseConfirm", null, null, null,
                null, Constants.ID_CONTENT_INSTANCE_FILTER_CRITERIA_MODE, true, true, null, false, false);
        reservedKeywords.add(M2MConstants.ATTR_CREATION_TIME);
        reservedKeywords.add(M2MConstants.ATTR_LAST_MODIFIED_TIME);
        reservedKeywords.add(M2MConstants.ATTR_CURRENT_NR_OF_INSTANCES);
        reservedKeywords.add(M2MConstants.ATTR_CURRENT_BYTE_SIZE);
        reservedKeywords.add(M2MConstants.ATTR_CONTENT_INSTANCE_COLLECTION);
        reservedKeywords.add(M2MConstants.ATTR_LATEST);
        reservedKeywords.add(M2MConstants.ATTR_OLDEST);
        reservedKeywords.add(M2MConstants.ATTR_SUBSCRIPTIONS_REFERENCE);
        reservedKeywords.add(M2MConstants.RES_SUBSCRIPTIONS);
    }

    // m2m:ContentInstances from ong:t_xml_obj
    // {
    // m2m:creationTime XoString { } // (optional) (xmlType: xsd:dateTime)
    // m2m:lastModifiedTime XoString { } // (optional) (xmlType: xsd:dateTime)
    // m2m:currentNrOfInstances XoString { } // (optional) (xmlType: xsd:long)
    // m2m:currentByteSize XoString { } // (optional) (xmlType: xsd:long)
    // m2m:contentInstanceCollection m2m:ContentInstanceCollection { } // (optional)
    // m2m:latest m2m:ReferenceToNamedResource { } // (optional)
    // m2m:oldest m2m:ReferenceToNamedResource { } // (optional)
    // m2m:subscriptionsReference XoString { } // (optional) (xmlType: xsd:anyURI)
    // }
    // alias m2m:ContentInstances with m2m:contentInstances

    public void reload(SclManager manager, String path, XoObject resource, SclTransaction transaction) throws ParseException,
            IOException, StorageException, XoException, M2MException {
        if (LOG.isInfoEnabled()) {
            LOG.info("Reload volatile contentInstances: " + path);
        }
        XoObject parentResource = null;
        XoObject lastCiResource = null;

        try {
            StorageRequestExecutor storageContext = manager.getStorageContext();

            parentResource = manager.getXoResource(getParentPath(path));

            // Check conditions
            long maxInstanceAge = getAndCheckLong(parentResource, M2MConstants.TAG_M2M_MAX_INSTANCE_AGE,
                    Constants.ID_MODE_OPTIONAL);

            // Check max instance age
            if (maxInstanceAge > 0L) {
                Date now = new Date();
                long currentNbOfInstances = Long.parseLong(resource
                        .getStringAttribute(M2MConstants.TAG_M2M_CURRENT_NR_OF_INSTANCES));
                long currentByteSize = Long.parseLong(resource.getStringAttribute(M2MConstants.TAG_M2M_CURRENT_BYTE_SIZE));

                Condition ciCondition = manager.getConditionBuilder().createStringCondition(Constants.ATTR_TYPE,
                        ConditionBuilder.OPERATOR_EQUAL, Constants.TYPE_CONTENT_INSTANCE);

                Date limitDate = new Date(now.getTime() - (maxInstanceAge * 1000L) + 1L);
                Condition lastCiToDeleteCondition = manager.getConditionBuilder().createDateCondition(
                        M2MConstants.ATTR_CREATION_TIME, ConditionBuilder.OPERATOR_LOWER, limitDate);
                Condition condition = manager.getConditionBuilder().createConjunction(ciCondition, lastCiToDeleteCondition);

                SearchResult searchResult = storageContext.search(path, StorageRequestExecutor.SCOPE_EXACT, condition,
                        StorageRequestExecutor.ORDER_ASC, 4);
                Map result = searchResult.getResults();
                Iterator it = result.entrySet().iterator();
                XoObject ciToDelete = null;
                Entry entry = null;
                while (it.hasNext()) {
                    try {
                        entry = (Entry) it.next();
                        ciToDelete = manager.getXoService().readBinaryXmlXoObject((byte[]) entry.getValue());
                        currentByteSize -= Long.parseLong(ciToDelete.getStringAttribute(M2MConstants.TAG_M2M_CONTENT_SIZE));
                        --currentNbOfInstances;
                        transaction.deleteResource((String) entry.getKey());
                    } finally {
                        if (ciToDelete != null) {
                            ciToDelete.free(true);
                            ciToDelete = null;
                        }
                    }
                }

                limitDate = new Date(now.getTime() - (maxInstanceAge * 1000L));
                Condition lastCiCondition = manager.getConditionBuilder().createDateCondition(M2MConstants.ATTR_CREATION_TIME,
                        ConditionBuilder.OPERATOR_GREATER, limitDate);
                condition = manager.getConditionBuilder().createConjunction(ciCondition, lastCiCondition);
                searchResult = manager.getStorageContext().search(path, StorageRequestExecutor.SCOPE_EXACT, condition,
                        StorageRequestExecutor.ORDER_ASC, 1);
                result = searchResult.getResults();
                it = result.values().iterator();
                if (it.hasNext()) {
                    lastCiResource = manager.getXoService().readBinaryXmlXoObject((byte[]) it.next());

                    Date lastCiCreationTime = SclResource.getAndCheckDateTime(lastCiResource,
                            M2MConstants.TAG_M2M_CREATION_TIME, Constants.ID_MODE_REQUIRED, -1L);
                    String timerId = manager.startResourceTimer(
                            lastCiCreationTime.getTime() + (maxInstanceAge * 1000L) - now.getTime(), path,
                            Constants.ID_RES_CONTENT_INSTANCES, null);
                    manager.getM2MContext().setAttribute(path + Constants.AT_INSTANCE_AGE_TIMER_ID_SUFFIX, timerId);
                    manager.getM2MContext().setAttribute(path + Constants.AT_INSTANCE_AGE_ID_SUFFIX,
                            lastCiResource.getStringAttribute(M2MConstants.ATTR_M2M_ID));
                }

                resource.setStringAttribute(M2MConstants.TAG_M2M_CURRENT_NR_OF_INSTANCES, String.valueOf(currentNbOfInstances));
                resource.setStringAttribute(M2MConstants.TAG_M2M_CURRENT_BYTE_SIZE, String.valueOf(currentByteSize));
                resource.setStringAttribute(M2MConstants.TAG_M2M_LAST_MODIFIED_TIME,
                        FormatUtils.formatDateTime(now, manager.getTimeZone()));

                Collection searchAttributes = new ArrayList();
                searchAttributes.add(new Pair(M2MConstants.ATTR_CREATION_TIME, FormatUtils.parseDateTime(resource
                        .getStringAttribute(M2MConstants.TAG_M2M_CREATION_TIME))));
                searchAttributes.add(new Pair(M2MConstants.ATTR_LAST_MODIFIED_TIME, now));
                searchAttributes.add(new Pair(Constants.ATTR_TYPE, Constants.TYPE_CONTENT_INSTANCES));
                transaction.updateResource(path, resource, searchAttributes);
            }
        } finally {
            if (parentResource != null) {
                parentResource.free(true);
            }
            if (lastCiResource != null) {
                lastCiResource.free(true);
            }
        }
    }

    public void checkRights(String logId, SclManager manager, String path, XoObject resource, URI requestingEntity, String flag)
            throws UnsupportedEncodingException, StorageException, XoException, M2MException {
        String parentPath = getParentPath(path);
        XoObject container = null;
        try {
            container = manager.getXoResource(parentPath);
            Container.getInstance().checkRights(logId, manager, parentPath, container, requestingEntity, flag);
        } finally {
            if (container != null) {
                container.free(true);
            }
        }
    }

    public void createResource(SclManager manager, String path, Date creationDate, String creationTime,
            SclTransaction transaction) throws XoException {
        XoObject resource = null;
        try {
            resource = manager.getXoService().newXmlXoObject(M2MConstants.TAG_M2M_CONTENT_INSTANCES);

            // Update resource
            resource.setNameSpace(M2MConstants.PREFIX_M2M);
            resource.setNameSpace(M2MConstants.PREFIX_XMIME);
            resource.setStringAttribute(M2MConstants.TAG_M2M_CREATION_TIME, creationTime);
            resource.setStringAttribute(M2MConstants.TAG_M2M_LAST_MODIFIED_TIME, creationTime);
            resource.setStringAttribute(M2MConstants.TAG_M2M_CURRENT_NR_OF_INSTANCES, "0");
            resource.setStringAttribute(M2MConstants.TAG_M2M_CURRENT_BYTE_SIZE, "0");

            // Create sub-resources
            Subscriptions.getInstance().createResource(manager, path + M2MConstants.URI_SEP + M2MConstants.RES_SUBSCRIPTIONS,
                    transaction);

            // Save resource
            Collection searchAttributes = new ArrayList();
            searchAttributes.add(new Pair(M2MConstants.ATTR_CREATION_TIME, creationDate));
            searchAttributes.add(new Pair(M2MConstants.ATTR_LAST_MODIFIED_TIME, creationDate));
            searchAttributes.add(new Pair(Constants.ATTR_TYPE, Constants.TYPE_CONTENT_INSTANCES));
            transaction.createResource(path, resource, searchAttributes);
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
            throws StorageException, XoException, M2MException {
        if (LOG.isInfoEnabled()) {
            LOG.info("Delete contentInstances resource on path: " + path);
        }
        XoObject parentResource = null;
        try {
            parentResource = manager.getXoResource(getParentPath(path));
            Map storageConfig = createStorageConfiguration(parentResource, M2MConstants.TAG_ACY_STORAGE_CONFIGURATION);
            Condition condition = manager.getConditionBuilder().createStringCondition(Constants.ATTR_TYPE,
                    ConditionBuilder.OPERATOR_EQUAL, Constants.TYPE_CONTENT_INSTANCE);
            SearchResult searchResult = null;
            if (storageConfig != null) {
                searchResult = manager.getStorageContext().search(storageConfig, path, StorageRequestExecutor.SCOPE_EXACT,
                        condition);
            } else {
                searchResult = manager.getStorageContext().search(path, StorageRequestExecutor.SCOPE_EXACT, condition);
            }
            Map children = searchResult.getResults();
            Iterator it = children.keySet().iterator();
            ContentInstance contentInstance = ContentInstance.getInstance();
            while (it.hasNext()) {
                contentInstance.deleteResource(logId, manager, (String) it.next(), transaction);
            }

            Subscriptions.getInstance().deleteResource(logId, manager,
                    path + M2MConstants.URI_SEP + M2MConstants.RES_SUBSCRIPTIONS, transaction);

            transaction.deleteResource(path);
        } finally {
            if (parentResource != null) {
                parentResource.free(true);
            }
        }
    }

    public void deleteResource(String logId, SclManager manager, String path, XoObject resource, SclTransaction transaction)
            throws StorageException, XoException, M2MException {
        deleteResource(logId, manager, path, transaction);
    }

    public void deleteChildResource(String logId, SclManager manager, String path, XoObject resource, XoObject childResource,
            Date now, SclTransaction transaction) throws ParseException, XoException {
        resource.setStringAttribute(M2MConstants.TAG_M2M_LAST_MODIFIED_TIME,
                FormatUtils.formatDateTime(now, manager.getTimeZone()));

        long currentNbOfInstances = Long.parseLong(resource.getStringAttribute(M2MConstants.TAG_M2M_CURRENT_NR_OF_INSTANCES));
        long currentByteSize = Long.parseLong(resource.getStringAttribute(M2MConstants.TAG_M2M_CURRENT_BYTE_SIZE));
        --currentNbOfInstances;
        currentByteSize -= Long.parseLong(childResource.getStringAttribute(M2MConstants.TAG_M2M_CONTENT_SIZE));

        resource.setStringAttribute(M2MConstants.TAG_M2M_CURRENT_NR_OF_INSTANCES, String.valueOf(currentNbOfInstances));
        resource.setStringAttribute(M2MConstants.TAG_M2M_CURRENT_BYTE_SIZE, String.valueOf(currentByteSize));

        transaction.addTransientOp(new CisChildDeleteOp(manager, path, childResource, now));
        Collection searchAttributes = new ArrayList();
        searchAttributes.add(new Pair(M2MConstants.ATTR_CREATION_TIME, FormatUtils.parseDateTime(resource
                .getStringAttribute(M2MConstants.TAG_M2M_CREATION_TIME))));
        searchAttributes.add(new Pair(M2MConstants.ATTR_LAST_MODIFIED_TIME, now));
        searchAttributes.add(new Pair(Constants.ATTR_TYPE, Constants.TYPE_CONTENT_INSTANCES));
        transaction.updateResource(path, resource, searchAttributes);
    }

    public void prepareResourceForResponse(String logId, SclManager manager, String path, XoObject resource,
            URI requestingEntity, FilterCriteria filterCriteria, Set supported) throws StorageException, XoException,
            M2MException {
        XoService xoService = manager.getXoService();
        String appPath = manager.getM2MContext().getApplicationPath();
        resource.setStringAttribute(Constants.ATTR_LATEST, null);
        resource.setStringAttribute(M2MConstants.TAG_M2M_SUBSCRIPTIONS_REFERENCE, appPath + URIUtils.encodePath(path)
                + M2MConstants.URI_SEP + M2MConstants.RES_SUBSCRIPTIONS + M2MConstants.URI_SEP);
        XoObject parentResource = null;
        try {
            String parentPath = getParentPath(path);
            parentResource = manager.getXoResource(parentPath);
            Map storageConfig = createStorageConfiguration(parentResource, M2MConstants.TAG_ACY_STORAGE_CONFIGURATION);
            // Add contentInstances children
            Condition condition = manager.getConditionBuilder().createStringCondition(Constants.ATTR_TYPE,
                    ConditionBuilder.OPERATOR_EQUAL, Constants.TYPE_CONTENT_INSTANCE);
            SearchResult searchResult = null;
            if (storageConfig != null) {
                searchResult = manager.getStorageContext().search(storageConfig, path, StorageRequestExecutor.SCOPE_EXACT,
                        condition);
            } else {
                searchResult = manager.getStorageContext().search(path, StorageRequestExecutor.SCOPE_EXACT, condition);
            }
            Map children = searchResult.getResults();
            XoObject contentInstanceCollection = xoService.newXmlXoObject(M2MConstants.TAG_M2M_CONTENT_INSTANCE_COLLECTION);
            resource.setXoObjectAttribute(M2MConstants.TAG_M2M_CONTENT_INSTANCE_COLLECTION, contentInstanceCollection);
            List contentInstanceCollectionList = contentInstanceCollection
                    .getXoObjectListAttribute(M2MConstants.TAG_M2M_CONTENT_INSTANCE);
            Iterator it = children.values().iterator();
            XoObject contentInstance = null;
            while (it.hasNext()) {
                contentInstance = xoService.readBinaryXmlXoObject((byte[]) it.next());
                if (ContentInstance.getInstance().filterMatches(contentInstance, filterCriteria)) {
                    ContentInstance.getInstance().prepareResourceForResponse(logId, manager, null, contentInstance,
                            requestingEntity, filterCriteria, supported);
                    contentInstance.clearNameSpaces();
                    contentInstanceCollectionList.add(contentInstance);
                } else {
                    contentInstance.free(true);
                }
            }
        } finally {
            if (parentResource != null) {
                parentResource.free(true);
            }
        }
    }

    public boolean dependsOnFilterCriteria() {
        return true;
    }

    public FilterCriteria buildFilterCriteria() {
        return new ContentInstanceFilterCriteria();
    }

    public boolean filterMatches(XoObject resource, FilterCriteria filterCriteria) {
        // Filter always matches on content instances
        return true;
    }

    public FilterCriteria mergeFilterCriteria(FilterCriteria mergedFilterCriteria, FilterCriteria aFilterCriteria) {
        ContentInstanceFilterCriteria result = (ContentInstanceFilterCriteria) super.mergeFilterCriteria(mergedFilterCriteria,
                aFilterCriteria);
        ContentInstanceFilterCriteria filterCriteria = (ContentInstanceFilterCriteria) aFilterCriteria;
        if (result != null && filterCriteria != null) {
            if (result.getSizeFrom() == null || filterCriteria.getSizeFrom() == null) {
                result.setSizeFrom(null);
            } else if (result.getSizeFrom().intValue() > filterCriteria.getSizeFrom().intValue()) {
                result.setSizeFrom(filterCriteria.getSizeFrom());
            }
            if (result.getSizeUntil() == null || filterCriteria.getSizeUntil() == null) {
                result.setSizeUntil(null);
            } else if (result.getSizeUntil().intValue() > filterCriteria.getSizeUntil().intValue()) {
                result.setSizeUntil(filterCriteria.getSizeUntil());
            }
            if (result.getContentType() == null || filterCriteria.getContentType() == null) {
                result.setContentType(null);
            } else {
                List mergedList = new ArrayList();
                mergedList.addAll(Arrays.asList(result.getContentType()));
                String[] resultContentType = result.getContentType();
                String[] fcContentType = filterCriteria.getContentType();
                for (int i = 0; i < fcContentType.length; ++i) {
                    if (Arrays.binarySearch(resultContentType, fcContentType[i]) < 0) {
                        mergedList.add(fcContentType[i]);
                    }
                }
                resultContentType = new String[mergedList.size()];
                mergedList.toArray(resultContentType);
                Arrays.sort(resultContentType);
                result.setContentType(resultContentType);
            }
            if (result.getMetaDataOnly() == null || filterCriteria.getMetaDataOnly() == null) {
                result.setMetaDataOnly(null);
            } else if (result.getMetaDataOnly() != filterCriteria.getMetaDataOnly()) {
                result.setMetaDataOnly(filterCriteria.getMetaDataOnly());
            }
        }
        return result;
    }

    public void prepareResponse(Map context, SclManager manager, String path, XoObject resource, FilterCriteria filterCriteria) {
        // TODO filter content instance according to filter criteria
    }

    public void restoreResponse(Map context, XoObject resource) {
        // TODO filter content instance according to filter criteria
    }

    public byte[] getResponseRepresentation(String logId, SclManager manager, String path, URI requestingEntity,
            FilterCriteria filterCriteria, Set supported, String mediaType) throws UnsupportedEncodingException,
            StorageException, XoException, M2MException {
        XoObject resource = null;
        try {
            resource = manager.getXoResource(path);
            prepareResourceForResponse(logId, manager, path, resource, requestingEntity, filterCriteria, supported);
            // Partial addressing
            String attributeAccessor = ((filterCriteria != null) ? filterCriteria.getAttributeAccessor() : null);
            if (attributeAccessor != null
                    && attributeAccessor.equals(M2MConstants.ATTR_CONTENT_INSTANCE_COLLECTION + M2MConstants.URI_SEP)) {
                XoObject contentInstanceCollection = resource
                        .getXoObjectAttribute(M2MConstants.TAG_M2M_CONTENT_INSTANCE_COLLECTION);
                contentInstanceCollection.setNameSpace(M2MConstants.PREFIX_M2M);
                contentInstanceCollection.setNameSpace(M2MConstants.PREFIX_XMIME);
                if (M2MConstants.MT_APPLICATION_EXI.equals(mediaType)) {
                    return contentInstanceCollection.saveExi();
                } else {
                    return contentInstanceCollection.saveXml();
                }
            }
            if (M2MConstants.MT_APPLICATION_EXI.equals(mediaType)) {
                return resource.saveExi();
            }
            return resource.saveXml();
        } finally {
            if (resource != null) {
                resource.free(true);
            }
        }
    }

    private void checkConditions(SclManager manager, String path, XoObject resource, XoObject parentResource,
            Date lastChildResourceCreationDate, String lastChildResourceId, long ciNbOfInstances, long ciContentSize, Date now,
            SclTransaction transaction) throws StorageException, XoException, M2MException {
        XoObject lastCiResource = null;
        StorageRequestExecutor storageContext = manager.getStorageContext();

        try {
            // Check conditions
            long maxNumberOfInstances = getAndCheckLong(parentResource, M2MConstants.TAG_M2M_MAX_NR_OF_INSTANCES,
                    Constants.ID_MODE_OPTIONAL);
            long maxByteSize = getAndCheckLong(parentResource, M2MConstants.TAG_M2M_MAX_BYTE_SIZE, Constants.ID_MODE_OPTIONAL);
            long maxInstanceAge = getAndCheckLong(parentResource, M2MConstants.TAG_M2M_MAX_INSTANCE_AGE,
                    Constants.ID_MODE_OPTIONAL);

            // long ciContentSize = Long.parseLong(childResource.getStringAttribute(M2MConstants.TAG_M2M_CONTENT_SIZE));

            if (maxNumberOfInstances > 0L && ciNbOfInstances > maxNumberOfInstances) {
                throw new M2MException(
                        "Nb of instances is bigger than the maxNbOfInstances defined in the container. Nb of instances is "
                                + ciNbOfInstances + ". Max nb of instances is " + maxNumberOfInstances,
                        StatusCode.STATUS_BAD_REQUEST);
            }

            if (maxByteSize > 0L && ciContentSize > maxByteSize) {
                throw new M2MException("Content size is bigger than the maxByteSize defined in the container. Content size is "
                        + ciContentSize + ". Max byte size is " + maxByteSize, StatusCode.STATUS_BAD_REQUEST);
            }

            if (maxInstanceAge > 0L
                    && (lastChildResourceCreationDate.getTime() + (maxInstanceAge * 1000L) - now.getTime()) <= 0L) {
                throw new M2MException("Content instance is older than the maxInstanceAge defined in the container: "
                        + maxInstanceAge, StatusCode.STATUS_BAD_REQUEST);
            }

            long currentNbOfInstances = Long.parseLong(resource
                    .getStringAttribute(M2MConstants.TAG_M2M_CURRENT_NR_OF_INSTANCES));
            long currentByteSize = Long.parseLong(resource.getStringAttribute(M2MConstants.TAG_M2M_CURRENT_BYTE_SIZE));

            Date lastCiCreationTimeToDelete = null;
            Condition ciCondition = manager.getConditionBuilder().createStringCondition(Constants.ATTR_TYPE,
                    ConditionBuilder.OPERATOR_EQUAL, Constants.TYPE_CONTENT_INSTANCE);
            Condition condition = null;
            currentNbOfInstances += ciNbOfInstances;
            currentByteSize += ciContentSize;

            // Check max nb of instances
            if (maxNumberOfInstances > 0L && currentNbOfInstances > maxNumberOfInstances) {
                SearchResult searchResult = storageContext.search(path, StorageRequestExecutor.SCOPE_EXACT, ciCondition,
                        StorageRequestExecutor.ORDER_ASC, (int) (currentNbOfInstances - maxNumberOfInstances));
                Map result = searchResult.getResults();
                Iterator it = result.entrySet().iterator();
                XoObject ciToDelete = null;
                Entry entry = null;
                while (it.hasNext()) {
                    try {
                        entry = (Entry) it.next();
                        ciToDelete = manager.getXoService().readBinaryXmlXoObject((byte[]) entry.getValue());
                        currentByteSize -= Long.parseLong(ciToDelete.getStringAttribute(M2MConstants.TAG_M2M_CONTENT_SIZE));
                        if (LOG.isInfoEnabled()) {
                            LOG.info("Content-instance to delete because of maxNrOfInstances limit: " + (String) entry.getKey());
                        }
                        transaction.deleteResource((String) entry.getKey());
                        if (!it.hasNext()) {
                            lastCiCreationTimeToDelete = getAndCheckDateTime(ciToDelete, M2MConstants.TAG_M2M_CREATION_TIME,
                                    Constants.ID_MODE_REQUIRED, -1L);
                        }
                    } finally {
                        if (ciToDelete != null) {
                            ciToDelete.free(true);
                            ciToDelete = null;
                        }
                    }
                }
                currentNbOfInstances = maxNumberOfInstances;
            }
            // Check max byte size
            if (maxByteSize > 0L) {
                while (currentByteSize > maxByteSize) {
                    if (lastCiCreationTimeToDelete != null) {
                        Condition lastCiToDeleteCondition = manager.getConditionBuilder().createDateCondition(
                                M2MConstants.ATTR_CREATION_TIME, ConditionBuilder.OPERATOR_GREATER, lastCiCreationTimeToDelete);
                        condition = manager.getConditionBuilder().createConjunction(ciCondition, lastCiToDeleteCondition);
                    } else {
                        condition = ciCondition;
                    }
                    SearchResult searchResult = storageContext.search(path, StorageRequestExecutor.SCOPE_EXACT, condition,
                            StorageRequestExecutor.ORDER_ASC, 4);
                    Map result = searchResult.getResults();
                    Iterator it = result.entrySet().iterator();
                    XoObject ciToDelete = null;
                    Entry entry = null;
                    boolean end = false;
                    while (it.hasNext() && !end) {
                        try {
                            entry = (Entry) it.next();
                            ciToDelete = manager.getXoService().readBinaryXmlXoObject((byte[]) entry.getValue());
                            currentByteSize -= Long.parseLong(ciToDelete.getStringAttribute(M2MConstants.TAG_M2M_CONTENT_SIZE));
                            --currentNbOfInstances;
                            if (LOG.isInfoEnabled()) {
                                LOG.info("Content-instance to delete because of maxByteSize limit: " + (String) entry.getKey());
                            }
                            transaction.deleteResource((String) entry.getKey());
                            if (!it.hasNext()) {
                                lastCiCreationTimeToDelete = getAndCheckDateTime(ciToDelete,
                                        M2MConstants.TAG_M2M_CREATION_TIME, Constants.ID_MODE_REQUIRED, -1L);
                            } else if (currentByteSize <= maxByteSize) {
                                end = true;
                                lastCiCreationTimeToDelete = getAndCheckDateTime(ciToDelete,
                                        M2MConstants.TAG_M2M_CREATION_TIME, Constants.ID_MODE_REQUIRED, -1L);
                            }
                        } finally {
                            if (ciToDelete != null) {
                                ciToDelete.free(true);
                                ciToDelete = null;
                            }
                        }
                    }
                }
            }
            // Check max instance age
            if (maxInstanceAge > 0L) {
                boolean end = false;
                while (!end) {
                    if (lastCiCreationTimeToDelete != null) {
                        Condition lastCiToDeleteCondition = manager.getConditionBuilder().createDateCondition(
                                M2MConstants.ATTR_CREATION_TIME, ConditionBuilder.OPERATOR_GREATER, lastCiCreationTimeToDelete);
                        condition = manager.getConditionBuilder().createConjunction(ciCondition, lastCiToDeleteCondition);
                    } else {
                        condition = ciCondition;
                    }

                    SearchResult searchResult = storageContext.search(path, StorageRequestExecutor.SCOPE_EXACT, condition,
                            StorageRequestExecutor.ORDER_ASC, 4);
                    Map result = searchResult.getResults();
                    Iterator it = result.entrySet().iterator();
                    end = !it.hasNext();
                    XoObject ciToDelete = null;
                    Entry entry = null;
                    Date ciToDeleteCreationDate = null;
                    while (it.hasNext() && !end) {
                        try {
                            entry = (Entry) it.next();
                            ciToDelete = manager.getXoService().readBinaryXmlXoObject((byte[]) entry.getValue());
                            ciToDeleteCreationDate = getAndCheckDateTime(ciToDelete, M2MConstants.TAG_M2M_CREATION_TIME,
                                    Constants.ID_MODE_REQUIRED, -1L);
                            if ((ciToDeleteCreationDate.getTime() + (maxInstanceAge * 1000)) <= now.getTime()) {
                                currentByteSize -= Long.parseLong(ciToDelete
                                        .getStringAttribute(M2MConstants.TAG_M2M_CONTENT_SIZE));
                                --currentNbOfInstances;
                                if (LOG.isInfoEnabled()) {
                                    LOG.info("Content-instance to delete because of maxInstanceAge limit: "
                                            + (String) entry.getKey());
                                }
                                transaction.deleteResource((String) entry.getKey());
                                lastCiCreationTimeToDelete = ciToDeleteCreationDate;
                            } else {
                                end = true;
                            }
                        } finally {
                            if (ciToDelete != null) {
                                ciToDelete.free(true);
                                ciToDelete = null;
                            }
                        }
                    }
                }


                if (lastCiCreationTimeToDelete != null) {
                    Condition lastCiToDeleteCondition = manager.getConditionBuilder().createDateCondition(
                            M2MConstants.ATTR_CREATION_TIME, ConditionBuilder.OPERATOR_GREATER, lastCiCreationTimeToDelete);
                    condition = manager.getConditionBuilder().createConjunction(ciCondition, lastCiToDeleteCondition);
                } else {
                    condition = ciCondition;
                }

                SearchResult searchResult = storageContext.search(path, StorageRequestExecutor.SCOPE_EXACT, condition,
                        StorageRequestExecutor.ORDER_ASC, 1);
                Map result = searchResult.getResults();
                Iterator it = result.entrySet().iterator();
                Date lastCiCreationDate = null;
                String lastCiId = null;
                if (it.hasNext()) {
                    Entry entry = (Entry) it.next();
                    lastCiResource = manager.getXoService().readBinaryXmlXoObject((byte[]) entry.getValue());
                    lastCiCreationDate = getAndCheckDateTime(lastCiResource, M2MConstants.TAG_M2M_CREATION_TIME,
                            Constants.ID_MODE_REQUIRED, -1L);
                    lastCiId = lastCiResource.getStringAttribute(M2MConstants.ATTR_M2M_ID);
                    if (lastCiCreationDate.getTime() > lastChildResourceCreationDate.getTime()) {
                        lastCiCreationDate = lastChildResourceCreationDate;
                        lastCiId = lastChildResourceId;
                    }
                } else {
                    lastCiCreationDate = lastChildResourceCreationDate;
                    lastCiId = lastChildResourceId;
                }
                transaction.addTransientOp(new CisChildCreateOp(manager, path, lastCiCreationDate, lastCiId, now,
                        maxInstanceAge));
            }

            resource.setStringAttribute(M2MConstants.TAG_M2M_CURRENT_NR_OF_INSTANCES, String.valueOf(currentNbOfInstances));
            resource.setStringAttribute(M2MConstants.TAG_M2M_CURRENT_BYTE_SIZE, String.valueOf(currentByteSize));
        } finally {
            if (lastCiResource != null) {
                lastCiResource.free(true);
            }
        }
    }

    /**
     * Finalize the <contentInstance> creation.
     *
     * @param manager THe SCL manager
     * @param path The path of the contentInstances resource that creates the <contentInstance>
     * @param resource The XO object representing the contentInstances resource
     * @param indication The M2M indication that creates the <contentInstance>
     * @param childPath The path to the created <contentInstance> resource
     * @param childResource The XO object representing the created <contentInstance> resource
     * @param now The current time
     * @param creationTime The current time as a string
     * @param sendRepresentation Whether to send the <contentInstance> representation in the response to the indication
     * @param notify Whether the creation was triggered by a NOTIFY indication
     * @param transaction The storage transaction
     * @throws IOException If any problem occurs while sending the response to the indication
     * @throws ParseException If any attribute id badly formatted
     * @throws StorageException If a problem occurs while running a transaction on the storage
     * @throws XoException If a problem occurs while building the representation of an XO object
     * @throws M2MException If any problem occurs in the M2M layer
     */
    private void finalizeCreateIndication(SclManager manager, String path, XoObject resource, Indication indication,
            String childPath, XoObject childResource, Date now, String creationTime, boolean sendRepresentation,
            boolean notify, SclTransaction transaction) throws IOException, ParseException, StorageException, XoException,
            M2MException {
        resource.setStringAttribute(Constants.ATTR_LATEST, childPath);
        resource.setStringAttribute(M2MConstants.TAG_M2M_LAST_MODIFIED_TIME, creationTime);

        Collection searchAttributes = new ArrayList();
        searchAttributes.add(new Pair(M2MConstants.ATTR_CREATION_TIME, FormatUtils.parseDateTime(resource
                .getStringAttribute(M2MConstants.TAG_M2M_CREATION_TIME))));
        searchAttributes.add(new Pair(M2MConstants.ATTR_LAST_MODIFIED_TIME, now));
        searchAttributes.add(new Pair(Constants.ATTR_TYPE, Constants.TYPE_CONTENT_INSTANCES));
        transaction.updateResource(path, resource, searchAttributes);
        transaction.execute();

        if (!notify) {
            ContentInstance.getInstance().sendCreatedSuccessResponse(manager, childPath, childResource, indication,
                    sendRepresentation);
        } else {
            ContentInstance.getInstance().sendSuccessResponse(manager, childPath, childResource, indication,
                    StatusCode.STATUS_OK, false, null, -1L);
        }

        String subscriptionsPath = path + M2MConstants.URI_SEP + M2MConstants.RES_SUBSCRIPTIONS;
        String subscriptionsLatestPath = subscriptionsPath + M2MConstants.URI_SEP + M2MConstants.ATTR_LATEST;
        SubsManager subscriptions = (SubsManager) manager.getM2MContext().getAttribute(subscriptionsPath);
        SubsManager subscriptionsLatest = (SubsManager) manager.getM2MContext().getAttribute(subscriptionsLatestPath);
        if (subscriptions != null) {
            subscriptions.resourceUpdated(resource, false, now.getTime());
        }
        if (subscriptionsLatest != null) {
            subscriptionsLatest.resourceUpdated(childResource, sendRepresentation, now.getTime());
        }
    }

    private void doCreateIndicationFromContentInstance(SclManager manager, String path, XoObject resource,
            Indication indication, XoObject representation) throws ParseException, IOException, XoException, StorageException,
            M2MException {
        LOG.info("doCreateIndicationFromContentInstance");
        XoObject childResource = null;
        XoObject parentResource = null;

        SclLogger.logRequestIndication(Constants.PT_CONTENT_INSTANCE_CREATE_REQUEST,
                Constants.PT_CONTENT_INSTANCE_CREATE_RESPONSE, indication, null, Constants.ID_LOG_REPRESENTATION);
        try {
            StorageRequestExecutor storageContext = manager.getStorageContext();
            boolean sendRepresentation = false;
            Date now = new Date();
            String creationTime = FormatUtils.formatDateTime(now, manager.getTimeZone());
            String id = representation.getStringAttribute(M2MConstants.ATTR_M2M_ID);
            if (id == null) {
                sendRepresentation = true;
                id = FormatUtils.formatDateTime(now, GMT);
            }
            String href = URIUtils.encodePath(id) + M2MConstants.URI_SEP;
            String childPath = path + M2MConstants.URI_SEP + id;

            if (id.length() == 0) {
                throw new M2MException("id cannot be empty", StatusCode.STATUS_BAD_REQUEST);
            } else if (reservedKeywords.contains(id)) {
                throw new M2MException(id + " is a reserved keywork in contentInstances resource",
                        StatusCode.STATUS_BAD_REQUEST);
            } else if (id.indexOf('/') != -1) {
                throw new M2MException("id cannot contains a '/' character: " + id, StatusCode.STATUS_BAD_REQUEST);
            }

            ContentInstance childController = ContentInstance.getInstance();
            SclTransaction transaction = new SclTransaction(storageContext);
            childResource = manager.getXoService().newXmlXoObject(M2MConstants.TAG_M2M_CONTENT_INSTANCE);

            // Check id
            byte[] testChildPath = storageContext.retrieve(childPath);
            if (testChildPath != null) {
                throw new M2MException("A Content Intance resource already exists with the id " + id,
                        StatusCode.STATUS_CONFLICT);
            }

            parentResource = manager.getXoResource(getParentPath(path));
            Map storageConfig = createStorageConfiguration(parentResource, M2MConstants.TAG_ACY_STORAGE_CONFIGURATION);

            sendRepresentation = childController.createResource(manager, childPath, childResource, storageConfig, id, href,
                    now, creationTime, representation, transaction) || sendRepresentation;

            long ciContentSize = Long.parseLong(childResource.getStringAttribute(M2MConstants.TAG_M2M_CONTENT_SIZE));
            Date lastCiCreationDate = getAndCheckDateTime(childResource, M2MConstants.TAG_M2M_CREATION_TIME,
                    Constants.ID_MODE_REQUIRED, -1L);
            String lastCiId = childResource.getStringAttribute(M2MConstants.ATTR_M2M_ID);
            checkConditions(manager, path, resource, parentResource, lastCiCreationDate, lastCiId, 1L, ciContentSize, now,
                    transaction);

            finalizeCreateIndication(manager, path, resource, indication, childPath, childResource, now, creationTime,
                    sendRepresentation, false, transaction);
        } finally {
            if (childResource != null) {
                childResource.free(true);
            }
            if (parentResource != null) {
                parentResource.free(true);
            }
        }
    }

    private void doCreateIndicationFromBytes(SclManager manager, String path, XoObject resource, Indication indication)
            throws ParseException, IOException, XoException, StorageException, M2MException {
        LOG.info("doCreateIndicationFromBytes");
        XoObject childResource = null;
        XoObject parentResource = null;

        SclLogger.logRequestIndication(Constants.PT_CONTENT_INSTANCE_CREATE_REQUEST,
                Constants.PT_CONTENT_INSTANCE_CREATE_RESPONSE, indication, null, Constants.ID_LOG_RAW_REPRESENTATION);
        try {
            StorageRequestExecutor storageContext = manager.getStorageContext();
            Date now = new Date();
            String creationTime = FormatUtils.formatDateTime(now, manager.getTimeZone());
            String id = FormatUtils.formatDateTime(now, manager.getTimeZone());
            String href = URIUtils.encodePath(id) + M2MConstants.URI_SEP;
            String childPath = path + M2MConstants.URI_SEP + id;
            ContentInstance childController = ContentInstance.getInstance();
            SclTransaction transaction = new SclTransaction(storageContext);
            childResource = manager.getXoService().newXmlXoObject(M2MConstants.TAG_M2M_CONTENT_INSTANCE);

            // Check id
            byte[] testChildPath = storageContext.retrieve(childPath);
            if (testChildPath != null) {
                throw new M2MException("A Content Intance resource already exists with the id " + id,
                        StatusCode.STATUS_CONFLICT);
            }

            parentResource = manager.getXoResource(getParentPath(path));
            Map storageConfig = createStorageConfiguration(parentResource, M2MConstants.TAG_ACY_STORAGE_CONFIGURATION);

            childController.createResourceFromBytes(manager, childPath, childResource, storageConfig, id, href, now,
                    creationTime, indication.getRawContentType(), indication.getRawBytes(), transaction);

            long ciContentSize = Long.parseLong(childResource.getStringAttribute(M2MConstants.TAG_M2M_CONTENT_SIZE));
            Date lastCiCreationDate = getAndCheckDateTime(childResource, M2MConstants.TAG_M2M_CREATION_TIME,
                    Constants.ID_MODE_REQUIRED, -1L);
            String lastCiId = childResource.getStringAttribute(M2MConstants.ATTR_M2M_ID);
            checkConditions(manager, path, resource, parentResource, lastCiCreationDate, lastCiId, 1L, ciContentSize, now,
                    transaction);

            finalizeCreateIndication(manager, path, resource, indication, childPath, childResource, now, creationTime, true,
                    false, transaction);
        } finally {
            if (childResource != null) {
                childResource.free(true);
            }
            if (parentResource != null) {
                parentResource.free(true);
            }
        }
    }

    private void doCreateIndicationFromExternalContentInstance(SclManager manager, String path, XoObject resource,
            Indication indication, XoObject representation) throws ParseException, IOException, XoException, StorageException,
            M2MException {
        LOG.info("doCreateIndicationFromExternalContentInstance");
        XoObject childResource = null;
        XoObject parentResource = null;

        SclLogger.logRequestIndication(Constants.PT_CONTENT_INSTANCE_CREATE_REQUEST,
                Constants.PT_CONTENT_INSTANCE_CREATE_RESPONSE, indication, null, Constants.ID_LOG_REPRESENTATION);
        try {
            StorageRequestExecutor storageContext = manager.getStorageContext();
            Date now = new Date();
            String creationTime = FormatUtils.formatDateTime(now, manager.getTimeZone());
            String id = representation.getStringAttribute(M2MConstants.ATTR_M2M_ID);
            String href = URIUtils.encodePath(id) + M2MConstants.URI_SEP;
            String childPath = path + M2MConstants.URI_SEP + id;
            ContentInstance childController = ContentInstance.getInstance();
            SclTransaction transaction = new SclTransaction(storageContext);
            childResource = manager.getXoService().newXmlXoObject(M2MConstants.TAG_M2M_CONTENT_INSTANCE);

            // Check id
            if (id == null) {
                throw new M2MException(
                        "The <notify> representation contains a <contentInstance> representation without an id attribute",
                        StatusCode.STATUS_BAD_REQUEST);
            }
            if (id.length() == 0) {
                throw new M2MException("id cannot be empty", StatusCode.STATUS_BAD_REQUEST);
            } else if (reservedKeywords.contains(id)) {
                throw new M2MException(id + " is a reserved keywork in contentInstances resource",
                        StatusCode.STATUS_BAD_REQUEST);
            } else if (id.indexOf('/') != -1) {
                throw new M2MException("id cannot contains a '/' character: " + id, StatusCode.STATUS_BAD_REQUEST);
            }
            byte[] testChildPath = storageContext.retrieve(childPath);
            if (testChildPath != null) {
                throw new M2MException("A Content Intance resource already exists with the id " + id,
                        StatusCode.STATUS_CONFLICT);
            }

            parentResource = manager.getXoResource(getParentPath(path));
            Map storageConfig = createStorageConfiguration(parentResource, M2MConstants.TAG_ACY_STORAGE_CONFIGURATION);

            childController.createResourceFromExternalContentInstance(manager, childPath, childResource, storageConfig, id,
                    href, representation, transaction);

            long ciContentSize = Long.parseLong(childResource.getStringAttribute(M2MConstants.TAG_M2M_CONTENT_SIZE));
            Date lastCiCreationDate = getAndCheckDateTime(childResource, M2MConstants.TAG_M2M_CREATION_TIME,
                    Constants.ID_MODE_REQUIRED, -1L);
            String lastCiId = childResource.getStringAttribute(M2MConstants.ATTR_M2M_ID);
            checkConditions(manager, path, resource, parentResource, lastCiCreationDate, lastCiId, 1L, ciContentSize, now,
                    transaction);

            finalizeCreateIndication(manager, path, resource, indication, childPath, childResource, now, creationTime, false,
                    true, transaction);
        } finally {
            if (childResource != null) {
                childResource.free(true);
            }
            if (parentResource != null) {
                parentResource.free(true);
            }
        }
    }

    private void doCreateIndicationFromExternalContentInstances(SclManager manager, String path, XoObject resource,
            Indication indication, XoObject representation) throws ParseException, IOException, XoException, StorageException,
            M2MException {
        LOG.info("doCreateIndicationFromExternalContentInstances");
        XoObject childResource = null;
        XoObject parentResource = null;

        SclLogger.logRequestIndication(Constants.PT_CONTENT_INSTANCE_CREATE_REQUEST,
                Constants.PT_CONTENT_INSTANCE_CREATE_RESPONSE, indication, null, Constants.ID_LOG_REPRESENTATION);
        try {
            StorageRequestExecutor storageContext = manager.getStorageContext();
            Date now = new Date();
            String creationTime = FormatUtils.formatDateTime(now, manager.getTimeZone());
            ContentInstance childController = ContentInstance.getInstance();
            SclTransaction transaction = new SclTransaction(storageContext);
            childResource = manager.getXoService().newXmlXoObject(M2MConstants.TAG_M2M_CONTENT_INSTANCE);

            XoObject contentInstanceCollection = representation
                    .getXoObjectAttribute(M2MConstants.TAG_M2M_CONTENT_INSTANCE_COLLECTION);
            if (contentInstanceCollection == null) {
                throw new M2MException(
                        "Received a contentInstances representation in a notify withtout the contentInstanceCollection attribute",
                        StatusCode.STATUS_BAD_REQUEST);
            }
            List contentInstanceList = contentInstanceCollection
                    .getXoObjectListAttribute(M2MConstants.TAG_M2M_CONTENT_INSTANCE);
            Iterator it = contentInstanceList.iterator();
            XoObject contentInstance = null;
            String id = null;
            String href = null;
            String childPath = null;
            long ciNbOfInstances = contentInstanceList.size();
            long ciContentSize = 0L;
            Date lastCiCreationDate = null;
            Date ciCreationDate = null;
            String lastCiId = null;

            parentResource = manager.getXoResource(getParentPath(path));
            Map storageConfig = createStorageConfiguration(parentResource, M2MConstants.TAG_ACY_STORAGE_CONFIGURATION);

            while (it.hasNext()) {
                contentInstance = (XoObject) it.next();
                // Check id
                id = contentInstance.getStringAttribute(M2MConstants.ATTR_M2M_ID);
                if (id == null) {
                    throw new M2MException(
                            "The <notify> representation contains a <contentInstance> representation without an id attribute",
                            StatusCode.STATUS_BAD_REQUEST);
                }
                href = URIUtils.encodePath(id) + M2MConstants.URI_SEP;
                childPath = path + M2MConstants.URI_SEP + id;

                if (id.length() == 0) {
                    throw new M2MException("id cannot be empty", StatusCode.STATUS_BAD_REQUEST);
                } else if (reservedKeywords.contains(id)) {
                    throw new M2MException(id + " is a reserved keywork in contentInstances resource",
                            StatusCode.STATUS_BAD_REQUEST);
                } else if (id.indexOf('/') != -1) {
                    throw new M2MException("id cannot contains a '/' character: " + id, StatusCode.STATUS_BAD_REQUEST);
                }

                byte[] testChildPath = storageContext.retrieve(childPath);
                if (testChildPath != null) {
                    throw new M2MException("A Content Intance resource already exists with the id " + id,
                            StatusCode.STATUS_CONFLICT);
                }

                childController.createResourceFromExternalContentInstance(manager, childPath, childResource, storageConfig, id,
                        href, contentInstance, transaction);

                ciContentSize += Long.parseLong(childResource.getStringAttribute(M2MConstants.TAG_M2M_CONTENT_SIZE));
                ciCreationDate = getAndCheckDateTime(childResource, M2MConstants.TAG_M2M_CREATION_TIME,
                        Constants.ID_MODE_REQUIRED, -1L);
                if (lastCiCreationDate == null || lastCiCreationDate.getTime() > ciCreationDate.getTime()) {
                    lastCiCreationDate = ciCreationDate;
                    lastCiId = id;
                }
                if (it.hasNext()) {
                    childResource.free(true);
                    childResource = manager.getXoService().newXmlXoObject(M2MConstants.TAG_M2M_CONTENT_INSTANCE);
                }
            }

            checkConditions(manager, path, resource, parentResource, lastCiCreationDate, lastCiId, ciNbOfInstances,
                    ciContentSize, now, transaction);

            finalizeCreateIndication(manager, path, resource, indication, childPath, childResource, now, creationTime, false,
                    true, transaction);
        } finally {
            if (childResource != null) {
                childResource.free(true);
            }
            if (parentResource != null) {
                parentResource.free(true);
            }
        }
    }

    public void doCreateIndication(SclManager manager, String path, XoObject resource, Indication indication)
            throws ParseException, IOException, XoException, StorageException, M2MException {
        XoObject representation = null;
        XoObject notifyContent = null;

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
        try {
            if (indication.getRawContentType() != null
                    && indication.getRawContentType().startsWith(M2MConstants.MT_APPLICATION_XML)) {
                try {
                    representation = indication.getRepresentation(null);
                } catch (XoException e) {
                    // Ignore use raw bytes instead...
                }
            }

            // Content-instance
            // Notify with contentInstances
            // Notify with contentInstance
            // Raw bytes

            if (representation != null) {
                if (manager.getSclConfig().isContentInstanceCreationOnNotify()
                        && M2MConstants.TAG_M2M_NOTIFY.equals(representation.getName())) {
                    XoObject representationAttr = representation.getXoObjectAttribute(M2MConstants.TAG_REPRESENTATION);
                    String contentType = representationAttr.getStringAttribute(M2MConstants.TAG_XMIME_CONTENT_TYPE);
                    byte[] rawContentInstance = representationAttr.getBufferAttribute(XoObject.ATTR_VALUE);
                    String contentName = null;
                    try {
                        if (contentType.startsWith(M2MConstants.MT_APPLICATION_XML)
                                || contentType.startsWith(M2MConstants.MT_TEXT_XML)) {
                            if (manager.getXoService().isXmlSupported()) {
                                notifyContent = manager.getXoService().readXmlXmlXoObject(rawContentInstance,
                                        M2MConstants.ENC_UTF8);
                            }
                        } else if (contentType.startsWith(M2MConstants.MT_APPLICATION_EXI)) {
                            if (manager.getXoService().isExiSupported()) {
                                notifyContent = manager.getXoService().readExiXmlXoObject(rawContentInstance,
                                        M2MConstants.ENC_UTF8);
                            }
                        }
                        if (notifyContent == null) {
                            throw new M2MException("Unsupported media type in notify content: " + contentType,
                                    StatusCode.STATUS_UNSUPPORTED_MEDIA_TYPE);
                        }
                        contentName = notifyContent.getName();
                    } catch (XoException e) {
                        // Ignore use raw bytes instead...
                    }

                    if (M2MConstants.TAG_M2M_CONTENT_INSTANCE.equals(contentName)) {
                        doCreateIndicationFromExternalContentInstance(manager, path, resource, indication, notifyContent);
                    } else if (M2MConstants.TAG_M2M_CONTENT_INSTANCES.equals(contentName)) {
                        doCreateIndicationFromExternalContentInstances(manager, path, resource, indication, notifyContent);
                    } else {
                        doCreateIndicationFromBytes(manager, path, resource, indication);
                    }
                } else if (M2MConstants.TAG_M2M_CONTENT_INSTANCE.equals(representation.getName())) {
                    doCreateIndicationFromContentInstance(manager, path, resource, indication, representation);
                } else {
                    doCreateIndicationFromBytes(manager, path, resource, indication);
                }
            } else {
                doCreateIndicationFromBytes(manager, path, resource, indication);
            }
        } finally {
            if (representation != null) {
                representation.free(true);
            }
            if (notifyContent != null) {
                notifyContent.free(true);
            }
        }
    }

    public void timeout(SclManager manager, String path, String timerId, M2MSession session, Serializable info)
            throws ParseException, StorageException, XoException, M2MException {
        SclTransaction transaction = new SclTransaction(manager.getStorageContext());
        XoObject resource = null;
        XoObject childResource = null;
        String childPath = path + M2MConstants.URI_SEP
                + ((String) manager.getM2MContext().getAttribute(path + Constants.AT_INSTANCE_AGE_ID_SUFFIX));
        Date now = new Date();
        try {
            resource = manager.getXoResource(path);
            childResource = manager.getXoResource(childPath);
            if (childResource != null) {
                long ciContentSize = Long.parseLong(childResource.getStringAttribute(M2MConstants.TAG_M2M_CONTENT_SIZE));

                long currentNbOfInstances = Long.parseLong(resource
                        .getStringAttribute(M2MConstants.TAG_M2M_CURRENT_NR_OF_INSTANCES));
                long currentByteSize = Long.parseLong(resource.getStringAttribute(M2MConstants.TAG_M2M_CURRENT_BYTE_SIZE));

                --currentNbOfInstances;
                currentByteSize -= ciContentSize;

                resource.setStringAttribute(M2MConstants.TAG_M2M_CURRENT_NR_OF_INSTANCES, String.valueOf(currentNbOfInstances));
                resource.setStringAttribute(M2MConstants.TAG_M2M_CURRENT_BYTE_SIZE, String.valueOf(currentByteSize));
                resource.setStringAttribute(M2MConstants.TAG_M2M_LAST_MODIFIED_TIME,
                        FormatUtils.formatDateTime(now, manager.getTimeZone()));

                transaction.deleteResource(childPath);

                transaction.addTransientOp(new CisChildDeleteOp(manager, childPath, childResource, now));
                Collection searchAttributes = new ArrayList();
                searchAttributes.add(new Pair(M2MConstants.ATTR_CREATION_TIME, FormatUtils.parseDateTime(resource
                        .getStringAttribute(M2MConstants.TAG_M2M_CREATION_TIME))));
                searchAttributes.add(new Pair(M2MConstants.ATTR_LAST_MODIFIED_TIME, now));
                searchAttributes.add(new Pair(Constants.ATTR_TYPE, Constants.TYPE_CONTENT_INSTANCES));
                transaction.updateResource(path, resource, searchAttributes);
                transaction.execute();
            }
        } finally {
            if (resource != null) {
                resource.free(true);
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
