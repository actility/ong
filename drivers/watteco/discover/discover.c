
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
#include <ctype.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>

#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <err.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#include "_whatstr.h"
#include "aw.h"
#include "watteco.h"

int	TraceLevel	= 2;
int	TraceDebug	= 1;

static	char *GetEnvVarName()
{
	return	"AW_DATA_DIR";	// for discover.x
}

//char *Pani6 = "aaaa::ff:ff00:23f4";
char *Pani6 = NULL;
int Panport = 80;

struct sockaddr_in6 Dest;

void Usage(char *p)
{
printf(
"%s [-a router_addr6] [-w out_file] [-s sleep_delay] [-T in_file] [-vh]\n",p);
printf(
"     -a router_addr6 is the router ipv6 address.\n");
printf(
"      If -a option is omitted, file $AW_DATA_DIR/router_addr.txt is used\n");
printf(
"      to get an address. This file is reloaded between requests.\n"); 
printf(
"     -s sleep_delay is the delay (s) between requests. Default 30s.\n");
printf(
"     -w out_file is the file name (no extension) to store responses from router.\n");
printf(
"      Default is $AW_DATA_DIR/discoverpan.txt\n");
printf(
"     -T in_file is a file name to test parsing of http responses.\n");
printf(
"     -h This help.\n");
printf(
"     -v Some debug traces.\n");
}

static	int	NbCap;
static	char	*TbCap[MAX_SSR_PER_RTR];

// tests only
void DoSensorDelete(char *todel)
{
	int	i;

	for	(i = 0 ; i < MAX_SSR_PER_RTR ; i++)
	{
		if	(TbCap[i] && strcmp(TbCap[i],todel) == 0)
		{
			free(TbCap[i]);
			TbCap[i] = NULL;
			NbCap--;
printf("delete sensor '%s' room=%d (%d)\n",todel,i,NbCap);
			return;
		}
	}

printf("sensor '%s' not found\n",todel);

}

// tests only
void DoSensorCreate(char *tocreate)
{
	int	i;
	int	room = -1;

	// really does not exist ?
	for	(i = 0 ; i < MAX_SSR_PER_RTR ; i++)
	{
		if	(TbCap[i] && strcmp(TbCap[i],tocreate) == 0)
		{
printf("sensor '%s' already exists at room=%d\n",tocreate,i);
			return;
		}
	}

	// find a room
	for	(i = 0 ; i < MAX_SSR_PER_RTR ; i++)
	{
		if	(TbCap[i] == NULL)
		{
			room	= i;
			break;
		}
	}
	if	(room < 0)
	{
printf("max sensors reached (%d) !!!\n",NbCap);
	}

	if	(room >= 0)
	{
		TbCap[room]	= strdup(tocreate);
		NbCap++;
printf("create '%s' room=%d (%d)\n",tocreate,room,NbCap);
	}

}

// tests only
void DoSensorDetection()
{
	int	ret;
	int	i;
	int	nb;
	char	*tbcur[MAX_SSR_PER_RTR];
	char	*tbdel[MAX_SSR_PER_RTR];
	char	*tbnew[MAX_SSR_PER_RTR];

	memset	(tbcur,0,sizeof(tbcur));
	memset	(tbdel,0,sizeof(tbdel));
	memset	(tbnew,0,sizeof(tbnew));

	nb	= 0;
	for	(i = 0 ; i < MAX_SSR_PER_RTR ; i++)
	{
		if	(TbCap[i])
		{
			tbcur[nb++]	= TbCap[i];
		}
	}

	ret	= AwSensorDiscover(NULL,tbcur,tbdel,tbnew);
	if	(ret < 0)
	{
		warnx("error in AwSensorDetection() err=%d\n",ret);
		return;
	}
	if	(ret == 0)
	{
		warnx("no change for AwSensorDetection()\n");
		return;
	}

	// delete old sensors
	for	(i = 0 ; tbdel[i] && i < MAX_SSR_PER_RTR ; i++)
	{
		DoSensorDelete(tbdel[i]);
		free(tbdel[i]);
		tbdel[i]	= NULL;
	}

	// create new sensors
	for	(i = 0 ; tbnew[i] && i < MAX_SSR_PER_RTR ; i++)
	{
		DoSensorCreate(tbnew[i]);
		free(tbnew[i]);
		tbnew[i]	= NULL;
	}

}

// tests only
void	SensorDump()
{
	int	i;

	printf	("current sensors after detection :\n");
	for	(i = 0 ; i < MAX_SSR_PER_RTR ; i++)
	{
		if	(TbCap[i])
		{
			printf("	[%02d] '%s'\n",i,TbCap[i]);
		}
	}
}

int main(int argc,char *argv[])
{
	char	*msg = HTTP_GET_ELEM_LIST;
	char	res[HTTP_RESPONSE_SIZE];
	char	*tab[MAX_SSR_PER_RTR];
	char	*inTestFile = NULL;
	char	*outFile = NULL;
	int	delay	= 30;

	int	optiona	= 0;
	int	optionv	= 0;
	int	optionw	= 0;
	int	optionT	= 0;

	int	nb;
	int	fdsock;
	int	sz;
	int	tot;
	char	buf[1000];
	char	*pt;
	int	c;
	int	i;
	FILE	*fout;
	char	tmp[512];

	signal	(SIGPIPE,SIG_IGN);
	if	(argc > 1 && strcmp(argv[1],"--version") == 0)
	{
		printf	("%s\n",watteco_whatStr);
		exit(0);
	}

	Pani6	= strdup("");
	while ((c = getopt (argc, argv, "hva:T:w:s:")) != -1) 
	{
		switch(c)
		{
		case	'a':
			Pani6	= strdup(optarg);
			optiona	= 1;
		break;
		case	'w':
			outFile	= strdup(optarg);
			optionw	= 1;
		break;
		case	's' :
			delay	= atoi(optarg);
			if (delay <= 0)
				delay	= 30;
		break;
		case	'v':
			optionv	= 1;
		break;
		case	'T' :	// tests only
			inTestFile = strdup(optarg);
			optionT	= 1;
			optionv	= 1;
		break;
		case	'h' :
		default :
			Usage(argv[0]);
			exit(1);
		break;
		}
	}

	Dest.sin6_family = AF_INET6;
	Dest.sin6_port = htons(Panport);
	if (optiona)
	{
		if (inet_pton(AF_INET6,Pani6,&Dest.sin6_addr) <= 0)
		{
			Usage(argv[0]);
			errx(1,"cannot convert addr6 '%s', abort\n",Pani6);
			exit(1);
		}
	}
	else
	{ // if !-a && AW_DATA_DIR it will be impossible to get PAN@
		pt	= getenv(AW_DATA_DIR);
		if	(!inTestFile && (!pt || !*pt))
		{
			Usage(argv[0]);
			errx(1,"%s unset, abort\n",AW_DATA_DIR);
			exit(1);
		}
	}


	if	(!optionw)
	{
		pt	= getenv(AW_DATA_DIR);
		if	((!pt || !*pt))
		{
			Usage(argv[0]);
			errx(1,"%s unset, abort\n",AW_DATA_DIR);
			exit(1);
		}
		sprintf	(tmp,"%s/discoverpan",pt);
		outFile	= strdup(tmp);
	}


	for	(;; sleep(optionT?1:delay))
	{

	if (optionT && inTestFile && *inTestFile)
	{	
		/*
		 * only for tests with -T option when reading http responses
		 * from a file
		 */
		static	time_t	last	= 0;
		struct	stat	st;
		int	fd;

		fd	= open(inTestFile,0);
		if	(fd < 0)
		{
			warn("wait for file '%s'\n",inTestFile);
			continue;
		}
		if	(fstat(fd,&st) < 0)
		{
			close(fd);
			warn("cannot stat on file '%s'\n",inTestFile);
			continue;
		}
		if	(st.st_mtime == last)
		{	// file date does not change
			close(fd);
			continue;
		}
		last	= st.st_mtime;
		fdsock	= fd;
		goto canread;
	}

	if	(!optiona)
	{	// PAN@ not given, get it from file
		char	pan[128];
		pt	= AwGetPanAddr(pan);
		if	(!pt || !*pt)
		{	// file not ready
			warnx("wait for PAN@\n");
			continue;
		}
		if	(Pani6 && strcmp(Pani6,pt))
		{	// first or new PAN@
			if (Pani6)
				free(Pani6);
			Pani6	= strdup(pt);
			warnx("got a new PAN@ '%s'\n",Pani6);
			if (inet_pton(AF_INET6,Pani6,&Dest.sin6_addr) <= 0)
			{
				warnx("cannot convert addr6 '%s', abort\n",Pani6);
				continue;
			}
		}
	}

	fdsock	= socket(AF_INET6,SOCK_STREAM,0);
	if (fdsock < 0)
	{
		warn("cannot create socket\n");
		continue;
	}

	// TODO connect could block 2/3 minutes on wrong address with prefixe ok
	// in this case we have a trace from tunslip6 :
	// 	"slip-bridge: Destination off-link but no route"
	if (connect(fdsock,(struct sockaddr *)&Dest,sizeof(Dest)) < 0)
	{
		close(fdsock);
		warn("cannot connect socket on '%s+%d'\n",Pani6,Panport);
		continue;
	}

	// send GET HTTP request to get list of sensors
	if (write(fdsock,msg,strlen(msg)) < 0)
	{
		close(fdsock);
		warn("cannot write on socket\n");
		continue;
	}

// direct access to "canread", here fdsock is a file not a socket
// only for tests with -T option when reading pseudo responses from a local file
canread :	


	tot	= 0;
	while ((sz=read(fdsock,buf,sizeof(buf)-1))>0)
	{
		if (tot+sz >= sizeof(res)-1)
		{
			warnx("response too long, truncated\n");
			break;
		}
		memcpy	(res+tot,buf,sz);
		tot	= tot + sz;
	}
	res[tot] = '\0';
	close(fdsock);

	if (tot <= 0)		// empty response ???
	{
		warnx("nothing to read, empty response ???\n");
		continue;
	}

	if (sz < 0)		// error while reading
	{
		warn("cannot read\n");
		continue;
	}

	if (sz == 0 && tot > 0)
	{	// normal close
	}

#if	0
	printf	("--\n");
	printf	("%s",res);
	printf	("--\n");
#endif


	nb	= AwParseRouter(res,tab);
	if (nb < 0)
	{
		warnx("error while discovering err=%d\n",nb);
		continue;
	}

	if	(optionv)
	{
		warnx	("responses from HTTP :");
		for	(i = 0 ; i < nb && tab[i]; i++)
		{
			printf	("	'%s'\n",tab[i]);
		}
	}

	sprintf	(tmp,"%s.tmp",outFile);
	fout	= fopen(tmp,"w");
	if	(fout)
	{
		char	tmp2[512];
		for	(i = 0 ; i < nb && tab[i]; i++)
		{
			fprintf	(fout,"%s\n",tab[i]);
		}
		fclose(fout);
		sprintf	(tmp2,"%s.txt",outFile);
		if	( AwFileCmp(tmp,tmp2) )
		{ // verify if files are really different
			unlink(tmp2);
			if (!link(tmp,tmp2))
				warn("discover.x cannot link '%s' '%s'\n",
						tmp,tmp2);
		}
		else
		{
			if	(optionv)
				warnx("no change in '%s'\n",outFile);
		}
		unlink(tmp);
	}
	else
		warnx("cannot open(w) '%s'\n",tmp);

	for	(i = 0 ; i < nb && tab[i]; i++)
	{
		free(tab[i]);
		tab[i]	= NULL;
	}


	if	(optionT)
	{
		DoSensorDetection();
		SensorDump();
	}



	}	// for (;;sleep)


	exit(0);
}
