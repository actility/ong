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
 * id $Id: SongContainerFacade.java 9309 2014-08-21 10:13:58Z JReich $
 * author $Author: JReich $
 * version $Revision: 9309 $
 * lastrevision $Date: 2014-08-21 12:13:58 +0200 (Thu, 21 Aug 2014) $
 * modifiedby $LastChangedBy: JReich $
 * lastmodified $LastChangedDate: 2014-08-21 12:13:58 +0200 (Thu, 21 Aug 2014) $
 */

package com.actility.m2m.servlet.song.internal;

import java.util.TimerTask;

import com.actility.m2m.be.EndpointContext;
import com.actility.m2m.framework.resources.ResourcesAccessorService;
import com.actility.m2m.servlet.ext.ExtProtocolContainer;
import com.actility.m2m.servlet.song.SongServletResponse;
import com.actility.m2m.xo.XoService;

public interface SongContainerFacade extends ExtProtocolContainer {
    EndpointContext getContext();

    XoService getXoService();

    ResourcesAccessorService getResourcesAccessorService();

    void scheduleTimer(TimerTask timer, long millis);

    BindingNode getBindingNode(String scheme);

    void destroyEndpoint(EndpointNode endpoint);

    boolean sendLocalRequest(String method);

    boolean sendRemoteRequest(String method);

    boolean sendForwardRequest(String method);

    void receivedLocalResponse(SongServletResponse response, long requestTime, long duration, String method);

    void receivedRemoteResponse(SongServletResponse response, long requestTime, long duration, String method);

    void receivedForwardResponse(SongServletResponse response, long requestTime, long duration, String method);

}
