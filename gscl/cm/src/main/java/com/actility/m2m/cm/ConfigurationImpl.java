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

package com.actility.m2m.cm;

import java.io.IOException;
import java.util.Dictionary;
import java.util.Enumeration;

import org.apache.log4j.Logger;
import org.osgi.framework.Constants;
import org.osgi.service.cm.Configuration;
import org.osgi.service.cm.ConfigurationAdmin;

import com.actility.m2m.cm.log.BundleLogger;
import com.actility.m2m.util.log.OSGiLogger;

public final class ConfigurationImpl implements Configuration {
    private static final Logger LOG = OSGiLogger.getLogger(ConfigurationImpl.class, BundleLogger.getStaticLogger());

    private final ConfigurationAdminManager manager;
    private final String factoryPid; // Factory PID
    private final String servicePid; // PID

    private ConfigurationDictionary properties;
    private boolean deleted;
    private boolean created;

    public ConfigurationImpl(ConfigurationAdminManager manager, String bundleLocation, String factoryPid, String servicePid) {
        this.manager = manager;
        this.factoryPid = factoryPid;
        this.servicePid = servicePid;
        this.deleted = false;
        this.created = false;

        this.properties = new ConfigurationDictionary();
        this.properties.put(ConfigurationAdmin.SERVICE_FACTORYPID, factoryPid);
        this.properties.put(Constants.SERVICE_PID, servicePid);
        if (bundleLocation != null) {
            this.properties.setLocation(bundleLocation);
        }
    }

    public ConfigurationImpl(ConfigurationAdminManager manager, ConfigurationDictionary properties) {
        this.manager = manager;
        this.factoryPid = (String) properties.get(ConfigurationAdmin.SERVICE_FACTORYPID);
        this.servicePid = (String) properties.get(Constants.SERVICE_PID);
        this.properties = properties;
        this.deleted = false;
        this.created = true;
    }

    public void delete() throws IOException {
        throwIfDeleted();
        deleted = true;
        manager.delete(servicePid);
    }

    public String getBundleLocation() {
        throwIfDeleted();
        // checkConfigPerm();
        return created ? (String) properties.getLocation() : null;
    }

    public String getFactoryPid() {
        throwIfDeleted();
        return factoryPid;
    }

    public String getPid() {
        throwIfDeleted();
        return servicePid;
    }

    public Dictionary getProperties() {
        throwIfDeleted();
        return (created) ? properties : null;
    }

    public void setBundleLocation(String bundleLocation) {
        throwIfDeleted();
        // checkConfigPerm();

        manager.setBundleLocation(servicePid, factoryPid, bundleLocation, properties);
    }

    public void update() throws IOException {
        throwIfDeleted();

        if (created) {
            manager.update(factoryPid, servicePid, properties);
        }
    }

    public void update(Dictionary properties) throws IOException {
        throwIfDeleted();
        created = true;
        ConfigurationDictionary old = this.properties;
        if (properties == null) {
            this.properties = new ConfigurationDictionary();
            if (old != null) {
                this.properties.setLocation(old.getLocation());
                this.properties.setDynamic(old.isDynamic());
            }
        } else if (properties != old) {
            this.properties = new ConfigurationDictionary();
            Enumeration e = properties.keys();
            while (e.hasMoreElements()) {
                Object key = e.nextElement();
                this.properties.put(key, properties.get(key));
            }
            if (old != null) {
                this.properties.setLocation(old.getLocation());
                this.properties.setDynamic(old.isDynamic());
            }
        }

        this.properties.put(Constants.SERVICE_PID, servicePid);
        if (factoryPid != null) {
            this.properties.put(ConfigurationAdmin.SERVICE_FACTORYPID, factoryPid);
        }

        try {
            update();
        } catch (IOException e) {
            this.properties = old;
            throw e;
        } catch (Exception e) {
            LOG.error("Error while updating properties.", e);
            this.properties = old;
        }
    }

    public boolean equals(Object obj) {
        if (!(obj instanceof Configuration)) {
            return false;
        }
        return servicePid.equals(((Configuration) obj).getPid());
    }

    public int hashCode() {
        return servicePid.hashCode();
    }

    private void throwIfDeleted() {
        if (deleted) {
            throw new IllegalStateException("Configuration for " + servicePid + " has been deleted.");
        }
    }

    // private void checkConfigPerm() {
    // SecurityManager sm = System.getSecurityManager();
    // if (null != sm) {
    // sm.checkPermission(CMConstants.CONFIGURATION_PERMISSION);
    // }
    // }
}
