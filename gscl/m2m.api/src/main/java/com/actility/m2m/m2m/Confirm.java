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
 * id $Id: Confirm.java 7363 2014-01-24 11:30:58Z JReich $
 * author $Author: JReich $
 * version $Revision: 7363 $
 * lastrevision $Date: 2014-01-24 12:30:58 +0100 (Fri, 24 Jan 2014) $
 * modifiedby $LastChangedBy: JReich $
 * lastmodified $LastChangedDate: 2014-01-24 12:30:58 +0100 (Fri, 24 Jan 2014) $
 */

package com.actility.m2m.m2m;

import java.io.IOException;
import java.io.Serializable;
import java.net.URI;
import java.util.Iterator;
import java.util.ListIterator;

import com.actility.m2m.xo.XoException;
import com.actility.m2m.xo.XoObject;

public interface Confirm extends Serializable {
    String getTransactionId();

    boolean isCommitted();

    Request getRequest();

    StatusCode getStatusCode();

    String getHeader(String name);

    Iterator getHeaderNames();

    ListIterator getHeaders(String name);

    URI getResourceURI();

    XoObject getRepresentation() throws IOException, XoException, M2MException;

    int getRawStatusCode();

    String getRawContentType();

    byte[] getRawBytes() throws IOException;

    void free();
}
