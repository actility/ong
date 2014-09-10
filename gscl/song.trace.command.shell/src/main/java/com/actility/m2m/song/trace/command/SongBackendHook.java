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
 * id $Id: SongBackendHook.java 6135 2013-10-17 07:52:20Z mlouiset $
 * author $Author: mlouiset $
 * version $Revision: 6135 $
 * lastrevision $Date: 2013-10-17 09:52:20 +0200 (Thu, 17 Oct 2013) $
 * modifiedby $LastChangedBy: mlouiset $
 * lastmodified $LastChangedDate: 2013-10-17 09:52:20 +0200 (Thu, 17 Oct 2013) $
 */

package com.actility.m2m.song.trace.command;

import java.io.BufferedWriter;
import java.io.FileWriter;
import java.io.IOException;
import java.text.DateFormat;
import java.text.SimpleDateFormat;
import java.util.Date;
import java.util.Iterator;

import com.actility.m2m.be.BackendEndpoint;
import com.actility.m2m.be.BackendException;
import com.actility.m2m.be.BackendHook;
import com.actility.m2m.be.BackendService;
import com.actility.m2m.be.messaging.BackendMessage;
import com.actility.m2m.be.messaging.InOut;
import com.actility.m2m.be.messaging.MessageExchange;
import com.actility.m2m.servlet.song.SongServletMessage;
import com.actility.m2m.servlet.song.SongServletRequest;
import com.actility.m2m.servlet.song.SongServletResponse;

public class SongBackendHook implements BackendHook {
    private static final DateFormat dateTimeFormat = new SimpleDateFormat("yyyy-MM-dd'T'HH:mm:ss");

    private BackendService backendService;
    private BufferedWriter out;

    public SongBackendHook(BackendService backendService, String file) throws IOException, BackendException {
        this.backendService = backendService;
        FileWriter fstream = new FileWriter(file);
        this.out = new BufferedWriter(fstream);
        Date now = new Date();
        out.write("<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n");
        out.write("<songTrace startDate=\"" + dateTimeFormat.format(now) + "\">\n");

        backendService.registerHook(this);
    }

    private void dumpMessage(SongServletMessage message) throws IOException {
        if (message.getRequestingEntity() != null) {
            out.write("requestingEntity=\"");
            out.write(message.getRequestingEntity().absoluteURI());
            out.write("\" ");
        }
        out.write("targetID=\"");
        if (message.getTargetID() != null) {
            out.write(message.getTargetID().absoluteURI());
        }
        out.write("\">\n");

        Iterator headerNames = message.getHeaderNames();
        while (headerNames.hasNext()) {
            String headerName = (String) headerNames.next();
            Iterator headers = message.getHeaders(headerName);
            while (headers.hasNext()) {
                String header = (String) headers.next();
                out.write("      <header name=\"");
                out.write(headerName);
                out.write("\" value=\"");
                out.write(header);
                out.write("\" />\n");
            }
        }

        if (message.getContentLength() > 0) {
            Object content = message.getContent();
            out.write("      <content length=\"");
            out.write(new Integer(message.getContentLength()).toString());
            if (String.class.equals(content.getClass())) {
                String stringContent = (String) content;
                out.write("\">\n");
                out.write("        <![CDATA[");
                out.write(stringContent);
                out.write("]]>\n");
                out.write("      </content>\n");
            } else {
                out.write("\" binary=\"true\" />\n");
            }
        }
    }

    private void dumpResponse(SongServletResponse response) throws IOException {
        out.write("    <songResponse statusCode=\"");
        out.write(new Integer(response.getStatus()).toString());
        out.write("\" ");
        dumpMessage(response);
        out.write("    </songResponse>\n");
    }

    private void dumpRequest(SongServletRequest request) throws IOException {
        out.write("    <songRequest protocol=\"");
        out.write(request.getProtocol());
        out.write("\" method=\"");
        out.write(request.getMethod());
        out.write("\" ");
        dumpMessage(request);
        out.write("    </songRequest>\n");
    }

    public synchronized void close() throws IOException {
        if (out != null) {
            backendService.unregisterHook();
            out.write("</songTrace>\n");
            out.close();
            out = null;
        }
    }

    public void finalize() throws IOException {
        if (out != null) {
            backendService.unregisterHook();
            out.write("</songTrace>\n");
            out.close();
            out = null;
        }
    }

    public synchronized void resolvedExchange(MessageExchange exchange, BackendEndpoint from, BackendEndpoint to) {
        try {
            if (out != null && "IN-OUT".equals(exchange.getPattern())) {
                InOut inOut = (InOut) exchange;
                BackendMessage inMessage = inOut.getInMessage();
                BackendMessage outMessage = inOut.getOutMessage();
                Date now = new Date();

                if (outMessage != null) {
                    // response
                    if (outMessage.getContent() instanceof SongServletResponse) {
                        out.write("  <resolvedExchange exchangeId=\"");
                        out.write(exchange.getExchangeId());
                        out.write("\" date=\"");
                        out.write(dateTimeFormat.format(now));
                        out.write("\" fromId=\"");
                        out.write(new Integer(from.getContext().getEndpointId()).toString());
                        out.write("\" fromName=\"");
                        out.write(from.toString());
                        out.write("\" toId=\"");
                        out.write(new Integer(to.getContext().getEndpointId()).toString());
                        out.write("\" toName=\"");
                        out.write(to.toString());
                        out.write("\">\n");
                        dumpResponse((SongServletResponse) outMessage.getContent());
                        out.write("  </resolvedExchange>\n");
                    }
                } else if (inMessage != null && inMessage.getContent() instanceof SongServletRequest) {
                    // request
                    out.write("  <resolvedExchange exchangeId=\"");
                    out.write(exchange.getExchangeId());
                    out.write("\" date=\"");
                    out.write(dateTimeFormat.format(now));
                    out.write("\" fromId=\"");
                    out.write(new Integer(from.getContext().getEndpointId()).toString());
                    out.write("\" fromName=\"");
                    out.write(from.toString());
                    out.write("\" toId=\"");
                    out.write(new Integer(to.getContext().getEndpointId()).toString());
                    out.write("\" toName=\"");
                    out.write(to.toString());
                    out.write("\">\n");
                    dumpRequest((SongServletRequest) inMessage.getContent());
                    out.write("  </resolvedExchange>\n");
                }
            }
        } catch (IOException e) {
            backendService.unregisterHook();
            out = null;
        }
    }

    public synchronized void unresolvedExchange(MessageExchange exchange, BackendEndpoint from) {
        try {
            if (out != null && "IN-OUT".equals(exchange.getPattern())) {
                InOut inOut = (InOut) exchange;
                BackendMessage inMessage = inOut.getInMessage();
                BackendMessage outMessage = inOut.getOutMessage();

                if (outMessage != null) {
                    // response
                    if (outMessage.getContent() instanceof SongServletResponse) {
                        out.write("  <unresolvedExchange exchangeId=\"");
                        out.write(exchange.getExchangeId());
                        out.write("\" fromId=\"");
                        out.write(new Integer(from.getContext().getEndpointId()).toString());
                        out.write("\" fromName=\"");
                        out.write(from.toString());
                        out.write("\">\n");
                        dumpResponse((SongServletResponse) outMessage.getContent());
                        out.write("  </unresolvedExchange>\n");
                    }
                } else if (inMessage != null && inMessage.getContent() instanceof SongServletRequest) {
                    // request
                    out.write("  <unresolvedExchange exchangeId=\"");
                    out.write(exchange.getExchangeId());
                    out.write("\" fromId=\"");
                    out.write(new Integer(from.getContext().getEndpointId()).toString());
                    out.write("\" fromName=\"");
                    out.write(from.toString());
                    out.write("\">\n");
                    dumpRequest((SongServletRequest) inMessage.getContent());
                    out.write("  </unresolvedExchange>\n");
                }
            }
        } catch (IOException e) {
            backendService.unregisterHook();
            out = null;
        }
    }

}
