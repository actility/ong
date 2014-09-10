
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
#include <time.h>
#include <pcap.h>  /* GIMME a libpcap plz! */
#include <errno.h>
#include <sys/socket.h>
#include <sys/timeb.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include	"watteco.h"

extern char *optarg;
extern int optind, opterr, optopt;

/*
 * for non root user
 * setcap 'CAP_NET_RAW+eip CAP_NET_ADMIN+eip' /.../snifwatt.x
 */

char	*DefaultDev	= "tun0";
char	*DefaultFilter	= "udp and (port 47002 or port 47004 or port 47003 or port 47005)";
int	DefaultSize	= 512;
int	ExitOnError	= 0;

/*
 * as we do not use librtbase, some var & functions are duplicated here
 */

int	TraceLevel	= 0;
int	TraceDebug	= 0;

void	rtl_trace()
{
}
void	rtl_tracedbg()
{
}
char *AwBinToStr(unsigned char *bin,int lg,char dst[],int max)	/* TS */
{
	int	i;
	int	j = 0;

	static	char	*tb	= "0123456789abcdef";

	for (i = 0 ; i < lg && j < max ; i++)
	{
		dst[j++]	= tb[(unsigned char)(bin[i]/16)];
		dst[j++]	= tb[(unsigned char)(bin[i]%16)];
//printf("%c %c\n",dst[j-2],dst[j-1]);
	}
	dst[j++]	= '\0';
	return	dst;
}
char	*rtl_hhmmss(time_t t,char *buf)		/* TS */
{
	struct	timeb	tp ;
	struct	tm	*dt;

	if (t == 0)
	{
		ftime	(&tp) ;
		dt	= localtime(&tp.time);
	}
	else
		dt	= localtime(&t);
	sprintf(buf,"%02d:%02d:%02d",dt->tm_hour,dt->tm_min,dt->tm_sec);

	return	buf ;
}





void my_callback(u_char *useless,const struct pcap_pkthdr* hdr,const u_char*
        pkt)
{

//	printf(".");

//printf("+ len=%d caplen=%d type=%x\n",hdr->len,hdr->caplen,*pkt);

	if	(*pkt != 0x60)		// ! ip6
		return;

	if	(*(pkt+6) != 0x11)	// ! udp
		return;

	if	(hdr->len != hdr->caplen || hdr->len <= 28)
	{	// ! enough capture or no udp data ...
		return;
	}

#if	0
	int	i;
	int	b;

	len	= hdr->len;
	for	(i = 0 , b = 1 ; i < len ; i++)
	{
		printf	("%02x",*pkt);
		if	( (b % 2) == 0 )
			printf(" ");
		if	( (b % 16) == 0)
			printf("\n");
		b++;
		pkt++;
	}
#endif


	u_char	*src = pkt + 8;
	u_char	*dst = pkt + 8 + 16;
	u_char	*prt = pkt + 8 + 16 + 16;
	u_char	*udp = pkt + 48;
	int	ludp = (int)htons(*(unsigned short *)(pkt+44));

	char	asrc[INET6_ADDRSTRLEN];
	char	adst[INET6_ADDRSTRLEN];
	unsigned short psrc;
	unsigned short pdst;
	char	when[128];


	// lg udp include src/dst ports and chksum
	ludp	= ludp - 8;
	if	(ludp <= 0)
		return;

	asrc[0]	= '\0';
	inet_ntop(AF_INET6,src,asrc,INET6_ADDRSTRLEN);
	adst[0]	= '\0';
	inet_ntop(AF_INET6,dst,adst,INET6_ADDRSTRLEN);

	psrc	= *(prt+0) * 256 + *(prt+1);
	pdst	= *(prt+2) * 256 + *(prt+3);

	printf	("%s.%d [%s]:%d -> [%s]:%d (sz=%d)\n",
			rtl_hhmmss(hdr->ts.tv_sec,when),hdr->ts.tv_usec/1000,
			asrc,psrc,adst,pdst,ludp);


	unsigned char	str[5000];
	int		ret;
	t_zcl_msg	mess;

	str[0]	= '\0';
	AwBinToStr(udp,ludp,(char *)str,sizeof(str));	/* TS */
	printf("# [%s]\n",str);

	str[0]	= '\0';
	ret	= AwZclDecode((char *)str,&mess,udp,ludp);
	if	(ret < 0)
	{
		printf("ERROR 0x%08x %d\n",ret,ret);
		fflush(stdout);
		if	(ExitOnError)
			exit(1);
		return;
	}

	printf	("OK 0x%08x %s\n",ret,str);
	fflush(stdout);
}

int main(int argc, char **argv)
{
	
	char		*dev;				/* name of the device to use */ 
	bpf_u_int32	netp;				/* ip          */
	bpf_u_int32	maskp	= PCAP_NETMASK_UNKNOWN;	/* subnet mask */

	pcap_t*			descr;
	struct bpf_program	fp;      /* hold compiled program     */
#if	0
	const u_char		*packet;
	struct pcap_pkthdr	hdr;     /* pcap.h */
#endif
	char		*filter;


	int	opt;
	int	ret;   /* return code */
	char	errbuf[PCAP_ERRBUF_SIZE];
	char	buffilter[2000];


	dev	= DefaultDev;
	filter	= DefaultFilter;


	while ((opt = getopt(argc, argv, "ei:")) != -1)
	{
		switch(opt)
		{
		case	'i':
			dev	= optarg;
		break;
		case	'e':
			ExitOnError	= 1;
		break;
		}
	}



	if	(optind < argc)
	{
		sprintf	(buffilter,"(%s) and ",filter);
		while	(optind < argc)
		{	// additional option assume filter
//			printf	("add filter '%s'\n", argv[optind]);
			strcat	(buffilter,argv[optind]);
			strcat	(buffilter," ");
			optind++;
		}
		filter	= buffilter;
//		printf	("new filter '%s'\n",filter);
	}


	/* ask pcap for the network address and mask of the device */
/*
	ret = pcap_lookupnet(dev,&netp,&maskp,errbuf);
	if	(ret == -1)
	{
		printf("%s\n",errbuf);
		exit(1);
	}
printf("lookup ok '%s'\n",dev);
*/


	descr = pcap_open_live(dev,BUFSIZ,0,100 /* ms */,errbuf);
	if	(descr == NULL)
	{
		printf("pcap_open_live(): %s\n",errbuf);
		exit(1);
	}
printf("openlive ok '%s'\n",dev);
/*
	ret	= pcap_set_snaplen(descr,DefaultSize);
	if	(ret != 0)
	{
		printf("pcap_set_snaplen(): %s\n",errbuf);
		exit(1);
	}
printf("openlive+snap_len ok '%s'\n",dev);
*/

	ret	= pcap_compile(descr,&fp,filter,0,maskp);
	if	(ret == -1)
	{
		printf("pcap_compile(): %s\n",errbuf);
		exit(1);
	}
printf("compile ok '%s'\n",filter);

	ret	= pcap_setfilter(descr,&fp);
	if	(ret == -1)
	{
		printf("pcap_setfilter(): %s\n",errbuf);
		exit(1);
	}
printf("setfilter ok '%s'\n",filter);


	pcap_loop(descr,-1,my_callback,NULL);

printf("done\n");

#if	0
	while	(1)
	{
		packet = pcap_next(descr,&hdr);
		if	(!packet)
		{
			usleep(100000);
			continue;
		}
		printf("+\n");
	}
#endif



	exit(0);
}

