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

/*
	Usefull functions to input formated IEC61131 data
	For example : TIME getDuration("TIME#10h30m15s");
 */

#include <stdio.h>
#include <stddef.h>
#include <strings.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>
#include <poll.h>

#include <mxml.h>

#include <rtlbase.h>
#include <rtllist.h>
#include <rtlhtbl.h>

#include "plc.h"

#include "iec_std_lib.h"


static char *cleanup(char *str) {
	char	*out = malloc(strlen(str)+1);
	char	*p = out;
	while	(*str) {
		if	(*str != '_')
			*p++	= *str;
		str++;
	}
	*p	= 0;
	return	out;
}

IEC_BOOL getBoolean(char *str) {
	BOOL	b = FALSE;
	if	(!strcasecmp(str, "TRUE") || !strcmp(str, "1"))
		b	= TRUE;
	return	b;
}

IEC_DINT getInteger(char *str) {
	char	*p = cleanup(str), *p1;
	IEC_DINT	i;

	char *base = strchr(p, '#');
	if	(base) {
		*base	= 0;
		p1	= base+1;
		base	= p;
	}
	else {
		base	= "10";
		p1	= p;
	}
	//printf ("### base=%s val=%s\n", base, p1);
	i	= strtol (p1, NULL, atoi(base));
	free (p);
	return i;
}

IEC_LREAL getLReal(char *str) {
	char	*p = cleanup(str);
	IEC_LREAL	r = strtod(p, NULL);
	free (p);
	return r;
}

IEC_REAL getReal(char *str) {
	char	*p = cleanup(str);
	IEC_REAL	r = strtof(p, NULL);
	free (p);
	return r;
}

#if 0 // UNUSED
/**
 * @brief Converts a value into its mantissa and base10 exponent
 * @param[in] val The value to convert
 * @param[out] exponent The base10 exponent
 * @return The mantissa
 */
static int64_t normalize_float(char *val, int16_t *exponent) {
	char *pt;
	*exponent = 0;

	// 1. Removes zeroes at left
	for (pt=val; *pt; pt++) {
		if	(*pt != '0')
			break;
	}
	val	= pt;

	// 2. Removes zeroes at right
	for (pt = val+strlen(val)-1; pt >= val; pt--) {
		if	(*pt != '0')
			break;
		*pt	= 0;
	}

	// 3. Counts digits at right
	pt	= strchr(val, '.');
	if	(pt) {
		int cnt = 1;
		while	(*pt) {
			*pt	= *(pt+1);
			pt++;
			cnt --;
		}
		*exponent = cnt;
	}
	return	atoll(val);
}
#endif // UNUSED

char getByte(char **pt) {
	char ret;
	if	(**pt == '$') {
		(*pt)	++;
		if	(**pt == '$') {
			(*pt)	++;
			return	'$';
		}
		if	(**pt == '\'') {
			(*pt)	++;
			return	'\'';
		}
		if	(**pt == 'L' || **pt == 'l') {
			(*pt)	++;
			return	0x0a;		// LF
		}
		if	(**pt == 'N' || **pt == 'n') {
			(*pt)	++;
			return	0x0a;		// Newline ???
		}
		if	(**pt == 'P' || **pt == 'p') {
			(*pt)	++;
			return	0x0c;		// FF
		}
		if	(**pt == 'R' || **pt == 'r') {
			(*pt)	++;
			return	0x0d;		// CR
		}
		if	(**pt == 'T' || **pt == 't') {
			(*pt)	++;
			return	0x08;		// Tab
		}
		if	(**pt == '\"') {
			(*pt)	++;
			return	'\"';
		}
		char tmp[3];
		tmp[0] = **pt;
		(*pt)	++;
		tmp[1] = **pt;
		(*pt)	++;
		tmp[2] = 0;
		ret	= strtol (tmp, NULL, 16);
		return	ret;
	}
	ret = **pt;
	(*pt)	++;
	return	ret;
}

IEC_STRING getString(char *str) {
	STRING	res;
	res.len	= 0;
	if	(*str == '\'') {
		char *pt	= str+1;
		char	*body = (char *)res.body;
		while	(*pt != '\'') {
			*body++	= getByte(&pt);
			res.len	++;
		}
	}
	else if	(*str == '\"') {
		char *pt	= str+1;
		char	*body = (char *)res.body;
		while	(*pt != '\"') {
			*body++	= getByte(&pt);
			res.len	++;
		}
	}
	return	res;
}

IEC_TIME getDuration(char *str) {
	TIME	t = {0,0};
	char	*p = cleanup(str), *p1;

	if	(!memcmp(p, "T#", 2))
		p1	= p+2;
	else if	(!memcmp(p, "t#", 2))
		p1	= p+2;
	else if	(!memcmp(p, "TIME#", 5))
		p1	= p+5;
	else if	(!memcmp(p, "time#", 5))
		p1	= p+5;
	else {
		free (p);
		return	t;
	}

	int	sign = 1;
	double ms=0, s=0, m=0, h=0, d=0;

	if	(*p1 == '-') {
		sign	= -1;
		p1	++;
	}

	char num[30];
	char alpha[30];
	char *p2;

	while	(*p1) {
		p2	= num;
		while	(isdigit(*p1) || (*p1 == '.'))
			*p2++ = *p1++;
		*p2 = 0;
		p2	= alpha;
		while	(isalpha(*p1))
			*p2++ = *p1++;
		*p2 = 0;

		//printf ("(%s) (%s)\n", num, alpha);
		double val = strtod(num,NULL);
		if	(!strcmp(alpha, "d"))
			d	= val;
		else if	(!strcmp(alpha, "h"))
			h	= val;
		else if	(!strcmp(alpha, "m"))
			m	= val;
		else if	(!strcmp(alpha, "s"))
			s	= val;
		else if	(!strcmp(alpha, "ms"))
			ms	= val;
	}

	t	= __time_to_timespec(sign, ms, s, m, h, d);

	free (p);
	return	t;
}

IEC_DATE getDate(char *str) {
	DATE	date = {0,0};
	char	*p = cleanup(str), *p1;

	if	(!memcmp(p, "D#", 2))
		p1	= p+2;
	else if	(!memcmp(p, "d#", 2))
		p1	= p+2;
	else if	(!memcmp(p, "DATE#", 5))
		p1	= p+5;
	else if	(!memcmp(p, "date#", 5))
		p1	= p+5;
	else {
		free (p);
		return	date;
	}

	int day=0, m=0, y=0;

	char	*elem;
	elem = strchr(p1, '-');
	if	(elem) {
		*elem = 0;
		y	= atoi(p1);
		p1	= elem+1;
	}
	elem = strchr(p1, '-');
	if	(elem) {
		*elem = 0;
		m	= atoi(p1);
		p1	= elem+1;
	}
	day	= atoi(p1);

	date	= __date_to_timespec(day, m, y);

	free (p);
	return	date;
}

IEC_TOD getTimeOfDay(char *str) {
	TOD	tod = {0,0};
	char	*p = cleanup(str), *p1;

	//	!! : cleanup has removed all '_'

	if	(!memcmp(p, "TOD#", 4))
		p1	= p+4;
	else if	(!memcmp(p, "tod#", 4))
		p1	= p+4;
	else if	(!memcmp(p, "TIMEOFDAY#", 10))
		p1	= p+10;
	else if	(!memcmp(p, "timeofday#", 10))
		p1	= p+10;
	else {
		free (p);
		return	tod;
	}

	double h=0, m=0, s=0; // *LNI* int_8 are more than enough here
                          // but means that __tod_to_timespec needs to be changed

	char	*elem;
	elem = strchr(p1, ':');
	if	(elem) {
		*elem = 0;
		h	= strtod(p1,NULL);
		p1	= elem+1;
	}
	elem = strchr(p1, ':');
	if	(elem) {
		*elem = 0;
		m	= strtod(p1,NULL);
		p1	= elem+1;
	}
	s	= strtod(p1,NULL);

	tod	= __tod_to_timespec(s, m, h);

	free (p);
	return	tod;
}

IEC_DT getDateAndTime(char *str) {
	DT	dt = {0,0};
	char	*p = cleanup(str), *p1;

	//	!! : cleanup has removed all '_'

	if	(!memcmp(p, "DT#", 3))
		p1	= p+3;
	else if	(!memcmp(p, "dt#", 3))
		p1	= p+3;
	else if	(!memcmp(p, "DATEANDTIME#", 12))
		p1	= p+12;
	else if	(!memcmp(p, "dateandtime#", 12))
		p1	= p+12;
	else {
		free (p);
		return	dt;
	}

	double h=0, min=0, s=0;
	int	d=-1, m=-1, y=-1;
	BOOL caret = FALSE;

	char	*elem;
	elem = strchr(p1, '-');
	if	(elem) {
		*elem = 0;
		y	= atoi(p1);
		p1	= elem+1;
		caret = TRUE;
	}
	elem = strchr(p1, '-');
	if	(elem) {
		*elem = 0;
		m	= atoi(p1);
		p1	= elem+1;
		caret = TRUE;
	}

	elem = strchr(p1, '-');
	if	(elem) {
		*elem = 0;
		d	= atoi(p1);
		p1	= elem+1;
		caret = TRUE;
	}
	else if (caret)
		d	= atoi(p1);

	elem = strchr(p1, ':');
	if	(elem) {
		*elem = 0;
		h	= strtod(p1,NULL);
		p1	= elem+1;

		elem = strchr(p1, ':');
		if	(elem) {
			*elem = 0;
			min	= strtod(p1,NULL);
			p1	= elem+1;
			s	= strtod(p1,NULL);
		}
		else {
			min	= strtod(p1,NULL);
		}
	}

	dt	= __dt_to_timespec(s, min, h, d, m, y);

	free (p);
	return	dt;
}

char *DATE2String(IEC_DATE d) {
	static char buf[14];
	struct tm tm;
	tm	= *localtime(&d.tv_sec);

	sprintf (buf, "DT#%04d-%02d-%02d",
		tm.tm_year+1900, tm.tm_mon+1, tm.tm_mday);
	return buf;
}

char *TOD2String(IEC_TOD tod) {
	static char buf[13];
	struct tm tm;
	tm	= *localtime(&tod.tv_sec);

	sprintf (buf, "TOD#%02d:%02d:%02d",
		tm.tm_hour, tm.tm_min, tm.tm_sec);
	return buf;
}

char *DT2String(IEC_DT dt) {
	static char buf[23];
	struct tm tm;
	tm	= *localtime(&dt.tv_sec);

	sprintf (buf, "DT#%04d-%02d-%02d-%02d:%02d:%02d",
		tm.tm_year+1900, tm.tm_mon+1, tm.tm_mday,
		tm.tm_hour, tm.tm_min, tm.tm_sec);
	return buf;
}

static char *_charsOnInt="0123456789-+#abcdefABCDEF_";
BOOL isInteger(char *str) {
	while	(*str) {
		if	(!strchr(_charsOnInt, *str))
			return	FALSE;
		str ++;
	}
	return TRUE;
}

int getTypeOfLiteral(char *str) {
	if	(!strncasecmp(str, "DINT#", 5))
		return	DINTtype;
	if	(!strncasecmp(str, "REAL#", 5))
		return	REALtype;
	if	(!strncasecmp(str, "LREAL#", 6))
		return	LREALtype;
	if	(!strncasecmp(str, "BOOL#", 5))
		return	BOOLtype;
	if	(!strncasecmp(str, "T#", 2))
		return	TIMEtype;
	if	(!strncasecmp(str, "TIME#", 5))
		return	TIMEtype;
	if	(!strncasecmp(str, "D#", 2))
		return	DATEtype;
	if	(!strncasecmp(str, "DATE#", 5))
		return	DATEtype;
	if	(!strncasecmp(str, "TOD#", 4))
		return	TODtype;
	if	(!strncasecmp(str, "DT#", 3))
		return	DTtype;
	return -1;
}

param_ret_t *getLiteral(char *str) {
	if	(!str)
		return NULL;
	static param_ret_t pret;

	if	(!strncasecmp(str, "DINT#", 5)) {
		pret.t	= DINTtype;
		pret.p.DINTvar	= getInteger(str+5);
	}
	else if	(!strncasecmp(str, "REAL#", 5)) {
		pret.t	= REALtype;
		pret.p.REALvar	= getReal(str+5);
	}
	else if	(!strncasecmp(str, "LREAL#", 6)) {
		pret.t	= LREALtype;
		pret.p.LREALvar	= getLReal(str+6);
	}
	else if	(!strncasecmp(str, "BOOL#", 5)) {
		pret.t	= BOOLtype;
		pret.p.BOOLvar	= getBoolean(str+5);
	}
	else if	(!strncasecmp(str, "T#", 2)) {
		pret.t	= TIMEtype;
		pret.p.TIMEvar	= getDuration(str);
	}
	else if	(!strncasecmp(str, "TIME#", 5)) {
		pret.t	= TIMEtype;
		pret.p.TIMEvar	= getDuration(str);
	}
	else if	(!strncasecmp(str, "D#", 2)) {
		pret.t	= DATEtype;
		pret.p.DATEvar	= getDate(str);
	}
	else if	(!strncasecmp(str, "DATE#", 5)) {
		pret.t	= DATEtype;
		pret.p.DATEvar	= getDate(str);
	}
	else if	(!strncasecmp(str, "TOD#", 4)) {
		pret.t	= TODtype;
		pret.p.TODvar	= getTimeOfDay(str);
	}
	else if	(!strncasecmp(str, "DT#", 3)) {
		pret.t	= DTtype;
		pret.p.DTvar	= getDateAndTime(str);
	}
	else if	(!strncasecmp(str, "STRING#", 7)) {
		pret.t	= STRINGtype;
		pret.p.STRINGvar	= getString(str+7);
	}
	else {
		pret.t	= DINTtype;
		pret.p.DINTvar	= getInteger(str);
	}

	return &pret;
}

