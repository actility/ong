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
 * id $Id: FilterCriteria.java 6080 2013-10-15 13:25:46Z mlouiset $
 * author $Author: mlouiset $
 * version $Revision: 6080 $
 * lastrevision $Date: 2013-10-15 15:25:46 +0200 (Tue, 15 Oct 2013) $
 * modifiedby $LastChangedBy: mlouiset $
 * lastmodified $LastChangedDate: 2013-10-15 15:25:46 +0200 (Tue, 15 Oct 2013) $
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
}
