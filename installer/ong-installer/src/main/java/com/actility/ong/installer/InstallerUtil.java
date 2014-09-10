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
 * id $Id: InstallerUtil.java 8956 2014-06-20 12:12:51Z MLouiset $
 * author $Author: MLouiset $
 * version $Revision: 8956 $
 * lastrevision $Date: 2014-06-20 14:12:51 +0200 (Fri, 20 Jun 2014) $
 * modifiedby $LastChangedBy: MLouiset $
 * lastmodified $LastChangedDate: 2014-06-20 14:12:51 +0200 (Fri, 20 Jun 2014) $
 */
package com.actility.ong.installer;

import java.io.File;
import java.io.PrintStream;
import java.text.DateFormat;
import java.text.SimpleDateFormat;
import java.util.Calendar;
import java.util.HashMap;
import java.util.Map;
import java.util.Properties;
import java.util.TreeMap;

import com.actility.ong.installer.config.ArchConfig;
import com.actility.ong.installer.config.ModuleConfig;
import com.actility.ong.installer.config.ProductConfig;

public class InstallerUtil {
    public static final String ZB_NO_SECURITY = "No Security";
    public static final String ZB_HA_STD_SECURITY = "HA Standard Security";

    public static final String FIRMWARE_SUBPATH_STANDALONE_GSC = "firmwares/ong/";
    public static final String FIRMWARE_SUBPATH_ZB_HA_STD_SECURITY = "firmwares/ongZbHAStdSecurity/";
    public static final String FIRMWARE_SUBPATH_ZB_NO_SECURITY = "firmwares/ongZbNoSecurity/";

    public static final String ONG_REMANANT_BACKUP_DIR = "ong-storage-backup";

    private static InstallerUtil installerUtil = new InstallerUtil();

    private final Map<String, ArchConfig> archProperties = new HashMap<String, ArchConfig>();
    private final Map<String, ProductConfig> productProperties = new HashMap<String, ProductConfig>();
    private final Map<String, ModuleConfig> moduleProperties = new HashMap<String, ModuleConfig>();

    /**
     * Get the singleton instance
     * 
     * @return the singleton instance
     */
    public static InstallerUtil getInstance() {
        return installerUtil;
    }

    public void addNewArch(String name, Properties properties) {
        ArchConfig config = new ArchConfig(name, properties);
        archProperties.put(config.getLabel(), config);
    }

    public void addNewProduct(String name, Properties properties) {
        ProductConfig config = new ProductConfig(name, properties);
        productProperties.put(config.getLabel(), config);
    }

    public void addNewModule(String name, Properties properties) {
        ModuleConfig config = new ModuleConfig(name, properties);
        moduleProperties.put(config.getName(), config);
    }

    public ArchConfig getArchFromLabel(String label) {
        return archProperties.get(label);
    }

    public ProductConfig getProductFromLabel(String label) {
        return productProperties.get(label);
    }

    public ModuleConfig getModuleFromName(String name) {
        return moduleProperties.get(name);
    }

    /**
     * Create a directory and all sub-directories required
     * 
     * @param dirName the full path of the directory to create.
     */
    public void makeLocalDir(PrintStream log, String dirName) {
        File f = new File(dirName);
        try {
            log.println("Create folder " + dirName);
            f.mkdirs();
        } catch (Exception e) {
            log.println("Unable to create folder " + dirName);
            e.printStackTrace(log);
        }
    }

    /**
     * Deletes recursively the given directory.
     * 
     * @param dir The directory to delete
     * @return Whether the deletion as succeeded or not. In case the deletion failed, the state of the directory is undefined.
     */
    public static boolean deleteDirectory(File dir) {
        if (dir.isDirectory()) {
            File[] children = dir.listFiles();
            for (int i = 0; i < children.length; i++) {
                boolean success = deleteDirectory(children[i]);
                if (!success) {
                    return false;
                }
            }
        }

        // The directory is now empty so delete it
        return dir.delete();
    }

    /**
     * Remove a directory and all sub-directories
     * 
     * @param dirName the full path of the directory to remove.
     */
    public void rmLocalDir(PrintStream log, String dirName) {
        File f = new File(dirName);
        try {
            log.println("removing folder " + dirName);
            deleteDirectory(f);
        } catch (Exception e) {
            log.println("unable to remove folder " + dirName);
            e.printStackTrace(log);
        }
    }

    public String[] getArchitectures() {
        Map<Integer, String> flavors = new TreeMap<Integer, String>();
        for (ArchConfig arch : archProperties.values()) {
            flavors.put(Integer.valueOf(arch.getIndex()), arch.getLabel());
        }
        String[] result = new String[flavors.size()];
        int index = 0;
        for (String label : flavors.values()) {
            result[index] = label;
            ++index;
        }
        return result;
    }

    public String[] getProducts() {
        Map<Integer, String> products = new TreeMap<Integer, String>();
        for (ProductConfig config : productProperties.values()) {
            products.put(Integer.valueOf(config.getIndex()), config.getLabel());
        }
        String[] result = new String[products.size()];
        int index = 0;
        for (String label : products.values()) {
            result[index] = label;
            ++index;
        }
        return result;
    }

    /**
     * Format the current date to include in filename and dirname
     * 
     * @return the date formatted this way: yyyy-MM-dd
     */
    public String getDateFormatForFileName() {
        Calendar calend = Calendar.getInstance();
        calend.setTimeInMillis(System.currentTimeMillis());
        DateFormat dateFormat = new SimpleDateFormat("yyyy-MM-dd");
        return dateFormat.format(calend.getTime());
    }

    /**
     * Format the current date and time to include in log entry
     * 
     * @return the date formatted this way: yyyy-MM-dd hh:mm:ss
     */
    public String getDateFormatForLogs() {
        Calendar calend = Calendar.getInstance();
        calend.setTimeInMillis(System.currentTimeMillis());
        DateFormat dateFormat = new SimpleDateFormat("yyyy-MM-dd hh:mm:ss");
        return dateFormat.format(calend.getTime());
    }

    /**
     * Build the firmware installation sub-path from provided installation tag.
     * 
     * @param installTag the installation tag as gathered from the MainInstallDialog window.
     * @return the firmware installation sub-path
     */
    public String getFirmwareSubPathFromTag(String installTag) {
        String arch = null;
        if (installTag.equals(InstallerUtil.ZB_NO_SECURITY)) {
            arch = FIRMWARE_SUBPATH_ZB_NO_SECURITY;
        } else if (installTag.equals(InstallerUtil.ZB_HA_STD_SECURITY)) {
            arch = FIRMWARE_SUBPATH_ZB_HA_STD_SECURITY;
        }
        return arch;
    }
}
