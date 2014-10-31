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
 * id $Id: EndpointNodeImpl.java 9481 2014-09-06 09:37:01Z JReich $
 * author $Author: JReich $
 * version $Revision: 9481 $
 * lastrevision $Date: 2014-09-06 11:37:01 +0200 (Sat, 06 Sep 2014) $
 * modifiedby $LastChangedBy: JReich $
 * lastmodified $LastChangedDate: 2014-09-06 11:37:01 +0200 (Sat, 06 Sep 2014) $
 */

package com.actility.m2m.servlet.song.node;

import java.io.IOException;

import javax.servlet.RequestDispatcher;
import javax.servlet.Servlet;
import javax.servlet.ServletException;

import org.apache.log4j.Logger;

import com.actility.m2m.be.EndpointContext;
import com.actility.m2m.be.messaging.BackendMessage;
import com.actility.m2m.be.messaging.InOnly;
import com.actility.m2m.be.messaging.InOut;
import com.actility.m2m.be.messaging.MessageExchange;
import com.actility.m2m.be.messaging.MessageExchangeFactory;
import com.actility.m2m.be.messaging.MessagingException;
import com.actility.m2m.framework.resources.BackupClassLoader;
import com.actility.m2m.servlet.ext.ExtProtocolContainer;
import com.actility.m2m.servlet.ext.ExtServletContext;
import com.actility.m2m.servlet.log.BundleLogger;
import com.actility.m2m.servlet.song.Proxy;
import com.actility.m2m.servlet.song.SongServlet;
import com.actility.m2m.servlet.song.SongServletRequest;
import com.actility.m2m.servlet.song.SongServletResponse;
import com.actility.m2m.servlet.song.internal.EndpointNode;
import com.actility.m2m.servlet.song.internal.SongContainerFacade;
import com.actility.m2m.servlet.song.message.InternalRequest;
import com.actility.m2m.servlet.song.message.LocalForwardedRequest;
import com.actility.m2m.servlet.song.message.LocalRequest;
import com.actility.m2m.servlet.song.message.OutgoingRequest;
import com.actility.m2m.util.Profiler;
import com.actility.m2m.util.log.OSGiLogger;

/**
 * This is a node link to a concrete endpoint in the backend service.
 *
 * TODO should add a valid boolean to check whether the endpoint has been deleted or not in the song container
 */
public abstract class EndpointNodeImpl implements EndpointNode {
    private static final Logger LOG = OSGiLogger.getLogger(EndpointNodeImpl.class, BundleLogger.getStaticLogger());
    protected SongServlet servlet;
    protected EndpointContext context;
    private final ExtServletContext servletContext;
    private final SongContainerFacade container;
    private final int priority;
    private final String servletName;

    protected EndpointNodeImpl(SongContainerFacade container, SongServlet servlet, ExtServletContext servletContext,
            String servletName, int priority) {
        this.container = container;
        this.servlet = servlet;
        this.servletContext = servletContext;
        this.servletName = servletName;
        this.priority = priority;
    }

    private InOut buildExchange(String id) throws MessagingException {
        MessageExchangeFactory factory = context.getDeliveryChannel().createExchangeFactory();
        InOut inOut = factory.createInOutExchange(id);
        inOut.setSource(context.getEndpointId());
        inOut.setDestination(container.getContext().getEndpointId());
        return inOut;
    }

    private void processRequest(MessageExchange exchange, OutgoingRequest request) {
        int statusCode = 0;

        if (Profiler.getInstance().isTraceEnabled()) {
            Profiler.getInstance().trace(
                    request.getId() + ": >>> " + getTarget() + ": " + request.getMethod() + " (requestingEntity: "
                            + request.getRequestingEntity().absoluteURI() + ") (targetID: "
                            + request.getTargetID().absoluteURI() + ")");
        }
        if (LOG.isInfoEnabled()) {
            LOG.info(request.getId() + ": >>> " + getTarget() + ": " + request.getMethod() + " (requestingEntity: "
                    + request.getRequestingEntity().absoluteURI() + ") (targetID: " + request.getTargetID().absoluteURI() + ")");
        }
        long now = System.currentTimeMillis();
        long expirationTime = request.getExpirationTime();
        if (now >= expirationTime) {
            statusCode = SongServletResponse.SC_GATEWAY_TIMEOUT;
        } else {
            BackupClassLoader backup = null;
            try {
                backup = container.getResourcesAccessorService().setThreadClassLoader(servlet.getClass());
                servletContext.setCurrentRequestHandler(container, this);
                servlet.service(request, null);
            } catch (ServletException se) {
                LOG.error(request.getId() + ": Problem while executing the service method", se);
                statusCode = SongServletResponse.SC_INTERNAL_SERVER_ERROR;
            } catch (IOException ioe) {
                LOG.error(request.getId() + ": Problem while executing the service method", ioe);
                statusCode = SongServletResponse.SC_INTERNAL_SERVER_ERROR;
            } catch (RuntimeException e) {
                LOG.error(request.getId() + ": An unexpected exception happened in the service method", e);
                statusCode = SongServletResponse.SC_INTERNAL_SERVER_ERROR;
            } finally {
                servletContext.setCurrentRequestHandler(container, null);
                if (backup != null) {
                    backup.restoreThreadClassLoader();
                }
            }
        }

        if (statusCode != 0 && !request.isResponded()) {
            SongServletResponse response = createResponse(request, statusCode);
            try {
                response.send();
            } catch (IOException e) {
                LOG.error(request.getId() + ": Problem while sending an error response", e);
            } catch (Exception e) {
                LOG.error(request.getId() + ": Cannot create an internal server error response", e);
            }
        }
    }

    private void processResponse(MessageExchange exchange, SongServletResponse response) {
        if (Profiler.getInstance().isTraceEnabled()) {
            Profiler.getInstance().trace(
                    response.getId() + ": >>> " + getTarget() + ": " + response.getStatus() + " " + response.getReasonPhrase());
        }
        if (LOG.isInfoEnabled()) {
            LOG.info(response.getId() + ": >>> " + getTarget() + ": " + response.getStatus() + " " + response.getReasonPhrase());
        }
        SongServletRequest request = response.getRequest();
        if (request.getClass() == LocalForwardedRequest.class) {
            Proxy proxy = request.getProxy();
            if (proxy == null || !proxy.getSupervised()) {
                try {
                    container.receivedForwardResponse(response, ((InternalRequest) request).getSentTime(),
                            System.currentTimeMillis() - ((InternalRequest) request).getSentTime(), request.getMethod());
                    if (LOG.isInfoEnabled()) {
                        LOG.info(response.getId()
                                + ": Response is forwarded to the final destination without notifying the servlet");
                    }
                    response.send();
                } catch (IOException e) {
                    LOG.error(response.getId() + ": Exception while sending the servlet response", e);
                }
            }
        } else {
            BackupClassLoader backup = null;
            try {
                backup = container.getResourcesAccessorService().setThreadClassLoader(servlet.getClass());
                servletContext.setCurrentRequestHandler(container, this);
                if (request.getClass() == LocalForwardedRequest.class) {
                    container.receivedForwardResponse(response, ((InternalRequest) request).getSentTime(),
                            System.currentTimeMillis() - ((InternalRequest) request).getSentTime(), request.getMethod());
                } else if (request.getClass() == LocalRequest.class) {
                    container.receivedLocalResponse(response, ((InternalRequest) request).getSentTime(),
                            System.currentTimeMillis() - ((InternalRequest) request).getSentTime(), request.getMethod());
                } else {
                    container.receivedRemoteResponse(response, ((InternalRequest) request).getSentTime(),
                            System.currentTimeMillis() - ((InternalRequest) request).getSentTime(), request.getMethod());
                }
                servlet.service(null, response);
            } catch (ServletException se) {
                LOG.error(response.getId() + ": Problem while executing the service method", se);
            } catch (IOException ioe) {
                LOG.error(response.getId() + ": Problem while executing the service method", ioe);
            } catch (Exception e) {
                LOG.error(response.getId() + ": An unexpected exception happened in the service method", e);
            } finally {
                servletContext.setCurrentRequestHandler(container, null);
                backup.restoreThreadClassLoader();
            }
        }
    }

    public final EndpointContext getContext() {
        return context;
    }

    public final int getPriority() {
        return priority;
    }

    public final void init(EndpointContext context) {
        this.context = context;
    }

    public final SongServlet getSongServlet() {
        return servlet;
    }

    public final ExtServletContext getServletContext() {
        return servletContext;
    }

    public abstract OutgoingRequest getRequest(Object request);

    public abstract SongServletResponse getResponse(Object response);

    public abstract SongServletResponse createResponse(SongServletRequest request, int statusCode);

    public final void process(InOut exchange) {
        // long processExt = -System.currentTimeMillis();
        if (LOG.isDebugEnabled()) {
            LOG.debug(exchange.getExchangeId() + ": " + getTarget() + " process InOut exchange");
        }

        if (exchange.getOutMessage() != null) {
            processResponse(exchange, getResponse(exchange.getOutMessage().getContent()));
        } else {
            processRequest(exchange, getRequest(exchange.getInMessage().getContent()));
        }
        // processExt += System.currentTimeMillis();
        // LOG.error("ProcessExt: " + processExt);
    }

    public final void process(InOnly exchange) {
        if (LOG.isDebugEnabled()) {
            LOG.debug(exchange.getExchangeId() + ": " + getTarget() + " process InOut exchange: " + exchange.getExchangeId());
        }
    }

    public final String getTarget() {
        return toString();
    }

    public final String toString() {
        return servletContext.getServletContextName() + "." + servletName;
    }

    public RequestDispatcher getRequestDispatcher(String path) {
        return servletContext.getRequestDispatcher(path);
    }

    public InOut buildRequestExchange(SongServletRequest request) throws MessagingException {
        InOut exchange = buildExchange(request.getId());
        BackendMessage message = exchange.createMessage();
        message.setContent(request);
        exchange.setInMessage(message);
        return exchange;
    }

    public InOut buildResponseExchange(InOut exchange, SongServletResponse response) throws MessagingException {
        InOut realExchange = exchange;
        BackendMessage message = realExchange.createMessage();
        message.setContent(response);
        realExchange.setOutMessage(message);
        return realExchange;
    }

    public SongContainerFacade getContainer() {
        return container;
    }

    public ExtProtocolContainer getProtocol() {
        return container;
    }

    public Servlet getServlet() {
        return servlet;
    }

    public void destroy() {
        servlet.destroy();
        container.destroyEndpoint(this);
    }
}
