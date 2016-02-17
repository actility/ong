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

package com.actility.m2m.servlet.song.http;

import java.io.IOException;
import java.util.TimerTask;

import org.apache.log4j.Logger;

import com.actility.m2m.servlet.song.SongServletRequest;
import com.actility.m2m.servlet.song.SongServletResponse;
import com.actility.m2m.servlet.song.http.log.BundleLogger;
import com.actility.m2m.util.log.OSGiLogger;

/**
 * Represents a waiting SONG request after it has been sent in a Long Polling HTTP response.
 * <p>
 * This waiting request arms a safety timer which will trigger a 504 Gateway Timeout if the request cannot be processed in time
 * by the long polling connection.
 */
public final class ContactWaitingResponseNotifyRequest extends TimerTask {
    private static final Logger LOG = OSGiLogger.getLogger(ContactWaitingResponseNotifyRequest.class, BundleLogger.LOG);

    private final SongHttpBinding songHttpBinding;
    private final LongPollingServer lpServer;
    private final String transactionId;
    private SongServletRequest request;

    /**
     * Builds a long polling waiting request.
     *
     * @param songHttpBinding The container that manages the SONG HTTP binding
     * @param lpServer The long polling that manages this waiting contact request
     * @param transactionId The transaction ID which uniquely identifies this waiting contact request
     * @param request The SONG request which is waiting in the long polling connection
     */
    public ContactWaitingResponseNotifyRequest(SongHttpBinding songHttpBinding, LongPollingServer lpServer, String transactionId,
            SongServletRequest request) {
        this.songHttpBinding = songHttpBinding;
        this.lpServer = lpServer;
        this.transactionId = transactionId;
        this.request = request;
    }

    /**
     * Gets the SONG request associated to this class.
     *
     * @return The SONG request associated to this class
     */
    public SongServletRequest getRequest() {
        return request;
    }

    /**
     * Gets the SONG request associated to this class.
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

    public void run() {
        long run = System.currentTimeMillis();
        try {
            lpServer.contactRequestExpiration();
            songHttpBinding.removePendingContactRequest(transactionId);
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
