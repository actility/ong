
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
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <errno.h>
#include <limits.h>
#include <signal.h>
#include <poll.h>
#include <ctype.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#include "rtlbase.h"
#include "rtlimsg.h"
#include "xoapipr.h"
#include "dIa.h"

#include "adaptor.h"

#include "define.h"
#include "struct.h"

#include "cproto.h"
#include "cmnproto.h"

int	main(int argc,char *argv[])
{
	int	ret;
	char	*targetID;
	char	ident[128];
	int	cluster;
	int	app;
	int	num;
	int	level;
	int	type;

	if	(argc <= 1)
		exit(1);

	targetID= argv[1];
	ret	= WParseMethod(targetID,ident,&app,&type,&level,&cluster,&num);
	printf("ParseMethod('%s') return %d\n",targetID,ret);

	printf	("type=%d\n",type);
	printf	("ident='%s'\n",ident);
	printf	("app=%d\n",app);
	printf	("level=%d\n",level);
	printf	("cluster=%x\n",cluster);
	printf	("num=%d\n",num);

	exit(0);
}
