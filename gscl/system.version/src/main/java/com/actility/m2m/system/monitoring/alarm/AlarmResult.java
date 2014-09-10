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

/**
 * This object is the result of an alarm execution
 * @author qdesrame
 *
 */
public class AlarmResult {
    private long timestamp;
    private String name;
    private String info;
    private int index;
    private int state;
    public AlarmResult(String name, String info, int index, int state) {
        this.timestamp = System.currentTimeMillis();
        this.name = name;
        this.info = info;
        this.index = index;
        this.state = state;
    }
    public AlarmResult(String name, String info, int state) {
        this.timestamp = System.currentTimeMillis();
        this.name = name;
        this.info = info;
        this.index = -1;
        this.state = state;
    }
    public AlarmResult(String name, int index, int state) {
        this.timestamp = System.currentTimeMillis();
        this.name = name;
        this.info = null;
        this.index = index;
        this.state = state;
    }
    public AlarmResult(String name, int state) {
        this.timestamp = System.currentTimeMillis();
        this.name = name;
        this.info = null;
        this.index = -1;
        this.state = state;
    }
    public long getTimestamp() {
        return timestamp;
    }
    public String getName() {
        return name;
    }
    public String getInfo() {
        return info;
    }
    public int getIndex() {
        return index;
    }
    public int getState() {
        return state;
    }
}
