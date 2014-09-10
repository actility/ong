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
 * id $Id: HttpClient.java 5992 2013-10-07 14:32:35Z JReich $
 * author $Author: JReich $
 * version $Revision: 5992 $
 * lastrevision $Date: 2013-10-07 16:32:35 +0200 (Mon, 07 Oct 2013) $
 * modifiedby $LastChangedBy: JReich $
 * lastmodified $LastChangedDate: 2013-10-07 16:32:35 +0200 (Mon, 07 Oct 2013) $
 */
package com.actility.m2m.http.client.ni;

/**
 * An HTTP client context that permits to send HTTP client requests.
 * <p>
 * Each HttpClient object has its own configuration and its own connections pool
 *
 */
public interface HttpClient {
    /**
     * Creates an HTTP client request to send on the network
     *
     * @param method The HTTP method (could be GET, PUT, POST, DELETE)
     * @param url The targeted URL
     * @return The created HTTP client request
     */
    HttpClientTransaction createTransaction(String method, String url);

    /**
     * Creates an HTTP client request to send on the network
     *
     * @param id An id that uniquely identifies the transaction. This id is not sent over the network, this is an internal
     *            parameter
     * @param method The HTTP method (could be GET, PUT, POST, DELETE)
     * @param url The targeted URL
     * @return The created HTTP client request
     */
    HttpClientTransaction createTransaction(String id, String method, String url);

    /**
     * Copies this HttpClient instance and create a new HTTP client context with the same configuration.
     * <p>
     * The created context has its own connections pool
     *
     * @return The created HttpClient
     */
    HttpClient copy();

    /**
     * Sets proxy authentication parameters
     *
     * @param proxyUsername The proxy user name
     * @param proxyPassword The proxy password
     */
    void setProxyCredentials(String proxyUsername, String proxyPassword);

    /**
     * Sets the proxy
     *
     * @param proxyHost The proxy hostname or IP
     * @param proxyPort The proxy port
     */
    void setProxy(String proxyHost, int proxyPort);

    /**
     * Sets the maximum allowed connections per final host
     *
     * @param maxSocketsPerHost The maximum allowed connections per final host
     */
    void setMaxSocketsPerHost(int maxSocketsPerHost);

    /**
     * Sets the maximum number of HTTP client sockets
     *
     * @param maxTotalSockets The maximum number of HTTP client sockets
     */
    void setMaxTotalSockets(int maxTotalSockets);

    /**
     * Sets whether the TCP no delay feature will be enabled on HTTP connections.
     *
     * @param tcpNoDelay Whether the TCP no delay feature will be enabled on HTTP connections
     */
    void setTcpNoDelay(boolean tcpNoDelay);

    /**
     * Sets the time to wait until we consider an unresponded HTTP client request as expired.
     *
     *
     * @param requestTimeout The time to wait until we consider an unresponded HTTP client request as expired
     */
    void setRequestTimeout(long requestTimeout);

    /**
     * Sets the time to wait until we consider an unresponded HTTP client socket as expired.
     *
     * @param connectionTimeout The time to wait until we consider an unresponded HTTP client socket as expired
     */
    void setConnectionTimeout(long connectionTimeout);
}
