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
 * id $Id: TimerService.java 6081 2013-10-15 13:33:50Z mlouiset $
 * author $Author: mlouiset $
 * version $Revision: 6081 $
 * lastrevision $Date: 2013-10-15 15:33:50 +0200 (Tue, 15 Oct 2013) $
 * modifiedby $LastChangedBy: mlouiset $
 * lastmodified $LastChangedDate: 2013-10-15 15:33:50 +0200 (Tue, 15 Oct 2013) $
 */

package com.actility.m2m.servlet;

import java.io.Serializable;

import javax.servlet.ServletContext;

/**
 * Allows servlet applications to set timers in order to receive notifications on timer expiration. Applications receive such
 * notifications through an implementation of the TimerListener interface. Applications using timers must implement this
 * interface and declare it as listener in the deployment descriptor. Servlet containers are required to make a TimerService
 * instance available to applications through a ServletContext attribute with name com.actility.servlet.TimerService.
 *
 * @see TimerListener ApplicationSession#getTimers()
 */
public interface TimerService {
    /**
     * Creates a one-time ServletTimer and schedules it to expire after the specified delay.
     * <p>
     * Not yet implemented.
     *
     * @param context The application context in which the timer is created
     * @param delay The time in milliseconds in which the timer while elapse
     * @param isPersistent Whether the timer is persistent
     * @param info A serializable information linked to the timer
     * @return The created timer
     */
    ServletTimer createTimer(ServletContext context, long delay, boolean isPersistent, Serializable info);

    /**
     * Creates a repeating ServletTimer and schedules it to expire after the specified delay and then again at approximately
     * regular intervals. The ServletTimer is rescheduled to expire in either a fixed-delay or fixed-rate manner as specified by
     * the fixedDelay argument. The semantics are the same as for Timer: In fixed-delay execution, each execution is scheduled
     * relative to the actual execution time of the previous execution. If an execution is delayed for any reason (such as
     * garbage collection or other background activity), subsequent executions will be delayed as well. In the long run, the
     * frequency of execution will generally be slightly lower than the reciprocal of the specified period (assuming the system
     * clock underlying Object.wait(long) is accurate). In fixed-rate execution, each execution is scheduled relative to the
     * scheduled execution time of the initial execution. If an execution is delayed for any reason (such as garbage collection
     * or other background activity), two or more executions will occur in rapid succession to "catch up." In the long run, the
     * frequency of execution will be exactly the reciprocal of the specified period (assuming the system clock underlying
     * Object.wait(long) is accurate).
     * <p>
     * Not yet implemented.
     *
     * @param context The application context in which the timer is created
     * @param delay The time in milliseconds in which the timer while elapse the first time
     * @param period The time in milliseconds between each expiration of the timer
     * @param fixedDelay Whether the delay is fixed
     * @param isPersistent Whether the timer is persistent
     * @param info A serializable information linked to the timer
     * @return The created timer
     */
    ServletTimer createTimer(ServletContext context, long delay, long period, boolean fixedDelay, boolean isPersistent,
            Serializable info);

    /**
     * Creates a one-time ServletTimer and schedules it to expire after the specified delay.
     *
     * @param appSession The application session in which the timer is created
     * @param delay The time in milliseconds in which the timer while elapse
     * @param isPersistent Whether the timer is persistent
     * @param info A serializable information linked to the timer
     * @return The created timer
     */
    ServletTimer createTimer(ApplicationSession appSession, long delay, boolean isPersistent, Serializable info);

    /**
     * Creates a repeating ServletTimer and schedules it to expire after the specified delay and then again at approximately
     * regular intervals. The ServletTimer is rescheduled to expire in either a fixed-delay or fixed-rate manner as specified by
     * the fixedDelay argument. The semantics are the same as for Timer: In fixed-delay execution, each execution is scheduled
     * relative to the actual execution time of the previous execution. If an execution is delayed for any reason (such as
     * garbage collection or other background activity), subsequent executions will be delayed as well. In the long run, the
     * frequency of execution will generally be slightly lower than the reciprocal of the specified period (assuming the system
     * clock underlying Object.wait(long) is accurate). In fixed-rate execution, each execution is scheduled relative to the
     * scheduled execution time of the initial execution. If an execution is delayed for any reason (such as garbage collection
     * or other background activity), two or more executions will occur in rapid succession to "catch up." In the long run, the
     * frequency of execution will be exactly the reciprocal of the specified period (assuming the system clock underlying
     * Object.wait(long) is accurate).
     *
     * @param appSession The application session in which the timer is created
     * @param delay The time in milliseconds in which the timer while elapse the first time
     * @param period The time in milliseconds between each expiration of the timer
     * @param fixedDelay Whether the delay is fixed
     * @param isPersistent Whether the timer is persistent
     * @param info A serializable information linked to the timer
     * @return The created timer
     */
    ServletTimer createTimer(ApplicationSession appSession, long delay, long period, boolean fixedDelay, boolean isPersistent,
            Serializable info);
}
