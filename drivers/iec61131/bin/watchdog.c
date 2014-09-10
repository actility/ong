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
#include <unistd.h>
#include <pthread.h>
#ifdef HAS_GLIB
#include <glib.h>
#endif

#include "watchdog.h"

#ifdef HAS_GLIB
static volatile gint _wd_counter;
#else
static volatile unsigned int _wd_counter;
#endif

static unsigned int _wd_max;


void *iec_watchdog_main(void *arg) {
	while	(1) {
		sleep (1);
#ifdef HAS_GLIB
		if	(g_atomic_int_dec_and_test (&_wd_counter) == TRUE) {
			abort();
		}
#else
		if	(--_wd_counter == 0) {
			abort();
		}
#endif
	}
}

void iec_watchdog_start(unsigned int max) {
	pthread_attr_t attr;
	pthread_t thread_id;
	int s;

	_wd_max = max;
	iec_watchdog_reset ();

	s = pthread_attr_init(&attr);
	if	(s != 0) {
		return;
	}
	s = pthread_create(&thread_id, &attr, &iec_watchdog_main, NULL);
	if	(s != 0) {
		return;
	}
}

void iec_watchdog_reset() {
#ifdef HAS_GLIB
	 g_atomic_int_set (_wd_counter, _wd_max);
#else
	_wd_counter = _wd_max;
#endif
}
