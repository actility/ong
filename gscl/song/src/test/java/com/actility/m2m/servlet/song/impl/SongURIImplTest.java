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
 * id $Id: SongURIImplTest.java 7478 2014-02-03 16:45:25Z JReich $
 * author $Author: JReich $
 * version $Revision: 7478 $
 * lastrevision $Date: 2014-02-03 17:45:25 +0100 (Mon, 03 Feb 2014) $
 * modifiedby $LastChangedBy: JReich $
 * lastmodified $LastChangedDate: 2014-02-03 17:45:25 +0100 (Mon, 03 Feb 2014) $
 */

package com.actility.m2m.servlet.song.impl;

import java.net.URI;

import junit.framework.TestCase;

import com.actility.m2m.servlet.song.SongURI;

public class SongURIImplTest extends TestCase {

    public void testNewUri_1() {
        SongURI songUri = new SongURIImpl(URI.create("http://toto.com/test"), null, null);

        assertEquals("http", songUri.getScheme());
        assertFalse(songUri.isSecure());
        assertEquals("toto.com", songUri.getHost());
        assertEquals(-1, songUri.getPort());
        assertEquals("/test", songUri.getPath());
        assertFalse(songUri.getQueryParameters().hasNext());
        assertEquals("http://toto.com/test", songUri.absoluteURI());
    }

    public void testNewUri_2() {
        SongURI songUri = new SongURIImpl(URI.create("http://toto.com/test/"), null, null);

        assertEquals("http", songUri.getScheme());
        assertFalse(songUri.isSecure());
        assertEquals("toto.com", songUri.getHost());
        assertEquals(-1, songUri.getPort());
        assertEquals("/test/", songUri.getPath());
        assertFalse(songUri.getQueryParameters().hasNext());
        assertEquals("http://toto.com/test/", songUri.absoluteURI());
    }

    public void testNewUri_3() {
        SongURI songUri = new SongURIImpl(URI.create("http://toto.com"), null, null);

        assertEquals("http", songUri.getScheme());
        assertFalse(songUri.isSecure());
        assertEquals("toto.com", songUri.getHost());
        assertEquals(-1, songUri.getPort());
        assertEquals("", songUri.getPath());
        assertFalse(songUri.getQueryParameters().hasNext());
        assertEquals("http://toto.com", songUri.absoluteURI());
    }

    public void testNewUri_4() {
        SongURI songUri = new SongURIImpl(URI.create("http://toto.com/"), null, null);

        assertEquals("http", songUri.getScheme());
        assertFalse(songUri.isSecure());
        assertEquals("toto.com", songUri.getHost());
        assertEquals(-1, songUri.getPort());
        assertEquals("/", songUri.getPath());
        assertFalse(songUri.getQueryParameters().hasNext());
        assertEquals("http://toto.com/", songUri.absoluteURI());
    }

    public void testNewUri_5() {
        SongURI songUri = new SongURIImpl(URI.create("http://toto.com/#fragment"), null, null);

        assertEquals("http", songUri.getScheme());
        assertFalse(songUri.isSecure());
        assertEquals("toto.com", songUri.getHost());
        assertEquals(-1, songUri.getPort());
        assertEquals("/", songUri.getPath());
        assertFalse(songUri.getQueryParameters().hasNext());
        assertEquals("fragment", songUri.getFragment());
        assertEquals("http://toto.com/#fragment", songUri.absoluteURI());
    }

    public void testNewUri_6() {
        SongURI songUri = new SongURIImpl(URI.create("http://toto.com?header1=val1&header2"), null, null);

        assertEquals("http", songUri.getScheme());
        assertFalse(songUri.isSecure());
        assertEquals("toto.com", songUri.getHost());
        assertEquals(-1, songUri.getPort());
        assertEquals("", songUri.getPath());
        assertEquals("val1", songUri.getQueryParameter("header1"));
        assertEquals("header2", songUri.getQueryParameter("header2"));
        if (!"http://toto.com?header1=val1&header2=header2".equals(songUri.absoluteURI())) {
            assertEquals("http://toto.com?header2=header2&header1=val1", songUri.absoluteURI());
        } else {
            assertEquals("http://toto.com?header1=val1&header2=header2", songUri.absoluteURI());
        }
    }

    public void testNewUri_7() {
        try {
            SongURI songUri = new SongURIImpl(URI.create("http://toto.com?=val1&header2"), null, null);
            fail();
        } catch (IllegalArgumentException e) {
            // OK
        }
    }

    public void testNewUri_8() {
        try {
            SongURI songUri = new SongURIImpl(URI.create("http:10.10.12.208:8080/m2m/applications/SYSTEM"), null, null);
            fail();
        } catch (IllegalArgumentException e) {
            // OK
        }
    }

    public void testNewUri_9() {
        try {
            SongURI songUri = new SongURIImpl("http", false, "10.10.12.208", 8080, "/toto maison/");
            songUri.setQueryParameter(
                    "targetID",
                    "http://ise2actility.poc1.actility.com:8080/m2m/discovery/test%20test?searchPrefix=/m2m/applications&searchString=ETSI.ObjectType/ETSI.IP&scope=exact");

            String targetId = songUri.getQueryParameter("targetID");

            assertEquals(
                    "http://10.10.12.208:8080/toto%20maison/?targetID=http://ise2actility.poc1.actility.com:8080/m2m/discovery/test%2520test?searchPrefix%3D/m2m/applications%26searchString%3DETSI.ObjectType/ETSI.IP%26scope%3Dexact",
                    songUri.absoluteURI());

            assertEquals("/toto maison/", songUri.getPath());

            assertEquals(
                    "http://ise2actility.poc1.actility.com:8080/m2m/discovery/test%20test?searchPrefix=/m2m/applications&searchString=ETSI.ObjectType/ETSI.IP&scope=exact",
                    targetId);

        } catch (IllegalArgumentException e) {
            e.printStackTrace();
            fail();
        }
    }

    public void testNewUri_10() {
        try {
            SongURI songUri = new SongURIImpl(
                    URI.create("http://10.10.12.208:8080/toto%20maison/?targetID=http://ise2actility.poc1.actility.com:8080/m2m/discovery/test%2520test%3FsearchPrefix%3D/m2m/applications%26searchString%3DETSI.ObjectType/ETSI.IP%26scope%3Dexact"),
                    null, null);
            String targetId = songUri.getQueryParameter("targetID");
            // SongURI targetIdUri = new SongURIImpl(URI.create(targetId), null, null);
            // songUri.setQueryParameter("targetID", targetIdUri.absoluteURI());
            assertEquals(
                    "http://10.10.12.208:8080/toto%20maison/?targetID=http://ise2actility.poc1.actility.com:8080/m2m/discovery/test%2520test?searchPrefix%3D/m2m/applications%26searchString%3DETSI.ObjectType/ETSI.IP%26scope%3Dexact",
                    songUri.absoluteURI());

            assertEquals("/toto maison/", songUri.getPath());

            assertEquals(
                    "http://ise2actility.poc1.actility.com:8080/m2m/discovery/test%20test?searchPrefix=/m2m/applications&searchString=ETSI.ObjectType/ETSI.IP&scope=exact",
                    targetId);
        } catch (IllegalArgumentException e) {
            e.printStackTrace();
            fail();
        }
    }
}
