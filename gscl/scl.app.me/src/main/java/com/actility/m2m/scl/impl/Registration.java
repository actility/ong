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
 * id $Id: Registration.java 3806 2012-12-21 09:46:11Z JReich $
 * author $Author: JReich $
 * version $Revision: 3806 $
 * lastrevision $Date: 2012-12-21 10:46:11 +0100 (Fri, 21 Dec 2012) $
 * modifiedby $LastChangedBy: JReich $
 * lastmodified $LastChangedDate: 2012-12-21 10:46:11 +0100 (Fri, 21 Dec 2012) $
 */

package com.actility.m2m.scl.impl;

import java.io.IOException;
import java.io.Serializable;
import java.io.UnsupportedEncodingException;
import java.net.URI;
import java.text.ParseException;
import java.util.Date;
import java.util.List;

import org.apache.log4j.Logger;

import com.actility.m2m.m2m.Confirm;
import com.actility.m2m.m2m.M2MConstants;
import com.actility.m2m.m2m.M2MException;
import com.actility.m2m.m2m.Request;
import com.actility.m2m.m2m.StatusCode;
import com.actility.m2m.scl.log.BundleLogger;
import com.actility.m2m.scl.model.Constants;
import com.actility.m2m.scl.model.SclManager;
import com.actility.m2m.scl.model.SclTransaction;
import com.actility.m2m.scl.res.Scl;
import com.actility.m2m.scl.res.SclBase;
import com.actility.m2m.scl.res.SclResource;
import com.actility.m2m.scl.res.Scls;
import com.actility.m2m.storage.StorageException;
import com.actility.m2m.util.URIUtils;
import com.actility.m2m.util.log.OSGiLogger;
import com.actility.m2m.xo.XoException;
import com.actility.m2m.xo.XoObject;
import com.actility.m2m.xo.XoService;

public final class Registration implements Serializable {

    /**
     *
     */
    private static final long serialVersionUID = 8854761248251012063L;

    private static final Logger LOG = OSGiLogger.getLogger(Registration.class, BundleLogger.getStaticLogger());

    private static final int ST_INIT_RETRIEVE_SCL_BASE = 0;
    private static final int ST_UPDATE_ACCESS_RIGHT = 1;
    private static final int ST_CREATE_ACCESS_RIGHT = 2;
    private static final int ST_UPDATE_SCL = 3;
    private static final int ST_CREATE_SCL = 4;
    private static final int ST_RETRIEVE_SCL_BASE = 5;
    private static final int ST_RETRIEVE_SCL_BASE_AND_RETRIEVE_SCL = 6;
    private static final int ST_RETRIEVE_SCL = 7;
    private static final int ST_UPDATE_M2M_POC = 8;
    private static final int ST_CREATE_M2M_POC = 9;
    private static final int ST_RETRIEVE_NC = 10;
    private static final int ST_CREATE_NC = 11;
    private static final int ST_REFRESH_SCL = 12;
    private static final int ST_REFRESH_UPDATE_SCL = 13;
    private static final int ST_RETRY_INIT = 14;
    private static final int ST_FINISHED = 15;

    private static final String[] ST_NAMES = { "ST_INIT_RETRIEVE_SCL_BASE", "ST_UPDATE_ACCESS_RIGHT", "ST_CREATE_ACCESS_RIGHT",
            "ST_UPDATE_SCL", "ST_CREATE_SCL", "ST_RETRIEVE_SCL_BASE", "ST_RETRIEVE_SCL_BASE_AND_RETRIEVE_SCL",
            "ST_RETRIEVE_SCL", "ST_UPDATE_M2M_POC", "ST_CREATE_M2M_POC", "ST_RETRIEVE_NC", "ST_CREATE_NC", "ST_REFRESH_SCL",
            "ST_REFRESH_UPDATE_SCL", "ST_RETRY_INIT", "ST_FINISHED" };

    private String timerId;
    private boolean localNsclCreated;
    private URI poc;
    private URI data;
    private String accessRightID;
    private boolean serverCapable;
    private long nextExpiration;
    private int state;

    private final String name;
    private final SclManager manager;
    private final URI nsclUri;
    private final URI tpkDevUri;
    private final URI sclsUri;
    private final URI sclUri;
    private final URI accessRightsUri;
    private final URI accessRightUri;
    private final URI m2mPocsUri;
    private final URI m2mPocUri;
    private final URI notificationChannelsUri;
    private final URI notificationChannelUri;
    private final String systemHolderRef;
    private final String supportHolderRef;
    private final String gsclHolderRef;
    private final String tpkDevHolderRef;

    public Registration(String name, SclManager manager, URI nsclUri, URI tpkDevUri, String systemReqEntity,
            String supportReqEntity) throws UnsupportedEncodingException {
        this.name = name.replace('/', '_');
        this.manager = manager;
        this.nsclUri = nsclUri;
        this.tpkDevUri = tpkDevUri;
        this.sclsUri = URI.create(nsclUri.toString() + M2MConstants.URI_SEP + M2MConstants.RES_SCLS);
        this.sclUri = URI.create(sclsUri.toString() + M2MConstants.URI_SEP + URIUtils.encodePath(name));
        this.accessRightsUri = URI.create(nsclUri.toString() + M2MConstants.URI_SEP + M2MConstants.RES_ACCESS_RIGHTS);
        this.accessRightUri = URI.create(accessRightsUri.toString() + M2MConstants.URI_SEP + URIUtils.encodePath(name));
        this.m2mPocsUri = URI.create(sclUri.toString() + M2MConstants.URI_SEP + M2MConstants.RES_M2M_POCS);
        this.m2mPocUri = URI.create(m2mPocsUri.toString() + M2MConstants.URI_SEP + "m2mPoc1");
        this.notificationChannelsUri = URI.create(sclUri.toString() + M2MConstants.URI_SEP
                + M2MConstants.RES_NOTIFICATION_CHANNELS);
        this.notificationChannelUri = URI.create(notificationChannelsUri.toString() + M2MConstants.URI_SEP + "nc1");
        this.systemHolderRef = nsclUri.toString() + M2MConstants.URI_SEP + M2MConstants.RES_APPLICATIONS + M2MConstants.URI_SEP
                + "SYSTEM";
        this.supportHolderRef = nsclUri.toString() + M2MConstants.URI_SEP + M2MConstants.RES_APPLICATIONS
                + M2MConstants.URI_SEP + "SUPPORT";
        this.gsclHolderRef = sclUri.toString();
        if (tpkDevUri != null) {
            this.tpkDevHolderRef = tpkDevUri.toString();
        } else {
            this.tpkDevHolderRef = null;
        }
        this.localNsclCreated = false;
    }

    private URI getLocalSclUri() throws M2MException {
        return manager.getM2MContext().createLocalUri(nsclUri, Constants.PATH_ROOT);
    }

    private XoObject getSclBase() throws UnsupportedEncodingException, StorageException, XoException {
        return manager.getXoResource(Constants.PATH_ROOT);
    }

    private void switchToState(int newState) {
        if (LOG.isInfoEnabled()) {
            LOG.info(nsclUri + ": Switch to state " + ST_NAMES[newState]);
        }
        this.state = newState;
    }

    private void sendRequest(String method, URI targetID, XoObject representation) throws IOException, XoException {
        if (LOG.isInfoEnabled()) {
            LOG.info(nsclUri + ": send " + method + " to \"" + targetID + "\"");
        }
        Request request = manager.getM2MContext().createRequest(method, sclUri, targetID);
        request.setAttribute(Constants.AT_REQUESTOR, this);
        if (representation != null) {
            // Force XML in mId
            request.setRepresentation(representation, M2MConstants.MT_APPLICATION_XML);
        }
        request.send();
    }

    private XoObject getUpdateSclRepresentation() throws UnsupportedEncodingException, StorageException, XoException {
        // TODO this could leak
        XoObject scl = null;
        XoObject sclBase = getSclBase();
        try {
            scl = SclBase.getInstance().createSclRepresentation(manager, sclBase);
            scl.setStringAttribute(M2MConstants.TAG_M2M_ACCESS_RIGHT_I_D, accessRightID);
        } finally {
            sclBase.free(true);
        }
        nextExpiration = Long.MAX_VALUE;
        scl.setStringAttribute(M2MConstants.TAG_M2M_MGMT_PROTOCOL_TYPE, M2MConstants.MGMT_PROTOCOL_TYPE_TR_069);
        return scl;
    }

    private XoObject getCreateSclRepresentation() throws UnsupportedEncodingException, StorageException, XoException,
            M2MException {
        // TODO this could leak
        XoObject scl = getUpdateSclRepresentation();

        scl.setStringAttribute(M2MConstants.ATTR_SCL_ID, name);
        scl.setStringAttribute(M2MConstants.TAG_M2M_LINK, getLocalSclUri().toString());
        return scl;
    }

    private XoObject getUpdateAccessRightRepresentation() throws XoException, M2MException {
        // TODO this could leak
        XoService xoService = manager.getXoService();
        XoObject accessRight = xoService.newXmlXoObject(M2MConstants.TAG_M2M_ACCESS_RIGHT);
        accessRight.setNameSpace(M2MConstants.PREFIX_M2M);
        XoObject permissions = xoService.newXmlXoObject(M2MConstants.TAG_M2M_PERMISSIONS);
        accessRight.setXoObjectAttribute(M2MConstants.TAG_M2M_PERMISSIONS, permissions);
        List permissionsList = permissions.getXoObjectListAttribute(M2MConstants.TAG_M2M_PERMISSION);

        XoObject pOwnerPermission = xoService.newXmlXoObject(M2MConstants.TAG_M2M_PERMISSION);
        permissionsList.add(pOwnerPermission);
        XoObject pOwnerPermissionFlags = xoService.newXmlXoObject(M2MConstants.TAG_M2M_PERMISSION_FLAGS);
        pOwnerPermission.setXoObjectAttribute(M2MConstants.TAG_M2M_PERMISSION_FLAGS, pOwnerPermissionFlags);
        List pOwnerPermissionFlagsList = pOwnerPermissionFlags.getStringListAttribute(M2MConstants.TAG_M2M_FLAG);
        pOwnerPermissionFlagsList.add(M2MConstants.FLAG_CREATE);
        pOwnerPermissionFlagsList.add(M2MConstants.FLAG_READ);
        pOwnerPermissionFlagsList.add(M2MConstants.FLAG_WRITE);
        pOwnerPermissionFlagsList.add(M2MConstants.FLAG_DELETE);
        pOwnerPermission.setXoObjectAttribute(M2MConstants.TAG_M2M_PERMISSION_FLAGS, pOwnerPermissionFlags);
        XoObject pOwnerPermissionHolders = xoService.newXmlXoObject(M2MConstants.TAG_M2M_PERMISSION_HOLDERS);
        pOwnerPermission.setXoObjectAttribute(M2MConstants.TAG_M2M_PERMISSION_HOLDERS, pOwnerPermissionHolders);
        XoObject holderRefs = xoService.newXmlXoObject(M2MConstants.TAG_M2M_HOLDER_REFS);
        pOwnerPermissionHolders.setXoObjectAttribute(M2MConstants.TAG_M2M_HOLDER_REFS, holderRefs);
        List holderRef = holderRefs.getStringListAttribute(M2MConstants.TAG_M2M_HOLDER_REF);
        holderRef.add(systemHolderRef);
        holderRef.add(gsclHolderRef);

        XoObject pSupportPermission = xoService.newXmlXoObject(M2MConstants.TAG_M2M_PERMISSION);
        permissionsList.add(pSupportPermission);
        XoObject pSupportPermissionFlags = xoService.newXmlXoObject(M2MConstants.TAG_M2M_PERMISSION_FLAGS);
        pSupportPermission.setXoObjectAttribute(M2MConstants.TAG_M2M_PERMISSION_FLAGS, pSupportPermissionFlags);
        List pSupportPermissionFlagsList = pSupportPermissionFlags.getStringListAttribute(M2MConstants.TAG_M2M_FLAG);
        pSupportPermissionFlagsList.add(M2MConstants.FLAG_READ);
        pSupportPermission.setXoObjectAttribute(M2MConstants.TAG_M2M_PERMISSION_FLAGS, pSupportPermissionFlags);
        XoObject pSupportPermissionHolders = xoService.newXmlXoObject(M2MConstants.TAG_M2M_PERMISSION_HOLDERS);
        pSupportPermission.setXoObjectAttribute(M2MConstants.TAG_M2M_PERMISSION_HOLDERS, pSupportPermissionHolders);
        holderRefs = xoService.newXmlXoObject(M2MConstants.TAG_M2M_HOLDER_REFS);
        pSupportPermissionHolders.setXoObjectAttribute(M2MConstants.TAG_M2M_HOLDER_REFS, holderRefs);
        holderRef = holderRefs.getStringListAttribute(M2MConstants.TAG_M2M_HOLDER_REF);
        holderRef.add(supportHolderRef);
        if (tpkDevHolderRef != null) {
            holderRef.add(tpkDevHolderRef);
        }

        XoObject pGsclPermission = xoService.newXmlXoObject(M2MConstants.TAG_M2M_PERMISSION);
        permissionsList.add(pGsclPermission);
        XoObject pGsclPermissionFlags = xoService.newXmlXoObject(M2MConstants.TAG_M2M_PERMISSION_FLAGS);
        pGsclPermission.setXoObjectAttribute(M2MConstants.TAG_M2M_PERMISSION_FLAGS, pGsclPermissionFlags);
        List pGsclPermissionFlagsList = pGsclPermissionFlags.getStringListAttribute(M2MConstants.TAG_M2M_FLAG);
        pGsclPermissionFlagsList.add(M2MConstants.FLAG_CREATE);
        pGsclPermission.setXoObjectAttribute(M2MConstants.TAG_M2M_PERMISSION_FLAGS, pGsclPermissionFlags);
        XoObject pGsclPermissionHolders = xoService.newXmlXoObject(M2MConstants.TAG_M2M_PERMISSION_HOLDERS);
        pGsclPermission.setXoObjectAttribute(M2MConstants.TAG_M2M_PERMISSION_HOLDERS, pGsclPermissionHolders);
        XoObject domains = xoService.newXmlXoObject(M2MConstants.TAG_M2M_DOMAINS);
        pGsclPermissionHolders.setXoObjectAttribute(M2MConstants.TAG_M2M_DOMAINS, domains);
        List domainsList = domains.getStringListAttribute(M2MConstants.TAG_M2M_DOMAIN);
        domainsList.add(getLocalSclUri().toString());

        XoObject selfPermissions = xoService.newXmlXoObject(M2MConstants.TAG_M2M_SELF_PERMISSIONS);
        accessRight.setXoObjectAttribute(M2MConstants.TAG_M2M_SELF_PERMISSIONS, selfPermissions);
        List selfPermissionsList = selfPermissions.getXoObjectListAttribute(M2MConstants.TAG_M2M_PERMISSION);

        selfPermissionsList.add(pOwnerPermission.copy(true));

        selfPermissionsList.add(pSupportPermission.copy(true));

        return accessRight;
    }

    private XoObject getCreateAccessRightRepresentation() throws XoException, M2MException {
        // TODO this could leak
        XoObject accessRight = getUpdateAccessRightRepresentation();
        accessRight.setStringAttribute(M2MConstants.ATTR_M2M_ID, name);
        return accessRight;
    }

    private XoObject getUpdateM2MPocRepresentation() throws XoException {
        // TODO this could leak
        XoObject m2mPoc = manager.getXoService().newXmlXoObject(M2MConstants.TAG_M2M_M2M_POC);
        m2mPoc.setNameSpace(M2MConstants.PREFIX_M2M);
        XoObject contactInfo = manager.getXoService().newXmlXoObject(M2MConstants.TAG_M2M_CONTACT_INFO);
        m2mPoc.setXoObjectAttribute(M2MConstants.TAG_M2M_CONTACT_INFO, contactInfo);
        contactInfo.setStringAttribute(M2MConstants.TAG_M2M_CONTACT_U_R_I, poc.toString());
        // TODO remove this
        contactInfo.setStringAttribute(M2MConstants.TAG_M2M_OTHER, "x");
        m2mPoc.setStringAttribute(M2MConstants.TAG_M2M_ONLINE_STATUS, M2MConstants.ONLINE_STATUS_ONLINE);
        return m2mPoc;
    }

    private XoObject getCreateM2MPocRepresentation() throws XoException {
        // TODO this could leak
        XoObject m2mPoc = getUpdateM2MPocRepresentation();
        m2mPoc.setStringAttribute(M2MConstants.ATTR_M2M_ID, "m2mPoc1");
        return m2mPoc;
    }

    private XoObject getCreateNotificationChannelRepresentation() throws XoException {
        // TODO this could leak
        XoObject nc = manager.getXoService().newXmlXoObject(M2MConstants.TAG_M2M_NOTIFICATION_CHANNEL);
        nc.setNameSpace(M2MConstants.PREFIX_M2M);
        nc.setStringAttribute(M2MConstants.ATTR_M2M_ID, "nc1");
        nc.setStringAttribute(M2MConstants.TAG_M2M_CHANNEL_TYPE, M2MConstants.CHANNEL_TYPE_LONG_POLLING);
        return nc;
    }

    private void createLongPoll(URI context, XoObject representation) throws M2MException {
        // TODO should check it is a valid <notificationChannel>
        SclResource.checkRepresentation(representation, M2MConstants.TAG_M2M_NOTIFICATION_CHANNEL);
        if (context == null) {
            throw new M2MException("Request does not declare a resource URI", StatusCode.STATUS_BAD_REQUEST);
        }
        URI newPoc = SclResource.getAndCheckURI(representation, M2MConstants.TAG_M2M_CONTACT_U_R_I, Constants.ID_MODE_REQUIRED);
        URI newData = SclResource.getAndCheckLongPollingChannelData(representation, M2MConstants.TAG_M2M_CHANNEL_DATA,
                Constants.ID_MODE_REQUIRED);
        newPoc = context.resolve(newPoc);
        newData = context.resolve(newData);
        if (LOG.isInfoEnabled()) {
            LOG.info(nsclUri + ": Create client long poll connection " + newPoc + " " + newData);
        }
        manager.getM2MContext().createClientLongPoll(newPoc, newData);
        this.poc = newPoc;
        this.data = newData;
    }

    private void deleteLongPoll() {
        if (poc != null && !serverCapable) {
            if (LOG.isInfoEnabled()) {
                LOG.info(nsclUri + ": Delete client long poll connection " + poc + " " + data);
            }
            manager.getM2MContext().deleteClientLongPoll(poc, data);
            poc = null;
            data = null;
        }
    }

    private void createLocalNscl() throws ParseException, UnsupportedEncodingException, StorageException, XoException,
            M2MException {
        if (!localNsclCreated) {
            XoObject scls = null;
            try {
                scls = manager.getXoResource(Constants.PATH_SCLS);
                Scls.getInstance().createDefaultChild(manager, Constants.PATH_SCLS, scls, Constants.NAME_NSCL, nsclUri, true);
                localNsclCreated = true;
            } finally {
                if (scls != null) {
                    scls.free(true);
                }
            }
        }
    }

    private void updateLocalNscl(XoObject representation) throws ParseException, UnsupportedEncodingException,
            StorageException, XoException, M2MException {
        XoObject scl = null;
        try {
            scl = manager.getXoResource(Constants.PATH_NSCL);
            Scl.getInstance().updateFromSclBase(manager, Constants.PATH_NSCL, scl, representation);
        } finally {
            if (scl != null) {
                scl.free(true);
            }
        }
    }

    private void deleteLocalNscl() throws ParseException, UnsupportedEncodingException, StorageException, XoException,
            M2MException {
        if (localNsclCreated) {
            SclTransaction transaction = new SclTransaction(manager.getStorageContext());
            Scl.getInstance().deleteResource(nsclUri.toString(), manager, Constants.PATH_NSCL, transaction);
            transaction.execute();
            localNsclCreated = false;
        }
    }

    private void sendCreateScl() throws IOException, XoException, StorageException, M2MException {
        XoObject representation = null;
        try {
            representation = getCreateSclRepresentation();
            sendRequest(M2MConstants.MD_CREATE, sclsUri, representation);
        } finally {
            if (representation != null) {
                representation.free(true);
            }
        }
    }

    private void sendUpdateScl() throws IOException, XoException, StorageException {
        XoObject representation = null;
        try {
            representation = getUpdateSclRepresentation();
            sendRequest(M2MConstants.MD_UPDATE, sclUri, representation);
        } finally {
            if (representation != null) {
                representation.free(true);
            }
        }
    }

    private void sendRetrieveSclBase() throws IOException, XoException {
        sendRequest(M2MConstants.MD_RETRIEVE, nsclUri, null);
    }

    private void sendRetrieveScl() throws IOException, XoException {
        sendRequest(M2MConstants.MD_RETRIEVE, sclUri, null);
    }

    private void sendCreateAccessRight() throws IOException, XoException, M2MException {
        XoObject representation = null;
        try {
            representation = getCreateAccessRightRepresentation();
            sendRequest(M2MConstants.MD_CREATE, accessRightsUri, representation);
        } finally {
            if (representation != null) {
                representation.free(true);
            }
        }
    }

    private void sendUpdateAccessRight() throws IOException, XoException, M2MException {
        XoObject representation = null;
        try {
            representation = getUpdateAccessRightRepresentation();
            sendRequest(M2MConstants.MD_UPDATE, accessRightUri, representation);
        } finally {
            if (representation != null) {
                representation.free(true);
            }
        }
    }

    private void sendCreateM2MPoc() throws IOException, XoException {
        XoObject representation = null;
        try {
            representation = getCreateM2MPocRepresentation();
            sendRequest(M2MConstants.MD_CREATE, m2mPocsUri, representation);
        } finally {
            if (representation != null) {
                representation.free(true);
            }
        }
    }

    private void sendUpdateM2MPoc() throws IOException, XoException {
        XoObject representation = null;
        try {
            representation = getUpdateM2MPocRepresentation();
            sendRequest(M2MConstants.MD_UPDATE, m2mPocUri, representation);
        } finally {
            if (representation != null) {
                representation.free(true);
            }
        }
    }

    private void sendCreateNc() throws IOException, XoException {
        XoObject representation = null;
        try {
            representation = getCreateNotificationChannelRepresentation();
            sendRequest(M2MConstants.MD_CREATE, notificationChannelsUri, representation);
        } finally {
            if (representation != null) {
                representation.free(true);
            }
        }
    }

    private void sendRetrieveNc() throws IOException, XoException {
        sendRequest(M2MConstants.MD_RETRIEVE, notificationChannelUri, null);
    }

    private void cleanupData() throws ParseException, UnsupportedEncodingException, StorageException, XoException, M2MException {
        accessRightID = null;
        deleteLongPoll();
        deleteLocalNscl();
    }

    private void startRefreshSclTimer() {
        long expiration = nextExpiration - System.currentTimeMillis();
        if (expiration > 0) {
            expiration = (expiration >> 1);
            long maxRefresh = manager.getSclConfig().getRegistrationExpirationDuration();
            if (expiration > maxRefresh) {
                expiration = maxRefresh;
            }
            if (LOG.isInfoEnabled()) {
                LOG.info(nsclUri + ": Start registration expiration timer for " + expiration + "ms");
            }
            timerId = manager.getM2MContext().startTimer(expiration, this);
        } else {
            timeout(timerId);
        }
    }

    private void startRetryInitTimer() {
        deleteLongPoll();
        try {
            deleteLocalNscl();
        } catch (M2MException e) {
            LOG.error(nsclUri + ": M2MException while deleting local nscl resource", e);
        } catch (XoException e) {
            LOG.error(nsclUri + ": XoException while deleting local nscl resource", e);
        } catch (StorageException e) {
            LOG.error(nsclUri + ": StorageException while deleting local nscl resource", e);
        } catch (UnsupportedEncodingException e) {
            LOG.error(nsclUri + ": UnsupportedEncodingException while deleting local nscl resource", e);
        } catch (ParseException e) {
            LOG.error(nsclUri + ": ParseException while deleting local nscl resource", e);
        }
        long retryDuration = manager.getSclConfig().getRegistrationRetryDuration();
        if (LOG.isInfoEnabled()) {
            LOG.info(nsclUri + ": Start retry timer for " + retryDuration + "ms");
        }
        timerId = manager.getM2MContext().startTimer(retryDuration, this);
    }

    private void extractAPoCHandling(XoObject sclBase, XoObject representation) throws ParseException, StorageException,
            XoException, M2MException {
        String aPoCHandling = (representation != null) ? representation.getStringAttribute(M2MConstants.TAG_M2M_A_POC_HANDLING)
                : null;
        if (aPoCHandling == null) {
            aPoCHandling = M2MConstants.APOC_HANDLING_SHALLOW;
        }
        SclBase.getInstance().updateAPoCHandling(manager, Constants.PATH_ROOT, sclBase, aPoCHandling);
    }

    private void extractNextExpiration(XoObject representation) throws M2MException {
        long now = System.currentTimeMillis();
        Date expirationTime = SclResource.getAndCheckDateTime(representation, M2MConstants.TAG_M2M_EXPIRATION_TIME,
                Constants.ID_MODE_REQUIRED, now);
        nextExpiration = expirationTime.getTime();
    }

    public URI getNsclUri() {
        return nsclUri;
    }

    public URI getTpkDevUri() {
        return tpkDevUri;
    }

    public synchronized void register() {
        boolean bindingRequested = false;
        try {
            localNsclCreated = (manager.getStorageContext().retrieve(Constants.PATH_NSCL) != null);
            deleteLocalNscl();
            this.serverCapable = manager.getM2MContext().canBeServer(sclsUri);
            bindingRequested = true;
            if (serverCapable) {
                this.poc = getLocalSclUri();
            }
            switchToState(ST_INIT_RETRIEVE_SCL_BASE);
            sendRetrieveSclBase();
        } catch (M2MException e) {
            if (bindingRequested) {
                LOG.error(nsclUri + ": Cannot build <scl> data", e);
            } else if (LOG.isInfoEnabled()) {
                LOG.info(nsclUri + ": SONG binding is not yet available");
            }
            switchToState(ST_RETRY_INIT);
            startRetryInitTimer();
        } catch (IOException e) {
            LOG.error(nsclUri + ": Cannot send a request to NSCL", e);
            switchToState(ST_RETRY_INIT);
            startRetryInitTimer();
        } catch (XoException e) {
            LOG.error(nsclUri + ": Cannot build representation", e);
            switchToState(ST_RETRY_INIT);
            startRetryInitTimer();
        } catch (StorageException e) {
            LOG.error(nsclUri + ": Cannot check presence of <nscl> resource", e);
            switchToState(ST_RETRY_INIT);
            startRetryInitTimer();
        } catch (ParseException e) {
            LOG.error(nsclUri + ": Cannot delete <nscl> resource from storage", e);
            switchToState(ST_RETRY_INIT);
            startRetryInitTimer();
        }
    }

    public synchronized void doSuccessConfirm(Confirm confirm) {
        if (LOG.isInfoEnabled()) {
            LOG.info(nsclUri + ": doSuccessConfirm " + ST_NAMES[state]);
        }
        XoObject representation = null;
        XoObject sclBase = null;
        try {
            switch (state) {
            case ST_INIT_RETRIEVE_SCL_BASE:
                switchToState(ST_UPDATE_ACCESS_RIGHT);
                sendUpdateAccessRight();
                break;
            case ST_UPDATE_ACCESS_RIGHT:
                accessRightID = accessRightUri.getPath();
                switchToState(ST_UPDATE_SCL);
                sendUpdateScl();
                break;
            case ST_CREATE_ACCESS_RIGHT:
                accessRightID = accessRightUri.getPath();
                representation = confirm.getRepresentation();
                if (representation != null) {
                    String id = representation.getStringAttribute(M2MConstants.ATTR_M2M_ID);
                    if (id != null) {
                        accessRightID = accessRightsUri.getPath() + URIUtils.encodePath(id);
                    }
                }
                switchToState(ST_UPDATE_SCL);
                sendUpdateScl();
                break;
            case ST_UPDATE_SCL:
                representation = confirm.getRepresentation();
                createLocalNscl();
                sclBase = getSclBase();
                extractAPoCHandling(sclBase, representation);
                if (representation == null) {
                    switchToState(ST_RETRIEVE_SCL_BASE_AND_RETRIEVE_SCL);
                } else {
                    extractNextExpiration(representation);
                    switchToState(ST_RETRIEVE_SCL_BASE);
                }
                sendRetrieveSclBase();
                break;
            case ST_CREATE_SCL:
                representation = confirm.getRepresentation();
                createLocalNscl();
                sclBase = getSclBase();
                extractAPoCHandling(sclBase, representation);
                if (representation == null) {
                    switchToState(ST_RETRIEVE_SCL_BASE_AND_RETRIEVE_SCL);
                } else {
                    extractNextExpiration(representation);
                    switchToState(ST_RETRIEVE_SCL_BASE);
                }
                sendRetrieveSclBase();
                break;
            case ST_RETRIEVE_SCL_BASE:
                representation = confirm.getRepresentation();
                updateLocalNscl(representation);
                if (poc != null) {
                    switchToState(ST_UPDATE_M2M_POC);
                    sendUpdateM2MPoc();
                } else {
                    switchToState(ST_RETRIEVE_NC);
                    sendRetrieveNc();
                }
                break;
            case ST_RETRIEVE_SCL_BASE_AND_RETRIEVE_SCL:
                representation = confirm.getRepresentation();
                updateLocalNscl(representation);
                switchToState(ST_RETRIEVE_SCL);
                sendRetrieveScl();
                break;
            case ST_RETRIEVE_SCL:
                representation = confirm.getRepresentation();
                sclBase = getSclBase();
                extractAPoCHandling(sclBase, representation);
                if (poc != null) {
                    switchToState(ST_UPDATE_M2M_POC);
                    sendUpdateM2MPoc();
                } else {
                    switchToState(ST_RETRIEVE_NC);
                    sendRetrieveNc();
                }
                break;
            case ST_UPDATE_M2M_POC:
                switchToState(ST_REFRESH_SCL);
                startRefreshSclTimer();
                break;
            case ST_CREATE_M2M_POC:
                switchToState(ST_REFRESH_SCL);
                startRefreshSclTimer();
                break;
            case ST_RETRIEVE_NC:
                representation = confirm.getRepresentation();
                createLongPoll(confirm.getResourceURI(), representation);
                switchToState(ST_UPDATE_M2M_POC);
                sendUpdateM2MPoc();
                break;
            case ST_CREATE_NC:
                representation = confirm.getRepresentation();
                createLongPoll(confirm.getResourceURI(), representation);
                switchToState(ST_UPDATE_M2M_POC);
                sendUpdateM2MPoc();
                break;
            case ST_REFRESH_SCL:
                LOG.error(nsclUri + ": Received doSuccessConfirm in ST_REFRESH_SCL");
                break;
            case ST_REFRESH_UPDATE_SCL:
                representation = confirm.getRepresentation();
                if (representation != null) {
                    extractNextExpiration(representation);
                }
                switchToState(ST_REFRESH_SCL);
                startRefreshSclTimer();
                break;
            case ST_RETRY_INIT:
                LOG.error(nsclUri + ": Received doSuccessConfirm in ST_RETRY_INIT");
                break;
            case ST_FINISHED:
                break;
            }
        } catch (ParseException e) {
            LOG.error(nsclUri + ": Cannot build representation", e);
            switchToState(ST_RETRY_INIT);
            startRetryInitTimer();
        } catch (IOException e) {
            LOG.error(nsclUri + ": Cannot send a request to NSCL", e);
            switchToState(ST_RETRY_INIT);
            startRetryInitTimer();
        } catch (StorageException e) {
            LOG.error(nsclUri + ": Cannot retrieve sclBase resource", e);
            switchToState(ST_RETRY_INIT);
            startRetryInitTimer();
        } catch (M2MException e) {
            LOG.error(nsclUri + ": Cannot build <scl> data", e);
            switchToState(ST_RETRY_INIT);
            startRetryInitTimer();
        } catch (XoException e) {
            LOG.error(nsclUri + ": Cannot build representation", e);
            switchToState(ST_RETRY_INIT);
            startRetryInitTimer();
        } finally {
            if (representation != null) {
                representation.free(true);
            }
            if (sclBase != null) {
                sclBase.free(true);
            }
        }
    }

    public synchronized void doUnsuccessConfirm(Confirm confirm) {
        if (LOG.isInfoEnabled()) {
            LOG.info(nsclUri + ": doUnsuccessConfirm " + ST_NAMES[state]);
        }
        XoObject sclBase = null;
        try {
            switch (state) {
            case ST_INIT_RETRIEVE_SCL_BASE:
                switchToState(ST_RETRY_INIT);
                startRetryInitTimer();
                break;
            case ST_UPDATE_ACCESS_RIGHT:
                if (confirm.getStatusCode() == StatusCode.STATUS_NOT_FOUND) {
                    switchToState(ST_CREATE_ACCESS_RIGHT);
                    sendCreateAccessRight();
                } else {
                    LOG.error(nsclUri + ": Cannot update remote <accessRight> on " + accessRightUri + ". Received "
                            + confirm.getStatusCode());
                    switchToState(ST_RETRY_INIT);
                    startRetryInitTimer();
                }
                break;
            case ST_CREATE_ACCESS_RIGHT:
                LOG.error(nsclUri + ": Cannot create remote <accessRight> on " + accessRightsUri + ". Received "
                        + confirm.getStatusCode());
                switchToState(ST_RETRY_INIT);
                startRetryInitTimer();
                break;
            case ST_UPDATE_SCL:
                if (confirm.getStatusCode() == StatusCode.STATUS_NOT_FOUND) {
                    switchToState(ST_CREATE_SCL);
                    sendCreateScl();
                } else {
                    LOG.error(nsclUri + ": Cannot update remote <scl> on " + sclUri + ". Received " + confirm.getStatusCode());
                    switchToState(ST_RETRY_INIT);
                    startRetryInitTimer();
                }
                break;
            case ST_CREATE_SCL:
                LOG.error(nsclUri + ": Cannot create remote <scl> on " + sclsUri + ". Received " + confirm.getStatusCode());
                switchToState(ST_RETRY_INIT);
                startRetryInitTimer();
                break;
            case ST_RETRIEVE_SCL_BASE:
                if (poc != null) {
                    switchToState(ST_UPDATE_M2M_POC);
                    sendUpdateM2MPoc();
                } else {
                    switchToState(ST_RETRIEVE_NC);
                    sendRetrieveNc();
                }
                break;
            case ST_RETRIEVE_SCL_BASE_AND_RETRIEVE_SCL:
                switchToState(ST_RETRIEVE_SCL);
                sendRetrieveScl();
                break;
            case ST_RETRIEVE_SCL:
                sclBase = getSclBase();
                extractAPoCHandling(sclBase, null);
                if (poc != null) {
                    switchToState(ST_UPDATE_M2M_POC);
                    sendUpdateM2MPoc();
                } else {
                    switchToState(ST_RETRIEVE_NC);
                    sendRetrieveNc();
                }
                break;
            case ST_UPDATE_M2M_POC:
                if (confirm.getStatusCode() == StatusCode.STATUS_NOT_FOUND) {
                    switchToState(ST_CREATE_M2M_POC);
                    sendCreateM2MPoc();
                } else {
                    LOG.error(nsclUri + ": Cannot update remote <m2mPoc> on " + m2mPocUri + ". Received "
                            + confirm.getStatusCode());
                    switchToState(ST_RETRY_INIT);
                    startRetryInitTimer();
                }
                break;
            case ST_CREATE_M2M_POC:
                LOG.error(nsclUri + ": Cannot create remote <m2mPoc> on " + m2mPocsUri + ". Received "
                        + confirm.getStatusCode());
                switchToState(ST_RETRY_INIT);
                startRetryInitTimer();
                break;
            case ST_RETRIEVE_NC:
                if (confirm.getStatusCode() == StatusCode.STATUS_NOT_FOUND) {
                    switchToState(ST_CREATE_NC);
                    sendCreateNc();
                } else {
                    LOG.error(nsclUri + ": Cannot retrieve remote <notificationChannel> on " + notificationChannelUri
                            + ". Received " + confirm.getStatusCode());
                    switchToState(ST_RETRY_INIT);
                    startRetryInitTimer();
                }
                break;
            case ST_CREATE_NC:
                LOG.error(nsclUri + ": Cannot create remote <notificationChannel> on " + notificationChannelsUri
                        + ". Received " + confirm.getStatusCode());
                switchToState(ST_RETRY_INIT);
                startRetryInitTimer();
                break;
            case ST_REFRESH_SCL:
                LOG.error(nsclUri + ": Received doUnsuccessConfirm in ST_REFRESH_SCL");
                break;
            case ST_REFRESH_UPDATE_SCL:
                LOG.error(nsclUri + ": Cannot refresh remote <scl> on " + sclUri + ". Received " + confirm.getStatusCode());
                switchToState(ST_RETRY_INIT);
                startRetryInitTimer();
                break;
            case ST_RETRY_INIT:
                LOG.error(nsclUri + ": Received doUnsuccessConfirm in ST_RETRY_INIT");
                break;
            case ST_FINISHED:
                break;
            }
        } catch (ParseException e) {
            LOG.error(nsclUri + ": Cannot build representation", e);
            switchToState(ST_RETRY_INIT);
            startRetryInitTimer();
        } catch (IOException e) {
            LOG.error(nsclUri + ": Cannot send a request to NSCL", e);
            switchToState(ST_RETRY_INIT);
            startRetryInitTimer();
        } catch (StorageException e) {
            LOG.error(nsclUri + ": Cannot retrieve sclBase resource", e);
            switchToState(ST_RETRY_INIT);
            startRetryInitTimer();
        } catch (M2MException e) {
            LOG.error(nsclUri + ": Cannot build <scl> data", e);
            switchToState(ST_RETRY_INIT);
            startRetryInitTimer();
        } catch (XoException e) {
            LOG.error(nsclUri + ": Cannot build representation", e);
            switchToState(ST_RETRY_INIT);
            startRetryInitTimer();
        } finally {
            if (sclBase != null) {
                sclBase.free(true);
            }
        }
    }

    public synchronized void timeout(String timerId) {
        if (LOG.isInfoEnabled()) {
            LOG.info(nsclUri + ": timeout " + ST_NAMES[state]);
        }
        try {
            switch (state) {
            case ST_INIT_RETRIEVE_SCL_BASE:
                LOG.error(nsclUri + ": Received timeout in ST_INIT_RETRIEVE_SCL_BASE");
                break;
            case ST_UPDATE_ACCESS_RIGHT:
                LOG.error(nsclUri + ": Received timeout in ST_UPDATE_ACCESS_RIGHT");
                break;
            case ST_CREATE_ACCESS_RIGHT:
                LOG.error(nsclUri + ": Received timeout in ST_CREATE_ACCESS_RIGHT");
                break;
            case ST_UPDATE_SCL:
                LOG.error(nsclUri + ": Received timeout in ST_UPDATE_SCL");
                break;
            case ST_CREATE_SCL:
                LOG.error(nsclUri + ": Received timeout in ST_CREATE_SCL");
                break;
            case ST_RETRIEVE_SCL_BASE:
                LOG.error(nsclUri + ": Received timeout in ST_RETRIEVE_SCL_BASE");
                break;
            case ST_RETRIEVE_SCL_BASE_AND_RETRIEVE_SCL:
                LOG.error(nsclUri + ": Received timeout in ST_RETRIEVE_SCL_BASE_AND_RETRIEVE_SCL");
                break;
            case ST_RETRIEVE_SCL:
                LOG.error(nsclUri + ": Received timeout in ST_RETRIEVE_SCL");
                break;
            case ST_UPDATE_M2M_POC:
                LOG.error(nsclUri + ": Received timeout in ST_UPDATE_M2M_POC");
                break;
            case ST_CREATE_M2M_POC:
                LOG.error(nsclUri + ": Received timeout in ST_CREATE_M2M_POC");
                break;
            case ST_RETRIEVE_NC:
                LOG.error(nsclUri + ": Received timeout in ST_RETRIEVE_NC");
                break;
            case ST_CREATE_NC:
                LOG.error(nsclUri + ": Received timeout in ST_CREATE_NC");
                break;
            case ST_REFRESH_SCL:
                switchToState(ST_REFRESH_UPDATE_SCL);
                sendUpdateScl();
                break;
            case ST_REFRESH_UPDATE_SCL:
                LOG.error(nsclUri + ": Received timeout in ST_REFRESH_UPDATE_SCL");
                break;
            case ST_RETRY_INIT:
                switchToState(ST_INIT_RETRIEVE_SCL_BASE);
                sendRetrieveSclBase();
                break;
            case ST_FINISHED:
                break;
            }
        } catch (IOException e) {
            LOG.error(nsclUri + ": Cannot send a request to NSCL", e);
            switchToState(ST_RETRY_INIT);
            startRetryInitTimer();
        } catch (StorageException e) {
            LOG.error(nsclUri + ": Cannot retrieve sclBase resource", e);
            switchToState(ST_RETRY_INIT);
            startRetryInitTimer();
        } catch (XoException e) {
            LOG.error(nsclUri + ": Cannot build representation", e);
            switchToState(ST_RETRY_INIT);
            startRetryInitTimer();
        }
    }

    public synchronized void unregister() {
        if (LOG.isInfoEnabled()) {
            LOG.info(nsclUri + ": unregister " + ST_NAMES[state]);
        }
        try {
            switch (state) {
            case ST_INIT_RETRIEVE_SCL_BASE:
                switchToState(ST_FINISHED);
                break;
            case ST_UPDATE_ACCESS_RIGHT:
                switchToState(ST_FINISHED);
                cleanupData();
                break;
            case ST_CREATE_ACCESS_RIGHT:
                switchToState(ST_FINISHED);
                cleanupData();
                break;
            case ST_UPDATE_SCL:
                switchToState(ST_FINISHED);
                cleanupData();
                break;
            case ST_CREATE_SCL:
                switchToState(ST_FINISHED);
                cleanupData();
                break;
            case ST_RETRIEVE_SCL_BASE:
                switchToState(ST_FINISHED);
                cleanupData();
                break;
            case ST_RETRIEVE_SCL_BASE_AND_RETRIEVE_SCL:
                switchToState(ST_FINISHED);
                cleanupData();
                break;
            case ST_RETRIEVE_SCL:
                switchToState(ST_FINISHED);
                cleanupData();
                break;
            case ST_UPDATE_M2M_POC:
                switchToState(ST_FINISHED);
                cleanupData();
                break;
            case ST_CREATE_M2M_POC:
                switchToState(ST_FINISHED);
                cleanupData();
                break;
            case ST_RETRIEVE_NC:
                switchToState(ST_FINISHED);
                cleanupData();
                break;
            case ST_CREATE_NC:
                switchToState(ST_FINISHED);
                cleanupData();
                break;
            case ST_REFRESH_SCL:
                manager.getM2MContext().cancelTimer(timerId);
                switchToState(ST_FINISHED);
                cleanupData();
                break;
            case ST_REFRESH_UPDATE_SCL:
                switchToState(ST_FINISHED);
                cleanupData();
                break;
            case ST_RETRY_INIT:
                manager.getM2MContext().cancelTimer(timerId);
                switchToState(ST_FINISHED);
                break;
            case ST_FINISHED:
                break;
            }
        } catch (ParseException e) {
            LOG.error(nsclUri + ": Cannot decode internal resource", e);
        } catch (UnsupportedEncodingException e) {
            LOG.error(nsclUri + ": Cannot retrieve sclBase resource", e);
        } catch (StorageException e) {
            LOG.error(nsclUri + ": Cannot retrieve sclBase resource", e);
        } catch (XoException e) {
            LOG.error(nsclUri + ": Cannot build representation", e);
        } catch (M2MException e) {
            LOG.error(nsclUri + ": M2M error while cleaning registration data", e);
        }
    }
}
