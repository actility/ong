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


#ifndef _UNITTEST_H_
#define _UNITTEST_H_

#if !defined(False)
typedef unsigned char boolean;
#define False (boolean)0
#define True (boolean)1
#endif

extern boolean STAssertEquals(int value, int expected, char *fmt, ...);
extern boolean STAssertEqualsWithAccuracy(double value, double expected, double accuracy, char *fmt, ...);
extern boolean STAssertTrue(boolean value, char *fmt, ...);
extern boolean STAssertFalse(boolean value, char *fmt, ...);
extern boolean STAssertStringEquals(char *value, char *expected, char *fmt, ...);
extern boolean STAssertSubstring(char *value, char *superString, char *fmt, ...);
extern boolean STAssertBinary(unsigned char *buf, int sz1, unsigned char *expected, int sz2, char *fmt, ...);
extern int STErrorCount();
void STStatistics();

#endif //_UNITTEST_H_
