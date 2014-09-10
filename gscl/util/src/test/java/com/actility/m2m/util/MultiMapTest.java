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
 * id $Id: MultiMapTest.java 7604 2014-02-06 13:01:34Z JReich $
 * author $Author: JReich $
 * version $Revision: 7604 $
 * lastrevision $Date: 2014-02-06 14:01:34 +0100 (Thu, 06 Feb 2014) $
 * modifiedby $LastChangedBy: JReich $
 * lastmodified $LastChangedDate: 2014-02-06 14:01:34 +0100 (Thu, 06 Feb 2014) $
 */

package com.actility.m2m.util;

import junit.framework.TestCase;
import junit.framework.TestSuite;

public class MultiMapTest extends TestCase {

    public void testGet() {
        MultiMap map = new MultiMap(String.CASE_INSENSITIVE_ORDER);
        map.put("Content-Type", "test");

        Object value = map.get("content-type");

        if (value != null) {
            assertEquals("test", value);
        } else {
            fail();
        }
    }

    public void testGet1() {
        MultiMap map = new MultiMap(String.CASE_INSENSITIVE_ORDER);
        map.put("Content-Type", "test1");
        map.put("Content-Type", "test2");

        Object value = map.get("content-type");

        if (value != null) {
            assertEquals("test1", value);
            System.out.println(map.toString());
        } else {
            fail();
        }
    }

    /**
     * Launch the test.
     *
     * @param args the command line arguments
     *
     * @generatedBy CodePro at 10/5/11 11:25 PM
     */
    public static void main(String[] args) {
        if (args.length == 0) {
            // Run all of the tests
            junit.textui.TestRunner.run(MultiMapTest.class);
        } else {
            // Run only the named tests
            TestSuite suite = new TestSuite("Selected tests");
            for (int i = 0; i < args.length; i++) {
                TestCase test = new MultiMapTest();
                test.setName(args[i]);
                suite.addTest(test);
            }
            junit.textui.TestRunner.run(suite);
        }
    }
}
