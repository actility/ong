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
 */

package com.actility.m2m.m2m;

import java.io.IOException;
import java.io.Serializable;
import java.net.URI;
import java.util.Iterator;
import java.util.ListIterator;

import com.actility.m2m.xo.XoException;
import com.actility.m2m.xo.XoObject;

public interface Indication extends Serializable {
    int CONDITION_READ = 0;
    int CONDITION_WRITE = 1;

    String getAcceptedMediaType(String[] availableMediaTypes) throws M2MException;

    String getTransactionId();

    Object getAttribute(String name);

    void setAttribute(String name, Object value);

    void removeAttribute(String name);

    boolean isCommitted();

    String getMethod();

    String getTargetPath();

    URI getTargetID();

    URI getRequestingEntity();

    String getHeader(String name);

    Iterator getHeaderNames();

    ListIterator getHeaders(String name);

    String getLocalHost();

    String getLocalAddr();

    int getLocalPort();

    String getRemoteHost();

    String getRemoteAddr();

    int getRemotePort();

    FilterCriteria getFilterCriteria() throws M2MException;

    ContentInstanceFilterCriteria getContentInstanceFilterCriteria() throws M2MException;

    Long getMaxSize() throws M2MException;

    String getSearchPrefix();

    String getQueryParameter(String name);

    Iterator getQueryParameters(String name);

    XoObject getRepresentation(String parentTag) throws IOException, XoException, M2MException;

    String getRawCharacterEncoding();

    String getRawContentType();

    byte[] getRawBytes() throws IOException;

    // TODO should be decomposed into hasETag checkETag and hastLastModified and checkLastModified
    // in order to avoid parsing ETag and LastModified on resource each time a request is received
    boolean checkConditions(String eTag, long lastModified, int conditionAccess) throws IOException;

    Response createSuccessResponse(StatusCode statusCode);

    Response createUnsuccessResponse(StatusCode statusCode);

    Request createRequest(URI requestingEntity, URI targetID) throws IOException;

    Response createResponseFromConfirm(Confirm confirm) throws IOException;

    Response createResponseFromConfirm(Confirm confirm, int rawStatusCode) throws IOException;

    void free();
}
