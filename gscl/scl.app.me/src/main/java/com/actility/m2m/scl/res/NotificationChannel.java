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
 * id $Id: NotificationChannel.java 3302 2012-10-10 14:19:16Z JReich $
 * author $Author: JReich $
 * version $Revision: 3302 $
 * lastrevision $Date: 2012-10-10 16:19:16 +0200 (Wed, 10 Oct 2012) $
 * modifiedby $LastChangedBy: JReich $
 * lastmodified $LastChangedDate: 2012-10-10 16:19:16 +0200 (Wed, 10 Oct 2012) $
 */

package com.actility.m2m.scl.res;

import java.io.UnsupportedEncodingException;
import java.net.URI;
import java.util.ArrayList;
import java.util.Collection;
import java.util.Date;

import org.apache.log4j.Logger;

import com.actility.m2m.m2m.Indication;
import com.actility.m2m.m2m.M2MConstants;
import com.actility.m2m.m2m.M2MException;
import com.actility.m2m.scl.log.BundleLogger;
import com.actility.m2m.scl.mem.NCCreateOp;
import com.actility.m2m.scl.mem.NCDeleteOp;
import com.actility.m2m.scl.model.Constants;
import com.actility.m2m.scl.model.SclManager;
import com.actility.m2m.scl.model.SclTransaction;
import com.actility.m2m.scl.model.VolatileResource;
import com.actility.m2m.storage.StorageException;
import com.actility.m2m.util.Pair;
import com.actility.m2m.util.log.OSGiLogger;
import com.actility.m2m.xo.XoException;
import com.actility.m2m.xo.XoObject;

/**
 * M2M Channel. Defines a specific channels used in the communication between to points. This is used for example to represent a
 * long poll connection between two Service Capability Layers.
 */
public final class NotificationChannel extends SclResource implements VolatileResource {
    private static final Logger LOG = OSGiLogger.getLogger(NotificationChannel.class, BundleLogger.getStaticLogger());

    private static final NotificationChannel INSTANCE = new NotificationChannel();

    public static NotificationChannel getInstance() {
        return INSTANCE;
    }

    private NotificationChannel() {
        // Cannot be instantiated
        super(false, "notificationChannelRetrieveRequestIndication", "notificationChannelRetrieveResponseConfirm", null, null,
                "notificationChannelDeleteRequestIndication", "notificationChannelDeleteResponseConfirm",
                Constants.ID_NO_FILTER_CRITERIA_MODE, true, false, NotificationChannels.getInstance(), false, false);
    }

    // m2m:NotificationChannel from ong:t_xml_obj
    // {
    // m2m:id XoString { embattr } // (optional) (xmlType: xsd:anyURI)
    // m2m:channelType XoString { } // (optional) (xmlType: m2m:ChannelType) (enum: LONG_POLLING )
    // m2m:contactURI XoString { } // (optional) (xmlType: xsd:anyURI)
    // m2m:channelData m2m:ChannelData { } // (optional)
    // m2m:creationTime XoString { } // (optional) (xmlType: xsd:dateTime)
    // m2m:lastModifiedTime XoString { } // (optional) (xmlType: xsd:dateTime)
    // }
    // alias m2m:NotificationChannel with m2m:notificationChannel

    public void reload(SclManager manager, String path, XoObject resource, SclTransaction transaction) throws M2MException {
        if (LOG.isInfoEnabled()) {
            LOG.info("Reload volatile <notificationChannel>: " + path);
        }
        if (M2MConstants.CHANNEL_TYPE_LONG_POLLING.equals(resource.getStringAttribute(M2MConstants.TAG_M2M_CHANNEL_TYPE))) {
            URI contactUri = URI.create(resource.getStringAttribute(M2MConstants.TAG_M2M_CONTACT_U_R_I));
            XoObject channelData = resource.getXoObjectAttribute(M2MConstants.TAG_M2M_CHANNEL_DATA);
            URI longPollingUri = URI.create(channelData.getStringAttribute(M2MConstants.TAG_M2M_LONG_POLLING_U_R_I));
            manager.getM2MContext().createServerLongPoll(contactUri, longPollingUri);
        }
    }

    public void checkRights(String logId, SclManager manager, String path, XoObject resource, URI requestingEntity, String flag)
            throws M2MException {
        checkDefaultAccessRight(logId, manager, path, requestingEntity);
    }

    public boolean createResource(String logId, SclManager manager, String path, XoObject resource, String id,
            Date creationDate, String creationTime, URI targetID, XoObject representation, SclTransaction transaction)
            throws M2MException {
        // id: O (response M*)
        // channelType: M (response M)
        // contactURI: NP (response M)
        // channelData: NP (response M)
        // creationTime: NP (response M)
        // lastModifiedTime: NP (response M)

        // Check representation
        if (representation != null) {
            checkRepresentation(representation, M2MConstants.TAG_M2M_NOTIFICATION_CHANNEL);
            getAndCheckChannelType(representation, M2MConstants.TAG_M2M_CHANNEL_TYPE, Constants.ID_MODE_REQUIRED);
            getAndCheckStringMode(representation, M2MConstants.TAG_M2M_CONTACT_U_R_I, Constants.ID_MODE_FORBIDDEN);
            getAndCheckXoObjectMode(representation, M2MConstants.TAG_M2M_CHANNEL_DATA, Constants.ID_MODE_FORBIDDEN);
            getAndCheckStringMode(representation, M2MConstants.TAG_M2M_CREATION_TIME, Constants.ID_MODE_FORBIDDEN);
        }

        // Update resource
        resource.setNameSpace(M2MConstants.PREFIX_M2M);
        resource.setNameSpace(M2MConstants.PREFIX_XSI);
        resource.setStringAttribute(M2MConstants.ATTR_M2M_ID, id);
        if (representation != null) {
            createStringMandatory(resource, representation, M2MConstants.TAG_M2M_CHANNEL_TYPE, null);
        } else {
            resource.setStringAttribute(M2MConstants.TAG_M2M_CHANNEL_TYPE, M2MConstants.CHANNEL_TYPE_LONG_POLLING);
        }
        resource.setStringAttribute(M2MConstants.TAG_M2M_CREATION_TIME, creationTime);
        resource.setStringAttribute(M2MConstants.TAG_M2M_LAST_MODIFIED_TIME, creationTime);

        // Warning: Do not save resource, this will be done by NCCreateOp
        Collection searchAttributes = new ArrayList();
        searchAttributes.add(new Pair(Constants.ATTR_TYPE, Constants.TYPE_NOTIFICATION_CHANNEL));
        searchAttributes.add(new Pair(M2MConstants.ATTR_CREATION_TIME, creationDate));
        searchAttributes.add(new Pair(M2MConstants.ATTR_LAST_MODIFIED_TIME, creationDate));
        transaction.addTransientOp(new NCCreateOp(manager, path, resource, targetID, searchAttributes, transaction));

        return true;
    }

    public boolean updateResource(String logId, SclManager manager, String path, XoObject resource, Indication indication,
            XoObject representation, Date now) {
        throw new UnsupportedOperationException();
    }

    public void deleteResource(String logId, SclManager manager, String path, SclTransaction transaction)
            throws UnsupportedEncodingException, XoException, StorageException, M2MException {
        if (LOG.isInfoEnabled()) {
            LOG.info("Delete <notificationChannel> resource on path: " + path);
        }
        XoObject resource = manager.getXoResource(path);
        try {
            deleteResource(logId, manager, path, resource, transaction);
        } finally {
            resource.free(true);
        }
    }

    public void deleteResource(String logId, SclManager manager, String path, XoObject resource, SclTransaction transaction)
            throws M2MException {
        transaction.deleteResource(path);

        // Restore long poll connection data
        URI contactUri = getAndCheckURI(resource, M2MConstants.TAG_M2M_CONTACT_U_R_I, Constants.ID_MODE_REQUIRED);
        URI longPollUri = getAndCheckLongPollingChannelData(resource, M2MConstants.TAG_M2M_CHANNEL_DATA,
                Constants.ID_MODE_REQUIRED);
        transaction.addTransientOp(new NCDeleteOp(logId, manager, contactUri, longPollUri));
    }

    public void deleteChildResource(String logId, SclManager manager, String path, XoObject resource, XoObject childResource,
            Date now, SclTransaction transaction) {
        throw new UnsupportedOperationException();
    }
}
