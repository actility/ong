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
 * id $Id: Category.java 7959 2014-03-07 14:20:01Z JReich $
 * author $Author: JReich $
 * version $Revision: 7959 $
 * lastrevision $Date: 2014-03-07 15:20:01 +0100 (Fri, 07 Mar 2014) $
 * modifiedby $LastChangedBy: JReich $
 * lastmodified $LastChangedDate: 2014-03-07 15:20:01 +0100 (Fri, 07 Mar 2014) $
 */

package org.apache.log4j;

import com.actility.m2m.util.log.BundleStaticLogger;

/**
 * Fake LOG4J Category class which redirects logs to OSGi Log service
 */
public class Category {
    protected BundleStaticLogger bundleLogger;
    protected String name;

    protected Category(BundleStaticLogger bundleLogger, String name) {
        if (bundleLogger == null) {
            throw new NullPointerException("BundleLogger is null for " + name);
        }
        this.bundleLogger = bundleLogger;
        this.name = name;
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
     * @return The built Category object.
     */
    public static Category getInstance(String name) {
        throw new UnsupportedOperationException();
    }

    /**
     * Shorthand for <code>getLogger(clazz.getName())</code>.
     *
     * @param clazz The name of <code>clazz</code> will be used as the name of the logger to retrieve. See
     *            {@link Logger#getLogger(String)} for more detailed information.
     * @return The buildCategory object.
     */
    public static Category getInstance(Class clazz) {
        throw new UnsupportedOperationException();
    }

    protected String buildMessage(Object message) {
        return "[" + Thread.currentThread().getName() + "] " + name + ": " + ((message != null) ? message.toString() : "null");
    }

    /**
     * Log a message object with the {@link Level#DEBUG DEBUG} level.
     *
     * <p>
     * This method first checks if this category is <code>DEBUG</code> enabled by comparing the level of this category with the
     * {@link Level#DEBUG DEBUG} level.
     * <p>
     * <b>WARNING</b> Note that passing a {@link Throwable} to this method will print the name of the <code>Throwable</code> but
     * no stack trace. To print a stack trace use the {@link #debug(Object, Throwable)} form instead.
     *
     * @param message the message object to log.
     */
    public void debug(Object message) {
        if (bundleLogger.doDebug()) {
            bundleLogger.doLog(buildMessage(message), "debug  ", org.osgi.service.log.LogService.LOG_DEBUG, null);
        }
    }

    /**
     * Log a message object with the <code>DEBUG</code> level including the stack trace of the {@link Throwable} <code>t</code>
     * passed as parameter.
     *
     * <p>
     * See {@link #debug(Object)} form for more detailed information.
     *
     * @param message the message object to log.
     * @param t the exception to log, including its stack trace.
     */
    public void debug(Object message, Throwable t) {
        if (bundleLogger.doDebug()) {
            bundleLogger.doLog(buildMessage(message), "debug  ", org.osgi.service.log.LogService.LOG_DEBUG, t);
        }
    }

    /**
     * Check whether this category is enabled for the <code>DEBUG</code> Level.
     *
     * <p>
     * This function is intended to lessen the computational cost of disabled log debug statements.
     *
     * <p>
     * For some <code>cat</code> Category object, when you write,
     *
     * <pre>
     * cat.debug(&quot;This is entry number: &quot; + i);
     * </pre>
     *
     * <p>
     * You incur the cost constructing the message, concatenatiion in this case, regardless of whether the message is logged or
     * not.
     *
     * <p>
     * If you are worried about speed, then you should write
     *
     * <pre>
     * if (cat.isDebugEnabled()) {
     *     cat.debug(&quot;This is entry number: &quot; + i);
     * }
     * </pre>
     *
     * <p>
     * This way you will not incur the cost of parameter construction if debugging is disabled for <code>cat</code>. On the
     * other hand, if the <code>cat</code> is debug enabled, you will incur the cost of evaluating whether the category is debug
     * enabled twice. Once in <code>isDebugEnabled</code> and once in the <code>debug</code>. This is an insignificant overhead
     * since evaluating a category takes about 1%% of the time it takes to actually log.
     *
     * @return boolean - <code>true</code> if this category is debug enabled, <code>false</code> otherwise.
     * */
    public boolean isDebugEnabled() {
        return bundleLogger.doDebug();
    }

    /**
     * Log a message object with the {@link Level#ERROR ERROR} Level.
     *
     * <p>
     * This method first checks if this category is <code>ERROR</code> enabled by comparing the level of this category with
     * {@link Level#ERROR ERROR} Level.
     * <p>
     * <b>WARNING</b> Note that passing a {@link Throwable} to this method will print the name of the <code>Throwable</code> but
     * no stack trace. To print a stack trace use the {@link #error(Object, Throwable)} form instead.
     *
     * @param message the message object to log
     */
    public void error(Object message) {
        bundleLogger.doLog(buildMessage(message), "ERROR  ", org.osgi.service.log.LogService.LOG_ERROR, null);
    }

    /**
     * Log a message object with the <code>ERROR</code> level including the stack trace of the {@link Throwable} <code>t</code>
     * passed as parameter.
     *
     * <p>
     * See {@link #error(Object)} form for more detailed information.
     *
     * @param message the message object to log.
     * @param t the exception to log, including its stack trace.
     */
    public void error(Object message, Throwable t) {
        bundleLogger.doLog(buildMessage(message), "ERROR  ", org.osgi.service.log.LogService.LOG_ERROR, t);
    }

    /**
     * Log a message object with the {@link Level#FATAL FATAL} Level.
     *
     * <p>
     * This method first checks if this category is <code>FATAL</code> enabled by comparing the level of this category with
     * {@link Level#FATAL FATAL} Level.
     *
     * <p>
     * <b>WARNING</b> Note that passing a {@link Throwable} to this method will print the name of the Throwable but no stack
     * trace. To print a stack trace use the {@link #fatal(Object, Throwable)} form instead.
     *
     * @param message the message object to log
     */
    public void fatal(Object message) {
        bundleLogger.doLog(buildMessage(message), "ERROR  ", org.osgi.service.log.LogService.LOG_ERROR, null);
    }

    /**
     * Log a message object with the <code>FATAL</code> level including the stack trace of the {@link Throwable} <code>t</code>
     * passed as parameter.
     *
     * <p>
     * See {@link #fatal(Object)} for more detailed information.
     *
     * @param message the message object to log.
     * @param t the exception to log, including its stack trace.
     */
    public void fatal(Object message, Throwable t) {
        bundleLogger.doLog(buildMessage(message), "ERROR  ", org.osgi.service.log.LogService.LOG_ERROR, null);
    }

    /**
     * Log a message object with the {@link Level#WARN WARN} Level.
     *
     * <p>
     * This method first checks if this category is <code>WARN</code> enabled by comparing the level of this category with
     * {@link Level#WARN WARN} Level.
     *
     * <p>
     * <b>WARNING</b> Note that passing a {@link Throwable} to this method will print the name of the Throwable but no stack
     * trace. To print a stack trace use the {@link #warn(Object, Throwable)} form instead.
     * <p>
     *
     * @param message the message object to log.
     */
    public void warn(Object message) {
        if (bundleLogger.doWarn()) {
            bundleLogger.doLog(buildMessage(message), "warning", org.osgi.service.log.LogService.LOG_WARNING, null);
        }
    }

    /**
     * Log a message with the <code>WARN</code> level including the stack trace of the {@link Throwable} <code>t</code> passed
     * as parameter.
     *
     * <p>
     * See {@link #warn(Object)} for more detailed information.
     *
     * @param message the message object to log.
     * @param t the exception to log, including its stack trace.
     */
    public void warn(Object message, Throwable t) {
        if (bundleLogger.doWarn()) {
            bundleLogger.doLog(buildMessage(message), "warning", org.osgi.service.log.LogService.LOG_WARNING, t);
        }
    }

    /**
     * Log a message object with the {@link Level#INFO INFO} Level.
     *
     * <p>
     * This method first checks if this category is <code>INFO</code> enabled by comparing the level of this category with
     * {@link Level#INFO INFO} Level.
     *
     * <p>
     * <b>WARNING</b> Note that passing a {@link Throwable} to this method will print the name of the Throwable but no stack
     * trace. To print a stack trace use the {@link #info(Object, Throwable)} form instead.
     *
     * @param message the message object to log
     */
    public void info(Object message) {
        if (bundleLogger.doInfo()) {
            bundleLogger.doLog(buildMessage(message), "info   ", org.osgi.service.log.LogService.LOG_INFO, null);
        }
    }

    /**
     * Log a message object with the <code>INFO</code> level including the stack trace of the {@link Throwable} <code>t</code>
     * passed as parameter.
     *
     * <p>
     * See {@link #info(Object)} for more detailed information.
     *
     * @param message the message object to log.
     * @param t the exception to log, including its stack trace.
     */
    public void info(Object message, Throwable t) {
        if (bundleLogger.doInfo()) {
            bundleLogger.doLog(buildMessage(message), "info   ", org.osgi.service.log.LogService.LOG_INFO, t);
        }
    }

    /**
     * Check whether this category is enabled for the info Level. See also {@link #isDebugEnabled}.
     *
     * @return boolean - <code>true</code> if this category is enabled for level info, <code>false</code> otherwise.
     */
    public boolean isInfoEnabled() {
        return bundleLogger.doInfo();
    }

    /**
     * If <code>assertion</code> parameter is <code>false</code>, then logs <code>msg</code> as an {@link #error(Object) error}
     * statement.
     *
     * <p>
     * The <code>assert</code> method has been renamed to <code>assertLog</code> because <code>assert</code> is a language
     * reserved word in JDK 1.4.
     *
     * @param assertion
     * @param msg The message to print if <code>assertion</code> is false.
     * @since 1.2
     */
    public void assertLog(boolean assertion, String msg) {
        if (!assertion) {
            this.error(msg);
        }
    }

    /**
     * This generic form is intended to be used by wrappers.
     *
     * @param priority The log priority
     * @param message The log message
     * @param t The throwable associated to log
     */
    public void log(Priority priority, Object message, Throwable t) {
        switch (priority.level) {
        case Priority.DEBUG_INT:
            this.debug(message, t);
            break;
        case Priority.ERROR_INT:
            this.error(message, t);
            break;
        case Priority.FATAL_INT:
            this.fatal(message, t);
            break;
        case Priority.INFO_INT:
            this.info(message, t);
            break;
        case Priority.WARN_INT:
            this.warn(message, t);
            break;
        default:
            this.debug(message, t);
        }
    }

    /**
     * This generic form is intended to be used by wrappers.
     *
     * @param priority The log priority
     * @param message The log message
     */
    public void log(Priority priority, Object message) {
        switch (priority.level) {
        case Priority.DEBUG_INT:
            this.debug(message);
            break;
        case Priority.ERROR_INT:
            this.error(message);
            break;
        case Priority.FATAL_INT:
            this.fatal(message);
            break;
        case Priority.INFO_INT:
            this.info(message);
            break;
        case Priority.WARN_INT:
            this.warn(message);
            break;
        default:
            this.debug(message);
        }
    }

    /**
     * Return the category name.
     *
     * @return The category name
     */
    public final String getName() {
        return name;
    }

    /**
     * Return the level in effect for this category/logger.
     *
     * <p>
     * The result is computed by simulation.
     *
     * @return The effective level
     */
    public Level getEffectiveLevel() {
        if (bundleLogger.doDebug()) {
            return Level.DEBUG;
        }
        if (bundleLogger.doInfo()) {
            return Level.INFO;
        }
        if (bundleLogger.doWarn()) {
            return Level.WARN;
        }
        return Level.ERROR;
    }

    /**
     * Returns the assigned {@link Level}, if any, for this Category. This implementation always returns null.
     *
     * @return Level - the assigned Level, can be <code>null</code>.
     */
    public final Level getLevel() {
        return null;
    }

    /**
     * @deprecated Please use {@link #getLevel} instead.
     */
    public final Level getPriority() {
        return null;
    }

    /**
     * Determines whether the priority passed as parameter is enabled in the underlying SLF4J logger. Each log4j priority is
     * mapped directly to its SLF4J equivalent, except for FATAL which is mapped as ERROR.
     *
     * @param p the priority to check against
     * @return true if this logger is enabled for the given level, false otherwise.
     */
    public boolean isEnabledFor(Priority p) {
        switch (p.level) {
        case Level.TRACE_INT:
            return bundleLogger.doInfo();
        case Priority.DEBUG_INT:
            return bundleLogger.doDebug();
        case Priority.INFO_INT:
            return bundleLogger.doInfo();
        case Priority.WARN_INT:
            return bundleLogger.doWarn();
        case Priority.ERROR_INT:
            return true;
        case Priority.FATAL_INT:
            return true;
        default:
            return false;
        }
    }

    /**
     * Util method to create a log.
     *
     * @param loggerName The Logger name
     * @param p The log priority
     * @param msg The log message
     * @param t The Throwable associated to the log
     */
    public void log(String loggerName, Priority p, Object msg, Throwable t) {
        log(p, msg, t);
    }
}
