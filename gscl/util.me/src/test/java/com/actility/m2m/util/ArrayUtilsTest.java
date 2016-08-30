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

package com.actility.m2m.util;

import com.actility.m2m.util.ArrayUtils;

import junit.framework.TestCase;
import junit.framework.TestSuite;

/**
 * The class <code>ArrayUtilsTest</code> contains tests for the class <code>{@link ArrayUtils}</code>.
 */
public class ArrayUtilsTest extends TestCase {

    public void testAppendArray_1() throws Exception {
        Object[] array = new Object[] {};
        Object elem = new Object();

        Object[] result = ArrayUtils.appendArray(array, elem);

        assertNotNull(result);
        assertEquals(1, result.length);
        assertNotNull(result[0]);
    }

    public void testToString_1() throws Exception {
        Object[] array = new Object[] { "toto" };
        String prefix = null;
        String suffix = "";
        String separator = "";

        String result = ArrayUtils.toString(array, prefix, suffix, separator);

        assertEquals("toto", result);
    }

    public void testToString_2() throws Exception {
        Object[] array = new Object[] {};
        String prefix = "";
        String suffix = null;
        String separator = "";

        String result = ArrayUtils.toString(array, prefix, suffix, separator);

        assertEquals("", result);
    }

    public void testToString_3() throws Exception {
        Object[] array = new Object[] {"toto", "tata"};
        String prefix = "[";
        String suffix = "]";
        String separator = " ";

        String result = ArrayUtils.toString(array, prefix, suffix, separator);

        assertEquals("[toto tata]", result);
    }

    public static void main(String[] args) {
        if (args.length == 0) {
            // Run all of the tests
            junit.textui.TestRunner.run(ArrayUtilsTest.class);
        } else {
            // Run only the named tests
            TestSuite suite = new TestSuite("Selected tests");
            for (int i = 0; i < args.length; i++) {
                TestCase test = new ArrayUtilsTest();
                test.setName(args[i]);
                suite.addTest(test);
            }
            junit.textui.TestRunner.run(suite);
        }
    }
}
