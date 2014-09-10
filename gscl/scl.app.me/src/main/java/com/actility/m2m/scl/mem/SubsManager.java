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
 * id $Id: SubsManager.java 3419 2012-10-26 13:30:11Z JReich $
 * author $Author: JReich $
 * version $Revision: 3419 $
 * lastrevision $Date: 2012-10-26 15:30:11 +0200 (Fri, 26 Oct 2012) $
 * modifiedby $LastChangedBy: JReich $
 * lastmodified $LastChangedDate: 2012-10-26 15:30:11 +0200 (Fri, 26 Oct 2012) $
 */

package com.actility.m2m.scl.mem;

import java.io.UnsupportedEncodingException;
import java.util.Date;
import java.util.HashMap;
import java.util.Iterator;
import java.util.Map;

import org.apache.log4j.Logger;

import com.actility.m2m.m2m.ContentInstanceFilterCriteria;
import com.actility.m2m.m2m.FilterCriteria;
import com.actility.m2m.m2m.M2MConstants;
import com.actility.m2m.m2m.M2MException;
import com.actility.m2m.m2m.StatusCode;
import com.actility.m2m.scl.log.BundleLogger;
import com.actility.m2m.scl.model.SclManager;
import com.actility.m2m.scl.model.SubscribedResource;
import com.actility.m2m.storage.StorageException;
import com.actility.m2m.util.FormatUtils;
import com.actility.m2m.util.log.OSGiLogger;
import com.actility.m2m.xo.XoException;
import com.actility.m2m.xo.XoObject;

public final class SubsManager {
    private static final Logger LOG = OSGiLogger.getLogger(SubsManager.class, BundleLogger.getStaticLogger());

    private final SclManager manager;
    private final String path;
    private final SubscribedResource subscribedResource;
    private final String subscribedPath;
    private final Map/* <String, SubscriptionManager> */subscriptions;
    private final Map resourceContext;
    /**
     * Global filter criteria which merges every subscriptions. Only relevant when there are more than one subscription
     */
    private FilterCriteria filterCriteria;

    public SubsManager(SclManager manager, SubscribedResource subscribedResource, String path, String subscribedPath) {
        this.manager = manager;
        this.subscribedResource = subscribedResource;
        this.path = path;
        this.subscriptions = new HashMap();
        this.resourceContext = subscribedResource.dependsOnFilterCriteria() ? new HashMap() : null;
        this.filterCriteria = null;
        this.subscribedPath = subscribedPath;
    }

    // Will ony be necessary if the transformation based on the requestingEntity for collection is done
    // public void prepareResourceForResponseAndSubscriptions(XoObject resource, URI indicationRequestingEntity,
    // Map indicationFilterCriteria) throws UnsupportedEncodingException, XmlStorageException, XoException, M2MException {
    // URI mergedRequestingEntity = requestingEntity;
    // Map mergedFilterCriteria = null;
    // if (!EqualsUtils.isEqual(mergedRequestingEntity, indicationRequestingEntity)) {
    // mergedRequestingEntity = null;
    // }
    // if (filterCriteria != null && indicationFilterCriteria != null) {
    // mergedFilterCriteria = new HashMap(filterCriteria);
    // subscribedResource.mergeFilterCriteria(mergedFilterCriteria, indicationFilterCriteria);
    // }
    // subscribedResource.prepareResourceForResponse(resource, mergedRequestingEntity, mergedFilterCriteria);
    // }

    public boolean isEmpty() {
        return subscriptions.isEmpty();
    }

    public void resourceUpdated(XoObject resource, boolean resourcePrepared, long now) {
        Iterator it = subscriptions.values().iterator();
        SubManager subscription = null;
        boolean firstIteration = true;
        // boolean prepareResponseRequired = (resourceContext != null) && ((subscriptions.size() > 1) || resourcePrepared);
        try {
            String attributeAccessor = null;
            XoObject contentInstanceCollection = null;
            XoObject resourceHandle = null;
            while (it.hasNext()) {
                subscription = (SubManager) it.next();
                if (subscribedResource.filterMatches(resource, subscription.getFilterCriteria())
                        && subscription.resourceUpdated(now)) {
                    // Send notify
                    try {
                        if (firstIteration) {
                            if (!resourcePrepared) {
                                // Build and buffer response representation for others subscriptions
                                // Build the generic response representation of the resource
                                // Transformations + create partially addressed attributes
                                resourceHandle = resource;
                            } else {
                                resourceHandle = resource.copy(true);
                            }
                            firstIteration = false;
                        } else {
                            resourceHandle = resource.copy(true);
                        }
                        subscribedResource.prepareResourceForResponse(manager, subscribedPath, resourceHandle,
                                subscription.getFilterCriteria(), null);

                        // TODO Could be further optimized by backuping and precalculating data into the resourceContext
                        // just the first time and fix the resource in each iteration with this backup.
                        // The backup is then destroyed at the end of processing
                        // if (prepareResponseRequired) {
                        // // Partial address with filter criteria
                        // subscribedResource.prepareResponse(resourceContext, manager, subscribedPath, resourceHandle,
                        // subscription.getFilterCriteria());
                        // }
                        // Partial addressing
                        attributeAccessor = ((subscription.getFilterCriteria() != null) ? subscription.getFilterCriteria()
                                .getAttributeAccessor() : null);
                        if (attributeAccessor != null
                                && attributeAccessor.equals(M2MConstants.ATTR_CONTENT_INSTANCE_COLLECTION
                                        + M2MConstants.URI_SEP)) {
                            contentInstanceCollection = resourceHandle
                                    .getXoObjectAttribute(M2MConstants.TAG_M2M_CONTENT_INSTANCE_COLLECTION);
                            contentInstanceCollection.setNameSpace(M2MConstants.PREFIX_M2M);
                            contentInstanceCollection.setNameSpace(M2MConstants.PREFIX_XMIME);

                            String mediaType = subscription.getMediaType();
                            if (M2MConstants.MT_APPLICATION_EXI.equals(mediaType)) {
                                subscription.sendNotify(StatusCode.STATUS_OK, contentInstanceCollection.saveExi(), mediaType,
                                        M2MConstants.MT_APPLICATION_EXI);
                            } else {
                                subscription.sendNotify(StatusCode.STATUS_OK, contentInstanceCollection.saveXml(), mediaType,
                                        M2MConstants.CT_APPLICATION_XML_UTF8);
                            }
                        } else {
                            String mediaType = subscription.getMediaType();
                            if (M2MConstants.MT_APPLICATION_EXI.equals(mediaType)) {
                                subscription.sendNotify(StatusCode.STATUS_OK, resourceHandle.saveExi(), mediaType,
                                        M2MConstants.MT_APPLICATION_EXI);
                            } else {
                                subscription.sendNotify(StatusCode.STATUS_OK, resourceHandle.saveXml(), mediaType,
                                        M2MConstants.CT_APPLICATION_XML_UTF8);
                            }
                        }
                        // if (prepareResponseRequired) {
                        // resourceContext.clear();
                        // }
                    } finally {
                        if (resourceHandle != null && resourceHandle != resource) {
                            resourceHandle.free(true);
                            resourceHandle = null;
                        }
                    }
                }
            }
        } catch (XoException e) {
            LOG.error(path + ": Cannot save resource to XML", e);
        } catch (M2MException e) {
            LOG.error(path + ": Cannot save resource to XML", e);
        } catch (StorageException e) {
            LOG.error(path + ": Cannot save resource to XML", e);
        } catch (UnsupportedEncodingException e) {
            LOG.error(path + ": Cannot save resource to XML", e);
        }
    }

    public void resourceDeleted() {
        Iterator it = subscriptions.values().iterator();
        SubManager subscription = null;
        while (it.hasNext()) {
            subscription = (SubManager) it.next();
            subscription.resourceDeleted();
        }
    }

    public boolean addSubscription(String id, SubManager subscription) {
        boolean added = false;
        if (subscriptions.size() < manager.getSclConfig().getMaxSubscriptionsPerResource()
                && manager.getStatsManager().getNbOfSubscriptions() < manager.getSclConfig().getMaxSubscriptions()) {
            if (resourceContext != null) {
                if (subscriptions.size() == 0) {
                    this.filterCriteria = subscription.getFilterCriteria();
                } else {
                    filterCriteria = subscribedResource.mergeFilterCriteria(filterCriteria, subscription.getFilterCriteria());
                }
            }
            manager.getStatsManager().incNbOfSubscriptions();
            subscriptions.put(id, subscription);
            added = true;
        }
        return added;
    }

    public SubManager getSubscription(String id) {
        return (SubManager) subscriptions.get(id);
    }

    public SubManager removeSubscription(String id) {
        SubManager subManager = (SubManager) subscriptions.remove(id);
        if (subManager != null) {
            manager.getStatsManager().decNbOfSubscriptions();
            if (resourceContext != null && subscriptions.size() > 0) {
                SubManager lastSubManager = null;
                if (subscriptions.size() == 1) {
                    lastSubManager = (SubManager) subscriptions.values().iterator().next();
                    this.filterCriteria = lastSubManager.getFilterCriteria();
                } else {
                    Iterator it = subscriptions.values().iterator();
                    while (it.hasNext()) {
                        lastSubManager = (SubManager) it.next();
                        filterCriteria = subscribedResource.mergeFilterCriteria(filterCriteria,
                                lastSubManager.getFilterCriteria());
                    }
                }
            }
        }
        return subManager;
    }

    public void updateIfModifiedSince(FilterCriteria subFilterCriteria, Date oldIfModifiedSince, Date newIfModifiedSince) {
        if (subscriptions.size() == 1) {
            filterCriteria = subFilterCriteria;
        } else {
            Date currentIfModifiedSince = (filterCriteria != null) ? filterCriteria.getIfModifiedSince() : null;
            if (currentIfModifiedSince == oldIfModifiedSince) {
                currentIfModifiedSince = newIfModifiedSince;
                Date tmpIfModifiedSince = null;
                Iterator it = subscriptions.values().iterator();
                SubManager subManager = null;
                while (it.hasNext() && currentIfModifiedSince != null) {
                    subManager = (SubManager) it.next();
                    tmpIfModifiedSince = (subManager.getFilterCriteria() != null) ? subManager.getFilterCriteria()
                            .getIfModifiedSince() : null;
                    if (tmpIfModifiedSince == null) {
                        currentIfModifiedSince = null;
                    } else if (tmpIfModifiedSince.getTime() < currentIfModifiedSince.getTime()) {
                        currentIfModifiedSince = tmpIfModifiedSince;
                    }
                }
                if (currentIfModifiedSince != null) {
                    if (filterCriteria == null) {
                        filterCriteria = subscribedResource.buildFilterCriteria();
                    }
                    filterCriteria.setIfModifiedSince(currentIfModifiedSince);
                }
            }
        }
    }

    public void printContentInstanceFilterCriteria(StringBuffer buffer) {
        ContentInstanceFilterCriteria cifc = (ContentInstanceFilterCriteria) filterCriteria;
        printFilterCriteria(buffer);
        if (cifc.getSizeFrom() != null) {
            buffer.append("  sizeFrom: ").append(cifc.getSizeFrom()).append("\n");
        }
        if (cifc.getSizeUntil() != null) {
            buffer.append("  sizeUntil: ").append(cifc.getSizeUntil()).append("\n");
        }
        if (cifc.getContentType() != null) {
            String[] array = cifc.getIfNoneMatch();
            buffer.append("  contentType: [");
            for (int i = 0; i < array.length; ++i) {
                buffer.append(" ").append(array[i]);
            }
            buffer.append(" ]\n");
        }
        if (cifc.getMetaDataOnly() != null) {
            buffer.append("  metaDataOnly: ").append(cifc.getMetaDataOnly()).append("\n");
        }
    }

    public void printFilterCriteria(StringBuffer buffer) {
        if (filterCriteria.getCreatedAfter() != null) {
            buffer.append("  createdAfter: ")
                    .append(FormatUtils.formatDateTime(filterCriteria.getCreatedAfter(), manager.getTimeZone())).append("\n");
        }
        if (filterCriteria.getCreatedBefore() != null) {
            buffer.append("  createdBefore: ")
                    .append(FormatUtils.formatDateTime(filterCriteria.getCreatedBefore(), manager.getTimeZone())).append("\n");
        }
        if (filterCriteria.getIfModifiedSince() != null) {
            buffer.append("  ifModifiedSince: ")
                    .append(FormatUtils.formatDateTime(filterCriteria.getIfModifiedSince(), manager.getTimeZone()))
                    .append("\n");
        }
        if (filterCriteria.getIfUnmodifiedSince() != null) {
            buffer.append("  ifUnmodifiedSince: ")
                    .append(FormatUtils.formatDateTime(filterCriteria.getIfUnmodifiedSince(), manager.getTimeZone()))
                    .append("\n");
        }
        if (filterCriteria.getSearchString() != null) {
            String[] array = filterCriteria.getSearchString();
            buffer.append("  searchString: [");
            for (int i = 0; i < array.length; ++i) {
                buffer.append(" ").append(array[i]);
            }
            buffer.append(" ]\n");
        }
        if (filterCriteria.getIfNoneMatch() != null) {
            String[] array = filterCriteria.getIfNoneMatch();
            buffer.append("  ifNoneMatch: [");
            for (int i = 0; i < array.length; ++i) {
                buffer.append(" ").append(array[i]);
            }
            buffer.append(" ]\n");
        }
        if (filterCriteria.getAttributeAccessor() != null) {
            buffer.append("  attributeAccessor: ").append(filterCriteria.getAttributeAccessor()).append("\n");
        }
    }

    public String toString() {
        synchronized (manager) {
            StringBuffer buffer = new StringBuffer();
            buffer.append("SubsManager on ").append(subscribedPath).append(" (");
            buffer.append(subscriptions.size()).append(" subscriptions)\n");
            if (filterCriteria != null) {
                if (filterCriteria instanceof ContentInstanceFilterCriteria) {
                    printContentInstanceFilterCriteria(buffer);
                } else {
                    printFilterCriteria(buffer);
                }
            }

            Iterator it = subscriptions.values().iterator();
            SubManager subManager = null;
            while (it.hasNext()) {
                subManager = (SubManager) it.next();
                subManager.print(buffer);
            }
            return buffer.toString();
        }
    }
}
