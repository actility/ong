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
 * id $Id: LocalForwardedResponse.java 8766 2014-05-21 15:41:17Z JReich $
 * author $Author: JReich $
 * version $Revision: 8766 $
 * lastrevision $Date: 2014-05-21 17:41:17 +0200 (Wed, 21 May 2014) $
 * modifiedby $LastChangedBy: JReich $
 * lastmodified $LastChangedDate: 2014-05-21 17:41:17 +0200 (Wed, 21 May 2014) $
 */

package com.actility.m2m.servlet.song.message;

import java.io.IOException;
import java.io.PrintWriter;
import java.util.Locale;

import javax.servlet.ServletOutputStream;

import org.apache.log4j.Logger;

import com.actility.m2m.be.messaging.DeliveryChannel;
import com.actility.m2m.be.messaging.InOut;
import com.actility.m2m.be.messaging.MessagingException;
import com.actility.m2m.servlet.ApplicationSession;
import com.actility.m2m.servlet.log.BundleLogger;
import com.actility.m2m.servlet.song.SongServletRequest;
import com.actility.m2m.servlet.song.SongServletResponse;
import com.actility.m2m.servlet.song.internal.BindingNode;
import com.actility.m2m.servlet.song.internal.EndpointNode;
import com.actility.m2m.util.Profiler;
import com.actility.m2m.util.log.OSGiLogger;

/**
 * Internal message built from a RemoteWrappedResponse or RemoteForwardedResponse.
 * <p>
 * This message is never handled by any servlet
 */
public final class LocalForwardedResponse extends HCReadOnlyRef implements SongServletResponse {
    private static final Logger LOG = OSGiLogger.getLogger(LocalForwardedResponse.class, BundleLogger.getStaticLogger());

    private final SongServletRequest request;

    /**
     * Builds a LocalForwardedResponse from the given RemoteWrappedResponse or RemoteForwardedResponse.
     *
     * @param node The node that creates the response
     * @param request The request corresponding to this response
     * @param response The message that is forwarded
     *
     */
    public LocalForwardedResponse(EndpointNode node, InternalMessage request, InternalMessage response) {
        super(node, response);
        this.request = (SongServletRequest) request;
        this.exchange = request.getExchange();
        this.committed = true;
    }

    public BindingNode getBindingNode() {
        return message.getBindingNode();
    }

    public SongServletRequest getRequest() {
        return request;
    }

    public int getStatus() {
        return ((SongServletResponse) message).getStatus();
    }

    public String getReasonPhrase() {
        return ((SongServletResponse) message).getReasonPhrase();
    }

    public void send() throws IOException {
        if (Profiler.getInstance().isTraceEnabled()) {
            Profiler.getInstance().trace(getId() + ": <<< " + node.getTarget() + ": " + getStatus() + " " + getReasonPhrase());
        }
        if (LOG.isInfoEnabled()) {
            LOG.info(getId() + ": <<< " + node.getTarget() + ": " + getStatus() + " " + getReasonPhrase());
            LOG.info(getId() + ": " + InternalMessage.getResponseAsString(this));
        }
        try {
            InOut realExchange = node.buildResponseExchange(exchange, this);
            node.getContext().getDeliveryChannel().send(realExchange, DeliveryChannel.IMPORTANT);
        } catch (MessagingException e) {
            throw new IOException(e.getMessage());
        }
    }

    public ApplicationSession getApplicationSession(boolean create) {
        throw new IllegalStateException(UNSUPPORTED_METHOD);
    }

    public void setBufferSize(int size) {
        throw new IllegalStateException(UNSUPPORTED_METHOD);
    }

    public int getBufferSize() {
        throw new IllegalStateException(UNSUPPORTED_METHOD);
    }

    public void flushBuffer() {
        throw new IllegalStateException(UNSUPPORTED_METHOD);
    }

    public void resetBuffer() {
        throw new IllegalStateException(UNSUPPORTED_METHOD);
    }

    public void reset() {
        throw new IllegalStateException(UNSUPPORTED_METHOD);
    }

    public void setLocale(Locale loc) {
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

    public void setStatus(int statusCode) {
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

    public void setCharacterEncoding(String enc) {
        throw new IllegalStateException(UNSUPPORTED_METHOD);
    }
}
