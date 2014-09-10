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
 * id $Id: AmbiguousPathLayout.java 3302 2012-10-10 14:19:16Z JReich $
 * author $Author: JReich $
 * version $Revision: 3302 $
 * lastrevision $Date: 2012-10-10 16:19:16 +0200 (Wed, 10 Oct 2012) $
 * modifiedby $LastChangedBy: JReich $
 * lastmodified $LastChangedDate: 2012-10-10 16:19:16 +0200 (Wed, 10 Oct 2012) $
 */

package com.actility.m2m.scl.impl;

import java.util.Map;

import com.actility.m2m.scl.model.PathLayout;

public final class AmbiguousPathLayout implements PathLayout {
    private final PathLayout[] pathLayouts;
    private final Map tagToPathLayout;

    public AmbiguousPathLayout(PathLayout[] pathLayouts, Map tagToPathLayout) {
        this.pathLayouts = pathLayouts;
        this.tagToPathLayout = tagToPathLayout;
    }

    public boolean isImplemented() {
        return false;
    }

    public PathLayout getSubLayout(String name) {
        PathLayout result = null;
        for (int i = 0; i < pathLayouts.length && result == null; ++i) {
            result = pathLayouts[i].getSubLayout(name);
        }
        return result;
    }

    public int getAttributeId(String name) {
        int result = -1;
        for (int i = 0; i < pathLayouts.length && result < 0; ++i) {
            result = pathLayouts[i].getAttributeId(name);
        }
        return result;
    }

    public PathLayout getWildcardLayout() {
        PathLayout result = null;
        for (int i = 0; i < pathLayouts.length && result == null; ++i) {
            result = pathLayouts[i].getWildcardLayout();
        }
        return result;
    }

    public int getResourceId() {
        return 0;
    }

    public int getDefaultResourceId() {
        return pathLayouts[0].getDefaultResourceId();
    }

    public int getResourceIdFromTag(String tag, String attribute) {
        PathLayout realPathLayout = (PathLayout) tagToPathLayout.get(tag);
        if (realPathLayout != null && realPathLayout.isImplemented()
                && (attribute == null || realPathLayout.getAttributeId(attribute) >= 0)) {
            return realPathLayout.getResourceId();
        }
        return -1;
    }

    public int getRetrieveMode() {
        return pathLayouts[0].getRetrieveMode();
    }
}
