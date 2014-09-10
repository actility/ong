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
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>

/*
 *	This is a dynamic sprintf, allocating and reallocating buffer on demand
 * size = 0 means no limitation
 */
int iec_vasnprintf(char **buf, size_t size, const char *format, va_list arg) {
	int rv = vsnprintf(NULL, 0, format, arg);
	if (rv < 0) return -1;

	int szbuf = *buf ? strlen(*buf) : 0;

	// Size limitation
	if	((size > 0) && (rv > size - szbuf))
		rv	= size - szbuf;

	if	(*buf) {
		*buf	= realloc(*buf, szbuf+rv+10);
	}
	else {
		*buf	= malloc(rv+10);
		**buf = 0;
	}

	if	(!*buf)
		return -1;
	return vsnprintf(*buf+szbuf, rv+1, format, arg);
}

int iec_asnprintf(char **buf, size_t size, const char *format, ...) {
	va_list arg;
	int rv;

	va_start(arg, format);
	rv = iec_vasnprintf(buf, size, format, arg);
	va_end(arg);
	return rv;
}

int iec_asprintf(char **buf, const char *format, ...) {
	va_list arg;
	int rv;

	va_start(arg, format);
	rv = iec_vasnprintf(buf, 0, format, arg);
	va_end(arg);
	return rv;
}

int iec_vasprintf(char **buf, const char *format, va_list arg) {
	return iec_vasnprintf (buf, 0, format, arg);
}
