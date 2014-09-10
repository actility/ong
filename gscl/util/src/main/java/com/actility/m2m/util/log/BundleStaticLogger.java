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
 * id $Id: BundleStaticLogger.java 8096 2014-03-12 13:10:45Z JReich $
 * author $Author: JReich $
 * version $Revision: 8096 $
 * lastrevision $Date: 2014-03-12 14:10:45 +0100 (Wed, 12 Mar 2014) $
 * modifiedby $LastChangedBy: JReich $
 * lastmodified $LastChangedDate: 2014-03-12 14:10:45 +0100 (Wed, 12 Mar 2014) $
 */

package com.actility.m2m.util.log;

import java.io.PrintStream;
import java.io.UnsupportedEncodingException;

import org.osgi.framework.BundleContext;
import org.osgi.framework.InvalidSyntaxException;
import org.osgi.framework.ServiceEvent;
import org.osgi.framework.ServiceListener;
import org.osgi.framework.ServiceReference;

import com.actility.m2m.log.LogService;
import com.actility.m2m.util.ByteArrayOutputStream;

/**
 * Utils class use to log messages with the Log4J API while using the OSGi LogService in the backend
 *
 */
public final class BundleStaticLogger implements ServiceListener {

    // Class name of the OSGI log service
    private static final String LOG_CLASS_OSGI = org.osgi.service.log.LogService.class.getName();

    // Class name of Knopflerfish extended log service
    private static final String LOG_CLASS_M2M = com.actility.m2m.log.LogService.class.getName();

    private static final String LOG_SERVICE_FILTER = "(|" + "(objectClass=" + LOG_CLASS_M2M + ")(objectClass=" + LOG_CLASS_OSGI
            + "))";

    // Handle to the framework
    private BundleContext bc;

    // Service reference for the current log service
    private ServiceReference logSR;

    // The current log service
    private org.osgi.service.log.LogService log;

    // The id of the calling bundle
    private String bundleName;

    private boolean m2mLog;

    // private final CalendarThreadLocal localCalendar;

     public BundleStaticLogger() {
         // localCalendar = new CalendarThreadLocal();
         init(null);
     }

    /**
     * Initializes the log stack for a bundle given its {@link BundleContext}.
     *
     * @param bc The BundleContext of the bundle which is initializing the log stack
     */
    public synchronized void init(BundleContext bc) {
        StringBuffer bundleName = new StringBuffer(24);
        if (bc != null) {
            bundleName.append("bid#");
            bundleName.append(String.valueOf(bc.getBundle().getBundleId()));
        } else {
            bundleName.append("unknown");
        }
        if (bundleName.length() < 8) {
            bundleName.append("        ");
            bundleName.setLength(8);
        }
        this.bundleName = bundleName.toString();
        this.bc = bc;
        if (bc != null) {
            try {
                bc.addServiceListener(this, LOG_SERVICE_FILTER);
            } catch (InvalidSyntaxException e) {
                doLog("Failed to register log service listener (filter=" + LOG_SERVICE_FILTER + ")", "ERROR  ",
                        org.osgi.service.log.LogService.LOG_DEBUG, e);
            }
        }
    }

    public void serviceChanged(ServiceEvent evt) {
        if (evt.getServiceReference() == logSR && evt.getType() == ServiceEvent.UNREGISTERING) {
            ungetLogService();
        }
    }

    /**
     * Unget the log service. Note that this method is synchronized on the same object as the internal method that calls the
     * actual log service. This ensures that the log service is not removed by this method while a log message is generated.
     */
    private synchronized void ungetLogService() {
        if (log != null) {
            bc.ungetService(logSR);
            logSR = null;
            log = null;
        }
    }

    /**
     * Sends a message to the LogService if possible.
     *
     * @param msg Human readable string describing the log.
     * @param levelName The log level as a string
     * @param level The severity of the message (Should be one of the four predefined severities).
     * @param e The exception eventually coming with the log message (could be null).
     */
    public void doLog(String msg, String levelName, int level, Throwable e) {
        if (bc != null && log == null) {
            synchronized (this) {
                if (bc != null && log == null) {
                    try {
                        logSR = bc.getServiceReference(LOG_CLASS_M2M);
                        if (logSR == null) {
                            // No service implementing the Knopflerfish
                            // extended log, try to look for a standard OSGi
                            // log service.
                            logSR = bc.getServiceReference(LOG_CLASS_OSGI);
                        }
                        if (logSR != null) {
                            log = (org.osgi.service.log.LogService) bc.getService(logSR);
                        }
                        if (log == null) {
                            // Failed to get log service clear the service reference.
                            logSR = null;
                        } else {
                            m2mLog = (log instanceof LogService);
                        }
                    } catch (IllegalStateException ise) {
                        // Bundle not active, can not fetch a log service.
                        bc = null;
                        log = null;
                        logSR = null;
                    }
                }
            }
        }
        if (log != null) {
            log.log(null, level, msg, e);
            if (e != null) {
                log.log(null, level, getStackTrace(e), null);
            }
        }
//        else if (bc != null) {
//            System.out.print(levelName);
//            System.out.print(' ');
//            // System.out.print(FormatUtils.formatDateTime(new Date(), (Calendar) localCalendar.get()));
//            System.out.print(FormatUtils.formatDateTime(new Date(), UtilConstants.LOCAL_TIMEZONE));
//            System.out.print(' ');
//            System.out.print(bundleName);
//            System.out.print(" - ");
//            System.out.print(msg);
//            if (e != null) {
//                System.out.print(" (");
//                System.out.print(e.toString());
//                System.out.print(')');
//                e.printStackTrace();
//            }
//            System.out.println();
//        }
    }

    private String getStackTrace(Throwable t) {
        ByteArrayOutputStream baos = new ByteArrayOutputStream();
        PrintStream ps = new PrintStream(baos);
        String exception = null;
        try {
            t.printStackTrace(ps);
            ps.flush();
            exception = baos.toString("UTF-8");
        } catch (UnsupportedEncodingException e) {
            // Internal ERROR

        } finally {
            ps.close();
        }
        return exception;
    }

    /**
     * Returns the current log level. There is no use to generate log entries with a severity level less than this value since
     * such entries will be thrown away by the log.
     *
     * @return the current severity log level for this bundle.
     */
    public int getLogLevel() {
        if (log != null && m2mLog) {
            return ((LogService) log).getLogLevel();
        }
        return org.osgi.service.log.LogService.LOG_DEBUG;
    }

    /**
     * Returns true if messages with severity debug or higher are saved by the log.
     *
     * @return <code>true</code> if messages with severity LOG_DEBUG or higher are included in the log, otherwise
     *         <code>false</code>.
     */
    public boolean doDebug() {
        return getLogLevel() >= org.osgi.service.log.LogService.LOG_DEBUG;
    }

    /**
     * Returns true if messages with severity warning or higher are saved by the log.
     *
     * @return <code>true</code> if messages with severity LOG_WARNING or higher are included in the log, otherwise
     *         <code>false</code>.
     */
    public boolean doWarn() {
        return getLogLevel() >= org.osgi.service.log.LogService.LOG_WARNING;
    }

    /**
     * Returns true if messages with severity info or higher are saved by the log.
     *
     * @return <code>true</code> if messages with severity LOG_INFO or higher are included in the log, otherwise
     *         <code>false</code>.
     */
    public boolean doInfo() {
        return getLogLevel() >= org.osgi.service.log.LogService.LOG_INFO;
    }

    /**
     * Returns true if messages with severity error or higher are saved by the log.
     *
     * @return <code>true</code> if messages with severity LOG_ERROR or higher are included in the log, otherwise
     *         <code>false</code>.
     */
    public boolean doError() {
        return getLogLevel() >= org.osgi.service.log.LogService.LOG_ERROR;
    }
}
