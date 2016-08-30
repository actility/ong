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

import static org.easymock.EasyMock.expectLastCall;
import static org.easymock.EasyMock.replay;
import static org.easymock.EasyMock.verify;

import org.custommonkey.xmlunit.XMLTestCase;
import org.easymock.EasyMock;

import com.actility.m2m.xo.XoObject;
import com.actility.m2m.xo.ni.NiXoService;

public class XoObjectImplTest extends XMLTestCase {

    public void testGetType() {
        NiXoService mockXoNativeService = EasyMock.createMock(NiXoService.class);
        String type = "myType";
        long xoHandle = 1;


        mockXoNativeService.xoNmType(xoHandle);
        expectLastCall().andReturn(type).times(1);
        mockXoNativeService.xoFree(xoHandle, true);
        expectLastCall().andReturn(0).times(1);

        replay(mockXoNativeService);

        XoObject xoObject = new XoObjectImpl(mockXoNativeService, xoHandle, false);
        assertEquals(type, xoObject.getType());
        xoObject.free(true);

        verify(mockXoNativeService);
    }
}
