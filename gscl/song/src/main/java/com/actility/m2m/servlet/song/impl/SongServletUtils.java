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
 * id $Id: SongServletUtils.java 6085 2013-10-15 14:08:29Z mlouiset $
 * author $Author: mlouiset $
 * version $Revision: 6085 $
 * lastrevision $Date: 2013-10-15 16:08:29 +0200 (Tue, 15 Oct 2013) $
 * modifiedby $LastChangedBy: mlouiset $
 * lastmodified $LastChangedDate: 2013-10-15 16:08:29 +0200 (Tue, 15 Oct 2013) $
 */

package com.actility.m2m.servlet.song.impl;

import java.net.URI;
import java.net.URISyntaxException;
import java.util.Map;

import javax.servlet.ServletException;

import com.actility.m2m.servlet.ApplicationSession;
import com.actility.m2m.servlet.NamespaceException;
import com.actility.m2m.servlet.ext.ExtApplicationSession;
import com.actility.m2m.servlet.ext.ExtServletContext;
import com.actility.m2m.servlet.song.LongPollURIs;
import com.actility.m2m.servlet.song.SongDynamicRouter;
import com.actility.m2m.servlet.song.SongFactory;
import com.actility.m2m.servlet.song.SongServletRequest;
import com.actility.m2m.servlet.song.SongServletResponse;
import com.actility.m2m.servlet.song.SongURI;
import com.actility.m2m.servlet.song.internal.BindingNode;
import com.actility.m2m.servlet.song.internal.EndpointNode;
import com.actility.m2m.servlet.song.message.InternalMessage;
import com.actility.m2m.servlet.song.message.LocalRequest;
import com.actility.m2m.servlet.song.message.RemoteForwardedRequest;
import com.actility.m2m.servlet.song.message.RemoteWrappedRequest;
import com.actility.m2m.util.EqualsUtils;

public class SongServletUtils implements SongFactory, SongDynamicRouter {
    protected SongContainer container;
    protected ExtServletContext servletContext;

    public SongServletUtils(SongContainer container, ExtServletContext servletContext) {
        this.container = container;
        this.servletContext = servletContext;
    }

    private LocalRequest createRequest(ApplicationSession appSession, String method, SongURI requestingEntity,
            SongURI targetID, InternalMessage internalRequest) {
        ExtApplicationSession realAppSession = (ExtApplicationSession) appSession;
        BindingNode bindingNode = container.getBindingNode(targetID.getScheme());
        BindingNode fromBindingNode = container.getBindingNode(requestingEntity.getScheme());

        if (bindingNode != null) {
            if (EqualsUtils.isNotEqual(bindingNode, fromBindingNode)) {
                throw new IllegalArgumentException(
                        "requestingEntity and targetID URIs must share the same protocol: requestingEntity -> "
                                + requestingEntity.getScheme() + ", targetID ->" + targetID.getScheme());
            }
            EndpointNode node = null;
            if (realAppSession != null) {
                node = (EndpointNode) realAppSession.getHandler(container);
            } else {
                node = (EndpointNode) servletContext.getCurrentRequestHandler(container);
            }

            return new LocalRequest(node, bindingNode, (ExtApplicationSession) appSession, method,
                    bindingNode.getDefaultProtocol(), requestingEntity, targetID, internalRequest);
        }
        throw new IllegalArgumentException("Unknown protocol for scheme: " + targetID.getScheme());
    }

    public ApplicationSession createApplicationSession() {
        return servletContext.createApplicationSession();
    }

    public SongServletRequest createRequestFrom(SongServletRequest request, SongURI requestingEntity, SongURI targetID) {
        if (!(request instanceof InternalMessage)) {
            throw new IllegalArgumentException("Cannot create a request from a request not created by the SONG container: "
                    + request.getClass());
        }
        InternalMessage internal = (InternalMessage) request;
        return createRequest(request.getApplicationSession(false), request.getMethod(), requestingEntity, targetID, internal);
    }

    public SongServletRequest createRequestFrom(SongServletRequest request, URI requestingEntity, URI targetID) {
        SongURI songRequestingEntity = (requestingEntity != null) ? createURI(requestingEntity) : null;
        SongURI songTargetID = (targetID != null) ? createURI(targetID) : null;
        return createRequestFrom(request, songRequestingEntity, songTargetID);
    }

    public SongServletRequest createRequestFrom(SongServletRequest request, String requestingEntity, String targetID)
            throws URISyntaxException {
        SongURI songRequestingEntity = (requestingEntity != null) ? createURI(requestingEntity) : null;
        SongURI songTargetID = (targetID != null) ? createURI(targetID) : null;
        return createRequestFrom(request, songRequestingEntity, songTargetID);
    }

    public SongServletResponse createResponseFrom(SongServletRequest request, SongServletResponse response, int statusCode) {
        return createResponseFrom(request, response, statusCode, InternalMessage.getReasonPhrase(statusCode));
    }

    public SongServletResponse createResponseFrom(SongServletRequest request, SongServletResponse response, int statusCode,
            String reasonPhrase) {
        if (!(response instanceof InternalMessage)) {
        throw new IllegalArgumentException("Cannot create a response from a response not created by the SONG container: "
                + request.getClass());
        }
        InternalMessage internal = (InternalMessage) response;
        if (request.getClass() == RemoteWrappedRequest.class) {
            return ((RemoteWrappedRequest) request).createResponse(statusCode, reasonPhrase, internal, false);
        } else if (request.getClass() == RemoteForwardedRequest.class) {
            return ((RemoteForwardedRequest) request).createResponse(statusCode, reasonPhrase, internal, false);
        } else {
            throw new IllegalArgumentException("Cannot create a response with a request which is not received: "
                    + request.getClass());
        }
    }

    public SongServletRequest createRequest(String method, SongURI requestingEntity, SongURI targetID) {
        return createRequest(null, method, requestingEntity, targetID);
    }

    public SongServletRequest createRequest(String method, URI requestingEntity, URI targetID) {
        return createRequest(null, method, requestingEntity, targetID);
    }

    public SongServletRequest createRequest(String method, String requestingEntity, String targetID) throws URISyntaxException {
        return createRequest(null, method, requestingEntity, targetID);
    }

    public SongServletRequest createRequest(ApplicationSession appSession, String method, SongURI requestingEntity,
            SongURI targetID) {
        if (appSession != null && !(appSession instanceof ExtApplicationSession)) {
            throw new IllegalArgumentException("invalid application session");
        }
        if (targetID == null) {
            throw new IllegalArgumentException("to cannot be null");
        }

        return createRequest(appSession, method, requestingEntity, targetID, null);
    }

    public SongServletRequest createRequest(ApplicationSession appSession, String method, URI requestingEntity, URI targetID) {
        SongURI songRequestingEntity = (requestingEntity != null) ? createURI(requestingEntity) : null;
        SongURI songTargetID = (targetID != null) ? createURI(targetID) : null;
        return createRequest(appSession, method, songRequestingEntity, songTargetID);
    }

    public SongServletRequest createRequest(ApplicationSession appSession, String method, String requestingEntity,
            String targetID) throws URISyntaxException {
        SongURI songRequestingEntity = (requestingEntity != null) ? createURI(requestingEntity) : null;
        SongURI songTargetID = (targetID != null) ? createURI(targetID) : null;
        return createRequest(appSession, method, songRequestingEntity, songTargetID);
    }

    public synchronized SongURI createURI(String scheme, String hostname, int port, String path) {
        return new SongURIImpl(scheme, scheme.endsWith("s"), hostname, port, path);
    }

    public SongURI createURI(URI uri) {
        return new SongURIImpl(uri, container.getLocalHostName(), container.getDefaultBindingNode());
    }

    public SongURI createURI(String uri) throws URISyntaxException {
        return new SongURIImpl(uri, container.getLocalHostName(), container.getDefaultBindingNode());
    }

    public void addPath(String path, Map configuration) throws ServletException, NamespaceException {
        if (path == null || path.length() == 0) {
            throw new ServletException("Can't register a SONG servlet on a null or empty path");
        }
        if (path.charAt(0) != '/' || (path.length() > 1 && path.endsWith("/")) || path.indexOf('*') != -1) {
            throw new ServletException("Bad path. It must start with '/', must not end with '/' and must not contains any '*'");
        }

        EndpointNode endpoint = (EndpointNode) servletContext.getCurrentRequestHandler(container);
        container.addPath(servletContext.getContextPath(), path, endpoint, configuration);
    }

    public void addPath(String path, ApplicationSession appSession, Map configuration) throws ServletException,
            NamespaceException {
        if (path == null || path.length() == 0) {
            throw new ServletException("Can't register a SONG servlet on a null ot empty path");
        }
        if (path.charAt(0) != '/' || (path.length() > 1 && path.endsWith("/")) || path.indexOf('*') != -1) {
            throw new ServletException("Bad path. It must start with '/', must not end with '/' and must not contains any '*'");
        }
        if (!(appSession instanceof ExtApplicationSession)) {
            throw new ServletException("Can't register a null or unknown Application session");
        }

        EndpointNode endpoint = (EndpointNode) servletContext.getCurrentRequestHandler(container);
        container.addSessionPath(servletContext.getContextPath(), path, endpoint, (ExtApplicationSession) appSession,
                configuration);
    }

    public void removePath(String path) {
        container.removePath(servletContext.getContextPath(), path);
    }

    public LongPollURIs createServerLongPoll(SongURI serverURI) throws ServletException {
        return container.createServerLongPoll(serverURI);
    }

    public void createServerLongPoll(SongURI contactURI, SongURI longPollURI) throws ServletException {
        container.createServerLongPoll(contactURI, longPollURI);
    }

    public void deleteServerLongPoll(SongURI contactURI, SongURI longPollURI) {
        container.deleteServerLongPoll(contactURI, longPollURI);
    }

    public void createClientLongPoll(SongURI contactURI, SongURI longPollURI) throws ServletException {
        container.createClientLongPoll(contactURI, longPollURI, servletContext.getContextPath());
    }

    public void deleteClientLongPoll(SongURI contactURI, SongURI longPollURI) {
        container.deleteClientLongPoll(contactURI, longPollURI);
    }

    public boolean canBeServerFrom(SongURI targetedURI) throws ServletException {
        return container.canBeServerFrom(targetedURI);
    }

    public LongPollURIs createServerLongPoll(String serverURI) throws URISyntaxException, ServletException {
        SongURI realServerURI = createURI(serverURI);
        return container.createServerLongPoll(realServerURI);
    }

    public void createServerLongPoll(String contactURI, String longPollURI) throws URISyntaxException, ServletException {
        SongURI realContactURI = createURI(contactURI);
        SongURI realLongPollURI = createURI(longPollURI);
        container.createServerLongPoll(realContactURI, realLongPollURI);
    }

    public void deleteServerLongPoll(String contactURI, String longPollURI) {
        try {
            SongURI realContactURI = createURI(contactURI);
            SongURI realLongPollURI = createURI(longPollURI);
            container.deleteServerLongPoll(realContactURI, realLongPollURI);
        } catch (URISyntaxException e) {
            // Ignore
        }
    }

    public void createClientLongPoll(String contactURI, String longPollURI) throws URISyntaxException, ServletException {
        SongURI realContactURI = createURI(contactURI);
        SongURI realLongPollURI = createURI(longPollURI);
        container.createClientLongPoll(realContactURI, realLongPollURI, servletContext.getContextPath());
    }

    public void deleteClientLongPoll(String contactURI, String longPollURI) {
        try {
            SongURI realContactURI = createURI(contactURI);
            SongURI realLongPollURI = createURI(longPollURI);
            container.deleteClientLongPoll(realContactURI, realLongPollURI);
        } catch (URISyntaxException e) {
            // Ignore
        }
    }

    public boolean canBeServerFrom(String targetedURI) throws URISyntaxException, ServletException {
        SongURI realTargetedURI = createURI(targetedURI);
        return container.canBeServerFrom(realTargetedURI);
    }

    public SongURI createLocalURIFrom(URI reference, String path) throws ServletException {
        BindingNode binding = container.getBindingNode(reference.isAbsolute() ? reference.getScheme() : "song");
        if (binding != null) {
            String scheme = binding.getServerScheme();
            String realPath = servletContext.getContextPath();
            if (path != null) {
                realPath += path;
            }
            return new SongURIImpl(scheme, scheme.endsWith("s"), container.getLocalHostName(), binding.getPort(), realPath);
        }
        throw new ServletException("Unknown scheme");
    }

    public SongURI createLocalURIFrom(String reference, String path) throws URISyntaxException, ServletException {
        if (reference == null) {
            throw new IllegalArgumentException("Cannot get the local URI from a null URI");
        }
        return createLocalURIFrom(new URI(reference), path);
    }

    public SongURI createLocalURIFrom(SongURI reference, String path) throws ServletException {
        BindingNode binding = container.getBindingNode(reference.getScheme());
        if (binding != null) {
            String scheme = binding.getServerScheme();
            String realPath = servletContext.getContextPath();
            if (path != null) {
                realPath += path;
            }
            return new SongURIImpl(scheme, scheme.endsWith("s"), container.getLocalHostName(), binding.getPort(), realPath);
        }
        throw new ServletException("Unknown scheme");
    }
}
