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
 * id $Id: ServletContainer.java 8767 2014-05-21 15:41:33Z JReich $
 * author $Author: JReich $
 * version $Revision: 8767 $
 * lastrevision $Date: 2014-05-21 17:41:33 +0200 (Wed, 21 May 2014) $
 * modifiedby $LastChangedBy: JReich $
 * lastmodified $LastChangedDate: 2014-05-21 17:41:33 +0200 (Wed, 21 May 2014) $
 */

package com.actility.m2m.servlet.impl;

import java.util.HashMap;
import java.util.Iterator;
import java.util.Map;
import java.util.TimerTask;

import javax.servlet.ServletContext;
import javax.servlet.ServletException;

import org.apache.log4j.Logger;

import com.actility.m2m.servlet.TimerListener;
import com.actility.m2m.servlet.ext.ExtProtocolContainer;
import com.actility.m2m.servlet.log.BundleLogger;
import com.actility.m2m.servlet.service.ext.ExtServletService;
import com.actility.m2m.util.log.OSGiLogger;

public final class ServletContainer implements ExtServletService {
    private static final Logger LOG = OSGiLogger.getLogger(ServletContainer.class, BundleLogger.getStaticLogger());
    private static final int DEFAULT_APPLICATION_TIMEOUT = 180000;

    // PORTAGE ThreadLocal
    // private final ThreadLocal currentRequestHandler = new ThreadLocal();
    private final HashMap currentRequestHandler = new HashMap();
    private final int applicationTimeout;
    private final TimerServiceImpl timerService;
    private String serverInfo;

    // Not thread safe
    private final Map applications;

    public ServletContainer(String serverInfo) {
        this.applications = new HashMap();
        this.applicationTimeout = DEFAULT_APPLICATION_TIMEOUT;
        this.timerService = new TimerServiceImpl();
        this.timerService.start();
        this.serverInfo = serverInfo;
    }

    public void stop() {
        LOG.info("Stopping...");
        timerService.stop();
        synchronized (this) {
            applications.clear();
        }
    }

    public synchronized ServletContext createApplication(String contextPath, String applicationName, Map initParams,
            TimerListener timerListener, Object[] listeners) throws ServletException {
        if (LOG.isDebugEnabled()) {
            LOG.debug("Create application: " + applicationName);
        }
        if (!applications.containsKey(applicationName)) {
          //PORTAGE ThreadLocal
            // ServletContextImpl context = new ServletContextImpl(this, currentRequestHandler, contextPath, applicationName,
            // initParams, timerListener, applicationTimeout, listeners, timerService);
            ServletContextImpl context = new ServletContextImpl(this, currentRequestHandler, contextPath, applicationName,
                                        initParams, timerListener, applicationTimeout, listeners, timerService);
            context.setAttribute("com.actility.servlet.TimerService", timerService);
            applications.put(applicationName, context);
            return context;
        }
        throw new ServletException("The application already exists: " + applicationName);
    }

    public void unregisterApplication(ServletContext context) {
        if (LOG.isDebugEnabled()) {
            LOG.debug("Unregister application: " + context.getServletContextName());
        }
        ServletContextImpl servletContext = null;
        synchronized (applications) {
            servletContext = (ServletContextImpl) applications.remove(context.getServletContextName());
        }
        if (servletContext != null) {
            servletContext.destroy();
        }
    }

    public synchronized Iterator getApplications() {
        return applications.values().iterator();
    }

    public synchronized void unregisterProtocolContainer(ExtProtocolContainer protocol) {
        Iterator it = applications.values().iterator();
        while (it.hasNext()) {
            ((ServletContextImpl) it.next()).removeProtocol(protocol);
        }
        protocol.destroy();
    }

    public void scheduleTimer(TimerTask timer, long millis) {
        timerService.schedule(timer, millis);
    }

    public String getServerInfo() {
        return serverInfo;
    }

    public void setServerInfo(String serverInfo) {
        this.serverInfo = serverInfo;
    }
}
