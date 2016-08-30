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
import junit.framework.TestCase;

import org.easymock.EasyMock;

import com.actility.m2m.xo.XoException;
import com.actility.m2m.xo.XoObject;
import com.actility.m2m.xo.ni.NiXoService;

public class XoServiceImplTest extends TestCase {


    public void testReadXmlXoObject() {
        NiXoService mockXoNativeService = EasyMock.createMock(NiXoService.class);
        XoServiceImpl xoService = new XoServiceImpl(mockXoNativeService);
        byte[] content = "any content".getBytes();
        long xoHandle = 1;

        // describe the expected calls on mock object
        try {
            // readXmlXoObject
            mockXoNativeService.xoReadXmlMem(content, false);
            expectLastCall().andReturn(xoHandle).times(1);

            // saveXml
            mockXoNativeService.xoWriteXmlMem(xoHandle, "", false);
            expectLastCall().andReturn(content).times(1);

            // free
            mockXoNativeService.xoFree(xoHandle, true);
            expectLastCall().andReturn(0).times(1);
        } catch (XoException e) {
            fail(e.toString());
        }

        // replay it
        replay(mockXoNativeService);

        // do the test
        try {
            XoObject xoObject = xoService.readXmlXmlXoObject(content, "UTF-8");
            assertNotNull(xoObject);
            System.out.println(new String(xoObject.saveXml()));
            xoObject.free(true);
        } catch (XoException e) {
            fail(e.toString());
        }

        // and do the verifications
        verify(mockXoNativeService);
    }

    public void testReadBinaryXmlXoObject() {
        NiXoService mockXoNativeService = EasyMock.createMock(NiXoService.class);
        XoServiceImpl xoService = new XoServiceImpl(mockXoNativeService);
        byte[] content = "any content".getBytes();
        byte[] binContent = "binary content".getBytes();
        long xoHandle = 1;
        long xoHandleBin = 2;

        // describe the expected calls on mock object
        try {
            // readXmlXoObject
            mockXoNativeService.xoReadXmlMem(content, false);
            expectLastCall().andReturn(xoHandle).times(1);

            // saveBinary
            mockXoNativeService.xoSaveMem(xoHandle);
            expectLastCall().andReturn(binContent).times(1);

            // readBinaryXmlXoObject
            mockXoNativeService.xoLoadMem(binContent);
            expectLastCall().andReturn(xoHandleBin).times(1);

            // free xoObject
            mockXoNativeService.xoFree(xoHandle, true);
            expectLastCall().andReturn(0).times(1);
            // free xoObject2
            mockXoNativeService.xoFree(xoHandleBin, true);
            expectLastCall().andReturn(0).times(1);
        } catch (XoException e) {
            fail(e.toString());
        }

        // replay it
        replay(mockXoNativeService);

        // do the test
        try {
            XoObject xoObject = xoService.readXmlXmlXoObject(content, "UTF-8");

            byte[] rawBinary = xoObject.saveBinary();

            XoObject xoObject2 = xoService.readBinaryXmlXoObject(rawBinary);
            assertNotNull(xoObject2);
            xoObject.free(true);
            xoObject2.free(true);
        } catch (XoException e) {
            fail(e.toString());
        }

        // and do the verifications
        verify(mockXoNativeService);
    }

    public void testNewXmlXoObject() {
        NiXoService mockXoNativeService = EasyMock.createMock(NiXoService.class);
        XoServiceImpl xoService = new XoServiceImpl(mockXoNativeService);
        long xoHandle = 1;

        // readXmlXoObject
        mockXoNativeService.xoNmNew("m2m:container");
        expectLastCall().andReturn(xoHandle).times(1);

        // free xoObject
        mockXoNativeService.xoFree(xoHandle, true);
        expectLastCall().andReturn(0).times(1);

        // replay it
        replay(mockXoNativeService);

        // do the test
        try {
            XoObject xoObject = xoService.newXmlXoObject("m2m:container");
            assertNotNull(xoObject);
            xoObject.free(true);
        } catch (XoException e) {
            fail();
        }
    }

    public void testReadPartialXmlXoObject() {
        NiXoService mockXoNativeService = EasyMock.createMock(NiXoService.class);
        XoServiceImpl xoService = new XoServiceImpl(mockXoNativeService);
        byte[] content = "any content".getBytes();
        long xoHandle = 1;

        // describe the expected calls on mock object
        try {
            // readXmlXoObject
            mockXoNativeService.xoReadPartialXmlMem(content, "m2m:container");
            expectLastCall().andReturn(xoHandle).times(1);

            // free xoObject
            mockXoNativeService.xoFree(xoHandle, true);
            expectLastCall().andReturn(0).times(1);

        } catch (XoException e) {
            fail(e.toString());
        }

        // replay it
        replay(mockXoNativeService);

        // do the test
        try {
            XoObject xoObject = xoService.readPartialXmlXmlXoObject(content, "UTF-8", "m2m:container");
            assertNotNull(xoObject);
            xoObject.free(true);
        } catch (XoException e) {
            fail();
        }
    }

////    public void testAMoi() {
////        try {
////            String obixXml = readFomFile(getClass().getClassLoader().getResource("obix.xml").getPath(), "UTF-8");
////            System.out.println(obixXml);
////            XoObject xoObject = xoService.readXmlXoObject(obixXml.getBytes(), "UTF-8");
////            System.out.println("Isnull ?");
////            assertNotNull(xoObject);
////            BufferedOutputStream out = new BufferedOutputStream(new FileOutputStream("/home/jreich/projects/contentInstance.bin"));
////            try {
////                out.write(xoObject.saveBinary());
////            } finally {
////                out.close();
////            }
////            System.out.println(new String(xoObject.saveXml()));
////            xoObject.free(true);
////        } catch (IOException e) {
////            e.printStackTrace();
////            fail();
////        } catch (XoException e) {
////            e.printStackTrace();
////            fail();
////        }
////    }
}
