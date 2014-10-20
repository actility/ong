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
 * id $Id: Scl.java 3302 2012-10-10 14:19:16Z JReich $
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
import java.util.Collection;
import java.util.Date;
import java.util.Iterator;
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
import com.actility.m2m.storage.StorageException;
import com.actility.m2m.util.FormatUtils;
import com.actility.m2m.util.Pair;
import com.actility.m2m.util.URIUtils;
import com.actility.m2m.util.log.OSGiLogger;
import com.actility.m2m.xo.XoException;
import com.actility.m2m.xo.XoObject;
import com.actility.m2m.xo.XoService;

/**
 * M2M Remote Service Capability Layer. Represents a remote Service Capability Layer on the current Service Capability Layer.
 */
public final class Scl extends SclResource implements VolatileResource, SubscribedResource {
    private static final Logger LOG = OSGiLogger.getLogger(NotificationChannel.class, BundleLogger.getStaticLogger());

    private static final Scl INSTANCE = new Scl();

    public static Scl getInstance() {
        return INSTANCE;
    }

    private Scl() {
        // Cannot be instantiated
        super(false, "sclRetrieveRequestIndication", "sclRetrieveResponseConfirm", "sclUpdateRequestIndication",
                "sclUpdateResponseConfirm", "sclDeleteRequestIndication", "sclDeleteResponseConfirm",
                Constants.ID_NO_FILTER_CRITERIA_MODE, true, true, Scls.getInstance(), true, false);
    }

    // m2m:Scl from ong:t_xml_obj
    // {
    // sclId XoString { embattr } // (optional) (xmlType: xsd:anyURI)
    // m2m:pocs m2m:AnyURIList { } // (optional)
    // m2m:remTriggerAddr XoString { } // (optional) (xmlType: xsd:anyURI)
    // m2m:onlineStatus XoString { } // (optional) (xmlType: m2m:OnlineStatus) (enum: ONLINE OFFLINE NOT_REACHABLE )
    // m2m:serverCapability XoString { } // (optional) (xmlType: xsd:boolean)
    // m2m:link XoString { } // (optional) (xmlType: xsd:anyURI)
    // m2m:schedule m2m:Schedule { } // (optional)
    // m2m:expirationTime XoString { } // (optional) (xmlType: xsd:dateTime)
    // m2m:accessRightID XoString { } // (optional) (xmlType: xsd:anyURI)
    // m2m:searchStrings m2m:SearchStrings { } // (optional)
    // m2m:creationTime XoString { } // (optional) (xmlType: xsd:dateTime)
    // m2m:lastModifiedTime XoString { } // (optional) (xmlType: xsd:dateTime)
    // m2m:locTargetDevice XoString { } // (optional) (xmlType: xsd:anyType)
    // m2m:mgmtProtocolType XoString { } // (optional) (xmlType: m2m:MgmtProtocolType) (enum: OMA DM BBF TR 069 )
    // m2m:integrityValResults m2m:IntegrityValResults { } // (optional)
    // m2m:aPocHandling XoString { } // (optional) (xmlType: m2m:APocHandling) (enum: SHALLOW DEEP )
    // m2m:containersReference XoString { } // (optional) (xmlType: xsd:anyURI)
    // m2m:groupsReference XoString { } // (optional) (xmlType: xsd:anyURI)
    // m2m:applicationsReference XoString { } // (optional) (xmlType: xsd:anyURI)
    // m2m:accessRightsReference XoString { } // (optional) (xmlType: xsd:anyURI)
    // m2m:subscriptionsReference XoString { } // (optional) (xmlType: xsd:anyURI)
    // m2m:mgmtObjsReference XoString { } // (optional) (xmlType: xsd:anyURI)
    // m2m:notificationChannelsReference XoString { } // (optional) (xmlType: xsd:anyURI)
    // m2m:m2mPocsReference XoString { } // (optional) (xmlType: xsd:anyURI)
    // m2m:attachedDevicesReference XoString { } // (optional) (xmlType: xsd:anyURI)
    // }
    // alias m2m:Scl with m2m:scl

    public void reload(SclManager manager, String path, XoObject resource, SclTransaction transaction) throws IOException,
            M2MException, StorageException, XoException {
        if (LOG.isInfoEnabled()) {
            LOG.info("Reload volatile <scl>: " + path);
        }
        long now = System.currentTimeMillis();
        Date expirationTime = getAndCheckDateTime(resource, M2MConstants.TAG_M2M_EXPIRATION_TIME, Constants.ID_MODE_OPTIONAL,
                -1L);
        if (expirationTime != null && expirationTime.getTime() > 0) {
            if (expirationTime.getTime() > now) {
                if (expirationTime.getTime() < FormatUtils.MAX_DATE) {
                    transaction.addTransientOp(new ExpirationTimerUpdateOp(manager, path, Constants.ID_RES_SCL, expirationTime
                            .getTime() - now));
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

    public void createDefaultResource(SclManager manager, String path, String sclId, Date creationDate, String creationTime,
            URI link, SclTransaction transaction) throws XoException {
        XoService xoService = manager.getXoService();

        // sclId: M (response M)
        // pocs: O (response M)
        // remTriggerAddr: O (response O)
        // onlineStatus: NP (response M)
        // serverCapability: NP (response M)
        // link: M (response M)
        // schedule: O (response O)
        // expirationTime: O (response M)
        // accessRightID: O (response O)
        // searchStrings: O (response M)
        // creationTime: NP (response M)
        // lastModifiedTime: NP (response M)
        // locTargetDevice: O (response O)
        // mgmtProtocolType: M (response M)
        // integrityValResults: O (response O)
        // aPocHandling: NP (response O)
        // containersReference: NP (response M)
        // groupsReference: NP (response M)
        // applicationsReference: NP (response M)
        // accessRightsReference: NP (response M)
        // subscriptionsReference: NP (response M)
        // mgmtObjsReference: NP (response M)
        // notificationChannelsReference: NP (response M)
        // m2mPocsReference: NP (response M)
        // attachedDevicesReference: NP (response M)

        XoObject resource = null;
        String linkStr = link.toString();
        try {
            resource = xoService.newXmlXoObject(M2MConstants.TAG_M2M_SCL);

            // Update resource
            resource.setNameSpace(M2MConstants.PREFIX_M2M);
            resource.setStringAttribute(M2MConstants.ATTR_SCL_ID, sclId);
            resource.setXoObjectAttribute(M2MConstants.TAG_M2M_POCS, xoService.newXmlXoObject(M2MConstants.TAG_M2M_POCS));
            resource.setStringAttribute(M2MConstants.TAG_M2M_ONLINE_STATUS, M2MConstants.ONLINE_STATUS_ONLINE);
            resource.setStringAttribute(M2MConstants.TAG_M2M_SERVER_CAPABILITY, M2MConstants.BOOLEAN_TRUE);
            resource.setStringAttribute(M2MConstants.TAG_M2M_LINK, linkStr);
            resource.setXoObjectAttribute(M2MConstants.TAG_M2M_SEARCH_STRINGS,
                    xoService.newXmlXoObject(M2MConstants.TAG_M2M_SEARCH_STRINGS));
            resource.setStringAttribute(M2MConstants.TAG_M2M_CREATION_TIME, creationTime);
            resource.setStringAttribute(M2MConstants.TAG_M2M_LAST_MODIFIED_TIME, creationTime);
            resource.setStringAttribute(M2MConstants.TAG_M2M_MGMT_PROTOCOL_TYPE, M2MConstants.MGMT_PROTOCOL_TYPE_TR_069);

            // Create sub-resources
            Containers.getInstance().createResource(manager, path + M2MConstants.URI_SEP + M2MConstants.RES_CONTAINERS,
                    creationDate, creationTime, null, transaction);
            Applications.getInstance().createResource(manager, path + M2MConstants.URI_SEP + M2MConstants.RES_APPLICATIONS,
                    creationDate, creationTime, null, transaction);
            AccessRights.getInstance().createResource(manager, path + M2MConstants.URI_SEP + M2MConstants.RES_ACCESS_RIGHTS,
                    creationDate, creationTime, null, transaction);
            Subscriptions.getInstance().createResource(manager, path + M2MConstants.URI_SEP + M2MConstants.RES_SUBSCRIPTIONS,
                    transaction);
            NotificationChannels.getInstance().createResource(manager,
                    path + M2MConstants.URI_SEP + M2MConstants.RES_NOTIFICATION_CHANNELS, creationDate, creationTime,
                    transaction);
            M2MPocs.getInstance().createResource(manager, path + M2MConstants.URI_SEP + M2MConstants.RES_M2M_POCS,
                    creationDate, creationTime, transaction);

            // Save resource
            Collection searchAttributes = new ArrayList();
            searchAttributes.add(new Pair(M2MConstants.ATTR_LINK, linkStr));
            searchAttributes.add(new Pair(Constants.ATTR_TYPE, Constants.TYPE_SCL));
            XoObject searchStrings = resource.getXoObjectAttribute(M2MConstants.TAG_M2M_SEARCH_STRINGS);
            List searchStringList = searchStrings.getStringListAttribute(M2MConstants.TAG_M2M_SEARCH_STRING);
            Iterator it = searchStringList.iterator();
            while (it.hasNext()) {
                searchAttributes.add(new Pair(M2MConstants.ATTR_SEARCH_STRING, it.next()));
            }
            searchAttributes.add(new Pair(M2MConstants.ATTR_CREATION_TIME, creationDate));
            searchAttributes.add(new Pair(M2MConstants.ATTR_LAST_MODIFIED_TIME, creationDate));
            transaction.createResource(path, resource, searchAttributes);
        } finally {
            if (resource != null) {
                resource.free(true);
            }
        }
    }

    public boolean createResource(SclManager manager, String path, XoObject resource, String targetPath, Date creationDate,
            String creationTime, XoObject representation, SclTransaction transaction) throws XoException, M2MException {
        // sclId: M (response M)
        // pocs: O (response M)
        // remTriggerAddr: O (response O)
        // onlineStatus: NP (response M)
        // serverCapability: NP (response M)
        // link: M (response M)
        // schedule: O (response O)
        // expirationTime: O (response M)
        // accessRightID: O (response O)
        // searchStrings: O (response M)
        // creationTime: NP (response M)
        // lastModifiedTime: NP (response M)
        // locTargetDevice: O (response O)
        // mgmtProtocolType: M (response M)
        // integrityValResults: O (response O)
        // aPocHandling: NP (response O)
        // containersReference: NP (response M)
        // groupsReference: NP (response M)
        // applicationsReference: NP (response M)
        // accessRightsReference: NP (response M)
        // subscriptionsReference: NP (response M)
        // mgmtObjsReference: NP (response M)
        // notificationChannelsReference: NP (response M)
        // m2mPocsReference: NP (response M)
        // attachedDevicesReference: NP (response M)

        // Check representation
        checkRepresentation(representation, M2MConstants.TAG_M2M_SCL);
        getAndCheckAnyURIList(representation, M2MConstants.TAG_M2M_POCS, Constants.ID_MODE_OPTIONAL);
        getAndCheckURI(representation, M2MConstants.TAG_M2M_REM_TRIGGER_ADDR, Constants.ID_MODE_OPTIONAL);
        getAndCheckStringMode(representation, M2MConstants.TAG_M2M_ONLINE_STATUS, Constants.ID_MODE_FORBIDDEN);
        getAndCheckStringMode(representation, M2MConstants.TAG_M2M_SERVER_CAPABILITY, Constants.ID_MODE_FORBIDDEN);
        URI link = getAndCheckURI(representation, M2MConstants.TAG_M2M_LINK, Constants.ID_MODE_REQUIRED);
        Date recvExpirationTime = getAndCheckDateTime(representation, M2MConstants.TAG_M2M_EXPIRATION_TIME,
                Constants.ID_MODE_OPTIONAL, creationDate.getTime());
        String accessRightID = getAndCheckAccessRightID(manager, representation, M2MConstants.TAG_M2M_ACCESS_RIGHT_I_D,
                Constants.ID_MODE_OPTIONAL, targetPath);
        getAndCheckStringMode(representation, M2MConstants.TAG_M2M_CREATION_TIME, Constants.ID_MODE_FORBIDDEN);
        getAndCheckMgmtProtocolType(representation, M2MConstants.TAG_M2M_MGMT_PROTOCOL_TYPE, Constants.ID_MODE_REQUIRED);
        getAndCheckIntegrityValResults(representation, M2MConstants.TAG_M2M_INTEGRITY_VAL_RESULTS, Constants.ID_MODE_OPTIONAL);
        getAndCheckStringMode(representation, M2MConstants.TAG_M2M_A_POC_HANDLING, Constants.ID_MODE_FORBIDDEN);
        getAndCheckStringMode(representation, M2MConstants.TAG_M2M_CONTAINERS_REFERENCE, Constants.ID_MODE_FORBIDDEN);
        getAndCheckStringMode(representation, M2MConstants.TAG_M2M_GROUPS_REFERENCE, Constants.ID_MODE_FORBIDDEN);
        getAndCheckStringMode(representation, M2MConstants.TAG_M2M_APPLICATIONS_REFERENCE, Constants.ID_MODE_FORBIDDEN);
        getAndCheckStringMode(representation, M2MConstants.TAG_M2M_ACCESS_RIGHTS_REFERENCE, Constants.ID_MODE_FORBIDDEN);
        getAndCheckStringMode(representation, M2MConstants.TAG_M2M_SUBSCRIPTIONS_REFERENCE, Constants.ID_MODE_FORBIDDEN);
        getAndCheckStringMode(representation, M2MConstants.TAG_M2M_MGMT_OBJS_REFERENCE, Constants.ID_MODE_FORBIDDEN);
        getAndCheckStringMode(representation, M2MConstants.TAG_M2M_NOTIFICATION_CHANNELS_REFERENCE, Constants.ID_MODE_FORBIDDEN);
        getAndCheckStringMode(representation, M2MConstants.TAG_M2M_M2M_POCS_REFERENCE, Constants.ID_MODE_FORBIDDEN);
        getAndCheckStringMode(representation, M2MConstants.TAG_M2M_ATTACHED_DEVICES_REFERENCE, Constants.ID_MODE_FORBIDDEN);

        // Update resource
        resource.setNameSpace(M2MConstants.PREFIX_M2M);
        createStringMandatory(resource, representation, M2MConstants.ATTR_SCL_ID, null);
        createXoObjectMandatory(manager, resource, representation, M2MConstants.TAG_M2M_POCS);
        createStringOptional(resource, representation, M2MConstants.TAG_M2M_REM_TRIGGER_ADDR);
        resource.setStringAttribute(M2MConstants.TAG_M2M_ONLINE_STATUS, M2MConstants.ONLINE_STATUS_OFFLINE);
        resource.setStringAttribute(M2MConstants.TAG_M2M_SERVER_CAPABILITY, M2MConstants.BOOLEAN_FALSE);
        createStringMandatory(resource, representation, M2MConstants.TAG_M2M_LINK, null);
        createXoObjectOptional(resource, representation, M2MConstants.TAG_M2M_SCHEDULE);
        // TODO M ? why not M* ?
        Date expirationTime = setExpirationTime(manager, resource, creationDate.getTime(), recvExpirationTime);
        createStringOptional(resource, representation, M2MConstants.TAG_M2M_ACCESS_RIGHT_I_D);
        createXoObjectMandatory(manager, resource, representation, M2MConstants.TAG_M2M_SEARCH_STRINGS);
        resource.setStringAttribute(M2MConstants.TAG_M2M_CREATION_TIME, creationTime);
        resource.setStringAttribute(M2MConstants.TAG_M2M_LAST_MODIFIED_TIME, creationTime);
        createStringOptional(resource, representation, M2MConstants.TAG_M2M_LOC_TARGET_DEVICE);
        createStringMandatory(resource, representation, M2MConstants.TAG_M2M_MGMT_PROTOCOL_TYPE, null);
        createXoObjectOptional(resource, representation, M2MConstants.TAG_M2M_INTEGRITY_VAL_RESULTS);
        // TODO aPoCHandling ?

        // Create sub-resources
        Containers.getInstance().createResource(manager, path + M2MConstants.URI_SEP + M2MConstants.RES_CONTAINERS,
                creationDate, creationTime, accessRightID, transaction);
        Applications.getInstance().createResource(manager, path + M2MConstants.URI_SEP + M2MConstants.RES_APPLICATIONS,
                creationDate, creationTime, accessRightID, transaction);
        AccessRights.getInstance().createResource(manager, path + M2MConstants.URI_SEP + M2MConstants.RES_ACCESS_RIGHTS,
                creationDate, creationTime, accessRightID, transaction);
        Subscriptions.getInstance().createResource(manager, path + M2MConstants.URI_SEP + M2MConstants.RES_SUBSCRIPTIONS,
                transaction);
        NotificationChannels.getInstance().createResource(manager,
                path + M2MConstants.URI_SEP + M2MConstants.RES_NOTIFICATION_CHANNELS, creationDate, creationTime, transaction);
        M2MPocs.getInstance().createResource(manager, path + M2MConstants.URI_SEP + M2MConstants.RES_M2M_POCS, creationDate,
                creationTime, transaction);

        // Save resource
        Collection searchAttributes = new ArrayList();
        searchAttributes.add(new Pair(M2MConstants.ATTR_LINK, link.toString()));
        searchAttributes.add(new Pair(Constants.ATTR_TYPE, Constants.TYPE_SCL));
        XoObject searchStrings = resource.getXoObjectAttribute(M2MConstants.TAG_M2M_SEARCH_STRINGS);
        List searchStringList = searchStrings.getStringListAttribute(M2MConstants.TAG_M2M_SEARCH_STRING);
        Iterator it = searchStringList.iterator();
        while (it.hasNext()) {
            searchAttributes.add(new Pair(M2MConstants.ATTR_SEARCH_STRING, it.next()));
        }
        searchAttributes.add(new Pair(M2MConstants.ATTR_CREATION_TIME, creationDate));
        searchAttributes.add(new Pair(M2MConstants.ATTR_LAST_MODIFIED_TIME, creationDate));
        transaction.createResource(path, resource, searchAttributes);

        transaction.addTransientOp(new ExpirationTimerUpdateOp(manager, path, Constants.ID_RES_SCL, expirationTime.getTime()
                - creationDate.getTime()));

        return false;
    }

    public boolean updateResource(String logId, SclManager manager, String path, XoObject resource, Indication indication,
            XoObject representation, Date now) throws ParseException, XoException, StorageException, M2MException {
        // sclId: NP (response M)
        // pocs: O (response M)
        // remTriggerAddr: O (response O)
        // onlineStatus: NP (response M)
        // serverCapability: NP (response M)
        // link: NP (response M)
        // schedule: O (response O)
        // expirationTime: O (response M)
        // accessRightID: O (response O)
        // searchStrings: O (response M)
        // creationTime: NP (response M)
        // lastModifiedTime: NP (response M)
        // locTargetDevice: O (response O)
        // mgmtProtocolType: M (response M)
        // integrityValResults: O (response O)
        // aPocHandling: NP (response O)
        // containersReference: NP (response M)
        // groupsReference: NP (response M)
        // applicationsReference: NP (response M)
        // accessRightsReference: NP (response M)
        // subscriptionsReference: NP (response M)
        // mgmtObjsReference: NP (response M)
        // notificationChannelsReference: NP (response M)
        // m2mPocsReference: NP (response M)
        // attachedDevicesReference: NP (response M)

        // Check representation
        checkRepresentation(representation, M2MConstants.TAG_M2M_SCL);
        getAndCheckStringMode(representation, M2MConstants.ATTR_SCL_ID, Constants.ID_MODE_FORBIDDEN);
        getAndCheckAnyURIList(representation, M2MConstants.TAG_M2M_POCS, Constants.ID_MODE_OPTIONAL);
        getAndCheckURI(representation, M2MConstants.TAG_M2M_REM_TRIGGER_ADDR, Constants.ID_MODE_OPTIONAL);
        getAndCheckStringMode(representation, M2MConstants.TAG_M2M_ONLINE_STATUS, Constants.ID_MODE_FORBIDDEN);
        getAndCheckStringMode(representation, M2MConstants.TAG_M2M_SERVER_CAPABILITY, Constants.ID_MODE_FORBIDDEN);
        getAndCheckStringMode(representation, M2MConstants.TAG_M2M_LINK, Constants.ID_MODE_FORBIDDEN);
        Date recvExpirationTime = getAndCheckDateTime(representation, M2MConstants.TAG_M2M_EXPIRATION_TIME,
                Constants.ID_MODE_OPTIONAL, now.getTime());
        getAndCheckAccessRightID(manager, representation, M2MConstants.TAG_M2M_ACCESS_RIGHT_I_D, Constants.ID_MODE_OPTIONAL,
                indication.getTargetPath());
        getAndCheckStringMode(representation, M2MConstants.TAG_M2M_CREATION_TIME, Constants.ID_MODE_FORBIDDEN);
        getAndCheckMgmtProtocolType(representation, M2MConstants.TAG_M2M_MGMT_PROTOCOL_TYPE, Constants.ID_MODE_REQUIRED);
        getAndCheckIntegrityValResults(representation, M2MConstants.TAG_M2M_INTEGRITY_VAL_RESULTS, Constants.ID_MODE_OPTIONAL);
        getAndCheckStringMode(representation, M2MConstants.TAG_M2M_A_POC_HANDLING, Constants.ID_MODE_FORBIDDEN);
        getAndCheckStringMode(representation, M2MConstants.TAG_M2M_CONTAINERS_REFERENCE, Constants.ID_MODE_FORBIDDEN);
        getAndCheckStringMode(representation, M2MConstants.TAG_M2M_GROUPS_REFERENCE, Constants.ID_MODE_FORBIDDEN);
        getAndCheckStringMode(representation, M2MConstants.TAG_M2M_APPLICATIONS_REFERENCE, Constants.ID_MODE_FORBIDDEN);
        getAndCheckStringMode(representation, M2MConstants.TAG_M2M_ACCESS_RIGHTS_REFERENCE, Constants.ID_MODE_FORBIDDEN);
        getAndCheckStringMode(representation, M2MConstants.TAG_M2M_SUBSCRIPTIONS_REFERENCE, Constants.ID_MODE_FORBIDDEN);
        getAndCheckStringMode(representation, M2MConstants.TAG_M2M_MGMT_OBJS_REFERENCE, Constants.ID_MODE_FORBIDDEN);
        getAndCheckStringMode(representation, M2MConstants.TAG_M2M_NOTIFICATION_CHANNELS_REFERENCE, Constants.ID_MODE_FORBIDDEN);
        getAndCheckStringMode(representation, M2MConstants.TAG_M2M_M2M_POCS_REFERENCE, Constants.ID_MODE_FORBIDDEN);
        getAndCheckStringMode(representation, M2MConstants.TAG_M2M_ATTACHED_DEVICES_REFERENCE, Constants.ID_MODE_FORBIDDEN);

        // Update resource
        updateXoObjectMandatory(manager, resource, representation, M2MConstants.TAG_M2M_POCS);
        updateStringOptional(resource, representation, M2MConstants.TAG_M2M_REM_TRIGGER_ADDR);
        updateXoObjectOptional(resource, representation, M2MConstants.TAG_M2M_SCHEDULE);
        // TODO M* ??
        Date expirationTime = setExpirationTime(manager, resource, now.getTime(), recvExpirationTime);
        updateStringOptional(resource, representation, M2MConstants.TAG_M2M_ACCESS_RIGHT_I_D);
        updateXoObjectMandatory(manager, resource, representation, M2MConstants.TAG_M2M_SEARCH_STRINGS);
        updateLastModifiedTime(manager, resource, now);
        updateStringOptional(resource, representation, M2MConstants.TAG_M2M_LOC_TARGET_DEVICE);
        updateStringMandatory(resource, representation, M2MConstants.TAG_M2M_MGMT_PROTOCOL_TYPE, null);
        updateXoObjectOptional(resource, representation, M2MConstants.TAG_M2M_INTEGRITY_VAL_RESULTS);
        // TODO aPoCHandling ??

        // Save resource
        Collection searchAttributes = new ArrayList();
        searchAttributes.add(new Pair(M2MConstants.ATTR_LINK, resource.getStringAttribute(M2MConstants.TAG_M2M_LINK)));
        searchAttributes.add(new Pair(Constants.ATTR_TYPE, Constants.TYPE_SCL));
        XoObject searchStrings = resource.getXoObjectAttribute(M2MConstants.TAG_M2M_SEARCH_STRINGS);
        List searchStringList = searchStrings.getStringListAttribute(M2MConstants.TAG_M2M_SEARCH_STRING);
        Iterator it = searchStringList.iterator();
        while (it.hasNext()) {
            searchAttributes.add(new Pair(M2MConstants.ATTR_SEARCH_STRING, it.next()));
        }
        searchAttributes.add(new Pair(M2MConstants.ATTR_CREATION_TIME, FormatUtils.parseDateTime(resource
                .getStringAttribute(M2MConstants.TAG_M2M_CREATION_TIME))));
        searchAttributes.add(new Pair(M2MConstants.ATTR_LAST_MODIFIED_TIME, now));
        manager.getStorageContext().update(path, resource.saveBinary(), searchAttributes);

        new ExpirationTimerUpdateOp(manager, path, Constants.ID_RES_SCL, expirationTime.getTime() - now.getTime()).postCommit();

        return false;
    }

    public void updateFromSclBase(SclManager manager, String path, XoObject resource, XoObject representation)
            throws ParseException, StorageException, XoException, M2MException {
        Date now = new Date();

        // searchStrings: M
        // aPocHandling: O

        // Check representation
        checkRepresentation(representation, M2MConstants.TAG_M2M_SCL_BASE);
        getAndCheckXoObjectMode(representation, M2MConstants.TAG_M2M_SEARCH_STRINGS, Constants.ID_MODE_REQUIRED);
        getAndCheckAPoCHandling(representation, M2MConstants.TAG_M2M_A_POC_HANDLING, Constants.ID_MODE_OPTIONAL);

        // Update resource
        updateXoObjectMandatory(manager, resource, representation, M2MConstants.TAG_M2M_SEARCH_STRINGS);
        updateLastModifiedTime(manager, resource, now);
        updateStringOptional(resource, representation, M2MConstants.TAG_M2M_A_POC_HANDLING);

        // Save resource
        Collection searchAttributes = new ArrayList();
        searchAttributes.add(new Pair(M2MConstants.ATTR_LINK, resource.getStringAttribute(M2MConstants.TAG_M2M_LINK)));
        searchAttributes.add(new Pair(Constants.ATTR_TYPE, Constants.TYPE_SCL));
        XoObject searchStrings = resource.getXoObjectAttribute(M2MConstants.TAG_M2M_SEARCH_STRINGS);
        List searchStringList = searchStrings.getStringListAttribute(M2MConstants.TAG_M2M_SEARCH_STRING);
        Iterator it = searchStringList.iterator();
        while (it.hasNext()) {
            searchAttributes.add(new Pair(M2MConstants.ATTR_SEARCH_STRING, it.next()));
        }
        searchAttributes.add(new Pair(M2MConstants.ATTR_CREATION_TIME, FormatUtils.parseDateTime(resource
                .getStringAttribute(M2MConstants.TAG_M2M_CREATION_TIME))));
        searchAttributes.add(new Pair(M2MConstants.ATTR_LAST_MODIFIED_TIME, now));
        manager.getStorageContext().update(path, resource.saveBinary(), searchAttributes);
    }

    public void updateFromM2MPocs(SclManager manager, String path, XoObject resource, String onlineStatus,
            String serverCapable, SclTransaction transaction) throws ParseException, XoException {
        Date now = new Date();

        // Update resource
        resource.setStringAttribute(M2MConstants.TAG_M2M_ONLINE_STATUS, onlineStatus);
        resource.setStringAttribute(M2MConstants.TAG_M2M_SERVER_CAPABILITY, serverCapable);
        updateLastModifiedTime(manager, resource, now);

        // Save resource
        Collection searchAttributes = new ArrayList();
        searchAttributes.add(new Pair(M2MConstants.ATTR_LINK, resource.getStringAttribute(M2MConstants.TAG_M2M_LINK)));
        searchAttributes.add(new Pair(Constants.ATTR_TYPE, Constants.TYPE_SCL));
        XoObject searchStrings = resource.getXoObjectAttribute(M2MConstants.TAG_M2M_SEARCH_STRINGS);
        List searchStringList = searchStrings.getStringListAttribute(M2MConstants.TAG_M2M_SEARCH_STRING);
        Iterator it = searchStringList.iterator();
        while (it.hasNext()) {
            searchAttributes.add(new Pair(M2MConstants.ATTR_SEARCH_STRING, it.next()));
        }
        searchAttributes.add(new Pair(M2MConstants.ATTR_CREATION_TIME, FormatUtils.parseDateTime(resource
                .getStringAttribute(M2MConstants.TAG_M2M_CREATION_TIME))));
        searchAttributes.add(new Pair(M2MConstants.ATTR_LAST_MODIFIED_TIME, now));
        transaction.updateResource(path, resource, searchAttributes);
    }

    public void deleteResource(String logId, SclManager manager, String path, SclTransaction transaction)
            throws UnsupportedEncodingException, StorageException, XoException, M2MException {
        if (LOG.isInfoEnabled()) {
            LOG.info("Delete <scl> resource on path: " + path);
        }
        Containers.getInstance().deleteResource(logId, manager, path + M2MConstants.URI_SEP + M2MConstants.RES_CONTAINERS,
                transaction);
        Applications.getInstance().deleteResource(logId, manager, path + M2MConstants.URI_SEP + M2MConstants.RES_APPLICATIONS,
                transaction);
        AccessRights.getInstance().deleteResource(logId, manager, path + M2MConstants.URI_SEP + M2MConstants.RES_ACCESS_RIGHTS,
                transaction);
        Subscriptions.getInstance().deleteResource(logId, manager,
                path + M2MConstants.URI_SEP + M2MConstants.RES_SUBSCRIPTIONS, transaction);
        NotificationChannels.getInstance().deleteResource(logId, manager,
                path + M2MConstants.URI_SEP + M2MConstants.RES_NOTIFICATION_CHANNELS, transaction);
        M2MPocs.getInstance().deleteResource(logId, manager, path + M2MConstants.URI_SEP + M2MConstants.RES_M2M_POCS,
                transaction);

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

    public void prepareResourceForResponse(String logId, SclManager manager, String path, XoObject resource,
            URI requestingEntity, FilterCriteria filterCriteria, Set supported) {
        String appPath = manager.getM2MContext().getApplicationPath();
        String accessRight = resource.getStringAttribute(M2MConstants.TAG_M2M_ACCESS_RIGHT_I_D);
        if (accessRight != null) {
            resource.setStringAttribute(M2MConstants.TAG_M2M_ACCESS_RIGHT_I_D, appPath + URIUtils.encodePath(accessRight)
                    + M2MConstants.URI_SEP);
        }
        String encodedPath = URIUtils.encodePath(path);
        resource.setStringAttribute(M2MConstants.TAG_M2M_CONTAINERS_REFERENCE, appPath + encodedPath + M2MConstants.URI_SEP
                + M2MConstants.RES_CONTAINERS + M2MConstants.URI_SEP);
        resource.setStringAttribute(M2MConstants.TAG_M2M_GROUPS_REFERENCE, appPath + encodedPath + M2MConstants.URI_SEP
                + M2MConstants.RES_GROUPS + M2MConstants.URI_SEP);
        resource.setStringAttribute(M2MConstants.TAG_M2M_APPLICATIONS_REFERENCE, appPath + encodedPath + M2MConstants.URI_SEP
                + M2MConstants.RES_APPLICATIONS + M2MConstants.URI_SEP);
        resource.setStringAttribute(M2MConstants.TAG_M2M_ACCESS_RIGHTS_REFERENCE, appPath + encodedPath + M2MConstants.URI_SEP
                + M2MConstants.RES_ACCESS_RIGHTS + M2MConstants.URI_SEP);
        resource.setStringAttribute(M2MConstants.TAG_M2M_SUBSCRIPTIONS_REFERENCE, appPath + encodedPath + M2MConstants.URI_SEP
                + M2MConstants.RES_SUBSCRIPTIONS + M2MConstants.URI_SEP);
        resource.setStringAttribute(M2MConstants.TAG_M2M_MGMT_OBJS_REFERENCE, appPath + encodedPath + M2MConstants.URI_SEP
                + M2MConstants.RES_MGMT_OBJS + M2MConstants.URI_SEP);
        resource.setStringAttribute(M2MConstants.TAG_M2M_NOTIFICATION_CHANNELS_REFERENCE, appPath + encodedPath
                + M2MConstants.URI_SEP + M2MConstants.RES_NOTIFICATION_CHANNELS + M2MConstants.URI_SEP);
        resource.setStringAttribute(M2MConstants.TAG_M2M_M2M_POCS_REFERENCE, appPath + encodedPath + M2MConstants.URI_SEP
                + M2MConstants.RES_M2M_POCS + M2MConstants.URI_SEP);
        resource.setStringAttribute(M2MConstants.TAG_M2M_ATTACHED_DEVICES_REFERENCE, appPath + encodedPath
                + M2MConstants.URI_SEP + M2MConstants.RES_ATTACHED_DEVICES + M2MConstants.URI_SEP);
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
