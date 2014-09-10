/*
* Copyright (C) Actility, SA. All Rights Reserved.
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
*/

/* ------------------- base64 ------------------ */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>



/**
 ** @brief Formats an ISO time
 ** @param A time_t (Unix time)
 ** @param The output buffer
 ** @return void
 **/
void rtl_gettimeofday_to_iso8601date(struct timeval *tv, struct timezone *tz, char *buf) {
        struct tm *tm = localtime(&tv->tv_sec);
        int h, m;
        h = tm->tm_gmtoff/3600;
        m = (tm->tm_gmtoff - h*3600) / 60;
        char sign = (h<0) ? '-':'+';
        sprintf (buf, "%04d-%02d-%02dT%02d:%02d:%02d.%ld%c%02d:%02d",
                tm->tm_year + 1900, tm->tm_mon + 1,
                tm->tm_mday, tm->tm_hour, tm->tm_min, 
		tm->tm_sec, tv->tv_usec / 1000,
                sign, abs(h), abs(m));
}

void rtl_getCurrentIso8601date(char *buf) {
        struct timeval tv;
        struct timezone tz;
        gettimeofday(&tv, &tz);
        rtl_gettimeofday_to_iso8601date(&tv, &tz, buf);
}
