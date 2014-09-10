
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
#include <sys/types.h>

#include <unistd.h>
#include <errno.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

//char *pani6 = "aaaa::ff:ff00:23f4";
//char *pani6 = "aaaa::ff:ff00:2343";

char *pani6 = "aaaa::ff:ff00:2343";
int panport = 47005;

char *srce6 = "aaaa::1";
int srceport = 47004;

char *recv6 = "aaaa::1";
int recvport = 47002;


#if	0
unsigned char msg[] = { 
		0x11, 
		0x00,		// read attribut request
		0x00, 0x50,
//		0x00, 0x00,	// WIP@
		0x00, 0x01,	// PAN id
	};
#endif
#if	0
unsigned char msg[] = { 
		0x11, 
		0x00,		// read attribut request
		0x04, 0x02,	// temperatureMeasurement
		0x00, 0x00,	// measuredValue
	};
#endif
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

struct sockaddr_in6 srce;
struct sockaddr_in6 dest;
struct sockaddr_in6 Recv;


int main(int argc,char *argv[])
{
	int	fdSend;
	int	fdRecv;

	srce.sin6_family = AF_INET6;
	srce.sin6_port = htons(srceport);

	dest.sin6_family = AF_INET6;
	dest.sin6_port = htons(panport);

	Recv.sin6_family = AF_INET6;
	Recv.sin6_port = htons(recvport);

	if (inet_pton(AF_INET6,pani6,&dest.sin6_addr) <= 0)
	{
		err(1,"cannot convert addr '%s'\n",pani6);
		exit(1);
	}

	if (inet_pton(AF_INET6,srce6,&srce.sin6_addr) <= 0)
	{
		err(1,"cannot convert addr '%s'\n",srce6);
		exit(1);
	}
	if (inet_pton(AF_INET6,recv6,&Recv.sin6_addr) <= 0)
	{
		err(1,"cannot convert addr '%s'\n",recv6);
		exit(1);
	}

	fdSend	= socket(AF_INET6,SOCK_DGRAM,0);
	if (fdSend < 0)
	{
		err(1,"cannot create socket send\n");
		exit(1);
	}
	if (bind(fdSend,(struct sockaddr *)&srce,sizeof(srce)) < 0)
	{
		err(1,"cannot bind send\n");
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

	if (sendto(fdSend,msg,szmsg,0,(struct sockaddr *)&dest,sizeof(dest)) < 0)
	{
		err(1,"cannot sendto\n");
		exit(1);
	}

	while (1)
	{
		int	sz;
		int	i;
		time_t	now;
		unsigned char	buf[4000];

		
		if ((sz=recvfrom(fdRecv,buf,sizeof(buf),0,NULL,0)) > 0)
		{
			time(&now);
			printf("%s :",ctime(&now));
			for (i = 0 ; i < sz ; i++)
				printf("%02x ",buf[i]);
			printf("\n");
		}
	}

	exit(0);
}
