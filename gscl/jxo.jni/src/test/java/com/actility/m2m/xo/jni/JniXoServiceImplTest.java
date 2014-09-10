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

package com.actility.m2m.xo.jni;

import java.io.BufferedReader;
import java.io.FileInputStream;
import java.io.IOException;
import java.io.InputStreamReader;

import junit.framework.TestCase;

import org.custommonkey.xmlunit.Diff;
import org.custommonkey.xmlunit.XMLUnit;
import org.xml.sax.SAXException;

import com.actility.m2m.xo.XoException;

public class JniXoServiceImplTest extends TestCase {

    private JniXoServiceImpl xoNativeService;

    @Override
    public void setUp() {
        xoNativeService = new JniXoServiceImpl();
        JniXo.start(getClass().getClassLoader().getResource("namespace.xns").getPath(), getClass().getClassLoader()
                .getResource("m2m.xor").getPath(), getClass().getClassLoader().getResource("m2m.xdi").getPath());
    }

    /**
     * Reads the given file and extract its content to a string.
     * 
     * @param filename The filename to read
     * @param encoding The encoding of the file content
     * @return the string extracted from the given file
     * @throws IOException In case an IO error occurs while reading to file or the content cannot be decoded with the given
     *             encoding
     */
    public static String readFromFile(String filename, String encoding) throws IOException {
        StringBuffer buffer = new StringBuffer(1024);
        BufferedReader reader = new BufferedReader(new InputStreamReader(new FileInputStream(filename), encoding));
        try {
            char[] buf = new char[1024];
            int numRead = 0;
            while ((numRead = reader.read(buf)) != -1) {
                buffer.append(buf, 0, numRead);
            }
        } finally {
            reader.close();
        }
        return buffer.toString();
    }

    public void testXoReadXmlMem() {
        try {
            String containerXml = readFromFile(getClass().getClassLoader().getResource("container.xml").getPath(), "UTF-8");
            long handle = xoNativeService.xoReadXmlMem(containerXml.getBytes(), false);
            assertTrue(handle != 0L);
            xoNativeService.xoFree(handle, true);
        } catch (IOException e) {
            fail();
        } catch (XoException e) {
            fail();
        }
    }

    public void testXoReadXmlMem2() {
        try {
            long handle = xoNativeService.xoReadXmlMem("vilcqgnfqlriughot".getBytes(), false);
            assertTrue(handle != 0L);
            fail();
        } catch (XoException e) {
            // OK
        }
    }

    public void testXoLoadMem() {
        try {
            String containerXml = readFromFile(getClass().getClassLoader().getResource("container.xml").getPath(), "UTF-8");
            long xoHandle = xoNativeService.xoReadXmlMem(containerXml.getBytes(), false);

            byte[] rawBinary = xoNativeService.xoSaveMem(xoHandle);

            long xoHandle2 = xoNativeService.xoLoadMem(rawBinary);
            assertTrue(xoHandle2 != 0L);
            xoNativeService.xoFree(xoHandle, true);
            xoNativeService.xoFree(xoHandle2, true);
        } catch (IOException e) {
            fail();
        } catch (XoException e) {
            fail();
        }
    }

    public void testXoNmNew() {
        long xoHandle = xoNativeService.xoNmNew("m2m:container");
        assertTrue(xoHandle != 0L);
        if (0 != xoHandle) {
            xoNativeService.xoFree(xoHandle, true);
        }
    }

    public void testXoReadPartialXmlMem() {
        try {
            String expirationXml = readFromFile(getClass().getClassLoader().getResource("expiration.xml").getPath(), "UTF-8");
            long xoHandle = xoNativeService.xoReadPartialXmlMem(expirationXml.getBytes(), "m2m:container");
            assertTrue(xoHandle != 0L);
            assertNotNull(xoNativeService.xoStringNmGetAttr(xoHandle, "m2m:expirationTime"));
            xoNativeService.xoFree(xoHandle, true);
        } catch (IOException e) {
            fail();
        } catch (XoException e) {
            fail();
        }
    }

    public void testXoReadPartialXmlMem2() {
        try {
            String longPollingChannelDataXml = readFromFile(
                    getClass().getClassLoader().getResource("longPollingChannelData.xml").getPath(), "UTF-8");
            long xoHandle = xoNativeService
                    .xoReadPartialXmlMem(longPollingChannelDataXml.getBytes(), "m2m:notificationChannel");
            assertTrue(xoHandle != 0L);
            long channelData = xoNativeService.xoObjectNmGetAttr(xoHandle, "m2m:channelData");
            assertTrue(channelData != 0L);
            assertEquals("m2m:LongPollingChannelData", xoNativeService.xoNmType(channelData));
            assertEquals("m2m:LongPollingChannelData", xoNativeService.xoStringNmGetAttr(channelData, "xsi:type"));
            assertNotNull(xoNativeService.xoStringNmGetAttr(channelData, "m2m:longPollingURI"));
            xoNativeService.xoFree(xoHandle, true);
        } catch (IOException e) {
            fail();
        } catch (XoException e) {
            fail();
        }
    }

    public void testXoReadPartialXmlMem3() {
        try {
            String channelDataXml = readFromFile(getClass().getClassLoader().getResource("channelData.xml").getPath(), "UTF-8");
            long xoHandle = xoNativeService.xoReadPartialXmlMem(channelDataXml.getBytes(), "m2m:notificationChannel");
            assertTrue(xoHandle != 0L);
            long channelData = xoNativeService.xoObjectNmGetAttr(xoHandle, "m2m:channelData");
            assertTrue(channelData != 0L);
            assertEquals("m2m:ChannelData", xoNativeService.xoNmType(channelData));
            assertNull(xoNativeService.xoStringNmGetAttr(channelData, "xsi:type"));
            assertNull(xoNativeService.xoStringNmGetAttr(channelData, "m2m:longPollingURI"));
            xoNativeService.xoFree(xoHandle, true);
        } catch (IOException e) {
            fail();
        } catch (XoException e) {
            fail();
        }
    }

    public void testXoNmType() {
        try {
            String applicationXml = readFromFile(getClass().getClassLoader().getResource("application.xml").getPath(), "UTF-8");
            long xoObject = xoNativeService.xoReadXmlMem(applicationXml.getBytes(), false);
            assertTrue(xoObject != 0L);
            assertEquals("m2m:Application", xoNativeService.xoNmType(xoObject));
            xoNativeService.xoFree(xoObject, true);
        } catch (IOException e) {
            e.printStackTrace();
            fail(e.toString());
        } catch (XoException e) {
            e.printStackTrace();
            fail(e.toString());
        }
    }

    public void testGetXoObjectAttribute() {
        try {
            String applicationXml = readFromFile(getClass().getClassLoader().getResource("application.xml").getPath(), "UTF-8");
            long xoObject = xoNativeService.xoReadXmlMem(applicationXml.getBytes(), false);
            assertTrue(xoObject != 0L);
            long announceTo = xoNativeService.xoObjectNmGetAttr(xoObject, "m2m:announceTo");
            assertTrue(announceTo != 0L);
            assertEquals("true", xoNativeService.xoStringNmGetAttr(announceTo, "m2m:activated"));
            xoNativeService.xoFree(xoObject, true);
        } catch (IOException e) {
            e.printStackTrace();
            fail(e.toString());
        } catch (XoException e) {
            e.printStackTrace();
            fail(e.toString());
        }
    }

    public void testGetStringAttribute1() {
        try {
            String applicationXml = readFromFile(getClass().getClassLoader().getResource("application.xml").getPath(), "UTF-8");
            long xoObject = xoNativeService.xoReadXmlMem(applicationXml.getBytes(), false);
            assertTrue(xoObject != 0L);
            assertEquals("/aPoC", xoNativeService.xoStringNmGetAttr(xoObject, "m2m:aPoC"));
            xoNativeService.xoFree(xoObject, true);
        } catch (IOException e) {
            e.printStackTrace();
            fail(e.toString());
        } catch (XoException e) {
            e.printStackTrace();
            fail(e.toString());
        }
    }

    // TODO wait fix from #0000979
    // public void testGetStringAttribute2() {
    // try {
    // String contentXml = readFromFile(getClass().getClassLoader().getResource("contentInstance.xml").getPath(), "UTF-8");
    // long xoObject = xoNativeService.xoReadXmlMem(contentXml.getBytes(), false);
    // assertTrue(xoObject != 0L);
    // long contentObject = xoNativeService.xoObjectNmGetAttr(xoObject, "m2m:content");
    // assertEquals("text/plain", xoNativeService.xoStringNmGetAttr(contentObject, "xmime:contentType"));
    // xoNativeService.xoFree(xoObject, true);
    // } catch (IOException e) {
    // e.printStackTrace();
    // fail(e.toString());
    // } catch (XoException e) {
    // e.printStackTrace();
    // fail(e.toString());
    // }
    // }

    // useless now that XoList cannot be tested here (?)
    public void testGetXoObjectListAttribute() {
        try {
            String applicationXml = readFromFile(getClass().getClassLoader().getResource("application.xml").getPath(), "UTF-8");
            long xoObject = xoNativeService.xoReadXmlMem(applicationXml.getBytes(), false);
            assertTrue(xoObject != 0L);
            long apocPaths = xoNativeService.xoObjectNmGetAttr(xoObject, "m2m:aPoCPaths");
            assertTrue(apocPaths != 0L);
            xoNativeService.xoFree(xoObject, true);
        } catch (IOException e) {
            e.printStackTrace();
            fail(e.toString());
        } catch (XoException e) {
            e.printStackTrace();
            fail(e.toString());
        }
    }

    // useless now that XoList cannot be tested here (?)
    public void testGetStringListAttribute() {
        try {
            String applicationXml = readFromFile(getClass().getClassLoader().getResource("application.xml").getPath(), "UTF-8");
            long xoObject = xoNativeService.xoReadXmlMem(applicationXml.getBytes(), false);
            assertTrue(xoObject != 0L);
            long searchStrings = xoNativeService.xoObjectNmGetAttr(xoObject, "m2m:searchStrings");
            assertTrue(searchStrings != 0L);
            xoNativeService.xoFree(xoObject, true);
        } catch (IOException e) {
            e.printStackTrace();
            fail(e.toString());
        } catch (XoException e) {
            e.printStackTrace();
            fail(e.toString());
        }
    }

    public void testSetXoObjectAttribute() {
        long xoObject = xoNativeService.xoNmNew("m2m:application");
        assertTrue(xoObject != 0L);
        long announceToOrig = xoNativeService.xoNmNew("m2m:announceTo");
        xoNativeService.xoObjectNmSetAttr(xoObject, "m2m:announceTo", announceToOrig);
        long announceToSet = xoNativeService.xoObjectNmGetAttr(xoObject, "m2m:announceTo");
        assertTrue(announceToSet != 0L);
        assertEquals(announceToOrig, announceToSet);
        xoNativeService.xoFree(xoObject, true);
    }

    public void testSetStringAttribute() {
        long xoObject = xoNativeService.xoNmNew("m2m:application");
        assertTrue(xoObject != 0L);
        xoNativeService.xoStringNmSetAttr(xoObject, "m2m:aPoC", "/aPoC");
        String aPoC = xoNativeService.xoStringNmGetAttr(xoObject, "m2m:aPoC");
        assertNotNull(aPoC);
        assertEquals("/aPoC", aPoC);
        xoNativeService.xoFree(xoObject, true);
    }

    public void testSaveXml() {
        try {
            String applicationXml = readFromFile(getClass().getClassLoader().getResource("application.xml").getPath(), "UTF-8");
            long xoObject = xoNativeService.xoReadXmlMem(applicationXml.getBytes(), false);
            assertTrue(xoObject != 0L);
            byte[] rawXml = xoNativeService.xoWriteXmlMem(xoObject, "", false);

            XMLUnit.setIgnoreWhitespace(true);
            Diff diff = new Diff(applicationXml, new String(rawXml));
            assertTrue(diff.identical());

            xoNativeService.xoFree(xoObject, true);
        } catch (IOException e) {
            e.printStackTrace();
            fail(e.toString());
        } catch (XoException e) {
            e.printStackTrace();
            fail(e.toString());
        } catch (SAXException e) {
            e.printStackTrace();
            fail(e.toString());
        }
    }

    public void testSavePartialXml1() {
        try {
            String applicationXml = readFromFile(getClass().getClassLoader().getResource("application.xml").getPath(), "UTF-8");
            String apocPathsXml = readFromFile(getClass().getClassLoader().getResource("apocpaths.xml").getPath(), "UTF-8");
            long xoObject = xoNativeService.xoReadXmlMem(applicationXml.getBytes(), false);
            assertTrue(xoObject != 0L);
            byte[] rawXml = xoNativeService.xoWritePartialXmlMem(xoObject, "m2m:aPoCPaths", "");

            assertNotNull(rawXml);
            XMLUnit.setIgnoreWhitespace(true);
            Diff diff = new Diff(apocPathsXml, new String(rawXml));
            assertTrue(diff.identical());

            xoNativeService.xoFree(xoObject, true);
        } catch (IOException e) {
            e.printStackTrace();
            fail(e.toString());
        } catch (XoException e) {
            e.printStackTrace();
            fail(e.toString());
        } catch (SAXException e) {
            e.printStackTrace();
            fail(e.toString());
        }
    }

    // TODO bug with namespaces in partial addressing. All namespaces are written every time
    // public void testSavePartialXml2() {
    // try {
    // String longPollingChannelData = readFromFile(getClass().getClassLoader().getResource("longPollingChannelData.xml")
    // .getPath(), "UTF-8");
    // long xoObject = xoNativeService.xoReadPartialXmlMem(longPollingChannelData.getBytes(), "m2m:notificationChannel");
    // assertTrue(xoObject != 0L);
    // long xoChannelData = xoNativeService.xoReadPartialXmlMem(longPollingChannelData.getBytes(), "m2m:channelData");
    // assertTrue(xoChannelData != 0L);
    // xoNativeService.xoUnSetNameSpace(xoChannelData, null);
    // xoNativeService.xoSetNameSpace(xoChannelData, "m2m");
    // xoNativeService.xoSetNameSpace(xoChannelData, "xsi");
    // xoNativeService.xoUnSetNameSpace(xoObject, null);
    // xoNativeService.xoSetNameSpace(xoObject, "m2m");
    // xoNativeService.xoSetNameSpace(xoObject, "xsi");
    // byte[] rawXml = xoNativeService.xoWritePartialXmlMem(xoObject, "m2m:channelData", "");
    // System.out.println(longPollingChannelData);
    // System.out.println(new String(rawXml));
    //
    // XMLUnit.setIgnoreWhitespace(true);
    // Diff diff = new Diff(longPollingChannelData, new String(rawXml));
    // System.out.println(diff);
    // assertTrue(diff.identical());
    //
    // xoNativeService.xoFree(xoObject, true);
    // } catch (IOException e) {
    // fail();
    // } catch (XoException e) {
    // fail();
    // } catch (SAXException e) {
    // fail();
    // e.printStackTrace();
    // }
    // }

    public void testSavePartialXml3() {
        try {
            String channelData = readFromFile(getClass().getClassLoader().getResource("channelData.xml").getPath(), "UTF-8");
            long xoObject = xoNativeService.xoReadPartialXmlMem(channelData.getBytes(), "m2m:notificationChannel");
            assertTrue(xoObject != 0L);
            byte[] rawXml = xoNativeService.xoWritePartialXmlMem(xoObject, "m2m:channelData", "");

            assertNotNull(rawXml);
            XMLUnit.setIgnoreWhitespace(true);
            Diff diff = new Diff(channelData, new String(rawXml));
            assertTrue(diff.identical());

            xoNativeService.xoFree(xoObject, true);
        } catch (IOException e) {
            e.printStackTrace();
            fail(e.toString());
        } catch (XoException e) {
            e.printStackTrace();
            fail(e.toString());
        } catch (SAXException e) {
            e.printStackTrace();
            fail(e.toString());
        }
    }

    // // TODO
    // public void testSavePartialXml4() {
    // try {
    // String applicationXml = XoServiceImplTest.readFromFile(getClass().getClassLoader().getResource("application.xml")
    // .getPath(), "UTF-8");
    // String expirationXml = XoServiceImplTest.readFromFile(getClass().getClassLoader().getResource("expiration.xml")
    // .getPath(), "UTF-8");
    // XoObject xoObject = new XoServiceImpl().readXmlXoObject(applicationXml.getBytes(), "UTF-8");
    // assertNotNull(xoObject);
    // System.out.println("save partial");
    // byte[] rawXml = xoObject.savePartialXml("m2m:expirationTime");
    // System.out.println("done");
    // System.out.println(new String(rawXml));
    //
    // XMLUnit.setIgnoreWhitespace(true);
    // Diff diff = new Diff(expirationXml, new String(rawXml));
    // System.out.println(diff);
    // assertTrue(diff.identical());
    //
    // xoObject.free(true);
    // } catch (IOException e) {
    // fail();
    // } catch (XoException e) {
    // fail();
    // } catch (SAXException e) {
    // fail();
    // e.printStackTrace();
    // }
    // }

    public void testSaveBinary() {
        try {
            String applicationXml = readFromFile(getClass().getClassLoader().getResource("container.xml").getPath(), "UTF-8");
            long xoObject = xoNativeService.xoReadXmlMem(applicationXml.getBytes(), false);
            byte[] rawBinary = xoNativeService.xoSaveMem(xoObject);

            assertNotNull(rawBinary);
            xoNativeService.xoFree(xoObject, true);
        } catch (IOException e) {
            e.printStackTrace();
            fail(e.toString());
        } catch (XoException e) {
            e.printStackTrace();
            fail(e.toString());
        }
    }

    // /////////////////////////////////////////////////////////////:
    // list manipulations

    public void testAdd1() {
        long searchStrings;
        String toto = "toto";

        // newXmlXoObject on "m2m:searchStrings"
        searchStrings = xoNativeService.xoNmNew("m2m:searchStrings");

        // add
        xoNativeService.xoStringNmAddInAttr(searchStrings, "m2m:searchString", toto);

        // get[0]
        assertEquals(toto, xoNativeService.xoStringNmGetAttr(searchStrings, "m2m:searchString[0]"));

        // free
        xoNativeService.xoFree(searchStrings, true);
    }

    public void testAdd2() {
        long aPocPaths;
        long xoHandleElt;

        // newXmlXoObject on "m2m:aPoCPaths"
        aPocPaths = xoNativeService.xoNmNew("m2m:aPoCPaths");

        // newXmlXoObject on "m2m:aPoCPath"
        xoHandleElt = xoNativeService.xoNmNew("m2m:aPoCPath");

        // add
        xoNativeService.xoObjectNmAddInAttr(aPocPaths, "m2m:aPoCPath", xoHandleElt);

        // get[0]
        assertEquals(xoHandleElt, xoNativeService.xoObjectNmGetAttr(aPocPaths, "m2m:aPoCPath[0]"));

        // free
        xoNativeService.xoFree(aPocPaths, true);
    }

    public void testClear1() {

        long searchStrings;
        String toto = "toto";
        String tata = "tata";
        String titi = "titi";

        // newXmlXoObject on "m2m:searchStrings"
        searchStrings = xoNativeService.xoNmNew("m2m:searchStrings");

        // add*3
        xoNativeService.xoStringNmAddInAttr(searchStrings, "m2m:searchString", toto);
        xoNativeService.xoStringNmAddInAttr(searchStrings, "m2m:searchString", tata);
        xoNativeService.xoStringNmAddInAttr(searchStrings, "m2m:searchString", titi);

        // clear
        xoNativeService.xoNmDropList(searchStrings, "m2m:searchString");

        // size
        assertEquals(0, xoNativeService.xoNmNbInAttr(searchStrings, "m2m:searchString"));

        // free
        xoNativeService.xoFree(searchStrings, true);
    }

    public void testClear2() {
        long aPoCPaths;
        long aPoCPath;

        // newXmlXoObject on "m2m:aPoCPaths"
        aPoCPaths = xoNativeService.xoNmNew("m2m:aPoCPaths");

        // newXmlXoObject on "m2m:aPoCPath"
        aPoCPath = xoNativeService.xoNmNew("m2m:aPoCPath");

        // add
        xoNativeService.xoObjectNmAddInAttr(aPoCPaths, "m2m:aPoCPath", aPoCPath);

        // clear
        xoNativeService.xoNmDropList(aPoCPaths, "m2m:aPoCPath");

        // size
        assertEquals(0, xoNativeService.xoNmNbInAttr(aPoCPaths, "m2m:aPoCPath"));

        // free
        xoNativeService.xoFree(aPoCPaths, true);
    }

    public void testContains1() {
        long searchStrings;

        // newXmlXoObject on "m2m:searchStrings"
        searchStrings = xoNativeService.xoNmNew("m2m:searchStrings");

        // add*3
        xoNativeService.xoStringNmAddInAttr(searchStrings, "m2m:searchString", "toto");
        xoNativeService.xoStringNmAddInAttr(searchStrings, "m2m:searchString", "tata");
        xoNativeService.xoStringNmAddInAttr(searchStrings, "m2m:searchString", "titi");

        // contains("toto")
        assertTrue(xoNativeService.xoStringNmIndexOf(searchStrings, "m2m:searchString", "toto") >= 0);

        // free
        xoNativeService.xoFree(searchStrings, true);

    }

    public void testContains2() {
        long searchStrings;

        // newXmlXoObject on "m2m:searchStrings"
        searchStrings = xoNativeService.xoNmNew("m2m:searchStrings");

        // add*3
        xoNativeService.xoStringNmAddInAttr(searchStrings, "m2m:searchString", "toto");
        xoNativeService.xoStringNmAddInAttr(searchStrings, "m2m:searchString", "tata");
        xoNativeService.xoStringNmAddInAttr(searchStrings, "m2m:searchString", "titi");

        // ! contains("papa")
        assertFalse(xoNativeService.xoStringNmIndexOf(searchStrings, "m2m:searchString", "papa") >= 0);

        // free
        xoNativeService.xoFree(searchStrings, true);

    }

    public void testContains3() {
        long aPoCPaths;
        long aPoCPath;

        // newXmlXoObject on "m2m:aPoCPaths"
        aPoCPaths = xoNativeService.xoNmNew("m2m:aPoCPaths");

        // newXmlXoObject on "m2m:aPoCPath"
        aPoCPath = xoNativeService.xoNmNew("m2m:aPoCPath");

        // add
        xoNativeService.xoObjectNmAddInAttr(aPoCPaths, "m2m:aPoCPath", aPoCPath);

        // contains(aPocPath)
        assertTrue(xoNativeService.xoObjectNmIndexOf(aPoCPaths, "m2m:aPoCPath", aPoCPath) >= 0);

        // free
        xoNativeService.xoFree(aPoCPaths, true);
    }

    public void testContains4() {
        long aPoCPaths;
        long aPoCPath;

        // newXmlXoObject on "m2m:aPoCPaths"
        aPoCPaths = xoNativeService.xoNmNew("m2m:aPoCPaths");

        // newXmlXoObject on "m2m:aPoCPath"
        aPoCPath = xoNativeService.xoNmNew("m2m:aPoCPath");

        // add
        xoNativeService.xoObjectNmAddInAttr(aPoCPaths, "m2m:aPoCPath", aPoCPath);

        // ! contains(xoHandle=10L)
        assertFalse(xoNativeService.xoObjectNmIndexOf(aPoCPaths, "m2m:aPoCPath", 10L) >= 0);

        // free
        xoNativeService.xoFree(aPoCPaths, true);
    }

    public void testGet1() {
        long searchStrings;

        // newXmlXoObject on "m2m:searchStrings"
        searchStrings = xoNativeService.xoNmNew("m2m:searchStrings");

        // add*3
        xoNativeService.xoStringNmAddInAttr(searchStrings, "m2m:searchString", "toto");
        xoNativeService.xoStringNmAddInAttr(searchStrings, "m2m:searchString", "tata");
        xoNativeService.xoStringNmAddInAttr(searchStrings, "m2m:searchString", "titi");

        // get[0]
        assertEquals("toto", xoNativeService.xoStringNmGetAttr(searchStrings, "m2m:searchString[0]"));
        // get[1]
        assertEquals("tata", xoNativeService.xoStringNmGetAttr(searchStrings, "m2m:searchString[1]"));

        // free
        xoNativeService.xoFree(searchStrings, true);
    }

    public void testGet2() {
        long searchStrings;

        // newXmlXoObject on "m2m:searchStrings"
        searchStrings = xoNativeService.xoNmNew("m2m:searchStrings");

        // add*3
        xoNativeService.xoStringNmAddInAttr(searchStrings, "m2m:searchString", "toto");
        xoNativeService.xoStringNmAddInAttr(searchStrings, "m2m:searchString", "tata");
        xoNativeService.xoStringNmAddInAttr(searchStrings, "m2m:searchString", "titi");

        // get[10]
        assertNull(xoNativeService.xoStringNmGetAttr(searchStrings, "m2m:searchString[10]"));

        // free
        xoNativeService.xoFree(searchStrings, true);
    }

    public void testGet3() {
        long aPoCPaths;
        long aPoCPath;

        // newXmlXoObject on "m2m:aPoCPaths"
        aPoCPaths = xoNativeService.xoNmNew("m2m:aPoCPaths");

        // newXmlXoObject on "m2m:aPoCPath"
        aPoCPath = xoNativeService.xoNmNew("m2m:aPoCPath");

        // add
        xoNativeService.xoObjectNmAddInAttr(aPoCPaths, "m2m:aPoCPath", aPoCPath);

        // get(0)
        assertEquals(aPoCPath, xoNativeService.xoObjectNmGetAttr(aPoCPaths, "m2m:aPoCPath[0]"));

        // free
        xoNativeService.xoFree(aPoCPaths, true);
    }

    public void testGet4() {
        long aPoCPaths;
        long aPoCPath;

        // newXmlXoObject on "m2m:aPoCPaths"
        aPoCPaths = xoNativeService.xoNmNew("m2m:aPoCPaths");

        // newXmlXoObject on "m2m:aPoCPath"
        aPoCPath = xoNativeService.xoNmNew("m2m:aPoCPath");

        // add
        xoNativeService.xoObjectNmAddInAttr(aPoCPaths, "m2m:aPoCPath", aPoCPath);

        // get(10)
        assertEquals(0, xoNativeService.xoObjectNmGetAttr(aPoCPaths, "m2m:aPoCPath[10]"));

        // free
        xoNativeService.xoFree(aPoCPaths, true);
    }

    public void testIndexOf1() {
        long searchStrings;

        // newXmlXoObject on "m2m:searchStrings"
        searchStrings = xoNativeService.xoNmNew("m2m:searchStrings");

        // add*3
        xoNativeService.xoStringNmAddInAttr(searchStrings, "m2m:searchString", "toto");
        xoNativeService.xoStringNmAddInAttr(searchStrings, "m2m:searchString", "tata");
        xoNativeService.xoStringNmAddInAttr(searchStrings, "m2m:searchString", "titi");

        // indexOf("toto")
        assertEquals(0, xoNativeService.xoStringNmIndexOf(searchStrings, "m2m:searchString", "toto"));

        // indexOf("tata")
        assertEquals(1, xoNativeService.xoStringNmIndexOf(searchStrings, "m2m:searchString", "tata"));

        // free
        xoNativeService.xoFree(searchStrings, true);
    }

    public void testIndexOf2() {
        long searchStrings;

        // newXmlXoObject on "m2m:searchStrings"
        searchStrings = xoNativeService.xoNmNew("m2m:searchStrings");

        // add*3
        xoNativeService.xoStringNmAddInAttr(searchStrings, "m2m:searchString", "toto");
        xoNativeService.xoStringNmAddInAttr(searchStrings, "m2m:searchString", "tata");
        xoNativeService.xoStringNmAddInAttr(searchStrings, "m2m:searchString", "titi");

        // indexOf("papa") not found
        assertEquals(-1, xoNativeService.xoStringNmIndexOf(searchStrings, "m2m:searchString", "papa"));

        // free
        xoNativeService.xoFree(searchStrings, true);
    }

    public void testIndexOf3() {
        long aPoCPaths;
        long aPoCPath1;
        long aPoCPath2;

        // newXmlXoObject on "m2m:aPoCPaths"
        aPoCPaths = xoNativeService.xoNmNew("m2m:aPoCPaths");

        // newXmlXoObject on "m2m:aPoCPath"
        aPoCPath1 = xoNativeService.xoNmNew("m2m:aPoCPath");
        aPoCPath2 = xoNativeService.xoNmNew("m2m:aPoCPath");

        assertFalse(aPoCPath1 == aPoCPath2);

        // add
        xoNativeService.xoObjectNmAddInAttr(aPoCPaths, "m2m:aPoCPath", aPoCPath1);
        xoNativeService.xoObjectNmAddInAttr(aPoCPaths, "m2m:aPoCPath", aPoCPath2);

        // indexOf(aPoCPath1)
        assertEquals(0, xoNativeService.xoObjectNmIndexOf(aPoCPaths, "m2m:aPoCPath", aPoCPath1));
        // indexOf(aPoCPath2)
        assertEquals(1, xoNativeService.xoObjectNmIndexOf(aPoCPaths, "m2m:aPoCPath", aPoCPath2));

        // free
        xoNativeService.xoFree(aPoCPaths, true);
    }

    public void testIndexOf4() {
        long aPoCPaths;
        long aPoCPath1;
        long aPoCPath2;

        // newXmlXoObject on "m2m:aPoCPaths"
        aPoCPaths = xoNativeService.xoNmNew("m2m:aPoCPaths");

        // newXmlXoObject on "m2m:aPoCPath"
        aPoCPath1 = xoNativeService.xoNmNew("m2m:aPoCPath");
        aPoCPath2 = xoNativeService.xoNmNew("m2m:aPoCPath");

        // add
        xoNativeService.xoObjectNmAddInAttr(aPoCPaths, "m2m:aPoCPath", aPoCPath1);
        xoNativeService.xoObjectNmAddInAttr(aPoCPaths, "m2m:aPoCPath", aPoCPath2);

        // indexOf(10), where 10 is an arbritrary choosen value with no chance that the xo handle 10 is in the list.
        assertEquals(-1, xoNativeService.xoObjectNmIndexOf(aPoCPaths, "m2m:aPoCPath", 10));

        // free
        xoNativeService.xoFree(aPoCPaths, true);
    }

    public void testIsEmpty1() {
        long searchStrings;

        // newXmlXoObject on "m2m:searchStrings"
        searchStrings = xoNativeService.xoNmNew("m2m:searchStrings");

        // empty
        assertEquals(0, xoNativeService.xoNmNbInAttr(searchStrings, "m2m:searchString"));

        // free
        xoNativeService.xoFree(searchStrings, true);
    }

    public void testIsEmpty2() {
        long searchStrings;

        // newXmlXoObject on "m2m:searchStrings"
        searchStrings = xoNativeService.xoNmNew("m2m:searchStrings");

        // add*2
        xoNativeService.xoStringNmAddInAttr(searchStrings, "m2m:searchString", "toto");
        xoNativeService.xoStringNmAddInAttr(searchStrings, "m2m:searchString", "titi");

        // not empty
        assertEquals(2, xoNativeService.xoNmNbInAttr(searchStrings, "m2m:searchString"));

        // free
        xoNativeService.xoFree(searchStrings, true);
    }

    public void testIsEmpty3() {
        long aPoCPaths;

        // newXmlXoObject on "m2m:aPoCPaths"
        aPoCPaths = xoNativeService.xoNmNew("m2m:aPoCPaths");

        // indexOf(10), where 10 is an arbitrary chosen value with no chance that the xo handle 10 is in the list.
        assertEquals(0, xoNativeService.xoNmNbInAttr(aPoCPaths, "m2m:aPoCPath"));

        // free
        xoNativeService.xoFree(aPoCPaths, true);
    }

    public void testIsEmpty4() {
        long aPoCPaths;
        long aPoCPath1;
        long aPoCPath2;

        // newXmlXoObject on "m2m:aPoCPaths"
        aPoCPaths = xoNativeService.xoNmNew("m2m:aPoCPaths");

        // newXmlXoObject on "m2m:aPoCPath"
        aPoCPath1 = xoNativeService.xoNmNew("m2m:aPoCPath");
        aPoCPath2 = xoNativeService.xoNmNew("m2m:aPoCPath");

        // add
        xoNativeService.xoObjectNmAddInAttr(aPoCPaths, "m2m:aPoCPath", aPoCPath1);
        xoNativeService.xoObjectNmAddInAttr(aPoCPaths, "m2m:aPoCPath", aPoCPath2);

        // indexOf(10), where 10 is an arbitrary chosen value with no chance that the xo handle 10 is in the list.
        assertEquals(2, xoNativeService.xoNmNbInAttr(aPoCPaths, "m2m:aPoCPath"));

        // free
        xoNativeService.xoFree(aPoCPaths, true);
    }

    public void testRemove1() {
        long searchStrings;

        // newXmlXoObject on "m2m:searchStrings"
        searchStrings = xoNativeService.xoNmNew("m2m:searchStrings");

        // add*2
        xoNativeService.xoStringNmAddInAttr(searchStrings, "m2m:searchString", "toto");
        xoNativeService.xoStringNmAddInAttr(searchStrings, "m2m:searchString", "titi");

        // not empty
        assertEquals(2, xoNativeService.xoNmNbInAttr(searchStrings, "m2m:searchString"));

        xoNativeService.xoNmSupInAttr(searchStrings, "m2m:searchString[0]");

        // not empty
        assertEquals(1, xoNativeService.xoNmNbInAttr(searchStrings, "m2m:searchString"));
        assertEquals("titi", xoNativeService.xoStringNmGetAttr(searchStrings, "m2m:searchString[0]"));

        // free
        xoNativeService.xoFree(searchStrings, true);
    }

    public void testRemove2() {
        long searchStrings;

        // newXmlXoObject on "m2m:searchStrings"
        searchStrings = xoNativeService.xoNmNew("m2m:searchStrings");

        // add*2
        xoNativeService.xoStringNmAddInAttr(searchStrings, "m2m:searchString", "toto");
        xoNativeService.xoStringNmAddInAttr(searchStrings, "m2m:searchString", "titi");

        // not empty
        assertEquals(2, xoNativeService.xoNmNbInAttr(searchStrings, "m2m:searchString"));

        xoNativeService.xoNmSupInAttr(searchStrings, "m2m:searchString[2]");

        // not empty
        assertEquals(2, xoNativeService.xoNmNbInAttr(searchStrings, "m2m:searchString"));
        assertEquals("toto", xoNativeService.xoStringNmGetAttr(searchStrings, "m2m:searchString[0]"));
        assertEquals("titi", xoNativeService.xoStringNmGetAttr(searchStrings, "m2m:searchString[1]"));

        // free
        xoNativeService.xoFree(searchStrings, true);
    }

    public void testRemove3() {
        long searchStrings;

        // newXmlXoObject on "m2m:searchStrings"
        searchStrings = xoNativeService.xoNmNew("m2m:searchStrings");

        // add*2
        xoNativeService.xoStringNmAddInAttr(searchStrings, "m2m:searchString", "toto");
        xoNativeService.xoStringNmAddInAttr(searchStrings, "m2m:searchString", "titi");

        // not empty
        assertEquals(2, xoNativeService.xoNmNbInAttr(searchStrings, "m2m:searchString"));

        xoNativeService.xoNmSupInAttr(searchStrings, "m2m:searchString[0]");

        // not empty
        assertEquals(1, xoNativeService.xoNmNbInAttr(searchStrings, "m2m:searchString"));
        assertEquals("titi", xoNativeService.xoStringNmGetAttr(searchStrings, "m2m:searchString[0]"));

        xoNativeService.xoNmSupInAttr(searchStrings, "m2m:searchString[0]");

        // empty
        assertEquals(0, xoNativeService.xoNmNbInAttr(searchStrings, "m2m:searchString"));

        // free
        xoNativeService.xoFree(searchStrings, true);
    }

    public void testRemove4() {
        long aPoCPaths;
        long aPoCPath1;
        long aPoCPath2;

        // newXmlXoObject on "m2m:aPoCPaths"
        aPoCPaths = xoNativeService.xoNmNew("m2m:aPoCPaths");

        // add*2
        aPoCPath1 = xoNativeService.xoNmNew("m2m:aPoCPath");
        xoNativeService.xoObjectNmAddInAttr(aPoCPaths, "m2m:aPoCPath", aPoCPath1);
        aPoCPath2 = xoNativeService.xoNmNew("m2m:aPoCPath");
        xoNativeService.xoObjectNmAddInAttr(aPoCPaths, "m2m:aPoCPath", aPoCPath2);

        // not empty
        assertEquals(2, xoNativeService.xoNmNbInAttr(aPoCPaths, "m2m:aPoCPath"));

        xoNativeService.xoNmSupInAttr(aPoCPaths, "m2m:aPoCPath[0]");

        // not empty
        assertEquals(1, xoNativeService.xoNmNbInAttr(aPoCPaths, "m2m:aPoCPath"));
        assertEquals(aPoCPath2, xoNativeService.xoObjectNmGetAttr(aPoCPaths, "m2m:aPoCPath[0]"));

        // free
        xoNativeService.xoFree(aPoCPaths, true);
    }

    public void testRemove5() {
        long aPoCPaths;
        long aPoCPath1;
        long aPoCPath2;

        // newXmlXoObject on "m2m:aPoCPaths"
        aPoCPaths = xoNativeService.xoNmNew("m2m:aPoCPaths");

        // add*2
        aPoCPath1 = xoNativeService.xoNmNew("m2m:aPoCPath");
        xoNativeService.xoObjectNmAddInAttr(aPoCPaths, "m2m:aPoCPath", aPoCPath1);
        aPoCPath2 = xoNativeService.xoNmNew("m2m:aPoCPath");
        xoNativeService.xoObjectNmAddInAttr(aPoCPaths, "m2m:aPoCPath", aPoCPath2);

        // not empty
        assertEquals(2, xoNativeService.xoNmNbInAttr(aPoCPaths, "m2m:aPoCPath"));

        xoNativeService.xoNmSupInAttr(aPoCPaths, "m2m:aPoCPath[2]");

        // not empty
        assertEquals(2, xoNativeService.xoNmNbInAttr(aPoCPaths, "m2m:aPoCPath"));
        assertEquals(aPoCPath1, xoNativeService.xoObjectNmGetAttr(aPoCPaths, "m2m:aPoCPath[0]"));
        assertEquals(aPoCPath2, xoNativeService.xoObjectNmGetAttr(aPoCPaths, "m2m:aPoCPath[1]"));

        // free
        xoNativeService.xoFree(aPoCPaths, true);
    }

    public void testRemove6() {
        long aPoCPaths;
        long aPoCPath1;
        long aPoCPath2;

        // newXmlXoObject on "m2m:aPoCPaths"
        aPoCPaths = xoNativeService.xoNmNew("m2m:aPoCPaths");

        // add*2
        aPoCPath1 = xoNativeService.xoNmNew("m2m:aPoCPath");
        xoNativeService.xoObjectNmAddInAttr(aPoCPaths, "m2m:aPoCPath", aPoCPath1);
        aPoCPath2 = xoNativeService.xoNmNew("m2m:aPoCPath");
        xoNativeService.xoObjectNmAddInAttr(aPoCPaths, "m2m:aPoCPath", aPoCPath2);

        // not empty
        assertEquals(2, xoNativeService.xoNmNbInAttr(aPoCPaths, "m2m:aPoCPath"));

        xoNativeService.xoNmSupInAttr(aPoCPaths, "m2m:aPoCPath[0]");

        // not empty
        assertEquals(1, xoNativeService.xoNmNbInAttr(aPoCPaths, "m2m:aPoCPath"));
        assertEquals(aPoCPath2, xoNativeService.xoObjectNmGetAttr(aPoCPaths, "m2m:aPoCPath[0]"));

        xoNativeService.xoNmSupInAttr(aPoCPaths, "m2m:aPoCPath[0]");

        // empty
        assertEquals(0, xoNativeService.xoNmNbInAttr(aPoCPaths, "m2m:aPoCPath"));

        // free
        xoNativeService.xoFree(aPoCPaths, true);
    }

    public void testIterator1() {

        long searchStrings = 1;
        String toto = "toto";
        String tata = "tata";
        String titi = "titi";

        // newXmlXoObject on "m2m:searchStrings"
        searchStrings = xoNativeService.xoNmNew("m2m:searchStrings");

        // add*3
        xoNativeService.xoStringNmAddInAttr(searchStrings, "m2m:searchString", toto);
        xoNativeService.xoStringNmAddInAttr(searchStrings, "m2m:searchString", tata);
        xoNativeService.xoStringNmAddInAttr(searchStrings, "m2m:searchString", titi);

        // iterator(), hasNext(), and next()
        assertTrue(xoNativeService.xoNmFirstAttr(searchStrings, "m2m:searchString") != 0L);
        assertEquals(toto, xoNativeService.xoStringNmGetAttr(searchStrings, "m2m:searchString"));

        assertTrue(xoNativeService.xoNmNextAttr(searchStrings, "m2m:searchString") != 0L);
        assertEquals(tata, xoNativeService.xoStringNmGetAttr(searchStrings, "m2m:searchString"));

        assertTrue(xoNativeService.xoNmNextAttr(searchStrings, "m2m:searchString") != 0L);
        assertEquals(titi, xoNativeService.xoStringNmGetAttr(searchStrings, "m2m:searchString"));

        assertEquals(0, xoNativeService.xoNmNextAttr(searchStrings, "m2m:searchString"));

        // free
        xoNativeService.xoFree(searchStrings, true);
    }

    public void testIterator2() {

        long aPoCPaths;
        long aPoCPath1;
        long aPoCPath2;
        long aPoCPath3;

        // newXmlXoObject on "m2m:aPoCPaths"
        aPoCPaths = xoNativeService.xoNmNew("m2m:aPoCPaths");
        assertTrue(aPoCPaths != 0L);

        // newXmlXoObject*3 on "m2m:aPoCPath"
        aPoCPath1 = xoNativeService.xoNmNew("m2m:aPoCPath");
        assertTrue(aPoCPath1 != 0L);
        aPoCPath2 = xoNativeService.xoNmNew("m2m:aPoCPath");
        assertTrue(aPoCPath2 != 0L);
        aPoCPath3 = xoNativeService.xoNmNew("m2m:aPoCPath");
        assertTrue(aPoCPath3 != 0L);

        // add*3
        xoNativeService.xoObjectNmAddInAttr(aPoCPaths, "m2m:aPoCPath", aPoCPath1);
        xoNativeService.xoObjectNmAddInAttr(aPoCPaths, "m2m:aPoCPath", aPoCPath2);
        xoNativeService.xoObjectNmAddInAttr(aPoCPaths, "m2m:aPoCPath", aPoCPath3);

        // iterator(), hasNext(), and next()
        assertTrue(xoNativeService.xoNmFirstAttr(aPoCPaths, "m2m:aPoCPath") != 0L);
        assertEquals(aPoCPath1, xoNativeService.xoObjectNmGetAttr(aPoCPaths, "m2m:aPoCPath"));

        assertTrue(xoNativeService.xoNmNextAttr(aPoCPaths, "m2m:aPoCPath") != 0L);
        assertEquals(aPoCPath2, xoNativeService.xoObjectNmGetAttr(aPoCPaths, "m2m:aPoCPath"));

        assertTrue(xoNativeService.xoNmNextAttr(aPoCPaths, "m2m:aPoCPath") != 0L);
        assertEquals(aPoCPath3, xoNativeService.xoObjectNmGetAttr(aPoCPaths, "m2m:aPoCPath"));

        // no more elements
        assertEquals(0, xoNativeService.xoNmNextAttr(aPoCPaths, "m2m:aPoCPath"));

        // free
        xoNativeService.xoFree(aPoCPaths, true);
    }

}
