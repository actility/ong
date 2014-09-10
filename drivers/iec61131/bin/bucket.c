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
#include <stddef.h>
#include <strings.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <poll.h>
#include <math.h>

#include <mxml.h>
#include <rtlbase.h>
#include <rtllist.h>
#include <rtlhtbl.h>
#include <dIa.h>

#include "iec_debug.h"
#include "iec_config.h"
#include "plc.h"
#include "literals.h"
#include "timeoper.h"
#include "iec_std_lib.h"
#include "vasprintf.h"
#include "tools.h"
#include "operating.h"
#include "xpath.h"
#include "cli.h"

#include "bucket.h"


extern iec_global_t GLOB;

double getXmlDurationDouble(char *str) {
	if	(str[0] == 'P' && str[1] == 'T') {
		str += 2;
		if	(str[strlen(str)-1] == 'S')
			str[strlen(str)-1] = 0;
	}
	return	atof(str);
}
void User_BucketInit(iec_user_t *user) {
	user->m_bucket.rate = 0.2;
	user->m_bucket.size = 50;
	if	(user->m_dictionnary) {
		char *res;
		res = User_GetDictionnary(user, "iecOutTokenBucketRate");
		if	(res) {
			user->m_bucket.rate = getXmlDurationDouble(res);
		}
		res = User_GetDictionnary(user, "iecOutTokenBucketSize");
		if	(res)
			user->m_bucket.size = atoi(res);
	}
	//	The bucket is full at startup : max tokens allowed
	user->m_bucket.count = user->m_bucket.size;
	//user->m_bucket.count = 0;
	gettimeofday (&user->m_bucket.last, NULL);
	RTL_TRDBG (TRACE_DETAIL, "User_BucketInit\n");
}

void User_BucketTime(iec_user_t *user) {
	double sec;
	struct timeval tv, last;

	if	(!user)
		return;

	gettimeofday (&tv, NULL);
	last = tv;
	SUB_TIMEVAL (tv, user->m_bucket.last);
	user->m_bucket.last = last;

	sec	= tv.tv_sec + (double)tv.tv_usec/1000000.0;
	user->m_bucket.count	+= sec / user->m_bucket.rate;

	if	(user->m_bucket.count > user->m_bucket.size)
		user->m_bucket.count = user->m_bucket.size;

	//RTL_TRDBG (TRACE_DETAIL, "User_BucketTime sec=%f count=%f\n", sec, user->m_bucket.count);
}

void AllUsers_BucketTime() {
	struct list_head *elem;
	list_for_each (elem, &GLOB.user_list) {
		iec_user_t *user = list_entry(elem, iec_user_t, head);
		User_BucketTime (user);
	}
}

BOOL User_BucketGet(iec_user_t *user) {
	RTL_TRDBG (TRACE_DETAIL, "User_BucketGet count=%f\n", user->m_bucket.count);
	if	(user->m_bucket.count < 1) {
		User_GSC_log(user, "OUT_TOKEN_BUCKET_EXCEEEDED", "Output change exceeded token bucket regulator limit. The Output has been discarded.", "", "");
		Board_Put(&GLOB.board_errors, "Output change exceeded token bucket regulator limit. The Output has been discarded.", "", "");
		return	FALSE;
	}
	user->m_bucket.count -= 1.0;
	return	TRUE;
}
