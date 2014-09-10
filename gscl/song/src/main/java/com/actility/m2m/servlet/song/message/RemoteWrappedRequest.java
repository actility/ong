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
 * id $Id: RemoteWrappedRequest.java 6085 2013-10-15 14:08:29Z mlouiset $
 * author $Author: mlouiset $
 * version $Revision: 6085 $
 * lastrevision $Date: 2013-10-15 16:08:29 +0200 (Tue, 15 Oct 2013) $
 * modifiedby $LastChangedBy: mlouiset $
 * lastmodified $LastChangedDate: 2013-10-15 16:08:29 +0200 (Tue, 15 Oct 2013) $
 */

package com.actility.m2m.servlet.song.message;

import java.io.BufferedReader;
import java.io.IOException;
import java.util.Enumeration;
import java.util.Locale;
import java.util.Map;

import javax.servlet.ServletInputStream;

import com.actility.m2m.servlet.ApplicationSession;
import com.actility.m2m.servlet.ext.ExtApplicationSession;
import com.actility.m2m.servlet.song.Proxy;
import com.actility.m2m.servlet.song.SongServletRequest;
import com.actility.m2m.servlet.song.SongServletResponse;
import com.actility.m2m.servlet.song.SongURI;
import com.actility.m2m.servlet.song.internal.BindingNode;
import com.actility.m2m.servlet.song.internal.EndpointNode;

/**
 * RemoteWrappedRequest which is a {@link SongServletRequest} built from a RemoteRequest.
 */
public final class RemoteWrappedRequest extends HCCopyOnWrite implements OutgoingRequest, Proxy {
    private ExtApplicationSession appSession;
    private final String contextPath;
    private final String servletPath;
    private final String pathInfo;
    private boolean supervised;
    private SongURI proxyTo;
    private boolean responded;

    /**
     * Builds a RemoteWrappedRequest from the given RemoteRequest.
     *
     * @param node The node that receives the request
     * @param request The message that is wrapped
     * @param appSession The ApplicationSession associated with the request if any
     * @param contextPath The context path of the application that receives the request
     * @param servletPath The path registered by a Service Servlet that was used to route this request
     * @param pathInfo Extra path info after servletPath in the request targetID
     */
    public RemoteWrappedRequest(EndpointNode node, InternalMessage request, ExtApplicationSession appSession,
            String contextPath, String servletPath, String pathInfo) {
        super(node, request);
        this.appSession = appSession;
        this.exchange = request.getExchange();
        this.contextPath = contextPath;
        this.servletPath = servletPath;
        this.pathInfo = pathInfo;
    }

    /**
     * Gets the Wrapped request which is a RemoteRequest
     *
     * @return The wrapped request
     */
    public InternalMessage getWrappedRequest() {
        return message;
    }

    public BindingNode getBindingNode() {
        return message.getBindingNode();
    }

    public ApplicationSession getApplicationSession(boolean create) {
        if (appSession == null && create) {
            appSession = node.getServletContext().createApplicationSession();
        }
        return appSession;
    }

    public SongServletResponse createResponse(int statuscode) {
        return createResponse(statuscode, getReasonPhrase(statuscode), null, false);
    }

    public SongServletResponse createResponse(int statuscode, boolean force) {
        return createResponse(statuscode, getReasonPhrase(statuscode), null, force);
    }

    public SongServletResponse createResponse(int statusCode, String reasonPhrase) {
        return createResponse(statusCode, reasonPhrase, null, false);
    }

    public SongServletResponse createResponse(int statusCode, String reasonPhrase, InternalMessage internal, boolean force) {
        if (!force) {
            checkCommitted();
        }
        if (!isValidStatusCode(statusCode)) {
            throw new IllegalArgumentException("Unknown status code: " + statusCode);
        }
        committed = true;
        return new LocalResponse(node, this, statusCode, reasonPhrase, internal);
    }

    public String getPathInfo() {
        return pathInfo;
    }

    public String getServletPath() {
        return servletPath;
    }

    public String getContextPath() {
        return contextPath;
    }

    public boolean isProxy() {
        return ((SongServletRequest) message).isProxy();
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
        send(REQUEST_TIMEOUT_DURATION, null);
    }

    public void send(long millis) throws IOException {
        send(millis, null);
    }

    public void send(SongURI poc) throws IOException {
        send(REQUEST_TIMEOUT_DURATION, poc);
    }

    public void send(long millis, SongURI poc) throws IOException {
        checkCommitted();
        committed = true;
        LocalForwardedRequest localRequest = new LocalForwardedRequest(node, this, this);
        localRequest.send(millis, poc);
    }

    public Proxy getProxy() {
        return this;
    }

    public long getSentTime() {
        return ((InternalRequest) message).getSentTime();
    }

    public long getExpirationTime() {
        return ((InternalRequest) message).getExpirationTime();
    }

    public boolean isResponded() {
        return responded;
    }

    public void setResponded(boolean responded) {
        this.responded = responded;
    }

    public String getLocalName() {
        return ((SongServletRequest) message).getLocalName();
    }

    public String getLocalAddr() {
        return ((SongServletRequest) message).getLocalAddr();
    }

    public int getLocalPort() {
        return ((SongServletRequest) message).getLocalPort();
    }

    public String getRemoteHost() {
        return ((SongServletRequest) message).getRemoteHost();
    }

    public String getRemoteAddr() {
        return ((SongServletRequest) message).getRemoteAddr();
    }

    public int getRemotePort() {
        return ((SongServletRequest) message).getRemotePort();
    }

    public String getScheme() {
        return message.getTargetID().getScheme();
    }

    public String getServerName() {
        return message.getTargetID().getHost();
    }

    public int getServerPort() {
        return message.getTargetID().getPort();
    }

    public boolean getSupervised() {
        return supervised;
    }

    public void setSupervised(boolean supervised) {
        checkCommitted();
        this.supervised = supervised;
    }

    public SongURI getProxyTo() {
        return proxyTo;
    }

    public void setProxyTo(SongURI proxy) {
        checkCommitted();
        this.proxyTo = proxy;
    }

    public ServletInputStream getInputStream() {
        throw new IllegalStateException(UNSUPPORTED_METHOD);
    }

    public Locale getLocale() {
        throw new IllegalStateException(UNSUPPORTED_METHOD);
    }

    public Enumeration getLocales() {
        throw new IllegalStateException(UNSUPPORTED_METHOD);
    }

    public String getParameter(String name) {
        throw new IllegalStateException(UNSUPPORTED_METHOD);
    }

    public Map getParameterMap() {
        throw new IllegalStateException(UNSUPPORTED_METHOD);
    }

    public Enumeration getParameterNames() {
        throw new IllegalStateException(UNSUPPORTED_METHOD);
    }

    public String[] getParameterValues(String name) {
        throw new IllegalStateException(UNSUPPORTED_METHOD);
    }

    public BufferedReader getReader() {
        throw new IllegalStateException(UNSUPPORTED_METHOD);
    }

    public String getRealPath(String path) {
        throw new IllegalStateException(UNSUPPORTED_METHOD);
    }
}
