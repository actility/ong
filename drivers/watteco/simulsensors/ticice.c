
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
#include <poll.h>
#include <unistd.h>
#include <errno.h>
#include <err.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#include "rtlbase.h"
#include "watteco.h"

#define	ABS(x)		((x) > 0 ? (x) : -(x))



char	*ReportAddress	= "2-ticice@::1";

int	TraceLevel	= 2;
int	TraceDebug	= 1;

void	*MainTbPoll;

char 	*Recv6 = "::1";
int 	Recvport = 47005;
struct sockaddr_in6 Recv;
int	FdRecv	= -1;

/*
char	*DescTemp = "11 01 0050 0002 00 4C 0006 01 0402 01 0402";
char	*DescHumi = "11 01 0050 0002 00 4C 0006 01 0405 01 0405";
char	*DescTempHumi = "11 01 0050 0002 00 4C 000A 02 0402 0405 02 0402 0405";
char	*DescTempHumiIllu = "11 01 0050 0002 00 4C 000E 03 0402 0405 0400 03 0402 0405 0400";
*/
char	BuffBin[1024];
int	SzBin;


char	*TbDesc[] = 
{ 
"11 01 0050 0002 00 4C 0006 02 0052 0053",
};

/*
int	CpuLoad()
{
	FILE	*f;
	int	result	= 0;

	f	= popen("./cpu.sh","r");
	if	(!f)
		return	0;

	if	( fscanf(f,"%d",&result) == 1)
	{
	}

	pclose(f);

	return	result;
}
*/



int	ZclUdpSendMessage(char *addr,char *zdata,int zlg);

void	EventReport0053(char *addr,int attr,char *buf,int sz)
{
	t_zcl_msg	zmsg;
	int		ret;
	char		*zdata;

	memset	(&zmsg,0,sizeof(t_zcl_msg));
	zdata			= (char *)&zmsg.u_pack.m_data;
	zmsg.m_cmd		= W_CMD_REPO_ATTR_EVNT;
	zmsg.m_cluster_id	= W_CLU_TICICE;
//	zmsg.m_attr_id		= W_ATT_TICICE_DATA;
	zmsg.m_attr_id		= attr;


	zmsg.u_param.m_tic_data.m_sz	= sz;
	memcpy(zmsg.u_param.m_tic_data.m_data,buf,sz);

	ret	= AwZclEncode(&zmsg,NULL,0);
	if	(ret < 0)
	{
		rtl_trace(0,"error encodng ZMSG err=%d !!!\n",ret);
		return;
	}
	ZclUdpSendMessage(addr,zdata,ret);
}

void	EventReport0052(char *addr,int nbclock,int idx)
{
	t_zcl_msg	zmsg;
	int		ret;
	char		*zdata;

	memset	(&zmsg,0,sizeof(t_zcl_msg));
	zdata			= (char *)&zmsg.u_pack.m_data;
	zmsg.m_cmd		= W_CMD_REPO_ATTR_EVNT;
	zmsg.m_cluster_id	= W_CLU_SIMPLEMETER;
	zmsg.m_attr_id		= W_ATT_SIMPLEMETER_CUR_VALUE;


	zmsg.u_param.m_metering_value.m_sumWh	= nbclock;
	zmsg.u_param.m_metering_value.m_sumVarh	= nbclock;
	zmsg.u_param.m_metering_value.m_count	= idx;
	zmsg.u_param.m_metering_value.m_curWh	= nbclock;
	zmsg.u_param.m_metering_value.m_curVarh	= nbclock;

	ret	= AwZclEncode(&zmsg,NULL,0);
	if	(ret < 0)
	{
		rtl_trace(0,"error encodng ZMSG err=%d !!!\n",ret);
		return;
	}
	ZclUdpSendMessage(addr,zdata,ret);
}

void	CmdRepoConfRqst(int numcap,char *addr,t_zcl_msg *zmsg)
{
	char		*zdata;
	int		ret;
rtl_trace(0,"recv reporting conigure for sensor num=%d\n",numcap);

	zdata	= (char *)&zmsg->u_pack.m_data;
	zmsg->m_cmd	= W_CMD_REPO_CONF_RESP;

	ret	= AwZclEncode(zmsg,NULL,0);
	if	(ret < 0)
	{
		rtl_trace(0,"error encodng ZMSG err=%d !!!\n",ret);
		return;
	}

dosend	:
	ZclUdpSendMessage(addr,zdata,ret);
//	zdata[0] = 0x11;
	if	(1)
	{	// for traces only
		t_zcl_msg	zout;
		memcpy(&zout,zmsg,sizeof(zout));
		ret = AwZclDecode(BuffBin,&zout,NULL,ret);
		if	(ret < 0)
		{
			rtl_trace(0,"error (re)decoding ZMSG err=%d!!!\n",ret);
			return;
		}
		fprintf(stderr,"%s",BuffBin);
		fflush(stderr);
	}

}

void	CmdReadAttrRqst(int numcap,char *addr,t_zcl_msg *zmsg)
{
	unsigned int	clat;
	char		*zdata;
	int		ret;
	char		*desc;

	zdata	= (char *)&zmsg->u_pack.m_data;

	clat	= W_CLAT(zmsg->m_cluster_id,zmsg->m_attr_id);
	switch	(clat)
	{
/****/	case	W_CLAT(W_CLU_CONFIGURATION,W_ATT_CONFIGURATION_PANID) :

rtl_trace(0,"send panid for sensor num=%d\n",numcap);
		zmsg->u_param.m_uint2_value.m_value	= 0x1234;
		zmsg->m_cmd	= W_CMD_READ_ATTR_RESP;
	break;
/****/	case	W_CLAT(W_CLU_CONFIGURATION,W_ATT_CONFIGURATION_DESC) :
		ret	= 0;
		desc	= TbDesc[ret];
		SzBin	= sizeof(BuffBin);
		AwStrToBin(desc,BuffBin,&SzBin);
		memcpy	(zdata,BuffBin,SzBin);
		ret	= SzBin;
rtl_trace(0,"send desc for sensor num=%d sz=%d\n",numcap,ret);
		goto	dosend;
	break;
/****/	case	W_CLAT(W_CLU_TICICE,W_ATT_TICICE_TYPE) :
rtl_trace(0,"send attr tic type for sensor num=%d\n",numcap);
		zmsg->u_param.m_uint1_value.m_value	= 0x06;
		zmsg->m_cmd	= W_CMD_READ_ATTR_RESP;
	break;
	default :
		return;
	break;
	}


	ret	= AwZclEncode(zmsg,NULL,0);
	if	(ret < 0)
	{
		rtl_trace(0,"error encodng ZMSG err=%d !!!\n",ret);
		return;
	}

dosend	:
	ZclUdpSendMessage(addr,zdata,ret);
//	zdata[0] = 0x11;
	if	(1)
	{	// for traces only
		t_zcl_msg	zout;
		memcpy(&zout,zmsg,sizeof(zout));
		ret = AwZclDecode(BuffBin,&zout,NULL,ret);
		if	(ret < 0)
		{
			rtl_trace(0,"error (re)decoding ZMSG err=%d!!!\n",ret);
			return;
		}
		fprintf(stderr,"%s",BuffBin);
		fflush(stderr);
	}

}


void 	SensorZclEventForNum(int numcap,char *addr,unsigned char *binbuf,int sz)
{
	t_zcl_msg	zmsg;
	int		ret;

	rtl_trace(1,"receive message for sensor num=%d '%s' sz=%d\n",
					numcap,addr,sz);

	ret	= AwZclDecode(BuffBin,&zmsg,binbuf,sz);
	if	(ret < 0)
	{
		rtl_trace(0,"error decoding ZMSG err=%d!!!\n",ret);
		return;
	}
	fprintf(stderr,"%s",BuffBin);
	fflush(stderr);

	switch	(zmsg.m_cmd)
	{
	case	W_CMD_READ_ATTR_RQST:
		CmdReadAttrRqst(numcap,addr,&zmsg);
	break;
	case	W_CMD_REPO_CONF_RQST:
		CmdRepoConfRqst(numcap,addr,&zmsg);
	break;
	}
}

void	DoClockSc()
{
	static	unsigned	int	nbclock	= 0;
	static	unsigned 	int	idx;

	int	sz;
//	char	*data = "00000007085001b83032313232383430373131360048432e2e002d0000054e0000192b48502e2e00000000530000003c4130303030303000";

	char	*data[] = {
	"00001f81fd6fff3b424153455f413500070b0c13280a0019ad000b475000070b0c1328003f44070b0c131e000000070b0c1314000000070b0c130a000000070b0c1300000000070b0c12320000005f513ac663bb387a725867a27d867f4501b36181184fbbe3fe24333737353000003a",
	"00001eb63d6c7adf010b0c020000010c0c0200009ad6070b0c13280a53fc967a3c708e32f0405725208b897f14371876105d433c070b0c1325168fcaa28c4a2e6153ef2c715514419a02e44a1b8a0958500a070b0c1326099346523bd8c710f6cb6305da3803af47f8fc126726567465",
	"00001eb63d6c7adf01090c020000010a0c0200009ad6070b0c13263b5b3dde5645e746d8801fee2e5da62e0d97b82a1d4933b3ce070b0c132732118bce3bbd5c0fcdf61d3d261fa1e392f411835f5344bc7b070b0c13280a11e6671cbd983ab4b70cd4370a8c174f26004b711848ee8f"
	};

	char	buf[W_ZCL_TIC_SIZE];



	if	((nbclock % 30) == 0)
	{
		sz	= sizeof(buf);
		AwStrToBin(data[idx%3],(unsigned char *)buf,&sz);
		EventReport0053(ReportAddress,W_ATT_TICICE_DATA+(idx%3),buf,sz);
		idx++;
		EventReport0052(ReportAddress,nbclock,idx);
	}
	nbclock++;

}

int	CB_ZclUdpEvent(void *tb,int fd,void *r1,void *r2,int revents)
{
	int	sz;
	int	port;
	unsigned char	buf[4000];
	struct sockaddr_in6 from;
	socklen_t	lenaddr;
        char str[INET6_ADDRSTRLEN];
        char sensor[INET6_ADDRSTRLEN];
	unsigned char numsimulsensor;

	rtl_trace(9,"callback event zcl/udp fd=%d FD=%d e=%d r1=%lx r2=%lx\n",
			fd,FdRecv,revents,r1,r2);

	if (fd != FdRecv)
	{
		rtl_trace(0,"fd=%d != myfd=%d\n",fd,FdRecv);
		rtl_pollRmv(MainTbPoll,fd);
		return	-1;
	}

	if ((revents & POLLOUT) == POLLOUT)
	{
		rtl_trace(1,"event write zcl/udp fd=%d FD=%d\n",fd,FdRecv);
	}

	if ((revents & POLLIN) == POLLIN)
	{
		rtl_trace(9,"event read zcl/udp fd=%d FD=%d\n",fd,FdRecv);
		lenaddr	= sizeof(from);
		if	((sz=recvfrom(fd,buf,sizeof(buf),0,
				(struct sockaddr *)&from,&lenaddr)) <= 0)
		{
			rtl_trace(0,"read error on zcludp err=%d ret=%d\n",errno,sz);
			return	0;
		}

		str[0]	= '\0';
		port	= ntohs(from.sin6_port);
		inet_ntop(AF_INET6,&from.sin6_addr,str,INET6_ADDRSTRLEN);

		numsimulsensor	= buf[0];
		buf[0]		= 0x11;
		sprintf		(sensor,"%d@%s",numsimulsensor,str);
		rtl_trace(9,"read zcl/udp message sz=%d from '%s'+%d\n",
							sz,sensor,port);
		if (numsimulsensor == 0 || numsimulsensor == 0x11)
			return	0;
		SensorZclEventForNum(numsimulsensor,sensor,buf,sz);
	}

	return	0;
}

void	ZclUdpBind()
{
	Recv.sin6_family = AF_INET6;
	Recv.sin6_port = htons(Recvport);

	if (inet_pton(AF_INET6,Recv6,&Recv.sin6_addr) <= 0)
	{
		rtl_trace(0,"cannot convert addr '%s'\n",Recv6);
		return;
	}

	FdRecv	= socket(AF_INET6,SOCK_DGRAM,0);
	if (FdRecv < 0)
	{
		rtl_trace(0,"cannot create socket zcl/udp\n");
		return;
	}
	if (bind(FdRecv,(struct sockaddr *)&Recv,sizeof(Recv)) < 0)
	{
		close(FdRecv);
		FdRecv = -1;
		rtl_trace(0,"cannot bind zcl/udp '%s'+%d\n",Recv6,Recvport);
		return;
	}

	rtl_pollAdd(MainTbPoll,FdRecv,CB_ZclUdpEvent,NULL,NULL,NULL);
}

void	MainLoop()
{
	time_t	lasttimesc	= 0;
	time_t	now		= 0;
	int	ret;

	while	(1)
	{
		// external events
		ret	= rtl_poll(MainTbPoll,100);

		// clocks
		now	= rtl_tmms();
		if	(ABS(now-lasttimesc) >= 1000)
		{
			DoClockSc();
			lasttimesc	= now;
		}
	}
}

/*
 *	first byte of message is used to send the sensor number
 */
int	ZclUdpSendMessage(char *addr,char *zdata,int zlg)
{
	static	int	fdsend	= -1;
	static	int	failure	= 0;
	struct sockaddr_in6 srce;
	char	*srceaddr = "::1";	// TODO
	int	srceport = 47003;	// TODO
	struct sockaddr_in6 dest;
	int	sendport = 47002;	// TODO
	char	*simul;
	unsigned char	numsimul;
	unsigned char	sv;

	if	(failure)
		return	-1;


	dest.sin6_family = AF_INET6;
	dest.sin6_port = htons(sendport);
	if	(fdsend < 0)
	{
		srce.sin6_family = AF_INET6;
		srce.sin6_port = htons(srceport);
		if (inet_pton(AF_INET6,srceaddr,&srce.sin6_addr) <= 0)
		{
			failure	= 1;
			rtl_trace(0,"cannot convert srceaddr '%s'\n",srceaddr);
			return	-2;
		}
		fdsend	= socket(AF_INET6,SOCK_DGRAM,0);
		if (fdsend < 0)
		{
			failure	= 1;
			rtl_trace(0,"cannot open sock srceaddr '%s'\n",srceaddr);
			return	-3;
		}
/*
		if (bind(fdsend,(struct sockaddr *)&srce,sizeof(srce)) < 0)
		{
			failure	= 1;
			rtl_trace(0,"cannot bind sock srceaddr '%s'+%d\n",
						srceaddr,srceport);
			return	-4;
		}
*/
	}

	numsimul= 0;
	sv	= zdata[0];
	simul	= strchr(addr,'@');	// num/ipv6
	if	(simul)
	{
		numsimul= atoi(addr);
		addr	= simul+1;
		zdata[0]= numsimul;
	}

	rtl_trace(1,"send message for sensor num=%d '%s' sz=%d\n",
					numsimul,addr,zlg);

	if (inet_pton(AF_INET6,addr,&dest.sin6_addr) <= 0)
	{
		zdata[0] = sv;
		rtl_trace(0,"cannot convert addr '%s'\n",addr);
		return	-5;
	}
	if (sendto(fdsend,zdata,zlg,0,(struct sockaddr *)&dest,sizeof(dest)) < 0)
	{
		zdata[0] = sv;
		rtl_trace(0,"cannot write to addr '%s'\n",addr);
		return	-6;
	}
	zdata[0] = sv;
	return	zlg;
}

int main(int argc,char *argv[])
{
	signal	(SIGPIPE,SIG_IGN);

	rtl_init();
	rtl_tracemutex();
	rtl_tracelevel(2);
	MainTbPoll	= rtl_pollInit();

	AwZclInitProto();

	ZclUdpBind();
	MainLoop();
	return	0;
}

