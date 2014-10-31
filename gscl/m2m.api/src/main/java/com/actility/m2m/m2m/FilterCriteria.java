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
 * id $Id: FilterCriteria.java 9044 2014-07-03 15:50:18Z JReich $
 * author $Author: JReich $
 * version $Revision: 9044 $
 * lastrevision $Date: 2014-07-03 17:50:18 +0200 (Thu, 03 Jul 2014) $
 * modifiedby $LastChangedBy: JReich $
 * lastmodified $LastChangedDate: 2014-07-03 17:50:18 +0200 (Thu, 03 Jul 2014) $
 */

package com.actility.m2m.m2m;

import java.util.Date;

public class FilterCriteria {
    private Date ifModifiedSince;
    private Date ifUnmodifiedSince;
    private String[] ifNoneMatch;
    private String attributeAccessor;
    private String[] searchString;
    private Date createdAfter;
    private Date createdBefore;
    private long maxSize;
    private String searchPrefix;
    private String[] inType;
    private String[] outType;

    public Date getIfModifiedSince() {
        return ifModifiedSince;
    }

    public void setIfModifiedSince(Date ifModifiedSince) {
        this.ifModifiedSince = ifModifiedSince;
    }

    public Date getIfUnmodifiedSince() {
        return ifUnmodifiedSince;
    }

    public void setIfUnmodifiedSince(Date ifUnmodifiedSince) {
        this.ifUnmodifiedSince = ifUnmodifiedSince;
    }

    public String[] getIfNoneMatch() {
        return ifNoneMatch;
    }

    public void setIfNoneMatch(String[] ifNoneMatch) {
        this.ifNoneMatch = ifNoneMatch;
    }

    public String getAttributeAccessor() {
        return attributeAccessor;
    }

    public void setAttributeAccessor(String attributeAccessor) {
        this.attributeAccessor = attributeAccessor;
    }

    public String[] getSearchString() {
        return searchString;
    }

    public void setSearchString(String[] searchString) {
        this.searchString = searchString;
    }

    public Date getCreatedAfter() {
        return createdAfter;
    }

    public void setCreatedAfter(Date createdAfter) {
        this.createdAfter = createdAfter;
    }

    public Date getCreatedBefore() {
        return createdBefore;
    }

    public void setCreatedBefore(Date createdBefore) {
        this.createdBefore = createdBefore;
    }

    public long getMaxSize() {
        return maxSize;
    }

    public void setMaxSize(long maxSize) {
        this.maxSize = maxSize;
    }

    public String getSearchPrefix() {
        return searchPrefix;
    }

    public void setSearchPrefix(String searchPrefix) {
        this.searchPrefix = searchPrefix;
    }

    public String[] getInType() {
        return inType;
    }

    public void setInType(String[] inType) {
        this.inType = inType;
    }

    public String[] getOutType() {
        return outType;
    }

    public void setOutType(String[] outType) {
        this.outType = outType;
    }
}
