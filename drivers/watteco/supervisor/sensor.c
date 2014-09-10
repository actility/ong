
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
#include <stdarg.h>
#include <unistd.h>
#include <pthread.h>
#include <errno.h>
#include <limits.h>
#include <signal.h>
#include <libgen.h>

#include <math.h>

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

extern	void	*MainIQ;


void	iSensorRetrieveAttrCompletion(t_sensor *cp,int err,t_zcl_msg *zmsg);

static	unsigned int	TbSensorState[ST_CP_STATE_MAX][2];

static	t_sensor	*TbSensor[MAX_SSR_PER_RTR];
static	int		NbSensor;
static	unsigned int	NbCongestion;

static	char	*Pani6;

static	t_tic_histo	TbTicHistory[10];
static	int		NbTicHistory = sizeof(TbTicHistory)/sizeof(t_tic_histo);

static	unsigned int	TbImplicitClusterIn[] = { W_CLU_CONFIGURATION };
static	int	NbImplicitClusterIn = sizeof(TbImplicitClusterIn)/sizeof(int);

static	int	CmpTicHistory(const void *o1, const void *o2)
{
	t_tic_histo	*h1	= (t_tic_histo *)o1;
	t_tic_histo	*h2	= (t_tic_histo *)o2;

	return	h1->th_when - h2->th_when;
}

// TODO il faut trier un tableau de pointeur sur le tableau circulaire
// sinon c'est un peu faux
static	void	SortTicHistory()
{
	qsort(TbTicHistory,(size_t)NbTicHistory,sizeof(t_tic_histo),
				CmpTicHistory);
}


static	void	AddTicHistory(char *addr,int type,char *data,int sz,unsigned int clat)
{
	static	unsigned int	 pos;
	int	idx;
	t_tic_histo	*h;

	// data is a tic binary frame
	// sz is the lenght of the binary frame
	
	if	(NbTicHistory <= 0)
		return;
	if	(!data || sz <= 0)
		return;

	idx	= pos % (NbTicHistory);
	h	= &TbTicHistory[idx];
	if	(h->th_data)
		free(h->th_data);
	if	(h->th_addr)
		free(h->th_addr);

	h->th_addr	= strdup(addr);
	if	(!h->th_addr)
		return;
	h->th_when	= rtl_timemono(NULL);
	h->th_type	= type;
	h->th_clat	= clat;
	h->th_sz	= sz;
	h->th_data	= (char *)malloc(sz+2);
	if	(!h->th_data)
		return;
	memcpy	(h->th_data,data,sz);
RTL_TRDBG(3,"add an entry in tic history table szdata=%d\n",sz);

	pos++;
}


static	void	DoWaitRespCommand(t_sensor *cp,int type,unsigned int clat)
{
	int	nbmsg	= ZclMsgPending;

	// DoWaitResp() called just after ZclUdpSendMessage() ignore the latest
	// message
	nbmsg--;	
	if	(nbmsg < 0)
		nbmsg	= 0;

	cp->cp_zcl_sendAt	= rtl_timemono(NULL);
	cp->cp_zcl_sendcount++;
	cp->cp_zcl_cmndcount++;

	cp->cp_zcl_waitResp	= type;
	cp->cp_zcl_waitClat	= clat;
	cp->cp_zcl_waitTime	= ZclSleepTime + (nbmsg * ZclPeriod);

RTL_TRDBG(1,"cp=%p '%s' will wait %d for (%d/%08x) pending=%d\n",
		cp,cp->cp_addr,cp->cp_zcl_waitTime,
		cp->cp_zcl_waitResp,cp->cp_zcl_waitClat,nbmsg);
}

static	int	Congestion()
{
	int		i;
	t_sensor	*cp;
	int		nb = 0;


	// TODO
	return	0;

	for	(i = 0 ; i < MAX_SSR_PER_RTR ; i++)
	{
		cp	= TbSensor[i];
		if	(!cp)		continue;
		if	(cp->cp_zcl_waitResp)
			nb++;
	}

	if	(nb > 0 && nb >= 5)
	{
		RTL_TRDBG(1,"congestion %d(%u)\n",nb,NbCongestion);
		NbCongestion++;
	}

	return	nb;
}


static	int	CanReport(int cluster,int attr,int member)
{
	if	(DIA_KO())
	{
		DiaRepDrp++;
		return	0;
	}
	if	(MdlCfg)
	{	// modeling activated check container
		char	m2mvar[256];
		char	*m2mid;

		sprintf	(m2mvar,"0x%04x.0x%04x.%d.m2m",cluster,attr,member);
		m2mid	= GetVar(m2mvar);
		if	(!m2mid || !*m2mid)
		{
			DiaRepDrp++;
RTL_TRDBG(3,"modeling no mapping for '%s' => drop report\n",m2mvar);
			return	0;
		}
	}
	return	1;
}


static	char	*PowerToRawString(t_zcl_power_value *power)
{
	static	char	buf[512];
	char		tmp[512];
	int		i;
	int		lg;

	buf[0]	= '\0';

	if	(power->m_count >= 1)
	{
		sprintf	(tmp,"%d ",power->m_level[0]);
		strcat	(buf,tmp);
	}
	else
	{
		sprintf	(tmp,"%d ",0);
		strcat	(buf,tmp);
	}


	sprintf	(tmp,"0x%x ",power->m_mode);
	strcat	(buf,tmp);

	sprintf	(tmp,"0x%x ",power->m_source);
	strcat	(buf,tmp);
	for	(i = 0; i < power->m_count; i++)
	{
		sprintf	(tmp,"%d,",power->m_level[i]);
		strcat	(buf,tmp);
	}
	lg	= strlen(buf);
	if	(lg)
	{
		if	(buf[lg-1] == ',')
			buf[lg-1]	= '\0';
	}

	return	buf;
}

static	void	AddrToIeee(char *addr,char *dst)
{
	char	tmp[64];
	char	*pt;
	struct sockaddr_in6 conv;

	*dst	= '\0';
	if	(!addr || !*addr)
		return;

	pt	= strchr(addr,'@');
	if	(pt)
		addr	= pt+1;
	if	(!addr || !*addr)
		return;

	if (inet_pton(AF_INET6,addr,&conv.sin6_addr) <= 0)
	{
		RTL_TRDBG(0,"cannot convert addr '%s' to ieee\n",addr);
		return;
	}
	AwBinToStr((unsigned char *)&conv.sin6_addr,16,tmp,sizeof(tmp));
	pt	= tmp+16;

	/* watteco */
	*pt	= '0';
	*(pt+1)	= '2';

	strcpy	(dst,pt);
}

static	void	SensorSetNetId()
{
	static	int	errtraced = 0;
	char	*pt;
	char	pan[128];
	char	net[128];
	char	mode[128];

	if	(!Pani6)
		Pani6	= strdup("");

	pt	= AwGetPanAddr(pan);
	if	(!pt || !*pt)
	{	// file not ready
		if	(errtraced)
			return;
		RTL_TRDBG(0,"wait for PAN@\n");
		errtraced	= 1;
		return;
	}
	if	(strcmp(Pani6,pt))
	{	// first or new PAN@
		RTL_TRDBG(1,"got a new PAN@ '%s' -> '%s'\n",Pani6,pt);
		if (Pani6)
			free(Pani6);
		Pani6	= strdup(pt);
		AddrToIeee(pan,net);
		RTL_TRDBG(1,"got a NETWORK ID '%s' -> '%s'\n",pan,net);
		SetVar("w_netid",net);
		rtl_imsgAdd(MainIQ,
		rtl_imsgAlloc(IM_DEF,IM_NETWORK_ID_UPDATE,NULL,0));
	}
	if	(DynRadioMode )
	{
		pt	= AwGetRadioMode(mode);
		if	(pt && *pt && strcmp(RadioMode,mode))
		{
		RTL_TRDBG(1,"got a new radiomode '%s' -> '%s'\n",RadioMode,pt);
			if	(RadioMode)	free(RadioMode);
			RadioMode	= strdup(pt);
			DoRadioMode();
		}
	}
}

static	char *SensorDescriptorName(int cluster,int attr,int mbr)
{
	return	"";
}

char	*SensorClusterToInterfaceName(int cluster)
{
	switch(cluster)
	{
	case	W_CLU_ONOFF		: return "OnOff";
	case	W_CLU_SIMPLEMETER	: return "SimpleMetering";
	case	W_CLU_TEMPERATURE	: return "TemperatureMeasurement";
	case	W_CLU_HUMIDITY		: return "RelativeHumidityMeasurement";
	case	W_CLU_OCCUPANCY		: return "x-Occupany";
	case	W_CLU_ANALOGINPUT	: return "x-AnalogicInput";
	case	W_CLU_BINARYINPUT	: return "x-BinaryInput";
	case	W_CLU_ILLUMINANCE	: return "x-Illuminance";
	case	W_CLU_TICICE		: return "x-CounterTicICE";
	case	W_CLU_TICCBE		: return "x-CounterTicCBE";
	case	W_CLU_TICCJE		: return "x-CounterTicCJE";

	case	W_CLU_CONFIGURATION	: return "x-Configuration";
	}
	RTL_TRDBG(0,"cannot find cluster name 0x%0x\n",cluster);
	return	"";
}


static	char	*SensorStateTxt(int state)
{
	switch	(state)
	{
	case	ST_CP_NULL :		return	"CP_NULL";
	case	ST_CP_ALLOCATED :	return	"CP_ALLOCATED";
	case	ST_CP_WAITINGPANID :	return	"CP_WAITINGPANID";
	case	ST_CP_WAITINGDESC :	return	"CP_WAITINGDESC";
	case	ST_CP_CONFIGURING :	return	"CP_CONFIGURING";
	case	ST_CP_RUNNING :		return	"CP_RUNNING";
	case	ST_CP_DISCONNECTED :	return	"CP_DISCONNECTED";
	case	ST_CP_ERROR :		return	"CP_ERROR";
	default	:			return	NULL;
	}
}

static	void	ForceInputCluster(t_zcl_simple_desc *desc)
{
	int	i;

	for	(i = 0 ; i < desc->m_nbclout ; i++)
	{
		desc->m_clin[desc->m_nbclin]	= desc->m_clout[i];
		desc->m_clout[i]		= 0;
		desc->m_nbclin++;
	}
	desc->m_nbclout	= 0;
}

static	char	*OnOffTxt(int relay)
{
	switch	(relay)
	{
	case	0	:	return	"off";
	case	1	:	return	"on";
	default		:	return	"unk";
	}
}

static	int	OkClusterIn(unsigned int clid,t_zcl_simple_desc *desc)
{
	int	i;

	for	(i = 0 ; i < desc->m_nbclin ; i++)
	{
		if	(clid == desc->m_clin[i])
			return	1;
	}
	return	0;
}

static	int	OkClusterOut(unsigned int clid,t_zcl_simple_desc *desc)
{
	int	i;

	for	(i = 0 ; i < desc->m_nbclout ; i++)
	{
		if	(clid == desc->m_clout[i])
			return	1;
	}
	return	0;
}

static	int	AddImplicitCluster(t_zcl_simple_desc *desc)
{
	int	n = 0;
	int	i = 0;
	unsigned int clin;

	for	(i = 0 ; i < NbImplicitClusterIn ; i++)
	{
		clin	= TbImplicitClusterIn[i];
		if	(!OkClusterIn(clin,desc))
		{
			desc->m_clin[desc->m_nbclin]	= clin;
			desc->m_epin[desc->m_nbclin]	= 0;
			desc->m_nbclin++;
			n++;
		}
	}

	return	n;
}

static	int	IsImplicitCluster(unsigned int cl)
{
	int	i = 0;

	for	(i = 0 ; i < NbImplicitClusterIn ; i++)
	{
		if	( cl == TbImplicitClusterIn[i])
			return	1;
	}
	return	0;
}

static	void	NoDuplicateCluster(t_zcl_simple_desc *desc)
{
	int	i;
	t_zcl_simple_desc	tmp;

	memset	(&tmp,0,sizeof(tmp));

	for	(i = 0 ; i < desc->m_nbclin ; i++)
	{
		if	(OkClusterIn(desc->m_clin[i],&tmp) <= 0)
		{
			tmp.m_clin[tmp.m_nbclin]	= desc->m_clin[i];
			tmp.m_nbclin++;
		}
	}
	for	(i = 0 ; i < desc->m_nbclout ; i++)
	{
		if	(OkClusterOut(desc->m_clout[i],&tmp) <= 0)
		{
			tmp.m_clout[tmp.m_nbclout]	= desc->m_clout[i];
			tmp.m_nbclout++;
		}
	}

	memcpy	(desc,&tmp,sizeof(tmp));
}

static	t_sensor	*SensorNumSerial(int num,int serial)
{
	if	(num < 0 || num >= MAX_SSR_PER_RTR || TbSensor[num] == NULL)
	{
		RTL_TRDBG(0,"ERROR bad num sensor %d\n",num);
		return	NULL;
	}
	if	(TbSensor[num]->cp_serial != serial)
	{
		RTL_TRDBG(0,"ERROR bad serial sensor %d\n",serial);
		return	NULL;
	}

	return	TbSensor[num];
}

static	t_sensor	*SensorFindByIeee(char *ieee)
{
	int	i;

	for	(i = 0 ; i < MAX_SSR_PER_RTR ; i++)
	{
		if	(TbSensor[i] && strcmp(TbSensor[i]->cp_ieee,ieee) == 0)
		{
			return	TbSensor[i];
		}
	}
	return	NULL;
}

#if	0	// TODO KO
void	iSensorZclSend(t_sensor *cp,void *data,int lg)
{
	t_zcl_msg	*zmsg;

	if	(lg == 0)
	{
		zmsg	= (t_zcl_msg *)data;
		lg	= AwZclEncode(zmsg,NULL,0);
		if	(lg < 0)
		{
			RTL_TRDBG(0,"ERROR cannot encode zcl message\n");
			return;
		}
		data	= (char *)&zmsg->u_pack.m_data;
	}

	if	(ZclUdpSendMessage(cp->cp_addr,data,lg) > 0)
	{
		RTL_TRDBG(0,"ERROR cannot send ZCL to '%s' sz=%d\n", 
				cp->cp_addr,lg);
		return;
	}

	cp->cp_sendAt	= rtl_timemono(NULL);
	cp->cp_sendcount++;
}
#endif

static	void	iSensorZclRqtPanId(t_sensor *cp)
{
	t_zcl_msg	zmsg;
	int		zlg;
	char		*zdata;

	memset	(&zmsg,0,sizeof(zmsg));
	zmsg.m_cmd		= W_CMD_READ_ATTR_RQST;
	zmsg.m_cluster_id	= W_CLU_CONFIGURATION;
	zmsg.m_attr_id		= W_ATT_CONFIGURATION_PANID;
	zdata			= (char *)&zmsg.u_pack.m_data;

#if	0
	RTL_TRDBG(1,"send <read attr panid> to '%s'\n", cp->cp_addr);
	iSensorZclSend(cp,&zmsg,0);
#endif

#if	1
	zlg	= AwZclEncode(&zmsg,NULL,0);
	if	(zlg < 0 || zmsg.m_xxcode == 0)
	{
		RTL_TRDBG(0,"ERROR cannot encode zcl message\n");
		return;
	}
	if	(ZclUdpSendMessage(cp->cp_addr,zdata,zlg) > 0)
	{
		RTL_TRDBG(1,"send <read attr panid> to '%s'\n", cp->cp_addr);
		DoWaitRespCommand(cp,W_CMD_READ_ATTR_RESP,
			W_CLAT(zmsg.m_cluster_id,zmsg.m_attr_id));
	}
#endif
}

static	void	iSensorZclRqtDescMonoEpn(t_sensor *cp)
{
	t_zcl_msg	zmsg;
	int		zlg;
	char		*zdata;

	memset	(&zmsg,0,sizeof(zmsg));
	zmsg.m_cmd		= W_CMD_READ_ATTR_RQST;
	zmsg.m_cluster_id	= W_CLU_CONFIGURATION;
	zmsg.m_attr_id		= W_ATT_CONFIGURATION_DESC;
	zdata			= (char *)&zmsg.u_pack.m_data;

#if	0
	RTL_TRDBG(1,"send <read attr desc> to '%s'\n", cp->cp_addr);
	iSensorZclSend(cp,&zmsg,0);
#endif

#if	1
	zlg	= AwZclEncode(&zmsg,NULL,0);
	if	(zlg < 0 || zmsg.m_xxcode == 0)
	{
		RTL_TRDBG(0,"ERROR cannot encode zcl message\n");
		return;
	}
	if	(ZclUdpSendMessage(cp->cp_addr,zdata,zlg) > 0)
	{
		RTL_TRDBG(1,"send <read attr desc> to '%s'\n", cp->cp_addr);
		DoWaitRespCommand(cp,W_CMD_READ_ATTR_RESP,
				W_CLAT(zmsg.m_cluster_id,zmsg.m_attr_id));
	}
#endif
}

static	void	iSensorZclRqtDescMultiEpn(t_sensor *cp)
{
	t_zcl_msg	zmsg;
	int		zlg;
	char		*zdata;

	memset	(&zmsg,0,sizeof(zmsg));
	zmsg.m_cmd		= W_CMD_READ_ATTR_RQST;
	zmsg.m_cluster_id	= W_CLU_CONFIGURATION;
	zmsg.m_attr_id		= W_ATT_CONFIGURATION_DESCEPN;
	zdata			= (char *)&zmsg.u_pack.m_data;

#if	0
	RTL_TRDBG(1,"send <read attr desc> to '%s'\n", cp->cp_addr);
	iSensorZclSend(cp,&zmsg,0);
#endif

#if	1
	zlg	= AwZclEncode(&zmsg,NULL,0);
	if	(zlg < 0 || zmsg.m_xxcode == 0)
	{
		RTL_TRDBG(0,"ERROR cannot encode zcl message\n");
		return;
	}
	if	(ZclUdpSendMessage(cp->cp_addr,zdata,zlg) > 0)
	{
		RTL_TRDBG(1,"send <read attr descepn> to '%s'\n", cp->cp_addr);
		DoWaitRespCommand(cp,W_CMD_READ_ATTR_RESP,
				W_CLAT(zmsg.m_cluster_id,zmsg.m_attr_id));
	}
#endif
}

static	void	iSensorZclRqtDesc(t_sensor *cp)
{
	if	(MultiEndPoint)
		iSensorZclRqtDescMultiEpn(cp);
	else
		iSensorZclRqtDescMonoEpn(cp);
}


static	void	iSensorZclRqtCovConfig(t_sensor *cp,int index)
{
	t_cov_to_zcl	*cz;
	unsigned int	 clat;
	unsigned char	*data;
	int		epnum;

	// le t_zcl_msg a envoyer est deja code dans cz->cz_data

	if	(index < 0 || index >= cp->cp_nbCov)
	{
		RTL_TRDBG(0,"ERROR bad covconfig index %d\n",index);
		return;
	}

	cz	= cp->cp_tbCov[index];
	// cz->cz_data is watteco binary buffer ready to send but coded for
	// endpoint 0 only
	epnum	= cp->cp_epCov[index];
	data	= (unsigned char *)cz->cz_data;
	*data	= (epnum << 5) | 0x11;
	clat	= W_CLAT(cz->cz_cluster,cz->cz_attribut);

#if	0
	RTL_TRDBG(1,"send <configure cluster> to '%s'\n", cp->cp_addr);
	iSensorZclSend(cp,binbuf,szbin);
#endif

#if	1
	if	(ZclUdpSendMessage(cp->cp_addr,cz->cz_data,cz->cz_sz) > 0)
	{
	RTL_TRDBG(1,"send <covconfigure cluster> to '%s'.%d clat=0x%08x\n", 
				cp->cp_addr,epnum,clat);
		DoWaitRespCommand(cp,W_CMD_REPO_CONF_RESP,clat);

		cz->cz_used++;
	}
#endif
}

static	void	iSensorZclRqtStateOnOff(t_sensor *cp)
{
	t_zcl_msg	zmsg;
	int		zlg;
	char		*zdata;

	memset	(&zmsg,0,sizeof(zmsg));
	zmsg.m_cmd		= W_CMD_READ_ATTR_RQST;
	zmsg.m_cluster_id	= W_CLU_ONOFF;
	zmsg.m_attr_id		= W_ATT_ONOFF_ONOFF;
	zdata			= (char *)&zmsg.u_pack.m_data;

#if	0
	RTL_TRDBG(1,"send <read attr onoff> to '%s'\n", cp->cp_addr);
	iSensorZclSend(cp,&zmsg,0);
#endif

#if	1
	zlg	= AwZclEncode(&zmsg,NULL,0);
	if	(zlg < 0 || zmsg.m_xxcode == 0)
	{
		RTL_TRDBG(0,"ERROR cannot encode zcl message\n");
		return;
	}
	if	(ZclUdpSendMessage(cp->cp_addr,zdata,zlg) > 0)
	{
		RTL_TRDBG(1,"send <read attr onoff> to '%s'\n", cp->cp_addr);
		DoWaitRespCommand(cp,W_CMD_READ_ATTR_RESP,
				W_CLAT(zmsg.m_cluster_id,zmsg.m_attr_id));
	}
#endif
}

static	void	iSensorZclRqtTicType(t_sensor *cp,unsigned int cl,unsigned int attr)
{
	t_zcl_msg	zmsg;
	int		zlg;
	char		*zdata;

	memset	(&zmsg,0,sizeof(zmsg));
	zmsg.m_cmd		= W_CMD_READ_ATTR_RQST;
#if	0
	zmsg.m_cluster_id	= W_CLU_TICICE;
	zmsg.m_attr_id		= W_ATT_TICICE_TYPE;
#endif
	zmsg.m_cluster_id	= cl;
	zmsg.m_attr_id		= attr;
	zdata			= (char *)&zmsg.u_pack.m_data;

#if	0
	RTL_TRDBG(1,"send <read attr onoff> to '%s'\n", cp->cp_addr);
	iSensorZclSend(cp,&zmsg,0);
#endif

#if	1
	zlg	= AwZclEncode(&zmsg,NULL,0);
	if	(zlg < 0 || zmsg.m_xxcode == 0)
	{
		RTL_TRDBG(0,"ERROR cannot encode zcl message\n");
		return;
	}
	if	(ZclUdpSendMessage(cp->cp_addr,zdata,zlg) > 0)
	{
		RTL_TRDBG(1,"send <read attr tictype> to '%s'\n", cp->cp_addr);
		DoWaitRespCommand(cp,W_CMD_READ_ATTR_RESP,
				W_CLAT(zmsg.m_cluster_id,zmsg.m_attr_id));
	}
#endif
}

static	void	iSensorSaveContext(t_sensor *cp)
{
	t_zcl_simple_desc	*desc;

	void	*o;
	int	i;
	int	ret;
	char	tmp[128];
	char	sfile[1024];

	desc	= &(cp->cp_desc);
	if	(desc->m_nbclin + desc->m_nbclout - cp->cp_implictCluster <= 0)
	{
		cp->cp_configured	= 0;
	}

	o	= XoNmNew("watt:sensor");
	XoNmSetAttr(o,"name$","watt:sensor",0);

	sprintf	(tmp,"%d",desc->m_nbep);
	XoNmSetAttr(o,"watt:nbep",tmp,0);

	for	(i = 0 ; i < desc->m_nbclin ; i++)
	{
		if	(IsImplicitCluster(desc->m_clin[i]))
			continue;
		sprintf	(tmp,"%04x:%d",desc->m_clin[i],desc->m_epin[i]);
		XoNmAddInAttr(o,"watt:clusterIn",tmp,0);
	}
	for	(i = 0 ; i < desc->m_nbclout ; i++)
	{
		if	(IsImplicitCluster(desc->m_clout[i]))
			continue;
		sprintf	(tmp,"%04x:%d",desc->m_clout[i],desc->m_epout[i]);
		XoNmAddInAttr(o,"watt:clusterOut",tmp,0);
	}

	sprintf	(tmp,"%d",cp->cp_configured);
	XoNmSetAttr(o,"watt:configured",tmp,0);

	if	(cp->cp_configLock > 0)
	{
		sprintf	(tmp,"%d",cp->cp_configLock);
		XoNmSetAttr(o,"watt:configurationLock",tmp,0);
	}

	sprintf	(tmp,"%d",cp->cp_simulated);
	XoNmSetAttr(o,"watt:simulated",tmp,0);

	if	(cp->cp_nicename && strlen(cp->cp_nicename))
		XoNmSetAttr(o,"watt:nicename",cp->cp_nicename,0);


	sprintf	(tmp,"%d",cp->cp_ticType);
	XoNmSetAttr(o,"watt:tictype",tmp,0);


	sprintf	(sfile,"%s/knownsensors/%s.xml",getenv(GetEnvVarName()),cp->cp_addr);
	ret	= XoWritXmlEx(sfile,o,0,"watt");
	if	(ret < 0)
	{
		RTL_TRDBG(0,"ERROR ret=%d cannot save xml '%s'\n",ret,sfile);
	}
	XoFree(o,1);
}

static	int	iSensorLoadContext(t_sensor *cp)
{
	t_zcl_simple_desc	*desc;

	void	*o;
	int	i;
	int	nb;
	int	ret;
	int	parse;
	int	epnum;
	char	*pt;
	char	sfile[1024];

	desc	= &(cp->cp_desc);
	sprintf	(sfile,"%s/knownsensors/%s.xml",getenv(GetEnvVarName()),cp->cp_addr);

	if	(access(sfile,R_OK) != 0)
	{
		RTL_TRDBG(2,"context does not exist '%s'\n",sfile);
		return	-1;
	}

	o	= XoReadXmlEx(sfile,NULL,0,&parse);
	if	(!o)
	{
		RTL_TRDBG(0,"ERROR ret=%d cannot read xml '%s'\n",parse,sfile);
		return	-2;
	}

	RTL_TRDBG(2,"cp=%p '%s' context retrieve\n",cp,cp->cp_addr);

	pt	= XoNmGetAttr(o,"watt:simulated",&ret);
	if	(pt)
	{
		cp->cp_simulated	= atoi(pt);
	}
	if	(cp->cp_simulated)
	{ // a simulated sensor is never configured and is always unknown
		RTL_TRDBG(2,"cp=%p '%s' context simulated %d\n",
					cp,cp->cp_addr,cp->cp_simulated);
		cp->cp_configured	= 0;
		cp->cp_configLock	= 0;
		desc->m_nbclin		= 0;
		desc->m_nbclout		= 0;
		cp->cp_ctxtLoaded	= 1;

//		XoSaveAscii(o,stdout);
		XoFree(o,1);
		return	1;
	}

	desc->m_nbep	= 1;	// at least one end point
	pt	= XoNmGetAttr(o,"watt:nbep",&ret);
	if	(pt && *pt && atoi(pt) > 1)
		desc->m_nbep	= atoi(pt);

	nb	= XoNmNbInAttr(o,"watt:clusterIn");
	if	(nb > 0)
	{
		if	(nb > sizeof(desc->m_clin)/sizeof(short))
			nb = sizeof(desc->m_clin)/sizeof(short);
		desc->m_nbclin	= nb;
		RTL_TRDBG(2,"cp=%p '%s' context clusterIn %d\n",cp,cp->cp_addr,nb);
		for	(i = 0 ; i < nb ; i++)
		{
			pt	= XoNmGetAttr(o,"watt:clusterIn[%d]",&ret,i);
			if	(!pt || !*pt)	continue;
			epnum	= 0;
			sscanf	(pt,"%x:%d",&parse,&epnum);
			desc->m_clin[i]	= parse;
			desc->m_epin[i]	= epnum;
		}
	}
	nb	= XoNmNbInAttr(o,"watt:clusterOut");
	if	(nb > 0)
	{
		if	(nb > sizeof(desc->m_clout)/sizeof(short))
			nb = sizeof(desc->m_clout)/sizeof(short);
		desc->m_nbclout	= nb;
		RTL_TRDBG(2,"cp=%p '%s' context clusterOut %d\n",cp,cp->cp_addr,nb);
		for	(i = 0 ; i < nb ; i++)
		{
			pt	= XoNmGetAttr(o,"watt:clusterOut[%d]",&ret,i);
			if	(!pt || !*pt)	continue;
			epnum	= 0;
			sscanf	(pt,"%x:%d",&parse,&epnum);
			desc->m_clout[i]	= parse;
			desc->m_epout[i]	= epnum;
		}
	}
	if	(AllInputCluster)
	{
		ForceInputCluster(&(cp->cp_desc));
	}
	if	(!DuplicateCluster)
	{
		NoDuplicateCluster(&(cp->cp_desc));
	}
	cp->cp_implictCluster	= AddImplicitCluster(&(cp->cp_desc));

	pt	= XoNmGetAttr(o,"watt:configured",&ret);
	if	(pt)
	{
		cp->cp_configured	= atoi(pt);
	}
	pt	= XoNmGetAttr(o,"watt:configurationLock",&ret);
	if	(pt && atoi(pt) > 0)
	{
		cp->cp_configLock	= atoi(pt);
		cp->cp_configured	= 1;
	}
	RTL_TRDBG(2,"cp=%p '%s' context configured %d lock=%d\n",
		cp,cp->cp_addr,cp->cp_configured,cp->cp_configLock);

	pt	= XoNmGetAttr(o,"watt:nicename",&ret);
	if	(pt)
	{
		cp->cp_nicename	= strdup(pt);
	}

	pt	= XoNmGetAttr(o,"watt:multiplyvalue",&ret);
	if	(pt)
	{
		cp->cp_multiplyvalue	= atof(pt);
	}
	pt	= XoNmGetAttr(o,"watt:addvalue",&ret);
	if	(pt)
	{
		cp->cp_addvalue	= atof(pt);
	}
	pt	= XoNmGetAttr(o,"watt:tictype",&ret);
	if	(pt)
	{
		cp->cp_ticType	= atoi(pt);
	}

	cp->cp_ctxtLoaded	= 1;
//	XoSaveAscii(o,stdout);
	XoFree(o,1);
	return	1;
}

static	void	iSensorStart(t_sensor *cp)
{
	int	nballocated;
	int	delay;

	cp->cp_startAt	= rtl_timemono(NULL);
	if	(Congestion())
		return;
	if	(cp->cp_startNoDelay)
	{
		cp->cp_startNoDelay	= 0;
		rtl_imsgAdd(MainIQ,
		rtl_imsgAlloc(IM_SENSOR,IM_SENSOR_START,
				(void *)cp->cp_num,cp->cp_serial));
	RTL_TRDBG(2,"cp=%p '%s' started now\n",cp,cp->cp_addr);
		return;
	}

	nballocated	= TbSensorState[ST_CP_ALLOCATED][ST_CP_STATE_CURRENT];
	nballocated--;	// self
	if	(nballocated < 0 || nballocated > MAX_SSR_PER_RTR)
		nballocated	= 0;

//	delay	= (rand() % 5)*1000;
	delay	= (nballocated * 1000 ) + 1000;
	rtl_imsgAddDelayed(MainIQ,
	rtl_imsgAlloc(IM_SENSOR,IM_SENSOR_START,
			(void *)cp->cp_num,cp->cp_serial),delay);
	RTL_TRDBG(2,"cp=%p '%s' will be started in %d because %d allocated\n",
		cp,cp->cp_addr,delay/1000,nballocated);
}

void	iSensorReset(t_sensor *cp)
{
	rtl_imsgAdd(MainIQ,
	rtl_imsgAlloc(IM_SENSOR,IM_SENSOR_RESET,
			(void *)cp->cp_num,cp->cp_serial));
}

void	iSensorDelete(t_sensor *cp)
{
	rtl_imsgAdd(MainIQ,
	rtl_imsgAlloc(IM_SENSOR,IM_SENSOR_DELETE,
			(void *)cp->cp_num,cp->cp_serial));
}

static	void	iSensorClearInfos(t_sensor *cp)
{
	cp->cp_stateOnOff	= -1;
#if	0	// now saved in context
	cp->cp_ticType		= 0xFF;
#endif
}

static	void	iSensorClearZclCounters(t_sensor *cp)
{
	cp->cp_zcl_waitResp	= 0;
	cp->cp_zcl_waitClat	= 0;
	cp->cp_zcl_waitTime	= 0;
	cp->cp_zcl_sendAt	= 0;
	cp->cp_zcl_recvAt	= 0;
	cp->cp_zcl_sendcount	= 0;
	cp->cp_zcl_recvcount	= 0;
	cp->cp_zcl_cmndcount	= 0;
	cp->cp_zcl_respcount	= 0;
	cp->cp_zcl_evntcount	= 0;
	cp->cp_zcl_rerrcount	= 0;
	cp->cp_zcl_methcount	= 0;

	cp->cp_zcl_readAt	= 0;
	cp->cp_zcl_readClat	= 0;
	cp->cp_zcl_readTid	= 0;
}

static	void	iSensorClearDiaCounters(t_sensor *cp)
{
	cp->cp_dia_devok	= 0;
	cp->cp_dia_appok	= 0;
	cp->cp_dia_accok	= 0;
	cp->cp_dia_datok	= 0;
	cp->cp_dia_metok	= 0;
	cp->cp_dia_repok	= 0;
	cp->cp_dia_reperr	= 0;
}

static	void	iSensorClearCov(t_sensor *cp)
{
	cp->cp_nbCov	= 0;
	cp->cp_minMaxCov= 0;
	cp->cp_minMinCov= 0;
	memset	(cp->cp_tbCov,0,sizeof(cp->cp_tbCov));
}

static	void	iSensorState(t_sensor *cp,int state)
{
	if	(state < ST_CP_NULL || state >= ST_CP_STATE_MAX)
	{
		return;
	}

	TbSensorState[state][ST_CP_STATE_CURRENT]++;
	TbSensorState[cp->cp_state][ST_CP_STATE_CURRENT]--;

	if	(state == cp->cp_state)	// same state
	{	// le numero de l'erreur a l'etat error a pu changer TODO
		if	(cp->cp_state == ST_CP_ERROR)
		{
		}
		return;
	}

	cp->cp_stateAt	= rtl_timemono(NULL);
	cp->cp_stateP	= cp->cp_state;
	cp->cp_state	= state;

RTL_TRDBG(1,"cp=%p '%s' from '%s' to '%s'\n",cp,cp->cp_addr,
	SensorStateTxt(cp->cp_stateP),SensorStateTxt(cp->cp_state));

	TbSensorState[cp->cp_state][ST_CP_STATE_ENTER]++;

	// leave state
	switch	(cp->cp_stateP)
	{
	default:
	break;
	}

	// enter state
	switch	(cp->cp_state)
	{
	case	ST_CP_ALLOCATED :
		iSensorClearInfos(cp);
		if	(cp->cp_stateP != ST_CP_DISCONNECTED)
		{
			iSensorClearZclCounters(cp);
			iSensorClearDiaCounters(cp);
		}
		iSensorStart(cp);
		cp->cp_startNoDelay	= 0;

	break;
	case	ST_CP_CONFIGURING :
		if	(WithCovConfig)
		{
			// in/out/server clusters to configured with cov
			cp->cp_nbCov	= CovToZclIndexWithDesc(&cp->cp_desc,
				cp->cp_tbCov,cp->cp_epCov,CZ_MAX_PER_SS);
			cp->cp_minMaxCov= CovToZclMaxReportTime(cp->cp_tbCov,
					cp->cp_nbCov);
			cp->cp_minMinCov= CovToZclMinReportTime(cp->cp_tbCov,
					cp->cp_nbCov);
			cp->cp_nbCfg	= cp->cp_nbCov;
		}
		else
		{
#if	0
			// out/server clusters to configured with sensorconfig files
			cp->cp_nbCfg	= cp->cp_desc.m_nbclout;
#endif
			cp->cp_nbCfg	= 0;
		}

		cp->cp_toCfg	= 0;
		cp->cp_configured = 0;
RTL_TRDBG(1,"cp=%p '%s' covconfig found %d\n",
				cp,cp->cp_addr,cp->cp_nbCfg);
	break;
	case	ST_CP_RUNNING :
		if (OkClusterOut(W_CLU_ONOFF,&cp->cp_desc) ||
			OkClusterIn(W_CLU_ONOFF,&cp->cp_desc))
		{
			iSensorZclRqtStateOnOff(cp);
		}
		if (OkClusterOut(W_CLU_TICICE,&cp->cp_desc) ||
			OkClusterIn(W_CLU_TICICE,&cp->cp_desc))
		{
			iSensorZclRqtTicType(cp,W_CLU_TICICE,W_ATT_TICICE_TYPE);
		}
		if (OkClusterOut(W_CLU_TICCBE,&cp->cp_desc) ||
			OkClusterIn(W_CLU_TICCBE,&cp->cp_desc))
		{
			iSensorZclRqtTicType(cp,W_CLU_TICCBE,W_ATT_TICCBE_TYPE);
		}
		if (OkClusterOut(W_CLU_TICCJE,&cp->cp_desc) ||
			OkClusterIn(W_CLU_TICCJE,&cp->cp_desc))
		{
			iSensorZclRqtTicType(cp,W_CLU_TICCJE,W_ATT_TICCJE_TYPE);
		}
	break;
	case	ST_CP_DISCONNECTED :
		cp->cp_zcl_waitResp	= 0;
		cp->cp_zcl_waitTime	= 0;
		cp->cp_zcl_waitClat	= 0;
	break;
	default:
	break;
	}
}

static	int	iSensorDiaStartDataSrvClt(t_sensor *cp,int srv,int app,
				int nb,unsigned short tb[],unsigned char ep[])
{
	int		n;
	int		nbd	= 0;
	char		tmp[256];
	char		map[256];
	char		*m2mid;

	// data containers declarations per cluster/attribut/member
	for	(n = 0 ; n < nb ; n++)
	{
		int	a;	// attribut number
		int	m;	// member number
		int	nba;	// number of attributs
		t_zcl_cluster	*cl;

		if	(app != ep[n] + 1)	// TODO verify
		{
			continue;
		}

		cl	= AwZclCluster(tb[n]);
		if	(!cl)
		{
			continue;
		}
		for	(nba = 0 ; nba < cl->cl_nb_attr ; nba++)
		{
		a	= cl->cl_tb_attr[nba]->at_id;
		for	(m = 0 ; m < MAX_MBR_PER_ATT ; m++)
		{
			if	(MdlCfg)
			{
				sprintf	(map,"0x%04x.0x%04x.%d.m2m",tb[n],a,m);
				m2mid	= GetVarNoWarn(map);
				if	(!m2mid || !*m2mid)	
				{
					continue;
				}
RTL_TRDBG(1,"cp=%p '%s' %0x04x %0x04x %d m2m mapping found '%s'\n",
				cp,cp->cp_addr,tb[n],a,m,m2mid);
			}

			sprintf	(tmp,"dat_inst_%04x_%04x_%d.xml",tb[n],a,m);
			if	(WXoIsTemplate(tmp) > 0)
			{
				DiaDatStart(cp->cp_num,cp->cp_serial,app,
								tb[n],a,m);
				nbd++;
			}
		}
		}
	}

	return	nbd;
}

static	int	iCmnSensorDiaStartDataMethod(t_cmn_sensor *cmn,int app)
{
	t_zcl_simple_desc	*desc;
	t_sensor	*cp = cmn->cmn_self;
	int		srv;

	desc	= &(cp->cp_desc);

	// data containers declarations per cluster/attribut/member
	cp->cp_nbDat	+= iSensorDiaStartDataSrvClt(cp,srv=1,app,
				desc->m_nbclin,desc->m_clin,desc->m_epin);
	cp->cp_nbDat	+= iSensorDiaStartDataSrvClt(cp,srv=0,app,
				desc->m_nbclout,desc->m_clout,desc->m_epout);

#if	0	// TODO 
	// methods declarations per cluster
	// for watteco methods/commands declarations, those in files 
	// itf_inst_xxxx_server.xml are suffisant (no input/output parameters)
	for	(n = 0 ; n < desc->m_nbclout ; n++)
	{
		sprintf	(tmp,"met_inst_%04x.xml",desc->m_clout[n]);
		if	(WXoIsTemplate(tmp) > 0)
		{
//			DiaMetStart(cp->cp_num,cp->cp_serial,app,
//							desc->m_clout[n]);
			nb++;
			nbm++;
		}
	}
	cp->cp_nbMet	= nbm;
#endif

RTL_TRDBG(1,"cp=%p '%s' interf/data=%d interf/method=%d found in templates\n",
		cp,cp->cp_addr,cp->cp_nbDat,cp->cp_nbMet);
	return	cp->cp_nbDat + cp->cp_nbMet;
}

static	void	iSenSorDiaDeclaration(t_sensor *cp)
{
	t_zcl_simple_desc	*desc;
	int	a;
	int	rig;
	int	nb	= 0;
	char	tmp[32];

	if	(cp->cp_dia_deleting)
		return;

	if	(DiaDisconnected())
		return;

	if	(DiaFull75())
	{
RTL_TRDBG(3,"cp=%p '%s' dia full => delayed\n",cp,cp->cp_addr);
		return;
	}

	desc	= &(cp->cp_desc);
	if	(desc->m_nbclin + desc->m_nbclout - cp->cp_implictCluster <= 0)
	{
		return;
	}

	if	(DIA_KO())
		return;

	if	(	   cp->cp_dia_accok != 0 
			&& cp->cp_dia_datok != 0 
		/*	&& cp->cp_dia_metok != 0 */
			&& cp->cp_dia_appok != 0
			&& cp->cp_dia_devok != 0
		)
	{	// to avoid DiaFindRequestByDevSerial
RTL_TRDBG(5,"cp=%p '%s' dia all not null => nothing\n",cp,cp->cp_addr);
		return;
	}

	if	(DiaFindRequestByDevSerial(cp->cp_num,cp->cp_serial))
	{
RTL_TRDBG(3,"cp=%p '%s' dia waiting => delayed\n",cp,cp->cp_addr);
		return;
	}

	if	(cp->cp_dia_accok == 0)
	{
		for	(a = 0 ; a < desc->m_nbep ; a++)
		{
			sprintf	(tmp,"%d",a+1);
			SetVar	("a_num",tmp);
			for	(rig = 0 ; rig < 3 ; rig++)
			{
				DiaAccStart(cp->cp_num,cp->cp_serial,a+1,rig);
			}
		}
		return;
	}

	if	(cp->cp_dia_datok == 0 && cp->cp_dia_metok == 0)
	{
		cp->cp_nbDat	= 0;
		cp->cp_nbMet	= 0;
		for	(a = 0 ; a < desc->m_nbep ; a++)
		{
			sprintf	(tmp,"%d",a+1);
			SetVar	("a_num",tmp);
			nb += iCmnSensorDiaStartDataMethod(&cp->cp_cmn,a+1);
		}
		if	(nb)
			return;
	}

	if	(cp->cp_dia_appok == 0)
	{
		for	(a = 0 ; a < desc->m_nbep ; a++)
		{
			sprintf	(tmp,"%d",a+1);
			SetVar	("a_num",tmp);
			DiaAppStart(cp->cp_num,cp->cp_serial,a+1);
		}
		return;
	}

	if	(cp->cp_dia_devok == 0)
	{
		DiaDevStart(cp->cp_num,cp->cp_serial);
		DiaNetStart();
	}

		
}


static	t_sensor	*SensorFindByAddr(char *addr)
{
	int	i;

	for	(i = 0 ; i < MAX_SSR_PER_RTR ; i++)
	{
		if	(TbSensor[i] && strcmp(TbSensor[i]->cp_addr,addr) == 0)
		{
			return	TbSensor[i];
		}
	}
	return	NULL;
}


static	t_sensor	*SensorFindByNiceName(char *name)
{
	int	i;

	for	(i = 0 ; i < MAX_SSR_PER_RTR ; i++)
	{
		if	(TbSensor[i] && strcmp(TbSensor[i]->cp_nicename,name) == 0)
		{
			return	TbSensor[i];
		}
	}
	return	NULL;
}

static	t_sensor	*SensorFindByNumSoftw(unsigned char numsoftw)
{
	int	i;

	for	(i = 0 ; i < MAX_SSR_PER_RTR ; i++)
	{
		if	(TbSensor[i] && TbSensor[i]->cp_softw == numsoftw)
		{
			return	TbSensor[i];
		}
	}
	return	NULL;
}


static	void	iSensorFree(t_sensor *cp)
{
	int	wmsg;

	wmsg	= rtl_imsgRemoveTo(MainIQ,(void *)cp->cp_num);
RTL_TRDBG(1,"free cp=%p '%s' wmsg=%d\n",cp,cp->cp_addr,wmsg);
	if	(cp->cp_state >= ST_CP_NULL && cp->cp_state < ST_CP_STATE_MAX)
	{
		TbSensorState[cp->cp_state][ST_CP_STATE_CURRENT]--;
	}
	if	(cp->cp_nicename)
		free	(cp->cp_nicename);
	if	(cp->cp_addr)
		free	(cp->cp_addr);

	free	(cp);
}

static	t_sensor *iSensorInit(t_sensor *cp,int num,int numsoftw,char *addr)
{
	cp->cp_self	= (void *)cp;
	cp->cp_num	= num;
	cp->cp_serial	= DoSerial();
	cp->cp_softw	= numsoftw;
	cp->cp_addr	= strdup(addr);
	cp->cp_stateOnOff	= -1;
	cp->cp_ticType		= 0xFF;

	if	(cp->cp_softw)
	{
		sprintf(cp->cp_ieee,"00000000000000%02X",cp->cp_softw);
		if	(strstr(addr,"+s"))
		{
			cp->cp_simulated	= 1;
		}
	}
	else
		AddrToIeee(cp->cp_addr,cp->cp_ieee);

	iSensorLoadContext(cp);
	return	cp;
}

static	t_sensor *SensorNew(int num,char *addr)
{
	t_sensor	*cp;
	char		*softw;
	unsigned char	numsoftw = 0;

	softw	= strchr(addr,'@');	// num/ipv6
	if	(softw)
	{
		numsoftw= atoi(addr);
		if	(!numsoftw || numsoftw == 17)
		{	// because 0x11 for proto ZCL
RTL_TRDBG(0,"softw sensor '%s' cannot be used num=%d\n",addr,numsoftw);
			return	NULL;
		}
	}

	cp	= (t_sensor *)malloc(sizeof(t_sensor));
	if	(!cp)
	{
		RTL_TRDBG(0,"malloc failure\n");
		return	NULL;
	}
	memset	(cp,0,sizeof(t_sensor));

	iSensorInit(cp,num,numsoftw,addr);

RTL_TRDBG(1,"new+init+load=%d cp=%p '%s'\n",cp->cp_ctxtLoaded,cp,cp->cp_addr);


	return	cp;
}

//
//	find a place to allocate a new sensor in the sensors table
//

static	void DoSensorCreate(char *tocreate)
{
	int		i;
	int		room = -1;
	t_sensor	*cp;

RTL_TRDBG(1,"try to create sensor '%s' nbsensors=%d\n",tocreate,NbSensor);

	// really does not exist ?
	for	(i = 0 ; i < MAX_SSR_PER_RTR ; i++)
	{
		if	(TbSensor[i] && strcmp(TbSensor[i]->cp_addr,tocreate) == 0)
		{
RTL_TRDBG(1,"sensor '%s' already exists at room=%d\n",tocreate,i);
			return;
		}
	}

	// find a room
	for	(i = 0 ; i < MAX_SSR_PER_RTR ; i++)
	{
		if	(TbSensor[i] == NULL)
		{
			room	= i;
			break;
		}
	}
	if	(room < 0)
	{
		RTL_TRDBG(0,"max sensors reached (%d) !!!\n",NbSensor);
		return;
	}

	cp	= SensorNew(room,tocreate);
	if	(cp == NULL)
		return;

	RTL_TRDBG(1,"create '%s' %x room=%d (%d)\n",tocreate,cp,room,NbSensor);

	TbSensor[room]	= cp;
	NbSensor++;

	iSensorState(cp,ST_CP_ALLOCATED);

}

//
//	find the place used in the sensors table and delete the sensor
//

static	int	DoSensorDelete(char *todel)
{
	int	i;

	for	(i = 0 ; i < MAX_SSR_PER_RTR ; i++)
	{
		if	(TbSensor[i] && strcmp(TbSensor[i]->cp_addr,todel) == 0)
		{
RTL_TRDBG(1,"delete sensor '%s' room=%d (%d)\n",todel,i,NbSensor);
			iSensorFree(TbSensor[i]);
			TbSensor[i] = NULL;
			NbSensor--;
			return	1;
		}
	}

RTL_TRDBG(1,"sensor '%s' not found\n",todel);
	return	0;
}


static	void	iSensorEventALLOCATED(t_sensor *cp,t_imsg *imsg,t_zcl_msg *zmsg)
{
	t_zcl_simple_desc	*desc;

	desc	= &(cp->cp_desc);
	switch	((unsigned int)imsg->im_type)
	{
	case	IM_SENSOR_START :
		if	(RequestPANID && cp->cp_ctxtLoaded == 0)
		{
			iSensorZclRqtPanId(cp);
			iSensorState(cp,ST_CP_WAITINGPANID);
			return;
		}

		if	(desc->m_nbclin + desc->m_nbclout - cp->cp_implictCluster <= 0)
		{
RTL_TRDBG(1,"cp=%p '%s' already known but no cluster\n",cp,cp->cp_addr);
			iSensorZclRqtDesc(cp);
			iSensorState(cp,ST_CP_WAITINGDESC);
			return;
		}


		if	(cp->cp_configured <= 0)
		{
RTL_TRDBG(1,"cp=%p '%s' already known but not configured\n",cp,cp->cp_addr);
			rtl_imsgAddDelayed(MainIQ,
			rtl_imsgAlloc(IM_SENSOR,IM_SENSOR_CONFIG_NEXT,
				(void *)cp->cp_num,cp->cp_serial),1000);
			iSensorState(cp,ST_CP_CONFIGURING);
			return;
		}
		if	(WithCovConfig)
		{
			cp->cp_nbCov	= CovToZclIndexWithDesc(&cp->cp_desc,
				cp->cp_tbCov,cp->cp_epCov,CZ_MAX_PER_SS);
			cp->cp_minMaxCov= CovToZclMaxReportTime(cp->cp_tbCov,
						cp->cp_nbCov);
			cp->cp_minMinCov= CovToZclMinReportTime(cp->cp_tbCov,
					cp->cp_nbCov);
		}
RTL_TRDBG(1,"cp=%p '%s' already known and configured\n",cp,cp->cp_addr);
		iSensorState(cp,ST_CP_RUNNING);
		return;
	break;
	}
}

static	void	iSensorEventWAITINGPANID(t_sensor *cp,t_imsg *imsg,t_zcl_msg *zmsg)
{
	switch	((unsigned int)imsg->im_type)
	{
	case	W_CLAT(W_CLU_CONFIGURATION,W_ATT_CONFIGURATION_PANID) :
	if	(zmsg->m_cmd == W_CMD_READ_ATTR_RESP && zmsg->m_status == 0)
	{
		cp->cp_panid	= zmsg->u_param.m_uint2_value.m_value;
RTL_TRDBG(1,"cp=%p '%s' panid=%0x\n",cp,cp->cp_addr,cp->cp_panid);
		iSensorZclRqtDesc(cp);
		iSensorState(cp,ST_CP_WAITINGDESC);
		return;
	}
	break;
	}
}

static	void	iSensorEventWAITINGDESC(t_sensor *cp,t_imsg *imsg,t_zcl_msg *zmsg)
{
	void	*desc;

	switch	((unsigned int)imsg->im_type)
	{
	case	W_CLAT(W_CLU_CONFIGURATION,W_ATT_CONFIGURATION_DESC) :
	if	(zmsg->m_cmd == W_CMD_READ_ATTR_RESP && zmsg->m_status == 0)
	{
RTL_TRDBG(1,"cp=%p '%s' read desc response OK\n",cp,cp->cp_addr);
		desc	= (void *)&(zmsg->u_param.m_simple_desc);
		memcpy	(&(cp->cp_desc),desc,sizeof(cp->cp_desc));

		if	(AllInputCluster)
		{
			ForceInputCluster(&(cp->cp_desc));
		}
		if	(!DuplicateCluster)
		{
			NoDuplicateCluster(&(cp->cp_desc));
		}
		cp->cp_implictCluster = AddImplicitCluster(&(cp->cp_desc));

		iSensorSaveContext(cp);

		rtl_imsgAddDelayed(MainIQ,
		rtl_imsgAlloc(IM_SENSOR,IM_SENSOR_CONFIG_NEXT,
			(void *)cp->cp_num,cp->cp_serial),1000);
		iSensorState(cp,ST_CP_CONFIGURING);
		return;
	}
	break;
	case	W_CLAT(W_CLU_CONFIGURATION,W_ATT_CONFIGURATION_DESCEPN) :
	if	(zmsg->m_cmd == W_CMD_READ_ATTR_RESP && zmsg->m_status == 0)
	{
RTL_TRDBG(1,"cp=%p '%s' read descepn response OK\n",cp,cp->cp_addr);
		desc	= (void *)&(zmsg->u_param.m_simple_desc);
		memcpy	(&(cp->cp_desc),desc,sizeof(cp->cp_desc));

		if	(AllInputCluster)
		{
			ForceInputCluster(&(cp->cp_desc));
		}
		if	(!DuplicateCluster)
		{
			NoDuplicateCluster(&(cp->cp_desc));
		}
		cp->cp_implictCluster = AddImplicitCluster(&(cp->cp_desc));

		iSensorSaveContext(cp);

		rtl_imsgAddDelayed(MainIQ,
		rtl_imsgAlloc(IM_SENSOR,IM_SENSOR_CONFIG_NEXT,
			(void *)cp->cp_num,cp->cp_serial),1000);
		iSensorState(cp,ST_CP_CONFIGURING);
		return;
	}
	break;
	}
}

static	void	iSensorEventCONFIGURING(t_sensor *cp,t_imsg *imsg,t_zcl_msg *zmsg)
{
	unsigned int	clat;


	switch	((unsigned int)imsg->im_type)
	{
	case	IM_TIMER_SENSOR :
		iSenSorDiaDeclaration(cp);
		return;
	break;
	case	IM_SENSOR_CONFIG_NEXT :
		if	(WithCovConfig)
		{
			if	(cp->cp_nbCov <= 0)
			{	// no config in covconfig file assume configured
				cp->cp_toCfg	= 0;
				cp->cp_nbCfg	= 0;
				rtl_imsgAdd(MainIQ,
				rtl_imsgAlloc(IM_SENSOR,IM_SENSOR_CONFIG_STOP,
					(void *)cp->cp_num,cp->cp_serial));
				return;
			}
RTL_TRDBG(1,"cp=%p '%s' start covconfigure cluster (%d/%d)\n",cp,cp->cp_addr,
					cp->cp_toCfg,cp->cp_nbCfg);
			iSensorZclRqtCovConfig(cp,cp->cp_toCfg);
			return;
		}
		return;
	break;
	case	IM_SENSOR_CONFIG_DONE :
RTL_TRDBG(1,"cp=%p '%s' configuration done\n",cp,cp->cp_addr);
		cp->cp_configured	= 1;
		iSensorSaveContext(cp);
		iSensorState(cp,ST_CP_RUNNING);
		return;
	break;
	case	IM_SENSOR_CONFIG_STOP :
RTL_TRDBG(1,"cp=%p '%s' configuration stop\n",cp,cp->cp_addr);
		cp->cp_configured	= 0;
		iSensorSaveContext(cp);
		cp->cp_configured	= 2;
		iSensorState(cp,ST_CP_RUNNING);
		return;
	break;
	}

	if	(!zmsg)
		return;

	clat	= W_CLAT(zmsg->m_cluster_id,zmsg->m_attr_id);
	if	(zmsg->m_cmd == W_CMD_REPO_CONF_RESP && zmsg->m_status == 0)
	{
RTL_TRDBG(1,"cp=%p '%s' configure response OK %04x (%d/%d)\n",
			cp,cp->cp_addr,clat,cp->cp_toCfg,cp->cp_nbCfg);

		if	(cp->cp_configLock)
		{
			cp->cp_configured	= 1;
			iSensorSaveContext(cp);
			return;
		}

		cp->cp_toCfg++;
		if	(cp->cp_toCfg >= cp->cp_nbCfg)
		{
			cp->cp_toCfg	= 0;
			cp->cp_nbCfg	= 0;
			rtl_imsgAdd(MainIQ,
			rtl_imsgAlloc(IM_SENSOR,IM_SENSOR_CONFIG_DONE,
				(void *)cp->cp_num,cp->cp_serial));
		}
		else
		{
			rtl_imsgAddDelayed(MainIQ,
			rtl_imsgAlloc(IM_SENSOR,IM_SENSOR_CONFIG_NEXT,
				(void *)cp->cp_num,cp->cp_serial),1000);
		}
		return;
	}
	if	(zmsg->m_cmd == W_CMD_REPO_CONF_RESP && zmsg->m_status != 0)
	{
RTL_TRDBG(1,"cp=%p '%s' configure response NOK %04x (%d/%d)\n",
			cp,cp->cp_addr,clat,cp->cp_toCfg,cp->cp_nbCfg);

		if	(cp->cp_configLock)
		{
			cp->cp_configLock	= 0;
			iSensorSaveContext(cp);
			return;
		}

		cp->cp_error	= ERR_SENSOR_REPO_CONF;
		iSensorState(cp,ST_CP_ERROR);

		return;
	}
}

static	void	iSensorEventRUNNING(t_sensor *cp,t_imsg *imsg,t_zcl_msg *zmsg)
{
	unsigned int	clat;

	switch	((unsigned int)imsg->im_type)
	{
	case	IM_TIMER_SENSOR :
		iSenSorDiaDeclaration(cp);
		return;
	break;
	}

	if	(!zmsg)
		return;

	clat	= W_CLAT(zmsg->m_cluster_id,zmsg->m_attr_id);
	if	(zmsg->m_cmd == W_CMD_REPO_ATTR_EVNT && zmsg->m_status == 0)
	{
RTL_TRDBG(1,"cp=%p '%s' reporting event %08x\n",cp,cp->cp_addr,clat);

	}
}

static	void	iSensorEventDISCONNECTED(t_sensor *cp,t_imsg *imsg,t_zcl_msg *zmsg)
{
	unsigned int	clat;
	time_t	now;
	t_zcl_simple_desc	*desc;

	desc	= &(cp->cp_desc);
	now	= rtl_timemono(NULL);
	switch	((unsigned int)imsg->im_type)
	{
	case	IM_TIMER_SENSOR :
		if	(ABS(now - cp->cp_stateAt) > SENSOR_RESET_DISC_TIMER)
		{
			iSensorReset(cp);
		}
		return;
	break;
	}

	if	(!zmsg)
		return;

	clat	= W_CLAT(zmsg->m_cluster_id,zmsg->m_attr_id);
	if	(zmsg->m_cmd == W_CMD_REPO_ATTR_EVNT && zmsg->m_status == 0)
	{	
		if	(desc->m_nbclin + desc->m_nbclout - cp->cp_implictCluster <= 0)
		{
			if	(Congestion())
				return;
RTL_TRDBG(1,"cp=%p '%s' reporting event but no cluster defined\n",cp,cp->cp_addr);
			iSensorZclRqtDesc(cp);
			iSensorState(cp,ST_CP_WAITINGDESC);
			return;
		}
		if	(cp->cp_configured <= 0)
		{
			if	(Congestion())
				return;
RTL_TRDBG(1,"cp=%p '%s' reporting event but not configured\n",cp,cp->cp_addr);
			rtl_imsgAddDelayed(MainIQ,
			rtl_imsgAlloc(IM_SENSOR,IM_SENSOR_CONFIG_NEXT,
				(void *)cp->cp_num,cp->cp_serial),1000);
			iSensorState(cp,ST_CP_CONFIGURING);
			return;
		}
		if	(!OkClusterOut(zmsg->m_cluster_id,desc) &&
			!OkClusterIn(zmsg->m_cluster_id,desc))
		{
			if	(Congestion())
				return;
RTL_TRDBG(1,"cp=%p '%s' reporting event but bad cluster\n",cp,cp->cp_addr);
			cp->cp_startNoDelay	= 1;
			iSensorReset(cp);
			return;
		}
RTL_TRDBG(1,"cp=%p '%s' reporting event and configured\n",cp,cp->cp_addr);
		iSensorState(cp,ST_CP_RUNNING);
		return;
	}
}


static	void	iSensorEvent(t_sensor *cp,t_imsg *imsg,t_zcl_msg *zmsg)
{
	time_t	now;
	int	maxwaitresp;

	if	(zmsg && cp->cp_softw == 0)
	{	// ZCL message on physical sensor
		if	(!MultiEndPoint && zmsg && zmsg->m_epn != 0)
		{	// EPN != 0 not treated
RTL_TRDBG(1,"cp=%p '%s' at '%s' EPN=%d != 0\n",cp,cp->cp_addr,
				SensorStateTxt(cp->cp_state),zmsg->m_epn);
			return;
		}
	}
//	maxwaitresp	= SENSOR_WAIT_RESP_TIMER;
	maxwaitresp	= cp->cp_zcl_waitTime;
	if	(maxwaitresp <= 0)
		maxwaitresp	= ZclSleepTime;
	if	(cp->cp_softw && cp->cp_simulated == 0)
	{ // a software sensor not simulated has a better response time
		maxwaitresp	= 30;
	}


if	(imsg->im_type != IM_TIMER_SENSOR)
RTL_TRDBG(1,"cp=%p '%s' at '%s' recv %08x (%d)\n",cp,cp->cp_addr,
	SensorStateTxt(cp->cp_state),imsg->im_type,imsg->im_type);

	// events for all states
	now	= rtl_timemono(NULL);
	switch	((unsigned int)imsg->im_type)
	{
	case	IM_TIMER_SENSOR :
		if	(cp->cp_state == ST_CP_ALLOCATED)
		{	// possible because of delayed start
			return;
		}
		// sensor does not answer to our read attribut request
		if (cp->cp_zcl_readClat && cp->cp_zcl_readAt &&
			ABS(now - cp->cp_zcl_readAt) > ZclSleepTime)
		{
			iSensorRetrieveAttrCompletion(cp,10,NULL);
			return;
		}
		// sensor does not answer to our request
		if	(cp->cp_zcl_waitResp && ABS(now - cp->cp_zcl_sendAt) > 
			maxwaitresp)
		{
			if	(Congestion())
				return;
RTL_TRDBG(1,"cp=%p '%s' does not answer to request 0x%08x wait=%d\n",
			cp,cp->cp_addr,cp->cp_zcl_waitResp,maxwaitresp);
			iSensorState(cp,ST_CP_DISCONNECTED);
			return;
		}
	break;
	case	IM_SENSOR_RESET :
		iSensorState(cp,ST_CP_ALLOCATED);
		return;
	break;
	case	IM_SENSOR_DELETE :
		DoSensorDelete(cp->cp_addr);
		DiaNetStart();
		return;
	break;
	}

	// TODO a mettre dans les bons etats
	if	(imsg->im_type == IM_TIMER_SENSOR 
				&& cp->cp_state == ST_CP_RUNNING
				&& cp->cp_configLock == 0)
	{
		// no report at all or too slow => not configured ?
		if	(cp->cp_zcl_recvAt == 0 || cp->cp_zcl_recvcount == 0)
		{
			if (cp->cp_minMaxCov && 
				ABS(now - cp->cp_startAt) > cp->cp_minMaxCov)
			{
				if	(Congestion())
					return;
RTL_TRDBG(1,"cp=%p '%s' no report => configured (%d)\n",cp,cp->cp_addr,
						cp->cp_configured);
				cp->cp_configured	= 0;
				iSensorState(cp,ST_CP_DISCONNECTED);
				return;
			}
		}
		else
		{
			if(cp->cp_minMaxCov && 
				ABS(now - cp->cp_zcl_recvAt) > cp->cp_minMaxCov)
			{
				if	(Congestion())
					return;
RTL_TRDBG(1,"cp=%p '%s' report too slow => configured (%d)\n",cp,cp->cp_addr,
						cp->cp_configured);
				cp->cp_configured	= 0;
				iSensorState(cp,ST_CP_DISCONNECTED);
				return;
			}
		}
	}


	// events depending on state
	switch(cp->cp_state)
	{
	case	ST_CP_ALLOCATED :
		iSensorEventALLOCATED(cp,imsg,zmsg);
	break;
	case	ST_CP_WAITINGPANID :
		iSensorEventWAITINGPANID(cp,imsg,zmsg);
	break;
	case	ST_CP_WAITINGDESC :
		iSensorEventWAITINGDESC(cp,imsg,zmsg);
	break;
	case	ST_CP_CONFIGURING :
		iSensorEventCONFIGURING(cp,imsg,zmsg);
	break;
	case	ST_CP_RUNNING :	
		iSensorEventRUNNING(cp,imsg,zmsg);
	break;
	case	ST_CP_DISCONNECTED :	
		iSensorEventDISCONNECTED(cp,imsg,zmsg);
	break;
	case	ST_CP_ERROR :
	break;
	}
}


static	void	iSensorZclEvent(t_sensor *cp,t_zcl_msg *zmsg)
{
	t_imsg		imsg;
	unsigned int	clat;
	int		epnum;
	int		sz	= 0;
	char		*data	= NULL;
	int		status	= 0;
	int		badresp	= 0;
	int		value	= 0;
//	int		value2	= 0;
	int		value3	= 0;
//	int		value4	= 0;
	unsigned int	uvalue4	= 0;

	float		fvalue;
	time_t		prevevent;
	int		verifyreporting	= 1;

	prevevent		= cp->cp_zcl_recvAt;
	cp->cp_zcl_recvAt	= rtl_timemono(NULL);
	status			= zmsg->m_status;
	epnum			= zmsg->m_epn;
	clat			= W_CLAT(zmsg->m_cluster_id,zmsg->m_attr_id);
	imsg.im_type		= clat;

#if	0
	if(zmsg->m_cmd == W_CMD_READ_ATTR_RESP && zmsg->m_status == 0)
	{
		if(clat == W_CLAT(W_CLU_CONFIGURATION,W_ATT_CONFIGURATION_DESC))
		{
RTL_TRDBG(1,"cp=%p '%s' unsolicited desc received => reset\n",cp,cp->cp_addr);
			iSensorReset(cp);
			return;
		}
	}
#endif

	switch	(zmsg->m_cmd)	
	{
	case	W_CMD_READ_ATTR_RESP:
		if (cp->cp_zcl_readClat == clat) 
		{
RTL_TRDBG(1,"cp=%p '%s' read response (%08x) tid=%d\n",
			cp,cp->cp_addr,cp->cp_zcl_readClat,cp->cp_zcl_readTid);
			iSensorRetrieveAttrCompletion(cp,0,zmsg);
			return;
		}
	case	W_CMD_REPO_CONF_RESP:
	case	W_CMD_READ_CONF_RESP:
		// this is a response, is there a request ?
		if (cp->cp_zcl_waitResp != zmsg->m_cmd || cp->cp_zcl_waitClat != clat) 
		{
RTL_TRDBG(1,"cp=%p '%s' unsolicited response resp=(%d/%08x) wait=(%d/%08x)\n",
	cp,cp->cp_addr,zmsg->m_cmd,clat,cp->cp_zcl_waitResp,cp->cp_zcl_waitClat);
			badresp 	= 1;
			return;
		}
RTL_TRDBG(1,"cp=%p '%s' ok response resp=(%d/%08x) wait=(%d/%08x)\n",
	cp,cp->cp_addr,zmsg->m_cmd,clat,cp->cp_zcl_waitResp,cp->cp_zcl_waitClat);
		cp->cp_zcl_respcount++;
		cp->cp_zcl_waitResp = 0;
		cp->cp_zcl_waitTime = 0;
		cp->cp_zcl_waitClat = 0;
		switch	(clat)
		{
		case	W_CLAT(W_CLU_ONOFF,W_ATT_ONOFF_ONOFF) :
			value	= zmsg->u_param.m_uint2_value.m_value;
RTL_TRDBG(1,"cp=%p '%s' read att response clat=%08x onoff status=%d value=%u\n",
				cp,cp->cp_addr,clat,status,value);

			if	(status != 0)
			{
				value	= -1;
			}
			cp->cp_stateOnOff	= value;

		break;
		case	W_CLAT(W_CLU_TICICE,W_ATT_TICICE_TYPE) :
		case	W_CLAT(W_CLU_TICCBE,W_ATT_TICCBE_TYPE) :
		case	W_CLAT(W_CLU_TICCJE,W_ATT_TICCJE_TYPE) :
			value	= zmsg->u_param.m_uint1_value.m_value;
RTL_TRDBG(1,"cp=%p '%s' read att response clat=%08x tictype status=%d value=%u\n",
				cp,cp->cp_addr,clat,status,value);

			if	(status != 0)
			{
				value	= 0xFF;
			}
			cp->cp_ticType	= value;
			iSensorSaveContext(cp);
			if (!CanReport(zmsg->m_cluster_id,zmsg->m_attr_id,0))
				break;
			DiaReportValue(cp->cp_num,cp->cp_serial,epnum+1,
			zmsg->m_cluster_id,zmsg->m_attr_id,0,"%d",value);
		break;
		}
	break;
	case	W_CMD_REPO_ATTR_EVNT:	
		// this is a reporting event, see the end of switch => no return
		verifyreporting	= 1;
		cp->cp_zcl_evntcount++;
		switch	(clat)
		{
		case	W_CLAT(W_CLU_TEMPERATURE,W_ATT_TEMPERATURE_CUR_VALUE) :
			value	= (int)zmsg->u_param.m_uint2_value.m_value;
			fvalue	= (float)value;
			fvalue	= fvalue / 100.0;
RTL_TRDBG(1,"cp=%p '%s'.%d report event clat=%08x current temper value=%.2f\n",
					cp,cp->cp_addr,epnum,clat,fvalue);

			if(!CanReport(zmsg->m_cluster_id,zmsg->m_attr_id,0))
				break;
			DiaReportValue(cp->cp_num,cp->cp_serial,epnum+1,
			zmsg->m_cluster_id,zmsg->m_attr_id,0,"%.2f",fvalue);
		break;
		case	W_CLAT(W_CLU_HUMIDITY,W_ATT_HUMIDITY_CUR_VALUE) :
			value	= zmsg->u_param.m_uint2_value.m_value;
			fvalue	= (float)value;
			fvalue	= fvalue / 100.0;
RTL_TRDBG(1,"cp=%p '%s'.%d report event clat=%08x current humidity value=%.2f\n",
					cp,cp->cp_addr,epnum,clat,fvalue);

			if(!CanReport(zmsg->m_cluster_id,zmsg->m_attr_id,0))
				break;
			DiaReportValue(cp->cp_num,cp->cp_serial,epnum+1,
			zmsg->m_cluster_id,zmsg->m_attr_id,0,"%.2f",fvalue);
		break;
		case	W_CLAT(W_CLU_ILLUMINANCE,W_ATT_ILLUMINANCE_CUR_VALUE) :
			value	= zmsg->u_param.m_uint2_value.m_value;
			fvalue	= powf(10.0 , ((float)(value-1) / 10000.0));
			value	= fvalue;
RTL_TRDBG(1,"cp=%p '%s'.%d report event clat=%08x current illuminance value=%u\n",
					cp,cp->cp_addr,epnum,clat,value);

			if(!CanReport(zmsg->m_cluster_id,zmsg->m_attr_id,0))
				break;
			DiaReportValue(cp->cp_num,cp->cp_serial,epnum+1,
			zmsg->m_cluster_id,zmsg->m_attr_id,0,"%u",value);
		break;
		case	W_CLAT(W_CLU_ONOFF,W_ATT_ONOFF_ONOFF) :
			verifyreporting	= 0;
			value	= zmsg->u_param.m_uint2_value.m_value;
RTL_TRDBG(1,"cp=%p '%s'.%d report event clat=%08x current onoff value=%u\n",
					cp,cp->cp_addr,epnum,clat,value);
			cp->cp_stateOnOff	= value;

			if(!CanReport(zmsg->m_cluster_id,zmsg->m_attr_id,0))
				break;
			DiaReportValue(cp->cp_num,cp->cp_serial,epnum+1,
			zmsg->m_cluster_id,zmsg->m_attr_id,0,"%u",value);
		break;
		case	W_CLAT(W_CLU_SIMPLEMETER,W_ATT_SIMPLEMETER_CUR_VALUE) :
			verifyreporting	= 0;
			value	= zmsg->u_param.m_metering_value.m_sumWh;
			value3	= zmsg->u_param.m_metering_value.m_curWh;
RTL_TRDBG(1,"cp=%p '%s'.%d report event clat=%08x current sumWh=%d curWh=%u\n",
					cp,cp->cp_addr,epnum,clat,value,value3);

			if(!CanReport(zmsg->m_cluster_id,zmsg->m_attr_id,0))
				break;
			DiaReportValue(cp->cp_num,cp->cp_serial,epnum+1,
			zmsg->m_cluster_id,zmsg->m_attr_id,0,"%u",value);
			if(!CanReport(zmsg->m_cluster_id,zmsg->m_attr_id,3))
				break;
			DiaReportValue(cp->cp_num,cp->cp_serial,epnum+1,
			zmsg->m_cluster_id,zmsg->m_attr_id,3,"%u",value3);

			value	= zmsg->u_param.m_metering_value.m_sumVarh;
			value3	= zmsg->u_param.m_metering_value.m_curVarh;
RTL_TRDBG(1,"cp=%p '%s'.%d report event clat=%08x current sumVarh=%d curVarh=%u\n",
					cp,cp->cp_addr,epnum,clat,value,value3);

			if(!CanReport(zmsg->m_cluster_id,zmsg->m_attr_id,1))
				break;
			DiaReportValue(cp->cp_num,cp->cp_serial,epnum+1,
			zmsg->m_cluster_id,zmsg->m_attr_id,1,"%u",value);
			if(!CanReport(zmsg->m_cluster_id,zmsg->m_attr_id,4))
				break;
			DiaReportValue(cp->cp_num,cp->cp_serial,epnum+1,
			zmsg->m_cluster_id,zmsg->m_attr_id,4,"%u",value3);
		break;
		case	W_CLAT(W_CLU_ANALOGINPUT,W_ATT_ANALOGINPUT_CUR_VALUE) :
			value	= zmsg->u_param.m_uint4_value.m_value;
			fvalue	= *(float *)&value;
RTL_TRDBG(1,"cp=%p '%s'.%d report event clat=%08x current analogic xvalue=%x fvalue=%f\n",
					cp,cp->cp_addr,epnum,clat,value,fvalue);

			if(cp->cp_multiplyvalue)
				fvalue	= fvalue * cp->cp_multiplyvalue;
			fvalue	= fvalue + cp->cp_addvalue;

			if(!CanReport(zmsg->m_cluster_id,zmsg->m_attr_id,0))
				break;
			DiaReportValue(cp->cp_num,cp->cp_serial,epnum+1,
			zmsg->m_cluster_id,zmsg->m_attr_id,0,"%f",fvalue);
		break;
		case	W_CLAT(W_CLU_BINARYINPUT,W_ATT_BINARYINPUT_CUR_VALUE) :
			value	= zmsg->u_param.m_uint1_value.m_value;
RTL_TRDBG(1,"cp=%p '%s'.%d report event clat=%08x current binary value=%u\n",
					cp,cp->cp_addr,epnum,clat,value);
			cp->cp_stateOnOff	= value;

			if(!CanReport(zmsg->m_cluster_id,zmsg->m_attr_id,0))
				break;
			DiaReportValue(cp->cp_num,cp->cp_serial,epnum+1,
			zmsg->m_cluster_id,zmsg->m_attr_id,0,"%u",value);
		break;
		case	W_CLAT(W_CLU_BINARYINPUT,W_ATT_BINARYINPUT_COUNTER) :
			uvalue4	= zmsg->u_param.m_uint4_value.m_value;
RTL_TRDBG(1,"cp=%p '%s'.%d report event clat=%08x counter binary value=%u\n",
					cp,cp->cp_addr,epnum,clat,uvalue4);

			if(!CanReport(zmsg->m_cluster_id,zmsg->m_attr_id,0))
				break;
			DiaReportValue(cp->cp_num,cp->cp_serial,epnum+1,
			zmsg->m_cluster_id,zmsg->m_attr_id,0,"%u",uvalue4);
		break;
		case	W_CLAT(W_CLU_OCCUPANCY,W_ATT_OCCUPANCY_OCCUPANCY) :
			value	= zmsg->u_param.m_uint1_value.m_value;
RTL_TRDBG(1,"cp=%p '%s'.%d report event clat=%08x current occupancy value=%u\n",
					cp,cp->cp_addr,epnum,clat,value);
			cp->cp_stateOnOff	= value;

			if(!CanReport(zmsg->m_cluster_id,zmsg->m_attr_id,0))
				break;
			DiaReportValue(cp->cp_num,cp->cp_serial,epnum+1,
			zmsg->m_cluster_id,zmsg->m_attr_id,0,"%u",value);
		break;
		case	W_CLAT(W_CLU_TICICE,W_ATT_TICICE_DATA) :
		case	W_CLAT(W_CLU_TICICE,W_ATT_TICICE_PER0) :
		case	W_CLAT(W_CLU_TICICE,W_ATT_TICICE_PER1) :
		case	W_CLAT(W_CLU_TICCBE,W_ATT_TICCBE_DATA) :
		case	W_CLAT(W_CLU_TICCJE,W_ATT_TICCJE_DATA) :
			verifyreporting	= 0;
			sz	= zmsg->u_param.m_tic_data.m_sz;
			data	= (char *)zmsg->u_param.m_tic_data.m_data;
RTL_TRDBG(1,"cp=%p '%s'.%d report event clat=%08x current ticvalue=... sz=%d\n",
					cp,cp->cp_addr,epnum,clat,sz);
			if	(sz <= 0)
				break;
#if	0	// history is now saved in binary
			abuf[0]	= '\0';
			AwBinToStr((unsigned char *)data,sz,abuf,sizeof(abuf));
#endif
			AddTicHistory(cp->cp_addr,cp->cp_ticType,data,sz,clat);


			if(!CanReport(zmsg->m_cluster_id,zmsg->m_attr_id,0))
				break;
			data	= TicBinToStr(cp->cp_ticType,clat,
						(unsigned char *)data,sz);
			if	(!data)
				break;
RTL_TRDBG(5,"cp=%p '%s'.%d report event clat=%08x current ticvalue='%s'\n",
				cp,cp->cp_addr,epnum,clat,data);
			DiaReportValue(cp->cp_num,cp->cp_serial,epnum+1,
			zmsg->m_cluster_id,zmsg->m_attr_id,0,"%s",data);
		break;
		case	W_CLAT(W_CLU_TICICE,W_ATT_TICICE_TYPE) :
		case	W_CLAT(W_CLU_TICCBE,W_ATT_TICCBE_TYPE) :
		case	W_CLAT(W_CLU_TICCJE,W_ATT_TICCJE_TYPE) :
			verifyreporting	= 0;
			value	= zmsg->u_param.m_uint1_value.m_value;
RTL_TRDBG(1,"cp=%p '%s'.%d report event clat=%08x current tictype value=%u\n",
					cp,cp->cp_addr,epnum,clat,value);
			cp->cp_ticType	= value;
			iSensorSaveContext(cp);
			if (!CanReport(zmsg->m_cluster_id,zmsg->m_attr_id,0))
				break;
			DiaReportValue(cp->cp_num,cp->cp_serial,epnum+1,
			zmsg->m_cluster_id,zmsg->m_attr_id,0,"%d",value);
		break;
		case	W_CLAT(W_CLU_CONFIGURATION,W_ATT_CONFIGURATION_POWER) :
			verifyreporting	= 0;
RTL_TRDBG(1,"cp=%p '%s'.%d report event clat=%08x power m=%x s=%x l=%d \n",
					cp,cp->cp_addr,epnum,clat,
					zmsg->u_param.m_power_value.m_mode,
					zmsg->u_param.m_power_value.m_source,
					zmsg->u_param.m_power_value.m_level[0]);
			memcpy	(&cp->cp_power,&zmsg->u_param.m_power_value,
				sizeof(t_zcl_power_value));

			// device level
			if	(cp->cp_dia_devok)
				DiaDevStart(cp->cp_num,cp->cp_serial);

			// 
			data	= PowerToRawString(&cp->cp_power);
			if	(!data)
				break;
			DiaReportValue(cp->cp_num,cp->cp_serial,epnum+1,
			zmsg->m_cluster_id,zmsg->m_attr_id,0,"%s",data);
		break;
		}	// switch (clat)
		
		// voir ce qu'on peut faire si non configure
		// ou waitingdesc
		//
#if	0
		// il faudrait la date du dernier event pour chaque point ...
		if(cp->cp_state == ST_CP_RUNNING && cp->cp_configLock == 0 &&
			verifyreporting && cp->cp_minMinCov && prevevent &&
			ABS(prevevent - cp->cp_zcl_recvAt) < cp->cp_minMinCov)
		{
RTL_TRDBG(1,"cp=%p '%s' report too speed => configured (%d)\n",cp,cp->cp_addr,
					cp->cp_configured);
			cp->cp_configured	= 0;
			iSensorState(cp,ST_CP_DISCONNECTED);
			return;
		}
#endif

	break;		// case W_CMD_REPO_ATTR_EVNT:
	default :
RTL_TRDBG(1,"cp=%p '%s' unsupported command cmd=(%d/%08x) wait=(%d/%08x)\n",
	cp,cp->cp_addr,zmsg->m_cmd,clat,cp->cp_zcl_waitResp,cp->cp_zcl_waitClat);
		badresp	= 1;
	break;
	}	// switch  (zmsg->m_cmd)

	if	(badresp)
	{
		return;
	}

	iSensorEvent(cp,&imsg,zmsg);
}




//
//	search for old / new sensors to delete / create
//	same functions for physical / software sensors
//	TODO physical are actualy never deleted because the router does not
//	keep a list of sensors
//
static	void	DoSensorDetectionFor(int softw,time_t *last)
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
		if	(!TbSensor[i])	continue;

		if	(!softw && TbSensor[i]->cp_softw == 0)
		{
			tbcur[nb++]	= TbSensor[i]->cp_addr;
		}
		if	(softw && TbSensor[i]->cp_softw != 0)
		{
			tbcur[nb++]	= TbSensor[i]->cp_addr;
		}
	}

	if	(!softw)
		ret	= AwSensorDiscover(last,tbcur,tbdel,tbnew);
	else
		ret	= AwSensorSoftware(last,tbcur,tbdel,tbnew);
	if	(ret < 0)
	{
		RTL_TRDBG(0,"error in AwSensorDetectionFor(%d) err=%d\n",
							softw,ret);
		return;
	}
	if	(ret == 0)
	{
		RTL_TRDBG(6,"no change in AwSensorDetectionFor(%d)\n",softw);
		return;
	}

	// delete old sensors
	for	(i = 0 ; tbdel[i] && i < MAX_SSR_PER_RTR ; i++)
	{
		if	(softw)
		{	// only for software sensors TODO
			DoSensorDelete(tbdel[i]);
		}
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

/*!
 *
 * @brief called by main loop to create all known sensors
 * @return void
 * 
 */
void SensorCreateKnown()
{
	char	path[1024];
	void	*dir;
	char	*name;
	char	*point;

	sprintf	(path,"%s/knownsensors",getenv(GetEnvVarName()));
	dir	= rtl_openDir(path);
	if	(!dir)
		return;

	RTL_TRDBG(2,"Search knownsensors in '%s'\n",path);

	while	( (name = rtl_readDir(dir)) && *name )
	{
		if	(*name == '.' && *(name+1) == '\0')	continue;
		if	(*name == '.' && *(name+1) == '.')	continue;
		point	= rtl_suffixname(name);
		if	( point && *point && strcmp(point+1,"xml") == 0 )
		{
			point	= strchr(name,'.');
			if	(point)	*point	= '\0';
			if	(!SoftwareSensor && strchr(name,'-'))
			{
			RTL_TRDBG(2,"In knownsensors found software sensor '%s' => dropped\n",name);
				continue;
			}
			RTL_TRDBG(2,"In knownsensors found '%s'\n",name);
			DoSensorCreate(name);
		}
	}

	rtl_closeDir(dir);

}

/*!
 *
 * @brief called by main loop if a change is detected by http request. Also
 * called periodicaly by SensorClockSc().
 * @return void
 * 
 */
void	DoSensorDetection(int force)
{
	static	time_t	lastphysical;
	static	time_t	lastsoftware;

	if	(force)
	{
		lastphysical	= 0;
		lastsoftware	= 0;
	}

	DoSensorDetectionFor(0,&lastphysical);
	DoSensorDetectionFor(1,&lastsoftware);
}

/*!
 *
 * @brief clock for sensors, must be called by main loop each second
 * 	- send a TIMER_SENSOR to each sensor every 10 seconds
 * @param now current time in second (rtl_timemono(2))
 * @return void
 * 
 */
void	SensorClockSc(time_t now)
{
	static	unsigned	int	nbclock	= 0;

	int		i;
	t_imsg		msg;
	t_sensor	*cp;

	int		xsec;
	int		clk30;	

	SensorSetNetId();

	xsec	= IM_TIMER_GEN_V / 1000;
	if	(xsec <= 0)
		xsec	= 10;

	++nbclock;
	if	(nbclock && (nbclock % xsec) != 0)
	{
//		RTL_TRDBG(0,"sensor clock %d\n",nbclock);
		return;
	}

	// each 30 second sensor detection does not depend on last modif time
	clk30	= (nbclock%30?0:1);
	DoSensorDetection(clk30);

	msg.im_class	= IM_SENSOR;
	msg.im_type	= IM_TIMER_SENSOR;
	for	(i = 0 ; i < MAX_SSR_PER_RTR ; i++)
	{
		cp	= TbSensor[i];
		if	(!cp)	continue;
		iSensorEvent(cp,&msg,NULL);
	}
}

int	SensorCmdSend(int target,char *buf)
{
	int		nb = 0;
	int		i;
	t_sensor	*cp;
	int		szbin;
	char		binbuf[1024];
	char		*in	= buf;
	char		*out	= &binbuf[0];
	int		cov	= 0;
	int		param;
	char		tmp[256];
	int		cluster;
	int		attribut;
	t_cov_to_zcl	cz;

	if	(!buf || !*buf)
		return	-1;

	if	(strncmp(buf,"+cov",4) == 0)//  +cov cluster attr min max mincov
	{
		buf	= buf+4;
		cov	= 1;

		memset	(&cz,0,sizeof(cz));

		param	= sscanf(buf,"%x %x %d %d %s",&cz.cz_cluster,
			&cz.cz_attribut,&cz.cz_mint,&cz.cz_maxt,binbuf);
		if	(param != 5)
		{
			return	-1;
		}
		cz.cz_mincov	= binbuf;
		DoCovDataTranslation(&cz);
		if	(cz.cz_error)
		{
			return	-11;
		}
		if	(cz.cz_sz <= 0 || !cz.cz_data)
		{
			return	-12;
		}
		out	= cz.cz_data;
		szbin	= cz.cz_sz;
		goto	dosend;
	}
	if	(strncmp(buf,"-cov",4) == 0)
	{
		buf	+= 4;
		cluster	= 0;
		attribut= 0;
		param	= sscanf(buf,"%x %x",&cluster,&attribut);
		if	(param < 1)
		{
			return	-2;
		}
		sprintf	(tmp,"11 08 %04x %04x 00",cluster,attribut);
		in	= tmp;
	}
	if	(strncmp(buf,"-wip",4) == 0)
	{
		in	= "11 00 0050 0000";
	}
	if	(strncmp(buf,"-panid",6) == 0)
	{
		in	= "11 00 0050 0001";
	}
	if	(strncmp(buf,"-sdesc",6) == 0)
	{
		if	(MultiEndPoint)
			in	= "11 00 0050 0004";
		else
			in	= "11 00 0050 0002";
	}

	// full message is given
	szbin	= sizeof(binbuf);
	AwStrToBin(in,(unsigned char *)binbuf,&szbin);
	if	(szbin <= 0)
	{
		return	-9;
	}
	out	= &binbuf[0];

dosend :

	for	(i = 0 ; i < MAX_SSR_PER_RTR ; i++)
	{
		cp	= TbSensor[i];
		if	(!cp)	continue;
		if	(target == INT_MAX || i == target)
		{
			ZclUdpSendMessageNow(cp->cp_addr,out,szbin);
			nb++;
		}
	}
	if	(cov && cz.cz_data)
	{
		free(cz.cz_data);
	}
	return	nb;
}

int	SensorCmdRecv(int target,char *buf)
{
	int		nb = 0;
	int		i;
	t_sensor	*cp;
	int		szbin;
	unsigned char	binbuf[1024];

	szbin	= sizeof(binbuf);
	AwStrToBin(buf,binbuf,&szbin);
	if	(szbin <= 0)
	{
		return	-1;
	}

	for	(i = 0 ; i < MAX_SSR_PER_RTR ; i++)
	{
		cp	= TbSensor[i];
		if	(!cp)	continue;
		if	(target == INT_MAX || i == target)
		{
			SensorZclEventFor(cp->cp_addr,binbuf,szbin);
			nb++;
		}
	}
	return	nb;
}

int	SensorCmdReset(int target)
{
	int		nb = 0;
	int		i;
	t_sensor	*cp;

	for	(i = 0 ; i < MAX_SSR_PER_RTR ; i++)
	{
		cp	= TbSensor[i];
		if	(!cp)	continue;
		if	(target == INT_MAX || i == target)
		{
			iSensorReset(cp);
			nb++;
		}
	}
	return	nb;
}

int	SensorCmdDelete(int target)
{
	int		nb = 0;
	int		i;
	t_sensor	*cp;

	for	(i = 0 ; i < MAX_SSR_PER_RTR ; i++)
	{
		cp	= TbSensor[i];
		if	(!cp)	continue;
		if	(target == INT_MAX || i == target)
		{
			rtl_imsgAdd(MainIQ,
			rtl_imsgAlloc(IM_SENSOR,IM_SENSOR_DELETE,
				(void *)cp->cp_num,cp->cp_serial));
			nb++;
		}
	}
	return	nb;
}

int	SensorCmdConfig(int target)
{
	int		nb = 0;
	int		i;
	t_sensor	*cp;

	for	(i = 0 ; i < MAX_SSR_PER_RTR ; i++)
	{
		cp	= TbSensor[i];
		if	(!cp)	continue;
		if	(target == INT_MAX || i == target)
		{
			if	(cp->cp_configLock)	continue;
			rtl_imsgAddDelayed(MainIQ,
			rtl_imsgAlloc(IM_SENSOR,IM_SENSOR_CONFIG_NEXT,
			(void *)cp->cp_num,cp->cp_serial),1000*(i+1));
			cp->cp_configured = 0;
			iSensorState(cp,ST_CP_CONFIGURING);
			nb++;
		}
	}
	return	nb;
}

int	SensorCmdDiaSync(int target)
{
	int		nb = 0;
	int		i;
	t_sensor	*cp;

	for	(i = 0 ; i < MAX_SSR_PER_RTR ; i++)
	{
		cp	= TbSensor[i];
		if	(!cp)	continue;
		if	(target == INT_MAX || i == target)
		{
			iSensorClearDiaCounters(cp);
			nb++;
		}
	}
	return	nb;

}

int	iCmnDiaDelete(t_sensor *cp,int andforget)
{
	t_zcl_simple_desc	*desc;
	int	a;
	int	rig;
	int	nb	= 0;

	desc			= &(cp->cp_desc);
	cp->cp_dia_deleting	= 1;
	cp->cp_andforget	= andforget;

RTL_TRDBG(2,"cp=%p '%s' admin delete andforget=%d\n",cp,cp->cp_addr,andforget);
	for	(a = 0 ; a < desc->m_nbep ; a++)
	{
		DiaAppDelete(cp->cp_num,cp->cp_serial,a+1);
		for	(rig = 0 ; rig < 3 ; rig++)
		{
			DiaAccDelete(cp->cp_num,cp->cp_serial,a+1,rig);
			nb++;
		}
	}

	DiaDevDelete(cp->cp_num,cp->cp_serial);
	if	(andforget)
	{	// will remove knownsensor if DIA deletion OK (diaucb.c)
	}
	return	nb;
}

int	SensorCmdDiaDelete(int target,int andforget)
{
	int		nb = 0;
	int		i;
	t_sensor	*cp;

	for	(i = 0 ; i < MAX_SSR_PER_RTR ; i++)
	{
		cp	= TbSensor[i];
		if	(!cp)	continue;
		if	(target == INT_MAX || i == target)
		{
			iCmnDiaDelete(cp,andforget);
			nb++;
		}
	}
	return	nb;

}

/*!
 *
 * @brief called by DIA client module to set the devices (sensors) list for a 
 * network.
 * see dianet.c
 * @param dst destination object (xo/xml)
 * @return void
 * 
 */
void	SensorSetDeviceListInObj(void *dst)
{
	int		i;
	t_sensor	*cp;
	char		target[256];
	void		*dev;

	if	(!dst)
		return;
	if	( XoIsObix(dst) <= 0)
	{
		RTL_TRDBG(0,"ERROR not an obix object\n");
		return;
	}
	for	(i = 0 ; i < MAX_SSR_PER_RTR ; i++)
	{
		cp	= TbSensor[i];
		if	(!cp)	continue;

		dev	= XoNmNew("o:ref");
		if	(!dev)
		{
			RTL_TRDBG(0,"ERROR cannot allocate 'o:ref'\n");
			return;
		}
		XoNmSetAttr(dev,"name$","ref",0);
		sprintf(target,"%s/DEV_%s/containers/DESCRIPTOR/contentInstances/latest/content",SCL_ROOT_APP,cp->cp_ieee);
		XoNmSetAttr(dev,"href",target,0);
		XoNmAddInAttr(dst,"[name=nodes].[]",dev,0,0);
//	XoSaveAscii(dev,stdout);
	}
//	XoSaveAscii(dst,stdout);
}

/*!
 *
 * @brief called by DIA client module to set the applications list for a device.
 * see diadev.c
 * @param cmn a pointer to the common structure of the sensor
 * @param dst destination object (xo/xml)
 * @return void
 * 
 */
void	iCmnSensorSetApplicationListInObj(t_cmn_sensor *cmn,void *dst)
{
	int	a_num	= 1;
	char	target[256];
	void	*dev;
	t_sensor	*cp = cmn->cmn_self;
	t_zcl_simple_desc	*desc	= &(cp->cp_desc);

	if	( XoIsObix(dst) <= 0)
	{
		RTL_TRDBG(0,"ERROR not an obix object\n");
		return;
	}
	for	(a_num = 0 ; a_num < desc->m_nbep ; a_num++)
	{
		dev	= XoNmNew("o:ref");
		if	(!dev)
		{
			RTL_TRDBG(0,"ERROR cannot allocate 'o:ref'\n");
			return;
		}
		XoNmSetAttr(dev,"name$","ref",0);

		sprintf(target,"%s/APP_%s.%d/containers/DESCRIPTOR/contentInstances/latest/content",SCL_ROOT_APP,cp->cp_ieee,a_num+1);

		XoNmSetAttr(dev,"href",target,0);
		XoNmAddInAttr(dst,"[name=applications].[]",dev,0,0);
	}
//	XoSaveAscii(dst,stdout);
}

/*!
 *
 * @brief called by DIA client module to set the 'search strings' list for a 
 * device.
 * see diaapp.c/diadat.c
 * @param cmn a pointer to the common structure of the sensor
 * @param dst destination object (xo/xml)
 * @return void
 * 
 */
void	iCmnSensorSetSearchStringInObj(t_cmn_sensor *cmn,void *dst)
{
	int		n;
	char		tmp[256];
	char		cnum[256];
	t_zcl_simple_desc	*desc;
	t_sensor	*cp = cmn->cmn_self;


	desc	= &(cp->cp_desc);
	for	(n = 0 ; n < desc->m_nbclin ; n++)
	{
		sprintf	(cnum,"0x%04x",desc->m_clin[n]);
		if (MdlCfg && !MdlGetInterface(MdlCfg,NULL,"serveur",cnum))
			continue;
		sprintf	(tmp,"ZigBee.ClusterID/server/0x%04x",desc->m_clin[n]);
		XoNmAddInAttr(dst,"m2m:searchStrings.m2m:searchString",tmp,0);
		sprintf	(tmp,"ZigBee.InterfaceID/%s.Srv",
				SensorClusterToInterfaceName(desc->m_clin[n]));
		XoNmAddInAttr(dst,"m2m:searchStrings.m2m:searchString",tmp,0);
	}
	for	(n = 0 ; n < desc->m_nbclout ; n++)
	{
		sprintf	(cnum,"0x%04x",desc->m_clout[n]);
		if (MdlCfg && !MdlGetInterface(MdlCfg,NULL,"client",cnum))
			continue;
		sprintf	(tmp,"ZigBee.ClusterID/client/0x%04x",desc->m_clout[n]);
		XoNmAddInAttr(dst,"m2m:searchStrings.m2m:searchString",tmp,0);
		sprintf	(tmp,"ZigBee.InterfaceID/%s.Clt",
				SensorClusterToInterfaceName(desc->m_clout[n]));
		XoNmAddInAttr(dst,"m2m:searchStrings.m2m:searchString",tmp,0);
	}
}

static	void	ModelInterface(void *mdlitf,void *obj)
{
	// in obj template delete attributs/commands/points which
	// are not defined in model config
	if	(MdlCfg)
		MdlConfigureInterfaceTemplate(mdlitf,obj);

	// clear possible status="cfg-mandatory" in interface
	// event if modeling is not active

	MdlUnsetStatusInterfaceTemplate(obj);
}

static	int	iSensorSetInterfaceListInObj(t_sensor *cp,int srv,int app,
			int nb,unsigned short tb[],unsigned char ep[],void *dst)
{
	int	parseflags	= XOML_PARSE_OBIX;
	int		n;
	int		found;
	char		tmp[256];
	char		cnum[256];
	void		*obj;
	void		*mdlitf;

	char		*type;

	if	(srv)
		type	= "server";
	else
		type	= "client";

	found	= 0;
	for	(n = 0 ; n < nb ; n++)
	{
		if	(app != ep[n] + 1)	// TODO verify
		{
			continue;
		}
		mdlitf	= NULL;
		sprintf	(cnum,"0x%04x",tb[n]);
		if	(MdlCfg)
		{	// model config exists
			mdlitf	= MdlGetInterface(MdlCfg,NULL,type,cnum);
			if	(!mdlitf)
			{	// interface not defined in model => dropped
				continue;
			}
		}

		sprintf	(tmp,"itf_inst_%04x_%s.xml",tb[n],type);
		obj	= WXoNewTemplate(tmp,parseflags);
		if	(!obj)
		{
RTL_TRDBG(0,"ERROR cp=%p '%s' interface '%s' not found in templates\n",
				cp,cp->cp_addr,tmp);
			continue;
		}

		ModelInterface(mdlitf,obj);

		XoNmAddInAttr(dst,"[name=interfaces].[]",obj,0,0);
		cp->cp_tbItf[cp->cp_nbItf]	= tb[n];
		cp->cp_epItf[cp->cp_nbItf]	= app - 1;
		cp->cp_nbItf++;
		found++;
	}
	return	found;
}

/*!
 *
 * @brief called by DIA client module to set the interfaces list for a 
 * application.
 * see diaapp.c
 * @param cmn a pointer to the common structure of the sensor
 * @param app application number [1..N]
 * @param dst destination object (xo/xml)
 * @return void
 * 
 */
void	iCmnSensorSetInterfaceListInObj(t_cmn_sensor *cmn,int app,void *dst)
{
	t_zcl_simple_desc	*desc;
	t_sensor	*cp = cmn->cmn_self;
	int		srv;

	if	( XoIsObix(dst) <= 0)
	{
		RTL_TRDBG(0,"ERROR not an obix object\n");
		return;
	}
	desc	= &(cp->cp_desc);

RTL_TRDBG(1,"cp=%p '%s' start interfaces description app=%d nbitf=%d\n",
				cp,cp->cp_addr,app,cp->cp_nbItf);

	if	(app == 1)	// TODO verify
	{
		cp->cp_nbItf	= 0;
	}
	iSensorSetInterfaceListInObj(cp,srv=1,app,
			desc->m_nbclin,desc->m_clin,desc->m_epin,dst);
	iSensorSetInterfaceListInObj(cp,srv=0,app,
			desc->m_nbclout,desc->m_clout,desc->m_epout,dst);

RTL_TRDBG(1,"cp=%p '%s' %d interfaces found in templates for app=%d\n",
				cp,cp->cp_addr,cp->cp_nbItf,app);

}

/*!
 *
 * @brief called by DIA server module to execute a method on the network
 * @param cmn a pointer to the common structure of the sensor
 * @param app application number
 * @param cluster cluster number
 * @param numm method number
 * @param obix the optional xo/obix with the method
 * @param targetid the original target ID
 * @return void
 * 
 */

int	CmnNetworkExecuteMethod(char *ident,int targetlevel,void *obix,char *targetid,int tid)
{
	RTL_TRDBG(1,"Execute network method '%s' %d %x '%s' tid=%d\n",
			ident,targetlevel,obix,targetid,tid);
	return	0;
}


/*!
 *
 * @brief called by DIA server module to execute a method on a device/appli
 * @param cmn a pointer to the common structure of the sensor
 * @param app application number
 * @param cluster cluster number
 * @param numm method number
 * @param obix the optional xo/obix with the method
 * @param targetid the original target ID
 * @return errcode which precise the dIa retcode !!!
 * 
 */
int	iCmnSensorExecuteMethod(t_cmn_sensor *cmn,int app,int cluster,int numm,int targetlevel,void *obix,char *targetid)
{
	t_zcl_msg	zmsg;
	int		zlg;
	char		*zdata;
	t_sensor	*cp = cmn->cmn_self;
	int		target;
	int		andforget;
	int		ret;

	memset	(&zmsg,0,sizeof(zmsg));
	zmsg.m_cmd		= W_CMD_COMMAND_RQST;
	zmsg.m_epn		= app - 1;
	zdata			= (char *)&zmsg.u_pack.m_data;

	// cov configuration method, in fact this command number does not exist
	// in watteco protocol
	if	(numm == 0xFF)	
	{
		char	*minT;
		char	*maxT;
		char	*covval;
		char	*point;
		int	tmin;
		int	tmax;
		int	attr;
		char	buf[2048];
		unsigned	int clat;

		target	= SensorNumFindByAddr(cp->cp_addr);
		if	(target < 0)
			return	-10;

		if	(cp->cp_state != ST_CP_RUNNING)
			return	-10;

		minT	= XoNmGetAttr(obix,"[name=minInterval].val",0);

		maxT	= XoNmGetAttr(obix,"[name=maxInterval].val",0);

		covval	= XoNmGetAttr(obix,"[name=minCov].val",0);

		point	= XoNmGetAttr(obix,"[name=numPoint].val",0);

RTL_TRDBG(1,"dyn covconfig for '%s' %08x '%s' '%s' '%s' '%s'\n",
			cp->cp_addr,cluster,minT,maxT,covval,point);
		if	(!minT || !*minT || !maxT || !*maxT)
			return	-10;
		if	(!covval || !*covval || !point || !*point)
			return	-10;

		if	(strcmp(covval,"nop") == 0)
		{
			return	0;
		}
		if	(strcmp(covval,"sscfglock") == 0)
		{
			cp->cp_configLock	= 1;
			iSensorSaveContext(cp);
			return	0;
		}
		if	(strcmp(covval,"sscfgunlock") == 0)
		{
			cp->cp_configLock	= 0;
			iSensorSaveContext(cp);
			return	0;
		}
		if	(strcmp(covval,"ssreset") == 0)
		{
			iSensorReset(cp);
			return	0;
		}
		if	(strcmp(covval,"ssclearreset") == 0)
		{
			memset(&cp->cp_desc,0,sizeof(cp->cp_desc));
			cp->cp_configured	= 0;
			cp->cp_configLock	= 0;
			iSensorReset(cp);
			return	0;
		}

		sscanf	(point,"%x",&attr);
		clat	= W_CLAT(cluster,attr);
		tmin	= parseISO8601Duration(minT);
		tmax	= parseISO8601Duration(maxT);
		sprintf	(buf,"+cov %04x %04x %d %d %s",cluster,attr,tmin,tmax,
									covval);

		// TODO pas terrible un iSensorZclRqtCovConfig() dynamique mieux
		ret	= SensorCmdSend(target,buf);
		if	(ret <= 0)
		{
RTL_TRDBG(1,"dyn covconfig for '%s' SensorCmdSend() => ret=%d\n",cp->cp_addr,ret);
			return	-20;
		}
		// TODO special state ?
		DoWaitRespCommand(cp,W_CMD_REPO_CONF_RESP,clat);

		cp->cp_configLock	= 1;
		iSensorSaveContext(cp);

		return	0;
	}
	
	switch	(cluster)
	{
	default :
		return	-100;	// bad cluster
	break;
	case	W_CLU_UNUSED	:
		target	= SensorNumFindByAddr(cp->cp_addr);
		if	(target < 0)
			return	-10;
		switch	(numm)
		{
		case	0 :	// delete
			ret	= SensorCmdDiaDelete(target,(andforget=1));
			if	(ret <= 0)
				return	-404;
			return	-202;
		break;
		case	1 :	// rebuild
			ret	= SensorCmdDiaDelete(target,(andforget=0));
			if	(ret <= 0)
				return	-404;
			return	-204;
		break;
		default :
			return	-400;
		break;
		}
	break;
	case	W_CLU_ONOFF :
		if	(numm < 0 || numm > 2)
			return	-100;
		if	(obix && numm == 2)
		{
			char	*vl;

			vl	= XoNmGetAttr(obix,"val",0);
			if	(vl && *vl)
			{
RTL_TRDBG(1,"iCmnSensorExecuteMethod() with obix %s='%s'\n","val",vl);
				if	(*vl == '0' || !strcasecmp(vl,"false"))
					numm	= 0;
				if	(*vl == '1' || !strcasecmp(vl,"true"))
					numm	= 1;

			}
		}
		zmsg.m_cluster_id	= cluster;
		zmsg.m_cmd_value	= numm;
	break;
	case	W_CLU_SIMPLEMETER :
		if	(numm < 0 || numm > 1)
			return	-100;
		zmsg.m_cluster_id	= cluster;
		zmsg.m_cmd_value	= numm;
	break;
	case	W_CLU_CONFIGURATION :
		if	(numm < 0 || numm > 1)
			return	-100;
		zmsg.m_cluster_id	= cluster;
		zmsg.m_cmd_value	= numm;
	break;
	}

	zlg	= AwZclEncode(&zmsg,NULL,0);
	if	(zlg < 0 || zmsg.m_xxcode == 0)
	{
		RTL_TRDBG(0,"ERROR cannot encode zcl message\n");
		return	-100;
	}
	if	(ZclUdpSendMessageNow(cp->cp_addr,zdata,zlg) > 0)
	{
		RTL_TRDBG(1,"send <command> to '%s'\n", cp->cp_addr);
		cp->cp_zcl_sendcount++;
		cp->cp_zcl_methcount++;
	}
	return	0;
}

char *getErrorInfoResponse(char *cmdFamily, int zStatus, char **statusCode,
    int *size, char **type)
{
  char *content = NULL;
  void *xo;
  int serialtype = XO_FMT_STD_XML;
  int parseflags = 0;
  char szTemp[64];

  if (0 == zStatus)
  {
    *size = 0;
    *type = "";
    *statusCode = "STATUS_OK";
  }
  else
  {
    SetVar("w_errStatus", "STATUS_INTERNAL_SERVER_ERROR");
    snprintf(szTemp, 64, "%s: %s status 0x%.2x", GetVar("w_ipuid"), cmdFamily, zStatus);
    SetVar("w_errAddInfo", szTemp);
    xo = WXoNewTemplate("err_info.xml", parseflags);
    if (xo)
    {
      content = WXoSerializeFree(xo, serialtype, parseflags, size, type);
    }
    *statusCode = "STATUS_INTERNAL_SERVER_ERROR";
  }

  return content;
}

void	iSensorRetrieveAttrCompletion(t_sensor *cp,int err,t_zcl_msg *zmsg)
{
	int	tid		= cp->cp_zcl_readTid;
	unsigned int clat	= cp->cp_zcl_readClat;
	char	templ[128];
	char	m2mvar[128];
	char	*m2mid;
	char	*content= NULL;
	int	size	= 0;
	char	*type	= "application/xml";
	char	*status	= "STATUS_CREATED";
	char	*value	= "1234";
	char	bufvalue[128];
	void	*xo;

	cp->cp_zcl_readClat	= 0;
	cp->cp_zcl_readTid	= 0;
	cp->cp_zcl_readAt	= 0;

	if	(err || !zmsg)
	{
		content	= getErrorInfoResponse("READ_ERR",err,&status,&size,&type);
		diaIncomingRequestCompletion(tid,content,size,type,status);
		if	(content)
			free(content);
		return;
	}

	bufvalue[0]	= '\0';
	value		= bufvalue;
	switch	(clat)
	{
	case	W_CLAT(W_CLU_BINARYINPUT,W_ATT_BINARYINPUT_POLARITY) :
	case	W_CLAT(W_CLU_BINARYINPUT,W_ATT_BINARYINPUT_EDGE_SELECT) :
		sprintf	(bufvalue,"%u",zmsg->u_param.m_uint1_value.m_value);
	case	W_CLAT(W_CLU_BINARYINPUT,W_ATT_BINARYINPUT_DEBOUNCE) :
		sprintf	(bufvalue,"%u",zmsg->u_param.m_uint2_value.m_value);
	break;
	default	:
		// TODO il faut decoder les autres attributs si necessaire
		// en attendant on retourne uint4
		sprintf	(bufvalue,"%u",zmsg->u_param.m_uint4_value.m_value);
	break;
	}
	SetVar("r_value", value);
	sprintf	(m2mvar,"0x%04x.0x%04x.0.attr",
					zmsg->m_cluster_id,zmsg->m_attr_id);
	m2mid	= GetVar(m2mvar);
	if	(!m2mid)
	{
		sprintf	(m2mvar,"0x%04x.0x%04x.0.m2m",
					zmsg->m_cluster_id,zmsg->m_attr_id);
		m2mid	= GetVar(m2mvar);
		if	(!m2mid)
			m2mid	= "x-unknow";
	}

RTL_TRDBG(1,"cp=%p '%s' read completion 0x%04x/0x%04x m2mid='%s' val='%s'\n",
			cp,cp->cp_addr,zmsg->m_cluster_id,zmsg->m_attr_id,
			m2mid,value);

	sprintf	(templ,"dat_inst_%04x_%04x_0.xml",
					zmsg->m_cluster_id,zmsg->m_attr_id);
	xo	= WXoNewTemplate(templ, XOML_PARSE_OBIX);
	if	(xo)
	{
		char	*xotype;

		xotype	= XoNmType(xo);
		if	(xotype && strcasecmp(xotype,"o:bool") == 0)
		{
			if	(value && *value && atoi(value) )
				value	= "true";
			else
				value	= "false";
			XoNmSetAttr(xo,"val",value,0,0);
		}
		if	(m2mid && strlen(m2mid))
		{
			XoNmSetAttr(xo,"name",m2mid,0,0);
		}
		content = WXoSerializeFree(xo,XO_FMT_STD_XML,XOML_PARSE_OBIX,
			&size, &type);
	}
	else
	{
		status	= "STATUS_INTERNAL_SERVER_ERROR";
	}
	diaIncomingRequestCompletion(tid,content,size,type,status);
	if	(content)
		free(content);
}

int iCmnSensorRetrieveAttrValue(t_cmn_sensor *cmn,int app,int cluster,int numm,
  int targetlevel,char *targetid,int tid)
{
	unsigned int	clat;
	t_sensor	*cp = cmn->cmn_self;
	char		*basen = basename(targetid);
	char		*pt;
	t_zcl_msg	zmsg;
	int		zlg;
	char		*zdata;

	memset	(&zmsg,0,sizeof(zmsg));
	zmsg.m_cmd		= W_CMD_READ_ATTR_RQST;
	zmsg.m_epn		= app - 1;
	zmsg.m_cluster_id	= cluster;
	zmsg.m_attr_id		= numm;
	zdata			= (char *)&zmsg.u_pack.m_data;

	pt	= strrchr(basen,'.');
	if	(pt)	*pt	= '\0';

	clat	= W_CLAT(cluster,numm);

RTL_TRDBG(1,"cp=%p '%s' read request 0x%04x/0x%04x basen='%s'\n",
			cp,cp->cp_addr,cluster,numm,basen);

	if	(!AwZclCluster(cluster))
	{
		RTL_TRDBG(0,"ERROR read attribut bad cluster 0x%04x\n",
				cluster);
		return	-10;
	}
	if	(!AwZclAttribut(cluster,numm))
	{
		RTL_TRDBG(0,"ERROR read attribut bad attribut 0x%04x/0x%04x\n",
				cluster,numm);
		return	-10;
	}
	if	(cp->cp_zcl_readAt || cp->cp_zcl_readClat || cp->cp_zcl_readTid)
	{
		time_t	since	= 0;

		since	= rtl_timemono(NULL);
		if	(cp->cp_zcl_readAt)
			since	= ABS(since - cp->cp_zcl_readAt);
RTL_TRDBG(0,"ERROR read attribut with pending request dur=%d tid=%d on 0x%08x\n",
		since,cp->cp_zcl_readTid,cp->cp_zcl_readClat);
		return	-10;
	}

	zlg	= AwZclEncode(&zmsg,NULL,0);
	if	(zlg < 0 || zmsg.m_xxcode == 0)
	{
		RTL_TRDBG(0,"ERROR cannot encode zcl message\n");
		return	-100;
	}
	if	(ZclUdpSendMessageNow(cp->cp_addr,zdata,zlg) > 0)
	{
		RTL_TRDBG(1,"send <read> to '%s'\n", cp->cp_addr);
		cp->cp_zcl_sendcount++;
		cp->cp_zcl_readcount++;
		cp->cp_zcl_readAt	= rtl_timemono(NULL);
		cp->cp_zcl_readClat	= clat;
		cp->cp_zcl_readTid	= tid;
	}
	else
		return	-100;
	return	1;	// > 0 for async response
}


int iCmnSensorUpdateAttrValue(t_cmn_sensor *cmn,int app,int cluster,int numm,
  int targetlevel,void *obix,char *targetid,int tid)
{
	unsigned int	clat;
	t_sensor	*cp = cmn->cmn_self;
	char		*nameattr = NULL;
	char		*avalue = NULL;
	char		*basen = basename(targetid);
	char		*pt;
	t_zcl_msg	zmsg;
	int		zlg;
	char		*zdata;

	memset	(&zmsg,0,sizeof(zmsg));
	zmsg.m_cmd		= W_CMD_WRIT_ATTR_RQST;
	zmsg.m_epn		= app - 1;
	zmsg.m_cluster_id	= cluster;
	zmsg.m_attr_id		= numm;
	zdata			= (char *)&zmsg.u_pack.m_data;

	pt	= strrchr(basen,'.');
	if	(pt)	*pt	= '\0';

  	if	(!obix)
		return	-10;
//	WXoSaveXml(obix,stdout);
	nameattr	= XoNmGetAttr(obix,"name",0);
	avalue		= XoNmGetAttr(obix,"val",0);
	if	(!nameattr || !*nameattr || !avalue || !*avalue)
		return	-10;

	if	(strcasecmp(avalue,"true") == 0)
		avalue	= "1";
	if	(strcasecmp(avalue,"false") == 0)
		avalue	= "0";

	clat	= W_CLAT(cluster,numm);

RTL_TRDBG(1,"cp=%p '%s' update 0x%04x/0x%04x name='%s' val='%s' basen='%s'\n",
			cp,cp->cp_addr,cluster,numm,nameattr,avalue,basen);

	switch	(clat)
	{
	case	W_CLAT(W_CLU_BINARYINPUT,W_ATT_BINARYINPUT_POLARITY) :
	case	W_CLAT(W_CLU_BINARYINPUT,W_ATT_BINARYINPUT_EDGE_SELECT) :
		zmsg.u_param.m_uint1_value.m_value = (unsigned int)atoi(avalue);
	case	W_CLAT(W_CLU_BINARYINPUT,W_ATT_BINARYINPUT_DEBOUNCE) :
		zmsg.u_param.m_uint2_value.m_value = (unsigned int)atoi(avalue);
	break;
	default	:
		RTL_TRDBG(0,"ERROR update attribut not treated 0x%04x/0x%04x\n",
				cluster,numm);
		return	-10;
	break;
	}
	zlg	= AwZclEncode(&zmsg,NULL,0);
	if	(zlg < 0 || zmsg.m_xxcode == 0)
	{
		RTL_TRDBG(0,"ERROR cannot encode zcl message\n");
		return	-100;
	}
	if	(ZclUdpSendMessageNow(cp->cp_addr,zdata,zlg) > 0)
	{
		RTL_TRDBG(1,"send <write> to '%s'\n", cp->cp_addr);
		cp->cp_zcl_sendcount++;
		cp->cp_zcl_writcount++;
	}
	else
		return	-100;
	return	0;
}


int	SensorNumFindByAddr(char *addr)
{
	int	i;

	for	(i = 0 ; i < MAX_SSR_PER_RTR ; i++)
	{
		if	(TbSensor[i] && strcmp(TbSensor[i]->cp_addr,addr) == 0)
		{
			return	i;
		}
	}
	return	-1;
}


/*!
 *
 * @brief called by DIA client module to retrieve the common structure of a 
 * sensor with its number and serial.
 * @param num sensor number [0..MAX_SSR_PER_RTR]
 * @param serial sensor serial [1..INT_MAX]
 * @return :
 * 	- the address of the common structure in the sensor found
 * 	- NULL if bad number or bad serial
 * 
 */
t_cmn_sensor	*CmnSensorNumSerial(int num,int serial)
{
	t_sensor	*cp;

	cp	= SensorNumSerial(num,serial);
	if	(cp)
		return	&cp->cp_cmn;

	return	NULL;
}

/*!
 *
 * @brief called by DIA server module to retrieve the common structure of a 
 * sensor with its ieee address. Mainly used for retargeting purpose.
 * @param ieee sensor ieee address
 * @return :
 * 	- the address of the common structure in the sensor found
 * 	- NULL if bad ieee address
 * 
 */
t_cmn_sensor	*CmnSensorFindByIeee(char *ieee)
{
	t_sensor	*cp;

	cp	= SensorFindByIeee(ieee);
	if	(cp)
		return	&cp->cp_cmn;

	return	NULL;
}


//
//	zcl message received for a software sensor
//
void	SensorZclEventForNumSoftw(unsigned char num,char *addr,unsigned char *buf,int sz)
{
	t_sensor	*cp;

	cp	= SensorFindByNumSoftw(num);
	if	(!cp)
	{
		RTL_TRDBG(0,"receive message from unknown softw %d '%s'\n",
					num,addr);
		return;
	}
	SensorZclEventFor(cp->cp_addr,buf,sz);
}

//
//	zcl message received for a hardware sensor
//
void	SensorZclEventFor(char *addr,unsigned char *buf,int sz)
{
	t_sensor	*cp;
	t_zcl_msg	zcl;
	int		ret;
	char		tmp[1024];

	cp	= SensorFindByAddr(addr);
	if	(!cp)
	{
		RTL_TRDBG(0,"receive message from unknown sensor '%s'\n",addr);
		// TODO create sensor ?
		return;
	}
	RTL_TRDBG(1,"receive message from sensor '%s' sz=%d\n",addr,sz);


	if	(0)
	{
		AwBinToStr(buf,sz,tmp,sizeof(tmp));
		RTL_TRDBG(0,"zclbin='%s'\n",tmp);
	}
	ret	= AwZclDecode(NULL,&zcl,buf,sz);
	if	(ret <= 0)
	{
		AwBinToStr(buf,sz,tmp,sizeof(tmp));
		RTL_TRDBG(0,"ERROR decoding zclbin='%s'\n",tmp);
		cp->cp_zcl_rerrcount++;
		cp->cp_zcl_recvcount++;
		return;
	}

RTL_TRDBG(1,"zcldecode : ret=%08x cmd=%02x status=%x cl=%04x at=%04x\n",
		ret,zcl.m_cmd,zcl.m_status,zcl.m_cluster_id,zcl.m_attr_id);

	iSensorZclEvent(cp,&zcl);
	cp->cp_zcl_recvcount++;
}

void	SensorEvent(t_imsg *imsg,void *data)
{
	t_sensor	*cp;

RTL_TRDBG(1,"receive internal message cl=%d ty=%d to=%d sr=%d\n",
	imsg->im_class,imsg->im_type,(int)imsg->im_to,imsg->im_serialto);


	if	(imsg->im_class != IM_SENSOR)
	{
		return;
	}

	cp	= SensorNumSerial((int)imsg->im_to,imsg->im_serialto);
	if	(!cp)
		return;

	iSensorEvent(cp,imsg,NULL);
}

void	SensorDump(void *cl,int shortf)
{
	int		i;
	t_sensor	*cp;
	char		*txt;
	t_zcl_simple_desc	*desc;
	t_zcl_power_value	*power;
	char		tmp[256];

	AdmPrint(cl,"current sensors detected :\n");
	if	(shortf)
	{
	t_cmn_sensor	tot;

	memset	(&tot,0,sizeof(tot));
	for	(i = 0 ; i < MAX_SSR_PER_RTR ; i++)
	{
		cp	= TbSensor[i];
		if	(!cp)	continue;

		desc	= &(cp->cp_desc);
		power	= &(cp->cp_power);
		txt	= SensorStateTxt(cp->cp_state);

		AdmPrint(cl,"[%03d] a=[%s] s=[%s] n=[%s] mV=%d\n",
			i,cp->cp_addr,txt,cp->cp_nicename?cp->cp_nicename:"",
			power->m_level[0]);
		AdmPrint(cl,"[%03d] s=%08d r=%08d m=%08d c=%08d a=%08d v=%08d e=%05d\n",
			i,cp->cp_zcl_sendcount,cp->cp_zcl_recvcount,
			cp->cp_zcl_methcount,
			cp->cp_zcl_cmndcount,cp->cp_zcl_respcount,
			cp->cp_zcl_evntcount,cp->cp_zcl_rerrcount);

		tot.cmn_dia_devok	+= cp->cp_dia_devok;
		tot.cmn_dia_appok	+= cp->cp_dia_appok;
		tot.cmn_dia_datok	+= cp->cp_dia_datok;
		tot.cmn_dia_metok	+= cp->cp_dia_metok;
		tot.cmn_dia_accok	+= cp->cp_dia_accok;
		tot.cmn_dia_repok	+= cp->cp_dia_repok;
		tot.cmn_dia_reperr	+= cp->cp_dia_reperr;
	}
		AdmPrint(cl,"--DIA--dev=%u app=%u acc=%u dat=%u met=%u  repok=%u reperr=%u\n",
					tot.cmn_dia_devok,
					tot.cmn_dia_appok,
					tot.cmn_dia_accok,
					tot.cmn_dia_datok,
					tot.cmn_dia_metok,
					tot.cmn_dia_repok,
					tot.cmn_dia_reperr);
	return;
	}

	for	(i = 0 ; i < MAX_SSR_PER_RTR ; i++)
	{
		cp	= TbSensor[i];
		if	(!cp)	continue;

		desc	= &(cp->cp_desc);
		power	= &(cp->cp_power);

		AdmPrint(cl,"[%03d] self=%p serial=%u softw=%03d addr=[%s]\n",
		i,cp,cp->cp_serial,cp->cp_softw,cp->cp_addr);

		AdmPrint(cl,"[%03d] ieee=[%s] nicename=[%s]\n",
		i,cp->cp_ieee,cp->cp_nicename?cp->cp_nicename:"");

		txt	= SensorStateTxt(cp->cp_state);
		AdmPrint(cl,"[%03d] state=[%s] (%s) stateP=[%s]",i,txt,
			SinceHHHHMMSS(cp->cp_stateAt,tmp),
			SensorStateTxt(cp->cp_stateP));

		switch(cp->cp_state)
		{
		case	ST_CP_ERROR :
		AdmPrint(cl,"\t error=%d",cp->cp_error);
		break;
		case	ST_CP_CONFIGURING :
		AdmPrint(cl,"\t tocfg=%d nbcfg=%d",
						cp->cp_toCfg,cp->cp_nbCfg);
		break;
		case	ST_CP_DISCONNECTED :
		txt	= SensorStateTxt(cp->cp_stateP);
		AdmPrint(cl,"\t from=[%s] ",txt);
		break;
		}
		AdmPrint(cl,"\n");


		AdmPrint(cl,"[%03d] configured=%d cfglock=%d simulated=%d onoff='%s' tictype=%d\n",
			i,cp->cp_configured,cp->cp_configLock,cp->cp_simulated,
			OnOffTxt(cp->cp_stateOnOff),
			cp->cp_ticType);

		AdmPrint(cl,"[%03d] pwm=0x%x pws=0x%x mV ",i,
			power->m_mode,power->m_source);
		AdmPrintDefTableUS(cl,(int)power->m_count,power->m_level,"%04d");
		AdmPrint(cl,"\n");

		AdmPrint(cl,"[%03d] nbep=%d\n",i,desc->m_nbep);
		AdmPrint(cl,"[%03d] clin=%d ",i,desc->m_nbclin);
		AdmPrintDefTableUS(cl,(int)desc->m_nbclin,desc->m_clin,"%04x");
		AdmPrint(cl," ");
		AdmPrintDefTableUC(cl,(int)desc->m_nbclin,desc->m_epin,"%d");
		AdmPrint(cl,"\n");

		AdmPrint(cl,"[%03d] clout=%d ",i,desc->m_nbclout);
		AdmPrintDefTableUS(cl,(int)desc->m_nbclout,desc->m_clout,"%04x");
		AdmPrint(cl," ");
		AdmPrintDefTableUC(cl,(int)desc->m_nbclout,desc->m_epout,"%d");
		AdmPrint(cl,"\n");

		AdmPrint(cl,"[%03d] minMincov=%d minMaxcov=%d\n",
			i,cp->cp_minMinCov,cp->cp_minMaxCov);
		AdmPrint(cl,"[%03d] nbcov=%d ",i,cp->cp_nbCov);
		AdmPrintDefTable(cl,cp->cp_nbCov,(int *)cp->cp_tbCov,"%x");
		AdmPrint(cl,"\n");

		AdmPrint(cl,"[%03d] epcov ",i);
		AdmPrintDefTable(cl,cp->cp_nbCov,cp->cp_epCov,"%d");
		AdmPrint(cl,"\n");

		AdmPrint(cl,"[%03d] nbitf=%d ",i,cp->cp_nbItf);
		AdmPrintDefTable(cl,cp->cp_nbItf,cp->cp_tbItf,"%04x");
		AdmPrint(cl,"\n");
		AdmPrint(cl,"[%03d] epitf ",i);
		AdmPrintDefTable(cl,cp->cp_nbItf,cp->cp_epItf,"%d");
		AdmPrint(cl,"\n");
		AdmPrint(cl,"[%03d] nbdat=%d nbmet=%d\n",i,
			cp->cp_nbDat,cp->cp_nbMet);

		AdmPrint(cl,"[%03d] zclsend=%u zclrecv=%u zclmeth=%u zclcmnd=%d zclresp=%u zclevnt=%u zclrerr=%u\n",
				i,cp->cp_zcl_sendcount,cp->cp_zcl_recvcount,
				cp->cp_zcl_methcount,
				cp->cp_zcl_cmndcount,cp->cp_zcl_respcount,
				cp->cp_zcl_evntcount,cp->cp_zcl_rerrcount);
		AdmPrint(cl,"[%03d] zclwait=%d zclclat=%x (%s)\n",
				i,cp->cp_zcl_waitResp,cp->cp_zcl_waitClat,
				SinceHHHHMMSS(cp->cp_zcl_sendAt,tmp));

		AdmPrint(cl,"[%03d] diadev=%u diaapp=%u diaacc=%u diadat=%u diamet=%d  diareportok=%u diareporterr=%u\n",
			i,cp->cp_dia_devok,cp->cp_dia_appok,cp->cp_dia_accok,
			cp->cp_dia_datok,cp->cp_dia_metok,
			cp->cp_dia_repok,cp->cp_dia_reperr);

		AdmPrint(cl,"[%03d] ------------------------------------------------------------ \n",i);

	}
}

void	SensorDumpState(void *cl)
{
	int	i;
	char	*txt;

	for	(i = 1 ; i < ST_CP_STATE_MAX ; i++)
	{
		txt	= SensorStateTxt(i);
		if	(!txt)	continue;
		AdmPrint(cl,"state=%-15.15s current=%05d enter=%d\r\n",
				txt,TbSensorState[i][ST_CP_STATE_CURRENT],
				TbSensorState[i][ST_CP_STATE_ENTER]);
	}
}

void	TicDump(void *cl,char *buf)
{
	int		i;
	t_tic_histo	*h;
	char		tmp[512];
	char		abuf[1024];

	SortTicHistory();

	for	(i = 0 ; i < NbTicHistory ; i++)
	{
		h	= &TbTicHistory[i];
		if	(!h->th_when || !h->th_data || h->th_sz <= 0)
			continue;
		AdmPrint(cl,"[%03d] [%s] clat=0x%08x timeago=%s sz=%d mt=%d\n",
					i,h->th_addr,h->th_clat,
					SinceHHHHMMSS(h->th_when,tmp),
					h->th_sz,h->th_type);

		abuf[0]	= '\0';
		AwBinToStr((unsigned char *)h->th_data,h->th_sz,abuf,sizeof(abuf));
		AdmPrint(cl,"[%03d] hex='%s'\n",i,abuf);
		AdmPrint(cl,"[%03d] edf='%s'\n",i,
		TicBinToStr(h->th_type,h->th_clat,
					(unsigned char *)h->th_data,h->th_sz));
	}
}
