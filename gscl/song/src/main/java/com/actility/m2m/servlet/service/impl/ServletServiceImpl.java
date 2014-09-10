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
 * id $Id: ServletServiceImpl.java 8767 2014-05-21 15:41:33Z JReich $
 * author $Author: JReich $
 * version $Revision: 8767 $
 * lastrevision $Date: 2014-05-21 17:41:33 +0200 (Wed, 21 May 2014) $
 * modifiedby $LastChangedBy: JReich $
 * lastmodified $LastChangedDate: 2014-05-21 17:41:33 +0200 (Wed, 21 May 2014) $
 */

package com.actility.m2m.servlet.service.impl;

import java.util.Iterator;
import java.util.Map;
import java.util.TimerTask;

import javax.servlet.ServletContext;
import javax.servlet.ServletException;

import org.apache.log4j.Logger;

import com.actility.m2m.servlet.TimerListener;
import com.actility.m2m.servlet.ext.ExtProtocolContainer;
import com.actility.m2m.servlet.impl.ServletContainer;
import com.actility.m2m.servlet.impl.ServletContextImpl;
import com.actility.m2m.servlet.log.BundleLogger;
import com.actility.m2m.servlet.service.ext.ExtServletService;
import com.actility.m2m.util.log.OSGiLogger;

public final class ServletServiceImpl implements ExtServletService {
    private static final Logger LOG = OSGiLogger.getLogger(ServletServiceImpl.class, BundleLogger.getStaticLogger());

    private ServletContainer servletContainer;

    public ServletServiceImpl(String serverInfo) {
        servletContainer = new ServletContainer(serverInfo);
    }

    // add a method to stop clearly
    public void stop() {
        if (servletContainer != null) {
            servletContainer.stop();
        }
    }

    public ServletContext createApplication(String contextPath, String applicationName, Map initParams,
            TimerListener timerListener, Object[] listeners) throws ServletException {
        if (contextPath == null) {
            throw new IllegalArgumentException("Cannot create an application with a null contextPath");
        }
        if (applicationName == null) {
            throw new IllegalArgumentException("Cannot create an application with a null applicationName");
        }
        return servletContainer.createApplication(contextPath, applicationName, initParams, timerListener, listeners);
    }

    public void unregisterApplication(ServletContext context) {
        if (context == null || ServletContextImpl.class != context.getClass()) {
            return;
        }
        servletContainer.unregisterApplication(context);
    }

    public Iterator getApplications() {
        return servletContainer.getApplications();
    }

    public void unregisterProtocolContainer(ExtProtocolContainer protocol) {
        servletContainer.unregisterProtocolContainer(protocol);
    }

    public void scheduleTimer(TimerTask timer, long millis) {
        servletContainer.scheduleTimer(timer, millis);
    }

    public void setServerInfo(String serverInfo) {
        servletContainer.setServerInfo(serverInfo);
    }
}
