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
 * id $Id: RequestImpl.java 7362 2014-01-24 11:30:51Z JReich $
 * author $Author: JReich $
 * version $Revision: 7362 $
 * lastrevision $Date: 2014-01-24 12:30:51 +0100 (Fri, 24 Jan 2014) $
 * modifiedby $LastChangedBy: JReich $
 * lastmodified $LastChangedDate: 2014-01-24 12:30:51 +0100 (Fri, 24 Jan 2014) $
 */

package com.actility.m2m.m2m.impl;

import java.io.IOException;
import java.net.URI;

import com.actility.m2m.m2m.M2MConstants;
import com.actility.m2m.m2m.Request;
import com.actility.m2m.servlet.song.SongFactory;
import com.actility.m2m.servlet.song.SongServletRequest;
import com.actility.m2m.servlet.song.SongURI;
import com.actility.m2m.xo.XoException;
import com.actility.m2m.xo.XoObject;

public final class RequestImpl implements Request {

    /**
     *
     */
    private static final long serialVersionUID = 3576735298612632157L;
    private final SongFactory songFactory;
    private final SongServletRequest songRequest;
    private SongURI targetHost;

    public RequestImpl(SongFactory songFactory, SongServletRequest songRequest) {
        this.songFactory = songFactory;
        this.songRequest = songRequest;
    }

    public SongServletRequest getSongRequest() {
        return songRequest;
    }

    public boolean isCommitted() {
        return songRequest.isCommitted();
    }

    public String getTransactionId() {
        return songRequest.getId();
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

    public void setRepresentation(XoObject representation, String mediaType) throws XoException, IOException {
        if (M2MConstants.MT_APPLICATION_EXI.equals(mediaType)) {
            songRequest.setContent(representation.saveExi(), M2MConstants.MT_APPLICATION_EXI);
        } else {
            songRequest.setContent(representation.saveXml(), M2MConstants.CT_APPLICATION_XML_UTF8);
        }
    }

    public void setRawContent(byte[] content, String contentType) throws IOException {
        songRequest.setContent(content, contentType);
    }

    public void setProxyHost(String host, int port) {
        songRequest.getProxy().setProxyTo(songFactory.createURI(songRequest.getTargetID().getScheme(), host, port, null));
    }

    public void setTargetHost(URI targetURI) {
        targetHost = songFactory.createURI(targetURI);
    }

    public void setTargetHost(String host, int port) {
        targetHost = songFactory.createURI(songRequest.getTargetID().getScheme(), host, port, null);
    }

    public void setTargetHost(String host, int port, String path) {
        targetHost = songFactory.createURI(songRequest.getTargetID().getScheme(), host, port, path);
    }

    public void addHeader(String name, String value) {
        songRequest.addHeader(name, value);
    }

    public void setHeader(String name, String value) {
        songRequest.setHeader(name, value);
    }

    public void setMaxSize(Long maxSize) {
        songRequest.getTargetID().addQueryParameter(M2MConstants.ATTR_MAX_SIZE, String.valueOf(maxSize));
    }

    public void setSearchPrefix(String searchPrefix) {
        songRequest.getTargetID().setQueryParameter(M2MConstants.ATTR_SEARCH_PREFIX, searchPrefix);
    }

    public void setQueryParameter(String name, String value) {
        songRequest.getTargetID().setQueryParameter(name, value);
    }

    public void addQueryParameter(String name, String value) {
        songRequest.getTargetID().addQueryParameter(name, value);
    }

    public void send() throws IOException {
        songRequest.setAttribute(M2MContextImpl.AT_REQUEST, this);
        if (targetHost != null) {
            songRequest.send(targetHost);
        } else {
            songRequest.send();
        }
    }

    public void send(long requestTimeout) throws IOException {
        songRequest.setAttribute(M2MContextImpl.AT_REQUEST, this);
        if (targetHost != null) {
            songRequest.send(requestTimeout, targetHost);
        } else {
            songRequest.send(requestTimeout);
        }
    }

}
