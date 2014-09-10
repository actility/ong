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
 * id $Id: LocalResponse.java 8767 2014-05-21 15:41:33Z JReich $
 * author $Author: JReich $
 * version $Revision: 8767 $
 * lastrevision $Date: 2014-05-21 17:41:33 +0200 (Wed, 21 May 2014) $
 * modifiedby $LastChangedBy: JReich $
 * lastmodified $LastChangedDate: 2014-05-21 17:41:33 +0200 (Wed, 21 May 2014) $
 */

package com.actility.m2m.servlet.song.message;

import java.io.IOException;
import java.io.PrintWriter;
import java.io.UnsupportedEncodingException;
import java.util.Locale;

import javax.servlet.ServletOutputStream;

import org.apache.log4j.Logger;

import com.actility.m2m.be.messaging.DeliveryChannel;
import com.actility.m2m.be.messaging.InOut;
import com.actility.m2m.be.messaging.MessagingException;
import com.actility.m2m.servlet.ApplicationSession;
import com.actility.m2m.servlet.log.BundleLogger;
import com.actility.m2m.servlet.song.SongServletMessage;
import com.actility.m2m.servlet.song.SongServletRequest;
import com.actility.m2m.servlet.song.SongServletResponse;
import com.actility.m2m.servlet.song.SongURI;
import com.actility.m2m.servlet.song.internal.BindingNode;
import com.actility.m2m.servlet.song.internal.EndpointNode;
import com.actility.m2m.util.Profiler;
import com.actility.m2m.util.log.OSGiLogger;

/**
 * LocalResponse which is a {@link SongServletResponse} initiated by a service servlet.
 */
public final class LocalResponse extends HC implements SongServletResponse {
    private static final Logger LOG = OSGiLogger.getLogger(LocalResponse.class, BundleLogger.getStaticLogger());

    private final OutgoingRequest request;
    private String reasonPhrase;
    private int statusCode;

    /**
     * Builds a LocalResponse which is a response initiated by a service servlet.
     *
     * @param node The node that creates the response
     * @param request The request corresponding to this response
     * @param statusCode The response status code
     * @param reasonPhrase The response reason phrase
     * @param internal An internal response to copy to build this response
     */
    public LocalResponse(EndpointNode node, OutgoingRequest request, int statusCode, String reasonPhrase,
            InternalMessage internal) {
        super(node, internal, request.getId());
        this.request = request;
        this.exchange = request.getExchange();
        this.statusCode = statusCode;
        this.reasonPhrase = reasonPhrase;
        if (internal == null) {
            setHeaderInternal(SongServletMessage.HD_SERVER, node.getServletContext().getServerInfo());
        }
        checkProtocolSession();
    }

    public BindingNode getBindingNode() {
        return request.getBindingNode();
    }

    public ApplicationSession getApplicationSession(boolean create) {
        return request.getApplicationSession(create);
    }

    public SongServletRequest getRequest() {
        return request;
    }

    public int getStatus() {
        return statusCode;
    }

    public String getReasonPhrase() {
        return reasonPhrase;
    }

    public String getMethod() {
        return request.getMethod();
    }

    public String getProtocol() {
        return request.getProtocol();
    }

    public String getTransport() {
        return request.getTransport();
    }

    public boolean isSecure() {
        return request.isSecure();
    }

    public SongURI getRequestingEntity() {
        return request.getRequestingEntity();
    }

    public SongURI getTargetID() {
        return request.getTargetID();
    }

    public void setStatus(int statusCode) {
        checkCommitted();
        this.statusCode = statusCode;
        this.reasonPhrase = getReasonPhrase(statusCode);
    }

    public void setCharacterEncoding(String enc) {
        try {
            super.setCharacterEncoding(enc);
        } catch (UnsupportedEncodingException e) {
            LOG.error("Set unknown encoding " + enc + " keep the previously defined one " + characterEncoding, e);
        }
    }

    public void send() throws IOException {
        checkCommitted();
        committed = true;
        if (Profiler.getInstance().isTraceEnabled()) {
            Profiler.getInstance().trace(getId() + ": <<< " + node.getTarget() + ": " + statusCode + " " + reasonPhrase);
        }
        if (LOG.isInfoEnabled()) {
            LOG.info(getId() + ": <<< " + node.getTarget() + ": " + statusCode + " " + reasonPhrase);
            LOG.info(getId() + ": " + InternalMessage.getResponseAsString(this));
        }
        try {
            request.setResponded(true);
            InOut realExchange = node.buildResponseExchange(exchange, this);
            node.getContext().getDeliveryChannel().send(realExchange, DeliveryChannel.IMPORTANT);
        } catch (MessagingException e) {
            throw new IOException(e.getMessage());
        }
    }

    public void flushBuffer() {
        throw new IllegalStateException(UNSUPPORTED_METHOD);
    }

    public int getBufferSize() {
        throw new IllegalStateException(UNSUPPORTED_METHOD);
    }

    public Locale getLocale() {
        throw new IllegalStateException(UNSUPPORTED_METHOD);
    }

    public ServletOutputStream getOutputStream() {
        throw new IllegalStateException(UNSUPPORTED_METHOD);
    }

    public PrintWriter getWriter() {
        throw new IllegalStateException(UNSUPPORTED_METHOD);
    }

    public void reset() {
        throw new IllegalStateException(UNSUPPORTED_METHOD);
    }

    public void resetBuffer() {
        throw new IllegalStateException(UNSUPPORTED_METHOD);
    }

    public void setBufferSize(int size) {
        throw new IllegalStateException(UNSUPPORTED_METHOD);
    }

    public void setLocale(Locale loc) {
        throw new IllegalStateException(UNSUPPORTED_METHOD);
    }

    public String getLocalAddr() {
        throw new IllegalStateException(UNSUPPORTED_METHOD);
    }

    public int getLocalPort() {
        throw new IllegalStateException(UNSUPPORTED_METHOD);
    }

    public String getRemoteAddr() {
        throw new IllegalStateException(UNSUPPORTED_METHOD);
    }

    public int getRemotePort() {
        throw new IllegalStateException(UNSUPPORTED_METHOD);
    }
}
