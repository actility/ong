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
 * id $Id: SongBindingNode.java 8767 2014-05-21 15:41:33Z JReich $
 * author $Author: JReich $
 * version $Revision: 8767 $
 * lastrevision $Date: 2014-05-21 17:41:33 +0200 (Wed, 21 May 2014) $
 * modifiedby $LastChangedBy: JReich $
 * lastmodified $LastChangedDate: 2014-05-21 17:41:33 +0200 (Wed, 21 May 2014) $
 */

package com.actility.m2m.servlet.song.node;

import java.net.InetAddress;

import org.apache.log4j.Logger;

import com.actility.m2m.be.messaging.InOut;
import com.actility.m2m.be.messaging.MessagingException;
import com.actility.m2m.servlet.ext.ExtApplicationSession;
import com.actility.m2m.servlet.log.BundleLogger;
import com.actility.m2m.servlet.song.SongBindingFacade;
import com.actility.m2m.servlet.song.SongServletRequest;
import com.actility.m2m.servlet.song.internal.BindingNode;
import com.actility.m2m.util.log.OSGiLogger;

public final class SongBindingNode implements BindingNode {
    private static final Logger LOG = OSGiLogger.getLogger(SongBindingNode.class, BundleLogger.getStaticLogger());
    public static final int SONG_PORT = 6070;

    private static final String[] SCHEMES = new String[] { "song", "songs" };

    private final InetAddress address;
    private final int port;

    public SongBindingNode(InetAddress address, int port) {
        this.address = address;
        this.port = port;
    }

    public String getContextPath() {
        return null;
    }

    public String getDefaultProtocol() {
        return "SONG/1.0";
    }

    public SongBindingFacade getSongBindingFacade() {
        // This is an internal factory
        return null;
    }

    public String getServerScheme() {
        return "song";
    }

    public String[] getManagedSchemes() {
        return SCHEMES;
    }

    public boolean isLongPollSupported() {
        return false;
    }

    public InetAddress getAddress() {
        return address;
    }

    public int getPort() {
        return port;
    }

    public String getTarget() {
        return "_INTERNAL_SONG.SONGBinding";
    }

    public void update(InOut exchange, SongServletRequest request, ExtApplicationSession appSession, String contextPath,
            String servletPath, String pathInfo) throws MessagingException {
        LOG.debug("SongBindingNode update");
        throw new MessagingException("SONG binding has no network implementation, "
                + "it must be only used for internal communication");
    }
}
