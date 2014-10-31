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
 * id $Id: HttpServerTransaction.java 9482 2014-09-06 09:37:41Z JReich $
 * author $Author: JReich $
 * version $Revision: 9482 $
 * lastrevision $Date: 2014-09-06 11:37:41 +0200 (Sat, 06 Sep 2014) $
 * modifiedby $LastChangedBy: JReich $
 * lastmodified $LastChangedDate: 2014-09-06 11:37:41 +0200 (Sat, 06 Sep 2014) $
 */

package com.actility.m2m.http.server;

import java.io.BufferedOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.io.OutputStreamWriter;
import java.io.PrintWriter;
import java.net.Socket;
import java.net.SocketException;
import java.net.SocketTimeoutException;

import javax.servlet.http.HttpServletResponse;

import org.apache.log4j.Logger;

import com.actility.m2m.http.HttpUtils;
import com.actility.m2m.m2m.StatusCode;
import com.actility.m2m.servlet.song.http.log.BundleLogger;
import com.actility.m2m.util.MultiMap;
import com.actility.m2m.util.log.OSGiLogger;

/**
 * Represents an HTTP server transaction.
 * <p>
 * It contains HTTP server request parameters.
 * <p>
 * It allows to set HTTP server response parameters and to send it.
 */
public final class HttpServerTransaction implements Runnable {
    private static final Logger LOG = OSGiLogger.getLogger(HttpServerTransaction.class, BundleLogger.LOG);

    private static final int ST_WAITING_FIRST = 1;
    private static final int ST_WAITING_FIRST_CLOSING = 2;
    private static final int ST_WAITING = 4;
    private static final int ST_REQUEST_PENDING = 8;
    private static final int ST_REQUEST_PENDING_CLOSING = 16;
    private static final int ST_CLOSING = 32;
    private static final int ST_CLOSED = 64;

    private static final int STS_WAITING = ST_WAITING | ST_WAITING_FIRST;
    private static final int STS_REQUEST_PENDING = ST_REQUEST_PENDING | ST_REQUEST_PENDING_CLOSING;
    private static final int STS_CLOSING = ST_REQUEST_PENDING_CLOSING | ST_CLOSING;
    private static final int STS_REQUEST_PENDING_OR_CLOSING = ST_REQUEST_PENDING | ST_REQUEST_PENDING_CLOSING | ST_CLOSING;

    private final HttpServerManager httpServerManager;
    private final MultiMap headers;
    private final byte[] buffer;

    private boolean sent;
    private boolean writerFlushed;
    private PrintWriter writer;
    private Socket socket;
    private InputStream inputStream;
    private int state;
    private String method;
    private int methodCode;
    private String httpVersion;
    private String rawRequestUri;
    private int contentLength;
    private String host;
    private boolean keepAlive;
    private String id;
    private int headersLength;

    private int read;
    private int position;

    /**
     * Creates an HTTP server transaction.
     *
     * @param httpServerManager The HTTP server that created this transaction
     * @param maxBufferSize The maximum buffer size allowed for this transaction
     */
    public HttpServerTransaction(HttpServerManager httpServerManager, int maxBufferSize) {
        this.httpServerManager = httpServerManager;
        this.buffer = new byte[maxBufferSize];
        this.headers = new MultiMap(String.CASE_INSENSITIVE_ORDER);
        this.state = ST_WAITING_FIRST;
    }

    /**
     * Associates this transaction to an HTTP socket
     *
     * @param socket The socket to link to this transaction
     * @throws IOException If any problem occurs while associating the HTTP socket to the transaction
     */
    public void init(Socket socket) throws IOException {
        this.socket = socket;
        this.inputStream = socket.getInputStream();
        this.state = ST_WAITING_FIRST;
        this.id = socket.getInetAddress().getHostAddress() + ":" + socket.getPort();
        if (LOG.isInfoEnabled()) {
            LOG.info(id + ": init " + this);
        }
    }

    /**
     * Resets the HTTP server transaction so that it can be re-used and associated later to a new HTTP socket
     */
    public void reset() {
        if (LOG.isDebugEnabled()) {
            LOG.debug(id + ": reset " + this);
        }
        sent = false;
        writerFlushed = false;
        writer = null;
        method = null;
        methodCode = 0;
        rawRequestUri = null;
        httpVersion = null;
        contentLength = 0;
        host = null;
        keepAlive = false;
        read = 0;
        position = 0;
        headersLength = 0;
        headers.clear();
    }

    /**
     * Deletes this transaction and stops the pending HTTP server transaction
     */
    public void destroy() {
        if (LOG.isDebugEnabled()) {
            LOG.debug(id + ": destroy " + this);
        }
        reset();
        socket = null;
        inputStream = null;
        state = 0;
        id = null;
    }

    /**
     * Gets the HTTP socket associated to this transaction if any
     *
     * @return The HTTP socket associated to this transaction if any
     */
    public Socket getSocket() {
        return socket;
    }

    /**
     * Closes the HTTP socket connection because the request is terminated and the connection is not keep-alive.
     * <p>
     * The socket must not be removed from the socket manager as it is already done.
     */
    public synchronized void terminatedAndClose() {
        // State machine
        // ST_WAITING_FIRST => -
        // ST_WAITING_FIRST_CLOSING => -
        // ST_WAITING => -
        // ST_REQUEST_PENDING => ST_CLOSED
        // ST_REQUEST_PENDING_CLOSING => ST_CLOSED
        // ST_CLOSING => ST_CLOSED
        // ST_CLOSED => ST_CLOSED
        if (LOG.isInfoEnabled()) {
            LOG.info(id + " Received event terminatedAndClose");
        }
        if ((state & STS_REQUEST_PENDING_OR_CLOSING) != 0) {
            if (LOG.isInfoEnabled()) {
                LOG.info(id + " Switching to state: ST_CLOSED");
            }
            state = ST_CLOSED;
            try {
                socket.close();
            } catch (IOException e) {
                LOG.error(
                        "IOException while closing HTTP socket " + socket.getInetAddress().getHostAddress() + ":"
                                + socket.getPort(), e);
            }
        }
    }

    /**
     * Closes the HTTP socket connection if it is waiting for an incoming HTTP request or close the connection when the request
     * is over.
     * <p>
     * The socket must not be removed from the socket manager as it is already done.
     */
    public synchronized void close() {
        // State machine
        // ST_WAITING_FIRST => ST_WAITING_FIRST_CLOSING
        // ST_WAITING_FIRST_CLOSING => ST_WAITING_FIRST_CLOSING
        // ST_WAITING => ST_CLOSED
        // ST_REQUEST_PENDING => ST_REQUEST_PENDING_CLOSING
        // ST_REQUEST_PENDING_CLOSING => ST_REQUEST_PENDING_CLOSING
        // ST_CLOSING => ST_CLOSING
        // ST_CLOSED => ST_CLOSED
        if (LOG.isInfoEnabled()) {
            LOG.info(id + " Received event close");
        }
        switch (state) {
        case ST_WAITING_FIRST:
            if (LOG.isInfoEnabled()) {
                LOG.info(id + " Switching to state: ST_WAITING_FIRST_CLOSING");
            }
            state = ST_WAITING_FIRST_CLOSING;
            break;
        case ST_WAITING:
            if (LOG.isInfoEnabled()) {
                LOG.info(id + " Switching to state: ST_CLOSED");
            }
            state = ST_CLOSED;
            try {
                socket.close();
            } catch (IOException e) {
                LOG.error(
                        "IOException while closing HTTP socket " + socket.getInetAddress().getHostAddress() + ":"
                                + socket.getPort(), e);
            }
            break;
        case ST_REQUEST_PENDING:
            if (LOG.isInfoEnabled()) {
                LOG.info(id + " Switching to state: ST_REQUEST_PENDING_CLOSING");
            }
            state = ST_REQUEST_PENDING_CLOSING;
            break;
        }
    }

    public void run() {
        HttpServerManager httpServerManager = this.httpServerManager;
        Socket socket = this.socket;
        if (LOG.isInfoEnabled()) {
            LOG.info(id + " Received event init");
        }
        synchronized (this) {
            // State machine
            // ST_WAITING_FIRST => ST_WAITING_FIRST
            // ST_WAITING_FIRST_CLOSING => ST_WAITING_FIRST_CLOSING
            // ST_WAITING => -
            // ST_REQUEST_PENDING => ST_WAITING
            // ST_REQUEST_PENDING_CLOSING => ST_CLOSING
            // ST_CLOSING => ST_CLOSING
            // ST_CLOSED => ST_CLOSED
            if (state == ST_REQUEST_PENDING) {
                if (LOG.isInfoEnabled()) {
                    LOG.info(id + " Switching to state: ST_WAITING");
                }
                state = ST_WAITING;
            } else if (state == ST_REQUEST_PENDING_CLOSING) {
                if (LOG.isInfoEnabled()) {
                    LOG.info(id + " Switching to state: ST_WAITING_CLOSING");
                }
                state = ST_CLOSING;
            }
        }
        try {
            if (state < ST_CLOSING) {
                try {
                    socket.setSoTimeout(httpServerManager.getConnectionTimeout());
                    read = readStream(0, buffer.length);
                    position = 0;
                    // Block until a request is received
                    // This could lead to a timeout
                    if (LOG.isInfoEnabled()) {
                        LOG.info(id + " Received event request received");
                    }
                    synchronized (this) {
                        // State machine
                        // ST_WAITING_FIRST => ST_REQUEST_PENDING
                        // ST_WAITING_FIRST_CLOSING => ST_REQUEST_PENDING_CLOSING
                        // ST_WAITING => ST_REQUEST_PENDING
                        // ST_REQUEST_PENDING => -
                        // ST_REQUEST_PENDING_CLOSING => -
                        // ST_CLOSING => ST_CLOSING
                        // ST_CLOSED => ST_CLOSED
                        if ((state & STS_WAITING) != 0) {
                            if (LOG.isInfoEnabled()) {
                                LOG.info(id + " Switching to state: ST_REQUEST_PENDING");
                            }
                            state = ST_REQUEST_PENDING;
                        } else if (state == ST_WAITING_FIRST_CLOSING) {
                            if (LOG.isInfoEnabled()) {
                                LOG.info(id + " Switching to state: ST_REQUEST_PENDING_CLOSING");
                            }
                            state = ST_REQUEST_PENDING_CLOSING;
                        }
                    }

                    if ((state & STS_REQUEST_PENDING) != 0) {
                        handleRequest();
                    }
                } catch (SocketException e) {
                    if (LOG.isInfoEnabled()) {
                        LOG.info(id + "Socket has been explicitly closed");
                    }
                    if (state != ST_CLOSED) {
                        if (LOG.isInfoEnabled()) {
                            LOG.info(id + " Switching to state: ST_CLOSING");
                        }
                        state = ST_CLOSING;
                    }
                } catch (SocketTimeoutException e) {
                    if (LOG.isInfoEnabled()) {
                        LOG.info(id + " Socket has expired");
                    }
                    if (state != ST_CLOSED) {
                        if (LOG.isInfoEnabled()) {
                            LOG.info(id + " Switching to state: ST_CLOSING");
                        }
                        state = ST_CLOSING;
                    }
                } catch (IOException e) {
                    LOG.error(id + "IO exception while reading socket input stream", e);
                    if (state != ST_CLOSED) {
                        if (LOG.isInfoEnabled()) {
                            LOG.info(id + " Switching to state: ST_CLOSING");
                        }
                        state = ST_CLOSING;
                    }
                } catch (HttpServerException e) {
                    if (state != ST_CLOSED) {
                        if (LOG.isInfoEnabled()) {
                            LOG.info(id + " Switching to state: ST_CLOSING");
                        }
                        state = ST_CLOSING;
                    }
                    httpServerManager.getHandler().doServerError(e, this);
                } catch (RuntimeException e) {
                    LOG.error("RuntimeException in HTTP transaction thread", e);
                    if (state != ST_CLOSED) {
                        if (LOG.isInfoEnabled()) {
                            LOG.info(id + " Switching to state: ST_CLOSING");
                        }
                        state = ST_CLOSING;
                    }
                    httpServerManager.getHandler().doServerError(
                            new HttpServerException("RuntimeException while treating incoming HTTP request",
                                    StatusCode.STATUS_INTERNAL_SERVER_ERROR, HttpServletResponse.SC_INTERNAL_SERVER_ERROR,
                                    HttpUtils.RP_INTERNAL_SERVER_ERROR, e), this);
                }
            }
        } catch (RuntimeException e) {
            LOG.error("RuntimeException while treating an error", e);
        } finally {
            if (state == ST_CLOSING) {
                try {
                    socket.close();
                } catch (IOException e) {
                    LOG.error("IO exception while closing socket", e);
                } catch (RuntimeException e) {
                    LOG.error("RuntimeException while finalizing http transaction", e);
                } finally {
                    if (LOG.isInfoEnabled()) {
                        LOG.info(id + " Switching to state: ST_CLOSED");
                    }
                    state = ST_CLOSED;
                    transactionTerminated(false);
                }
            }
        }
    }

    /**
     * Returns whether this HTTP server transaction is secure.
     *
     * @return Whether this HTTP server transaction is secure
     */
    public boolean isSecure() {
        return httpServerManager.isSecure();
    }

    /**
     * Gets the HTTP server method (could be GET, PUT, POST or DELETE)
     *
     * @return The HTTP server method
     */
    public String getMethod() {
        return method;
    }

    /**
     * Gets the HTTP server method code:
     * <ul>
     * <li>GET: 0</li>
     * <li>POST: 1</li>
     * <li>PUT: 2</li>
     * <li>DELETE: 3</li>
     * </ul>
     *
     * @return The HTTP server method code
     */
    public int getMethodCode() {
        return methodCode;
    }

    /**
     * Gets the HTTP server version part.
     * <p>
     * For example, HTTP/1.0 or HTTP/1.1
     *
     * @return The HTTP server version part
     */
    public String getHttpVersion() {
        return httpVersion;
    }

    /**
     * Gets the HTTP request URI as received from the network
     *
     * @return The HTTP request URI as received from the network
     */
    public String getRawRequestUri() {
        return rawRequestUri;
    }

    /**
     * Gets the HTTP server request content-length
     *
     * @return The HTTP server request content-length
     */
    public int getContentLength() {
        return contentLength;
    }

    /**
     * Gets the HTTP server request host as received in Host header
     *
     * @return The HTTP server request host as received in Host header
     */
    public String getHost() {
        return host;
    }

    /**
     * Gets a MultiMap containing all HTTP server request headers
     *
     * @return A multimap containing all HTTP server request headers
     */
    public MultiMap getHeaders() {
        return headers;
    }

    /**
     * Get the HTTP server request content as a whole
     *
     * @return The HTTP server request content as a whole
     */
    public byte[] getRawContent() {
        return buffer;
    }

    /**
     * Gets the HTTP server transaction id generated from the remote connection IP+port
     *
     * @return The HTTP server transaction id generated from the remote connection IP+port
     */
    public String getId() {
        return id;
    }

    /**
     * Prints a character to send to the remote peer and part of the HTTP server response
     *
     * @param character The character to send to remote peer
     */
    public void print(char character) {
        if ((state & STS_CLOSING) == 0) {
            checkWriter();
            writer.print(character);
        }
    }

    /**
     * Prints an integer to send to the remote peer as a string and part of the HTTP server response
     *
     * @param value The integer to send to remote peer as a string
     */
    public void print(int value) {
        if ((state & STS_CLOSING) == 0) {
            checkWriter();
            writer.print(value);
        }
    }

    /**
     * Prints a string to send to the remote peer and part of the HTTP server response
     *
     * @param content The string to send to remote peer
     */
    public void print(String content) {
        if ((state & STS_CLOSING) == 0) {
            checkWriter();
            writer.print(content);
        }
    }

    /**
     * Sends the given body to the remote peer and part of the HTTP server response
     *
     * @param rawContent The body to send to remote peer
     */
    public void writeBody(byte[] rawContent) {
        if ((state & STS_CLOSING) == 0) {
            checkWriter();
            writerFlushed = true;
            if (writer.checkError()) {
                state = ST_CLOSING;
            }
            if ((state & STS_CLOSING) == 0) {
                try {
                    socket.getOutputStream().write(rawContent);
                } catch (IOException e) {
                    LOG.error(e.getMessage(), e);
                    state = ST_CLOSING;
                }
            }
        }
    }

    /**
     * Flushes the HTTP server response to the remote peer
     *
     * @throws IOException In case any problem occurrs while sending the HTTP server response
     */
    public void send() throws IOException {
        try {
            if ((state & STS_CLOSING) != 0) {
                throw new IOException("Problem while sending HTTP response to remote peer");
            }
            checkWriter();
            if (!writerFlushed) {
                if (writer.checkError()) {
                    state = ST_CLOSING;
                }
                if ((state & STS_CLOSING) != 0) {
                    throw new IOException("Problem while sending HTTP response to remote peer");
                }
            } else {
                socket.getOutputStream().flush();
            }
            sent = true;
        } catch (IOException e) {
            state = ST_CLOSING;
            throw e;
        }
    }

    /**
     * Terminates the HTTP server transaction processing so that it can be re-used to treat another HTTP request in the same
     * socket if the HTTP connection is maintained. Otherwise the transaction returns to the HTTP server pool and while be
     * associated to a new HTTP socket later on.
     */
    public void terminated() {
        if (!sent) {
            state = ST_CLOSING;
        }
        transactionTerminated((state & STS_CLOSING) == 0 && keepAlive);
    }

    private void checkWriter() {
        if (writer == null) {
            try {
                OutputStream outputStream = new BufferedOutputStream(socket.getOutputStream());
                writer = new PrintWriter(new OutputStreamWriter(outputStream, "ISO-8859-1"));
            } catch (IOException e) {
                LOG.error(e.getMessage(), e);
                state = ST_CLOSING;
            }
        }
    }

    private void transactionTerminated(boolean keepAlive) {
        httpServerManager.transactionTerminated(this, keepAlive);
    }

    private int readStream(int offset, int length) throws IOException {
        int bytesRead = inputStream.read(buffer, offset, length);
        if (bytesRead == -1) {
            throw new SocketException("Remote side has closed the socket");
        }
        return bytesRead;
    }

    private void initBuffer() throws IOException {
        if (read == 0 || position == read) {
            // Read input stream
            read = readStream(0, buffer.length);
            position = 0;
        }
    }

    private void handleRequest() throws IOException, HttpServerException {
        parseRequestLine();
        parseHeaders();

        boolean http11 = httpVersion.equals(HttpUtils.PT_HTTP_1_1);

        host = (String) headers.get(HttpUtils.HD_HOST);
        if (http11 && rawRequestUri.charAt(0) == '/' && host == null) {
            throw new HttpServerException("No Host header detected", StatusCode.STATUS_BAD_REQUEST,
                    HttpServletResponse.SC_BAD_REQUEST, HttpUtils.RP_BAD_REQUEST);
        }
        keepAlive = http11;
        String connectionStr = (String) headers.get(HttpUtils.HD_CONNECTION);
        if (connectionStr == null) {
            connectionStr = (String) headers.get(HttpUtils.HD_PROXY_CONNECTION);
        }
        if (connectionStr != null) {
            int index = connectionStr.indexOf(',');

            if (index != -1) {
                int offset = 0;
                int end = index;
                String value = null;

                while (index != -1) {
                    if (offset < end && connectionStr.charAt(offset) <= ' ') {
                        do {
                            ++offset;
                        } while (offset < end && connectionStr.charAt(offset) <= ' ');
                    }
                    if ((end - 1) > offset && connectionStr.charAt(end - 1) <= ' ') {
                        do {
                            --end;
                        } while ((end - 1) > offset && connectionStr.charAt(end - 1) <= ' ');
                    }
                    value = connectionStr.substring(offset, end);
                    if ("close".equalsIgnoreCase(value)) {
                        keepAlive = false;
                        index = -1;
                    } else if ("keep-alive".equalsIgnoreCase(value)) {
                        keepAlive = true;
                        index = -1;
                    } else {
                        offset = index + 1;
                        index = connectionStr.indexOf(',', index + 1);
                        end = index;
                    }
                }
                end = connectionStr.length();
                if (offset < end && connectionStr.charAt(offset) <= ' ') {
                    do {
                        ++offset;
                    } while (offset < end && connectionStr.charAt(offset) <= ' ');
                }
                if ((end - 1) > offset && connectionStr.charAt(end - 1) <= ' ') {
                    do {
                        --end;
                    } while ((end - 1) > offset && connectionStr.charAt(end - 1) <= ' ');
                }
                value = connectionStr.substring(offset, end);
                if ("close".equalsIgnoreCase(value)) {
                    keepAlive = false;
                } else if ("keep-alive".equalsIgnoreCase(value)) {
                    keepAlive = true;
                } else if (index == -1) {
                    LOG.error("Received a Connection header with an unknown value. Fall back to default");
                }
            } else {
                if ("close".equalsIgnoreCase(connectionStr)) {
                    keepAlive = false;
                } else if ("keep-alive".equalsIgnoreCase(connectionStr)) {
                    keepAlive = true;
                } else {
                    LOG.error("Received a Connection header with an unknown value. Fall back to default");
                }
            }
        }
        if (methodCode == HttpUtils.MD_POST_CODE || methodCode == HttpUtils.MD_PUT_CODE) {
            String contentLengthStr = (String) headers.get(HttpUtils.HD_CONTENT_LENGTH);
            if (contentLengthStr != null) {
                try {
                    contentLength = Integer.parseInt(contentLengthStr);
                    if (contentLength > httpServerManager.getMaxContentLength()) {
                        throw new HttpServerException("Received request with a too large content: " + contentLength,
                                StatusCode.STATUS_BAD_REQUEST, HttpServletResponse.SC_REQUEST_ENTITY_TOO_LARGE,
                                HttpUtils.RP_REQUEST_ENTITY_TOO_LARGE);
                    }
                } catch (NumberFormatException e) {
                    throw new HttpServerException("Content-Length header is not a number", StatusCode.STATUS_BAD_REQUEST,
                            HttpServletResponse.SC_BAD_REQUEST, HttpUtils.RP_BAD_REQUEST);
                }
            } else {
                throw new HttpServerException("Received request with no Content-Length", StatusCode.STATUS_BAD_REQUEST,
                        HttpServletResponse.SC_LENGTH_REQUIRED, HttpUtils.RP_LENGTH_REQUIRED);
            }
        } else {
            contentLength = 0;
            headers.remove(HttpUtils.HD_CONTENT_LENGTH);
        }
        if (contentLength != 0) {
            parseContent();
        }
        httpServerManager.getHandler().doServerRequest(this);
    }

    private void readMore() throws HttpServerException, IOException {
        if (position != 0) {
            // Move non-read part from buffer to the beginning
            System.arraycopy(buffer, position, buffer, 0, read - position);
            read -= position;
            position = 0;
            read += readStream(read, buffer.length - read);
        } else if (read < buffer.length) {
            read += readStream(read, buffer.length - read);
        } else {
            throw new HttpServerException("HTTP line is too long", StatusCode.STATUS_BAD_REQUEST,
                    HttpServletResponse.SC_REQUEST_ENTITY_TOO_LARGE, HttpUtils.RP_REQUEST_ENTITY_TOO_LARGE);
        }
    }

    private int indexOf(byte[] buffer, int offset, byte value) {
        for (int i = offset; i < read; ++i) {
            if (buffer[i] == value) {
                return i;
            }
        }
        return -1;
    }

    private int indexOf(byte[] buffer, int offset, byte value1, byte value2) {
        byte value = 0;
        for (int i = offset; i < read; ++i) {
            value = buffer[i];
            if (value == value1 || value == value2) {
                return i;
            }
        }
        return -1;
    }

    private void skipSPandHT() throws IOException {
        byte value = 0;
        for (int i = position; i < read; ++i) {
            value = buffer[i];
            if (value == (byte) ' ' || value == (byte) '\t') {
                ++position;
            } else {
                break;
            }
        }
        while (position == read) {
            read = readStream(0, buffer.length);
            position = 0;
            for (int i = 0; i < read; ++i) {
                value = buffer[i];
                if (value == (byte) ' ' || value == (byte) '\t') {
                    ++position;
                } else {
                    break;
                }
            }
        }
    }

    private char readCurrent() throws IOException {
        initBuffer();
        return (char) buffer[position];
    }

    private String readHeaderName() throws IOException, HttpServerException {
        initBuffer();
        int current = position;
        int index = 0;
        boolean end = false;
        byte value = 0;
        while (!end) {
            index = indexOf(buffer, current, (byte) '\r', (byte) ':');
            // We reach end of string
            if (index == -1) {
                if (position != 0) {
                    current -= position;
                }
                readMore();
            } else {
                value = buffer[index];
                if (value == (byte) '\r') {
                    throw new HttpServerException("No header name detected on line: "
                            + new String(buffer, position, index - position), StatusCode.STATUS_BAD_REQUEST,
                            HttpServletResponse.SC_BAD_REQUEST, HttpUtils.RP_BAD_REQUEST);
                }
                end = true;
            }
        }
        int begin = position;
        int length = index - begin;
        position = index + 1;
        headersLength += length;
        if (length > httpServerManager.getMaxHeaderLineLength()) {
            throw new HttpServerException("HTTP header is too large", StatusCode.STATUS_BAD_REQUEST,
                    HttpServletResponse.SC_REQUEST_ENTITY_TOO_LARGE, HttpUtils.RP_REQUEST_ENTITY_TOO_LARGE);
        } else if (headersLength > httpServerManager.getMaxHeadersLength()) {
            throw new HttpServerException("HTTP header part is too large", StatusCode.STATUS_BAD_REQUEST,
                    HttpServletResponse.SC_REQUEST_ENTITY_TOO_LARGE, HttpUtils.RP_REQUEST_ENTITY_TOO_LARGE);
        }
        skipSPandHT();
        return new String(buffer, begin, length);
    }

    private String readLine(int prefixLength) throws IOException, HttpServerException {
        initBuffer();
        int current = position;
        int index = 0;
        boolean end = false;
        while (!end) {
            index = indexOf(buffer, current, (byte) '\n');
            // We reach end of string
            if (index == -1) {
                if (position != 0) {
                    current -= position;
                }
                readMore();
            } else {
                --index;
                if (index >= 0 && buffer[index] != (byte) '\r') {
                    // Sequence is not found
                    current = index + 2;
                } else {
                    end = true;
                }
            }
        }

        int begin = position;
        int length = index - begin;
        position = index + 2;
        headersLength += length;
        if ((prefixLength + length) > httpServerManager.getMaxHeaderLineLength()) {
            throw new HttpServerException("HTTP header is too large", StatusCode.STATUS_BAD_REQUEST,
                    HttpServletResponse.SC_REQUEST_ENTITY_TOO_LARGE, HttpUtils.RP_REQUEST_ENTITY_TOO_LARGE);
        } else if (headersLength > httpServerManager.getMaxHeadersLength()) {
            throw new HttpServerException("HTTP header part is too large", StatusCode.STATUS_BAD_REQUEST,
                    HttpServletResponse.SC_REQUEST_ENTITY_TOO_LARGE, HttpUtils.RP_REQUEST_ENTITY_TOO_LARGE);
        }
        return new String(buffer, begin, length);
    }

    private String readUntilSP() throws IOException, HttpServerException {
        initBuffer();
        int index = indexOf(buffer, position, (byte) '\r', (byte) ' ');
        while (index == -1) {
            readMore();
            index = indexOf(buffer, position, (byte) '\r', (byte) ' ');
        }
        if (buffer[index] == '\r') {
            throw new HttpServerException("Cannot decode HTTP request line", StatusCode.STATUS_BAD_REQUEST,
                    HttpServletResponse.SC_BAD_REQUEST, HttpUtils.RP_BAD_REQUEST);
        }
        int begin = position;
        int length = (index - begin);
        position = index + 1;
        headersLength += length;
        if (length > httpServerManager.getMaxHeaderLineLength()) {
            throw new HttpServerException("HTTP request-line is too large", StatusCode.STATUS_BAD_REQUEST,
                    HttpServletResponse.SC_REQUEST_ENTITY_TOO_LARGE, HttpUtils.RP_REQUEST_ENTITY_TOO_LARGE);
        } else if (headersLength > httpServerManager.getMaxHeadersLength()) {
            throw new HttpServerException("HTTP header part is too large", StatusCode.STATUS_BAD_REQUEST,
                    HttpServletResponse.SC_REQUEST_ENTITY_TOO_LARGE, HttpUtils.RP_REQUEST_ENTITY_TOO_LARGE);
        }
        return new String(buffer, begin, length);
    }

    /**
     * Parses the request line of the received HTTP request.
     * <p>
     * <code>Request-Line   = Method SP Request-URI SP HTTP-Version CRLF</code>
     */
    private void parseRequestLine() throws IOException, HttpServerException {
        method = readUntilSP().toUpperCase();
        if (HttpUtils.MD_GET.equals(method)) {
            methodCode = HttpUtils.MD_GET_CODE;
        } else if (HttpUtils.MD_POST.equals(method)) {
            methodCode = HttpUtils.MD_POST_CODE;
        } else if (HttpUtils.MD_PUT.equals(method)) {
            methodCode = HttpUtils.MD_PUT_CODE;
        } else if (HttpUtils.MD_DELETE.equals(method)) {
            methodCode = HttpUtils.MD_DELETE_CODE;
        } else {
            throw new HttpServerException("Not implemented HTTP request recevied: " + method, StatusCode.STATUS_BAD_REQUEST,
                    HttpServletResponse.SC_NOT_IMPLEMENTED, HttpUtils.RP_NOT_IMPLEMENTED);
        }
        if (LOG.isInfoEnabled()) {
            LOG.info(id + ": Parsed method is " + method);
        }

        // parse requestUri
        rawRequestUri = readUntilSP();
        if (LOG.isInfoEnabled()) {
            LOG.info(id + ": Parsed requestURI is " + rawRequestUri);
        }

        // parse httpVersion
        httpVersion = readLine(0);
        if (LOG.isInfoEnabled()) {
            LOG.info(id + ": Parsed http version is " + httpVersion);
        }
    }

    private void parseHeaders() throws HttpServerException, IOException {
        String name = null;
        int maxHeaders = httpServerManager.getMaxNbOfHeaders();
        boolean end = false;
        char currentChar = 0;
        StringBuffer buffer = new StringBuffer();

        while (!end) {
            currentChar = readCurrent();
            switch (currentChar) {
            case ' ':
            case '\t':
                ++position;
                if (name == null) {
                    throw new HttpServerException("Continued header found on the first line", StatusCode.STATUS_BAD_REQUEST,
                            HttpServletResponse.SC_BAD_REQUEST, HttpUtils.RP_BAD_REQUEST);
                }
                skipSPandHT();
                buffer.append(readLine(name.length() + buffer.length()));
                break;
            case '\r':
                ++position;
                if (readCurrent() == '\n') {
                    ++position;
                    end = true;
                } else {
                    throw new HttpServerException("An HTTP line starts with a \r without a \n after",
                            StatusCode.STATUS_BAD_REQUEST, HttpServletResponse.SC_BAD_REQUEST, HttpUtils.RP_BAD_REQUEST);
                }
                break;
            default:
                if (buffer.length() > 0) {
                    if (LOG.isInfoEnabled()) {
                        LOG.info(id + ": Add header " + name + ": " + buffer.toString());
                    }
                    headers.putLast(name, buffer.toString());
                    buffer.setLength(0);
                    if (headers.size() > maxHeaders) {
                        throw new HttpServerException("Too much headers received in the request",
                                StatusCode.STATUS_BAD_REQUEST, HttpServletResponse.SC_REQUEST_ENTITY_TOO_LARGE,
                                HttpUtils.RP_REQUEST_ENTITY_TOO_LARGE);
                    }
                }
                name = readHeaderName().toLowerCase();

                // add header value
                buffer.append(readLine(name.length()));
                break;
            }
        }
        if (buffer.length() > 0) {
            if (LOG.isInfoEnabled()) {
                LOG.info(id + ": Add header " + name + ": " + buffer.toString());
            }
            headers.putLast(name, buffer.toString());
            buffer.setLength(0);
            if (headers.size() > maxHeaders) {
                throw new HttpServerException("Too much headers received in the request", StatusCode.STATUS_BAD_REQUEST,
                        HttpServletResponse.SC_REQUEST_ENTITY_TOO_LARGE, HttpUtils.RP_REQUEST_ENTITY_TOO_LARGE);
            }
        }
    }

    private void parseContent() throws IOException {
        if (position != 0) {
            // Move non-read part from buffer to the beginning
            System.arraycopy(buffer, position, buffer, 0, read - position);
            read -= position;
            while (read < contentLength) {
                read += readStream(read, contentLength - read);
            }
        }
    }
}
