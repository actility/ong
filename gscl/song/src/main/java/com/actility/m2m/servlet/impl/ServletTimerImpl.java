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
 * id $Id: ServletTimerImpl.java 8767 2014-05-21 15:41:33Z JReich $
 * author $Author: JReich $
 * version $Revision: 8767 $
 * lastrevision $Date: 2014-05-21 17:41:33 +0200 (Wed, 21 May 2014) $
 * modifiedby $LastChangedBy: JReich $
 * lastmodified $LastChangedDate: 2014-05-21 17:41:33 +0200 (Wed, 21 May 2014) $
 */

package com.actility.m2m.servlet.impl;

import java.io.Serializable;

import org.apache.log4j.Logger;

import com.actility.m2m.servlet.ApplicationSession;
import com.actility.m2m.servlet.ServletTimer;
import com.actility.m2m.servlet.TimerListener;
import com.actility.m2m.servlet.log.BundleLogger;
import com.actility.m2m.util.RunnableTimerTask;
import com.actility.m2m.util.UUID;
import com.actility.m2m.util.log.OSGiLogger;

public final class ServletTimerImpl implements ServletTimer, Runnable {
    private static final Logger LOG = OSGiLogger.getLogger(ServletTimer.class, BundleLogger.getStaticLogger());

    private static final int UUID_LENGTH = 40;

    private RunnableTimerTask timerTask;
    private final ApplicationSessionImpl appSession;
    private final String id;
    private final Serializable info;
    private final boolean repeatingTimer;
    private long scheduledExecutionTime;
    private long delay;
    private long period;
    private long numInvocations;
    private boolean fixedDelay;
    private long firstExecution;
    private TimerListener listener;

    /**
     * Constructor for non-repeating timer.
     *
     * @param info Information about the timer
     * @param delay Delay until execution
     * @param listener Listener that will get timeout events.
     */
    public ServletTimerImpl(Serializable info, long delay, TimerListener listener, ApplicationSessionImpl appSession) {
        this(info, delay, false, 0, listener, appSession);
    }

    /**
     * Constructor for repeating times
     *
     * @param info Information about the timer
     * @param delay Delay until first execution
     * @param fixedDelay Whether fixed delay mode should be used
     * @param period Period between execution
     * @param listener Listener that will get timeout events.
     */
    public ServletTimerImpl(Serializable info, long delay, boolean fixedDelay, long period, TimerListener listener,
            ApplicationSessionImpl appSession) {
        this.id = UUID.randomUUID(UUID_LENGTH);
        this.info = info;
        this.delay = delay;
        this.scheduledExecutionTime = delay + System.currentTimeMillis();
        this.fixedDelay = fixedDelay;
        this.period = period;
        this.listener = listener;
        this.appSession = appSession;
        this.numInvocations = 0;
        this.firstExecution = 0;
        this.repeatingTimer = (period > 0);
        this.timerTask = new RunnableTimerTask(this);
    }

    public void cancel() {
        LOG.debug("removeServletTimer");
        appSession.removeServletTimer(this);
        LOG.debug("cancel timerTask");
        timerTask.cancel();
    }

    public ApplicationSession getApplicationSession() {
        return appSession;
    }

    public String getId() {
        return id;
    }

    public Serializable getInfo() {
        return info;
    }

    public synchronized long getTimeRemaining() {
        return scheduledExecutionTime - System.currentTimeMillis();
    }

    public synchronized long scheduledExecutionTime() {
        return this.scheduledExecutionTime;
    }

    public RunnableTimerTask getServletTimerTask() {
        return timerTask;
    }

    public String toString() {
        StringBuffer sb = new StringBuffer();
        sb.append("Info = ").append(info).append(System.getProperty("line.separator")).append("Scheduled execution time = ");
        synchronized (this) {
            sb.append(scheduledExecutionTime);
        }
        sb.append(System.getProperty("line.separator"));
        sb.append("Time now = ").append(System.currentTimeMillis()).append(System.getProperty("line.separator"));
        sb.append("ApplicationSession = ").append(appSession).append(System.getProperty("line.separator"));
        sb.append("Delay = ").append(delay).append(System.getProperty("line.separator"));
        return sb.toString();
    }

    /**
     * Helper to calculate when next execution time is.
     *
     */
    private synchronized void estimateNextExecution() {
        if (fixedDelay) {
            scheduledExecutionTime = period + System.currentTimeMillis();
        } else {
            if (firstExecution == 0) {
                // save timestamp of first execution
                firstExecution = scheduledExecutionTime;
            }
            long now = System.currentTimeMillis();
            long executedTime = (numInvocations * period);
            numInvocations++;
            scheduledExecutionTime = firstExecution + executedTime;
            if (LOG.isDebugEnabled()) {
                LOG.debug("next execution estimated to run at " + scheduledExecutionTime);
                LOG.debug("current time is " + now);
            }
        }
    }

    public void run() {
        long run = System.currentTimeMillis();
        // PORTAGE chgt de classLoader pour le currentThread
        // ClassLoader oldClassLoader = Thread.currentThread().getContextClassLoader();
        try {
            // ClassLoader cl = listener.getClass().getClassLoader();
            // Thread.currentThread().setContextClassLoader(cl);
            LOG.debug("Call timeout");
            listener.timeout(this);
        } catch (Exception e) {
            LOG.error("An unexpected exception happened in the timer callback", e);
        } finally {
            // Thread.currentThread().setContextClassLoader(oldClassLoader);
            if (repeatingTimer) {
                LOG.debug("Reset repeating servlet timer");
                estimateNextExecution();
            } else {
                LOG.debug("Cancel servlet timer");
                // this non-repeating timer is now "ready"
                // and should not be included in the list of active timers
                // The application may already have canceled() the timer though
                cancel(); // dont bother about return value....
            }
        }
        run = System.currentTimeMillis() - run;
        if (run > 500) {
            LOG.error(appSession.getId() + ": The servlet timer has taken more than 500 ms to be handled: " + run + " ms");
        }
    }
}
