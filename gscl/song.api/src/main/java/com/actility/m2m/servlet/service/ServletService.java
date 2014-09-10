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
 * id $Id: ServletService.java 6081 2013-10-15 13:33:50Z mlouiset $
 * author $Author: mlouiset $
 * version $Revision: 6081 $
 * lastrevision $Date: 2013-10-15 15:33:50 +0200 (Tue, 15 Oct 2013) $
 * modifiedby $LastChangedBy: mlouiset $
 * lastmodified $LastChangedDate: 2013-10-15 15:33:50 +0200 (Tue, 15 Oct 2013) $
 */

package com.actility.m2m.servlet.service;

import java.util.Map;

import javax.servlet.ServletContext;
import javax.servlet.ServletException;

import com.actility.m2m.servlet.ServletTimer;
import com.actility.m2m.servlet.TimerListener;

/**
 * The Servlet Service allows other bundles in the OSGi environment to dynamically create {@link ServletContext servlet
 * applications}. This application can be used to manage a set of servlets bound to several protocols. A bundle may later
 * unregister its applications.
 */
public interface ServletService {

    /**
     * Creates an empty servlet application.
     *
     * @param contextPath The context path of the application, must start with '/' and must not end with '/' except for a root
     *            application in the context path '/'
     * @param applicationName The name of the application
     * @param initParams The configuration of the application or null if there is none
     * @param timerListener The timer listener related to that application. Must be provided if the application uses
     *            {@link ServletTimer timers} otherwise it is null
     * @param listeners A list of servlet listeners or null if there are none
     *
     * @return The created application represented by its {@link ServletContext}
     * @throws ServletException If the name of the application is already in used or if a problem occurs during its registration
     */
    ServletContext createApplication(String contextPath, String applicationName, Map initParams, TimerListener timerListener,
            Object[] listeners) throws ServletException;

    /**
     * Unregisters a servlet application. This automatically unregisters all servlets bound to that application from there
     * corresponding protocol services.
     *
     * @param context The {@link ServletContext} to unregister
     */
    void unregisterApplication(ServletContext context);
}
