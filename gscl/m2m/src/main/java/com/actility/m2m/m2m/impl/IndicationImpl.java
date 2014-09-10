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
 * id $Id: IndicationImpl.java 8761 2014-05-21 15:31:37Z JReich $
 * author $Author: JReich $
 * version $Revision: 8761 $
 * lastrevision $Date: 2014-05-21 17:31:37 +0200 (Wed, 21 May 2014) $
 * modifiedby $LastChangedBy: JReich $
 * lastmodified $LastChangedDate: 2014-05-21 17:31:37 +0200 (Wed, 21 May 2014) $
 */

package com.actility.m2m.m2m.impl;

import java.io.IOException;
import java.net.URI;
import java.text.ParseException;
import java.util.ArrayList;
import java.util.Date;
import java.util.Iterator;
import java.util.List;
import java.util.ListIterator;
import java.util.Map.Entry;

import org.apache.log4j.Logger;

import com.actility.m2m.m2m.Confirm;
import com.actility.m2m.m2m.ContentInstanceFilterCriteria;
import com.actility.m2m.m2m.FilterCriteria;
import com.actility.m2m.m2m.Indication;
import com.actility.m2m.m2m.M2MConstants;
import com.actility.m2m.m2m.M2MException;
import com.actility.m2m.m2m.M2MUtils;
import com.actility.m2m.m2m.ProxyIndication;
import com.actility.m2m.m2m.Request;
import com.actility.m2m.m2m.Response;
import com.actility.m2m.m2m.StatusCode;
import com.actility.m2m.m2m.log.BundleLogger;
import com.actility.m2m.servlet.song.SongFactory;
import com.actility.m2m.servlet.song.SongServletMessage;
import com.actility.m2m.servlet.song.SongServletRequest;
import com.actility.m2m.servlet.song.SongServletResponse;
import com.actility.m2m.servlet.song.SongURI;
import com.actility.m2m.util.CharacterUtils;
import com.actility.m2m.util.FormatUtils;
import com.actility.m2m.util.log.OSGiLogger;
import com.actility.m2m.xo.XoException;
import com.actility.m2m.xo.XoObject;
import com.actility.m2m.xo.XoService;

public final class IndicationImpl implements ProxyIndication {
    /**
     *
     */
    private static final long serialVersionUID = 6757509873523269770L;

    private static final Logger LOG = OSGiLogger.getLogger(IndicationImpl.class, BundleLogger.getStaticLogger());

    private static final long L_ACCEPT_STOP_1 = CharacterUtils.lowMask(" ,;");
    private static final long H_ACCEPT_STOP_1 = CharacterUtils.highMask(" ,;");

    private static final long L_ACCEPT_STOP_2 = CharacterUtils.lowMask(" =");
    private static final long H_ACCEPT_STOP_2 = CharacterUtils.highMask(" =");

    private final SongFactory songFactory;
    private final SongServletRequest songRequest;
    private final XoService xoService;
    private final M2MUtils m2mUtils;
    private SongURI targetHost;
    private XoObject representation;

    public IndicationImpl(SongFactory songFactory, XoService xoService, M2MUtils m2mUtils, SongServletRequest songRequest) {
        this.songFactory = songFactory;
        this.xoService = xoService;
        this.m2mUtils = m2mUtils;
        this.songRequest = songRequest;
    }

    public SongServletRequest getSongRequest() {
        return songRequest;
    }

    public M2MUtils getM2MUtils() {
        return m2mUtils;
    }

    public String getAcceptedXoMediaType() throws M2MException {
        return m2mUtils.getAcceptedXoMediaType(songRequest.getHeader(SongServletMessage.HD_ACCEPT));
    }

    public String getAcceptedMediaType(String[] availableMediaTypes) throws M2MException {
        return m2mUtils.getAcceptedMediaType(songRequest.getHeader(SongServletMessage.HD_ACCEPT), availableMediaTypes);
    }

    public boolean isCommitted() {
        return songRequest.isCommitted();
    }

    public String getTransactionId() {
        return songRequest.getId();
    }

    public String getMethod() {
        return songRequest.getMethod();
    }

    public String getTargetPath() {
        return songRequest.getPathInfo();
    }

    public URI getTargetID() {
        return songRequest.getTargetID().toURI();
    }

    public URI getRequestingEntity() {
        return (songRequest.getRequestingEntity() != null) ? songRequest.getRequestingEntity().toURI() : null;
    }

    public Object getAttribute(String name) {
        if (name.startsWith(M2MContextImpl.M2M_PREFIX)) {
            return null;
        }
        return songRequest.getAttribute(name);
    }

    public void setAttribute(String name, Object attribute) {
        if (name.startsWith(M2MContextImpl.M2M_PREFIX)) {
            throw new IllegalArgumentException(
                    "m2m: prefix is reserved for M2M Layer internal attributes. Cannot set an attribute with this prefix: "
                            + name);
        }
        songRequest.setAttribute(name, attribute);
    }

    public void removeAttribute(String name) {
        if (name.startsWith(M2MContextImpl.M2M_PREFIX)) {
            throw new IllegalArgumentException(
                    "m2m: prefix is reserved for M2M Layer internal attributes. Cannot remove an attribute with this prefix: "
                            + name);
        }
        songRequest.removeAttribute(name);
    }

    public String getHeader(String name) {
        return songRequest.getHeader(name);
    }

    public Iterator getHeaderNames() {
        return songRequest.getHeaderNames();
    }

    public ListIterator getHeaders(String name) {
        return songRequest.getHeaders(name);
    }

    public String getLocalHost() {
        return songRequest.getLocalName();
    }

    public String getLocalAddr() {
        return songRequest.getLocalAddr();
    }

    public int getLocalPort() {
        return songRequest.getLocalPort();
    }

    public String getRemoteHost() {
        return songRequest.getRemoteHost();
    }

    public String getRemoteAddr() {
        return songRequest.getRemoteAddr();
    }

    public int getRemotePort() {
        return songRequest.getRemotePort();
    }

    public void setProxyHost(String host, int port) {
        songRequest.getProxy().setProxyTo(songFactory.createURI(songRequest.getTargetID().getScheme(), host, port, null));
    }

    public void setTargetHost(URI targetURI) {
        targetHost = songFactory.createURI(targetURI);
    }

    public void send() throws IOException {
        if (targetHost != null) {
            songRequest.send(targetHost);
        } else {
            songRequest.send();
        }
    }

    public void send(long requestTimeout) throws IOException {
        if (targetHost != null) {
            songRequest.send(requestTimeout, targetHost);
        } else {
            songRequest.send(requestTimeout);
        }
    }

    private void buildFilterCriteria(FilterCriteria filterCriteria) throws M2MException {
        SongURI targetID = songRequest.getTargetID();
        List ifNoneMatch = new ArrayList();
        List searchString = new ArrayList();
        Iterator it = targetID.getQueryParameters();
        try {
            Entry queryParameter = null;
            String key = null;
            String value = null;
            while (it.hasNext()) {
                queryParameter = (Entry) it.next();
                key = (String) queryParameter.getKey();
                value = (String) queryParameter.getValue();
                if (M2MConstants.ATTR_IF_MODIFIED_SINCE.equals(key)) {
                    LOG.error("IfModifiedSince: " + value);
                    filterCriteria.setIfModifiedSince(FormatUtils.parseDateTime(value));
                } else if (M2MConstants.ATTR_IF_UNMODIFIED_SINCE.equals(key)) {
                    filterCriteria.setIfUnmodifiedSince(FormatUtils.parseDateTime(value));
                } else if (M2MConstants.ATTR_IF_NONE_MATCH.equals(key)) {
                    ifNoneMatch.add(value);
                } else if (M2MConstants.ATTR_ATTRIBUTE_ACCESSOR.equals(key)) {
                    filterCriteria.setAttributeAccessor(value);
                } else if (M2MConstants.ATTR_SEARCH_STRING.equals(key)) {
                    searchString.add(value);
                } else if (M2MConstants.ATTR_CREATED_AFTER.equals(key)) {
                    filterCriteria.setCreatedAfter(FormatUtils.parseDateTime(value));
                } else if (M2MConstants.ATTR_CREATED_BEFORE.equals(key)) {
                    filterCriteria.setCreatedBefore(FormatUtils.parseDateTime(value));
                }
            }
            String[] listStr = null;
            if (ifNoneMatch.size() > 0) {
                listStr = new String[ifNoneMatch.size()];
                filterCriteria.setIfNoneMatch((String[]) ifNoneMatch.toArray(listStr));
            }
            if (searchString.size() > 0) {
                listStr = new String[searchString.size()];
                filterCriteria.setSearchString((String[]) searchString.toArray(listStr));
            }
        } catch (ParseException e) {
            throw new M2MException("Unable to decode filter criteria from query parameters: " + e.getMessage(),
                    StatusCode.STATUS_BAD_REQUEST, e);
        }
    }

    public FilterCriteria getFilterCriteria() throws M2MException {
        FilterCriteria filterCriteria = null;
        if (SongServletRequest.MD_RETRIEVE.equals(songRequest.getMethod())) {
            filterCriteria = new FilterCriteria();
            buildFilterCriteria(filterCriteria);
        }
        return filterCriteria;
    }

    public ContentInstanceFilterCriteria getContentInstanceFilterCriteria() throws M2MException {
        ContentInstanceFilterCriteria filterCriteria = null;
        if (SongServletRequest.MD_RETRIEVE.equals(songRequest.getMethod())) {
            filterCriteria = new ContentInstanceFilterCriteria();
            buildFilterCriteria(filterCriteria);
            SongURI targetID = songRequest.getTargetID();
            List contentType = new ArrayList();
            Iterator it = targetID.getQueryParameters();
            try {
                Entry queryParameter = null;
                String key = null;
                String value = null;
                while (it.hasNext()) {
                    queryParameter = (Entry) it.next();
                    key = (String) queryParameter.getKey();
                    value = (String) queryParameter.getValue();
                    if (M2MConstants.ATTR_SIZE_FROM.equals(key)) {
                        filterCriteria.setSizeFrom(Integer.valueOf(value));
                    } else if (M2MConstants.ATTR_SIZE_UNTIL.equals(key)) {
                        filterCriteria.setSizeUntil(Integer.valueOf(value));
                    } else if (M2MConstants.ATTR_CONTENT_TYPE.equals(key)) {
                        contentType.add(value);
                    } else if (M2MConstants.ATTR_META_DATA_ONLY.equals(key)) {
                        filterCriteria.setMetaDataOnly(Boolean.valueOf(value));
                    }
                }
                String[] listStr = null;
                if (contentType.size() > 0) {
                    listStr = new String[contentType.size()];
                    filterCriteria.setContentType((String[]) contentType.toArray(listStr));
                }
            } catch (NumberFormatException e) {
                throw new M2MException("Unable to decode filter criteria from query parameters: " + e.getMessage(),
                        StatusCode.STATUS_BAD_REQUEST, e);
            }
        }
        return filterCriteria;
    }

    public Long getMaxSize() throws M2MException {
        String maxSize = songRequest.getTargetID().getQueryParameter(M2MConstants.ATTR_MAX_SIZE);
        try {
            return (maxSize != null) ? Long.valueOf(maxSize) : null;
        } catch (NumberFormatException e) {
            throw new M2MException("maxSize primitive attribute is not an integer: " + maxSize, StatusCode.STATUS_BAD_REQUEST,
                    e);
        }
    }

    public String getSearchPrefix() {
        return songRequest.getTargetID().getQueryParameter(M2MConstants.ATTR_SEARCH_PREFIX);
    }

    public String getQueryParameter(String name) {
        return songRequest.getTargetID().getQueryParameter(name);
    }

    public Iterator getQueryParameters(String name) {
        return songRequest.getTargetID().getQueryParameters(name);
    }

    public XoObject getRepresentation(String parentTag) throws IOException, XoException, M2MException {
        if (representation == null && songRequest.getContentLength() > 0) {
            if (songRequest.getContentType().startsWith(M2MConstants.MT_APPLICATION_XML)
                    || songRequest.getContentType().startsWith(M2MConstants.MT_TEXT_XML)) {
                if (xoService.isXmlSupported()) {
                    if (parentTag == null) {
                        representation = xoService.readXmlXmlXoObject(songRequest.getRawContent(),
                                songRequest.getCharacterEncoding());
                    } else {
                        representation = xoService.readPartialXmlXmlXoObject(songRequest.getRawContent(),
                                songRequest.getCharacterEncoding(), parentTag);
                    }
                }
            } else if (songRequest.getContentType().startsWith(M2MConstants.MT_APPLICATION_EXI)) {
                if (xoService.isExiSupported()) {
                    if (parentTag == null) {
                        representation = xoService.readExiXmlXoObject(songRequest.getRawContent(),
                                songRequest.getCharacterEncoding());
                    } else {
                        representation = xoService.readPartialExiXmlXoObject(songRequest.getRawContent(),
                                songRequest.getCharacterEncoding(), parentTag);
                    }
                }
            }
            if (representation == null) {
                throw new M2MException("Unsupported media type: " + songRequest.getContentType(),
                        StatusCode.STATUS_UNSUPPORTED_MEDIA_TYPE);
            }
        }
        return representation;
    }

    public String getRawContentType() {
        return songRequest.getContentType();
    }

    public String getRawCharacterEncoding() {
        return songRequest.getCharacterEncoding();
    }

    public byte[] getRawBytes() throws IOException {
        return songRequest.getRawContent();
    }

    public boolean checkConditions(String eTag, long lastModified, int conditionAccess) throws IOException {
        boolean result = true;
        if (eTag != null) {
            ListIterator it = songRequest.getHeaders(SongServletMessage.HD_IF_MATCH);
            result = !it.hasNext();
            String value = null;
            while (it.hasNext() && !result) {
                value = (String) it.next();
                if (eTag.equals(value)) {
                    result = true;
                }
            }
            it = songRequest.getHeaders(SongServletMessage.HD_IF_NONE_MATCH);
            while (it.hasNext() && result) {
                value = (String) it.next();
                if (eTag.equals(value)) {
                    result = false;
                }
            }
        }
        if (lastModified != -1) {
            String headerValue = songRequest.getHeader(SongServletMessage.HD_IF_MODIFIED_SINCE);
            Date dateValue = null;
            if (headerValue != null) {
                try {
                    dateValue = FormatUtils.parseHttpDate(headerValue);
                    result = result && (lastModified > dateValue.getTime());
                } catch (ParseException e) {
                    LOG.error("Received a SONG request with an invalid If-Modified-Since header: " + headerValue);
                }
            }
            headerValue = songRequest.getHeader(SongServletMessage.HD_IF_UNMODIFIED_SINCE);
            if (headerValue != null) {
                try {
                    dateValue = FormatUtils.parseHttpDate(headerValue);
                    result = result && (lastModified <= dateValue.getTime());
                } catch (ParseException e) {
                    LOG.error("Received a SONG request with an invalid If-Unmodified-Since header: " + headerValue);
                }
            }
        }
        if (!result) {
            if (conditionAccess == Indication.CONDITION_READ) {
                songRequest.createResponse(SongServletResponse.SC_NOT_MODIFIED).send();
            } else {
                songRequest.createResponse(SongServletResponse.SC_PRECONDITION_FAILED).send();
            }
        }
        return result;
    }

    public Response createSuccessResponse(StatusCode statusCode) {
        return new ResponseImpl(statusCode, true, this);
    }

    public Response createUnsuccessResponse(StatusCode statusCode) {
        return new ResponseImpl(statusCode, false, this);
    }

    public Request createRequest(URI requestingEntity, URI targetID) throws IOException {
        Request request = new RequestImpl(songFactory, songFactory.createRequestFrom(songRequest, requestingEntity, targetID));
        ((RequestImpl) request).getSongRequest().setAttribute(M2MContextImpl.AT_INDICATION, this);
        return request;
    }

    public Response createResponseFromConfirm(Confirm confirm) throws IOException {
        return createResponseFromConfirm(confirm, -1);
    }

    public Response createResponseFromConfirm(Confirm confirm, int rawStatusCode) throws IOException {
        if (confirm.getClass() == ConfirmImpl.class) {
            ConfirmImpl realConfirm = (ConfirmImpl) confirm;
            SongServletResponse inSongResponse = realConfirm.getSongResponse();
            IndicationImpl indication = (IndicationImpl) inSongResponse.getRequest().getAttribute(M2MContextImpl.AT_INDICATION);
            SongServletResponse outSongResponse = null;
            if (rawStatusCode != -1) {
                outSongResponse = songFactory.createResponseFrom(songRequest, inSongResponse, rawStatusCode);
            } else {
                outSongResponse = songFactory.createResponseFrom(songRequest, inSongResponse, inSongResponse.getStatus(),
                        inSongResponse.getReasonPhrase());
            }
            return new ResponseImpl(indication, outSongResponse);
        }
        return null;
    }

    public void free() {
        if (representation != null) {
            representation.free(true);
            representation = null;
        }
    }
}
