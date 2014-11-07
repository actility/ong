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
 * id $Id: Logger.java 8774 2014-05-21 15:50:27Z JReich $
 * author $Author: JReich $
 * version $Revision: 8774 $
 * lastrevision $Date: 2014-05-21 17:50:27 +0200 (Wed, 21 May 2014) $
 * modifiedby $LastChangedBy: JReich $
 * lastmodified $LastChangedDate: 2014-05-21 17:50:27 +0200 (Wed, 21 May 2014) $
 */

package org.apache.log4j;

import com.actility.m2m.util.log.BundleLogger;
import com.actility.m2m.util.log.BundleStaticLogger;

/**
 * Fake LOG4J Logger class which redirects logs to OSGi Log service
 */
public final class Logger extends Category {

    private static final Logger DEFAULT = new Logger(new BundleStaticLogger(), "root");

    public Logger(BundleStaticLogger bundleLogger, String name) {
        super(bundleLogger, name);
    }

    /**
     * Retrieve a logger named according to the value of the <code>name</code> parameter. If the named logger already exists,
     * then the existing instance will be returned. Otherwise, a new instance is created.
     *
     * <p>
     * By default, loggers do not have a set level but inherit it from their neareast ancestor with a set level. This is one of
     * the central features of log4j.
     *
     * @param name The name of the logger to retrieve.
     * @return The built Logger
     */
    public static Logger getLogger(String name) {
        return new Logger(BundleLogger.getStaticLogger(), name);
    }

    /**
     * Shorthand for <code>getLogger(clazz.getName())</code>.
     *
     * @param clazz The name of <code>clazz</code> will be used as the name of the logger to retrieve. See
     *            {@link #getLogger(String)} for more detailed information.
     * @return The built Logger
     */
    public static Logger getLogger(Class clazz) {
        return new Logger(BundleLogger.getStaticLogger(), clazz.getName());
    }

    /**
     * Return the root logger for the current logger repository.
     * <p>
     * The {@link #getName Logger.getName()} method for the root logger always returns stirng value: "root". However, calling
     * <code>Logger.getLogger("root")</code> does not retrieve the root logger but a logger just under root named "root".
     * <p>
     * In other words, calling this method is the only way to retrieve the root logger.
     *
     * @return The root Logger
     */
    public static Logger getRootLogger() {
        return DEFAULT;
    }

    /**
     * Log a message object with the {@link org.apache.log4j.Level#TRACE TRACE} level.
     *
     * @param message the message object to log.
     * @see #debug(Object) for an explanation of the logic applied.
     * @since 1.2.12
     */
    public void trace(Object message) {
        if (bundleLogger.doInfo()) {
            bundleLogger.doLog(buildMessage(message), "info   ", org.osgi.service.log.LogService.LOG_INFO, null);
        }
    }

    /**
     * Log a message object with the <code>TRACE</code> level including the stack trace of the {@link Throwable}<code>t</code>
     * passed as parameter.
     *
     * <p>
     * See {@link #debug(Object)} form for more detailed information.
     * </p>
     *
     * @param message the message object to log.
     * @param t the exception to log, including its stack trace.
     * @since 1.2.12
     */
    public void trace(Object message, Throwable t) {
        if (bundleLogger.doInfo()) {
            bundleLogger.doLog(buildMessage(message), "info   ", org.osgi.service.log.LogService.LOG_INFO, t);
        }
    }

    /**
     * Check whether this category is enabled for the TRACE Level.
     *
     * @since 1.2.12
     *
     * @return boolean - <code>true</code> if this category is enabled for level TRACE, <code>false</code> otherwise.
     */
    public boolean isTraceEnabled() {
        return bundleLogger.doInfo();
    }
}
