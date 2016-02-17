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

package com.actility.m2m.log;

/**
 * This LogService provides an extra method for querying the service about the
 * current log level. Log entries that are less severe than the current log
 * level will be discarded by the log.
 * <p>
 * The log levels have the following hierarchy:
 * <ol>
 * <li><code>LOG_ERROR</code></li>
 * <li><code>LOG_WARNING</code></li>
 * <li><code>LOG_INFO</code></li>
 * <li><code>LOG_DEBUG</code></li>
 * </ol>
 *
 * @author Gatespace AB
 */
public interface LogService extends org.osgi.service.log.LogService {
  /**
   * Get the current log level. The log will discard log entries with a
   * level that is less severe than the current level. E.g., if the current
   * log level is {@link #LOG_WARNING} then the log will discard all log
   * entries with level {@link #LOG_INFO} and {@link #LOG_DEBUG}. I.e. there
   * is no need for a bundle to try to send such log entries to the log.
   * The bundle may actually save a number of CPU-cycles by getting the
   * log level and do nothing if the intended log entry is less severe than
   * the current log level.
   *
   * @return the lowest severity level that is accepted into the log.
   */
  int getLogLevel();
}
