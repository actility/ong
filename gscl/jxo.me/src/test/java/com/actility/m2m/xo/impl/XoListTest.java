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

import java.util.Iterator;
import java.util.List;

import junit.framework.TestCase;

import org.easymock.EasyMock;

import com.actility.m2m.xo.XoException;
import com.actility.m2m.xo.XoObject;
import com.actility.m2m.xo.XoService;
import com.actility.m2m.xo.ni.NiXoService;

public class XoListTest extends TestCase {

    public void testAdd1() {
        NiXoService mockXoNativeService = EasyMock.createMock(NiXoService.class);
        XoService xoService = new XoServiceImpl(mockXoNativeService);
        long searchStrings = 1;
        String toto = "toto";

        // newXmlXoObject on "m2m:searchStrings"
        mockXoNativeService.xoNmNew("m2m:searchStrings");
        expectLastCall().andReturn(searchStrings).times(1);

        // add
        mockXoNativeService.xoStringNmAddInAttr(searchStrings, "m2m:searchString", toto);
        expectLastCall().andReturn(0).times(1);

        // get[0]
        mockXoNativeService.xoStringNmGetAttr(searchStrings, "m2m:searchString[0]");
        expectLastCall().andReturn(toto).times(1);

        // free
        mockXoNativeService.xoFree(searchStrings, true);
        expectLastCall().andReturn(0).times(1);

        replay(mockXoNativeService);

        try {
            XoObject xoObject = xoService.newXmlXoObject("m2m:searchStrings");
            List searchString = xoObject.getStringListAttribute("m2m:searchString");
            searchString.add("toto");
            assertEquals("toto", searchString.get(0));
            xoObject.free(true);
        } catch (XoException e) {
            fail();
        }

        verify(mockXoNativeService);
    }

    public void testAdd2() {
        NiXoService mockXoNativeService = EasyMock.createMock(NiXoService.class);
        XoService xoService = new XoServiceImpl(mockXoNativeService);
        long aPocPaths = 1;
        long xoHandleElt = 2;

        // newXmlXoObject on "m2m:aPoCPaths"
        mockXoNativeService.xoNmNew("m2m:aPoCPaths");
        expectLastCall().andReturn(aPocPaths).times(1);

        // newXmlXoObject on "m2m:aPoCPath"
        mockXoNativeService.xoNmNew("m2m:aPoCPath");
        expectLastCall().andReturn(xoHandleElt).times(1);

        // add
        mockXoNativeService.xoObjectNmAddInAttr(aPocPaths, "m2m:aPoCPath", xoHandleElt);
        expectLastCall().andReturn(0).times(1);

        // get[0]
        mockXoNativeService.xoObjectNmGetAttr(aPocPaths, "m2m:aPoCPath[0]");
        expectLastCall().andReturn(xoHandleElt).times(1);

        // free
        mockXoNativeService.xoFree(aPocPaths, true);
        expectLastCall().andReturn(0).times(1);

        replay(mockXoNativeService);

        try {
            XoObject xoObject = xoService.newXmlXoObject("m2m:aPoCPaths");
            List apocPath = xoObject.getXoObjectListAttribute("m2m:aPoCPath");
            XoObject apocPathElem = xoService.newXmlXoObject("m2m:aPoCPath");
            apocPath.add(apocPathElem);
            assertEquals(apocPathElem, apocPath.get(0));
            xoObject.free(true);
        } catch (XoException e) {
            fail();
        }
    }

    public void testClear1() {

        NiXoService mockXoNativeService = EasyMock.createMock(NiXoService.class);
        XoService xoService = new XoServiceImpl(mockXoNativeService);
        long searchStrings = 1;
        String toto = "toto";
        String tata = "tata";
        String titi = "titi";

        // newXmlXoObject on "m2m:searchStrings"
        mockXoNativeService.xoNmNew("m2m:searchStrings");
        expectLastCall().andReturn(searchStrings).times(1);

        // add*3
        mockXoNativeService.xoStringNmAddInAttr(searchStrings, "m2m:searchString", toto);
        expectLastCall().andReturn(0).times(1);
        mockXoNativeService.xoStringNmAddInAttr(searchStrings, "m2m:searchString", tata);
        expectLastCall().andReturn(0).times(1);
        mockXoNativeService.xoStringNmAddInAttr(searchStrings, "m2m:searchString", titi);
        expectLastCall().andReturn(0).times(1);

        // clear
        mockXoNativeService.xoNmDropList(searchStrings, "m2m:searchString");
        expectLastCall().andReturn(0).times(1);

        // size
        mockXoNativeService.xoNmNbInAttr(searchStrings, "m2m:searchString");
        expectLastCall().andReturn(0).times(1);

        // free
        mockXoNativeService.xoFree(searchStrings, true);
        expectLastCall().andReturn(0).times(1);

        replay(mockXoNativeService);

        try {
            XoObject xoObject = xoService.newXmlXoObject("m2m:searchStrings");
            List searchString = xoObject.getStringListAttribute("m2m:searchString");
            searchString.add(toto);
            searchString.add(tata);
            searchString.add(titi);

            searchString.clear();
            assertEquals(0, searchString.size());

            xoObject.free(true);
        } catch (XoException e) {
            fail();
        }

        verify(mockXoNativeService);
    }

    public void testClear2() {
        NiXoService mockXoNativeService = EasyMock.createMock(NiXoService.class);
        XoService xoService = new XoServiceImpl(mockXoNativeService);
        long aPoCPaths = 1;
        long aPoCPath1 = 2;
        long aPoCPath2 = 3;
        long aPoCPath3 = 4;

        // newXmlXoObject on "m2m:aPoCPaths"
        mockXoNativeService.xoNmNew("m2m:aPoCPaths");
        expectLastCall().andReturn(aPoCPaths).times(1);

        // newXmlXoObject*3 on "m2m:aPoCPath"
        mockXoNativeService.xoNmNew("m2m:aPoCPath");
        expectLastCall().andReturn(aPoCPath1).times(1);
        mockXoNativeService.xoNmNew("m2m:aPoCPath");
        expectLastCall().andReturn(aPoCPath2).times(1);
        mockXoNativeService.xoNmNew("m2m:aPoCPath");
        expectLastCall().andReturn(aPoCPath3).times(1);

        // add*3
        mockXoNativeService.xoObjectNmAddInAttr(aPoCPaths, "m2m:aPoCPath", aPoCPath1);
        expectLastCall().andReturn(0).times(1);
        mockXoNativeService.xoObjectNmAddInAttr(aPoCPaths, "m2m:aPoCPath", aPoCPath2);
        expectLastCall().andReturn(0).times(1);
        mockXoNativeService.xoObjectNmAddInAttr(aPoCPaths, "m2m:aPoCPath", aPoCPath3);
        expectLastCall().andReturn(0).times(1);

        // clear
        mockXoNativeService.xoNmDropList(aPoCPaths, "m2m:aPoCPath");
        expectLastCall().andReturn(0).times(1);

        // size
        mockXoNativeService.xoNmNbInAttr(aPoCPaths, "m2m:aPoCPath");
        expectLastCall().andReturn(0).times(1);

        // free
        mockXoNativeService.xoFree(aPoCPaths, true);
        expectLastCall().andReturn(0).times(1);

        replay(mockXoNativeService);

        try {
            XoObject xoObject = xoService.newXmlXoObject("m2m:aPoCPaths");
            List apocPaths = xoObject.getXoObjectListAttribute("m2m:aPoCPath");
            XoObject apocPathElem1 = xoService.newXmlXoObject("m2m:aPoCPath");
            XoObject apocPathElem2 = xoService.newXmlXoObject("m2m:aPoCPath");
            XoObject apocPathElem3 = xoService.newXmlXoObject("m2m:aPoCPath");
            apocPaths.add(apocPathElem1);
            apocPaths.add(apocPathElem2);
            apocPaths.add(apocPathElem3);
            apocPaths.clear();
            assertEquals(0, apocPaths.size());
            xoObject.free(true);
        } catch (XoException e) {
            fail();
        }

        verify(mockXoNativeService);
    }

    // public void testContains1() {
    // try {
    // XoObject xoObject = xoService.newXmlXoObject("m2m:searchStrings");
    // List searchString = xoObject.getStringListAttribute("m2m:searchString");
    // searchString.add("toto");
    // searchString.add("tata");
    // searchString.add("titi");
    // assertTrue(searchString.contains("toto"));
    // xoObject.free(true);
    // } catch (XoException e) {
    // fail();
    // }
    // }
    //
    // public void testContains2() {
    // try {
    // XoObject xoObject = xoService.newXmlXoObject("m2m:searchStrings");
    // List searchString = xoObject.getStringListAttribute("m2m:searchString");
    // searchString.add("toto");
    // searchString.add("tata");
    // searchString.add("titi");
    // assertFalse(searchString.contains("papa"));
    // xoObject.free(true);
    // } catch (XoException e) {
    // fail();
    // }
    // }
    //
    // public void testContains3() {
    // try {
    // XoObject xoObject = xoService.newXmlXoObject("m2m:aPoCPaths");
    // List apocPath = xoObject.getXoObjectListAttribute("m2m:aPoCPath");
    // XoObject apocPathElem = xoService.newXmlXoObject("m2m:aPoCPath");
    // apocPath.add(apocPathElem);
    // assertTrue(apocPath.contains(apocPathElem));
    // xoObject.free(true);
    // } catch (XoException e) {
    // fail();
    // }
    // }
    //
    // public void testContains4() {
    // try {
    // XoObject xoObject = xoService.newXmlXoObject("m2m:aPoCPaths");
    // List apocPath = xoObject.getXoObjectListAttribute("m2m:aPoCPath");
    // XoObject apocPathElem = xoService.newXmlXoObject("m2m:aPoCPath");
    // apocPath.add(apocPathElem);
    // assertFalse(apocPath.contains(new XoObjectImpl(10, true)));
    // xoObject.free(true);
    // } catch (XoException e) {
    // fail();
    // }
    // }
    //
    // public void testGet1() {
    // try {
    // XoObject xoObject = xoService.newXmlXoObject("m2m:searchStrings");
    // List searchString = xoObject.getStringListAttribute("m2m:searchString");
    // searchString.add("toto");
    // searchString.add("tata");
    // searchString.add("titi");
    // assertEquals("toto", searchString.get(0));
    // xoObject.free(true);
    // } catch (XoException e) {
    // fail();
    // }
    // }
    //
    // public void testGet2() {
    // XoObject xoObject = null;
    // try {
    // xoObject = xoService.newXmlXoObject("m2m:searchStrings");
    // List searchString = xoObject.getStringListAttribute("m2m:searchString");
    // searchString.add("toto");
    // searchString.add("tata");
    // searchString.add("titi");
    // searchString.get(10);
    // fail();
    // } catch (XoException e) {
    // fail();
    // } catch (IndexOutOfBoundsException e) {
    // if (xoObject != null) {
    // xoObject.free(true);
    // }
    // }
    // }
    //
    // public void testGet3() {
    // try {
    // XoObject xoObject = xoService.newXmlXoObject("m2m:aPoCPaths");
    // List apocPath = xoObject.getXoObjectListAttribute("m2m:aPoCPath");
    // XoObject apocPathElem = xoService.newXmlXoObject("m2m:aPoCPath");
    // apocPath.add(apocPathElem);
    // assertEquals(apocPathElem, apocPath.get(0));
    // xoObject.free(true);
    // } catch (XoException e) {
    // fail();
    // }
    // }
    //
    // public void testGet4() {
    // XoObject xoObject = null;
    // try {
    // xoObject = xoService.newXmlXoObject("m2m:aPoCPaths");
    // List apocPath = xoObject.getXoObjectListAttribute("m2m:aPoCPath");
    // XoObject apocPathElem = xoService.newXmlXoObject("m2m:aPoCPath");
    // apocPath.add(apocPathElem);
    // apocPath.get(10);
    // fail();
    // } catch (XoException e) {
    // fail();
    // } catch (IndexOutOfBoundsException e) {
    // if (xoObject != null) {
    // xoObject.free(true);
    // }
    // }
    // }
    //
    // public void testIndexOf1() {
    // try {
    // XoObject xoObject = xoService.newXmlXoObject("m2m:searchStrings");
    // List searchString = xoObject.getStringListAttribute("m2m:searchString");
    // searchString.add("toto");
    // searchString.add("tata");
    // searchString.add("titi");
    // assertEquals(0, searchString.indexOf("toto"));
    // xoObject.free(true);
    // } catch (XoException e) {
    // fail();
    // }
    // }
    //
    // public void testIndexOf2() {
    // try {
    // XoObject xoObject = xoService.newXmlXoObject("m2m:searchStrings");
    // List searchString = xoObject.getStringListAttribute("m2m:searchString");
    // searchString.add("toto");
    // searchString.add("tata");
    // searchString.add("titi");
    // assertEquals(-1, searchString.indexOf("papa"));
    // xoObject.free(true);
    // } catch (XoException e) {
    // fail();
    // }
    // }
    //
    // public void testIndexOf3() {
    // try {
    // XoObject xoObject = xoService.newXmlXoObject("m2m:aPoCPaths");
    // List apocPath = xoObject.getXoObjectListAttribute("m2m:aPoCPath");
    // XoObject apocPathElem = xoService.newXmlXoObject("m2m:aPoCPath");
    // apocPath.add(apocPathElem);
    // assertEquals(0, apocPath.indexOf(apocPathElem));
    // xoObject.free(true);
    // } catch (XoException e) {
    // fail();
    // }
    // }
    //
    // public void testIndexOf4() {
    // try {
    // XoObject xoObject = xoService.newXmlXoObject("m2m:aPoCPaths");
    // List apocPath = xoObject.getXoObjectListAttribute("m2m:aPoCPath");
    // XoObject apocPathElem = xoService.newXmlXoObject("m2m:aPoCPath");
    // apocPath.add(apocPathElem);
    // assertEquals(-1, apocPath.indexOf(new XoObjectImpl(10, true)));
    // xoObject.free(true);
    // } catch (XoException e) {
    // fail();
    // }
    // }
    //
    // public void testIsEmpty1() {
    // try {
    // XoObject xoObject = xoService.newXmlXoObject("m2m:searchStrings");
    // List searchString = xoObject.getStringListAttribute("m2m:searchString");
    // assertTrue(searchString.isEmpty());
    // xoObject.free(true);
    // } catch (XoException e) {
    // fail();
    // }
    // }
    //
    // public void testIsEmpty2() {
    // try {
    // XoObject xoObject = xoService.newXmlXoObject("m2m:searchStrings");
    // List searchString = xoObject.getStringListAttribute("m2m:searchString");
    // searchString.add("toto");
    // assertFalse(searchString.isEmpty());
    // xoObject.free(true);
    // } catch (XoException e) {
    // fail();
    // }
    // }
    //
    // public void testIsEmpty3() {
    // try {
    // XoObject xoObject = xoService.newXmlXoObject("m2m:aPoCPaths");
    // List apocPath = xoObject.getXoObjectListAttribute("m2m:aPoCPath");
    // assertTrue(apocPath.isEmpty());
    // xoObject.free(true);
    // } catch (XoException e) {
    // fail();
    // }
    // }
    //
    // public void testIsEmpty4() {
    // try {
    // XoObject xoObject = xoService.newXmlXoObject("m2m:aPoCPaths");
    // List apocPath = xoObject.getXoObjectListAttribute("m2m:aPoCPath");
    // XoObject apocPathElem = xoService.newXmlXoObject("m2m:aPoCPath");
    // apocPath.add(apocPathElem);
    // assertFalse(apocPath.isEmpty());
    // xoObject.free(true);
    // } catch (XoException e) {
    // fail();
    // }
    // }
    //
    // public void testRemove1() {
    // try {
    // XoObject xoObject = xoService.newXmlXoObject("m2m:searchStrings");
    // List searchString = xoObject.getStringListAttribute("m2m:searchString");
    // searchString.add("toto");
    // searchString.remove("toto");
    // assertEquals(0, searchString.size());
    // xoObject.free(true);
    // } catch (XoException e) {
    // fail();
    // }
    // }
    //
    // public void testRemove2() {
    // try {
    // XoObject xoObject = xoService.newXmlXoObject("m2m:searchStrings");
    // List searchString = xoObject.getStringListAttribute("m2m:searchString");
    // searchString.add("toto");
    // searchString.remove("tata");
    // assertEquals(1, searchString.size());
    // xoObject.free(true);
    // } catch (XoException e) {
    // fail();
    // }
    // }
    //
    // public void testRemove3() {
    // try {
    // XoObject xoObject = xoService.newXmlXoObject("m2m:searchStrings");
    // List searchString = xoObject.getStringListAttribute("m2m:searchString");
    // searchString.add("toto");
    // searchString.remove(0);
    // assertEquals(0, searchString.size());
    // xoObject.free(true);
    // } catch (XoException e) {
    // fail();
    // }
    // }
    //
    // public void testRemove4() {
    // try {
    // XoObject xoObject = xoService.newXmlXoObject("m2m:searchStrings");
    // List searchString = xoObject.getStringListAttribute("m2m:searchString");
    // searchString.add("toto");
    // searchString.remove(1);
    // assertEquals(1, searchString.size());
    // xoObject.free(true);
    // } catch (XoException e) {
    // fail();
    // }
    // }
    //
    // public void testRemove5() {
    // try {
    // XoObject xoObject = xoService.newXmlXoObject("m2m:aPoCPaths");
    // List apocPath = xoObject.getXoObjectListAttribute("m2m:aPoCPath");
    // XoObject apocPathElem = xoService.newXmlXoObject("m2m:aPoCPath");
    // apocPath.add(apocPathElem);
    // apocPath.remove(apocPathElem);
    // assertEquals(0, apocPath.size());
    // xoObject.free(true);
    // } catch (XoException e) {
    // fail();
    // }
    // }
    //
    // public void testRemove6() {
    // try {
    // XoObject xoObject = xoService.newXmlXoObject("m2m:aPoCPaths");
    // List apocPath = xoObject.getXoObjectListAttribute("m2m:aPoCPath");
    // XoObject apocPathElem = xoService.newXmlXoObject("m2m:aPoCPath");
    // apocPath.add(apocPathElem);
    // apocPath.remove(new XoObjectImpl(10, true));
    // assertEquals(1, apocPath.size());
    // xoObject.free(true);
    // } catch (XoException e) {
    // fail();
    // }
    // }
    //
    // public void testRemove7() {
    // try {
    // XoObject xoObject = xoService.newXmlXoObject("m2m:aPoCPaths");
    // List apocPath = xoObject.getXoObjectListAttribute("m2m:aPoCPath");
    // XoObject apocPathElem = xoService.newXmlXoObject("m2m:aPoCPath");
    // apocPath.add(apocPathElem);
    // apocPath.remove(0);
    // assertEquals(0, apocPath.size());
    // xoObject.free(true);
    // } catch (XoException e) {
    // fail();
    // }
    // }
    //
    // public void testRemove8() {
    // try {
    // XoObject xoObject = xoService.newXmlXoObject("m2m:aPoCPaths");
    // List apocPath = xoObject.getXoObjectListAttribute("m2m:aPoCPath");
    // XoObject apocPathElem = xoService.newXmlXoObject("m2m:aPoCPath");
    // apocPath.add(apocPathElem);
    // apocPath.remove(1);
    // assertEquals(1, apocPath.size());
    // xoObject.free(true);
    // } catch (XoException e) {
    // fail();
    // }
    // }

    public void testIterator1() {

        NiXoService mockXoNativeService = EasyMock.createMock(NiXoService.class);
        XoService xoService = new XoServiceImpl(mockXoNativeService);
        long searchStrings = 1;
        long aPoCPath1 = 2;
        long aPoCPath2 = 3;
        long aPoCPath3 = 4;
        String toto = "toto";
        String tata = "tata";
        String titi = "titi";

        // newXmlXoObject on "m2m:searchStrings"
        mockXoNativeService.xoNmNew("m2m:searchStrings");
        expectLastCall().andReturn(searchStrings).times(1);

        // add*3
        mockXoNativeService.xoStringNmAddInAttr(searchStrings, "m2m:searchString", toto);
        expectLastCall().andReturn(0).times(1);
        mockXoNativeService.xoStringNmAddInAttr(searchStrings, "m2m:searchString", tata);
        expectLastCall().andReturn(0).times(1);
        mockXoNativeService.xoStringNmAddInAttr(searchStrings, "m2m:searchString", titi);
        expectLastCall().andReturn(0).times(1);

        // iterator(), hasNext(), and next()
        mockXoNativeService.xoNmFirstAttr(searchStrings, "m2m:searchString");
        expectLastCall().andReturn(aPoCPath1).times(1);
        mockXoNativeService.xoStringNmGetAttr(searchStrings, "m2m:searchString");
        expectLastCall().andReturn(toto).times(1);

        mockXoNativeService.xoNmNextAttr(searchStrings, "m2m:searchString");
        expectLastCall().andReturn(aPoCPath2).times(1);
        mockXoNativeService.xoStringNmGetAttr(searchStrings, "m2m:searchString");
        expectLastCall().andReturn(tata).times(1);

        mockXoNativeService.xoNmNextAttr(searchStrings, "m2m:searchString");
        expectLastCall().andReturn(aPoCPath3).times(1);
        mockXoNativeService.xoStringNmGetAttr(searchStrings, "m2m:searchString");
        expectLastCall().andReturn(titi).times(1);

        mockXoNativeService.xoNmNextAttr(searchStrings, "m2m:searchString");
        expectLastCall().andReturn(0).times(1);

        // free
        mockXoNativeService.xoFree(searchStrings, true);
        expectLastCall().andReturn(0).times(1);

        replay(mockXoNativeService);

        try {
            XoObject xoObject = xoService.newXmlXoObject("m2m:searchStrings");
            List searchString = xoObject.getStringListAttribute("m2m:searchString");
            searchString.add(toto);
            searchString.add(tata);
            searchString.add(titi);

            String[] expected = new String[] { "toto", "tata", "titi" };

            int i = 0;
            Iterator it = searchString.iterator();
            while (it.hasNext()) {
                String next = (String) it.next();
                assertEquals(expected[i], next);
                ++i;
            }

            xoObject.free(true);
        } catch (XoException e) {
            fail();
        }

        verify(mockXoNativeService);
    }

    public void testIterator2() {

        NiXoService mockXoNativeService = EasyMock.createMock(NiXoService.class);
        XoService xoService = new XoServiceImpl(mockXoNativeService);
        long aPoCPaths = 1;
        long aPoCPath1 = 2;
        long aPoCPath2 = 3;
        long aPoCPath3 = 4;

        // newXmlXoObject on "m2m:aPoCPaths"
        mockXoNativeService.xoNmNew("m2m:aPoCPaths");
        expectLastCall().andReturn(aPoCPaths).times(1);

        // newXmlXoObject*3 on "m2m:aPoCPath"
        mockXoNativeService.xoNmNew("m2m:aPoCPath");
        expectLastCall().andReturn(aPoCPath1).times(1);
        mockXoNativeService.xoNmNew("m2m:aPoCPath");
        expectLastCall().andReturn(aPoCPath2).times(1);
        mockXoNativeService.xoNmNew("m2m:aPoCPath");
        expectLastCall().andReturn(aPoCPath3).times(1);

        // add*3
        mockXoNativeService.xoObjectNmAddInAttr(aPoCPaths, "m2m:aPoCPath", aPoCPath1);
        expectLastCall().andReturn(0).times(1);
        mockXoNativeService.xoObjectNmAddInAttr(aPoCPaths, "m2m:aPoCPath", aPoCPath2);
        expectLastCall().andReturn(0).times(1);
        mockXoNativeService.xoObjectNmAddInAttr(aPoCPaths, "m2m:aPoCPath", aPoCPath3);
        expectLastCall().andReturn(0).times(1);

        // iterator(), hasNext(), and next()
        mockXoNativeService.xoNmFirstAttr(aPoCPaths, "m2m:aPoCPath");
        expectLastCall().andReturn(aPoCPath1).times(1);
        mockXoNativeService.xoObjectNmGetAttr(aPoCPaths, "m2m:aPoCPath");
        expectLastCall().andReturn(aPoCPath1).times(1);

        mockXoNativeService.xoNmNextAttr(aPoCPaths, "m2m:aPoCPath");
        expectLastCall().andReturn(aPoCPath2).times(1);
        mockXoNativeService.xoObjectNmGetAttr(aPoCPaths, "m2m:aPoCPath");
        expectLastCall().andReturn(aPoCPath2).times(1);

        mockXoNativeService.xoNmNextAttr(aPoCPaths, "m2m:aPoCPath");
        expectLastCall().andReturn(aPoCPath3).times(1);
        mockXoNativeService.xoObjectNmGetAttr(aPoCPaths, "m2m:aPoCPath");
        expectLastCall().andReturn(aPoCPath3).times(1);

        mockXoNativeService.xoNmNextAttr(aPoCPaths, "m2m:aPoCPath");
        expectLastCall().andReturn(0).times(1);

        // free
        mockXoNativeService.xoFree(aPoCPaths, true);
        expectLastCall().andReturn(0).times(1);

        replay(mockXoNativeService);

        try {
            XoObject xoObject = xoService.newXmlXoObject("m2m:aPoCPaths");
            List apocPaths = xoObject.getXoObjectListAttribute("m2m:aPoCPath");
            XoObject apocPathElem1 = xoService.newXmlXoObject("m2m:aPoCPath");
            XoObject apocPathElem2 = xoService.newXmlXoObject("m2m:aPoCPath");
            XoObject apocPathElem3 = xoService.newXmlXoObject("m2m:aPoCPath");
            apocPaths.add(apocPathElem1);
            apocPaths.add(apocPathElem2);
            apocPaths.add(apocPathElem3);

            XoObject[] expected = new XoObject[] { apocPathElem1, apocPathElem2, apocPathElem3 };

            int i = 0;
            Iterator it = apocPaths.iterator();
            while (it.hasNext()) {
                XoObject next = (XoObject) it.next();
                assertEquals(expected[i], next);
                ++i;
            }

            xoObject.free(true);
        } catch (XoException e) {
            fail();
        }

        verify(mockXoNativeService);
    }
}
