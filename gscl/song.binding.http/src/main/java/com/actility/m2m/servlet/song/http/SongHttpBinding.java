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
 * id $Id: SongHttpBinding.java 8679 2014-04-30 15:24:21Z JReich $
 * author $Author: JReich $
 * version $Revision: 8679 $
 * lastrevision $Date: 2014-04-30 17:24:21 +0200 (Wed, 30 Apr 2014) $
 * modifiedby $LastChangedBy: JReich $
 * lastmodified $LastChangedDate: 2014-04-30 17:24:21 +0200 (Wed, 30 Apr 2014) $
 */

package com.actility.m2m.servlet.song.http;

import java.io.IOException;
import java.io.UnsupportedEncodingException;
import java.net.InetAddress;
import java.net.Socket;
import java.net.URISyntaxException;
import java.net.URLDecoder;
import java.net.UnknownHostException;
import java.text.ParseException;
import java.util.ArrayList;
import java.util.Date;
import java.util.HashMap;
import java.util.Iterator;
import java.util.List;
import java.util.ListIterator;
import java.util.Map;
import java.util.Map.Entry;
import java.util.SortedSet;
import java.util.Timer;
import java.util.TreeSet;

import javax.servlet.ServletContext;
import javax.servlet.ServletException;
import javax.servlet.http.HttpServletResponse;

import org.apache.log4j.Logger;

import com.actility.m2m.http.HttpUtils;
import com.actility.m2m.http.client.ni.HttpClient;
import com.actility.m2m.http.client.ni.HttpClientException;
import com.actility.m2m.http.client.ni.HttpClientHandler;
import com.actility.m2m.http.client.ni.HttpClientTransaction;
import com.actility.m2m.http.server.HttpServerHandler;
import com.actility.m2m.http.server.HttpServerTransaction;
import com.actility.m2m.m2m.M2MConstants;
import com.actility.m2m.m2m.M2MException;
import com.actility.m2m.m2m.M2MService;
import com.actility.m2m.m2m.StatusCode;
import com.actility.m2m.servlet.song.LongPollURIs;
import com.actility.m2m.servlet.song.SongBindingFactory;
import com.actility.m2m.servlet.song.SongServlet;
import com.actility.m2m.servlet.song.SongServletMessage;
import com.actility.m2m.servlet.song.SongServletRequest;
import com.actility.m2m.servlet.song.SongServletResponse;
import com.actility.m2m.servlet.song.SongURI;
import com.actility.m2m.servlet.song.http.log.BundleLogger;
import com.actility.m2m.song.binding.http.LongPollingClientStats;
import com.actility.m2m.song.binding.http.LongPollingServerStats;
import com.actility.m2m.song.binding.http.SongHttpBindingStatsService;
import com.actility.m2m.util.CharacterUtils;
import com.actility.m2m.util.FormatUtils;
import com.actility.m2m.util.MultiMap;
import com.actility.m2m.util.Profiler;
import com.actility.m2m.util.StringUtils;
import com.actility.m2m.util.URIUtils;
import com.actility.m2m.util.UUID;
import com.actility.m2m.util.log.OSGiLogger;
import com.actility.m2m.xo.XoException;
import com.actility.m2m.xo.XoObject;
import com.actility.m2m.xo.XoService;

/**
 * Concrete binding which manages translation between HTTP messages and SONG messages.
 */
public final class SongHttpBinding extends SongServlet implements HttpServerHandler, SongHttpBindingStatsService,
        HttpClientHandler {
    private static final Logger LOG = OSGiLogger.getLogger(SongHttpBinding.class, BundleLogger.LOG);

    /**
     *
     */
    private static final long serialVersionUID = -2067331068203764045L;

    private static final String HTTP_DEFAULT_CONTENT_TYPE = "application/octet-stream";

    private static final String HTTP_AT_LONG_POLLING_SERVER = "com.actility.m2m.servlet.song.http.LongPollingServer";
    private static final String HTTP_AT_LONG_POLLING_CLIENT = "com.actility.m2m.servlet.song.http.LongPollingClient";
    private static final String HTTP_AT_TRANSACTION_ID = "com.actility.m2m.servlet.song.http.TransactionId";
    private static final String HTTP_AT_HTTP_TRANSACTION = "com.actility.m2m.servlet.song.http.HttpTransaction";
    // private static final String HTTP_AT_CONTACT_URI = "com.actility.m2m.servlet.song.http.ContactUri";
    // private static final String HTTP_AT_CORRELATION_ID = "com.actility.m2m.servlet.song.http.CorrelationId";

    public static final long L_BASIC = URIUtils.L_UNRESERVED | CharacterUtils.L_ESCAPED
            | CharacterUtils.lowMask(".?@&=+$,;/[]");
    public static final long H_BASIC = URIUtils.H_UNRESERVED | CharacterUtils.H_ESCAPED
            | CharacterUtils.highMask(".?@&=+$,;/[]");

    // /**
    // * Special HTTP header to declare the contact to which the ETSI response will be sent in an asynchronous dialog.
    // */
    // public static final String HTTP_HD_ETSI_CONTACT_URI = "X-etsi-contactURI";
    // /**
    // * Special HTTP header to declare an id to map an ETSI request to an ETSI response in an asynchronous dialog.
    // */
    // public static final String HTTP_HD_ETSI_CORRELATION_ID = "X-etsi-correlationID";
    /**
     * Special HTTP header to declare the original IP of HTTP request if it is treated by a proxy.
     */
    public static final String HTTP_HD_FORWARD_FOR = "X-Forwarded-For";
    /**
     * Special HTTP header to embed the SONG method of a SONG request in a long polling HTTP response.
     */
    public static final String HTTP_HD_METHOD = "X-Acy-Method";
    /**
     * Special HTTP header to embed a transaction ID in the long polling HTTP response and the long polling HTTP request to
     * correlate the SONG request to its SONG response.
     */
    public static final String HTTP_HD_TRANSACTION_ID = "X-Acy-Transaction-ID";
    /**
     * Special HTTP header to embed the Target-ID of a SONG request in a long polling HTTP response. It is also used to change
     * the targetID as received from the HTTP request
     */
    public static final String HTTP_HD_TARGET_ID = "X-Acy-Target-ID";
    /**
     * Special HTTP header to embed the Requesting-Entity of a SONG request in a long polling HTTP response.
     */
    public static final String HTTP_HD_REQUESTING_ENTITY = "X-Acy-Requesting-Entity";
    /**
     * Special HTTP header to embed the status code of a SONG response in a long polling HTTP request.
     */
    public static final String HTTP_HD_STATUS_CODE = "X-Acy-Status-Code";
    /**
     * Special HTTP header to embed the status code of a SONG response in a long polling HTTP request.
     */
    public static final String HTTP_HD_NEXT_HOP_URI = "X-Acy-Next-Hop-URI";
    /**
     * Query parameter to override the targetID as received from the HTTP request
     */
    public static final String HTTP_QUERY_TARGET_ID = "targetID";
    /**
     * Query parameter to override declare the targetID as server or proxy
     */
    public static final String HTTP_QUERY_MODE = "mode";

    /**
     * Serialize the given server http request.
     *
     * @param httpTransaction The http request to serialize
     * @return The serialized http request
     */
    public static String getServerHttpRequestAsString(HttpServerTransaction httpTransaction) {
        StringBuffer buffer = new StringBuffer();
        buffer.append(httpTransaction.getMethod());
        buffer.append(' ');
        buffer.append(httpTransaction.getRawRequestUri());
        buffer.append(' ');
        buffer.append(httpTransaction.getHttpVersion());

        MultiMap headers = httpTransaction.getHeaders();
        Iterator it = headers.entrySet().iterator();
        Entry entry = null;
        while (it.hasNext()) {
            entry = (Entry) it.next();
            buffer.append((String) entry.getKey());
            buffer.append(": ");
            buffer.append((String) entry.getValue());
            buffer.append('\n');
        }
        buffer.append('\n');

        return buffer.toString();
    }

    /**
     * Serialize the given server http response.
     *
     * @param httpTransaction The HTTP transaction
     * @param statusCode The status code of the http response
     * @param reasonPhrase The reason phrase of the http response
     * @return The serialized http response
     */
    public static String getServerHttpResponseAsString(HttpServerTransaction httpTransaction, int statusCode,
            String reasonPhrase) {
        StringBuffer buffer = new StringBuffer();
        buffer.append(httpTransaction.getHttpVersion());
        buffer.append(' ');
        buffer.append(statusCode);
        buffer.append(' ');
        buffer.append(reasonPhrase);

        return buffer.toString();
    }

    // /**
    // * Serialize the given client http request.
    // *
    // * @param httpRequest The http request to serialize
    // * @return The serialized http request
    // */
    // public static String getClientHttpRequestAsString(HttpClientTransaction httpRequest) {
    // StringBuffer buffer = new StringBuffer();
    // if (httpRequest.getClass() == GetMethod.class) {
    // buffer.append("GET");
    // } else if (httpRequest.getClass() == PostMethod.class) {
    // buffer.append("POST");
    // } else if (httpRequest.getClass() == PutMethod.class) {
    // buffer.append("PUT");
    // } else if (httpRequest.getClass() == DeleteMethod.class) {
    // buffer.append("DELETE");
    // }
    // buffer.append(' ');
    // try {
    // buffer.append(httpRequest.getURI());
    // } catch (URIException e) {
    // buffer.append("<< uri exception >>");
    // }
    // buffer.append(" HTTP/1.1\n");
    //
    // Header[] headers = httpRequest.getRequestHeaders();
    // Header header = null;
    // for (int i = 0; i < headers.length; ++i) {
    // header = headers[i];
    // buffer.append(header.getName());
    // buffer.append(": ");
    // buffer.append(header.getValue());
    // buffer.append('\n');
    // }
    // buffer.append('\n');
    //
    // // if (httpRequest instanceof EntityEnclosingMethod) {
    // // EntityEnclosingMethod entityEnclosing = (EntityEnclosingMethod) httpRequest;
    // // if (ByteArrayRequestEntity.class == entityEnclosing.getRequestEntity().getClass()) {
    // // ByteArrayRequestEntity entity = (ByteArrayRequestEntity) entityEnclosing.getRequestEntity();
    // // if (entity.getContentLength() > 0) {
    // // byte[] content = entity.getContent();
    // // String contentType = entity.getContentType();
    // // if (contentType.startsWith("text/")) {
    // // String charset = "UTF-8";
    // // int index = contentType.indexOf("charset");
    // // if (index != -1) {
    // // index = contentType.indexOf('=', index + 7);
    // // if (index != -1) {
    // // int beginIndex = index + 1;
    // // index = contentType.indexOf(';', beginIndex);
    // // if (index != -1) {
    // // charset = contentType.substring(beginIndex, index).trim();
    // // } else {
    // // charset = contentType.substring(beginIndex).trim();
    // // }
    // // }
    // // }
    // // try {
    // // buffer.append(new String(content, charset));
    // // } catch (UnsupportedEncodingException e) {
    // // buffer.append("<< unsupported encoding >>");
    // // }
    // // } else {
    // // buffer.append("<< binary content >>");
    // // }
    // // }
    // // }
    // // }
    //
    // return buffer.toString();
    // }

    /**
     * Serialize the given http response.
     *
     * @param httpResponse The http response to serialize
     * @return The serialized http response
     */
    public static String getClientHttpResponseAsString(HttpClientTransaction httpResponse) {
        StringBuffer buffer = new StringBuffer();
        buffer.append("HTTP/1.1 ");
        buffer.append(httpResponse.getResponseStatusCode());
        buffer.append(' ');
        buffer.append(httpResponse.getResponseStatusText());
        buffer.append('\n');

        Iterator it = httpResponse.getResponseHeaders();
        while (it.hasNext()) {
            Entry entry = (Entry) it.next();
            buffer.append((String) entry.getKey());
            buffer.append(": ");
            buffer.append((String) entry.getValue());
            buffer.append('\n');
        }
        buffer.append('\n');

        return buffer.toString();
    }

    private final XoService xoService;
    private final M2MService m2mService;
    private final InetAddress localAddress;
    private final int localPort;
    private final ServletContext servletContext;
    private final SortedSet notCopiedHttpHeaders;
    private final SortedSet notCopiedSongHeaders;
    private final Timer timerService;
    private final HttpClient httpClient;
    private final byte[] favicon;
    private int maxContentLength;
    private long requestExpirationTimer;
    private long longPollingServerRequestExpirationTimer;
    private long longPollingClientRequestExpirationTimer;
    private long contactRequestWaitingTimer;
    private SongBindingFactory songBindingFactory;
    private LongPollingServersCleaner longPollingServersCleaner;

    // Not thread safe
    /**
     * Maps an HTTP client request to a SONG request or a LongPollingClient
     */
    private final Map/* <HttpClientTransaction, Object> */pendingHttpClientRequests;
    /**
     * Maps a long polling transaction ID to the Contact send on the server side through an HTTP response
     */
    private final Map/* <String, ContactWaitingResponseNotify> */pendingContactRequests;
    /**
     * Maps a long polling ID (longPollingURI '|' contactURI) to the object that handles the long polling client connection
     */
    private final Map/* <String, LongPollingClient> */longPollingIdToLongPollingClient;
    /**
     * Maps a contact path to the long polling server
     */
    private final Map/* <String, LongPollingServer> */contactToLongPollingServer;
    /**
     * Maps a long polling path to the long polling server
     */
    private final Map/* <String, LongPollingServer> */longPollingToLongPollingServer;

    private final LongPollingServer[] waitingLongPollingServers;
    private int readIndex;
    private int writeIndex;

    /**
     * Builds the HTTP SONG Binding.
     *
     * @param xoService Service use to generate M2M error messages when needed
     * @param httpClient HTTP client used to send HTTP requests to remote servers
     * @param localAddress The local address on which the HTTP server is running
     * @param localPort The local port on which the HTTP server is running
     * @param maxServerSockets The maximum number of sockets allowed on the HTTP server side
     * @param maxContentLength The maximum content length authorized in the body of a received HTTP request or response
     * @param requestExpirationTimer The maximum duration to wait before to consider an incoming HTTP request as timed out
     * @param longPollingServerRequestExpirationTimer The maximum duration to wait for a contact request on the server side
     *            before to respond the HTTP long polling request with an empty response
     * @param longPollingClientRequestExpirationTimer The maximum duration to wait for an HTTP long polling response on the
     *            client side before to re-send an HTTP long polling request
     * @param contactRequestWaitingTimer The maximum duration to wait for an available HTTP long polling request to send a
     *            contact request. If this limit is reached the received contact request is considered as timed out
     * @param proxyHost The HTTP proxy hostname if any
     * @param proxyPort The HTTP proxy port if any
     * @param proxyUsername The proxy username if authentication is required, null otherwise
     * @param proxyPassword The proxy password if authentication is required, null otherwise
     * @param clientMaxSocketsPerHost The maximum number of sockets allowed per host on the HTTP client side
     * @param clientMaxSockets The maximum number of sockets allowed on the HTTP client side
     * @param servletContext {@link ServletContext} of the SONG Application part
     * @param favicon The favicon image for browsers as a byte array
     */
    public SongHttpBinding(XoService xoService, M2MService m2mService, HttpClient httpClient, InetAddress localAddress,
            int localPort, int maxServerSockets, int maxContentLength, long requestExpirationTimer,
            long longPollingServerRequestExpirationTimer, long longPollingClientRequestExpirationTimer,
            long contactRequestWaitingTimer, String proxyHost, int proxyPort, String proxyUsername, String proxyPassword,
            int clientMaxSocketsPerHost, int clientMaxSockets, ServletContext servletContext, byte[] favicon) {
        this.xoService = xoService;
        this.m2mService = m2mService;
        this.httpClient = httpClient;
        this.localAddress = localAddress;
        this.localPort = localPort;
        this.maxContentLength = maxContentLength;
        this.requestExpirationTimer = requestExpirationTimer;
        this.longPollingServerRequestExpirationTimer = longPollingServerRequestExpirationTimer;
        this.longPollingClientRequestExpirationTimer = longPollingClientRequestExpirationTimer;
        this.contactRequestWaitingTimer = contactRequestWaitingTimer;
        this.timerService = new Timer();
        this.servletContext = servletContext;
        this.pendingHttpClientRequests = new HashMap();
        this.pendingContactRequests = new HashMap/* <HttpClientTransaction, ContactWaitingResponseNotifyRequest> */();
        this.longPollingIdToLongPollingClient = new HashMap();
        this.contactToLongPollingServer = new HashMap();
        this.longPollingToLongPollingServer = new HashMap();
        this.waitingLongPollingServers = new LongPollingServer[maxServerSockets];
        this.readIndex = 0;
        this.writeIndex = 0;
        this.favicon = favicon;
        this.longPollingServersCleaner = new LongPollingServersCleaner(this);

        this.notCopiedHttpHeaders = new TreeSet(String.CASE_INSENSITIVE_ORDER);
        this.notCopiedHttpHeaders.add(HttpUtils.HD_AUTHORIZATION);
        this.notCopiedHttpHeaders.add(HttpUtils.HD_DATE);
        this.notCopiedHttpHeaders.add(HttpUtils.HD_FROM);
        this.notCopiedHttpHeaders.add(HttpUtils.HD_REFERER);
        this.notCopiedHttpHeaders.add(HttpUtils.HD_CACHE_CONTROL);
        this.notCopiedHttpHeaders.add(HttpUtils.HD_PRAGMA);
        this.notCopiedHttpHeaders.add(HttpUtils.HD_ACCEPT_RANGES);
        this.notCopiedHttpHeaders.add(HttpUtils.HD_AGE);
        this.notCopiedHttpHeaders.add(HttpUtils.HD_CONNECTION);
        this.notCopiedHttpHeaders.add(HttpUtils.HD_CONTENT_RANGE);
        this.notCopiedHttpHeaders.add(HttpUtils.HD_CONTENT_TYPE);
        this.notCopiedHttpHeaders.add(HttpUtils.HD_CONTENT_LENGTH);
        this.notCopiedHttpHeaders.add(HttpUtils.HD_HOST);
        this.notCopiedHttpHeaders.add(HttpUtils.HD_IF_RANGE);
        this.notCopiedHttpHeaders.add(HttpUtils.HD_RANGE);
        this.notCopiedHttpHeaders.add(HttpUtils.HD_TE);
        this.notCopiedHttpHeaders.add(HttpUtils.HD_TRAILER);
        this.notCopiedHttpHeaders.add(HttpUtils.HD_TRANSFER_ENCODING);
        this.notCopiedHttpHeaders.add(HttpUtils.HD_UPGRADE);
        this.notCopiedHttpHeaders.add(HttpUtils.HD_VARY);
        this.notCopiedHttpHeaders.add(HttpUtils.HD_VIA);
        this.notCopiedHttpHeaders.add(HttpUtils.HD_WARNING);
        this.notCopiedHttpHeaders.add(HttpUtils.HD_USER_AGENT);
        this.notCopiedHttpHeaders.add(HttpUtils.HD_SERVER);
        // this.notCopiedHttpHeaders.add(HTTP_HD_ETSI_CONTACT_URI);
        // this.notCopiedHttpHeaders.add(HTTP_HD_ETSI_CORRELATION_ID);
        this.notCopiedHttpHeaders.add(HTTP_HD_TRANSACTION_ID);
        this.notCopiedHttpHeaders.add(HTTP_HD_STATUS_CODE);
        this.notCopiedHttpHeaders.add(HTTP_HD_METHOD);
        this.notCopiedHttpHeaders.add(HTTP_HD_TARGET_ID);
        this.notCopiedHttpHeaders.add(HTTP_HD_REQUESTING_ENTITY);

        this.notCopiedSongHeaders = new TreeSet(String.CASE_INSENSITIVE_ORDER);
        this.notCopiedSongHeaders.add(SongServletMessage.HD_USER_AGENT);
        this.notCopiedSongHeaders.add(SongServletMessage.HD_SERVER);
        this.notCopiedSongHeaders.add(SongServletMessage.HD_CONTENT_TYPE);
        this.notCopiedSongHeaders.add(HttpUtils.HD_AUTHORIZATION);
        this.notCopiedSongHeaders.add(HttpUtils.HD_LOCATION);
        this.notCopiedSongHeaders.add(HTTP_HD_NEXT_HOP_URI);
        this.notCopiedSongHeaders.add(HTTP_HD_METHOD);
        this.notCopiedSongHeaders.add(HTTP_HD_TARGET_ID);
        this.notCopiedSongHeaders.add(HTTP_HD_REQUESTING_ENTITY);
        this.notCopiedSongHeaders.add(HTTP_HD_TRANSACTION_ID);
        // this.notCopiedSongHeaders.add(HTTP_HD_ETSI_CONTACT_URI);
        // this.notCopiedSongHeaders.add(HTTP_HD_ETSI_CORRELATION_ID);
        this.notCopiedSongHeaders.add(SongServletMessage.HD_CONTENT_LENGTH);

        if (proxyHost != null) {
            if (proxyUsername != null && proxyPassword != null) {
                httpClient.setProxyCredentials(proxyUsername, proxyPassword);
            }
            httpClient.setProxy(proxyHost, proxyPort);
        }

        httpClient.setMaxSocketsPerHost(clientMaxSocketsPerHost);
        httpClient.setMaxTotalSockets(clientMaxSockets);
        httpClient.setTcpNoDelay(true);
        httpClient.setRequestTimeout(requestExpirationTimer);
        httpClient.setConnectionTimeout(requestExpirationTimer);

        this.longPollingServersCleaner.start();
    }

    public void init() {
        songBindingFactory = (SongBindingFactory) getServletContext().getAttribute(SONG_BINDING_FACTORY);
    }

    /**
     * Sets the maximum content length authorized in the body of a received HTTP request or response.
     *
     * @param maxContentLength The maximum content length authorized in the Payload of a received HTTP request or response
     */
    public void setMaxContentLength(int maxContentLength) {
        if (maxContentLength != this.maxContentLength) {
            if (LOG.isInfoEnabled()) {
                LOG.info("Modifying maxContentLength from " + this.maxContentLength + " to " + maxContentLength);
            }
            this.maxContentLength = maxContentLength;
        }
    }

    /**
     * Sets the maximum duration to wait before to consider an incoming HTTP request as timed out.
     *
     * @param requestExpirationTimer The maximum duration to wait before to consider an incoming HTTP request as timed out
     */
    public void setRequestExpirationTimer(long requestExpirationTimer) {
        if (requestExpirationTimer != this.requestExpirationTimer) {
            if (LOG.isInfoEnabled()) {
                LOG.info("Modifying requestExpirationTimer from " + this.requestExpirationTimer + " to "
                        + requestExpirationTimer);
            }
            httpClient.setRequestTimeout(requestExpirationTimer);
            httpClient.setConnectionTimeout(requestExpirationTimer);
            this.requestExpirationTimer = requestExpirationTimer;
        }
    }

    /**
     * Sets the maximum duration to wait for a SONG request on the server side before to respond the HTTP long polling request
     * with an empty response. Only relevant on the server side of a long polling connection.
     *
     * @param longPollServerRequestExpirationTimer The maximum duration to wait for a SONG request on the server side before to
     *            respond the HTTP long polling request with an empty response
     */
    public void setLongPollingServerRequestExpirationTimer(long longPollServerRequestExpirationTimer) {
        if (longPollServerRequestExpirationTimer != this.longPollingServerRequestExpirationTimer) {
            LOG.info("Modifying longPollServerRequestExpirationTimer from " + this.longPollingServerRequestExpirationTimer
                    + " to " + longPollServerRequestExpirationTimer);
            this.longPollingServerRequestExpirationTimer = longPollServerRequestExpirationTimer;
        }
    }

    /**
     * Gets the maximum duration to wait for a long polling response on the client side before to re-send a long polling
     * request. Only relevant on the client side of a long polling connection.
     *
     * @return The maximum duration to wait for a SONG request on the server side before to respond the HTTP long polling
     *         request with an empty response
     */
    public long getLongPollClientRequestExpirationTimer() {
        return this.longPollingClientRequestExpirationTimer;
    }

    /**
     * Sets the maximum duration to wait for an HTTP long polling response on the client side before to re-send an HTTP long
     * poll request. Only relevant on the client side of a long polling connection.
     *
     * @param longPollClientRequestExpirationTimer The maximum duration to wait for an HTTP long polling response on the client
     *            side before to re-send an HTTP long polling request
     */
    public void setLongPollingClientRequestExpirationTimer(long longPollClientRequestExpirationTimer) {
        if (longPollClientRequestExpirationTimer != this.longPollingClientRequestExpirationTimer) {
            LOG.info("Modifying longPollClientRequestExpirationTimer from " + this.longPollingClientRequestExpirationTimer
                    + " to " + longPollClientRequestExpirationTimer);
            this.longPollingClientRequestExpirationTimer = longPollClientRequestExpirationTimer;
        }
    }

    /**
     * Sets the maximum duration to wait for an available HTTP long polling request to send a SONG request. If this limit is
     * reached the received SONG request is considered as timed out. Only relevant on the server side of a long polling
     * connection.
     *
     * @param longPollRequestWaitingTimer The maximum duration to wait for an available HTTP long polling request to send a SONG
     *            request. If this limit is reached the received SONG request is considered as timed out
     */
    public void setContactRequestWaitingTimer(long longPollRequestWaitingTimer) {
        if (longPollRequestWaitingTimer != this.contactRequestWaitingTimer) {
            if (LOG.isInfoEnabled()) {
                LOG.info("Modifying threadExpiration from " + this.contactRequestWaitingTimer + " to "
                        + longPollRequestWaitingTimer);
            }
            this.contactRequestWaitingTimer = longPollRequestWaitingTimer;
        }
    }

    /**
     * Sets the proxy configuration.
     *
     * @param proxyHost The proxy hostname
     * @param proxyPort The proxy port
     * @param proxyUsername The proxy username if authentication is required, null otherwise
     * @param proxyPassword The proxy password if authentication is required, null otherwise
     */
    public void setProxyConfiguration(String proxyHost, int proxyPort, String proxyUsername, String proxyPassword) {
        if (LOG.isInfoEnabled()) {
            LOG.info("Modifying proxy config to (proxyPort: " + proxyPort + ") (proxyUsername: " + proxyUsername
                    + ") (proxyPassword: xxx)");
        }
        if (proxyHost != null) {
            if (proxyUsername != null && proxyPassword != null) {
                httpClient.setProxyCredentials(proxyUsername, proxyPassword);
            }
            httpClient.setProxy(proxyHost, proxyPort);
        } else {
            httpClient.setProxy(null, -1);
        }
    }

    /**
     * Stops and deletes the Http Song binding.
     */
    public void destroy() {
        synchronized (longPollingIdToLongPollingClient) {
            Iterator it = longPollingIdToLongPollingClient.values().iterator();
            LongPollingClient lpClient = null;
            while (it.hasNext()) {
                lpClient = (LongPollingClient) it.next();
                lpClient.stopLongPolling();
            }
            longPollingIdToLongPollingClient.clear();
        }
        synchronized (contactToLongPollingServer) {
            synchronized (longPollingToLongPollingServer) {
                contactToLongPollingServer.clear();
                longPollingToLongPollingServer.clear();
            }
        }
        synchronized (pendingContactRequests) {
            pendingContactRequests.clear();
        }
        synchronized (pendingHttpClientRequests) {
            pendingHttpClientRequests.clear();
        }
        timerService.cancel();
        longPollingServersCleaner.end();
    }

    /*
     * HTTP Servlet part
     */
    public void doServerError(com.actility.m2m.http.server.HttpServerException httpException,
            HttpServerTransaction httpTransaction) {
        LOG.error("Incoming HTTP request triggered an error", httpException);
        XoObject errorInfo = null;

        if (httpTransaction.getMethodCode() == HttpUtils.MD_POST_CODE
                && httpTransaction.getRawRequestUri().startsWith("/http/lp/")
                && httpTransaction.getHeaders().get(/* HTTP_HD_ETSI_CORRELATION_ID */HTTP_HD_TRANSACTION_ID) != null) {
            try {
                // responseNotify request with a song response
                SongServletRequest songRequest = null;
                MultiMap headers = httpTransaction.getHeaders();
                // String requestId = (String) headers.get(HTTP_HD_ETSI_CORRELATION_ID);
                String requestId = (String) headers.get(HTTP_HD_TRANSACTION_ID);
                int statusCode = HttpServletResponse.SC_BAD_GATEWAY;
                synchronized (pendingContactRequests) {
                    ContactWaitingResponseNotifyRequest contactWaitingResponseNotify = (ContactWaitingResponseNotifyRequest) pendingContactRequests
                            .remove(requestId);
                    if (contactWaitingResponseNotify != null && contactWaitingResponseNotify.cancel()) {
                        songRequest = contactWaitingResponseNotify.popRequest();
                    }
                }
                if (songRequest != null) {
                    errorInfo = xoService.newXmlXoObject(M2MConstants.TAG_M2M_ERROR_INFO);
                    errorInfo.setNameSpace(M2MConstants.PREFIX_M2M);
                    errorInfo.setStringAttribute(M2MConstants.TAG_M2M_STATUS_CODE, StatusCode.STATUS_BAD_GATEWAY.name());
                    errorInfo.setStringAttribute(M2MConstants.TAG_M2M_ADDITIONAL_INFO,
                            "Problem with the long polling request: " + httpException.getMessage());

                    byte[] errorInfoContent = null;
                    String contentType = null;
                    try {
                        String mediaType = m2mService.getM2MUtils().getAcceptedXoMediaType(
                                songRequest.getHeader(SongServletMessage.HD_ACCEPT));
                        if (M2MConstants.MT_APPLICATION_EXI.equals(mediaType)) {
                            errorInfoContent = errorInfo.saveExi();
                            contentType = M2MConstants.MT_APPLICATION_EXI;
                        } else {
                            errorInfoContent = errorInfo.saveXml();
                            contentType = M2MConstants.CT_APPLICATION_XML_UTF8;
                        }
                    } catch (M2MException e) {
                        // Ignore error
                        LOG.error(e.getMessage(), e);
                    }

                    LongPollingServer lpServer = (LongPollingServer) songRequest.getAttribute(HTTP_AT_LONG_POLLING_SERVER);
                    SongServletResponse songResponse = songRequest.createResponse(convertHttpStatusToSongStatus(statusCode,
                            songRequest.isProxy()));

                    songResponse.setHeader(SongServletMessage.HD_SERVER, (String) headers.get(HttpUtils.HD_USER_AGENT));

                    // Set content
                    if (errorInfoContent != null && contentType != null) {
                        // Set content
                        songResponse.setContent(errorInfoContent, contentType);
                    }
                    lpServer.errorContactResponseReceived(songResponse);
                    // TODO check ? or it is in errorContactresponseReceived ?
                    // if (LOG.isInfoEnabled()) {
                    // LOG.info(songResponse.getId() + ": <<< HTTP.SONGBinding: " + songResponse.getStatus() + " "
                    // + songResponse.getReasonPhrase());
                    // }
                    // songResponse.send();
                }
            } catch (IOException e) {
                LOG.error("IOException while trying to respond an " + httpException.getStatus()
                        + " error to a long polling request", e);
            } catch (XoException e) {
                LOG.error("XoException while trying to respond an " + httpException.getStatus()
                        + " error to a long polling request", e);
            } finally {
                if (errorInfo != null) {
                    errorInfo.free(true);
                }
            }
        }

        try {
            errorInfo = xoService.newXmlXoObject(M2MConstants.TAG_M2M_ERROR_INFO);
            errorInfo.setNameSpace(M2MConstants.PREFIX_M2M);
            errorInfo.setStringAttribute(M2MConstants.TAG_M2M_STATUS_CODE, httpException.getM2MStatus().name());
            errorInfo.setStringAttribute(M2MConstants.TAG_M2M_ADDITIONAL_INFO, httpException.getMessage());
            byte[] errorInfoContent = null;
            String contentType = null;
            try {
                String mediaType = m2mService.getM2MUtils().getAcceptedXoMediaType(
                        (String) httpTransaction.getHeaders().get(HttpUtils.HD_ACCEPT));
                if (M2MConstants.MT_APPLICATION_EXI.equals(mediaType)) {
                    errorInfoContent = errorInfo.saveExi();
                    contentType = M2MConstants.MT_APPLICATION_EXI;
                } else {
                    errorInfoContent = errorInfo.saveXml();
                    contentType = M2MConstants.CT_APPLICATION_XML_UTF8;
                }
            } catch (M2MException e) {
                // Ignore error
                LOG.error(e.getMessage(), e);
            }

            httpTransaction.print(httpTransaction.getHttpVersion());
            httpTransaction.print(' ');
            httpTransaction.print(httpException.getStatus());
            httpTransaction.print(' ');
            httpTransaction.print(httpException.getReasonPhrase());
            httpTransaction.print("\r\n");

            httpTransaction.print(HttpUtils.HD_CONTENT_LENGTH);
            httpTransaction.print(": ");
            httpTransaction.print(errorInfoContent.length);
            httpTransaction.print("\r\n");

            httpTransaction.print(HttpUtils.HD_CONTENT_TYPE);
            httpTransaction.print(": ");
            httpTransaction.print(contentType);
            httpTransaction.print("\r\n");

            httpTransaction.print(HttpUtils.HD_SERVER);
            httpTransaction.print(": ");
            httpTransaction.print(getServletContext().getServerInfo());
            httpTransaction.print("\r\n");

            httpTransaction.print(HttpUtils.HD_DATE);
            httpTransaction.print(": ");
            httpTransaction.print(FormatUtils.formatHttpDate(new Date()));
            httpTransaction.print("\r\n");

            httpTransaction.print("\r\n");
            httpTransaction.writeBody(errorInfoContent);

            if (LOG.isInfoEnabled()) {
                LOG.info(httpTransaction.getId() + ": <<< HTTP.HTTPBinding: " + httpException.getStatus() + " "
                        + httpException.getReasonPhrase());
            }
            if (LOG.isDebugEnabled()) {
                LOG.debug(getServerHttpResponseAsString(httpTransaction, httpException.getStatus(),
                        httpException.getReasonPhrase()));
            }
            if (Profiler.getInstance().isTraceEnabled()) {
                Profiler.getInstance().trace(
                        httpTransaction.getId() + ": <<< HTTP.HTTPBinding: " + httpException.getStatus() + " "
                                + httpException.getReasonPhrase());
            }
            httpTransaction.send();
        } catch (IOException e) {
            LOG.error("IOException while trying to respond an " + httpException.getStatus() + " error to the client", e);
        } catch (XoException e) {
            LOG.error("XoException while trying to respond an " + httpException.getStatus() + " error to the client", e);
        } finally {
            if (errorInfo != null) {
                errorInfo.free(true);
            }
            httpTransaction.terminated();
        }
    }

    public void doServerRequest(HttpServerTransaction httpTransaction) throws IOException {
        if (httpTransaction.getMethodCode() == HttpUtils.MD_POST_CODE) {
            if (httpTransaction.getRawRequestUri().startsWith("/http/lp/")) {
                // Long polling
                if (httpTransaction.getHeaders().get(/* HTTP_HD_ETSI_CORRELATION_ID */HTTP_HD_TRANSACTION_ID) != null) {
                    handleSongResponseFromHttpResponseNotifyRequest(httpTransaction);
                } else {
                    handleHttpLongPollingRequest(httpTransaction);
                }
            } else {
                handleSongRequestFromHttpRequest(httpTransaction);
            }
        } else if (httpTransaction.getMethodCode() == HttpUtils.MD_GET_CODE) {
            if ("/favicon.ico".equals(httpTransaction.getRawRequestUri())) {
                if (Profiler.getInstance().isTraceEnabled()) {
                    Profiler.getInstance().trace(
                            httpTransaction.getId() + ": >>> HTTP.HTTPBinding: " + httpTransaction.getMethod() + " "
                                    + httpTransaction.getRawRequestUri());
                }
                if (LOG.isInfoEnabled()) {
                    LOG.info(httpTransaction.getId() + ": >>> HTTP.HTTPBinding: " + httpTransaction.getMethod() + " "
                            + httpTransaction.getRawRequestUri());
                }
                try {
                    httpTransaction.print(httpTransaction.getHttpVersion());
                    httpTransaction.print(' ');
                    httpTransaction.print(HttpServletResponse.SC_OK);
                    httpTransaction.print(" OK\r\n");

                    httpTransaction.print(HttpUtils.HD_CONTENT_LENGTH);
                    httpTransaction.print(": ");
                    httpTransaction.print(favicon.length);
                    httpTransaction.print("\r\n");

                    httpTransaction.print(HttpUtils.HD_CONTENT_TYPE);
                    httpTransaction.print(": image/vnd.microsoft.icon\r\n");

                    httpTransaction.print(HttpUtils.HD_SERVER);
                    httpTransaction.print(": ");
                    httpTransaction.print(getServletContext().getServerInfo());
                    httpTransaction.print("\r\n");

                    httpTransaction.print(HttpUtils.HD_DATE);
                    httpTransaction.print(": ");
                    httpTransaction.print(FormatUtils.formatHttpDate(new Date()));
                    httpTransaction.print("\r\n");

                    httpTransaction.print("\r\n");
                    httpTransaction.writeBody(favicon);
                    if (LOG.isInfoEnabled()) {
                        LOG.info(httpTransaction.getId() + ": <<< HTTP.HTTPBinding: 200 OK");
                    }
                    if (Profiler.getInstance().isTraceEnabled()) {
                        Profiler.getInstance().trace(httpTransaction.getId() + ": <<< HTTP.HTTPBinding: 200 OK");
                    }
                    httpTransaction.send();
                } finally {
                    httpTransaction.terminated();
                }
            } else {
                handleSongRequestFromHttpRequest(httpTransaction);
            }
        } else {
            handleSongRequestFromHttpRequest(httpTransaction);
        }
    }

    public void sendClientRequest(HttpClientTransaction httpTransaction, Object context) throws HttpClientException {
        synchronized (pendingHttpClientRequests) {
            httpTransaction.sendRequest();
            pendingHttpClientRequests.put(httpTransaction, context);
        }
    }

    public void doClientResponse(HttpClientTransaction httpTransaction) throws IOException {
        try {
            if (LOG.isInfoEnabled()) {
                LOG.info(httpTransaction.getId() + ": >>> HTTP.HTTPBinding: Received HTTP response");
            }
            Object context = null;
            synchronized (pendingHttpClientRequests) {
                context = pendingHttpClientRequests.remove(httpTransaction);
            }
            if (context != null) {
                if (context.getClass() == LongPollingClient.class) {
                    // This is a long polling response
                    LongPollingClient longPollingClient = (LongPollingClient) context;
                    longPollingClient.receivedLongPollingResponse(httpTransaction);
                } else if (context instanceof SongServletResponse) {
                    // This is a response notify response
                    LongPollingClient lpClient = (LongPollingClient) ((SongServletResponse) context).getRequest().getAttribute(
                            HTTP_AT_LONG_POLLING_CLIENT);
                    if (httpTransaction.getResponseStatusCode() < 300) {
                        lpClient.sentContactResponse();
                    } else {
                        lpClient.errorContactResponse();
                    }
                    if (Profiler.getInstance().isTraceEnabled()) {
                        Profiler.getInstance().trace(
                                httpTransaction.getId() + ": >>> HTTP.HTTPBinding: " + httpTransaction.getResponseStatusCode()
                                        + " " + httpTransaction.getResponseStatusText() + " (Response notify response)");
                    }
                    if (LOG.isInfoEnabled()) {
                        LOG.info(httpTransaction.getId() + ": >>> HTTP.HTTPBinding: " + httpTransaction.getResponseStatusCode()
                                + " " + httpTransaction.getResponseStatusText() + " (Response notify response)");
                    }
                    if (LOG.isDebugEnabled()) {
                        LOG.debug(getClientHttpResponseAsString(httpTransaction));
                    }
                } else {
                    // This is a normal response
                    SongServletRequest songRequest = (SongServletRequest) context;
                    handleSongResponseFromHttpResponse(httpTransaction, songRequest);
                }
            }
        } finally {
            httpTransaction.release();
        }
    }

    /*
     * SONG Servlet part
     */
    public void doRetrieve(SongServletRequest songRequest) throws ServletException, IOException {
        if (LOG.isInfoEnabled()) {
            LOG.info(songRequest.getId() + ": >>> HTTP.SONGBinding: RETRIEVE (requestingEntity: "
                    + songRequest.getRequestingEntity().absoluteURI() + ") (targetID: "
                    + songRequest.getTargetID().absoluteURI() + ")");
        }
        handleSongRequest(songRequest, HttpUtils.MD_GET_CODE, null);
    }

    public void doUpdate(SongServletRequest songRequest) throws ServletException, IOException {
        if (LOG.isInfoEnabled()) {
            LOG.info(songRequest.getId() + ": >>> HTTP.SONGBinding: UPDATE (requestingEntity: "
                    + songRequest.getRequestingEntity().absoluteURI() + ") (targetID: "
                    + songRequest.getTargetID().absoluteURI() + ")");
        }
        handleSongRequest(songRequest, HttpUtils.MD_PUT_CODE, null);
    }

    public void doCreate(SongServletRequest songRequest) throws ServletException, IOException {
        if (LOG.isInfoEnabled()) {
            LOG.info(songRequest.getId() + ": >>> HTTP.SONGBinding: CREATE (requestingEntity: "
                    + songRequest.getRequestingEntity().absoluteURI() + ") (targetID: "
                    + songRequest.getTargetID().absoluteURI() + ")");
        }
        handleSongRequest(songRequest, HttpUtils.MD_POST_CODE, SongServletRequest.MD_CREATE);
    }

    public void doDelete(SongServletRequest songRequest) throws ServletException, IOException {
        if (LOG.isInfoEnabled()) {
            LOG.info(songRequest.getId() + ": >>> HTTP.SONGBinding: DELETE (requestingEntity: "
                    + songRequest.getRequestingEntity().absoluteURI() + ") (targetID: "
                    + songRequest.getTargetID().absoluteURI() + ")");
        }
        handleSongRequest(songRequest, HttpUtils.MD_DELETE_CODE, null);
    }

    public void doProvisionalResponse(SongServletResponse songResponse) throws ServletException, IOException {
        if (LOG.isInfoEnabled()) {
            LOG.info(songResponse.getId() + ": >>> HTTP.SONGBinding: " + songResponse.getStatus() + " "
                    + songResponse.getReasonPhrase());
        }
        handleSongResponse(songResponse);
    }

    public void doSuccessResponse(SongServletResponse songResponse) throws ServletException, IOException {
        if (LOG.isInfoEnabled()) {
            LOG.info(songResponse.getId() + ": >>> HTTP.SONGBinding: " + songResponse.getStatus() + " "
                    + songResponse.getReasonPhrase());
        }
        handleSongResponse(songResponse);
    }

    public void doErrorResponse(SongServletResponse songResponse) throws ServletException, IOException {
        if (LOG.isInfoEnabled()) {
            LOG.info(songResponse.getId() + ": >>> HTTP.SONGBinding: " + songResponse.getStatus() + " "
                    + songResponse.getReasonPhrase());
        }
        handleSongResponse(songResponse);
    }

    /**
     * Sends an empty HTTP response to a long polling request
     *
     * @param statusCode The HTTP status code
     * @param reasonPhrase THe HTTP status text
     * @param httpTransaction The HTTP server transaction which holds the HTTP request to which the response is sent
     * @throws IOException If any problem occurs while sending the HTTP response
     */
    public void initHttpLongPollingResponse(int statusCode, String reasonPhrase, HttpServerTransaction httpTransaction)
            throws IOException {
        try {
            httpTransaction.print(httpTransaction.getHttpVersion());
            httpTransaction.print(' ');
            httpTransaction.print(statusCode);
            httpTransaction.print(' ');
            httpTransaction.print(reasonPhrase);
            httpTransaction.print("\r\n");

            httpTransaction.print(HttpUtils.HD_CONTENT_LENGTH);
            httpTransaction.print(": 0\r\n");

            httpTransaction.print(HttpUtils.HD_SERVER);
            httpTransaction.print(": ");
            httpTransaction.print(getServletContext().getServerInfo());
            httpTransaction.print("\r\n");

            httpTransaction.print(HttpUtils.HD_DATE);
            httpTransaction.print(": ");
            httpTransaction.print(FormatUtils.formatHttpDate(new Date()));
            httpTransaction.print("\r\n");

            httpTransaction.print("\r\n");

            if (LOG.isInfoEnabled()) {
                LOG.info(httpTransaction.getId() + ": <<< HTTP.HTTPBinding: " + statusCode + " " + reasonPhrase
                        + " (Long Polling)");
            }
            if (LOG.isDebugEnabled()) {
                LOG.debug(getServerHttpResponseAsString(httpTransaction, statusCode, reasonPhrase));
            }
            if (Profiler.getInstance().isTraceEnabled()) {
                Profiler.getInstance().trace(
                        httpTransaction.getId() + ": <<< HTTP.HTTPBinding: " + statusCode + " " + reasonPhrase
                                + " (Long Polling)");
            }
            httpTransaction.send();
        } finally {
            httpTransaction.terminated();
        }
    }

    /**
     * Checks if the HTTP response is valid regarding the maximum length supported by the Song HTTP binding.
     *
     * @param httpTransaction The HTTP transaction where to check the HTTP response
     * @return Whether the HTTP response is valid
     */
    boolean checkHttpResponse(HttpClientTransaction httpTransaction) {
        if (httpTransaction.getResponseStatusCode() != HttpServletResponse.SC_NO_CONTENT
                && httpTransaction.getResponseStatusCode() != HttpServletResponse.SC_NOT_MODIFIED) {
            if (httpTransaction.getResponseContentLength() < 0) {
                LOG.error("Received response with no content-length");
                return false;
            } else if (httpTransaction.getResponseContentLength() > maxContentLength) {
                LOG.error("Received response with a too large content: " + httpTransaction.getResponseContentLength());
                return false;
            }
        }
        return true;
    }

    /**
     * Creates a server long polling connection with the given URI.
     * <p>
     * The method is in charge to generate a contact and long polling URI for the created connection.
     *
     * @param serverURI The URI that is the base to build the server long polling connection and which allows to build the
     *            {@link LongPollURIs}
     * @return The contact and long polling URI of the long polling connection
     */
    public LongPollURIs createServerLongPolling(SongURI serverURI) {
        if (LOG.isInfoEnabled()) {
            LOG.info("createServerLongPolling from: " + serverURI.absoluteURI());
        }
        // Generate contact + long polling uri
        String contactPath = "/contact/" + UUID.randomUUID(40);
        String longPollingPath = "/lp/" + UUID.randomUUID(40);
        synchronized (contactToLongPollingServer) {
            synchronized (longPollingToLongPollingServer) {
                int retry = 10;
                while (retry > 0
                        && (contactToLongPollingServer.containsKey(contactPath) || longPollingToLongPollingServer
                                .containsKey(longPollingPath))) {
                    // Generate contact + long polling uri
                    contactPath = "/contact/" + UUID.randomUUID(40 - retry);
                    longPollingPath = "/lp/" + UUID.randomUUID(40 - retry);
                    --retry;
                }
                if (contactToLongPollingServer.containsKey(contactPath)
                        || longPollingToLongPollingServer.containsKey(longPollingPath)) {
                    throw new RuntimeException("Collision detected while generating server long polling URIs");
                }
                LongPollingServer longPollingServer = new LongPollingServer(this, timerService, contactPath + "|"
                        + longPollingPath, contactRequestWaitingTimer, longPollingServerRequestExpirationTimer);
                contactToLongPollingServer.put(contactPath, longPollingServer);
                longPollingToLongPollingServer.put(longPollingPath, longPollingServer);
            }
        }
        SongURI contactURI = songBindingFactory.createURI(serverURI.isSecure() ? "https" : "http", serverURI.getHost(),
                serverURI.getPort(), servletContext.getContextPath() + contactPath);
        SongURI longPollingURI = songBindingFactory.createURI(serverURI.isSecure() ? "https" : "http", serverURI.getHost(),
                serverURI.getPort(), servletContext.getContextPath() + longPollingPath);
        if (LOG.isInfoEnabled()) {
            LOG.info("Long polling server connection created with contact URI " + contactURI.absoluteURI()
                    + " and long polling URI " + longPollingURI.absoluteURI());
        }
        return new LongPollURIs(contactURI, longPollingURI);
    }

    /**
     * Creates a server long polling connection with the given contact and long polling URIs.
     * <p>
     * The container is allowed to reject the given URIs if it cannot managed them.
     *
     * @param contactURI The contact URI to use in the long polling connection
     * @param longPollingURI The long polling URI to use in the long polling connection
     * @throws ServletException If the binding cannot manage the given contact and long polling URIs
     */
    public void createServerLongPolling(SongURI contactURI, SongURI longPollingURI) throws ServletException {
        if (LOG.isInfoEnabled()) {
            LOG.info("createServerLongPolling for: contact=" + contactURI.absoluteURI() + " longPolling="
                    + longPollingURI.absoluteURI());
        }
        String contactBasePath = servletContext.getContextPath() + "/contact/";
        String longPollingBasePath = servletContext.getContextPath() + "/lp/";
        if (!contactURI.getPath().startsWith(contactBasePath)) {
            throw new ServletException("Try to create a long polling server connection with a bad contact path: "
                    + contactURI.getPath() + " while it should start with " + contactBasePath);
        }
        if (!longPollingURI.getPath().startsWith(longPollingBasePath)) {
            throw new ServletException("Try to create a long polling server connection with a bad long polling path: "
                    + longPollingURI.getPath() + " while it should start with " + longPollingBasePath);
        }
        int contextPathLength = servletContext.getContextPath().length();
        LongPollingServer longPollingServer = new LongPollingServer(this, timerService, contactURI.getPath() + "|"
                + longPollingURI.getPath(), contactRequestWaitingTimer, longPollingServerRequestExpirationTimer);
        synchronized (contactToLongPollingServer) {
            synchronized (longPollingToLongPollingServer) {
                String contactPath = contactURI.getPath().substring(contextPathLength);
                String longPollingPath = longPollingURI.getPath().substring(contextPathLength);
                if (contactToLongPollingServer.containsKey(contactPath)
                        || longPollingToLongPollingServer.containsKey(longPollingPath)) {
                    throw new RuntimeException("Collision detected while generating server long polling URIs");
                }
                contactToLongPollingServer.put(contactPath, longPollingServer);
                longPollingToLongPollingServer.put(longPollingPath, longPollingServer);
            }
        }
        if (LOG.isInfoEnabled()) {
            LOG.info("Long polling server connection created with contact URI " + contactURI.absoluteURI()
                    + " and long polling URI " + longPollingURI.absoluteURI());
        }
    }

    /**
     * Deletes a server long polling connection given its contact and long polling URI.
     *
     * @param contactURI The contact URI of the long polling connection
     * @param longPollingURI The long polling URI of the long polling connection
     */
    public void deleteServerLongPolling(SongURI contactURI, SongURI longPollingURI) {
        if (LOG.isInfoEnabled()) {
            LOG.info("deleteServerLongPolling for: contact=" + contactURI.absoluteURI() + " longPolling="
                    + longPollingURI.absoluteURI());
        }
        int contextPathLength = servletContext.getContextPath().length();
        LongPollingServer longPollingServer = null;
        synchronized (contactToLongPollingServer) {
            synchronized (longPollingToLongPollingServer) {
                longPollingServer = (LongPollingServer) contactToLongPollingServer.remove(contactURI.getPath().substring(
                        contextPathLength));
                longPollingToLongPollingServer.remove(longPollingURI.getPath().substring(contextPathLength));
            }
        }
        if (longPollingServer != null) {
            synchronized (longPollingServer) {
                if (longPollingServer.getCount() == 0) {
                    // Sends a message to flush the long polling request
                    longPollingServer.offer(new ContactWaitingLongPollingRequest());
                } else {
                    ContactWaitingLongPollingRequest waitingRequest = longPollingServer.poll();
                    while (waitingRequest != null) {
                        if (waitingRequest.cancel()) {
                            try {
                                waitingRequest.popRequest().createResponse(SongServletResponse.SC_NOT_FOUND).send();
                            } catch (IOException e) {
                                LOG.error("Cannot send 404 to pending SONG requests in the deleted long polling queue: "
                                        + contactURI.getPath(), e);
                            }
                        }
                        waitingRequest = longPollingServer.poll();
                    }
                }
            }
        }
    }

    /**
     * Creates a client long polling connection from the given contact and long polling URIs.
     * <p>
     * This will trigger the connection creation on the long polling URI. An entry will also be added to the container external
     * aliases route table mapping the contact URI to the current application.
     *
     * @param contactURI The contact URI of the long polling connection
     * @param longPollingURI The long polling URI of the long polling connection
     * @throws ServletException If a long polling connection if already opened for the given contact and long polling URIs
     */
    public void createClientLongPolling(SongURI contactURI, SongURI longPollingURI) throws ServletException {
        if (LOG.isInfoEnabled()) {
            LOG.info("createClientLongPolling for: contact=" + contactURI.absoluteURI() + " longPolling="
                    + longPollingURI.absoluteURI());
        }
        try {
            String longPollingId = contactURI.absoluteURI() + "|" + longPollingURI.absoluteURI();
            InetAddress address = InetAddress.getByName(longPollingURI.getHost());
            HttpClient httpClient = this.httpClient.copy();
            httpClient.setRequestTimeout(longPollingClientRequestExpirationTimer);
            LongPollingClient lpClient = new LongPollingClient(this, timerService, httpClient, contactURI, address,
                    longPollingURI.getPort(), longPollingURI.absoluteURI());
            synchronized (longPollingIdToLongPollingClient) {
                if (longPollingIdToLongPollingClient.containsKey(longPollingId)) {
                    throw new ServletException("Long polling connection is already opened");
                }
                longPollingIdToLongPollingClient.put(longPollingId, lpClient);
            }
            lpClient.sendLongPollingRequest();
            if (LOG.isInfoEnabled()) {
                LOG.info("client long polling created for: contact=" + contactURI.absoluteURI() + " longPolling="
                        + longPollingURI.absoluteURI());
            }
        } catch (UnknownHostException e) {
            throw new ServletException("Cannot determine the address of the long polling connection", e);
        }
    }

    /**
     * Deletes a client long polling connection given its contact and long polling URI.
     *
     * @param contactURI The contact URI of the long polling connection
     * @param longPollingURI The long polling URI of the long polling connection
     */
    public void deleteClientLongPolling(SongURI contactURI, SongURI longPollingURI) {
        if (LOG.isInfoEnabled()) {
            LOG.info("deleteClientLongPolling for: contact=" + contactURI.absoluteURI() + " longPolling="
                    + longPollingURI.absoluteURI());
        }
        String longPollingId = contactURI.absoluteURI() + "|" + longPollingURI.absoluteURI();
        LongPollingClient lpClient = null;
        synchronized (longPollingIdToLongPollingClient) {
            lpClient = (LongPollingClient) longPollingIdToLongPollingClient.remove(longPollingId);
        }
        if (lpClient != null) {
            lpClient.stopLongPolling();
        } else {
            LOG.error("Try to stop an unknown client long polling connection");
        }
        if (LOG.isInfoEnabled()) {
            LOG.info("client long polling deleted for: contact=" + contactURI.absoluteURI() + " longPolling="
                    + longPollingURI.absoluteURI());
        }
    }

    /**
     * Adds a pending contact request to list of all pending contact requests.
     *
     * @param lpServer Long polling server to which the contact request is linked
     * @param transactionId The ID that uniquely identifies the pending contact request. It will also be used to retrieve the
     *            contact request when the contact response will be received
     * @param songRequest The SONG request which is related to the contact request
     */
    public void addPendingContactRequest(LongPollingServer lpServer, String transactionId, SongServletRequest songRequest) {
        songRequest.setAttribute(HTTP_AT_LONG_POLLING_SERVER, lpServer);
        synchronized (pendingContactRequests) {
            ContactWaitingResponseNotifyRequest contactWaitingResponseNotify = new ContactWaitingResponseNotifyRequest(this,
                    lpServer, transactionId, songRequest);
            timerService.schedule(contactWaitingResponseNotify, requestExpirationTimer);
            pendingContactRequests.put(transactionId, contactWaitingResponseNotify);
        }
    }

    /**
     * Removes a contact request from the list of all pending contact requests given its ID
     *
     * @param transactionId The ID that uniquely identifies the pending contact request
     */
    public void removePendingContactRequest(String transactionId) {
        synchronized (pendingContactRequests) {
            ContactWaitingResponseNotifyRequest contactWaitingResponseNotify = (ContactWaitingResponseNotifyRequest) pendingContactRequests
                    .remove(transactionId);
            if (contactWaitingResponseNotify != null) {
                contactWaitingResponseNotify.cancel();
            }
        }
    }

    /**
     * Adds a long polling server to the list of waiting long polling servers.
     * <p>
     * This list is used by the LongPollingServersCleaner to determine if a long polling request is expired
     *
     * @param longPollingServer The long polling server to add in the list of waiting long polling servers
     * @return Whether the insertion succeeded
     */
    public boolean addWaitingLongPollingServer(LongPollingServer longPollingServer) {
        if (LOG.isDebugEnabled()) {
            LOG.debug("Add waiting long polling servers " + longPollingServer);
        }
        boolean result = true;
        int newIndex = (writeIndex + 1) % waitingLongPollingServers.length;
        if (newIndex == readIndex) {
            fixWaitingLongPollingServers();
            newIndex = (writeIndex + 1) % waitingLongPollingServers.length;
        }
        result = newIndex != readIndex;
        if (result) {
            waitingLongPollingServers[writeIndex] = longPollingServer;
            longPollingServer.setIndex(writeIndex);
            if (LOG.isDebugEnabled()) {
                LOG.debug("Add waiting long polling server to index " + writeIndex + ". New write index is " + newIndex);
            }
            writeIndex = newIndex;
        }
        return result;
    }

    /**
     * Checks if some long polling server requests has expired according to the given reference date
     *
     * @param now The reference date
     */
    public void checkWaitingLongPollingServersForExpiration(long now) {
        if (LOG.isDebugEnabled()) {
            LOG.debug("Start: Check waiting long polling servers for expiration " + now);
        }
        synchronized (waitingLongPollingServers) {
            LongPollingServer longPollingServer = null;
            while (longPollingServer == null && readIndex != writeIndex) {
                longPollingServer = waitingLongPollingServers[readIndex];
                if (longPollingServer == null) {
                    readIndex = (readIndex + 1) % waitingLongPollingServers.length;
                    if (LOG.isDebugEnabled()) {
                        LOG.debug("New read index is " + readIndex);
                    }
                } else if (longPollingServer.checkExpired(now)) {
                    if (LOG.isDebugEnabled()) {
                        LOG.debug("Long polling server has expired " + longPollingServer);
                    }
                    waitingLongPollingServers[readIndex] = null;
                    longPollingServer = null;
                    readIndex = (readIndex + 1) % waitingLongPollingServers.length;
                    if (LOG.isDebugEnabled()) {
                        LOG.debug("New read index is " + readIndex);
                    }
                }
            }
        }
        if (LOG.isDebugEnabled()) {
            LOG.debug("End: Check waiting long polling servers for expiration " + now);
        }
    }

    /**
     * Removed a long polling server from the list of waiting long polling servers.
     *
     * @param longPollingServer The long polling server to remove from the list of long polling servers
     */
    public void removeWaitingLongPollingServer(LongPollingServer longPollingServer) {
        if (LOG.isDebugEnabled()) {
            LOG.debug("Remove waiting long polling servers " + longPollingServer + " at index " + longPollingServer.getIndex());
        }
        waitingLongPollingServers[longPollingServer.getIndex()] = null;
        longPollingServer.setIndex(-1);
    }

    private void fixWaitingLongPollingServers() {
        if (LOG.isDebugEnabled()) {
            LOG.debug("Fix waiting long polling servers");
        }
        int waitingLongPollingServersLength = waitingLongPollingServers.length;
        int currentOffset = 0;
        LongPollingServer longPollingServer = null;
        int newIndex = 0;
        for (int index = readIndex; index != writeIndex; index = ((index + 1) % waitingLongPollingServersLength)) {
            longPollingServer = waitingLongPollingServers[index];
            if (longPollingServer == null) {
                ++currentOffset;
            } else if (currentOffset != 0) {
                newIndex = (index - currentOffset + waitingLongPollingServersLength) % waitingLongPollingServersLength;
                longPollingServer.setIndex(newIndex);
                waitingLongPollingServers[newIndex] = longPollingServer;
                waitingLongPollingServers[index] = null;
                if (LOG.isDebugEnabled()) {
                    LOG.debug("Switching long polling server from index " + index + " to index " + newIndex);
                }
            }
        }
        writeIndex = ((writeIndex - currentOffset + waitingLongPollingServersLength) % waitingLongPollingServersLength);
        if (LOG.isDebugEnabled()) {
            LOG.debug("New write index is " + writeIndex);
        }
    }

    private void handleSongRequest(SongServletRequest songRequest, int httpMethodCode, String songMethod)
            throws ServletException, IOException {
        LOG.debug("handleSongRequest");
        if (songRequest.isCommitted()) {
            // Normal request
            handleHttpRequestFromSongRequest(songRequest, httpMethodCode, songMethod);
        } else {
            // Request to the HTTP binding application (long polling only)
            handleHttpContactRequestFromSongRequest(songRequest);
        }
    }

    private void handleSongResponse(SongServletResponse songResponse) throws ServletException, IOException {
        LOG.debug("handleSongResponse");
        boolean longPollingRequest = songResponse.getRequest().getAttribute(
        /* HTTP_AT_CONTACT_URI */SongBindingFactory.AT_LONG_POLL_URI) != null;
        if (!longPollingRequest) {
            handleHttpResponseFromSongResponse(songResponse);
        } else {
            handleHttpResponseNotifyRequestFromSongResponse(songResponse);
        }
    }

    /**
     * Handles a received HTTP request from a long polling connection to wait a SONG request and use it to build and send an
     * HTTP response .
     *
     * @param httpRequest The received HTTP request
     * @param httpResponse The HTTP response of the received HTTP request to build
     * @throws IOException If a problem occurs writing the HTTP response
     */
    private void handleHttpLongPollingRequest(HttpServerTransaction httpTransaction) throws IOException {
        LOG.debug("handleHttpLongPollingRequest");
        if (Profiler.getInstance().isTraceEnabled()) {
            Profiler.getInstance().trace(
                    httpTransaction.getId() + ": >>> HTTP.HTTPBinding: " + httpTransaction.getMethod() + " "
                            + httpTransaction.getRawRequestUri() + " (Long Polling Request)");
        }
        if (LOG.isInfoEnabled()) {
            LOG.info(httpTransaction.getId() + ": >>> HTTP.HTTPBinding: " + httpTransaction.getMethod() + " "
                    + httpTransaction.getRawRequestUri() + " (Long Polling Request)");
        }
        int statusCode = 200;
        String reasonPhrase = null;
        try {
            LongPollingServer longPollingServer = null;
            int contextPathLength = servletContext.getContextPath().length();
            String longPollingPath = httpTransaction.getRawRequestUri().substring(contextPathLength);
            synchronized (longPollingToLongPollingServer) {
                longPollingServer = (LongPollingServer) longPollingToLongPollingServer.get(longPollingPath);
            }

            if (longPollingServer != null) {
                synchronized (waitingLongPollingServers) {
                    longPollingServer.longPollingRequestReceived(httpTransaction);
                }
            } else {
                LOG.error(httpTransaction.getId() + ": Received a long polling request on an unknown long polling connection");
                statusCode = HttpServletResponse.SC_NOT_FOUND;
                reasonPhrase = "Not Found";
                initHttpLongPollingResponse(statusCode, reasonPhrase, httpTransaction);
            }
        } catch (UnsupportedEncodingException uee) {
            LOG.error(httpTransaction.getId() + ": UnsupportedEncodingException in handle request", uee);
            statusCode = HttpServletResponse.SC_BAD_REQUEST;
            reasonPhrase = "Bad Request";
            initHttpLongPollingResponse(statusCode, reasonPhrase, httpTransaction);
        }
    }

    /**
     * Handles a received HTTP request to build and send a SONG request.
     *
     * @param httpTransation The received HTTP request
     * @throws IOException If a problem occurs while reading the received HTTP request or sending the SONG request or writing
     *             the HTTP response
     */
    private void handleSongRequestFromHttpRequest(HttpServerTransaction httpTransaction) throws IOException {
        LOG.debug("handleSongRequestFromHttpRequest");
        if (Profiler.getInstance().isTraceEnabled()) {
            Profiler.getInstance().trace(
                    httpTransaction.getId() + ": >>> HTTP.HTTPBinding: " + httpTransaction.getMethod() + " "
                            + httpTransaction.getRawRequestUri());
        }
        if (LOG.isInfoEnabled()) {
            LOG.info(httpTransaction.getId() + ": >>> HTTP.HTTPBinding: " + httpTransaction.getMethod() + " "
                    + httpTransaction.getRawRequestUri());
        }
        try {
            SongServletRequest songRequest = buildSongRequestFromHttpRequest(httpTransaction);
            songRequest.setAttribute(HTTP_AT_HTTP_TRANSACTION, httpTransaction);

            // Send the request and get the response
            if (LOG.isInfoEnabled()) {
                LOG.info(songRequest.getId() + ": <<< HTTP.SONGBinding: " + songRequest.getMethod() + " (requestingEntity: "
                        + songRequest.getRequestingEntity().absoluteURI() + ") (targetID: "
                        + songRequest.getTargetID().absoluteURI() + ")");
            }
            songRequest.send();
        } catch (UnsupportedEncodingException e) {
            doServerError(new com.actility.m2m.http.server.HttpServerException(e.getMessage(),
                    StatusCode.STATUS_INTERNAL_SERVER_ERROR, HttpServletResponse.SC_INTERNAL_SERVER_ERROR,
                    HttpUtils.RP_INTERNAL_SERVER_ERROR, e), httpTransaction);
        } catch (com.actility.m2m.http.server.HttpServerException e) {
            doServerError(e, httpTransaction);
        } catch (IOException e) {
            doServerError(new com.actility.m2m.http.server.HttpServerException(e.getMessage(),
                    StatusCode.STATUS_INTERNAL_SERVER_ERROR, HttpServletResponse.SC_INTERNAL_SERVER_ERROR,
                    HttpUtils.RP_INTERNAL_SERVER_ERROR, e), httpTransaction);
        }
    }

    private void handleSongResponseFromHttpResponse(HttpClientTransaction httpTransaction, SongServletRequest songRequest)
            throws IOException {
        if (Profiler.getInstance().isTraceEnabled()) {
            Profiler.getInstance().trace(
                    songRequest.getId() + ": >>> HTTP.HTTPBinding: " + httpTransaction.getResponseStatusCode() + " "
                            + httpTransaction.getResponseStatusText());
        }
        if (LOG.isInfoEnabled()) {
            LOG.info(songRequest.getId() + ": >>> HTTP.HTTPBinding: " + httpTransaction.getResponseStatusCode() + " "
                    + httpTransaction.getResponseStatusText());
        }
        if (LOG.isDebugEnabled()) {
            LOG.debug(getClientHttpResponseAsString(httpTransaction));
        }

        // Examine the response status
        if (checkHttpResponse(httpTransaction)) {
            SongServletResponse response = buildSongResponseFromHttpResponse(httpTransaction, songRequest);
            if (LOG.isInfoEnabled()) {
                LOG.info(response.getId() + ": <<< HTTP.SONGBinding: " + response.getStatus() + " "
                        + response.getReasonPhrase());
            }
            response.send();
        } else {
            InetAddress remoteAddress = null;
            InetAddress localAddress = null;
            try {
                remoteAddress = InetAddress.getByName(httpTransaction.getRemoteAddress());
                localAddress = InetAddress.getByName(httpTransaction.getLocalAddress());
            } catch (UnknownHostException uhe) {
                LOG.error("Cannot determine the remote address from the request URI of the HTTP request", uhe);
                remoteAddress = this.localAddress;
                localAddress = this.localAddress;
            }
            SongServletResponse songResponse = songBindingFactory.createResponse(songRequest,
                    SongServletResponse.SC_INTERNAL_SERVER_ERROR, SongServletResponse.RP_INTERNAL_SERVER_ERROR, localAddress,
                    httpTransaction.getLocalPort(), remoteAddress, httpTransaction.getRemotePort());

            songResponse.setHeader(SongServletMessage.HD_SERVER, getServletContext().getServerInfo());
            songResponse.send();
        }
    }

    /**
     * Handles a received Response notify request to build and send a SONG response.
     *
     * @param httpRequest The received HTTP request
     * @param httpResponse The HTTP response of the received HTTP request to build
     * @throws IOException If a problem occurs while reading the received HTTP request or sending the SONG response
     */
    private void handleSongResponseFromHttpResponseNotifyRequest(HttpServerTransaction httpTransaction) throws IOException {
        LOG.debug("handleSongResponseFromHttpContactRequest");
        if (Profiler.getInstance().isTraceEnabled()) {
            Profiler.getInstance().trace(
                    httpTransaction.getId() + ": >>> HTTP.HTTPBinding: " + httpTransaction.getMethod() + " "
                            + httpTransaction.getRawRequestUri() + " (Response Notify Request)");
        }
        if (LOG.isInfoEnabled()) {
            LOG.info(httpTransaction.getId() + ": >>> HTTP.HTTPBinding: " + httpTransaction.getMethod() + " "
                    + httpTransaction.getRawRequestUri() + " (Response Notify Request)");
        }

        SongServletRequest songRequest = null;
        MultiMap headers = httpTransaction.getHeaders();
        String statusStr = (String) headers.get(HTTP_HD_STATUS_CODE);
        // String requestId = (String) headers.get(HTTP_HD_ETSI_CORRELATION_ID);
        String requestId = (String) headers.get(HTTP_HD_TRANSACTION_ID);
        int statusCode = HttpServletResponse.SC_ACCEPTED;
        String reasonPhrase = "Accepted";
        try {
            if (statusStr != null) {
                int status = Integer.valueOf(statusStr).intValue();
                synchronized (pendingContactRequests) {
                    ContactWaitingResponseNotifyRequest contactWaitingResponseNotify = null;
                    if (status < 200) {
                        contactWaitingResponseNotify = (ContactWaitingResponseNotifyRequest) pendingContactRequests
                                .get(requestId);
                        if (contactWaitingResponseNotify != null && contactWaitingResponseNotify.cancel()) {
                            songRequest = contactWaitingResponseNotify.getRequest();
                        }
                    } else {
                        contactWaitingResponseNotify = (ContactWaitingResponseNotifyRequest) pendingContactRequests
                                .remove(requestId);
                        if (contactWaitingResponseNotify != null && contactWaitingResponseNotify.cancel()) {
                            songRequest = contactWaitingResponseNotify.popRequest();
                        }
                    }
                }
                if (songRequest != null) {
                    // Retrieve long polling server connection
                    LongPollingServer lpServer = (LongPollingServer) songRequest.getAttribute(HTTP_AT_LONG_POLLING_SERVER);
                    SongServletResponse songResponse = buildSongResponseFromHttpResponseNotifyRequest(status, httpTransaction,
                            songRequest);
                    lpServer.contactResponseReceived(songResponse);
                    initHttpLongPollingResponse(statusCode, reasonPhrase, httpTransaction);
                } else {
                    LOG.error("Received a song response in a long polling request with an unknown transaction id: " + requestId);
                    statusCode = HttpServletResponse.SC_FORBIDDEN;
                    reasonPhrase = "Forbidden";
                    initHttpLongPollingResponse(statusCode, reasonPhrase, httpTransaction);
                }
            } else {
                LOG.error("Received a song response in a long polling request with no status code");
                statusCode = HttpServletResponse.SC_BAD_REQUEST;
                reasonPhrase = "Bad Request";
                initHttpLongPollingResponse(statusCode, reasonPhrase, httpTransaction);
            }
        } catch (NumberFormatException e) {
            LOG.error("Received a song response in a long polling request with a bad status code: " + statusStr, e);
        }
    }

    /**
     * Handles a received SONG request to build an HTTP request.
     *
     * @param songRequest The received SONG request
     * @param httpMethod The HTTP method of the HTTP request to build
     * @param songMethod The SONG method to insert in HTTP request headers in case SONG method is not completely described by
     *            the HTTP method
     * @throws IOException If a problem occurs while sending the HTTP request or sending the SONG response
     * @throws ServletException If an HTTP response is not received
     */
    private void handleHttpRequestFromSongRequest(SongServletRequest songRequest, int httpMethodCode, String songMethod)
            throws ServletException, IOException {
        LOG.debug("handleHttpRequestFromSongRequest");
        Long expirationTime = (Long) songRequest.getAttribute(SongBindingFactory.AT_EXPIRATION_TIME);
        long requestDuration = 0;
        if (expirationTime != null) {
            long requestExpirationTimer = this.requestExpirationTimer;
            requestDuration = expirationTime.longValue() - System.currentTimeMillis();
            if (requestDuration > requestExpirationTimer) {
                requestDuration = requestExpirationTimer;
            }
        }
        if (requestDuration >= 0) {
            try {
                // Prepare a request object
                HttpClientTransaction httpTransaction = buildHttpRequestFromSongRequest(songRequest, httpMethodCode, songMethod);
                // TODO httpTransaction.setRequestDuration(requestDuration);
                if (LOG.isInfoEnabled()) {
                    LOG.info(songRequest.getId() + ": <<< HTTP.HTTPBinding: " + httpMethodCode + " "
                            + songRequest.getTargetID().absoluteURI());
                }
                // TODO ?
                // if (LOG.isDebugEnabled()) {
                // LOG.debug(getClientHttpRequestAsString(httpTransaction));
                // }

                if (Profiler.getInstance().isTraceEnabled()) {
                    Profiler.getInstance().trace(
                            songRequest.getId() + ": <<< HTTP.HTTPBinding: " + httpMethodCode + " "
                                    + songRequest.getTargetID().absoluteURI());
                }
                // Execute the request
                sendClientRequest(httpTransaction, songRequest);
            } catch (HttpClientException e) {
                LOG.error("HTTP binding exception", e);
                throw new ServletException("HTTP binding exception", e);
            }
        } else {
            SongServletResponse songResponse = songBindingFactory.createResponse(songRequest,
                    SongServletResponse.SC_GATEWAY_TIMEOUT, SongServletResponse.RP_GATEWAY_TIMEOUT, localAddress, localPort,
                    localAddress, localPort);

            songResponse.setHeader(SongServletMessage.HD_SERVER, "SONG HTTP Binding");
            songResponse.send();
        }
    }

    /**
     * Handles a received SONG response to build and send an HTTP request in a long polling connection.
     *
     * @param songResponse The received SONG response
     * @throws IOException If a problem occurs while sending the HTTP request
     */
    private void handleHttpResponseNotifyRequestFromSongResponse(SongServletResponse songResponse) throws IOException {
        LOG.debug("handleHttpResponseNotifyRequestFromSongResponse");
        // Prepare a request object
        try {
            HttpClientTransaction httpTransaction = buildHttpResponseNotifyRequestFromSongResponse(songResponse);
            // Execute the request
            if (LOG.isInfoEnabled()) {
                String target = (String) songResponse.getRequest().getAttribute(
                /* HTTP_AT_CONTACT_URI */SongBindingFactory.AT_LONG_POLL_URI);
                LOG.info(songResponse.getId() + ": <<< HTTP.HTTPBinding: POST " + target + " (Response Notify Request)");
            }
            // TODO ?
            // if (LOG.isDebugEnabled()) {
            // LOG.debug(getClientHttpRequestAsString(httpTransaction));
            // }
            if (Profiler.getInstance().isTraceEnabled()) {
                String target = (String) songResponse.getRequest().getAttribute(
                /* HTTP_AT_CONTACT_URI */SongBindingFactory.AT_LONG_POLL_URI);
                Profiler.getInstance().trace(
                        songResponse.getId() + ": <<< HTTP.HTTPBinding: POST " + target + " (Response Notify Request)");
            }
            sendClientRequest(httpTransaction, songResponse);
        } catch (HttpClientException e) {
            LOG.error("HTTP binding exception", e);
            // Ignore
        }
    }

    private void handleHttpResponseFromSongResponse(SongServletResponse songResponse) throws IOException, ServletException {
        LOG.debug("handleHttpResponseFromSongResponse");
        HttpServerTransaction httpTransaction = (HttpServerTransaction) songResponse.getRequest().getAttribute(
                HTTP_AT_HTTP_TRANSACTION);
        if (httpTransaction != null) {
            buildHttpResponseFromSongResponse(httpTransaction, songResponse);
        } else {
            throw new ServletException("No HTTP transaction associated with the request: " + songResponse.getRequest());
        }
    }

    /**
     * Handles a received SONG request to build an HTTP response in a long polling connection.
     *
     * @param songRequest The received SONG request
     * @throws IOException If a problem occurs while sending the SONG response
     */
    private void handleHttpContactRequestFromSongRequest(SongServletRequest songRequest) throws IOException {
        LOG.debug("handleHttpContactRequestFromSongRequest");
        LongPollingServer longPollingServer = null;

        String path = songRequest.getPathInfo();
        if (path != null && path.startsWith("/contact/")) {

            synchronized (contactToLongPollingServer) {
                longPollingServer = (LongPollingServer) contactToLongPollingServer.get(path);
            }
            if (longPollingServer != null) {
                synchronized (waitingLongPollingServers) {
                    longPollingServer.contactRequestReceived(songRequest);
                }
            } else {
                LOG.error(songRequest.getId() + ": Received SONG request on an unknown long polling contact: " + path);
                if (LOG.isInfoEnabled()) {
                    LOG.info(songRequest.getId() + ": <<< HTTP.SONGBinding: 404 Not Found");
                }
                songRequest.createResponse(SongServletResponse.SC_NOT_FOUND).send();
            }
        } else {
            LOG.error(songRequest.getId() + ": Received SONG request on an invalid long polling contact: " + path);
            if (LOG.isInfoEnabled()) {
                LOG.info(songRequest.getId() + ": <<< HTTP.SONGBinding: 404 Not Found");
            }
            songRequest.createResponse(SongServletResponse.SC_NOT_FOUND).send();
        }
    }

    private SongServletRequest buildSongRequestFromHttpRequest(HttpServerTransaction httpTransaction)
            throws UnsupportedEncodingException, com.actility.m2m.http.server.HttpServerException {
        LOG.debug("buildSongRequestFromHttpRequest");

        // Build SONG method
        String songMethod = null;
        int httpMethod = httpTransaction.getMethodCode();
        switch (httpMethod) {
        case HttpUtils.MD_GET_CODE:
            songMethod = SongServletRequest.MD_RETRIEVE;
            break;
        case HttpUtils.MD_POST_CODE:
            songMethod = SongServletRequest.MD_CREATE;
            break;
        case HttpUtils.MD_PUT_CODE:
            songMethod = SongServletRequest.MD_UPDATE;
            break;
        case HttpUtils.MD_DELETE_CODE:
            songMethod = SongServletRequest.MD_DELETE;
            break;
        default:
            throw new com.actility.m2m.http.server.HttpServerException("Unknown HTTP method: " + httpMethod,
                    StatusCode.STATUS_BAD_REQUEST, HttpServletResponse.SC_BAD_REQUEST, HttpUtils.RP_BAD_REQUEST);
        }

        // Build requestingEntity
        MultiMap headers = httpTransaction.getHeaders();
        SongURI requestingEntity = null;
        String authorization = (String) headers.get(HttpUtils.HD_AUTHORIZATION);
        if (authorization != null && authorization.startsWith("Basic ")) {
            String tmpRequestingEntity = null;
            String base64 = authorization.substring(6);
            try {
                authorization = new String(FormatUtils.parseBase64(base64), "US-ASCII");
                int semiColonIndex = authorization.indexOf(':');
                if (semiColonIndex != -1) {
                    tmpRequestingEntity = URLDecoder.decode(authorization.substring(0, semiColonIndex), "US-ASCII");
                } else {
                    tmpRequestingEntity = URLDecoder.decode(authorization, "US-ASCII");
                }

                requestingEntity = songBindingFactory.createURI(tmpRequestingEntity);
            } catch (IllegalArgumentException e) {
                throw new com.actility.m2m.http.server.HttpServerException("Requesting entity is not a valid URI: "
                        + tmpRequestingEntity, StatusCode.STATUS_BAD_REQUEST, HttpServletResponse.SC_BAD_REQUEST,
                        HttpUtils.RP_BAD_REQUEST, e);
            } catch (URISyntaxException e) {
                throw new com.actility.m2m.http.server.HttpServerException("Requesting entity is not a valid URI: "
                        + tmpRequestingEntity, StatusCode.STATUS_BAD_REQUEST, HttpServletResponse.SC_BAD_REQUEST,
                        HttpUtils.RP_BAD_REQUEST, e);
            } catch (ParseException e) {
                throw new com.actility.m2m.http.server.HttpServerException(
                        "Authorization header basic value is not a valid base64 string: " + base64,
                        StatusCode.STATUS_BAD_REQUEST, HttpServletResponse.SC_BAD_REQUEST, HttpUtils.RP_BAD_REQUEST, e);
            }
        } else {
            throw new com.actility.m2m.http.server.HttpServerException("No requesting entity found in HTTP request",
                    StatusCode.STATUS_FORBIDDEN, HttpServletResponse.SC_FORBIDDEN, HttpUtils.RP_FORBIDDEN);
        }

        // Build targetID
        SongURI targetID = null;
        String requestUri = httpTransaction.getRawRequestUri();
        boolean proxy = false;
        try {
            if (requestUri.charAt(0) == '/') {
                String host = httpTransaction.getHost();
                if (host == null) {
                    throw new com.actility.m2m.http.server.HttpServerException("No Host header found in HTTP request",
                            StatusCode.STATUS_BAD_REQUEST, HttpServletResponse.SC_BAD_REQUEST, HttpUtils.RP_BAD_REQUEST);
                }
                targetID = songBindingFactory.createURI((httpTransaction.isSecure() ? "https://" : "http://") + host
                        + requestUri);
                String targetIdQuery = targetID.getQueryParameter(HTTP_QUERY_TARGET_ID);
                if (targetIdQuery != null) {
                    String queryMode = targetID.getQueryParameter(HTTP_QUERY_MODE);
                    proxy = true;
                    if (queryMode != null && "server".equals(queryMode)) {
                        proxy = false;
                    }
                    targetID = songBindingFactory.createURI(targetIdQuery);
                }
            } else {
                proxy = true;
                targetID = songBindingFactory.createURI(requestUri);
            }
        } catch (IllegalArgumentException e) {
            throw new com.actility.m2m.http.server.HttpServerException("Target ID is not a valid URI: " + requestUri,
                    StatusCode.STATUS_BAD_REQUEST, HttpServletResponse.SC_BAD_REQUEST, HttpUtils.RP_BAD_REQUEST, e);
        } catch (URISyntaxException e) {
            throw new com.actility.m2m.http.server.HttpServerException("Target ID is not a valid URI: " + requestUri,
                    StatusCode.STATUS_BAD_REQUEST, HttpServletResponse.SC_BAD_REQUEST, HttpUtils.RP_BAD_REQUEST, e);
        }

        // Build request
        String id = httpTransaction.getId();
        String xForwardedFor = (String) httpTransaction.getHeaders().get(HTTP_HD_FORWARD_FOR);
        if (xForwardedFor != null) {
            id = xForwardedFor + "|" + id;
        }
        Socket httpSocket = httpTransaction.getSocket();
        SongServletRequest songRequest = songBindingFactory.createRequest(httpTransaction.getHttpVersion(), songMethod,
                requestingEntity, targetID, httpSocket.getLocalAddress(), httpSocket.getLocalPort(),
                httpSocket.getInetAddress(), httpSocket.getPort(), proxy, id);

        // Set headers
        songRequest.setHeader(SongServletMessage.HD_USER_AGENT, (String) headers.get(HttpUtils.HD_USER_AGENT));

        Iterator it = headers.entrySet().iterator();
        Entry entry = null;
        String headerName = null;
        while (it.hasNext()) {
            entry = (Entry) it.next();
            headerName = (String) entry.getKey();
            if (!notCopiedHttpHeaders.contains(headerName)) {
                songRequest.setHeader(headerName, (String) entry.getValue());
            }
        }

        // Set content
        if (httpTransaction.getContentLength() > 0) {
            String contentType = (String) headers.get(HttpUtils.HD_CONTENT_TYPE);
            if (contentType == null) {
                LOG.error("Received an HTTP request without a Content-Type header. Default to " + HTTP_DEFAULT_CONTENT_TYPE);
                contentType = HTTP_DEFAULT_CONTENT_TYPE;
            }
            songRequest.getContentType();
            byte[] copiedRawContent = new byte[httpTransaction.getContentLength()];
            System.arraycopy(httpTransaction.getRawContent(), 0, copiedRawContent, 0, httpTransaction.getContentLength());
            songRequest.setContent(copiedRawContent, contentType);
        }

        return songRequest;
    }

    /**
     * Builds a SONG request from an HTTP response coming from a long polling connection.
     *
     * @param longPollingClient The object that manages the long polling client connection from which this HTTP response was
     *            received
     * @param httpTransaction The HTTP transaction where the HTTP response is embedded
     * @param baseURI All messages received from the long polling connection are considered to be received from this base URI.
     *            The long polling message only defines the requested local path
     * @param remoteAddress The remote address of the server which has generated the HTTP response
     * @param remotePort The remote port of the server which has generated the HTTP response
     * @param longPollingURI The remote long polling URI from which the response is received
     * @return The created SONG Request
     * @throws URISyntaxException If the Request URI embedded in the HTTP response is invalid
     * @throws IOException If the HTTP response content cannot be read or the SONG request content cannot be set
     */
    SongServletRequest buildSongRequestFromHttpLongPollingResponse(LongPollingClient longPollingClient,
            HttpClientTransaction httpTransaction, SongURI baseURI, InetAddress remoteAddress, int remotePort,
            /* String correlationId */String longPollingURI) throws URISyntaxException, IOException {
        LOG.debug("buildSongRequestFromHttpLongPollingResponse");
        String method = httpTransaction.getResponseHeader(HTTP_HD_METHOD);
        String uri = httpTransaction.getResponseHeader(HTTP_HD_TARGET_ID);
        SongURI targetId = null;
        if (uri.charAt(0) == '/') {
            targetId = baseURI.copy();
            targetId.setPath(httpTransaction.getResponseHeader(HTTP_HD_TARGET_ID));
        } else {
            targetId = songBindingFactory.createURI(uri);
        }

        // Build requestingEntity
        SongURI requestingEntity = songBindingFactory.createURI(httpTransaction.getResponseHeader(HTTP_HD_REQUESTING_ENTITY));

        SongServletRequest songRequest = songBindingFactory.createRequest(httpTransaction.getProtocol(), method,
                requestingEntity, targetId, localAddress, localPort, remoteAddress, remotePort, false/* , correlationId */);

        if (httpTransaction.getResponseHeader(HttpUtils.HD_SERVER) != null) {
            songRequest.setHeader(SongServletMessage.HD_USER_AGENT, httpTransaction.getResponseHeader(HttpUtils.HD_SERVER));
        }

        Iterator it = httpTransaction.getResponseHeaders();
        while (it.hasNext()) {
            Entry entry = (Entry) it.next();
            if (!notCopiedHttpHeaders.contains(entry.getKey())) {
                songRequest.addHeader((String) entry.getKey(), (String) entry.getValue());
            }
        }

        int contentLength = httpTransaction.getResponseContentLength();

        if (contentLength > 0) {
            String contentType = httpTransaction.getResponseHeader(HttpUtils.HD_CONTENT_TYPE);
            if (contentType == null) {
                LOG.error("Received an HTTP Long polling response without a Content-Type header. Default to "
                        + HTTP_DEFAULT_CONTENT_TYPE);
                contentType = HTTP_DEFAULT_CONTENT_TYPE;
            }
            songRequest.setContent(httpTransaction.getResponseBody(), contentType);
        }

        // songRequest.setAttribute(HTTP_AT_CONTACT_URI, httpTransaction.getResponseHeader(HTTP_HD_ETSI_CONTACT_URI));
        // songRequest.setAttribute(HTTP_AT_CORRELATION_ID, correlationId);
        songRequest.setAttribute(HTTP_AT_LONG_POLLING_CLIENT, longPollingClient);
        songRequest.setAttribute(SongBindingFactory.AT_LONG_POLL_URI, longPollingURI);
        songRequest.setAttribute(HTTP_AT_TRANSACTION_ID, httpTransaction.getResponseHeader(HTTP_HD_TRANSACTION_ID));
        return songRequest;
    }

    private SongServletResponse buildSongResponseFromHttpResponse(HttpClientTransaction httpTransaction,
            SongServletRequest songRequest) throws IOException {
        LOG.debug("buildSongResponseFromHttpResponse");
        InetAddress remoteAddress = null;
        InetAddress localAddress = null;
        try {
            remoteAddress = InetAddress.getByName(httpTransaction.getRemoteAddress());
            localAddress = InetAddress.getByName(httpTransaction.getLocalAddress());
        } catch (UnknownHostException uhe) {
            LOG.error("Cannot determine the remote address from the request URI of the HTTP request", uhe);
            remoteAddress = this.localAddress;
            localAddress = this.localAddress;
        }
        int statusCode = convertHttpStatusToSongStatus(httpTransaction.getResponseStatusCode(), songRequest.getProxy()
                .getProxyTo() != null);

        SongServletResponse songResponse = songBindingFactory.createResponse(songRequest, statusCode,
                httpTransaction.getResponseStatusText(), localAddress, httpTransaction.getLocalPort(), remoteAddress,
                httpTransaction.getRemotePort());

        if (SongServletRequest.MD_CREATE.equals(songRequest.getMethod())) {
            String contentLocation = httpTransaction.getResponseHeader(HttpUtils.HD_CONTENT_LOCATION);
            if (contentLocation == null) {
                contentLocation = httpTransaction.getResponseHeader(HttpUtils.HD_LOCATION);
                if (contentLocation != null) {
                    songResponse.setHeader(SongServletMessage.HD_CONTENT_LOCATION, contentLocation);
                }
            }
        }
        if (httpTransaction.getResponseHeader(HttpUtils.HD_SERVER) != null) {
            songResponse.setHeader(SongServletMessage.HD_SERVER, httpTransaction.getResponseHeader(HttpUtils.HD_SERVER));
        }
        Iterator it = httpTransaction.getResponseHeaders();
        while (it.hasNext()) {
            Entry entry = (Entry) it.next();
            if (!notCopiedHttpHeaders.contains(entry.getKey())) {
                songResponse.addHeader((String) entry.getKey(), (String) entry.getValue());
            }
        }

        if (statusCode >= HttpServletResponse.SC_OK && statusCode != HttpServletResponse.SC_NO_CONTENT
                && statusCode != HttpServletResponse.SC_NOT_MODIFIED) {
            int contentLength = httpTransaction.getResponseContentLength();
            if (contentLength > 0) {
                String contentType = httpTransaction.getResponseHeader(HttpUtils.HD_CONTENT_TYPE);
                if (contentType == null) {
                    LOG.error("Received an HTTP response without a Content-Type header. Default to "
                            + HTTP_DEFAULT_CONTENT_TYPE);
                    contentType = HTTP_DEFAULT_CONTENT_TYPE;
                }
                songResponse.setContent(httpTransaction.getResponseBody(), contentType);
            }
        }
        return songResponse;
    }

    private SongServletResponse buildSongResponseFromHttpResponseNotifyRequest(int status,
            HttpServerTransaction httpTransaction, SongServletRequest songRequest) throws UnsupportedEncodingException {
        LOG.debug("buildSongResponseFromHttpContactResponse");
        SongServletResponse songResponse = songRequest.createResponse(convertHttpStatusToSongStatus(status,
                songRequest.isProxy()));
        MultiMap headers = httpTransaction.getHeaders();

        if (SongServletRequest.MD_CREATE.equals(songRequest.getMethod())) {
            String contentLocation = (String) headers.get(HttpUtils.HD_CONTENT_LOCATION);
            if (contentLocation == null) {
                contentLocation = (String) headers.get(HttpUtils.HD_LOCATION);
                if (contentLocation != null) {
                    songResponse.setHeader(SongServletMessage.HD_CONTENT_LOCATION, contentLocation);
                }
            }
        }
        songResponse.setHeader(SongServletMessage.HD_SERVER, (String) headers.get(HttpUtils.HD_USER_AGENT));

        Iterator it = headers.entrySet().iterator();
        String headerName = null;
        Entry entry = null;
        while (it.hasNext()) {
            entry = (Entry) it.next();
            headerName = (String) entry.getKey();
            if (!notCopiedHttpHeaders.contains(headerName)) {
                songResponse.addHeader(headerName, (String) entry.getValue());
            }
        }

        // Set content
        if (httpTransaction.getContentLength() > 0) {
            String contentType = (String) headers.get(HttpUtils.HD_CONTENT_TYPE);
            if (contentType == null) {
                LOG.error("Received an HTTP response notify request without a Content-Type header. Default to "
                        + HTTP_DEFAULT_CONTENT_TYPE);
                contentType = HTTP_DEFAULT_CONTENT_TYPE;
            }
            byte[] copiedRawContent = new byte[httpTransaction.getContentLength()];
            System.arraycopy(httpTransaction.getRawContent(), 0, copiedRawContent, 0, httpTransaction.getContentLength());
            songResponse.setContent(copiedRawContent, contentType);
        }

        return songResponse;
    }

    private HttpClientTransaction buildHttpRequestFromSongRequest(SongServletRequest songRequest, int httpMethodCode,
            String songMethod) throws ServletException, IOException, HttpClientException {
        LOG.debug("buildHttpRequestFromSongRequest");
        HttpClientTransaction httpTransaction = null;
        boolean content = false;
        SongURI targetUri = songRequest.getTargetID();
        SongURI proxyTo = songRequest.getProxy().getProxyTo();
        SongURI pocUri = (SongURI) songRequest.getAttribute(SongBindingFactory.AT_POC_URI);

        if (proxyTo != null && !"http".equals(proxyTo.getScheme())) {
            targetUri = proxyTo.copy();
            targetUri.setQueryParameter(HTTP_QUERY_TARGET_ID, songRequest.getTargetID().absoluteURI());
        } else if (pocUri != null) {
            targetUri = pocUri.copy();
            targetUri.setQueryParameter(HTTP_QUERY_TARGET_ID, songRequest.getTargetID().absoluteURI());
            targetUri.setQueryParameter(HTTP_QUERY_MODE, "server");
        }

        switch (httpMethodCode) {
        case HttpUtils.MD_GET_CODE:
            httpTransaction = httpClient.createTransaction(HttpUtils.MD_GET, targetUri.absoluteURI());
            break;
        case HttpUtils.MD_POST_CODE:
            content = true;
            httpTransaction = httpClient.createTransaction(HttpUtils.MD_POST, targetUri.absoluteURI());
            break;
        case HttpUtils.MD_PUT_CODE:
            content = true;
            httpTransaction = httpClient.createTransaction(HttpUtils.MD_PUT, targetUri.absoluteURI());
            break;
        case HttpUtils.MD_DELETE_CODE:
            httpTransaction = httpClient.createTransaction(HttpUtils.MD_DELETE, targetUri.absoluteURI());
            break;
        default:
            throw new ServletException("Unknown HTTP method: " + httpMethodCode);
        }

        if (proxyTo != null && "http".equals(proxyTo.getScheme())) {
            // Send to a proxy URI
            httpTransaction.setRequestProxy(proxyTo.getHost(), proxyTo.getPort());
        }

        SongURI requestingEntity = songRequest.getRequestingEntity();
        if (requestingEntity != null) {
            String authorization = StringUtils.quote(requestingEntity.absoluteURI(), L_BASIC, H_BASIC) + ":";
            authorization = "Basic " + FormatUtils.formatBase64(authorization.getBytes("US-ASCII"));
            httpTransaction.addRequestHeader(HttpUtils.HD_AUTHORIZATION, authorization);
        }
        if (content) {
            httpTransaction.addRequestHeader(HttpUtils.HD_CONTENT_LENGTH, String.valueOf(songRequest.getContentLength()));
        }

        if (proxyTo != null && proxyTo.getPath() != null && !"/".equals(proxyTo.getPath())) {
            httpTransaction.addRequestHeader(HTTP_HD_NEXT_HOP_URI, proxyTo.getPath());
        } else if (pocUri != null && pocUri.getPath() != null && !"/".equals(pocUri.getPath())) {
            httpTransaction.addRequestHeader(HTTP_HD_NEXT_HOP_URI, pocUri.getPath());
        }
        httpTransaction.addRequestHeader(HttpUtils.HD_USER_AGENT, songRequest.getHeader(SongServletMessage.HD_USER_AGENT));

        Iterator headers = songRequest.getHeaderNames();
        String headerName = null;
        while (headers.hasNext()) {
            headerName = (String) headers.next();
            if (!notCopiedSongHeaders.contains(headerName)) {
                httpTransaction.addRequestHeader(headerName, songRequest.getHeader(headerName));
            }
        }

        if (content && songRequest.getContentLength() > 0) {
            if (songRequest.getContentType() != null) {
                httpTransaction.addRequestHeader(HttpUtils.HD_CONTENT_TYPE, String.valueOf(songRequest.getContentType()));
            }
            httpTransaction.setRequestBody(songRequest.getRawContent());
        }

        return httpTransaction;
    }

    private HttpClientTransaction buildHttpResponseNotifyRequestFromSongResponse(SongServletResponse songResponse)
            throws IOException, HttpClientException {
        LOG.debug("buildHttpLongPollingRequestFromSongResponse");
        SongServletRequest songRequest = songResponse.getRequest();
        HttpClientTransaction httpTransaction = httpClient.createTransaction(HttpUtils.MD_POST,
                (String) songRequest.getAttribute(/* HTTP_AT_CONTACT_URI */SongBindingFactory.AT_LONG_POLL_URI));

        httpTransaction.addRequestHeader(HttpUtils.HD_USER_AGENT, songResponse.getHeader(SongServletMessage.HD_SERVER));
        httpTransaction.addRequestHeader(HttpUtils.HD_CONTENT_LENGTH, String.valueOf(songResponse.getContentLength()));
        if (SongServletRequest.MD_CREATE.equals(songResponse.getRequest().getMethod())) {
            String contentLocation = songResponse.getHeader(SongServletMessage.HD_CONTENT_LOCATION);
            if (contentLocation != null) {
                httpTransaction.addRequestHeader(HttpUtils.HD_LOCATION, contentLocation);
            }
        }

        // httpTransaction
        // .addRequestHeader(HTTP_HD_ETSI_CORRELATION_ID, (String) songRequest.getAttribute(HTTP_AT_CORRELATION_ID));
        httpTransaction.addRequestHeader(HTTP_HD_TRANSACTION_ID, (String) songRequest.getAttribute(HTTP_AT_TRANSACTION_ID));
        httpTransaction.addRequestHeader(HTTP_HD_STATUS_CODE, String.valueOf(songResponse.getStatus()));
        if (LOG.isDebugEnabled()) {
            // LOG.debug("Set transaction id header to: " + (String) songRequest.getAttribute(HTTP_AT_CORRELATION_ID));
            LOG.debug("Set transaction id header to: " + (String) songRequest.getAttribute(HTTP_AT_TRANSACTION_ID));
            LOG.debug("Set status code header to: " + String.valueOf(songResponse.getStatus()));
        }

        Iterator headers = songResponse.getHeaderNames();
        String headerName = null;
        while (headers.hasNext()) {
            headerName = (String) headers.next();
            if (!notCopiedSongHeaders.contains(headerName)) {
                httpTransaction.addRequestHeader(headerName, songResponse.getHeader(headerName));
            }
        }

        if (songResponse.getContentLength() > 0) {
            if (songResponse.getContentType() != null) {
                httpTransaction.addRequestHeader(HttpUtils.HD_CONTENT_TYPE, String.valueOf(songResponse.getContentType()));
            }
            httpTransaction.setRequestBody(songResponse.getRawContent());
        }
        return httpTransaction;
    }

    private void buildHttpResponseFromSongResponse(HttpServerTransaction httpTransaction, SongServletResponse songResponse)
            throws IOException {
        LOG.debug("buildHttpResponseFromSongResponse");
        try {
            httpTransaction.print(httpTransaction.getHttpVersion());
            httpTransaction.print(' ');
            httpTransaction.print(songResponse.getStatus());
            httpTransaction.print(' ');
            httpTransaction.print(songResponse.getReasonPhrase());
            httpTransaction.print("\r\n");

            if (songResponse.getStatus() != HttpServletResponse.SC_NO_CONTENT
                    && songResponse.getStatus() != HttpServletResponse.SC_NOT_MODIFIED) {
                httpTransaction.print(HttpUtils.HD_CONTENT_LENGTH);
                httpTransaction.print(": ");
                httpTransaction.print(String.valueOf(songResponse.getContentLength()));
                httpTransaction.print("\r\n");
            }

            if (songResponse.getContentType() != null) {
                httpTransaction.print(HttpUtils.HD_CONTENT_TYPE);
                httpTransaction.print(": ");
                httpTransaction.print(songResponse.getContentType());
                httpTransaction.print("\r\n");
            }

            if (SongServletRequest.MD_CREATE.equals(songResponse.getRequest().getMethod())) {
                String contentLocation = songResponse.getHeader(SongServletMessage.HD_CONTENT_LOCATION);
                if (contentLocation != null) {
                    httpTransaction.print(HttpUtils.HD_LOCATION);
                    httpTransaction.print(": ");
                    httpTransaction.print(contentLocation);
                    httpTransaction.print("\r\n");
                }
            }

            httpTransaction.print(HttpUtils.HD_SERVER);
            httpTransaction.print(": ");
            httpTransaction.print(songResponse.getHeader(SongServletMessage.HD_SERVER));
            httpTransaction.print("\r\n");

            httpTransaction.print(HttpUtils.HD_DATE);
            httpTransaction.print(": ");
            httpTransaction.print(FormatUtils.formatHttpDate(new Date()));
            httpTransaction.print("\r\n");

            Iterator it = songResponse.getHeaderNames();
            String header = null;
            ListIterator lit = null;
            while (it.hasNext()) {
                header = (String) it.next();
                if (!notCopiedSongHeaders.contains(header)) {
                    lit = songResponse.getHeaders(header);
                    while (lit.hasNext()) {
                        httpTransaction.print(header);
                        httpTransaction.print(": ");
                        httpTransaction.print((String) lit.next());
                        httpTransaction.print("\r\n");
                    }
                }
            }

            httpTransaction.print("\r\n");
            if (songResponse.getContentLength() > 0) {
                httpTransaction.writeBody(songResponse.getRawContent());
            }
            if (LOG.isInfoEnabled()) {
                LOG.info(songResponse.getId() + ": <<< HTTP.HTTPBinding: " + songResponse.getStatus() + " "
                        + songResponse.getReasonPhrase());
            }
            if (Profiler.getInstance().isTraceEnabled()) {
                Profiler.getInstance().trace(
                        songResponse.getId() + ": <<< HTTP.HTTPBinding: " + songResponse.getStatus() + " "
                                + songResponse.getReasonPhrase());
            }
            httpTransaction.send();
        } finally {
            httpTransaction.terminated();
        }
    }

    void buildHttpLongPollingResponseFromSongRequest(String transactionId, HttpServerTransaction httpTransaction,
            SongServletRequest songRequest) throws IOException {
        LOG.debug("buildHttpLongPollingResponseFromSongRequest");
        try {
            httpTransaction.print(httpTransaction.getHttpVersion());
            httpTransaction.print(' ');
            httpTransaction.print(HttpServletResponse.SC_OK);
            httpTransaction.print(" OK\r\n");

            if (songRequest.getContentType() != null) {
                httpTransaction.print(HttpUtils.HD_CONTENT_TYPE);
                httpTransaction.print(": ");
                httpTransaction.print(songRequest.getContentType());
                httpTransaction.print("\r\n");
            }

            httpTransaction.print(HttpUtils.HD_CONTENT_LENGTH);
            httpTransaction.print(": ");
            httpTransaction.print(String.valueOf(songRequest.getContentLength()));
            httpTransaction.print("\r\n");

            // Long polling headers
            httpTransaction.print(HTTP_HD_METHOD);
            httpTransaction.print(": ");
            httpTransaction.print(songRequest.getMethod());
            httpTransaction.print("\r\n");
            httpTransaction.print(HTTP_HD_TARGET_ID);
            httpTransaction.print(": ");
            httpTransaction.print(songRequest.getTargetID().absoluteURI());
            httpTransaction.print("\r\n");

            httpTransaction.print(HTTP_HD_REQUESTING_ENTITY);
            httpTransaction.print(": ");
            httpTransaction.print(songRequest.getRequestingEntity().absoluteURI());
            httpTransaction.print("\r\n");

            httpTransaction.print(HTTP_HD_TRANSACTION_ID);
            httpTransaction.print(": ");
            httpTransaction.print(transactionId);
            httpTransaction.print("\r\n");

            // httpTransaction.print(HTTP_HD_ETSI_CORRELATION_ID);
            // httpTransaction.print(": ");
            // httpTransaction.print(transactionId);
            // httpTransaction.print("\r\n");
            //
            // httpTransaction.print(HTTP_HD_ETSI_CONTACT_URI);
            // httpTransaction.print(": ");
            // httpTransaction.print((httpTransaction.isSecure() ? "https://" : "http://") + httpTransaction.getHost()
            // + httpTransaction.getRawRequestUri());
            // httpTransaction.print("\r\n");

            httpTransaction.print(HttpUtils.HD_SERVER);
            httpTransaction.print(": ");
            httpTransaction.print(songRequest.getHeader(SongServletMessage.HD_USER_AGENT));
            httpTransaction.print("\r\n");

            Iterator it = songRequest.getHeaderNames();
            String header = null;
            ListIterator lit = null;
            while (it.hasNext()) {
                header = (String) it.next();
                if (!notCopiedSongHeaders.contains(header)) {
                    lit = songRequest.getHeaders(header);
                    while (lit.hasNext()) {
                        httpTransaction.print(header);
                        httpTransaction.print(": ");
                        httpTransaction.print((String) lit.next());
                        httpTransaction.print("\r\n");
                    }
                }
            }

            httpTransaction.print("\r\n");
            if (songRequest.getContentLength() > 0) {
                httpTransaction.writeBody(songRequest.getRawContent());
            }
            if (LOG.isInfoEnabled()) {
                LOG.info(transactionId + ": <<< HTTP.HTTPBinding: 200 OK (Long Polling Response)");
            }
            if (Profiler.getInstance().isTraceEnabled()) {
                Profiler.getInstance().trace(transactionId + ": <<< HTTP.HTTPBinding: 200 OK (Long Polling Response)");
            }
            httpTransaction.send();
        } finally {
            httpTransaction.terminated();
        }
    }

    private int convertHttpStatusToSongStatus(int status, boolean proxy) {
        int newStatus = status;
        if (status < 200) {
            LOG.error("Received an HTTP response with an unknown status code: " + status
                    + ". Use 500 Internal Server Error instead");
            newStatus = SongServletResponse.SC_INTERNAL_SERVER_ERROR;
        } else if (status < 300) {
            if (status == 203 || status > 204) {
                LOG.error("Received an HTTP response with an unknown status code: " + status + ". Use 200 OK instead");
                newStatus = SongServletResponse.SC_OK;
            }
        } else if (status < 400) {
            if (status != 304) {
                if (proxy) {
                    LOG.error("Received an HTTP response with an unknown status code: " + status
                            + ". Use 502 Bad Gateway instead");
                    newStatus = SongServletResponse.SC_BAD_GATEWAY;
                } else {
                    LOG.error("Received an HTTP response with an unknown status code: " + status
                            + ". Use 500 Internal Server Error instead");
                    newStatus = SongServletResponse.SC_INTERNAL_SERVER_ERROR;
                }
            }
        } else if (status < 500) {
            if (status == 402 || status == 407 || status == 411 || status == 413 || status == 414 || status > 415) {
                LOG.error("Received an HTTP response with an unknown status code: " + status + ". Use 400 Bad Request instead");
                newStatus = SongServletResponse.SC_BAD_REQUEST;
            }
        } else if (status > 504) {
            if (proxy) {
                LOG.error("Received an HTTP response with an unknown status code: " + status + ". Use 502 Bad Gateway instead");
                newStatus = SongServletResponse.SC_BAD_GATEWAY;
            } else {
                LOG.error("Received an HTTP response with an unknown status code: " + status
                        + ". Use 500 Internal Server Error instead");
                newStatus = SongServletResponse.SC_INTERNAL_SERVER_ERROR;
            }
        }
        return newStatus;
    }

    /**
     * Gets the HTTP client object which allows to send HTTP request to remote servers.
     *
     * @return The HTTP client
     */
    public HttpClient getHttpClient() {
        return httpClient;
    }

    public LongPollingClientStats getLongPollingClientStatistics(String id) {
        synchronized (longPollingIdToLongPollingClient) {
            LongPollingClient lpClient = (LongPollingClient) longPollingIdToLongPollingClient.get(id);
            if (lpClient != null) {
                return lpClient.getLongPollingClientStats();
            }
            return null;
        }
    }

    public LongPollingServerStats getLongPollingServerStatistics(String id) {
        synchronized (longPollingToLongPollingServer) {
            LongPollingServer lpServer = (LongPollingServer) longPollingToLongPollingServer.get(id);
            if (lpServer != null) {
                return lpServer.getLongPollingServerStats();
            }
            return null;
        }
    }

    public List/* <String> */listLongPollingClients() {
        List results = new ArrayList();
        synchronized (longPollingIdToLongPollingClient) {
            Iterator it = longPollingIdToLongPollingClient.keySet().iterator();
            while (it.hasNext()) {
                results.add(it.next());
            }
        }
        return results;
    }

    public List/* <String> */listLongPollingServers() {
        List results = new ArrayList();
        synchronized (longPollingToLongPollingServer) {
            Iterator it = longPollingToLongPollingServer.keySet().iterator();
            while (it.hasNext()) {
                results.add(it.next());
            }
        }
        return results;
    }
}
