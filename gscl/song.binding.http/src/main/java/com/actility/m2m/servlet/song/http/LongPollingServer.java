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
 * id $Id: LongPollingServer.java 8521 2014-04-14 09:05:59Z JReich $
 * author $Author: JReich $
 * version $Revision: 8521 $
 * lastrevision $Date: 2014-04-14 11:05:59 +0200 (Mon, 14 Apr 2014) $
 * modifiedby $LastChangedBy: JReich $
 * lastmodified $LastChangedDate: 2014-04-14 11:05:59 +0200 (Mon, 14 Apr 2014) $
 */

package com.actility.m2m.servlet.song.http;

import java.io.IOException;
import java.util.Date;
import java.util.Timer;

import javax.servlet.http.HttpServletResponse;

import org.apache.log4j.Logger;

import com.actility.m2m.http.server.HttpServerTransaction;
import com.actility.m2m.servlet.song.SongServletMessage;
import com.actility.m2m.servlet.song.SongServletRequest;
import com.actility.m2m.servlet.song.SongServletResponse;
import com.actility.m2m.servlet.song.http.log.BundleLogger;
import com.actility.m2m.servlet.song.http.stats.LongPollingServerStatsImpl;
import com.actility.m2m.song.binding.http.LongPollingServerStats;
import com.actility.m2m.util.UUID;
import com.actility.m2m.util.log.OSGiLogger;

/**
 * Manages a server long polling connection.
 * <p>
 * Its goal is to hold waiting long polling HTTP request and wait for a Contact request request.
 * <p>
 * When a Contact request is received it is directly sent embedded in the long polling response
 * <p>
 * If no Contact request is received before the long polling request waiting timer, then an empty long polling response is sent
 *
 */
public final class LongPollingServer {
    private static final Logger LOG = OSGiLogger.getLogger(LongPollingServer.class, BundleLogger.LOG);

    private final SongHttpBinding songHttpBinding;
    private final String longPollingId;
    private final ContactWaitingLongPollingRequest[] queue;
    private final Timer timerService;
    private long contactRequestWaitingTimer;
    private long longPollingServerRequestExpirationTimer;
    private int readIndex;
    private int writeIndex;
    private int index;
    private HttpServerTransaction longPollingWaitingRequest;
    private long longPollingRequestReceptionTime;

    private long lastContactRequest;
    private long nbOfContactRequests;
    private long nbOfWaitingContactRequests;
    private long nbOfWaitingResponseContactRequests;
    private long accOfContactRequests;
    private long accOfNormalEndContactRequests;
    private long accOfErrorEndContactRequests;
    private long accOfExpiredEndContactRequests;
    private long accOfExpiredWaitingEndContactRequests;
    private long accOfServerFullEndContactRequests;
    private long lastContactResponse;
    private long accOfContactResponses;
    private long lastLongPollingRequest;
    private long nbOfLongPollingRequests;
    private long accOfLongPollingRequests;
    private long accOfContactEndLongPollingRequests;
    private long accOfErrorEndLongPollingRequests;
    private long accOfConcurrentEndLongPollingRequests;
    private long accOfExpiredEndLongPollingRequests;

    /**
     * Builds a server long polling thread.
     *
     * @param songHttpBinding The HTTP SONG binding
     * @param timerService Service that permits to start timers
     * @param longPollingId Unique ID of this long polling server
     * @param contactRequestWaitingTimer The time to wait a long polling server request before to consider a contact request as
     *            expired
     * @param longPollingServerRequestExpirationTimer The time to wait a contact request to send in a long polling response
     *            before to consider a long polling request as expired
     */
    public LongPollingServer(SongHttpBinding songHttpBinding, Timer timerService, String longPollingId,
            long contactRequestWaitingTimer, long longPollingServerRequestExpirationTimer) {
        this.songHttpBinding = songHttpBinding;
        this.timerService = timerService;
        this.longPollingId = longPollingId;
        this.queue = new ContactWaitingLongPollingRequest[33];
        this.readIndex = 0;
        this.writeIndex = 0;
        this.contactRequestWaitingTimer = contactRequestWaitingTimer;
        this.longPollingServerRequestExpirationTimer = longPollingServerRequestExpirationTimer;
        this.index = -1;
    }

    /**
     * Gets the long polling server unique ID
     *
     * @return The long polling server unique ID
     */
    public String getId() {
        return longPollingId;
    }

    /**
     * Gets the number of waiting contact requests in the long polling server queue.
     *
     * @return The number of waiting contact requests in the long polling server queue
     */
    public int getCount() {
        return (writeIndex - readIndex + queue.length) % queue.length;
    }

    /**
     * Gets the index associated to the long polling server
     * <p>
     * This index is the position of the long polling server into the list of all waiting long polling servers. This global list
     * allow to manage long polling request expiration with only one timer instead of using one timer per long polling server
     *
     * @return The index associated to the long polling server
     */
    public int getIndex() {
        return index;
    }

    /**
     * Sets the index of the long polling server
     * <p>
     * This index is the position of the long polling server into the list of all waiting long polling servers. This global list
     * allow to manage long polling request expiration with only one timer instead of using one timer per long polling server
     *
     * @param index The index of the long polling server
     */
    public void setIndex(int index) {
        this.index = index;
    }

    /**
     * Checks whether the long polling server is expired according to the given date.
     * <p>
     * In case the long polling server is expired, this method sends an empty 202 Accepted response
     *
     * @param now The reference date that is used to determinate if the long polling server is expired
     * @return Whether the long polling server is expired
     */
    public synchronized boolean checkExpired(long now) {
        HttpServerTransaction httpTransaction = null;
        if ((now - longPollingRequestReceptionTime) >= longPollingServerRequestExpirationTimer) {
            if (longPollingWaitingRequest != null) {
                httpTransaction = longPollingWaitingRequest;
                longPollingWaitingRequest = null;
                index = -1;
            }
        }

        if (httpTransaction != null) {
            try {
                if (LOG.isInfoEnabled()) {
                    LOG.info(longPollingId + ": No SONG request received on long polling connection, send 202 ("
                            + httpTransaction.getId() + ")");
                }
                songHttpBinding.initHttpLongPollingResponse(HttpServletResponse.SC_ACCEPTED, "Accepted", httpTransaction);
                ++accOfExpiredEndLongPollingRequests;
            } catch (IOException e) {
                ++accOfErrorEndLongPollingRequests;
                LOG.error(longPollingId + ": Cannot send Accepted response to long polling client", e);
            }
            return true;
        }
        if (LOG.isDebugEnabled()) {
            LOG.debug(longPollingId + ": Long polling request is not expired");
        }
        return false;
    }

    /**
     * Callback to notify the reception of a contact request
     *
     * @param songRequest The contact request received from the SONG container
     * @throws IOException If any problem occurs while processing the contact request
     */
    public synchronized void contactRequestReceived(SongServletRequest songRequest) throws IOException {
        lastContactRequest = System.currentTimeMillis();
        ++accOfContactRequests;
        HttpServerTransaction httpTransaction = null;
        ContactWaitingLongPollingRequest waitingRequest = null;
        boolean inserted = false;
        if (longPollingWaitingRequest != null) {
            httpTransaction = longPollingWaitingRequest;
            songHttpBinding.removeWaitingLongPollingServer(this);
            longPollingWaitingRequest = null;
        } else {
            waitingRequest = new ContactWaitingLongPollingRequest(this, songRequest);
            inserted = offer(waitingRequest);
        }

        if (httpTransaction != null) {
            String transactionId = UUID.randomUUID(40);
            songHttpBinding.addPendingContactRequest(this, transactionId, songRequest);
            try {
                songHttpBinding.buildHttpLongPollingResponseFromSongRequest(transactionId, httpTransaction, songRequest);
                ++accOfContactEndLongPollingRequests;
                ++nbOfContactRequests;
                ++nbOfWaitingResponseContactRequests;
            } catch (IOException e) {
                ++accOfErrorEndLongPollingRequests;
                ++accOfErrorEndContactRequests;
                songHttpBinding.removePendingContactRequest(transactionId);
                throw e;
            } catch (RuntimeException e) {
                ++accOfErrorEndLongPollingRequests;
                ++accOfErrorEndContactRequests;
                songHttpBinding.removePendingContactRequest(transactionId);
                throw e;
            }
        } else if (inserted) {
            ++nbOfContactRequests;
            ++nbOfWaitingContactRequests;
            timerService.schedule(waitingRequest, contactRequestWaitingTimer);
        } else {
            ++accOfServerFullEndContactRequests;
            LOG.error(longPollingId + ": Long polling waiting queue is full");
            SongServletResponse songResponse = songRequest.createResponse(SongServletResponse.SC_SERVICE_UNAVAILABLE);
            songResponse.addHeader(SongServletMessage.HD_RETRY_AFTER, "5");
            if (LOG.isInfoEnabled()) {
                LOG.info(songRequest.getId() + ": <<< HTTP.SONGBinding: 503 Service Unavailable");
            }
            songResponse.send();
        }
    }

    /**
     * Callback to notify the reception of a contact response
     *
     * @param songResponse The contact response received from the long polling connection
     * @throws IOException If any problem occurs while processing the contact response
     */
    public synchronized void contactResponseReceived(SongServletResponse songResponse) throws IOException {
        lastContactResponse = System.currentTimeMillis();
        ++accOfContactResponses;
        --nbOfWaitingResponseContactRequests;
        --nbOfContactRequests;
        if (LOG.isInfoEnabled()) {
            LOG.info(songResponse.getId() + ": <<< HTTP.SONGBinding: " + songResponse.getStatus() + " "
                    + songResponse.getReasonPhrase());
        }
        try {
            songResponse.send();
            ++accOfNormalEndContactRequests;
        } catch (IOException e) {
            ++accOfErrorEndContactRequests;
            throw e;
        }
    }

    public synchronized void errorContactResponseReceived(SongServletResponse songResponse) throws IOException {
        lastContactResponse = System.currentTimeMillis();
        ++accOfContactResponses;
        --nbOfWaitingResponseContactRequests;
        --nbOfContactRequests;
        if (LOG.isInfoEnabled()) {
            LOG.info(songResponse.getId() + ": <<< HTTP.SONGBinding: " + songResponse.getStatus() + " "
                    + songResponse.getReasonPhrase());
        }
        ++accOfErrorEndContactRequests;
        songResponse.send();
    }

    /**
     * Callback to notify the reception of a long polling request
     *
     * @param httpTransaction The HTTP server transaction embedding the long polling request
     * @throws IOException If any problem occurs while processing the long polling request
     */
    public synchronized void longPollingRequestReceived(HttpServerTransaction httpTransaction) throws IOException {
        if (LOG.isInfoEnabled()) {
            LOG.info(longPollingId + ": Long polling received (" + httpTransaction.getId() + ")");
        }
        lastLongPollingRequest = System.currentTimeMillis();
        ++accOfLongPollingRequests;
        ContactWaitingLongPollingRequest waitingRequest = null;
        HttpServerTransaction previousLongPollingRequest = null;
        boolean waiting = false;
        long previousLongPollingDuration = 0L;

        previousLongPollingRequest = longPollingWaitingRequest;
        previousLongPollingDuration = longPollingRequestReceptionTime;
        longPollingRequestReceptionTime = System.currentTimeMillis();
        previousLongPollingDuration = longPollingRequestReceptionTime - previousLongPollingDuration;
        if (previousLongPollingRequest != null) {
            songHttpBinding.removeWaitingLongPollingServer(this);
        }
        waitingRequest = poll();
        if (waitingRequest == null) {
            // No waiting contact request to forward
            // Buffer longPollServer for expiration
            if (index != -1) {
                LOG.error(longPollingId
                        + ": Long polling server is still registered to the list of waiting servers while it must not");
                songHttpBinding.removeWaitingLongPollingServer(this);
            }
            if (!songHttpBinding.addWaitingLongPollingServer(this)) {
                longPollingWaitingRequest = null;
            } else {
                ++nbOfLongPollingRequests;
                waiting = true;
                longPollingWaitingRequest = httpTransaction;
            }
        } else {
            longPollingWaitingRequest = null;
        }

        if (previousLongPollingRequest != null) {
            LOG.error(longPollingId
                    + ": No SONG request received on long polling connection, send 202 to a previous long polling request ("
                    + previousLongPollingRequest.getId() + "). Duration between requests is " + previousLongPollingDuration
                    + "ms");
            try {
                --nbOfLongPollingRequests;
                songHttpBinding.initHttpLongPollingResponse(HttpServletResponse.SC_ACCEPTED, "Accepted",
                        previousLongPollingRequest);
                ++accOfConcurrentEndLongPollingRequests;
            } catch (IOException e) {
                ++accOfErrorEndLongPollingRequests;
                LOG.error(longPollingId + ": IOException while trying to respond a 202 to a previous long polling request", e);
            }
        }
        // Get waiting request
        if (waitingRequest != null) {
            if (waitingRequest.isFlushRequest()) {
                // Flush the long polling request
                LOG.error(longPollingId + ": Long polling server is stopped, send 404 (" + httpTransaction.getId() + ")");
                ++accOfErrorEndLongPollingRequests;
                songHttpBinding.initHttpLongPollingResponse(HttpServletResponse.SC_NOT_FOUND, "Not Found", httpTransaction);
            } else {
                if (LOG.isInfoEnabled()) {
                    LOG.info(longPollingId + ": Waiting request found");
                }
                SongServletRequest songRequest = waitingRequest.popRequest();
                String transactionId = UUID.randomUUID(40);
                songHttpBinding.addPendingContactRequest(this, transactionId, songRequest);
                try {
                    --nbOfWaitingContactRequests;
                    songHttpBinding.buildHttpLongPollingResponseFromSongRequest(transactionId, httpTransaction, songRequest);
                    ++accOfContactEndLongPollingRequests;
                    ++nbOfWaitingResponseContactRequests;
                } catch (IOException e) {
                    // TODO should respond to contact request
                    ++accOfErrorEndContactRequests;
                    ++accOfErrorEndLongPollingRequests;
                    songHttpBinding.removePendingContactRequest(transactionId);
                    throw e;
                } catch (RuntimeException e) {
                    // TODO should respond to contact request
                    ++accOfErrorEndContactRequests;
                    ++accOfErrorEndLongPollingRequests;
                    songHttpBinding.removePendingContactRequest(transactionId);
                    throw e;
                }
            }
        } else if (!waiting) {
            LOG.error(httpTransaction.getId()
                    + ": Too much pending long polling request in server. Reject long polling with 503");
            ++accOfErrorEndLongPollingRequests;
            songHttpBinding.initHttpLongPollingResponse(HttpServletResponse.SC_SERVICE_UNAVAILABLE, "Service Unavailable",
                    httpTransaction);
        }
    }

    public synchronized void waitingContactRequestExpiration() {
        --nbOfContactRequests;
        --nbOfWaitingContactRequests;
        ++accOfExpiredWaitingEndContactRequests;
    }

    public synchronized void contactRequestExpiration() {
        --nbOfWaitingResponseContactRequests;
        --nbOfContactRequests;
        ++accOfExpiredEndContactRequests;
    }

    /**
     * Inserts a waiting contact request to the internal queue of the long polling server.
     * <p>
     * Returns directly if the array is full.
     *
     * @param o The waiting contact request to add to the internal queue
     * @return Whether the waiting contact request has been added
     */
    public boolean offer(ContactWaitingLongPollingRequest o) {
        boolean inserted = false;
        int newIndex = (writeIndex + 1) % queue.length;
        if (newIndex != readIndex) {
            queue[writeIndex] = o;
            writeIndex = newIndex;
            inserted = true;
        }
        return inserted;
    }

    /**
     * Retrieves a waiting contact request from the internal queue.
     * <p>
     * This call returns null if the queue is empty.
     *
     * @return The retrieved waiting contact request or null if the queue is empty
     */
    public ContactWaitingLongPollingRequest poll() {
        ContactWaitingLongPollingRequest result = null;
        while (result == null && readIndex != writeIndex) {
            result = queue[readIndex];
            queue[readIndex] = null;
            readIndex = (readIndex + 1) % queue.length;
            if (!result.cancel()) {
                result = null;
            }
        }
        return result;
    }

    public synchronized LongPollingServerStats getLongPollingServerStats() {
        return new LongPollingServerStatsImpl(new Date(lastContactRequest), nbOfContactRequests, nbOfWaitingContactRequests,
                nbOfWaitingResponseContactRequests, accOfContactRequests, accOfNormalEndContactRequests,
                accOfErrorEndContactRequests, accOfExpiredEndContactRequests, accOfExpiredWaitingEndContactRequests,
                accOfServerFullEndContactRequests, new Date(lastContactResponse), accOfContactResponses, new Date(
                        lastLongPollingRequest), nbOfLongPollingRequests, accOfLongPollingRequests,
                accOfContactEndLongPollingRequests, accOfErrorEndLongPollingRequests, accOfConcurrentEndLongPollingRequests,
                accOfExpiredEndLongPollingRequests);
    }
}
