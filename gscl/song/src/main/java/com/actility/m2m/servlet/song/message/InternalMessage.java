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
 * id $Id: InternalMessage.java 8767 2014-05-21 15:41:33Z JReich $
 * author $Author: JReich $
 * version $Revision: 8767 $
 * lastrevision $Date: 2014-05-21 17:41:33 +0200 (Wed, 21 May 2014) $
 * modifiedby $LastChangedBy: JReich $
 * lastmodified $LastChangedDate: 2014-05-21 17:41:33 +0200 (Wed, 21 May 2014) $
 */

package com.actility.m2m.servlet.song.message;

import java.io.IOException;
import java.io.UnsupportedEncodingException;
import java.text.ParseException;
import java.util.Enumeration;
import java.util.HashMap;
import java.util.HashSet;
import java.util.Iterator;
import java.util.Map;
import java.util.Set;

import javax.servlet.RequestDispatcher;
import javax.servlet.ServletContext;
import javax.servlet.ServletRequestAttributeEvent;
import javax.servlet.ServletRequestAttributeListener;

import org.apache.log4j.Logger;

import com.actility.m2m.be.messaging.InOut;
import com.actility.m2m.m2m.M2MConstants;
import com.actility.m2m.m2m.M2MException;
import com.actility.m2m.m2m.StatusCode;
import com.actility.m2m.servlet.ApplicationSession;
import com.actility.m2m.servlet.ext.ExtApplicationSession;
import com.actility.m2m.servlet.log.BundleLogger;
import com.actility.m2m.servlet.song.Parameterable;
import com.actility.m2m.servlet.song.ServletParseException;
import com.actility.m2m.servlet.song.SongServletMessage;
import com.actility.m2m.servlet.song.SongServletRequest;
import com.actility.m2m.servlet.song.SongServletResponse;
import com.actility.m2m.servlet.song.internal.BindingNode;
import com.actility.m2m.servlet.song.internal.EndpointNode;
import com.actility.m2m.util.ArrayUtils;
import com.actility.m2m.util.CharacterUtils;
import com.actility.m2m.util.EmptyUtils;
import com.actility.m2m.util.IteratorEnum;
import com.actility.m2m.util.MultiMap;
import com.actility.m2m.util.StringReader;
import com.actility.m2m.util.log.OSGiLogger;
import com.actility.m2m.xo.XoException;
import com.actility.m2m.xo.XoObject;
import com.actility.m2m.xo.XoService;

/**
 * Abstract class to represent a {@link SongServletMessage}.
 * <p>
 * Several patterns exists in the servlet communication:
 * <p>
 * <b>Service Servlet => Binding Servlet</b><br/>
 *
 * <pre>
 * LocalRequest             => LocalWrappedRequest<br/>RemoteWrappedResponse    <= RemoteResponse
 * </pre>
 *
 * <pre>
 * LocalForwardedRequest    => LocalWrappedRequest<br/>RemoteWrappedResponse    <= RemoteResponse
 * </pre>
 * <p>
 * <b>Binding Servlet => Service Servlet</b><br/>
 *
 * <pre>
 * RemoteRequest            => RemoteWrappedRequest<br/>LocalWrappedResponse     <= LocalResponse
 * </pre>
 *
 * <pre>
 * RemoteRequest            => RemoteWrappedRequest<br/>LocalWrappedResponse     <= LocalForwardedResponse
 * </pre>
 * <p>
 * <b>Service Servlet => Service Servlet</b><br/>
 *
 * <pre>
 * LocalRequest             => RemoteForwardedRequest<br/>RemoteForwardedResponse  <= LocalResponse
 * </pre>
 *
 * <pre>
 * LocalForwardedRequest    => RemoteForwardedRequest<br/>RemoteForwardedResponse  <= LocalResponse
 * </pre>
 *
 * <pre>
 * LocalRequest             => RemoteForwardedRequest<br/>RemoteForwardedResponse  <= LocalForwardedResponse
 * </pre>
 *
 * <pre>
 * LocalForwardedRequest    => RemoteForwardedRequest<br/>RemoteForwardedResponse  <= LocalForwardedResponse
 * </pre>
 *
 */
public abstract class InternalMessage implements SongServletMessage {
    private static final Logger LOG = OSGiLogger.getLogger(InternalMessage.class, BundleLogger.getStaticLogger());

    protected static final long REQUEST_TIMEOUT_DURATION = 32000;
    protected static final String UNSUPPORTED_METHOD = "Unsupported method for the given message";

    private static final long L_WHITESPACES = CharacterUtils.lowMask(" \t");
    private static final long H_WHITESPACES = CharacterUtils.highMask(" \t");

    private static final long L_EQUAL_SEMICOLON_WHITESPACES = CharacterUtils.lowMask(" \t=;");
    private static final long H_EQUAL_SEMICOLON_WHITESPACES = CharacterUtils.highMask(" \t=;");

    private static final long L_SEMICOLON_WHITESPACES = CharacterUtils.lowMask(" \t;");
    private static final long H_SEMICOLON_WHITESPACES = CharacterUtils.highMask(" \t;");

    private static final Set SYSTEM_HEADERS = new HashSet();
    private static final Set PARAMETERABLE_HEADERS = new HashSet();
    private static final Map STATUS_CODE_TO_REASON_PHRASE = new HashMap();

    {
        SYSTEM_HEADERS.add(SongServletMessage.HD_CONTENT_TYPE);
        PARAMETERABLE_HEADERS.add(SongServletMessage.HD_CONTENT_TYPE);

        STATUS_CODE_TO_REASON_PHRASE.put(new Integer(SongServletResponse.SC_OK), "OK");
        STATUS_CODE_TO_REASON_PHRASE.put(new Integer(SongServletResponse.SC_CREATED), "Created");
        STATUS_CODE_TO_REASON_PHRASE.put(new Integer(SongServletResponse.SC_ACCEPTED), "Accepted");
        STATUS_CODE_TO_REASON_PHRASE.put(new Integer(SongServletResponse.SC_NO_CONTENT), "No Content");
        STATUS_CODE_TO_REASON_PHRASE.put(new Integer(SongServletResponse.SC_NOT_MODIFIED), "Not Modified");
        STATUS_CODE_TO_REASON_PHRASE.put(new Integer(SongServletResponse.SC_BAD_REQUEST), "Bad Request");
        STATUS_CODE_TO_REASON_PHRASE.put(new Integer(SongServletResponse.SC_PERMISSION_DENIED), "Permission Denied");
        STATUS_CODE_TO_REASON_PHRASE.put(new Integer(SongServletResponse.SC_FORBIDDEN), "Forbidden");
        STATUS_CODE_TO_REASON_PHRASE.put(new Integer(SongServletResponse.SC_NOT_FOUND), "Not Found");
        STATUS_CODE_TO_REASON_PHRASE.put(new Integer(SongServletResponse.SC_METHOD_NOT_ALLOWED), "Method Not Allowed");
        STATUS_CODE_TO_REASON_PHRASE.put(new Integer(SongServletResponse.SC_NOT_ACCEPTABLE), "Not Acceptable");
        STATUS_CODE_TO_REASON_PHRASE.put(new Integer(SongServletResponse.SC_REQUEST_TIMEOUT), "Request Timeout");
        STATUS_CODE_TO_REASON_PHRASE.put(new Integer(SongServletResponse.SC_CONFLICT), "Conflict");
        STATUS_CODE_TO_REASON_PHRASE.put(new Integer(SongServletResponse.SC_DELETED), "Deleted");
        STATUS_CODE_TO_REASON_PHRASE.put(new Integer(SongServletResponse.SC_PRECONDITION_FAILED), "Precondition Failed");
        STATUS_CODE_TO_REASON_PHRASE.put(new Integer(SongServletResponse.SC_UNSUPPORTED_MEDIA_TYPE), "Unsupported Media Type");
        STATUS_CODE_TO_REASON_PHRASE.put(new Integer(SongServletResponse.SC_INTERNAL_SERVER_ERROR), "Internal Server Error");
        STATUS_CODE_TO_REASON_PHRASE.put(new Integer(SongServletResponse.SC_NOT_IMPLEMENTED), "Not Implemented");
        STATUS_CODE_TO_REASON_PHRASE.put(new Integer(SongServletResponse.SC_BAD_GATEWAY), "Bad Gateway");
        STATUS_CODE_TO_REASON_PHRASE.put(new Integer(SongServletResponse.SC_SERVICE_UNAVAILABLE), "Service Unavailable");
        STATUS_CODE_TO_REASON_PHRASE.put(new Integer(SongServletResponse.SC_GATEWAY_TIMEOUT), "Gateway Timeout");
    }

    protected final EndpointNode node;
    protected InOut exchange;
    protected boolean committed;
    protected Map attributes;

    private static final void dumpMessage(StringBuffer buffer, SongServletMessage message) {
        Iterator headerNames = message.getHeaderNames();
        String headerName = null;
        Iterator headers = null;
        String header = null;
        while (headerNames.hasNext()) {
            headerName = (String) headerNames.next();
            headers = message.getHeaders(headerName);
            while (headers.hasNext()) {
                header = (String) headers.next();
                buffer.append(headerName);
                buffer.append(": ");
                buffer.append(header);
                buffer.append(System.getProperty("line.separator"));
            }
        }
        buffer.append(System.getProperty("line.separator"));

        if (message.getContentLength() > 0) {
            try {
                Object content = message.getContent();
                if (String.class.equals(content.getClass())) {
                    String stringContent = (String) content;
                    buffer.append(stringContent);
                } else {
                    buffer.append("<< binary content >>");
                }
            } catch (UnsupportedEncodingException e) {
                buffer.append("<< unsupported encoding >>");
            } catch (IOException e) {
                buffer.append("<< cannot read content >>");
            }
        }
    }

    public static final String getRequestAsString(SongServletRequest request) {
        StringBuffer buffer = new StringBuffer();
        buffer.append(request.getMethod()).append(' ').append(request.getTargetID().absoluteURI()).append(' ')
                .append(request.getProtocol());
        buffer.append(System.getProperty("line.separator"));
        buffer.append("Requesting-Entity: ").append(request.getRequestingEntity().absoluteURI())
                .append(System.getProperty("line.separator"));
        dumpMessage(buffer, request);
        return buffer.toString();
    }

    public static final String getResponseAsString(SongServletResponse response) {
        StringBuffer buffer = new StringBuffer();
        buffer.append(response.getProtocol()).append(' ').append(response.getStatus()).append(' ')
                .append(response.getReasonPhrase()).append(System.getProperty("line.separator"));
        dumpMessage(buffer, response);
        return buffer.toString();
    }

    public static final boolean isParameterableHeader(String name) {
        return PARAMETERABLE_HEADERS.contains(name);
    }

    public static final boolean isSystemHeader(String name) {
        return SYSTEM_HEADERS.contains(name);
    }

    public static final boolean isValidStatusCode(int statusCode) {
        return STATUS_CODE_TO_REASON_PHRASE.containsKey(new Integer(statusCode));
    }

    public static final String getReasonPhrase(int statusCode) {
        return (String) STATUS_CODE_TO_REASON_PHRASE.get(new Integer(statusCode));
    }

    public static final void checkEncoding(String encoding) throws UnsupportedEncodingException {
        "".getBytes(encoding);
    }

    protected InternalMessage(EndpointNode node) {
        this.node = node;
        this.committed = false;
    }

    private void readParams(StringReader reader, Map paramMap) throws ParseException, ServletParseException {
        char current = reader.readCurrent();
        String key = null;
        String value = null;
        while (current == ';') {
            key = null;
            value = null;
            reader.skipOffset(1);
            current = reader.skip(L_WHITESPACES, H_WHITESPACES);
            if (current == 0) {
                throw new ServletParseException("Header with an unterminated parameter");
            }
            key = reader.readUntil(L_EQUAL_SEMICOLON_WHITESPACES, H_EQUAL_SEMICOLON_WHITESPACES);
            current = reader.skip(L_WHITESPACES, H_WHITESPACES);
            if (current == '=') {
                // =
                reader.skipOffset(1);
                current = reader.skip(L_WHITESPACES, H_WHITESPACES);
                if (current == 0) {
                    throw new ServletParseException("Header with an unterminated parameter");
                }
                value = reader.readUntil(L_SEMICOLON_WHITESPACES, H_SEMICOLON_WHITESPACES);
                current = reader.skip(L_WHITESPACES, H_WHITESPACES);
            }
            if (key.length() == 0) {
                throw new ServletParseException("Parameter key is empty");
            }
            if (value != null && value.length() == 0) {
                throw new ServletParseException("Parameter value is empty");
            }
            paramMap.put(key, (value != null) ? value : "");
        }
    }

    protected final Parameterable createParameterable(String name, String header) throws ServletParseException {
        Map paramMap = new HashMap();
        StringReader reader = new StringReader(header);
        String headerValue = null;

        try {
            char current = reader.skip(L_WHITESPACES, H_WHITESPACES);
            if (current == 0) {
                throw new ServletParseException("Empty header, cannot extract a parameterable");
            }
            if (current == '<') {
                headerValue = reader.readUntil('>');
                if (reader.readCurrent() == 0) {
                    throw new ServletParseException("Header field is not terminated with a >");
                }
                reader.skipOffset(1);
                reader.skip(L_WHITESPACES, H_WHITESPACES);
                if (!reader.isTerminated()) {
                    readParams(reader, paramMap);
                }
            } else {
                headerValue = reader.readUntil(L_SEMICOLON_WHITESPACES, H_SEMICOLON_WHITESPACES);
                reader.skip(L_WHITESPACES, H_WHITESPACES);
                if (!reader.isTerminated()) {
                    readParams(reader, paramMap);
                }
            }
        } catch (ParseException e) {
            throw new ServletParseException("Bad parameterable header", e);
        }
        return new ParameterableImpl(headerValue, paramMap, !isSystemHeader(name));
    }

    public abstract MultiMap getHeadersInternal();

    public abstract String getContentTypeInternal();

    public abstract Object getContentInternal();

    public abstract byte[] getRawContentInternal();

    /**
     * Gets the binding corresponding to the scheme used within the SONG communication.
     *
     * @return The binding corresponding to the scheme used within the SONG communication
     */
    public abstract BindingNode getBindingNode();

    public final EndpointNode getNode() {
        return node;
    }

    public final ServletContext getServletContext() {
        return node.getServletContext();
    }

    public final InOut getExchange() {
        return exchange;
    }

    public final RequestDispatcher getRequestDispatcher(String path) {
        return node.getRequestDispatcher(path);
    }

    public final void checkProtocolSession() {
        ExtApplicationSession appSession = (ExtApplicationSession) getApplicationSession(false);
        if (appSession != null) {
            appSession.protocolSessionReadyToInvalidate(null);
        }
    }

    protected final void checkCommitted() {
        if (committed) {
            throw new IllegalStateException("The message is committed, cannot modify its content");
        }
    }

    public final ApplicationSession getApplicationSession() {
        return getApplicationSession(true);
    }

    public final String getRemoteUser() {
        return null;
    }

    // PORTAGE java.security.Principal
    // public final Principal getUserPrincipal() {
    // return null;
    // }

    public final boolean isCommitted() {
        return committed;
    }

    public final boolean isUserInRole(String role) {
        return false;
    }

    /**
     * Sets the committed flag (for test use only).
     *
     * @param committed The committed flag to set
     */
    public final void setCommitted(boolean committed) {
        this.committed = committed;
    }

    public final Object getAttribute(String name) {
        return (attributes != null) ? attributes.get(name) : null;
    }

    public final Enumeration getAttributeNames() {
        if (attributes != null) {
            return new IteratorEnum(attributes.keySet().iterator());
        }
        return EmptyUtils.EMPTY_ENUMERATION;
    }

    public final void setAttribute(String name, Object o) {
        if (attributes == null) {
            attributes = new HashMap();
        }

        // Construct an event with the new value
        ServletRequestAttributeEvent event = null;

        Object previousValue = attributes.put(name, o);

        if (this instanceof SongServletRequest) {
            Iterator it = node.getServletContext().getServletRequestAttributeListeners();
            ServletRequestAttributeListener listener = null;
            while (it.hasNext()) {
                if (event == null) {
                    event = new ServletRequestAttributeEvent(node.getServletContext(), (SongServletRequest) this, name,
                            (previousValue != null) ? previousValue : o);
                }
                listener = (ServletRequestAttributeListener) it.next();
                if (previousValue == null) {
                    listener.attributeAdded(event);
                } else {
                    listener.attributeReplaced(event);
                }
            }
        }
    }

    public final void removeAttribute(String name) {
        ServletRequestAttributeEvent event = null;

        if (attributes != null) {
            final Object value = attributes.remove(name);

            if (this instanceof SongServletRequest) {
                final Iterator it = node.getServletContext().getServletRequestAttributeListeners();
                ServletRequestAttributeListener listener = null;
                while (it.hasNext()) {
                    if (event == null) {
                        event = new ServletRequestAttributeEvent(node.getServletContext(), (SongServletRequest) this, name,
                                value);
                    }
                    listener = (ServletRequestAttributeListener) it.next();
                    listener.attributeRemoved(event);
                }
            }
        }
    }

    public final void setErrorInfoBody(SongServletResponse response, StatusCode m2mStatus, String message) {
        XoObject errorInfo = null;
        try {
            byte[] errorInfoContent = null;
            String contentType = null;
            String mediaType = getAcceptedXoMediaType(getHeader(SongServletMessage.HD_ACCEPT));
            errorInfo = node.getContainer().getXoService().newXmlXoObject(M2MConstants.TAG_M2M_ERROR_INFO);
            errorInfo.setNameSpace(M2MConstants.PREFIX_M2M);
            errorInfo.setStringAttribute(M2MConstants.TAG_M2M_STATUS_CODE, m2mStatus.name());
            errorInfo.setStringAttribute(M2MConstants.TAG_M2M_ADDITIONAL_INFO, message);
            if (M2MConstants.MT_APPLICATION_EXI.equals(mediaType)) {
                errorInfoContent = errorInfo.saveExi();
                contentType = M2MConstants.MT_APPLICATION_EXI;
            } else {
                errorInfoContent = errorInfo.saveXml();
                contentType = M2MConstants.CT_APPLICATION_XML_UTF8;
            }
            response.setContent(errorInfoContent, contentType);
        } catch (IOException e) {
            // Ignore error
            LOG.error(e.getMessage(), e);
        } catch (M2MException e) {
            // Ignore error
            LOG.error(e.getMessage(), e);
        } catch (XoException e) {
            // Ignore error
            LOG.error(e.getMessage(), e);
        } finally {
            if (errorInfo != null) {
                errorInfo.free(true);
            }
        }
    }

    private static final long L_ACCEPT_STOP_1 = CharacterUtils.lowMask(" ,;");
    private static final long H_ACCEPT_STOP_1 = CharacterUtils.highMask(" ,;");

    private static final long L_ACCEPT_STOP_2 = CharacterUtils.lowMask(" =");
    private static final long H_ACCEPT_STOP_2 = CharacterUtils.highMask(" =");

    private static String[] availableXoMediaTypes;

    public static void setAvailableXoMediaTypes(XoService xoService) {
        String[] availableMediaTypes = null;
        if (xoService.isXmlSupported() && xoService.isExiSupported()) {
            availableMediaTypes = new String[] { M2MConstants.MT_APPLICATION_XML, M2MConstants.MT_APPLICATION_EXI };
        } else if (xoService.isXmlSupported()) {
            availableMediaTypes = new String[] { M2MConstants.MT_APPLICATION_XML };
        } else if (xoService.isExiSupported()) {
            availableMediaTypes = new String[] { M2MConstants.MT_APPLICATION_EXI };
        }
        InternalMessage.availableXoMediaTypes = availableMediaTypes;
    }

    public static String[] getAvailableXoMediaTypes() {
        return availableXoMediaTypes;
    }

    private Map extractAccept(String accept) throws M2MException {
        Map mediaTypeToWeight = new HashMap();
        try {
            StringReader reader = new StringReader(accept);
            char current = '0';
            boolean added = false;
            do {
                added = false;
                reader.skip(' ');
                String mediaType = reader.readUntil(L_ACCEPT_STOP_1, H_ACCEPT_STOP_1);
                if (!reader.isTerminated()) {
                    current = reader.readCurrent();
                    if (current == ' ') {
                        reader.skip(' ');
                    }
                    if (!reader.isTerminated()) {
                        current = reader.readCurrentAndSkip();
                        while (current == ';') {
                            reader.skip(' ');
                            String param = reader.readUntil(L_ACCEPT_STOP_2, H_ACCEPT_STOP_2);
                            reader.skip(' ');
                            reader.checkCurrentAndSkip('=');
                            reader.skip(' ');
                            String value = reader.readUntil(L_ACCEPT_STOP_1, H_ACCEPT_STOP_1);
                            reader.skip(' ');
                            if (!reader.isTerminated()) {
                                current = reader.readCurrentAndSkip();
                            } else {
                                current = '0';
                            }
                            if ("q".equals(param)) {
                                added = true;
                                mediaTypeToWeight.put(mediaType, Double.valueOf(value));
                            }
                        }
                        if ((reader.isTerminated() || current == ',') && !added) {
                            mediaTypeToWeight.put(mediaType, new Double(1));
                        }
                    } else {
                        mediaTypeToWeight.put(mediaType, new Double(1));
                    }
                } else {
                    mediaTypeToWeight.put(mediaType, new Double(1));
                }
            } while (current == ',');
        } catch (ParseException e) {
            throw new M2MException("Cannot parse accept header: " + accept, StatusCode.STATUS_BAD_REQUEST, e);
        }

        return mediaTypeToWeight;
    }

    private String getAcceptedXoMediaType(String accept) throws M2MException {
        return getAcceptedMediaType(accept, availableXoMediaTypes);
    }

    public String getAcceptedMediaType(String accept, String[] acceptedMediaTypes) throws M2MException {
        double bestWeight = -1;
        String mediaType = null;

        if (accept != null) {
            Map mediaTypeToWeight = extractAccept(accept);
            for (int i = 0; i < acceptedMediaTypes.length; ++i) {
                Double weight = (Double) mediaTypeToWeight.get(acceptedMediaTypes[i]);
                if (weight != null && weight.doubleValue() > bestWeight) {
                    bestWeight = weight.doubleValue();
                    mediaType = acceptedMediaTypes[i];
                } else {
                    int index = acceptedMediaTypes[i].indexOf('/');
                    if (index != -1) {
                        weight = (Double) mediaTypeToWeight.get(acceptedMediaTypes[i].substring(0, index + 1) + "*");
                        if (weight != null && weight.doubleValue() > bestWeight) {
                            bestWeight = weight.doubleValue();
                            mediaType = acceptedMediaTypes[i];
                        } else {
                            weight = (Double) mediaTypeToWeight.get("*/*");
                            if (weight != null && weight.doubleValue() > bestWeight) {
                                bestWeight = weight.doubleValue();
                                mediaType = acceptedMediaTypes[i];
                            }
                        }
                    } else {
                        weight = (Double) mediaTypeToWeight.get("*/*");
                        if (weight != null && weight.doubleValue() > bestWeight) {
                            bestWeight = weight.doubleValue();
                            mediaType = acceptedMediaTypes[i];
                        }
                    }
                }
            }
        } else {
            mediaType = acceptedMediaTypes[0];
        }
        if (mediaType == null) {
            throw new M2MException("Only following media types are supported: "
                    + ArrayUtils.toString(acceptedMediaTypes, "[ ", " ]", ", "), StatusCode.STATUS_NOT_ACCEPTABLE);
        }

        return mediaType;
    }
}
