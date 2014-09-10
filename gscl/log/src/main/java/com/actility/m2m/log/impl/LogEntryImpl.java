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

import java.util.Date;

import org.osgi.framework.Bundle;
import org.osgi.framework.Constants;
import org.osgi.framework.ServiceReference;
import org.osgi.service.log.LogEntry;

import com.actility.m2m.util.FormatUtils;
import com.actility.m2m.util.UtilConstants;

public final class LogEntryImpl implements LogEntry {
    // Log entry data.
    private final Bundle bundle;

    private final ServiceReference serviceReference;

    private final int level;

    private final String message;

    private final Throwable e;

    private final long millis;

//    private final CalendarThreadLocal localCalendar;

    public LogEntryImpl(Bundle bc, int l, String m) {
        this(bc, null, l, m, null);
    }

    public LogEntryImpl(Bundle bc, int l, String m, Throwable e) {
        this(bc, null, l, m, e);
    }

    public LogEntryImpl(Bundle bc, ServiceReference sd, int l, String m) {
        this(bc, sd, l, m, null);
    }

    public LogEntryImpl(Bundle bc, ServiceReference sd, int l, String m, Throwable e) {
        this.bundle = bc;
        this.serviceReference = sd;
        this.level = l;
        this.message = m;
        this.e = e;
        this.millis = System.currentTimeMillis();
//        localCalendar = new CalendarThreadLocal();
    }

    /**
     * Returns a string representing this log entry. The format is:
     */
    public String toString() {
        StringBuffer sb = new StringBuffer(100);
        LogUtils.fromLevel(sb, level, "       ");
        sb.append(' ');
//        sb.append(FormatUtils.formatDateTime(new Date(millis), (Calendar) localCalendar.get()));
        sb.append(FormatUtils.formatDateTime(new Date(millis), UtilConstants.LOCAL_TIMEZONE));
        sb.append(' ');
        // Reserve 8 chars for the bundle id, pad with spaces if needed
        int bidEndPos = sb.length() + 8;
        sb.append("bid#");
        if (bundle != null) {
            sb.append(bundle.getBundleId());
        }
        sb.append("    ");
        sb.setLength(bidEndPos);
        sb.append(" - ");
        if (serviceReference != null) {
            sb.append('[');
            sb.append(serviceReference.getProperty(Constants.SERVICE_ID).toString());
            sb.append(';');
            String[] clazzes = (String[]) serviceReference.getProperty(Constants.OBJECTCLASS);
            for (int i = 0; i < clazzes.length; i++) {
                if (i > 0) {
                    sb.append(',');
                }
                sb.append(clazzes[i]);
            }
            sb.append("] ");
        }
        sb.append(message);
        if (e != null) {
            sb.append(" (");
            sb.append(e.toString());
            sb.append(')');
        }

        return sb.toString();
    }

    public Bundle getBundle() {
        return bundle;
    }

    public ServiceReference getServiceReference() {
        return serviceReference;
    }

    public int getLevel() {
        return level;
    }

    public String getMessage() {
        return message;
    }

    public Throwable getException() {
        return e;
    }

    public long getTime() {
        return millis;
    }
}
