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
 * id $Id: LongPollingClient.java 9625 2014-09-25 15:18:00Z JReich $
 * author $Author: JReich $
 * version $Revision: 9625 $
 * lastrevision $Date: 2014-09-25 17:18:00 +0200 (Thu, 25 Sep 2014) $
 * modifiedby $LastChangedBy: JReich $
 * lastmodified $LastChangedDate: 2014-09-25 17:18:00 +0200 (Thu, 25 Sep 2014) $
 */

package com.actility.m2m.servlet.song.http;

import java.io.IOException;
import java.net.InetAddress;
import java.net.URISyntaxException;
import java.util.Date;
import java.util.Random;
import java.util.Timer;

import javax.servlet.http.HttpServletResponse;

import org.apache.log4j.Level;
import org.apache.log4j.Logger;

import com.actility.m2m.http.HttpUtils;
import com.actility.m2m.http.client.ni.HttpClient;
import com.actility.m2m.http.client.ni.HttpClientException;
import com.actility.m2m.http.client.ni.HttpClientTransaction;
import com.actility.m2m.m2m.M2MException;
import com.actility.m2m.servlet.song.ChannelClientListener;
import com.actility.m2m.servlet.song.SongServletRequest;
import com.actility.m2m.servlet.song.SongServletResponse;
import com.actility.m2m.servlet.song.SongURI;
import com.actility.m2m.servlet.song.http.log.BundleLogger;
import com.actility.m2m.servlet.song.http.stats.LongPollingClientStatsImpl;
import com.actility.m2m.song.binding.http.LongPollingClientStats;
import com.actility.m2m.util.Profiler;
import com.actility.m2m.util.RunnableTimerTask;
import com.actility.m2m.util.log.OSGiLogger;
import com.actility.m2m.xo.XoException;

/**
 * Manages a client long polling connection.
 * <p>
 * Its goal is to send long polling HTTP request to the remote long polling URI and wait for an HTTP response embedding a
 * Contact request.
 * <p>
 * When a Contact request is received it is directly forwarded to the SONG router and this object immediately sends a new long
 * polling HTTP request to the remote long polling URI.
 *
 */
public final class LongPollingClient implements Runnable {
    private static final Logger LOG = OSGiLogger.getLogger(LongPollingClient.class, BundleLogger.LOG);

    private static final int MAX_ERRORS = 7;
    private static final long RETRY_TIMER = 5000L;
    private static final long RANDOM_RETRY_TIMER = 20000L;

    private final SongHttpBinding songHttpBinding;
    private final String longPollingURI;
    private final String requestingEntity;
    private final SongURI relatedRequestingEntity;
    private final SongURI relatedTargetID;
    private final InetAddress remoteAddress;
    private final int remotePort;
    private final Timer timerService;
    private final HttpClient httpClient;
    private final boolean cc;
    private final ChannelClientListener channelListener;

    private volatile boolean end;

    // Stats
    private long lastContactRequest;
    private long nbOfContactRequests;
    private long accOfContactRequests;
    private long accOfNormalEndContactRequests;
    private long accOfBadRequestEndContactRequests;
    private long accOfErrorEndContactRequests;
    private long lastContactResponse;
    private long accOfContactResponses;
    private long accOfErrorEndContactResponses;
    private long lastLongPollingRequest;
    private long nbOfLongPollingRequests;
    private long accOfLongPollingRequests;
    private long accOfContactEndLongPollingRequests;
    private long accOfEmptyEndLongPollingRequests;
    private long accOfErrorEndLongPollingRequests;
    private long accOfExceptionEndLongPollingRequests;
    private long accOfExpiredEndLongPollingRequests;
    private int nbErrors;
    private long nextExecution;
    private RunnableTimerTask timerTask;

    /**
     * Builds a client long polling thread.
     *
     * @param songHttpBinding The HTTP SONG binding which will be used to convert an HTTP response to a SONG request
     * @param timerService Service that permits to start timers
     * @param httpClient The HTTP client that will be used as a based
     * @param remoteAddress The long polling remote host address
     * @param remotePort The long polling remote port
     * @param longPollingURI The long polling URI
     * @param requestingEntity The requesting entity to use in long polling requests
     * @param relatedRequestingEntity Only relevant for &lt;notificationChannel&gt;. The requesting entity to use when generating the SONG request from the received &lt;notification&gt;
     * @param relatedTargetID Only relevant for &lt;notificationChannel&gt;. The target ID to use when generating the SONG request from the received &lt;notification&gt;
     * @param channelListener A listener that will receive events on the current channel lifecycle
     * @param cc Whether this is a &lt;communicationChannel&gt; (otherwise this is a &lt;notificationChannel&gt;)
     */
    public LongPollingClient(SongHttpBinding songHttpBinding, Timer timerService, HttpClient httpClient,
            InetAddress remoteAddress, int remotePort, String longPollingURI, String requestingEntity,
            SongURI relatedRequestingEntity, SongURI relatedTargetID, ChannelClientListener channelListener, boolean cc) {
        this.songHttpBinding = songHttpBinding;
        this.remoteAddress = remoteAddress;
        this.remotePort = remotePort;
        this.longPollingURI = longPollingURI;
        this.timerService = timerService;
        this.httpClient = httpClient;
        this.requestingEntity = requestingEntity;
        this.relatedRequestingEntity = relatedRequestingEntity;
        this.relatedTargetID = relatedTargetID;
        this.channelListener = channelListener;
        this.cc = cc;
        this.nbErrors = 0;
        this.timerTask = new RunnableTimerTask(this);
    }

    private void startRetryTimer(boolean error) {
        long waitDuration = RETRY_TIMER + (Math.abs(new Random().nextLong()) % (RANDOM_RETRY_TIMER + 1000L));
        synchronized (this) {
            if (error) {
                channelListener.channelError();
            }
            nextExecution = System.currentTimeMillis() + waitDuration;
        }
        if (LOG.isInfoEnabled()) {
            LOG.info(longPollingURI + ": Sleep " + waitDuration + "ms to retry connection");
        }
        timerTask = new RunnableTimerTask(this);
        timerService.schedule(timerTask, waitDuration);
    }

    /**
     * Stops the long polling client connection and wait to join the thread.
     */
    public void stopLongPolling() {
        if (LOG.isInfoEnabled()) {
            LOG.info(longPollingURI + ": Stopping client long polling connection");
        }
        end = true;
        timerTask.cancel();
        if (LOG.isInfoEnabled()) {
            LOG.info(longPollingURI + ": Client long polling connection is stopped: " + longPollingURI);
        }
    }

    public synchronized LongPollingClientStats getLongPollingClientStats() {
        long now = System.currentTimeMillis();
        return new LongPollingClientStatsImpl(new Date(lastContactRequest), nbOfContactRequests, accOfContactRequests,
                accOfNormalEndContactRequests, accOfBadRequestEndContactRequests, accOfErrorEndContactRequests, new Date(
                        lastContactResponse), accOfContactResponses, accOfErrorEndContactResponses, new Date(
                        lastLongPollingRequest), nbOfLongPollingRequests, accOfLongPollingRequests,
                accOfContactEndLongPollingRequests, accOfEmptyEndLongPollingRequests, accOfErrorEndLongPollingRequests,
                accOfExceptionEndLongPollingRequests, accOfExpiredEndLongPollingRequests, (nbErrors > 0) ? "ERROR (" + nbErrors
                        + ") " + ((nextExecution > now) ? "(while be resumed in " + (nextExecution - now) + "ms)" : "") : "OK");
    }

    public synchronized void sentContactResponse() {
        lastContactResponse = System.currentTimeMillis();
        ++accOfContactResponses;
        --nbOfContactRequests;
        ++accOfNormalEndContactRequests;
    }

    public synchronized void errorContactResponse() {
        lastContactResponse = System.currentTimeMillis();
        ++accOfContactResponses;
        --nbOfContactRequests;
        ++accOfErrorEndContactResponses;
    }

    /**
     * Sends a long polling request to the server
     */
    public void sendLongPollingRequest() {
        boolean wait = false;
        boolean sent = false;
        int nbErrors = this.nbErrors;
        try {
            LOG.debug("Create long polling POST request");
            HttpClientTransaction httpTransaction = httpClient.createTransaction(HttpUtils.MD_POST, longPollingURI);
            httpTransaction.addRequestHeader(HttpUtils.HD_CONTENT_LENGTH, "0");
            httpTransaction.addRequestHeader(HttpUtils.HD_FROM, requestingEntity);

            // Execute the request
            if (Profiler.getInstance().isTraceEnabled()) {
                Profiler.getInstance().trace(
                        longPollingURI + ": >>> HTTP.HTTPBinding: POST " + longPollingURI + " (Long Polling Request)");
            }
            if (LOG.isInfoEnabled()) {
                LOG.info(longPollingURI + ": >>> HTTP.HTTPBinding: POST " + longPollingURI + " (Long Polling Request)");
            }
            // if (LOG.isDebugEnabled()) {
            // LOG.debug(SongHttpBinding.getClientHttpRequestAsString(httpTransaction));
            // }
            synchronized (this) {
                lastLongPollingRequest = System.currentTimeMillis();
                ++nbOfLongPollingRequests;
                ++accOfLongPollingRequests;
            }
            sent = true;
            songHttpBinding.sendClientRequest(httpTransaction, this);
        } catch (HttpClientException e) {
            LOG.error(longPollingURI + ": HTTP problem between client and server in long polling connection", e);
            wait = true;
            this.nbErrors += MAX_ERRORS + 1;
            synchronized (this) {
                if (sent) {
                    --nbOfLongPollingRequests;
                }
                ++accOfExceptionEndLongPollingRequests;
            }
        } catch (RuntimeException e) {
            LOG.error(longPollingURI + ": Long polling runtime exception", e);
            wait = true;
            this.nbErrors += MAX_ERRORS + 1;
            synchronized (this) {
                if (sent) {
                    --nbOfLongPollingRequests;
                }
                ++accOfExceptionEndLongPollingRequests;
            }
        } catch (Throwable e) {
            LOG.error(longPollingURI + ": Long polling fatal error", e);
            wait = true;
            this.nbErrors += MAX_ERRORS + 1;
            synchronized (this) {
                if (sent) {
                    --nbOfLongPollingRequests;
                }
                ++accOfExceptionEndLongPollingRequests;
            }
        } finally {
            if (!end && wait) {
                startRetryTimer(nbErrors <= MAX_ERRORS && this.nbErrors > MAX_ERRORS);
            }
        }
    }

    private void handleLongPollingResponse(HttpClientTransaction httpTransaction) {
        if (LOG.isInfoEnabled()) {
            LOG.info(longPollingURI + ": Handle received long polling HTTP response");
        }
        SongServletRequest songRequest = null;
        if (songHttpBinding.checkHttpResponse(httpTransaction)) {
            if (httpTransaction.getResponseContentLength() > 0) {
                synchronized (this) {
                    lastContactRequest = System.currentTimeMillis();
                    ++accOfContactRequests;
                }

                try {
                    if (cc) {
                        songRequest = songHttpBinding.buildSongRequestFromHttpCCLongPollingResponse(this, httpTransaction,
                                remoteAddress, remotePort);
                    } else {
                        songRequest = songHttpBinding.buildSongRequestFromHttpNCLongPollingResponse(this, httpTransaction,
                                remoteAddress, remotePort, relatedRequestingEntity, relatedTargetID);
                    }
                    synchronized (this) {
                        ++accOfContactEndLongPollingRequests;
                    }
                } catch (URISyntaxException e) {
                    synchronized (this) {
                        ++accOfBadRequestEndContactRequests;
                    }
                    LOG.warn("Bad request-uri in received long polling response", e);
                } catch (IOException e) {
                    synchronized (this) {
                        ++accOfErrorEndContactRequests;
                    }
                    LOG.warn("Cannot create SONG request from HTTP long polling response", e);
                } catch (XoException e) {
                    synchronized (this) {
                        ++accOfBadRequestEndContactRequests;
                    }
                    if (LOG.isEnabledFor(Level.WARN)) {
                        LOG.warn("Cannot create SONG request from HTTP long polling response", e);
                    }
                } catch (M2MException e) {
                    synchronized (this) {
                        ++accOfBadRequestEndContactRequests;
                    }
                    if (LOG.isEnabledFor(Level.WARN)) {
                        LOG.warn("Cannot create SONG request from HTTP long polling response", e);
                    }
                }
            } else {
                synchronized (this) {
                    ++accOfEmptyEndLongPollingRequests;
                }
            }
        } else {
            LOG.error("Received an HTTP Long polling response which could not be accepted");
            synchronized (this) {
                ++accOfErrorEndLongPollingRequests;
            }
        }
        if (songRequest != null) {
            try {
                if (LOG.isInfoEnabled()) {
                    LOG.info(songRequest.getId() + ": <<< HTTP.SONGBinding: " + songRequest.getMethod()
                            + " (requestingEntity: " + songRequest.getRequestingEntity().absoluteURI() + ") (targetID: "
                            + songRequest.getTargetID().absoluteURI() + ")");
                }
                songRequest.send();
                synchronized (this) {
                    ++nbOfContactRequests;
                }
            } catch (IOException e) {
                synchronized (this) {
                    ++accOfErrorEndContactRequests;
                }
                LOG.warn("Cannot send the received SONG request", e);
            }
        }
    }

    /**
     * Callback to notify a long polling response received from the server
     *
     * @param httpTransaction The HTTP client transaction containing the long polling HTTP response
     */
    public void receivedLongPollingResponse(HttpClientTransaction httpTransaction) {
        boolean wait = false;
        int nbErrors = this.nbErrors;
        try {
            synchronized (this) {
                --nbOfLongPollingRequests;
            }
            int statusCode = httpTransaction.getResponseStatusCode();

            if (Profiler.getInstance().isTraceEnabled()) {
                Profiler.getInstance().trace(
                        longPollingURI + ": <<< HTTP.HTTPBinding: " + httpTransaction.getResponseStatusCode() + " "
                                + httpTransaction.getResponseStatusText() + " (Long Polling Response)");
            }
            if (LOG.isInfoEnabled()) {
                LOG.info(longPollingURI + ": <<< HTTP.HTTPBinding: " + httpTransaction.getResponseStatusCode() + " "
                        + httpTransaction.getResponseStatusText() + " (Long Polling Response)");
            }
            if (statusCode == HttpServletResponse.SC_OK) {
                if (LOG.isDebugEnabled()) {
                    LOG.debug(SongHttpBinding.getClientHttpResponseAsString(httpTransaction));
                }
                handleLongPollingResponse(httpTransaction);
                synchronized (this) {
                    if (this.nbErrors > MAX_ERRORS) {
                        channelListener.channelOk();
                    }
                    this.nbErrors = 0;
                }
            } else {
                synchronized (this) {
                    if (statusCode >= HttpServletResponse.SC_MULTIPLE_CHOICES) {
                        wait = true;
                        ++accOfErrorEndLongPollingRequests;
                        if (statusCode == SongServletResponse.SC_REQUEST_TIMEOUT
                                || statusCode == SongServletResponse.SC_GATEWAY_TIMEOUT) {
                            ++this.nbErrors;
                        } else {
                            this.nbErrors += MAX_ERRORS + 1;
                        }
                    } else {
                        ++accOfEmptyEndLongPollingRequests;
                        if (this.nbErrors > MAX_ERRORS) {
                            channelListener.channelOk();
                        }
                        this.nbErrors = 0;
                    }
                }
            }
        } catch (RuntimeException e) {
            LOG.error(longPollingURI + ": Long polling runtime exception", e);
            wait = true;
            this.nbErrors += MAX_ERRORS + 1;
            synchronized (this) {
                ++accOfExceptionEndLongPollingRequests;
            }
        } catch (Throwable e) {
            LOG.error(longPollingURI + ": Long polling fatal error", e);
            wait = true;
            this.nbErrors += MAX_ERRORS + 1;
            synchronized (this) {
                ++accOfExceptionEndLongPollingRequests;
            }
        } finally {
            if (!end) {
                if (wait) {
                    startRetryTimer(nbErrors <= MAX_ERRORS && this.nbErrors > MAX_ERRORS);
                } else {
                    // Send long polling request as soon as possible
                    sendLongPollingRequest();
                }
            }
        }
    }

    /**
     * Callback to notify the long polling retry timer expiration.
     * <p>
     * Sends a new long polling HTTP request
     */
    public void run() {
        sendLongPollingRequest();
    }
}
