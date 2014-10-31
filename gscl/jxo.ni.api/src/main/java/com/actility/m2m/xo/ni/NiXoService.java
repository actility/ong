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

package com.actility.m2m.xo.ni;

import com.actility.m2m.xo.XoException;

public interface NiXoService {
    public String xoNmType(long handle);

    public long xoLoadMem(byte[] rawObject);

    public long xoReadXmlMem(byte[] rawObject, boolean obix) throws XoException;

    public long xoReadPartialXmlMem(byte[] rawObject, String rootType) throws XoException;

    public long xoReadExiMem(byte[] rawObject, boolean obix) throws XoException;

    public long xoReadPartialExiMem(byte[] rawObject, String rootType) throws XoException;

    public long xoNmNew(String type);

    public long xoNewCopy(long xoHandle, boolean recurse);

    public int xoSetNameSpace(long xoHandle, String prefix);

    public int xoUnSetNameSpace(long xoHandle, String prefix);

    public void xoNmXoBufferBinaryToXml(long handle, String path, boolean obix) throws XoException;

    public void xoNmXoBufferBinaryToExi(long handle, String path, boolean obix) throws XoException;

    public void xoNmXoBufferXmlToBinary(long handle, String path, boolean obix) throws XoException;

    public void xoNmXoBufferXmlToExi(long handle, String path, boolean obix) throws XoException;

    public void xoNmXoBufferExiToBinary(long handle, String path, boolean obix) throws XoException;

    public void xoNmXoBufferExiToXml(long handle, String path, boolean obix) throws XoException;

    public boolean isBinarySupported();

    public byte[] xoSaveMem(long xoHandle);

    public boolean isXmlSupported();

    public byte[] xoWriteXmlMem(long xoHandle, String defaultNamespace, boolean obix) throws XoException;

    public byte[] xoWritePartialXmlMem(long xoHandle, String attributeName, String defaultNamespace) throws XoException;

    public boolean isExiSupported();

    public byte[] xoWriteExiMem(long xoHandle, String defaultNamespace, boolean obix) throws XoException;

    public byte[] xoWritePartialExiMem(long xoHandle, String attributeName, String defaultNamespace) throws XoException;

    public boolean xoNmContainsAttr(long xoHandle, String attributeName);

    public long xoObjectNmGetAttr(long xoHandle, String attributeName);

    public String xoStringNmGetAttr(long xoHandle, String attributeName);

    public byte[] xoBufferNmGetAttr(long xoHandle, String attributeName);

    public void xoObjectNmSetAttr(long xoHandle, String attributeName, long xoHandleValue);

    public void xoStringNmSetAttr(long xoHandle, String attributeName, String value);

    public void xoBufferNmSetAttr(long xoHandle, String attributeName, byte[] value);

    public void xoNmUnSetAttr(long xoHandle, String attributeName);

    public int xoObjectNmAddInAttr(long xoHandle, String attributeName, long xoHandleValue);

    public int xoStringNmAddInAttr(long xoHandle, String attributeName, String value);

    public int xoObjectNmIndexOf(long handle, String attributeName, long xoHandleValue);

    public int xoStringNmIndexOf(long handle, String attributeName, String value);

    public int xoNmNbInAttr(long handle, String attributeName);

    public int xoNmFirstAttr(long handle, String attributeName);

    public int xoNmNextAttr(long handle, String attributeName);

    public int xoNmLastAttr(long handle, String attributeName);

    public int xoNmSupInAttr(long handle, String attributeName);

    public int xoNmDropList(long handle, String attributeName);

    public int xoFree(long xoHandle, boolean cascadeMode);

    public void xoDumpNbOcc();
}
