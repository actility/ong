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
 * id $Id: SubscribedResource.java 3302 2012-10-10 14:19:16Z JReich $
 * author $Author: JReich $
 * version $Revision: 3302 $
 * lastrevision $Date: 2012-10-10 16:19:16 +0200 (Wed, 10 Oct 2012) $
 * modifiedby $LastChangedBy: JReich $
 * lastmodified $LastChangedDate: 2012-10-10 16:19:16 +0200 (Wed, 10 Oct 2012) $
 */

package com.actility.m2m.scl.model;

import java.io.UnsupportedEncodingException;
import java.net.URI;
import java.util.Map;
import java.util.Set;

import com.actility.m2m.m2m.FilterCriteria;
import com.actility.m2m.m2m.M2MException;
import com.actility.m2m.storage.StorageException;
import com.actility.m2m.xo.XoException;
import com.actility.m2m.xo.XoObject;

public interface SubscribedResource {
    boolean dependsOnFilterCriteria();

    FilterCriteria buildFilterCriteria();

    boolean filterMatches(XoObject resource, FilterCriteria filterCriteria);

    FilterCriteria mergeFilterCriteria(FilterCriteria mergedFilterCriteria, FilterCriteria filterCriteria);

    void prepareResourceForResponse(String logId, SclManager manager, String path, XoObject resource, URI requestingEntity,
            FilterCriteria filterCriteria, Set supported) throws UnsupportedEncodingException, StorageException, XoException,
            M2MException;

    void prepareResponse(Map context, SclManager manager, String path, XoObject resource, FilterCriteria filterCriteria)
            throws UnsupportedEncodingException, StorageException, XoException, M2MException;

    void restoreResponse(Map context, XoObject resource);

    byte[] getResponseRepresentation(String logId, SclManager manager, String path, URI requestingEntity,
            FilterCriteria filterCriteria, Set supported, String mediaType) throws UnsupportedEncodingException,
            StorageException, XoException, M2MException;

}
