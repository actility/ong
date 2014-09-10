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
 * id $Id: ResponseImpl.java 8761 2014-05-21 15:31:37Z JReich $
 * author $Author: JReich $
 * version $Revision: 8761 $
 * lastrevision $Date: 2014-05-21 17:31:37 +0200 (Wed, 21 May 2014) $
 * modifiedby $LastChangedBy: JReich $
 * lastmodified $LastChangedDate: 2014-05-21 17:31:37 +0200 (Wed, 21 May 2014) $
 */

package com.actility.m2m.m2m.impl;

import java.io.IOException;
import java.net.URI;
import java.util.Date;

import org.apache.log4j.Logger;

import com.actility.m2m.m2m.Indication;
import com.actility.m2m.m2m.M2MConstants;
import com.actility.m2m.m2m.M2MException;
import com.actility.m2m.m2m.Response;
import com.actility.m2m.m2m.StatusCode;
import com.actility.m2m.m2m.log.BundleLogger;
import com.actility.m2m.servlet.song.SongServletMessage;
import com.actility.m2m.servlet.song.SongServletRequest;
import com.actility.m2m.servlet.song.SongServletResponse;
import com.actility.m2m.util.FormatUtils;
import com.actility.m2m.util.log.OSGiLogger;
import com.actility.m2m.xo.XoException;
import com.actility.m2m.xo.XoObject;

public final class ResponseImpl implements Response {

    /**
     *
     */
    private static final long serialVersionUID = -5041950959092955566L;

    private static final Logger LOG = OSGiLogger.getLogger(ResponseImpl.class, BundleLogger.getStaticLogger());

    private static final int[] M2M_STATUS_TO_SONG_STATUS = new int[19];
    private static final String COMMA_SEPARATOR = ", ";

    static {
        M2M_STATUS_TO_SONG_STATUS[StatusCode.STATUS_OK.ordinal()] = SongServletResponse.SC_INTERNAL_SERVER_ERROR;
        M2M_STATUS_TO_SONG_STATUS[StatusCode.STATUS_CREATED.ordinal()] = SongServletResponse.SC_INTERNAL_SERVER_ERROR;
        M2M_STATUS_TO_SONG_STATUS[StatusCode.STATUS_ACCEPTED.ordinal()] = SongServletResponse.SC_INTERNAL_SERVER_ERROR;
        M2M_STATUS_TO_SONG_STATUS[StatusCode.STATUS_BAD_REQUEST.ordinal()] = SongServletResponse.SC_BAD_REQUEST;
        M2M_STATUS_TO_SONG_STATUS[StatusCode.STATUS_PERMISSION_DENIED.ordinal()] = SongServletResponse.SC_PERMISSION_DENIED;
        M2M_STATUS_TO_SONG_STATUS[StatusCode.STATUS_FORBIDDEN.ordinal()] = SongServletResponse.SC_FORBIDDEN;
        M2M_STATUS_TO_SONG_STATUS[StatusCode.STATUS_NOT_FOUND.ordinal()] = SongServletResponse.SC_NOT_FOUND;
        M2M_STATUS_TO_SONG_STATUS[StatusCode.STATUS_METHOD_NOT_ALLOWED.ordinal()] = SongServletResponse.SC_METHOD_NOT_ALLOWED;
        M2M_STATUS_TO_SONG_STATUS[StatusCode.STATUS_NOT_ACCEPTABLE.ordinal()] = SongServletResponse.SC_NOT_ACCEPTABLE;
        M2M_STATUS_TO_SONG_STATUS[StatusCode.STATUS_REQUEST_TIMEOUT.ordinal()] = SongServletResponse.SC_REQUEST_TIMEOUT;
        M2M_STATUS_TO_SONG_STATUS[StatusCode.STATUS_CONFLICT.ordinal()] = SongServletResponse.SC_CONFLICT;
        M2M_STATUS_TO_SONG_STATUS[StatusCode.STATUS_UNSUPPORTED_MEDIA_TYPE.ordinal()] = SongServletResponse.SC_UNSUPPORTED_MEDIA_TYPE;
        M2M_STATUS_TO_SONG_STATUS[StatusCode.STATUS_INTERNAL_SERVER_ERROR.ordinal()] = SongServletResponse.SC_INTERNAL_SERVER_ERROR;
        M2M_STATUS_TO_SONG_STATUS[StatusCode.STATUS_NOT_IMPLEMENTED.ordinal()] = SongServletResponse.SC_NOT_IMPLEMENTED;
        M2M_STATUS_TO_SONG_STATUS[StatusCode.STATUS_BAD_GATEWAY.ordinal()] = SongServletResponse.SC_BAD_GATEWAY;
        M2M_STATUS_TO_SONG_STATUS[StatusCode.STATUS_SERVICE_UNAVAILABLE.ordinal()] = SongServletResponse.SC_SERVICE_UNAVAILABLE;
        M2M_STATUS_TO_SONG_STATUS[StatusCode.STATUS_GATEWAY_TIMEOUT.ordinal()] = SongServletResponse.SC_GATEWAY_TIMEOUT;
        M2M_STATUS_TO_SONG_STATUS[StatusCode.STATUS_DELETED.ordinal()] = SongServletResponse.SC_DELETED;
        M2M_STATUS_TO_SONG_STATUS[StatusCode.STATUS_EXPIRED.ordinal()] = SongServletResponse.SC_INTERNAL_SERVER_ERROR;
    }

    private final IndicationImpl indication;
    private final SongServletResponse songResponse;

    public ResponseImpl(StatusCode statusCode, boolean success, IndicationImpl indication) {
        this.indication = indication;
        SongServletRequest songRequest = indication.getSongRequest();
        int status = SongServletResponse.SC_OK;
        if (success) {
            if (StatusCode.STATUS_CREATED == statusCode) {
                status = SongServletResponse.SC_CREATED;
            } else if (StatusCode.STATUS_ACCEPTED == statusCode) {
                status = SongServletResponse.SC_ACCEPTED;
            } else {
                String method = songRequest.getMethod();
                if (SongServletRequest.MD_UPDATE.equals(method) || SongServletRequest.MD_DELETE.equals(method)) {
                    status = SongServletResponse.SC_NO_CONTENT;
                }
            }
        } else {
            int statusOrdinal = statusCode.ordinal();
            if (statusOrdinal < 0 && statusOrdinal >= M2M_STATUS_TO_SONG_STATUS.length) {
                status = SongServletResponse.SC_INTERNAL_SERVER_ERROR;
                LOG.error("M2M application tries to send an unsuccess response with an unknown or bad status code: "
                        + statusCode.name() + " (" + statusCode.ordinal() + ")");
            } else {
                status = M2M_STATUS_TO_SONG_STATUS[statusOrdinal];
            }
        }
        this.songResponse = songRequest.createResponse(status);
    }

    public ResponseImpl(IndicationImpl indication, SongServletResponse songResponse) {
        this.indication = indication;
        this.songResponse = songResponse;
    }

    public SongServletResponse getSongResponse() {
        return songResponse;
    }

    public boolean isCommitted() {
        return songResponse.isCommitted();
    }

    public String getTransactionId() {
        return songResponse.getId();
    }

    public Indication getIndication() {
        return indication;
    }

    public void setResouceURI(URI resourceURI) {
        songResponse.setHeader(SongServletMessage.HD_CONTENT_LOCATION, resourceURI.toString());
    }

    public void setRepresentation(XoObject representation) throws XoException, IOException, M2MException {
        if (songResponse.getStatus() == SongServletResponse.SC_NO_CONTENT) {
            songResponse.setStatus(SongServletResponse.SC_OK);
        }
        String mediaType = indication.getAcceptedXoMediaType();
        if (M2MConstants.MT_APPLICATION_EXI.equals(mediaType)) {
            songResponse.setContent(representation.saveExi(), M2MConstants.MT_APPLICATION_EXI);
        } else {
            songResponse.setContent(representation.saveXml(), M2MConstants.CT_APPLICATION_XML_UTF8);
        }
    }

    public void setRepresentationWithDefaultMediaType(XoObject representation) throws XoException, IOException {
        if (songResponse.getStatus() == SongServletResponse.SC_NO_CONTENT) {
            songResponse.setStatus(SongServletResponse.SC_OK);
        }
        String mediaType = indication.getM2MUtils().getAvailableXoMediaTypes()[0];
        if (M2MConstants.MT_APPLICATION_EXI.equals(mediaType)) {
            songResponse.setContent(representation.saveExi(), M2MConstants.MT_APPLICATION_EXI);
        } else {
            songResponse.setContent(representation.saveXml(), M2MConstants.CT_APPLICATION_XML_UTF8);
        }
    }

    public void setRawContent(byte[] content, String contentType) throws IOException {
        if (songResponse.getStatus() == SongServletResponse.SC_NO_CONTENT) {
            songResponse.setStatus(SongServletResponse.SC_OK);
        }
        songResponse.setContent(content, contentType);
    }

    public void setCacheExpiration(long cacheExpiration) {
        songResponse.setHeader(SongServletMessage.HD_EXPIRES,
                FormatUtils.formatHttpDate(new Date(System.currentTimeMillis() + cacheExpiration)));
    }

    public void setLastModified(long lastModified) {
        songResponse.setHeader(SongServletMessage.HD_LAST_MODIFIED, FormatUtils.formatHttpDate(new Date(lastModified)));
    }

    public void setETag(String eTag) {
        songResponse.setHeader(SongServletMessage.HD_ETAG, eTag);
    }

    public void setAllow(boolean createAllowed, boolean retrieveAllowed, boolean updateAllowed, boolean deleteAllowed) {
        StringBuffer acceptHeader = new StringBuffer();
        if (createAllowed) {
            acceptHeader.append(SongServletRequest.MD_CREATE);
            acceptHeader.append(COMMA_SEPARATOR);
        }
        if (retrieveAllowed) {
            acceptHeader.append(SongServletRequest.MD_RETRIEVE);
            acceptHeader.append(COMMA_SEPARATOR);
        }
        if (updateAllowed) {
            acceptHeader.append(SongServletRequest.MD_UPDATE);
            acceptHeader.append(COMMA_SEPARATOR);
        }
        if (deleteAllowed) {
            acceptHeader.append(SongServletRequest.MD_DELETE);
            acceptHeader.append(COMMA_SEPARATOR);
        }
        if (acceptHeader.length() > 2) {
            acceptHeader.setLength(acceptHeader.length() - 2);
        }
        songResponse.setHeader(SongServletMessage.HD_ACCEPT, acceptHeader.toString());
    }

    public void addHeader(String name, String value) {
        songResponse.addHeader(name, value);
    }

    public void setHeader(String name, String value) {
        songResponse.setHeader(name, value);
    }

    public void send() throws IOException {
        songResponse.send();
    }
}
