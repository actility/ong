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

package com.actility.m2m.xo.impl;

import com.actility.m2m.xo.XoException;
import com.actility.m2m.xo.XoObject;
import com.actility.m2m.xo.XoService;
import com.actility.m2m.xo.ni.NiXoService;

public final class XoServiceImpl implements XoService {

    private NiXoService xoNativeService;

    public XoServiceImpl(NiXoService xoNativeService) {
        this.xoNativeService = xoNativeService;
    }

    public boolean isBinarySupported() {
        return xoNativeService.isBinarySupported();
    }

    public XoObject readBinaryXmlXoObject(byte[] rawObject) throws XoException {
        long objectHandle = xoNativeService.xoLoadMem(rawObject);
        if (objectHandle != 0) {
            return new XoObjectImpl(xoNativeService, objectHandle, false);
        }
        return null;
    }

    public XoObject readBinaryObixXoObject(byte[] rawObject) throws XoException {
        long objectHandle = xoNativeService.xoLoadMem(rawObject);
        if (objectHandle != 0) {
            return new XoObjectImpl(xoNativeService, objectHandle, true);
        }
        return null;
    }

    public boolean isXmlSupported() {
        return xoNativeService.isXmlSupported();
    }

    public XoObject readXmlXmlXoObject(byte[] rawObject, String encoding) throws XoException {
        long objectHandle = xoNativeService.xoReadXmlMem(rawObject, false);
        if (objectHandle != 0) {
            return new XoObjectImpl(xoNativeService, objectHandle, false);
        }
        return null;
    }

    public XoObject readPartialXmlXmlXoObject(byte[] rawObject, String encoding, String rootType) throws XoException {
        long objectHanldle = xoNativeService.xoReadPartialXmlMem(rawObject, rootType);
        if (objectHanldle != 0) {
            return new XoObjectImpl(xoNativeService, objectHanldle, false);
        }
        return null;
    }

    public XoObject readXmlObixXoObject(byte[] rawObject, String encoding) throws XoException {
        long objectHandle = xoNativeService.xoReadXmlMem(rawObject, true);
        if (objectHandle != 0) {
            return new XoObjectImpl(xoNativeService, objectHandle, true);
        }
        return null;
    }

    public boolean isExiSupported() {
        return xoNativeService.isExiSupported();
    }

    public XoObject readExiXmlXoObject(byte[] rawObject, String encoding) throws XoException {
        long objectHandle = xoNativeService.xoReadExiMem(rawObject, false);
        if (objectHandle != 0) {
            return new XoObjectImpl(xoNativeService, objectHandle, false);
        }
        return null;
    }

    public XoObject readPartialExiXmlXoObject(byte[] rawObject, String encoding, String rootType) throws XoException {
        long objectHanldle = xoNativeService.xoReadPartialExiMem(rawObject, rootType);
        if (objectHanldle != 0) {
            return new XoObjectImpl(xoNativeService, objectHanldle, false);
        }
        return null;
    }

    public XoObject readExiObixXoObject(byte[] rawObject, String encoding) throws XoException {
        long objectHandle = xoNativeService.xoReadExiMem(rawObject, true);
        if (objectHandle != 0) {
            return new XoObjectImpl(xoNativeService, objectHandle, true);
        }
        return null;
    }

    public XoObject newXmlXoObject(String type) throws XoException {
        XoObject result = null;
        long value = xoNativeService.xoNmNew(type);
        if (value == 0) {
            throw new XoException("Unknown Xo type: " + type);
        }
        result = new XoObjectImpl(xoNativeService, value, false);
        return result;
    }

    public XoObject newObixXoObject(String type) throws XoException {
        XoObject result = null;
        long value = xoNativeService.xoNmNew(type);
        if (value == 0) {
            throw new XoException("Unknown Xo type: " + type);
        }
        result = new XoObjectImpl(xoNativeService, value, true);
        return result;
    }

    public void dumptStats() {
        xoNativeService.xoDumpNbOcc();
    }
}
