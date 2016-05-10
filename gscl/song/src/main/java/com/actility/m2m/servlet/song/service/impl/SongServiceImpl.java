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
 * id $Id: SongServiceImpl.java 9309 2014-08-21 10:13:58Z JReich $
 * author $Author: JReich $
 * version $Revision: 9309 $
 * lastrevision $Date: 2014-08-21 12:13:58 +0200 (Thu, 21 Aug 2014) $
 * modifiedby $LastChangedBy: JReich $
 * lastmodified $LastChangedDate: 2014-08-21 12:13:58 +0200 (Thu, 21 Aug 2014) $
 */

package com.actility.m2m.servlet.song.service.impl;

import java.net.InetAddress;
import java.net.UnknownHostException;
import java.text.ParseException;
import java.util.Iterator;
import java.util.Map;

import javax.servlet.ServletContext;
import javax.servlet.ServletException;

import org.apache.log4j.Logger;

import com.actility.m2m.be.BackendException;
import com.actility.m2m.be.BackendService;
import com.actility.m2m.framework.resources.ResourcesAccessorService;
import com.actility.m2m.servlet.NamespaceException;
import com.actility.m2m.servlet.TimerListener;
import com.actility.m2m.servlet.ext.ExtServletContext;
import com.actility.m2m.servlet.log.BundleLogger;
import com.actility.m2m.servlet.service.ext.ExtServletService;
import com.actility.m2m.servlet.song.SongServlet;
import com.actility.m2m.servlet.song.binding.SongBindingFacade;
import com.actility.m2m.servlet.song.binding.service.ext.ExtSongBindingService;
import com.actility.m2m.servlet.song.ext.SongNode;
import com.actility.m2m.servlet.song.impl.RouteConfiguration;
import com.actility.m2m.servlet.song.impl.SongContainer;
import com.actility.m2m.servlet.song.service.SongService;
import com.actility.m2m.servlet.song.service.ext.ExtSongService;
import com.actility.m2m.servlet.song.service.ext.SongStats;
import com.actility.m2m.transport.logger.TransportLoggerService;
import com.actility.m2m.util.CharacterUtils;
import com.actility.m2m.util.StringReader;
import com.actility.m2m.util.log.OSGiLogger;
import com.actility.m2m.xo.XoService;

/**
 * The SONG Service
 */
public final class SongServiceImpl implements ExtSongService, ExtSongBindingService {
    private static final Logger LOG = OSGiLogger.getLogger(SongService.class, BundleLogger.getStaticLogger());

    private static final long L_WHITESPACES = CharacterUtils.lowMask(" \t");
    private static final long H_WHITESPACES = CharacterUtils.highMask(" \t");

    private static final String PROP_ROUTES_CONFIGURATION = "routesConfiguration";

    private static final RouteConfiguration[] DEFAULT_ROUTES = new RouteConfiguration[] { new RouteConfiguration("0.0.0.0", 0,
            false) };

    private SongContainer container;
    private ExtServletService servletService;

    public SongServiceImpl(String[] config, ExtServletService servletService, BackendService backendService,
            XoService xoService, ResourcesAccessorService resourcesAccessorService,
            TransportLoggerService transportLoggerService, String hostname, String domainName, long maxRemoteRequests)
            throws BackendException, UnknownHostException {
        this.servletService = servletService;
        try {
            RouteConfiguration[] routesConfiguration = parseRoutesConfiguration(config);
            if (routesConfiguration == null) {
                routesConfiguration = DEFAULT_ROUTES;
            }
            container = new SongContainer(servletService, backendService, xoService, resourcesAccessorService,
                    transportLoggerService, routesConfiguration, hostname, domainName, maxRemoteRequests);
        } catch (BackendException e) {
            LOG.error("Cannot start the SONG service", e);
            throw e;
        } catch (UnknownHostException e) {
            LOG.error("Cannot determine localhost", e);
            throw e;
        }
    }

    private RouteConfiguration[] parseRoutesConfiguration(String[] config) {
        if (config == null || config.length == 0) {
            return null;
        }
        RouteConfiguration[] result = new RouteConfiguration[config.length];
        StringReader reader = null;
        String ip = null;
        String cidrStr = null;
        String serverCapable = null;
        for (int i = 0; i < config.length; ++i) {
            reader = new StringReader(config[i]);
            try {
                reader.skip(L_WHITESPACES, H_WHITESPACES);
                ip = reader.readUntil('/');
                reader.skipOffset(1);
                cidrStr = reader.readUntil(':');
                reader.skipOffset(1);
                serverCapable = reader.readUntil(L_WHITESPACES, H_WHITESPACES);
                result[i] = new RouteConfiguration(ip, Integer.parseInt(cidrStr), Boolean.TRUE.toString().equalsIgnoreCase(
                        serverCapable));
            } catch (ParseException e) {
                LOG.error("Route configuration syntax", e);
            } catch (NumberFormatException e) {
                LOG.error("Route configuration has a bad cidr: " + cidrStr, e);
            } catch (IllegalArgumentException e) {
                LOG.error("Problem while parsing route configuration", e);
            }
        }

        return result;
    }

    private String normalizeString(String value) {
        String resultValue = value;
        if (resultValue != null) {
            resultValue = value.trim();
            if (resultValue.length() == 0) {
                resultValue = null;
            }
        }
        return resultValue;
    }

    // add a method to stop clearly
    public void stop() {
        servletService.unregisterProtocolContainer(container);
    }

    public ServletContext createApplication(String contextPath, String applicationName, Map initParams,
            TimerListener timerListener, Object[] listeners) throws ServletException {
        return servletService.createApplication(contextPath, applicationName, initParams, timerListener, listeners);
    }

    public void unregisterApplication(ServletContext context) {
        servletService.unregisterApplication(context);
    }

    public void registerBindingServlet(ServletContext context, String servletName, SongServlet servlet, Map initParams,
            SongBindingFacade facade, String serverScheme, int serverPort, String[] managedSchemes, boolean longPollSupported,
            String defaultProtocol, InetAddress address, int port, Map configuration) throws NamespaceException,
            ServletException {
        if (context == null) {
            throw new ServletException("Can't register a SONG binding on a null application context");
        }
        if (!(context instanceof ExtServletContext)) {
            throw new ServletException("Unknown servlet context, not built with the container");
        }
        if (servletName == null) {
            throw new ServletException("Can't register a SONG binding without a name");
        }
        if (servlet == null) {
            throw new ServletException("Can't register a null SONG servlet");
        }
        if (facade == null) {
            throw new ServletException("Can't register a SONG binding with a null factory");
        }
        if (serverScheme == null) {
            throw new ServletException("Can't register a SONG binding with a null server scheme");
        }
        if (managedSchemes == null || managedSchemes.length < 1) {
            throw new ServletException("Can't register a SONG binding without a managed scheme");
        }
        if (defaultProtocol == null) {
            throw new ServletException("Can't register a SONG binding without a default protocol");
        }
        if (defaultProtocol.indexOf('/') < 0) {
            throw new ServletException("Default protocol must be of the form PROTOCOL/VERSION");
        }
        if (address == null) {
            throw new ServletException("Can't register a SONG binding with a null address");
        }
        if (port <= 0) {
            throw new ServletException("Can't register a SONG binding with an invalid port");
        }

        container.registerBindingServlet(context, servletName, servlet, initParams, facade, serverScheme, serverPort,
                managedSchemes, longPollSupported, defaultProtocol, address, port, configuration);
    }

    public void registerServiceServlet(ServletContext context, String servletName, SongServlet servlet, Map initParams,
            Map configuration) throws ServletException {
        if (context == null) {
            throw new ServletException("Can't register a SONG servlet on a null application context");
        }
        if (!(context instanceof ExtServletContext)) {
            throw new ServletException("Unknown servlet context, not built with the container");
        }
        if (servletName == null) {
            throw new ServletException("Can't register a SONG servlet without a name");
        }
        if (servlet == null) {
            throw new ServletException("Can't register a null SONG servlet");
        }

        container.registerServiceServlet(context, servletName, servlet, initParams, configuration);
    }

    public void registerServiceServlet(ServletContext context, String path, String servletName, SongServlet servlet,
            Map initParams, Map configuration) throws NamespaceException, ServletException {
        if (context == null) {
            throw new ServletException("Can't register a SONG servlet on a null application context");
        }
        if (!(context instanceof ExtServletContext)) {
            throw new ServletException("Unknown servlet context, not built with the container");
        }
        if (path == null || path.length() == 0) {
            throw new ServletException("Can't register a SONG servlet on a null or empty path");
        }
        if (path.charAt(0) != '/' || (path.length() > 1 && path.endsWith("/")) || path.indexOf('*') != -1) {
            throw new ServletException("Bad path. It must start with '/', must not end with '/' and must not contains any '*'");
        }
        if (servletName == null) {
            throw new ServletException("Can't register a SONG servlet without a name");
        }
        if (servlet == null) {
            throw new ServletException("Can't register a null SONG servlet");
        }

        container.registerServiceServlet(context, path, servletName, servlet, initParams, configuration);
    }

    public void unregisterServlet(SongServlet servlet) {
        if (servlet != null && (servlet.getServletContext() instanceof ExtServletContext)) {
            ExtServletContext servletContext = (ExtServletContext) servlet.getServletContext();
            servletContext.removeServlet(servlet.getServletName());
        }
    }

    public void registerProxyServlet(ServletContext context, String servletName, SongServlet servlet, Map initParams,
            Map configuration) throws ServletException {
        if (context == null) {
            throw new ServletException("Can't register a proxy SONG servlet on a null application context");
        }
        if (!(context instanceof ExtServletContext)) {
            throw new ServletException("Unknown servlet context, not built with the container");
        }
        if (servletName == null) {
            throw new ServletException("Can't register a proxy SONG servlet without a name");
        }
        if (servlet == null) {
            throw new ServletException("Can't register a null proxy SONG servlet");
        }

        container.registerProxyServlet(context, servletName, servlet, initParams, configuration);
    }

    public String[] getRoutesConfiguration() {
        RouteConfiguration[] routesConfiguration = container.getRoutesConfiguration();
        String[] rawConf = new String[routesConfiguration.length];
        for (int i = 0; i < routesConfiguration.length; ++i) {
            rawConf[i] = routesConfiguration[i].getSubnet() + " -> " + routesConfiguration[i].isServerCapable();
        }
        return rawConf;
    }

    public SongNode getProxyNode() {
        return container.getProxyNode();
    }

    public Iterator getSchemeRoutes() {
        return container.getSchemeRoutes();
    }

    public Iterator getPathRoutes() {
        return container.getPathRoutes();
    }

    public Iterator getPrivatePathRoutes() {
        return container.getPrivatePathRoutes();
    }

    public Iterator getExternalAliasRoutes() {
        return container.getExternalAliasRoutes();
    }

    public SongStats getSongStats() {
        return container.getSongStats();
    }

    public void setRoutesConfiguration(String[] config) {
        RouteConfiguration[] routesConfiguration = parseRoutesConfiguration(config);
        container.setRoutesConfiguration(routesConfiguration);
    }

    public void setMaxRemoteRequests(long maxRemoteRequests) {
        container.setMaxRemoteRequests(maxRemoteRequests);
    }
}
