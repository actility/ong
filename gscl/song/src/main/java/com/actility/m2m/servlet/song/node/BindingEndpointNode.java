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
 * id $Id: BindingEndpointNode.java 8767 2014-05-21 15:41:33Z JReich $
 * author $Author: JReich $
 * version $Revision: 8767 $
 * lastrevision $Date: 2014-05-21 17:41:33 +0200 (Wed, 21 May 2014) $
 * modifiedby $LastChangedBy: JReich $
 * lastmodified $LastChangedDate: 2014-05-21 17:41:33 +0200 (Wed, 21 May 2014) $
 */

package com.actility.m2m.servlet.song.node;

import java.net.InetAddress;

import org.apache.log4j.Logger;

import com.actility.m2m.be.messaging.InOut;
import com.actility.m2m.be.messaging.MessagingException;
import com.actility.m2m.servlet.ext.ExtApplicationSession;
import com.actility.m2m.servlet.ext.ExtServletContext;
import com.actility.m2m.servlet.log.BundleLogger;
import com.actility.m2m.servlet.song.SongBindingFacade;
import com.actility.m2m.servlet.song.SongServlet;
import com.actility.m2m.servlet.song.SongServletRequest;
import com.actility.m2m.servlet.song.SongServletResponse;
import com.actility.m2m.servlet.song.internal.BindingNode;
import com.actility.m2m.servlet.song.internal.SongContainerFacade;
import com.actility.m2m.servlet.song.message.InternalMessage;
import com.actility.m2m.servlet.song.message.LocalForwardedRequest;
import com.actility.m2m.servlet.song.message.LocalForwardedResponse;
import com.actility.m2m.servlet.song.message.LocalRequest;
import com.actility.m2m.servlet.song.message.LocalResponse;
import com.actility.m2m.servlet.song.message.LocalWrappedRequest;
import com.actility.m2m.servlet.song.message.LocalWrappedResponse;
import com.actility.m2m.servlet.song.message.OutgoingRequest;
import com.actility.m2m.servlet.song.message.RemoteWrappedRequest;
import com.actility.m2m.util.log.OSGiLogger;

public final class BindingEndpointNode extends EndpointNodeImpl implements BindingNode {

    private static final Logger LOG = OSGiLogger.getLogger(BindingEndpointNode.class, BundleLogger.getStaticLogger());
    private final SongBindingFacade songBindingFacade;
    private final String serverScheme;
    private final String[] managedSchemes;
    private final boolean longPollSupported;
    private final String defaultProtocol;
    private final InetAddress address;
    private final int port;

    public BindingEndpointNode(SongContainerFacade container, SongServlet servlet, ExtServletContext servletContext,
            String servletName, int priority, SongBindingFacade songBindingFacade, String serverScheme,
            String[] managedSchemes, boolean longPollSupported, String defaultProtocol, InetAddress address, int port) {
        super(container, servlet, servletContext, servletName, priority);
        this.defaultProtocol = defaultProtocol;
        this.serverScheme = serverScheme;
        this.managedSchemes = managedSchemes;
        this.longPollSupported = longPollSupported;
        this.songBindingFacade = songBindingFacade;
        this.address = address;
        this.port = port;
    }

    public String getContextPath() {
        return null;
    }

    public String getDefaultProtocol() {
        return defaultProtocol;
    }

    public String getServerScheme() {
        return serverScheme;
    }

    public SongBindingFacade getSongBindingFacade() {
        return songBindingFacade;
    }

    public String[] getManagedSchemes() {
        return managedSchemes;
    }

    public boolean isLongPollSupported() {
        return longPollSupported;
    }

    public InetAddress getAddress() {
        return address;
    }

    public int getPort() {
        return port;
    }

    public void update(InOut exchange, SongServletRequest request, ExtApplicationSession appSession, String contextPath,
            String servletPath, String pathInfo) throws MessagingException {
        Class requestClass = request.getClass();
        LocalWrappedRequest wrappedRequest = null;
        if (LocalRequest.class == requestClass) {
            LocalRequest localRequest = (LocalRequest) request;
            wrappedRequest = new LocalWrappedRequest(this, localRequest, localRequest.getPoc());
        } else if (LocalForwardedRequest.class == requestClass) {
            LocalForwardedRequest localForwardedRequest = (LocalForwardedRequest) request;
            wrappedRequest = new LocalWrappedRequest(this, localForwardedRequest, localForwardedRequest.getPoc());
        } else {
            throw new MessagingException("Unexpected request type: " + request.getClass());
        }
        exchange.getInMessage().setContent(wrappedRequest);
        exchange.setDestination(context.getEndpointId());
    }

    public OutgoingRequest getRequest(Object request) {
        Class requestClass = request.getClass();
        if (LocalWrappedRequest.class == requestClass) {
            return (LocalWrappedRequest) request;
        } else {
            throw new IllegalArgumentException("Unexpected received request: " + requestClass);
        }
    }

    public SongServletResponse getResponse(Object response) {
        SongServletResponse newResponse = null;
        Class responseClass = response.getClass();
        if (LocalResponse.class == responseClass || LocalForwardedResponse.class == responseClass) {
            RemoteWrappedRequest remoteWrappedRequest = (RemoteWrappedRequest) ((SongServletResponse) response).getRequest();
            newResponse = new LocalWrappedResponse(this, remoteWrappedRequest.getWrappedRequest(), (InternalMessage) response);
        } else {
            throw new IllegalArgumentException("Unexpected received response: " + response.getClass());
        }
        return newResponse;
    }

    public SongServletResponse createResponse(SongServletRequest request, int statusCode) {
        LocalWrappedRequest realRequest = (LocalWrappedRequest) request;
        return realRequest
                .createResponse(statusCode, InternalMessage.getReasonPhrase(statusCode), address, port, address, port);
    }
}
