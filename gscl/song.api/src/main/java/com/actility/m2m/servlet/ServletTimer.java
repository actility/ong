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
 * id $Id: ServletTimer.java 6081 2013-10-15 13:33:50Z mlouiset $
 * author $Author: mlouiset $
 * version $Revision: 6081 $
 * lastrevision $Date: 2013-10-15 15:33:50 +0200 (Tue, 15 Oct 2013) $
 * modifiedby $LastChangedBy: mlouiset $
 * lastmodified $LastChangedDate: 2013-10-15 15:33:50 +0200 (Tue, 15 Oct 2013) $
 */

package com.actility.m2m.servlet;

import java.io.Serializable;

/**
 * Created by the TimerService for servlet applications wishing to schedule future tasks.
 *
 * @see TimerService TimerListener
 */
public interface ServletTimer {
    /**
     * Cancels this timer. If the task has been scheduled for one-time execution and has not yet expired, or has not yet been
     * scheduled, it will never run. If the task has been scheduled for repeated execution, it will never expire again. Note
     * that calling this method on a repeating ServletTimer from within the timerFired method of a TimerListener absolutely
     * guarantees that the timer will not fire again (unless rescheduled). This method may be called repeatedly; the second and
     * subsequent calls have no effect.
     */
    void cancel();

    /**
     * Returns a string containing the unique identifier assigned to this timer task. The identifier is assigned by the servlet
     * container and is implementation dependent.
     *
     * @return a string specifying the identifier assigned to this session
     */
    String getId();

    /**
     * Get the information associated with the timer at the time of creation.
     *
     * @return The information associated with the timer
     */
    Serializable getInfo();

    /**
     * Returns the scheduled expiration time of the most recent actual expiration of this timer. This method is typically
     * invoked from within TimerService timerFired to determine whether the timer callback was sufficiently timely to warrant
     * performing the scheduled activity:
     * <code>
     * public void run() {
     *   if (System.currentTimeMillis() - scheduledExecutionTime() &gt;= MAX_TARDINESS)
     *     return; // Too late; skip this execution.
     *             // Perform the task
     * }
     * </code>
     * This method is typically not used in conjunction with fixed-delay execution repeating tasks, as
     * their scheduled execution times are allowed to drift over time, and so are not terribly significant.
     *
     * @return The scheduled expiration time of the timer
     */
    long scheduledExecutionTime();

    /**
     * Get the number of milliseconds that will elapse before the next scheduled timer expiration. For a one-time timer that has
     * already expired (i.e., current time &gt; scheduled expiry time) this method will return the time remaining as a negative
     * value.
     *
     * @return the number of milliseconds that will elapse before the next scheduled timer expiration.
     */
    long getTimeRemaining();

    /**
     * Returns the application session associated with this {@link ServletTimer}.
     *
     * @return The application session assiocated with this {@link ServletTimer}
     */
    ApplicationSession getApplicationSession();
}
