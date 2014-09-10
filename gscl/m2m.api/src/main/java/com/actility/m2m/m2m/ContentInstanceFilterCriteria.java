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
 * id $Id: ContentInstanceFilterCriteria.java 6080 2013-10-15 13:25:46Z mlouiset $
 * author $Author: mlouiset $
 * version $Revision: 6080 $
 * lastrevision $Date: 2013-10-15 15:25:46 +0200 (Tue, 15 Oct 2013) $
 * modifiedby $LastChangedBy: mlouiset $
 * lastmodified $LastChangedDate: 2013-10-15 15:25:46 +0200 (Tue, 15 Oct 2013) $
 */

package com.actility.m2m.m2m;

public final class ContentInstanceFilterCriteria extends FilterCriteria {
    private Integer sizeFrom;
    private Integer sizeUntil;
    private String[] contentType;
    private Boolean metaDataOnly;

    public Integer getSizeFrom() {
        return sizeFrom;
    }

    public void setSizeFrom(Integer sizeFrom) {
        this.sizeFrom = sizeFrom;
    }

    public Integer getSizeUntil() {
        return sizeUntil;
    }

    public void setSizeUntil(Integer sizeUntil) {
        this.sizeUntil = sizeUntil;
    }

    public String[] getContentType() {
        return contentType;
    }

    public void setContentType(String[] contentType) {
        this.contentType = contentType;
    }

    public Boolean getMetaDataOnly() {
        return metaDataOnly;
    }

    public void setMetaDataOnly(Boolean metaDataOnly) {
        this.metaDataOnly = metaDataOnly;
    }
}
