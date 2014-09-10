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

package com.actility.m2m.log.impl;

import java.io.File;
import java.util.ArrayList;
import java.util.Dictionary;
import java.util.HashMap;
import java.util.Hashtable;
import java.util.Iterator;
import java.util.List;
import java.util.Map;

import org.osgi.framework.Bundle;
import org.osgi.framework.BundleContext;
import org.osgi.framework.BundleEvent;
import org.osgi.framework.BundleListener;
import org.osgi.service.cm.ConfigurationException;
import org.osgi.service.cm.ManagedService;
import org.osgi.service.log.LogService;

import com.actility.m2m.framework.resources.ResourcesAccessorService;
import com.actility.m2m.util.FileUtils;
import com.actility.m2m.util.UtilConstants;

public class LogConfiguration implements ManagedService, BundleListener {

    private static final String PROP_LOG_DIRECTORY = ".config.logDirectory";
    private static final String PROP_MAX_NB_OF_FILES = ".config.maxNbOfFiles";
    private static final String PROP_MAX_FILE_SIZE = ".config.maxFileSize";
    private static final String PROP_FORCE_FLUSH = ".config.forceFlush";
    private static final String PROP_LOG_LEVEL = ".config.logLevel";
    private static final String PROP_MAX_NB_OF_LOGS_IN_MEMORY = ".config.maxNbOfLogsInMemory";
    private static final String PROP_LOG_TO_STDOUT = ".config.logToStdout";

    // Keys for properties in the configuration
    private final static String ATTR_MAX_NB_OF_FILES = "maxNbOfFiles";
    private final static String ATTR_MAX_FILE_SIZE = "maxFileSize";
    private final static String ATTR_FORCE_FLUSH = "forceFlush";
    private final static String ATTR_LOG_LEVEL = "logLevel";
    private final static String ATTR_LOG_LEVEL_PER_BUNDLE = "logLevelPerBundle";
    private final static String ATTR_MAX_NB_OF_LOGS_IN_MEMORY = "maxNbOfLogsInMemory";
    private final static String ATTR_LOG_TO_STDOUT = "logToStdout";

    private final static int DEFAULT_MAX_NB_OF_FILES = 5;
    private final static int DEFAULT_MAX_FILE_SIZE = 1048576; // ~1Mo
    private final static boolean DEFAULT_FORCE_FLUSH = false;
    private final static int DEFAULT_LOG_LEVEL = LogService.LOG_ERROR;
    private final static int DEFAULT_MAX_NB_OF_LOGS_IN_MEMORY = 50;
    private final static boolean DEFAULT_LOG_TO_STDOUT = false;

    /** The directory that the file log is written to. */
    private final BundleContext context;
    private final ResourcesAccessorService resourcesAccessorService;

    private final File logDirectory;
    private int maxNbOfFile;
    private int maxFileSize;
    private boolean forceFlush;
    private int logLevel;
    private Map/* <String, Integer> */logLevelPerBundle;
    private int maxNbOfLogsInMemory;
    private boolean logToStdout;

    // Mapping from bundle id to log level. This is a cache computed
    // by evaluate all installed bundles against the current blFilters
    private Map bids;

    public LogConfiguration(BundleContext context, ResourcesAccessorService resourcesAccessorService, Dictionary config) {
        this.context = context;
        this.resourcesAccessorService = resourcesAccessorService;
        this.bids = new HashMap();

        initBids();

        File rootact = new File(resourcesAccessorService.getProperty(context, UtilConstants.FW_PROP_ROOTACT));
        this.logDirectory = FileUtils.normalizeFile(rootact,
                resourcesAccessorService.getProperty(context, context.getBundle().getSymbolicName() + PROP_LOG_DIRECTORY));
        setMaxNbOfFiles(config);
        setMaxFileSize(config);
        setForceFlush(config);
        setLogLevel(config);
        setLogLevelPerBundle(config);
        setMaxNbOfLogsInMemory(config);
        setLogToStdout(config);
    }

    public File getLogDirectory() {
        return logDirectory;
    }

    public int getMaxNbOfFiles() {
        return maxNbOfFile;
    }

    private void setMaxNbOfFiles(Dictionary properties) {
        Integer value = (Integer) properties.get(ATTR_MAX_NB_OF_FILES);
        if (value == null) {
            String valueStr = resourcesAccessorService.getProperty(context, context.getBundle().getSymbolicName()
                    + PROP_MAX_NB_OF_FILES);
            if (valueStr != null) {
                try {
                    value = Integer.valueOf(valueStr);
                } catch (NumberFormatException e) {
                    // Ignore
                }
            }
        }
        this.maxNbOfFile = (value != null) ? value.intValue() : DEFAULT_MAX_NB_OF_FILES;
    }

    public int getMaxFileSize() {
        return maxFileSize;
    }

    private void setMaxFileSize(Dictionary properties) {
        Integer value = (Integer) properties.get(ATTR_MAX_FILE_SIZE);
        if (value == null) {
            String valueStr = resourcesAccessorService.getProperty(context, context.getBundle().getSymbolicName()
                    + PROP_MAX_FILE_SIZE);
            if (valueStr != null) {
                try {
                    value = Integer.valueOf(valueStr);
                } catch (NumberFormatException e) {
                    // Ignore
                }
            }
        }
        this.maxFileSize = (value != null) ? value.intValue() : DEFAULT_MAX_FILE_SIZE;
    }

    public boolean getForceFlush() {
        return forceFlush;
    }

    private void setForceFlush(Dictionary properties) {
        Boolean value = (Boolean) properties.get(ATTR_FORCE_FLUSH);
        if (value == null) {
            String valueStr = resourcesAccessorService.getProperty(context, context.getBundle().getSymbolicName()
                    + PROP_FORCE_FLUSH);
            if (valueStr != null) {
                value = new Boolean(valueStr.equalsIgnoreCase("true"));
            }
        }
        this.forceFlush = (value != null) ? value.booleanValue() : DEFAULT_FORCE_FLUSH;
    }

    public int getLogLevel() {
        return logLevel;
    }

    private void setLogLevel(Dictionary properties) {
        String value = (String) properties.get(ATTR_LOG_LEVEL);
        if (value == null) {
            String valueStr = resourcesAccessorService.getProperty(context, context.getBundle().getSymbolicName()
                    + PROP_LOG_LEVEL);
            if (valueStr != null) {
                value = valueStr;
            }
        }
        this.logLevel = (value != null) ? LogUtils.toLevel(value, DEFAULT_LOG_LEVEL) : DEFAULT_LOG_LEVEL;
    }

    public int getLogLevelForBundle(Bundle bundle) {
        Integer level = (Integer) logLevelPerBundle.get(bids.get(new Long(bundle.getBundleId())));
        return (level != null) ? level.intValue() : logLevel;
    }

    private void setLogLevelPerBundle(Dictionary properties) {
        List value = (List) properties.get(ATTR_LOG_LEVEL_PER_BUNDLE);
        if (value == null) {
            value = new ArrayList();
        }
        Map logLevelPerBundle = new HashMap();
        Iterator it = value.iterator();
        while (it.hasNext()) {
            String token = (String) it.next();
            int semiColonIndex = token.indexOf(':');
            if (semiColonIndex != -1 && semiColonIndex != (token.length() - 1)) {
                logLevelPerBundle.put(token.substring(0, semiColonIndex),
                        new Integer(LogUtils.toLevel(token.substring(semiColonIndex + 1, token.length()), DEFAULT_LOG_LEVEL)));
            }
        }
        this.logLevelPerBundle = logLevelPerBundle;
    }

    public int getMaxNbOfLogsInMemory() {
        return maxNbOfLogsInMemory;
    }

    private void setMaxNbOfLogsInMemory(Dictionary properties) {
        Integer value = (Integer) properties.get(ATTR_MAX_NB_OF_LOGS_IN_MEMORY);
        if (value == null) {
            String valueStr = resourcesAccessorService.getProperty(context, context.getBundle().getSymbolicName()
                    + PROP_MAX_NB_OF_LOGS_IN_MEMORY);
            if (valueStr != null) {
                try {
                    value = Integer.valueOf(valueStr);
                } catch (NumberFormatException e) {
                    // Ignore
                }
            }
        }
        this.maxNbOfLogsInMemory = (value != null) ? value.intValue() : DEFAULT_MAX_NB_OF_LOGS_IN_MEMORY;
    }

    public boolean getLogToStdout() {
        return logToStdout;
    }

    private void setLogToStdout(Dictionary properties) {
        Boolean value = (Boolean) properties.get(ATTR_LOG_TO_STDOUT);
        if (value == null) {
            String valueStr = resourcesAccessorService.getProperty(context, context.getBundle().getSymbolicName()
                    + PROP_LOG_TO_STDOUT);
            if (valueStr != null) {
                value = new Boolean(valueStr.equalsIgnoreCase("true"));
            }
        }
        this.logToStdout = (value != null) ? value.booleanValue() : DEFAULT_LOG_TO_STDOUT;
    }

    // (Re-)compute the cache bidFilters from blFilters and the current
    // set of bundles.

    private void initBids() {
        Bundle[] bundles = context.getBundles();
        bids.clear();
        for (int i = bundles.length - 1; 0 <= i; i--) {
            Bundle bundle = bundles[i];
            addBid(bundle);
        }
    }

    /**
     * Compute and cache a bidFilter entry from blFilters for the given bundle.
     *
     * @param bundle The bundle to update the cached log level for.
     */
    private void addBid(Bundle bundle) {
        Map bids = new HashMap(this.bids);
        bids.put(new Long(bundle.getBundleId()), getSymbolicName(bundle));
        this.bids = bids;
    }

    private void removeBid(Bundle bundle) {
        Map bids = new HashMap(this.bids);
        bids.remove(new Long(bundle.getBundleId()));
        this.bids = bids;
    }

    private String getSymbolicName(Bundle bundle) {
        Dictionary d = bundle.getHeaders();
        String bsn = (String) d.get("Bundle-SymbolicName");
        if (bsn != null && bsn.length() > 0) {
            // Remove parameters and directives from the value
            int semiPos = bsn.indexOf(';');
            if (-1 < semiPos) {
                bsn = bsn.substring(0, semiPos).trim();
            }
        } else {
            bsn = bundle.getLocation();
        }
        return bsn;
    }

    // Implements BundleListener

    public void bundleChanged(BundleEvent event) {
        switch (event.getType()) {
        case BundleEvent.INSTALLED: // Fall through
            addBid(event.getBundle());
            break;
        case BundleEvent.UNINSTALLED:
            removeBid(event.getBundle());
            break;
        default:
        }
    }

    public void updated(Dictionary properties) throws ConfigurationException, IllegalArgumentException {
        if (properties == null) {
            properties = new Hashtable();
        }
        setMaxNbOfFiles(properties);
        setMaxFileSize(properties);
        setForceFlush(properties);
        setLogLevel(properties);
        setLogLevelPerBundle(properties);
        setMaxNbOfLogsInMemory(properties);
        setLogToStdout(properties);
    }
}
