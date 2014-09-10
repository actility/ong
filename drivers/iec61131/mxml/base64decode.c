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
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <rtlbase.h>

char input[100*1024];

int main(int ac, char **av) {
	// no checks !
	int fd = open(av[1], 0);
	long sz = read (fd, input, sizeof(input));
	input[sz] = 0;
	close (fd);

	printf ("[%s]\n", input);
	int target_size = (double)strlen(input)*3.0/4.0 + 10;
	printf ("### %d %d\n", strlen(input), target_size);
	char *bigbuff = malloc(target_size);
	sz = rtl_base64_decode (bigbuff, input, strlen(input));
	if	(sz < 0) {
		printf ("ERROR\n");
		return -1;
	}

	bigbuff[sz] = 0;
	printf ("[%s]\n", bigbuff);
	return 0;
}
