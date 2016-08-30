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

package com.actility.m2m.util;

import java.util.Locale;
import java.util.TimeZone;

/**
 * Some useful constants
 */
public interface UtilConstants {
    /**
     * The GMT timezone
     */
    TimeZone GMT_TIMEZONE = TimeZone.getTimeZone("GMT");

    /**
     * The local timezone
     */
    TimeZone LOCAL_TIMEZONE = TimeZone.getDefault();

    /**
     * The locale locale
     */
    Locale LOCAL_LOCALE = Locale.getDefault();

    /**
     * Actility ROOTACT OSGi framework property name
     */
    String FW_PROP_ROOTACT = "com.actility.m2m.framework.config.rootact";

    /**
     * Default OS shell OSGi framework property name
     */
    String FW_PROP_OS_SHELL = "com.actility.m2m.framework.config.os.shell";

    /**
     * Instance ID of OSGi framework property name
     */
    String FW_PROP_INSTANCE_ID = "com.actility.m2m.framework.config.instanceId";

    /**
     * Service Provider domain name of OSGi framework property name
     */
    String FW_PROP_SP_DOMAIN_NAME = "com.actility.m2m.framework.config.spDomainName";
}