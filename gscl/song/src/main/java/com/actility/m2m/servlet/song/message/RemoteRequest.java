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
 * id $Id: RemoteRequest.java 8767 2014-05-21 15:41:33Z JReich $
 * author $Author: JReich $
 * version $Revision: 8767 $
 * lastrevision $Date: 2014-05-21 17:41:33 +0200 (Wed, 21 May 2014) $
 * modifiedby $LastChangedBy: JReich $
 * lastmodified $LastChangedDate: 2014-05-21 17:41:33 +0200 (Wed, 21 May 2014) $
 */

package com.actility.m2m.servlet.song.message;

import java.io.BufferedReader;
import java.io.IOException;
import java.net.InetAddress;
import java.util.Enumeration;
import java.util.Locale;
import java.util.Map;

import javax.servlet.ServletInputStream;

import org.apache.log4j.Logger;

import com.actility.m2m.be.messaging.BackendMessage;
import com.actility.m2m.be.messaging.DeliveryChannel;
import com.actility.m2m.be.messaging.MessagingException;
import com.actility.m2m.m2m.StatusCode;
import com.actility.m2m.servlet.ApplicationSession;
import com.actility.m2m.servlet.log.BundleLogger;
import com.actility.m2m.servlet.song.Proxy;
import com.actility.m2m.servlet.song.SongServletRequest;
import com.actility.m2m.servlet.song.SongServletResponse;
import com.actility.m2m.servlet.song.SongURI;
import com.actility.m2m.servlet.song.internal.BindingNode;
import com.actility.m2m.servlet.song.internal.EndpointNode;
import com.actility.m2m.util.Profiler;
import com.actility.m2m.util.log.OSGiLogger;

/**
 * RemoteRequest which is a {@link SongServletRequest} initiated by a binding servlet.
 */
public final class RemoteRequest extends HC implements InternalRequest {
    private static final Logger LOG = OSGiLogger.getLogger(RemoteRequest.class, BundleLogger.getStaticLogger());

    private final InetAddress localAddress;
    private final int localPort;
    private final InetAddress remoteAddress;
    private final int remotePort;
    private final boolean proxy;
    private final SongURI requestingEntity;
    private final SongURI targetID;
    private final String method;
    private final String protocol;
    private long sendTime;
    private long expirationTime;

    /**
     * Builds a RemoteRequest which is a request initiated by a binding servlet.
     *
     * @param node The node that creates the request
     * @param method The request method (CREATE, RETRIEVE, UPDATE, DELETE)
     * @param protocol The request protocol and version (HTTP/1.1, SONG/1.0)
     * @param requestingEntity The requestingEntity which defines the request issuer
     * @param targetID The targetID which defines the request destination
     * @param localAddress The local address on which the request was received
     * @param localPort The local port on which the request was received
     * @param remoteAddress The remote address from which the request was sent
     * @param remotePort The remote port from which the request was sent
     * @param proxy Whether the server is acting as a proxy for this request
     * @param id The ID associated to the SONG transaction
     */
    public RemoteRequest(EndpointNode node, String method, String protocol, SongURI requestingEntity, SongURI targetID,
            InetAddress localAddress, int localPort, InetAddress remoteAddress, int remotePort, boolean proxy, String id) {
        super(node, null, id);
        this.method = method;
        this.protocol = protocol;
        this.requestingEntity = requestingEntity;
        this.targetID = targetID;
        this.localAddress = localAddress;
        this.localPort = localPort;
        this.remoteAddress = remoteAddress;
        this.remotePort = remotePort;
        this.proxy = proxy;
    }

    public BindingNode getBindingNode() {
        return (BindingNode) node;
    }

    public String getLocalName() {
        return localAddress.getHostName();
    }

    public String getLocalAddr() {
        return localAddress.getHostAddress();
    }

    public int getLocalPort() {
        return localPort;
    }

    public String getRemoteHost() {
        return remoteAddress.getHostName();
    }

    public String getRemoteAddr() {
        return remoteAddress.getHostAddress();
    }

    public int getRemotePort() {
        return remotePort;
    }

    public boolean isProxy() {
        return proxy;
    }

    public String getMethod() {
        return method;
    }

    public String getProtocol() {
        return protocol;
    }

    public String getTransport() {
        return null;
    }

    public boolean isSecure() {
        return getTargetID().isSecure();
    }

    public SongURI getRequestingEntity() {
        return requestingEntity;
    }

    public SongURI getTargetID() {
        return targetID;
    }

    public long getSentTime() {
        return sendTime;
    }

    public long getExpirationTime() {
        return expirationTime;
    }

    public String getScheme() {
        return targetID.getScheme();
    }

    public String getServerName() {
        return targetID.getHost();
    }

    public int getServerPort() {
        return targetID.getPort();
    }

    public SongServletResponse createResponse(int statusCode, String reasonPhrase, StatusCode m2mStatus, String message,
            Exception exception) {
        RemoteWrappedRequest wrappedRequest = new RemoteWrappedRequest(null, this, null, null, null, null);
        LocalResponse response = new LocalResponse(node, wrappedRequest, SongServletResponse.SC_SERVICE_UNAVAILABLE,
                SongServletResponse.RP_SERVICE_UNAVAILABLE, null);
        if (exception != null) {
            response.setAttribute("Exception", exception);
        }

        setErrorInfoBody(response, m2mStatus, message);

        return response;
    }

    public void sendSyncResponse(SongServletResponse response) throws MessagingException {
        BackendMessage errorMessage = exchange.createMessage();
        errorMessage.setContent(response);
        exchange.setOutMessage(errorMessage);
        if (Profiler.getInstance().isTraceEnabled()) {
            Profiler.getInstance().trace(
                    getId() + ": <<< " + node.getTarget() + ": " + response.getStatus() + " " + response.getReasonPhrase());
        }
        if (LOG.isInfoEnabled()) {
            LOG.info(getId() + ": <<< " + node.getTarget() + ": " + response.getStatus() + " " + response.getReasonPhrase());
            LOG.info(getId() + ": " + InternalMessage.getResponseAsString(response));
        }
        node.getContainer().getContext().getDeliveryChannel().sendSync(exchange);
    }

    public void send() throws IOException {
        checkCommitted();
        committed = true;
        sendTime = System.currentTimeMillis();
        expirationTime = sendTime + REQUEST_TIMEOUT_DURATION;
        try {
            exchange = node.buildRequestExchange(this);
            if (Profiler.getInstance().isTraceEnabled()) {
                Profiler.getInstance().trace(
                        getId() + ": <<< " + node.getTarget() + ": " + getMethod() + " (requestingEntity: "
                                + getRequestingEntity().absoluteURI() + ") (targetID: " + getTargetID().absoluteURI() + ")");
            }
            if (LOG.isInfoEnabled()) {
                LOG.info(getId() + ": <<< " + node.getTarget() + ": " + getMethod() + " (requestingEntity: "
                        + getRequestingEntity().absoluteURI() + ") (targetID: " + getTargetID().absoluteURI() + ")");
                LOG.info(getId() + ": " + InternalMessage.getRequestAsString(this));
            }
            int priority = DeliveryChannel.NORMAL;
            Integer requestPriority = (Integer) getAttribute("com.actility.servlet.song.Priority");
            if (requestPriority != null) {
                priority = requestPriority.intValue();
            }
            if (node.getContainer().sendRemoteRequest(method)) {
                node.getContext().getDeliveryChannel().send(exchange, priority);
            } else {
                // send 503
                SongServletResponse response = createResponse(SongServletResponse.SC_SERVICE_UNAVAILABLE,
                        SongServletResponse.RP_SERVICE_UNAVAILABLE, StatusCode.STATUS_SERVICE_UNAVAILABLE,
                        "Too much pending remote requests", null);
                sendSyncResponse(response);
            }
        } catch (MessagingException e) {
            // Send 500
            SongServletResponse response = createResponse(SongServletResponse.SC_INTERNAL_SERVER_ERROR,
                    SongServletResponse.RP_INTERNAL_SERVER_ERROR, StatusCode.STATUS_INTERNAL_SERVER_ERROR,
                    "Cannot send request to destination endpoint", e);
            try {
                sendSyncResponse(response);
            } catch (MessagingException e1) {
                node.getContainer().receivedRemoteResponse(response, getSentTime(), System.currentTimeMillis() - getSentTime(),
                        getMethod());
                throw new IOException(e.getMessage());
            }
        }
    }

    public void send(SongURI poc) {
        throw new IllegalStateException(UNSUPPORTED_METHOD);
    }

    public void send(long millis, SongURI poc) {
        throw new IllegalStateException(UNSUPPORTED_METHOD);
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

    public Proxy getProxy() {
        throw new IllegalStateException(UNSUPPORTED_METHOD);
    }

    public void send(long millis) {
        throw new IllegalStateException(UNSUPPORTED_METHOD);
    }

    public String getPathInfo() {
        throw new IllegalStateException(UNSUPPORTED_METHOD);
    }

    public String getServletPath() {
        throw new IllegalStateException(UNSUPPORTED_METHOD);
    }

    public String getContextPath() {
        throw new IllegalStateException(UNSUPPORTED_METHOD);
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
