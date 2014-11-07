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
 * id $Id: HCCopyOnWrite.java 8766 2014-05-21 15:41:17Z JReich $
 * author $Author: JReich $
 * version $Revision: 8766 $
 * lastrevision $Date: 2014-05-21 17:41:17 +0200 (Wed, 21 May 2014) $
 * modifiedby $LastChangedBy: JReich $
 * lastmodified $LastChangedDate: 2014-05-21 17:41:17 +0200 (Wed, 21 May 2014) $
 */

package com.actility.m2m.servlet.song.message;

import java.io.IOException;
import java.io.UnsupportedEncodingException;
import java.util.ArrayList;
import java.util.Iterator;
import java.util.List;
import java.util.ListIterator;

import com.actility.m2m.servlet.song.Parameterable;
import com.actility.m2m.servlet.song.ServletParseException;
import com.actility.m2m.servlet.song.SongServletMessage;
import com.actility.m2m.servlet.song.internal.EndpointNode;
import com.actility.m2m.util.EmptyUtils;
import com.actility.m2m.util.MultiMap;

/**
 * HC stands for the tuple Headers and Content.
 * <p>
 * This class implements the management of headers and content coming from another HC. Any modification of the HC triggers the
 * copy of the original one to not alter it
 *
 */
public abstract class HCCopyOnWrite extends InternalMessage {
    protected final InternalMessage message;
    protected MultiMap headers;
    protected String contentType;
    protected String characterEncoding;
    protected Object content;
    protected byte[] rawContent;
    protected int contentLength;

    protected HCCopyOnWrite(EndpointNode node, InternalMessage message) {
        super(node);
        this.message = message;
        this.contentType = message.getContentTypeInternal();
        this.characterEncoding = message.getCharacterEncoding();
        this.contentLength = message.getContentLength();
        this.content = message.getContentInternal();
        this.rawContent = message.getRawContentInternal();
    }

    protected final void copyHeaders() {
        headers = new MultiMap();
        final Iterator it = message.getHeaderNames();
        String headerName = null;
        ListIterator lit = null;
        String headerValue = null;
        while (it.hasNext()) {
            headerName = (String) it.next();
            lit = message.getHeaders(headerName);
            while (lit.hasNext()) {
                headerValue = (String) lit.next();
                headers.put(headerName, headerValue);
            }
        }
    }

    public final String getId() {
        return message.getId();
    }

    private final void setHeaderInternal(String name, String value) {
        headers.removeAndPut(name, value);
    }

    public final MultiMap getHeadersInternal() {
        return (headers != null) ? headers : message.getHeadersInternal();
    }

    public final String getContentTypeInternal() {
        return contentType;
    }

    public final Object getContentInternal() {
        return content;
    }

    public final byte[] getRawContentInternal() {
        return rawContent;
    }

    public final void addHeader(String name, String value) {
        checkCommitted();
        if (name == null) {
            throw new IllegalArgumentException("Cannot set a header from a null name");
        }
        if (isSystemHeader(name)) {
            throw new IllegalArgumentException("Cannot modify the system header: " + name);
        }
        if (headers == null) {
            copyHeaders();
        }
        if (name.equals(SongServletMessage.HD_CONTENT_TYPE)) {
            setContentType(value);
        } else {
            headers.put(name, value);
        }
    }

    public final String getHeader(String name) {
        if (name == null) {
            throw new IllegalArgumentException("Cannot get a header from a null name");
        }
        if (headers != null) {
            return (String) headers.get(name);
        }
        return message.getHeader(name);
    }

    public final Iterator getHeaderNames() {
        if (headers != null) {
            return headers.keySet().iterator();
        }
        return message.getHeaderNames();
    }

    public final ListIterator getHeaders(String name) {
        if (name == null) {
            throw new IllegalArgumentException("Cannot get headers from a null name");
        }
        if (headers != null) {
            List values = headers.getAll(name);
            return (values != null) ? values.listIterator() : EmptyUtils.EMPTY_LIST_ITERATOR;
        }
        return message.getHeaders(name);
    }

    public final void removeHeader(String name) {
        checkCommitted();
        if (name == null) {
            throw new IllegalArgumentException("Cannot remove a header from a null name");
        }
        if (isSystemHeader(name)) {
            throw new IllegalArgumentException("Cannot modify the system header: " + name);
        }
        if (headers == null) {
            copyHeaders();
        }
        headers.removeAll(name);
    }

    public final void setHeader(String name, String value) {
        checkCommitted();
        if (name == null) {
            throw new IllegalArgumentException("Cannot set a header from a null name");
        }
        if (isSystemHeader(name)) {
            throw new IllegalArgumentException("Cannot modify the system header: " + name);
        }
        if (headers == null) {
            copyHeaders();
        }
        setHeaderInternal(name, value);
    }

    public final void addParameterableHeader(String name, Parameterable param, boolean first) {
        checkCommitted();
        if (headers == null) {
            copyHeaders();
        }
        if (first) {
            headers.putFirst(name, param.toString());
        } else {
            headers.putLast(name, param.toString());
        }
    }

    public final Parameterable getParameterableHeader(String name) throws ServletParseException {
        if (name == null) {
            throw new IllegalArgumentException("Cannot get a Parametrable header with a null name");
        }
        if (!isParameterableHeader(name)) {
            throw new ServletParseException(name + " header is not parameterable");
        }
        if (headers != null) {
            String header = getHeader(name);
            return (header != null) ? createParameterable(name, header) : null;
        }
        return message.getParameterableHeader(name);
    }

    public final ListIterator getParameterableHeaders(String name) throws ServletParseException {
        if (headers != null) {
            List values = headers.getAll(name);
            if (values == null) {
                return EmptyUtils.EMPTY_LIST_ITERATOR;
            }
            List parameterables = new ArrayList();
            Iterator it = values.iterator();
            String value = null;
            while (it.hasNext()) {
                value = (String) it.next();
                parameterables.add(createParameterable(name, value));
            }
            return parameterables.listIterator();
        }
        return message.getParameterableHeaders(name);
    }

    public final void setParameterableHeader(String name, Parameterable param) {
        checkCommitted();
        if (headers == null) {
            copyHeaders();
        }
        setHeaderInternal(name, param.toString());
    }

    public final String getCharacterEncoding() {
        return characterEncoding;
    }

    public final Object getContent() throws IOException, UnsupportedEncodingException {
        if (content == null) {
            if (rawContent != null) {
                // Build content from rawContent
                if (contentType != null && (contentType.startsWith("text/") || contentType.startsWith("application/xml"))) {
                    content = new String(rawContent, characterEncoding);
                } else {
                    content = rawContent;
                }
            } else {
                content = message.getContent();
            }
        }
        return content;
    }

    public final int getContentLength() {
        return contentLength;
    }

    public final String getContentType() {
        return getHeader(SongServletMessage.HD_CONTENT_TYPE);
    }

    public final byte[] getRawContent() {
        return rawContent;
    }

    public void setCharacterEncoding(String enc) throws UnsupportedEncodingException {
        checkCommitted();
        checkEncoding(enc);
        this.characterEncoding = enc;
    }

    public final void setContent(Object content, String contentType) throws UnsupportedEncodingException {
        checkCommitted();
        if (content == null) {
            throw new IllegalArgumentException("Cannot set a null content");
        }
        if (contentType == null) {
            throw new IllegalArgumentException("Cannot set a content with a null content-type");
        }
        try {
            Parameterable contentTypeParameterable = createParameterable(SongServletMessage.HD_CONTENT_TYPE, contentType);
            String encoding = contentTypeParameterable.getParameter("charset");
            if (encoding == null) {
                encoding = this.characterEncoding;
            } else {
                checkEncoding(encoding);
            }
            // Build rawContent
            if (byte[].class.equals(content.getClass())) {
                this.rawContent = (byte[]) content;
            } else if (String.class.equals(content.getClass())) {
                this.rawContent = ((String) content).getBytes(characterEncoding);
                this.content = content;
            } else if (contentTypeParameterable.getValue().startsWith("text/")) {
                String string = content.toString();
                this.rawContent = string.getBytes(characterEncoding);
                this.content = string;
            } else {
                throw new IllegalArgumentException("Cannot serialize the given content " + content.toString()
                        + " with the given content-type " + contentType);
            }
            this.characterEncoding = encoding;
            this.contentType = contentTypeParameterable.getValue();
            this.contentLength = rawContent.length;
            setHeaderInternal(SongServletMessage.HD_CONTENT_TYPE, contentType);
            setHeaderInternal(SongServletMessage.HD_CONTENT_LENGTH, Integer.toString(contentLength));
        } catch (ServletParseException e1) {
            throw new IllegalArgumentException(e1.getMessage());
        }
    }

    public final void setContentLength(int len) {
        checkCommitted();
        if (len < 0) {
            throw new IllegalArgumentException("Cannot set a negative content length: " + len);
        }
        this.contentLength = len;
        setHeaderInternal(SongServletMessage.HD_CONTENT_LENGTH, Integer.toString(contentLength));
    }

    public final void setContentType(String type) {
        checkCommitted();
        if (type == null) {
            throw new IllegalArgumentException("Cannot set a null content type");
        }
        try {
            Parameterable contentTypeParameterable = createParameterable(SongServletMessage.HD_CONTENT_TYPE, type);
            String encoding = contentTypeParameterable.getParameter("charset");
            if (encoding != null) {
                setCharacterEncoding(encoding);
            }
            this.contentType = contentTypeParameterable.getValue();
            setHeaderInternal(SongServletMessage.HD_CONTENT_TYPE, contentType);
        } catch (ServletParseException e1) {
            throw new IllegalArgumentException(e1.getMessage());
        } catch (UnsupportedEncodingException e1) {
            throw new IllegalArgumentException(e1.getMessage());
        }
    }
}
