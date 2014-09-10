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
 * id $Id: LocalRequest.java 8767 2014-05-21 15:41:33Z JReich $
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
import com.actility.m2m.be.messaging.MessagingException;
import com.actility.m2m.m2m.StatusCode;
import com.actility.m2m.servlet.ApplicationSession;
import com.actility.m2m.servlet.ext.ExtApplicationSession;
import com.actility.m2m.servlet.log.BundleLogger;
import com.actility.m2m.servlet.song.Proxy;
import com.actility.m2m.servlet.song.SongServletMessage;
import com.actility.m2m.servlet.song.SongServletRequest;
import com.actility.m2m.servlet.song.SongServletResponse;
import com.actility.m2m.servlet.song.SongURI;
import com.actility.m2m.servlet.song.internal.BindingNode;
import com.actility.m2m.servlet.song.internal.EndpointNode;
import com.actility.m2m.util.Profiler;
import com.actility.m2m.util.log.OSGiLogger;

/**
 * LocalRequest which is a {@link SongServletRequest} initiated by a service servlet.
 */
public final class LocalRequest extends HC implements InternalRequest, Proxy {
    private static final Logger LOG = OSGiLogger.getLogger(LocalRequest.class, BundleLogger.getStaticLogger());

    private ExtApplicationSession appSession;
    private final BindingNode bindingNode;
    private boolean supervised;
    private SongURI poc;
    private SongURI proxyTo;
    private long sendTime;
    private long expirationTime;
    private final SongURI requestingEntity;
    private final SongURI targetID;
    private final String method;
    private final String protocol;

    // private RunnableTimerTask timer;

    /**
     * Builds a LocalRequest which is a request initiated by a service servlet.
     *
     * @param node The node that creates the request
     * @param bindingNode The binding corresponding to the scheme used within the SONG communication
     * @param appSession The application session associated with the request
     * @param method The request method (CREATE, RETRIEVE, UPDATE, DELETE)
     * @param protocol The request protocol and version (HTTP/1.1, SONG/1.0)
     * @param requestingEntity The requestingEntity which defines the request issuer
     * @param targetID The targetID which defines the request destination
     * @param internal An internal request to copy to build this request
     */
    public LocalRequest(EndpointNode node, BindingNode bindingNode, ExtApplicationSession appSession, String method,
            String protocol, SongURI requestingEntity, SongURI targetID, InternalMessage internal) {
        super(node, internal, null);
        this.method = method;
        this.protocol = protocol;
        this.requestingEntity = requestingEntity;
        this.targetID = targetID;
        this.appSession = appSession;
        this.bindingNode = bindingNode;
        if (internal == null) {
            setHeaderInternal(SongServletMessage.HD_USER_AGENT, node.getServletContext().getServerInfo());
        }
    }

    public SongURI getPoc() {
        return poc;
    }

    public BindingNode getBindingNode() {
        return bindingNode;
    }

    public long getSentTime() {
        return sendTime;
    }

    public long getExpirationTime() {
        return expirationTime;
    }

    // public boolean receivedResponse() {
    // return timer.cancel();
    // }

    public ApplicationSession getApplicationSession(boolean create) {
        if (appSession == null && create) {
            appSession = node.getServletContext().createApplicationSession();
        }
        return appSession;
    }

    public String getMethod() {
        return method;
    }

    public String getProtocol() {
        return protocol;
    }

    public String getTransport() {
        // Not applicable
        return null;
    }

    public boolean isSecure() {
        return targetID.isSecure();
    }

    public SongURI getRequestingEntity() {
        return requestingEntity;
    }

    public SongURI getTargetID() {
        return targetID;
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
            throw new IllegalArgumentException("Expiration timer for local request must be strictly greater than 0: " + millis);
        }
        checkCommitted();
        committed = true;
        sendTime = System.currentTimeMillis();
        expirationTime = sendTime + millis;
        if (proxyTo != null && !proxyTo.getScheme().equals(targetID.getScheme())) {
            throw new IOException("Proxy to server \"" + proxyTo.absoluteURI()
                    + "\" uses a scheme different from the targetID \"" + targetID.absoluteURI() + "\"");
        }
        this.poc = poc;
        try {
            // timer = new RunnableTimerTask(this);
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
            // node.getContainer().scheduleTimer(timer, millis);
            int priority = node.getPriority();
            Integer requestPriority = (Integer) getAttribute("com.actility.servlet.song.Priority");
            if (requestPriority != null) {
                priority = requestPriority.intValue();
            }
            if (node.getContainer().sendLocalRequest(method)) {
                node.getContext().getDeliveryChannel().send(exchange, priority);
            } else {
                // Send 503
                SongServletResponse response = createResponse(SongServletResponse.SC_SERVICE_UNAVAILABLE,
                        SongServletResponse.RP_SERVICE_UNAVAILABLE, StatusCode.STATUS_SERVICE_UNAVAILABLE,
                        "Too much pending local requests", null);
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
                node.getContainer().receivedLocalResponse(response, getSentTime(), System.currentTimeMillis() - getSentTime(),
                        getMethod());
                throw new IOException(e.getMessage());
            }
        }
    }

    public Proxy getProxy() {
        return this;
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

    // TODO should be placed elsewhere as the timer will be started only once when delivered to the destination servlet
    // public void run() {
    // long run = System.currentTimeMillis();
    // InOut exchange = this.exchange;
    // if (LOG.isInfoEnabled()) {
    // LOG.info(exchange.getExchangeId() + ": Expiration timer has expired for request");
    // }
    //
    // BindingNode bindingNode = this.bindingNode;
    // // TODO redo SongServletResponse response = new RemoteErrorResponse(request, SongServletResponse.SC_SERVER_TIMEOUT,
    // // bindingNode.getAddress(), bindingNode.getPort(), bindingNode.getAddress(), bindingNode.getPort());
    // SongServletResponse response = new RemoteErrorResponse(this, SongServletResponse.SC_GATEWAY_TIMEOUT,
    // bindingNode.getAddress(), bindingNode.getPort(), bindingNode.getAddress(), bindingNode.getPort());
    // try {
    // BackendMessage errorMessage = exchange.createMessage();
    // errorMessage.setContent(response);
    // exchange.setOutMessage(errorMessage);
    // if (LOG.isInfoEnabled()) {
    // LOG.info(exchange.getExchangeId() + ": <<< Router: " + response.getStatus() + " " + response.getReasonPhrase());
    // }
    // if (LOG.isDebugEnabled()) {
    // LOG.debug(exchange.getExchangeId() + ": " + InternalMessage.getResponseAsString(response));
    // }
    // node.getContainer().getContext().getDeliveryChannel().send(exchange, DeliveryChannel.IMPORTANT);
    // } catch (MessagingException e) {
    // LOG.error("Problem while sending 408 Request Timeout", e);
    // }
    // run = System.currentTimeMillis() - run;
    // if (run > 500) {
    // LOG.error("The local request expiration has taken more than 500 ms to be handled: " + run + " ms");
    // }
    // }

    public SongServletResponse createResponse(int statuscode) {
        throw new IllegalStateException(UNSUPPORTED_METHOD);
    }

    public SongServletResponse createResponse(int statusCode, String reasonPhrase) {
        throw new IllegalStateException(UNSUPPORTED_METHOD);
    }

    public String getLocalName() {
        throw new IllegalStateException(UNSUPPORTED_METHOD);
    }

    public String getRemoteHost() {
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

    public String getPathInfo() {
        throw new IllegalStateException(UNSUPPORTED_METHOD);
    }

    public BufferedReader getReader() {
        throw new IllegalStateException(UNSUPPORTED_METHOD);
    }

    public String getRealPath(String path) {
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

    public boolean isProxy() {
        throw new IllegalStateException(UNSUPPORTED_METHOD);
    }
}
