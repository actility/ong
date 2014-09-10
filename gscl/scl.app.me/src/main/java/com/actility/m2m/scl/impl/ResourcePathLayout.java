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
 * id $Id: ResourcePathLayout.java 3302 2012-10-10 14:19:16Z JReich $
 * author $Author: JReich $
 * version $Revision: 3302 $
 * lastrevision $Date: 2012-10-10 16:19:16 +0200 (Wed, 10 Oct 2012) $
 * modifiedby $LastChangedBy: JReich $
 * lastmodified $LastChangedDate: 2012-10-10 16:19:16 +0200 (Wed, 10 Oct 2012) $
 */

package com.actility.m2m.scl.impl;

import java.util.HashMap;
import java.util.Map;

import com.actility.m2m.scl.model.PathLayout;

public final class ResourcePathLayout implements PathLayout {
    private final Map attributes;
    private final Map subLayouts;
    private final PathLayout wildcardLayout;
    private final boolean implemented;
    private final boolean extensible;
    private final int resourceId;
    private final int retrieveMode;

    public ResourcePathLayout(int resourceId, int retrieveMode, PathLayout wildcardLayout, boolean extensible,
            boolean implemented) {
        this.resourceId = resourceId;
        this.retrieveMode = retrieveMode;
        this.wildcardLayout = wildcardLayout;
        this.extensible = extensible;
        this.implemented = implemented;
        this.subLayouts = new HashMap();
        this.attributes = new HashMap();
    }

    public boolean isImplemented() {
        return implemented;
    }

    public PathLayout getSubLayout(String name) {
        return (PathLayout) subLayouts.get(name);
    }

    public int getAttributeId(String name) {
        int result = -1;
        Integer attributeId = (Integer) attributes.get(name);
        if (attributeId != null) {
            result = attributeId.intValue();
        } else if (extensible) {
            result = 0;
        }
        return result;
    }

    public PathLayout getWildcardLayout() {
        return wildcardLayout;
    }

    public void addSubResourceLayout(String name, PathLayout subLayout) {
        subLayouts.put(name, subLayout);
    }

    public void addAttribute(String name, int attributeId) {
        attributes.put(name, new Integer(attributeId));
    }

    public int getResourceId() {
        return resourceId;
    }

    public int getDefaultResourceId() {
        return resourceId;
    }

    public int getResourceIdFromTag(String tag, String attribute) {
        return -1;
    }

    public int getRetrieveMode() {
        return retrieveMode;
    }
}
