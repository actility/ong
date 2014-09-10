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
 * id $Id: LocalForwardedRequest.java 8767 2014-05-21 15:41:33Z JReich $
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
import com.actility.m2m.servlet.song.SongServletResponse;
import com.actility.m2m.servlet.song.SongURI;
import com.actility.m2m.servlet.song.internal.BindingNode;
import com.actility.m2m.servlet.song.internal.EndpointNode;
import com.actility.m2m.util.Profiler;
import com.actility.m2m.util.log.OSGiLogger;

/**
 * Internal message built from a RemoteWrappedRequest or RemoteForwardedRequest.
 * <p>
 * This message is never handled by any servlet
 *
 */
public final class LocalForwardedRequest extends HCReadOnlyRef implements InternalRequest {
    private static final Logger LOG = OSGiLogger.getLogger(LocalForwardedRequest.class, BundleLogger.getStaticLogger());

    private final Proxy proxy;
    private SongURI poc;
    private long sendTime;
    private long expirationTime;

    /**
     * Builds a LocalForwardedRequest from the given RemoteWrappedRequest or RemoteForwardedRequest.
     *
     * @param node The node that creates the request
     * @param request The request that is forwarded
     * @param proxy The proxy to which the request must be sent if any
     *
     */
    public LocalForwardedRequest(EndpointNode node, InternalMessage request, Proxy proxy) {
        super(node, request);
        this.proxy = proxy;
        this.committed = true;
    }

    public SongURI getPoc() {
        return poc;
    }

    /**
     * Gets the Forwarded request which is a RemoteWrappedRequest or RemoteForwardedRequest
     *
     * @return The forwarded request
     */
    public InternalMessage getForwardedRequest() {
        return message;
    }

    public BindingNode getBindingNode() {
        return message.getBindingNode();
    }

    public long getSentTime() {
        return sendTime;
    }

    public long getExpirationTime() {
        return expirationTime;
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

    public SongServletResponse createResponse(int statusCode, String reasonPhrase, StatusCode m2mStatus, String message,
            Exception exception) {
        BindingNode bindingFacade = getBindingNode();
        InetAddress address = bindingFacade.getAddress();
        int port = bindingFacade.getPort();
        LocalWrappedRequest wrappedRequest = new LocalWrappedRequest(null, this, null);
        SongServletResponse response = wrappedRequest.createResponse(statusCode, reasonPhrase, address, port, address, port);
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

    public void send(long millis, SongURI poc) throws IOException {
        if (millis <= 0) {
            throw new IllegalArgumentException("Expiration timer must be strictly greater than 0: " + millis);
        }
        sendTime = System.currentTimeMillis();
        expirationTime = sendTime + millis;
        long forwardedExpiration = ((InternalRequest) message).getExpirationTime();
        if (expirationTime > forwardedExpiration) {
            expirationTime = forwardedExpiration;
        }
        if (proxy.getProxyTo() != null && !proxy.getProxyTo().getScheme().equals(getTargetID().getScheme())) {
            throw new IOException("Proxy to server \"" + proxy.getProxyTo().absoluteURI()
                    + "\" uses a scheme different from the targetID \"" + getTargetID().absoluteURI() + "\"");
        }
        this.poc = poc;
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
            int priority = DeliveryChannel.IMPORTANT;
            Integer requestPriority = (Integer) message.getAttribute("com.actility.servlet.song.Priority");
            if (requestPriority != null) {
                priority = requestPriority.intValue();
            }
            if (node.getContainer().sendForwardRequest(getMethod())) {
                node.getContext().getDeliveryChannel().send(exchange, priority);
            } else {
                // send 503
                SongServletResponse response = createResponse(SongServletResponse.SC_SERVICE_UNAVAILABLE,
                        SongServletResponse.RP_SERVICE_UNAVAILABLE, StatusCode.STATUS_SERVICE_UNAVAILABLE,
                        "Too much pending forward requests", null);
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
                node.getContainer().receivedForwardResponse(response, getSentTime(),
                        System.currentTimeMillis() - getSentTime(), getMethod());
                throw new IOException(e.getMessage());
            }
        }
    }

    public Proxy getProxy() {
        return proxy;
    }

    public ApplicationSession getApplicationSession(boolean create) {
        return null;
    }

    public String getParameter(String name) {
        throw new IllegalStateException(UNSUPPORTED_METHOD);
    }

    public Enumeration getParameterNames() {
        throw new IllegalStateException(UNSUPPORTED_METHOD);
    }

    public String[] getParameterValues(String name) {
        throw new IllegalStateException(UNSUPPORTED_METHOD);
    }

    public Map getParameterMap() {
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

    public String getRemoteHost() {
        throw new IllegalStateException(UNSUPPORTED_METHOD);
    }

    public Locale getLocale() {
        throw new IllegalStateException(UNSUPPORTED_METHOD);
    }

    public Enumeration getLocales() {
        throw new IllegalStateException(UNSUPPORTED_METHOD);
    }

    public String getRealPath(String path) {
        throw new IllegalStateException(UNSUPPORTED_METHOD);
    }

    public String getLocalName() {
        throw new IllegalStateException(UNSUPPORTED_METHOD);
    }

    public SongServletResponse createResponse(int statuscode) {
        throw new IllegalStateException(UNSUPPORTED_METHOD);
    }

    public SongServletResponse createResponse(int statusCode, String reasonPhrase) {
        throw new IllegalStateException(UNSUPPORTED_METHOD);
    }

    public ServletInputStream getInputStream() {
        throw new IllegalStateException(UNSUPPORTED_METHOD);
    }

    public BufferedReader getReader() {
        throw new IllegalStateException(UNSUPPORTED_METHOD);
    }

    public boolean isProxy() {
        throw new IllegalStateException(UNSUPPORTED_METHOD);
    }

    public String getContextPath() {
        throw new IllegalStateException(UNSUPPORTED_METHOD);
    }

    public String getServletPath() {
        throw new IllegalStateException(UNSUPPORTED_METHOD);
    }

    public String getPathInfo() {
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

}
