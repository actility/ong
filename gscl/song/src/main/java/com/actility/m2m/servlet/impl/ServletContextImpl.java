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
 * id $Id: ServletContextImpl.java 8767 2014-05-21 15:41:33Z JReich $
 * author $Author: JReich $
 * version $Revision: 8767 $
 * lastrevision $Date: 2014-05-21 17:41:33 +0200 (Wed, 21 May 2014) $
 * modifiedby $LastChangedBy: JReich $
 * lastmodified $LastChangedDate: 2014-05-21 17:41:33 +0200 (Wed, 21 May 2014) $
 */

package com.actility.m2m.servlet.impl;

import java.io.InputStream;
import java.net.URL;
import java.util.ArrayList;
import java.util.Enumeration;
import java.util.HashMap;
import java.util.Iterator;
import java.util.List;
import java.util.Map;
import java.util.Set;
import java.util.Timer;

import javax.servlet.RequestDispatcher;
import javax.servlet.Servlet;
import javax.servlet.ServletConfig;
import javax.servlet.ServletContext;
import javax.servlet.ServletContextAttributeEvent;
import javax.servlet.ServletContextAttributeListener;
import javax.servlet.ServletContextEvent;
import javax.servlet.ServletContextListener;
import javax.servlet.ServletException;
import javax.servlet.ServletRequestAttributeListener;
import javax.servlet.ServletRequestListener;

import org.apache.log4j.Logger;

import com.actility.m2m.servlet.ApplicationSessionAttributeListener;
import com.actility.m2m.servlet.ApplicationSessionListener;
import com.actility.m2m.servlet.ProtocolSessionAttributeListener;
import com.actility.m2m.servlet.ProtocolSessionListener;
import com.actility.m2m.servlet.TimerListener;
import com.actility.m2m.servlet.ext.ExtApplicationSession;
import com.actility.m2m.servlet.ext.ExtProtocolContainer;
import com.actility.m2m.servlet.ext.ExtProtocolServlet;
import com.actility.m2m.servlet.ext.ExtServletContext;
import com.actility.m2m.servlet.log.BundleLogger;
import com.actility.m2m.util.EmptyUtils;
import com.actility.m2m.util.IteratorEnum;
import com.actility.m2m.util.UUID;
import com.actility.m2m.util.log.OSGiLogger;

public class ServletContextImpl implements ExtServletContext {

    private static final Logger LOG = OSGiLogger.getLogger(ServletContextImpl.class, BundleLogger.getStaticLogger());
    private static final String[] EV_ID_TO_NAME = new String[] { "Creation", "Deletion" };
    private static final int CREATION = 0;
    private static final int DELETION = 1;

    private static final int MAJOR = 2;
    private static final int MINOR = 5;

    private static final int UUID_LENGTH = 40;

    private final ServletContainer servletContainer;
    private final Map currentRequestHandlers;
    private String contextPath;
    private final Map initParameters;
    private final String applicationName;
    private final TimerListener timerListener;
    private final int applicationTimeout;
    private List applicationSessionAttributeListeners;
    private List applicationSessionListeners;
    private List protocolSessionAttributeListeners;
    private List protocolSessionListeners;
    private List servletContextAttributeListeners;
    private List servletContextListeners;
    private List servletRequestListeners;
    private List servletRequestAttributeListeners;
    private final Timer timerService;

    // Not thread-safe
    private final Map servlets;
    private final Map attributes;
    private final Map applicationSessions;

    // PORTAGE ThreadLocal
    // public ServletContextImpl(ServletContainer servletContainer, ThreadLocal currentRequestHandler, String contextPath,
    // String applicationName, Map initParameters, TimerListener timerListener, int applicationTimeout,
    // Object[] listeners, Timer timerService) {
    public ServletContextImpl(ServletContainer servletContainer, Map currentRequestHandler, String contextPath,
            String applicationName, Map initParameters, TimerListener timerListener, int applicationTimeout,
            Object[] listeners, Timer timerService) {
        if (contextPath.length() != 0 && contextPath.charAt(0) != '/') {
            throw new IllegalArgumentException("Cannot create a servlet context with a path which does not start with a '/': "
                    + contextPath);
        }
        if ("/".equals(contextPath)) {
            this.contextPath = "";
        } else if (contextPath.endsWith("/")) {
            this.contextPath = contextPath.substring(0, contextPath.length() - 1);
        } else {
            this.contextPath = contextPath;
        }
        this.servletContainer = servletContainer;
        this.currentRequestHandlers = new HashMap();
        this.initParameters = initParameters;
        this.applicationName = applicationName;
        this.attributes = new HashMap();
        this.timerListener = timerListener;
        this.applicationTimeout = applicationTimeout;
        this.applicationSessions = new HashMap();
        this.servlets = new HashMap();
        this.timerService = timerService;

        // Build listener lists
        if (listeners != null) {
            Object listener = null;
            for (int k = 0; k < listeners.length; ++k) {
                listener = listeners[k];
                if (listener instanceof ApplicationSessionAttributeListener) {
                    applicationSessionAttributeListeners = buildListIfNull(applicationSessionAttributeListeners);
                    applicationSessionAttributeListeners.add(listener);
                }
                if (listener instanceof ApplicationSessionListener) {
                    applicationSessionListeners = buildListIfNull(applicationSessionListeners);
                    applicationSessionListeners.add(listener);
                }
                if (listener instanceof ProtocolSessionAttributeListener) {
                    protocolSessionAttributeListeners = buildListIfNull(protocolSessionAttributeListeners);
                    protocolSessionAttributeListeners.add(listener);
                }
                if (listener instanceof ProtocolSessionListener) {
                    protocolSessionListeners = buildListIfNull(protocolSessionListeners);
                    protocolSessionListeners.add(listener);
                }
                if (listener instanceof ServletContextAttributeListener) {
                    servletContextAttributeListeners = buildListIfNull(servletContextAttributeListeners);
                    servletContextAttributeListeners.add(listener);
                }
                if (listener instanceof ServletContextListener) {
                    servletContextListeners = buildListIfNull(servletContextListeners);
                    servletContextListeners.add(listener);
                }
                if (listener instanceof ServletRequestListener) {
                    servletRequestListeners = buildListIfNull(servletRequestListeners);
                    servletRequestListeners.add(listener);
                }
                if (listener instanceof ServletRequestAttributeListener) {
                    servletRequestAttributeListeners = buildListIfNull(servletRequestAttributeListeners);
                    servletRequestAttributeListeners.add(listener);
                }
            }
        }
        notifyServletContextListeners(CREATION);
    }

    private void notifyServletContextListeners(int eventType) {
        // PORTAGE chgt de classLoader pour le currentThread
        // ClassLoader oldLoader = java.lang.Thread.currentThread().getContextClassLoader();
        ServletContextEvent event = new ServletContextEvent(this);
        // java.lang.Thread.currentThread().setContextClassLoader(this.getClass().getClassLoader());
        if (LOG.isDebugEnabled()) {
            LOG.debug("Notifying servlet context listeners of context " + applicationName + " of following event "
                    + EV_ID_TO_NAME[eventType]);
        }
        Iterator it = getServletContextListeners();
        ServletContextListener listener = null;
        while (it.hasNext()) {
            listener = (ServletContextListener) it.next();
            try {
                if (LOG.isDebugEnabled()) {
                    LOG.debug("Notifying servlet context listener " + listener.getClass().getName() + " of context "
                            + applicationName + " of following event " + EV_ID_TO_NAME[eventType]);
                }
                if (CREATION == eventType) {
                    listener.contextInitialized(event);
                } else {
                    listener.contextDestroyed(event);
                }
            } catch (Exception e) {
                LOG.error("ServletContextListener threw exception", e);
            }
        }
        // java.lang.Thread.currentThread().setContextClassLoader(oldLoader);
    }

    private List buildListIfNull(List list) {
        return (list == null) ? new ArrayList() : list;
    }

    public Object getAttribute(String name) {
        return attributes.get(name);
    }

    public Enumeration getAttributeNames() {
        return new IteratorEnum(attributes.keySet().iterator());
    }

    public ServletContext getContext(String uripath) {
        return null;
    }

    public String getContextPath() {
        return contextPath;
    }

    public String getInitParameter(String name) {
        return (initParameters != null) ? (String) initParameters.get(name) : null;
    }

    public Enumeration getInitParameterNames() {
        if (initParameters != null) {
            return new IteratorEnum(initParameters.keySet().iterator());
        }
        return EmptyUtils.EMPTY_ENUMERATION;
    }

    public int getMajorVersion() {
        return MAJOR;
    }

    public int getMinorVersion() {
        return MINOR;
    }

    public String getMimeType(String file) {
        throw new RuntimeException("MimeType null");
    }

    public RequestDispatcher getNamedDispatcher(String name) {
        ExtProtocolServlet servlet = null;
        synchronized (servlets) {
            servlet = (ExtProtocolServlet) servlets.get(name);
        }
        if (servlet != null) {
            return servlet.getProtocol().getRequestDispatcher(servlet);
        }
        return null;
    }

    public String getRealPath(String path) {
        return null;
    }

    public RequestDispatcher getRequestDispatcher(String path) {
        return null;
    }

    public URL getResource(String path) {
        return null;
    }

    public InputStream getResourceAsStream(String path) {
        return null;
    }

    public Set getResourcePaths(String path) {
        return null;
    }

    public String getServerInfo() {
        return servletContainer.getServerInfo();
    }

    public Servlet getServlet(String name) {
        return null;
    }

    public String getServletContextName() {
        return applicationName;
    }

    public Enumeration getServletNames() {
        return EmptyUtils.EMPTY_ENUMERATION;
    }

    public Enumeration getServlets() {
        return EmptyUtils.EMPTY_ENUMERATION;
    }

    public void log(String msg) {
        if (LOG.isDebugEnabled()) {
            LOG.debug(applicationName + ": " + msg);
        }
    }

    public void log(Exception exception, String msg) {
        log(msg, exception);
    }

    public void log(String message, Throwable throwable) {
        LOG.error(message, throwable);
    }

    public void removeAttribute(String name) {
        if (name == null) {
            return;
        }

        ServletContextAttributeEvent event = null;

        Object value = attributes.remove(name);

        // Call the valueUnbound() method if necessary
        Iterator it = getServletContextAttributeListeners();
        ServletContextAttributeListener listener = null;
        while (it.hasNext()) {
            if (event == null) {
                event = new ServletContextAttributeEvent(this, name, value);
            }
            listener = (ServletContextAttributeListener) it.next();
            if (LOG.isDebugEnabled()) {
                LOG.debug("notifying ServletContextAttributeListener " + listener.getClass().getName()
                        + " of attribute removed on name " + name);
            }
            try {
                listener.attributeRemoved(event);
            } catch (Exception e) {
                LOG.error("ServletContextAttributeListener threw exception", e);
            }
        }
    }

    public void setAttribute(String name, Object attribute) {
        if (name == null) {
            throw new IllegalArgumentException("Name of attribute to bind cant be null!!!");
        }
        if (attribute == null) {
            throw new IllegalArgumentException("Attribute that is to be bound cant be null!!!");
        }

        // Construct an event with the new value
        ServletContextAttributeEvent event = null;

        Object previousValue = attributes.put(name, attribute);

        // Call the valueBound() method if necessary
        Iterator it = getServletContextAttributeListeners();
        ServletContextAttributeListener listener = null;
        while (it.hasNext()) {
            if (event == null) {
                event = new ServletContextAttributeEvent(this, name, (previousValue != null) ? previousValue : attribute);
            }
            listener = (ServletContextAttributeListener) it.next();
            try {
                if (previousValue == null) {
                    if (LOG.isDebugEnabled()) {
                        LOG.debug("notifying ServletContextAttributeListener " + listener.getClass().getName()
                                + " of attribute added on name " + name);
                    }
                    listener.attributeAdded(event);
                } else {
                    if (LOG.isDebugEnabled()) {
                        LOG.debug("notifying ServletContextAttributeListener " + listener.getClass().getName()
                                + " of attribute replaced on name " + name);
                    }
                    listener.attributeReplaced(event);
                }
            } catch (Exception e) {
                LOG.error("ServletContextAttributeListener threw exception", e);
            }
        }
    }

    public final void addServlet(String name, ExtProtocolServlet servlet, Map initParams) throws ServletException {
        synchronized (servlets) {
            if (servlets.containsKey(name)) {
                throw new ServletException("Servlet " + name + " already exists in the application " + applicationName);
            }
            servlets.put(name, servlet);
        }
        ServletConfig config = buildServletConfig(name, initParams);
        servlet.getServlet().init(config);
    }

    public final void removeServlet(String servletName) {
        ExtProtocolServlet removedServlet = null;
        synchronized (servlets) {
            removedServlet = (ExtProtocolServlet) servlets.remove(servletName);
        }
        if (removedServlet != null) {
            removedServlet.destroy();
        }
    }

    public final ExtProtocolServlet getDefaultServlet(ExtProtocolContainer protocol) {
        synchronized (servlets) {
            Iterator it = servlets.values().iterator();
            ExtProtocolServlet servlet = null;
            while (it.hasNext()) {
                servlet = (ExtProtocolServlet) it.next();
                if (servlet.getProtocol() == protocol) {
                    return servlet;
                }
            }
        }
        return null;
    }

    public final long getApplicationTimeout() {
        return applicationTimeout;
    }

    public final Iterator getApplicationSessions() {
        synchronized (applicationSessions) {
            return applicationSessions.values().iterator();
        }
    }

    public final void removeApplicationSession(String id) {
        LOG.debug("Remove application session");
        Object appSession = null;
        synchronized (applicationSessions) {
            appSession = applicationSessions.remove(id);
        }
        if (appSession == null && LOG.isDebugEnabled()) {
            LOG.debug("Removed an unknown application session: " + id);
        }
    }

    public final Iterator getApplicationSessionAttributeListeners() {
        return (applicationSessionAttributeListeners != null) ? applicationSessionAttributeListeners.iterator()
                : EmptyUtils.EMPTY_ITERATOR;
    }

    public final Iterator getApplicationSessionListeners() {
        return (applicationSessionListeners != null) ? applicationSessionListeners.iterator() : EmptyUtils.EMPTY_ITERATOR;
    }

    public final Iterator getProtocolSessionAttributeListeners() {
        return (protocolSessionAttributeListeners != null) ? protocolSessionAttributeListeners.iterator()
                : EmptyUtils.EMPTY_ITERATOR;
    }

    public final Iterator getProtocolSessionListeners() {
        return (protocolSessionListeners != null) ? protocolSessionListeners.iterator() : EmptyUtils.EMPTY_ITERATOR;
    }

    public final Iterator getServletContextAttributeListeners() {
        return (servletContextAttributeListeners != null) ? servletContextAttributeListeners.iterator()
                : EmptyUtils.EMPTY_ITERATOR;
    }

    public final Iterator getServletContextListeners() {
        return (servletContextListeners != null) ? servletContextListeners.iterator() : EmptyUtils.EMPTY_ITERATOR;
    }

    public final Iterator getServletRequestListeners() {
        return (servletRequestListeners != null) ? servletRequestListeners.iterator() : EmptyUtils.EMPTY_ITERATOR;
    }

    public final Iterator getServletRequestAttributeListeners() {
        return (servletRequestAttributeListeners != null) ? servletRequestAttributeListeners.iterator()
                : EmptyUtils.EMPTY_ITERATOR;
    }

    public final TimerListener getTimerListener() {
        return timerListener;
    }

    public final Timer getTimerService() {
        return timerService;
    }

    public final ExtApplicationSession createApplicationSession() {
        if (LOG.isDebugEnabled()) {
            LOG.debug("createApplicationSession: " + applicationName);
        }
        String id = UUID.randomUUID(UUID_LENGTH) + "@" + applicationName;
        Map handlers = new HashMap();
        Map.Entry entry = null;
        // ThreadLocal currentRequestHandler = null;
        synchronized (currentRequestHandlers) {
            Iterator it = this.currentRequestHandlers.entrySet().iterator();
            while (it.hasNext()) {
                entry = (Map.Entry) it.next();
                // currentRequestHandler = (ThreadLocal) entry.getValue();
                // handlers.put(entry.getKey(), currentRequestHandler.get());
                handlers.put(entry.getKey(), entry.getValue());
            }
        }
        ApplicationSessionImpl appSession = new ApplicationSessionImpl(id, this, handlers);
        synchronized (applicationSessions) {
            applicationSessions.put(id, appSession);
        }
        return appSession;
    }

    public final String getDescription() {
        StringBuffer buf = new StringBuffer();
        buf.append("{ application: ").append(applicationName).append(System.getProperty("line.separator"));
        buf.append("  { contextPath: ").append(contextPath).append(" }");
        buf.append(System.getProperty("line.separator"));
        synchronized (servlets) {
            Iterator it = servlets.values().iterator();
            ExtProtocolServlet pws = null;
            while (it.hasNext()) {
                pws = (ExtProtocolServlet) it.next();
                pws.getProtocol().getDescription(buf, pws);
            }
        }

        buf.append('}');
        return buf.toString();
    }

    public final void destroy() {
        notifyServletContextListeners(DELETION);

        ApplicationSessionImpl appSession = null;
        synchronized (applicationSessions) {
            Iterator it = applicationSessions.values().iterator();
            while (it.hasNext()) {
                appSession = (ApplicationSessionImpl) it.next();
                appSession.invalidate(false, false);
            }
            applicationSessions.clear();
        }
        ExtProtocolServlet servlet = null;
        synchronized (servlets) {
            Iterator it = servlets.values().iterator();
            while (it.hasNext()) {
                servlet = (ExtProtocolServlet) it.next();
                servlet.destroy();
            }
            servlets.clear();
        }
    }

    public final ExtProtocolServlet setCurrentRequestHandler(ExtProtocolContainer protocol, ExtProtocolServlet servlet) {
        // ThreadLocal currentRequestHandler = null;
        // synchronized (currentRequestHandlers) {
        // currentRequestHandler = (ThreadLocal) currentRequestHandlers.get(protocol);
        // if (currentRequestHandler == null) {
        // currentRequestHandler = new ThreadLocal();
        // currentRequestHandlers.put(protocol, currentRequestHandler);
        // }
        // }

        // ExtProtocolServlet previous = (ExtProtocolServlet) currentRequestHandler.get();
        // currentRequestHandler.set(servlet);
        ExtProtocolServlet previous = (ExtProtocolServlet) currentRequestHandlers.get(protocol);
        currentRequestHandlers.put(protocol, servlet);
        return previous;
    }

    public final ExtProtocolServlet getCurrentRequestHandler(ExtProtocolContainer protocol) {
        // ThreadLocal currentRequestHandler = null;
        ExtProtocolServlet requestHandler = null;
        // synchronized (currentRequestHandlers) {
        // currentRequestHandler = (ThreadLocal) currentRequestHandlers.get(protocol);
        // }
        //
        // if (currentRequestHandler != null) {
        // requestHandler = (ExtProtocolServlet) currentRequestHandler.get();
        // }
        requestHandler = (ExtProtocolServlet) currentRequestHandlers.get(protocol);
        if (requestHandler == null) {
            requestHandler = getDefaultServlet(protocol);
        }
        return requestHandler;
    }

    public final void removeProtocol(ExtProtocolContainer protocol) {
        ApplicationSessionImpl appSession = null;
        synchronized (applicationSessions) {
            Iterator it = applicationSessions.values().iterator();
            while (it.hasNext()) {
                appSession = (ApplicationSessionImpl) it.next();
                appSession.removeProtocol(protocol);
            }
        }
        ExtProtocolServlet servlet = null;
        synchronized (servlets) {
            Iterator it = servlets.values().iterator();
            while (it.hasNext()) {
                servlet = (ExtProtocolServlet) it.next();
                if (servlet.getProtocol() == protocol) {
                    servlet.destroy();
                }
            }
        }
    }

    public ServletConfig buildServletConfig(String name, Map initParams) {
        return new ServletConfigImpl(name, this, initParams);
    }
}
