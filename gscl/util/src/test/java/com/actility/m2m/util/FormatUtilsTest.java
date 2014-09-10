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
 * id $Id: FormatUtilsTest.java 7656 2014-02-07 14:58:28Z JReich $
 * author $Author: JReich $
 * version $Revision: 7656 $
 * lastrevision $Date: 2014-02-07 15:58:28 +0100 (Fri, 07 Feb 2014) $
 * modifiedby $LastChangedBy: JReich $
 * lastmodified $LastChangedDate: 2014-02-07 15:58:28 +0100 (Fri, 07 Feb 2014) $
 */

package com.actility.m2m.util;

import java.text.ParseException;
import java.util.Date;
import java.util.TimeZone;

import junit.framework.TestCase;
import junit.framework.TestSuite;

public class FormatUtilsTest extends TestCase {

    private TimeZone utcTimeZone = TimeZone.getTimeZone("UTC");
    private TimeZone laTimeZone = TimeZone.getTimeZone("America/Los_Angeles");

    public void testParseDuration1() {
        try {
            assertEquals("P3650DT5M0.120S", FormatUtils.formatDuration(FormatUtils.parseDuration("P10YT05M0.12S")));
        } catch (ParseException e) {
            e.printStackTrace();
            fail();
        }
    }

    public void testParseDuration2() {
        assertEquals("PT0S", FormatUtils.formatDuration(0));
    }

    public void testParseDuration3() {
        try {
            assertEquals(0, FormatUtils.parseDuration("PT0S"));
        } catch (ParseException e) {
            e.printStackTrace();
            fail();
        }
    }

    public void testParseDateTime1() {
        try {
            assertEquals("2000-01-05T20:30:00.000Z",
                    FormatUtils.formatDateTime(FormatUtils.parseDateTime("2000-01-05T20:30:00Z"), utcTimeZone));
        } catch (ParseException e) {
            e.printStackTrace();
            fail();
        }
    }

    public void testParseDateTime2() {
        try {
            assertEquals("2000-01-06T01:30:00.000Z",
                    FormatUtils.formatDateTime(FormatUtils.parseDateTime("2000-01-05T20:30:00-05:00"), utcTimeZone));
        } catch (ParseException e) {
            e.printStackTrace();
            fail();
        }
    }

    public void testParseDateTime3() {
        try {
            assertEquals("2000-01-05T20:30:00.000Z",
                    FormatUtils.formatDateTime(FormatUtils.parseDateTime("2000-01-05T20:30:00"), utcTimeZone));
        } catch (ParseException e) {
            e.printStackTrace();
            fail();
        }
    }

    // TZ=MST date -d '2011-10-31 UTC 20:30:00' +"%Y-%m-%d %T %z" --> 2011-10-31 13:30:00 -0700
    // warning, in MST time zone, DST starts on the second Sunday of March and ends on the first Sunday of November.
    public void testParseDateTime4() {
        try {
            assertEquals("2011-10-31T13:30:00.000-07:00",
                    FormatUtils.formatDateTime(FormatUtils.parseDateTime("2011-10-31T20:30:00Z"), laTimeZone));
        } catch (ParseException e) {
            e.printStackTrace();
            fail();
        }
    }

    public void testParseDateTime5() {
        try {
            assertEquals("2011-10-31T18:30:00.000-07:00",
                    FormatUtils.formatDateTime(FormatUtils.parseDateTime("2011-10-31T20:30:00-05:00"), laTimeZone));
        } catch (ParseException e) {
            e.printStackTrace();
            fail();
        }
    }

    public void testParseDateTime6() {
        try {
            assertEquals("2011-10-31T13:30:00.000-07:00",
                    FormatUtils.formatDateTime(FormatUtils.parseDateTime("2011-10-31T20:30:00"), laTimeZone));
        } catch (ParseException e) {
            e.printStackTrace();
            fail();
        }
    }

    public void testParseDateTime7() {
        try {
            assertEquals("2011-11-06T12:30:00.000-08:00",
                    FormatUtils.formatDateTime(FormatUtils.parseDateTime("2011-11-06T20:30:00Z"), laTimeZone));
        } catch (ParseException e) {
            e.printStackTrace();
            fail();
        }
    }

    public void testParseDateTime8() {
        try {
            assertEquals("2011-11-06T17:30:00.000-08:00",
                    FormatUtils.formatDateTime(FormatUtils.parseDateTime("2011-11-06T20:30:00-05:00"), laTimeZone));
        } catch (ParseException e) {
            e.printStackTrace();
            fail();
        }
    }

    public void testParseDateTime9() {
        try {
            assertEquals("2011-11-06T12:30:00.000-08:00",
                    FormatUtils.formatDateTime(FormatUtils.parseDateTime("2011-11-06T20:30:00"), laTimeZone));
        } catch (ParseException e) {
            e.printStackTrace();
            fail();
        }
    }

    public void testParseDateTime10() {
        try {
            assertEquals("9999-12-31T08:59:59.999Z",
                    FormatUtils.formatDateTime(FormatUtils.parseDateTime("9999-12-31T08:59:59.999Z"), utcTimeZone));
        } catch (ParseException e) {
            e.printStackTrace();
            fail();
        }
    }

    public void testParseDateTime11() {
        try {
            assertEquals("1970-01-01T00:00:00.000Z",
                    FormatUtils.formatDateTime(FormatUtils.parseDateTime("1970-01-01T00:00:00.000Z"), utcTimeZone));
        } catch (ParseException e) {
            e.printStackTrace();
            fail();
        }
    }

    public void testParseDateTime12() {
        try {
            FormatUtils.parseDateTime("10000-01-01T00:00:00.001Z");
            fail();
        } catch (ParseException e) {
            // OK
        }
    }

    public void testParseDateTime13() {
        try {
            FormatUtils.parseDateTime("1969-12-31T23:59:59.999Z");
            fail();
        } catch (ParseException e) {
            // OK
        }
    }

    public void testParseDateTime14() {
        assertEquals("9999-12-31T08:59:59.999Z", FormatUtils.formatDateTime(new Date(FormatUtils.MAX_DATE), utcTimeZone));
    }

    public void testParseDate1() {
        try {
            assertEquals("2000-01-05Z", FormatUtils.formatDate(FormatUtils.parseDate("2000-01-05Z"), utcTimeZone));
        } catch (ParseException e) {
            e.printStackTrace();
            fail();
        }
    }

    public void testParseDate2() {
        try {
            assertEquals("2000-01-05Z", FormatUtils.formatDate(FormatUtils.parseDate("2000-01-05-05:00"), utcTimeZone));
        } catch (ParseException e) {
            e.printStackTrace();
            fail();
        }
    }

    public void testParseDate3() {
        try {
            assertEquals("2000-01-05Z", FormatUtils.formatDate(FormatUtils.parseDate("2000-01-05"), utcTimeZone));
        } catch (ParseException e) {
            e.printStackTrace();
            fail();
        }
    }

    public void testParseTime1() {
        try {
            assertEquals("20:30:00.000Z", FormatUtils.formatTime(FormatUtils.parseTime("20:30:00Z"), utcTimeZone));
        } catch (ParseException e) {
            e.printStackTrace();
            fail();
        }
    }

    public void testParseTime2() {
        try {
            assertEquals("01:30:00.000Z", FormatUtils.formatTime(FormatUtils.parseTime("20:30:00-05:00"), utcTimeZone));
        } catch (ParseException e) {
            e.printStackTrace();
            fail();
        }
    }

    public void testParseTime3() {
        try {
            assertEquals("20:30:00.000Z", FormatUtils.formatTime(FormatUtils.parseTime("20:30:00"), utcTimeZone));
        } catch (ParseException e) {
            e.printStackTrace();
            fail();
        }
    }

    /**
     * Launch the test.
     *
     * @param args the command line arguments
     *
     * @generatedBy CodePro at 10/5/11 11:27 PM
     */
    public static void main(String[] args) {
        if (args.length == 0) {
            // Run all of the tests
            junit.textui.TestRunner.run(FormatUtilsTest.class);
        } else {
            // Run only the named tests
            TestSuite suite = new TestSuite("Selected tests");
            for (int i = 0; i < args.length; i++) {
                TestCase test = new FormatUtilsTest();
                test.setName(args[i]);
                suite.addTest(test);
            }
            junit.textui.TestRunner.run(suite);
        }
    }
}
