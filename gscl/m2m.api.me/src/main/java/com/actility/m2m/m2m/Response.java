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

import com.actility.m2m.xo.XoException;
import com.actility.m2m.xo.XoObject;

public interface Response extends Serializable {
    String getTransactionId();

    boolean isCommitted();

    Indication getIndication();

    void setResouceURI(URI resourceURI);

    void setRepresentation(XoObject representation) throws XoException, IOException, M2MException;

    void setRepresentationWithDefaultMediaType(XoObject representation) throws XoException, IOException;

    void setRawContent(byte[] content, String contentType) throws IOException;

    void setCacheExpiration(long cacheExpiration);

    void setLastModified(long lastModified);

    void setETag(String eTag);

    void setAllow(boolean createAllowed, boolean retrieveAllowed, boolean updateAllowed, boolean deleteAllowed);

    void addHeader(String name, String value);

    void setHeader(String name, String value);

    void send() throws IOException;
}
