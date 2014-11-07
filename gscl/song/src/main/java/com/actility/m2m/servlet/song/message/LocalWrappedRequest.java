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
 * id $Id: LocalWrappedRequest.java 9834 2014-10-31 16:26:14Z JReich $
 * author $Author: JReich $
 * version $Revision: 9834 $
 * lastrevision $Date: 2014-10-31 17:26:14 +0100 (Fri, 31 Oct 2014) $
 * modifiedby $LastChangedBy: JReich $
 * lastmodified $LastChangedDate: 2014-10-31 17:26:14 +0100 (Fri, 31 Oct 2014) $
 */

package com.actility.m2m.servlet.song.message;

import java.io.BufferedReader;
import java.net.InetAddress;
import java.util.Enumeration;
import java.util.Locale;
import java.util.Map;

import javax.servlet.ServletInputStream;

import com.actility.m2m.servlet.ApplicationSession;
import com.actility.m2m.servlet.song.Proxy;
import com.actility.m2m.servlet.song.SongServletRequest;
import com.actility.m2m.servlet.song.SongServletResponse;
import com.actility.m2m.servlet.song.SongURI;
import com.actility.m2m.servlet.song.binding.SongBindingFactory;
import com.actility.m2m.servlet.song.internal.BindingNode;
import com.actility.m2m.servlet.song.internal.EndpointNode;

/**
 * LocalWrappedRequest which is a {@link SongServletRequest} built from a LocalRequest or LocalForwardedRequest.
 */
public final class LocalWrappedRequest extends HCReadOnlyRef implements OutgoingRequest {

    private boolean responded;

    /**
     * Builds a LocalWrappedRequest from the given LocalRequest or LocalForwardedRequest.
     *
     * @param node The node that receives the request
     * @param request The message that is wrapped
     * @param poc Point of contact of the remote target. This is used in case the remote target URI is not routable and the real
     *            destination is determined by another mean
     */
    public LocalWrappedRequest(EndpointNode node, InternalMessage request, SongURI poc) {
        super(node, request);
        this.exchange = request.getExchange();
        this.committed = true;
        this.responded = false;
        if (node != null) {
            setAttribute(SongBindingFactory.AT_EXPIRATION_TIME, new Long(((InternalRequest) message).getExpirationTime()));
            setAttribute(SongBindingFactory.AT_POC_URI, poc);
        }
    }

    /**
     * Creates a RemoteResponse to this request.
     *
     * @param statusCode The response status code
     * @param reasonPhrase The response reason phrase
     * @param localAddress The local address on which the response was received
     * @param localPort The local port on which the response was received
     * @param remoteAddress The remote address on which the response was sent
     * @param remotePort The remote port on which the response was sent
     * @return The built RemoteResponse to the current request
     */
    public SongServletResponse createResponse(int statusCode, String reasonPhrase, InetAddress localAddress, int localPort,
            InetAddress remoteAddress, int remotePort) {
        if (!responded) {
            return new RemoteResponse(node, this, statusCode, reasonPhrase, localAddress, localPort, remoteAddress, remotePort);
        }
        throw new IllegalStateException("Request is already responded. Cannot create a new response");
    }

    /**
     * Gets the Wrapped request which is a LocalRequest or LocalForwardedRequest
     *
     * @return The wrapped request
     */
    public InternalMessage getWrappedRequest() {
        return message;
    }

    public BindingNode getBindingNode() {
        return message.getBindingNode();
    }

    public Proxy getProxy() {
        return ((SongServletRequest) message).getProxy();
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

    public ApplicationSession getApplicationSession(boolean create) {
        throw new IllegalStateException(UNSUPPORTED_METHOD);
    }

    public SongServletResponse createResponse(int statuscode) {
        throw new IllegalStateException(UNSUPPORTED_METHOD);
    }

    public SongServletResponse createResponse(int statusCode, String reasonPhrase) {
        throw new IllegalStateException(UNSUPPORTED_METHOD);
    }

    public void send() {
        throw new IllegalStateException(UNSUPPORTED_METHOD);
    }

    public void send(long millis) {
        throw new IllegalStateException(UNSUPPORTED_METHOD);
    }

    public ServletInputStream getInputStream() {
        throw new IllegalStateException(UNSUPPORTED_METHOD);
    }

    public String getPathInfo() {
        throw new IllegalStateException(UNSUPPORTED_METHOD);
    }

    public BufferedReader getReader() {
        throw new IllegalStateException(UNSUPPORTED_METHOD);
    }

    public String getServletPath() {
        throw new IllegalStateException(UNSUPPORTED_METHOD);
    }

    public String getContextPath() {
        throw new IllegalStateException(UNSUPPORTED_METHOD);
    }

    public String getLocalAddr() {
        throw new IllegalStateException(UNSUPPORTED_METHOD);
    }

    public int getLocalPort() {
        throw new IllegalStateException(UNSUPPORTED_METHOD);
    }

    public String getRemoteAddr() {
        throw new IllegalStateException(UNSUPPORTED_METHOD);
    }

    public int getRemotePort() {
        throw new IllegalStateException(UNSUPPORTED_METHOD);
    }

    public String getLocalName() {
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

    public String getRealPath(String path) {
        throw new IllegalStateException(UNSUPPORTED_METHOD);
    }

    public String getRemoteHost() {
        throw new IllegalStateException(UNSUPPORTED_METHOD);
    }

    public String getScheme() {
        throw new IllegalStateException(UNSUPPORTED_METHOD);
    }

    public String getServerName() {
        throw new IllegalStateException(UNSUPPORTED_METHOD);
    }

    public int getServerPort() {
        throw new IllegalStateException(UNSUPPORTED_METHOD);
    }

    public boolean isProxy() {
        throw new IllegalStateException(UNSUPPORTED_METHOD);
    }

    public void send(SongURI poc) {
        throw new IllegalStateException(UNSUPPORTED_METHOD);
    }

    public void send(long millis, SongURI poc) {
        throw new IllegalStateException(UNSUPPORTED_METHOD);
    }
}
