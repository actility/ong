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
 * id $Id: XoObjectImpl.java 7355 2014-01-23 22:42:06Z JReich $
 * author $Author: JReich $
 * version $Revision: 7355 $
 * lastrevision $Date: 2014-01-23 23:42:06 +0100 (Thu, 23 Jan 2014) $
 * modifiedby $LastChangedBy: JReich $
 * lastmodified $LastChangedDate: 2014-01-23 23:42:06 +0100 (Thu, 23 Jan 2014) $
 */

package com.actility.m2m.xo.impl;

import java.util.List;

import com.actility.m2m.xo.XoException;
import com.actility.m2m.xo.XoObject;
import com.actility.m2m.xo.ni.NiXoService;

public final class XoObjectImpl implements XoObject {
    private final boolean obix;
    private final long handle;
    private final NiXoService xoNativeService;

    public XoObjectImpl(NiXoService xoNativeService, long handle, boolean obix) {
        this.xoNativeService = xoNativeService;
        this.handle = handle;
        this.obix = obix;
    }

    public long getHandle() {
        return handle;
    }

    public String getType() {
        return xoNativeService.xoNmType(handle);
    }

    public String getName() {
        return xoNativeService.xoStringNmGetAttr(handle, "name$");
    }

    public void setName(String name) {
        if (name == null) {
            xoNativeService.xoNmUnSetAttr(handle, "name$");
        } else {
            xoNativeService.xoStringNmSetAttr(handle, "name$", name);
        }
    }

    public void setNameSpace(String prefix) {
        xoNativeService.xoSetNameSpace(handle, prefix);
    }

    public void unsetNameSpace(String prefix) {
        xoNativeService.xoUnSetNameSpace(handle, prefix);
    }

    public void clearNameSpaces() {
        xoNativeService.xoUnSetNameSpace(handle, null);
    }

    public XoObject getXoObjectAttribute(String path) {
        long objectHandle = xoNativeService.xoObjectNmGetAttr(handle, path);
        if (objectHandle != 0) {
            return new XoObjectImpl(xoNativeService, objectHandle, obix);
        }
        return null;
    }

    public String getStringAttribute(String path) {
        return xoNativeService.xoStringNmGetAttr(handle, path);
    }

    public byte[] getBufferAttribute(String path) {
        return xoNativeService.xoBufferNmGetAttr(handle, path);
    }

    public List getXoObjectListAttribute(String path) {
        return new XoList(xoNativeService, handle, obix, path, XoList.TYPE_XO_OBJECT);
    }

    public List getStringListAttribute(String path) {
        return new XoList(xoNativeService, handle, obix, path, XoList.TYPE_STRING);
    }

    public void setXoObjectAttribute(String path, XoObject value) {
        if (value == null) {
            xoNativeService.xoNmUnSetAttr(handle, path);
        } else {
            xoNativeService.xoObjectNmSetAttr(handle, path, ((XoObjectImpl) value).handle);
        }
    }

    public void setStringAttribute(String path, String value) {
        if (value == null) {
            xoNativeService.xoNmUnSetAttr(handle, path);
        } else {
            xoNativeService.xoStringNmSetAttr(handle, path, value);
        }
    }

    public void setBufferAttribute(String path, byte[] value) {
        if (value == null) {
            xoNativeService.xoNmUnSetAttr(handle, path);
        } else {
            xoNativeService.xoBufferNmSetAttr(handle, path, value);
        }
    }

    public void xoBufferBinaryToXml(String path, boolean obix) throws XoException {
        xoNativeService.xoNmXoBufferBinaryToXml(handle, path, obix);
    }

    public void xoBufferBinaryToExi(String path, boolean obix) throws XoException {
        xoNativeService.xoNmXoBufferBinaryToExi(handle, path, obix);
    }

    public void xoBufferXmlToBinary(String path, boolean obix) throws XoException {
        xoNativeService.xoNmXoBufferXmlToBinary(handle, path, obix);
    }

    public void xoBufferXmlToExi(String path, boolean obix) throws XoException {
        xoNativeService.xoNmXoBufferXmlToExi(handle, path, obix);
    }

    public void xoBufferExiToBinary(String path, boolean obix) throws XoException {
        xoNativeService.xoNmXoBufferExiToBinary(handle, path, obix);
    }

    public void xoBufferExiToXml(String path, boolean obix) throws XoException {
        xoNativeService.xoNmXoBufferExiToXml(handle, path, obix);
    }

    public boolean isBinarySupported() {
        return xoNativeService.isBinarySupported();
    }

    public byte[] saveBinary() {
        return xoNativeService.xoSaveMem(handle);
    }

    public boolean isXmlSupported() {
        return xoNativeService.isXmlSupported();
    }

    public byte[] saveXml() throws XoException {
        return xoNativeService.xoWriteXmlMem(handle, "", obix);
    }

    public byte[] savePartialXml(String attributeName) throws XoException {
        return xoNativeService.xoWritePartialXmlMem(handle, attributeName, "");
    }

    public boolean isExiSupported() {
        return xoNativeService.isExiSupported();
    }

    public byte[] saveExi() throws XoException {
        return xoNativeService.xoWriteExiMem(handle, "", obix);
    }

    public byte[] savePartialExi(String attributeName) throws XoException {
        return xoNativeService.xoWritePartialExiMem(handle, attributeName, "");
    }

    public XoObject copy(boolean recurse) {
        long newHandle = xoNativeService.xoNewCopy(handle, recurse);
        return new XoObjectImpl(xoNativeService, newHandle, obix);
    }

    public void free(boolean cascadeMode) {
        xoNativeService.xoFree(handle, cascadeMode);
    }

    public int hashCode() {
        final int prime = 31;
        int result = 1;
        result = (int) (prime * result + handle);
        return result;
    }

    public boolean equals(Object obj) {
        if (this == obj) {
            return true;
        }
        if (obj == null) {
            return false;
        }
        if (getClass() != obj.getClass()) {
            return false;
        }
        XoObjectImpl other = (XoObjectImpl) obj;
        if (handle != other.handle) {
            return false;
        }
        return true;
    }
}
