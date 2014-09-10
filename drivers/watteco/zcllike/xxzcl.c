
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
#include <time.h>
#include <signal.h>
#include <ctype.h>
#include <sys/types.h>

#include <unistd.h>
#include <errno.h>
#include <err.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#include "_whatstr.h"
#include "aw.h"
#include "watteco.h"

char *Pani6 = "bbbb::ff:ff00:1605";
int Panport = 47005;

char *Srce6 = "bbbb::1";
//char *Srce6 = "fe80::1";
int Srceport = 47004;

#if	0
unsigned char msg[] = { 
		0x11, 
		0x00,		// read attribut request
		0x00, 0x50,
//		0x00, 0x00,	// WIP@
		0x00, 0x01,	// PAN id
	};
unsigned char msg[] = { 
		0x11, 
		0x00,		// read attribut request
		0x04, 0x02,	// temperatureMeasurement
		0x00, 0x00,	// measuredValue
	};
unsigned char msg[] = { 
		0x11, 
		0x06,		// configure reporting
		0x04, 0x02,	// temperatureMeasurement
		0x00, 0x00,	// measuredValue
		0x00, 0x29,
		0x00, 0x00,	// min(s)
		0x00, 0x00,	// max(s)
		0x00, 0x10,	// 1/100°
	};

int szmsg = sizeof(msg) / sizeof(char);
#endif

struct sockaddr_in6 Srce;
struct sockaddr_in6 Dest;

int main(int argc,char *argv[])
{
	int	fdSend;
	unsigned char	*msg = NULL;
	unsigned char	bin[1000];
	unsigned char	dump[1000];
	int		szmsg = sizeof(bin);

	signal	(SIGPIPE,SIG_IGN);
	if	(argc > 1 && strcmp(argv[1],"--version") == 0)
	{
		printf	("%s\n",watteco_whatStr);
		exit(0);
	}

	if (argc != 2)
	{
		printf("default command : ask for PanID\n");
		msg	= rtl_strToBin((char *)"11,00,0050,0001",bin,&szmsg);
	}
	else
	{
		szmsg	= sizeof(bin);
		msg	= rtl_strToBin((char *)argv[1],bin,&szmsg);
	}
	if (!msg || szmsg <= 0)
	{
		err(1,"bad format\n");
		exit(1);
	}

	rtl_binToStr(bin,szmsg,(char *)dump,sizeof(dump)-10);
	printf	("[%s]\n",dump);



	Srce.sin6_family = AF_INET6;
	Srce.sin6_port = htons(Srceport);

	Dest.sin6_family = AF_INET6;
	Dest.sin6_port = htons(Panport);

	if (inet_pton(AF_INET6,Pani6,&Dest.sin6_addr) <= 0)
	{
		err(1,"cannot convert addr '%s'\n",Pani6);
		exit(1);
	}

	if (inet_pton(AF_INET6,Srce6,&Srce.sin6_addr) <= 0)
	{
		err(1,"cannot convert addr '%s'\n",Srce6);
		exit(1);
	}

	fdSend	= socket(AF_INET6,SOCK_DGRAM,0);
	if (fdSend < 0)
	{
		err(1,"cannot create socket send\n");
		exit(1);
	}
/*
*/
	if (bind(fdSend,(struct sockaddr *)&Srce,sizeof(Srce)) < 0)
	{
		err(1,"cannot bind\n");
		exit(1);
	}

	if (sendto(fdSend,msg,szmsg,0,(struct sockaddr *)&Dest,sizeof(Dest)) < 0)
	{
		err(1,"cannot sendto\n");
		exit(1);
	}

	printf("ok send sz=%d\n",szmsg);


	exit(0);
}
