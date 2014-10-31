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

public final class Indicator {
    private final String name;
    private final String type;
    private Object value;
    private int refreshInterval;
    private int refreshPosition;

    /**
     * Constructor
     *
     * @param name the name of the indicator
     * @param value the value of the indicator
     * @param type the type of the indicator
     * @param refreshInterval the interval (in seconds) between each refresh
     */
    public Indicator(String name, Object value, String type, int refreshInterval) {
        this.name = name;
        this.value = value;
        this.type = type;
        this.refreshInterval = refreshInterval;
        this.refreshPosition = -1;
    }

    /**
     * Constructor
     *
     * @param name the name of the indicator
     * @param type the type of the indicator
     * @param refreshInterval the interval (in seconds) between each refresh
     */
    public Indicator(String name, String type, int refreshInterval) {
        super();
        this.name = name;
        this.value = null;
        this.type = type;
        this.refreshInterval = refreshInterval;
        this.refreshPosition = -1;
    }

    /**
     * Gets the values of the indicator
     *
     * @return
     */
    public Object getValue() {
        return value;
    }

    /**
     * Sets the value of the indicator
     *
     * @param value
     */
    public void setValue(Object value) {
        this.value = value;
    }

    /**
     * Gives the interval (in seconds) between each refresh
     *
     * @return
     */
    public int getRefreshInterval() {
        return refreshInterval;
    }

    /**
     * Sets the interval (in seconds) between each refresh
     *
     * @param refreshInterval
     */
    public void setRefreshInterval(int refreshInterval) {
        this.refreshInterval = refreshInterval;
    }

    /**
     * This method is called each minutes and return true if it's the time to update the value
     *
     * @return
     */
    public boolean updateRefreshPosition() {
        if (refreshInterval > 0) {
            refreshPosition++;
            if (refreshPosition == refreshInterval) {
                refreshPosition = 0;
                return true;
            }
        } else {
            if (refreshInterval == -1) {
                if (refreshPosition == -1) {
                    refreshPosition = -2;
                    return true;
                }
            }
        }
        return false;
    }

    /**
     * Gets the name of the indicator
     *
     * @return
     */
    public String getName() {
        return name;
    }

    /**
     * Get the type of the indicator
     *
     * @return
     */
    public String getType() {
        return type;
    }

    public int hashCode() {
        final int prime = 31;
        int result = 1;
        result = prime * result + ((name == null) ? 0 : name.hashCode());
        return result;
    }

    public boolean equals(Object obj) {
        if (this == obj) {
            return true;
        }
        if (obj == null) {
            return false;
        }
        if (getClass() != obj.getClass()) {
            return obj instanceof String && obj.equals(name);
        }
        Indicator other = (Indicator) obj;
        if (name == null) {
            if (other.name != null) {
                return false;
            }
        } else if (!name.equals(other.name)) {
            return false;
        }
        return true;
    }

    public String toString() {
        return "[" + name + "<" + type + ">:" + value + "]";
    }

    /**
     * This class describes all allowed indicator names
     *
     * @author qdesrame
     *
     */
    public class Name {
        public final static String CPU = "CPU";
        public final static String UPTIME = "uptime";
        public final static String LOAD_AVG_PT1M = "loadAvgPT1M";
        public final static String LOAD_AVG_PT5M = "loadAvgPT5M";
        public final static String LOAD_AVG_PT15M = "loadAvgPT15M";
        public final static String PROC_TOTAL = "procTotal";
        public final static String MEM_USED = "memUsed";
        public final static String MEM_FREE = "memFree";
        public final static String MEM_BUFFERS = "memBuffers";
        public final static String MEM_CACHED = "memCached";
        public final static String MEM_SWP_USED = "memSwpUsed";
        public final static String MEM_SWP_FREE = "memSwpFree";
        public final static String DISK_PART_USED = "diskPartUsed";
        public final static String DISK_PART_FREE = "diskPartFree";
        public final static String DISK_READ_IO = "diskReadIo";
        public final static String DISK_WRITE_IO = "diskWriteIo";
        public final static String NET_RECV_IO = "netRecvIo";
        public final static String NET_SENT_IO = "netSendIo";
        public final static String MAIN_PWR_ON = "mainPwrOn";
        public final static String BAT_PWR_LEVEL = "batPwrLevel";
        public final static String BAT_REMAINING_TIME = "batRemainingTime";
        public final static String TEMP_CORE = "tempCore";
        public final static String TEMP_FRAME = "tempFrame";
        public final static String TIME_SYNC = "timeSync";
        public final static String LRR_LINK_STATE = "lrrLinkState";
        public final static String SRV_STATUS = "srvStatus";
        public final static String SRV_PID = "srvPid";
        public final static String SRV_START_DATE = "srvStartDate";
        public final static String SRV_NB_RESTARTS = "srcNbRestarts";

        // STATIC INDICATOR
        public final static String NUM_CPU = "numCpu";
        public final static String MEM_TOTAL = "memTotal";
        public final static String MEM_SWP_TOTAL = "memSwpTotal";
        public final static String DISK_PART_SIZE = "diskPartSize";
        public final static String DISK_PART_NAME = "diskPartName";
        public final static String DISK_NAME = "diskName";
        public final static String NET_NAME = "netName";
        public final static String NET_BANDWIDTH = "netBandwidth";
        public final static String NET_BANDWIDTH_USABLE = "netBandwidthUsable";
        public final static String SRV_NAME = "srvName";
        public final static String BAT_NAME = "batName";
    }
}
