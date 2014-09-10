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
 * id $Id: HttpServerManager.java 8521 2014-04-14 09:05:59Z JReich $
 * author $Author: JReich $
 * version $Revision: 8521 $
 * lastrevision $Date: 2014-04-14 11:05:59 +0200 (Mon, 14 Apr 2014) $
 * modifiedby $LastChangedBy: JReich $
 * lastmodified $LastChangedDate: 2014-04-14 11:05:59 +0200 (Mon, 14 Apr 2014) $
 */

package com.actility.m2m.http.server;

import java.io.IOException;
import java.net.ServerSocket;
import java.net.Socket;
import java.net.SocketException;

import org.apache.log4j.Logger;

import com.actility.m2m.servlet.song.http.log.BundleLogger;
import com.actility.m2m.util.Profiler;
import com.actility.m2m.util.log.OSGiLogger;


/**
 * HTTP server manager that maintain the listening socket and distribute incoming sockets to HTTP server transactions.
 */
public final class HttpServerManager implements Runnable {
    private static final Logger LOG = OSGiLogger.getLogger(HttpServerManager.class, BundleLogger.LOG);

    private boolean end;

    private final int serverPort;
    private final HttpServerHandler handler;
    private final int maxBufferSize;
    private final boolean secure;

    private ServerSocket serverSocket;
    private HttpServerTransaction[] transactionsPool;
    private int transactionsCount;

    private int connectionTimeout;
    private int maxNbOfHeaders;
    private final int maxHeaderLineLength;
    private final int maxHeadersLength;
    private final int maxContentLength;

    /**
     * Creates the HTTP server manager
     *
     * @param handler The HTTP server handler that will be notified of all HTTP requests
     * @param secure Whether this server is secure
     * @param serverPort The HTTP server port
     * @param maxSockets The maximum number of sockets allowed on the server
     * @param connectionTimeout The connection timeout
     * @param maxNbOfHeaders The maximum number of headers allowed in an incoming HTTP request
     * @param maxHeaderLineLength The maximum header line length allowed in an incoming HTTP request
     * @param maxHeadersLength The maximum header part length allowed in an incoming HTTP request
     * @param maxContentLength The maximum content-length allowed in an incoming HTTP request
     * @throws IOException If any problem occurs while creating the HTTP server
     */
    public HttpServerManager(HttpServerHandler handler, boolean secure, int serverPort, int maxSockets, int connectionTimeout,
            int maxNbOfHeaders, int maxHeaderLineLength, int maxHeadersLength, int maxContentLength) throws IOException {
        this.handler = handler;
        int maxBufferSize = maxContentLength;
        if (maxHeadersLength > maxBufferSize) {
            maxBufferSize = maxHeadersLength;
        }
        if (maxHeaderLineLength > maxBufferSize) {
            maxBufferSize = maxHeaderLineLength;
        }
        this.maxBufferSize = maxBufferSize;
        this.secure = secure;
        end = false;
        this.serverPort = serverPort;
        transactionsPool = new HttpServerTransaction[maxSockets];
        transactionsCount = maxSockets;
        for (int i = 0; i < maxSockets; ++i) {
            transactionsPool[i] = new HttpServerTransaction(this, maxBufferSize);
        }
        this.connectionTimeout = connectionTimeout;
        this.maxNbOfHeaders = maxNbOfHeaders;
        this.maxHeaderLineLength = maxHeaderLineLength;
        this.maxHeadersLength = maxHeadersLength;
        this.maxContentLength = maxContentLength;
    }

    /**
     * Stops and destroys every pending HTTP server transactions
     */
    public void destroy() {
        end = true;
        try {
            if (serverSocket != null) {
                serverSocket.close();
            }
        } catch (IOException e) {
            LOG.error("Cannot close HTTP server socket", e);
        }
    }

    /**
     * Whether the HTTP server is secure
     *
     * @return Whether the HTTP server is secure
     */
    public boolean isSecure() {
        return secure;
    }

    /**
     * Gets the time to wait until we consider an I/O operation on an HTTP server socket as expired
     *
     * @return The time to wait until we consider an I/O operation on an HTTP server socket as expired
     */
    public int getConnectionTimeout() {
        return connectionTimeout;
    }

    /**
     * Sets the time to wait until we consider an I/O operation on an HTTP server socket as expired
     *
     * @param connectionTimeout The time to wait until we consider an I/O operation on an HTTP server socket as expired
     */
    public void setConnectionTimeout(int connectionTimeout) {
        this.connectionTimeout = connectionTimeout;
    }

    /**
     * Gets the maximum number of headers allowed in an incoming HTTP request
     *
     * @return The maximum number of headers allowed in an incoming HTTP request
     */
    public int getMaxNbOfHeaders() {
        return maxNbOfHeaders;
    }

    /**
     * Sets the maximum number of headers allowed in an incoming HTTP request
     *
     * @param maxNbOfHeaders The maximum number of headers allowed in an incoming HTTP request
     */
    public void setMaxNbOfHeaders(int maxNbOfHeaders) {
        this.maxNbOfHeaders = maxNbOfHeaders;
    }

    /**
     * Gets the maximum header line length allowed in an incoming HTTP request
     *
     * @return The maximum header line length allowed in an incoming HTTP request
     */
    public int getMaxHeaderLineLength() {
        return maxHeaderLineLength;
    }

    /**
     * Gets the maximum header part length allowed in an incoming HTTP request
     *
     * @return The maximum header part length allowed in an incoming HTTP request
     */
    public int getMaxHeadersLength() {
        return maxHeadersLength;
    }

    /**
     * Gets the maximum content-length allowed in an incoming HTTP request
     *
     * @return The maximum content-length allowed in an incoming HTTP request
     */
    public int getMaxContentLength() {
        return maxContentLength;
    }

    /**
     * Gets the HTTP server handler that will be notified of all HTTP requests
     *
     * @return The HTTP server handler that will be notified of all HTTP requests
     */
    public HttpServerHandler getHandler() {
        return handler;
    }

    /**
     * Gets the maximum number of sockets allowed on the server
     *
     * @return The maximum number of sockets allowed on the server
     */
    public synchronized int getMaxSockets() {
        return transactionsPool.length;
    }

    /**
     * Sets the maximum number of sockets allowed on the server
     *
     * @param maxSockets The maximum number of sockets allowed on the server
     */
    public synchronized void setMaxSockets(int maxSockets) {
        if (maxSockets != transactionsPool.length) {
            if (maxSockets < 1) {
                LOG.error("Illegal size for HTTP socket pool (" + maxSockets + ") keep current size ("
                        + transactionsPool.length + ")");
            } else {
                if (maxSockets > transactionsPool.length) {
                    HttpServerTransaction[] tmpPool = this.transactionsPool;
                    transactionsPool = new HttpServerTransaction[maxSockets];
                    System.arraycopy(tmpPool, 0, transactionsPool, 0, transactionsCount);
                    for (int i = transactionsCount; i < maxSockets - tmpPool.length; ++i) {
                        transactionsPool[i] = new HttpServerTransaction(this, maxBufferSize);
                    }
                } else if (maxSockets < transactionsPool.length) {
                    HttpServerTransaction[] tmpPool = this.transactionsPool;
                    transactionsPool = new HttpServerTransaction[maxSockets];
                    if (transactionsCount <= maxSockets) {
                        System.arraycopy(tmpPool, 0, transactionsPool, 0, transactionsCount);
                    } else {
                        System.arraycopy(tmpPool, 0, transactionsPool, 0, maxSockets);
                        transactionsCount = maxSockets;
                    }
                }
            }
        }
    }

    private HttpServerTransaction getTransaction() {
        HttpServerTransaction transaction = null;
        synchronized (transactionsPool) {
            if (transactionsCount > 0) {
                --transactionsCount;
                transaction = transactionsPool[transactionsCount];
            }
        }

        if (LOG.isInfoEnabled()) {
            LOG.info("getTransaction: " + transaction);
        }
        return transaction;
    }

    private void putTransaction(HttpServerTransaction transaction) {
        transaction.destroy();
        if (LOG.isInfoEnabled()) {
            LOG.info("putTransaction: " + transaction);
        }
        synchronized (transactionsPool) {
            if (transactionsCount < transactionsPool.length) {
                transactionsPool[transactionsCount] = transaction;
                ++transactionsCount;
            }
        }
    }

    private synchronized void acceptSocket(Socket socket) throws IOException {
        if (LOG.isInfoEnabled()) {
            LOG.info(socket.getInetAddress().getHostAddress() + ":" + socket.getPort() + ": Incoming HTTP socket");
        }
        if (Profiler.getInstance().isTraceEnabled()) {
            Profiler.getInstance().trace(
                    socket.getInetAddress().getHostAddress() + ":" + socket.getPort() + ": Incoming HTTP socket");
        }
        // TODO check an incoming HTTP socket which is directly closed by client (does it trigger SocketException which stops
        // the mainserver socket ?)
        HttpServerTransaction transaction = getTransaction();
        if (transaction != null) {
            transaction.init(socket);
            try {
                new Thread(transaction, "HttpSocket-" + transaction.getId()).start();
            } catch (Throwable e) {
                e.printStackTrace();
            }
        } else {
            LOG.error("Close incoming socket from " + socket.getInetAddress().getHostAddress() + ":" + socket.getPort()
                    + " because HTTP socket pool is full");
            try {
                socket.close();
            } catch (IOException e) {
                LOG.error("IOException while closing incoming HTTP socket", e);
            }
        }
    }

    synchronized void transactionTerminated(HttpServerTransaction transaction, boolean keepAlive) {
        if (LOG.isInfoEnabled()) {
            LOG.info(transaction.getId() + ": Transaction terminated, keepAlive: " + keepAlive);
        }
        if (keepAlive) {
            transaction.reset();
            new Thread(transaction, "HttpSocket-" + transaction.getId()).start();
        } else {
            transaction.terminatedAndClose();
            putTransaction(transaction);
        }
    }

    public void run() {
        ServerSocket serverSocket = null;
        Socket incomingSocket = null;

        try {
            while (!end) {
                try {
                    // 1. creation
                    if (serverSocket != null) {
                        LOG.error("HTTP server socket has been closed by an external event. Wait 5s and reopen it");
                        Thread.sleep(5000L);
                    }
                    this.serverSocket = new ServerSocket(serverPort);
                    serverSocket = this.serverSocket;

                    // 2. read server socket
                    while (!end && !serverSocket.isClosed()) {
                        try {
                            incomingSocket = serverSocket.accept();
                            acceptSocket(incomingSocket);
                        } catch (SocketException e) {
                            if (!serverSocket.isClosed()) {
                                LOG.error("SocketException in HTTP server socket", e);
                            }
                        } catch (IOException e) {
                            LOG.error("IOException in HTTP server socket", e);
                        } catch (RuntimeException e) {
                            LOG.error("RuntimeException in HTTP server socket", e);
                        }
                    }
                } catch (IOException e) {
                    LOG.error("IOException in HTTP server socket creation", e);
                } catch (RuntimeException e) {
                    LOG.error("RuntimeException in HTTP server socket creation", e);
                } catch (InterruptedException e) {
                    LOG.error("InterruptedException in HTTP server socket creation", e);
                }
            }
            LOG.info("HTTP server has been stopped");
        } finally {
            try {
                if (serverSocket != null) {
                    serverSocket.close();
                }
            } catch (IOException e) {
                LOG.error("IOException while closing HTTP server socket", e);
            }
        }
    }
}
