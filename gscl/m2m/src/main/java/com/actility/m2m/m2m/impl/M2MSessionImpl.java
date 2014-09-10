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
 * id $Id: M2MSessionImpl.java 6083 2013-10-15 13:38:23Z mlouiset $
 * author $Author: mlouiset $
 * version $Revision: 6083 $
 * lastrevision $Date: 2013-10-15 15:38:23 +0200 (Tue, 15 Oct 2013) $
 * modifiedby $LastChangedBy: mlouiset $
 * lastmodified $LastChangedDate: 2013-10-15 15:38:23 +0200 (Tue, 15 Oct 2013) $
 */

package com.actility.m2m.m2m.impl;

import java.io.Serializable;

import com.actility.m2m.m2m.M2MSession;
import com.actility.m2m.servlet.ApplicationSession;
import com.actility.m2m.servlet.ServletTimer;
import com.actility.m2m.servlet.TimerService;

public final class M2MSessionImpl implements M2MSession {

    private final TimerService timerService;
    private final ApplicationSession appSession;

    public M2MSessionImpl(TimerService timerService, ApplicationSession appSession, long timeout) {
        this.timerService = timerService;
        this.appSession = appSession;
        this.appSession.setInvalidateWhenReady(false);
        this.appSession.setExpiresMillis(timeout);
    }

    public ApplicationSession getApplicationSession() {
        return appSession;
    }

    public Object getAttribute(String name) {
        if (name.startsWith(M2MContextImpl.M2M_PREFIX)) {
            return null;
        }
        return appSession.getAttribute(name);
    }

    public void setAttribute(String name, Object attribute) {
        if (name.startsWith(M2MContextImpl.M2M_PREFIX)) {
            throw new IllegalArgumentException(
                    "m2m: prefix is reserved for M2M Layer internal attributes. Cannot set an attribute with this prefix: "
                            + name);
        }
        appSession.setAttribute(name, attribute);
    }

    public void removeAttribute(String name) {
        if (name.startsWith(M2MContextImpl.M2M_PREFIX)) {
            throw new IllegalArgumentException(
                    "m2m: prefix is reserved for M2M Layer internal attributes. Cannot remove an attribute with this prefix: "
                            + name);
        }
        appSession.removeAttribute(name);
    }

    public String getId() {
        return appSession.getId();
    }

    public void setExpires(long expires) {
        appSession.setExpiresMillis(expires);
    }

    public void delete() {
        appSession.invalidate();
    }

    public String startTimer(long timeout, Serializable info) {
        ServletTimer timer = timerService.createTimer(appSession, timeout, false, info);
        appSession.setAttribute(M2MContextImpl.M2M_TIMER_PREFIX + timer.getId(), this);
        return timer.getId();
    }

    public void cancelTimer(String timerId) {
        ServletTimer timer = appSession.getTimer(timerId);
        if (timer != null) {
            timer.cancel();
        }
    }
}
