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
 * id $Id: $
 * author $Author: $
 * version $Revision: $
 * lastrevision $Date: $
 * modifiedby $LastChangedBy: $
 * lastmodified $LastChangedDate: $
 */

package com.actility.m2m.storage.driver.sqlite.jni.impl;

import com.actility.m2m.storage.driver.sqlite.ni.NiAttribute;

public final class JNiAttributeImpl implements NiAttribute {

    private final InternalAttribute attribute;

    public JNiAttributeImpl(InternalAttribute attribute) {
        this.attribute = attribute;
    }

    protected InternalAttribute getInternalAttribute() {
        return attribute;
    }

    public void setName(String value) {
        attribute.setName(value);
    }

    public String getName() {
        return attribute.getName();
    }

    public void setStringValue(String value) {
        attribute.setString_value(value);
    }

    public String getStringValue() {
        return attribute.getString_value();
    }

    public void setIntValue(int value) {
        attribute.setInt_value(value);
    }

    public int getIntValue() {
        return attribute.getInt_value();
    }

    public void setDateValue(String value) {
        attribute.setDate_value(value);
    }

    public String getDateValue() {
        return attribute.getDate_value();
    }

    public void setType(int value) {
        attribute.setType(value);
    }

    public int getType() {
        return attribute.getType();
    }

}
