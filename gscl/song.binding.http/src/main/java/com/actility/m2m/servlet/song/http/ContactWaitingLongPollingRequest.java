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
 * id $Id: ContactWaitingLongPollingRequest.java 8521 2014-04-14 09:05:59Z JReich $
 * author $Author: JReich $
 * version $Revision: 8521 $
 * lastrevision $Date: 2014-04-14 11:05:59 +0200 (Mon, 14 Apr 2014) $
 * modifiedby $LastChangedBy: JReich $
 * lastmodified $LastChangedDate: 2014-04-14 11:05:59 +0200 (Mon, 14 Apr 2014) $
 */

package com.actility.m2m.servlet.song.http;

import java.io.IOException;
import java.util.TimerTask;

import org.apache.log4j.Logger;

import com.actility.m2m.servlet.song.SongServletRequest;
import com.actility.m2m.servlet.song.SongServletResponse;
import com.actility.m2m.servlet.song.http.log.BundleLogger;
import com.actility.m2m.util.log.OSGiLogger;

/**
 * Represents a waiting SONG request in a long polling server connection.
 * <p>
 * It is waiting for all SONG requests before it in the queue to be sent and then for an incoming HTTP long polling request to
 * arrive on the long polling URI.
 * <p>
 * This waiting request also arms a safety timer which will trigger a 408 Request Timeout if the request cannot be processed in
 * time by the long polling connection.
 */
public final class ContactWaitingLongPollingRequest extends TimerTask {
    private static final Logger LOG = OSGiLogger.getLogger(ContactWaitingLongPollingRequest.class, BundleLogger.LOG);

    private final LongPollingServer lpServer;
    private SongServletRequest request;
    private final boolean flush;

    /**
     * Builds a long polling waiting request.
     *
     * @param lpServer The long polling server which queues this contact request
     * @param request The SONG request which is waiting for the long polling connection to be ready
     */
    public ContactWaitingLongPollingRequest(LongPollingServer lpServer, SongServletRequest request) {
        this.lpServer = lpServer;
        this.request = request;
        flush = false;
    }

    /**
     * Builds a flush long polling waiting request.
     */
    public ContactWaitingLongPollingRequest() {
        this.lpServer = null;
        flush = true;
    }

    /**
     * Pops the SONG request associated to this class.
     * <p>
     * This call resets the request to <code>null</code> so this method must be called only once. This is to prevent memory
     * leaks in java.util.Timer which keeps a reference to TimerTasks even if they are cancelled
     *
     * @return The SONG request associated to this class
     */
    public SongServletRequest popRequest() {
        SongServletRequest request = this.request;
        this.request = null;
        return request;
    }

    /**
     * Checks whether this is a flush request which means the long polling request must be responded by a 404 Not Found.
     *
     * @return Whether this is a flush request
     */
    public boolean isFlushRequest() {
        return flush;
    }

    public void run() {
        long run = System.currentTimeMillis();
        try {
            lpServer.waitingContactRequestExpiration();
            SongServletResponse response = request.createResponse(SongServletResponse.SC_GATEWAY_TIMEOUT);
            if (LOG.isInfoEnabled()) {
                LOG.info(response.getId() + ": <<< HTTP.SONGBinding: " + response.getStatus() + " "
                        + response.getReasonPhrase());
            }
            response.send();
            request = null;
        } catch (IOException e) {
            LOG.error("Cannot send the 504 gateway timeout response to the expired long polling request", e);
        }
        run = System.currentTimeMillis() - run;
        if (run > 500) {
            LOG.error("The 504 response has taken more than 500 ms to be handled: " + run + " ms");
        }
    }
}
