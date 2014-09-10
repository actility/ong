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
 * id $Id: SclLogger.java 3302 2012-10-10 14:19:16Z JReich $
 * author $Author: JReich $
 * version $Revision: 3302 $
 * lastrevision $Date: 2012-10-10 16:19:16 +0200 (Wed, 10 Oct 2012) $
 * modifiedby $LastChangedBy: JReich $
 * lastmodified $LastChangedDate: 2012-10-10 16:19:16 +0200 (Wed, 10 Oct 2012) $
 */

package com.actility.m2m.scl.model;

import java.io.IOException;
import java.io.UnsupportedEncodingException;

import org.apache.log4j.Logger;

import com.actility.m2m.m2m.Indication;
import com.actility.m2m.m2m.M2MConstants;
import com.actility.m2m.m2m.M2MException;
import com.actility.m2m.m2m.Response;
import com.actility.m2m.m2m.StatusCode;
import com.actility.m2m.scl.log.BundleLogger;
import com.actility.m2m.util.log.OSGiLogger;
import com.actility.m2m.xo.XoException;
import com.actility.m2m.xo.XoObject;

public final class SclLogger {
    private static final Logger LOG = OSGiLogger.getLogger(SclLogger.class, BundleLogger.getStaticLogger());

    private SclLogger() {
        // Static class
    }

    public static void logRequestIndication(String requestIndication, String responseConfirm, Indication indication,
            String parentTag, int flags) throws IOException, M2MException, XoException {
        if (LOG.isInfoEnabled()) {
            indication.setAttribute(Constants.AT_PT_RESPONSE, responseConfirm);
            StringBuffer buffer = new StringBuffer();
            buffer.append(indication.getTransactionId());
            buffer.append(" >>> ");
            buffer.append(requestIndication);
            buffer.append(System.getProperty("line.separator"));
            buffer.append("  Primitive Attribute");
            buffer.append(System.getProperty("line.separator"));
            buffer.append("    requestingEntity: ");
            buffer.append(indication.getRequestingEntity());
            buffer.append(System.getProperty("line.separator"));
            buffer.append("    targetID: ");
            buffer.append(indication.getTargetID());
            buffer.append(System.getProperty("line.separator"));
            if ((flags & Constants.ID_LOG_REPRESENTATION) != 0) {
                buffer.append("  Resource");
                buffer.append(System.getProperty("line.separator"));
                XoObject representation = indication.getRepresentation(parentTag);
                if (representation == null) {
                    buffer.append("<< no representation >>");
                } else {
                    buffer.append(new String(representation.saveXml(), M2MConstants.ENC_UTF8));
                }
                buffer.append(System.getProperty("line.separator"));
            }
            if ((flags & Constants.ID_LOG_RAW_REPRESENTATION) != 0) {
                buffer.append("  Raw Resource");
                buffer.append(System.getProperty("line.separator"));

                byte[] rawContent = indication.getRawBytes();
                if (rawContent != null) {
                    String contentType = indication.getRawContentType();
                    if (contentType != null
                            && (contentType.startsWith(M2MConstants.MT_APPLICATION_XML) || contentType
                                    .startsWith(M2MConstants.MT_TEXT))) {
                        try {
                            buffer.append(new String(indication.getRawBytes(), indication.getRawCharacterEncoding()));
                        } catch (IOException e) {
                            buffer.append("<< representation cannot be decoded >>");
                        }
                    } else {
                        buffer.append("<< binary representation >>");
                    }
                } else {
                    buffer.append("<< no representation >>");
                }
                buffer.append(System.getProperty("line.separator"));
            }
            LOG.info(buffer.toString());
        }
    }

    public static void logResponseConfirm(Response response, StatusCode statusCode, XoObject representation, String childTag)
            throws UnsupportedEncodingException, XoException {
        if (LOG.isInfoEnabled()) {
            Indication indication = response.getIndication();
            String responseConfirm = (String) indication.getAttribute(Constants.AT_PT_RESPONSE);
            if (responseConfirm != null) {
                StringBuffer buffer = new StringBuffer();
                buffer.append(indication.getTransactionId());
                buffer.append(" <<< ");
                buffer.append(responseConfirm);
                buffer.append(System.getProperty("line.separator"));
                buffer.append("  Primitive Attribute");
                buffer.append(System.getProperty("line.separator"));
                buffer.append("    statusCode: ");
                buffer.append(statusCode.name());
                buffer.append(System.getProperty("line.separator"));
                if (representation != null) {
                    if (M2MConstants.TAG_M2M_ERROR_INFO.equals(representation.getName())) {
                        buffer.append("    errorInfo: ");
                        buffer.append(new String(representation.saveXml(), M2MConstants.ENC_UTF8));
                        buffer.append(System.getProperty("line.separator"));
                    } else {
                        buffer.append("  Resource");
                        buffer.append(System.getProperty("line.separator"));
                        buffer.append(new String(representation.saveXml(), M2MConstants.ENC_UTF8));
                        buffer.append(System.getProperty("line.separator"));
                    }
                }
                LOG.info(buffer.toString());
            }
        }
    }

    public static void logResponseConfirm(Response response, StatusCode statusCode, byte[] content, String contentType)
            throws UnsupportedEncodingException {
        if (LOG.isInfoEnabled()) {
            Indication indication = response.getIndication();
            String responseConfirm = (String) indication.getAttribute(Constants.AT_PT_RESPONSE);
            if (responseConfirm != null) {
                StringBuffer buffer = new StringBuffer();
                buffer.append(indication.getTransactionId());
                buffer.append(" <<< ");
                buffer.append(responseConfirm);
                buffer.append(System.getProperty("line.separator"));
                buffer.append("  Primitive Attribute");
                buffer.append(System.getProperty("line.separator"));
                buffer.append("    statusCode: ");
                buffer.append(statusCode.name());
                buffer.append(System.getProperty("line.separator"));
                if (content != null && content.length > 0) {
                    buffer.append("  Raw Resource");
                    buffer.append(System.getProperty("line.separator"));

                    if (contentType != null
                            && (contentType.startsWith(M2MConstants.MT_APPLICATION_XML) || contentType
                                    .startsWith(M2MConstants.MT_TEXT))) {
                        buffer.append(new String(content, M2MConstants.ENC_UTF8));
                    } else {
                        buffer.append("<< binary representation >>");
                    }
                    buffer.append(System.getProperty("line.separator"));
                }
                LOG.info(buffer.toString());
            }
        }
    }
}
