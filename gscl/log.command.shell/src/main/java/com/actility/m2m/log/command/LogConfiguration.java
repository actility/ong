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
 * id $Id: FileCommand.java 6119 2013-10-16 12:37:56Z mlouiset $
 * author $Author: mlouiset $
 * version $Revision: 6119 $
 * lastrevision $Date: 2013-10-16 14:37:56 +0200 (Wed, 16 Oct 2013) $
 * modifiedby $LastChangedBy: mlouiset $
 * lastmodified $LastChangedDate: 2013-10-16 14:37:56 +0200 (Wed, 16 Oct 2013) $
 */

package com.actility.m2m.log.command;

import java.io.IOException;
import java.util.ArrayList;
import java.util.Dictionary;
import java.util.HashMap;
import java.util.Hashtable;
import java.util.Iterator;
import java.util.List;
import java.util.Map;

import org.osgi.framework.BundleContext;
import org.osgi.service.cm.Configuration;
import org.osgi.service.cm.ConfigurationAdmin;

import com.actility.m2m.framework.resources.ResourcesAccessorService;

public final class LogConfiguration {
    private static final String CONFIG_PID = "com.actility.m2m.log.config";

    private static final String PROP_LOG_DIRECTORY = "com.actility.m2m.log.config.logDirectory";
    private static final String PROP_MAX_NB_OF_FILES = "com.actility.m2m.log.config.maxNbOfFiles";
    private static final String PROP_MAX_FILE_SIZE = "com.actility.m2m.log.config.maxFileSize";
    private static final String PROP_FORCE_FLUSH = "com.actility.m2m.log.config.forceFlush";
    private static final String PROP_LOG_LEVEL = "com.actility.m2m.log.config.logLevel";
    private static final String PROP_MAX_NB_OF_LOGS_IN_MEMORY = "com.actility.m2m.log.config.maxNbOfLogsInMemory";
    private static final String PROP_LOG_TO_STDOUT = "com.actility.m2m.log.config.logToStdout";

    // Keys for properties in the configuration
    private final static String ATTR_MAX_NB_OF_FILES = "maxNbOfFiles";
    private final static String ATTR_MAX_FILE_SIZE = "maxFileSize";
    private final static String ATTR_FORCE_FLUSH = "forceFlush";
    private final static String ATTR_LOG_LEVEL = "logLevel";
    private final static String ATTR_LOG_LEVEL_PER_BUNDLE = "logLevelPerBundle";
    private final static String ATTR_MAX_NB_OF_LOGS_IN_MEMORY = "maxNbOfLogsInMemory";
    private final static String ATTR_LOG_TO_STDOUT = "logToStdout";

    private final ResourcesAccessorService resourcesAccessorService;
    private final BundleContext context;
    private final Configuration config;
    private final Dictionary properties;
    private boolean modified;

    public LogConfiguration(ResourcesAccessorService resourcesAccessorService, BundleContext context,
            ConfigurationAdmin configAdmin) throws IOException {
        this.resourcesAccessorService = resourcesAccessorService;
        this.context = context;
        this.config = configAdmin.getConfiguration(CONFIG_PID);
        Dictionary properties = config.getProperties();
        if (properties == null) {
            properties = new Hashtable();
        }
        this.properties = properties;
        this.modified = false;
    }

    public String getLogDirectory() {
        return resourcesAccessorService.getProperty(context, PROP_LOG_DIRECTORY);
    }

    public int getMaxNbOfFiles() {
        Integer value = (Integer) properties.get(ATTR_MAX_NB_OF_FILES);
        if (value == null) {
            String valueStr = resourcesAccessorService.getProperty(context, PROP_MAX_NB_OF_FILES);
            if (valueStr != null) {
                value = Integer.valueOf(valueStr);
            }
        }
        return (value != null) ? value.intValue() : -1;
    }

    public void setMaxNbOfFiles(int maxNbOfFiles) {
        Integer newValue = new Integer(maxNbOfFiles);
        Integer oldValue = (Integer) properties.put(ATTR_MAX_NB_OF_FILES, newValue);
        if (oldValue == null || !oldValue.equals(newValue)) {
            modified = true;
        }
    }

    public int getMaxFileSize() {
        Integer value = (Integer) properties.get(ATTR_MAX_FILE_SIZE);
        if (value == null) {
            String valueStr = resourcesAccessorService.getProperty(context, PROP_MAX_FILE_SIZE);
            if (valueStr != null) {
                value = Integer.valueOf(valueStr);
            }
        }
        return (value != null) ? value.intValue() : -1;
    }

    public void setMaxFileSize(int maxFileSize) {
        Integer newValue = new Integer(maxFileSize);
        Integer oldValue = (Integer) properties.put(ATTR_MAX_FILE_SIZE, newValue);
        if (oldValue == null || !oldValue.equals(newValue)) {
            modified = true;
        }
    }

    public boolean getForceFlush() {
        Boolean value = (Boolean) properties.get(ATTR_FORCE_FLUSH);
        if (value == null) {
            String valueStr = resourcesAccessorService.getProperty(context, PROP_FORCE_FLUSH);
            if (valueStr != null) {
                value = new Boolean("true".equalsIgnoreCase(valueStr));
            }
        }
        return (value != null) ? value.booleanValue() : false;
    }

    public void setForceFlush(boolean forceFlush) {
        Boolean newValue = new Boolean(forceFlush);
        Boolean oldValue = (Boolean) properties.put(ATTR_FORCE_FLUSH, newValue);
        if (oldValue == null || !oldValue.equals(newValue)) {
            modified = true;
        }
    }

    public String getLogLevel() {
        String value = (String) properties.get(ATTR_LOG_LEVEL);
        if (value == null) {
            value = resourcesAccessorService.getProperty(context, PROP_LOG_LEVEL);
        }
        return value;
    }

    public void setLogLevel(String logLevel) {
        if ("default".equals(logLevel)) {
            String oldValue = (String) properties.remove(ATTR_LOG_LEVEL);
            if (oldValue != null) {
                modified = true;
            }
        } else {
            String oldValue = (String) properties.put(ATTR_LOG_LEVEL, logLevel);
            if (oldValue == null || !oldValue.equals(logLevel)) {
                modified = true;
            }
        }
    }

    public Map getLogLevelPerBundle() {
        Map/* <String, String> */result = new HashMap/* <String, String> */();

        List/* <String> */value = (List/* <String> */) properties.get(ATTR_LOG_LEVEL_PER_BUNDLE);
        if (value != null) {
            Iterator it = value.iterator();
            while (it.hasNext()) {
                String token = (String) it.next();
                int semiColonIndex = token.indexOf(':');
                if (semiColonIndex != -1 && semiColonIndex != (token.length() - 1)) {
                    result.put(token.substring(0, semiColonIndex), token.substring(semiColonIndex + 1, token.length()));
                }
            }
        }
        return result;
    }

    public void setLogLevelForBundle(String symbolicName, String logLevel) {
        List/* <String> */oldValue = (List/* <String> */) properties.get(ATTR_LOG_LEVEL_PER_BUNDLE);
        if (oldValue != null) {
            Iterator it = oldValue.iterator();
            int index = 0;
            while (it.hasNext()) {
                String oldToken = (String) it.next();
                int semiColonIndex = oldToken.indexOf(':');
                if (semiColonIndex != -1 && semiColonIndex == symbolicName.length()
                        && oldToken.regionMatches(false, 0, symbolicName, 0, semiColonIndex)) {
                    if (semiColonIndex == (oldToken.length() - 1)
                            || !logLevel.regionMatches(true, 0, oldToken, semiColonIndex + 1, logLevel.length())) {
                        modified = true;
                        oldValue.set(index, symbolicName + ":" + logLevel);
                        properties.put(ATTR_LOG_LEVEL_PER_BUNDLE, oldValue);
                    }
                    break;
                }
                ++index;
            }
            if (index == oldValue.size()) {
                modified = true;
                oldValue.add(symbolicName + ":" + logLevel);
                properties.put(ATTR_LOG_LEVEL_PER_BUNDLE, oldValue);
            }
        } else {
            List newValue = new ArrayList();
            newValue.add(symbolicName + ":" + logLevel);
            modified = true;
            properties.put(ATTR_LOG_LEVEL_PER_BUNDLE, newValue);
        }
    }

    public void removeLogLevelForBundle(String symbolicName) {
        List/* <String> */oldValue = (List/* <String> */) properties.get(ATTR_LOG_LEVEL_PER_BUNDLE);
        if (oldValue != null) {
            Iterator it = oldValue.iterator();
            int index = 0;
            while (it.hasNext()) {
                String oldToken = (String) it.next();
                int semiColonIndex = oldToken.indexOf(':');
                if (semiColonIndex != -1 && semiColonIndex == symbolicName.length()
                        && oldToken.regionMatches(false, 0, symbolicName, 0, semiColonIndex)) {
                    modified = true;
                    oldValue.remove(index);
                    properties.put(ATTR_LOG_LEVEL_PER_BUNDLE, oldValue);
                    break;
                }
                ++index;
            }
        }
    }

    public int getMaxNbOfLogsInMemory() {
        Integer value = (Integer) properties.get(ATTR_MAX_NB_OF_LOGS_IN_MEMORY);
        if (value == null) {
            String valueStr = resourcesAccessorService.getProperty(context, PROP_MAX_NB_OF_LOGS_IN_MEMORY);
            if (valueStr != null) {
                value = Integer.valueOf(valueStr);
            }
        }
        return (value != null) ? value.intValue() : -1;
    }

    public void setMaxNbOfLogsInMemory(int maxNbOfLogsInMemory) {
        Integer newValue = new Integer(maxNbOfLogsInMemory);
        Integer oldValue = (Integer) properties.put(ATTR_MAX_NB_OF_LOGS_IN_MEMORY, newValue);
        if (oldValue == null || !oldValue.equals(newValue)) {
            modified = true;
        }
    }

    public boolean getLogToStdout() {
        Boolean value = (Boolean) properties.get(ATTR_LOG_TO_STDOUT);
        if (value == null) {
            String valueStr = resourcesAccessorService.getProperty(context, PROP_LOG_TO_STDOUT);
            if (valueStr != null) {
                value = new Boolean("true".equalsIgnoreCase(valueStr));
            }
        }
        return (value != null) ? value.booleanValue() : false;
    }

    public void setLogToStdout(boolean logToStdout) {
        Boolean newValue = new Boolean(logToStdout);
        Boolean oldValue = (Boolean) properties.put(ATTR_LOG_TO_STDOUT, newValue);
        if (oldValue == null || !oldValue.equals(newValue)) {
            modified = true;
        }
    }

    public void save() throws IOException {
        if (modified) {
            config.update(properties);
        }
    }
}
