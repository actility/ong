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
 * id $Id: JniXo.java 4184 2013-01-23 10:20:36Z JReich $
 * author $Author: JReich $
 * version $Revision: 4184 $
 * lastrevision $Date: 2013-01-23 11:20:36 +0100 (Wed, 23 Jan 2013) $
 * modifiedby $LastChangedBy: JReich $
 * lastmodified $LastChangedDate: 2013-01-23 11:20:36 +0100 (Wed, 23 Jan 2013) $
 */

package com.actility.m2m.xo.jni;

import com.actility.m2m.xo.XoException;

public final class JniXo {
    public static boolean ARCH_64 = false;

    static {
        try {
            System.loadLibrary("JxoJni");
            int result = init();
            if (result == 8) {
                ARCH_64 = true;
            } else {
                ARCH_64 = false;
            }
        } catch (Exception e) {
            e.printStackTrace();
        }
    }

    private JniXo() {
        // Block instantiatiation of this class
    }

    /**
     * Initialize library
     */
    protected static native int init();

    public static native boolean start(String namespacesFile, String refsFile, String dictFile);

    public static native void stop();

    public static native boolean isBinarySupported();

    public static native boolean isXmlSupported();

    public static native boolean isExiSupported();

    public static native String xoNmType(int handle);

    public static native int xoLoadMem(byte[] rawObject);

    public static native int xoReadXmlMem(byte[] rawObject, boolean obix) throws XoException;

    public static native int xoReadPartialXmlMem(byte[] rawObject, String rootType) throws XoException;

    public static native int xoReadExiMem(byte[] rawObject, boolean obix) throws XoException;

    public static native int xoReadPartialExiMem(byte[] rawObject, String rootType) throws XoException;

    public static native int xoNmNew(String type);

    public static native int xoNewCopy(int xoHandle, boolean recurse);

    public static native int xoSetNameSpace(int xoHandle, String prefix);

    public static native int xoUnSetNameSpace(int xoHandle, String prefix);

    public static native void xoNmXoBufferBinaryToXml(int xoHandle, String path, boolean obix) throws XoException;

    public static native void xoNmXoBufferBinaryToExi(int xoHandle, String path, boolean obix) throws XoException;

    public static native void xoNmXoBufferXmlToBinary(int xoHandle, String path, boolean obix) throws XoException;

    public static native void xoNmXoBufferXmlToExi(int xoHandle, String path, boolean obix) throws XoException;

    public static native void xoNmXoBufferExiToBinary(int xoHandle, String path, boolean obix) throws XoException;

    public static native void xoNmXoBufferExiToXml(int xoHandle, String path, boolean obix) throws XoException;

    public static native byte[] xoWriteXmlMem(int xoHandle, String defaultNamespace, boolean obix) throws XoException;

    public static native byte[] xoWritePartialXmlMem(int xoHandle, String attributeName, String defaultNamespace)
            throws XoException;

    public static native byte[] xoWriteExiMem(int xoHandle, boolean obix) throws XoException;

    public static native byte[] xoWritePartialExiMem(int xoHandle, String attributeName) throws XoException;

    public static native byte[] xoSaveMem(int xoHandle);

    public static native boolean xoNmContainsAttr(int xoHandle, String attributeName);

    public static native int xoObjectNmGetAttr(int xoHandle, String attributeName);

    public static native String xoStringNmGetAttr(int xoHandle, String attributeName);

    public static native byte[] xoBufferNmGetAttr(int xoHandle, String attributeName);

    public static native void xoObjectNmSetAttr(int xoHandle, String attributeName, int xoHandleValue);

    public static native void xoStringNmSetAttr(int xoHandle, String attributeName, String value);

    public static native void xoBufferNmSetAttr(int xoHandle, String attributeName, byte[] value);

    public static native void xoNmUnSetAttr(int xoHandle, String attributeName);

    public static native int xoObjectNmAddInAttr(int xoHandle, String attributeName, int xoHandleValue);

    public static native int xoStringNmAddInAttr(int xoHandle, String attributeName, String value);

    public static native int xoObjectNmIndexOf(int handle, String attributeName, int xoHandleValue);

    public static native int xoStringNmIndexOf(int handle, String attributeName, String value);

    public static native int xoNmNbInAttr(int handle, String attributeName);

    public static native int xoNmFirstAttr(int handle, String attributeName);

    public static native int xoNmNextAttr(int handle, String attributeName);

    public static native int xoNmLastAttr(int handle, String attributeName);

    public static native int xoNmSupInAttr(int handle, String attributeName);

    public static native int xoNmDropList(int handle, String attributeName);

    public static native int xoFree(int xoHandle, boolean cascadeMode);

    public static native String xoNmType64(long handle);

    public static native long xoLoadMem64(byte[] rawObject);

    public static native long xoReadXmlMem64(byte[] rawObject, boolean obix) throws XoException;

    public static native long xoReadPartialXmlMem64(byte[] rawObject, String rootType) throws XoException;

    public static native long xoReadExiMem64(byte[] rawObject, boolean obix) throws XoException;

    public static native long xoReadPartialExiMem64(byte[] rawObject, String rootType) throws XoException;

    public static native long xoNmNew64(String type);

    public static native long xoNewCopy64(long xoHandle, boolean recurse);

    public static native int xoSetNameSpace64(long xoHandle, String prefix);

    public static native int xoUnSetNameSpace64(long xoHandle, String prefix);

    public static native void xoNmXoBufferBinaryToXml64(long xoHandle, String path, boolean obix) throws XoException;

    public static native void xoNmXoBufferBinaryToExi64(long xoHandle, String path, boolean obix) throws XoException;

    public static native void xoNmXoBufferXmlToBinary64(long xoHandle, String path, boolean obix) throws XoException;

    public static native void xoNmXoBufferXmlToExi64(long xoHandle, String path, boolean obix) throws XoException;

    public static native void xoNmXoBufferExiToBinary64(long xoHandle, String path, boolean obix) throws XoException;

    public static native void xoNmXoBufferExiToXml64(long xoHandle, String path, boolean obix) throws XoException;

    public static native byte[] xoWriteXmlMem64(long xoHandle, String defaultNamespace, boolean obix) throws XoException;

    public static native byte[] xoWritePartialXmlMem64(long xoHandle, String attributeName, String defaultNamespace)
            throws XoException;

    public static native byte[] xoWriteExiMem64(long xoHandle, boolean obix) throws XoException;

    public static native byte[] xoWritePartialExiMem64(long xoHandle, String attributeName) throws XoException;

    public static native byte[] xoSaveMem64(long xoHandle);

    public static native boolean xoNmContainsAttr64(long xoHandle, String attributeName);

    public static native long xoObjectNmGetAttr64(long xoHandle, String attributeName);

    public static native String xoStringNmGetAttr64(long xoHandle, String attributeName);

    public static native byte[] xoBufferNmGetAttr64(long xoHandle, String attributeName);

    public static native void xoObjectNmSetAttr64(long xoHandle, String attributeName, long xoHandleValue);

    public static native void xoStringNmSetAttr64(long xoHandle, String attributeName, String value);

    public static native void xoBufferNmSetAttr64(long xoHandle, String attributeName, byte[] value);

    public static native void xoNmUnSetAttr64(long xoHandle, String attributeName);

    public static native int xoObjectNmAddInAttr64(long xoHandle, String attributeName, long xoHandleValue);

    public static native int xoStringNmAddInAttr64(long xoHandle, String attributeName, String value);

    public static native int xoObjectNmIndexOf64(long handle, String attributeName, long xoHandleValue);

    public static native int xoStringNmIndexOf64(long handle, String attributeName, String value);

    public static native int xoNmNbInAttr64(long handle, String attributeName);

    public static native int xoNmFirstAttr64(long handle, String attributeName);

    public static native int xoNmNextAttr64(long handle, String attributeName);

    public static native int xoNmLastAttr64(long handle, String attributeName);

    public static native int xoNmSupInAttr64(long handle, String attributeName);

    public static native int xoNmDropList64(long handle, String attributeName);

    public static native int xoFree64(long xoHandle, boolean cascadeMode);

    public static native void xoDumpNbOcc();
}
