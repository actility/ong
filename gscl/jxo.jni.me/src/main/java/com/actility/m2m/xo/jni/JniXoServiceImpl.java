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

package com.actility.m2m.xo.jni;

import com.actility.m2m.xo.XoException;
import com.actility.m2m.xo.ni.NiXoService;

public class JniXoServiceImpl implements NiXoService {

    public String xoNmType(long handle) {
        if (JniXo.ARCH_64) {
            return JniXo.xoNmType64(handle);
        }
        return JniXo.xoNmType((int) handle);
    }

    public long xoLoadMem(byte[] rawObject) {
        if (JniXo.ARCH_64) {
            return JniXo.xoLoadMem64(rawObject);
        }
        return JniXo.xoLoadMem(rawObject);
    }

    public long xoReadXmlMem(byte[] rawObject, boolean obix) throws XoException {
        if (JniXo.ARCH_64) {
            return JniXo.xoReadXmlMem64(rawObject, obix);
        }
        return JniXo.xoReadXmlMem(rawObject, obix);
    }

    public long xoReadPartialXmlMem(byte[] rawObject, String rootType) throws XoException {
        if (JniXo.ARCH_64) {
            return JniXo.xoReadPartialXmlMem64(rawObject, rootType);
        }
        return JniXo.xoReadPartialXmlMem(rawObject, rootType);
    }

    public long xoReadExiMem(byte[] rawObject, boolean obix) throws XoException {
        if (JniXo.ARCH_64) {
            return JniXo.xoReadExiMem64(rawObject, obix);
        }
        return JniXo.xoReadExiMem(rawObject, obix);
    }

    public long xoReadPartialExiMem(byte[] rawObject, String rootType) throws XoException {
        if (JniXo.ARCH_64) {
            return JniXo.xoReadPartialExiMem64(rawObject, rootType);
        }
        return JniXo.xoReadPartialExiMem(rawObject, rootType);
    }

    public long xoNmNew(String type) {
        if (JniXo.ARCH_64) {
            return JniXo.xoNmNew64(type);
        }
        return JniXo.xoNmNew(type);
    }

    public long xoNewCopy(long xoHandle, boolean recurse) {
        if (JniXo.ARCH_64) {
            return JniXo.xoNewCopy64(xoHandle, recurse);
        }
        return JniXo.xoNewCopy((int) xoHandle, recurse);
    }

    public int xoSetNameSpace(long xoHandle, String prefix) {
        if (JniXo.ARCH_64) {
            return JniXo.xoSetNameSpace64(xoHandle, prefix);
        }
        return JniXo.xoSetNameSpace((int) xoHandle, prefix);
    }

    public int xoUnSetNameSpace(long xoHandle, String prefix) {
        if (JniXo.ARCH_64) {
            return JniXo.xoUnSetNameSpace64(xoHandle, prefix);
        }
        return JniXo.xoUnSetNameSpace((int) xoHandle, prefix);
    }

    public void xoNmXoBufferBinaryToXml(long xoHandle, String path, boolean obix) throws XoException {
        if (JniXo.ARCH_64) {
            JniXo.xoNmXoBufferBinaryToXml64(xoHandle, path, obix);
        } else {
            JniXo.xoNmXoBufferBinaryToXml((int) xoHandle, path, obix);
        }
    }

    public void xoNmXoBufferBinaryToExi(long xoHandle, String path, boolean obix) throws XoException {
        if (JniXo.ARCH_64) {
            JniXo.xoNmXoBufferBinaryToExi64(xoHandle, path, obix);
        } else {
            JniXo.xoNmXoBufferBinaryToExi((int) xoHandle, path, obix);
        }
    }

    public void xoNmXoBufferXmlToBinary(long xoHandle, String path, boolean obix) throws XoException {
        if (JniXo.ARCH_64) {
            JniXo.xoNmXoBufferXmlToBinary64(xoHandle, path, obix);
        } else {
            JniXo.xoNmXoBufferXmlToBinary((int) xoHandle, path, obix);
        }
    }

    public void xoNmXoBufferXmlToExi(long xoHandle, String path, boolean obix) throws XoException {
        if (JniXo.ARCH_64) {
            JniXo.xoNmXoBufferXmlToExi64(xoHandle, path, obix);
        } else {
            JniXo.xoNmXoBufferXmlToExi((int) xoHandle, path, obix);
        }
    }

    public void xoNmXoBufferExiToBinary(long xoHandle, String path, boolean obix) throws XoException {
        if (JniXo.ARCH_64) {
            JniXo.xoNmXoBufferExiToBinary64(xoHandle, path, obix);
        } else {
            JniXo.xoNmXoBufferExiToBinary((int) xoHandle, path, obix);
        }
    }

    public void xoNmXoBufferExiToXml(long xoHandle, String path, boolean obix) throws XoException {
        if (JniXo.ARCH_64) {
            JniXo.xoNmXoBufferExiToXml64(xoHandle, path, obix);
        } else {
            JniXo.xoNmXoBufferExiToXml((int) xoHandle, path, obix);
        }
    }

    public boolean isBinarySupported() {
        return JniXo.isBinarySupported();
    }

    public byte[] xoSaveMem(long xoHandle) {
        if (JniXo.ARCH_64) {
            return JniXo.xoSaveMem64(xoHandle);
        }
        return JniXo.xoSaveMem((int) xoHandle);
    }

    public boolean isXmlSupported() {
        return JniXo.isXmlSupported();
    }

    public byte[] xoWriteXmlMem(long xoHandle, String defaultNamespace, boolean obix) throws XoException {
        if (JniXo.ARCH_64) {
            return JniXo.xoWriteXmlMem64(xoHandle, defaultNamespace, obix);
        }
        return JniXo.xoWriteXmlMem((int) xoHandle, defaultNamespace, obix);
    }

    public byte[] xoWritePartialXmlMem(long xoHandle, String attributeName, String defaultNamespace) throws XoException {
        if (JniXo.ARCH_64) {
            return JniXo.xoWritePartialXmlMem64(xoHandle, attributeName, defaultNamespace);
        }
        return JniXo.xoWritePartialXmlMem((int) xoHandle, attributeName, defaultNamespace);
    }

    public boolean isExiSupported() {
        return JniXo.isExiSupported();
    }

    public byte[] xoWriteExiMem(long xoHandle, String defaultNamespace, boolean obix) throws XoException {
        if (JniXo.ARCH_64) {
            return JniXo.xoWriteExiMem64(xoHandle, obix);
        }
        return JniXo.xoWriteExiMem((int) xoHandle, obix);
    }

    public byte[] xoWritePartialExiMem(long xoHandle, String attributeName, String defaultNamespace) throws XoException {
        if (JniXo.ARCH_64) {
            return JniXo.xoWritePartialExiMem64(xoHandle, attributeName);
        }
        return JniXo.xoWritePartialExiMem((int) xoHandle, attributeName);
    }

    public boolean xoNmContainsAttr(long xoHandle, String attributeName) {
        if (JniXo.ARCH_64) {
            return JniXo.xoNmContainsAttr64(xoHandle, attributeName);
        }
        return JniXo.xoNmContainsAttr((int) xoHandle, attributeName);
    }

    public long xoObjectNmGetAttr(long xoHandle, String attributeName) {
        if (JniXo.ARCH_64) {
            return JniXo.xoObjectNmGetAttr64(xoHandle, attributeName);
        }
        return JniXo.xoObjectNmGetAttr((int) xoHandle, attributeName);
    }

    public String xoStringNmGetAttr(long xoHandle, String attributeName) {
        if (JniXo.ARCH_64) {
            return JniXo.xoStringNmGetAttr64(xoHandle, attributeName);
        }
        return JniXo.xoStringNmGetAttr((int) xoHandle, attributeName);
    }

    public byte[] xoBufferNmGetAttr(long xoHandle, String attributeName) {
        if (JniXo.ARCH_64) {
            return JniXo.xoBufferNmGetAttr64(xoHandle, attributeName);
        }
        return JniXo.xoBufferNmGetAttr((int) xoHandle, attributeName);
    }

    public void xoObjectNmSetAttr(long xoHandle, String attributeName, long xoHandleValue) {
        if (JniXo.ARCH_64) {
            JniXo.xoObjectNmSetAttr64(xoHandle, attributeName, xoHandleValue);
        } else {
            JniXo.xoObjectNmSetAttr((int) xoHandle, attributeName, (int) xoHandleValue);
        }
    }

    public void xoStringNmSetAttr(long xoHandle, String attributeName, String value) {
        if (JniXo.ARCH_64) {
            JniXo.xoStringNmSetAttr64(xoHandle, attributeName, value);
        } else {
            JniXo.xoStringNmSetAttr((int) xoHandle, attributeName, value);
        }
    }

    public void xoBufferNmSetAttr(long xoHandle, String attributeName, byte[] value) {
        if (JniXo.ARCH_64) {
            JniXo.xoBufferNmSetAttr64(xoHandle, attributeName, value);
        } else {
            JniXo.xoBufferNmSetAttr((int) xoHandle, attributeName, value);
        }
    }

    public void xoNmUnSetAttr(long xoHandle, String attributeName) {
        if (JniXo.ARCH_64) {
            JniXo.xoNmUnSetAttr64(xoHandle, attributeName);
        } else {
            JniXo.xoNmUnSetAttr((int) xoHandle, attributeName);
        }
    }

    public int xoObjectNmAddInAttr(long xoHandle, String attributeName, long xoHandleValue) {
        if (JniXo.ARCH_64) {
            return JniXo.xoObjectNmAddInAttr64(xoHandle, attributeName, xoHandleValue);
        }
        return JniXo.xoObjectNmAddInAttr((int) xoHandle, attributeName, (int) xoHandleValue);
    }

    public int xoStringNmAddInAttr(long xoHandle, String attributeName, String value) {
        if (JniXo.ARCH_64) {
            return JniXo.xoStringNmAddInAttr64(xoHandle, attributeName, value);
        }
        return JniXo.xoStringNmAddInAttr((int) xoHandle, attributeName, value);
    }

    public int xoObjectNmIndexOf(long handle, String attributeName, long xoHandleValue) {
        if (JniXo.ARCH_64) {
            return JniXo.xoObjectNmIndexOf64(handle, attributeName, xoHandleValue);
        }
        return JniXo.xoObjectNmIndexOf((int) handle, attributeName, (int) xoHandleValue);
    }

    public int xoStringNmIndexOf(long handle, String attributeName, String value) {
        if (JniXo.ARCH_64) {
            return JniXo.xoStringNmIndexOf64(handle, attributeName, value);
        }
        return JniXo.xoStringNmIndexOf((int) handle, attributeName, value);
    }

    public int xoNmNbInAttr(long handle, String attributeName) {
        if (JniXo.ARCH_64) {
            return JniXo.xoNmNbInAttr64(handle, attributeName);
        }
        return JniXo.xoNmNbInAttr((int) handle, attributeName);
    }

    public int xoNmFirstAttr(long handle, String attributeName) {
        if (JniXo.ARCH_64) {
            return JniXo.xoNmFirstAttr64(handle, attributeName);
        }
        return JniXo.xoNmFirstAttr((int) handle, attributeName);
    }

    public int xoNmNextAttr(long handle, String attributeName) {
        if (JniXo.ARCH_64) {
            return JniXo.xoNmNextAttr64(handle, attributeName);
        }
        return JniXo.xoNmNextAttr((int) handle, attributeName);
    }

    public int xoNmLastAttr(long handle, String attributeName) {
        if (JniXo.ARCH_64) {
            return JniXo.xoNmLastAttr64(handle, attributeName);
        }
        return JniXo.xoNmLastAttr((int) handle, attributeName);
    }

    public int xoNmSupInAttr(long handle, String attributeName) {
        if (JniXo.ARCH_64) {
            return JniXo.xoNmSupInAttr64(handle, attributeName);
        }
        return JniXo.xoNmSupInAttr((int) handle, attributeName);
    }

    public int xoNmDropList(long handle, String attributeName) {
        if (JniXo.ARCH_64) {
            return JniXo.xoNmDropList64(handle, attributeName);
        }
        return JniXo.xoNmDropList((int) handle, attributeName);
    }

    public int xoFree(long xoHandle, boolean cascadeMode) {
        if (JniXo.ARCH_64) {
            return JniXo.xoFree64(xoHandle, cascadeMode);
        }
        return JniXo.xoFree((int) xoHandle, cascadeMode);
    }

    public void xoDumpNbOcc() {
        JniXo.xoDumpNbOcc();
    }

}
