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
 * id $Id: ServletEndpointNode.java 8766 2014-05-21 15:41:17Z JReich $
 * author $Author: JReich $
 * version $Revision: 8766 $
 * lastrevision $Date: 2014-05-21 17:41:17 +0200 (Wed, 21 May 2014) $
 * modifiedby $LastChangedBy: JReich $
 * lastmodified $LastChangedDate: 2014-05-21 17:41:17 +0200 (Wed, 21 May 2014) $
 */

package com.actility.m2m.servlet.song.node;

import java.util.ArrayList;
import java.util.Collections;
import java.util.List;

import org.apache.log4j.Logger;

import com.actility.m2m.be.messaging.InOut;
import com.actility.m2m.be.messaging.MessagingException;
import com.actility.m2m.servlet.ext.ExtApplicationSession;
import com.actility.m2m.servlet.ext.ExtServletContext;
import com.actility.m2m.servlet.log.BundleLogger;
import com.actility.m2m.servlet.song.SongServlet;
import com.actility.m2m.servlet.song.SongServletRequest;
import com.actility.m2m.servlet.song.SongServletResponse;
import com.actility.m2m.servlet.song.internal.SongContainerFacade;
import com.actility.m2m.servlet.song.message.InternalMessage;
import com.actility.m2m.servlet.song.message.LocalForwardedRequest;
import com.actility.m2m.servlet.song.message.LocalForwardedResponse;
import com.actility.m2m.servlet.song.message.LocalRequest;
import com.actility.m2m.servlet.song.message.LocalResponse;
import com.actility.m2m.servlet.song.message.LocalWrappedRequest;
import com.actility.m2m.servlet.song.message.OutgoingRequest;
import com.actility.m2m.servlet.song.message.RemoteForwardedRequest;
import com.actility.m2m.servlet.song.message.RemoteForwardedResponse;
import com.actility.m2m.servlet.song.message.RemoteRequest;
import com.actility.m2m.servlet.song.message.RemoteResponse;
import com.actility.m2m.servlet.song.message.RemoteWrappedRequest;
import com.actility.m2m.servlet.song.message.RemoteWrappedResponse;
import com.actility.m2m.util.log.OSGiLogger;

public final class ServletEndpointNode extends EndpointNodeImpl {

    private static final Logger LOG = OSGiLogger.getLogger(ServletEndpointNode.class, BundleLogger.getStaticLogger());
    private List paths;

    public ServletEndpointNode(SongContainerFacade container, SongServlet servlet, ExtServletContext servletContext,
            String servletName, int priority) {
        super(container, servlet, servletContext, servletName, priority);
    }

    public String getContextPath() {
        return getServletContext().getContextPath();
    }

    public List getPaths() {
        return (paths != null) ? paths : Collections.EMPTY_LIST;
    }

    public void addPath(String path) {
        if (LOG.isDebugEnabled()) {
            LOG.debug("Add path: " + path);
        }
        if (paths == null) {
            paths = new ArrayList(1);
        }
        paths.add(path);
    }

    public void removePath(String path) {
        if (LOG.isDebugEnabled()) {
            LOG.debug("Remove path: " + path);
        }
        if (paths != null) {
            paths.remove(path);
            if (paths.size() == 0) {
                paths = null;
            }
        }
    }

    public void update(InOut exchange, SongServletRequest request, ExtApplicationSession appSession, String contextPath,
            String servletPath, String pathInfo) throws MessagingException {
        Class requestClass = request.getClass();
        SongServletRequest newRequest = null;
        if (RemoteRequest.class == requestClass) {
            newRequest = new RemoteWrappedRequest(this, (InternalMessage) request, appSession, contextPath, servletPath,
                    pathInfo);
        } else if (LocalRequest.class == requestClass) {
            newRequest = new RemoteForwardedRequest(this, (InternalMessage) request, appSession, contextPath, servletPath,
                    pathInfo, ((LocalRequest) request).getPoc());
        } else if (LocalForwardedRequest.class == requestClass) {
            newRequest = new RemoteForwardedRequest(this, (InternalMessage) request, appSession, contextPath, servletPath,
                    pathInfo, ((LocalForwardedRequest) request).getPoc());
        } else {
            throw new MessagingException("Unexpected request type: " + request.getClass());
        }
        exchange.setDestination(context.getEndpointId());
        exchange.getInMessage().setContent(newRequest);
    }

    public OutgoingRequest getRequest(Object request) {
        Class requestClass = request.getClass();
        if (RemoteWrappedRequest.class == requestClass ) {
            return (RemoteWrappedRequest) request;
        } else if (RemoteForwardedRequest.class == requestClass) {
            return (RemoteForwardedRequest) request;
        } else {
            throw new IllegalArgumentException("Unexpected received request: " + requestClass);
        }
    }

    public SongServletResponse getResponse(Object response) {
        SongServletResponse newResponse = null;
        Class responseClass = response.getClass();
        if (RemoteResponse.class == responseClass) {
            RemoteResponse remoteResponse = (RemoteResponse) response;
            LocalWrappedRequest wrappedRequest = (LocalWrappedRequest) remoteResponse.getRequest();
            newResponse = new RemoteWrappedResponse(this, wrappedRequest.getWrappedRequest(), remoteResponse);
        } else if (LocalResponse.class == responseClass || LocalForwardedResponse.class == responseClass) {
            RemoteForwardedRequest forwardedRequest = (RemoteForwardedRequest) ((SongServletResponse) response).getRequest();
            newResponse = new RemoteForwardedResponse(this, forwardedRequest.getForwardedRequest(),
                    (InternalMessage) response);
        } else {
            throw new IllegalArgumentException("Unexpected received response: " + response.getClass());
        }
        return newResponse;
    }

    public SongServletResponse createResponse(SongServletRequest request, int statusCode) {
        Class requestClass = request.getClass();
        if (RemoteWrappedRequest.class == requestClass ) {
            return ((RemoteWrappedRequest) request).createResponse(statusCode, true);
        } else if (RemoteForwardedRequest.class == requestClass) {
            return ((RemoteForwardedRequest) request).createResponse(statusCode, true);
        } else {
            throw new IllegalArgumentException("Unexpected request: " + requestClass);
        }
    }
}
