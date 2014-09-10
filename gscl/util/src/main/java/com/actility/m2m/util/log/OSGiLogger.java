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
 * id $Id: OSGiLogger.java 7601 2014-02-06 09:58:16Z JReich $
 * author $Author: JReich $
 * version $Revision: 7601 $
 * lastrevision $Date: 2014-02-06 10:58:16 +0100 (Thu, 06 Feb 2014) $
 * modifiedby $LastChangedBy: JReich $
 * lastmodified $LastChangedDate: 2014-02-06 10:58:16 +0100 (Thu, 06 Feb 2014) $
 */

package com.actility.m2m.util.log;

import org.apache.log4j.Logger;


/**
 * Utils class to initialize the OSGi log context and to allow to use it with the Log4J API.
 */
public final class OSGiLogger {
    /**
     * Get an Actility logger
     * <p>
     * This is the same idea as Log4J with Logger.getLogger
     *
     * @param clazz The class that will log messages
     * @param bundleLogger The associated bundle logger (this is to add bundle infos and to filter logs)
     * @return The created Logger
     */
    public static Logger getLogger(Class clazz, BundleStaticLogger bundleLogger) {
        return new Logger(bundleLogger, clazz.getName());
    }
}
