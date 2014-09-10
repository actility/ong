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
#include <time.h>
#include <sys/time.h>

int main(int ac, char **av) {
	struct timeval tv;
	if	(gettimeofday(&tv, NULL) < 0) {
		perror ("gettimeofday");
		return	-2;
	}
	printf ("%ld : %ld usec\n", tv.tv_sec, tv.tv_usec);

	struct timespec ts;
	if	(clock_gettime(CLOCK_MONOTONIC, &ts) < 0) {
		perror ("clock_gettime");
		return	-2;
	}
	printf ("%ld : %ld ns\n", ts.tv_sec, ts.tv_nsec);
	return	0;
}
