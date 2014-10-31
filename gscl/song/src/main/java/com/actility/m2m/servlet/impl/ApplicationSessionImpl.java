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
 * id $Id: ApplicationSessionImpl.java 9309 2014-08-21 10:13:58Z JReich $
 * author $Author: JReich $
 * version $Revision: 9309 $
 * lastrevision $Date: 2014-08-21 12:13:58 +0200 (Thu, 21 Aug 2014) $
 * modifiedby $LastChangedBy: JReich $
 * lastmodified $LastChangedDate: 2014-08-21 12:13:58 +0200 (Thu, 21 Aug 2014) $
 */

package com.actility.m2m.servlet.impl;

import java.io.Serializable;
import java.util.Collection;
import java.util.Collections;
import java.util.HashMap;
import java.util.Iterator;
import java.util.Map;
import java.util.SortedMap;
import java.util.TreeMap;

import javax.servlet.ServletException;

import org.apache.log4j.Logger;

import com.actility.m2m.framework.resources.BackupClassLoader;
import com.actility.m2m.servlet.ApplicationSession;
import com.actility.m2m.servlet.ApplicationSessionAttributeListener;
import com.actility.m2m.servlet.ApplicationSessionBindingEvent;
import com.actility.m2m.servlet.ApplicationSessionBindingListener;
import com.actility.m2m.servlet.ApplicationSessionEvent;
import com.actility.m2m.servlet.ApplicationSessionListener;
import com.actility.m2m.servlet.ProtocolSession;
import com.actility.m2m.servlet.ServletTimer;
import com.actility.m2m.servlet.ext.ExtApplicationSession;
import com.actility.m2m.servlet.ext.ExtProtocolContainer;
import com.actility.m2m.servlet.ext.ExtProtocolServlet;
import com.actility.m2m.servlet.ext.ExtProtocolSession;
import com.actility.m2m.servlet.ext.ExtServletContext;
import com.actility.m2m.servlet.log.BundleLogger;
import com.actility.m2m.util.EmptyUtils;
import com.actility.m2m.util.RunnableTimerTask;
import com.actility.m2m.util.log.OSGiLogger;

/**
 * Concrete implementation of the {@link ApplicationSession} in the SONG servlet API. It represents an applicative sessions,
 * abstract from the protocol sessions.
 */
public final class ApplicationSessionImpl implements ExtApplicationSession, Serializable, Runnable {
    private static final Logger LOG = OSGiLogger.getLogger(ApplicationSessionImpl.class, BundleLogger.getStaticLogger());
    private static final long SECOND_IN_MILLISECONDS = 1000;

    private static final long serialVersionUID = -4649488453047327525L;

    private static final char SESSION_KEY_SEPARATOR = '@';
    private static final char SESSION_KEY_SEPARATOR_NEXT = '@' + 1;
    private static final long MINUTE_IN_MILLISECONDS = 60000L;

    private static final String[] ST_ID_TO_NAME = { "VALID", "READY_TO_INVALIDATE", "INVALIDATING", "INVALIDATED" };
    private static final String[] EV_ID_TO_NAME = { "Creation", "Deletion", "Expiration", "ReadyToInvalidate" };

    /**
     * Valid state for the application session.
     */
    private static final int ST_VALID = 0;

    /**
     * Ready to invalidate state for the application session.
     */
    private static final int ST_READY_TO_INVALIDATE = 1;

    /**
     * Invalidating state for the application session.
     */
    private static final int ST_INVALIDATING = 2;

    /**
     * Invalidated state for the application session.
     */
    private static final int ST_INVALIDATED = 3;

    /**
     * Creation event for the application session.
     */
    private static final int EV_CREATION = 0;

    /**
     * Deletion event for the application session.
     */
    private static final int EV_DELETION = 1;

    /**
     * Expiration event for the application session.
     */
    private static final int EV_EXPIRATION = 2;

    /**
     * Ready to invalidate event for the application session.
     */
    private static final int EV_READY_TO_INVALIDATE = 3;

    private Map attributes;
    private final SortedMap sessions;

    private final String id;
    private final Map handlers;
    private long lastAccessedTime;
    private final long creationTime;
    /* Use as a buffer the real value is in the timer */
    private long expirationTime;
    private int state;

    private boolean invalidateWhenReady;

    private transient RunnableTimerTask expirationTimerTask;
    private transient Map servletTimers;

    private final transient ServletContextImpl context;

    /**
     * Constructor of the Application Session.
     *
     * @param context The servlet context of the application that has created this application session
     */
    public ApplicationSessionImpl(String id, ServletContextImpl context, Map handlers) {
        this.id = id;
        this.handlers = handlers;
        this.sessions = Collections.synchronizedSortedMap(new TreeMap());
        this.lastAccessedTime = System.currentTimeMillis();
        this.creationTime = this.lastAccessedTime;
        setState(ST_VALID);
        this.invalidateWhenReady = true;
        this.context = context;
        updateExpirationTimerTask(creationTime, context.getApplicationTimeout());
        notifyApplicationSessionListeners(EV_CREATION);
    }

    public void addProtocolSession(ExtProtocolSession session) throws ServletException {
        String sessionKey = buildSessionKey(session.getProtocol().getProtocolName(), session.getId());
        setState(ST_VALID);
        if (sessions.containsKey(sessionKey)) {
            throw new ServletException("A session with the same id already exists: " + sessionKey);
        }
        sessions.put(sessionKey, session);
        if (LOG.isDebugEnabled()) {
            LOG.debug("Added session " + sessionKey + " to app session " + context.getServletContextName());
        }
    }

    public void removeProtocolSession(ExtProtocolSession session) {
        String sessionKey = buildSessionKey(session.getProtocol().getProtocolName(), session.getId());
        Object oldProtocolSession = sessions.remove(sessionKey);
        if (oldProtocolSession != null) {
            if (LOG.isDebugEnabled()) {
                LOG.debug("Removed session " + sessionKey + " from app session " + context.getServletContextName());
            }
            updateReadyToInvalidateState();
        }
    }

    public void protocolSessionReadyToInvalidate(ExtProtocolSession session) {
        updateReadyToInvalidateState();
    }

    public Object getAttribute(String name) {
        checkValid();
        return (attributes != null) ? attributes.get(name) : null;
    }

    public Iterator getAttributeNames() {
        checkValid();
        return (attributes != null) ? attributes.keySet().iterator() : EmptyUtils.EMPTY_ITERATOR;
    }

    public long getCreationTime() {
        checkValid();
        return creationTime;
    }

    public long getExpirationTime() {
        checkValid();
        return (expirationTimerTask != null) ? expirationTime : -1;
    }

    public String getId() {
        return id;
    }

    public long getLastAccessedTime() {
        return lastAccessedTime;
    }

    public void access() {
        LOG.debug("Application session accessed");
        this.lastAccessedTime = System.currentTimeMillis();
        updateExpirationTimerTask(lastAccessedTime, context.getApplicationTimeout());
    }

    public int getTimersSize() {
        checkValid();
        return (servletTimers != null) ? servletTimers.size() : 0;
    }

    public int getProtocolSessionsSize() {
        return sessions.size();
    }

    public Iterator getSessions() {
        return sessions.values().iterator();
    }

    public Iterator getSessions(String protocol) {
        checkValid();
        if (protocol == null) {
            throw new IllegalArgumentException("protocol given in argument is null");
        }
        SortedMap subMap = sessions.subMap(protocol + SESSION_KEY_SEPARATOR, protocol + SESSION_KEY_SEPARATOR_NEXT);
        return subMap.values().iterator();
    }

    public ProtocolSession getSession(String protocol, String id) {
        checkValid();
        if (protocol == null) {
            throw new IllegalArgumentException("protocol is null");
        }
        if (id == null) {
            throw new IllegalArgumentException("id is null");
        }
        String sessionKey = buildSessionKey(protocol, id);
        return (ProtocolSession) sessions.get(sessionKey);
    }

    public Collection getTimers() {
        checkValid();
        return (servletTimers != null) ? servletTimers.values() : Collections.EMPTY_LIST;
    }

    public ServletTimer getTimer(String id) {
        checkValid();
        return (servletTimers != null) ? (ServletTimer) servletTimers.get(id) : null;
    }

    public void invalidate() {
        invalidate(false, true);
    }

    public boolean isValid() {
        return ST_VALID == state || ST_READY_TO_INVALIDATE == state;
    }

    public void removeAttribute(String name) {
        if (ST_INVALIDATING != state) {
            if (ST_INVALIDATED == state) {
                throw new IllegalStateException("Can not unbind object to session that has been invalidated!!");
            }
            removeAttribute(name, null, null);
        }
    }

    public void setAttribute(String name, Object attribute) {
        checkValid();

        if (name == null) {
            throw new IllegalArgumentException("Name of attribute to bind cant be null!!!");
        }
        if (attribute == null) {
            throw new IllegalArgumentException("Attribute that is to be bound cant be null!!!");
        }

        if (attributes == null) {
            attributes = new HashMap();
        }

        // Construct an event with the new value
        ApplicationSessionBindingEvent event = null;

        Object previousValue = attributes.put(name, attribute);

        // Call the valueBound() method if necessary
        if (attribute != previousValue && attribute instanceof ApplicationSessionBindingListener) {
            // Don't call any notification if replacing with the same value
            event = new ApplicationSessionBindingEvent(this, name);
            try {
                ((ApplicationSessionBindingListener) attribute).valueBound(event);
            } catch (Exception e) {
                LOG.error("ApplicationSessionBindingListener threw exception", e);
            }
        }
        if (previousValue != attribute && previousValue != null && previousValue instanceof ApplicationSessionBindingListener) {
            if (event == null) {
                event = new ApplicationSessionBindingEvent(this, name);
            }
            try {
                ((ApplicationSessionBindingListener) previousValue).valueUnbound(event);
            } catch (Exception e) {
                LOG.error("ApplicationSessionBindingListener threw exception", e);
            }
        }

        Iterator it = context.getApplicationSessionAttributeListeners();
        ApplicationSessionAttributeListener listener = null;
        while (it.hasNext()) {
            if (event == null) {
                event = new ApplicationSessionBindingEvent(this, name);
            }
            listener = (ApplicationSessionAttributeListener) it.next();
            try {
                if (previousValue == null) {
                    if (LOG.isDebugEnabled()) {
                        LOG.debug("notifying ApplicationSessionAttributeListener " + listener.getClass().getName()
                                + " of attribute added on name " + name);
                    }
                    listener.attributeAdded(event);
                } else {
                    if (LOG.isDebugEnabled()) {
                        LOG.debug("notifying ApplicationSessionAttributeListener " + listener.getClass().getName()
                                + " of attribute replaced on name " + name);
                    }
                    listener.attributeReplaced(event);
                }
            } catch (Exception e) {
                LOG.error("ApplicationSessionAttributeListener threw exception", e);
            }
        }
    }

    public int setExpires(int deltaMinutes) {
        checkValid();
        setState(ST_VALID);
        if (LOG.isDebugEnabled()) {
            LOG.debug("Postponing the expiration of the application session " + id + " to expire in " + deltaMinutes
                    + " minutes.");
        }
        long deltaMilliseconds = 0;
        deltaMilliseconds = deltaMinutes * MINUTE_IN_MILLISECONDS;
        if (updateExpirationTimerTask(System.currentTimeMillis(), deltaMilliseconds)) {
            return deltaMinutes;
        }
        return Integer.MAX_VALUE;
    }

    public long setExpiresMillis(long deltaMilliseconds) {
        checkValid();
        setState(ST_VALID);
        if (LOG.isDebugEnabled()) {
            LOG.debug("Postponing the expiration of the application session " + id + " to expire in " + deltaMilliseconds
                    + " milliseconds.");
        }
        if (updateExpirationTimerTask(System.currentTimeMillis(), deltaMilliseconds)) {
            return deltaMilliseconds;
        }
        return Long.MAX_VALUE;
    }

    public String getApplicationName() {
        return context.getServletContextName();
    }

    public boolean getInvalidateWhenReady() {
        checkValid();
        return invalidateWhenReady;
    }

    public boolean isReadyToInvalidate() {
        checkValid();
        return ST_READY_TO_INVALIDATE == state;
    }

    public void setInvalidateWhenReady(boolean invalidateWhenReady) {
        checkValid();
        this.invalidateWhenReady = invalidateWhenReady;
    }

    public ServletContextImpl getInternalServletContext() {
        return context;
    }

    public ExtServletContext getServletContext() {
        return context;
    }

    public ExtProtocolServlet getHandler(ExtProtocolContainer protocol) {
        ExtProtocolServlet requestHandler = (ExtProtocolServlet) handlers.get(protocol);
        if (requestHandler == null) {
            requestHandler = context.getDefaultServlet(protocol);
        }
        return requestHandler;
    }

    public void run() {
        long run = System.currentTimeMillis();
        if (LOG.isDebugEnabled()) {
            LOG.debug("Expiration timer has expired for application session: " + id);
        }

        long sleep = getDelay();
        if (sleep <= 0) {
            sleep = expired();
        }
        if (sleep > 0) {
            // if the session has been accessed since we started it, put it to sleep
            if (LOG.isDebugEnabled()) {
                LOG.debug("expirationTime is " + expirationTime + ", now is " + System.currentTimeMillis() + " sleeping for "
                        + sleep / SECOND_IN_MILLISECONDS + " seconds");
            }
            expirationTimerTask = new RunnableTimerTask(this);
            context.getTimerService().schedule(expirationTimerTask, sleep);
        }
        run = System.currentTimeMillis() - run;
        if (run > 500) {
            LOG.error("The application session expiration has taken more than 500 ms to be handled: " + run + " ms");
        }
    }

    public void removeProtocol(ExtProtocolContainer protocol) {
        synchronized (sessions) {
            Iterator it = sessions.values().iterator();
            ExtProtocolSession session = null;
            while (it.hasNext()) {
                session = (ExtProtocolSession) it.next();
                if (session.getProtocol() == protocol) {
                    session.invalidate();
                }
            }
        }
    }

    /**
     * Adds a servlet timer to the application session.
     *
     * @param servletTimer The servlet timer to add
     */
    public void addServletTimer(ServletTimer servletTimer) {
        if (servletTimers == null) {
            servletTimers = new HashMap();
        }
        if (!servletTimers.containsKey(servletTimer.getId())) {
            servletTimers.put(servletTimer.getId(), servletTimer);
        }
    }

    /**
     * Removes a servlet timer from the application session.
     *
     * @param servletTimer The servlet timer to remove
     */
    public void removeServletTimer(ServletTimer servletTimer) {
        if (servletTimers != null) {
            servletTimers.remove(servletTimer.getId());
        }
        updateReadyToInvalidateState();
    }

    public void invalidate(boolean bypassCheck, boolean removeFromContext) {
        // JSR 289 Section 6.1.2.2.1
        // When the IllegalStateException is thrown, the application is guaranteed
        // that the state of the ApplicationSession object will be unchanged from its state prior to the invalidate()
        // method call. Even session objects that were eligible for invalidation will not have been invalidated.
        if (!bypassCheck && ST_INVALIDATED == state) {
            throw new IllegalStateException("ApplicationSession already invalidated !");
        }
        notifyApplicationSessionListeners(EV_DELETION);

        setState(ST_INVALIDATING);
        if (LOG.isDebugEnabled()) {
            LOG.debug("Invalidating the following application session " + id);
        }

        // doing the invalidation
        Iterator it = sessions.values().iterator();
        ProtocolSession session = null;
        while (it.hasNext()) {
            session = (ProtocolSession) it.next();
            if (session.isValid()) {
                session.invalidate();
            }
        }

        if (attributes != null) {
            it = attributes.entrySet().iterator();
            Map.Entry entry = null;
            while (it.hasNext()) {
                entry = (Map.Entry) it.next();
                removeAttribute((String) entry.getKey(), entry.getValue(), it);
            }
        }

        setState(ST_INVALIDATED);
        // cancelling the timers
        if (servletTimers != null) {
            it = servletTimers.values().iterator();
            ServletTimer timer = null;
            while (it.hasNext()) {
                timer = (ServletTimer) it.next();
                timer.cancel();
            }
        }

        if (expirationTimerTask != null) {
            cancelExpirationTimer();
        }

        // This is the place for statistics

        if (removeFromContext) {
            context.removeApplicationSession(id);
        }
        expirationTimerTask = null;
        sessions.clear();
        if (servletTimers != null) {
            servletTimers.clear();
        }

        servletTimers = null;
        attributes = null;
        if (LOG.isDebugEnabled()) {
            LOG.debug("The following application session " + id + " has been invalidated");
        }
    }

    /**
     * Checks whether a timer listener is configured for the application session.
     *
     * @return Whether a timer listener is configured for the application session
     */
    public boolean hasTimerListener() {
        return context.getTimerListener() != null;
    }

    /**
     * Gets a reference to the concrete implementation of the servlet context used.
     *
     * @return The concrete implementation of the servlet context
     */
    public ServletContextImpl getServletContextInternal() {
        return context;
    }

    public void checkValid() {
        if (!isValid()) {
            throw new IllegalStateException("ApplicationSession is invalidated !");
        }
    }

    public int hashCode() {
        final int prime = 31;
        int result = 1;
        result = prime * result + ((id == null) ? 0 : id.hashCode());
        return result;
    }

    public boolean equals(Object obj) {
        if (this == obj) {
            return true;
        }
        if (obj == null) {
            return false;
        }
        if (getClass() != obj.getClass()) {
            return false;
        }
        ApplicationSessionImpl other = (ApplicationSessionImpl) obj;
        if (id == null) {
            if (other.id != null) {
                return false;
            }
        } else if (!id.equals(other.id)) {
            return false;
        }
        return true;
    }

    public String toString() {
        return getId();
    }

    private void notifyApplicationSessionListeners(int eventType) {
        BackupClassLoader backup = context.getServletContainer().getResourcesAccessorService()
                .setThreadClassLoader(context.getClass());
        ApplicationSessionEvent event = new ApplicationSessionEvent(this);
        if (LOG.isDebugEnabled()) {
            LOG.debug("Notifying application session listeners of context " + context.getServletContextName()
                    + " of following event " + eventType);
        }
        Iterator it = context.getApplicationSessionListeners();
        ApplicationSessionListener listener = null;
        while (it.hasNext()) {
            listener = (ApplicationSessionListener) it.next();
            try {
                if (LOG.isDebugEnabled()) {
                    LOG.debug("Notifying application session listener " + listener.getClass().getName() + " of context "
                            + context.getServletContextName() + " of following event " + EV_ID_TO_NAME[eventType]);
                }
                switch (eventType) {
                case EV_CREATION:
                    listener.sessionCreated(event);
                    break;
                case EV_DELETION:
                    listener.sessionDestroyed(event);
                    break;
                case EV_EXPIRATION:
                    listener.sessionExpired(event);
                    break;
                case EV_READY_TO_INVALIDATE:
                    listener.sessionReadyToInvalidate(event);
                    break;
                }
            } catch (Exception e) {
                LOG.error("ApplicationSessionListener threw exception", e);
            }
        }
        backup.restoreThreadClassLoader();
    }

    private void setState(int state) {
        if (LOG.isDebugEnabled()) {
            LOG.debug(getId() + " switching from state " + ST_ID_TO_NAME[this.state] + " to state " + ST_ID_TO_NAME[state]);
        }
        this.state = state;
    }

    private String buildSessionKey(String protocol, String id) {
        return protocol + SESSION_KEY_SEPARATOR + id;
    }

    private void cancelExpirationTimer() {
        // TODO should check return value to avoid concurrency problems
        expirationTimerTask.cancel();
    }

    private long expired() {
        notifyApplicationSessionListeners(EV_EXPIRATION);
        // It is possible that the application grant an extension to the lifetime of the session, thus the application
        // should not be treated as expired.
        long delay = getDelay();
        if (delay <= 0) {
            LOG.debug("Application session is not refreshed, the application session is going to be invalidated");
            if (isValid()) {
                invalidate(true, true);
            } else {
                LOG.debug("Application session is already invalid no need to invalidate it");
            }
        } else if (LOG.isDebugEnabled()) {
            LOG.debug("Application session has been refreshed and will expires in " + delay + " ms");
        }
        return delay;
    }

    private long getDelay() {
        return expirationTime - System.currentTimeMillis();
    }

    private void removeAttribute(String name, Object attributeValue, Iterator attributesIt) {
        if (name == null) {
            return;
        }

        ApplicationSessionBindingEvent event = null;

        if (attributes != null) {
            Object value = null;
            if (attributesIt != null) {
                attributesIt.remove();
                value = attributeValue;
            } else {
                value = attributes.remove(name);
            }

            // Call the valueUnbound() method if necessary
            if (value instanceof ApplicationSessionBindingListener) {
                event = new ApplicationSessionBindingEvent(this, name);
                ((ApplicationSessionBindingListener) value).valueUnbound(event);
            }

            Iterator it = context.getApplicationSessionAttributeListeners();
            ApplicationSessionAttributeListener listener = null;
            while (it.hasNext()) {
                if (event == null) {
                    event = new ApplicationSessionBindingEvent(this, name);
                }
                listener = (ApplicationSessionAttributeListener) it.next();
                if (LOG.isDebugEnabled()) {
                    LOG.debug("notifying ApplicationSessionAttributeListener " + listener.getClass().getName()
                            + " of attribute removed on name " + name);
                }
                try {
                    listener.attributeRemoved(event);
                } catch (Exception e) {
                    LOG.error("ApplicationSessionAttributeListener threw exception", e);
                }
            }
        }
    }

    private void updateReadyToInvalidateState() {
        if (ST_VALID == state) {
            Iterator it = getSessions();
            ProtocolSession session = null;
            while (it.hasNext()) {
                session = (ProtocolSession) it.next();
                if (session.isValid() && !session.isReadyToInvalidate()) {
                    if (LOG.isDebugEnabled()) {
                        LOG.debug("Protocol Session not ready to be invalidated : " + session.getId());
                    }
                    return;
                }
            }

            if (servletTimers == null || servletTimers.size() == 0) {
                if (LOG.isDebugEnabled()) {
                    LOG.debug("All sessions are ready to be invalidated, no timers alive, can invalidate this application session "
                            + id);
                }
                setState(ST_READY_TO_INVALIDATE);
                if (invalidateWhenReady) {
                    notifyApplicationSessionListeners(EV_READY_TO_INVALIDATE);
                    // The flag can be updated by the listener
                    if (invalidateWhenReady) {
                        invalidate(true, true);
                    }
                }
            } else if (LOG.isDebugEnabled()) {
                LOG.debug(servletTimers.size() + " Timers still alive, cannot invalidate this application session " + id);
            }
        } else if (LOG.isDebugEnabled()) {
            LOG.debug("Application session already invalidated " + id);
        }
    }

    private boolean updateExpirationTimerTask(long now, long delay) {
        boolean result = true;
        if (delay <= 0) {
            LOG.debug("The application session will never expire");
            expirationTime = -1;
            if (expirationTimerTask != null) {
                cancelExpirationTimer();
                expirationTimerTask = null;
            }
            result = false;
        } else {
            if (LOG.isDebugEnabled()) {
                LOG.debug("The application session will expire in " + delay);
            }
            long oldExpirationTime = expirationTime;
            expirationTime = now + delay;
            if (expirationTimerTask != null) {
                if (oldExpirationTime > expirationTime) {
                    cancelExpirationTimer();
                    expirationTimerTask = new RunnableTimerTask(this);
                    context.getTimerService().schedule(expirationTimerTask, delay);
                }
            } else {
                expirationTimerTask = new RunnableTimerTask(this);
                context.getTimerService().schedule(expirationTimerTask, delay);
            }
        }
        return result;
    }
}
