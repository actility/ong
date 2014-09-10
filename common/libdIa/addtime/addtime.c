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
#include <sys/time.h>

int main(int ac, char **av) {
	struct timeval tv;

	if	(ac < 2) {
		fprintf (stderr, "Syntax addtime (delta_seconds)\n");
		return	-1;
	}
	int	delta = atoi(av[1]);

	if	(gettimeofday(&tv, NULL) < 0) {
		perror ("gettimeofday");
		return	-2;
	}
	tv.tv_sec	+= delta;
	if	(settimeofday(&tv, NULL) < 0) {
		perror ("settimeofday");
		return	-2;
	}
	return	0;
}
