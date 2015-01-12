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
 * Please contact Actility, SA.,  4, rue Ampère 22300 LANNION FRANCE
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

package com.actility.m2m.system.monitoring;

import java.io.BufferedReader;
import java.io.File;
import java.io.FileReader;
import java.io.IOException;
import java.io.InputStreamReader;
import java.text.ParseException;
import java.util.ArrayList;
import java.util.Date;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

import org.apache.log4j.Logger;

import com.actility.m2m.system.config.Configuration;
import com.actility.m2m.system.log.BundleLogger;
import com.actility.m2m.system.version.Utils;
import com.actility.m2m.util.FormatUtils;
import com.actility.m2m.util.UtilConstants;
import com.actility.m2m.util.log.OSGiLogger;

public final class SystemInformationManager {
    private static final Logger LOG = OSGiLogger.getLogger(SystemInformationManager.class, BundleLogger.LOG);

    private static final File FILE_PROC_UPTIME = new File("/proc/uptime");
    private static final File FILE_PROC_MEMORY = new File("/proc/meminfo");
    private static final File FILE_PROC_LOADAVG = new File("/proc/loadavg");
    private static final File FILE_PROC_CPUINFO = new File("/proc/cpuinfo");
    private static final File FOLDER_DISK_DEVICE = new File("/sys/block/");
    private static final File FILE_ETC_NTP_CONF = new File("/etc/ntp.conf");
    private static File FILE_ONG_NTP_CONF = null;
    // private static final File FILE_PROC_NET_DEV = new File("/proc/net/dev");
    private static final File FOLDER_NET_DEVICE = new File("/sys/class/net/");
    private static final File FOLDER_PROC_ACPI_BATTERY = new File("/proc/acpi/battery/");
    private static final File FOLDER_PROC_ACPI_ADAPTER = new File("/proc/acpi/ac_adapter/");
    private static final File FOLDER_TEMPERATURE = new File("/sys/bus/platform/devices/coretemp.0");
    // private static final File FOLDER_TEMPERATURE = new File("/home/qdesrame/coretemp.0");
    private static final String CMD_DF = "df";
    private static final String PATH_RELATIVE_SECTOR_SIZE = File.separatorChar + "queue" + File.separatorChar
            + "hw_sector_size";
    private static final String[] CMD_ONG_STATUS = new String[] { "sh", "-c", "$ROOTACT\"/etc/acy\" status" };

    private static final String CATEGORY_CPU = "CATEGORY_CPU";
    private static final String CATEGORY_UPTIME = "CATEGORY_UPTIME";
    private static final String CATEGORY_LOADAVG = "CATEGORY_LOAD_AVG";
    private static final String CATEGORY_MEMORY = "CATEGORY_MEMORY";
    private static final String CATEGORY_DISK_SPACE = "CATEGORY_DISK_SPACE";
    private static final String CATEGORY_DISK_IO = "CATEGORY_DISK_IO";
    private static final String CATEGORY_NET_IO = "CATEGORY_NET_IO";
    private static final String CATEGORY_POWER = "CATEGORY_POWER";
    private static final String CATEGORY_TEMPERATURE = "CATEGORY_TEMPERATURE";
    private static final String CATEGORY_TIME = "CATEGORY_TIME";
    private static final String CATEGORY_ONG_SERVICES = "CATEGORY_ONG_SERVICES";

    private final String shellCmd;
    private final String rootactPath;
    private final Configuration configuration;
    private final Map/* <Indicator> */statics;

    /**
     * Constructor
     *
     * @param config
     * @param statics
     * @param shellCmd
     */
    public SystemInformationManager(Configuration config, Map/* <Indicator> */statics, String shellCmd, String rootactPath) {
        this.configuration = config;
        this.statics = statics;
        this.shellCmd = shellCmd;
        this.rootactPath = rootactPath;
        FILE_ONG_NTP_CONF = new File(this.rootactPath + "etc/ntp/ntp.cfg");
        SystemInformationManager.CMD_ONG_STATUS[0] = this.shellCmd;
    }

    /**
     * Updates given indicators
     *
     * @param indicators
     */
    public void updateIndicators(List/* <Indicator> */indicators) {
        this.updateIndicators(indicators, false);
    }

    /**
     * Updates given indicators
     *
     * @param indicators
     * @param forced if the update is forced
     */
    public void updateIndicators(List/* <Indicator> */indicators, boolean forced) {
        List categories = new ArrayList();
        for (int i = 0; i < indicators.size(); i++) {
            Indicator indicator = (Indicator) indicators.get(i);
            String category = getCategory(indicator);
            if (LOG.isDebugEnabled()) {
                LOG.debug("Indicator " + indicator.getName() + " has " + category + " category");
            }
            if (category == null) {
                LOG.error("The indicator called " + ((Indicator) indicators.get(i)).getName() + " hasn't category joined");
                indicators.remove(i);
                i--;
                continue;
            }
            if (!categories.contains(category)) {
                categories.add(category);
            }
        }
        updateCategories(categories, indicators, forced);
    }

    /**
     * Updates categories from given indicators
     *
     * @param categories
     * @param indicators
     * @param forced
     */
    private void updateCategories(List/* <String> */categories, List/* <Indicator> */indicators, boolean forced) {
        for (int i = 0; i < categories.size(); i++) {
            String name = (String) categories.get(i);
            if (name.equals(CATEGORY_CPU)) {
                updateCategoryCpu(indicators, forced);
            } else if (name.equals(CATEGORY_UPTIME)) {
                updateCategoryUptime(indicators, forced);
            } else if (name.equals(CATEGORY_LOADAVG)) {
                updateCategoryLoadavg(indicators, forced);
            } else if (name.equals(CATEGORY_MEMORY)) {
                updateCategoryMemory(indicators, forced);
            } else if (name.equals(CATEGORY_DISK_SPACE)) {
                updateCategoryDiskSpace(indicators, forced);
            } else if (name.equals(CATEGORY_DISK_IO)) {
                updateCategoryDiskIo(indicators, forced);
            } else if (name.equals(CATEGORY_NET_IO)) {
                updateCategoryNetIo(indicators, forced);
            } else if (name.equals(CATEGORY_POWER)) {
                updateCategoryPower(indicators, forced);
            } else if (name.equals(CATEGORY_TEMPERATURE)) {
                updateCategoryTemperature(indicators, forced);
            } else if (name.equals(CATEGORY_TIME)) {
                updateCategoryTime(indicators, forced);
            } else if (name.equals(CATEGORY_ONG_SERVICES)) {
                updateCategoryOngServices(indicators, forced);
            } else {
                LOG.error("The category called " + name + " not exists");
            }
        }
    }

    /**
     * Updates the cpu category
     *
     * @param indicators
     * @param forced
     */
    private void updateCategoryCpu(List/* <Indicator> */indicators, boolean forced) {
        int index = Utils.getIndexOf(indicators, Indicator.Name.NUM_CPU);
        if (index == -1) {
            LOG.error("Indicator " + Indicator.Name.NUM_CPU + " isn't in the list");
            return;
        }
        Indicator indicator = (Indicator) indicators.get(index);
        try {
            if (FILE_PROC_CPUINFO.exists()) {
                BufferedReader br = new BufferedReader(new FileReader(FILE_PROC_CPUINFO));
                String line = null;
                int cpus = 0;
                while ((line = br.readLine()) != null) {
                    if (line.length() > 9) {
                        String categorie = line.substring(0, 9);
                        if (categorie.equalsIgnoreCase("Processor")) {
                            cpus++;
                        }
                    }
                }
                indicator.setValue(new Integer(cpus));
                br.close();
            } else {
                throw new IOException("The file '" + FILE_PROC_CPUINFO + "' not exists");
            }
        } catch (IOException e) {
            LOG.error("", e);
            indicators.remove(index);
        }
    }

    /**
     * Updates the uptime category
     *
     * @param indicators
     * @param forced
     */
    private void updateCategoryUptime(List/* <Indicator> */indicators, boolean forced) {
        int index = Utils.getIndexOf(indicators, Indicator.Name.UPTIME);
        if (index == -1) {
            LOG.error("Indicator " + Indicator.Name.UPTIME + " isn't in the list");
            return;
        }
        Indicator indicator = (Indicator) indicators.get(index);
        try {
            if (FILE_PROC_UPTIME.exists()) {
                BufferedReader br = new BufferedReader(new FileReader(FILE_PROC_UPTIME));
                String line = br.readLine();
                if (line != null) {
                    int space = line.indexOf(' ');
                    if (space != -1 && space < line.length()) {
                        line = line.substring(0, space);
                        try {
                            long uptime = (long) (Float.parseFloat(line) * 1000);
                            indicator.setValue(FormatUtils.formatDuration(uptime));
                        } catch (NumberFormatException e) {
                            indicators.remove(index);
                            LOG.info("Unable to refresh category uptime");
                        }
                    }
                }
                br.close();
            } else {
                throw new IOException("The file '" + FILE_PROC_UPTIME + "' not exists");
            }
        } catch (IOException e) {
            LOG.error("", e);
            indicators.remove(index);
        }
    }

    /**
     * Updates the load average category
     *
     * @param indicators
     * @param forced
     */
    private void updateCategoryLoadavg(List/* <Indicator> */indicators, boolean forced) {
        Indicator loadAvgPt1m = null, loadAvgPt5m = null, loadAvgPt15m = null, procTotal = null;
        int indexLoadAvgPt1m = Utils.getIndexOf(indicators, Indicator.Name.LOAD_AVG_PT1M), indexLoadAvgPt5m = Utils.getIndexOf(
                indicators, Indicator.Name.LOAD_AVG_PT5M), indexLoadAvgPt15m = Utils.getIndexOf(indicators,
                Indicator.Name.LOAD_AVG_PT15M), indexProcTotal = Utils.getIndexOf(indicators, Indicator.Name.PROC_TOTAL);
        if (indexLoadAvgPt1m == -1 && indexLoadAvgPt5m == -1 && indexLoadAvgPt15m == -1 && indexProcTotal == -1) {
            LOG.error("Category Load Average : Indicators are not in the list");
            return;
        }
        if (indexLoadAvgPt1m != -1) {
            loadAvgPt1m = (Indicator) indicators.get(indexLoadAvgPt1m);
        }
        if (indexLoadAvgPt5m != -1) {
            loadAvgPt5m = (Indicator) indicators.get(indexLoadAvgPt5m);
        }
        if (indexLoadAvgPt15m != -1) {
            loadAvgPt15m = (Indicator) indicators.get(indexLoadAvgPt15m);
        }
        if (indexProcTotal != -1) {
            procTotal = (Indicator) indicators.get(indexProcTotal);
        }
        try {
            if (FILE_PROC_LOADAVG.exists()) {
                BufferedReader br = new BufferedReader(new FileReader(FILE_PROC_LOADAVG));
                String line = br.readLine();
                if (line != null) {
                    List infos = new ArrayList();
                    int begin = 0;
                    int pos = 0;
                    while ((pos = line.indexOf(' ', begin)) != -1 && pos > begin) {
                        infos.add(line.substring(begin, pos));
                        begin = pos + 1;
                    }
                    if (infos.size() < 4) {
                        throw new IOException("There is not enough informations (" + infos.size() + ") in " + FILE_PROC_LOADAVG);
                    }
                    if (indexLoadAvgPt1m != -1) {
                        try {
                            loadAvgPt1m.setValue(new Float((String) infos.get(0)));
                        } catch (NumberFormatException e) {
                            LOG.error(
                                    "Impossible to get " + Indicator.Name.LOAD_AVG_PT1M + ", it's not a number ("
                                            + infos.get(0) + ")", e);
                            indicators.remove(Indicator.Name.LOAD_AVG_PT1M);
                        }
                    }
                    if (indexLoadAvgPt5m != -1) {
                        try {
                            loadAvgPt5m.setValue(new Float((String) infos.get(1)));
                        } catch (NumberFormatException e) {
                            LOG.error(
                                    "Impossible to get " + Indicator.Name.LOAD_AVG_PT5M + ", it's not a number ("
                                            + infos.get(1) + ")", e);
                            indicators.remove(Indicator.Name.LOAD_AVG_PT5M);
                        }
                    }
                    if (indexLoadAvgPt15m != -1) {
                        try {
                            loadAvgPt15m.setValue(new Float((String) infos.get(2)));
                        } catch (NumberFormatException e) {
                            LOG.error(
                                    "Impossible to get " + Indicator.Name.LOAD_AVG_PT15M + ", it's not a number ("
                                            + infos.get(2) + ")", e);
                            indicators.remove(Indicator.Name.LOAD_AVG_PT15M);
                        }
                    }
                    if (indexProcTotal != -1) {
                        String proc = (String) infos.get(3);
                        int slash = proc.indexOf('/');
                        if (slash != -1 && slash < proc.length() - 1) {
                            // uptime.setActiveProcess(Integer.parseInt(proc.substring(0, slash)));
                            try {
                                procTotal.setValue(new Integer(proc.substring(slash + 1)));
                            } catch (NumberFormatException e) {
                                LOG.error(
                                        "Impossible to get " + Indicator.Name.PROC_TOTAL + ", it's not a number ("
                                                + proc.substring(slash + 1) + ")", e);
                                indicators.remove(Indicator.Name.PROC_TOTAL);
                            }
                        } else {
                            indicators.remove(Indicator.Name.PROC_TOTAL);
                        }
                    }
                } else {
                    throw new IOException("The file '" + FILE_PROC_LOADAVG + "' is empty");
                }
                br.close();
            } else {
                throw new IOException("The file '" + FILE_PROC_LOADAVG + "' not exists");
            }
        } catch (IOException e) {
            LOG.error("", e);
            indicators.remove(Indicator.Name.LOAD_AVG_PT1M);
            indicators.remove(Indicator.Name.LOAD_AVG_PT5M);
            indicators.remove(Indicator.Name.LOAD_AVG_PT15M);
            indicators.remove(Indicator.Name.PROC_TOTAL);
        }
    }

    private static final String INFO_MEMORY_TOTAL = "MemTotal:";
    private static final String INFO_MEMORY_FREE = "MemFree:";
    private static final String INFO_MEMORY_BUFFERS = "Buffers:";
    private static final String INFO_MEMORY_CACHED = "Cached:";
    private static final String INFO_SWAP_TOTAL = "SwapTotal:";
    private static final String INFO_SWAP_FREE = "SwapFree:";

    /**
     * Updates the memory category
     *
     * @param indicators
     * @param forced
     */
    private void updateCategoryMemory(List/* <Indicator> */indicators, boolean forced) {
        Indicator memUsed = null, memFree = null, memTotal = null, memBuffers = null, memCached = null, swpUsed = null, swpFree = null, swpTotal = null;
        int indexMemUsed = Utils.getIndexOf(indicators, Indicator.Name.MEM_USED), indexMemFree = Utils.getIndexOf(indicators,
                Indicator.Name.MEM_FREE), indexMemTotal = Utils.getIndexOf(indicators, Indicator.Name.MEM_TOTAL), indexMemBuffers = Utils
                .getIndexOf(indicators, Indicator.Name.MEM_BUFFERS), indexMemCached = Utils.getIndexOf(indicators,
                Indicator.Name.MEM_CACHED), indexSwpUsed = Utils.getIndexOf(indicators, Indicator.Name.MEM_SWP_USED), indexSwpFree = Utils
                .getIndexOf(indicators, Indicator.Name.MEM_SWP_FREE), indexSwpTotal = Utils.getIndexOf(indicators,
                Indicator.Name.MEM_SWP_TOTAL);
        if (indexMemUsed == -1 && indexMemFree == -1 && indexMemTotal == -1 && indexMemBuffers == -1 && indexMemCached == -1
                && indexSwpUsed == -1 && indexSwpFree == -1 && indexSwpTotal == -1) {
            LOG.error("Category Memory : Indicators are not in the list");
            return;
        }
        if (indexMemUsed != -1) {
            memUsed = (Indicator) indicators.get(indexMemUsed);
        }
        if (indexMemFree != -1) {
            memFree = (Indicator) indicators.get(indexMemFree);
        }
        if (indexMemTotal != -1) {
            memTotal = (Indicator) indicators.get(indexMemTotal);
        } else {
            memTotal = (Indicator) statics.get(Indicator.Name.MEM_TOTAL);
        }
        if (indexMemBuffers != -1) {
            memBuffers = (Indicator) indicators.get(indexMemBuffers);
        }
        if (indexMemCached != -1) {
            memCached = (Indicator) indicators.get(indexMemCached);
        }
        if (indexSwpUsed != -1) {
            swpUsed = (Indicator) indicators.get(indexSwpUsed);
        }
        if (indexSwpFree != -1) {
            swpFree = (Indicator) indicators.get(indexSwpFree);
        }
        if (indexSwpTotal != -1) {
            swpTotal = (Indicator) indicators.get(indexSwpTotal);
        } else {
            swpTotal = (Indicator) statics.get(Indicator.Name.MEM_SWP_TOTAL);
        }
        int memoryTotal = -1, swapTotal = -1;
        Object memoryTotalValue = memTotal.getValue();
        if (memoryTotalValue != null && memoryTotalValue instanceof Integer) {
            memoryTotal = ((Integer) memoryTotalValue).intValue();
        }
        Object swapTotalValue = swpTotal.getValue();
        if (swapTotalValue != null && swapTotalValue instanceof Integer) {
            swapTotal = ((Integer) swapTotalValue).intValue();
        }
        try {
            if (FILE_PROC_MEMORY.exists()) {
                BufferedReader br = new BufferedReader(new FileReader(FILE_PROC_MEMORY));
                try {
                    String line = null;
                    while ((line = br.readLine()) != null) {
                        int dotPos = line.indexOf(':');
                        if (dotPos > 0 && dotPos + 1 < line.length()) {
                            String type = line.substring(0, dotPos + 1);
                            line = line.substring(type.length() + 1).trim();
                            // if (LOG.isInfoEnabled()) {
                            // LOG.info(type+" : "+line);
                            // }
                            if (line.endsWith(" kB") && line.length() > 3) {
                                line = line.substring(0, line.length() - 3);
                                if (type.equals(INFO_MEMORY_TOTAL)) {
                                    if (memoryTotal == -1) {
                                        try {
                                            memoryTotal = Integer.parseInt(line);
                                            if (LOG.isInfoEnabled()) {
                                                LOG.info("Value of memory total:" + memoryTotal);
                                            }
                                            if (indexMemTotal != -1) {
                                                memTotal.setValue(new Integer(memoryTotal));
                                            }
                                        } catch (NumberFormatException e) {
                                            LOG.error("Impossible to get " + Indicator.Name.MEM_TOTAL + ", it's not a number ("
                                                    + line + ")", e);
                                            if (indexMemTotal != -1) {
                                                indicators.remove(Indicator.Name.MEM_TOTAL);
                                            }
                                        }
                                    }
                                } else if (type.equals(INFO_MEMORY_FREE)) {
                                    try {
                                        int memoryFree = Integer.parseInt(line);
                                        if (indexMemFree != -1) {
                                            memFree.setValue(new Integer(memoryFree));
                                        }
                                        if (indexMemUsed != -1) {
                                            if (memoryTotal != -1) {
                                                memUsed.setValue(new Integer(memoryTotal - memoryFree));
                                            } else {
                                                indicators.remove(Indicator.Name.MEM_USED);
                                            }
                                        }
                                    } catch (NumberFormatException e) {
                                        LOG.error("Impossible to get " + Indicator.Name.MEM_FREE + ", it's not a number ("
                                                + line + ")", e);
                                        if (indexMemFree != -1) {
                                            indicators.remove(Indicator.Name.MEM_FREE);
                                        }
                                        if (indexMemUsed != -1) {
                                            indicators.remove(Indicator.Name.MEM_USED);
                                        }
                                    }
                                } else if (type.equals(INFO_MEMORY_BUFFERS)) {
                                    try {
                                        if (indexMemBuffers != -1) {
                                            memBuffers.setValue(new Integer(line));
                                        }
                                    } catch (NumberFormatException e) {
                                        LOG.error("Impossible to get " + Indicator.Name.MEM_BUFFERS + ", it's not a number ("
                                                + line + ")", e);
                                        if (indexMemBuffers != -1) {
                                            indicators.remove(Indicator.Name.MEM_BUFFERS);
                                        }
                                    }
                                } else if (type.equals(INFO_MEMORY_CACHED)) {
                                    try {
                                        if (indexMemCached != -1) {
                                            memCached.setValue(new Integer(line));
                                        }
                                    } catch (NumberFormatException e) {
                                        LOG.error("Impossible to get " + Indicator.Name.MEM_CACHED + ", it's not a number ("
                                                + line + ")", e);
                                        if (indexMemCached != -1) {
                                            indicators.remove(Indicator.Name.MEM_CACHED);
                                        }
                                    }
                                } else if (type.equals(INFO_SWAP_TOTAL)) {
                                    if (swapTotal == -1) {
                                        try {
                                            swapTotal = Integer.parseInt(line);
                                            if (indexSwpTotal != -1) {
                                                swpTotal.setValue(new Integer(swapTotal));
                                            }
                                        } catch (NumberFormatException e) {
                                            LOG.error("Impossible to get " + Indicator.Name.MEM_SWP_TOTAL
                                                    + ", it's not a number (" + line + ")", e);
                                            if (indexSwpTotal != -1) {
                                                indicators.remove(Indicator.Name.MEM_SWP_TOTAL);
                                            }
                                        }
                                    }
                                } else if (type.equals(INFO_SWAP_FREE)) {
                                    try {
                                        int swapFree = Integer.parseInt(line);
                                        if (indexSwpFree != -1) {
                                            swpFree.setValue(new Integer(swapFree));
                                        }
                                        if (indexSwpUsed != -1) {
                                            if (swapTotal != -1) {
                                                swpUsed.setValue(new Integer(swapTotal - swapFree));
                                            } else {
                                                indicators.remove(Indicator.Name.MEM_SWP_USED);
                                            }
                                        }
                                    } catch (NumberFormatException e) {
                                        LOG.error("Impossible to get " + Indicator.Name.MEM_SWP_FREE + ", it's not a number ("
                                                + line + ")", e);
                                        if (indexSwpFree != -1) {
                                            indicators.remove(Indicator.Name.MEM_SWP_FREE);
                                        }
                                        if (indexSwpUsed != -1) {
                                            indicators.remove(Indicator.Name.MEM_SWP_USED);
                                        }
                                    }
                                }
                            }
                            // break;
                        }
                    }
                } finally {
                    br.close();
                }
            } else {
                throw new IOException("The file '" + FILE_PROC_MEMORY + "' not exists");
            }
        } catch (IOException e) {
            LOG.error("", e);
            indicators.remove(Indicator.Name.MEM_USED);
            indicators.remove(Indicator.Name.MEM_FREE);
            indicators.remove(Indicator.Name.MEM_TOTAL);
            indicators.remove(Indicator.Name.MEM_BUFFERS);
            indicators.remove(Indicator.Name.MEM_CACHED);
            indicators.remove(Indicator.Name.MEM_SWP_USED);
            indicators.remove(Indicator.Name.MEM_SWP_FREE);
            indicators.remove(Indicator.Name.MEM_SWP_TOTAL);
        }
    }

    private Map/* <String,String> */diskParts = new HashMap();

    /**
     * Updates the disk space category
     *
     * @param indicators
     * @param forced
     */
    private void updateCategoryDiskSpace(List/* <Indicator> */indicators, boolean forced) {
        List diskMonitoringExclusion = (List) configuration.getValue(Configuration.Name.DISK_PART_MONITORING_EXCLUSIONS/*
                                                                                                                        * ,
                                                                                                                        * List.
                                                                                                                        * class
                                                                                                                        */);
        if (diskMonitoringExclusion == null) {
            diskMonitoringExclusion = new ArrayList();
        }
        Indicator diskPartUsed = null, diskPartFree = null, diskPartSize = null, diskPartName = null;
        int indexDiskPartUsed = Utils.getIndexOf(indicators, Indicator.Name.DISK_PART_USED), indexDiskPartFree = Utils
                .getIndexOf(indicators, Indicator.Name.DISK_PART_FREE), indexDiskPartSize = Utils.getIndexOf(indicators,
                Indicator.Name.DISK_PART_SIZE), indexDiskPartName = Utils.getIndexOf(indicators, Indicator.Name.DISK_PART_NAME);
        if (indexDiskPartUsed == -1 && indexDiskPartFree == -1 && indexDiskPartSize == -1 && indexDiskPartName == -1) {
            LOG.error("Category Disk space : Indicators are not in the list");
            return;
        }
        if (indexDiskPartUsed != -1) {
            diskPartUsed = (Indicator) indicators.get(indexDiskPartUsed);
        }
        if (indexDiskPartFree != -1) {
            diskPartFree = (Indicator) indicators.get(indexDiskPartFree);
        }
        if (indexDiskPartSize != -1) {
            diskPartSize = (Indicator) indicators.get(indexDiskPartSize);
        }
        if (indexDiskPartName != -1) {
            diskPartName = (Indicator) indicators.get(indexDiskPartName);
        } else {
            diskPartName = (Indicator) statics.get(Indicator.Name.DISK_PART_NAME);
        }
        boolean firstInit = false;
        List diskPartsNames = new ArrayList();
        Object diskPartNameValue = diskPartName.getValue();
        if (diskPartNameValue != null && diskPartNameValue instanceof List && ((List) diskPartNameValue).size() > 0) {
            diskPartsNames = (List) diskPartNameValue;
        } else {
            diskPartName.setValue(diskPartsNames);
            firstInit = true;
        }
        try {
            Runtime runtime = Runtime.getRuntime();
            Process process = runtime.exec(CMD_DF);
            process.waitFor();
            if (process.exitValue() != 0) {
                throw new IOException("Error occured during execution of command '" + CMD_DF + "'. Status code:"
                        + process.exitValue());
            }
            String read = null;
            boolean first = false;
            BufferedReader br = new BufferedReader(new InputStreamReader(process.getInputStream()));

            int counter = 0;
            boolean newLine = true;
            String name = null;
            String size = null;
            String used = null;
            String free = null;
            String mount = null;
            while ((read = br.readLine()) != null) {
                if (!first) {
                    first = true;
                } else {
                    if (newLine == true) {
                        name = null;
                        size = null;
                        used = null;
                        free = null;
                        mount = null;
                        counter = 0;
                    } else {
                        newLine = true;
                    }
                    read = read.trim();
                    while (read.length() != 0) {
                        int space = read.indexOf(' ');
                        if (space == -1) {
                            space = read.length();
                            if (counter != 5) {
                                newLine = false;
                            }
                        }
                        String tmp = read.substring(0, space);
                        switch (counter) {
                        case 0:
                            name = tmp;
                            // if (LOG.isInfoEnabled()) {
                            // LOG.info("disk name :"+name);
                            // }
                            break;
                        case 1:
                            size = tmp;
                            // if (LOG.isInfoEnabled()) {
                            // LOG.info("disk size :"+size);
                            // }
                            break;
                        case 2:
                            used = tmp;
                            // if (LOG.isInfoEnabled()) {
                            // LOG.info("disk used :"+used);
                            // }
                            break;
                        case 3:
                            free = tmp;
                            // if (LOG.isInfoEnabled()) {
                            // LOG.info("disk free :"+free);
                            // }
                            break;
                        case 4:
                            break;
                        case 5:
                            mount = tmp;
                            break;
                        default:
                            break;
                        }
                        read = read.substring(space).trim();
                        counter++;
                    }
                    if (newLine) {
                        if (firstInit) {
                            if (indexDiskPartName != -1 && name != null && !diskMonitoringExclusion.contains(name)) {
                                if (mount != null) {
                                    diskParts.put(mount, new Integer(diskPartsNames.size()));
                                }
                                diskPartsNames.add(name);
                            }
                        }
                        int pos = -1;
                        if (mount != null) {
                            Integer position = (Integer) diskParts.get(mount);
                            if (position != null) {
                                pos = position.intValue();
                            }
                        }
                        if (pos == -1) {
                            for (int i = 0; i < diskPartsNames.size(); i++) {
                                String partName = (String) diskPartsNames.get(i);
                                if (name.equals(partName)) {
                                    pos = i;
                                    break;
                                }
                            }
                        }
                        if (pos != -1) {
                            // LOG.debug("It's the disk :"+i+":"+name);
                            if (indexDiskPartUsed != -1) {
                                Object diskPartUsedValue = diskPartUsed.getValue();
                                List diskPartsUsed = new ArrayList();
                                if (diskPartUsedValue != null && diskPartUsedValue instanceof List) {
                                    diskPartsUsed = (List) diskPartUsedValue;
                                } else {
                                    diskPartUsed.setValue(diskPartsUsed);
                                }
                                for (int j = diskPartsUsed.size(); j < diskPartsNames.size(); j++) {
                                    diskPartsUsed.add(null);
                                }
                                if (used != null) {
                                    try {
                                        diskPartsUsed.set(pos, new Integer(used));
                                    } catch (NumberFormatException e) {
                                        LOG.error("", e);
                                    }
                                }
                            }
                            if (indexDiskPartFree != -1) {
                                Object diskPartFreeValue = diskPartFree.getValue();
                                List diskPartsFree = new ArrayList();
                                if (diskPartFreeValue != null && diskPartFreeValue instanceof List) {
                                    diskPartsFree = (List) diskPartFreeValue;
                                } else {
                                    diskPartFree.setValue(diskPartsFree);
                                }
                                for (int j = diskPartsFree.size(); j < diskPartsNames.size(); j++) {
                                    diskPartsFree.add(null);
                                }
                                if (free != null) {
                                    try {
                                        diskPartsFree.set(pos, new Integer(free));
                                    } catch (NumberFormatException e) {
                                        LOG.error("", e);
                                    }
                                }
                            }
                            if (indexDiskPartSize != -1) {
                                Object diskPartSizeValue = diskPartSize.getValue();
                                List diskPartsSize = new ArrayList();
                                if (diskPartSizeValue != null && diskPartSizeValue instanceof List) {
                                    diskPartsSize = (List) diskPartSizeValue;
                                } else {
                                    diskPartSize.setValue(diskPartsSize);
                                }
                                for (int j = diskPartsSize.size(); j < diskPartsNames.size(); j++) {
                                    diskPartsSize.add(null);
                                }
                                if (size != null) {
                                    try {
                                        diskPartsSize.set(pos, new Integer(size));
                                    } catch (NumberFormatException e) {
                                        LOG.error("", e);
                                    }
                                }
                            }
                        }
                        // for(int i=0;i<diskPartsNames.size();i++){
                        // String partName = (String) diskPartsNames.get(i);
                        // if(name.equals(partName)){
                        // // LOG.debug("It's the disk :"+i+":"+name);
                        // if(indexDiskPartUsed!=-1){
                        // Object diskPartUsedValue = diskPartUsed.getValue();
                        // List diskPartsUsed = new ArrayList();
                        // if(diskPartUsedValue!=null && diskPartUsedValue instanceof List){
                        // diskPartsUsed = (List) diskPartUsedValue;
                        // }else{
                        // diskPartUsed.setValue(diskPartsUsed);
                        // }
                        // for(int j=diskPartsUsed.size(); j<diskPartsNames.size();j++){
                        // diskPartsUsed.add(null);
                        // }
                        // if(used!=null){
                        // try{
                        // diskPartsUsed.set(i, new Integer(used));
                        // }catch(NumberFormatException e){
                        // LOG.error("", e);
                        // }
                        // }
                        // }
                        // if(indexDiskPartFree!=-1){
                        // Object diskPartFreeValue = diskPartFree.getValue();
                        // List diskPartsFree = new ArrayList();
                        // if(diskPartFreeValue!=null && diskPartFreeValue instanceof List){
                        // diskPartsFree = (List) diskPartFreeValue;
                        // }else{
                        // diskPartFree.setValue(diskPartsFree);
                        // }
                        // for(int j=diskPartsFree.size(); j<diskPartsNames.size();j++){
                        // diskPartsFree.add(null);
                        // }
                        // if(free!=null){
                        // try{
                        // diskPartsFree.set(i, new Integer(free));
                        // }catch(NumberFormatException e){
                        // LOG.error("", e);
                        // }
                        // }
                        // }
                        // if(indexDiskPartSize!=-1){
                        // Object diskPartSizeValue = diskPartSize.getValue();
                        // List diskPartsSize = new ArrayList();
                        // if(diskPartSizeValue!=null && diskPartSizeValue instanceof List){
                        // diskPartsSize = (List) diskPartSizeValue;
                        // }else{
                        // diskPartSize.setValue(diskPartsSize);
                        // }
                        // for(int j=diskPartsSize.size(); j<diskPartsNames.size();j++){
                        // diskPartsSize.add(null);
                        // }
                        // if(size!=null){
                        // try{
                        // diskPartsSize.set(i, new Integer(size));
                        // }catch(NumberFormatException e){
                        // LOG.error("", e);
                        // }
                        // }
                        // }
                        // break;
                        // }
                        // }
                    }
                }
            }
            br.close();
        } catch (IOException e) {
            LOG.error("", e);
            indicators.remove(Indicator.Name.DISK_PART_USED);
            indicators.remove(Indicator.Name.DISK_PART_FREE);
            indicators.remove(Indicator.Name.DISK_PART_SIZE);
            indicators.remove(Indicator.Name.DISK_PART_NAME);
        } catch (InterruptedException e) {
            LOG.error("", e);
            indicators.remove(Indicator.Name.DISK_PART_USED);
            indicators.remove(Indicator.Name.DISK_PART_FREE);
            indicators.remove(Indicator.Name.DISK_PART_SIZE);
            indicators.remove(Indicator.Name.DISK_PART_NAME);
        }
    }

    private List disksBlockSize = new ArrayList();
    private List disksIo = new ArrayList();

    /**
     * Updates the disk io category
     *
     * @param indicators
     * @param forced
     */
    private void updateCategoryDiskIo(List/* <Indicator> */indicators, boolean forced) {
        List diskMonitoringExclusion = (List) configuration.getValue(Configuration.Name.DISK_MONITORING_EXCLUSIONS/*
                                                                                                                   * ,
                                                                                                                   * List.class
                                                                                                                   */);
        if (diskMonitoringExclusion == null) {
            diskMonitoringExclusion = new ArrayList();
        }
        Indicator diskName = null, diskReadIo = null, diskWriteIo = null;
        int indexDiskName = Utils.getIndexOf(indicators, Indicator.Name.DISK_NAME), indexDiskReadIo = Utils.getIndexOf(
                indicators, Indicator.Name.DISK_READ_IO), indexDiskWriteIo = Utils.getIndexOf(indicators,
                Indicator.Name.DISK_WRITE_IO);
        if (indexDiskName == -1 && indexDiskReadIo == -1 && indexDiskWriteIo == -1) {
            LOG.error("Category DiskIo : Indicators are not in the list");
            return;
        }
        if (indexDiskName != -1) {
            diskName = (Indicator) indicators.get(indexDiskName);
        } else {
            diskName = (Indicator) statics.get(Indicator.Name.DISK_NAME);
        }
        if (indexDiskReadIo != -1) {
            diskReadIo = (Indicator) indicators.get(indexDiskReadIo);
        }
        if (indexDiskWriteIo != -1) {
            diskWriteIo = (Indicator) indicators.get(indexDiskWriteIo);
        }
        boolean firstInit = false;
        List deviceNames = new ArrayList();
        Object diskNameValue = diskName.getValue();
        if (diskNameValue != null && diskNameValue instanceof List && ((List) diskNameValue).size() > 0) {
            deviceNames = (List) diskNameValue;
        } else {
            diskName.setValue(deviceNames);
            firstInit = true;
        }
        try {
            if (!forced) {
                if (FOLDER_DISK_DEVICE.isDirectory()) {
                    File[] devices = FOLDER_DISK_DEVICE.listFiles();
                    for (int i = 0; i < devices.length; i++) {
                        File device = devices[i];
                        if (device.isDirectory()) {
                            String name = device.getName();
                            if (name.startsWith("hd") || name.startsWith("sd") || name.startsWith("mmc")
                                    || name.startsWith("mtd")) {
                                String readBlock = null;
                                String writeBlock = null;
                                File[] deviceInfos = device.listFiles();
                                for (int j = 0; j < deviceInfos.length; j++) {
                                    File deviceInfo = deviceInfos[j];
                                    if (deviceInfo.getName().equals("stat")) {
                                        BufferedReader br = new BufferedReader(new FileReader(deviceInfo));
                                        String line = br.readLine();
                                        br.close();
                                        if (line != null) {
                                            if (line.equals("       0        0        0        0        0        0        0        0        0        0        0")) {
                                                break;
                                            }
                                            line = line.trim();
                                            int space;
                                            int counter = 0;
                                            while ((space = line.indexOf(' ')) != -1) {
                                                String val = line.substring(0, space);
                                                if (val.length() > 0) {
                                                    if (counter == 2) {
                                                        readBlock = val;
                                                    }
                                                    if (counter == 6) {
                                                        writeBlock = val;
                                                        break;
                                                    }
                                                }
                                                line = line.substring(space).trim();
                                                counter++;
                                            }
                                            if (firstInit) {
                                                // LOG.info("it's the first init");
                                                if (indexDiskName != -1 && name != null
                                                        && !diskMonitoringExclusion.contains(name)) {
                                                    deviceNames.add(name);
                                                    File sectorSizeFile = new File(device.getAbsolutePath()
                                                            + PATH_RELATIVE_SECTOR_SIZE);
                                                    if (sectorSizeFile.exists() && sectorSizeFile.isFile()) {
                                                        br = new BufferedReader(new FileReader(sectorSizeFile));
                                                        String blockSize = br.readLine();
                                                        if (blockSize != null) {
                                                            try {
                                                                disksBlockSize.add(new Integer(blockSize));
                                                            } catch (NumberFormatException e) {
                                                                LOG.error("Sector size of " + name + " isn't valid ("
                                                                        + blockSize + ")", e);
                                                            }
                                                            try {
                                                                disksIo.add(new Integer[] { new Integer(readBlock),
                                                                        new Integer(writeBlock) });
                                                            } catch (NumberFormatException e) {
                                                                LOG.error("Disk IO of " + name + " isn't valid (" + readBlock
                                                                        + "," + writeBlock + ")", e);
                                                            }
                                                        }
                                                        br.close();
                                                    }
                                                }
                                            }
                                            for (int k = 0; k < deviceNames.size(); k++) {
                                                String deviceName = (String) deviceNames.get(k);
                                                if (name.equals(deviceName)) {
                                                    Integer blockSize = (Integer) disksBlockSize.get(k);
                                                    Integer[] oldValue = (Integer[]) disksIo.get(k);
                                                    LOG.debug("device: " + name + ", oldValue:[" + oldValue[0].intValue() + ","
                                                            + oldValue[1].intValue() + "],blockSize:" + blockSize);
                                                    if (indexDiskReadIo != -1) {
                                                        Object diskReadIoValue = diskReadIo.getValue();
                                                        List devicesReadIo = new ArrayList();
                                                        if (diskReadIoValue != null && diskReadIoValue instanceof List) {
                                                            devicesReadIo = (List) diskReadIoValue;
                                                        } else {
                                                            diskReadIo.setValue(devicesReadIo);
                                                        }
                                                        for (int l = devicesReadIo.size(); l < deviceNames.size(); l++) {
                                                            devicesReadIo.add(null);
                                                        }
                                                        try {
                                                            Integer newValue = new Integer(readBlock);
                                                            if (blockSize != null && oldValue != null) {
                                                                // if (LOG.isInfoEnabled()) {
                                                                // LOG.info("Number of read block in 1 min: "+(newValue.intValue()-oldValue[0].intValue()));
                                                                // }
                                                                // if (LOG.isDebugEnabled()) {
                                                                // LOG.debug("Setting readIo index "+k+" with value :"+(float)(newValue.intValue()-
                                                                // oldValue[0].intValue())*blockSize.intValue()/1024/60);
                                                                // }
                                                                devicesReadIo.set(k, new Float(
                                                                        (newValue.intValue() - oldValue[0].intValue())
                                                                                * blockSize.intValue() / 1024 / 60));
                                                            }
                                                        } catch (NumberFormatException e) {
                                                            LOG.error("Disk Read Io of " + name + " isn't valid (" + readBlock
                                                                    + ")", e);
                                                        }
                                                    }
                                                    if (indexDiskWriteIo != -1) {
                                                        Object diskWriteIoValue = diskWriteIo.getValue();
                                                        List devicesWriteIo = new ArrayList();
                                                        if (diskWriteIoValue != null && diskWriteIoValue instanceof List) {
                                                            devicesWriteIo = (List) diskWriteIoValue;
                                                        } else {
                                                            diskWriteIo.setValue(devicesWriteIo);
                                                        }
                                                        for (int l = devicesWriteIo.size(); l < deviceNames.size(); l++) {
                                                            devicesWriteIo.add(null);
                                                        }
                                                        try {
                                                            Integer newValue = new Integer(writeBlock);
                                                            if (blockSize != null && oldValue != null) {
                                                                // if (LOG.isInfoEnabled()) {
                                                                // LOG.info("Number of writed block in 1 min: "+(newValue.intValue()-oldValue[1].intValue()));
                                                                // }
                                                                // if (LOG.isDebugEnabled()) {
                                                                // LOG.debug("Setting writeIo index "+k+" with value :"+(newValue.intValue()-
                                                                // oldValue[1].intValue())*blockSize.intValue()/1024/60);
                                                                // }
                                                                devicesWriteIo.set(k, new Float(
                                                                        (newValue.intValue() - oldValue[1].intValue())
                                                                                * blockSize.intValue() / 1024 / 60));
                                                            }
                                                        } catch (NumberFormatException e) {
                                                            LOG.error("Disk Read Io of " + name + " isn't valid (" + readBlock
                                                                    + ")", e);
                                                        }
                                                    }
                                                    oldValue[0] = new Integer(readBlock);
                                                    oldValue[1] = new Integer(writeBlock);
                                                    if (LOG.isDebugEnabled()) {
                                                        LOG.debug("device: " + name + ", newValue:[" + oldValue[0].intValue()
                                                                + "," + oldValue[1].intValue() + "]");
                                                    }
                                                    break;
                                                }
                                            }
                                        }
                                        break;
                                    }
                                }
                            }
                        }
                    }
                } else {
                    throw new IOException(FOLDER_DISK_DEVICE + " is not a directory");
                }
            }
        } catch (IOException e) {
            LOG.error("", e);
            indicators.remove(Indicator.Name.DISK_NAME);
            indicators.remove(Indicator.Name.DISK_READ_IO);
            indicators.remove(Indicator.Name.DISK_WRITE_IO);
        }
    }

    private List netsIo = new ArrayList();

    /**
     * Updates the net io category
     *
     * @param indicators
     * @param forced
     */
    private void updateCategoryNetIo(List/* <Indicator> */indicators, boolean forced) {
        List netMonitoringExclusion = (List) configuration
                .getValue(Configuration.Name.NET_MONITORING_EXCLUSIONS/* , List.class */);
        if (netMonitoringExclusion == null) {
            netMonitoringExclusion = new ArrayList();
        }
        Indicator netName = null, netRecvIo = null, netSentIo = null, netBandwidth = null, netBandwidthUsable = null;
        int indexNetName = Utils.getIndexOf(indicators, Indicator.Name.NET_NAME), indexNetRecvIo = Utils.getIndexOf(indicators,
                Indicator.Name.NET_RECV_IO), indexNetSentIo = Utils.getIndexOf(indicators, Indicator.Name.NET_SENT_IO), indexNetBandwidth = Utils
                .getIndexOf(indicators, Indicator.Name.NET_BANDWIDTH), indexNetBandwidthUsable = Utils.getIndexOf(indicators,
                Indicator.Name.NET_BANDWIDTH_USABLE);
        if (indexNetName == -1 && indexNetRecvIo == -1 && indexNetSentIo == -1 && indexNetBandwidth == -1
                && indexNetBandwidthUsable == -1) {
            LOG.error("Category NetIo : Indicators are not in the list");
            return;
        }
        if (indexNetName != -1) {
            netName = (Indicator) indicators.get(indexNetName);
        } else {
            netName = (Indicator) statics.get(Indicator.Name.NET_NAME);
        }
        if (indexNetRecvIo != -1) {
            netRecvIo = (Indicator) indicators.get(indexNetRecvIo);
        }
        if (indexNetSentIo != -1) {
            netSentIo = (Indicator) indicators.get(indexNetSentIo);
        }
        if (indexNetBandwidth != -1) {
            netBandwidth = (Indicator) indicators.get(indexNetBandwidth);
        }
        if (indexNetBandwidthUsable != -1) {
            netBandwidthUsable = (Indicator) indicators.get(indexNetBandwidthUsable);
        }
        boolean firstInit = false;
        List interfaceNames = new ArrayList();
        Object netNameValue = netName.getValue();
        if (netNameValue != null && netNameValue instanceof List && ((List) netNameValue).size() > 0) {
            interfaceNames = (List) netNameValue;
        } else {
            netName.setValue(interfaceNames);
            firstInit = true;
        }
        try {
            if (!forced) {
                if (FOLDER_NET_DEVICE.isDirectory()) {
                    File[] devices = FOLDER_NET_DEVICE.listFiles();
                    for (int i = 0; i < devices.length; i++) {
                        File device = devices[i];
                        if (device.isDirectory()) {
                            String name = device.getName();
                            String received = null;
                            String transmit = null;
                            String speed = null;
                            File[] deviceInfos = device.listFiles();
                            boolean file1 = false;
                            boolean file2 = false;
                            for (int j = 0; j < deviceInfos.length; j++) {
                                File deviceInfo = deviceInfos[j];
                                if (LOG.isDebugEnabled()) {
                                    LOG.debug("Current net file :" + deviceInfo.getAbsolutePath());
                                }
                                if (file1 && file2) {
                                    break;
                                }
                                if (deviceInfo.getName().equals("speed")) {
                                    if (indexNetBandwidth != -1) {
                                        try {
                                            BufferedReader br = new BufferedReader(new FileReader(deviceInfo));
                                            String line = br.readLine();
                                            speed = line;
                                            file1 = true;
                                            br.close();
                                        } catch (IOException e) {
                                            // This occurs for net devices which do not have speed (like lo)
                                        }
                                    } else {
                                        file1 = true;
                                    }
                                }
                                if (deviceInfo.getName().equals("statistics")) {
                                    boolean stat1 = false;
                                    boolean stat2 = false;
                                    if (deviceInfo.isDirectory()) {
                                        File[] statsFiles = deviceInfo.listFiles();
                                        for (int k = 0; k < statsFiles.length; k++) {
                                            File statsFile = statsFiles[k];
                                            if (stat1 && stat2) {
                                                break;
                                            }
                                            if (statsFile.getName().equals("rx_bytes")) {
                                                try {
                                                    BufferedReader br = new BufferedReader(new FileReader(statsFile));
                                                    String line = br.readLine();
                                                    received = line;
                                                    stat1 = true;
                                                    br.close();
                                                } catch (IOException e) {
                                                    LOG.error("", e);
                                                }
                                            }
                                            if (statsFile.getName().equals("tx_bytes")) {
                                                try {
                                                    BufferedReader br = new BufferedReader(new FileReader(statsFile));
                                                    String line = br.readLine();
                                                    transmit = line;
                                                    stat2 = true;
                                                    br.close();
                                                } catch (IOException e) {
                                                    LOG.error("", e);
                                                }
                                            }
                                        }
                                        file2 = true;
                                    }
                                }
                            }
                            if (firstInit) {
                                if (indexNetName != -1 && name != null && !netMonitoringExclusion.contains(name)) {
                                    interfaceNames.add(name);
                                    try {
                                        netsIo.add(new Long[] { new Long(received), new Long(transmit) });
                                    } catch (NumberFormatException e) {
                                        LOG.error("Net IO of " + name + " isn't valid (" + received + "," + transmit + ")", e);
                                    }
                                }
                                if (indexNetBandwidthUsable != -1) {
                                    Object maxRateNetIoValue = configuration.getValue(Configuration.Name.MAX_RATE_NET_IO/*
                                                                                                                         * ,
                                                                                                                         * Float
                                                                                                                         * .
                                                                                                                         * class
                                                                                                                         */);
                                    if (maxRateNetIoValue != null) {
                                        Object netBandwidthUsableValue = netBandwidthUsable.getValue();
                                        List bandwithUsable = new ArrayList();
                                        if (netBandwidthUsableValue != null && netBandwidthUsableValue instanceof List) {
                                            bandwithUsable = (List) netBandwidthUsableValue;
                                        } else {
                                            netBandwidthUsable.setValue(bandwithUsable);
                                        }
                                        for (int k = bandwithUsable.size(); k < interfaceNames.size(); k++) {
                                            bandwithUsable.add(maxRateNetIoValue);
                                        }
                                    } else {
                                        indicators.remove(Indicator.Name.NET_BANDWIDTH_USABLE);
                                    }
                                }
                            }
                            for (int k = 0; k < interfaceNames.size(); k++) {
                                String deviceName = (String) interfaceNames.get(k);
                                if (name.equals(deviceName)) {
                                    Long[] oldValue = (Long[]) netsIo.get(k);
                                    if (LOG.isDebugEnabled()) {
                                        LOG.debug("interface: " + name + ", oldValue:[" + oldValue[0].longValue() + ","
                                                + oldValue[1].longValue() + "]");
                                    }
                                    if (indexNetRecvIo != -1) {
                                        Object netRecvIoValue = netRecvIo.getValue();
                                        List interfaceRecvIo = new ArrayList();
                                        if (netRecvIoValue != null && netRecvIoValue instanceof List) {
                                            interfaceRecvIo = (List) netRecvIoValue;
                                        } else {
                                            netRecvIo.setValue(interfaceRecvIo);
                                        }
                                        for (int l = interfaceRecvIo.size(); l < interfaceNames.size(); l++) {
                                            interfaceRecvIo.add(null);
                                        }
                                        try {
                                            Long newValue = new Long(received);
                                            if (oldValue != null) {
                                                interfaceRecvIo
                                                        .set(k, new Float(
                                                                (newValue.longValue() - oldValue[0].longValue()) / 1024 / 60));
                                            }
                                        } catch (NumberFormatException e) {
                                            LOG.error("Interface Recv Io of " + name + " isn't valid (" + received + ")", e);
                                        }
                                    }
                                    if (indexNetSentIo != -1) {
                                        Object netSentIoValue = netSentIo.getValue();
                                        List interfaceSentIo = new ArrayList();
                                        if (netSentIoValue != null && netSentIoValue instanceof List) {
                                            interfaceSentIo = (List) netSentIoValue;
                                        } else {
                                            netSentIo.setValue(interfaceSentIo);
                                        }
                                        for (int l = interfaceSentIo.size(); l < interfaceNames.size(); l++) {
                                            interfaceSentIo.add(null);
                                        }
                                        try {
                                            Long newValue = new Long(transmit);
                                            if (oldValue != null) {
                                                // if (LOG.isInfoEnabled()) {
                                                // LOG.info("Number of writed block in 1 min: "+(newValue.intValue()-oldValue[1].intValue()));
                                                // }
                                                // if (LOG.isDebugEnabled()) {
                                                // LOG.debug("Setting writeIo index "+k+" with value :"+(newValue.intValue()-
                                                // oldValue[1].intValue())*blockSize.intValue()/1024/60);
                                                // }
                                                interfaceSentIo
                                                        .set(k, new Float(
                                                                (newValue.longValue() - oldValue[1].longValue()) / 1024 / 60));
                                            }
                                        } catch (NumberFormatException e) {
                                            LOG.error("Interface Sent Io of " + name + " isn't valid (" + transmit + ")", e);
                                        }
                                    }
                                    if (indexNetBandwidth != -1) {
                                        Object netBandwidthValue = netBandwidth.getValue();
                                        List bandwidth = new ArrayList();
                                        if (netBandwidthValue != null && netBandwidthValue instanceof List) {
                                            bandwidth = (List) netBandwidthValue;
                                        } else {
                                            netBandwidth.setValue(bandwidth);
                                        }
                                        for (int l = bandwidth.size(); l < interfaceNames.size(); l++) {
                                            bandwidth.add(null);
                                        }
                                        if (speed != null) {
                                            try {
                                                Float speedValue = new Float(speed);
                                                speedValue = new Float(speedValue.floatValue() * 1024);
                                                bandwidth.set(k, speedValue);
                                            } catch (NumberFormatException e) {
                                                LOG.error("Interface Recv Io of " + name + " isn't valid (" + received + ")", e);
                                            }
                                        }
                                    } else {
                                        indicators.remove(Indicator.Name.NET_BANDWIDTH);
                                    }
                                    oldValue[0] = new Long(received);
                                    oldValue[1] = new Long(transmit);
                                    if (LOG.isDebugEnabled()) {
                                        LOG.debug("interface: " + name + ", newValue:[" + oldValue[0].longValue() + ","
                                                + oldValue[1].longValue() + "]");
                                    }
                                    break;
                                }
                            }
                        }
                    }
                } else {
                    throw new IOException(FOLDER_NET_DEVICE + " is not a directory");
                }
            }
        } catch (IOException e) {
            LOG.error("", e);
            indicators.remove(Indicator.Name.NET_NAME);
            indicators.remove(Indicator.Name.NET_RECV_IO);
            indicators.remove(Indicator.Name.NET_SENT_IO);
            indicators.remove(Indicator.Name.NET_BANDWIDTH);
            indicators.remove(Indicator.Name.NET_BANDWIDTH_USABLE);
        }
    }

    /**
     * Updates the power category
     *
     * @param indicators
     * @param forced
     */
    private void updateCategoryPower(List/* <Indicator> */indicators, boolean forced) {
        Indicator batName = null, mainPwrOn = null, batPwrLevel = null, batRemainingTime = null;
        int indexBatName = Utils.getIndexOf(indicators, Indicator.Name.BAT_NAME), indexMainPwrOn = Utils.getIndexOf(indicators,
                Indicator.Name.MAIN_PWR_ON), indexBatPwrLevel = Utils.getIndexOf(indicators, Indicator.Name.BAT_PWR_LEVEL), indexBatRemainingTime = Utils
                .getIndexOf(indicators, Indicator.Name.BAT_REMAINING_TIME);
        if (indexBatName == -1 && indexMainPwrOn == -1 && indexBatPwrLevel == -1 && indexBatRemainingTime == -1) {
            LOG.error("Category Power : Indicators are not in the list");
            return;
        }
        if (indexBatName != -1) {
            batName = (Indicator) indicators.get(indexBatName);
        } else {
            batName = (Indicator) statics.get(Indicator.Name.BAT_NAME);
        }
        if (indexMainPwrOn != -1) {
            mainPwrOn = (Indicator) indicators.get(indexMainPwrOn);
        }
        if (indexBatPwrLevel != -1) {
            batPwrLevel = (Indicator) indicators.get(indexBatPwrLevel);
        }
        if (indexBatRemainingTime != -1) {
            batRemainingTime = (Indicator) indicators.get(indexBatRemainingTime);
        }
        boolean firstInit = false;
        List batteryNames = new ArrayList();
        Object batNameValue = batName.getValue();
        if (batNameValue != null && batNameValue instanceof List && ((List) batNameValue).size() > 0) {
            batteryNames = (List) batNameValue;
        } else {
            batName.setValue(batteryNames);
            firstInit = true;
        }
        try {
            if (indexMainPwrOn != -1) {
                if (FOLDER_PROC_ACPI_ADAPTER.isDirectory() && FOLDER_PROC_ACPI_ADAPTER.exists()) {
                    File[] adapters = FOLDER_PROC_ACPI_ADAPTER.listFiles();
                    if (adapters.length > 0) {
                        File adapter = adapters[0];
                        if (adapter.exists() && adapter.isDirectory()) {
                            File state = new File(adapter.getAbsolutePath() + File.separatorChar + "state");
                            if (state.exists()) {
                                BufferedReader br = new BufferedReader(new FileReader(state));
                                String read = br.readLine();
                                if (read != null) {
                                    int space = read.indexOf(' ');
                                    if (space != -1) {
                                        String power = read.substring(space).trim();
                                        mainPwrOn.setValue(new Boolean(power.equals("on-line")));
                                    }
                                }
                                br.close();
                            }
                        }
                    }
                }
            }
        } catch (IOException e) {
            LOG.error("", e);
            indicators.remove(Indicator.Name.MAIN_PWR_ON);
        }
        try {
            if (FOLDER_PROC_ACPI_BATTERY.isDirectory() && FOLDER_PROC_ACPI_BATTERY.exists()) {
                File[] batteries = FOLDER_PROC_ACPI_BATTERY.listFiles();
                for (int i = 0; i < batteries.length; i++) {
                    File battery = batteries[i];
                    if (battery.exists() && battery.isDirectory()) {
                        String name = battery.getName();
                        // String normalCapacity = null;
                        String maxCapacity = null;
                        String currentCapacity = null;
                        // String present = null;
                        String rate = null;
                        String state = null;
                        File stateFile = new File(battery.getAbsolutePath() + File.separatorChar + "state");
                        if (stateFile.exists()) {
                            BufferedReader br = new BufferedReader(new FileReader(stateFile));
                            String read;
                            int counter = 0;
                            while ((read = br.readLine()) != null) {
                                int dot = read.indexOf(':');
                                if (dot != -1) {
                                    String tmpVal = read.substring(dot + 1).trim();
                                    int space = tmpVal.indexOf(' ');
                                    if (space != -1) {
                                        tmpVal = tmpVal.substring(0, space);
                                    }
                                    // if(counter==0){
                                    // present = tmpVal;
                                    // }
                                    if (counter == 2) {
                                        state = tmpVal;
                                    }
                                    if (counter == 3) {
                                        rate = tmpVal;
                                    }
                                    if (counter == 4) {
                                        currentCapacity = tmpVal;
                                        break;
                                    }
                                }
                                counter++;
                            }
                            br.close();
                        }
                        File infoFile = new File(battery.getAbsolutePath() + File.separatorChar + "info");
                        if (infoFile.exists()) {
                            BufferedReader br = new BufferedReader(new FileReader(infoFile));
                            String read;
                            int counter = 0;
                            while ((read = br.readLine()) != null) {
                                int dot = read.indexOf(':');
                                if (dot != -1) {
                                    String tmpVal = read.substring(dot + 1).trim();
                                    int space = tmpVal.indexOf(' ');
                                    if (space != -1) {
                                        tmpVal = tmpVal.substring(0, space);
                                    }
                                    // if(counter==1){
                                    // normalCapacity = tmpVal;
                                    // }
                                    if (counter == 2) {
                                        maxCapacity = tmpVal;
                                        break;
                                    }
                                }
                                counter++;
                            }
                            br.close();
                        }
                        // System.out.println(name+"\t"+state+"\t"+normalCapacity+"\t"+maxCapacity+"\t"+currentCapacity+"\t"+rate+"\t"+present);
                        if (firstInit) {
                            // LOG.info("it's the first init");
                            if (indexBatName != -1 && name != null) {
                                batteryNames.add(name);
                            }
                        }
                        for (int j = 0; j < batteryNames.size(); j++) {
                            String batteryName = (String) batteryNames.get(j);
                            if (name.equals(batteryName)) {
                                // if (LOG.isDebugEnabled()) {
                                // LOG.debug("It's the disk :"+i+":"+name);
                                // }
                                if (indexBatPwrLevel != -1) {
                                    Object batPwrLevelValue = batPwrLevel.getValue();
                                    List batteriesPwrLevel = new ArrayList();
                                    if (batPwrLevelValue != null && batPwrLevelValue instanceof List) {
                                        batteriesPwrLevel = (List) batPwrLevelValue;
                                    } else {
                                        batPwrLevel.setValue(batteriesPwrLevel);
                                    }
                                    for (int k = batteriesPwrLevel.size(); k < batteryNames.size(); k++) {
                                        batteriesPwrLevel.add(null);
                                    }
                                    if (maxCapacity != null && currentCapacity != null) {
                                        try {
                                            batteriesPwrLevel.set(j,
                                                    new Float(Float.parseFloat(currentCapacity) / Float.parseFloat(maxCapacity)
                                                            * 100));
                                        } catch (NumberFormatException e) {
                                            LOG.error("", e);
                                        }
                                    }
                                }
                                if (indexBatRemainingTime != -1) {
                                    Object batRemainingTimeValue = batRemainingTime.getValue();
                                    List batteriesRemainingTime = new ArrayList();
                                    if (batRemainingTimeValue != null && batRemainingTimeValue instanceof List) {
                                        batteriesRemainingTime = (List) batRemainingTimeValue;
                                    } else {
                                        batRemainingTime.setValue(batteriesRemainingTime);
                                    }
                                    for (int k = batteriesRemainingTime.size(); k < batteryNames.size(); k++) {
                                        batteriesRemainingTime.add(null);
                                    }
                                    if (rate != null && currentCapacity != null && state != null && !state.equals("charged")) {
                                        float newRate = Float.parseFloat(rate);
                                        if (newRate != 0.0f) {
                                            float newCurrentCapacity = Float.parseFloat(currentCapacity);
                                            try {
                                                batteriesRemainingTime.set(j, FormatUtils.formatDuration(Math
                                                        .round((newCurrentCapacity / newRate) * 3600 * 1000)));
                                            } catch (NumberFormatException e) {
                                                LOG.error("", e);
                                            }
                                        }
                                    }
                                }
                                break;
                            }
                        }
                    }
                }
            }
        } catch (IOException e) {
            LOG.error("", e);
            indicators.remove(Indicator.Name.BAT_NAME);
            indicators.remove(Indicator.Name.BAT_PWR_LEVEL);
            indicators.remove(Indicator.Name.BAT_REMAINING_TIME);
        }
    }

    /**
     * Updates the temperature category
     *
     * @param indicators
     * @param forced
     */
    private void updateCategoryTemperature(List/* <Indicator> */indicators, boolean forced) {
        int indexTempCore = Utils.getIndexOf(indicators, Indicator.Name.TEMP_CORE);
        if (indexTempCore != -1) {
            Indicator tempCore = (Indicator) indicators.get(indexTempCore);
            List values = new ArrayList();
            if (FOLDER_TEMPERATURE.exists() && FOLDER_TEMPERATURE.isDirectory()) {
                File[] temps = FOLDER_TEMPERATURE.listFiles();
                for (int i = 0; i < temps.length; i++) {
                    File temp = temps[i];
                    String name = temp.getName();
                    if (name.startsWith("temp")) {
                        if (name.endsWith("_label")) {
                            try {
                                BufferedReader br = new BufferedReader(new FileReader(temp));
                                String line = br.readLine();
                                br.close();
                                if (line != null && line.startsWith("Core")) {
                                    int space = line.indexOf(' ');
                                    if (space != -1) {
                                        String coreNumber = line.substring(space + 1);
                                        String fileNumber = name.substring(4, name.lastIndexOf("_label"));
                                        File inputTemp = new File(FOLDER_TEMPERATURE.getAbsolutePath() + File.separatorChar
                                                + "temp" + fileNumber + "_input");
                                        br = new BufferedReader(new FileReader(inputTemp));
                                        String line2 = br.readLine();
                                        br.close();
                                        if (line2 != null) {
                                            try {
                                                int intCoreNumber = Integer.parseInt(coreNumber);
                                                while (values.size() <= intCoreNumber) {
                                                    values.add(null);
                                                }
                                                Float floatTemp = new Float(line2);
                                                values.set(intCoreNumber, new Float(floatTemp.floatValue() / 1000));
                                                // System.out.println("tempCore["+coreNumber+"] = "+line2);
                                            } catch (NumberFormatException e) {
                                                LOG.error("", e);
                                            }
                                        }
                                    }
                                }
                            } catch (IOException e) {
                                LOG.error("", e);
                            }
                        }
                    }
                }
            }
            tempCore.setValue(values);
        }
    }

    private String ntpServer = null;

    /**
     * Updates the time category
     *
     * @param indicators
     * @param forced
     */
    private void updateCategoryTime(List/* <Indicator> */indicators, boolean forced) {
        Indicator timeSync = null;
        int indexTimeSync = Utils.getIndexOf(indicators, Indicator.Name.TIME_SYNC);
        if (indexTimeSync != -1) {
            timeSync = (Indicator) indicators.get(indexTimeSync);
        } else {
            LOG.error("Category time : Indicator are not in the list");
            return;
        }
        try {
            if (FILE_ETC_NTP_CONF.exists()) {
                if (ntpServer == null) {
                    BufferedReader br = new BufferedReader(new FileReader(FILE_ETC_NTP_CONF));
                    String read = null;
                    while ((read = br.readLine()) != null) {
                        if (read.length() > 0) {
                            if (read.charAt(0) == '#') {
                                continue;
                            }
                            if (read.startsWith("server ")) {
                                String server = read.substring(7);
                                ntpServer = server;
                                break;
                            }
                        }
                    }
                    br.close();
                }
                if (ntpServer != null) {
                    String cmd = "ntpdate -q " + ntpServer;
                    Runtime runtime = Runtime.getRuntime();
                    Process process = runtime.exec(cmd);
                    process.waitFor();
                    if (process.exitValue() != 0) {
                        throw new Exception("Error occured during execution of command '" + cmd + "'. Status code:"
                                + process.exitValue());
                    }
                    String read = null;
                    BufferedReader br = new BufferedReader(new InputStreamReader(process.getInputStream()));
                    String lastLine = null;
                    while ((read = br.readLine()) != null) {
                        lastLine = read;
                    }
                    br.close();
                    lastLine = lastLine.trim();
                    int space;
                    boolean ready = false;
                    String value = null;
                    while ((space = lastLine.indexOf(' ')) != -1) {
                        lastLine = lastLine.substring(space).trim();
                        if (ready == true) {
                            space = lastLine.indexOf(' ');
                            if (space == -1) {
                                value = lastLine;
                            } else {
                                value = lastLine.substring(0, space);
                            }
                            break;
                        }
                        if (lastLine.startsWith("offset")) {
                            ready = true;
                        }
                    }
                    if (value != null) {
                        try {
                            float val = Float.parseFloat(value);
                            timeSync.setValue(new Boolean(val < 0.5f));
                        } catch (NumberFormatException e) {
                            LOG.error("Gap isn't a float (" + value + ")", e);
                        }
                    }
                }
            } else {
                if (FILE_ONG_NTP_CONF.exists()) {
                    if (ntpServer == null) {
                        BufferedReader br = new BufferedReader(new FileReader(FILE_ONG_NTP_CONF));
                        String read = null;
                        while ((read = br.readLine()) != null) {
                            if (read.startsWith("NTP_SERVER=")) {
                                String server = read.substring(11);
                                ntpServer = server;
                                break;
                            }
                        }
                        br.close();
                    }
                    if (ntpServer != null) {
                        Runtime runtime = Runtime.getRuntime();
                        String cmd = rootactPath + "bin/ntpclient -h " + ntpServer + " -s";
                        Process process = runtime.exec(cmd);
                        process.waitFor();
                        if (process.exitValue() != 0) {
                            throw new Exception("Error occured during execution of command '" + cmd + "'. Status code:"
                                    + process.exitValue());
                        }
                        BufferedReader br = new BufferedReader(new InputStreamReader(process.getInputStream()));
                        String read = br.readLine();
                        br.close();
                        String value = null;
                        if (read != null && read.length() > 0) {
                            read = read.trim();
                            int space;
                            int counter = 0;
                            while ((space = read.indexOf(' ')) != -1) {
                                read = read.substring(space).trim();
                                if (counter == 3) {
                                    space = read.indexOf(' ');
                                    if (space == -1) {
                                        value = read;
                                    } else {
                                        value = read.substring(0, space);
                                    }
                                    break;
                                }
                                counter++;
                            }
                        }
                        if (value != null) {
                            try {
                                float val = Float.parseFloat(value) / 1000 / 1000;
                                timeSync.setValue(new Boolean(val < 0.5f));
                            } catch (NumberFormatException e) {
                                LOG.error("Gap isn't a float (" + value + ")", e);
                            }
                        }
                    }
                } else {
                    throw new Exception("Impossible to determine " + Indicator.Name.TIME_SYNC);
                }
            }
        } catch (Exception e) {
            indicators.remove(Indicator.Name.TIME_SYNC);
            LOG.error("", e);
        }
    }

    /**
     * Updates the ong services category
     *
     * @param indicators
     * @param forced
     */
    private void updateCategoryOngServices(List/* <Indicator> */indicators, boolean forced) {
        Indicator srvName = null, srvStatus = null, srvPid = null, srvStartDate = null, srvNbRestarts = null;
        int indexSrvName = Utils.getIndexOf(indicators, Indicator.Name.SRV_NAME), indexSrvStatus = Utils.getIndexOf(indicators,
                Indicator.Name.SRV_STATUS), indexSrvPid = Utils.getIndexOf(indicators, Indicator.Name.SRV_PID), indexSrvStartDate = Utils
                .getIndexOf(indicators, Indicator.Name.SRV_START_DATE), indexSrvNbRestart = Utils.getIndexOf(indicators,
                Indicator.Name.SRV_NB_RESTARTS);
        if (indexSrvName == -1 && indexSrvStatus == -1 && indexSrvPid == -1 && indexSrvStartDate == -1
                && indexSrvNbRestart == -1) {
            LOG.error("Category ONG services : Indicators are not in the list");
            return;
        }
        if (indexSrvName != -1) {
            srvName = (Indicator) indicators.get(indexSrvName);
        } else {
            srvName = (Indicator) statics.get(Indicator.Name.SRV_NAME);
        }
        if (indexSrvStatus != -1) {
            srvStatus = (Indicator) indicators.get(indexSrvStatus);
        }
        if (indexSrvPid != -1) {
            srvPid = (Indicator) indicators.get(indexSrvPid);
        }
        if (indexSrvStartDate != -1) {
            srvStartDate = (Indicator) indicators.get(indexSrvStartDate);
        }
        if (indexSrvNbRestart != -1) {
            srvNbRestarts = (Indicator) indicators.get(indexSrvNbRestart);
        }
        boolean firstInit = false;
        List servicesNames = new ArrayList();
        Object srvNameValue = srvName.getValue();
        if (srvNameValue != null && srvNameValue instanceof List && ((List) srvNameValue).size() > 0) {
            servicesNames = (List) srvNameValue;
        } else {
            srvName.setValue(servicesNames);
            firstInit = true;
        }
        try {
            Runtime runtime = Runtime.getRuntime();
            Process process = runtime.exec(CMD_ONG_STATUS);
            process.waitFor();
            if (process.exitValue() != 0) {
                BufferedReader br = new BufferedReader(new InputStreamReader(process.getErrorStream()));
                String read = null;
                String result = "";
                while ((read = br.readLine()) != null) {
                    result += read + "\n";
                }
                br.close();
                throw new IOException("Error occured during execution of command '" + CMD_ONG_STATUS[2] + "'. Status code:"
                        + process.exitValue() + "\n Error:" + result);
            }
            String read = null;
            BufferedReader br = new BufferedReader(new InputStreamReader(process.getInputStream()));
            while ((read = br.readLine()) != null) {
                String serviceIs = " service is ";
                int posServiceIs = read.indexOf(serviceIs);
                if (posServiceIs != -1 && read.length() > posServiceIs + serviceIs.length()) {
                    String name = read.substring(0, posServiceIs);
                    String status = null;
                    int pid = -1;
                    Date start = null;
                    int nbRestart = -1;

                    int posOpen = read.indexOf('(');
                    if (posOpen != -1 && read.charAt(read.length() - 1) == ')') {
                        status = read.substring(posServiceIs + serviceIs.length(), posOpen - 1);
                        read = read.substring(posOpen + 1, read.length() - 1);
                        int posComma = read.indexOf(',');
                        // if(read.startsWith("name=")){
                        // name = read.substring("name=".length(), posComma);
                        // }
                        read = read.substring(posComma + 1);
                        posComma = read.indexOf(',');
                        if (read.startsWith("pid=")) {
                            try {
                                pid = Integer.parseInt(read.substring("pid=".length(), posComma));
                            } catch (NumberFormatException e) {
                                LOG.error("", e);
                            }
                        }
                        read = read.substring(posComma + 1);
                        posComma = read.indexOf(',');
                        if (read.startsWith("startDate=")) {
                            try {
                                start = FormatUtils.parseDateTime(read.substring("startDate=".length(), posComma));
                                // start = read.substring("startDate=".length(), posComma);
                            } catch (NumberFormatException e) {
                                LOG.error("", e);
                            } catch (ParseException e) {
                                LOG.error("", e);
                            }
                        }
                        read = read.substring(posComma + 1);
                        if (read.startsWith("nbRestarts=")) {
                            try {
                                nbRestart = Integer.parseInt(read.substring("nbRestarts=".length()));
                            } catch (NumberFormatException e) {
                                LOG.error("", e);
                            }
                        }
                    } else {
                        status = read.substring(posServiceIs + serviceIs.length());
                    }
                    //
                    // System.out.print("Name : "+name+"\t");
                    // System.out.print("Status : "+status+"\t");
                    // System.out.print("Pid : "+ pid+"\t");
                    // System.out.print("StartDate : "+ start+"\t");
                    // System.out.print("NbRestarts : "+nbRestart+"\n");
                    if (firstInit) {
                        if (indexSrvName != -1 && name != null) {
                            servicesNames.add(name);
                        }
                    }
                    for (int i = 0; i < servicesNames.size(); i++) {
                        String serviceName = (String) servicesNames.get(i);
                        if (name.equals(serviceName)) {
                            if (indexSrvStatus != -1) {
                                Object srvStatusValue = srvStatus.getValue();
                                List servicesStatus = new ArrayList();
                                if (srvStatusValue != null && srvStatusValue instanceof List) {
                                    servicesStatus = (List) srvStatusValue;
                                } else {
                                    srvStatus.setValue(servicesStatus);
                                }
                                for (int j = servicesStatus.size(); j < servicesNames.size(); j++) {
                                    servicesStatus.add(null);
                                }
                                if (LOG.isDebugEnabled()) {
                                    LOG.debug("Setting status index " + i + " with value :" + status);
                                }
                                servicesStatus.set(i, status);
                            }
                            if (indexSrvPid != -1) {
                                Object srvPidValue = srvPid.getValue();
                                List servicesPids = new ArrayList();
                                if (srvPidValue != null && srvPidValue instanceof List) {
                                    servicesPids = (List) srvPidValue;
                                } else {
                                    srvPid.setValue(servicesPids);
                                }
                                for (int j = servicesPids.size(); j < servicesNames.size(); j++) {
                                    servicesPids.add(null);
                                }
                                if (LOG.isDebugEnabled()) {
                                    LOG.debug("Setting pid index " + i + " with value :" + pid);
                                }
                                if (pid != -1) {
                                    servicesPids.set(i, new Integer(pid));
                                }
                            }
                            if (indexSrvStartDate != -1) {
                                Object srvStartDateValue = srvStartDate.getValue();
                                List servicesStartDates = new ArrayList();
                                if (srvStartDateValue != null && srvStartDateValue instanceof List) {
                                    servicesStartDates = (List) srvStartDateValue;
                                } else {
                                    srvStartDate.setValue(servicesStartDates);
                                }
                                for (int j = servicesStartDates.size(); j < servicesNames.size(); j++) {
                                    servicesStartDates.add(null);
                                }
                                if (LOG.isDebugEnabled()) {
                                    LOG.debug("Setting start date index " + i + " with value :" + start);
                                }
                                if (start != null) {
                                    servicesStartDates.set(i, FormatUtils.formatDateTime(start, UtilConstants.LOCAL_TIMEZONE));
                                }
                            }
                            if (indexSrvNbRestart != -1) {
                                Object srvNbRestartsValue = srvNbRestarts.getValue();
                                List servicesNbRestarts = new ArrayList();
                                if (srvNbRestartsValue != null && srvNbRestartsValue instanceof List) {
                                    servicesNbRestarts = (List) srvNbRestartsValue;
                                } else {
                                    srvNbRestarts.setValue(servicesNbRestarts);
                                }
                                for (int j = servicesNbRestarts.size(); j < servicesNames.size(); j++) {
                                    servicesNbRestarts.add(null);
                                }
                                if (LOG.isDebugEnabled()) {
                                    LOG.debug("Setting nb restart index " + i + " with value :" + nbRestart);
                                }
                                if (nbRestart != -1) {
                                    servicesNbRestarts.set(i, new Integer(nbRestart));
                                }
                            }
                            break;
                        }
                    }
                }
            }
            br.close();
        } catch (IOException e) {
            LOG.error("", e);
            indicators.remove(Indicator.Name.SRV_NAME);
            indicators.remove(Indicator.Name.SRV_STATUS);
            indicators.remove(Indicator.Name.SRV_PID);
            indicators.remove(Indicator.Name.SRV_START_DATE);
            indicators.remove(Indicator.Name.SRV_NB_RESTARTS);
        } catch (InterruptedException e) {
            LOG.error("", e);
            indicators.remove(Indicator.Name.SRV_NAME);
            indicators.remove(Indicator.Name.SRV_STATUS);
            indicators.remove(Indicator.Name.SRV_PID);
            indicators.remove(Indicator.Name.SRV_START_DATE);
            indicators.remove(Indicator.Name.SRV_NB_RESTARTS);
        }
    }

    /**
     * Gets the category of the given indicator
     *
     * @param indicator
     * @return
     */
    private static String getCategory(Indicator indicator) {
        String name = indicator.getName();
        if (name.equals(Indicator.Name.NUM_CPU)) {
            return CATEGORY_CPU;
        }
        if (name.equals(Indicator.Name.UPTIME)) {
            return CATEGORY_UPTIME;
        }
        if (name.equals(Indicator.Name.LOAD_AVG_PT1M) || name.equals(Indicator.Name.LOAD_AVG_PT5M)
                || name.equals(Indicator.Name.LOAD_AVG_PT15M) || name.equals(Indicator.Name.PROC_TOTAL)) {
            return CATEGORY_LOADAVG;
        }
        if (name.equals(Indicator.Name.MEM_USED) || name.equals(Indicator.Name.MEM_FREE)
                || name.equals(Indicator.Name.MEM_BUFFERS) || name.equals(Indicator.Name.MEM_CACHED)
                || name.equals(Indicator.Name.MEM_SWP_USED) || name.equals(Indicator.Name.MEM_SWP_FREE)
                || name.equals(Indicator.Name.MEM_TOTAL) || name.equals(Indicator.Name.MEM_SWP_TOTAL)) {
            return CATEGORY_MEMORY;
        }
        if (name.equals(Indicator.Name.DISK_PART_USED) || name.equals(Indicator.Name.DISK_PART_FREE)
                || name.equals(Indicator.Name.DISK_PART_SIZE) || name.equals(Indicator.Name.DISK_PART_NAME)) {
            return CATEGORY_DISK_SPACE;
        }
        if (name.equals(Indicator.Name.DISK_READ_IO) || name.equals(Indicator.Name.DISK_WRITE_IO)
                || name.equals(Indicator.Name.DISK_NAME)) {
            return CATEGORY_DISK_IO;
        }
        if (name.equals(Indicator.Name.NET_RECV_IO) || name.equals(Indicator.Name.NET_SENT_IO)
                || name.equals(Indicator.Name.NET_BANDWIDTH) || name.equals(Indicator.Name.NET_BANDWIDTH_USABLE)
                || name.equals(Indicator.Name.NET_NAME)) {
            return CATEGORY_NET_IO;
        }
        if (name.equals(Indicator.Name.MAIN_PWR_ON) || name.equals(Indicator.Name.BAT_PWR_LEVEL)
                || name.equals(Indicator.Name.BAT_REMAINING_TIME) || name.equals(Indicator.Name.BAT_NAME)) {
            return CATEGORY_POWER;
        }
        if (name.equals(Indicator.Name.TEMP_CORE)) {
            return CATEGORY_TEMPERATURE;
        }
        if (name.equals(Indicator.Name.TIME_SYNC)) {
            return CATEGORY_TIME;
        }
        if (name.equals(Indicator.Name.SRV_PID) || name.equals(Indicator.Name.SRV_STATUS)
                || name.equals(Indicator.Name.SRV_START_DATE) || name.equals(Indicator.Name.SRV_NB_RESTARTS)
                || name.equals(Indicator.Name.SRV_NAME)) {
            return CATEGORY_ONG_SERVICES;
        }
        return null;
    }
}
