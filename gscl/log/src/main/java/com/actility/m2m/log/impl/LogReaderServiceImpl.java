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

import java.util.ArrayList;
import java.util.Collections;
import java.util.Enumeration;
import java.util.Iterator;
import java.util.List;

import org.osgi.framework.Bundle;
import org.osgi.framework.BundleContext;
import org.osgi.service.log.LogEntry;
import org.osgi.service.log.LogListener;
import org.osgi.service.log.LogReaderService;

public final class LogReaderServiceImpl implements LogReaderService {
    /** The history list (an array used as a circular list). */
    private LogEntry[] history;

    /**
     * The index in <code>history</code> where the next entry shall be * inserted.
     */
    private int historyInsertionPoint = 0;

    /**
     * A Vector with LogListener objects.
     */
    private List/* <LogListener> */listeners = new ArrayList/* <LogListener> */(2);

    /**
     * A FileLog that writes log entries to file. (Accessed from the LogConfigCommandGroup).
     */
    private final FileLog fileLog;

    private final LogConfiguration configuration;

    /**
     * The constructor fetches the destination(s) of the log entries from the system properties.
     *
     * @param context Our handle to the framework.
     * @param configuration The log configuration to use.
     */
    public LogReaderServiceImpl(BundleContext context, LogConfiguration configuration) {
        this.configuration = configuration;

        history = new LogEntry[configuration.getMaxNbOfLogsInMemory()];
        historyInsertionPoint = 0;

        fileLog = new FileLog(context, configuration);
    }

    /**
     * Subscribe method.
     *
     * @param listener A log listener to be notify when new log entries arrives.
     */
    public synchronized void addLogListener(LogListener listener) {
        if (listener == null) {
            throw new IllegalArgumentException("LogListener can not be null");
        }
        if (!listeners.contains(listener)) {
            listeners.add(listener);
        }
    }

    /**
     * Unsubscribe method. LogListeners are removed when number of subscriptions are 0.
     *
     * @param listener A log listener to be removed.
     */
    public synchronized void removeLogListener(LogListener listener) {
        if (listener == null) {
            throw new IllegalArgumentException("LogListener can not be null");
        }
        listeners.remove(listener);
    }

    /**
     * The stop method is called by the bundle activator when the log bundle is stoped. If a <code>fileLog</code> is active shut
     * it down.
     */
    public void stop() {
        synchronized (this) {
            if (fileLog != null) {
                fileLog.stop();
            }
        }
    }

    /*
     * Return an enumeration of the historyLength last entries in the log.
     */
    public synchronized Enumeration getLog() {
        List logs = new ArrayList();
        int i = historyInsertionPoint;
        do {
            if (history[i] != null) {
                logs.add(history[i]);
            }
            i = (i + 1) % history.length;
        } while (i != historyInsertionPoint);
        return Collections.enumeration(logs);
    }

    /**
     * Returns the filter level for a specific bundle.
     *
     * @param bundle the bundle to get the filter level for.
     * @return log filter level for the specified bundle.
     */
    public int getLogLevel(Bundle bundle) {
        return (bundle != null) ? configuration.getLogLevelForBundle(bundle) : configuration.getLogLevel();
    }

    /**
     * A new log entry has arrived. If its numeric level is less than or equal to the filter level then output it and store it
     * in the memory log. All LogReaderServiceImpls are notified for all new logEntries, i.e., the logFilter is not used for
     * this. The LogReaderService will notify the LogListeners.
     *
     * @param logEntry The new LogEntry
     */
    public synchronized void log(LogEntryImpl logEntry) {
        if (logEntry.getLevel() <= getLogLevel(logEntry.getBundle())) {
            fileLog.logged(logEntry);
            if (configuration.getLogToStdout()) {
                System.out.println(logEntry);
            }
            int maxSize = configuration.getMaxNbOfLogsInMemory();
            if (history.length != maxSize) {
                // Fix memory size
                LogEntry[] newHistory = new LogEntry[maxSize];
                if (newHistory.length < history.length - historyInsertionPoint) {
                    System.arraycopy(history, historyInsertionPoint, newHistory, 0, newHistory.length);
                } else {
                    System.arraycopy(history, historyInsertionPoint, newHistory, 0, history.length - historyInsertionPoint);
                    if (newHistory.length - (history.length - historyInsertionPoint) < historyInsertionPoint) {
                        System.arraycopy(history, 0, newHistory, history.length - historyInsertionPoint, newHistory.length
                                - (history.length - historyInsertionPoint));
                    } else {
                        System.arraycopy(history, 0, newHistory, history.length - historyInsertionPoint, historyInsertionPoint);
                    }
                    historyInsertionPoint = 0;
                    this.history = newHistory;
                }
            }
            history[historyInsertionPoint] = logEntry;
            historyInsertionPoint = (historyInsertionPoint + 1) % history.length;
        }
        callback(logEntry);
    }

    /**
     * Used by {@link LogReaderServiceImpl} for every new log entry. Note that the callback operation is not disturbed by
     * changes in the listener set since such changes will result in that <code>listerners</code> refers to a new object, but
     * the callback operation will continue its enumeration of the old listeners object.
     *
     * @param logEntry A log entry to send to all listeners.
     */
    private void callback(LogEntry logEntry) {
        if (listeners.size() > 0) {
            Iterator it = listeners.iterator();

            while (it.hasNext()) {
                try {
                    ((LogListener) it.next()).logged(logEntry);
                } catch (Exception exc) {
                }
            }
        }
    }
}