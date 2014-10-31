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
 * id $Id: SclManagerImpl.java 4306 2013-02-18 10:22:20Z JReich $
 * author $Author: JReich $
 * version $Revision: 4306 $
 * lastrevision $Date: 2013-02-18 11:22:20 +0100 (Mon, 18 Feb 2013) $
 * modifiedby $LastChangedBy: JReich $
 * lastmodified $LastChangedDate: 2013-02-18 11:22:20 +0100 (Mon, 18 Feb 2013) $
 */

package com.actility.m2m.scl.impl;

import java.io.File;
import java.io.IOException;
import java.io.Serializable;
import java.io.UnsupportedEncodingException;
import java.net.URI;
import java.text.ParseException;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.HashMap;
import java.util.Iterator;
import java.util.List;
import java.util.Map;

import org.apache.log4j.Logger;

import com.actility.m2m.m2m.Confirm;
import com.actility.m2m.m2m.Indication;
import com.actility.m2m.m2m.M2MConstants;
import com.actility.m2m.m2m.M2MContext;
import com.actility.m2m.m2m.M2MEventHandler;
import com.actility.m2m.m2m.M2MException;
import com.actility.m2m.m2m.M2MProxyHandler;
import com.actility.m2m.m2m.M2MSession;
import com.actility.m2m.m2m.ProxyIndication;
import com.actility.m2m.m2m.Response;
import com.actility.m2m.m2m.StatusCode;
import com.actility.m2m.scl.log.BundleLogger;
import com.actility.m2m.scl.mem.SubManager;
import com.actility.m2m.scl.model.Constants;
import com.actility.m2m.scl.model.PathLayout;
import com.actility.m2m.scl.model.ResourceController;
import com.actility.m2m.scl.model.SclConfig;
import com.actility.m2m.scl.model.SclLogger;
import com.actility.m2m.scl.model.SclManager;
import com.actility.m2m.scl.model.SclTransaction;
import com.actility.m2m.scl.model.StatsManager;
import com.actility.m2m.scl.model.VolatileResource;
import com.actility.m2m.scl.res.AccessRight;
import com.actility.m2m.scl.res.AccessRights;
import com.actility.m2m.scl.res.Application;
import com.actility.m2m.scl.res.Applications;
import com.actility.m2m.scl.res.CommunicationChannel;
import com.actility.m2m.scl.res.CommunicationChannels;
import com.actility.m2m.scl.res.Container;
import com.actility.m2m.scl.res.Containers;
import com.actility.m2m.scl.res.ContentInstance;
import com.actility.m2m.scl.res.ContentInstances;
import com.actility.m2m.scl.res.Discovery;
import com.actility.m2m.scl.res.LocationContainer;
import com.actility.m2m.scl.res.M2MPoc;
import com.actility.m2m.scl.res.M2MPocs;
import com.actility.m2m.scl.res.NotificationChannel;
import com.actility.m2m.scl.res.NotificationChannels;
import com.actility.m2m.scl.res.Scl;
import com.actility.m2m.scl.res.SclBase;
import com.actility.m2m.scl.res.SclResource;
import com.actility.m2m.scl.res.Scls;
import com.actility.m2m.scl.res.Subcontainers;
import com.actility.m2m.scl.res.Subscription;
import com.actility.m2m.scl.res.Subscriptions;
import com.actility.m2m.storage.Condition;
import com.actility.m2m.storage.Document;
import com.actility.m2m.storage.SearchResult;
import com.actility.m2m.storage.StorageException;
import com.actility.m2m.storage.StorageRequestExecutor;
import com.actility.m2m.util.EmptyUtils;
import com.actility.m2m.util.Profiler;
import com.actility.m2m.util.StringReader;
import com.actility.m2m.util.log.OSGiLogger;
import com.actility.m2m.xo.XoException;
import com.actility.m2m.xo.XoObject;
import com.actility.m2m.xo.XoParseException;
import com.actility.m2m.xo.XoService;
import com.actility.m2m.xo.XoUnknownTypeException;

public final class SclManagerImpl implements /* SearchResultHandler, */M2MEventHandler, M2MProxyHandler, SclManager {
    private static final Logger LOG = OSGiLogger.getLogger(SclManagerImpl.class, BundleLogger.getStaticLogger());

    public static final int RM_STORAGE_RETRIEVE = 1;
    public static final int RM_STORAGE_RETRIEVE_WITH_CONFIG = 2;
    public static final int RM_STORAGE_LATEST = 3;
    public static final int RM_STORAGE_OLDEST = 4;
    public static final int RM_NO_STORAGE = 5;

    private static final int ST_INIT = 0;
    private static final int ST_READY = 1;
    private static final int ST_STOPPED = 2;

    private final XoService xoService;
    private final SclConfig sclConfig;
    private final StorageRequestExecutor storageContext;
    private final StatsManager statsManager;
    private final ResourcePathLayout sclPathLayout;

    private int state;
    private Registration nsclRegistration;
    private M2MContext m2mContext;

    // private List pathsToReload;

    public SclManagerImpl(SclConfig sclConfig, XoService xoService, StorageRequestExecutor storageContext)
            throws UnsupportedEncodingException {
        this.sclConfig = sclConfig;
        this.xoService = xoService;
        this.storageContext = storageContext;
        this.statsManager = new StatsManager();

        sclPathLayout = buildSclPathLayout();

        state = ST_INIT;
    }

    private ResourcePathLayout buildSclPathLayout() {
        ResourcePathLayout subscriptionLayout = new ResourcePathLayout(Constants.ID_RES_SUBSCRIPTION, RM_STORAGE_RETRIEVE,
                null, false, true);
        subscriptionLayout.addAttribute(M2MConstants.ATTR_ID, Constants.ID_ATTR_ID);
        subscriptionLayout.addAttribute(M2MConstants.ATTR_EXPIRATION_TIME, Constants.ID_ATTR_EXPIRATION_TIME);
        subscriptionLayout.addAttribute(M2MConstants.ATTR_MINIMAL_TIME_BETWEEN_NOTIFICATIONS,
                Constants.ID_ATTR_MINIMAL_TIME_BETWEEN_NOTIFICATIONS);
        subscriptionLayout.addAttribute(M2MConstants.ATTR_DELAY_TOLERANCE, Constants.ID_ATTR_DELAY_TOLERANCE);
        subscriptionLayout.addAttribute(M2MConstants.ATTR_CREATION_TIME, Constants.ID_ATTR_CREATION_TIME);
        subscriptionLayout.addAttribute(M2MConstants.ATTR_LAST_MODIFIED_TIME, Constants.ID_ATTR_LAST_MODIFIED_TIME);
        subscriptionLayout.addAttribute(M2MConstants.ATTR_FILTER_CRITERIA, Constants.ID_ATTR_FILTER_CRITERIA);
        subscriptionLayout.addAttribute(M2MConstants.ATTR_SUBSCRIPTION_TYPE, Constants.ID_ATTR_SUBSCRIPTION_TYPE);
        subscriptionLayout.addAttribute(M2MConstants.ATTR_CONTACT, Constants.ID_ATTR_CONTACT);

        ResourcePathLayout subscriptionsLayout = new ResourcePathLayout(Constants.ID_RES_SUBSCRIPTIONS, RM_STORAGE_RETRIEVE,
                subscriptionLayout, false, true);
        subscriptionsLayout.addAttribute(M2MConstants.ATTR_SUBSCRIPTION_COLLECTION, Constants.ID_ATTR_SUBSCRIPTION_COLLECTION);

        ResourcePathLayout contentInstanceLayout = new ResourcePathLayout(Constants.ID_RES_CONTENT_INSTANCE,
                RM_STORAGE_RETRIEVE_WITH_CONFIG, null, false, true);
        contentInstanceLayout.addAttribute(M2MConstants.ATTR_ID, Constants.ID_ATTR_ID);
        contentInstanceLayout.addAttribute(M2MConstants.ATTR_HREF, Constants.ID_ATTR_HREF);
        contentInstanceLayout.addAttribute(M2MConstants.ATTR_CREATION_TIME, Constants.ID_ATTR_CREATION_TIME);
        contentInstanceLayout.addAttribute(M2MConstants.ATTR_LAST_MODIFIED_TIME, Constants.ID_ATTR_LAST_MODIFIED_TIME);
        contentInstanceLayout.addAttribute(M2MConstants.ATTR_DELAY_TOLERANCE, Constants.ID_ATTR_DELAY_TOLERANCE);
        contentInstanceLayout.addAttribute(M2MConstants.ATTR_CONTENT_TYPES, Constants.ID_ATTR_CONTENT_TYPES);
        contentInstanceLayout.addAttribute(M2MConstants.ATTR_CONTENT_SIZE, Constants.ID_ATTR_CONTENT_SIZE);
        contentInstanceLayout.addAttribute(M2MConstants.ATTR_CONTENT, Constants.ID_ATTR_CONTENT);

        PathLayout latestLayout = new WrappedPathLayout(contentInstanceLayout, RM_STORAGE_LATEST);

        PathLayout oldestLayout = new WrappedPathLayout(contentInstanceLayout, RM_STORAGE_OLDEST);

        ResourcePathLayout contentInstancesLayout = new ResourcePathLayout(Constants.ID_RES_CONTENT_INSTANCES,
                RM_STORAGE_RETRIEVE, contentInstanceLayout, false, true);
        contentInstancesLayout.addAttribute(M2MConstants.ATTR_CREATION_TIME, Constants.ID_ATTR_CREATION_TIME);
        contentInstancesLayout.addAttribute(M2MConstants.ATTR_LAST_MODIFIED_TIME, Constants.ID_ATTR_LAST_MODIFIED_TIME);
        contentInstancesLayout.addAttribute(M2MConstants.ATTR_CURRENT_NR_OF_INSTANCES, Constants.ID_ATTR_MAX_NR_OF_INSTANCES);
        contentInstancesLayout.addAttribute(M2MConstants.ATTR_CURRENT_BYTE_SIZE, Constants.ID_ATTR_CURRENT_BYTE_SIZE);
        contentInstancesLayout.addAttribute(M2MConstants.ATTR_CONTENT_INSTANCE_COLLECTION,
                Constants.ID_ATTR_CONTENT_INSTANCE_COLLECTION);
        contentInstancesLayout.addAttribute(M2MConstants.ATTR_SUBSCRIPTIONS_REFERENCE,
                Constants.ID_ATTR_SUBSCRIPTIONS_REFERENCE);
        contentInstancesLayout.addSubResourceLayout(M2MConstants.ATTR_LATEST, latestLayout);
        contentInstancesLayout.addSubResourceLayout(M2MConstants.ATTR_OLDEST, oldestLayout);
        contentInstancesLayout.addSubResourceLayout(M2MConstants.RES_SUBSCRIPTIONS, subscriptionsLayout);

        ResourcePathLayout subcontainersLayout = new ResourcePathLayout(Constants.ID_RES_SUBCONTAINERS, RM_STORAGE_RETRIEVE,
                null, false, true);
        subcontainersLayout.addAttribute(M2MConstants.ATTR_ACCESS_RIGHT_I_D, Constants.ID_ATTR_ACCESS_RIGHT_I_D);
        subcontainersLayout.addAttribute(M2MConstants.ATTR_CREATION_TIME, Constants.ID_ATTR_CREATION_TIME);
        subcontainersLayout.addAttribute(M2MConstants.ATTR_LAST_MODIFIED_TIME, Constants.ID_ATTR_LAST_MODIFIED_TIME);
        subcontainersLayout.addAttribute(M2MConstants.ATTR_CONTAINER_COLLECTION, Constants.ID_ATTR_CONTAINER_COLLECTION);
        subcontainersLayout.addAttribute(M2MConstants.ATTR_SUBSCRIPTIONS_REFERENCE, Constants.ID_ATTR_SUBSCRIPTIONS_REFERENCE);
        subcontainersLayout.addSubResourceLayout(M2MConstants.RES_SUBSCRIPTIONS, subscriptionsLayout);

        ResourcePathLayout containerLayout = new ResourcePathLayout(Constants.ID_RES_CONTAINER, RM_STORAGE_RETRIEVE, null,
                false, true);
        containerLayout.addAttribute(M2MConstants.ATTR_ID, Constants.ID_ATTR_ID);
        containerLayout.addAttribute(M2MConstants.ATTR_EXPIRATION_TIME, Constants.ID_ATTR_EXPIRATION_TIME);
        containerLayout.addAttribute(M2MConstants.ATTR_ACCESS_RIGHT_I_D, Constants.ID_ATTR_ACCESS_RIGHT_I_D);
        containerLayout.addAttribute(M2MConstants.ATTR_SEARCH_STRINGS, Constants.ID_ATTR_SEARCH_STRINGS);
        containerLayout.addAttribute(M2MConstants.ATTR_CREATION_TIME, Constants.ID_ATTR_CREATION_TIME);
        containerLayout.addAttribute(M2MConstants.ATTR_LAST_MODIFIED_TIME, Constants.ID_ATTR_LAST_MODIFIED_TIME);
        containerLayout.addAttribute(M2MConstants.ATTR_ANNOUNCE_TO, Constants.ID_ATTR_ANNOUNCE_TO);
        containerLayout.addAttribute(M2MConstants.ATTR_MAX_NR_OF_INSTANCES, Constants.ID_ATTR_MAX_NR_OF_INSTANCES);
        containerLayout.addAttribute(M2MConstants.ATTR_MAX_BYTE_SIZE, Constants.ID_ATTR_MAX_BYTE_SIZE);
        containerLayout.addAttribute(M2MConstants.ATTR_MAX_INSTANCE_AGE, Constants.ID_ATTR_MAX_INSTANCE_AGE);
        containerLayout.addAttribute(M2MConstants.ATTR_CONTENT_INSTANCES_REFERENCE,
                Constants.ID_ATTR_CONTENT_INSTANCES_REFERENCE);
        containerLayout.addAttribute(M2MConstants.ATTR_SUBCONTAINERS_REFERENCE, Constants.ID_ATTR_SUBCONTAINERS_REFERENCE);
        containerLayout.addAttribute(M2MConstants.ATTR_SUBSCRIPTIONS_REFERENCE, Constants.ID_ATTR_SUBSCRIPTIONS_REFERENCE);
        containerLayout.addSubResourceLayout(M2MConstants.RES_CONTENT_INSTANCES, contentInstancesLayout);
        containerLayout.addSubResourceLayout(M2MConstants.RES_SUBCONTAINERS, subcontainersLayout);
        containerLayout.addSubResourceLayout(M2MConstants.RES_SUBSCRIPTIONS, subscriptionsLayout);

        ResourcePathLayout containerAnncLayout = new ResourcePathLayout(Constants.ID_RES_CONTAINER_ANNC, RM_STORAGE_RETRIEVE,
                null, false, false);
        containerAnncLayout.addAttribute(M2MConstants.ATTR_ID, Constants.ID_ATTR_ID);
        containerAnncLayout.addAttribute(M2MConstants.ATTR_LINK, Constants.ID_ATTR_LINK);
        containerAnncLayout.addAttribute(M2MConstants.ATTR_ACCESS_RIGHT_I_D, Constants.ID_ATTR_ACCESS_RIGHT_I_D);
        containerAnncLayout.addAttribute(M2MConstants.ATTR_SEARCH_STRINGS, Constants.ID_ATTR_SEARCH_STRINGS);
        containerAnncLayout.addAttribute(M2MConstants.ATTR_EXPIRATION_TIME, Constants.ID_ATTR_EXPIRATION_TIME);

        ResourcePathLayout locationContainerLayout = new ResourcePathLayout(Constants.ID_RES_LOCATION_CONTAINER,
                RM_STORAGE_RETRIEVE, null, false, true);
        locationContainerLayout.addAttribute(M2MConstants.ATTR_ID, Constants.ID_ATTR_ID);
        locationContainerLayout.addAttribute(M2MConstants.ATTR_EXPIRATION_TIME, Constants.ID_ATTR_EXPIRATION_TIME);
        locationContainerLayout.addAttribute(M2MConstants.ATTR_ACCESS_RIGHT_I_D, Constants.ID_ATTR_ACCESS_RIGHT_I_D);
        locationContainerLayout.addAttribute(M2MConstants.ATTR_SEARCH_STRINGS, Constants.ID_ATTR_SEARCH_STRINGS);
        locationContainerLayout.addAttribute(M2MConstants.ATTR_CREATION_TIME, Constants.ID_ATTR_CREATION_TIME);
        locationContainerLayout.addAttribute(M2MConstants.ATTR_LAST_MODIFIED_TIME, Constants.ID_ATTR_LAST_MODIFIED_TIME);
        locationContainerLayout.addAttribute(M2MConstants.ATTR_ANNOUNCE_TO, Constants.ID_ATTR_ANNOUNCE_TO);
        locationContainerLayout.addAttribute(M2MConstants.ATTR_MAX_NR_OF_INSTANCES, Constants.ID_ATTR_MAX_NR_OF_INSTANCES);
        locationContainerLayout.addAttribute(M2MConstants.ATTR_MAX_BYTE_SIZE, Constants.ID_ATTR_MAX_BYTE_SIZE);
        locationContainerLayout.addAttribute(M2MConstants.ATTR_MAX_INSTANCE_AGE, Constants.ID_ATTR_MAX_INSTANCE_AGE);
        locationContainerLayout.addAttribute(M2MConstants.ATTR_LOCATION_CONTAINER_TYPE,
                Constants.ID_ATTR_LOCATION_CONTAINER_TYPE);
        locationContainerLayout.addAttribute(M2MConstants.ATTR_CONTENT_INSTANCES_REFERENCE,
                Constants.ID_ATTR_CONTENT_INSTANCES_REFERENCE);
        locationContainerLayout.addAttribute(M2MConstants.ATTR_SUBSCRIPTIONS_REFERENCE,
                Constants.ID_ATTR_SUBSCRIPTIONS_REFERENCE);
        locationContainerLayout.addAttribute(M2MConstants.ATTR_SUBCONTAINERS_REFERENCE,
                Constants.ID_ATTR_SUBCONTAINERS_REFERENCE);
        locationContainerLayout.addSubResourceLayout(M2MConstants.RES_CONTENT_INSTANCES, contentInstancesLayout);
        locationContainerLayout.addSubResourceLayout(M2MConstants.RES_SUBCONTAINERS, subcontainersLayout);
        locationContainerLayout.addSubResourceLayout(M2MConstants.RES_SUBSCRIPTIONS, subscriptionsLayout);

        ResourcePathLayout locationContainerAnncLayout = new ResourcePathLayout(Constants.ID_RES_LOCATION_CONTAINER_ANNC,
                RM_STORAGE_RETRIEVE, null, false, false);
        locationContainerAnncLayout.addAttribute(M2MConstants.ATTR_ID, Constants.ID_ATTR_ID);
        locationContainerAnncLayout.addAttribute(M2MConstants.ATTR_LINK, Constants.ID_ATTR_LINK);
        locationContainerAnncLayout.addAttribute(M2MConstants.ATTR_ACCESS_RIGHT_I_D, Constants.ID_ATTR_ACCESS_RIGHT_I_D);
        locationContainerAnncLayout.addAttribute(M2MConstants.ATTR_SEARCH_STRINGS, Constants.ID_ATTR_SEARCH_STRINGS);
        locationContainerAnncLayout.addAttribute(M2MConstants.ATTR_EXPIRATION_TIME, Constants.ID_ATTR_EXPIRATION_TIME);

        PathLayout[] containerLayouts = new PathLayout[4];
        containerLayouts[0] = containerLayout;
        containerLayouts[1] = containerAnncLayout;
        containerLayouts[2] = locationContainerLayout;
        containerLayouts[3] = locationContainerAnncLayout;
        Map tagToContainerLayouts = new HashMap();
        tagToContainerLayouts.put(M2MConstants.TAG_M2M_CONTAINER, containerLayout);
        tagToContainerLayouts.put(M2MConstants.TAG_M2M_CONTAINER_ANNC, containerAnncLayout);
        tagToContainerLayouts.put(M2MConstants.TAG_M2M_LOCATION_CONTAINER, locationContainerLayout);
        tagToContainerLayouts.put(M2MConstants.TAG_M2M_LOCATION_CONTAINER_ANNC, locationContainerAnncLayout);
        AmbiguousPathLayout ambiguousContainerLayout = new AmbiguousPathLayout(containerLayouts, tagToContainerLayouts);

        ResourcePathLayout containersLayout = new ResourcePathLayout(Constants.ID_RES_CONTAINERS, RM_STORAGE_RETRIEVE,
                ambiguousContainerLayout, false, true);
        containersLayout.addAttribute(M2MConstants.ATTR_ACCESS_RIGHT_I_D, Constants.ID_ATTR_ACCESS_RIGHT_I_D);
        containersLayout.addAttribute(M2MConstants.ATTR_CREATION_TIME, Constants.ID_ATTR_CREATION_TIME);
        containersLayout.addAttribute(M2MConstants.ATTR_LAST_MODIFIED_TIME, Constants.ID_ATTR_LAST_MODIFIED_TIME);
        containersLayout.addAttribute(M2MConstants.ATTR_CONTAINER_COLLECTION, Constants.ID_ATTR_CONTAINER_COLLECTION);
        containersLayout.addAttribute(M2MConstants.ATTR_CONTAINER_ANNC_COLLECTION, Constants.ID_ATTR_CONTAINER_ANNC_COLLECTION);
        containersLayout.addAttribute(M2MConstants.ATTR_LOCATION_CONTAINER_COLLECTION,
                Constants.ID_ATTR_LOCATION_CONTAINER_COLLECTION);
        containersLayout.addAttribute(M2MConstants.ATTR_LOCATION_CONTAINER_ANNC_COLLECTION,
                Constants.ID_ATTR_LOCATION_CONTAINER_ANNC_COLLECTION);
        containersLayout.addAttribute(M2MConstants.ATTR_SUBSCRIPTIONS_REFERENCE, Constants.ID_ATTR_SUBSCRIPTIONS_REFERENCE);
        containersLayout.addSubResourceLayout(M2MConstants.RES_SUBSCRIPTIONS, subscriptionsLayout);

        ResourcePathLayout groupsLayout = new ResourcePathLayout(Constants.ID_RES_GROUPS, RM_STORAGE_RETRIEVE, null, false,
                false);
        groupsLayout.addAttribute(M2MConstants.ATTR_GROUP_COLLECTION, Constants.ID_ATTR_GROUP_COLLECTION);
        groupsLayout.addAttribute(M2MConstants.ATTR_GROUP_ANNC_COLLECTION, Constants.ID_ATTR_GROUP_ANNC_COLLECTION);
        groupsLayout.addAttribute(M2MConstants.ATTR_SUBSCRIPTIONS_REFERENCE, Constants.ID_ATTR_SUBSCRIPTIONS_REFERENCE);
        groupsLayout.addAttribute(M2MConstants.ATTR_ACCESS_RIGHT_I_D, Constants.ID_ATTR_ACCESS_RIGHT_I_D);
        groupsLayout.addAttribute(M2MConstants.ATTR_CREATION_TIME, Constants.ID_ATTR_CREATION_TIME);
        groupsLayout.addAttribute(M2MConstants.ATTR_LAST_MODIFIED_TIME, Constants.ID_ATTR_LAST_MODIFIED_TIME);

        ResourcePathLayout accessRightLayout = new ResourcePathLayout(Constants.ID_RES_ACCESS_RIGHT, RM_STORAGE_RETRIEVE, null,
                false, true);
        accessRightLayout.addAttribute(M2MConstants.ATTR_ID, Constants.ID_ATTR_ID);
        accessRightLayout.addAttribute(M2MConstants.ATTR_EXPIRATION_TIME, Constants.ID_ATTR_EXPIRATION_TIME);
        accessRightLayout.addAttribute(M2MConstants.ATTR_SEARCH_STRINGS, Constants.ID_ATTR_SEARCH_STRINGS);
        accessRightLayout.addAttribute(M2MConstants.ATTR_CREATION_TIME, Constants.ID_ATTR_CREATION_TIME);
        accessRightLayout.addAttribute(M2MConstants.ATTR_LAST_MODIFIED_TIME, Constants.ID_ATTR_LAST_MODIFIED_TIME);
        accessRightLayout.addAttribute(M2MConstants.ATTR_ANNOUNCE_TO, Constants.ID_ATTR_ANNOUNCE_TO);
        accessRightLayout.addAttribute(M2MConstants.ATTR_PERMISSIONS, Constants.ID_ATTR_PERMISSIONS);
        accessRightLayout.addAttribute(M2MConstants.ATTR_SELF_PERMISSIONS, Constants.ID_ATTR_SELF_PERMISSIONS);
        accessRightLayout.addAttribute(M2MConstants.ATTR_SUBSCRIPTIONS_REFERENCE, Constants.ID_ATTR_SUBSCRIPTIONS_REFERENCE);
        accessRightLayout.addSubResourceLayout(M2MConstants.RES_SUBSCRIPTIONS, subscriptionsLayout);

        ResourcePathLayout accessRightAnncLayout = new ResourcePathLayout(Constants.ID_RES_ACCESS_RIGHT_ANNC,
                RM_STORAGE_RETRIEVE, null, false, false);
        accessRightAnncLayout.addAttribute(M2MConstants.ATTR_ID, Constants.ID_ATTR_ID);
        accessRightAnncLayout.addAttribute(M2MConstants.ATTR_LINK, Constants.ID_ATTR_LINK);
        accessRightAnncLayout.addAttribute(M2MConstants.ATTR_ACCESS_RIGHT_I_D, Constants.ID_ATTR_ACCESS_RIGHT_I_D);
        accessRightAnncLayout.addAttribute(M2MConstants.ATTR_SEARCH_STRINGS, Constants.ID_ATTR_SEARCH_STRINGS);
        accessRightAnncLayout.addAttribute(M2MConstants.ATTR_EXPIRATION_TIME, Constants.ID_ATTR_EXPIRATION_TIME);

        PathLayout[] accessRightLayouts = new PathLayout[2];
        accessRightLayouts[0] = accessRightLayout;
        accessRightLayouts[1] = accessRightAnncLayout;
        Map tagToAccessRightLayouts = new HashMap();
        tagToAccessRightLayouts.put(M2MConstants.TAG_M2M_ACCESS_RIGHT, accessRightLayout);
        tagToAccessRightLayouts.put(M2MConstants.TAG_M2M_ACCESS_RIGHT_ANNC, accessRightAnncLayout);
        AmbiguousPathLayout ambiguousAccessRightLayout = new AmbiguousPathLayout(accessRightLayouts, tagToAccessRightLayouts);

        ResourcePathLayout accessRightsLayout = new ResourcePathLayout(Constants.ID_RES_ACCESS_RIGHTS, RM_STORAGE_RETRIEVE,
                ambiguousAccessRightLayout, false, true);
        accessRightsLayout.addAttribute(M2MConstants.ATTR_ACCESS_RIGHT_I_D, Constants.ID_ATTR_ACCESS_RIGHT_I_D);
        accessRightsLayout.addAttribute(M2MConstants.ATTR_CREATION_TIME, Constants.ID_ATTR_CREATION_TIME);
        accessRightsLayout.addAttribute(M2MConstants.ATTR_LAST_MODIFIED_TIME, Constants.ID_ATTR_LAST_MODIFIED_TIME);
        accessRightsLayout.addAttribute(M2MConstants.ATTR_ACCESS_RIGHT_COLLECTION, Constants.ID_ATTR_ACCESS_RIGHT_COLLECTION);
        accessRightsLayout.addAttribute(M2MConstants.ATTR_ACCESS_RIGHT_ANNC_COLLECTION,
                Constants.ID_ATTR_ACCESS_RIGHT_ANNC_COLLECTION);
        accessRightsLayout.addAttribute(M2MConstants.ATTR_SUBSCRIPTIONS_REFERENCE, Constants.ID_ATTR_SUBSCRIPTIONS_REFERENCE);
        accessRightsLayout.addSubResourceLayout(M2MConstants.RES_SUBSCRIPTIONS, subscriptionsLayout);

        ResourcePathLayout notificationChannelLayout = new ResourcePathLayout(Constants.ID_RES_NOTIFICATION_CHANNEL,
                RM_STORAGE_RETRIEVE, null, false, true);
        notificationChannelLayout.addAttribute(M2MConstants.ATTR_ID, Constants.ID_ATTR_ID);
        notificationChannelLayout.addAttribute(M2MConstants.ATTR_CHANNEL_TYPE, Constants.ID_ATTR_CHANNEL_TYPE);
        notificationChannelLayout.addAttribute(M2MConstants.ATTR_CONTACT_U_R_I, Constants.ID_ATTR_CONTACT_U_R_I);
        notificationChannelLayout.addAttribute(M2MConstants.ATTR_CHANNEL_DATA, Constants.ID_ATTR_CHANNEL_DATA);
        notificationChannelLayout.addAttribute(M2MConstants.ATTR_CREATION_TIME, Constants.ID_ATTR_CREATION_TIME);
        notificationChannelLayout.addAttribute(M2MConstants.ATTR_LAST_MODIFIED_TIME, Constants.ID_ATTR_LAST_MODIFIED_TIME);

        ResourcePathLayout notificationChannelsLayout = new ResourcePathLayout(Constants.ID_RES_NOTIFICATION_CHANNELS,
                RM_STORAGE_RETRIEVE, notificationChannelLayout, false, true);
        notificationChannelsLayout.addAttribute(M2MConstants.ATTR_CREATION_TIME, Constants.ID_ATTR_CREATION_TIME);
        notificationChannelsLayout.addAttribute(M2MConstants.ATTR_LAST_MODIFIED_TIME, Constants.ID_ATTR_LAST_MODIFIED_TIME);
        notificationChannelsLayout.addAttribute(M2MConstants.ATTR_NOTIFICATION_CHANNEL_COLLECTION,
                Constants.ID_ATTR_NOTIFICATION_CHANNEL_COLLECTION);

        ResourcePathLayout communicationChannelLayout = new ResourcePathLayout(Constants.ID_RES_COMMUNICATION_CHANNEL,
                RM_STORAGE_RETRIEVE, null, false, true);
        communicationChannelLayout.addAttribute(M2MConstants.ATTR_ID, Constants.ID_ATTR_ID);
        communicationChannelLayout.addAttribute(M2MConstants.ATTR_CHANNEL_TYPE, Constants.ID_ATTR_CHANNEL_TYPE);
        communicationChannelLayout.addAttribute(M2MConstants.ATTR_CONTACT_U_R_I, Constants.ID_ATTR_CONTACT_U_R_I);
        communicationChannelLayout.addAttribute(M2MConstants.ATTR_CHANNEL_DATA, Constants.ID_ATTR_CHANNEL_DATA);
        communicationChannelLayout.addAttribute(M2MConstants.ATTR_CREATION_TIME, Constants.ID_ATTR_CREATION_TIME);
        communicationChannelLayout.addAttribute(M2MConstants.ATTR_LAST_MODIFIED_TIME, Constants.ID_ATTR_LAST_MODIFIED_TIME);

        ResourcePathLayout communicationChannelsLayout = new ResourcePathLayout(Constants.ID_RES_COMMUNICATION_CHANNELS,
                RM_STORAGE_RETRIEVE, communicationChannelLayout, false, true);
        communicationChannelsLayout.addAttribute(M2MConstants.ATTR_CREATION_TIME, Constants.ID_ATTR_CREATION_TIME);
        communicationChannelsLayout.addAttribute(M2MConstants.ATTR_LAST_MODIFIED_TIME, Constants.ID_ATTR_LAST_MODIFIED_TIME);
        communicationChannelsLayout.addAttribute(M2MConstants.ATTR_COMMUNICATION_CHANNEL_COLLECTION,
                Constants.ID_ATTR_COMMUNICATION_CHANNEL_COLLECTION);

        ResourcePathLayout mgmtObjsLayout = new ResourcePathLayout(Constants.ID_RES_MGMT_OBJS, RM_STORAGE_RETRIEVE, null,
                false, false);
        mgmtObjsLayout.addAttribute(M2MConstants.ATTR_MGMT_OBJ_COLLECTION, Constants.ID_ATTR_MGMT_OBJ_COLLECTION);
        mgmtObjsLayout.addAttribute(M2MConstants.ATTR_MGMT_CMD_COLLECTION, Constants.ID_ATTR_MGMT_CMD_COLLECTION);
        mgmtObjsLayout.addAttribute(M2MConstants.ATTR_SUBSCRIPTIONS_REFERENCE, Constants.ID_ATTR_SUBSCRIPTIONS_REFERENCE);
        mgmtObjsLayout.addAttribute(M2MConstants.ATTR_CREATION_TIME, Constants.ID_ATTR_CREATION_TIME);
        mgmtObjsLayout.addAttribute(M2MConstants.ATTR_LAST_MODIFIED_TIME, Constants.ID_ATTR_LAST_MODIFIED_TIME);
        mgmtObjsLayout.addSubResourceLayout(M2MConstants.RES_SUBSCRIPTIONS, subscriptionsLayout);

        ResourcePathLayout applicationLayout = new ResourcePathLayout(Constants.ID_RES_APPLICATION, RM_STORAGE_RETRIEVE, null,
                true, true);
        applicationLayout.addAttribute(M2MConstants.ATTR_APP_ID, Constants.ID_ATTR_APP_ID);
        applicationLayout.addAttribute(M2MConstants.ATTR_EXPIRATION_TIME, Constants.ID_ATTR_EXPIRATION_TIME);
        applicationLayout.addAttribute(M2MConstants.ATTR_ACCESS_RIGHT_I_D, Constants.ID_ATTR_ACCESS_RIGHT_I_D);
        applicationLayout.addAttribute(M2MConstants.ATTR_SEARCH_STRINGS, Constants.ID_ATTR_SEARCH_STRINGS);
        applicationLayout.addAttribute(M2MConstants.ATTR_CREATION_TIME, Constants.ID_ATTR_CREATION_TIME);
        applicationLayout.addAttribute(M2MConstants.ATTR_LAST_MODIFIED_TIME, Constants.ID_ATTR_LAST_MODIFIED_TIME);
        applicationLayout.addAttribute(M2MConstants.ATTR_ANNOUNCE_TO, Constants.ID_ATTR_ANNOUNCE_TO);
        applicationLayout.addAttribute(M2MConstants.ATTR_A_PO_C, Constants.ID_ATTR_A_PO_C);
        applicationLayout.addAttribute(M2MConstants.ATTR_A_PO_C_PATHS, Constants.ID_ATTR_A_PO_C_PATHS);
        applicationLayout.addAttribute(M2MConstants.ATTR_LOC_REQUESTOR, Constants.ID_ATTR_LOC_REQUESTOR);
        applicationLayout.addAttribute(M2MConstants.ATTR_CONTAINERS_REFERENCE, Constants.ID_ATTR_CONTAINERS_REFERENCE);
        applicationLayout.addAttribute(M2MConstants.ATTR_GROUPS_REFERENCE, Constants.ID_ATTR_GROUPS_REFERENCE);
        applicationLayout.addAttribute(M2MConstants.ATTR_ACCESS_RIGHTS_REFERENCE, Constants.ID_ATTR_ACCESS_RIGHTS_REFERENCE);
        applicationLayout.addAttribute(M2MConstants.ATTR_SUBSCRIPTIONS_REFERENCE, Constants.ID_ATTR_SUBSCRIPTIONS_REFERENCE);
        applicationLayout.addAttribute(M2MConstants.ATTR_NOTIFICATION_CHANNELS_REFERENCE,
                Constants.ID_ATTR_NOTIFICATION_CHANNELS_REFERENCE);
        applicationLayout.addSubResourceLayout(M2MConstants.RES_CONTAINERS, containersLayout);
        applicationLayout.addSubResourceLayout(M2MConstants.RES_GROUPS, groupsLayout);
        applicationLayout.addSubResourceLayout(M2MConstants.RES_ACCESS_RIGHTS, accessRightsLayout);
        applicationLayout.addSubResourceLayout(M2MConstants.RES_SUBSCRIPTIONS, subscriptionsLayout);
        applicationLayout.addSubResourceLayout(M2MConstants.RES_NOTIFICATION_CHANNELS, notificationChannelsLayout);

        ResourcePathLayout applicationAnncLayout = new ResourcePathLayout(Constants.ID_RES_APPLICATION_ANNC,
                RM_STORAGE_RETRIEVE, null, false, false);
        applicationAnncLayout.addAttribute(M2MConstants.ATTR_ID, Constants.ID_ATTR_ID);
        applicationAnncLayout.addAttribute(M2MConstants.ATTR_LINK, Constants.ID_ATTR_LINK);
        applicationAnncLayout.addAttribute(M2MConstants.ATTR_ACCESS_RIGHT_I_D, Constants.ID_ATTR_ACCESS_RIGHT_I_D);
        applicationAnncLayout.addAttribute(M2MConstants.ATTR_SEARCH_STRINGS, Constants.ID_ATTR_SEARCH_STRINGS);
        applicationAnncLayout.addAttribute(M2MConstants.ATTR_EXPIRATION_TIME, Constants.ID_ATTR_EXPIRATION_TIME);
        applicationAnncLayout.addAttribute(M2MConstants.ATTR_CONTAINERS_REFERENCE, Constants.ID_ATTR_CONTAINERS_REFERENCE);
        applicationAnncLayout.addAttribute(M2MConstants.ATTR_GROUPS_REFERENCE, Constants.ID_ATTR_GROUPS_REFERENCE);
        applicationAnncLayout
                .addAttribute(M2MConstants.ATTR_ACCESS_RIGHTS_REFERENCE, Constants.ID_ATTR_ACCESS_RIGHTS_REFERENCE);
        applicationAnncLayout.addSubResourceLayout(M2MConstants.RES_CONTAINERS, containersLayout);
        applicationLayout.addSubResourceLayout(M2MConstants.RES_GROUPS, groupsLayout);
        applicationLayout.addSubResourceLayout(M2MConstants.RES_ACCESS_RIGHTS, accessRightsLayout);

        PathLayout[] applicationLayouts = new PathLayout[2];
        applicationLayouts[0] = applicationLayout;
        applicationLayouts[1] = applicationAnncLayout;
        Map tagToApplicationLayouts = new HashMap();
        tagToApplicationLayouts.put(M2MConstants.TAG_M2M_APPLICATION, applicationLayout);
        tagToApplicationLayouts.put(M2MConstants.TAG_M2M_APPLICATION_ANNC, applicationAnncLayout);
        AmbiguousPathLayout ambiguousApplicationLayout = new AmbiguousPathLayout(applicationLayouts, tagToApplicationLayouts);

        ResourcePathLayout applicationsLayout = new ResourcePathLayout(Constants.ID_RES_APPLICATIONS, RM_STORAGE_RETRIEVE,
                ambiguousApplicationLayout, false, true);
        applicationsLayout.addAttribute(M2MConstants.ATTR_ACCESS_RIGHT_I_D, Constants.ID_ATTR_ACCESS_RIGHT_I_D);
        applicationsLayout.addAttribute(M2MConstants.ATTR_CREATION_TIME, Constants.ID_ATTR_CREATION_TIME);
        applicationsLayout.addAttribute(M2MConstants.ATTR_LAST_MODIFIED_TIME, Constants.ID_ATTR_LAST_MODIFIED_TIME);
        applicationsLayout.addAttribute(M2MConstants.ATTR_APPLICATION_COLLECTION, Constants.ID_ATTR_APPLICATION_COLLECTION);
        applicationsLayout.addAttribute(M2MConstants.ATTR_APPLICATION_ANNC_COLLECTION,
                Constants.ID_ATTR_APPLICATION_ANNC_COLLECTION);
        applicationsLayout.addAttribute(M2MConstants.ATTR_SUBSCRIPTIONS_REFERENCE, Constants.ID_ATTR_SUBSCRIPTIONS_REFERENCE);
        applicationsLayout.addAttribute(M2MConstants.ATTR_MGMT_OBJS_REFERENCE, Constants.ID_ATTR_MGMT_OBJS_REFERENCE);
        applicationsLayout.addSubResourceLayout(M2MConstants.RES_SUBSCRIPTIONS, subscriptionsLayout);
        applicationsLayout.addSubResourceLayout(M2MConstants.RES_MGMT_OBJS, mgmtObjsLayout);

        ResourcePathLayout m2mPocLayout = new ResourcePathLayout(Constants.ID_RES_M2M_POC, RM_STORAGE_RETRIEVE, null, false,
                true);
        m2mPocLayout.addAttribute(M2MConstants.ATTR_ID, Constants.ID_ATTR_ID);
        m2mPocLayout.addAttribute(M2MConstants.ATTR_CONTACT_INFO, Constants.ID_ATTR_CONTACT_INFO);
        m2mPocLayout.addAttribute(M2MConstants.ATTR_EXPIRATION_TIME, Constants.ID_ATTR_EXPIRATION_TIME);
        m2mPocLayout.addAttribute(M2MConstants.ATTR_ONLINE_STATUS, Constants.ID_ATTR_ONLINE_STATUS);
        m2mPocLayout.addAttribute(M2MConstants.ATTR_CREATION_TIME, Constants.ID_ATTR_CREATION_TIME);
        m2mPocLayout.addAttribute(M2MConstants.ATTR_LAST_MODIFIED_TIME, Constants.ID_ATTR_LAST_MODIFIED_TIME);

        ResourcePathLayout m2mPocsLayout = new ResourcePathLayout(Constants.ID_RES_M2M_POCS, RM_STORAGE_RETRIEVE, m2mPocLayout,
                false, true);
        m2mPocsLayout.addAttribute(M2MConstants.ATTR_CREATION_TIME, Constants.ID_ATTR_CREATION_TIME);
        m2mPocsLayout.addAttribute(M2MConstants.ATTR_LAST_MODIFIED_TIME, Constants.ID_ATTR_LAST_MODIFIED_TIME);
        m2mPocsLayout.addAttribute(M2MConstants.ATTR_M2M_POC_COLLECTION, Constants.ID_ATTR_M2M_POC_COLLECTION);

        ResourcePathLayout attachedDevicesLayout = new ResourcePathLayout(Constants.ID_RES_ATTACHED_DEVICES,
                RM_STORAGE_RETRIEVE, null, false, false);
        attachedDevicesLayout.addAttribute(M2MConstants.ATTR_ATTACHED_DEVICE_COLLECTION,
                Constants.ID_ATTR_ATTACHED_DEVICE_COLLECTION);
        attachedDevicesLayout
                .addAttribute(M2MConstants.ATTR_SUBSCRIPTIONS_REFERENCE, Constants.ID_ATTR_SUBSCRIPTIONS_REFERENCE);
        attachedDevicesLayout.addAttribute(M2MConstants.ATTR_ACCESS_RIGHT_I_D, Constants.ID_ATTR_ACCESS_RIGHT_I_D);
        attachedDevicesLayout.addAttribute(M2MConstants.ATTR_CREATION_TIME, Constants.ID_ATTR_CREATION_TIME);
        attachedDevicesLayout.addAttribute(M2MConstants.ATTR_LAST_MODIFIED_TIME, Constants.ID_ATTR_LAST_MODIFIED_TIME);

        ResourcePathLayout sclAnncsLayout = new ResourcePathLayout(Constants.ID_RES_SCL_ANNCS, RM_STORAGE_RETRIEVE, null,
                false, false);
        sclAnncsLayout.addAttribute(M2MConstants.ATTR_SCL_ANNC_COLLECTION, Constants.ID_ATTR_SCL_ANNC_COLLECTION);
        sclAnncsLayout.addAttribute(M2MConstants.ATTR_SUBSCRIPTIONS_REFERENCE, Constants.ID_ATTR_SUBSCRIPTIONS_REFERENCE);
        sclAnncsLayout.addAttribute(M2MConstants.ATTR_ACCESS_RIGHT_I_D, Constants.ID_ATTR_ACCESS_RIGHT_I_D);
        sclAnncsLayout.addAttribute(M2MConstants.ATTR_CREATION_TIME, Constants.ID_ATTR_CREATION_TIME);
        sclAnncsLayout.addAttribute(M2MConstants.ATTR_LAST_MODIFIED_TIME, Constants.ID_ATTR_LAST_MODIFIED_TIME);

        ResourcePathLayout sclLayout = new ResourcePathLayout(Constants.ID_RES_SCL, RM_STORAGE_RETRIEVE, null, false, true);
        sclLayout.addAttribute(M2MConstants.ATTR_SCL_ID, Constants.ID_ATTR_SCL_ID);
        sclLayout.addAttribute(M2MConstants.ATTR_POCS, Constants.ID_ATTR_POCS);
        sclLayout.addAttribute(M2MConstants.ATTR_REM_TRIGGER_ADDR, Constants.ID_ATTR_REM_TRIGGER_ADDR);
        sclLayout.addAttribute(M2MConstants.ATTR_ONLINE_STATUS, Constants.ID_ATTR_ONLINE_STATUS);
        sclLayout.addAttribute(M2MConstants.ATTR_SERVER_CAPABILITY, Constants.ID_ATTR_SERVER_CAPABILITY);
        sclLayout.addAttribute(M2MConstants.ATTR_LINK, Constants.ID_ATTR_LINK);
        sclLayout.addAttribute(M2MConstants.ATTR_SCHEDULE, Constants.ID_ATTR_SCHEDULE);
        sclLayout.addAttribute(M2MConstants.ATTR_EXPIRATION_TIME, Constants.ID_ATTR_EXPIRATION_TIME);
        sclLayout.addAttribute(M2MConstants.ATTR_ACCESS_RIGHT_I_D, Constants.ID_ATTR_ACCESS_RIGHT_I_D);
        sclLayout.addAttribute(M2MConstants.ATTR_SEARCH_STRINGS, Constants.ID_ATTR_SEARCH_STRINGS);
        sclLayout.addAttribute(M2MConstants.ATTR_CREATION_TIME, Constants.ID_ATTR_CREATION_TIME);
        sclLayout.addAttribute(M2MConstants.ATTR_LAST_MODIFIED_TIME, Constants.ID_ATTR_LAST_MODIFIED_TIME);
        sclLayout.addAttribute(M2MConstants.ATTR_LOC_TARGET_DEVICE, Constants.ID_ATTR_LOC_TARGET_DEVICE);
        sclLayout.addAttribute(M2MConstants.ATTR_MGMT_PROTOCOL_TYPE, Constants.ID_ATTR_MGMT_PROTOCOL_TYPE);
        sclLayout.addAttribute(M2MConstants.ATTR_INTEGRITY_VAL_RESULTS, Constants.ID_ATTR_INTEGRITY_VAL_RESULTS);
        sclLayout.addAttribute(M2MConstants.ATTR_A_PO_C_HANDLING, Constants.ID_ATTR_A_PO_C_HANDLING);
        sclLayout.addAttribute(M2MConstants.ATTR_CONTAINERS_REFERENCE, Constants.ID_ATTR_CONTAINERS_REFERENCE);
        sclLayout.addAttribute(M2MConstants.ATTR_GROUPS_REFERENCE, Constants.ID_ATTR_GROUPS_REFERENCE);
        sclLayout.addAttribute(M2MConstants.ATTR_APPLICATIONS_REFERENCE, Constants.ID_ATTR_APPLICATIONS_REFERENCE);
        sclLayout.addAttribute(M2MConstants.ATTR_ACCESS_RIGHTS_REFERENCE, Constants.ID_ATTR_ACCESS_RIGHTS_REFERENCE);
        sclLayout.addAttribute(M2MConstants.ATTR_SUBSCRIPTIONS_REFERENCE, Constants.ID_ATTR_SUBSCRIPTIONS_REFERENCE);
        sclLayout.addAttribute(M2MConstants.ATTR_MGMT_OBJS_REFERENCE, Constants.ID_ATTR_MGMT_OBJS_REFERENCE);
        sclLayout.addAttribute(M2MConstants.ATTR_NOTIFICATION_CHANNELS_REFERENCE,
                Constants.ID_ATTR_NOTIFICATION_CHANNELS_REFERENCE);
        sclLayout.addAttribute(M2MConstants.ATTR_COMMUNICATION_CHANNELS_REFERENCE,
                Constants.ID_ATTR_COMMUNICATION_CHANNELS_REFERENCE);
        sclLayout.addAttribute(M2MConstants.ATTR_M2M_POCS_REFERENCE, Constants.ID_ATTR_M2M_POCS_REFERENCE);
        sclLayout.addAttribute(M2MConstants.ATTR_ATTACHED_DEVICES_REFERENCE, Constants.ID_ATTR_ATTACHED_DEVICES_REFERENCE);
        sclLayout.addSubResourceLayout(M2MConstants.RES_CONTAINERS, containersLayout);
        sclLayout.addSubResourceLayout(M2MConstants.RES_GROUPS, groupsLayout);
        sclLayout.addSubResourceLayout(M2MConstants.RES_APPLICATIONS, applicationsLayout);
        sclLayout.addSubResourceLayout(M2MConstants.RES_ACCESS_RIGHTS, accessRightsLayout);
        sclLayout.addSubResourceLayout(M2MConstants.RES_SUBSCRIPTIONS, subscriptionsLayout);
        sclLayout.addSubResourceLayout(M2MConstants.RES_MGMT_OBJS, mgmtObjsLayout);
        sclLayout.addSubResourceLayout(M2MConstants.RES_NOTIFICATION_CHANNELS, notificationChannelsLayout);
        sclLayout.addSubResourceLayout(M2MConstants.RES_COMMUNICATION_CHANNELS, communicationChannelsLayout);
        sclLayout.addSubResourceLayout(M2MConstants.RES_M2M_POCS, m2mPocsLayout);
        sclLayout.addSubResourceLayout(M2MConstants.RES_ATTACHED_DEVICES, attachedDevicesLayout);
        sclLayout.addSubResourceLayout(M2MConstants.RES_SCL_ANNCS, sclAnncsLayout);

        ResourcePathLayout sclsLayout = new ResourcePathLayout(Constants.ID_RES_SCLS, RM_STORAGE_RETRIEVE, sclLayout, false,
                true);
        sclsLayout.addAttribute(M2MConstants.ATTR_ACCESS_RIGHT_I_D, Constants.ID_ATTR_ACCESS_RIGHT_I_D);
        sclsLayout.addAttribute(M2MConstants.ATTR_CREATION_TIME, Constants.ID_ATTR_CREATION_TIME);
        sclsLayout.addAttribute(M2MConstants.ATTR_LAST_MODIFIED_TIME, Constants.ID_ATTR_LAST_MODIFIED_TIME);
        sclsLayout.addAttribute(M2MConstants.ATTR_SCL_COLLECTION, Constants.ID_ATTR_SCL_COLLECTION);
        sclsLayout.addAttribute(M2MConstants.ATTR_SUBSCRIPTIONS_REFERENCE, Constants.ID_ATTR_SUBSCRIPTIONS_REFERENCE);
        sclsLayout.addAttribute(M2MConstants.ATTR_MGMT_OBJS_REFERENCE, Constants.ID_ATTR_MGMT_OBJS_REFERENCE);
        sclsLayout.addSubResourceLayout(M2MConstants.RES_SUBSCRIPTIONS, subscriptionsLayout);
        sclsLayout.addSubResourceLayout(M2MConstants.RES_MGMT_OBJS, mgmtObjsLayout);

        ResourcePathLayout discoveryLayout = new ResourcePathLayout(Constants.ID_RES_DISCOVERY, RM_NO_STORAGE, null, false,
                true);

        ResourcePathLayout sclBaseLayout = new ResourcePathLayout(Constants.ID_RES_SCL_BASE, RM_STORAGE_RETRIEVE, null, false,
                true);
        sclBaseLayout.addAttribute(M2MConstants.ATTR_ACCESS_RIGHT_I_D, Constants.ID_ATTR_ACCESS_RIGHT_I_D);
        sclBaseLayout.addAttribute(M2MConstants.ATTR_SEARCH_STRINGS, Constants.ID_ATTR_SEARCH_STRINGS);
        sclBaseLayout.addAttribute(M2MConstants.ATTR_CREATION_TIME, Constants.ID_ATTR_CREATION_TIME);
        sclBaseLayout.addAttribute(M2MConstants.ATTR_LAST_MODIFIED_TIME, Constants.ID_ATTR_LAST_MODIFIED_TIME);
        sclBaseLayout.addAttribute(M2MConstants.ATTR_A_PO_C_HANDLING, Constants.ID_ATTR_A_PO_C_HANDLING);
        sclBaseLayout.addAttribute(M2MConstants.ATTR_SCLS_REFERENCE, Constants.ID_ATTR_SCLS_REFERENCE);
        sclBaseLayout.addAttribute(M2MConstants.ATTR_APPLICATIONS_REFERENCE, Constants.ID_ATTR_APPLICATIONS_REFERENCE);
        sclBaseLayout.addAttribute(M2MConstants.ATTR_CONTAINERS_REFERENCE, Constants.ID_ATTR_CONTAINERS_REFERENCE);
        sclBaseLayout.addAttribute(M2MConstants.ATTR_GROUPS_REFERENCE, Constants.ID_ATTR_GROUPS_REFERENCE);
        sclBaseLayout.addAttribute(M2MConstants.ATTR_ACCESS_RIGHTS_REFERENCE, Constants.ID_ATTR_ACCESS_RIGHTS_REFERENCE);
        sclBaseLayout.addAttribute(M2MConstants.ATTR_SUBSCRIPTIONS_REFERENCE, Constants.ID_ATTR_SUBSCRIPTIONS_REFERENCE);
        sclBaseLayout.addAttribute(M2MConstants.ATTR_DISCOVERY_REFERENCE, Constants.ID_ATTR_DISCOVERY_REFERENCE);
        sclBaseLayout.addSubResourceLayout(M2MConstants.RES_SCLS, sclsLayout);
        sclBaseLayout.addSubResourceLayout(M2MConstants.RES_APPLICATIONS, applicationsLayout);
        sclBaseLayout.addSubResourceLayout(M2MConstants.RES_CONTAINERS, containersLayout);
        sclBaseLayout.addSubResourceLayout(M2MConstants.RES_GROUPS, groupsLayout);
        sclBaseLayout.addSubResourceLayout(M2MConstants.RES_ACCESS_RIGHTS, accessRightsLayout);
        sclBaseLayout.addSubResourceLayout(M2MConstants.RES_SUBSCRIPTIONS, subscriptionsLayout);
        sclBaseLayout.addSubResourceLayout(M2MConstants.RES_DISCOVERY, discoveryLayout);

        return sclBaseLayout;
    }

    private boolean isLocalScl(URI requestingEntity) throws M2MException {
        return m2mContext.createLocalUri(requestingEntity, Constants.PATH_ROOT).equals(requestingEntity);
    }

    private RequestTarget getRequestTarget(String path) throws StorageException, XoException, M2MException {
        LOG.info("getRequestTarget with path: " + path);
        String resourcePath = null;
        int attributeId = 0;
        String attribute = null;
        String rawSubPath = null;
        PathLayout pathLayout = sclPathLayout;
        RequestTarget result = null;

        if (path == null || path.equals(M2MConstants.URI_SEP) || path.equals(Constants.PATH_ROOT)) {
            resourcePath = Constants.PATH_ROOT;
        } else {
            try {
                PathLayout subPathLayout = null;
                StringReader reader = new StringReader(path);
                String pathToken = null;
                reader.skipOffset(1);
                int previousOffset = 0;
                while (!reader.isTerminated() && pathLayout != null) {
                    previousOffset = reader.getOffset();
                    pathToken = reader.readUntil('/');
                    subPathLayout = pathLayout.getSubLayout(pathToken);
                    if (subPathLayout != null) {
                        if (LOG.isInfoEnabled()) {
                            LOG.info("Found sub-resource on path: " + pathToken);
                        }
                        pathLayout = subPathLayout;
                    } else if ((attributeId = pathLayout.getAttributeId(pathToken)) >= 0) {
                        // end: consume reader entirely
                        resourcePath = path.substring(0, previousOffset - 1);
                        attribute = pathToken;
                        if (LOG.isInfoEnabled()) {
                            LOG.info("Found attribute on path: " + pathToken);
                        }
                        if (!reader.isTerminated()) {
                            reader.skipOffset(1);
                            if (!reader.isTerminated()) {
                                rawSubPath = reader.getMessage().substring(reader.getOffset());
                                reader.skipOffset(rawSubPath.length());
                            }
                        }
                    } else {
                        if (LOG.isInfoEnabled()) {
                            LOG.info("Use wildcard: " + pathToken);
                        }
                        pathLayout = pathLayout.getWildcardLayout();
                    }
                    if (!reader.isTerminated()) {
                        reader.skipOffset(1);
                    }
                }
                if (resourcePath == null && reader.isTerminated()) {
                    if (path.endsWith(M2MConstants.URI_SEP)) {
                        resourcePath = path.substring(0, path.length() - 1);
                    } else {
                        resourcePath = path;
                    }
                }
            } catch (ParseException e) {
                // Ignore path is unknown
            }
        }

        if (pathLayout != null) {
            if (LOG.isInfoEnabled()) {
                LOG.info("Found resource path: " + resourcePath);
                LOG.info("Found resource: " + pathLayout.getResourceId());
                LOG.info("Found retrieve mode: " + pathLayout.getRetrieveMode());
            }
            if (pathLayout.getResourceId() == 0) {
                LOG.info("Ambiguous path, retrieve type from Xo resource");
                XoObject xoResource = retrieveXoResource(pathLayout.getRetrieveMode(), resourcePath);
                if (xoResource != null) {
                    int resourceId = pathLayout.getResourceIdFromTag(xoResource.getName(), attribute);
                    LOG.info("Resource id deducted from tag " + xoResource.getName() + ": " + resourceId);
                    if (resourceId != -1) {
                        result = new RequestTarget(getResourceController(resourceId), resourcePath, xoResource, attributeId,
                                attribute, rawSubPath);
                    }
                }
            } else if (pathLayout.isImplemented()) {
                if (pathLayout.getRetrieveMode() != RM_NO_STORAGE) {
                    XoObject xoResource = retrieveXoResource(pathLayout.getRetrieveMode(), resourcePath);
                    if (xoResource != null) {
                        result = new RequestTarget(getResourceController(pathLayout.getResourceId()), resourcePath, xoResource,
                                attributeId, attribute, rawSubPath);
                    }
                } else {
                    result = new RequestTarget(getResourceController(pathLayout.getResourceId()), resourcePath, null,
                            attributeId, attribute, rawSubPath);
                }
            } else {
                throw new M2MException("Targetted resource is not implemented", StatusCode.STATUS_NOT_IMPLEMENTED);
            }
        }
        return result;
    }

    private XoObject retrieveXoResource(int retrieveMode, String resourcePath) throws StorageException, XoException,
            M2MException {
        int slashIndex = 0;
        Iterator it = null;
        Document document = null;
        switch (retrieveMode) {
        case RM_STORAGE_RETRIEVE:
            if (LOG.isInfoEnabled()) {
                LOG.info("Search path " + resourcePath + " into storage");
            }
            document = storageContext.retrieve(null, resourcePath, null);
            if (document != null) {
                return xoService.readBinaryXmlXoObject(document.getContent());
            }
            return null;
        case RM_STORAGE_RETRIEVE_WITH_CONFIG:
            String grandParentPath = SclResource.getGrandParentPath(resourcePath);
            XoObject grandParentResource = null;
            try {
                grandParentResource = retrieveXoResource(RM_STORAGE_RETRIEVE, grandParentPath);
                Map storageConfig = SclResource.createStorageConfiguration(grandParentResource,
                        M2MConstants.TAG_ACY_STORAGE_CONFIGURATION);
                if (grandParentResource != null) {
                    if (LOG.isInfoEnabled()) {
                        LOG.info("Search path " + resourcePath + " into storage");
                    }
                    if (storageConfig != null) {
                        document = storageContext.retrieve(storageConfig, resourcePath, null);
                    } else {
                        document = storageContext.retrieve(null, resourcePath, null);
                    }
                    if (document != null) {
                        return xoService.readBinaryXmlXoObject(document.getContent());
                    }
                }
            } finally {
                if (grandParentResource != null) {
                    grandParentResource.free(true);
                }
            }
            return null;
        case RM_STORAGE_LATEST:
            slashIndex = resourcePath.lastIndexOf('/', resourcePath.length() - 1);
            if (slashIndex != -1) {
                String contentInstancesPath = resourcePath.substring(0, slashIndex);
                if (LOG.isInfoEnabled()) {
                    LOG.info("Search latest contentInstance in " + contentInstancesPath);
                }
                Condition condition = storageContext.getStorageFactory().createStringCondition(Constants.ATTR_TYPE,
                        Condition.ATTR_OP_EQUAL, Constants.TYPE_CONTENT_INSTANCE);
                SearchResult searchResult = storageContext.search(null, contentInstancesPath,
                        StorageRequestExecutor.SCOPE_ONE_LEVEL, condition, StorageRequestExecutor.ORDER_DESC, 1, true,
                        EmptyUtils.EMPTY_LIST);
                it = searchResult.getResults();
                if (it.hasNext()) {
                    document = (Document) it.next();
                    return xoService.readBinaryXmlXoObject(document.getContent());
                }
            }
            return null;
        case RM_STORAGE_OLDEST:
            slashIndex = resourcePath.lastIndexOf('/', resourcePath.length() - 1);
            if (slashIndex != -1) {
                String contentInstancesPath = resourcePath.substring(0, slashIndex);
                if (LOG.isInfoEnabled()) {
                    LOG.info("Search oldest contentInstance in " + contentInstancesPath);
                }
                Condition condition = storageContext.getStorageFactory().createStringCondition(Constants.ATTR_TYPE,
                        Condition.ATTR_OP_EQUAL, Constants.TYPE_CONTENT_INSTANCE);
                SearchResult searchResult = storageContext.search(null, contentInstancesPath,
                        StorageRequestExecutor.SCOPE_ONE_LEVEL, condition, StorageRequestExecutor.ORDER_ASC, 1, true,
                        EmptyUtils.EMPTY_LIST);
                it = searchResult.getResults();
                if (it.hasNext()) {
                    document = (Document) it.next();
                    return xoService.readBinaryXmlXoObject(document.getContent());
                }
            }
            return null;
        default:
            return null;
        }
    }

    private ResourceController getResourceController(int resourceId) {
        switch (resourceId) {
        case Constants.ID_RES_SCL_BASE:
            return SclBase.getInstance();
        case Constants.ID_RES_SCLS:
            return Scls.getInstance();
        case Constants.ID_RES_SCL:
            return Scl.getInstance();
        case Constants.ID_RES_APPLICATIONS:
            return Applications.getInstance();
        case Constants.ID_RES_APPLICATION:
            return Application.getInstance();
        case Constants.ID_RES_APPLICATION_ANNC:
            return null;
        case Constants.ID_RES_ACCESS_RIGHTS:
            return AccessRights.getInstance();
        case Constants.ID_RES_ACCESS_RIGHT:
            return AccessRight.getInstance();
        case Constants.ID_RES_ACCESS_RIGHT_ANNC:
            return null;
        case Constants.ID_RES_COMMUNICATION_CHANNELS:
            return CommunicationChannels.getInstance();
        case Constants.ID_RES_COMMUNICATION_CHANNEL:
            return CommunicationChannel.getInstance();
        case Constants.ID_RES_CONTAINERS:
            return Containers.getInstance();
        case Constants.ID_RES_CONTAINER:
            return Container.getInstance();
        case Constants.ID_RES_CONTAINER_ANNC:
            return null;
        case Constants.ID_RES_LOCATION_CONTAINER:
            return LocationContainer.getInstance();
        case Constants.ID_RES_LOCATION_CONTAINER_ANNC:
            return null;
        case Constants.ID_RES_CONTENT_INSTANCES:
            return ContentInstances.getInstance();
        case Constants.ID_RES_CONTENT_INSTANCE:
            return ContentInstance.getInstance();
        case Constants.ID_RES_GROUPS:
        case Constants.ID_RES_GROUP:
        case Constants.ID_RES_GROUP_ANNC:
        case Constants.ID_RES_MEMBERS_CONTENT:
            return null;
        case Constants.ID_RES_SUBSCRIPTIONS:
            return Subscriptions.getInstance();
        case Constants.ID_RES_SUBSCRIPTION:
            return Subscription.getInstance();
        case Constants.ID_RES_M2M_POCS:
            return M2MPocs.getInstance();
        case Constants.ID_RES_M2M_POC:
            return M2MPoc.getInstance();
        case Constants.ID_RES_MGMT_OBJS:
        case Constants.ID_RES_MGMT_OBJ:
        case Constants.ID_RES_PARAMETERS:
        case Constants.ID_RES_MGMT_CMD:
        case Constants.ID_RES_EXEC_INSTANCES:
        case Constants.ID_RES_EXEC_INSTANCE:
        case Constants.ID_RES_ATTACHED_DEVICES:
        case Constants.ID_RES_ATTACHED_DEVICE:
            return null;
        case Constants.ID_RES_NOTIFICATION_CHANNELS:
            return NotificationChannels.getInstance();
        case Constants.ID_RES_NOTIFICATION_CHANNEL:
            return NotificationChannel.getInstance();
        case Constants.ID_RES_DISCOVERY:
            return Discovery.getInstance();
        case Constants.ID_RES_SCL_ANNCS:
            return null;
        case Constants.ID_RES_SUBCONTAINERS:
            return Subcontainers.getInstance();
        }
        return null;
    }

    private String buildUriWithoutPath(URI uri) {
        StringBuffer buffer = new StringBuffer();
        String scheme = uri.getScheme();
        if (scheme != null) {
            buffer.append(scheme);
            buffer.append(':');
        }
        if (uri.isOpaque()) {
            buffer.append(uri.getSchemeSpecificPart());
        } else {
            String host = uri.getHost();
            String authority = uri.getAuthority();
            if (host != null) {
                String userInfo = uri.getUserInfo();
                int port = uri.getPort();
                buffer.append("//");
                if (userInfo != null) {
                    buffer.append(userInfo);
                    buffer.append('@');
                }
                boolean needBrackets = ((host.indexOf(':') >= 0) && host.charAt(0) != '[' && !host.endsWith("]"));
                if (needBrackets) {
                    buffer.append('[');
                }
                buffer.append(host);
                if (needBrackets) {
                    buffer.append(']');
                }
                if (port != -1) {
                    buffer.append(':');
                    buffer.append(port);
                }
                buffer.append('/');
            } else if (authority != null) {
                buffer.append("//");
                buffer.append(authority);
            }
        }
        return buffer.toString();
    }

    private RequestTarget getSclResourceFromTargetID(String transactionId, URI targetID) throws M2MException {
        XoObject xoObject = null;
        String subPath = null;
        try {
            String partialTargetId = buildUriWithoutPath(targetID);
            List conditions = new ArrayList();
            conditions.add(storageContext.getStorageFactory().createStringCondition(Constants.ATTR_TYPE,
                    Condition.ATTR_OP_EQUAL, Constants.TYPE_SCL));
            conditions.add(storageContext.getStorageFactory().createStringCondition(M2MConstants.ATTR_LINK,
                    Condition.ATTR_OP_STARTS_WITH, partialTargetId));
            Condition conditionPair = storageContext.getStorageFactory().createConjunction(Condition.COND_OP_AND, conditions);
            SearchResult searchResult = storageContext.search(null, Constants.PATH_SCLS + M2MConstants.URI_SEP,
                    StorageRequestExecutor.SCOPE_ONE_LEVEL, conditionPair, StorageRequestExecutor.ORDER_UNKNOWN, -1, true,
                    EmptyUtils.EMPTY_LIST);
            Iterator it = searchResult.getResults();
            String target = targetID.toString();
            String link = null;

            Document document = null;
            boolean end = false;
            int linkLength = 0;
            int targetLength = target.length();
            if (target.charAt(target.length() - 1) == '/') {
                targetLength = target.length() - 1;
            }
            while (!end && it.hasNext()) {
                document = (Document) it.next();
                subPath = document.getPath();
                xoObject = xoService.readBinaryXmlXoObject(document.getContent());
                try {
                    link = xoObject.getStringAttribute(M2MConstants.TAG_M2M_LINK);
                    linkLength = link.length();
                    if (link.charAt(link.length() - 1) == '/') {
                        linkLength = link.length() - 1;
                    }
                    if (targetLength == linkLength) {
                        end = target.regionMatches(true, 0, link, 0, linkLength);
                    } else if (targetLength > linkLength) {
                        end = (target.regionMatches(true, 0, link, 0, linkLength) && target.charAt(linkLength) == '/');
                    }
                } finally {
                    if (!end) {
                        xoObject.free(true);
                        xoObject = null;
                    }
                }
            }
        } catch (XoException e) {
            throw new M2MException("Cannot decode a server <scl> resource: " + e.getMessage(),
                    StatusCode.STATUS_INTERNAL_SERVER_ERROR, e);
        } catch (StorageException e) {
            throw new M2MException("Cannot search for a matching server <scl> resource: " + e.getMessage(),
                    StatusCode.STATUS_INTERNAL_SERVER_ERROR, e);
        }
        if (xoObject != null) {
            return new RequestTarget(null, subPath, xoObject, 0, null, null);
        }
        return null;
    }

    private XoObject getFirstOnlineM2MPoc(String transactionId, String sclPath) throws M2MException {
        XoObject xoObject = null;
        try {
            List conditions = new ArrayList(2);
            conditions.add(storageContext.getStorageFactory().createStringCondition(Constants.ATTR_TYPE,
                    Condition.ATTR_OP_EQUAL, Constants.TYPE_M2M_POC));
            conditions.add(storageContext.getStorageFactory().createIntegerCondition(M2MConstants.ATTR_ONLINE_STATUS,
                    Condition.ATTR_OP_EQUAL, new Integer(Constants.ID_ONLINE_STATUS_ONLINE)));
            Condition conditionPair = storageContext.getStorageFactory().createConjunction(Condition.COND_OP_AND, conditions);
            SearchResult searchResult = storageContext.search(null, sclPath + M2MConstants.URI_SEP + M2MConstants.RES_M2M_POCS
                    + M2MConstants.URI_SEP, StorageRequestExecutor.SCOPE_ONE_LEVEL, conditionPair,
                    StorageRequestExecutor.ORDER_UNKNOWN, 1, true, EmptyUtils.EMPTY_LIST);
            Iterator it = searchResult.getResults();
            if (it.hasNext()) {
                xoObject = xoService.readBinaryXmlXoObject(((Document) it.next()).getContent());
            }
        } catch (XoException e) {
            throw new M2MException("Cannot decode a server <m2mPoc> resource: " + e.getMessage(),
                    StatusCode.STATUS_INTERNAL_SERVER_ERROR, e);
        } catch (StorageException e) {
            throw new M2MException("Cannot search for a matching server <m2mPoc> resource: " + e.getMessage(),
                    StatusCode.STATUS_INTERNAL_SERVER_ERROR, e);
        }
        return xoObject;
    }

    private void retargetProxyIndication(ProxyIndication indication) throws IOException, M2MException {
        if (sclConfig.isGscl()) {
            // Send to NSCL
            URI nsclUri = nsclRegistration.getNsclUri();
            if (LOG.isInfoEnabled()) {
                LOG.info(indication.getTransactionId() + ": Send indication to \"" + nsclUri.toString() + "\"");
            }
            if (indication.getTargetID().toString().startsWith(nsclUri.toString())) {
                indication.setTargetHost(nsclUri);
            } else {
                indication.setProxyHost(nsclUri.getHost(), nsclUri.getPort());
            }
            indication.send();
        } else {
            // Search ONLINE m2mPoc for targetID
            RequestTarget scl = null;
            XoObject m2mPoc = null;
            try {
                scl = getSclResourceFromTargetID(indication.getTransactionId(), indication.getTargetID());
                if (scl != null) {
                    int onlineStatus = SclResource.getAndCheckOnlineStatus(scl.getResource(),
                            M2MConstants.TAG_M2M_ONLINE_STATUS, Constants.ID_MODE_REQUIRED);
                    switch (onlineStatus) {
                    case Constants.ID_ONLINE_STATUS_ONLINE:
                        m2mPoc = getFirstOnlineM2MPoc(indication.getTransactionId(), scl.getPath());
                        if (m2mPoc != null) {
                            URI contact = SclResource.getAndCheckURI(m2mPoc, M2MConstants.TAG_M2M_CONTACT_INFO + "."
                                    + M2MConstants.TAG_M2M_CONTACT_U_R_I, Constants.ID_MODE_REQUIRED);
                            if (LOG.isInfoEnabled()) {
                                LOG.info(indication.getTransactionId() + ": Send indication to \"" + contact.toString() + "\"");
                            }
                            // Send to GSCL
                            indication.setTargetHost(contact);
                            indication.send();
                        } else {
                            sendUnsuccessResponse(indication, StatusCode.STATUS_NOT_FOUND,
                                    "No <m2mPoc> found to address targetID \"" + indication.getTargetID() + "\"");
                        }
                        break;
                    case Constants.ID_ONLINE_STATUS_OFFLINE:
                        sendUnsuccessResponse(indication, StatusCode.STATUS_SERVICE_UNAVAILABLE, "<scl> is offline");
                        break;
                    default:
                        sendUnsuccessResponse(indication, StatusCode.STATUS_SERVICE_UNAVAILABLE, "<scl> is not reachable");
                        break;
                    }
                } else {
                    sendUnsuccessResponse(indication, StatusCode.STATUS_NOT_FOUND,
                            "TargetID does not correspond to any registered <scl> \"" + indication.getTargetID() + "\"");
                }
            } finally {
                if (scl != null) {
                    scl.getResource().free(true);
                }
                if (m2mPoc != null) {
                    m2mPoc.free(true);
                }
            }
        }
    }

    private void sendUnsuccessResponse(Indication indication, StatusCode statusCode, String message) throws IOException,
            M2MException {
        if (!indication.isCommitted()) {
            Response response = indication.createUnsuccessResponse(statusCode);
            XoObject errorInfo = null;
            try {
                errorInfo = xoService.newXmlXoObject(M2MConstants.TAG_M2M_ERROR_INFO);
                errorInfo.setNameSpace(M2MConstants.PREFIX_M2M);
                errorInfo.setStringAttribute(M2MConstants.TAG_M2M_STATUS_CODE, statusCode.name());
                errorInfo.setStringAttribute(M2MConstants.TAG_M2M_ADDITIONAL_INFO, message);
                response.setRepresentation(errorInfo);

                SclLogger.logResponseConfirm(response, statusCode, errorInfo, null);

                response.send();
            } catch (XoException e) {
                LOG.error(response.getTransactionId() + ": Cannot send unsuccess response with status (" + statusCode.name()
                        + ") and message (" + message + ")", e);
                throw new M2MException(response.getTransactionId() + ": Cannot send unsuccess response with status ("
                        + statusCode.name() + ") and message (" + message + ")", StatusCode.STATUS_INTERNAL_SERVER_ERROR, e);
            } finally {
                if (errorInfo != null) {
                    errorInfo.free(true);
                }
            }
        } else {
            throw new M2MException(indication.getTransactionId() + ": Cannot send unsuccess response with status ("
                    + statusCode.name() + ") and message (" + message + ")", StatusCode.STATUS_INTERNAL_SERVER_ERROR);
        }
    }

    private void restoreResources(Condition condition, VolatileResource resourceCtrl, SclTransaction transaction)
            throws ParseException, IOException, StorageException, XoException, M2MException {
        SearchResult result = storageContext.search(null, Constants.PATH_ROOT, StorageRequestExecutor.SCOPE_ONE_LEVEL,
                condition, StorageRequestExecutor.ORDER_UNKNOWN, -1, true, EmptyUtils.EMPTY_LIST);
        Iterator it = result.getResults();
        Document document = null;
        XoObject resource = null;
        while (it.hasNext()) {
            resource = null;
            document = (Document) it.next();
            try {
                resource = xoService.readBinaryXmlXoObject(document.getContent());
                resourceCtrl.reload(this, document.getPath(), resource, transaction);
            } finally {
                if (resource != null) {
                    resource.free(true);
                }
            }
        }
    }

    private void checkAuthentication(Indication indication) throws M2MException {
        Map reqEntityToIps = sclConfig.getReqEntityToIps();
        if (reqEntityToIps != null) {
            String[] ips = (String[]) sclConfig.getReqEntityToIps().get(indication.getRequestingEntity());
            if (ips != null) {
                String remoteAddr = indication.getRemoteAddr();
                if (Arrays.binarySearch(ips, remoteAddr) < 0) {
                    throw new M2MException("Authorization for requesting entity " + indication.getRequestingEntity()
                            + " failed", StatusCode.STATUS_PERMISSION_DENIED);
                }
            }
        }
    }

    public void init(M2MContext m2mContext) {
        this.m2mContext = m2mContext;
    }

    public void initRegistration(URI nsclUri, URI tpkDevUri) throws UnsupportedEncodingException {
        String name = (String) m2mContext.getAttribute("com.actility.servlet.song.HostName");
        URI computedNsclUri = nsclUri;
        if (computedNsclUri != null) {
            if (computedNsclUri.getPath() != null && computedNsclUri.getPath().endsWith(M2MConstants.URI_SEP)) {
                String nsclUriStr = computedNsclUri.toString();
                computedNsclUri = URI.create(nsclUriStr.substring(0, nsclUriStr.length() - 1));
            }
            computedNsclUri = computedNsclUri.normalize();
            if (LOG.isInfoEnabled()) {
                LOG.info("Initialize registration to NSCL on " + computedNsclUri.toString());
            }
            String systemReqEntity = computedNsclUri.toString() + M2MConstants.URI_SEP + M2MConstants.RES_APPLICATIONS
                    + M2MConstants.URI_SEP + "SYSTEM";
            String supportReqEntity = computedNsclUri.toString() + M2MConstants.URI_SEP + M2MConstants.RES_APPLICATIONS
                    + M2MConstants.URI_SEP + "SUPPORT";
            this.nsclRegistration = new Registration(name, this, computedNsclUri, tpkDevUri, systemReqEntity, supportReqEntity);
        } else {
            this.nsclRegistration = null;
        }
    }

    public URI getNsclUri() {
        if (sclConfig.isGscl()) {
            return nsclRegistration.getNsclUri();
        }
        return null;
    }

    public URI getLocalSclUri(URI reference) throws M2MException {
        return m2mContext.createLocalUri(reference, Constants.PATH_ROOT);
    }

    public URI createPocUriFromServerRoot(URI reference, String path) throws M2MException {
        URI result = m2mContext.createLocalUri(reference, Constants.PATH_ROOT);
        String resultStr = result.toString();
        result = URI.create(resultStr.substring(0, resultStr.length() - m2mContext.getApplicationPath().length()) + path);
        return result;
    }

    public SclConfig getSclConfig() {
        return sclConfig;
    }

    public M2MContext getM2MContext() {
        return m2mContext;
    }

    public StorageRequestExecutor getStorageContext() {
        return storageContext;
    }

    public XoService getXoService() {
        return xoService;
    }

    public StatsManager getStatsManager() {
        return statsManager;
    }

    public int getDefaultResourceId(String path) {
        PathLayout pathLayout = sclPathLayout;

        if (path != null && !path.equals(Constants.PATH_ROOT)) {
            try {
                PathLayout subPathLayout = null;
                StringReader reader = new StringReader(path);
                String pathToken = null;
                reader.skipOffset(1);
                while (!reader.isTerminated() && pathLayout != null) {
                    pathToken = reader.readUntil('/');
                    subPathLayout = pathLayout.getSubLayout(pathToken);
                    if (subPathLayout != null) {
                        pathLayout = subPathLayout;
                    } else if (pathLayout.getAttributeId(pathToken) >= 0) {
                        pathLayout = null;
                    } else {
                        pathLayout = pathLayout.getWildcardLayout();
                    }
                    if (!reader.isTerminated()) {
                        reader.skipOffset(1);
                    }
                }
            } catch (ParseException e) {
                // Ignore path is unknown
            }
        }
        if (pathLayout != null) {
            return pathLayout.getDefaultResourceId();
        }
        return -1;
    }

    public ResourceController getControllerFromTag(String tag) {
        if (tag.equals(M2MConstants.TAG_M2M_SCL_BASE)) {
            return SclBase.getInstance();
        } else if (tag.equals(M2MConstants.TAG_M2M_SCLS)) {
            return Scls.getInstance();
        } else if (tag.equals(M2MConstants.TAG_M2M_SCL)) {
            return Scl.getInstance();
        } else if (tag.equals(M2MConstants.TAG_M2M_APPLICATIONS)) {
            return Applications.getInstance();
        } else if (tag.equals(M2MConstants.TAG_M2M_APPLICATION)) {
            return Application.getInstance();
        } else if (tag.equals(M2MConstants.TAG_M2M_APPLICATION_ANNC)) {
            return null;
        } else if (tag.equals(M2MConstants.TAG_M2M_ACCESS_RIGHTS)) {
            return AccessRights.getInstance();
        } else if (tag.equals(M2MConstants.TAG_M2M_ACCESS_RIGHT)) {
            return AccessRight.getInstance();
        } else if (tag.equals(M2MConstants.TAG_M2M_ACCESS_RIGHT_ANNC)) {
            return null;
        } else if (tag.equals(M2MConstants.TAG_M2M_COMMUNICATION_CHANNELS)) {
            return CommunicationChannels.getInstance();
        } else if (tag.equals(M2MConstants.TAG_M2M_COMMUNICATION_CHANNEL)) {
            return CommunicationChannel.getInstance();
        } else if (tag.equals(M2MConstants.TAG_M2M_CONTAINERS)) {
            return Containers.getInstance();
        } else if (tag.equals(M2MConstants.TAG_M2M_CONTAINER)) {
            return Container.getInstance();
        } else if (tag.equals(M2MConstants.TAG_M2M_CONTAINER_ANNC)) {
            return null;
        } else if (tag.equals(M2MConstants.TAG_M2M_LOCATION_CONTAINER)) {
            return LocationContainer.getInstance();
        } else if (tag.equals(M2MConstants.TAG_M2M_LOCATION_CONTAINER_ANNC)) {
            return null;
        } else if (tag.equals(M2MConstants.TAG_M2M_CONTENT_INSTANCES)) {
            return ContentInstances.getInstance();
        } else if (tag.equals(M2MConstants.TAG_M2M_CONTENT_INSTANCE)) {
            return ContentInstance.getInstance();
        } else if (tag.equals(M2MConstants.TAG_M2M_GROUPS)) {
            return null;
        } else if (tag.equals(M2MConstants.TAG_M2M_GROUP)) {
            return null;
        } else if (tag.equals(M2MConstants.TAG_M2M_GROUP_ANNC)) {
            return null;
        } else if (tag.equals(M2MConstants.TAG_M2M_SUBSCRIPTIONS)) {
            return Subscriptions.getInstance();
        } else if (tag.equals(M2MConstants.TAG_M2M_SUBSCRIPTION)) {
            return Subscription.getInstance();
        } else if (tag.equals(M2MConstants.TAG_M2M_M2M_POCS)) {
            return M2MPocs.getInstance();
        } else if (tag.equals(M2MConstants.TAG_M2M_M2M_POC)) {
            return M2MPoc.getInstance();
        } else if (tag.equals(M2MConstants.TAG_M2M_NOTIFICATION_CHANNELS)) {
            return NotificationChannels.getInstance();
        } else if (tag.equals(M2MConstants.TAG_M2M_NOTIFICATION_CHANNEL)) {
            return NotificationChannel.getInstance();
        } else if (tag.equals(M2MConstants.TAG_M2M_DISCOVERY)) {
            return Discovery.getInstance();
        } else if (tag.equals(M2MConstants.TAG_M2M_SUBCONTAINERS)) {
            return Subcontainers.getInstance();
        }
        return null;
    }

    public Object[] getControllerAndResource(String path) throws StorageException, XoException, M2MException {
        RequestTarget requestTarget = getRequestTarget(path);
        if (requestTarget != null && requestTarget.getAttribute() == null) {
            return new Object[] { requestTarget.getController(), requestTarget.getResource() };
        }
        return null;
    }

    public XoObject getXoResource(String path) throws StorageException, XoException {
        Document document = storageContext.retrieve(null, path, null);
        byte[] rawResource = document.getContent();
        if (rawResource != null) {
            return xoService.readBinaryXmlXoObject(rawResource);
        }
        return null;
    }

    public String startResourceTimer(long timeout, String path, int ctrlId, Serializable info) {
        return m2mContext.startTimer(timeout, new TimerData(path, ctrlId, info));
    }

    private void ready(boolean fsProperlyCreated) {
        state = ST_READY;
        if (fsProperlyCreated && nsclRegistration != null) {
            if (LOG.isInfoEnabled()) {
                LOG.info("Start registration for: " + nsclRegistration.getNsclUri().toString());
            }
            nsclRegistration.register();
        }
        synchronized (this) {
            this.notify();
        }
    }

    public synchronized void start(File resourcesFolder) throws ParseException, UnsupportedEncodingException, IOException,
            XoException, StorageException, M2MException {
        XoObject sclBase = null;
        try {
            sclBase = getXoResource(Constants.PATH_ROOT);
            if (sclBase == null) {
                LOG.info("SclBase is not created, create a new one");
                SclTransaction transaction = new SclTransaction(storageContext);
                SclBase.getInstance().createResource(this, Constants.PATH_ROOT, transaction);
                transaction.execute();

                URI gsclCoapBaseUri = null;
                URI gsclHttpBaseUri = null;
                URI nsclBaseUri = null;
                URI tpkDevUri = null;
                if (nsclRegistration == null) {
                    nsclBaseUri = m2mContext.createLocalUri(URI.create("http://a.com"), Constants.PATH_ROOT);
                } else {
                    gsclCoapBaseUri = m2mContext.createLocalUri(URI.create("coap://a.com"), Constants.PATH_ROOT);
                    gsclHttpBaseUri = m2mContext.createLocalUri(URI.create("http://a.com"), Constants.PATH_ROOT);
                    nsclBaseUri = nsclRegistration.getNsclUri();
                    tpkDevUri = nsclRegistration.getTpkDevUri();
                }
                String domainName = (String) m2mContext.getAttribute("com.actility.servlet.song.DomainName");
                M2MFileSystemLoader fsLoader = new M2MFileSystemLoader(m2mContext, nsclBaseUri, tpkDevUri, gsclCoapBaseUri,
                        gsclHttpBaseUri, domainName, resourcesFolder);
                synchronized (this) {
                    if (fsLoader.populate()) {
                        ready(fsLoader.isProperlyCreated());
                    } else {
                        this.wait();
                    }
                }
                if (!fsLoader.isProperlyCreated()) {
                    throw new M2MException("Cannot create M2M file system", StatusCode.STATUS_INTERNAL_SERVER_ERROR);
                }
            } else {
                LOG.info("SclBase is already created, restore volatile data");

                SclTransaction transaction = new SclTransaction(getStorageContext());

                // Restore <sclBase>
                SclBase.getInstance().reload(this, Constants.PATH_ROOT, sclBase, transaction);
                // Restore <accessRight>
                Condition condition = storageContext.getStorageFactory().createStringCondition(Constants.ATTR_TYPE,
                        Condition.ATTR_OP_EQUAL, Constants.TYPE_ACCESS_RIGHT);
                restoreResources(condition, AccessRight.getInstance(), transaction);
                // Restore <application>
                condition = storageContext.getStorageFactory().createStringCondition(Constants.ATTR_TYPE,
                        Condition.ATTR_OP_EQUAL, Constants.TYPE_APPLICATION);
                restoreResources(condition, Application.getInstance(), transaction);
                // Restore <container>
                condition = storageContext.getStorageFactory().createStringCondition(Constants.ATTR_TYPE,
                        Condition.ATTR_OP_EQUAL, Constants.TYPE_CONTAINER);
                restoreResources(condition, Container.getInstance(), transaction);
                // Restore <locationContainer>
                condition = storageContext.getStorageFactory().createStringCondition(Constants.ATTR_TYPE,
                        Condition.ATTR_OP_EQUAL, Constants.TYPE_LOCATION_CONTAINER);
                restoreResources(condition, LocationContainer.getInstance(), transaction);
                // Restore <scl>
                condition = storageContext.getStorageFactory().createStringCondition(Constants.ATTR_TYPE,
                        Condition.ATTR_OP_EQUAL, Constants.TYPE_SCL);
                restoreResources(condition, Scl.getInstance(), transaction);
                // Restore <notificationChannel>
                condition = storageContext.getStorageFactory().createStringCondition(Constants.ATTR_TYPE,
                        Condition.ATTR_OP_EQUAL, Constants.TYPE_NOTIFICATION_CHANNEL);
                restoreResources(condition, NotificationChannel.getInstance(), transaction);
                // Restore <m2mPocs>
                condition = storageContext.getStorageFactory().createStringCondition(Constants.ATTR_TYPE,
                        Condition.ATTR_OP_EQUAL, Constants.TYPE_M2M_POC);
                restoreResources(condition, M2MPoc.getInstance(), transaction);
                // Restore <subscription>
                condition = storageContext.getStorageFactory().createStringCondition(Constants.ATTR_TYPE,
                        Condition.ATTR_OP_EQUAL, Constants.TYPE_SUBSCRIPTION);
                restoreResources(condition, Subscription.getInstance(), transaction);
                // Restore contentInstances
                condition = storageContext.getStorageFactory().createStringCondition(Constants.ATTR_TYPE,
                        Condition.ATTR_OP_EQUAL, Constants.TYPE_CONTENT_INSTANCES);
                restoreResources(condition, ContentInstances.getInstance(), transaction);

                transaction.execute();

                ready(true);
            }
        } catch (InterruptedException e) {
            LOG.error("InterruptedException while waiting for completion of the storage population job", e);
        } finally {
            if (sclBase != null) {
                sclBase.free(true);
            }
        }
    }

    public synchronized void stop() {
        state = ST_STOPPED;
        if (nsclRegistration != null) {
            if (LOG.isInfoEnabled()) {
                LOG.info("Stop registration for: " + nsclRegistration.getNsclUri().toString());
            }
            nsclRegistration.unregister();
        }
    }

    public synchronized void doCreateIndication(Indication indication) throws IOException, M2MException {
        if (LOG.isInfoEnabled()) {
            LOG.info(indication.getTransactionId() + ": >>> Received doCreateIndication to targetID \""
                    + indication.getTargetID() + "\"");
        }
        if (Profiler.getInstance().isTraceEnabled()) {
            Profiler.getInstance().trace(
                    indication.getTransactionId() + ": >>> Received doCreateIndication to targetID \""
                            + indication.getTargetID() + "\"");
        }
        if (state == ST_READY || (state == ST_INIT && isLocalScl(indication.getRequestingEntity()))) {
            RequestTarget requestTarget = null;
            try {

                checkAuthentication(indication);

                requestTarget = getRequestTarget(indication.getTargetPath());
                if (Profiler.getInstance().isTraceEnabled()) {
                    Profiler.getInstance().trace(
                            indication.getTransactionId() + ": Resource on path \"" + indication.getTargetPath()
                                    + "\" has been retrieved");
                }
                if (requestTarget == null) {
                    sendUnsuccessResponse(indication, StatusCode.STATUS_NOT_FOUND,
                            "Resource on URI " + indication.getTargetID() + " does not exist");
                } else if (requestTarget.getAttribute() != null) {
                    requestTarget.getController().doCreateIndication(this, requestTarget.getPath(),
                            requestTarget.getResource(), indication, requestTarget);
                } else {
                    requestTarget.getController().doCreateIndication(this, requestTarget.getPath(),
                            requestTarget.getResource(), indication);
                }
            } catch (ParseException e) {
                LOG.error(indication.getTransactionId() + ": Problem while parsing a resource or representation", e);
                sendUnsuccessResponse(indication, StatusCode.STATUS_INTERNAL_SERVER_ERROR,
                        "Problem while parsing a resource or representation: " + e.getMessage());
            } catch (StorageException e) {
                LOG.error(indication.getTransactionId() + ": Problem while accessing the storage", e);
                sendUnsuccessResponse(indication, StatusCode.STATUS_INTERNAL_SERVER_ERROR,
                        "Problem while accessing the storage: " + e.getMessage());
            } catch (XoException e) {
                if (XoParseException.class == e.getClass()) {
                    LOG.error(indication.getTransactionId() + ": Problem while reading request content", e);
                    sendUnsuccessResponse(indication, StatusCode.STATUS_BAD_REQUEST, "Problem while reading request content: "
                            + e.getMessage());
                } else if (XoUnknownTypeException.class == e.getClass()) {
                    LOG.error(indication.getTransactionId() + ": Problem while reading request content", e);
                    sendUnsuccessResponse(indication, StatusCode.STATUS_BAD_REQUEST, "Problem while reading request content: "
                            + e.getMessage());
                } else {
                    LOG.error(indication.getTransactionId() + ": Problem while reading XO object", e);
                    sendUnsuccessResponse(indication, StatusCode.STATUS_INTERNAL_SERVER_ERROR,
                            "Problem while reading XO object: " + e.getMessage());
                }
            } catch (RuntimeException e) {
                LOG.error(indication.getTransactionId() + ": Internal server error", e);
                sendUnsuccessResponse(indication, StatusCode.STATUS_INTERNAL_SERVER_ERROR,
                        "Internal server error: " + e.getMessage());
            } catch (M2MException e) {
                sendUnsuccessResponse(indication, e.getStatusCode(), e.getMessage());
            } finally {
                if (requestTarget != null && requestTarget.getResource() != null) {
                    requestTarget.getResource().free(true);
                }
            }
        } else {
            sendUnsuccessResponse(indication, StatusCode.STATUS_SERVICE_UNAVAILABLE, "Service is unavailable for now");
        }
    }

    public synchronized void doRetrieveIndication(Indication indication) throws IOException, M2MException {
        if (LOG.isInfoEnabled()) {
            LOG.info(indication.getTransactionId() + ": >>> Received doRetrieveIndication to targetID \""
                    + indication.getTargetID() + "\"");
        }
        if (Profiler.getInstance().isTraceEnabled()) {
            Profiler.getInstance().trace(
                    indication.getTransactionId() + ": >>> Received doRetrieveIndication to targetID \""
                            + indication.getTargetID() + "\"");
        }
        if (state == ST_READY || (state == ST_INIT && isLocalScl(indication.getRequestingEntity()))) {
            RequestTarget requestTarget = null;
            try {
                checkAuthentication(indication);

                requestTarget = getRequestTarget(indication.getTargetPath());
                if (Profiler.getInstance().isTraceEnabled()) {
                    Profiler.getInstance().trace(
                            indication.getTransactionId() + ": Resource on path \"" + indication.getTargetPath()
                                    + "\" has been retrieved");
                }
                if (requestTarget == null) {
                    sendUnsuccessResponse(indication, StatusCode.STATUS_NOT_FOUND,
                            "Resource on URI " + indication.getTargetID() + " does not exist");
                } else if (requestTarget.getAttribute() != null) {
                    requestTarget.getController().doRetrieveIndication(this, requestTarget.getPath(),
                            requestTarget.getResource(), indication, requestTarget);
                } else {
                    requestTarget.getController().doRetrieveIndication(this, requestTarget.getPath(),
                            requestTarget.getResource(), indication);
                }
            } catch (ParseException e) {
                LOG.error(indication.getTransactionId() + ": Problem while parsing a resource or representation", e);
                sendUnsuccessResponse(indication, StatusCode.STATUS_INTERNAL_SERVER_ERROR,
                        "Problem while parsing a resource or representation: " + e.getMessage());
            } catch (StorageException e) {
                LOG.error(indication.getTransactionId() + ": Problem while accessing the storage", e);
                sendUnsuccessResponse(indication, StatusCode.STATUS_INTERNAL_SERVER_ERROR,
                        "Problem while accessing the storage: " + e.getMessage());
            } catch (XoException e) {
                if (XoParseException.class == e.getClass()) {
                    LOG.error(indication.getTransactionId() + ": Problem while reading request content", e);
                    sendUnsuccessResponse(indication, StatusCode.STATUS_BAD_REQUEST, "Problem while reading request content: "
                            + e.getMessage());
                } else if (XoUnknownTypeException.class == e.getClass()) {
                    LOG.error(indication.getTransactionId() + ": Problem while reading request content", e);
                    sendUnsuccessResponse(indication, StatusCode.STATUS_BAD_REQUEST, "Problem while reading request content: "
                            + e.getMessage());
                } else {
                    LOG.error(indication.getTransactionId() + ": Problem while reading XO object", e);
                    sendUnsuccessResponse(indication, StatusCode.STATUS_INTERNAL_SERVER_ERROR,
                            "Problem while reading XO object: " + e.getMessage());
                }
            } catch (RuntimeException e) {
                LOG.error(indication.getTransactionId() + ": Internal server error", e);
                sendUnsuccessResponse(indication, StatusCode.STATUS_INTERNAL_SERVER_ERROR,
                        "Internal server error: " + e.getMessage());
            } catch (M2MException e) {
                sendUnsuccessResponse(indication, e.getStatusCode(), e.getMessage());
            } finally {
                if (requestTarget != null && requestTarget.getResource() != null) {
                    requestTarget.getResource().free(true);
                }
            }
        } else {
            sendUnsuccessResponse(indication, StatusCode.STATUS_SERVICE_UNAVAILABLE, "Service is unavailable for now");
        }
    }

    public synchronized void doUpdateIndication(Indication indication) throws IOException, M2MException {
        if (LOG.isInfoEnabled()) {
            LOG.info(indication.getTransactionId() + ": >>> Received doUpdateIndication to targetID \""
                    + indication.getTargetID() + "\"");
        }
        if (Profiler.getInstance().isTraceEnabled()) {
            Profiler.getInstance().trace(
                    indication.getTransactionId() + ": >>> Received doUpdateIndication to targetID \""
                            + indication.getTargetID() + "\"");
        }
        if (state == ST_READY || (state == ST_INIT && isLocalScl(indication.getRequestingEntity()))) {
            RequestTarget requestTarget = null;
            try {
                checkAuthentication(indication);

                requestTarget = getRequestTarget(indication.getTargetPath());
                if (Profiler.getInstance().isTraceEnabled()) {
                    Profiler.getInstance().trace(
                            indication.getTransactionId() + ": Resource on path \"" + indication.getTargetPath()
                                    + "\" has been retrieved");
                }
                if (requestTarget == null) {
                    sendUnsuccessResponse(indication, StatusCode.STATUS_NOT_FOUND,
                            "Resource on URI " + indication.getTargetID() + " does not exist");
                } else if (requestTarget.getAttribute() != null) {
                    requestTarget.getController().doUpdateIndication(this, requestTarget.getPath(),
                            requestTarget.getResource(), indication, requestTarget);
                } else {
                    requestTarget.getController().doUpdateIndication(this, requestTarget.getPath(),
                            requestTarget.getResource(), indication);
                }
            } catch (ParseException e) {
                LOG.error(indication.getTransactionId() + ": Problem while parsing a resource or representation", e);
                sendUnsuccessResponse(indication, StatusCode.STATUS_INTERNAL_SERVER_ERROR,
                        "Problem while parsing a resource or representation: " + e.getMessage());
            } catch (StorageException e) {
                LOG.error(indication.getTransactionId() + ": Problem while accessing the storage", e);
                sendUnsuccessResponse(indication, StatusCode.STATUS_INTERNAL_SERVER_ERROR,
                        "Problem while accessing the storage: " + e.getMessage());
            } catch (XoException e) {
                if (XoParseException.class == e.getClass()) {
                    LOG.error(indication.getTransactionId() + ": Problem while reading request content", e);
                    sendUnsuccessResponse(indication, StatusCode.STATUS_BAD_REQUEST, "Problem while reading request content: "
                            + e.getMessage());
                } else if (XoUnknownTypeException.class == e.getClass()) {
                    LOG.error(indication.getTransactionId() + ": Problem while reading request content", e);
                    sendUnsuccessResponse(indication, StatusCode.STATUS_BAD_REQUEST, "Problem while reading request content: "
                            + e.getMessage());
                } else {
                    LOG.error(indication.getTransactionId() + ": Problem while reading XO object", e);
                    sendUnsuccessResponse(indication, StatusCode.STATUS_INTERNAL_SERVER_ERROR,
                            "Problem while reading XO object: " + e.getMessage());
                }
            } catch (RuntimeException e) {
                LOG.error(indication.getTransactionId() + ": Internal server error", e);
                sendUnsuccessResponse(indication, StatusCode.STATUS_INTERNAL_SERVER_ERROR,
                        "Internal server error: " + e.getMessage());
            } catch (M2MException e) {
                sendUnsuccessResponse(indication, e.getStatusCode(), e.getMessage());
            } finally {
                if (requestTarget != null && requestTarget.getResource() != null) {
                    requestTarget.getResource().free(true);
                }
            }
        } else {
            sendUnsuccessResponse(indication, StatusCode.STATUS_SERVICE_UNAVAILABLE, "Service is unavailable for now");
        }
    }

    public synchronized void doDeleteIndication(Indication indication) throws IOException, M2MException {
        if (LOG.isInfoEnabled()) {
            LOG.info(indication.getTransactionId() + ": >>> Received doDeleteIndication to targetID \""
                    + indication.getTargetID() + "\"");
        }
        if (Profiler.getInstance().isTraceEnabled()) {
            Profiler.getInstance().trace(
                    indication.getTransactionId() + ": >>> Received doDeleteIndication to targetID \""
                            + indication.getTargetID() + "\"");
        }
        if (state == ST_READY || (state == ST_INIT && isLocalScl(indication.getRequestingEntity()))) {
            RequestTarget requestTarget = null;
            // xoService.dumptStats();
            try {
                checkAuthentication(indication);

                requestTarget = getRequestTarget(indication.getTargetPath());
                if (Profiler.getInstance().isTraceEnabled()) {
                    Profiler.getInstance().trace(
                            indication.getTransactionId() + ": Resource on path \"" + indication.getTargetPath()
                                    + "\" has been retrieved");
                }
                if (requestTarget == null) {
                    sendUnsuccessResponse(indication, StatusCode.STATUS_NOT_FOUND,
                            "Resource on URI " + indication.getTargetID() + " does not exist");
                } else if (requestTarget.getAttribute() != null) {
                    requestTarget.getController().doDeleteIndication(this, requestTarget.getPath(),
                            requestTarget.getResource(), indication, requestTarget);
                } else {
                    requestTarget.getController().doDeleteIndication(this, requestTarget.getPath(),
                            requestTarget.getResource(), indication);
                }
            } catch (ParseException e) {
                LOG.error(indication.getTransactionId() + ": Problem while parsing a resource or representation", e);
                sendUnsuccessResponse(indication, StatusCode.STATUS_INTERNAL_SERVER_ERROR,
                        "Problem while parsing a resource or representation: " + e.getMessage());
            } catch (StorageException e) {
                LOG.error(indication.getTransactionId() + ": Problem while accessing the storage", e);
                sendUnsuccessResponse(indication, StatusCode.STATUS_INTERNAL_SERVER_ERROR,
                        "Problem while accessing the storage: " + e.getMessage());
            } catch (XoException e) {
                if (XoParseException.class == e.getClass()) {
                    LOG.error(indication.getTransactionId() + ": Problem while reading request content", e);
                    sendUnsuccessResponse(indication, StatusCode.STATUS_BAD_REQUEST, "Problem while reading request content: "
                            + e.getMessage());
                } else if (XoUnknownTypeException.class == e.getClass()) {
                    LOG.error(indication.getTransactionId() + ": Problem while reading request content", e);
                    sendUnsuccessResponse(indication, StatusCode.STATUS_BAD_REQUEST, "Problem while reading request content: "
                            + e.getMessage());
                } else {
                    LOG.error(indication.getTransactionId() + ": Problem while reading XO object", e);
                    sendUnsuccessResponse(indication, StatusCode.STATUS_INTERNAL_SERVER_ERROR,
                            "Problem while reading XO object: " + e.getMessage());
                }
            } catch (RuntimeException e) {
                LOG.error(indication.getTransactionId() + ": Internal server error", e);
                sendUnsuccessResponse(indication, StatusCode.STATUS_INTERNAL_SERVER_ERROR,
                        "Internal server error: " + e.getMessage());
            } catch (M2MException e) {
                sendUnsuccessResponse(indication, e.getStatusCode(), e.getMessage());
            } finally {
                if (requestTarget != null && requestTarget.getResource() != null) {
                    requestTarget.getResource().free(true);
                }
            }
        } else {
            sendUnsuccessResponse(indication, StatusCode.STATUS_SERVICE_UNAVAILABLE, "Service is unavailable for now");
        }
        // xoService.dumptStats();
    }

    public synchronized void doSuccessConfirm(Confirm confirm) throws IOException, M2MException {
        if (LOG.isInfoEnabled()) {
            LOG.info(confirm.getTransactionId() + ": >>> Received doSuccessConfirm with status \""
                    + confirm.getStatusCode().name() + "\"");
        }
        if (Profiler.getInstance().isTraceEnabled()) {
            Profiler.getInstance().trace(
                    confirm.getTransactionId() + ": >>> Received doSuccessConfirm with status \""
                            + confirm.getStatusCode().name() + "\"");
        }
        if (state == ST_INIT) {
            M2MFileSystemLoader fsLoader = (M2MFileSystemLoader) confirm.getRequest().getAttribute(
                    M2MFileSystemLoader.AT_FS_LOADER);
            if (fsLoader.doSuccessConfirm(confirm)) {
                ready(fsLoader.isProperlyCreated());
            }
        } else {
            Indication indication = null;
            try {
                indication = (Indication) confirm.getRequest().getAttribute(Constants.AT_INDICATION);
                if (indication != null) {
                    Response response = indication.createResponseFromConfirm(confirm);
                    SclLogger.logResponseConfirm(response, confirm.getStatusCode(), confirm.getRawBytes(),
                            confirm.getRawContentType());
                    response.send();
                } else {
                    Object requestor = confirm.getRequest().getAttribute(Constants.AT_REQUESTOR);
                    if (requestor != null) {
                        if (requestor.getClass() == Registration.class) {
                            ((Registration) requestor).doSuccessConfirm(confirm);
                        } else {
                            ((SubManager) requestor).doSuccessConfirm(confirm);
                        }
                    }
                }
            } catch (RuntimeException e) {
                LOG.error("Internal server error", e);
            }
        }
    }

    public synchronized void doUnsuccessConfirm(Confirm confirm) throws IOException {
        if (LOG.isInfoEnabled()) {
            LOG.info(confirm.getTransactionId() + ": >>> Received doUnsuccessConfirm with status \""
                    + confirm.getStatusCode().name() + "\"");
        }
        if (Profiler.getInstance().isTraceEnabled()) {
            Profiler.getInstance().trace(
                    confirm.getTransactionId() + ": >>> Received doUnsuccessConfirm with status \""
                            + confirm.getStatusCode().name() + "\"");
        }
        if (state == ST_INIT) {
            M2MFileSystemLoader fsLoader = (M2MFileSystemLoader) confirm.getRequest().getAttribute(
                    M2MFileSystemLoader.AT_FS_LOADER);
            if (fsLoader.doUnsuccessConfirm(confirm)) {
                ready(fsLoader.isProperlyCreated());
            }
        } else {
            Indication indication = null;
            try {
                indication = (Indication) confirm.getRequest().getAttribute(Constants.AT_INDICATION);
                if (indication != null) {
                    Response response = indication.createResponseFromConfirm(confirm);
                    SclLogger.logResponseConfirm(response, confirm.getStatusCode(), confirm.getRawBytes(),
                            confirm.getRawContentType());
                    response.send();
                } else {
                    Object requestor = confirm.getRequest().getAttribute(Constants.AT_REQUESTOR);
                    if (requestor != null) {
                        if (requestor.getClass() == Registration.class) {
                            ((Registration) requestor).doUnsuccessConfirm(confirm);
                        } else {
                            ((SubManager) requestor).doUnsuccessConfirm(confirm);
                        }
                    }
                }
            } catch (RuntimeException e) {
                LOG.error("Internal server error", e);
            }
        }
    }

    public synchronized void timeout(String timerId, M2MSession session, Serializable info) {
        if (LOG.isInfoEnabled()) {
            if (session != null) {
                LOG.info(timerId + "-" + session.getId() + ": Received timeout");
            } else {
                LOG.info(timerId + ": Received timeout");
            }
        }
        if (Profiler.getInstance().isTraceEnabled()) {
            if (session != null) {
                Profiler.getInstance().trace(timerId + "-" + session.getId() + ": Received timeout");
            } else {
                Profiler.getInstance().trace(timerId + ": Received timeout");
            }
        }

        try {
            if (info.getClass() == Registration.class) {
                Registration registration = (Registration) info;
                registration.timeout(timerId);
            } else if (info.getClass() == SubManager.class) {
                SubManager subManager = (SubManager) info;
                subManager.timeout(timerId);
            } else if (info.getClass() == TimerData.class) {
                TimerData timerData = (TimerData) info;
                XoObject resource = null;
                Document document = null;
                try {
                    document = storageContext.retrieve(null, timerData.getPath(), null);
                    resource = xoService.readBinaryXmlXoObject(document.getContent());
                    ResourceController ctrl = getResourceController(timerData.getCtrlId());
                    ctrl.timeout(this, timerData.getPath(), timerId, null, timerData.getInfo());
                } catch (ParseException e) {
                    LOG.error(timerId + ": Exception while treating timeout event", e);
                } catch (IOException e) {
                    LOG.error(timerId + ": Exception while treating timeout event", e);
                } catch (StorageException e) {
                    if (LOG.isInfoEnabled()) {
                        LOG.info(timerId + ": received a timeout on a deleted resource");
                    }
                } catch (XoException e) {
                    LOG.error(timerId + ": Exception while treating timeout event", e);
                } catch (M2MException e) {
                    LOG.error(timerId + ": Exception while treating timeout event", e);
                } finally {
                    if (resource != null) {
                        resource.free(true);
                    }
                }
            }
        } catch (RuntimeException e) {
            LOG.error("Internal server error", e);
        }
    }

    public synchronized void sessionExpired(M2MSession session) {
        if (LOG.isInfoEnabled()) {
            LOG.info(session.getId() + ": Received sessionExpired");
        }

    }

    public synchronized void doProxyCreateIndication(ProxyIndication indication) throws IOException, M2MException {
        if (LOG.isInfoEnabled()) {
            LOG.info(indication.getTransactionId() + ": >>> Received doProxyCreateIndication to targetID \""
                    + indication.getTargetID() + "\"");
        }
        if (Profiler.getInstance().isTraceEnabled()) {
            Profiler.getInstance().trace(
                    indication.getTransactionId() + ": >>> Received doProxyCreateIndication to targetID \""
                            + indication.getTargetID() + "\"");
        }
        if (state == ST_READY) {
            checkAuthentication(indication);

            retargetProxyIndication(indication);
        } else {
            sendUnsuccessResponse(indication, StatusCode.STATUS_SERVICE_UNAVAILABLE, "Service is unavailable for now");
        }
    }

    public synchronized void doProxyRetrieveIndication(ProxyIndication indication) throws IOException, M2MException {
        if (LOG.isInfoEnabled()) {
            LOG.info(indication.getTransactionId() + ": >>> Received doProxyRetrieveIndication to targetID \""
                    + indication.getTargetID() + "\"");
        }
        if (Profiler.getInstance().isTraceEnabled()) {
            Profiler.getInstance().trace(
                    indication.getTransactionId() + ": >>> Received doProxyRetrieveIndication to targetID \""
                            + indication.getTargetID() + "\"");
        }
        if (state == ST_READY) {
            checkAuthentication(indication);

            retargetProxyIndication(indication);
        } else {
            sendUnsuccessResponse(indication, StatusCode.STATUS_SERVICE_UNAVAILABLE, "Service is unavailable for now");
        }
    }

    public synchronized void doProxyUpdateIndication(ProxyIndication indication) throws IOException, M2MException {
        if (LOG.isInfoEnabled()) {
            LOG.info(indication.getTransactionId() + ": >>> Received doProxyUpdateIndication to targetID \""
                    + indication.getTargetID() + "\"");
        }
        if (Profiler.getInstance().isTraceEnabled()) {
            Profiler.getInstance().trace(
                    indication.getTransactionId() + ": >>> Received doProxyUpdateIndication to targetID \""
                            + indication.getTargetID() + "\"");
        }
        if (state == ST_READY) {
            checkAuthentication(indication);

            retargetProxyIndication(indication);
        } else {
            sendUnsuccessResponse(indication, StatusCode.STATUS_SERVICE_UNAVAILABLE, "Service is unavailable for now");
        }
    }

    public synchronized void doProxyDeleteIndication(ProxyIndication indication) throws IOException, M2MException {
        if (LOG.isInfoEnabled()) {
            LOG.info(indication.getTransactionId() + ": >>> Received doProxyDeleteIndication to targetID \""
                    + indication.getTargetID() + "\"");
        }
        if (Profiler.getInstance().isTraceEnabled()) {
            Profiler.getInstance().trace(
                    indication.getTransactionId() + ": >>> Received doProxyDeleteIndication to targetID \""
                            + indication.getTargetID() + "\"");
        }
        if (state == ST_READY) {
            checkAuthentication(indication);

            retargetProxyIndication(indication);
        } else {
            sendUnsuccessResponse(indication, StatusCode.STATUS_SERVICE_UNAVAILABLE, "Service is unavailable for now");
        }
    }
}
