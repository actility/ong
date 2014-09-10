
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

/*! @file main.c
 *
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <errno.h>
#include <limits.h>
#include <signal.h>
#include <poll.h>
#include <ctype.h>
#ifndef	MACOSX
#include <malloc.h>
#endif

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#include "_whatstr.h"

#include "rtlbase.h"
#include "rtlimsg.h"
#include "xoapipr.h"
#include "dIa.h"

#include "adaptor.h"

#include "define.h"
#include "struct.h"

#include "cproto.h"
#include "cmnproto.h"
#include "extern.h"

#ifdef	MACOSX
t_objet_ref	*XoTabObjUser ;
int		XoNbObjUser  = 0 ;
#endif

int	Operating	= 1;


void	*MdlCfg;	// => modelconfig.xml if WithModeling != 0
void	*CovCfg;	// => modelconfig.xml if WithCovConfig != 0

int	RequestPANID	= 0;

char	*TraceFile	= NULL;
int	TraceSize	= 200*1024;	// for each file
int	TraceLevel	= -1;
int	TraceDebug	= 1;
int	TraceProto	= 0;
int	PseudoOng	= 0;
int	SoftwareSensor	= 0;
int	WithDia		= 1;
int	HttpSensorList	= 1;
int	WithModeling	= 0;
int	WithCovConfig	= 1;
int	AllInputCluster	= 0;
int	DuplicateCluster= 1;
int	MultiEndPoint	= 1;


char	*RadioMode		= NULL;	// rf212 | lora
int	DynRadioMode		= 1;		// can change radio mode ?
int	rf212_sleeptime		= 660;
float	rf212_msgfrequence	= 1.0;
int	lora_sleeptime		= 4000;
float	lora_msgfrequence	= 0.2;

int	ZclSleepTime;		// rf212_sleeptime;
float	ZclFreq;		// rf212_msgfrequence;
float	ZclPeriod;		// 1.0 / ZclFreq;
int	ZclTic;			// (int)(ZclPeriod * 10);

int	DiaTraceRequest	= 0;

int	ActiveSniffer	= 0;
void	*MainTbPoll;
void	*MainIQ;

unsigned int	DiaIpuOk;
unsigned int	DiaNetOk;
unsigned int	DiaIpuTmt;
unsigned int	DiaNetTmt;
unsigned int	DiaIpuErr;
unsigned int	DiaNetErr;
unsigned int	DiaRepDrp;

char	*GetAdaptorVersion()
{
	return	AwVersion();
}

/*!
 *
 * @brief calculate the next serial number which can be used to identity any 
 * object/structure/messsage. 0 is not a valid serial number. Mainly used for
 * t_sensor structure to identity sensors in conjonction with a sensor number.
 * @return [1..INT_MAX]
 * 
 */
int	DoSerial()
{
	static	int	Serial	= INT_MAX;

	if	(Serial == INT_MAX)
		Serial	= 1;
	else
		Serial++;
	return	Serial;
}

/*!
 *
 * @brief called by main loop every 100 ms.
 * @return void
 * 
 */
void	DoClockMs()
{
#if	0
	RTL_TRDBG(1,"DoClockMs()\n");
#endif
	DiaClientClockMs(0);
	ZclClockMs(0);
}

/*!
 *
 * @brief called by main loop every 1 second.
 * @return void
 * 
 */
void	DoClockSc()
{
	static	unsigned	int	nbclock	= 0;
	time_t	now		= 0;

//	RTL_TRDBG(1,"DoClockSc()\n");
	nbclock++;
	rtl_timemono(&now);



	AdmClockSc(now);
	ZclClockSc(now);
	DiaClockSc(now);
	SensorClockSc(now);

}

/*!
 *
 * @brief called by main loop to dispatch an internal message
 * @return void
 * 
 */
void	DoInternalEvent(t_imsg *imsg)
{
	RTL_TRDBG(1,"receive event cl=%d ty=%d\n",imsg->im_class,imsg->im_type);
	switch(imsg->im_class)
	{
	case	IM_DEF :
		switch(imsg->im_type)
		{
		case	IM_LIST_SENSOR_UPDATE :
			DoSensorDetection(1);
		break;
		case	IM_LIST_SENSOR_SAME :
		break;
		case	IM_NETWORK_ID_UPDATE :
//			DiaSetRequestMode(RQT_UPDATE_ELEM|RQT_UPDATE_CONT);
			DiaIpuStart();
//			DiaSetRequestMode(RQT_UPDATE_ELEM|RQT_UPDATE_CONT);
			DiaNetStart();
		break;
		case IM_SERVICE_STATUS_RQST :
			ServiceStatusResponse();
		break;
		}
	break;
	case	IM_SENSOR :
		SensorEvent(imsg,NULL);
	break;
	}
}

/*!
 *
 * @brief called by main loop to dispatch an internal timer
 * @return void
 * 
 */
void	DoInternalTimer(t_imsg *imsg)
{
	u_int	szmalloc;
#ifndef	MACOSX
	struct	mallinfo info;
	info	= mallinfo();
	szmalloc= info.uordblks;
#else
	szmalloc= 0;
#endif

	RTL_TRDBG(1,"receive timer cl=%d ty=%d vsize=%ld alloc=%u\n",
		imsg->im_class,imsg->im_type,rtl_vsize(getpid()),szmalloc);

	switch(imsg->im_class)
	{
	case	IM_DEF :
		switch(imsg->im_type)
		{
		case	IM_TIMER_GEN :
		rtl_imsgAdd(MainIQ,
		rtl_timerAlloc(IM_DEF,IM_TIMER_GEN,IM_TIMER_GEN_V,NULL,0));
		break;
		}
	break;
	case	IM_SENSOR :
		SensorEvent(imsg,NULL);
	break;
	}
}

/*!
 *
 * @brief fake code ... just to draw it in doxygen+graphviz graphs
 * @return void
 * 
 */
void	FAKE_RTL_POLL()
{
	if	(0)
	{
		CB_AdmTcpListen(NULL,0,NULL,NULL,0);
		CB_AdmTcpRequest(NULL,0,NULL,NULL,0);
		CB_AdmTcpEvent(NULL,0,NULL,NULL,0);

		CB_ZclUdpRequest(NULL,0,NULL,NULL,0);
		CB_ZclUdpEvent(NULL,0,NULL,NULL,0);

		CB_DiaUdpRequest(NULL,0,NULL,NULL,0);
		CB_DiaUdpEvent(NULL,0,NULL,NULL,0);
	}
}

/*!
 *
 * @brief the loop of the main thread
 * 	- treat internal events
 * 	- call rtl_poll() to wait for external events
 * 	- call DoClockMs() and DoClockSc() if necessary
 * 	- treat internal timers
 * @return void
 * 
 */
void	MainLoop()
{
	time_t	lasttimems	= 0;
	time_t	lasttimesc	= 0;
	time_t	now		= 0;
	int	ret;

	t_imsg	*msg;

	FAKE_RTL_POLL();

	while	(1)
	{
		// internal events
		while ((msg= rtl_imsgGet(MainIQ,IMSG_MSG)) != NULL)
		{
			DoInternalEvent(msg);
			rtl_imsgFree(msg);
		}

		// external events
		ret	= rtl_poll(MainTbPoll,/*MSTIC*/10);

		// clocks
		now	= rtl_tmmsmono();
		if	(ABS(now-lasttimems) >= 100)
		{
			DoClockMs();
			lasttimems	= now;
		}
		if	(ABS(now-lasttimesc) >= 1000)
		{
			DoClockSc();
			lasttimesc	= now;
		}

		// internal timer
		while ((msg= rtl_imsgGet(MainIQ,IMSG_TIMER)) != NULL)
		{
			DoInternalTimer(msg);
			rtl_imsgFree(msg);
		}
		DiaFlushRequest();
	}
}

/*!
 *
 * @brief start a thread to check the list of known sensors by sending a http
 * request to the router.
 * @return void
 * 
 */
void	HttpStart()
{
	pthread_attr_t	at;
	pthread_t	th;


	if (pthread_attr_init(&at))
	{
		RTL_TRDBG(0,"cannot init thread attr err=%s\n",STRERRNO);
		return;
	}

	pthread_attr_setdetachstate(&at,PTHREAD_CREATE_DETACHED);

	if (pthread_create(&th,&at,HttpRun,NULL))
	{
		RTL_TRDBG(0,"cannot create thread err=%s\n",STRERRNO);
	}
}

void	Usage(char *prg,char *fmt)
{
printf("%s %s usage :\n",prg,fmt);
}

/*!
 *
 * @brief analyze command line options.
 * @return int
 * 
 */
int DoArg(int initial,int argc,char *argv[])
{
	extern char *optarg;
	extern int optind;
	int	c;
	char	*pt;

	char	*fmtgetopt	= "0hXDOSHMCt:d:p:";

	int	i;

	for	(i = 1 ; i < argc ; i++)
	{
		if	(strcmp(argv[i],"--version") == 0)
		{
			pt	= AwVersion();
			printf	("%s\n",rtl_version());
			printf	("%s\n",XoVersion());
			printf	("%s\n",dia_getVersion());
			printf	("%s\n",pt);
			if	(strcmp(pt,watteco_whatStr) != 0)
			{
				printf	("%s\n",watteco_whatStr);
				printf	("compilation required\n");
			}
			exit(0);
		}
		if	(strcmp(argv[i],"--help") == 0)
		{
			Usage(argv[0],fmtgetopt);
			exit(0);
		}
		if	(strcmp(argv[i],"--dev") == 0)
		{
			Operating	= 0;
		}
	}

	if	(initial)
		return	argc;

	while ((c = getopt (argc, argv, fmtgetopt)) != -1) 
	{
	switch(c)
	{
	case	'h' :
		Usage(argv[0],fmtgetopt);
		exit(0);
	break;
	case	't' :
		TraceLevel	= atoi(optarg);
	break;
	case	'd' :
		TraceDebug	= atoi(optarg);
	break;
	case	'p' :
		TraceProto	= atoi(optarg);
	break;
	case	'D' :
		WithDia	= 0;
	break;
	case	'O' :
		PseudoOng	= 1;
	break;
	case	'S' :
		SoftwareSensor	= 1;
	break;
	case	'X' :
		DiaTraceRequest	= 1;
	break;
	case	'H' :
		HttpSensorList	= 0;
	break;
	case	'M' :
		WithModeling	= 0;
	break;
	case	'C' :
		WithCovConfig	= 0;
	break;

	default :
	break;
	}
	}

	return	argc;
}


/*!
 *
 * @brief command line options can also be set/unset with configuration files.
 * @return void
 * 
 */
void	SetOption(char *name,char *value)
{
	if	(strcmp(name,"tracefile") == 0)
	{
		TraceFile	= strdup(value);
		return;
	}
	if	(strcmp(name,"tracesize") == 0)
	{
		TraceSize	= atoi(value);
		return;
	}
	if	(strcmp(name,"tracelevel") == 0)
	{
		TraceLevel	= atoi(value);
		return;
	}
	if	(strcmp(name,"tracedebug") == 0)
	{
		TraceDebug	= atoi(value);
		return;
	}
	if	(strcmp(name,"withdia") == 0)
	{
		WithDia		= atoi(value);
		return;
	}
	if	(strcmp(name,"tracedia") == 0)
	{
		DiaTraceRequest	= atoi(value);
		return;
	}
	if	(strcmp(name,"pseudoong") == 0)
	{
		PseudoOng	= atoi(value);
		return;
	}
	if	(strcmp(name,"softwaresensors") == 0)
	{
		SoftwareSensor	= atoi(value);
		return;
	}
	if	(strcmp(name,"httpsensorlist") == 0)
	{
		HttpSensorList	= atoi(value);
		return;
	}
	if	(strcmp(name,"modeling") == 0)
	{
		WithModeling	= atoi(value);
		return;
	}
	if	(strcmp(name,"covconfig") == 0)
	{
		WithCovConfig	= atoi(value);
		return;
	}
	if	(strcmp(name,"allinputcluster") == 0)
	{
		AllInputCluster	= atoi(value);
		return;
	}
	if	(strcmp(name,"duplicatecluster") == 0)
	{
		DuplicateCluster	= atoi(value);
		return;
	}
	if	(strcmp(name,"multiendpoint") == 0)
	{
		MultiEndPoint	= atoi(value);
		return;
	}
	if	(strcmp(name,"radiomode") == 0)
	{
		if	(*value == '?')
		{
			value++;
			DynRadioMode	= 1;
		}
		else
			DynRadioMode	= 0;
		if	(strcasecmp(value,"rf212") == 0)
		{
			if	(RadioMode)	free(RadioMode);
			RadioMode	= strdup(value);
		}
		if	(strcasecmp(value,"lora") == 0)
		{
			if	(RadioMode)	free(RadioMode);
			RadioMode	= strdup(value);
		}
		return;
	}
	if	(strcmp(name,"rf212_sleeptime") == 0)
	{
		rf212_sleeptime	= atoi(value);
		return;
	}
	if	(strcmp(name,"rf212_msgfrequence") == 0)
	{
		rf212_msgfrequence	= atof(value);
		return;
	}
	if	(strcmp(name,"lora_sleeptime") == 0)
	{
		lora_sleeptime	= atoi(value);
		return;
	}
	if	(strcmp(name,"lora_msgfrequence") == 0)
	{
		lora_msgfrequence	= atof(value);
		return;
	}


	RTL_TRDBG(0,"ERROR parameter/option '%s' not found\n",name);
}

void	DoRadioMode()
{
	// default rf212
	ZclFreq		= rf212_msgfrequence;
	ZclSleepTime	= rf212_sleeptime;

	if	(strcasecmp(RadioMode,"lora") == 0)
	{
		ZclFreq		= lora_msgfrequence;
		ZclSleepTime	= lora_sleeptime;
	}

	if	(ZclFreq > 10.0)
		ZclFreq	= 10.0;
	if	(ZclFreq < 0.1)
		ZclFreq	= 0.1;
	ZclPeriod	= 1.0 / ZclFreq;
	ZclTic		= (int)(ZclPeriod * 10);

	RTL_TRDBG(0,"radiomode='%s' sleep=%d freq=%f period=%f tic=%d dyn=%d\n",
	RadioMode,ZclSleepTime,ZclFreq,ZclPeriod,ZclTic,DynRadioMode);
}

/*!
 *
 * @brief analyze configuration files for command line options and variables
 * @param hot cold or hot configuration phase
 * @param custom analyzing the custom or the default configuration file
 * @param cfg the file configuration path
 * @return void
 * 
 */
void	DoConfig(int hot,int custom,char *fcfg)
{
	void	*o;
	void	*var;
	int	parseerr;
	int	parseflags	= XOML_PARSE_OBIX;

	int	nb;
	int	i;

	o	= XoReadXmlEx(fcfg,NULL,parseflags,&parseerr);
	if	(parseerr > 0)
	{
		RTL_TRDBG(0,"ERROR config file '%s' parsing error=%d \n",
					fcfg,parseerr);
		return;
	}
	if	(parseerr < 0)
	{
		RTL_TRDBG(0,"ERROR config file '%s' loading error=%d \n",
					fcfg,parseerr);
		return;
	}

	if (!o)
	{
		RTL_TRDBG(0,"ERROR config file '%s' not found\n",fcfg);
		return;
	}
//	XoSaveAscii(o,stdout);
	nb	= XoNmNbInAttr(o,"[name=parameters].[]",0);
	for	(i = 0 ; i < nb ; i++)
	{
		char	*name;
		char	*value;

		var	= XoNmGetAttr(o,"[name=parameters].[%d]",0,i);
		if	(!var)	continue;

		name	= XoNmGetAttr(var,"name",0);
		value	= XoNmGetAttr(var,"val",0);

		RTL_TRDBG(0,"hot=%d cust=%d parameter='%s' value='%s'\n",
			hot,custom,name,value);
		if	(name && *name && value && *value)
		{
			SetOption(name,value);
		}
	}
	nb	= XoNmNbInAttr(o,"[name=variables].[]",0);
	for	(i = 0 ; i < nb ; i++)
	{
		char	*name;
		char	*value;

		var	= XoNmGetAttr(o,"[name=variables].[%d]",0,i);
		if	(!var)	continue;

		name	= XoNmGetAttr(var,"name",0);
		value	= XoNmGetAttr(var,"val",0);

		RTL_TRDBG(0,"hot=%d cust=%d variable='%s' value='%s'\n",
			hot,custom,name,value);
		if	(name && *name && value && *value)
		{
			SetVar(name,value);
		}
	}

	// custom configuration can not change m2m mapping
	nb	= XoNmNbInAttr(o,"[name=mapping].[]",0);
	for	(i = 0 ; custom == 0 && i < nb ; i++)
	{
		char	*name;
		char	*value;

		var	= XoNmGetAttr(o,"[name=mapping].[%d]",0,i);
		if	(!var)	continue;

		name	= XoNmGetAttr(var,"name",0);
		value	= XoNmGetAttr(var,"val",0);

		RTL_TRDBG(0,"hot=%d cust=%d mapping='%s' value='%s'\n",
			hot,custom,name,value);
		if	(name && *name && value && *value)
		{
			AddVar(name);
			SetVar(name,value);
		}
	}

	if	(MultiEndPoint == 0)
		DuplicateCluster	= 0;

	if	(MultiEndPoint)
		SoftwareSensor		= 0;

	XoFree	(o,1);
}

void	ServiceStatusResponse()
{
	FILE	*f;
	char	tmp[256];

	f	= fopen(SERVICE_STATUS_FILE,"w");
	if	(f)
	{
		sprintf	(tmp,"STATUS=RUNNING+SIG+IMSG");
		fprintf	(f,"%s",tmp);
		fclose	(f);
		RTL_TRDBG(0,"Service status '%s'\n",tmp);
	}
}

void	ServiceStatus(int sig)
{
	FILE	*f;
	char	tmp[256];

	signal	(SIGUSR1,SIG_IGN);
	RTL_TRDBG(0,"Service status sig=%d\n",sig);

	f	= fopen(SERVICE_STATUS_FILE,"w");
	if	(f)
	{
		sprintf	(tmp,"STATUS=RUNNING+SIG");
		fprintf	(f,"%s",tmp);
		fclose	(f);
		RTL_TRDBG(0,"Service status '%s'\n",tmp);
	}
	rtl_imsgAdd(MainIQ,
		rtl_imsgAlloc(IM_DEF,IM_SERVICE_STATUS_RQST,NULL,0));

	signal	(SIGUSR1,ServiceStatus);
}

void	ServiceStop(int sig)
{

	RTL_TRDBG(0,"Service stopped sig=%d\n",sig);
	sleep(3);
	exit(0);
}

char	*DoFilePid()
{
	static	char	file[128];
	char	tmp[128];
	char	*pt;

	file[0]	= '\0';
	pt	= getenv("ROOTACT");
	if	(pt)
		sprintf	(file,"%s",pt);
	sprintf(tmp,"/var/run/%s.pid",GetAdaptorName());
	strcat	(file,tmp);
	return	file;
}

void	ServiceWritePid()
{
	FILE	*f;

	f	= fopen(DoFilePid(),"w");
	if	(f)
	{
		fprintf(f,"%d\n",getpid());
		fclose(f);
	}
}

void	ServiceExit()
{
	unlink	(DoFilePid());
}




/*!
 *
 * @brief watteco sensors supervision
 * 	- verify $ROOTACT and $GetEnvVarName() environment variables
 * 	- first analyze of command line options (for --version)
 * 	- initialize traces module
 * 	- initialize poll and timers/messages modules
 * 	- initialize xo module and load M2M and adaptor specific definitions
 * 	- initialize variables module
 * 	- load default configuration
 * 	- load custom configuration
 * 	- second analyze of command line options to force some options
 * 	- initialize zcl/udp module
 * 	- initialize adm/tcp module
 * 	- initialize dia/udp module
 * 	- start http thread
 * 	- create sensor for knwon sensors list
 * 	- start DIA request for IPU and NETWORK
 * 	- allocate general timer 
 * 	- enter main loop
 *
 * @return int
 * 
 */
int main(int argc,char *argv[])
{
	char	cfgroot[512];
	char	cfgcust[512];
	char	tmp[512];
	char	dir[512];
	char	*pt;
	int	initial;
	int	hot;
	int	custom;
	int	i;

	TraceLevel	= 0;

	DoArg(initial=1,argc,argv);

	atexit(ServiceExit);
	ServiceWritePid();

	signal	(SIGHUP,SIG_IGN);
	signal	(SIGPIPE,SIG_IGN);
	signal	(SIGTERM,ServiceStop);
	signal	(SIGUSR1,ServiceStatus);

	RadioMode	= strdup("rf212");

	if	(!Operating)
	{
		pt	= getenv("ROOTACT");
		if	((!pt || !*pt))
		{
			printf("%s unset, abort\n","ROOTACT");
			exit(1);
		}
		sprintf	(dir,"%s/%s/supervisor",pt,GetAdaptorName());
		if	(chdir(dir) != 0)
		{
			printf("cannot chdir '%s'\n",dir);
			exit(1);
		}
		sprintf	(cfgroot,"%s/%s/config/spvconfig.xml",pt,GetAdaptorName());

		pt	= getenv(GetEnvVarName());
		if	((!pt || !*pt))
		{
			sprintf(tmp,"%s/usr/data/%s",getenv("ROOTACT"),GetAdaptorName());
			printf("%s unset, use '%s'\n",GetEnvVarName(),tmp);
			setenv(GetEnvVarName(),tmp,1);
			pt	= tmp;
		}
		sprintf	(cfgcust,"%s/config/spvconfig.xml",pt);

		if	(strstr(argv[0],"tmodel.x"))
		{
			MdlMain(argc,argv,cfgroot,cfgcust);
			exit(0);
		}

		sprintf	(dir,"%s/knownsensors",pt);
		rtl_mkdirp(dir);
		printf("+ %s\n",dir);
		sprintf	(dir,"%s/sensorconfig",pt);
		rtl_mkdirp(dir);
		printf("+ %s\n",dir);
	}
	else
	{
		pt	= getenv("ROOTACT");
		if	((!pt || !*pt))
		{
			printf("%s unset, abort\n","ROOTACT");
			exit(1);
		}
#if	0
		sprintf	(dir,"%s/bin",pt);
		if	(chdir(dir) != 0)
		{
			printf("cannot chdir '%s'\n",dir);
			exit(1);
		}
#endif
		sprintf(tmp,"%s/usr/data/%s",getenv("ROOTACT"),GetAdaptorName());
		setenv(GetEnvVarName(),tmp,1);
		sprintf	(dir,"%s/knownsensors",tmp);
		rtl_mkdirp(dir);
		printf("+ %s\n",dir);
		sprintf	(dir,"%s/sensorconfig",tmp);
		rtl_mkdirp(dir);
		printf("+ %s\n",dir);

		sprintf	(cfgroot,"%s/etc/%s/spvconfig.xml",pt,GetAdaptorName());
		sprintf	(cfgcust,"%s/usr/etc/%s/spvconfig.xml",
							pt,GetAdaptorName());

	}


	rtl_init();
	MainTbPoll	= rtl_pollInit();
	MainIQ		= rtl_imsgInitIq();

	XoInit(0);
	if	(!Operating)
	{	// devel
		sprintf	(tmp,"%s/m2mxoref/xoref",getenv("ROOTACT"));
		XoLoadNameSpaceDir(tmp);
		XoLoadNameSpaceDir(".");
//		DiaSetDefaultRequestMode(RQT_UPDATE_ELEM|RQT_UPDATE_CONT);
	}
	else
	{
		sprintf	(tmp,"%s/etc/xo",getenv("ROOTACT"));
		XoLoadNameSpaceDir(tmp);
		sprintf	(tmp,"%s/etc/%s",getenv("ROOTACT"),GetAdaptorName());
		XoLoadNameSpaceDir(tmp);
	}

	XoDumpNameSpace(stdout);

	if	(LoadM2MXoRef() < 0)
		exit(1);

	if	(LoadDrvXoRef(GetAdaptorName()) < 0)
		exit(1);

	TraceLevel	= -1;

	VarInit();	// before DoConfig()
	tmp[0]	= '\0';
	if	(!gethostname(tmp,sizeof(tmp)))
	{
		for (i = 0 ; tmp[i] ; i++)
			tmp[i]	= tolower(tmp[i]);
		SetVar	("w_boxname",tmp);
	}
	DoConfig(hot=0,custom=0,cfgroot);
	DoConfig(hot=0,custom=1,cfgcust);

	// convert boxname and domainname to lowercase
	sprintf(tmp,"%s", GetVar("w_boxname"));
	for (i = 0; tmp[i]; i++) tmp[i] = tolower(tmp[i]);
	SetVar("w_boxname", tmp);
	sprintf(tmp,"%s", GetVar("w_domainname"));
	for (i = 0; tmp[i]; i++) tmp[i] = tolower(tmp[i]);
	SetVar("w_domainname", tmp);

	printf	("w_boxname='%s'\n",GetVar("w_boxname"));
	printf	("w_domainname='%s'\n",GetVar("w_domainname"));

	DoArg(initial=0,argc,argv);
	if	(!TraceFile || !*TraceFile)
	{
		sprintf	(dir,"%s/var/log/%s",
			getenv("ROOTACT"),GetAdaptorName());
		rtl_mkdirp(dir);
		printf("+ %s\n",dir);
		sprintf	(dir,"%s/var/log/%s/%s.log",
			getenv("ROOTACT"),GetAdaptorName(),GetAdaptorName());
		TraceFile	= dir;
	}
	else
	{
	}
//	rtl_tracestack0((void *)&custom);
	rtl_tracemutex();
	rtl_tracelevel(TraceLevel);
	if	(strcmp(TraceFile,"stderr") != 0)
	{
		rtl_tracesizemax(TraceSize);
		rtl_tracerotate(TraceFile);
	}

	RTL_TRDBG(0,"start awspv.x/main th=%lx pid=%d stack=%p\n",
			(long)pthread_self(),getpid(),&custom);
	RTL_TRDBG(0,"%s\n",rtl_version());
	RTL_TRDBG(0,"%s\n",XoVersion());
	RTL_TRDBG(0,"%s\n",dia_getVersion());
	RTL_TRDBG(0,"%s\n",AwVersion());
	RTL_TRDBG(0,"%s\n",DrvcommonVersion());

	RTL_TRDBG(0,"Operating=%d\n",Operating);
	RTL_TRDBG(0,"TraceLevel=%d\n",TraceLevel);
	RTL_TRDBG(0,"TraceDebug=%d\n",TraceDebug);
	RTL_TRDBG(0,"TraceProto=%d\n",TraceProto);
	RTL_TRDBG(0,"TraceDia=%d\n",DiaTraceRequest);
	RTL_TRDBG(0,"WithDia=%d\n",WithDia);
	RTL_TRDBG(0,"SoftwareSensor=%d\n",SoftwareSensor);
	RTL_TRDBG(0,"PseudoOng=%d\n",PseudoOng);
	RTL_TRDBG(0,"HttpSensorList=%d\n",HttpSensorList);
	RTL_TRDBG(0,"WithModeling=%d\n",WithModeling);
	RTL_TRDBG(0,"WithCovConfig=%d\n",WithCovConfig);
	RTL_TRDBG(0,"AllInputCluster=%d\n",AllInputCluster);
	RTL_TRDBG(0,"DuplicateCluster=%d\n",DuplicateCluster);
	RTL_TRDBG(0,"MultiEndPoint=%d\n",MultiEndPoint);

	MdlCfg	= NULL;

	if	(WithModeling || WithCovConfig)
	{
		void	*obj;
		obj	= MdlLoad(hot=0);
		if	(!obj)
			exit(1);
		if	(WithModeling)
			MdlCfg	= obj;
		if	(WithCovConfig)
		{
			CovCfg	= obj;
			CovToZclLoadTranslator(CovCfg);
		}
	}

	DoRadioMode();

	AwZclInitProto();
	AdmTcpInit();
	AdmTcpSetCB(AdmCmd);
	AdmTcpSetCBFree(AdmCliFree);
	ZclUdpInit();

	if	(WithDia)
		DiaUdpInit();

	if	(HttpSensorList)
		HttpStart();


	// we start with known sensors
	SensorCreateKnown();



	rtl_imsgAdd(MainIQ,
	rtl_timerAlloc(IM_DEF,IM_TIMER_GEN,IM_TIMER_GEN_V,NULL,0));
	MainLoop();

	RTL_TRDBG(0,"end !!! awspv.x/main th=%lx\n",(long)pthread_self());
	exit(1);
}


int MdlMain(int argc,char *argv[],char *cfgroot,char *cfgcust)
{
	int	hot;
	int	custom;

	int	c;

	char	*prefixe= NULL;
	char	*type	= "server";
	char	*cluster= NULL;
	char	*mapping= NULL;
	int	numcluster;
	char	tmp[512];

	extern char *optarg;
	extern int optind;
	char	*fmtgetopt	= "t:P:T:C:M:";

	void	*mdlitf;
	void	*templ;
	int	parseflags	= XOML_PARSE_OBIX;

	void	*cov;
	int	nb;
	int	i;

	RadioMode = strdup("rf212");

	rtl_init();

	RTL_TRDBG(0,"start tmodel.x/main th=%lx pid=%d stack=%p\n",
			(long)pthread_self(),getpid(),&custom);

	XoInit(0);
	XoLoadNameSpaceDir(".");

	if	(LoadM2MXoRef() < 0)
		exit(1);

	if	(LoadDrvXoRef(GetAdaptorName()) < 0)
		exit(1);


	VarInit();	// before DoConfig()
	DoConfig(hot=0,custom=0,cfgroot);
	DoConfig(hot=0,custom=1,cfgcust);
	rtl_tracelevel(TraceLevel);

	MdlCfg	= MdlLoad(hot=0);
	if	(!MdlCfg)
		exit(1);

//	XoSaveAscii(MdlCfg,stdout);

	while ((c = getopt (argc, argv, fmtgetopt)) != -1) 
	{
	switch(c)
	{
	case	'h' :
// TODO		Usage(argv[0],fmtgetopt);
		exit(0);
	break;
	case	't' :
		TraceLevel	= atoi(optarg);
	break;
	case	'd' :
		TraceDebug	= atoi(optarg);
	break;
	case	'P' :
		prefixe		= optarg;
	break;
	case	'T' :
		type		= optarg;
	break;
	case	'C' :
		cluster		= optarg;
		if	(*cluster != '0' || *(cluster+1) != 'x')
		{
			sprintf	(tmp,"0x%s",cluster);
			cluster	= strdup(tmp);
		}
		sscanf(cluster,"%x",&numcluster);
	break;
	case	'M' :
		mapping		= optarg;
	break;
	}
	}

	CovToZclLoadTranslator(MdlCfg);

	if	(!cluster)
	{
		RTL_TRDBG(0,"no interface model ...\n");
		exit(1);
	}

	RTL_TRDBG(0,"search interface model '%s' '%s' '%s'\n",
							prefixe,type,cluster);

	mdlitf	= MdlGetInterface(MdlCfg,prefixe,type,cluster);
	if	(!mdlitf)
	{
		RTL_TRDBG(0,"interface model not found '%s' '%s' '%s'\n",
							prefixe,type,cluster);
		exit(1);
	}

	//int MdlGetNbCovConfigurationEntries(void *itf)
	{
	nb	= XoNmNbInAttr(mdlitf,"ong:covConfigurations");
	}
	printf	("covConfiguration count=%d\n",nb);
	for	(i = 0 ; i < nb ; i++)
	{
		char	*min,*max,*chg;
		//void *MdlGetCovConfigurationByNumEntry(void *itf, int n)
		{
		cov = XoNmGetAttr(mdlitf,"ong:covConfigurations[%d]",NULL,i);
		}
		if	(!cov)	break;
		min	= XoNmGetAttr(cov,"ong:minInterval",0,0);
		max	= XoNmGetAttr(cov,"ong:maxInterval",0,0);
		chg	= XoNmGetAttr(cov,"ong:minCOV",0,0);
		printf("min='%s' max='%s' chg='%s'\n",min,max,chg);
	}

	SetVar("d_ieee","0xXXXXXXXXXXXX");

	sprintf	(tmp,"itf_inst_%04x_%s.xml",numcluster,type);
	templ	= WXoNewTemplate(tmp,parseflags);
	if	(!templ)
	{
		RTL_TRDBG(0,"interface template not found '%s' %04x\n",
							type,numcluster);
		exit(1);
	}


	MdlConfigureInterfaceTemplate(mdlitf,templ);


//	XoSaveAscii(templ,stdout);
//

	WXoSaveXml(templ,stdout);

	if	(!mapping)
		exit(0);

	SetVar("c_num",mapping);
	sprintf	(tmp,"dat_cont.xml");
	templ	= WXoNewTemplate(tmp,0);
	if	(!templ)
	{
		RTL_TRDBG(0,"container template not found '%s'\n",tmp);
		exit(1);
	}

	if	(MdlCfg)
		MdlConfigureDataContainerTemplate(mdlitf,templ,mapping);

	WXoSaveXml(templ,stdout);



	return	0;
}
