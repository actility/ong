
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
#include <stdarg.h>
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
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <netdb.h>

#include "rtlbase.h"
#include "rtllist.h"

#include "rtlclif.h"


#define	CLI_MAX_FD	32
#define	TCP_FRAME_SZ	1024



static	t_clif	*TbCli[CLI_MAX_FD];
static	int	FdListen = -1;
static	int	(*UserCmdFct)(t_clif *,char *);

static	int	TcpKeepAlive	= 1;
#if	0
static	int	TcpKeepIdle	= 60;
static	int	TcpKeepIntvl	= 10;
static	int	TcpKeepCnt	= 2;
#endif


/*!
 *
 * @brief set keep alive attributs on a fd socket
 * Actualy we dont change system attributs, we just set keep alive mode.
 * @param fd the socket fd
 * @return void
 * 
 */
static	void	AdmKeepAlive(int fd)
{
	if	( TcpKeepAlive > 0 )
	{
		setsockopt(fd,SOL_SOCKET,SO_KEEPALIVE,
				(char *)&TcpKeepAlive,sizeof(TcpKeepAlive));
#if	0
		if	( TcpKeepIdle > 0 )
		{
			setsockopt(fd,IPPROTO_TCP,TCP_KEEPIDLE,
			(char *)&TcpKeepIdle,sizeof(TcpKeepIdle));
		}
		if	( TcpKeepIntvl > 0 )
		{
			setsockopt(fd,IPPROTO_TCP,TCP_KEEPINTVL,
			(char *)&TcpKeepIntvl,sizeof(TcpKeepIntvl));
		}
		if	( TcpKeepCnt > 0 )
		{
			setsockopt(fd,IPPROTO_TCP,TCP_KEEPCNT,
			(char *)&TcpKeepCnt,sizeof(TcpKeepCnt));
		}
#endif
	}
}

/*!
 *
 * @brief Call back function when we need to store data waiting to be sent
 * @param cl the destination connection
 * @param buf the data to store
 * @param len the lenght of data
 * @return void
 * 
 */
static	void	AdmAddBuf(t_clif *cl,char *buf,int len)
{
	int	fd;
	t_clout	*out;

	fd	= cl->cl_fd;

	RTL_TRDBG(9,"Adm add buffer lg=%d\n",len);
	if	(!buf || len <= 0)
		return;

	// buf deja decoupe en morceaux plus petits que TCP_FRAME_SZ
	out	= (t_clout *)malloc(sizeof(t_clout));
	if	( !out )
		return;

	out->o_len	= len;
	out->o_buf	= (char *)malloc(len);
	if	( !out->o_buf )
		return;
	memcpy	(out->o_buf,buf,len);

	list_add_tail( &(out->list), &(cl->cl_lout.list) );
}
