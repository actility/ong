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
 * id $Id: SearchResult.java 8030 2014-03-07 17:30:45Z JReich $
 * author $Author: JReich $
 * version $Revision: 8030 $
 * lastrevision $Date: 2014-03-07 18:30:45 +0100 (Fri, 07 Mar 2014) $
 * modifiedby $LastChangedBy: JReich $
 * lastmodified $LastChangedDate: 2014-03-07 18:30:45 +0100 (Fri, 07 Mar 2014) $
 */

package com.actility.m2m.storage;

import java.util.Map;

public class SearchResult {
    private final boolean hasMore;
    // Map associating path (String )to raw document (byte array)
    private final Map results;

    public SearchResult(boolean hasMore, Map results) {
        this.hasMore = hasMore;
        this.results = results;
    }

    public boolean isHasMore() {
        return hasMore;
    }

    public Map getResults() {
        return results;
    }
}
