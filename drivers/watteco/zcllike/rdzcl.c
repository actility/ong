
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

char *Recv6 = "::";
//char *Recv6 = "aaaa::1";
int Recvport = 47002;
struct sockaddr_in6 Recv;


int main(int argc,char *argv[])
{
	int	fdRecv;

	signal	(SIGPIPE,SIG_IGN);
	if	(argc > 1 && strcmp(argv[1],"--version") == 0)
	{
		printf	("%s\n",watteco_whatStr);
		exit(0);
	}

	Recv.sin6_family = AF_INET6;
	Recv.sin6_port = htons(Recvport);

	if (inet_pton(AF_INET6,Recv6,&Recv.sin6_addr) <= 0)
	{
		err(1,"cannot convert addr '%s'\n",Recv6);
		exit(1);
	}

	fdRecv	= socket(AF_INET6,SOCK_DGRAM,0);
	if (fdRecv < 0)
	{
		err(1,"cannot create socket recv\n");
		exit(1);
	}
	if (bind(fdRecv,(struct sockaddr *)&Recv,sizeof(Recv)) < 0)
	{
		err(1,"cannot bind recv\n");
		exit(1);
	}

	while (1)
	{
		int	sz;
		int	i;
		unsigned char	buf[4000];
		struct sockaddr_in6 from;
		socklen_t	lenaddr;
		int	port;
           	char str[INET6_ADDRSTRLEN];

	
		lenaddr	= sizeof(from);
		if ((sz=recvfrom(fdRecv,buf,sizeof(buf),0,
				(struct sockaddr *)&from,&lenaddr)) > 0)
		{
			port	= ntohs(from.sin6_port);
			str[0]	= '\0';
			if (inet_ntop(AF_INET6,&from.sin6_addr,str,INET6_ADDRSTRLEN))
				printf("[%s]:%d => ",str,port);
			else
				printf("[???]:%d => ",port);
			for (i = 0 ; i < sz ; i++)
				printf("%02x ",buf[i]);
			printf("\n");
		}
	}

	exit(0);
}
