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

#include <stdio.h>
#include <stdarg.h>
#include <string.h>

#include "unitTest.h"

#if !defined(False)
typedef unsigned char boolean;
#define False (boolean)0
#define True (boolean)1
#endif

static int _testCount=0, _errorCount=0;

boolean
STAssertEquals(int value, int expected, char *fmt, ...) {
	_testCount ++;
	if	(value == expected)
		return	True;
	va_list listArg;
	va_start(listArg,fmt);
	fprintf (stderr, "*** ");
	vfprintf (stderr, fmt, listArg);
	fprintf (stderr, " Value '%d' should be equal to '%d'\n", value, expected);
	va_end(listArg);
	_errorCount ++;
	return False;
}

boolean
STAssertEqualsWithAccuracy(double value, double expected, double accuracy, char *fmt, ...) {
	_testCount ++;
	if	((value >= expected-accuracy) && (value <= expected+accuracy))
		return	True;
	va_list listArg;
	va_start(listArg,fmt);
	fprintf (stderr, "*** ");
	vfprintf (stderr, fmt, listArg);
	fprintf (stderr, " Value %f should be equal to %f (+/- %f)\n", value, expected, accuracy);
	va_end(listArg);
	_errorCount ++;
	return False;
}

boolean
STAssertTrue(boolean value, char *fmt, ...) {
	_testCount ++;
	if	(value == True)
		return	True;
	va_list listArg;
	va_start(listArg,fmt);
	fprintf (stderr, "*** ");
	vfprintf (stderr, fmt, listArg);
	fprintf (stderr, " Value should be True\n");
	va_end(listArg);
	_errorCount ++;
	return False;
}

boolean
STAssertFalse(boolean value, char *fmt, ...) {
	_testCount ++;
	if	(value == False)
		return	True;
	va_list listArg;
	va_start(listArg,fmt);
	fprintf (stderr, "*** ");
	vfprintf (stderr, fmt, listArg);
	fprintf (stderr, " Value should be False\n");
	va_end(listArg);
	_errorCount ++;
	return False;
}

boolean
STAssertStringEquals(char *value, char *expected, char *fmt, ...) {
	_testCount ++;
	if	(!value && !expected)
		return	True;
	if	(value && expected && !strcmp(value, expected))
		return	True;
	va_list listArg;
	va_start(listArg,fmt);
	fprintf (stderr, "*** ");
	vfprintf (stderr, fmt, listArg);
	fprintf (stderr, " Value '%s' should be equal to '%s'\n", value, expected);
	va_end(listArg);
	_errorCount ++;
	return False;
}

boolean
STAssertSubstring(char *value, char *superString, char *fmt, ...) {
	_testCount ++;
	if	(strstr(value, superString))
		return	True;
	va_list listArg;
	va_start(listArg,fmt);
	fprintf (stderr, "*** ");
	vfprintf (stderr, fmt, listArg);
	fprintf (stderr, " Value '%s' should be a substring of '%s'\n", value, superString);
	va_end(listArg);
	_errorCount ++;
	return False;
}

boolean
STAssertBinary(unsigned char *buf, int sz1, unsigned char *expected, int sz2, char *fmt, ...) {
	va_list listArg;
	_testCount ++;
	if	(sz1 != sz2) {
		va_start(listArg,fmt);
		fprintf (stderr, "*** ");
		vfprintf (stderr, fmt, listArg);
		fprintf (stderr, " Buffer size %d should be %d\n", sz1, sz2);
		va_end(listArg);
		_errorCount ++;
		return False;
	}
	int i;
	for	(i=0; i<sz1; i++) {
		if	(buf[i] != expected[i]) {
			va_start(listArg,fmt);
			fprintf (stderr, "*** ");
			vfprintf (stderr, fmt, listArg);
			fprintf (stderr, " Buffer[%d]=%d should be %d\n", i, buf[i], expected[i]);
			va_end(listArg);
			_errorCount ++;
			return False;
		}
	}
	return	True;
}

void
STInit() {
	_testCount = 0;
	_errorCount = 0;
}

int STErrorCount() {
	return _errorCount;
}

void STStatistics() {
	fprintf (stderr, "Unit Test Statistics\n");
	fprintf (stderr, "  %d tests passed. %d success. %d failed\n", _testCount, _testCount-_errorCount, _errorCount);
}
