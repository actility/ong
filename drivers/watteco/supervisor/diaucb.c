
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

/*! @file sensor.c
 *
 */
#include <stdio.h>			/************************************/
#include <stdlib.h>			/*     run in main thread           */
#include <string.h>			/************************************/
#include <unistd.h>
#include <pthread.h>
#include <errno.h>
#include <limits.h>
#include <signal.h>

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

#include "xoapipr.h"

#include "define.h"
#include "state.h"
#include "struct.h"

#include "sensor.h"


#include "cproto.h"
#include "cmnproto.h"
#include "extern.h"

//#define	TST_XO

int	WDiaUCBRequestOk_DiaDevDelOk(t_dia_req *preq,t_cmn_sensor *cmn)
{
	t_sensor	*cp = (cmn?cmn->cmn_self:NULL);

	if	(!cp)
		return	0;

	if	(cp->cp_andforget)
	{	// remove knownsensor
		char	sfile[1024];
		sprintf	(sfile,"%s/knownsensors/%s.xml",
					getenv(GetEnvVarName()),cp->cp_addr);
		unlink(sfile);
	}

	iSensorDelete(cp);

	return	0;
}

int	WDiaUCBRequestOk(char *reqname,t_dia_req *preq,t_cmn_sensor *cmn)
{
#if	TST_XO	// TODO tests only
//	int	parseflags	= XOML_PARSE_OBIX;
	int	parseflags	= 0;
	void	*xo;
#endif
	char	*xotype	= "????";

#if	TST_XO	// TODO tests only
	xo	= WDiaXoLoadResponse(preq,parseflags);
	if	(xo)
		xotype	= XoNmType(xo);
#endif

	RTL_TRDBG(3,"WDiaUCBRequestOk for '%s/%s' xotype='%s'\n",
					reqname,preq->rq_name,xotype);

	if	(strncmp(reqname,"DiaIpu",6) == 0)
		DiaIpuOk++;
	if	(strncmp(reqname,"DiaNet",6) == 0)
		DiaNetOk++;

#if	TST_XO	// TODO tests only
	if	(xo)
		XoFree(xo,1);
#endif

	return	0;
}

int	WDiaUCBRequestTimeout(char *reqname,t_dia_req *preq,t_cmn_sensor *cmn)
{
	RTL_TRDBG(3,"WDiaUCBRequestTimeout for '%s/%s'\n",reqname,preq->rq_name);

	if	(strncmp(reqname,"DiaIpu",6) == 0)
	{
		DiaIpuOk	= 0;
		DiaIpuTmt++;
		DiaIpuStart();
		return	0;
	}
	if	(strncmp(reqname,"DiaNet",6) == 0)
	{
		DiaNetOk	= 0;
		DiaNetTmt++;
		DiaNetStart();
		return	0;
	}
	return	0;
}

int	WDiaUCBRequestError(char *reqname,t_dia_req *preq,t_cmn_sensor *cmn)
{
	RTL_TRDBG(3,"WDiaUCBRequestError for '%s/%s'\n",reqname,preq->rq_name);

	if	(strncmp(reqname,"DiaIpu",6) == 0)
	{
		DiaIpuOk	= 0;
		DiaIpuErr++;
		DiaIpuStart();
	}
	if	(strncmp(reqname,"DiaNet",6) == 0)
	{
		DiaNetOk	= 0;
		DiaNetErr++;
		DiaNetStart();
	}

	return	0;
}


int	WDiaUCBPreLoadTemplate(char *reqname,t_dia_req *preq,char *target,
		t_cmn_sensor *cmn,void **xo,char *templatename)
{
	RTL_TRDBG(3,"WDiaUCBPreLoadTemplate for '%s/%s'\n",reqname,preq->rq_name);

	if	(cmn)
	{
		SetVar("d_ieee",cmn->cmn_ieee);
	}

	if	(cmn && preq)
	{
		char	anum[128];
		sprintf(anum,"%d",preq->rq_app);
		SetVar("a_num",anum);
	}

	if	(cmn && preq && strcmp(reqname,"DiaDatContCreate") == 0)
	{
		char	cnum[128];
		sprintf(cnum,"%04x.0x%04x.%d.m2m",
			preq->rq_cluster,preq->rq_attribut,preq->rq_member);
		SetVar("c_num",cnum);
	}

	if	(cmn && preq && strcmp(reqname,"DiaDevInstCreate") == 0)
	{
		t_sensor	*cp = cmn->cmn_self;
		char	tmp[128];

RTL_TRDBG(3,"SET power variables for '%s'\n",cp->cp_addr);

		sprintf	(tmp,"0x%x",cp->cp_power.m_mode);
		SetVar	("d_powmode",tmp);

		sprintf	(tmp,"%s",AwZclPowerSourceTxt(cp->cp_power.m_source));
		SetVar	("d_powsrc",tmp);


		SetVar	("d_powlvl","");	// l0,l33,l66,l99 ...

		sprintf	(tmp,"%d",cp->cp_power.m_level[0]);
		SetVar	("d_powval",tmp);
	}

	return	0;
}



int	WDiaUCBPostLoadTemplate(char *reqname,t_dia_req *preq,char *target,t_cmn_sensor *cmn,void *xo)
{
	char	tmp[256];
	char	m2mvar[256];
	void	*mdlitf	= NULL;

	RTL_TRDBG(3,"WDiaUCBPostLoadTemplate for '%s/%s'\n",reqname,preq->rq_name);

	if	(!xo)
		return	0;

	if	(strcmp(reqname,"DiaNetInstCreate") == 0)
	{
		SensorSetDeviceListInObj(xo);
		return	0;
	}
	if	(strcmp(reqname,"DiaDevInstCreate") == 0)
	{
		iCmnSensorSetApplicationListInObj(cmn,xo);
		return	0;
	}
	if	(strcmp(reqname,"DiaAppElemCreate") == 0)
	{
		iCmnSensorSetSearchStringInObj(cmn,xo);
		return	0;
	}
	if	(strcmp(reqname,"DiaAppInstCreate") == 0)
	{
		iCmnSensorSetInterfaceListInObj(cmn,preq->rq_app,xo);
		return	0;
	}
	if	(strcmp(reqname,"DiaDatElemCreate") == 0)
	{
		iCmnSensorSetSearchStringInObj(cmn,xo);
		return	0;
	}

	//
	// modeling
	//

	if	(MdlCfg == NULL)
		return	0;


	if	(strcmp(reqname,"DiaDatContCreate") == 0)
	{
		sprintf	(tmp,"0x%04x",preq->rq_cluster);
		mdlitf	= MdlGetInterface(MdlCfg,NULL,"server",tmp);
		if	(mdlitf)
		{
			sprintf	(m2mvar,"0x%04x.0x%04x.%d.m2m",
			    preq->rq_cluster,preq->rq_attribut,preq->rq_member);
			MdlConfigureDataContainerTemplate(mdlitf,xo,m2mvar);
		}
	}

	return	0;
}
