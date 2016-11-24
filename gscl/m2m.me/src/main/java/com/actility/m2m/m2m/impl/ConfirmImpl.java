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
 */

package com.actility.m2m.m2m.impl;

import java.io.IOException;
import java.net.URI;
import java.net.URISyntaxException;
import java.util.Iterator;
import java.util.ListIterator;

import org.apache.log4j.Logger;

import com.actility.m2m.m2m.Confirm;
import com.actility.m2m.m2m.M2MConstants;
import com.actility.m2m.m2m.M2MException;
import com.actility.m2m.m2m.Request;
import com.actility.m2m.m2m.StatusCode;
import com.actility.m2m.m2m.log.BundleLogger;
import com.actility.m2m.servlet.song.SongServletMessage;
import com.actility.m2m.servlet.song.SongServletResponse;
import com.actility.m2m.util.log.OSGiLogger;
import com.actility.m2m.xo.XoException;
import com.actility.m2m.xo.XoObject;
import com.actility.m2m.xo.XoService;

public final class ConfirmImpl implements Confirm {
    /**
     *
     */
    private static final long serialVersionUID = -7843000168974152801L;

    private static final Logger LOG = OSGiLogger.getLogger(ConfirmImpl.class, BundleLogger.getStaticLogger());

    private static final StatusCode[][] SONG_STATUS_TO_M2M_STATUS = new StatusCode[4][];

    static {
        // 2xx responses
        SONG_STATUS_TO_M2M_STATUS[0] = new StatusCode[5];
        SONG_STATUS_TO_M2M_STATUS[0][0] = StatusCode.STATUS_OK;
        SONG_STATUS_TO_M2M_STATUS[0][1] = StatusCode.STATUS_CREATED;
        SONG_STATUS_TO_M2M_STATUS[0][2] = StatusCode.STATUS_ACCEPTED;
        SONG_STATUS_TO_M2M_STATUS[0][3] = StatusCode.STATUS_OK;
        SONG_STATUS_TO_M2M_STATUS[0][4] = StatusCode.STATUS_OK;

        // 3xx responses
        SONG_STATUS_TO_M2M_STATUS[1] = new StatusCode[5];
        SONG_STATUS_TO_M2M_STATUS[1][0] = null;
        SONG_STATUS_TO_M2M_STATUS[1][1] = null;
        SONG_STATUS_TO_M2M_STATUS[1][2] = null;
        SONG_STATUS_TO_M2M_STATUS[1][3] = null;
        SONG_STATUS_TO_M2M_STATUS[1][4] = StatusCode.STATUS_OK;

        // 4xx responses
        SONG_STATUS_TO_M2M_STATUS[2] = new StatusCode[16];
        SONG_STATUS_TO_M2M_STATUS[2][0] = StatusCode.STATUS_BAD_REQUEST;
        SONG_STATUS_TO_M2M_STATUS[2][1] = StatusCode.STATUS_PERMISSION_DENIED;
        SONG_STATUS_TO_M2M_STATUS[2][2] = StatusCode.STATUS_BAD_REQUEST;
        SONG_STATUS_TO_M2M_STATUS[2][3] = StatusCode.STATUS_FORBIDDEN;
        SONG_STATUS_TO_M2M_STATUS[2][4] = StatusCode.STATUS_NOT_FOUND;
        SONG_STATUS_TO_M2M_STATUS[2][5] = StatusCode.STATUS_METHOD_NOT_ALLOWED;
        SONG_STATUS_TO_M2M_STATUS[2][6] = StatusCode.STATUS_NOT_ACCEPTABLE;
        SONG_STATUS_TO_M2M_STATUS[2][7] = StatusCode.STATUS_NOT_IMPLEMENTED;
        SONG_STATUS_TO_M2M_STATUS[2][8] = StatusCode.STATUS_REQUEST_TIMEOUT;
        SONG_STATUS_TO_M2M_STATUS[2][9] = StatusCode.STATUS_CONFLICT;
        SONG_STATUS_TO_M2M_STATUS[2][10] = StatusCode.STATUS_DELETED;
        SONG_STATUS_TO_M2M_STATUS[2][11] = null;
        SONG_STATUS_TO_M2M_STATUS[2][12] = StatusCode.STATUS_BAD_REQUEST;
        SONG_STATUS_TO_M2M_STATUS[2][13] = null;
        SONG_STATUS_TO_M2M_STATUS[2][14] = null;
        SONG_STATUS_TO_M2M_STATUS[2][15] = StatusCode.STATUS_UNSUPPORTED_MEDIA_TYPE;

        // 5xx responses
        SONG_STATUS_TO_M2M_STATUS[3] = new StatusCode[5];
        SONG_STATUS_TO_M2M_STATUS[3][0] = StatusCode.STATUS_INTERNAL_SERVER_ERROR;
        SONG_STATUS_TO_M2M_STATUS[3][1] = StatusCode.STATUS_NOT_IMPLEMENTED;
        SONG_STATUS_TO_M2M_STATUS[3][2] = StatusCode.STATUS_BAD_GATEWAY;
        SONG_STATUS_TO_M2M_STATUS[3][3] = StatusCode.STATUS_SERVICE_UNAVAILABLE;
        SONG_STATUS_TO_M2M_STATUS[3][4] = StatusCode.STATUS_GATEWAY_TIMEOUT;
    }

    private final XoService xoService;
    private final SongServletResponse songResponse;
    private final StatusCode statusCode;
    private XoObject representation;

    public ConfirmImpl(XoService xoService, SongServletResponse response) {
        this.xoService = xoService;
        this.songResponse = response;
        int status = response.getStatus();
        int statusClass = status / 100;
        int statusValue = status % 100;
        StatusCode m2mStatus = null;
        if (statusClass == 2) {
            if (statusValue < SONG_STATUS_TO_M2M_STATUS[0].length) {
                m2mStatus = SONG_STATUS_TO_M2M_STATUS[0][statusValue];
            }
        } else if (statusClass == 3) {
            if (statusValue < SONG_STATUS_TO_M2M_STATUS[1].length) {
                m2mStatus = SONG_STATUS_TO_M2M_STATUS[1][statusValue];
            }
        } else if (statusClass == 4) {
            if (statusValue < SONG_STATUS_TO_M2M_STATUS[2].length) {
                m2mStatus = SONG_STATUS_TO_M2M_STATUS[2][statusValue];
            }
        } else if (statusClass == 5) {
            if (statusValue < SONG_STATUS_TO_M2M_STATUS[3].length) {
                m2mStatus = SONG_STATUS_TO_M2M_STATUS[3][statusValue];
            }
        }
        if (m2mStatus == null) {
            LOG.error("Received an unknown SONG status code: " + status);
            this.statusCode = StatusCode.STATUS_INTERNAL_SERVER_ERROR;
        } else {
            this.statusCode = m2mStatus;
        }
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

    public StatusCode getStatusCode() {
        return statusCode;
    }

    public URI getResourceURI() {
        URI result = null;
        try {
            result = new URI(songResponse.getHeader(SongServletMessage.HD_CONTENT_LOCATION));
        } catch (URISyntaxException e) {
            LOG.error("Invalid URI received in response confirm Content-Location header: "
                    + songResponse.getHeader(SongServletMessage.HD_CONTENT_LOCATION));
        } catch (RuntimeException e) {
            LOG.error("Invalid URI received in response confirm Content-Location header: "
                    + songResponse.getHeader(SongServletMessage.HD_CONTENT_LOCATION));
        }
        return result;
    }

    public XoObject getRepresentation() throws IOException, XoException, M2MException {
        if (representation == null && songResponse.getContentLength() > 0) {
            if (songResponse.getContentType().startsWith(M2MConstants.MT_APPLICATION_XML)
                    || songResponse.getContentType().startsWith(M2MConstants.MT_TEXT_XML)) {
                if (xoService.isXmlSupported()) {
                    representation = xoService.readXmlXmlXoObject(songResponse.getRawContent(),
                            songResponse.getCharacterEncoding());
                }
            } else if (songResponse.getContentType().startsWith(M2MConstants.MT_APPLICATION_EXI)) {
                if (xoService.isExiSupported()) {
                    representation = xoService.readExiXmlXoObject(songResponse.getRawContent(),
                            songResponse.getCharacterEncoding());
                }
            }
            if (representation == null) {
                throw new M2MException("Unsupported media type: " + songResponse.getContentType(),
                        StatusCode.STATUS_UNSUPPORTED_MEDIA_TYPE);
            }
        }
        return representation;
    }

    public int getRawStatusCode() {
        return songResponse.getStatus();
    }

    public String getRawContentType() {
        return songResponse.getContentType();
    }

    public byte[] getRawBytes() throws IOException {
        return songResponse.getRawContent();
    }

    public Request getRequest() {
        return (Request) songResponse.getRequest().getAttribute(M2MContextImpl.AT_REQUEST);
    }

    public String getHeader(String name) {
        return songResponse.getHeader(name);
    }

    public Iterator getHeaderNames() {
        return songResponse.getHeaderNames();
    }

    public ListIterator getHeaders(String name) {
        return songResponse.getHeaders(name);
    }

    public void free() {
        if (representation != null) {
            representation.free(true);
            representation = null;
        }
    }
}
