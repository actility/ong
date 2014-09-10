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
 * id $Id: FormatUtils.java 7655 2014-02-07 14:43:04Z JReich $
 * author $Author: JReich $
 * version $Revision: 7655 $
 * lastrevision $Date: 2014-02-07 15:43:04 +0100 (Fri, 07 Feb 2014) $
 * modifiedby $LastChangedBy: JReich $
 * lastmodified $LastChangedDate: 2014-02-07 15:43:04 +0100 (Fri, 07 Feb 2014) $
 */

package com.actility.m2m.util;

import java.text.ParseException;
import java.text.SimpleDateFormat;
import java.util.Arrays;
import java.util.Calendar;
import java.util.Collection;
import java.util.Date;
import java.util.Iterator;
import java.util.Locale;
import java.util.TimeZone;

/**
 * Utils class for parsing and formatting common data types
 */
public final class FormatUtils {

    private FormatUtils() {
        // Static class
    }

    /**
     * Date format pattern used to parse HTTP date headers in RFC 1123 format.
     */
    public static final String PATTERN_RFC1123 = "EEE, dd MMM yyyy HH:mm:ss zzz";

    /**
     * Date format pattern used to parse HTTP date headers in RFC 1036 format.
     */
    public static final String PATTERN_RFC1036 = "EEEE, dd-MMM-yy HH:mm:ss zzz";

    /**
     * Date format pattern used to parse HTTP date headers in ANSI C <code>asctime()</code> format.
     */
    public static final String PATTERN_ASCTIME = "EEE MMM d HH:mm:ss yyyy";

    /**
     * Maximum date accepted, in milliseconds: date -u -d '1970-01-01 UTC 253402246799 seconds' +"%Y-%m-%d %T %z" 9999-12-31
     * 08:59:59 +0000
     */
    public static final long MAX_DATE = 253402246799999L;

    /**
     * Minimum date accepted, in milliseconds: date -u -d '1970-01-01 UTC 0 seconds' +"%Y-%m-%d %T %z" 1970-01-01 00:00:00 +0000
     */
    public static final long MIN_DATE = 0L;

    private static final Collection DEFAULT_PATTERNS = Arrays.asList(new String[] { PATTERN_ASCTIME, PATTERN_RFC1036,
            PATTERN_RFC1123 });

    private static final Date DEFAULT_TWO_DIGIT_YEAR_START;

    private static final char BITS6_TO_BASE64[] = { 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O',
            'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z', 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l',
            'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z', '0', '1', '2', '3', '4', '5', '6', '7', '8',
            '9', '+', '/' };

    private static final int BASE64_TO_BITS6[] = { -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
            -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 62, -1, -1, -1, 63,
            52, 53, 54, 55, 56, 57, 58, 59, 60, 61, -1, -1, -1, -1, -1, -1, -1, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13,
            14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, -1, -1, -1, -1, -1, -1, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36,
            37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, -1, -1, -1, -1, -1 };

    private static final long YEAR_IN_MILLIS = 365L * 24L * 60L * 60L * 1000L;
    private static final long MONTH_IN_MILLIS = 30L * 24L * 60L * 60L * 1000L;
    private static final long DAY_IN_MILLIS = 24L * 60L * 60L * 1000L;
    private static final long HOUR_IN_MILLIS = 60L * 60L * 1000L;
    private static final long MINUTE_IN_MILLIS = 60L * 1000L;
    private static final long SECOND_IN_MILLIS = 1000L;

    private static final long L_TIMEZONE = CharacterUtils.lowMask("-+0123456789:");
    private static final long H_TIMEZONE = CharacterUtils.highMask("-+0123456789:");

    static {
        Calendar calendar = Calendar.getInstance();
        calendar.set(Calendar.YEAR, 2000);
        calendar.set(Calendar.MONTH, Calendar.JANUARY);
        calendar.set(Calendar.DATE, 1);
        calendar.set(Calendar.HOUR, 0);
        calendar.set(Calendar.MINUTE, 0);
        DEFAULT_TWO_DIGIT_YEAR_START = calendar.getTime();
    }

    private static int checkLevel(StringReader reader, int stepLevel, int currentLevel) throws ParseException {
        if (stepLevel < currentLevel) {
            throw new ParseException(reader.getMessage(), reader.getOffset());
        }
        return stepLevel + 1;
    }

    private static void append(StringBuffer buffer, int num, int minLen) {
        String s = Integer.toString(num);
        for (int i = s.length(); i < minLen; i++) {
            buffer.append('0');
        }
        buffer.append(s);
    }

    private static Calendar parseInternalDateTime(boolean parseDate, boolean parseTime, String dateTime) throws ParseException {
        StringReader reader = new StringReader(dateTime);

        // boolean positive = true;
        int year = 0;
        int month = 0;
        int day = 0;
        int hour = 0;
        int minute = 0;
        int second = 0;
        int millis = 0;

        if (parseDate) {
            // Sign
            if (reader.readCurrent() == '-') {
                throw new ParseException("Parsed year is below the minimum value (1970)", reader.getOffset());
            } else if (reader.readCurrent() == '+') {
                reader.skipOffset(1);
            }

            year = reader.readIntAndCheckLength(1, 9);
            reader.checkCurrentAndSkip('-');
            month = reader.readIntAndCheckLength(2);
            reader.checkCurrentAndSkip('-');
            day = reader.readIntAndCheckLength(2);

            if (parseTime) {
                reader.checkCurrentAndSkip('T');
            }
        }

        if (parseTime) {
            hour = reader.readIntAndCheckLength(2);
            reader.checkCurrentAndSkip(':');
            minute = reader.readIntAndCheckLength(2);
            reader.checkCurrentAndSkip(':');
            second = reader.readIntAndCheckLength(2);

            if (!reader.isTerminated() && reader.readCurrent() == '.') {
                reader.skipOffset(1);
                int offset = reader.getOffset();
                millis = reader.readInt();
                if ((reader.getOffset() - offset) < 1 || (reader.getOffset() - offset) > 3) {
                    throw new ParseException(dateTime, reader.getOffset());
                }
            }
        }

        StringBuffer timeZone = new StringBuffer();
        timeZone.append("GMT");
        if (!reader.isTerminated()) {
            if (reader.readCurrent() == 'Z') {
                reader.skipOffset(1);
            } else {
                reader.read(timeZone, L_TIMEZONE, H_TIMEZONE);
            }
        }
        if (!reader.isTerminated()) {
            throw new ParseException(dateTime, reader.getOffset());
        }

        if (parseDate) {
            // Check min/max dates
            if (year > 9999) {
                // Max date 292278994-08-17T07:12:55.807Z -->
                throw new ParseException("Parsed year is above the maximum value (9999)", reader.getOffset());
            } else if (year < 1970) {
                // Min date -292269055-12-02T16:47:04.192Z
                throw new ParseException("Parsed year is under the minimum value (1970)", reader.getOffset());
            }
        }

        Calendar calendar = Calendar.getInstance(TimeZone.getTimeZone(timeZone.toString()));

        calendar.set(Calendar.YEAR, year);
        calendar.set(Calendar.MONTH, parseDate ? month - 1 : month);
        calendar.set(Calendar.DAY_OF_MONTH, day);
        calendar.set(Calendar.HOUR_OF_DAY, hour);
        calendar.set(Calendar.MINUTE, minute);
        calendar.set(Calendar.SECOND, second);
        calendar.set(Calendar.MILLISECOND, millis);

        if (parseDate) {
            long timeInMillis = calendar.getTime().getTime();
            // Check min/max dates

            if ((timeInMillis < MIN_DATE) || (timeInMillis > MAX_DATE)) {
                throw new ParseException("Parsed datetime is above the maximum value (9999-12-31T08:59:59.999Z)",
                        reader.getOffset());
            }
        }

        return calendar;
    }

    /**
     * Generic method for formatting ISO8601 date/time field.
     *
     * @param parseDate specifies if the the formatted ISO8601 field should contain the date.
     * @param parseTime specifies if the the formatted ISO8601 field should contain the time.
     * @param date the current date/time, relatively to the given time zone.
     * @param timeZone the time zone to consider for formmating the date/time.
     * @return the ISO 8601 formatted date and/or time.
     */
    private static String formatInternalDateTime(boolean parseDate, boolean parseTime, Date date, Calendar calendar,
            TimeZone timeZone) {
        StringBuffer buffer = new StringBuffer();
        calendar.setTime(date);

        if (parseDate) {
            int year = calendar.get(Calendar.YEAR);
            if (year < 0) {
                buffer.append('-');
            }
            append(buffer, year, 4);
            buffer.append('-');
            append(buffer, calendar.get(Calendar.MONTH) + 1, 2);
            buffer.append('-');
            append(buffer, calendar.get(Calendar.DAY_OF_MONTH), 2);
            if (parseTime) {
                buffer.append('T');
            }
        }
        if (parseTime) {
            append(buffer, calendar.get(Calendar.HOUR_OF_DAY), 2);
            buffer.append(':');
            append(buffer, calendar.get(Calendar.MINUTE), 2);
            buffer.append(':');
            append(buffer, calendar.get(Calendar.SECOND), 2);
            int millis = calendar.get(Calendar.MILLISECOND);
            buffer.append('.');
            append(buffer, millis, 3);
        }
        // JDK 1.4: int offset = tz.getOffset(cal.getTimeInMillis());
        // int offset = calendar.get(Calendar.ZONE_OFFSET);
        int offset = timeZone.getRawOffset();
        if (timeZone.useDaylightTime()) {
            offset = timeZone.getOffset(
                    1,
                    calendar.get(Calendar.YEAR),
                    calendar.get(Calendar.MONTH),
                    calendar.get(Calendar.DAY_OF_MONTH),
                    calendar.get(Calendar.DAY_OF_WEEK),
                    (calendar.get(Calendar.HOUR_OF_DAY) * 3600000) + (calendar.get(Calendar.MINUTE) * 60000)
                            + (calendar.get(Calendar.SECOND) * 1000) + calendar.get(Calendar.MILLISECOND));
        }
        if (offset == 0) {
            buffer.append('Z');
        } else {
            if (offset < 0) {
                buffer.append('-');
                offset = -offset;
            } else {
                buffer.append('+');
            }
            int hours = (int) (offset / HOUR_IN_MILLIS);
            int minutes = (int) ((offset % HOUR_IN_MILLIS) / MINUTE_IN_MILLIS);
            append(buffer, hours, 2);
            buffer.append(':');
            append(buffer, minutes, 2);
        }
        return buffer.toString();
    }

    /**
     * Parse a duration according to the xs:duration definition in the XML schema specification.
     * <p>
     * This is an estimation of the duration as if years or months are used it is not possible to determine an exact duration in
     * milliseconds.
     * <p>
     * For this function we then consider a year to be of 365 days and a month to be of 30 days.
     *
     * @param duration The string duration to parse
     * @return The corresponding duration in milliseconds
     * @throws ParseException if the duration string is badly formatted
     */
    public static long parseDuration(String duration) throws ParseException {
        StringReader reader = new StringReader(duration);
        long millis = 0;
        long tmp = 0;
        int level = 0;
        boolean positive = true;
        if (reader.readCurrent() == '-') {
            positive = false;
            reader.skipOffset(1);
        } else if (reader.readCurrent() == '+') {
            reader.skipOffset(1);
        }
        reader.checkCurrentAndSkip('P');

        while (!reader.isTerminated() && reader.readCurrent() >= '0' && reader.readCurrent() <= '9') {
            tmp = reader.readLong();
            switch (reader.readCurrentAndSkip()) {
            case 'Y':
                level = checkLevel(reader, 0, level);
                millis += YEAR_IN_MILLIS * tmp;
                break;
            case 'M':
                level = checkLevel(reader, 1, level);
                millis += MONTH_IN_MILLIS * tmp;
                break;
            case 'D':
                level = checkLevel(reader, 2, level);
                millis += DAY_IN_MILLIS * tmp;
                break;
            default:
                throw new ParseException(duration, reader.getOffset() - 1);
            }
        }
        if (!reader.isTerminated()) {
            reader.checkCurrentAndSkip('T');
            while (!reader.isTerminated() && reader.readCurrent() >= '0' && reader.readCurrent() <= '9') {
                tmp = reader.readLong();
                switch (reader.readCurrentAndSkip()) {
                case 'H':
                    level = checkLevel(reader, 3, level);
                    millis += HOUR_IN_MILLIS * tmp;
                    break;
                case 'M':
                    level = checkLevel(reader, 4, level);
                    millis += MINUTE_IN_MILLIS * tmp;
                    break;
                case 'S':
                    level = checkLevel(reader, 5, level);
                    millis += SECOND_IN_MILLIS * tmp;
                    break;
                case '.':
                    level = checkLevel(reader, 5, level);
                    millis += SECOND_IN_MILLIS * tmp;
                    int length = reader.getOffset();
                    tmp = reader.readLong();
                    length = reader.getOffset() - length;
                    if (tmp >= 1000L) {
                        throw new ParseException(duration, reader.getOffset());
                    }
                    switch (length) {
                    case 1:
                        millis += tmp * 100;
                        break;
                    case 2:
                        millis += tmp * 10;
                        break;
                    case 3:
                        millis += tmp;
                        break;
                    default:
                        throw new ParseException(duration, reader.getOffset());
                    }
                    reader.checkCurrentAndSkip('S');
                    break;
                default:
                    throw new ParseException(duration, reader.getOffset());
                }
            }
        }

        reader.checkTerminated();
        if (!positive) {
            millis = -millis;
        }
        return millis;
    }

    /**
     * Formats a string duration correponding to the given duration in milliseconds and according to the xs:duration data type
     * of the XML schema specification.
     *
     * @param duration The duration in milliseconds
     * @return A string duration representing the given duration in milliseconds
     */
    public static String formatDuration(long duration) {
        if (duration == 0) {
            return "PT0S";
        }
        StringBuffer s = new StringBuffer();
        long absDuration = duration;
        if (duration < 0) {
            s.append('-');
            absDuration = -duration;
        }
        s.append('P');
        long days = absDuration / DAY_IN_MILLIS;
        long time = absDuration % DAY_IN_MILLIS;
        long hours = time / HOUR_IN_MILLIS;
        long minutes = (time % HOUR_IN_MILLIS) / MINUTE_IN_MILLIS;
        long seconds = (time % MINUTE_IN_MILLIS) / SECOND_IN_MILLIS;
        long millis = time % SECOND_IN_MILLIS;
        if (days != 0) {
            s.append(days);
            s.append('D');
        }
        if (time != 0) {
            s.append('T');
            if (hours != 0) {
                s.append(hours);
                s.append('H');
            }
            if (minutes != 0) {
                s.append(minutes);
                s.append('M');
            }
            if (millis != 0) {
                s.append(seconds);
                s.append('.');
                append(s, (int) millis, 3);
                s.append('S');
            } else if (seconds != 0) {
                s.append(seconds);
                s.append('S');
            }
        }
        return s.toString();
    }

    /**
     * Parse a date time according to the xs:dateTime definition in the XML schema specification.
     * <p>
     * This method builds a {@link Date} from the given dateTime which means the timezone info is lost.
     *
     * @param dateTime The dateTime to parse
     * @return The built {@link Date}
     * @throws ParseException if the dateTime string is badly formatted
     */
    public static Date parseDateTime(String dateTime) throws ParseException {
        return parseInternalDateTime(true, true, dateTime).getTime();
    }

    /**
     * Formats a date time according to the xs:dateTime definition in the XML schema specification.
     *
     * @param dateTime The {@link Date} to format as a date time
     * @param timeZone The timezone in which the date time is formatted
     * @return The formatted date time
     */
    public static String formatDateTime(Date dateTime, TimeZone timeZone) {
        Calendar calendar = Calendar.getInstance(timeZone);
        return formatInternalDateTime(true, true, dateTime, calendar, timeZone);
    }

    /**
     * Formats a date time according to the xs:dateTime definition in the XML schema specification.
     *
     * @param dateTime The {@link Date} to format as a date time
     * @param calendar The calendar in the correct timezone with which the date time will be formatted
     * @return The formatted date time
     */
    public static String formatDateTime(Date dateTime, Calendar calendar) {
        return formatInternalDateTime(true, true, dateTime, calendar, calendar.getTimeZone());
    }

    /**
     * Parse a date according to the xs:date definition in the XML schema specification.
     * <p>
     * This method builds a {@link Date} from the given date which means the timezone info is lost.
     *
     * @param date The date to parse
     * @return The built {@link Date}
     * @throws ParseException if the date string is badly formatted
     */
    public static Date parseDate(String date) throws ParseException {
        return parseInternalDateTime(true, false, date).getTime();
    }

    /**
     * Formats a date according to the xs:date definition in the XML schema specification.
     *
     * @param date The {@link Date} to format as a date
     * @param timeZone The timezone in which the date is formatted
     * @return The formatted date
     */
    public static String formatDate(Date date, TimeZone timeZone) {
        Calendar calendar = Calendar.getInstance(timeZone);
        return formatInternalDateTime(true, false, date, calendar, timeZone);
    }

    /**
     * Formats a date according to the xs:date definition in the XML schema specification.
     *
     * @param date The {@link Date} to format as a date
     * @param calendar The calendar in the correct timezone with which the date time will be formatted
     * @return The formatted date
     */
    public static String formatDate(Date date, Calendar calendar) {
        return formatInternalDateTime(true, false, date, calendar, calendar.getTimeZone());
    }

    /**
     * Parse a time according to the xs:time definition in the XML schema specification.
     * <p>
     * This method builds a {@link Date} from the given time which means the timezone info is lost.
     *
     * @param time The time to parse
     * @return The built {@link Date}
     * @throws ParseException if the time string is badly formatted
     */
    public static Date parseTime(String time) throws ParseException {
        return parseInternalDateTime(false, true, time).getTime();
    }

    /**
     * Formats a time according to the xs:time definition in the XML schema specification.
     *
     * @param time The {@link Date} to format as a time
     * @param timeZone The timezone in which the time is formatted
     * @return The formatted time
     */
    public static String formatTime(Date time, TimeZone timeZone) {
        Calendar calendar = Calendar.getInstance(timeZone);
        return formatInternalDateTime(false, true, time, calendar, timeZone);
    }

    /**
     * Formats a time according to the xs:time definition in the XML schema specification.
     *
     * @param time The {@link Date} to format as a time
     * @param calendar The calendar in the correct timezone with which the date time will be formatted
     * @return The formatted time
     */
    public static String formatTime(Date time, Calendar calendar) {
        return formatInternalDateTime(false, true, time, calendar, calendar.getTimeZone());
    }

    /**
     * Checks the given string is a valid base64
     *
     * @param str The base64 string to check
     * @return Whether the given string is a valid base64
     */
    public static int checkBase64(String str) {
        if ((str.length() % 4) != 0) {
            return -1;
        }
        byte[] in = str.getBytes();
        int size = 0;
        int len = in.length;
        if (len > 0) {
            if (in[len - 2] == '=') {
                size = (len / 4 * 3) - 2;
            } else if (in[len - 1] == '=') {
                size = (len / 4 * 3) - 1;
            } else {
                size = len / 4 * 3;
            }
            int char1 = 0;
            int char2 = 0;
            int char3 = 0;
            int char4 = 0;
            int i = 0;
            for (; (len - i) > 4; i += 4) {
                char1 = BASE64_TO_BITS6[(in[i] & 0xff)];
                char2 = BASE64_TO_BITS6[(in[i + 1] & 0xff)];
                char3 = BASE64_TO_BITS6[(in[i + 2] & 0xff)];
                char4 = BASE64_TO_BITS6[(in[i + 3] & 0xff)];
                if (char1 < 0 || char2 < 0 || char3 < 0 || char4 < 0) {
                    return -1;
                }
            }
            char1 = BASE64_TO_BITS6[(in[i] & 0xff)];
            char2 = BASE64_TO_BITS6[(in[i + 1] & 0xff)];
            if (char1 < 0 || char2 < 0) {
                return -1;
            }
            if (in[i + 2] != '=') {
                char3 = BASE64_TO_BITS6[(in[i + 2] & 0xff)];
                if (char3 < 0) {
                    return -1;
                }
            }
            if (in[i + 3] != '=') {
                char4 = BASE64_TO_BITS6[(in[i + 3] & 0xff)];
                if (char4 < 0) {
                    return -1;
                }
            }
        }

        return size;
    }

    /**
     * Parses a base64 string to a byte array.
     *
     * @param str The base64 string
     * @return The parsed byte array
     * @throws ParseException If given string is not a valid base64
     */
    public static byte[] parseBase64(String str) throws ParseException {
        if ((str.length() % 4) != 0) {
            throw new ParseException("Base64 stream not a multiple of 4 characters", 0);
        }
        byte[] in = str.getBytes();
        int len = in.length;
        byte[] out = null;
        if (len > 0) {
            if (in[len - 2] == '=') {
                out = new byte[(len / 4 * 3) - 2];
            } else if (in[len - 1] == '=') {
                out = new byte[(len / 4 * 3) - 1];
            } else {
                out = new byte[len / 4 * 3];
            }
            int char1 = 0;
            int char2 = 0;
            int char3 = 0;
            int char4 = 0;
            int i = 0;
            int j = 0;
            for (; (len - i) > 4; i += 4) {
                char1 = BASE64_TO_BITS6[(in[i] & 0xff)];
                char2 = BASE64_TO_BITS6[(in[i + 1] & 0xff)];
                char3 = BASE64_TO_BITS6[(in[i + 2] & 0xff)];
                char4 = BASE64_TO_BITS6[(in[i + 3] & 0xff)];
                if (char1 < 0 || char2 < 0 || char3 < 0 || char4 < 0) {
                    throw new ParseException("Forbidden character detected in base64 string: " + str, i);
                }
                out[j] = (byte) ((char1 << 2) + (char2 >> 4));
                ++j;
                out[j] = (byte) (((char2 & 0x0f) << 4) + (char3 >> 2));
                ++j;
                out[j] = (byte) (((char3 & 0x03) << 6) + char4);
                ++j;
            }
            char1 = BASE64_TO_BITS6[(in[i] & 0xff)];
            char2 = BASE64_TO_BITS6[(in[i + 1] & 0xff)];
            if (char1 < 0 || char2 < 0) {
                throw new ParseException("Forbidden character detected in base64 string: " + str, i);
            }
            out[j] = (byte) ((char1 << 2) + (char2 >> 4));
            ++j;
            if (in[i + 2] != '=') {
                char3 = BASE64_TO_BITS6[(in[i + 2] & 0xff)];
                if (char3 < 0) {
                    throw new ParseException("Forbidden character detected in base64 string: " + str, i + 2);
                }
                out[j] = (byte) (((char2 & 0x0f) << 4) + (char3 >> 2));
                ++j;
            }
            if (in[i + 3] != '=') {
                char4 = BASE64_TO_BITS6[(in[i + 3] & 0xff)];
                if (char4 < 0) {
                    throw new ParseException("Forbidden character detected in base64 string: " + str, i + 3);
                }
                out[j] = (byte) (((char3 & 0x03) << 6) + char4);
            }
        } else {
            out = EmptyUtils.EMPTY_BYTES_ARRAY;
        }

        return out;
    }

    /**
     * Encode a raw byte array to a Base64 String.
     *
     * @param in Byte array to encode.
     * @return The encoded base64 string
     */
    public static String formatBase64(byte[] in) {
        return formatBase64(in, in.length);
    }

    /**
     * Encode a raw byte array to a Base64 String.
     *
     * @param in Byte array to encode.
     * @param len The number of bytes to read from the given byte array
     * @return The encoded base64 string
     */
    public static String formatBase64(byte[] in, int len) {
        StringBuffer base64 = new StringBuffer(((len * 4 / 3) + 2));
        int byte1 = 0;
        int byte2 = 0;
        int byte3 = 0;
        int i = 0;
        for (; (len - i) >= 3; i += 3) {
            byte1 = in[i] & 0xff;
            byte2 = in[i + 1] & 0xff;
            byte3 = in[i + 2] & 0xff;
            base64.append(BITS6_TO_BASE64[byte1 >> 2]);
            base64.append(BITS6_TO_BASE64[((byte1 & 0x03) << 4) + (byte2 >> 4)]);
            base64.append(BITS6_TO_BASE64[((byte2 & 0x0f) << 2) + (byte3 >> 6)]);
            base64.append(BITS6_TO_BASE64[(byte3 & 0x3f)]);
        }
        if (len - i > 0) {
            byte1 = in[i] & 0xff;
            base64.append(BITS6_TO_BASE64[byte1 >> 2]);
            if ((len - i) == 1) {
                base64.append(BITS6_TO_BASE64[(byte1 & 0x03) << 4]);
                base64.append('=');
            } else {
                byte2 = in[i + 1] & 0xff;
                base64.append(BITS6_TO_BASE64[((byte1 & 0x03) << 4) + (byte2 >> 4)]);
                base64.append(BITS6_TO_BASE64[(byte2 & 0x0f) << 2]);
            }
            base64.append('=');
        }
        return base64.toString();
    }

    /**
     * Parses a date value. The formats used for parsing the date value are retrieved from the default http params.
     *
     * @param dateValue the date value to parse
     *
     * @return the parsed date
     *
     * @throws ParseException if the value could not be parsed using any of the supported date formats
     */
    public static Date parseHttpDate(String dateValue) throws ParseException {
        return parseHttpDate(dateValue, null, null);
    }

    /**
     * Parses the date value using the given date formats.
     *
     * @param dateValue the date value to parse
     * @param dateFormats the date formats to use
     *
     * @return the parsed date
     *
     * @throws ParseException if none of the dataFormats could parse the dateValue
     */
    public static Date parseHttpDate(String dateValue, Collection dateFormats) throws ParseException {
        return parseHttpDate(dateValue, dateFormats, null);
    }

    /**
     * Parses the date value using the given date formats.
     *
     * @param dateValue the date value to parse
     * @param dateFormats the date formats to use
     * @param startDate During parsing, two digit years will be placed in the range <code>startDate</code> to
     *            <code>startDate + 100 years</code>. This value may be <code>null</code>. When <code>null</code> is given as a
     *            parameter, year <code>2000</code> will be used.
     *
     * @return the parsed date
     *
     * @throws ParseException if none of the dataFormats could parse the dateValue
     */
    public static Date parseHttpDate(String dateValue, Collection dateFormats, Date startDate) throws ParseException {

        if (dateValue == null) {
            throw new IllegalArgumentException("dateValue is null");
        }
        if (dateFormats == null) {
            dateFormats = DEFAULT_PATTERNS;
        }
        if (startDate == null) {
            startDate = DEFAULT_TWO_DIGIT_YEAR_START;
        }
        // trim single quotes around date if present
        // see issue #5279
        if (dateValue.length() > 1 && dateValue.startsWith("'") && dateValue.endsWith("'")) {
            dateValue = dateValue.substring(1, dateValue.length() - 1);
        }

        SimpleDateFormat dateParser = null;
        Iterator formatIter = dateFormats.iterator();

        while (formatIter.hasNext()) {
            String format = (String) formatIter.next();
            if (dateParser == null) {
                dateParser = new SimpleDateFormat(format, Locale.US);
                dateParser.setTimeZone(TimeZone.getTimeZone("GMT"));
                dateParser.set2DigitYearStart(startDate);
            } else {
                dateParser.applyPattern(format);
            }
            try {
                return dateParser.parse(dateValue);
            } catch (ParseException pe) {
                // ignore this exception, we will try the next format
            }
        }

        // we were unable to parse the date
        throw new ParseException("Unable to parse the date " + dateValue, 0);
    }

    /**
     * Formats the given date according to the RFC 1123 pattern.
     *
     * @param date The date to format.
     * @return An RFC 1123 formatted date string.
     *
     * @see #PATTERN_RFC1123
     */
    public static String formatHttpDate(Date date) {
        return formatHttpDate(date, PATTERN_RFC1123);
    }

    /**
     * Formats the given date according to the specified pattern. The pattern must conform to that used by the
     * {@link SimpleDateFormat simple date format} class.
     *
     * @param date The date to format.
     * @param pattern The pattern to use for formatting the date.
     * @return A formatted date string.
     *
     * @throws IllegalArgumentException If the given date pattern is invalid.
     *
     * @see SimpleDateFormat
     */
    public static String formatHttpDate(Date date, String pattern) {
        if (date == null) {
            throw new IllegalArgumentException("date is null");
        }
        if (pattern == null) {
            throw new IllegalArgumentException("pattern is null");
        }

        SimpleDateFormat formatter = new SimpleDateFormat(pattern, Locale.US);
        formatter.setTimeZone(UtilConstants.GMT_TIMEZONE);
        return formatter.format(date);
    }
}
