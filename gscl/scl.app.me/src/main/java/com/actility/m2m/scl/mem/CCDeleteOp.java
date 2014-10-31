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
 * id $Id: NCDeleteOp.java 3302 2012-10-10 14:19:16Z JReich $
 * author $Author: JReich $
 * version $Revision: 3302 $
 * lastrevision $Date: 2012-10-10 16:19:16 +0200 (Wed, 10 Oct 2012) $
 * modifiedby $LastChangedBy: JReich $
 * lastmodified $LastChangedDate: 2012-10-10 16:19:16 +0200 (Wed, 10 Oct 2012) $
 */

package com.actility.m2m.scl.mem;

import java.net.URI;

import org.apache.log4j.Logger;

import com.actility.m2m.m2m.ChannelServerListener;
import com.actility.m2m.m2m.M2MException;
import com.actility.m2m.scl.log.BundleLogger;
import com.actility.m2m.scl.model.SclManager;
import com.actility.m2m.scl.model.TransientOp;
import com.actility.m2m.util.log.OSGiLogger;

public final class CCDeleteOp implements TransientOp {
    private static final Logger LOG = OSGiLogger.getLogger(NCDeleteOp.class, BundleLogger.getStaticLogger());

    private final String logId;
    private final SclManager manager;
    private final URI contactUri;
    private final URI longPollUri;
    private final ChannelServerListener channelListener;
    private boolean done;

    public CCDeleteOp(String logId, SclManager manager, URI contactUri, URI longPollUri, ChannelServerListener channelListener) {
        this.logId = logId;
        this.manager = manager;
        this.contactUri = contactUri;
        this.longPollUri = longPollUri;
        this.channelListener = channelListener;
    }

    public void prepare() {
        // Delete long poll connection
        manager.getM2MContext().deleteServerCommunicationChannel(contactUri, longPollUri);
        done = true;
    }

    public void rollback() {
        if (done) {
            try {
                manager.getM2MContext().createServerNotificationChannel(contactUri, longPollUri, channelListener);
            } catch (M2MException e) {
                LOG.error(logId + ": Unable to rollback communication channel deletion", e);
            }
        }
    }

    public void postCommit() {
        // Nothing to do
    }
}
