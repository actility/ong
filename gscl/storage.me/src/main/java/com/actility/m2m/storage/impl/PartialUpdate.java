/*******************************************************************************
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
 *******************************************************************************/
package com.actility.m2m.storage.impl;

import java.util.List;
import java.util.Map;

import com.actility.m2m.storage.Condition;
import com.actility.m2m.storage.Document;

public class PartialUpdate extends Modification {

    private final byte[] content;
    private final List attrOps;

    public PartialUpdate(Map configuration, Document document, byte[] content, List attrOps, Condition condition) {
        super(configuration, document, condition);
        this.content = content;
        this.attrOps = attrOps;
    }

    public byte[] getContent() {
        return content;
    }

    public List getAttrOps() {
        return attrOps;
    }
}
