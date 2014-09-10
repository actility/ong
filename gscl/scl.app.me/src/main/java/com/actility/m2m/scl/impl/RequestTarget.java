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
 * id $Id: RequestTarget.java 3302 2012-10-10 14:19:16Z JReich $
 * author $Author: JReich $
 * version $Revision: 3302 $
 * lastrevision $Date: 2012-10-10 16:19:16 +0200 (Wed, 10 Oct 2012) $
 * modifiedby $LastChangedBy: JReich $
 * lastmodified $LastChangedDate: 2012-10-10 16:19:16 +0200 (Wed, 10 Oct 2012) $
 */

package com.actility.m2m.scl.impl;

import java.util.ArrayList;
import java.util.List;

import com.actility.m2m.scl.model.PartialAccessor;
import com.actility.m2m.scl.model.ResourceController;
import com.actility.m2m.xo.XoObject;

public final class RequestTarget implements PartialAccessor {

    private final ResourceController controller;
    private final String path;
    private final XoObject resource;
    private final int attributeId;
    private final String attribute;
    private final String rawSubPath;
    private List subPath;

    public RequestTarget(ResourceController controller, String path, XoObject resource, int attributeId, String attribute, String rawSubPath) {
        this.controller = controller;
        this.path = path;
        this.resource = resource;
        this.attributeId = attributeId;
        this.attribute = attribute;
        this.rawSubPath = (rawSubPath != null && rawSubPath.length() > 0) ? rawSubPath : null;
    }

    public ResourceController getController() {
        return controller;
    }

    public String getPath() {
        return path;
    }

    public XoObject getResource() {
        return resource;
    }

    public int getAttributeId() {
        return attributeId;
    }

    public String getAttribute() {
        return attribute;
    }

    public List getSubPath() {
        if (subPath == null && rawSubPath != null) {
            subPath = new ArrayList();
            int beginOffset = 0;
            int endOffset = 0;
            int rawSubPathLength = rawSubPath.length();
            while (beginOffset < rawSubPathLength) {
                endOffset = rawSubPath.indexOf('/', beginOffset);
                if (endOffset == -1) {
                    subPath.add(rawSubPath.substring(beginOffset));
                    beginOffset = rawSubPathLength;
                } else {
                    if (endOffset - beginOffset > 0) {
                        subPath.add(rawSubPath.substring(beginOffset, endOffset));
                    }
                    beginOffset = endOffset + 1;
                }
            }
        }
        return subPath;
    }

}
