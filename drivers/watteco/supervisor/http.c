
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

#include <stdio.h>			/************************************/
#include <stdlib.h>			/*     run in separated thread      */
#include <string.h>			/************************************/
#include <unistd.h>
#include <pthread.h>
#include <errno.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>


#include "rtlbase.h"
#include "rtlimsg.h"


#include "adaptor.h"


#include "define.h"
#include "struct.h"

char	*GetEnvVarName();

extern	void	*MainIQ;

static	void	*HttpTbPoll;

static	char	*Pani6;
static	char	*Request	= HTTP_GET_ELEM_LIST;
static	char	*OutFile	= "discoverpan";
static	int	Httpport = 80;
static	struct sockaddr_in6 Dest;

static	void	ProcessHttpResponse(int fdsock)
{
	int	tot;
	int	sz;
	int	i;
	int	nb;
	char	res[HTTP_RESPONSE_SIZE];
	char	*tab[MAX_SSR_PER_RTR];
	char	tmp[512];
	char	buf[1000];
	FILE	*fout;

	tmp[0]	= '\0';
	RTL_TRDBG(3,"start reading http response fd=%d\n",fdsock);
	tot	= 0;
	while ((sz=read(fdsock,buf,sizeof(buf)-1))>0)
	{
		if (tot+sz >= sizeof(res)-1)
		{
			RTL_TRDBG(0,"response too long, truncated\n");
			break;
		}
		memcpy	(res+tot,buf,sz);
		tot	= tot + sz;
	}
	res[tot] = '\0';
	close(fdsock);
	RTL_TRDBG(1,"http response sz=%d ret=%d\n",tot,sz);

	if (tot <= 0)		// empty response ???
	{
		RTL_TRDBG(1,"nothing to read, empty response ???\n");
		return;
	}

	if (sz < 0)		// error while reading
	{
		RTL_TRDBG(0,"cannot read\n");
		return;
	}

	if (sz == 0 && tot > 0)
	{	// normal close
	}

	nb	= AwParseRouter(res,tab);
	if (nb < 0)
	{
		RTL_TRDBG(0,"error while discovering err=%d\n",errno);
		return;
	}

	RTL_TRDBG(2,"responses from HTTP :\n");
	for	(i = 0 ; i < nb && tab[i]; i++)
	{
		RTL_TRDBG(2,"	'%s'\n",tab[i]);
	}

	sprintf	(tmp,"/%s/%s.tmp",getenv(GetEnvVarName()),OutFile);
	fout	= fopen(tmp,"w");
	if	(fout)
	{
		char	tmp2[512];
		char	tmp3[512];
		for	(i = 0 ; i < nb && tab[i]; i++)
		{
			fprintf	(fout,"%s\n",tab[i]);
		}
		fclose(fout);
		sprintf	(tmp2,"/%s/%s.txt",getenv(GetEnvVarName()),OutFile);
		sprintf	(tmp3,"/%s/%s.old",getenv(GetEnvVarName()),OutFile);
		if	( AwFileCmp(tmp,tmp2) )
		{ // verify if files are really different
			if	(!unlink(tmp3))
			{
				RTL_TRDBG(0,"ERROR cannot unlink '%s' '%s'\n",
					tmp3,STRERRNO);
			}
			if	(!link(tmp2,tmp3))
			{
				RTL_TRDBG(0,"ERROR cannot link '%s' '%s' '%s'\n",
					tmp2,tmp3,STRERRNO);
			}

			if	(!unlink(tmp2))
			{
				RTL_TRDBG(0,"ERROR cannot unlink '%s' '%s'\n",
					tmp2,STRERRNO);
			}
			if	(!link(tmp,tmp2))
			{
				RTL_TRDBG(0,"ERROR cannot link '%s' '%s' '%s'\n",
					tmp,tmp2,STRERRNO);
				goto	fctend;
			}


			rtl_imsgAdd(MainIQ,
			rtl_imsgAlloc(IM_DEF,IM_LIST_SENSOR_UPDATE,NULL,0));

		}
		else
		{
			RTL_TRDBG(1,"no change in '%s'\n",OutFile);
			rtl_imsgAdd(MainIQ,
			rtl_imsgAlloc(IM_DEF,IM_LIST_SENSOR_SAME,NULL,0));
		}
	}
	else
		RTL_TRDBG(0,"cannot open(w) '%s'\n",tmp);

fctend :
	if	(strlen(tmp))
		unlink(tmp);

	for	(i = 0 ; i < nb && tab[i]; i++)
	{
		free(tab[i]);
		tab[i]	= NULL;
	}
}

static	void	HttpLoop()
{
	char	*pt;
	char	pan[128];
	int	fdsock;

	for	(;;sleep(60*1))
	{
	pt	= AwGetPanAddr(pan);
	if	(!pt || !*pt)
	{	// file not ready
		RTL_TRDBG(1,"wait for PAN@\n");
		continue;
	}
	if	(strcmp(Pani6,pt))
	{	// first or new PAN@
		RTL_TRDBG(1,"got a new PAN@ '%s' -> '%s'\n",Pani6,pt);
		if (Pani6)
			free(Pani6);
		Pani6	= strdup(pt);
		if (inet_pton(AF_INET6,Pani6,&Dest.sin6_addr) <= 0)
		{
			RTL_TRDBG(0,"cannot convert addr6 '%s', abort\n",Pani6);
			continue;
		}
	}
	fdsock	= socket(AF_INET6,SOCK_STREAM,0);
	if (fdsock < 0)
	{
		RTL_TRDBG(0,"cannot create socket\n");
		continue;
	}

	// TODO connect could block 2/3 minutes on wrong address with prefixe ok
	// in this case we have a trace from tunslip6 :
	// 	"slip-bridge: Destination off-link but no route"
	RTL_TRDBG(1,"try connect to '%s+%d'\n",Pani6,Httpport);
	if (connect(fdsock,(struct sockaddr *)&Dest,sizeof(Dest)) < 0)
	{
		close(fdsock);
		RTL_TRDBG(1,"cannot connect socket on '%s+%d'\n",Pani6,Httpport);
		continue;
	}

	RTL_TRDBG(3,"send request '%s' to '%s+%d'\n",Request,Pani6,Httpport);
	// send GET HTTP request to get list of sensors
	if (write(fdsock,Request,strlen(Request)) < 0)
	{
		close(fdsock);
		RTL_TRDBG(0,"cannot write on socket fd=%d err=%d\n",
			fdsock,errno);
		continue;
	}
	ProcessHttpResponse(fdsock);
	close(fdsock);
	}
}


void	*HttpRun(void *param)
{
	RTL_TRDBG(0,"start awspv.x/http th=%lx pid=%d\n",
				(long)pthread_self(),getpid());

	Pani6		= strdup("");
	Dest.sin6_family= AF_INET6;
	Dest.sin6_port 	= htons(Httpport);

	// HttpTbPoll could be used if we open http socket in non blocking
	// mode to have a asyncronous POLLOUT when connection is ready.
	HttpTbPoll	= rtl_pollInit();
	HttpLoop();
	RTL_TRDBG(0,"ERROR end of awspv.x/http th=%lx\n",(long)pthread_self());
	return	NULL;
}

