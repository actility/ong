
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
#include <stdlib.h>			/*     run in main thread           */
#include <string.h>			/************************************/
#include <stdarg.h>
#include <stddef.h>
#include <unistd.h>
#include <pthread.h>
#include <errno.h>
#include <limits.h>
#include <signal.h>
#include <poll.h>
#ifndef	MACOSX
#include <malloc.h>
#endif

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
#include "rtlimsg.h"

#include "adaptor.h"

#include "xoapipr.h"

#include "define.h"
#include "struct.h"

#include "cproto.h"
#include "cmnproto.h"
#include "extern.h"


extern	t_cli	*TbCli[];	// only for sniffer

/*!
 *
 * @brief adjust the count of cli connections in "snif mode"
 * A cli is in "snif mode" when it wants to dump messages received or sent on
 * a sensor. This is done by setting the sensor address in the cl_sensor field.
 * "*" address means that we want to snif all sensors.
 * @return void
 * 
 */
void	AdmSnifferCount()
{
	t_cli	*wh;
	int	i = 0;

	ActiveSniffer	= 0;
	for	(i = 0 ; i < CLI_MAX_FD ; i++)
	{
		wh	= TbCli[i];
		if	(!wh)	continue;
		if	(strlen(wh->cl_sensor))	
			ActiveSniffer++;
	}
	RTL_TRDBG(1,"Active sniffers %d\n",ActiveSniffer);
}

void	AdmCliFree(t_cli *cl)
{
	if	(!cl)
		return;

	strcpy	(cl->cl_sensor,"");
	AdmSnifferCount();
}

void	AdmUsage(t_cli *cl)
{
AdmPrint(cl,"quit|exit\n");
AdmPrint(cl,"shutdown|stop\n");
AdmPrint(cl,"who\n");
AdmPrint(cl,"close\n");
AdmPrint(cl,"core\n");
AdmPrint(cl,"xoocc\n");

AdmPrint(cl,"trace [level]\n");
AdmPrint(cl,"diatrace [level]\n");
AdmPrint(cl,"getvar [varname]\n");
AdmPrint(cl,"setvar varname [value]\n");
AdmPrint(cl,"addvar varname\n");

AdmPrint(cl,"help\n");
AdmPrint(cl,"info\n");
//AdmPrint(cl,"syncrtr\n");
AdmPrint(cl,"ss\n");
AdmPrint(cl,"sensor\n");
AdmPrint(cl,"stsensor\n");
AdmPrint(cl,"sendsensor index|addr|* zclmessage\n");
AdmPrint(cl,"   where zclmessage is a full message xx yy zz tt ...\n");
AdmPrint(cl,"   or -wip|-panid|-sdesc|-cov cluster [attr]\n");
AdmPrint(cl,"   or +cov cluster attr mint maxt val0[:val1[:val2]]]\n");
AdmPrint(cl,"recvsensor index|addr|* zclmessage\n");
AdmPrint(cl,"resetsensor index|addr|*\n");
AdmPrint(cl,"deletesensor index|addr|*\n");
AdmPrint(cl,"forgetsensor index|addr|* (deletesensor+diadeletesensor)\n");
AdmPrint(cl,"configsensor index|addr|*\n");
AdmPrint(cl,"diasyncsensor index|addr|*\n");
AdmPrint(cl,"diadeletesensor index|addr|*\n");
AdmPrint(cl,"snif address|*\n");
AdmPrint(cl,"diaipu *\n");
AdmPrint(cl,"dianet *\n");
AdmPrint(cl,"method target\n");
AdmPrint(cl,"debug [0|1]\n");
AdmPrint(cl,"covcfg\n");
AdmPrint(cl,"ticdata\n");

}

void	AdmInfo(t_cli *cl)
{
	u_int	szmalloc;
#ifndef	MACOSX
	struct	mallinfo info;
	info	= mallinfo();
	szmalloc= info.uordblks;
#else
	szmalloc= 0;
#endif
	unsigned long vsize;
	unsigned int nbm;
	unsigned int nbt;

	vsize	= rtl_vsize(getpid());

	nbm	= rtl_imsgVerifCount(MainIQ);
	nbt	= rtl_timerVerifCount(MainIQ);

	AdmPrint(cl,"vsize=%fMo (%u)\n",((float)vsize)/1024.0/1024.0,vsize);
	AdmPrint(cl,"alloc=%u\n",szmalloc);
	AdmPrint(cl,"sniffer=%d\n",ActiveSniffer);
	AdmPrint(cl,"pseudoong=%d withdia=%d softsensors=%d httpsensorlist=%d\n",
		PseudoOng,WithDia,SoftwareSensor,HttpSensorList);
	AdmPrint(cl,"covconfig=%d modeling=%d\n",
		WithCovConfig,WithModeling);
	AdmPrint(cl,"allinput=%d duplicatecluster=%d multiendpoint=%d\n",
		AllInputCluster,DuplicateCluster,MultiEndPoint);
	AdmPrint(cl,"zclfdrecv=%d\n",ZclGetFd());
	AdmPrint(cl,"zclfdsend=%d\n",ZclUdpSendMessageNow(NULL,NULL,0));
	AdmPrint(cl,"zclpending=%u(%u)\n",ZclMsgPending,ZclMsgMaxPending);
	AdmPrint(cl,"zclfreq=%f zclperiod=%f zcltic=%d\n",
					ZclFreq,ZclPeriod,ZclTic);
	AdmPrint(cl,"zclsenderr=%d zclineterr=%d\n",ZclSendErr,ZclInetErr);
	AdmPrint(cl,"#msg=%d\n",nbm);
	AdmPrint(cl,"#timer=%d\n",nbt);
	AdmPrint(cl,"diainuse=%u(%u)\n",DiaNbReqInuse,DiaMaxReqInuse);
	AdmPrint(cl,"diapending=%u(%u)\n",DiaNbReqPending,DiaMaxReqPending);
	AdmPrint(cl,"diasendcount=%u (c=%u,r=%u,u=%u,d=%u)\n",DiaSendCount,
		DiaCreaCount,DiaRetrCount,DiaUpdaCount,DiaDeleCount);
	AdmPrint(cl,"diartrycount=%u\n",DiaRtryCount);
	AdmPrint(cl,"diarecvcount=%u\n",DiaRecvCount);
	AdmPrint(cl,"diatimecount=%u\n",DiaTimeCount);
	AdmPrint(cl,"diadisconnect=%u\n",DiaDisconnected());
	AdmPrint(cl,"diaIPUok=%u diaIpuTmt=%u diaIPUerr=%u\n",
					DiaIpuOk,DiaIpuTmt,DiaIpuErr);
	AdmPrint(cl,"diaNETok=%u diaIpuTmt=%u diaNETerr=%u diaRepDrp=%u\n",
				DiaNetOk,DiaIpuTmt,DiaNetErr,DiaRepDrp);
}

int	AdmCmd(t_cli *cl,char *pbuf)
{
	char	cmd[256];
	char	arg[256];
	char	data[256];
	int	nb;
	char	*buf	= pbuf;

	if	(!strlen(buf))
		return	CLI_PROMPT;

	if	(strcmp(buf,"help") == 0 || buf[0] == '?')
	{
		AdmUsage(cl);
		return	CLI_PROMPT;
	}
#if	0
	if	(strcmp(buf,"syncrtr") == 0)
	{
		HttpStart();
		return	CLI_PROMPT;
	}
#endif
	if	(strcmp(buf,"shutdown") == 0)
	{
//		XoFreeAll();
		XoEnd();
		exit(0);
	}
	if	(strcmp(buf,"info") == 0)
	{
		AdmInfo(cl);
		return	CLI_PROMPT;
	}

	if	(strcmp(buf,"sensor") == 0)
	{
		SensorDump(cl,0);
		return	CLI_PROMPT;
	}
	if	(strcmp(buf,"ss") == 0)
	{
		SensorDump(cl,1);
		return	CLI_PROMPT;
	}
	if	(strcmp(buf,"stsensor") == 0)
	{
		SensorDumpState(cl);
		return	CLI_PROMPT;
	}
	if	(  strncmp(buf,"sendsensor",10) == 0
		|| strncmp(buf,"recvsensor",10) == 0
		|| strncmp(buf,"resetsensor",11) == 0
		|| strncmp(buf,"deletesensor",12) == 0
		|| strncmp(buf,"forgetsensor",12) == 0
		|| strncmp(buf,"configsensor",12) == 0
		|| strncmp(buf,"diasyncsensor",13) == 0
		|| strncmp(buf,"diadeletesensor",15) == 0
		)
	{
		int	target;

		cmd[0]	= '\0';
		arg[0]	= '\0';
		data[0]	= '\0';
		buf	= AdmAcceptKeep(buf,cmd);
		buf	= AdmAdvance(buf);
		buf	= AdmAcceptKeep(buf,arg);
		buf	= AdmAdvance(buf);
		if	(strlen(arg) == 0)
			return	CLI_PROMPT;
		if	(*arg == '*')
			target	= INT_MAX;
		else
		{
			target	= SensorNumFindByAddr(arg);
			if	(target < 0 && *arg >= '0' && *arg <= '9')
				target	= atoi(arg);
		}
		if	(target < 0)
			return	CLI_PROMPT;

		nb	= 0;
		if	(strlen(buf) && strcmp(cmd,"sendsensor") == 0)
		{
//			AdmPrint(cl,"sensor send %s '%s'\n",arg,buf);
			nb	= SensorCmdSend(target,buf);
		}
		if	(strlen(buf) && strcmp(cmd,"recvsensor") == 0)
		{
//			AdmPrint(cl,"sensor recv %s '%s'\n",arg,buf);
			nb	= SensorCmdRecv(target,buf);
		}
		if	(strcmp(cmd,"resetsensor") == 0)
		{
			nb	= SensorCmdReset(target);
		}
		if	(strcmp(cmd,"deletesensor") == 0)
		{
			nb	= SensorCmdDelete(target);
		}
		if	(strcmp(cmd,"forgetsensor") == 0)
		{
			nb	= SensorCmdDiaDelete(target,1);
		}
		if	(strcmp(cmd,"configsensor") == 0)
		{
			nb	= SensorCmdConfig(target);
		}
		if	(strcmp(cmd,"diasyncsensor") == 0)
		{
			nb	= SensorCmdDiaSync(target);
		}
		if	(strcmp(cmd,"diadeletesensor") == 0)
		{
			nb	= SensorCmdDiaDelete(target,0);
		}
		AdmPrint(cl,"#sensor targeted %d\n",nb);
		return	CLI_PROMPT;
	}
	if	( strncmp(buf,"snif",4) == 0 )
	{
		buf	+= 4;
		buf	= AdmAdvance(buf);
		strcpy	(cl->cl_sensor,buf);
		AdmPrint(cl,"snif sensor set to [%s]\n",cl->cl_sensor);
		AdmSnifferCount();
		return	CLI_PROMPT;
	}

	if	( strncmp(buf,"trace",5) == 0 )
	{
		int	v;
		buf	+= 5;
		buf	= AdmAdvance(buf);
		if	(strlen(buf))
		{
			v	= atoi(buf);
			TraceLevel	= v;
			rtl_tracelevel(TraceLevel);
		}
		AdmPrint(cl,"trace=%d debug=%d proto=%d\n",
					TraceLevel,TraceDebug,TraceProto);
		return	CLI_PROMPT;
	}
	if	( strncmp(buf,"diatrace",8) == 0 )
	{
		int	v;
		buf	+= 8;
		buf	= AdmAdvance(buf);
		if	(strlen(buf))
		{
			v	= atoi(buf);
			DiaTraceRequest	= v;
		}
		AdmPrint(cl,"diatrace=%d\n",DiaTraceRequest);
		return	CLI_PROMPT;
	}
	if	( strncmp(buf,"debug",5) == 0 )
	{
		int	v;
		buf	+= 5;
		buf	= AdmAdvance(buf);
		if	(strlen(buf))
		{
			v	= atoi(buf);
			TraceDebug	= v;
		}
		AdmPrint(cl,"trace=%d debug=%d\n",TraceLevel,TraceDebug);
		return	CLI_PROMPT;
	}
	if	(strncmp(buf,"diaipu",6) == 0)
	{
		buf	+= 6;
		buf	= AdmAdvance(buf);
		CmdDiaIpu(cl,buf);
		return	CLI_PROMPT;
	}
	if	(strncmp(buf,"dianet",6) == 0)
	{
		buf	+= 6;
		buf	= AdmAdvance(buf);
		CmdDiaNet(cl,buf);
		return	CLI_PROMPT;
	}

	if	(strncmp(buf,"method",6) == 0)
	{
		buf	+= 6;
		buf	= AdmAdvance(buf);
		CB_DIAcREATErEQUEST("./admtcp", buf);
		return	CLI_PROMPT;
	}
	if	(strncmp(buf,"covcfg",6) == 0)
	{
		buf	+= 6;
		buf	= AdmAdvance(buf);
		CovToCzDump(cl,buf);
		return	CLI_PROMPT;
	}
	if	(strncmp(buf,"ticdata",7) == 0)
	{
		buf	+= 7;
		buf	= AdmAdvance(buf);
		TicDump(cl,buf);
		return	CLI_PROMPT;
	}
	return	CLI_NFOUND;
}

void	AdmZclProtoTee(int in,char *addr,char *bin,int sz)
{
	t_cli	*wh;
	int	i = 0;
	char	tmp[256];
	char	dump[2000];
	t_zcl_msg	mess;
	int		ret;

	if	( ActiveSniffer <= 0)
		return;

	for	(i = 0 ; i < CLI_MAX_FD ; i++)
	{
		wh	= TbCli[i];
		if	(!wh)	continue;
		if	(!strlen(wh->cl_sensor))	continue;
		rtl_timemono(&wh->cl_lasttcpread);
		if	(wh->cl_sensor[0] == '*' || !strcmp(wh->cl_sensor,addr))
		{
			AwBinToStr((unsigned char *)bin,sz,dump,sizeof(dump));
			AdmPrint(wh,"\n%s ZCL %s [%s] : ",rtl_hhmmssms(tmp),
				addr,in?"<<":">>");
			AdmPrint(wh,"[%s]\n",dump);

			dump[0]	= '\0';
			ret	= AwZclDecode((char *)dump,&mess,
						(unsigned char *)bin,sz);
			AdmPrint(wh,"# %08x %s\n",ret,dump);
		}
	}
}
