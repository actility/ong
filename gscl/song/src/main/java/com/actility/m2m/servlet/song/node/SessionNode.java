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
 * id $Id: SessionNode.java 8766 2014-05-21 15:41:17Z JReich $
 * author $Author: JReich $
 * version $Revision: 8766 $
 * lastrevision $Date: 2014-05-21 17:41:17 +0200 (Wed, 21 May 2014) $
 * modifiedby $LastChangedBy: JReich $
 * lastmodified $LastChangedDate: 2014-05-21 17:41:17 +0200 (Wed, 21 May 2014) $
 */

package com.actility.m2m.servlet.song.node;

import org.apache.log4j.Logger;

import com.actility.m2m.be.messaging.InOut;
import com.actility.m2m.be.messaging.MessagingException;
import com.actility.m2m.servlet.ext.ExtApplicationSession;
import com.actility.m2m.servlet.log.BundleLogger;
import com.actility.m2m.servlet.song.SongServletRequest;
import com.actility.m2m.servlet.song.internal.InternalNode;
import com.actility.m2m.util.log.OSGiLogger;

public final class SessionNode implements InternalNode {
    private static final Logger LOG = OSGiLogger.getLogger(SessionNode.class, BundleLogger.getStaticLogger());
    private final ServletEndpointNode destination;
    private final ExtApplicationSession appSession;

    public SessionNode(ServletEndpointNode destination, ExtApplicationSession appSession) {
        this.destination = destination;
        this.appSession = appSession;
    }

    public String getContextPath() {
        return destination.getContextPath();
    }

    public void update(InOut exchange, SongServletRequest request, ExtApplicationSession appSession, String contextPath,
            String servletPath, String pathInfo) throws MessagingException {
        LOG.debug("SongSessionNode update");
        destination.update(exchange, request, this.appSession, contextPath, servletPath, pathInfo);
    }

    public String getTarget() {
        return destination.getTarget() + " (session)";
    }
}
