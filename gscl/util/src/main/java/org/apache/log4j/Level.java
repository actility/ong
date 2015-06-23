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
 * id $Id: Level.java 3303 2012-10-10 14:21:02Z JReich $
 * author $Author: JReich $
 * version $Revision: 3303 $
 * lastrevision $Date: 2012-10-10 16:21:02 +0200 (Wed, 10 Oct 2012) $
 * modifiedby $LastChangedBy: JReich $
 * lastmodified $LastChangedDate: 2012-10-10 16:21:02 +0200 (Wed, 10 Oct 2012) $
 */

package org.apache.log4j;

import java.io.Serializable;

/**
 * Defines the minimum set of levels recognized by the system, that is <code>OFF</code>, <code>FATAL</code>, <code>ERROR</code>,
 * <code>WARN</code>, <code>INFO</code>, <code>DEBUG</code> and <code>ALL</code>.
 */
public final class Level extends Priority implements Serializable {

    /**
     * Serialization id.
     */
    private static final long serialVersionUID = 3831391842909520692L;

    /**
     * TRACE level integer value.
     *
     * @since 1.2.12
     */
    public static final int TRACE_INT = 5000;

    /**
     * The <code>OFF</code> has the highest possible rank and is intended to turn off logging.
     */
    public static final Level OFF = new Level(OFF_INT, "OFF", 0);

    /**
     * The <code>FATAL</code> level designates very severe error events that will presumably lead the application to abort.
     */
    public static final Level FATAL = new Level(FATAL_INT, "FATAL", 0);

    /**
     * The <code>ERROR</code> level designates error events that might still allow the application to continue running.
     */
    public static final Level ERROR = new Level(ERROR_INT, "ERROR", 3);

    /**
     * The <code>WARN</code> level designates potentially harmful situations.
     */
    public static final Level WARN = new Level(WARN_INT, "WARN", 4);

    /**
     * The <code>INFO</code> level designates informational messages that highlight the progress of the application at
     * coarse-grained level.
     */
    public static final Level INFO = new Level(INFO_INT, "INFO", 6);

    /**
     * The <code>DEBUG</code> Level designates fine-grained informational events that are most useful to debug an application.
     */
    public static final Level DEBUG = new Level(DEBUG_INT, "DEBUG", 7);

    /**
     * The <code>TRACE</code> Level designates finer-grained informational events than the <code>DEBUG</code> level.
     *
     * @since 1.2.12
     */
    public static final Level TRACE = new Level(TRACE_INT, "TRACE", 7);

    /**
     * The <code>ALL</code> has the lowest possible rank and is intended to turn on all logging.
     */
    public static final Level ALL = new Level(ALL_INT, "ALL", 7);

    /**
     * Instantiate a Level object.
     */
    protected Level(int level, String levelStr, int syslogEquivalent) {
        super(level, levelStr, syslogEquivalent);
    }

    /**
     * Convert the string passed as argument to a level. If the conversion fails, then this method returns {@link #DEBUG}.
     *
     * @param level The level to parse
     * @return The parsed level or {@link Level#DEBUG} if the string cannotbe parsed
     */
    public static Level toLevel(String level) {
        return toLevel(level, Level.DEBUG);
    }

    /**
     * Convert an integer passed as argument to a level. If the conversion fails, then this method returns {@link #DEBUG}.
     *
     * @param level The level as an integer value
     * @return The determined level or {@link Level#DEBUG}
     */
    public static Level toLevel(int level) {
        return toLevel(level, Level.DEBUG);
    }

    /**
     * Convert an integer passed as argument to a level. If the conversion fails, then this method returns the specified
     * default.
     *
     * @param level The level as an integer value
     * @param defaultLevel The defaut level to returnin case the given level cannot be determined
     * @return The determined level or defautl level
     */
    public static Level toLevel(int level, Level defaultLevel) {
        switch (level) {
        case ALL_INT:
            return ALL;
        case DEBUG_INT:
            return Level.DEBUG;
        case INFO_INT:
            return Level.INFO;
        case WARN_INT:
            return Level.WARN;
        case ERROR_INT:
            return Level.ERROR;
        case FATAL_INT:
            return Level.FATAL;
        case OFF_INT:
            return OFF;
        case TRACE_INT:
            return Level.TRACE;
        default:
            return defaultLevel;
        }
    }

    /**
     * Convert the string passed as argument to a level. If the conversion fails, then this method returns the value of
     * <code>defaultLevel</code>.
     *
     * @param level The level to parse
     * @param defaultLevel Default level to return in case the given string cannot be parsed
     * @return The parsed level or default level
     */
    public static Level toLevel(String level, Level defaultLevel) {
        if (level == null) {
            return defaultLevel;
        }

        String s = level.toUpperCase();

        if (s.equals("ALL")) {
            return Level.ALL;
        }
        if (s.equals("DEBUG")) {
            return Level.DEBUG;
        }
        if (s.equals("INFO")) {
            return Level.INFO;
        }
        if (s.equals("WARN")) {
            return Level.WARN;
        }
        if (s.equals("ERROR")) {
            return Level.ERROR;
        }
        if (s.equals("FATAL")) {
            return Level.FATAL;
        }
        if (s.equals("OFF")) {
            return Level.OFF;
        }
        if (s.equals("TRACE")) {
            return Level.TRACE;
        }
        if (s.equals("\u0130NFO")) {
            return Level.INFO;
        }
        return defaultLevel;
    }

}
