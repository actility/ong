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
 * id $Id: TimerServiceImpl.java 9481 2014-09-06 09:37:01Z JReich $
 * author $Author: JReich $
 * version $Revision: 9481 $
 * lastrevision $Date: 2014-09-06 11:37:01 +0200 (Sat, 06 Sep 2014) $
 * modifiedby $LastChangedBy: JReich $
 * lastmodified $LastChangedDate: 2014-09-06 11:37:01 +0200 (Sat, 06 Sep 2014) $
 */

package com.actility.m2m.servlet.impl;

import java.io.Serializable;
import java.util.Timer;

import javax.servlet.ServletContext;

import org.apache.log4j.Logger;

import com.actility.m2m.servlet.ApplicationSession;
import com.actility.m2m.servlet.ServletTimer;
import com.actility.m2m.servlet.TimerListener;
import com.actility.m2m.servlet.TimerService;
import com.actility.m2m.servlet.log.BundleLogger;
import com.actility.m2m.util.log.OSGiLogger;

public final class TimerServiceImpl extends Timer implements TimerService {
    private static final Logger LOG = OSGiLogger.getLogger(TimerService.class, BundleLogger.getStaticLogger());

    public ServletTimer createTimer(ServletContext context, long delay, boolean isPersistent, Serializable info) {
        throw new UnsupportedOperationException();
    }

    public ServletTimer createTimer(ServletContext context, long delay, long period, boolean fixedDelay, boolean isPersistent,
            Serializable info) {
        throw new UnsupportedOperationException();
    }

    public ServletTimer createTimer(ApplicationSession appSession, long delay, boolean isPersistent, Serializable info) {
        if (LOG.isInfoEnabled()) {
            LOG.info("Create timer for application " + appSession.getApplicationName() + " (delay=" + delay + ", persistent="
                    + isPersistent + ")");
        }
        ApplicationSessionImpl applicationSessionImpl = (ApplicationSessionImpl) appSession;

        applicationSessionImpl.checkValid();

        if (!applicationSessionImpl.hasTimerListener()) {
            throw new IllegalStateException("No Timer listeners have been configured for this application ");
        }
        TimerListener listener = applicationSessionImpl.getServletContextInternal().getTimerListener();
        ServletTimerImpl servletTimer = createTimerLocally(listener, delay, isPersistent, info, applicationSessionImpl);

        return servletTimer;
    }

    public ServletTimer createTimer(ApplicationSession appSession, long delay, long period, boolean fixedDelay,
            boolean isPersistent, Serializable info) {
        if (period < 1) {
            throw new IllegalArgumentException("Period should be greater than 0");
        }
        if (LOG.isInfoEnabled()) {
            LOG.info("Create timer for application " + appSession.getApplicationName() + " (delay=" + delay + ", period="
                    + period + ", fixedDelay=" + fixedDelay + ", persistent=" + isPersistent + ")");
        }
        ApplicationSessionImpl applicationSessionImpl = (ApplicationSessionImpl) appSession;

        applicationSessionImpl.checkValid();

        if (applicationSessionImpl.getServletContextInternal().getTimerListener() == null) {
            throw new IllegalStateException("No Timer listeners have been configured for this application ");
        }
        TimerListener timerListener = applicationSessionImpl.getServletContextInternal().getTimerListener();
        ServletTimerImpl servletTimer = createTimerLocally(timerListener, delay, period, fixedDelay, isPersistent, info,
                applicationSessionImpl);

        return servletTimer;
    }

    /**
     *
     * @param listener
     * @param delay
     * @param isPersistent
     * @param info
     * @param applicationSession
     * @return
     */
    private ServletTimerImpl createTimerLocally(TimerListener listener, long delay, boolean isPersistent, Serializable info,
            ApplicationSessionImpl applicationSession) {
        ServletTimerImpl servletTimer = new ServletTimerImpl(info, delay, listener, applicationSession);
        super.schedule(servletTimer.getServletTimerTask(), delay);
        applicationSession.addServletTimer(servletTimer);
        if (isPersistent) {
            persist(servletTimer);
        }
        return servletTimer;
    }

    /**
     *
     * @param listener
     * @param delay
     * @param period
     * @param fixedDelay
     * @param isPersistent
     * @param info
     * @param applicationSession
     * @return
     */
    private ServletTimerImpl createTimerLocally(TimerListener listener, long delay, long period, boolean fixedDelay,
            boolean isPersistent, Serializable info, ApplicationSessionImpl applicationSession) {
        final ServletTimerImpl servletTimer = new ServletTimerImpl(info, delay, fixedDelay, period, listener,
                applicationSession);
        if (fixedDelay) {
            super.schedule(servletTimer.getServletTimerTask(), delay, period);
        } else {
            super.scheduleAtFixedRate(servletTimer.getServletTimerTask(), delay, period);
        }
        applicationSession.addServletTimer(servletTimer);
        if (isPersistent) {
            persist(servletTimer);
        }
        return servletTimer;
    }

    /**
     *
     * @param st
     */
    private void persist(ServletTimerImpl st) {
        // TODO - implement persistance

    }

    public void stop() {
        super.cancel();
        if (LOG.isInfoEnabled()) {
            LOG.info("Stopped timer service " + this);
        }
    }

    public void start() {
        if (LOG.isInfoEnabled()) {
            LOG.info("Started timer service " + this);
        }
    }
}
