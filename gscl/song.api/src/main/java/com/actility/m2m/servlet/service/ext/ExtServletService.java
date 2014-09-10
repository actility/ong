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
 * id $Id: ExtServletService.java 6081 2013-10-15 13:33:50Z mlouiset $
 * author $Author: mlouiset $
 * version $Revision: 6081 $
 * lastrevision $Date: 2013-10-15 15:33:50 +0200 (Tue, 15 Oct 2013) $
 * modifiedby $LastChangedBy: mlouiset $
 * lastmodified $LastChangedDate: 2013-10-15 15:33:50 +0200 (Tue, 15 Oct 2013) $
 */

package com.actility.m2m.servlet.service.ext;

import java.util.Iterator;
import java.util.TimerTask;

import com.actility.m2m.servlet.ext.ExtProtocolContainer;
import com.actility.m2m.servlet.ext.ExtServletContext;
import com.actility.m2m.servlet.service.ServletService;

/**
 * Extensions the {@link ServletService} used to access internals of the servlet container.
 */
public interface ExtServletService extends ServletService {

    /**
     * Gets an iterator on {@link ExtServletContext}.
     *
     * @return An iterator on {@link ExtServletContext}
     */
    Iterator getApplications();

    /**
     * Unregisters a protocol container from the servlet container
     *
     * @param protocol The protocl to unregister from the servlet container
     */
    void unregisterProtocolContainer(ExtProtocolContainer protocol);

    /**
     * Schedule the given timer with the global timer management system of the servlet container.
     *
     * @param timer The timer to start
     * @param millis The timer expiration
     */
    void scheduleTimer(TimerTask timer, long millis);
}
