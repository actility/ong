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

package com.actility.m2m.transport.logger.log;

import java.io.IOException;
import java.util.Date;
import java.util.Enumeration;
import java.util.Iterator;
import java.util.ListIterator;

import org.apache.log4j.Logger;

import com.actility.m2m.servlet.song.SongServletRequest;
import com.actility.m2m.servlet.song.SongServletResponse;
import com.actility.m2m.transport.logger.TransportLoggerService;
import com.actility.m2m.transport.logger.log.log.BundleLogger;
import com.actility.m2m.util.FormatUtils;
import com.actility.m2m.util.UtilConstants;
import com.actility.m2m.util.log.OSGiLogger;

public class TransportLoggerServiceImpl implements TransportLoggerService {
    private static final Logger LOG = OSGiLogger.getLogger(TransportLoggerServiceImpl.class, BundleLogger.getStaticLogger());

    public void logTransportTransaction(SongServletResponse response, long duration, long requestTime) {
        try {
            SongServletRequest request = response.getRequest();

            StringBuffer buffer = new StringBuffer();
            buffer.append("Request started at ").append(
                    FormatUtils.formatDateTime(new Date(requestTime), UtilConstants.LOCAL_TIMEZONE));
            buffer.append(" and has taken ").append(duration).append("ms to complete\n");
            buffer.append("Request:\n");
            buffer.append("  Attributes:\n");
            Enumeration names = request.getAttributeNames();
            while (names.hasMoreElements()) {
                String name = (String) names.nextElement();
                buffer.append("    ").append(name).append(": ").append(request.getAttribute(name)).append("\n");
            }
            buffer.append("  Method: ").append(request.getMethod()).append("\n");
            buffer.append("  Target-ID: ").append(request.getTargetID().absoluteURI()).append("\n");
            buffer.append("  Requesting-Entity: ").append(request.getRequestingEntity().absoluteURI()).append("\n");
            buffer.append("  Method: ").append(request.getMethod()).append("\n");
            buffer.append("  Headers:\n");
            Iterator headers = request.getHeaderNames();
            while (headers.hasNext()) {
                String header = (String) headers.next();
                ListIterator lit = request.getHeaders(header);
                while (lit.hasNext()) {
                    String value = (String) lit.next();
                    buffer.append("    ").append(header).append(": ").append(value).append("\n");
                }
            }
            if (request.getContentLength() > 0) {
                buffer.append("  Content:\n");
                if (request.getContentLength() < 4096) {
                    String contentType = request.getContentType();
                    if (contentType != null && (contentType.startsWith("text/") || contentType.startsWith("application/xml"))) {
                        buffer.append(new String(request.getRawContent(), request.getCharacterEncoding())).append("\n");
                    } else {
                        buffer.append("Base64: ").append(FormatUtils.formatBase64(request.getRawContent())).append("\n");
                    }
                } else {
                    buffer.append("<< too big >>\n");
                }
            }
            buffer.append("\n");
            buffer.append("Response:\n");
            buffer.append("  Attributes:\n");
            names = response.getAttributeNames();
            while (names.hasMoreElements()) {
                String name = (String) names.nextElement();
                buffer.append("    ").append(name).append(": ").append(response.getAttribute(name)).append("\n");
            }
            buffer.append("  Status-Code: ").append(response.getStatus()).append("\n");
            buffer.append("  Reason-Phrase: ").append(response.getReasonPhrase()).append("\n");
            buffer.append("  Headers:\n");
            headers = response.getHeaderNames();
            while (headers.hasNext()) {
                String header = (String) headers.next();
                ListIterator lit = response.getHeaders(header);
                while (lit.hasNext()) {
                    String value = (String) lit.next();
                    buffer.append("    ").append(header).append(": ").append(value).append("\n");
                }
            }
            if (response.getContentLength() > 0) {
                buffer.append("  Content:\n");
                if (response.getContentLength() < 4096) {
                    String contentType = response.getContentType();
                    if (contentType != null && (contentType.startsWith("text/") || contentType.startsWith("application/xml"))) {
                        buffer.append(new String(response.getRawContent(), response.getCharacterEncoding())).append("\n");
                    } else {
                        buffer.append("Base64: ").append(FormatUtils.formatBase64(response.getRawContent())).append("\n");
                    }
                } else {
                    buffer.append("<< too big >>\n");
                }
            }

            LOG.error(buffer.toString());
        } catch (IOException e) {
            LOG.error(e.getMessage(), e);
        }
    }
}
