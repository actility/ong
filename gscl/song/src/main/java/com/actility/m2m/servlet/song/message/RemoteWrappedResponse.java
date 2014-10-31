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
 * id $Id: RemoteWrappedResponse.java 8766 2014-05-21 15:41:17Z JReich $
 * author $Author: JReich $
 * version $Revision: 8766 $
 * lastrevision $Date: 2014-05-21 17:41:17 +0200 (Wed, 21 May 2014) $
 * modifiedby $LastChangedBy: JReich $
 * lastmodified $LastChangedDate: 2014-05-21 17:41:17 +0200 (Wed, 21 May 2014) $
 */

package com.actility.m2m.servlet.song.message;

import java.io.IOException;
import java.io.PrintWriter;
import java.io.UnsupportedEncodingException;
import java.util.Locale;

import javax.servlet.ServletOutputStream;

import org.apache.log4j.Logger;

import com.actility.m2m.servlet.ApplicationSession;
import com.actility.m2m.servlet.log.BundleLogger;
import com.actility.m2m.servlet.song.SongServletRequest;
import com.actility.m2m.servlet.song.SongServletResponse;
import com.actility.m2m.servlet.song.SongURI;
import com.actility.m2m.servlet.song.internal.BindingNode;
import com.actility.m2m.servlet.song.internal.EndpointNode;
import com.actility.m2m.util.log.OSGiLogger;

/**
 * RemoteWrappedResponse which is a {@link SongServletResponse} built from a RemoteResponse.
 */
public final class RemoteWrappedResponse extends HCCopyOnWrite implements SongServletResponse {
    private static final Logger LOG = OSGiLogger.getLogger(RemoteWrappedResponse.class, BundleLogger.getStaticLogger());

    private final SongServletRequest request;
    private int status;
    private String reasonPhrase;

    /**
     * Builds a RemoteWrappedResponse from the given RemoteResponse.
     *
     * @param node The node that receives the request
     * @param request The request corresponding to this response
     * @param response The response that is wrapped
     */
    public RemoteWrappedResponse(EndpointNode node, InternalMessage request, InternalMessage response) {
        super(node, response);
        this.request = (SongServletRequest) request;
        this.status = ((SongServletResponse) response).getStatus();
        this.reasonPhrase = ((SongServletResponse) response).getReasonPhrase();
        if (LocalForwardedRequest.class != request.getClass()) {
            this.committed = true;
            checkProtocolSession();
        }
    }

    public BindingNode getBindingNode() {
        return message.getBindingNode();
    }

    public ApplicationSession getApplicationSession(boolean create) {
        return request.getApplicationSession(create);
    }

    public void setCharacterEncoding(String enc) {
        try {
            super.setCharacterEncoding(enc);
        } catch (UnsupportedEncodingException e) {
            LOG.error("Set unknown encoding " + enc + " keep the previously defined one " + message.getCharacterEncoding(), e);
        }
    }

    public SongServletRequest getRequest() {
        return request;
    }

    public int getStatus() {
        return status;
    }

    public String getReasonPhrase() {
        return reasonPhrase;
    }

    public void setStatus(int statusCode) {
        checkCommitted();
        this.status = statusCode;
        this.reasonPhrase = getReasonPhrase(statusCode);
    }

    public String getLocalAddr() {
        return ((SongServletResponse) message).getLocalAddr();
    }

    public int getLocalPort() {
        return ((SongServletResponse) message).getLocalPort();
    }

    public String getRemoteAddr() {
        return ((SongServletResponse) message).getRemoteAddr();
    }

    public int getRemotePort() {
        return ((SongServletResponse) message).getRemotePort();
    }

    public String getMethod() {
        return message.getMethod();
    }

    public String getProtocol() {
        return message.getProtocol();
    }

    public String getTransport() {
        return message.getTransport();
    }

    public boolean isSecure() {
        return message.isSecure();
    }

    public SongURI getRequestingEntity() {
        return message.getRequestingEntity();
    }

    public SongURI getTargetID() {
        return message.getTargetID();
    }

    public void send() throws IOException {
        checkCommitted();
        this.committed = true;
        checkProtocolSession();
        LocalForwardedRequest realRequest = (LocalForwardedRequest) request;
        InternalMessage forwardedRequest = realRequest.getForwardedRequest();
        LocalForwardedResponse localResponse = new LocalForwardedResponse(forwardedRequest.getNode(), forwardedRequest, this);
        localResponse.send();
    }

    public void flushBuffer() {
        throw new IllegalStateException(UNSUPPORTED_METHOD);
    }

    public int getBufferSize() {
        throw new IllegalStateException(UNSUPPORTED_METHOD);
    }

    public Locale getLocale() {
        throw new IllegalStateException(UNSUPPORTED_METHOD);
    }

    public ServletOutputStream getOutputStream() {
        throw new IllegalStateException(UNSUPPORTED_METHOD);
    }

    public PrintWriter getWriter() {
        throw new IllegalStateException(UNSUPPORTED_METHOD);
    }

    public void reset() {
        throw new IllegalStateException(UNSUPPORTED_METHOD);
    }

    public void resetBuffer() {
        throw new IllegalStateException(UNSUPPORTED_METHOD);
    }

    public void setBufferSize(int size) {
        throw new IllegalStateException(UNSUPPORTED_METHOD);
    }

    public void setLocale(Locale loc) {
        throw new IllegalStateException(UNSUPPORTED_METHOD);
    }
}
