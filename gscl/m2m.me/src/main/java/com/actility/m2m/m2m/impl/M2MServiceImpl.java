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
 */

package com.actility.m2m.m2m.impl;

import java.net.URI;
import java.util.Map;

import javax.servlet.ServletContext;
import javax.servlet.ServletException;

import org.apache.log4j.Logger;

import com.actility.m2m.framework.resources.ResourcesAccessorService;
import com.actility.m2m.m2m.M2MContext;
import com.actility.m2m.m2m.M2MEventHandler;
import com.actility.m2m.m2m.M2MException;
import com.actility.m2m.m2m.M2MProxyHandler;
import com.actility.m2m.m2m.M2MService;
import com.actility.m2m.m2m.M2MUtils;
import com.actility.m2m.m2m.StatusCode;
import com.actility.m2m.m2m.log.BundleLogger;
import com.actility.m2m.servlet.NamespaceException;
import com.actility.m2m.servlet.song.service.SongService;
import com.actility.m2m.util.log.OSGiLogger;
import com.actility.m2m.xo.XoService;

public final class M2MServiceImpl implements M2MService {
    private static final Logger LOG = OSGiLogger.getLogger(M2MServiceImpl.class, BundleLogger.getStaticLogger());

    private final SongService songService;
    private final XoService xoService;
    private final ResourcesAccessorService resourcesAccessorService;
    private final M2MUtils m2mUtils;

    public M2MServiceImpl(SongService songService, XoService xoService,
            ResourcesAccessorService resourcesAccessorService) {
        this.songService = songService;
        this.xoService = xoService;
        this.resourcesAccessorService = resourcesAccessorService;
        this.m2mUtils = new M2MUtilsImpl(xoService);
    }

    public M2MContext registerScl(String applicationName, String applicationPath, M2MEventHandler m2mHandler,
            M2MProxyHandler proxyHandler, Map configuration) throws M2MException {
        if (LOG.isInfoEnabled()) {
            LOG.info("Registering scl " + applicationName);
        }
        ServletContext servletContext = null;
        boolean error = false;
        try {
            M2MContextImpl context = new M2MContextImpl(null, proxyHandler, m2mHandler, xoService,
                    resourcesAccessorService, m2mUtils);
            m2mHandler.init(context);
            servletContext = songService.createApplication(applicationPath, applicationName, null, context,
                    new Object[] { context });
            songService.registerServiceServlet(servletContext, "/", "Servlet", context, null, configuration);
            songService.registerProxyServlet(servletContext, "Proxy", context, null, configuration);
            return context;
        } catch (ServletException e) {
            error = true;
            throw new M2MException("Cannot create the M2M application " + applicationName,
                    StatusCode.STATUS_INTERNAL_SERVER_ERROR, e);
        } catch (NamespaceException e) {
            error = true;
            throw new M2MException("Application path '" + applicationPath
                    + "' is already registered for M2M application " + applicationName,
                    StatusCode.STATUS_INTERNAL_SERVER_ERROR, e);
        } catch (RuntimeException e) {
            error = true;
            e.printStackTrace();
            throw new M2MException("RuntimeException while registering the M2M application " + applicationName,
                    StatusCode.STATUS_INTERNAL_SERVER_ERROR, e);
        } finally {
            if (error && servletContext != null) {
                songService.unregisterApplication(servletContext);
            }
        }
    }

    public M2MContext registerApplication(String applicationName, String applicationPath, URI sclUri,
            M2MEventHandler m2mHandler, Map configuration) throws M2MException {
        if (LOG.isInfoEnabled()) {
            LOG.info("Registering application " + applicationName);
        }
        ServletContext servletContext = null;
        boolean error = false;
        try {
            M2MContextImpl context = new M2MContextImpl(sclUri, null, m2mHandler, xoService, resourcesAccessorService,
                    m2mUtils);
            m2mHandler.init(context);
            servletContext = songService.createApplication(applicationPath, applicationName, null, context,
                    new Object[] { context });
            songService.registerServiceServlet(servletContext, "/", "Servlet", context, null, configuration);
            return context;
        } catch (ServletException e) {
            error = true;
            throw new M2MException("Cannot create the M2M application " + applicationName,
                    StatusCode.STATUS_INTERNAL_SERVER_ERROR, e);
        } catch (NamespaceException e) {
            error = true;
            throw new M2MException("Application path '" + applicationPath
                    + "' is already registered for M2M application " + applicationName,
                    StatusCode.STATUS_INTERNAL_SERVER_ERROR, e);
        } catch (RuntimeException e) {
            error = true;
            throw new M2MException("RuntimeException while registering the M2M application " + applicationName,
                    StatusCode.STATUS_INTERNAL_SERVER_ERROR, e);
        } finally {
            if (error && servletContext != null) {
                songService.unregisterApplication(servletContext);
            }
        }
    }

    public void unregisterContext(M2MContext m2mContext) {
        if (LOG.isInfoEnabled()) {
            LOG.info("Unregistering application " + m2mContext.getApplicationName());
        }
        if (M2MContextImpl.class == m2mContext.getClass()) {
            M2MContextImpl m2mContextImpl = (M2MContextImpl) m2mContext;
            songService.unregisterApplication(m2mContextImpl.getServletContext());
        }
    }

    public void stop() {
        // TODO
    }

    public M2MUtils getM2MUtils() {
        return m2mUtils;
    }
}
