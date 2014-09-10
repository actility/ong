
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

/*! @file zcludp.c
 *
 */
#include <stdio.h>			/************************************/
#include <stdlib.h>			/*     run in main thread           */
#include <string.h>			/************************************/
#include <stddef.h>
#include <unistd.h>
#include <pthread.h>
#include <errno.h>
#include <limits.h>
#include <signal.h>
#include <poll.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#include "rtlbase.h"
#include "rtlimsg.h"
#include "rtllist.h"

#include "adaptor.h"

#include "define.h"
#include "struct.h"

#include "cproto.h"
#include "cmnproto.h"
#include "extern.h"

typedef	struct
{
	char		*um_addr;
	char		*um_data;
	int		um_size;
	struct	list_head	list;
}	t_udp_msg;

static	char	*Prefi6;
static	int	Recvport = 47002;	// TODO
static	struct sockaddr_in6 Recv;
static	int	FdRecv = -1;

static	struct	list_head	MsgList;

unsigned	int	ZclSendErr;
unsigned	int	ZclInetErr;
unsigned	int	ZclMsgPending;
unsigned	int	ZclMsgMaxPending;

/*!
 *
 * @brief Call back function for rtlPoll to get the requested events.
 * This call back function is raised when rtlpoll needs to set the appropriate
 * POLL flags for the provided file descriptor. Here we have an UDP socket, we
 * request for POLLIN only.
 * @param tb the table of all polled file descriptors
 * @param fd the file descriptor on which events need to be detected
 * @param r1 user's data placeholder #1
 * @param r2 user's data placeholder #2
 * @param revents the current requested events
 * @return -1 in case of error detection, 0 otherwise
 * 
 */

int	CB_ZclUdpRequest(void *tb,int fd,void *r1,void *r2,int revents)
{
	RTL_TRDBG(9,"callback request zcl/udp fd=%d FD=%d e=%d r1=%lx r2=%lx\n",
			fd,FdRecv,revents,r1,r2);

	if (fd != FdRecv)
	{
		RTL_TRDBG(0,"fd=%d != myfd=%d\n",fd,FdRecv);
		rtl_pollRmv(MainTbPoll,fd);
		return	-1;
	}

#if	0
	// if we want to control POLLOUT event
	if	(ZclUdpNeedWrite())
	{
		return	POLLIN|POLLOUT;
	}
#endif

	return	POLLIN;
}

/*!
 *
 * @brief Call back function for rtlPoll to treat the detected events.
 * This call back function is raised when rtlpoll detects events on the 
 * the provided file descriptor. Here the file descriptor is an UDP socket.
 * If POLLIN is detected we try to read data 
 * @param tb the table of all polled file descriptors
 * @param fd the file descriptor on which events were detected
 * @param r1 user's data placeholder #1
 * @param r2 user's data placeholder #2
 * @param revents the current detected events
 * @return -1 in case of error detection, 0 otherwise
 * 
 */
int	CB_ZclUdpEvent(void *tb,int fd,void *r1,void *r2,int revents)
{
	int	sz;
	int	port;
	unsigned char	buf[4000];
	struct sockaddr_in6 from;
	socklen_t	lenaddr;
        char str[INET6_ADDRSTRLEN];
        char sensor[INET6_ADDRSTRLEN];
	unsigned char numsensor;

	RTL_TRDBG(7,"callback event zcl/udp fd=%d FD=%d e=%d r1=%lx r2=%lx\n",
			fd,FdRecv,revents,r1,r2);

	if (fd != FdRecv)
	{
		RTL_TRDBG(0,"fd=%d != myfd=%d\n",fd,FdRecv);
		rtl_pollRmv(MainTbPoll,fd);
		return	-1;
	}

	if ((revents & POLLOUT) == POLLOUT)
	{
		RTL_TRDBG(7,"event write zcl/udp fd=%d FD=%d\n",fd,FdRecv);
#if	0
		// if we have something to write
		if	(ZclUdpNeedWrite())
		{
		}
#endif
	}

	if ((revents & POLLIN) == POLLIN)
	{
		RTL_TRDBG(5,"event read zcl/udp fd=%d FD=%d\n",fd,FdRecv);
		lenaddr	= sizeof(from);
		if	((sz=recvfrom(fd,buf,sizeof(buf),0,
				(struct sockaddr *)&from,&lenaddr)) <= 0)
		{
			RTL_TRDBG(0,"read error on zcludp err=%d ret=%d\n",errno,sz);
			return	0;
		}

		str[0]	= '\0';
		port	= ntohs(from.sin6_port);
		inet_ntop(AF_INET6,&from.sin6_addr,str,INET6_ADDRSTRLEN);

		if	(SoftwareSensor && buf[0] != 0x11)
		{	// TODO simulation => pas en exploitation ???
			numsensor	= buf[0];
			buf[0]		= 0x11;
			sprintf		(sensor,"%d@%s",numsensor,str);
			RTL_TRDBG(5,"read zcl/udp softw sz=%d from '%s'+%d\n",
							sz,sensor,port);
			SensorZclEventForNumSoftw(numsensor,sensor,buf,sz);
			if	( ActiveSniffer > 0)
				AdmZclProtoTee(0,sensor,(char *)buf,sz);
			return	0;
		}
		RTL_TRDBG(5,"read zcl/udp message sz=%d from '%s'+%d\n",
						sz,str,port);
		SensorZclEventFor(str,(unsigned char *)buf,sz);
		if	( ActiveSniffer > 0)
			AdmZclProtoTee(0,str,(char *)buf,sz);
	}

	return	0;
}





static	int	SoftSendMessage(char *addr,char *zdata,int zlg)
{
	static	int	fdsend	= -1;
	static	int	failure	= 0;
	struct sockaddr_in6 srce;
	char	*srceaddr = "::1";	// TODO
	int	srceport = 47004;	// TODO
	struct sockaddr_in6 dest;
	int	sendport = 47005;	// TODO
	char	*softw;
	unsigned char	numsoftw;
	unsigned char	sv;
/*
	if	(failure)
		return	-1;
*/

	if	(SoftwareSensor <= 0)
	{
		RTL_TRDBG(0,"softawre sensor not configured and call to SoftSendMessage() for '%s'\n",addr);
		return	-1;
	}
	if	(!addr)
		return	-1;

	if	(fdsend < 0)
	{
		memset(&srce,0,sizeof(srce));
		srce.sin6_family = AF_INET6;
		srce.sin6_port = htons(srceport);
		if (inet_pton(AF_INET6,srceaddr,&srce.sin6_addr) <= 0)
		{
			failure	= 1;
			RTL_TRDBG(0,"cannot convert srceaddr '%s'\n",srceaddr);
			return	-2;
		}
		fdsend	= socket(AF_INET6,SOCK_DGRAM,0);
		if (fdsend < 0)
		{
			failure	= 1;
			RTL_TRDBG(0,"cannot open sock srceaddr '%s'\n",srceaddr);
			return	-3;
		}
		if (bind(fdsend,(struct sockaddr *)&srce,sizeof(srce)) < 0)
		{
			failure	= 1;
			RTL_TRDBG(0,"cannot bind sock srceaddr '%s'\n",srceaddr);
			return	-4;
		}
		RTL_TRDBG(0,"ok bind/sendto zclsoft/udp '%s':%d fd=%d\n",
					srceaddr,srceport,fdsend);
	}

	numsoftw=0;
	sv	= zdata[0];
	softw	= strchr(addr,'@');	// num/ipv6
	if	(!softw)
		return	-5;
	numsoftw= atoi(addr);
	addr	= softw+1;
	zdata[0]= numsoftw;

	memset(&dest,0,sizeof(dest));
	dest.sin6_family = AF_INET6;
	dest.sin6_port = htons(sendport);
	if (inet_pton(AF_INET6,addr,&dest.sin6_addr) <= 0)
	{
		zdata[0] = sv;
		RTL_TRDBG(0,"cannot convert destaddr '%s'\n",addr);
		ZclInetErr++;
		return	-5;
	}
	if (sendto(fdsend,zdata,zlg,0,(struct sockaddr *)&dest,sizeof(dest)) < 0)
	{
		zdata[0] = sv;
		RTL_TRDBG(0,"cannot write to addr '%s'\n",addr);
		ZclSendErr++;
		return	-6;
	}
	RTL_TRDBG(5,"write zcl/udp message sz=%d to '%s'+%d\n",
					zlg,addr,sendport);
	zdata[0] = sv;
	if	( ActiveSniffer > 0)
		AdmZclProtoTee(1,addr,(char *)zdata,zlg);
	return	zlg;
}


int	ZclUdpSendMessageNow(char *addr,char *zdata,int zlg)
{
	static	int	fdsend	= -1;
	static	int	failure	= 0;
	struct sockaddr_in6 srce;
	int	srceport = 47004;	// TODO
	struct sockaddr_in6 dest;
	int	sendport = 47005;	// TODO
	char	*softw;
	unsigned char	numsoftw;
	unsigned char	sv;
/*
	if	(failure)
		return	-1;
*/

	if	(!addr)
		return	-1;

	if	(strchr(addr,'@'))
		return	SoftSendMessage(addr,zdata,zlg);

	static	char	lastsrce[64];
	char	srceaddr[64];
#if	0	// tests for lora
	strcpy	(srceaddr,"aaaa::ff:ff00:2705");
#else
	strncpy	(srceaddr,addr,4);
	srceaddr[4]	= '\0';
	strcat	(srceaddr,"::1");
	if	(fdsend >= 0)
	{
		if	(strcmp(lastsrce,srceaddr) != 0)
		{
			RTL_TRDBG(1,"bind/sendto switch srce '%s' -> '%s'\n",
					lastsrce,srceaddr);
			close(fdsend);
			fdsend	= -1;
		}
	}
	strcpy	(lastsrce,srceaddr);
#endif

	if	(fdsend < 0)
	{
		memset(&srce,0,sizeof(srce));
		srce.sin6_family = AF_INET6;
		srce.sin6_port = htons(srceport);
		if (inet_pton(AF_INET6,srceaddr,&srce.sin6_addr) <= 0)
		{
			failure	= 1;
			RTL_TRDBG(0,"cannot convert srceaddr '%s'\n",srceaddr);
			return	-2;
		}
		fdsend	= socket(AF_INET6,SOCK_DGRAM,0);
		if (fdsend < 0)
		{
			failure	= 1;
			RTL_TRDBG(0,"cannot open sock srceaddr '%s'\n",srceaddr);
			return	-3;
		}
		if (bind(fdsend,(struct sockaddr *)&srce,sizeof(srce)) < 0)
		{
			failure	= 1;
			RTL_TRDBG(0,"cannot bind sock srceaddr '%s'\n",srceaddr);
			return	-4;
		}
		RTL_TRDBG(1,"ok bind/sendto zclhard/udp '%s':%d fd=%d\n",
				srceaddr,srceport,fdsend);
	}

	numsoftw=0;
	sv	= zdata[0];
	softw	= strchr(addr,'@');	// num/ipv6
	if	(softw)
	{
		numsoftw= atoi(addr);
		addr	= softw+1;
		zdata[0]= numsoftw;
	}

	memset(&dest,0,sizeof(dest));
	dest.sin6_family = AF_INET6;
	dest.sin6_port = htons(sendport);
	if (inet_pton(AF_INET6,addr,&dest.sin6_addr) <= 0)
	{
		zdata[0] = sv;
		RTL_TRDBG(0,"cannot convert destaddr '%s'\n",addr);
		ZclInetErr++;
		return	-5;
	}
	if (sendto(fdsend,zdata,zlg,0,(struct sockaddr *)&dest,sizeof(dest)) < 0)
	{
		zdata[0] = sv;
		RTL_TRDBG(0,"cannot write to addr '%s'\n",addr);
		ZclSendErr++;
		return	-6;
	}
	RTL_TRDBG(1,"write zcl/udp message sz=%d to '%s'+%d\n",
					zlg,addr,sendport);
	zdata[0] = sv;
	if	( ActiveSniffer > 0)
		AdmZclProtoTee(1,addr,(char *)zdata,zlg);
	return	zlg;
}

int	ZclGetFd()
{
	return	FdRecv;
}

void	ZclUdpBind()
{

	if (FdRecv >= 0)
	{
		RTL_TRDBG(0,"socket zcl/udp exists => close it\n");
		close(FdRecv);
		rtl_pollRmv(MainTbPoll,FdRecv);
		FdRecv = -1;
	}

	memset(&Recv,0,sizeof(Recv));
	Recv.sin6_family = AF_INET6;
	Recv.sin6_port = htons(Recvport);

#if	0
	// meme si pas de softwaresensor on se binde sur :: et plus sur aaaa::1
	// ceci pour ecouter plusieurs dongles et on attend plus que de la 
	// variable Prefi6 soit renseignee

	if (strlen(Prefi6) == 0)
		return;

	if	(!SoftwareSensor)
	{
		if (inet_pton(AF_INET6,Prefi6,&Recv.sin6_addr) <= 0)
		{
			RTL_TRDBG(0,"cannot convert bindaddr '%s'\n",Prefi6);
			return;
		}
	}
#endif

	FdRecv	= socket(AF_INET6,SOCK_DGRAM,0);
	if (FdRecv < 0)
	{
		RTL_TRDBG(0,"cannot create socket zcl/udp\n");
		return;
	}
	if (bind(FdRecv,(struct sockaddr *)&Recv,sizeof(Recv)) < 0)
	{
		close(FdRecv);
		rtl_pollRmv(MainTbPoll,FdRecv);
		FdRecv = -1;
		RTL_TRDBG(0,"cannot bind zcl/udp '%s'+%d\n",Prefi6,Recvport);
		return;
	}
	RTL_TRDBG(0,"ok bind/recv zclhard/udp '%s':%d fd=%d\n",
				"*",Recvport,FdRecv);

	rtl_pollAdd(MainTbPoll,FdRecv,CB_ZclUdpEvent,CB_ZclUdpRequest,NULL,NULL);
//	rtl_pollAdd(MainTbPoll,FdRecv,CB_ZclUdpEvent,NULL,NULL,NULL);
}

/*!
 *
 * @brief must be call periodically
 * 	- try to get IPV6 router prefixe
 * 	- if we get a prefixe (or if the prefixe changes) we bind a socket at
 * 	address "prefixe::1+47002" to receive messages from the router.
 * @param now current time in second (time(2))
 * @return void
 * 
 */
void	ZclGetPrefixeAndBind()
{
	char	*pt;
	char	addr[128];

	pt	= AwGetPanPref(addr);
	if	(!pt || !*pt)
	{	// file not ready
		RTL_TRDBG(1,"wait for PAN prefixe\n");
		return;
	}
	if	(strcmp(Prefi6,pt))
	{	// first or new PAN prefixe
		RTL_TRDBG(1,"got a new PAN prefixe '%s' -> '%s'\n",Prefi6,pt);
		if (Prefi6)
			free(Prefi6);
		Prefi6	= strdup(pt);
		ZclUdpBind();
	}
}

int	ZclUdpSendMessage(char *addr,char *zdata,int zlg)
{
	t_udp_msg	*msg;
	int		period;

	if	(!addr || !zdata || zlg <= 0)
		return	-1;


	if	(ZclMsgPending == 0)
	{
		// TODO special case : no message sent since ?
		period	= (int)(ZclPeriod * 1000.0);
	}

	msg	= (t_udp_msg *)malloc(sizeof(t_udp_msg));
	if	(!msg)
		return	-2;
	msg->um_data	= (char *)malloc(zlg);
	if	(!msg->um_data)
		return	-3;
	msg->um_addr	= (char *)malloc(strlen(addr)+1);
	if	(!msg->um_addr)
		return	-3;
	memcpy	(msg->um_data,zdata,zlg);
	strcpy	(msg->um_addr,addr);
	msg->um_size	= zlg;
	list_add_tail(&msg->list,&MsgList);
	ZclMsgPending++;
	if	(ZclMsgPending > ZclMsgMaxPending)
		ZclMsgMaxPending	= ZclMsgPending;
	return	ZclMsgPending;
}


void	ZclUdpRunQueue(int force)
{
	static	unsigned	int	count;
	t_udp_msg	*msg;
	t_udp_msg	*tmp;
	int		nb = 0;

	if	(ZclTic <= 0)
		return;

	count++;
	if	(force == 0)
	{
		if	(count % ZclTic != 0)
			return;
	}
	list_for_each_entry_safe(msg,tmp,&MsgList,list) 
	{
		list_del(&msg->list);
		ZclMsgPending--;
		ZclUdpSendMessageNow(msg->um_addr,msg->um_data,msg->um_size);
		free(msg->um_addr);
		free(msg->um_data);
		free(msg);
		nb++;
		if	(nb >= force)
			break;
	}
	if	(nb > 0)
	{
		RTL_TRDBG(1,"ZclUdpRunQueue zclTic=%d sent=%d force=%d\n",
					ZclTic,nb,force);
	}
}

void	ZclClockMs()
{
	ZclUdpRunQueue(0);
}

/*!
 *
 * @brief clock for zcl/udp link, must be called by main loop each second
 * @param now current time in second (time(2))
 * @return void
 * 
 */
void	ZclClockSc(time_t now)
{
	ZclGetPrefixeAndBind();
}

/*!
 *
 * @brief initialize watteco/zcl/udp module
 * @return void
 * 
 */
void	ZclUdpInit()
{
	Prefi6	= strdup("");
	ZclGetPrefixeAndBind();
	INIT_LIST_HEAD(&MsgList);
}
