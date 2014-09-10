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

#include <ctype.h>
#include <string.h>

static char b64string[] =
	"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

long rtl_base64_encode (char *to, const char *from, unsigned int len) {
	const char *fromp = from;
	char *top = to;
	unsigned char cbyte;
	unsigned char obyte;
	char end[3];

	for (; len >= 3; len -= 3) {
		cbyte = *fromp++;
		*top++ = b64string[(int)(cbyte >> 2)];
		obyte = (cbyte << 4) & 0x30;		/* 0011 0000 */

		cbyte = *fromp++;
		obyte |= (cbyte >> 4);			/* 0000 1111 */
		*top++ = b64string[(int)obyte];
		obyte = (cbyte << 2) & 0x3C;		/* 0011 1100 */

		cbyte = *fromp++;
		obyte |= (cbyte >> 6);			/* 0000 0011 */
		*top++ = b64string[(int)obyte];
		*top++ = b64string[(int)(cbyte & 0x3F)];/* 0011 1111 */
	}

	if (len) {
		end[0] = *fromp++;
		if (--len) end[1] = *fromp++; else end[1] = 0;
		end[2] = 0;

		cbyte = end[0];
		*top++ = b64string[(int)(cbyte >> 2)];
		obyte = (cbyte << 4) & 0x30;		/* 0011 0000 */

		cbyte = end[1];
		obyte |= (cbyte >> 4);
		*top++ = b64string[(int)obyte];
		obyte = (cbyte << 2) & 0x3C;		/* 0011 1100 */

		if (len) *top++ = b64string[(int)obyte];
		else *top++ = '=';
		*top++ = '=';
	}
	*top = 0;
	return top - to;
}

/* badchar(): check if c is decent; puts either the */
/* location of c or null into p.                  */
#define badchar(c,p) (!(p = memchr(b64string, c, 64)))

long rtl_base64_decode (char *to, const char *from, unsigned int len) {
	const char *fromp = from;
	char *top = to;
	char *p;
	unsigned char cbyte;
	unsigned char obyte;
	int padding = 0;

	//for (; len >= 4; len -= 4) {
	while (len >= 4) {
		cbyte = *fromp++;
		if	(isspace(cbyte)) {
			len --;
			continue;
		}
		if (cbyte == '=') cbyte = 0;
		else {
			if (badchar(cbyte, p)) return -1;
			cbyte = (p - b64string);
		}
		obyte = cbyte << 2;		/* 1111 1100 */

		if ((cbyte = *fromp++) == '=') cbyte = 0;
		else {
			if (badchar(cbyte, p)) return -1;
			cbyte = p - b64string;
		}
		obyte |= cbyte >> 4;		/* 0000 0011 */
		*top++ = obyte;

		obyte = cbyte << 4;		/* 1111 0000 */
		if ((cbyte = *fromp++) == '=') { cbyte = 0; padding++; }
		else {
			padding = 0;
			if (badchar (cbyte, p)) return -1;
			cbyte = p - b64string;
		}
		obyte |= cbyte >> 2;		/* 0000 1111 */
		*top++ = obyte;

		obyte = cbyte << 6;		/* 1100 0000 */
		if ((cbyte = *fromp++) == '=') { cbyte = 0; padding++; }
		else {
			padding = 0;
			if (badchar (cbyte, p)) return -1;
			cbyte = p - b64string;
		}
		obyte |= cbyte;			/* 0011 1111 */
		*top++ = obyte;
		len -= 4;
	}

	*top = 0;
	//if (len) return -1;
	return (top - to) - padding;
}

