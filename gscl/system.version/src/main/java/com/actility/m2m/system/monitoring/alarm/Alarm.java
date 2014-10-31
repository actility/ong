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

package com.actility.m2m.system.monitoring.alarm;

import java.util.ArrayList;
import java.util.List;
import java.util.Map;

import org.apache.log4j.Logger;

import com.actility.m2m.system.config.Configuration;
import com.actility.m2m.system.log.BundleLogger;
import com.actility.m2m.util.log.OSGiLogger;

public final class Alarm {
    private static final Logger LOG = OSGiLogger.getLogger(Alarm.class, BundleLogger.LOG);

    private static final int ALARM_STATE_UNSET = 0;
    public static final int ALARM_STATE_CLEAR = 1;
    public static final int ALARM_STATE_WARNING = 2;
    public static final int ALARM_STATE_INDETERMINATE = 3;
    public static final int ALARM_STATE_MINOR = 4;
    public static final int ALARM_STATE_MAJOR = 5;
    public static final int ALARM_STATE_CRITICAL = 6;

    private final String[] targetedIndicator;
    private final String name;
    private final String varInfo;
    private final AlarmExecution execution;
    private final int refreshInterval;
    private final int raisedInterval;
    private final int raised;
    private int refreshPosition;
    private List/* <Integer> */states;
    private List/* <Integer */lastAlarmPositions;

    /**
     * Constructor
     *
     * @param name
     * @param targetedIndicators
     * @param varInfo
     * @param execution
     * @param refreshInterval
     * @param raisedInterval
     */
    public Alarm(String name, String[] targetedIndicators, String varInfo, AlarmExecution execution, int refreshInterval,
            int raisedInterval) {
        this.name = name;
        this.targetedIndicator = targetedIndicators;
        this.varInfo = varInfo;
        this.execution = execution;
        this.refreshInterval = refreshInterval;
        this.raisedInterval = raisedInterval;
        this.refreshPosition = -1;
        this.states = new ArrayList();
        this.lastAlarmPositions = new ArrayList();
        this.raised = ALARM_STATE_CLEAR;
    }

    /**
     * Constructor
     *
     * @param name
     * @param targetedIndicators
     * @param execution
     * @param refreshInterval
     * @param raisedInterval
     */
    public Alarm(String name, String[] targetedIndicators, AlarmExecution execution, int refreshInterval, int raisedInterval) {
        this.name = name;
        this.targetedIndicator = targetedIndicators;
        this.varInfo = null;
        this.execution = execution;
        this.refreshInterval = refreshInterval;
        this.raisedInterval = raisedInterval;
        this.refreshPosition = -1;
        this.states = new ArrayList();
        this.lastAlarmPositions = new ArrayList();
        this.raised = ALARM_STATE_CLEAR;
    }

    /**
     * Constructor
     *
     * @param name
     * @param targetedIndicators
     * @param varInfo
     * @param execution
     * @param refreshInterval
     * @param raisedInterval
     * @param alarmAtBoot
     */
    public Alarm(String name, String[] targetedIndicators, String varInfo, AlarmExecution execution, int refreshInterval,
            int raisedInterval, boolean alarmAtBoot) {
        this.name = name;
        this.targetedIndicator = targetedIndicators;
        this.varInfo = varInfo;
        this.execution = execution;
        this.refreshInterval = refreshInterval;
        this.raisedInterval = raisedInterval;
        this.refreshPosition = -1;
        this.states = new ArrayList();
        this.lastAlarmPositions = new ArrayList();
        if (alarmAtBoot) {
            this.raised = ALARM_STATE_UNSET;
        } else {
            this.raised = ALARM_STATE_CLEAR;
        }
    }

    /**
     * Constructor
     *
     * @param name
     * @param targetedIndicators
     * @param execution
     * @param refreshInterval
     * @param raisedInterval
     * @param alarmAtBoot
     */
    public Alarm(String name, String[] targetedIndicators, AlarmExecution execution, int refreshInterval, int raisedInterval,
            boolean alarmAtBoot) {
        this.name = name;
        this.targetedIndicator = targetedIndicators;
        this.varInfo = null;
        this.execution = execution;
        this.refreshInterval = refreshInterval;
        this.raisedInterval = raisedInterval;
        this.refreshPosition = -1;
        this.states = new ArrayList();
        this.lastAlarmPositions = new ArrayList();
        if (alarmAtBoot) {
            this.raised = ALARM_STATE_UNSET;
        } else {
            this.raised = ALARM_STATE_CLEAR;
        }
    }

    /**
     * Executes the system monitoring
     *
     * @param configuration
     * @param indicators
     * @return
     */
    public List/* <AlarmResult> */executeMonitoring(Configuration configuration, Map/* <Indicator> */indicators) {
        List results = new ArrayList();
        try {
            List/* <AlarmExecutionResult> */executionResults = this.execution.execute(configuration, indicators);
            if (executionResults.size() != states.size()) {
                if (states.size() != 0) {
                    LOG.warn("Length of states list in alarm " + name + "has changed (old:" + states.size() + "/new:"
                            + executionResults.size() + ")");
                    states = new ArrayList();
                }
                for (int i = 0; i < executionResults.size(); i++) {
                    states.add(new Integer(raised));
                }
            }
            if (executionResults.size() != lastAlarmPositions.size()) {
                if (lastAlarmPositions.size() != 0) {
                    lastAlarmPositions = new ArrayList();
                }
                for (int i = 0; i < executionResults.size(); i++) {
                    lastAlarmPositions.add(null);
                }
            }
            for (int i = 0; i < executionResults.size(); i++) {
                AlarmExecutionResult result = (AlarmExecutionResult) executionResults.get(i);
                String varInfo = result.getInfo() == null ? this.varInfo : result.getInfo();
                if (result.getState() != ((Integer) states.get(i)).intValue()) {
                    results.add(new AlarmResult(name, varInfo, result.getIndex(), result.getState()));
                    states.set(i, new Integer(result.getState()));
                    lastAlarmPositions.set(i, new Integer(0));
                } else {
                    Integer lastAlarm = (Integer) lastAlarmPositions.get(i);
                    if (lastAlarm != null) {
                        if (lastAlarm.intValue() >= raisedInterval) {
                            if (result.getState() != ALARM_STATE_CLEAR) {
                                results.add(new AlarmResult(name, varInfo, result.getIndex(), result.getState()));
                                states.set(i, new Integer(result.getState()));
                                lastAlarmPositions.set(i, new Integer(0));
                            }
                        }
                    }
                }
            }
        } catch (AlarmExecutionException e) {
            if (e.getLevel() == AlarmExecutionException.LEVEL_CLEAR) {
                if (LOG.isInfoEnabled()) {
                    LOG.info("Alarm " + name + " cannot be executed :" + e.getMessage());
                }
            } else {
                LOG.error("An exception occured during alarm " + name + " execution: " + e.getMessage());
            }
        }
        return results;
    }

    /**
     * Updates the position between each refresh and return true if the alarm monitoring script should be reload
     *
     * @return
     */
    public boolean updateRefreshPosition() {
        refreshPosition++;
        for (int i = 0; i < lastAlarmPositions.size(); i++) {
            Integer lastAlarm = (Integer) lastAlarmPositions.get(i);
            if (lastAlarm != null) {
                if (lastAlarm.intValue() < raisedInterval) {
                    lastAlarmPositions.set(i, new Integer(lastAlarm.intValue() + 1));
                }
            }
        }
        if (refreshPosition == 0 && raised == ALARM_STATE_UNSET) {
            return true;
        }
        if (refreshPosition == refreshInterval) {
            refreshPosition = 0;
            return true;
        }
        return false;
    }

    /**
     * Gets indicators alarm necessary to execute his monitoring script
     *
     * @return
     */
    public String[] getTargetedIndicators() {
        return targetedIndicator;
    }

    /**
     * This class describes the authorized alarm names
     *
     * @author qdesrame
     *
     */
    public class Name {
        public final static String HIGH_LOAD_AVG_THRESHOLD = "highLoadAvgThreshold";
        public final static String HIGH_RAM_THRESHOLD = "highRamThreshold";
        public final static String HIGH_DISK_THRESHOLD = "highDiskThreshold";
        public final static String HIGH_DISK_IO_THRESHOLD = "highDiskIoThreshold";
        public final static String DISK_FAILURE = "diskFailure";
        public final static String POWER_SUPPLY_FAILURE = "powerSupplyFailure";
        public final static String LOW_BATTERY_THRESHOLD = "lowBatteryThreshold";
        public final static String BATTERY_CHARGING_FAILURE = "batteryChargingFailure";
        public final static String HIGH_NET_IO_THRESHOLD = "highNetIoThreshold";
        public final static String NET_FAILURE = "netFailure";
        public final static String HIGH_TEMP_THRESHOLD = "highTempThreshold";
        public final static String ANTENNA_FAILURE = "antennaFailure";
        public final static String RECEIVER_FAILURE = "receiverFailure";
        public final static String TRANSMITTER_FAILURE = "transmitterFailure";
        public final static String COOLING_FAN_FAILURE = "coolingFanFailure";
        public final static String TIME_SYNC_ERROR = "timeSyncError";
        public final static String LRR_LINK_FAILURE = "lrrLinkFailure";
        public final static String SYSTEM_REBOOT_DETECTED = "systemRebootDetected";
        public final static String SCL_REBOOT_DETECTED = "sclRebootDetected";
        public final static String SOFTWARE_ERROR = "softwareError";
        public final static String EXCESSIVE_ERROR_RATE = "excessiveErrorRate";
        public final static String EXCESSIVE_RESPONSE_TIME = "excessiveResponseTime";
        public final static String SFWR_ENVIRONMENT_PROBLEM = "sfwrEnvironmentProblem";
        public final static String SFWR_DOWNLOAD_FAILURE = "sfwrDownloadFailure";
    }
}
