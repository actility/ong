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

package com.actility.m2m.cm;

import java.io.IOException;

import org.osgi.framework.InvalidSyntaxException;
import org.osgi.service.cm.Configuration;
import org.osgi.service.cm.ConfigurationAdmin;

/**
 * Implementation of the OSGi configuration admin service
 */
public final class ConfigurationAdminImpl implements ConfigurationAdmin {
    private final ConfigurationAdminManager manager;
    private final String callingBundleLocation;

    public ConfigurationAdminImpl(ConfigurationAdminManager manager, String callingBundleLocation) {
        this.manager = manager;
        this.callingBundleLocation = callingBundleLocation;
    }

    public Configuration createFactoryConfiguration(String factoryPid) throws IOException {
//        boolean checkLocation = false;
//        try {
//            checkConfigPerm();
//        } catch (SecurityException e) {
//            checkLocation = true;
//        }
        boolean checkLocation = true;
        return manager.createFactoryConfiguration(factoryPid, callingBundleLocation, checkLocation);
    }

    public Configuration createFactoryConfiguration(String factoryPid, String location) throws IOException {
//        checkConfigPerm();
        return manager.createFactoryConfiguration(factoryPid, location, false);
    }

    public Configuration getConfiguration(String pid) throws IOException {
//        boolean checkLocation = false;
//        try {
//            checkConfigPerm();
//        } catch (SecurityException e) {
//            checkLocation = true;
//        }
        boolean checkLocation = true;
        return manager.getConfiguration(pid, callingBundleLocation, checkLocation);
    }

    public Configuration getConfiguration(String pid, String location) throws IOException {
//        checkConfigPerm();
        return manager.getConfiguration(pid, location, false);
    }

    public Configuration[] listConfigurations(String filterString) throws IOException, InvalidSyntaxException {
//        boolean checkLocation = false;
//        try {
//            checkConfigPerm();
//        } catch (SecurityException e) {
//            checkLocation = true;
//        }
        boolean checkLocation = true;
        return manager.listConfigurations(filterString, callingBundleLocation, checkLocation);

    }

//    private void checkConfigPerm() {
//        SecurityManager sm = System.getSecurityManager();
//        if (null != sm) {
//            sm.checkPermission(CMConstants.CONFIGURATION_PERMISSION);
//        }
//    }
}
