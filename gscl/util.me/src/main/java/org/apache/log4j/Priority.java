/*******************************************************************************
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
 *******************************************************************************/

package org.apache.log4j;

/**
 * LOG4J Priority class.
 */
public class Priority {

    protected int level;
    protected String levelStr;
    protected int syslogEquivalent;

    /**
     * OFF Priority
     */
    public static final int OFF_INT = Integer.MAX_VALUE;
    /**
     * FATAL Priority
     */
    public static final int FATAL_INT = 50000;
    /**
     * ERROR Priority
     */
    public static final int ERROR_INT = 40000;
    /**
     * WARN Priority
     */
    public static final int WARN_INT = 30000;
    /**
     * INFO Priority
     */
    public static final int INFO_INT = 20000;
    /**
     * DEBUG Priority
     */
    public static final int DEBUG_INT = 10000;
    /**
     * ALL Priority
     */
    public static final int ALL_INT = Integer.MIN_VALUE;

    /**
     * @deprecated Use {@link Level#FATAL} instead.
     */
    public static final Priority FATAL = new Level(FATAL_INT, "FATAL", 0);

    /**
     * @deprecated Use {@link Level#ERROR} instead.
     */
    public static final Priority ERROR = new Level(ERROR_INT, "ERROR", 3);

    /**
     * @deprecated Use {@link Level#WARN} instead.
     */
    public static final Priority WARN = new Level(WARN_INT, "WARN", 4);

    /**
     * @deprecated Use {@link Level#INFO} instead.
     */
    public static final Priority INFO = new Level(INFO_INT, "INFO", 6);

    /**
     * @deprecated Use {@link Level#DEBUG} instead.
     */
    public static final Priority DEBUG = new Level(DEBUG_INT, "DEBUG", 7);

    /**
     * Default constructor for deserialization.
     */
    protected Priority() {
        level = DEBUG_INT;
        levelStr = "DEBUG";
        syslogEquivalent = 7;
    }

    /**
     * Instantiate a level object.
     */
    protected Priority(int level, String levelStr, int syslogEquivalent) {
        this.level = level;
        this.levelStr = levelStr;
        this.syslogEquivalent = syslogEquivalent;
    }

    public int hashCode() {
        return levelStr.hashCode();
    }

    /**
     * Two priorities are equal if their level fields are equal.
     *
     * @since 1.2
     */
    public boolean equals(Object o) {
        if (this == o) {
            return true;
        }
        if (o == null || !(o instanceof Priority)) {
            return false;
        }
        Priority priority = (Priority) o;
        return this.level == priority.level;
    }

    /**
     * Return the syslog equivalent of this priority as an integer.
     *
     * @return The syslog equivalent of this priority as an integer
     */
    public final int getSyslogEquivalent() {
        return syslogEquivalent;
    }

    /**
     * Returns <code>true</code> if this level has a higher or equal level than the level passed as argument, <code>false</code>
     * otherwise.
     * <p>
     * You should think twice before overriding the default implementation of <code>isGreaterOrEqual</code> method.
     *
     * @param r The priority to compare
     * @return <code>true</code> if this level has a higher or equal level than the level passed as argument, <code>false</code>
     *         otherwise
     */
    public boolean isGreaterOrEqual(Priority r) {
        return level >= r.level;
    }

    /**
     * Return all possible priorities as an array of Level objects in descending order.
     *
     * @deprecated This method will be removed with no replacement.
     */
    public static Priority[] getAllPossiblePriorities() {
        return new Priority[] { Priority.FATAL, Priority.ERROR, Level.WARN, Priority.INFO, Priority.DEBUG };
    }

    /**
     * Returns the string representation of this priority.
     */
    public final String toString() {
        return levelStr;
    }

    /**
     * Returns the integer representation of this level.
     *
     * @return The integer representation of this leve
     */
    public final int toInt() {
        return level;
    }

    /**
     * @deprecated Please use the {@link Level#toLevel(String)} method instead.
     */
    public static Priority toPriority(String sArg) {
        return Level.toLevel(sArg);
    }

    /**
     * @deprecated Please use the {@link Level#toLevel(int)} method instead.
     */
    public static Priority toPriority(int val) {
        return toPriority(val, Priority.DEBUG);
    }

    /**
     * @deprecated Please use the {@link Level#toLevel(int, Level)} method instead.
     */
    public static Priority toPriority(int val, Priority defaultPriority) {
        return Level.toLevel(val, (Level) defaultPriority);
    }

    /**
     * @deprecated Please use the {@link Level#toLevel(String, Level)} method instead.
     */
    public static Priority toPriority(String sArg, Priority defaultPriority) {
        return Level.toLevel(sArg, (Level) defaultPriority);
    }
}
