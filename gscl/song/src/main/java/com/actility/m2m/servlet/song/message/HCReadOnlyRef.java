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
 * id $Id: HCReadOnlyRef.java 6085 2013-10-15 14:08:29Z mlouiset $
 * author $Author: mlouiset $
 * version $Revision: 6085 $
 * lastrevision $Date: 2013-10-15 16:08:29 +0200 (Tue, 15 Oct 2013) $
 * modifiedby $LastChangedBy: mlouiset $
 * lastmodified $LastChangedDate: 2013-10-15 16:08:29 +0200 (Tue, 15 Oct 2013) $
 */

package com.actility.m2m.servlet.song.message;

import java.io.IOException;
import java.io.UnsupportedEncodingException;
import java.util.Iterator;
import java.util.ListIterator;

import com.actility.m2m.servlet.song.Parameterable;
import com.actility.m2m.servlet.song.ServletParseException;
import com.actility.m2m.servlet.song.SongURI;
import com.actility.m2m.servlet.song.internal.EndpointNode;
import com.actility.m2m.util.MultiMap;

/**
 * HC stands for the tuple Headers and Content.
 * <p>
 * This class implements the management of headers and content coming from another HC. No modification can be perform on it
 *
 */
public abstract class HCReadOnlyRef extends InternalMessage {
    protected final InternalMessage message;

    protected HCReadOnlyRef(EndpointNode node, InternalMessage message) {
        super(node);
        this.message = message;
    }

    public final String getId() {
        return message.getId();
    }

    public final MultiMap getHeadersInternal() {
        return message.getHeadersInternal();
    }

    public final String getContentTypeInternal() {
        return message.getContentTypeInternal();
    }

    public final Object getContentInternal() {
        return message.getContentInternal();
    }

    public final byte[] getRawContentInternal() {
        return message.getRawContentInternal();
    }

    public final void addHeader(String name, String value) {
        throw new IllegalStateException(UNSUPPORTED_METHOD);
    }

    public final void addParameterableHeader(String name, Parameterable param, boolean first) {
        throw new IllegalStateException(UNSUPPORTED_METHOD);
    }

    public final String getHeader(String name) {
        return message.getHeader(name);
    }

    public final Iterator getHeaderNames() {
        return message.getHeaderNames();
    }

    public final ListIterator getHeaders(String name) {
        if (name == null) {
            throw new IllegalArgumentException("Cannot get headers from a null name");
        }
        return message.getHeaders(name);
    }

    public final Parameterable getParameterableHeader(String name) throws ServletParseException {
        if (name == null) {
            throw new IllegalArgumentException("Cannot get a Parametrable header with a null name");
        }
        return message.getParameterableHeader(name);
    }

    public final ListIterator getParameterableHeaders(String name) throws ServletParseException {
        return message.getParameterableHeaders(name);
    }

    public final void removeHeader(String name) {
        checkCommitted();
        message.removeHeader(name);
    }

    public final void setHeader(String name, String value) {
        checkCommitted();
        message.setHeader(name, value);
    }

    public final void setParameterableHeader(String name, Parameterable param) {
        checkCommitted();
        message.setParameterableHeader(name, param);
    }

    public final String getCharacterEncoding() {
        return message.getCharacterEncoding();
    }

    public final Object getContent() throws IOException {
        return message.getContent();
    }

    public final int getContentLength() {
        return message.getContentLength();
    }

    public final String getContentType() {
        return message.getContentType();
    }

    public final byte[] getRawContent() throws IOException {
        return message.getRawContent();
    }

    public void setCharacterEncoding(String enc) throws UnsupportedEncodingException {
        checkCommitted();
        message.setCharacterEncoding(enc);
    }

    public final void setContent(Object content, String contentType) throws UnsupportedEncodingException {
        checkCommitted();
        message.setContent(content, contentType);
    }

    public final void setContentLength(int len) {
        checkCommitted();
        message.setContentLength(len);
    }

    public final void setContentType(String type) {
        checkCommitted();
        message.setContentType(type);
    }

    public final SongURI getRequestingEntity() {
        return message.getRequestingEntity();
    }

    public final SongURI getTargetID() {
        return message.getTargetID();
    }

    public final String getMethod() {
        return message.getMethod();
    }

    public final String getProtocol() {
        return message.getProtocol();
    }

    public final boolean isSecure() {
        return message.isSecure();
    }

    public final String getTransport() {
        // Not applicable
        return null;
    }
}
