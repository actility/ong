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
 * id $Id: Logger.java 8031 2014-03-07 17:31:11Z JReich $
 * author $Author: JReich $
 * version $Revision: 8031 $
 * lastrevision $Date: 2014-03-07 18:31:11 +0100 (Fri, 07 Mar 2014) $
 * modifiedby $LastChangedBy: JReich $
 * lastmodified $LastChangedDate: 2014-03-07 18:31:11 +0100 (Fri, 07 Mar 2014) $
 */

package com.actility.m2m.storage.driver;

public interface Logger {

    /**
     * Replies as soon as possible. The actual of the log is done asynchronously
     *
     * @param appId identifier of the application extracted of the path passed to the API request which raised the error
     * @param containerId identifier of the container extracted of the path passed to the API request which raised the error
     * @param logMirroring log mirroring value (application / container / none)
     * @param level error
     * @param code error code (can be used for applicative error handling)
     * @param shortDescription short error description
     * @param detail detailed error description
     * @param href path of the M2M resource
     */
    public void Log(String appId, Long containerId, String logMirroring, int level, String code, String shortDescription,
            String detail, String href);
}
