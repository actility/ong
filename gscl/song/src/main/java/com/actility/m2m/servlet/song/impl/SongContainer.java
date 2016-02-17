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
 * id $Id: SongContainer.java 9481 2014-09-06 09:37:01Z JReich $
 * author $Author: JReich $
 * version $Revision: 9481 $
 * lastrevision $Date: 2014-09-06 11:37:01 +0200 (Sat, 06 Sep 2014) $
 * modifiedby $LastChangedBy: JReich $
 * lastmodified $LastChangedDate: 2014-09-06 11:37:01 +0200 (Sat, 06 Sep 2014) $
 */

package com.actility.m2m.servlet.song.impl;

import java.net.InetAddress;
import java.net.URI;
import java.net.UnknownHostException;
import java.util.Arrays;
import java.util.HashMap;
import java.util.HashSet;
import java.util.Iterator;
import java.util.Map;
import java.util.NoSuchElementException;
import java.util.Set;
import java.util.SortedMap;
import java.util.TimerTask;
import java.util.TreeMap;

import javax.servlet.RequestDispatcher;
import javax.servlet.ServletContext;
import javax.servlet.ServletException;

import org.apache.log4j.Logger;

import com.actility.m2m.be.BackendEndpoint;
import com.actility.m2m.be.BackendException;
import com.actility.m2m.be.BackendService;
import com.actility.m2m.be.EndpointContext;
import com.actility.m2m.be.messaging.DeliveryChannel;
import com.actility.m2m.be.messaging.InOnly;
import com.actility.m2m.be.messaging.InOut;
import com.actility.m2m.be.messaging.MessagingException;
import com.actility.m2m.framework.resources.ResourcesAccessorService;
import com.actility.m2m.m2m.StatusCode;
import com.actility.m2m.servlet.NamespaceException;
import com.actility.m2m.servlet.ext.ExtApplicationSession;
import com.actility.m2m.servlet.ext.ExtProtocolServlet;
import com.actility.m2m.servlet.ext.ExtServletContext;
import com.actility.m2m.servlet.log.BundleLogger;
import com.actility.m2m.servlet.service.ext.ExtServletService;
import com.actility.m2m.servlet.song.ChannelClientListener;
import com.actility.m2m.servlet.song.ChannelServerListener;
import com.actility.m2m.servlet.song.LongPollingURIs;
import com.actility.m2m.servlet.song.SongServlet;
import com.actility.m2m.servlet.song.SongServletResponse;
import com.actility.m2m.servlet.song.SongURI;
import com.actility.m2m.servlet.song.binding.SongBindingFacade;
import com.actility.m2m.servlet.song.ext.SongNode;
import com.actility.m2m.servlet.song.internal.BindingNode;
import com.actility.m2m.servlet.song.internal.EndpointNode;
import com.actility.m2m.servlet.song.internal.InternalNode;
import com.actility.m2m.servlet.song.internal.SongContainerFacade;
import com.actility.m2m.servlet.song.message.InternalMessage;
import com.actility.m2m.servlet.song.message.InternalRequest;
import com.actility.m2m.servlet.song.message.LocalForwardedRequest;
import com.actility.m2m.servlet.song.message.LocalRequest;
import com.actility.m2m.servlet.song.message.RemoteRequest;
import com.actility.m2m.servlet.song.node.BindingEndpointNode;
import com.actility.m2m.servlet.song.node.ServletEndpointNode;
import com.actility.m2m.servlet.song.node.SessionNode;
import com.actility.m2m.servlet.song.node.SongBindingNode;
import com.actility.m2m.servlet.song.service.ext.SongStats;
import com.actility.m2m.transport.logger.TransportLoggerService;
import com.actility.m2m.util.ArrayUtils;
import com.actility.m2m.util.Profiler;
import com.actility.m2m.util.log.OSGiLogger;
import com.actility.m2m.xo.XoService;

public final class SongContainer implements BackendEndpoint, SongContainerFacade {
    private static final Logger LOG = OSGiLogger.getLogger(SongContainer.class, BundleLogger.getStaticLogger());

    private static final String SONG_PROTOCOL = "SONG";

    private final ExtServletService servletService;
    private final BackendService backendService;
    private final XoService xoService;
    private final ResourcesAccessorService resourcesAccessorService;
    private final TransportLoggerService transportLoggerService;
    private final Set localhostNames;
    private final BindingNode defaultBindingNode;

    // Not thread safe
    private EndpointContext context;
    /**
     * Maps a path to a servlet
     */
    private final SortedMap pathRoutes;
    /**
     * Maps a private path to a servlet. This path is not visible from outside.
     */
    private final SortedMap privatePathRoutes;
    /**
     * Maps external URI to internal context path
     */
    private final SortedMap externalAliasRoutes;
    /**
     * Maps URI schemes to binding node
     */
    private final Map schemeRoutes;
    /**
     * Proxy servlet
     */
    private ServletEndpointNode proxyServlet;

    private RouteConfiguration[] routesConfiguration;
    /**
     * Maximum pending remote requests
     */
    private long maxRemoteRequests;
    /**
     * Complete local host name of the machine with its domain
     */
    private final String completeHostName;
    /**
     * Name of the machine
     */
    private final String hostName;
    /**
     * Domain name of the machine
     */
    private final String domainName;

    /**
     * Default constructor for the SONG Container.
     *
     * @param servletService The generic servlet service
     * @param backendService The Backend service in charge of managing threads, message passing and queuing
     * @param xoService The XO service in charge of serializing objects
     * @param resourcesAccessorService The resources accessor service in charge of accessing low level resources
     * @param transportLoggerService The transport logger service in charge of logging SONG messages and compute statistics
     * @param routesConfiguration Defines IP routes on which the application is server capable or not
     * @param hostName The local host name
     * @param domainName The local domain name
     * @param maxRemoteRequests The maximum number of concurrent remote requests
     * @throws BackendException If any problem occurs while registering to the Bacnkend service
     * @throws UnknownHostException If a problem occurs while trying to detect the local hostname if not given
     */
    public SongContainer(ExtServletService servletService, BackendService backendService, XoService xoService,
            ResourcesAccessorService resourcesAccessorService, TransportLoggerService transportLoggerService,
            RouteConfiguration[] routesConfiguration, String hostName, String domainName, long maxRemoteRequests)
            throws BackendException, UnknownHostException {
        this.servletService = servletService;
        this.backendService = backendService;
        this.xoService = xoService;
        InternalMessage.setAvailableXoMediaTypes(xoService);
        this.resourcesAccessorService = resourcesAccessorService;
        this.transportLoggerService = transportLoggerService;
        this.pathRoutes = new TreeMap();
        this.privatePathRoutes = new TreeMap();
        this.externalAliasRoutes = new TreeMap();
        this.schemeRoutes = new HashMap();
        this.localhostNames = new HashSet();
        this.routesConfiguration = routesConfiguration;
        this.maxRemoteRequests = maxRemoteRequests;

        // Init localhost names
        addLocalhostAlias("127.0.0.1");
        addLocalhostAlias("localhost");
        addLocalhostAlias("localhost.localdomain");

        InetAddress localhost = InetAddress.getLocalHost();
        defaultBindingNode = new SongBindingNode(localhost, SongBindingNode.SONG_PORT);
        addLocalhostAlias(localhost.getHostAddress());
        addLocalhostAlias(localhost.getHostName());
        StringBuffer localHostName = new StringBuffer();
        String realDomainName = null;
        if ((hostName != null) && (!"".equals(hostName.trim()))) {
            localHostName.append(hostName);
        } else {
            localHostName.append(localhost.getHostName());
        }
        this.hostName = localHostName.toString();
        localHostName.append('.');
        if (domainName != null) {
            localHostName.append(domainName);
            realDomainName = domainName;
        } else {
            localHostName.append("localdomain");
            realDomainName = "localdomain";
        }
        this.domainName = realDomainName;
        this.completeHostName = localHostName.toString().toLowerCase();
        addLocalhostAlias(this.completeHostName);
        String[] managedSchemes = defaultBindingNode.getManagedSchemes();
        for (int i = 0; i < managedSchemes.length; ++i) {
            schemeRoutes.put(managedSchemes[i], defaultBindingNode);
        }
        backendService.registerEndpoint(this, null);
    }

    public void destroy() {
        try {
            backendService.unregisterEndpoint(context.getEndpointId());
        } catch (Exception e) {
            LOG.error("Exception thrown", e);
        } finally {
            synchronized (pathRoutes) {
                synchronized (privatePathRoutes) {
                    pathRoutes.clear();
                    privatePathRoutes.clear();
                }
            }
            synchronized (schemeRoutes) {
                schemeRoutes.clear();
            }
            context = null;
        }
    }

    protected static final class MatchResult {
        private final InternalNode songNode;
        private final String contextPath;
        private final String servletPath;
        private final String pathInfo;

        MatchResult(InternalNode songNode, String contextPath, String servletPath, String pathInfo) {
            this.songNode = songNode;
            this.contextPath = contextPath;
            this.servletPath = servletPath;
            this.pathInfo = pathInfo;
        }

        public InternalNode getSongNode() {
            return songNode;
        }

        public String getContextPath() {
            return contextPath;
        }

        public String getServletPath() {
            return servletPath;
        }

        public String getPathInfo() {
            return pathInfo;
        }
    }

    private void addLocalhostAlias(String alias) {
        if (alias != null) {
            this.localhostNames.add(alias);
        }
    }

    private String findBestPrefix(SortedMap map, String input) {
        String lastKey = null;
        boolean finished = false;
        String workingInput = input;
        SortedMap hmap = null;
        try {
            while (!finished) {
                hmap = map.headMap(workingInput);
                lastKey = (String) hmap.lastKey();
                if (lastKey != null) {
                    if (workingInput.startsWith(lastKey)) {
                        finished = true;
                    } else {
                        // Build the maximum common part of the 2 strings
                        workingInput = buildMaxCommonPart(lastKey, workingInput);
                    }
                } else {
                    finished = true;
                }
            }
        } catch (NoSuchElementException e) {
            lastKey = null;
        }
        return lastKey;
    }

    private String buildMaxCommonPart(String foundPath, String realPath) {
        int offset = 0;
        while (foundPath.charAt(offset) == realPath.charAt(offset)) {
            ++offset;
        }
        return foundPath.substring(0, offset) + "/";
    }

    private MatchResult resolveExternalAlias(String id, URI uri) {
        // 1. Try to get the path directly
        // Search for a song servlet
        String externalAlias = uri.toString();
        String externalContextPath = null;
        MatchResult result = null;
        String bestPrefix = null;
        String internalContextPath = null;

        int slashOffset = 0;
        if (!externalAlias.endsWith("/")) {
            slashOffset = 1;
            externalAlias += "//";
        } else {
            externalAlias += "/";
        }

        if (LOG.isInfoEnabled()) {
            LOG.info(id + ": Resolve with external alias \"" + externalAlias + "\"");
        }
        synchronized (externalAliasRoutes) {
            bestPrefix = findBestPrefix(externalAliasRoutes, externalAlias);
            if (bestPrefix != null) {
                internalContextPath = (String) externalAliasRoutes.get(bestPrefix);
            }
        }

        if (internalContextPath != null) {
            String remainingPath = "";
            if ((externalAlias.length() - slashOffset) >= (bestPrefix.length() + 1)) {
                remainingPath = externalAlias.substring(bestPrefix.length() - 1, externalAlias.length() - 1 - slashOffset);
            }
            externalContextPath = uri.getPath().substring(0, uri.getPath().length() - remainingPath.length());
            URI newTarget = URI.create(internalContextPath + remainingPath);
            if (LOG.isInfoEnabled()) {
                LOG.info(id + ": External alias resolved to local path \"" + newTarget + "\"");
            }
            result = resolveLocalServlet(id, externalContextPath, newTarget, false);
        } else if (LOG.isInfoEnabled()) {
            LOG.info(id + ": External alias cannot be resolved");
        }
        return result;
    }

    private MatchResult resolveLocalServlet(String id, String contextPath, URI uri, boolean localRequest) {
        // 1. Try to get the path directly
        // Search for a song servlet
        String realPath = uri.getPath();
        // Need at least a double slash at the end
        if (!realPath.endsWith("/")) {
            realPath += "//";
        } else {
            realPath += "/";
        }

        if (LOG.isInfoEnabled()) {
            LOG.info(id + ": Resolve with local path \"" + realPath + "\"");
        }
        MatchResult result = null;
        String bestPrefix = null;
        InternalNode node = null;
        String servletPath = null;
        String pathInfo = null;
        if (localRequest) {
            synchronized (privatePathRoutes) {
                bestPrefix = findBestPrefix(privatePathRoutes, realPath);
                if (bestPrefix != null) {
                    node = (InternalNode) privatePathRoutes.get(bestPrefix);
                    pathInfo = "/";
                    if (realPath.length() > (bestPrefix.length() + 1)) {
                        pathInfo = realPath.substring(bestPrefix.length() - 1, uri.getPath().length());
                    }
                    servletPath = bestPrefix.substring(0, bestPrefix.length() - 1);
                }
            }
        }
        if (node == null) {
            synchronized (pathRoutes) {
                bestPrefix = findBestPrefix(pathRoutes, realPath);
                if (bestPrefix != null) {
                    node = (InternalNode) pathRoutes.get(bestPrefix);
                    pathInfo = "/";
                    if (realPath.length() > (bestPrefix.length() + 1)) {
                        pathInfo = realPath.substring(bestPrefix.length() - 1, uri.getPath().length());
                    }
                    servletPath = bestPrefix.substring(0, bestPrefix.length() - 1);
                }
            }
        }

        if (node != null) {
            if (LOG.isInfoEnabled()) {
                LOG.info(id + ": Local path resolved to servlet \"" + node.getTarget() + "\"");
            }
            String realServletPath = servletPath.substring(node.getContextPath().length());
            result = new MatchResult(node, (contextPath != null) ? contextPath : node.getContextPath(), realServletPath,
                    pathInfo);
        } else if (LOG.isInfoEnabled()) {
            LOG.info(id + ": Local path cannot be resolved");
        }
        return result;
    }

    private MatchResult resolveScheme(String id, String scheme) {
        // Search for a binding servlet
        if (LOG.isInfoEnabled()) {
            LOG.info(id + ": Resolve with scheme \"" + scheme + "\"");
        }
        BindingNode endpoint = (BindingNode) schemeRoutes.get(scheme);
        if (endpoint != null) {
            if (LOG.isInfoEnabled()) {
                LOG.info(id + ": Scheme resolved to binding \"" + endpoint.getTarget() + "\"");
            }
            return new MatchResult(endpoint, null, null, null);
        }
        if (LOG.isInfoEnabled()) {
            LOG.info(id + ": Scheme cannot be resolved");
        }
        return null;
    }

    private MatchResult resolveProxyNode(String id, URI uri) {
        // 1. Try to get the path directly
        if (LOG.isInfoEnabled()) {
            LOG.info(id + ": Resolve proxy request with host \"" + uri.getHost() + "\"");
        }
        MatchResult result = null;
        if (localhostNames.contains(uri.getHost())) {
            if (LOG.isInfoEnabled()) {
                LOG.info(id + ": Host corresponds to the local server");
                LOG.info(id + ": Proxy request host resolved to proxy servlet \"" + proxyServlet.getTarget() + "\"");
            }
            result = new MatchResult(proxyServlet, null, null, null);
        } else {
            if (LOG.isInfoEnabled()) {
                LOG.info(id + ": Host corresponds to a remote server");
            }
            result = resolveScheme(id, uri.getScheme());
        }
        return result;
    }

    private MatchResult resolveNode(String id, URI uri, boolean localRequest) {
        // 1. Try to get the path directly
        if (LOG.isInfoEnabled()) {
            LOG.info(id + ": Resolve request with host \"" + uri.getHost() + "\"");
        }
        MatchResult result = null;
        if (localhostNames.contains(uri.getHost())) {
            if (LOG.isInfoEnabled()) {
                LOG.info(id + ": Host corresponds to the local server. Resolve request with port \"" + uri.getPort() + "\"");
            }
            result = resolveScheme(id, uri.getScheme());
            if (result != null) {
                BindingNode bindingNode = (BindingNode) result.getSongNode();
                if (uri.getPort() == bindingNode.getPort() || uri.getPort() == bindingNode.getServerURIPort()) {
                    if (LOG.isInfoEnabled()) {
                        LOG.info(id + ": Port corresponds to a local application");
                    }
                    result = resolveLocalServlet(id, null, uri.normalize(), localRequest);
                } else {
                    if (LOG.isInfoEnabled()) {
                        LOG.info(id + ": Port corresponds to an external application. Forward request to binding");
                    }
                }
            }
        } else {
            if (LOG.isInfoEnabled()) {
                LOG.info(id + ": Host corresponds to a remote server");
            }
            result = resolveScheme(id, uri.getScheme());
        }
        return result;
    }

    private String buildServletName(String applicationName, String servletName) {
        return applicationName + "." + servletName;
    }

    private void registerInternalServlet(String fullServletName, String servletName, EndpointNode endpoint, Map initParams,
            Map configuration) throws ServletException {
        try {
            ExtServletContext servletContext = endpoint.getServletContext();
            if (ServletEndpointNode.class.equals(endpoint.getClass())) {
                if (servletContext.getAttribute(SongServlet.SONG_FACTORY) == null) {
                    SongServletUtils servletUtils = new SongServletUtils(this, endpoint.getServletContext());
                    servletContext.setAttribute(SongServlet.SONG_FACTORY, servletUtils);
                    servletContext.setAttribute(SongServlet.SONG_DYNAMIC_ROUTER, servletUtils);
                    servletContext.setAttribute(SongServlet.HOST_NAME, hostName);
                    servletContext.setAttribute(SongServlet.DOMAIN_NAME, domainName);
                }
            } else {
                if (servletContext.getAttribute(SongServlet.SONG_BINDING_FACTORY) == null) {
                    servletContext.setAttribute(SongServlet.SONG_BINDING_FACTORY,
                            new SongBindingUtils(this, endpoint.getServletContext(), (BindingEndpointNode) endpoint));
                }
            }
            backendService.registerEndpoint(endpoint, configuration);
            // // TODO should wait for the endpoint init before

            // Must end with this as the servlet is initialized
            endpoint.getServletContext().addServlet(servletName, endpoint, initParams);
        } catch (ServletException e) {
            backendService.unregisterEndpoint(endpoint.getContext().getEndpointId());
            throw e;
        } catch (BackendException e) {
            throw new ServletException("Problem while registering the servlet to the Backend service", e);
        }
    }

    private boolean isPrivatePath(Map configuration) {
        if (configuration != null) {
            return Boolean.TRUE.equals(configuration.get("com.actility.servlet.song.PrivatePath"));
        }
        return false;
    }

    private int getPriority(Map configuration, boolean binding) {
        if (configuration != null) {
            Integer priority = (Integer) configuration.get("com.actility.servlet.song.Priority");
            if (priority != null) {
                switch (priority.intValue()) {
                case DeliveryChannel.CRITICAL:
                    return DeliveryChannel.CRITICAL;
                case DeliveryChannel.IMPORTANT:
                    return DeliveryChannel.IMPORTANT;
                case DeliveryChannel.NORMAL:
                    return DeliveryChannel.NORMAL;
                }
            }
        }
        return (binding) ? DeliveryChannel.NORMAL : DeliveryChannel.IMPORTANT;
    }

    private ServletEndpointNode getServletEndpoint(EndpointNode endpoint) throws ServletException {
        if (ServletEndpointNode.class != endpoint.getClass()) {
            throw new ServletException("Can add a path on that kind of servlet: " + endpoint.getTarget());
        }
        return (ServletEndpointNode) endpoint;
    }

    private void addPath(String fullPath, String path, boolean privatePath, ServletEndpointNode servletEndpoint,
            SongNode servletNode) throws NamespaceException {
        synchronized (pathRoutes) {
            synchronized (privatePathRoutes) {
                if (pathRoutes.containsKey(fullPath) || privatePathRoutes.containsKey(fullPath)) {
                    throw new NamespaceException("Path is already in use can't use the given servlet: " + fullPath);
                }
                if (privatePath) {
                    privatePathRoutes.put(fullPath, servletNode);
                } else {
                    pathRoutes.put(fullPath, servletNode);
                }
                servletEndpoint.addPath(path);
            }
        }
    }

    public XoService getXoService() {
        return xoService;
    }

    public ResourcesAccessorService getResourcesAccessorService() {
        return resourcesAccessorService;
    }

    public String getLocalHostName() {
        return completeHostName;
    }

    public String getProtocolName() {
        return SONG_PROTOCOL;
    }

    public void setRoutesConfiguration(RouteConfiguration[] routesConfiguration) {
        if (Arrays.equals(routesConfiguration, this.routesConfiguration)) {
            if (LOG.isInfoEnabled()) {
                LOG.info("Modifying routesConfiguration from " + ArrayUtils.toString(this.routesConfiguration, "[", "]", ", ")
                        + " to " + ArrayUtils.toString(routesConfiguration, "[", "]", ", "));
            }
            this.routesConfiguration = routesConfiguration;
        }
    }

    public void setMaxRemoteRequests(long maxRemoteRequests) {
        synchronized (songStats) {
            if (maxRemoteRequests != this.maxRemoteRequests) {
                if (LOG.isInfoEnabled()) {
                    LOG.info("Modifying maxRemoteRequests from " + this.maxRemoteRequests + " to " + maxRemoteRequests);
                }
                this.maxRemoteRequests = maxRemoteRequests;
            }
        }
    }

    public RequestDispatcher getRequestDispatcher(ExtProtocolServlet servlet) {
        if (!(servlet instanceof EndpointNode)) {
            return null;
        }
        EndpointNode endpoint = (EndpointNode) servlet;
        return new SongRequestDispatcher(endpoint);
    }

    public EndpointContext getContext() {
        return context;
    }

    public void init(EndpointContext context) {
        this.context = context;
    }

    public void process(InOut exchange) {
        // long process = -System.currentTimeMillis();
        if (LOG.isDebugEnabled()) {
            LOG.debug(exchange.getExchangeId() + ": Router process InOut exchange");
        }
        if (exchange.getOutMessage() != null) {
            // TODO is this possible now ?
            LOG.error(exchange.getExchangeId() + ": Request is already responded, it will not be routed.");
            return;
        }
        Object message = exchange.getInMessage().getContent();
        Class messageClass = message.getClass();
        if (LocalRequest.class != messageClass && RemoteRequest.class != messageClass
                && LocalForwardedRequest.class != messageClass) {
            throw new IllegalArgumentException("Received a message which is not a SONG request");
        }
        int errorStatusCode = 0;
        String errorReasonPhrase = null;
        StatusCode errorM2MStatus = null;
        String errorMessage = null;
        Exception errorException = null;

        InternalRequest request = (InternalRequest) message;
        if (Profiler.getInstance().isTraceEnabled()) {
            Profiler.getInstance().trace(
                    request.getId() + ": >>> Router: " + request.getMethod() + " (requestingEntity: "
                            + request.getRequestingEntity().absoluteURI() + ") (targetID: "
                            + request.getTargetID().absoluteURI() + ")");
        }
        if (LOG.isInfoEnabled()) {
            LOG.info(request.getId() + ": >>> Router: " + request.getMethod() + " (requestingEntity: "
                    + request.getRequestingEntity().absoluteURI() + ") (targetID: " + request.getTargetID().absoluteURI() + ")");
        }

        long now = System.currentTimeMillis();
        if (now < request.getExpirationTime()) {
            // Resolve exchange
            try {
                MatchResult matchingResult = null;
                if (RemoteRequest.class == messageClass) {
                    if (request.isProxy()) {
                        matchingResult = new MatchResult(proxyServlet, null, null, null);
                    } else {
                        matchingResult = resolveLocalServlet(request.getId(), null, request.getTargetID().toURI().normalize(),
                                false);
                    }
                } else if (request.getProxy().getProxyTo() != null) {
                    matchingResult = resolveProxyNode(request.getId(), request.getProxy().getProxyTo().toURI().normalize());
                } else {
                    SongURI targetURI = request.getTargetID();
                    if (LocalRequest.class == messageClass) {
                        LocalRequest localRequest = (LocalRequest) request;
                        if (localRequest.getPoc() != null) {
                            targetURI = localRequest.getPoc();
                        }
                    } else {
                        LocalForwardedRequest localRequest = (LocalForwardedRequest) request;
                        if (localRequest.getPoc() != null) {
                            targetURI = localRequest.getPoc();
                        }
                    }
                    matchingResult = resolveNode(request.getId(), targetURI.toURI().normalize(), true);
                }

                if (matchingResult != null) {
                    matchingResult.getSongNode().update(exchange, request, null, matchingResult.getContextPath(),
                            matchingResult.getServletPath(), matchingResult.getPathInfo());
                    if (Profiler.getInstance().isTraceEnabled()) {
                        Profiler.getInstance().trace(
                                request.getId() + ": <<< Router: " + request.getMethod() + " (requestingEntity: "
                                        + request.getRequestingEntity().absoluteURI() + ") (targetID: "
                                        + request.getTargetID().absoluteURI() + ") (forwardedTo: "
                                        + matchingResult.getSongNode().getTarget() + ")");
                    }
                    if (LOG.isInfoEnabled()) {
                        LOG.info(request.getId() + ": <<< Router: " + request.getMethod() + " (requestingEntity: "
                                + request.getRequestingEntity().absoluteURI() + ") (targetID: "
                                + request.getTargetID().absoluteURI() + ") (forwardedTo: "
                                + matchingResult.getSongNode().getTarget() + ")");
                    }
                    context.getDeliveryChannel().send(exchange, DeliveryChannel.IMPORTANT);
                } else {
                    if (LOG.isInfoEnabled()) {
                        LOG.info(request.getId() + ": Could not find a matching servlet for this request");
                    }
                    errorStatusCode = SongServletResponse.SC_NOT_FOUND;
                    errorReasonPhrase = SongServletResponse.RP_NOT_FOUND;
                    errorM2MStatus = StatusCode.STATUS_NOT_FOUND;
                    errorMessage = "Unknown path: " + request.getTargetID().getPath();
                }
            } catch (MessagingException e) {
                LOG.error(request.getId() + ": Problem while processing the InOut exchange", e);
                if (e.isTemporary()) {
                    errorStatusCode = SongServletResponse.SC_SERVICE_UNAVAILABLE;
                    errorReasonPhrase = SongServletResponse.RP_SERVICE_UNAVAILABLE;
                    errorM2MStatus = StatusCode.STATUS_SERVICE_UNAVAILABLE;
                    errorMessage = "Too much waiting events";
                    errorException = e;
                } else {
                    errorStatusCode = SongServletResponse.SC_INTERNAL_SERVER_ERROR;
                    errorReasonPhrase = SongServletResponse.RP_INTERNAL_SERVER_ERROR;
                    errorM2MStatus = StatusCode.STATUS_INTERNAL_SERVER_ERROR;
                    errorMessage = "Cannot send internal event to destination";
                    errorException = e;
                }
            }
        } else {
            errorStatusCode = SongServletResponse.SC_GATEWAY_TIMEOUT;
            errorReasonPhrase = SongServletResponse.RP_GATEWAY_TIMEOUT;
            errorM2MStatus = StatusCode.STATUS_GATEWAY_TIMEOUT;
            errorMessage = "Request expired before reaching final destination";
        }

        if (errorStatusCode != 0) {
            // Respond to request with an error
            SongServletResponse response = null;
            try {
                if (LocalRequest.class == messageClass) {
                    LocalRequest localRequest = (LocalRequest) request;
                    response = localRequest.createResponse(errorStatusCode, errorReasonPhrase, errorM2MStatus, errorMessage,
                            errorException);
                    localRequest.sendSyncResponse(response);
                } else if (LocalForwardedRequest.class == messageClass) {
                    LocalForwardedRequest localForwardedRequest = (LocalForwardedRequest) request;
                    response = localForwardedRequest.createResponse(errorStatusCode, errorReasonPhrase, errorM2MStatus,
                            errorMessage, errorException);
                    localForwardedRequest.sendSyncResponse(response);
                } else {
                    RemoteRequest remoteRequest = (RemoteRequest) request;
                    response = remoteRequest.createResponse(errorStatusCode, errorReasonPhrase, errorM2MStatus, errorMessage,
                            errorException);
                    remoteRequest.sendSyncResponse(response);
                }
            } catch (MessagingException e) {
                LOG.error(request.getId() + ": Cannot send the error response \"" + errorStatusCode + "\"", e);
            }
        }
        // process += System.currentTimeMillis();
        // LOG.error("process: " + process);
    }

    public void process(InOnly exchange) {
        if (LOG.isInfoEnabled()) {
            LOG.info(exchange.getExchangeId() + ": Router process InOnly exchange");
        }
        // Nothing to do
    }

    public void removeExternalAlias(SongURI contactURI, String contextPath) {
        String aliasURI = contactURI.absoluteURI();
        if (aliasURI.endsWith("/")) {
            aliasURI = aliasURI.substring(0, aliasURI.length() - 1);
        }
        synchronized (externalAliasRoutes) {
            externalAliasRoutes.put(aliasURI, contextPath);
        }
    }

    public RouteConfiguration[] getRoutesConfiguration() {
        return routesConfiguration;
    }

    public SongNode getProxyNode() {
        return proxyServlet;
    }

    public Iterator getSchemeRoutes() {
        synchronized (schemeRoutes) {
            return schemeRoutes.entrySet().iterator();
        }
    }

    public Iterator getPathRoutes() {
        synchronized (pathRoutes) {
            return pathRoutes.entrySet().iterator();
        }
    }

    public Iterator getPrivatePathRoutes() {
        synchronized (privatePathRoutes) {
            return privatePathRoutes.entrySet().iterator();
        }
    }

    public Iterator getExternalAliasRoutes() {
        synchronized (externalAliasRoutes) {
            return externalAliasRoutes.entrySet().iterator();
        }
    }

    public BindingNode getBindingNode(String scheme) {
        synchronized (schemeRoutes) {
            return (BindingNode) schemeRoutes.get(scheme);
        }
    }

    public BindingNode getDefaultBindingNode() {
        return defaultBindingNode;
    }

    public void getDescription(StringBuffer buf, ExtProtocolServlet servlet) {
        if (servlet instanceof EndpointNode) {
            EndpointNode endpoint = (EndpointNode) servlet;
            buf.append("  { SONG: ").append(endpoint.getSongServlet().getServletName())
                    .append(System.getProperty("line.separator"));
            if (ServletEndpointNode.class == endpoint.getClass()) {
                ServletEndpointNode servletEndpoint = (ServletEndpointNode) endpoint;
                if (!servletEndpoint.getPaths().isEmpty()) {
                    Iterator it = servletEndpoint.getPaths().iterator();
                    String path = null;
                    while (it.hasNext()) {
                        path = (String) it.next();
                        buf.append("    { path: ").append(path).append(" }").append(System.getProperty("line.separator"));
                    }
                }
                if (servletEndpoint == proxyServlet) {
                    buf.append("    { proxyServlet }").append(System.getProperty("line.separator"));
                }
            } else {
                BindingEndpointNode bindingEndpoint = (BindingEndpointNode) endpoint;
                buf.append(System.getProperty("line.separator"));
                String[] managedSchemes = bindingEndpoint.getManagedSchemes();
                buf.append("    { binding: ");
                for (int i = 0; i < managedSchemes.length; ++i) {
                    buf.append(managedSchemes[i]).append(' ');
                }
                buf.append('}').append(System.getProperty("line.separator"));
            }
            buf.append("  }").append(System.getProperty("line.separator"));
        }
    }

    public String toString() {
        return "SONG.Router";
    }

    public void scheduleTimer(TimerTask timer, long millis) {
        servletService.scheduleTimer(timer, millis);
    }

    public void addPath(String contextPath, String path, EndpointNode endpoint, Map configuration) throws ServletException,
            NamespaceException {
        String realPath = contextPath + path;
        String normalizedPath = path;
        if (!realPath.endsWith("/")) {
            realPath += "/";
            normalizedPath += "/";
        }
        if (LOG.isInfoEnabled()) {
            LOG.info(endpoint.getTarget() + ": addPath " + realPath);
        }

        ServletEndpointNode servletEnpoint = getServletEndpoint(endpoint);
        addPath(realPath, normalizedPath, isPrivatePath(configuration), servletEnpoint, servletEnpoint);
    }

    public void addSessionPath(String contextPath, String path, EndpointNode endpoint, ExtApplicationSession appSession,
            Map configuration) throws ServletException, NamespaceException {
        String realPath = contextPath + path;
        String normalizedPath = path;
        if (!realPath.endsWith("/")) {
            realPath += "/";
            normalizedPath += "/";
        }
        if (LOG.isInfoEnabled()) {
            LOG.info(endpoint.getTarget() + ": addSessionPath " + realPath);
        }

        ServletEndpointNode servletEnpoint = getServletEndpoint(endpoint);
        SessionNode sessionNode = new SessionNode(servletEnpoint, appSession);
        addPath(realPath, normalizedPath, isPrivatePath(configuration), servletEnpoint, sessionNode);
    }

    public void removePath(String contextPath, String path) {
        if (path == null) {
            return;
        }

        String realPath = contextPath + path;
        String normalizedPath = path;
        if (!realPath.endsWith("/")) {
            realPath += "/";
            normalizedPath += "/";
        }
        if (LOG.isInfoEnabled()) {
            LOG.info(contextPath + ": unregisterPath " + realPath);
        }

        SongNode node = null;
        synchronized (pathRoutes) {
            synchronized (privatePathRoutes) {
                node = (SongNode) pathRoutes.remove(realPath);
                if (node == null) {
                    node = (SongNode) privatePathRoutes.remove(realPath);
                }
            }
        }
        if (node != null) {
            if (ServletEndpointNode.class == node.getClass()) {
                ((ServletEndpointNode) node).removePath(normalizedPath);
            }
        }
    }

    public void registerBindingServlet(ServletContext context, String servletName, SongServlet servlet, Map initParams,
            SongBindingFacade facade, String serverScheme, int serverPort, String[] managedSchemes, boolean longPollSupported,
            String defaultProtocol, InetAddress address, int port, Map configuration) throws NamespaceException,
            ServletException {
        String fullServletName = buildServletName(context.getServletContextName(), servletName);
        if (LOG.isInfoEnabled()) {
            LOG.info("registerBindingServlet " + fullServletName);
        }

        BindingEndpointNode endpoint = new BindingEndpointNode(this, servlet, (ExtServletContext) context, servletName,
                getPriority(configuration, true), facade, serverScheme, serverPort, managedSchemes, longPollSupported,
                defaultProtocol, address, port);

        registerInternalServlet(fullServletName, servletName, endpoint, initParams, configuration);
        String scheme = null;
        synchronized (schemeRoutes) {
            for (int i = 0; i < managedSchemes.length; ++i) {
                scheme = managedSchemes[i];
                if (!schemeRoutes.containsKey(scheme)) {
                    schemeRoutes.put(scheme, endpoint);
                } else {
                    for (int j = i; j > 0; --j) {
                        schemeRoutes.remove(managedSchemes[j - 1]);
                    }
                    throw new NamespaceException("Servlet for the scheme " + scheme + " already exists");
                }
            }
        }
    }

    public void registerServiceServlet(ServletContext context, String servletName, SongServlet servlet, Map initParams,
            Map configuration) throws ServletException {
        String fullServletName = buildServletName(context.getServletContextName(), servletName);
        if (LOG.isInfoEnabled()) {
            LOG.info("registerServlet " + fullServletName);
        }

        ServletEndpointNode endpoint = new ServletEndpointNode(this, servlet, (ExtServletContext) context, servletName,
                getPriority(configuration, false));
        registerInternalServlet(fullServletName, servletName, endpoint, initParams, configuration);
    }

    public void registerServiceServlet(ServletContext context, String path, String servletName, SongServlet servlet,
            Map initParams, Map configuration) throws NamespaceException, ServletException {
        String fullServletName = buildServletName(context.getServletContextName(), servletName);
        String realPath = context.getContextPath() + path;
        String normalizedPath = path;
        if (!realPath.endsWith("/")) {
            realPath += "/";
            normalizedPath += "/";
        }
        if (LOG.isInfoEnabled()) {
            LOG.info("registerServlet " + fullServletName + " with path " + realPath);
        }

        ServletEndpointNode endpoint = new ServletEndpointNode(this, servlet, (ExtServletContext) context, servletName,
                getPriority(configuration, false));

        registerInternalServlet(fullServletName, servletName, endpoint, initParams, configuration);
        synchronized (pathRoutes) {
            synchronized (privatePathRoutes) {
                if (pathRoutes.containsKey(realPath) || privatePathRoutes.containsKey(realPath)) {
                    throw new NamespaceException("Path is already in use can't use the given servlet: " + realPath);
                }
                if (isPrivatePath(configuration)) {
                    privatePathRoutes.put(realPath, endpoint);
                } else {
                    pathRoutes.put(realPath, endpoint);
                }
                endpoint.addPath(normalizedPath);
            }
        }
    }

    public void registerProxyServlet(ServletContext context, String servletName, SongServlet servlet, Map initParams,
            Map configuration) throws ServletException {
        String fullServletName = buildServletName(context.getServletContextName(), servletName);
        if (LOG.isInfoEnabled()) {
            LOG.info("registerProxyServlet " + fullServletName);
        }
        ServletEndpointNode endpoint = new ServletEndpointNode(this, servlet, (ExtServletContext) context, servletName,
                getPriority(configuration, false));

        registerInternalServlet(fullServletName, servletName, endpoint, initParams, configuration);
        synchronized (this) {
            if (proxyServlet != null) {
                throw new ServletException("A default proxy servlet is already registered. Cannot register servlet: "
                        + fullServletName);
            }
            proxyServlet = endpoint;
        }
    }

    public void destroyEndpoint(EndpointNode endpoint) {
        if (LOG.isInfoEnabled()) {
            LOG.info(endpoint.getTarget() + ": unregisterServlet");
        }

        if (BindingEndpointNode.class == endpoint.getClass()) {
            BindingEndpointNode bindingEndpoint = (BindingEndpointNode) endpoint;
            String[] managedSchemes = bindingEndpoint.getManagedSchemes();
            synchronized (schemeRoutes) {
                for (int i = 0; i < managedSchemes.length; ++i) {
                    schemeRoutes.remove(managedSchemes[i]);
                }
            }
        } else if (ServletEndpointNode.class == endpoint.getClass()) {
            ServletEndpointNode servletEndpoint = (ServletEndpointNode) endpoint;
            Iterator it = servletEndpoint.getPaths().iterator();
            String registration = null;
            synchronized (pathRoutes) {
                synchronized (privatePathRoutes) {
                    while (it.hasNext()) {
                        registration = (String) it.next();
                        pathRoutes.remove(servletEndpoint.getServletContext().getContextPath() + registration);
                        privatePathRoutes.remove(servletEndpoint.getServletContext().getContextPath() + registration);
                    }
                }
            }
            synchronized (this) {
                if (proxyServlet == servletEndpoint) {
                    proxyServlet = null;
                }
            }
        }
        backendService.unregisterEndpoint(endpoint.getContext().getEndpointId());
    }

    private SongBindingFacade getSongBindingFacade(SongURI songURI) throws ServletException {
        BindingNode bindingNode = getBindingNode(songURI.getScheme());
        if (bindingNode != null) {
            return bindingNode.getSongBindingFacade();
        }
        throw new ServletException("Scheme " + songURI.getScheme() + " does not a corresponding binding");
    }

    public LongPollingURIs createServerNotificationChannel(SongURI serverURI, ChannelServerListener listener)
            throws ServletException {
        if (LOG.isInfoEnabled()) {
            LOG.info("Create server <notificationChannel> connection on " + serverURI.absoluteURI());
        }
        SongBindingFacade facade = getSongBindingFacade(serverURI);
        if (facade != null) {
            return facade.createServerNotificationChannel(serverURI, listener);
        }
        throw new UnsupportedOperationException("SONG binding does not support <notificationChannel> connections");
    }

    public void createServerNotificationChannel(SongURI contactURI, SongURI longPollingURI, ChannelServerListener listener)
            throws ServletException {
        if (LOG.isInfoEnabled()) {
            LOG.info("Create server <notificationChannel> connection with contact " + contactURI.absoluteURI()
                    + " and long polling " + longPollingURI.absoluteURI());
        }
        SongBindingFacade facade = getSongBindingFacade(contactURI);
        if (facade != null) {
            facade.createServerNotificationChannel(contactURI, longPollingURI, listener);
        } else {
            throw new UnsupportedOperationException("SONG binding does not support <notificationChannel> connections");
        }
    }

    public void deleteServerNotificationChannel(SongURI contactURI, SongURI longPollingURI) {
        if (LOG.isInfoEnabled()) {
            LOG.info("Delete server <notificationChannel> connection on contact " + contactURI.absoluteURI()
                    + " and long polling " + longPollingURI.absoluteURI());
        }
        try {
            SongBindingFacade facade = getSongBindingFacade(contactURI);
            facade.deleteServerNotificationChannel(contactURI, longPollingURI);
        } catch (ServletException e) {
            LOG.warn("Servlet exception while deleting the server <notificationChannel> connection", e);
        } catch (UnsupportedOperationException e) {
            LOG.warn("Try to delete a server <notificationChannel> connection on a binding which does not support this", e);
        }
    }

    public void createClientNotificationChannel(SongURI contactURI, SongURI longPollingURI, SongURI requestingEntity,
            SongURI relatedRequestingEntity, SongURI relatedTargetID, String contextPath, ChannelClientListener listener)
            throws ServletException {
        if (LOG.isInfoEnabled()) {
            LOG.info("Create client <notificationChannel> connection with contact " + contactURI.absoluteURI()
                    + " and long polling " + longPollingURI.absoluteURI());
        }
        SongBindingFacade facade = getSongBindingFacade(contactURI);
        if (facade != null) {
            String aliasURI = contactURI.absoluteURI();
            if (!aliasURI.endsWith("/")) {
                aliasURI += "/";
            }
            synchronized (externalAliasRoutes) {
                facade.createClientNotificationChannel(contactURI, longPollingURI, requestingEntity, relatedRequestingEntity,
                        relatedTargetID, listener);
                externalAliasRoutes.put(aliasURI, contextPath);
            }
        } else {
            throw new UnsupportedOperationException("SONG binding does not support <notificationChannel> connections");
        }
    }

    public void deleteClientNotificationChannel(SongURI contactURI, SongURI longPollingURI) {
        if (LOG.isInfoEnabled()) {
            LOG.info("Delete client <notificationChannel> connection on contact " + contactURI.absoluteURI()
                    + " and long polling " + longPollingURI.absoluteURI());
        }
        try {
            SongBindingFacade facade = getSongBindingFacade(contactURI);
            if (facade != null) {
                String aliasURI = contactURI.absoluteURI();
                if (!aliasURI.endsWith("/")) {
                    aliasURI += "/";
                }
                synchronized (externalAliasRoutes) {
                    externalAliasRoutes.remove(aliasURI);
                }
                facade.deleteClientNotificationChannel(contactURI, longPollingURI);
            }
        } catch (ServletException e) {
            LOG.warn("Servlet exception while deleting the client <notificationChannel> connection", e);
        } catch (UnsupportedOperationException e) {
            LOG.warn("Try to delete a client <notificationChannel> connection on a binding which does not support this", e);
        }
    }

    public LongPollingURIs createServerCommunicationChannel(SongURI serverURI, ChannelServerListener listener)
            throws ServletException {
        if (LOG.isInfoEnabled()) {
            LOG.info("Create server <communicationChannel> connection on " + serverURI.absoluteURI());
        }
        SongBindingFacade facade = getSongBindingFacade(serverURI);
        if (facade != null) {
            return facade.createServerCommunicationChannel(serverURI, listener);
        }
        throw new UnsupportedOperationException("SONG binding does not support <communicationChannel> connections");
    }

    public void createServerCommunicationChannel(SongURI contactURI, SongURI longPollingURI, ChannelServerListener listener)
            throws ServletException {
        if (LOG.isInfoEnabled()) {
            LOG.info("Create server <communicationChannel> connection with contact " + contactURI.absoluteURI()
                    + " and long polling " + longPollingURI.absoluteURI());
        }
        SongBindingFacade facade = getSongBindingFacade(contactURI);
        if (facade != null) {
            facade.createServerCommunicationChannel(contactURI, longPollingURI, listener);
        } else {
            throw new UnsupportedOperationException("SONG binding does not support <communicationChannel> connections");
        }
    }

    public void deleteServerCommunicationChannel(SongURI contactURI, SongURI longPollingURI) {
        if (LOG.isInfoEnabled()) {
            LOG.info("Delete server <communicationChannel> connection on contact " + contactURI.absoluteURI()
                    + " and long polling " + longPollingURI.absoluteURI());
        }
        try {
            SongBindingFacade facade = getSongBindingFacade(contactURI);
            facade.deleteServerCommunicationChannel(contactURI, longPollingURI);
        } catch (ServletException e) {
            LOG.warn("Servlet exception while deleting the server <communicationChannel> connection", e);
        } catch (UnsupportedOperationException e) {
            LOG.warn("Try to delete a server <communicationChannel> connection on a binding which does not support this", e);
        }
    }

    public void createClientCommunicationChannel(SongURI contactURI, SongURI longPollingURI, SongURI requestingEntity,
            String contextPath, ChannelClientListener listener) throws ServletException {
        if (LOG.isInfoEnabled()) {
            LOG.info("Create client <communicationChannel> connection with contact " + contactURI.absoluteURI()
                    + " and long polling " + longPollingURI.absoluteURI());
        }
        SongBindingFacade facade = getSongBindingFacade(contactURI);
        if (facade != null) {
            String aliasURI = contactURI.absoluteURI();
            if (!aliasURI.endsWith("/")) {
                aliasURI += "/";
            }
            synchronized (externalAliasRoutes) {
                facade.createClientCommunicationChannel(contactURI, longPollingURI, requestingEntity, listener);
                externalAliasRoutes.put(aliasURI, contextPath);
            }
        } else {
            throw new UnsupportedOperationException("SONG binding does not support <communicationChannel> connections");
        }
    }

    public void deleteClientCommunicationChannel(SongURI contactURI, SongURI longPollingURI) {
        if (LOG.isInfoEnabled()) {
            LOG.info("Delete client <comunicationChannel> connection on contact " + contactURI.absoluteURI()
                    + " and long polling " + longPollingURI.absoluteURI());
        }
        try {
            SongBindingFacade facade = getSongBindingFacade(contactURI);
            if (facade != null) {
                String aliasURI = contactURI.absoluteURI();
                if (!aliasURI.endsWith("/")) {
                    aliasURI += "/";
                }
                synchronized (externalAliasRoutes) {
                    externalAliasRoutes.remove(aliasURI);
                }
                facade.deleteClientCommunicationChannel(contactURI, longPollingURI);
            }
        } catch (ServletException e) {
            LOG.warn("Servlet exception while deleting the client <communicationChannel> connection", e);
        } catch (UnsupportedOperationException e) {
            LOG.warn("Try to delete a client <communicationChannel> connection on a binding which does not support this", e);
        }
    }

    public boolean canBeServerFrom(SongURI targetedURI) throws ServletException {
        BindingNode bindingNode = getBindingNode(targetedURI.getScheme());

        try {
            if (bindingNode != null) {
                if (bindingNode.isLongPollSupported()) {
                    RouteConfiguration[] routesConfiguration = this.routesConfiguration;
                    InetAddress address = InetAddress.getByName(targetedURI.getHost());
                    if (LOG.isDebugEnabled()) {
                        LOG.debug("Check canBeServerFrom " + address.getHostAddress());
                    }
                    for (int i = 0; i < routesConfiguration.length; ++i) {
                        if (routesConfiguration[i].isInRoute(address)) {
                            if (LOG.isDebugEnabled()) {
                                LOG.debug("Subnet route matches " + routesConfiguration[i].getSubnet() + " -> "
                                        + routesConfiguration[i].isServerCapable());
                            }
                            return routesConfiguration[i].isServerCapable();
                        }
                    }
                }
            } else {
                throw new ServletException("Scheme " + targetedURI.getScheme() + " does not have a corresponding binding");
            }
            LOG.debug("No configuration found for route, return true");
            return true;
        } catch (UnknownHostException e) {
            return false;
        }
    }

    private SongStatsImpl songStats = new SongStatsImpl();

    public SongStats getSongStats() {
        return new SongStatsImpl(songStats);
    }

    public boolean sendLocalRequest(String method) {
        songStats.sendLocalRequest(method);
        return true;
    }

    public boolean sendRemoteRequest(String method) {
        synchronized (songStats) {
            long nbRemoteRequests = songStats.getNbRequests(SongStats.GROUP_REMOTE, SongStats.SUB_GROUP_GLOBAL);
            if (nbRemoteRequests < maxRemoteRequests) {
                songStats.sendRemoteRequest(method);
                return true;
            }
        }
        return false;
    }

    public boolean sendForwardRequest(String method) {
        songStats.sendForwardRequest(method);
        return true;
    }

    public void receivedLocalResponse(SongServletResponse response, long requestTime, long duration, String method) {
        if (response.getStatus() >= SongServletResponse.SC_INTERNAL_SERVER_ERROR) {
            transportLoggerService.logTransportTransaction(response, duration, requestTime);
        }
        songStats.receivedLocalResponse(duration, method, response.getStatus());
    }

    public void receivedRemoteResponse(SongServletResponse response, long requestTime, long duration, String method) {
        if (response.getStatus() >= SongServletResponse.SC_INTERNAL_SERVER_ERROR) {
            transportLoggerService.logTransportTransaction(response, duration, requestTime);
        }
        songStats.receivedRemoteResponse(duration, method, response.getStatus());
    }

    public void receivedForwardResponse(SongServletResponse response, long requestTime, long duration, String method) {
        if (response.getStatus() >= SongServletResponse.SC_INTERNAL_SERVER_ERROR) {
            transportLoggerService.logTransportTransaction(response, duration, requestTime);
        }
        songStats.receivedForwardResponse(duration, method, response.getStatus());
    }
}
