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
 * id $Id: SubManager.java 4186 2013-01-23 10:35:19Z JReich $
 * author $Author: JReich $
 * version $Revision: 4186 $
 * lastrevision $Date: 2013-01-23 11:35:19 +0100 (Wed, 23 Jan 2013) $
 * modifiedby $LastChangedBy: JReich $
 * lastmodified $LastChangedDate: 2013-01-23 11:35:19 +0100 (Wed, 23 Jan 2013) $
 */

package com.actility.m2m.scl.mem;

import java.io.IOException;
import java.io.Serializable;
import java.io.UnsupportedEncodingException;
import java.net.URI;
import java.net.URISyntaxException;
import java.text.ParseException;
import java.util.Date;

import org.apache.log4j.Logger;

import com.actility.m2m.m2m.Confirm;
import com.actility.m2m.m2m.ContentInstanceFilterCriteria;
import com.actility.m2m.m2m.FilterCriteria;
import com.actility.m2m.m2m.M2MConstants;
import com.actility.m2m.m2m.M2MException;
import com.actility.m2m.m2m.Request;
import com.actility.m2m.m2m.StatusCode;
import com.actility.m2m.scl.log.BundleLogger;
import com.actility.m2m.scl.model.Constants;
import com.actility.m2m.scl.model.SclManager;
import com.actility.m2m.scl.model.SclTransaction;
import com.actility.m2m.scl.model.SubscribedResource;
import com.actility.m2m.scl.res.Subscription;
import com.actility.m2m.storage.StorageException;
import com.actility.m2m.util.FormatUtils;
import com.actility.m2m.util.UtilConstants;
import com.actility.m2m.util.log.OSGiLogger;
import com.actility.m2m.xo.XoException;
import com.actility.m2m.xo.XoObject;

public final class SubManager implements Serializable {
    /**
     *
     */
    private static final long serialVersionUID = 6302094925169424397L;

    private static final Logger LOG = OSGiLogger.getLogger(SubManager.class, BundleLogger.getStaticLogger());

    private static final int ST_WAITING_NOTIFICATION = 0;
    private static final int ST_PENDING_NOTIFY = 1;
    private static final int ST_PENDING_NOTIFY_AND_NOTIFICATION = 2;
    private static final int ST_WAITING_MINT = 3;
    private static final int ST_WAITING_RETRY = 4;
    private static final int ST_EXPIRING_PENDING_NOTIFY_WAITING_RETRY = 5;
    private static final int ST_DELETING_PENDING_NOTIFY_WAITING_RETRY = 6;
    private static final int ST_EXPIRING_WAITING_RETRY = 7;
    private static final int ST_DELETING_WAITING_RETRY = 8;
    private static final int ST_EXPIRING_PENDING_NOTIFY = 9;
    private static final int ST_DELETING_PENDING_NOTIFY = 10;
    private static final int ST_EXPIRING = 11;
    private static final int ST_DELETING = 12;
    private static final int ST_DELETED = 14;

    private static final String[] ST_NAMES = { "ST_WAITING_NOTIFICATION", "ST_PENDING_NOTIFY",
            "ST_PENDING_NOTIFY_AND_NOTIFICATION", "ST_WAITING_MINT", "ST_WAITING_RETRY",
            "ST_EXPIRING_PENDING_NOTIFY_WAITING_RETRY", "ST_DELETING_PENDING_NOTIFY_WAITING_RETRY",
            "ST_EXPIRING_WAITING_RETRY", "ST_DELETING_WAITING_RETRY", "ST_EXPIRING_PENDING_NOTIFY",
            "ST_DELETING_PENDING_NOTIFY", "ST_EXPIRING", "ST_DELETING", "ST_DELETED" };

    private static final long RETRY_TIMER = 10000L;

    private final SubsManager subsManager;
    private final SubscribedResource subscribeResource;
    private final String path;
    private final String id;
    private final String subscribedPath;
    private final URI subscriber;
    private final SclManager manager;

    private int nbRetry;
    private String timerId;
    private String expirationTimerId;

    private long pendingLastSuccessRequestTime;
    private long lastSuccessConfirmTime;
    private int state;

    private long minimalTimeBetweenNotifications;
    // private long delayTolerance;
    private URI contactUri;
    private long expirationTime;

    private FilterCriteria filterCriteria;
    private String mediaType;

    // private long ifModifiedSince;
    // private long ifUnmodifiedSince;
    // private long ifNoneMatch;
    // private List searchStrings;
    // private long createdAfter;
    // private long createdBefore;
    // private String attributeAccessor;

    public SubManager(SubsManager subsManager, SclManager manager, String path, String id,
            SubscribedResource subscribeResource, String subscribedToPath, XoObject subscription,
            FilterCriteria filterCriteria, String mediaType) {
        this.subsManager = subsManager;
        this.manager = manager;
        this.path = path;
        this.id = id;
        this.subscribeResource = subscribeResource;
        this.subscribedPath = subscribedToPath;
        this.subscriber = URI.create(subscription.getStringAttribute(M2MConstants.TAG_M2M_SUBSCRIBER_ID));
        this.lastSuccessConfirmTime = 0;
        this.filterCriteria = filterCriteria;
        this.mediaType = mediaType;
        subscriptionUpdated(subscription);
        this.state = ST_WAITING_NOTIFICATION;
    }

    private void updateNotifyRepresentation(XoObject notify, StatusCode statusCode, byte[] representation, String contentType)
            throws XoException, M2MException {
        notify.setStringAttribute(M2MConstants.TAG_M2M_STATUS_CODE, statusCode.name());
        notify.setStringAttribute(M2MConstants.TAG_SUBSCRIPTION_REFERENCE,
                manager.getM2MContext().createLocalUri(contactUri, path).toString());
        XoObject contentBase64 = manager.getXoService().newXmlXoObject(M2MConstants.TYPE_XMIME_BASE64_BINARY);
        contentBase64.setName(M2MConstants.TAG_REPRESENTATION);
        notify.setXoObjectAttribute(M2MConstants.TAG_REPRESENTATION, contentBase64);
        contentBase64.setBufferAttribute(XoObject.ATTR_VALUE, representation);
        contentBase64.setStringAttribute(M2MConstants.TAG_XMIME_CONTENT_TYPE, contentType);
    }

    private void sendErrorNotify(StatusCode statusCode, String message) {
        XoObject errorInfo = null;
        try {
            errorInfo = manager.getXoService().newXmlXoObject(M2MConstants.TAG_M2M_ERROR_INFO);
            errorInfo.setNameSpace(M2MConstants.PREFIX_M2M);
            errorInfo.setStringAttribute(M2MConstants.TAG_M2M_STATUS_CODE, statusCode.name());
            errorInfo.setStringAttribute(M2MConstants.TAG_M2M_ADDITIONAL_INFO, message);
            String mediaType = this.mediaType;
            if (M2MConstants.MT_APPLICATION_EXI.equals(mediaType)) {
                sendNotify(statusCode, errorInfo.saveExi(), mediaType, M2MConstants.MT_APPLICATION_EXI);
            } else {
                sendNotify(statusCode, errorInfo.saveXml(), mediaType, M2MConstants.CT_APPLICATION_XML_UTF8);
            }
        } catch (XoException e) {
            LOG.error(path + ": Xo error while trying to send an error notity", e);
        } finally {
            if (errorInfo != null) {
                errorInfo.free(true);
            }
        }
    }

    public boolean startExpirationTimer() {
        boolean expired = false;
        if (expirationTimerId != null) {
            manager.getM2MContext().cancelTimer(expirationTimerId);
            expirationTimerId = null;
        }
        long now = System.currentTimeMillis();
        if (expirationTime > 0 && expirationTime < FormatUtils.MAX_DATE) {
            if (expirationTime > now) {
                expirationTimerId = manager.getM2MContext().startTimer(expirationTime - now, this);
            } else {
                expired = true;
                subscriptionExpired();
            }
        }
        return expired;
    }

    private void retry() {
        // TODO replace 3 by configuration maxNbOfRetries
        if (nbRetry < 3) {
            switch (state) {
            case ST_WAITING_NOTIFICATION:
                LOG.error(path + ": Received a subscription retry in state " + ST_NAMES[state]);
                break;
            case ST_PENDING_NOTIFY:
            case ST_PENDING_NOTIFY_AND_NOTIFICATION:
                state = ST_WAITING_RETRY;
                break;
            case ST_WAITING_MINT:
            case ST_WAITING_RETRY:
            case ST_EXPIRING_PENDING_NOTIFY_WAITING_RETRY:
            case ST_DELETING_PENDING_NOTIFY_WAITING_RETRY:
            case ST_EXPIRING_WAITING_RETRY:
            case ST_DELETING_WAITING_RETRY:
                LOG.error(path + ": Received a subscription retry in state " + ST_NAMES[state]);
                break;
            case ST_EXPIRING_PENDING_NOTIFY:
                state = ST_EXPIRING_PENDING_NOTIFY_WAITING_RETRY;
                break;
            case ST_DELETING_PENDING_NOTIFY:
                state = ST_DELETING_PENDING_NOTIFY_WAITING_RETRY;
                break;
            case ST_EXPIRING:
                state = ST_EXPIRING_WAITING_RETRY;
                break;
            case ST_DELETING:
                state = ST_DELETING_WAITING_RETRY;
                break;
            case ST_DELETED:
                LOG.error(path + ": Received a subscription retry in state " + ST_NAMES[state]);
                break;
            }
            ++nbRetry;
            timerId = manager.getM2MContext().startTimer(RETRY_TIMER, this);
        } else {
            LOG.error(path + ": Too much retries attempted to replay event on <subscription>. Giving up");
            // Omit this notification
            switch (state) {
            case ST_WAITING_NOTIFICATION:
                LOG.error(path + ": Received a subscription retry in state " + ST_NAMES[state]);
                break;
            case ST_PENDING_NOTIFY:
                nbRetry = 0;
                state = ST_WAITING_NOTIFICATION;
                break;
            case ST_PENDING_NOTIFY_AND_NOTIFICATION:
                nbRetry = 0;
                state = ST_PENDING_NOTIFY;
                retrieveAndSendNotify();
                break;
            case ST_WAITING_MINT:
            case ST_WAITING_RETRY:
            case ST_EXPIRING_PENDING_NOTIFY_WAITING_RETRY:
            case ST_DELETING_PENDING_NOTIFY_WAITING_RETRY:
            case ST_EXPIRING_WAITING_RETRY:
            case ST_DELETING_WAITING_RETRY:
                LOG.error(path + ": Received a subscription retry in state " + ST_NAMES[state]);
                break;
            case ST_EXPIRING_PENDING_NOTIFY:
                nbRetry = 0;
                state = ST_EXPIRING;
                break;
            case ST_DELETING_PENDING_NOTIFY:
                nbRetry = 0;
                state = ST_DELETING;
                break;
            case ST_EXPIRING:
            case ST_DELETING:
                nbRetry = 0;
                state = ST_DELETED;
                break;
            case ST_DELETED:
                LOG.error(path + ": Received a subscription retry in state " + ST_NAMES[state]);
                break;
            }
        }
    }

    public void retrieveAndSendNotify() {
        if (LOG.isInfoEnabled()) {
            LOG.info(path + ": retrieve and send notify");
        }
        try {
            String mediaType = this.mediaType;
            String contentType = M2MConstants.CT_APPLICATION_XML_UTF8;
            if (M2MConstants.MT_APPLICATION_EXI.equals(mediaType)) {
                contentType = M2MConstants.MT_APPLICATION_EXI;
            }
            byte[] notify = subscribeResource.getResponseRepresentation(path, manager, subscriber, subscribedPath,
                    filterCriteria, null, mediaType);
            if (notify != null) {
                state = ST_PENDING_NOTIFY;
                sendNotify(StatusCode.STATUS_OK, notify, mediaType, contentType);
            } else {
                if (LOG.isInfoEnabled()) {
                    LOG.info(path + ": no representation retrieved wait next notification");
                }
                state = ST_WAITING_NOTIFICATION;
            }
        } catch (UnsupportedEncodingException e) {
            LOG.error(path + ": Unsupported encoding error while trying to retrieve subscribed resource and send a notity", e);
            retry();
        } catch (StorageException e) {
            LOG.error(path + ": Storage error while trying to retrieve subscribed resource send a notity", e);
            retry();
        } catch (XoException e) {
            LOG.error(path + ": Xo error while trying to retrieve subscribed resource send a notity", e);
            retry();
        } catch (M2MException e) {
            LOG.error(path + ": M2M error while trying to retrieve subscribed resource send a notity", e);
            retry();
        }
    }

    public FilterCriteria getFilterCriteria() {
        return filterCriteria;
    }

    public void sendNotify(StatusCode statusCode, byte[] resource, String mediaType, String contentType) {
        XoObject notify = null;
        try {
            pendingLastSuccessRequestTime = System.currentTimeMillis();
            notify = manager.getXoService().newXmlXoObject(M2MConstants.TAG_M2M_NOTIFY);
            notify.setNameSpace(M2MConstants.PREFIX_M2M);
            notify.setNameSpace(M2MConstants.PREFIX_XMIME);
            updateNotifyRepresentation(notify, statusCode, resource, contentType);
            if (LOG.isInfoEnabled()) {
                LOG.info(path + ": subscription send NOTIFY to \"" + contactUri + "\"");
            }
            URI localRequestingEntity = manager.getM2MContext().createLocalUri(contactUri, Constants.PATH_ROOT);
            Request request = manager.getM2MContext().createRequest(M2MConstants.MD_CREATE, localRequestingEntity, contactUri);
            request.setAttribute(Constants.AT_REQUESTOR, this);
            request.setAttribute(Constants.AT_STATUS, statusCode);
            request.setRepresentation(notify, mediaType);
            // TODO replace 10000L by configuration notifyExpiration
            request.send(10000L);
        } catch (IOException e) {
            LOG.error(path + ": IO error while trying to send a notity", e);
            retry();
        } catch (XoException e) {
            LOG.error(path + ": Xo error while trying to send a notity", e);
            retry();
        } catch (M2MException e) {
            LOG.error(path + ": M2M error while trying to send a notity", e);
            retry();
        } finally {
            if (notify != null) {
                notify.free(true);
            }
        }
    }

    public void subscriptionUpdated(XoObject subscription) {
        String minimalTimeBetweenNotificationsAttr = subscription
                .getStringAttribute(M2MConstants.TAG_M2M_MINIMAL_TIME_BETWEEN_NOTIFICATIONS);
        if (minimalTimeBetweenNotificationsAttr != null) {
            try {
                this.minimalTimeBetweenNotifications = Long.parseLong(minimalTimeBetweenNotificationsAttr);
            } catch (NumberFormatException e) {
                this.minimalTimeBetweenNotifications = 0;
                LOG.error(path + ": Invalid minimalTimeBetweenNotification detected in subscription. Use 0 value instead", e);
            }
        } else {
            this.minimalTimeBetweenNotifications = 0;
        }
        String contact = subscription.getStringAttribute(M2MConstants.TAG_M2M_CONTACT);
        if (contact != null) {
            try {
                this.contactUri = new URI(contact);
            } catch (URISyntaxException e) {
                LOG.error(path + ": Invalid contact URI detected in subscription: " + contact, e);
            }
        } else {
            LOG.error(path + ": contact URI is null");
        }
        String expirationTimeAttr = subscription.getStringAttribute(M2MConstants.TAG_M2M_EXPIRATION_TIME);
        if (expirationTimeAttr != null) {
            try {
                this.expirationTime = FormatUtils.parseDateTime(expirationTimeAttr).getTime();
            } catch (ParseException e) {
                this.expirationTime = 0;
                LOG.error(path + ": Invalid expirationTime detected in subscription. Use 0 value instead", e);
            }
        } else {
            this.expirationTime = 0;
        }
    }

    public void subscriptionExpired() {
        try {
            switch (state) {
            case ST_WAITING_NOTIFICATION:
                state = ST_EXPIRING;
                subsManager.removeSubscription(id);
                manager.getStorageContext().delete(null, manager.getStorageContext().getStorageFactory().createDocument(path),
                        null);
                sendErrorNotify(StatusCode.STATUS_EXPIRED, "Subscription has expired");
                break;
            case ST_PENDING_NOTIFY:
            case ST_PENDING_NOTIFY_AND_NOTIFICATION:
                nbRetry = 0;
                state = ST_EXPIRING_PENDING_NOTIFY;
                subsManager.removeSubscription(id);
                manager.getStorageContext().delete(null, manager.getStorageContext().getStorageFactory().createDocument(path),
                        null);
                sendErrorNotify(StatusCode.STATUS_EXPIRED, "Subscription has expired");
                break;
            case ST_WAITING_MINT:
            case ST_WAITING_RETRY:
                nbRetry = 0;
                state = ST_EXPIRING;
                subsManager.removeSubscription(id);
                manager.getM2MContext().cancelTimer(timerId);
                manager.getStorageContext().delete(null, manager.getStorageContext().getStorageFactory().createDocument(path),
                        null);
                sendErrorNotify(StatusCode.STATUS_EXPIRED, "Subscription has expired");
                break;
            case ST_EXPIRING_PENDING_NOTIFY_WAITING_RETRY:
            case ST_DELETING_PENDING_NOTIFY_WAITING_RETRY:
            case ST_EXPIRING_WAITING_RETRY:
            case ST_DELETING_WAITING_RETRY:
            case ST_EXPIRING_PENDING_NOTIFY:
            case ST_DELETING_PENDING_NOTIFY:
            case ST_EXPIRING:
            case ST_DELETING:
            case ST_DELETED:
                LOG.error(path + ": Received a subscription expired in state " + ST_NAMES[state]);
                break;
            }
        } catch (StorageException e) {
            LOG.error(path + ": Cannot delete subscription while treating its expiration", e);
            retry();
        }
    }

    public void subscriptionDeleted() {
        // Stop timer
        switch (state) {
        case ST_WAITING_NOTIFICATION:
            if (expirationTimerId != null) {
                manager.getM2MContext().cancelTimer(expirationTimerId);
            }
            state = ST_DELETED;
            break;
        case ST_PENDING_NOTIFY:
        case ST_PENDING_NOTIFY_AND_NOTIFICATION:
            if (expirationTimerId != null) {
                manager.getM2MContext().cancelTimer(expirationTimerId);
            }
            state = ST_DELETING;
            break;
        case ST_WAITING_MINT:
        case ST_WAITING_RETRY:
            if (expirationTimerId != null) {
                manager.getM2MContext().cancelTimer(expirationTimerId);
            }
            manager.getM2MContext().cancelTimer(timerId);
            state = ST_DELETED;
            break;
        case ST_EXPIRING_PENDING_NOTIFY_WAITING_RETRY:
        case ST_DELETING_PENDING_NOTIFY_WAITING_RETRY:
        case ST_EXPIRING_WAITING_RETRY:
        case ST_DELETING_WAITING_RETRY:
        case ST_EXPIRING_PENDING_NOTIFY:
        case ST_DELETING_PENDING_NOTIFY:
        case ST_EXPIRING:
        case ST_DELETING:
            break;
        case ST_DELETED:
            LOG.error(path + ": Received a subscription deleted in state " + ST_NAMES[state]);
            break;
        }
    }

    public boolean resourceUpdated(long now) {
        boolean sendNotify = false;
        switch (state) {
        case ST_WAITING_NOTIFICATION:
            if (now > (lastSuccessConfirmTime + minimalTimeBetweenNotifications)) {
                state = ST_PENDING_NOTIFY;
                sendNotify = true;
            } else {
                state = ST_WAITING_MINT;
                timerId = manager.getM2MContext().startTimer((lastSuccessConfirmTime + minimalTimeBetweenNotifications) - now,
                        this);
            }
            break;
        case ST_PENDING_NOTIFY:
        case ST_PENDING_NOTIFY_AND_NOTIFICATION:
            // We set the value to -1 set that it can be reset to zero when using ++ in doUnsuccessConfirm
            nbRetry = -1;
            state = ST_PENDING_NOTIFY_AND_NOTIFICATION;
            break;
        case ST_WAITING_MINT:
            break;
        case ST_WAITING_RETRY:
            // We set the value to -1 set that it can be reset to zero when using ++ in doUnsuccessConfirm
            nbRetry = -1;
            break;
        case ST_EXPIRING_PENDING_NOTIFY_WAITING_RETRY:
        case ST_DELETING_PENDING_NOTIFY_WAITING_RETRY:
        case ST_EXPIRING_WAITING_RETRY:
        case ST_DELETING_WAITING_RETRY:
        case ST_EXPIRING_PENDING_NOTIFY:
        case ST_DELETING_PENDING_NOTIFY:
        case ST_EXPIRING:
        case ST_DELETING:
        case ST_DELETED:
            LOG.error(path + ": Received a resource updated in state " + ST_NAMES[state]);
            break;
        }
        return sendNotify;
    }

    public void resourceDeleted() {
        switch (state) {
        case ST_WAITING_NOTIFICATION:
            if (expirationTimerId != null) {
                manager.getM2MContext().cancelTimer(expirationTimerId);
            }
            state = ST_DELETING;
            sendErrorNotify(StatusCode.STATUS_DELETED, "Subscribed resource has been deleted");
            break;
        case ST_PENDING_NOTIFY:
        case ST_PENDING_NOTIFY_AND_NOTIFICATION:
            if (expirationTimerId != null) {
                manager.getM2MContext().cancelTimer(expirationTimerId);
            }
            nbRetry = 0;
            state = ST_DELETING_PENDING_NOTIFY;
            sendErrorNotify(StatusCode.STATUS_DELETED, "Subscribed resource has been deleted");
            break;
        case ST_WAITING_MINT:
        case ST_WAITING_RETRY:
            if (expirationTimerId != null) {
                manager.getM2MContext().cancelTimer(expirationTimerId);
            }
            manager.getM2MContext().cancelTimer(timerId);
            nbRetry = 0;
            state = ST_DELETING;
            sendErrorNotify(StatusCode.STATUS_DELETED, "Subscribed resource has been deleted");
            break;
        case ST_EXPIRING_PENDING_NOTIFY_WAITING_RETRY:
        case ST_DELETING_PENDING_NOTIFY_WAITING_RETRY:
        case ST_EXPIRING_WAITING_RETRY:
        case ST_DELETING_WAITING_RETRY:
        case ST_EXPIRING_PENDING_NOTIFY:
        case ST_DELETING_PENDING_NOTIFY:
        case ST_EXPIRING:
        case ST_DELETING:
        case ST_DELETED:
            LOG.error(path + ": Received a resource deleted in state " + ST_NAMES[state]);
            break;
        }
    }

    public void timeout(String timerId) {
        try {
            switch (state) {
            case ST_WAITING_NOTIFICATION:
            case ST_PENDING_NOTIFY:
            case ST_PENDING_NOTIFY_AND_NOTIFICATION:
                if (expirationTimerId != null && timerId.equals(expirationTimerId)) {
                    subscriptionExpired();
                } else {
                    LOG.error(path + ": Received a timeout in state " + ST_NAMES[state]);
                }
                break;
            case ST_WAITING_MINT:
            case ST_WAITING_RETRY:
                if (expirationTimerId != null && timerId.equals(expirationTimerId)) {
                    subscriptionExpired();
                } else {
                    state = ST_PENDING_NOTIFY;
                    retrieveAndSendNotify();
                }
                break;
            case ST_EXPIRING_PENDING_NOTIFY_WAITING_RETRY:
                if (expirationTimerId != null && timerId.equals(expirationTimerId)) {
                    LOG.error(path + ": Received a timeout in state " + ST_NAMES[state]);
                } else {
                    state = ST_EXPIRING_PENDING_NOTIFY;
                    manager.getStorageContext().delete(null,
                            manager.getStorageContext().getStorageFactory().createDocument(path), null);
                    sendErrorNotify(StatusCode.STATUS_EXPIRED, "Subscription has expired");
                }
                break;
            case ST_DELETING_PENDING_NOTIFY_WAITING_RETRY:
                if (expirationTimerId != null && timerId.equals(expirationTimerId)) {
                    LOG.error(path + ": Received a timeout in state " + ST_NAMES[state]);
                } else {
                    state = ST_DELETING_PENDING_NOTIFY;
                    sendErrorNotify(StatusCode.STATUS_DELETED, "Subscribed resource has been deleted");
                }
                break;
            case ST_EXPIRING_WAITING_RETRY:
                if (expirationTimerId != null && timerId.equals(expirationTimerId)) {
                    LOG.error(path + ": Received a timeout in state " + ST_NAMES[state]);
                } else {
                    state = ST_EXPIRING;
                    manager.getStorageContext().delete(null,
                            manager.getStorageContext().getStorageFactory().createDocument(path), null);
                    sendErrorNotify(StatusCode.STATUS_EXPIRED, "Subscription has expired");
                }
                break;
            case ST_DELETING_WAITING_RETRY:
                if (expirationTimerId != null && timerId.equals(expirationTimerId)) {
                    LOG.error(path + ": Received a timeout in state " + ST_NAMES[state]);
                } else {
                    state = ST_DELETING;
                    sendErrorNotify(StatusCode.STATUS_DELETED, "Subscribed resource has been deleted");
                }
                break;
            case ST_EXPIRING_PENDING_NOTIFY:
            case ST_DELETING_PENDING_NOTIFY:
            case ST_EXPIRING:
            case ST_DELETING:
            case ST_DELETED:
                LOG.error(path + ": Received a timeout in state " + ST_NAMES[state]);
                break;
            }
        } catch (StorageException e) {
            LOG.error(path + ": Cannot delete subscription while treating its expiration", e);
            retry();
        }
    }

    public void doSuccessConfirm(Confirm confirm) {
        if (LOG.isInfoEnabled()) {
            LOG.info(path + ": Received confirm response to notify");
        }
        switch (state) {
        case ST_WAITING_NOTIFICATION:
            LOG.error(path + ": Received a success confirm in state " + ST_NAMES[state]);
            break;
        case ST_PENDING_NOTIFY:
            if (subscribeResource.dependsOnFilterCriteria()) {
                if (filterCriteria == null) {
                    filterCriteria = subscribeResource.buildFilterCriteria();
                }
                Date ifModifiedSince = new Date(pendingLastSuccessRequestTime);
                Date oldIfModifiedSince = filterCriteria.getIfModifiedSince();
                filterCriteria.setIfModifiedSince(ifModifiedSince);
                subsManager.updateIfModifiedSince(filterCriteria, oldIfModifiedSince, ifModifiedSince);
            }
            lastSuccessConfirmTime = System.currentTimeMillis();
            nbRetry = 0;
            state = ST_WAITING_NOTIFICATION;
            break;
        case ST_PENDING_NOTIFY_AND_NOTIFICATION:
            if (subscribeResource.dependsOnFilterCriteria()) {
                if (filterCriteria == null) {
                    filterCriteria = subscribeResource.buildFilterCriteria();
                }
                Date ifModifiedSince = new Date(pendingLastSuccessRequestTime);
                Date oldIfModifiedSince = filterCriteria.getIfModifiedSince();
                filterCriteria.setIfModifiedSince(ifModifiedSince);
                subsManager.updateIfModifiedSince(filterCriteria, oldIfModifiedSince, ifModifiedSince);
            }
            lastSuccessConfirmTime = System.currentTimeMillis();
            nbRetry = 0;
            if (minimalTimeBetweenNotifications <= 0) {
                state = ST_PENDING_NOTIFY;
                retrieveAndSendNotify();
            } else {
                state = ST_WAITING_MINT;
                timerId = manager.getM2MContext().startTimer(minimalTimeBetweenNotifications, this);
            }
            break;
        case ST_WAITING_MINT:
        case ST_WAITING_RETRY:
            LOG.error(path + ": Received a success confirm in state " + ST_NAMES[state]);
            break;
        case ST_EXPIRING_PENDING_NOTIFY_WAITING_RETRY:
            state = ST_EXPIRING_WAITING_RETRY;
            break;
        case ST_DELETING_PENDING_NOTIFY_WAITING_RETRY:
            state = ST_DELETING_WAITING_RETRY;
            break;
        case ST_EXPIRING_WAITING_RETRY:
        case ST_DELETING_WAITING_RETRY:
            LOG.error(path + ": Received a success confirm in state " + ST_NAMES[state]);
            break;
        case ST_EXPIRING_PENDING_NOTIFY:
            state = ST_EXPIRING;
            break;
        case ST_DELETING_PENDING_NOTIFY:
            state = ST_DELETING;
            break;
        case ST_EXPIRING:
        case ST_DELETING:
            state = ST_DELETED;
            break;
        case ST_DELETED:
            LOG.error(path + ": Received a success confirm in state " + ST_NAMES[state]);
            break;
        }
    }

    public void doUnsuccessConfirm(Confirm confirm) {
        switch (state) {
        case ST_WAITING_NOTIFICATION:
            LOG.error(path + ": Received an unsuccess confirm in state " + ST_NAMES[state]);
            break;
        case ST_PENDING_NOTIFY:
        case ST_PENDING_NOTIFY_AND_NOTIFICATION:
            int statusCode = confirm.getStatusCode().ordinal();
            if (statusCode == StatusCode.STATUS_INTERNAL_SERVER_ERROR.ordinal()
                    || statusCode == StatusCode.STATUS_NOT_IMPLEMENTED.ordinal()
                    || statusCode == StatusCode.STATUS_BAD_GATEWAY.ordinal()
                    || statusCode == StatusCode.STATUS_SERVICE_UNAVAILABLE.ordinal()
                    || statusCode == StatusCode.STATUS_GATEWAY_TIMEOUT.ordinal()) {
                retry();
            } else {
                // Delete subscription
                XoObject resource = null;
                try {
                    resource = manager.getXoResource(path);
                    SclTransaction transaction = new SclTransaction(manager.getStorageContext());
                    Subscription.getInstance().deleteResource(path, manager, path, resource, transaction);
                    transaction.execute();
                } catch (ParseException e) {
                    LOG.error(path + ": Cannot decode internal resource", e);
                    retry();
                } catch (StorageException e) {
                    LOG.error(path + ": Storage error while trying to delete the subscription", e);
                    retry();
                } catch (XoException e) {
                    LOG.error(path + ": Cannot decode the subscription", e);
                    retry();
                } catch (M2MException e) {
                    LOG.error(path + ": M2M error while trying to delete the subscription", e);
                    retry();
                } finally {
                    if (resource != null) {
                        resource.free(true);
                    }
                }
            }
            break;
        case ST_WAITING_MINT:
        case ST_WAITING_RETRY:
            LOG.error(path + ": Received an unsuccess confirm in state " + ST_NAMES[state]);
            break;
        case ST_EXPIRING_PENDING_NOTIFY_WAITING_RETRY:
            state = ST_DELETING_WAITING_RETRY;
            break;
        case ST_DELETING_PENDING_NOTIFY_WAITING_RETRY:
            state = ST_DELETING_WAITING_RETRY;
            break;
        case ST_EXPIRING_WAITING_RETRY:
        case ST_DELETING_WAITING_RETRY:
            LOG.error(path + ": Received an unsuccess confirm in state " + ST_NAMES[state]);
            break;
        case ST_EXPIRING_PENDING_NOTIFY:
            if (!StatusCode.STATUS_OK.equals(confirm.getRequest().getAttribute(Constants.AT_STATUS))) {
                // Expiring response
                statusCode = confirm.getStatusCode().ordinal();
                if (statusCode == StatusCode.STATUS_INTERNAL_SERVER_ERROR.ordinal()
                        || statusCode == StatusCode.STATUS_NOT_IMPLEMENTED.ordinal()
                        || statusCode == StatusCode.STATUS_BAD_GATEWAY.ordinal()
                        || statusCode == StatusCode.STATUS_SERVICE_UNAVAILABLE.ordinal()
                        || statusCode == StatusCode.STATUS_GATEWAY_TIMEOUT.ordinal()) {
                    retry();
                } else {
                    state = ST_EXPIRING;
                }
            } else {
                state = ST_EXPIRING;
            }
            break;
        case ST_DELETING_PENDING_NOTIFY:
            if (!StatusCode.STATUS_OK.equals(confirm.getRequest().getAttribute(Constants.AT_STATUS))) {
                // Deleting response
                statusCode = confirm.getStatusCode().ordinal();
                if (statusCode == StatusCode.STATUS_INTERNAL_SERVER_ERROR.ordinal()
                        || statusCode == StatusCode.STATUS_NOT_IMPLEMENTED.ordinal()
                        || statusCode == StatusCode.STATUS_BAD_GATEWAY.ordinal()
                        || statusCode == StatusCode.STATUS_SERVICE_UNAVAILABLE.ordinal()
                        || statusCode == StatusCode.STATUS_GATEWAY_TIMEOUT.ordinal()) {
                    retry();
                } else {
                    state = ST_DELETING;
                }
            } else {
                state = ST_DELETING;
            }
            break;
        case ST_EXPIRING:
            if (!StatusCode.STATUS_OK.equals(confirm.getRequest().getAttribute(Constants.AT_STATUS))) {
                // Expiring response
                statusCode = confirm.getStatusCode().ordinal();
                if (statusCode == StatusCode.STATUS_INTERNAL_SERVER_ERROR.ordinal()
                        || statusCode == StatusCode.STATUS_NOT_IMPLEMENTED.ordinal()
                        || statusCode == StatusCode.STATUS_BAD_GATEWAY.ordinal()
                        || statusCode == StatusCode.STATUS_SERVICE_UNAVAILABLE.ordinal()
                        || statusCode == StatusCode.STATUS_GATEWAY_TIMEOUT.ordinal()) {
                    retry();
                } else {
                    state = ST_DELETED;
                }
            } else {
                state = ST_DELETED;
            }
            break;
        case ST_DELETING:
            if (!StatusCode.STATUS_OK.equals(confirm.getRequest().getAttribute(Constants.AT_STATUS))) {
                // Deleting response
                statusCode = confirm.getStatusCode().ordinal();
                if (statusCode == StatusCode.STATUS_INTERNAL_SERVER_ERROR.ordinal()
                        || statusCode == StatusCode.STATUS_NOT_IMPLEMENTED.ordinal()
                        || statusCode == StatusCode.STATUS_BAD_GATEWAY.ordinal()
                        || statusCode == StatusCode.STATUS_SERVICE_UNAVAILABLE.ordinal()
                        || statusCode == StatusCode.STATUS_GATEWAY_TIMEOUT.ordinal()) {
                    retry();
                } else {
                    state = ST_DELETED;
                }
            } else {
                state = ST_DELETED;
            }
            break;
        case ST_DELETED:
            LOG.error(path + ": Received an unsuccess confirm in state " + ST_NAMES[state]);
            break;
        }
    }

    public void printContentInstanceFilterCriteria(StringBuffer buffer) {
        ContentInstanceFilterCriteria cifc = (ContentInstanceFilterCriteria) filterCriteria;
        printFilterCriteria(buffer);
        if (cifc.getSizeFrom() != null) {
            buffer.append("      sizeFrom: ").append(cifc.getSizeFrom()).append("\n");
        }
        if (cifc.getSizeUntil() != null) {
            buffer.append("      sizeUntil: ").append(cifc.getSizeUntil()).append("\n");
        }
        if (cifc.getContentType() != null) {
            String[] array = cifc.getIfNoneMatch();
            buffer.append("      contentType: [");
            for (int i = 0; i < array.length; ++i) {
                buffer.append(" ").append(array[i]);
            }
            buffer.append(" ]\n");
        }
        if (cifc.getMetaDataOnly() != null) {
            buffer.append("      metaDataOnly: ").append(cifc.getMetaDataOnly()).append("\n");
        }
    }

    public void printFilterCriteria(StringBuffer buffer) {
        if (filterCriteria.getCreatedAfter() != null) {
            buffer.append("      createdAfter: ")
                    .append(FormatUtils.formatDateTime(filterCriteria.getCreatedAfter(), UtilConstants.LOCAL_TIMEZONE))
                    .append("\n");
        }
        if (filterCriteria.getCreatedBefore() != null) {
            buffer.append("      createdBefore: ")
                    .append(FormatUtils.formatDateTime(filterCriteria.getCreatedBefore(), UtilConstants.LOCAL_TIMEZONE))
                    .append("\n");
        }
        if (filterCriteria.getIfModifiedSince() != null) {
            buffer.append("      ifModifiedSince: ")
                    .append(FormatUtils.formatDateTime(filterCriteria.getIfModifiedSince(), UtilConstants.LOCAL_TIMEZONE))
                    .append("\n");
        }
        if (filterCriteria.getIfUnmodifiedSince() != null) {
            buffer.append("      ifUnmodifiedSince: ")
                    .append(FormatUtils.formatDateTime(filterCriteria.getIfUnmodifiedSince(), UtilConstants.LOCAL_TIMEZONE))
                    .append("\n");
        }
        if (filterCriteria.getSearchString() != null) {
            String[] array = filterCriteria.getSearchString();
            buffer.append("      searchString: [");
            for (int i = 0; i < array.length; ++i) {
                buffer.append(" ").append(array[i]);
            }
            buffer.append(" ]\n");
        }
        if (filterCriteria.getIfNoneMatch() != null) {
            String[] array = filterCriteria.getIfNoneMatch();
            buffer.append("      ifNoneMatch: [");
            for (int i = 0; i < array.length; ++i) {
                buffer.append(" ").append(array[i]);
            }
            buffer.append(" ]\n");
        }
        if (filterCriteria.getAttributeAccessor() != null) {
            buffer.append("      attributeAccessor: ").append(filterCriteria.getAttributeAccessor()).append("\n");
        }
    }

    public void print(StringBuffer buffer) {
        buffer.append("    sub: ").append(id).append(" state: ").append(ST_NAMES[state]).append(" nbRetry: ").append(nbRetry)
                .append(" pendingLastSuccessRequestTime: ").append(pendingLastSuccessRequestTime)
                .append(" lastSuccessConfirmTime: ").append(lastSuccessConfirmTime).append(" mediaType: ").append(mediaType)
                .append('\n');

        if (filterCriteria != null) {
            if (filterCriteria instanceof ContentInstanceFilterCriteria) {
                printContentInstanceFilterCriteria(buffer);
            } else {
                printFilterCriteria(buffer);
            }
        }
    }

    public String getMediaType() {
        return mediaType;
    }

    public void setMediaType(String mediaType) {
        this.mediaType = mediaType;
    }

    public URI getSubscriber() {
        return subscriber;
    }
}
