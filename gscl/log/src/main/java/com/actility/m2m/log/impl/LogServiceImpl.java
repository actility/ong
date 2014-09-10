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
 * id $Id: $
 * author $Author: $
 * version $Revision: $
 * lastrevision $Date: $
 * modifiedby $LastChangedBy: $
 * lastmodified $LastChangedDate: $
 */

package com.actility.m2m.log.impl;

import org.osgi.framework.Bundle;
import org.osgi.framework.ServiceReference;

import com.actility.m2m.log.LogService;

public class LogServiceImpl implements LogService {

    /** The bundle that owns this service object. */
    private Bundle bundle;

    /** The log reader service factory to forward requests to. */
    private LogReaderServiceImpl logReaderService;

    /**
     * The constructor saves the Bundle and the LogReaderServiceFactory.
     *
     * @param bundle
     *            The bundle that requested this service.
     * @param lrsf
     *            The LogReaderServiceFactory that implements the log
     *            functionality.
     */
    public LogServiceImpl(Bundle bundle, LogReaderServiceImpl lrsf) {
        this.bundle = bundle;
        this.logReaderService = lrsf;
    }

    /**
     * Create LogEntry object and send it to <code>lrsf</code>.
     *
     * @param level
     *            The severity level of the entry to create.
     * @param msg
     *            The message of the entry to create.
     */
    public void log(int level, String msg) {
        logReaderService.log(new LogEntryImpl(bundle, level, msg));
    }

    /**
     * Create LogEntry object and send it to <code>lrsf</code>.
     *
     * @param level
     *            The severity level of the entry to create.
     * @param msg
     *            The message of the entry to create.
     * @param t
     *            A Throwable that goes with the entry.
     */
    public void log(int level, String msg, Throwable t) {
        logReaderService.log(new LogEntryImpl(bundle, level, msg, t));
    }

    /**
     * Create LogEntry object and send it to <code>lrsf</code>.
     *
     * @param sref
     *            A ServiceReference for the service that wants this entry.
     * @param level
     *            The severity level of the entry to create.
     * @param msg
     *            The message of the entry to create.
     */
    public void log(ServiceReference sref, int level, String msg) {
        logReaderService.log(new LogEntryImpl(bundle, sref, level, msg));
    }

    /**
     * Create LogEntry object and send it to <code>lrsf</code>.
     *
     * @param sref
     *            A ServiceReference for the service that wants this entry.
     * @param level
     *            The severity level of the entry to create.
     * @param msg
     *            The message of the entry to create.
     * @param t
     *            A Throwable that goes with the entry.
     */
    public void log(ServiceReference sref, int level, String msg, Throwable t) {
        logReaderService.log(new LogEntryImpl(bundle, sref, level, msg, t));
    }

    /**
     * Get the current log level. The file log and the memory log will discard
     * log entries with a level that is less severe than the current level.
     * I.e., entries with a level that is numerically larger than the value
     * returned by this method. All log entries are always forwarded to registered
     * log listeners.
     *
     * E.g., If the current log level is LOG_WARNING then the log
     * will discard all log entries with level LOG_INFO and LOG_DEBUG. I.e.,
     * there is no need for a bundle to try to send such log entries to the
     * log. The bundle may actually save a number of CPU-cycles by getting the
     * log level and do nothing if the intended log entry is less severe than
     * the current log level.
     *
     * @return the lowest severity level that is accepted into the log.
     */
    public int getLogLevel() {
        return logReaderService.getLogLevel(bundle);
    }

}
