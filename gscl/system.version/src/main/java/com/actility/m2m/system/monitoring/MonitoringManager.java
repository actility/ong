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

import java.io.Serializable;
import java.text.ParseException;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.Iterator;
import java.util.List;
import java.util.Map;

import org.apache.log4j.Level;
import org.apache.log4j.Logger;

import com.actility.m2m.system.config.Configuration;
import com.actility.m2m.system.log.BundleLogger;
import com.actility.m2m.system.monitoring.alarm.Alarm;
import com.actility.m2m.system.monitoring.alarm.AlarmExecution;
import com.actility.m2m.system.monitoring.alarm.AlarmExecutionException;
import com.actility.m2m.system.monitoring.alarm.AlarmExecutionResult;
import com.actility.m2m.system.monitoring.alarm.AlarmResult;
import com.actility.m2m.system.version.Utils;
import com.actility.m2m.util.FormatUtils;
import com.actility.m2m.util.log.OSGiLogger;
import com.actility.m2m.xo.XoException;
import com.actility.m2m.xo.XoObject;
import com.actility.m2m.xo.XoService;

public final class MonitoringManager implements Serializable {
    /**
     *
     */
    private static final long serialVersionUID = -5217513851094081299L;

    private static final Logger LOG = OSGiLogger.getLogger(MonitoringManager.class, BundleLogger.LOG);

    private final List/* <MonitoringListener */listeners;
    private final Map/* <Indicator> */indicators;
    private final Map/* <Indicator> */statics;
    private final List/* <Alarms> */alarms;
    private final SystemInformationManager infoManager;
    private final Configuration configuration;
    private final XoService xoService;

    /**
     * Constructor
     *
     * @param xoService
     * @param configuration
     * @param shellCmd
     */
    public MonitoringManager(XoService xoService, Configuration configuration, String shellCmd, String rootactPath) {
        this.xoService = xoService;
        this.configuration = configuration;
        this.listeners = new ArrayList();
        indicators = new HashMap();
        statics = new HashMap();
        alarms = new ArrayList();
        initIndicators();
        infoManager = new SystemInformationManager(configuration, statics, shellCmd, rootactPath);
        initAlarms();
    }

    /**
     * This method is called when a snapshot operation is doing
     *
     * @param indicatorNames
     * @return
     * @throws XoException
     */
    public byte[] snapshot(List/* <String> */indicatorNames) throws XoException {
        List indicatorsToUpdate = new ArrayList();
        List fixedIndicators = new ArrayList();
        for (int i = 0; i < indicatorNames.size(); i++) {
            Indicator indicator = (Indicator) indicators.get(indicatorNames.get(i));
            if (indicator != null) {
                if (indicator.getRefreshInterval() >= 0) {
                    indicatorsToUpdate.add(indicator);
                } else {
                    fixedIndicators.add(indicator);
                }
            }
        }
        infoManager.updateIndicators(indicatorsToUpdate, true);
        indicatorsToUpdate.addAll(fixedIndicators);
        if (indicatorsToUpdate.size() > 0) {
            return exportToContentInstance(indicatorsToUpdate);
        } else {
            return null;
        }
    }

    /**
     * This method is called each minutes to refresh alarms and indicators
     */
    public void onRefresh() {
        List/* <Indicator> */logIndicatorsToUpdate = new ArrayList();
        List/* <Indicator> */alarmIndicatorsToUpdate = new ArrayList();
        List/* <Alarm> */alarmsToExec = new ArrayList();
        Iterator it = indicators.keySet().iterator();
        while (it.hasNext()) {
            String key = (String) it.next();
            Indicator indicator = (Indicator) indicators.get(key);
            if (indicator.updateRefreshPosition()) {
                logIndicatorsToUpdate.add(indicator);
            }
        }
        for (int i = 0; i < alarms.size(); i++) {
            Alarm alarm = (Alarm) alarms.get(i);
            if (alarm.updateRefreshPosition()) {
                alarmsToExec.add(alarm);
                String[] indics = alarm.getTargetedIndicators();
                for (int j = 0; j < indics.length; j++) {
                    String indic = indics[j];
                    if (Utils.getIndexOf(logIndicatorsToUpdate, indic) == -1) {
                        Indicator indicator = (Indicator) indicators.get(indic);
                        if (indicator != null) {
                            alarmIndicatorsToUpdate.add(indicator);
                        }
                    }
                }
            }
        }
        infoManager.updateIndicators(logIndicatorsToUpdate);
        loggingReport(logIndicatorsToUpdate);
        infoManager.updateIndicators(alarmIndicatorsToUpdate, true);
        for (int i = 0; i < alarmsToExec.size(); i++) {
            Alarm alarm = (Alarm) alarmsToExec.get(i);
            Map/* <String,Indicator> */indicators = new HashMap();
            String[] indics = alarm.getTargetedIndicators();
            for (int j = 0; j < indics.length; j++) {
                String indic = indics[j];
                indicators.put(indic, this.indicators.get(indic));
            }
            List/* <AlarmResult> */results = alarm.executeMonitoring(configuration, indicators);
            for (int j = 0; j < results.size(); j++) {
                AlarmResult result = (AlarmResult) results.get(j);
                try {
                    byte[] content = Utils.generateAlarmResultContent(xoService, result);
                    sendReport("/m2m/applications/SYSTEM/containers/alarmLogs/contentInstances/", content);
                } catch (XoException e) {
                    LOG.error("An error occured during Xo generation", e);
                }
            }
        }
    }

    /**
     * Creates reports and sends them from given indicators
     *
     * @param indicators
     */
    private void loggingReport(List/* <Indicator> */indicators) {
        List highRateLogs = new ArrayList();
        List mediumRateLogs = new ArrayList();
        List lowRateLogs = new ArrayList();
        List resources = new ArrayList();
        for (int i = 0; i < indicators.size(); i++) {
            Indicator indicator = (Indicator) indicators.get(i);
            switch (indicator.getRefreshInterval()) {
            case -1:
                resources.add(indicator);
                break;
            case 1:
                highRateLogs.add(indicator);
                break;
            case 15:
                mediumRateLogs.add(indicator);
                break;
            case 180:
                lowRateLogs.add(indicator);
                break;
            default:
                LOG.error("The indicator " + indicator.getName() + " has not logging container (interval="
                        + indicator.getRefreshInterval() + ")");
                break;
            }
        }
        try {
            if (resources.size() > 0) {
                byte[] resourcesContent = exportToContentInstance(resources);
                if (resourcesContent != null) {
                    sendReport("/m2m/applications/SYSTEM/containers/resources/contentInstances/", resourcesContent);
                }
            }
            if (highRateLogs.size() > 0) {
                byte[] highRateLogsContent = exportToContentInstance(highRateLogs);
                if (highRateLogsContent != null) {
                    sendReport("/m2m/applications/SYSTEM/containers/highRateLogs/contentInstances/", highRateLogsContent);
                }
            }
            if (mediumRateLogs.size() > 0) {
                byte[] mediumRateLogsContent = exportToContentInstance(mediumRateLogs);
                if (mediumRateLogsContent != null) {
                    sendReport("/m2m/applications/SYSTEM/containers/mediumRateLogs/contentInstances/", mediumRateLogsContent);
                }
            }
            if (lowRateLogs.size() > 0) {
                byte[] lowRateLogsContent = exportToContentInstance(lowRateLogs);
                if (lowRateLogsContent != null) {
                    sendReport("/m2m/applications/SYSTEM/containers/lowRateLogs/contentInstances/", lowRateLogsContent);
                }
            }
        } catch (XoException e) {
            LOG.error("An error occured during Xo generation", e);
        }
    }

    /**
     * Transforms indicators in a readable report
     *
     * @param indicators
     * @return
     * @throws XoException
     */
    private byte[] exportToContentInstance(List/* <Indicator> */indicators) throws XoException {
        int nbModification = 0;
        byte[] result = null;
        XoObject xoObj = xoService.newObixXoObject("o:obj");
        try {
            List childs = xoObj.getXoObjectListAttribute("[]");
            for (int i = 0; i < indicators.size(); i++) {
                Indicator indicator = (Indicator) (indicators.get(i));
                if (indicator.getValue() == null) {
                    if (LOG.isEnabledFor(Level.WARN)) {
                        LOG.warn("Value of indicator " + indicator.getName() + " is null !");
                    }
                    continue;
                }
                String type = indicator.getType();
                if (type.startsWith("list<") && type.endsWith(">")) {
                    if (type.length() > 6) {
                        String subType = type.substring(5, type.length() - 1);
                        if (indicator.getValue() instanceof List) {
                            List list = (List) indicator.getValue();
                            for (int j = 0; j < list.size(); j++) {
                                Object subValue = list.get(j);
                                if (subValue != null) {
                                    childs.add(exportObjToObix(subType, indicator.getName() + "." + j, subValue));
                                    nbModification++;
                                }
                            }
                        } else {
                            LOG.error("Generating obix object : value of indicator " + indicator.getName()
                                    + /*
                                       * ); LOG.error(
                                       */"isn't type a list (" + indicator.getValue().getClass() + ")");
                        }
                    } else {
                        LOG.error("Indicator type isn't valid :" + type);
                    }
                } else {
                    childs.add(exportObjToObix(type, indicator.getName(), indicator.getValue()));
                    nbModification++;
                }
            }
            if (nbModification > 0) {
                result = xoObj.saveXml();
            }
        } finally {
            Utils.releaseXoObjectFromMemory(xoObj);
        }
        return result;
    }

    /**
     * Transforms a value to a obix object
     *
     * @param type
     * @param name
     * @param value
     * @return
     * @throws XoException
     */
    private XoObject exportObjToObix(String type, String name, Object value) throws XoException {
        if (type.equals("bool") || type.equals("int") || type.equals("real") || type.equals("str") || type.equals("enum")
                || type.equals("uri") || type.equals("abstime") || type.equals("reltime") || type.equals("date")
                || type.equals("time")) {
            XoObject xoObject = xoService.newObixXoObject("o:" + type);
            xoObject.setStringAttribute("name", name);
            xoObject.setStringAttribute("val", value.toString());
            return xoObject;
        } else {
            throw new XoException("The type " + type + " isn't a valid obix type");
        }
    }

    /**
     * Sends a report to all listeners
     *
     * @param target
     * @param content
     */
    private void sendReport(String target, byte[] content) {
        for (int i = 0; i < listeners.size(); i++) {
            MonitoringListener listener = (MonitoringListener) listeners.get(i);
            listener.sendMonitoringReport(target, content);
        }
    }

    /**
     * Adds a monitoring listener
     *
     * @param listener
     */
    public void addListener(MonitoringListener listener) {
        listeners.add(listener);
    }

    /**
     * Initializes all indicators
     */
    private void initIndicators() {
        LOG.debug("Indicators initialization");
        // indicators.put(Indicator.Name.CPU, new Indicator(Indicator.Name.CPU, , ));
        indicators.put(Indicator.Name.UPTIME, new Indicator(Indicator.Name.UPTIME, "reltime", 0));
        indicators.put(Indicator.Name.LOAD_AVG_PT1M, new Indicator(Indicator.Name.LOAD_AVG_PT1M, "real", 1));
        indicators.put(Indicator.Name.LOAD_AVG_PT5M, new Indicator(Indicator.Name.LOAD_AVG_PT5M, "real", 0));
        indicators.put(Indicator.Name.LOAD_AVG_PT15M, new Indicator(Indicator.Name.LOAD_AVG_PT15M, "real", 15));
        indicators.put(Indicator.Name.PROC_TOTAL, new Indicator(Indicator.Name.PROC_TOTAL, "int", 0));
        indicators.put(Indicator.Name.MEM_USED, new Indicator(Indicator.Name.MEM_USED, "int", 0));
        indicators.put(Indicator.Name.MEM_FREE, new Indicator(Indicator.Name.MEM_FREE, "int", 180));
        indicators.put(Indicator.Name.MEM_BUFFERS, new Indicator(Indicator.Name.MEM_BUFFERS, "int", 0));
        indicators.put(Indicator.Name.MEM_CACHED, new Indicator(Indicator.Name.MEM_CACHED, "int", 0));
        indicators.put(Indicator.Name.MEM_SWP_USED, new Indicator(Indicator.Name.MEM_SWP_USED, "int", 0));
        indicators.put(Indicator.Name.MEM_SWP_FREE, new Indicator(Indicator.Name.MEM_SWP_FREE, "int", 180));
        indicators.put(Indicator.Name.DISK_PART_USED, new Indicator(Indicator.Name.DISK_PART_USED, "list<int>", 0));
        indicators.put(Indicator.Name.DISK_PART_FREE, new Indicator(Indicator.Name.DISK_PART_FREE, "list<int>", 180));
        indicators.put(Indicator.Name.DISK_READ_IO, new Indicator(Indicator.Name.DISK_READ_IO, "list<real>", 1));
        indicators.put(Indicator.Name.DISK_WRITE_IO, new Indicator(Indicator.Name.DISK_WRITE_IO, "list<real>", 1));
        indicators.put(Indicator.Name.NET_RECV_IO, new Indicator(Indicator.Name.NET_RECV_IO, "list<real>", 1));
        indicators.put(Indicator.Name.NET_SENT_IO, new Indicator(Indicator.Name.NET_SENT_IO, "list<real>", 1));
        indicators.put(Indicator.Name.MAIN_PWR_ON, new Indicator(Indicator.Name.MAIN_PWR_ON, "bool", 0));
        indicators.put(Indicator.Name.BAT_PWR_LEVEL, new Indicator(Indicator.Name.BAT_PWR_LEVEL, "list<real>", 180));
        indicators.put(Indicator.Name.BAT_REMAINING_TIME,
                new Indicator(Indicator.Name.BAT_REMAINING_TIME, "list<reltime>", 180));
        indicators.put(Indicator.Name.TEMP_CORE, new Indicator(Indicator.Name.TEMP_CORE, "list<real>", 180));
        // indicators.put(Indicator.Name.TEMP_FRAME, new Indicator(Indicator.Name.TEMP_FRAME, , ));
        indicators.put(Indicator.Name.TIME_SYNC, new Indicator(Indicator.Name.TIME_SYNC, "bool", 180));
        // indicators.put(Indicator.Name.LRR_LINK_STATE, new Indicator(Indicator.Name.LRR_LINK_STATE, , ));
        indicators.put(Indicator.Name.SRV_STATUS, new Indicator(Indicator.Name.SRV_STATUS, "list<str>", 0));
        indicators.put(Indicator.Name.SRV_PID, new Indicator(Indicator.Name.SRV_PID, "list<int>", 0));
        indicators.put(Indicator.Name.SRV_START_DATE, new Indicator(Indicator.Name.SRV_START_DATE, "list<abstime>", 0));
        indicators.put(Indicator.Name.SRV_NB_RESTARTS, new Indicator(Indicator.Name.SRV_NB_RESTARTS, "list<int>", 0));
        // STATIC INDICATOR
        Indicator numCpu = new Indicator(Indicator.Name.NUM_CPU, "int", -1);
        Indicator memTotal = new Indicator(Indicator.Name.MEM_TOTAL, "int", -1);
        Indicator memSwpTotal = new Indicator(Indicator.Name.MEM_SWP_TOTAL, "int", -1);
        Indicator diskPartSize = new Indicator(Indicator.Name.DISK_PART_SIZE, "list<int>", -1);
        Indicator diskPartName = new Indicator(Indicator.Name.DISK_PART_NAME, "list<str>", -1);
        Indicator diskName = new Indicator(Indicator.Name.DISK_NAME, "list<str>", -1);
        Indicator netName = new Indicator(Indicator.Name.NET_NAME, "list<str>", -1);
        Indicator netBandWidth = new Indicator(Indicator.Name.NET_BANDWIDTH, "list<real>", -1);
        Indicator netBandWidthUsable = new Indicator(Indicator.Name.NET_BANDWIDTH_USABLE, "list<real>", -1);
        Indicator srvName = new Indicator(Indicator.Name.SRV_NAME, "list<str>", -1);
        Indicator batName = new Indicator(Indicator.Name.BAT_NAME, "list<str>", -1);
        indicators.put(Indicator.Name.NUM_CPU, numCpu);
        statics.put(Indicator.Name.NUM_CPU, numCpu);
        indicators.put(Indicator.Name.MEM_TOTAL, memTotal);
        statics.put(Indicator.Name.MEM_TOTAL, memTotal);
        indicators.put(Indicator.Name.MEM_SWP_TOTAL, memSwpTotal);
        statics.put(Indicator.Name.MEM_SWP_TOTAL, memSwpTotal);
        indicators.put(Indicator.Name.DISK_PART_SIZE, diskPartSize);
        statics.put(Indicator.Name.DISK_PART_SIZE, diskPartSize);
        indicators.put(Indicator.Name.DISK_PART_NAME, diskPartName);
        statics.put(Indicator.Name.DISK_PART_NAME, diskPartName);
        indicators.put(Indicator.Name.DISK_NAME, diskName);
        statics.put(Indicator.Name.DISK_NAME, diskName);
        indicators.put(Indicator.Name.NET_NAME, netName);
        statics.put(Indicator.Name.NET_NAME, netName);
        indicators.put(Indicator.Name.NET_BANDWIDTH, netBandWidth);
        statics.put(Indicator.Name.NET_BANDWIDTH, netBandWidth);
        indicators.put(Indicator.Name.NET_BANDWIDTH_USABLE, netBandWidthUsable);
        statics.put(Indicator.Name.NET_BANDWIDTH_USABLE, netBandWidthUsable);
        indicators.put(Indicator.Name.SRV_NAME, srvName);
        statics.put(Indicator.Name.SRV_NAME, srvName);
        indicators.put(Indicator.Name.BAT_NAME, batName);
        statics.put(Indicator.Name.BAT_NAME, batName);
    }

    /**
     * Initializes all alarms
     */
    private void initAlarms() {
        LOG.debug("Alarms initialization");
        alarms.add(new Alarm(Alarm.Name.HIGH_LOAD_AVG_THRESHOLD, new String[] { Indicator.Name.LOAD_AVG_PT15M,
                Indicator.Name.NUM_CPU }, new AlarmExecution() {
            public List/* <AlarmExecutionResult> */execute(Configuration configuration, Map indicators)
                    throws AlarmExecutionException {
                List results = new ArrayList();
                Indicator loadAvg = (Indicator) indicators.get(Indicator.Name.LOAD_AVG_PT15M);
                if (loadAvg == null) {
                    throw new AlarmExecutionException("Indicator " + Indicator.Name.LOAD_AVG_PT15M + " is null",
                            AlarmExecutionException.LEVEL_ERROR);
                }
                Object loadAvgValue = loadAvg.getValue();
                if (loadAvgValue == null || !(loadAvgValue instanceof Float)) {
                    throw new AlarmExecutionException("Value of indicator " + Indicator.Name.LOAD_AVG_PT15M
                            + " is null or is not a Float", AlarmExecutionException.LEVEL_ERROR);
                }
                float load = ((Float) loadAvgValue).floatValue();

                Indicator numCpu = (Indicator) indicators.get(Indicator.Name.NUM_CPU);
                if (numCpu == null) {
                    throw new AlarmExecutionException("Indicator " + Indicator.Name.NUM_CPU + " is null",
                            AlarmExecutionException.LEVEL_ERROR);
                }
                Object numCpuValue = numCpu.getValue();
                if (numCpuValue == null || !(numCpuValue instanceof Integer)) {
                    throw new AlarmExecutionException("Value of indicator " + Indicator.Name.NUM_CPU
                            + " is null or is not a Integer", AlarmExecutionException.LEVEL_ERROR);
                }
                int ncpu = ((Integer) numCpuValue).intValue();

                Float alarmLoadAvgThresholdCrit = (Float) configuration
                        .getValue(Configuration.Name.ALARM_LOAD_AVG_THRESHOLD_CRIT/* , Float.class */);
                if (alarmLoadAvgThresholdCrit == null) {
                    throw new AlarmExecutionException("Configuration value " + Configuration.Name.ALARM_LOAD_AVG_THRESHOLD_CRIT
                            + " cannot be null", AlarmExecutionException.LEVEL_ERROR);
                }

                Float alarmLoadAvgThresholdMaj = (Float) configuration
                        .getValue(Configuration.Name.ALARM_LOAD_AVG_THRESHOLD_MAJ/* , Float.class */);
                if (alarmLoadAvgThresholdMaj == null) {
                    throw new AlarmExecutionException("Configuration value " + Configuration.Name.ALARM_LOAD_AVG_THRESHOLD_MAJ
                            + " cannot be null", AlarmExecutionException.LEVEL_ERROR);
                }

                if (load > alarmLoadAvgThresholdCrit.floatValue() * ncpu) {
                    results.add(new AlarmExecutionResult(Alarm.ALARM_STATE_CRITICAL, "Load average is too high: " + load));
                } else if (load > alarmLoadAvgThresholdMaj.floatValue() * ncpu) {
                    results.add(new AlarmExecutionResult(Alarm.ALARM_STATE_MAJOR, "Load average is too high: " + load));
                } else if (load <= alarmLoadAvgThresholdMaj.floatValue() * ncpu) {
                    results.add(new AlarmExecutionResult(Alarm.ALARM_STATE_CLEAR));
                }
                return results;
            }
        }, 5, 60));
        alarms.add(new Alarm(Alarm.Name.HIGH_RAM_THRESHOLD, new String[] { Indicator.Name.MEM_FREE, Indicator.Name.MEM_TOTAL },
                new AlarmExecution() {
                    public List execute(Configuration configuration, Map indicators) throws AlarmExecutionException {
                        List results = new ArrayList();
                        Indicator memFree = (Indicator) indicators.get(Indicator.Name.MEM_FREE);
                        if (memFree == null) {
                            throw new AlarmExecutionException("Indicator " + Indicator.Name.MEM_FREE + " is null",
                                    AlarmExecutionException.LEVEL_ERROR);
                        }
                        Object memFreeValue = memFree.getValue();
                        if (memFreeValue == null || !(memFreeValue instanceof Integer)) {
                            throw new AlarmExecutionException("Value of indicator " + Indicator.Name.MEM_FREE
                                    + " is null or is not an Integer", AlarmExecutionException.LEVEL_ERROR);
                        }
                        int free = ((Integer) memFreeValue).intValue();
                        Indicator memTotal = (Indicator) indicators.get(Indicator.Name.MEM_TOTAL);
                        if (memTotal == null) {
                            throw new AlarmExecutionException("Indicator " + Indicator.Name.MEM_TOTAL + " is null",
                                    AlarmExecutionException.LEVEL_ERROR);
                        }
                        Object memTotalValue = memTotal.getValue();
                        if (memTotalValue == null || !(memTotalValue instanceof Integer)) {
                            throw new AlarmExecutionException("Value of indicator " + Indicator.Name.MEM_TOTAL
                                    + " is null or is not an Integer", AlarmExecutionException.LEVEL_ERROR);
                        }
                        int total = ((Integer) memTotalValue).intValue();
                        Float alarmRamThresholdCrit = (Float) configuration
                                .getValue(Configuration.Name.ALARM_RAM_THRESHOLD_CRIT/* , Float.class */);
                        if (alarmRamThresholdCrit == null) {
                            throw new AlarmExecutionException("Configuration value "
                                    + Configuration.Name.ALARM_RAM_THRESHOLD_CRIT + " cannot be null",
                                    AlarmExecutionException.LEVEL_ERROR);
                        }
                        Float alarmRamThresholdMaj = (Float) configuration
                                .getValue(Configuration.Name.ALARM_RAM_THRESHOLD_MAJ/* , Float.class */);
                        if (alarmRamThresholdMaj == null) {
                            throw new AlarmExecutionException("Configuration value "
                                    + Configuration.Name.ALARM_RAM_THRESHOLD_MAJ + " cannot be null",
                                    AlarmExecutionException.LEVEL_ERROR);
                        }
                        float result = (float) free / total;
                        if (result < alarmRamThresholdCrit.floatValue()) {
                            results.add(new AlarmExecutionResult(Alarm.ALARM_STATE_CRITICAL, "There are only " + free
                                    + "KB of free ram"));
                        } else if (result < alarmRamThresholdMaj.floatValue()) {
                            results.add(new AlarmExecutionResult(Alarm.ALARM_STATE_MAJOR, "There are only " + free
                                    + "KB of free ram"));
                        } else if (result >= alarmRamThresholdMaj.floatValue()) {
                            results.add(new AlarmExecutionResult(Alarm.ALARM_STATE_CLEAR));
                        }
                        return results;
                    }
                }, 15, 60));
        alarms.add(new Alarm(Alarm.Name.HIGH_DISK_THRESHOLD, new String[] { Indicator.Name.DISK_PART_FREE,
                Indicator.Name.DISK_PART_SIZE }, new AlarmExecution() {
            public List execute(Configuration configuration, Map indicators) throws AlarmExecutionException {
                List results = new ArrayList();
                Indicator diskPartFree = (Indicator) indicators.get(Indicator.Name.DISK_PART_FREE);
                if (diskPartFree == null) {
                    throw new AlarmExecutionException("Indicator " + Indicator.Name.DISK_PART_FREE + " is null",
                            AlarmExecutionException.LEVEL_ERROR);
                }
                Object diskPartFreeValue = diskPartFree.getValue();
                if (diskPartFreeValue == null || !(diskPartFreeValue instanceof List)) {
                    throw new AlarmExecutionException("Value of indicator " + Indicator.Name.DISK_PART_FREE
                            + " is null or is not a List", AlarmExecutionException.LEVEL_ERROR);
                }
                List partFree = ((List) diskPartFreeValue);
                Indicator diskPartSize = (Indicator) indicators.get(Indicator.Name.DISK_PART_SIZE);
                if (diskPartSize == null) {
                    throw new AlarmExecutionException("Indicator " + Indicator.Name.DISK_PART_SIZE + " is null",
                            AlarmExecutionException.LEVEL_ERROR);
                }
                Object diskPartSizeValue = diskPartSize.getValue();
                if (diskPartSizeValue == null || !(diskPartSizeValue instanceof List)) {
                    throw new AlarmExecutionException("Value of indicator " + Indicator.Name.DISK_PART_SIZE
                            + " is null or is not a List", AlarmExecutionException.LEVEL_ERROR);
                }
                List partSize = ((List) diskPartSizeValue);
                if (partFree.size() != partSize.size()) {
                    throw new AlarmExecutionException("List size of indicator" + Indicator.Name.DISK_PART_FREE + "("
                            + partFree.size() + ") is different of list size of indicator" + Indicator.Name.DISK_PART_SIZE
                            + "(" + partSize.size() + ")", AlarmExecutionException.LEVEL_ERROR);
                }
                Float alarmDiskThresholdCrit = (Float) configuration.getValue(Configuration.Name.ALARM_DISK_THRESHOLD_CRIT/*
                                                                                                                           * ,
                                                                                                                           * Float
                                                                                                                           * .
                                                                                                                           * class
                                                                                                                           */);
                if (alarmDiskThresholdCrit == null) {
                    throw new AlarmExecutionException("Configuration value " + Configuration.Name.ALARM_DISK_THRESHOLD_CRIT
                            + " cannot be null", AlarmExecutionException.LEVEL_ERROR);
                }
                Float alarmDiskThresholdMaj = (Float) configuration.getValue(Configuration.Name.ALARM_DISK_THRESHOLD_MAJ/*
                                                                                                                         * ,
                                                                                                                         * Float
                                                                                                                         * .
                                                                                                                         * class
                                                                                                                         */);
                if (alarmDiskThresholdMaj == null) {
                    throw new AlarmExecutionException("Configuration value " + Configuration.Name.ALARM_DISK_THRESHOLD_MAJ
                            + " cannot be null", AlarmExecutionException.LEVEL_ERROR);
                }
                for (int i = 0; i < partFree.size(); i++) {
                    Object partFreeValue = partFree.get(i);
                    if (partFreeValue == null || !(partFreeValue instanceof Integer)) {
                        throw new AlarmExecutionException("Value of indicator " + Indicator.Name.DISK_PART_FREE + "[" + i
                                + "] is null or is not an Integer", AlarmExecutionException.LEVEL_ERROR);
                    }
                    int free = ((Integer) partFreeValue).intValue();
                    Object partSizeValue = partSize.get(i);
                    if (partSizeValue == null || !(partSizeValue instanceof Integer)) {
                        throw new AlarmExecutionException("Value of indicator " + Indicator.Name.DISK_PART_SIZE + "[" + i
                                + "] is null or is not an Integer", AlarmExecutionException.LEVEL_ERROR);
                    }
                    int size = ((Integer) partSizeValue).intValue();
                    float result = (float) free / size;
                    if (result < alarmDiskThresholdCrit.floatValue()) {
                        results.add(new AlarmExecutionResult(Alarm.ALARM_STATE_CRITICAL, "There are only " + free
                                + "KB of free space on disk partition", i));
                    } else if (result < alarmDiskThresholdMaj.floatValue()) {
                        results.add(new AlarmExecutionResult(Alarm.ALARM_STATE_MAJOR, "There are only " + free
                                + "KB of free space on disk partition", i));
                    } else if (result >= alarmDiskThresholdMaj.floatValue()) {
                        results.add(new AlarmExecutionResult(Alarm.ALARM_STATE_CLEAR, i));
                    }
                }
                return results;
            }
        }, 15, 60));
        alarms.add(new Alarm(Alarm.Name.HIGH_DISK_IO_THRESHOLD, new String[] { Indicator.Name.DISK_READ_IO,
                Indicator.Name.DISK_WRITE_IO }, new AlarmExecution() {
            public List execute(Configuration configuration, Map indicators) throws AlarmExecutionException {
                List results = new ArrayList();
                Float alarmDiskIoThresholdMaj = (Float) configuration
                        .getValue(Configuration.Name.ALARM_DISK_IO_THRESHOLD_MAJ/* , Float.class */);
                if (alarmDiskIoThresholdMaj == null) {
                    throw new AlarmExecutionException("Configuration value " + Configuration.Name.ALARM_DISK_IO_THRESHOLD_MAJ
                            + " cannot be null", AlarmExecutionException.LEVEL_CLEAR);
                }
                Float alarmDiskIoThresholdCrit = (Float) configuration
                        .getValue(Configuration.Name.ALARM_DISK_IO_THRESHOLD_CRIT/* , Float.class */);
                if (alarmDiskIoThresholdCrit == null) {
                    throw new AlarmExecutionException("Configuration value " + Configuration.Name.ALARM_DISK_IO_THRESHOLD_CRIT
                            + " cannot be null", AlarmExecutionException.LEVEL_CLEAR);
                }
                Float maxRateDiskIo = (Float) configuration.getValue(Configuration.Name.MAX_RATE_DISK_IO/* , Float.class */);
                if (maxRateDiskIo == null) {
                    throw new AlarmExecutionException("Configuration value " + Configuration.Name.MAX_RATE_DISK_IO
                            + " cannot be null", AlarmExecutionException.LEVEL_CLEAR);
                }
                Indicator diskReadIo = (Indicator) indicators.get(Indicator.Name.DISK_READ_IO);
                if (diskReadIo == null) {
                    throw new AlarmExecutionException("Indicator " + Indicator.Name.DISK_READ_IO + " is null",
                            AlarmExecutionException.LEVEL_ERROR);
                }
                Object diskReadIoValue = diskReadIo.getValue();
                if (diskReadIoValue == null || !(diskReadIoValue instanceof List)) {
                    throw new AlarmExecutionException("Value of indicator " + Indicator.Name.DISK_READ_IO
                            + " is null or is not a List", AlarmExecutionException.LEVEL_ERROR);
                }
                List readsIo = ((List) diskReadIoValue);
                Indicator diskWriteIo = (Indicator) indicators.get(Indicator.Name.DISK_WRITE_IO);
                if (diskWriteIo == null) {
                    throw new AlarmExecutionException("Indicator " + Indicator.Name.DISK_WRITE_IO + " is null",
                            AlarmExecutionException.LEVEL_ERROR);
                }
                Object diskWriteIoValue = diskWriteIo.getValue();
                if (diskWriteIoValue == null || !(diskWriteIoValue instanceof List)) {
                    throw new AlarmExecutionException("Value of indicator " + Indicator.Name.DISK_WRITE_IO
                            + " is null or is not a List", AlarmExecutionException.LEVEL_ERROR);
                }
                List writesIo = ((List) diskWriteIoValue);
                if (writesIo.size() != readsIo.size()) {
                    throw new AlarmExecutionException("List size of indicator" + Indicator.Name.DISK_WRITE_IO + "("
                            + writesIo.size() + ") is different of list size of indicator" + Indicator.Name.DISK_READ_IO + "("
                            + readsIo.size() + ")", AlarmExecutionException.LEVEL_ERROR);
                }
                for (int i = 0; i < writesIo.size(); i++) {
                    Object writeIoValue = writesIo.get(i);
                    if (writeIoValue == null || !(writeIoValue instanceof Float)) {
                        throw new AlarmExecutionException("Value of indicator " + Indicator.Name.DISK_WRITE_IO + "[" + i
                                + "] is null or is not a Float", AlarmExecutionException.LEVEL_ERROR);
                    }
                    float writeIo = ((Float) writeIoValue).floatValue();
                    Object readIoValue = readsIo.get(i);
                    if (readIoValue == null || !(readIoValue instanceof Float)) {
                        throw new AlarmExecutionException("Value of indicator " + Indicator.Name.DISK_READ_IO + "[" + i
                                + "] is null or is not a Float", AlarmExecutionException.LEVEL_ERROR);
                    }
                    float readIo = ((Float) readIoValue).floatValue();
                    if (readIo / maxRateDiskIo.floatValue() > alarmDiskIoThresholdCrit.floatValue()
                            || writeIo / maxRateDiskIo.floatValue() > alarmDiskIoThresholdCrit.floatValue()) {
                        results.add(new AlarmExecutionResult(Alarm.ALARM_STATE_CRITICAL, i));
                    } else if (readIo / maxRateDiskIo.floatValue() > alarmDiskIoThresholdMaj.floatValue()
                            || writeIo / maxRateDiskIo.floatValue() > alarmDiskIoThresholdMaj.floatValue()) {
                        results.add(new AlarmExecutionResult(Alarm.ALARM_STATE_MAJOR, i));
                    } else if (readIo / maxRateDiskIo.floatValue() <= alarmDiskIoThresholdMaj.floatValue()
                            || writeIo / maxRateDiskIo.floatValue() <= alarmDiskIoThresholdMaj.floatValue()) {
                        results.add(new AlarmExecutionResult(Alarm.ALARM_STATE_CLEAR, i));
                    }
                }
                return results;
            }
        }, 15, 60));
        // alarms.add(new Alarm(Alarm.Name.DISK_FAILURE, new String[]{Indicator.Name.DISK_NAME}, new AlarmExecution() {
        // private String CMD_SMARTCTL = "smartctl -H ";
        // public List execute(Configuration configuration, Map indicators) throws AlarmExecutionException {
        // List results = new ArrayList();
        //
        // Indicator diskName = (Indicator) indicators.get(Indicator.Name.DISK_NAME);
        // if(diskName==null){
        // throw new AlarmExecutionException("Indicator "+Indicator.Name.DISK_NAME+" is null");
        // }
        // Object diskNameValue = diskName.getValue();
        // if(diskNameValue==null || !(diskNameValue instanceof List)){
        // throw new AlarmExecutionException("Value of indicator "+Indicator.Name.DISK_NAME+" is null or is not a List");
        // }
        // List diskNames = ((List)diskNameValue);
        // for(int i=0; i<diskNames.size();i++){
        // String name = ((String) diskNames.get(i));
        // try{
        // Runtime runtime = Runtime.getRuntime();
        // Process process = runtime.exec(CMD_SMARTCTL+"/dev/"+diskName);
        // process.waitFor();
        // if (process.exitValue() != 0) {
        // throw new IOException("Error occured during execution of command '" + CMD_SMARTCTL + "'. Status code:"
        // + process.exitValue());
        // }
        // // String read = null;
        // // BufferedReader br = new BufferedReader(new InputStreamReader(process.getInputStream()));
        // // while ((read = br.readLine()) != null) {
        // //
        // // }
        // // br.close();
        // } catch (IOException e) {
        // LOG.error("",e);
        // } catch (InterruptedException e) {
        // LOG.error("",e);
        // }
        // }
        // return results;
        // }
        // }, 1440, 1440,true));
        alarms.add(new Alarm(Alarm.Name.POWER_SUPPLY_FAILURE, new String[] { Indicator.Name.MAIN_PWR_ON },
                new AlarmExecution() {
                    public List execute(Configuration configuration, Map indicators) throws AlarmExecutionException {
                        List results = new ArrayList();
                        Indicator mainPwrOn = (Indicator) indicators.get(Indicator.Name.MAIN_PWR_ON);
                        if (mainPwrOn == null) {
                            throw new AlarmExecutionException("Indicator " + Indicator.Name.MAIN_PWR_ON + " is null",
                                    AlarmExecutionException.LEVEL_ERROR);
                        }
                        Object mainPwrOnValue = mainPwrOn.getValue();
                        if (mainPwrOnValue != null) {
                            if (!(mainPwrOnValue instanceof Boolean)) {
                                throw new AlarmExecutionException("Value of indicator " + Indicator.Name.MAIN_PWR_ON
                                        + " is null or is not a Boolean", AlarmExecutionException.LEVEL_ERROR);
                            }
                            boolean power = ((Boolean) mainPwrOnValue).booleanValue();
                            if (power) {
                                results.add(new AlarmExecutionResult(Alarm.ALARM_STATE_CLEAR, "Power is plugged"));
                            } else {
                                results.add(new AlarmExecutionResult(Alarm.ALARM_STATE_CRITICAL, "Power is not plugged"));
                            }
                        } else {
                            LOG.info("Value of indicator " + Indicator.Name.MAIN_PWR_ON + " is null");
                        }

                        return results;
                    }
                }, 1440, 1440, true));
        alarms.add(new Alarm(Alarm.Name.HIGH_NET_IO_THRESHOLD, new String[] { Indicator.Name.NET_RECV_IO,
                Indicator.Name.NET_SENT_IO }, new AlarmExecution() {
            public List execute(Configuration configuration, Map indicators) throws AlarmExecutionException {
                List results = new ArrayList();
                Float alarmNetIoThresholdMaj = (Float) configuration.getValue(Configuration.Name.ALARM_NET_IO_THRESHOLD_MAJ/*
                                                                                                                            * ,
                                                                                                                            * Float
                                                                                                                            * .
                                                                                                                            * class
                                                                                                                            */);
                if (alarmNetIoThresholdMaj == null) {
                    throw new AlarmExecutionException("Configuration value " + Configuration.Name.ALARM_NET_IO_THRESHOLD_MAJ
                            + " cannot be null", AlarmExecutionException.LEVEL_CLEAR);
                }
                Float alarmNetIoThresholdCrit = (Float) configuration
                        .getValue(Configuration.Name.ALARM_NET_IO_THRESHOLD_CRIT/* , Float.class */);
                if (alarmNetIoThresholdCrit == null) {
                    throw new AlarmExecutionException("Configuration value " + Configuration.Name.ALARM_NET_IO_THRESHOLD_CRIT
                            + " cannot be null", AlarmExecutionException.LEVEL_CLEAR);
                }
                Float maxRateNetIo = (Float) configuration.getValue(Configuration.Name.MAX_RATE_NET_IO/* , Float.class */);
                if (maxRateNetIo == null) {
                    throw new AlarmExecutionException("Configuration value " + Configuration.Name.MAX_RATE_NET_IO
                            + " cannot be null", AlarmExecutionException.LEVEL_CLEAR);
                }
                Indicator netRecvIo = (Indicator) indicators.get(Indicator.Name.NET_RECV_IO);
                if (netRecvIo == null) {
                    throw new AlarmExecutionException("Indicator " + Indicator.Name.NET_RECV_IO + " is null",
                            AlarmExecutionException.LEVEL_ERROR);
                }
                Object netRecvIoValue = netRecvIo.getValue();
                if (netRecvIoValue == null || !(netRecvIoValue instanceof List)) {
                    throw new AlarmExecutionException("Value of indicator " + Indicator.Name.NET_RECV_IO
                            + " is null or is not a List", AlarmExecutionException.LEVEL_ERROR);
                }
                List recvsIo = ((List) netRecvIoValue);
                Indicator netSentIo = (Indicator) indicators.get(Indicator.Name.NET_SENT_IO);
                if (netSentIo == null) {
                    throw new AlarmExecutionException("Indicator " + Indicator.Name.NET_SENT_IO + " is null",
                            AlarmExecutionException.LEVEL_ERROR);
                }
                Object netSentIoValue = netSentIo.getValue();
                if (netSentIoValue == null || !(netSentIoValue instanceof List)) {
                    throw new AlarmExecutionException("Value of indicator " + Indicator.Name.NET_SENT_IO
                            + " is null or is not a List", AlarmExecutionException.LEVEL_ERROR);
                }
                List sentsIo = ((List) netSentIoValue);
                if (sentsIo.size() != recvsIo.size()) {
                    throw new AlarmExecutionException("List size of indicator" + Indicator.Name.NET_SENT_IO + "("
                            + sentsIo.size() + ") is different of list size of indicator" + Indicator.Name.NET_RECV_IO + "("
                            + recvsIo.size() + ")", AlarmExecutionException.LEVEL_ERROR);
                }
                for (int i = 0; i < sentsIo.size(); i++) {
                    Object sentIoValue = sentsIo.get(i);
                    if (sentIoValue == null || !(sentIoValue instanceof Float)) {
                        throw new AlarmExecutionException("Value of indicator " + Indicator.Name.NET_SENT_IO + "[" + i
                                + "] is null or is not a Float", AlarmExecutionException.LEVEL_ERROR);
                    }
                    float sentIo = ((Float) sentIoValue).floatValue();
                    Object recvIoValue = recvsIo.get(i);
                    if (recvIoValue == null || !(recvIoValue instanceof Float)) {
                        throw new AlarmExecutionException("Value of indicator " + Indicator.Name.NET_RECV_IO + "[" + i
                                + "] is null or is not a Float", AlarmExecutionException.LEVEL_ERROR);
                    }
                    float recvIo = ((Float) recvIoValue).floatValue();
                    if (recvIo / maxRateNetIo.floatValue() > alarmNetIoThresholdCrit.floatValue()
                            || sentIo / maxRateNetIo.floatValue() > alarmNetIoThresholdCrit.floatValue()) {
                        results.add(new AlarmExecutionResult(Alarm.ALARM_STATE_CRITICAL, i));
                    } else if (recvIo / maxRateNetIo.floatValue() > alarmNetIoThresholdMaj.floatValue()
                            || sentIo / maxRateNetIo.floatValue() > alarmNetIoThresholdMaj.floatValue()) {
                        results.add(new AlarmExecutionResult(Alarm.ALARM_STATE_MAJOR, i));
                    } else if (recvIo / maxRateNetIo.floatValue() <= alarmNetIoThresholdMaj.floatValue()
                            || sentIo / maxRateNetIo.floatValue() <= alarmNetIoThresholdMaj.floatValue()) {
                        results.add(new AlarmExecutionResult(Alarm.ALARM_STATE_CLEAR, i));
                    }
                }
                return results;
            }
        }, 15, 60));
        // alarms.add(new Alarm(Alarm.Name.NET_FAILURE, new String[]{}, new AlarmExecution() {
        // public List execute(Configuration configuration, Map indicators) throws AlarmExecutionException {
        // List results = new ArrayList();
        // return results;
        // }
        // }, 15, 60,true));
        alarms.add(new Alarm(Alarm.Name.HIGH_TEMP_THRESHOLD, new String[] { Indicator.Name.TEMP_CORE }, new AlarmExecution() {
            public List execute(Configuration configuration, Map indicators) throws AlarmExecutionException {
                List results = new ArrayList();

                Float alarmTempThresholdCrit = (Float) configuration.getValue(Configuration.Name.ALARM_TEMP_THRESHOLD_CRIT/*
                                                                                                                           * ,
                                                                                                                           * Float
                                                                                                                           * .
                                                                                                                           * class
                                                                                                                           */);
                if (alarmTempThresholdCrit == null) {
                    throw new AlarmExecutionException("Configuration value " + Configuration.Name.ALARM_TEMP_THRESHOLD_CRIT
                            + " cannot be null", AlarmExecutionException.LEVEL_CLEAR);
                }

                Float alarmTempThresholdMaj = (Float) configuration.getValue(Configuration.Name.ALARM_TEMP_THRESHOLD_MAJ/*
                                                                                                                         * ,
                                                                                                                         * Float
                                                                                                                         * .
                                                                                                                         * class
                                                                                                                         */);
                if (alarmTempThresholdMaj == null) {
                    throw new AlarmExecutionException("Configuration value " + Configuration.Name.ALARM_TEMP_THRESHOLD_MAJ
                            + " cannot be null", AlarmExecutionException.LEVEL_CLEAR);
                }
                Indicator tempCore = (Indicator) indicators.get(Indicator.Name.TEMP_CORE);
                if (tempCore == null) {
                    throw new AlarmExecutionException("Indicator " + Indicator.Name.TEMP_CORE + " is null",
                            AlarmExecutionException.LEVEL_ERROR);
                }
                Object tempCoreValue = tempCore.getValue();
                if (tempCoreValue == null || !(tempCoreValue instanceof List)) {
                    throw new AlarmExecutionException("Value of indicator " + Indicator.Name.TEMP_CORE
                            + " is null or is not a List", AlarmExecutionException.LEVEL_ERROR);
                }
                List tempsCore = ((List) tempCoreValue);
                for (int i = 0; i < tempsCore.size(); i++) {
                    Object tempValue = tempsCore.get(i);
                    if (tempValue == null || !(tempValue instanceof Float)) {
                        throw new AlarmExecutionException("Value of indicator " + Indicator.Name.TEMP_CORE + "[" + i
                                + "] is null or is not a Float", AlarmExecutionException.LEVEL_ERROR);
                    }
                    float temp = ((Float) tempValue).floatValue();
                    if (temp > alarmTempThresholdCrit.floatValue()) {
                        results.add(new AlarmExecutionResult(Alarm.ALARM_STATE_CRITICAL, "Cpu's temperature is too high: "
                                + temp + "°C", i));
                    } else if (temp > alarmTempThresholdMaj.floatValue()) {
                        results.add(new AlarmExecutionResult(Alarm.ALARM_STATE_MAJOR, "Cpu's temperature is too high: " + temp
                                + "°C", i));
                    } else if (temp <= alarmTempThresholdMaj.floatValue()) {
                        results.add(new AlarmExecutionResult(Alarm.ALARM_STATE_CLEAR, i));
                    }
                }
                return results;
            }
        }, 15, 60));
        alarms.add(new Alarm(Alarm.Name.TIME_SYNC_ERROR, new String[] { Indicator.Name.TIME_SYNC }, new AlarmExecution() {
            public List execute(Configuration configuration, Map indicators) throws AlarmExecutionException {
                List results = new ArrayList();
                Indicator timeSync = (Indicator) indicators.get(Indicator.Name.TIME_SYNC);
                if (timeSync == null) {
                    throw new AlarmExecutionException("Indicator " + Indicator.Name.TIME_SYNC + " is null",
                            AlarmExecutionException.LEVEL_ERROR);
                }
                Object timeSyncValue = timeSync.getValue();
                if (timeSyncValue == null || !(timeSyncValue instanceof Boolean)) {
                    throw new AlarmExecutionException("Value of indicator " + Indicator.Name.TIME_SYNC
                            + " is null or is not a Boolean", AlarmExecutionException.LEVEL_ERROR);
                }
                boolean isTimeSync = ((Boolean) timeSync.getValue()).booleanValue();
                if (isTimeSync) {
                    results.add(new AlarmExecutionResult(Alarm.ALARM_STATE_CLEAR, "System clock is correctly synchronized"));
                } else {
                    results.add(new AlarmExecutionResult(Alarm.ALARM_STATE_CRITICAL, "System clock is not synchronized"));
                }
                return results;
            }
        }, 60, 60, true));
        alarms.add(new Alarm(Alarm.Name.SYSTEM_REBOOT_DETECTED, new String[] { Indicator.Name.UPTIME }, new AlarmExecution() {
            // private boolean start = true;
            public List execute(Configuration configuration, Map indicators) throws AlarmExecutionException {
                List results = new ArrayList();
                Indicator uptime = (Indicator) indicators.get(Indicator.Name.UPTIME);
                if (uptime == null) {
                    throw new AlarmExecutionException("Indicator " + Indicator.Name.UPTIME + " is null",
                            AlarmExecutionException.LEVEL_ERROR);
                }
                Object uptimeValue = uptime.getValue();
                if (uptimeValue == null || !(uptimeValue instanceof String)) {
                    throw new AlarmExecutionException("Value of indicator " + Indicator.Name.UPTIME
                            + " is null or is not a String", AlarmExecutionException.LEVEL_ERROR);
                }
                String strUptime = uptimeValue.toString();
                try {
                    long upTime = FormatUtils.parseDuration(strUptime);
                    if (upTime < 5 * 60 * 1000) {
                        results.add(new AlarmExecutionResult(Alarm.ALARM_STATE_CRITICAL, "A system reboot occured"));
                    } else {
                        results.add(new AlarmExecutionResult(Alarm.ALARM_STATE_CLEAR));
                    }
                } catch (ParseException e) {
                    throw new AlarmExecutionException("Value of indicator " + Indicator.Name.UPTIME + "(val:" + strUptime
                            + ") is not a duration", AlarmExecutionException.LEVEL_ERROR);
                }
                return results;
            }
        }, 15, 15, true));
        alarms.add(new Alarm(Alarm.Name.SCL_REBOOT_DETECTED, new String[] { Indicator.Name.UPTIME }, new AlarmExecution() {
            private boolean start = true;

            public List execute(Configuration configuration, Map indicators) throws AlarmExecutionException {
                List results = new ArrayList();
                if (start) {
                    results.add(new AlarmExecutionResult(Alarm.ALARM_STATE_CRITICAL, "Scl software has rebooted"));
                    start = false;
                } else {
                    results.add(new AlarmExecutionResult(Alarm.ALARM_STATE_CLEAR));
                }
                return results;
            }
        }, 15, 15, true));
    }
}
