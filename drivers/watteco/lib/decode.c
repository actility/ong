
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
#include <stddef.h>
#include <stdarg.h>
#include <string.h>
#include <time.h>
#include <signal.h>
#include <ctype.h>
#include <sys/types.h>

#include <unistd.h>
#include <errno.h>
#include <err.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>


#include "rtlbase.h"
#include "watteco.h"

#define	AW_SPRINTF(b,...) { if (b) { AwSprintf(b,__VA_ARGS__); } }

//#define	TRACE

typedef	struct
{
	t_zcl_msg	*m;
	int		sz;
	t_zcl_cluster	*cl;
	t_zcl_attribut	*at;
	t_zcl_command	*cmd;
	unsigned char	*param;
	unsigned char	*pend;
	void		*punion;
}	t_zcl_dec_ctxt;

static	void	AwSprintf(char *buf,char *fmt,...)
{
	va_list	listArg;
	char	tmp[1024];

	if	(!buf)
		return;

	va_start(listArg,fmt);
	vsprintf(tmp,fmt,listArg);
	va_end(listArg);

	strcat	(buf,tmp);
}

static	int	AwZclDecodeSimpleDesc(char *f,int epn,unsigned char *param,
	t_zcl_simple_desc *desc,int tot,unsigned char *pend)
{
	int		nb,i,done=0;
	unsigned short	supported;
	t_zcl_cluster	*cls;
	int		szin = 0;


	nb	= *param;	// # of cluster in
	param	+= 1;
	szin++;
	if	(nb > 0)
	{
		//desc->m_nbclin	= nb;
		// OUTPUT
		AW_SPRINTF(f,"clusterinput=");
	}
#ifdef	TRACE
printf("------------------------------   #clin=%d\n",nb);
#endif
retryclusteroutput :
	for	( i = 0 ; i < nb && param < pend; i++ )
	{
		char	tmp[128];
		supported = *(param)*256 + *(param+1);
#ifdef	TRACE
printf("------------------------------   c1=%d c2=%d v=%04x\n",
				*param,*(param+1),supported);
#endif
#ifdef	TRACE
printf("------------------------------   c1=%d c2=%d v=%04x\n",
				*param,*(param+1),supported);
#endif
		param 	+= 2;
		szin	= szin + 2;
		cls	= AwZclCluster(supported);
		if (done==0)
		{
			desc->m_clin[desc->m_nbclin] = supported;
			desc->m_epin[desc->m_nbclin] = epn;
			desc->m_nbclin++;
		}
		else
		{
			desc->m_clout[desc->m_nbclout] = supported;
			desc->m_epout[desc->m_nbclout] = epn;
			desc->m_nbclout++;
		}
		// OUTPUT
		sprintf	(tmp,"unknown(%04x)",supported);
		AW_SPRINTF(f,"%s",cls?cls->cl_name:tmp);
		if (i != nb-1)
		{
			// OUTPUT
			AW_SPRINTF(f,",");
		}

	}
	if	(done || param >= pend)
	{
		return	szin;
	}
	nb	= *param;
	param	+= 1;
	szin++;
#ifdef	TRACE
printf("------------------------------   #clout=%d\n",nb);
#endif
	if	(nb > 0)
	{
		//desc->m_nbclout	= nb;
		// OUTPUT
		AW_SPRINTF(f,"&clusteroutput=");
		done	= 1;
		goto retryclusteroutput;
	}
	return	szin;
}

static	void	AwZclDecodeSimpleConfig(char *f,unsigned char *param,void *punion,int nbbytes)
{
	t_zcl_simple_config	val;

	memset	(&val,0,sizeof(val));
//	val.m_min_time = ntohs(*(ushort *)param);
	val.m_min_time = *(param)*256 + *(param+1);
	param 	+= 2;
//	val.m_max_time = ntohs(*(ushort *)param);
	val.m_max_time = *(param)*256 + *(param+1);
	param 	+= 2;
	switch	(nbbytes)
	{
	case	1 :
		val.m_min_change = *param;
		param	+= 1;
	break;
	case	4 :
//		val.m_min_change = ntohl(*(unsigned int *)param);
		val.m_min_change = (*(param) << 24) + (*(param+1) << 16)
					+ (*(param+2) << 8) + *(param+3);
		param 	+= 4;
	break;
	case	2 :
	default :
//		val.m_min_change = ntohs(*(ushort *)param);
		val.m_min_change = *(param)*256 + *(param+1);
		param 	+= 2;
	break;
	}
	memcpy	(punion,&val,sizeof(val));
	// OUTPUT
	AW_SPRINTF(f,"mint=%d&maxt=%d&minc=%u",
						val.m_min_time,
						val.m_max_time,
						(unsigned int)val.m_min_change);
}

static	void	AwZclDecodeMeteringValue(char *f,unsigned char *param,void *punion)
{
	t_zcl_metering_value	val;

	memset	(&val,0,sizeof(val));

	val.m_metersz	= *param++;

	val.m_sumWh	= 
		(*param << 16) | (*(param+1) << 8) | (*(param+2));
	param	+= 3;

	val.m_sumVarh	= 
		(*param << 16) | (*(param+1) << 8) | (*(param+2));
	param	+= 3;

	val.m_count	= 
		(*(param) << 8) | (*(param+1));
	param	+= 2;

	val.m_curWh	= 
		(*(param) << 8) | (*(param+1));
	param	+= 2;

	val.m_curVarh	= 
		(*(param) << 8) | (*(param+1));
	param	+= 2;

	memcpy	(punion,&val,sizeof(val));
	if (f)
	{
		AW_SPRINTF(f,"size=%d&xsumwh=%06x&xsumvarh=%06x",
			val.m_metersz,val.m_sumWh,val.m_sumVarh);
		AW_SPRINTF(f,"&count=%d&xcurwh=%04x&xcurvarh=%04x",
			val.m_count,val.m_curWh,val.m_curVarh);
	}
}

static	void	AwZclDecodeMeteringConfig(char *f,unsigned char *param,void *punion)
{
	t_zcl_metering_config	val;

	memset	(&val,0,sizeof(val));
//	val.m_min_time = ntohs(*(ushort *)param);
	val.m_min_time = *(param)*256 + *(param+1);
	param 	+= 2;
//	val.m_max_time = ntohs(*(ushort *)param);
	val.m_max_time = *(param)*256 + *(param+1);
	param 	+= 2;

	val.m_metersz	= *param++;

	val.m_sumWh	= 
		(*param << 16) | (*(param+1) << 8) | (*(param+2));
	param	+= 3;

	val.m_sumVarh	= 
		(*param << 16) | (*(param+1) << 8) | (*(param+2));
	param	+= 3;

	val.m_count	= 
		(*(param) << 8) | (*(param+1));
	param	+= 2;

	val.m_curWh	= 
		(*(param) << 8) | (*(param+1));
	param	+= 2;

	val.m_curVarh	= 
		(*(param) << 8) | (*(param+1));
	param	+= 2;

	memcpy	(punion,&val,sizeof(val));

	// OUTPUT
	if (f)
	{
		AW_SPRINTF(f,"mint=%d&maxt=%d",
			val.m_min_time,
			val.m_max_time);
		AW_SPRINTF(f,"&size=%d&xsumwh=%06x&xsumvarh=%06x",
			val.m_metersz,val.m_sumWh,val.m_sumVarh);
		AW_SPRINTF(f,"&count=%d&xcurwh=%04x&xcurvarh=%04x",
			val.m_count,val.m_curWh,val.m_curVarh);
	}
}

static	int	AwZclDecodeRepoConfRqst(char *f,t_zcl_dec_ctxt *ctxt)
{
/*
	W_CMD_REPO_CONF_RQST
*/
	unsigned int	clat;	// cluster + attribut
	unsigned char	*param = ctxt->param;
	void		*punion = ctxt->punion;

	clat	= W_CLAT(ctxt->cl->cl_id,ctxt->at->at_id);

	// OUTPUT
	AW_SPRINTF(f,"clat=%08x&",clat);

	switch	(clat)
	{
/*2o*/	case	W_CLAT(W_CLU_TEMPERATURE,	W_ATT_TEMPERATURE_CUR_VALUE) :
	case	W_CLAT(W_CLU_HUMIDITY,		W_ATT_HUMIDITY_CUR_VALUE) :
	case	W_CLAT(W_CLU_ILLUMINANCE,	W_ATT_ILLUMINANCE_CUR_VALUE) :
	case	W_CLAT(W_CLU_BINARYINPUT,	W_ATT_BINARYINPUT_DEBOUNCE) :
		AwZclDecodeSimpleConfig(f,param,punion,2);
	break;
/*1o*/	case	W_CLAT(W_CLU_ONOFF,		W_ATT_ONOFF_ONOFF) :
	case	W_CLAT(W_CLU_OCCUPANCY,		W_ATT_OCCUPANCY_OCCUPANCY) :
	case	W_CLAT(W_CLU_BINARYINPUT,	W_ATT_BINARYINPUT_CUR_VALUE) :
	case	W_CLAT(W_CLU_BINARYINPUT,	W_ATT_BINARYINPUT_POLARITY) :
	case	W_CLAT(W_CLU_BINARYINPUT,	W_ATT_BINARYINPUT_EDGE_SELECT) :
		AwZclDecodeSimpleConfig(f,param,punion,1);
	break;
/*4o*/	case	W_CLAT(W_CLU_ANALOGINPUT,	W_ATT_ANALOGINPUT_CUR_VALUE) :
	case	W_CLAT(W_CLU_BINARYINPUT,	W_ATT_BINARYINPUT_COUNTER) :
		AwZclDecodeSimpleConfig(f,param,punion,4);
	break;
/*?o*/	case	W_CLAT(W_CLU_SIMPLEMETER,	W_ATT_SIMPLEMETER_CUR_VALUE) :
		AwZclDecodeMeteringConfig(f,param,punion);
	break;

/****/	default :
		rtl_trace(0,"AwZclDecodeReadConfRqst() clat=%08x not treated\n",
					clat);
	break;
	}

	return	clat;
}

static	int	AwZclDecodeReadConfResp(char *f,t_zcl_dec_ctxt *ctxt)
{
/*
	W_CMD_READ_CONF_RESP
*/
	unsigned int	clat;	// cluster + attribut
	clat	=	AwZclDecodeRepoConfRqst(f,ctxt);
	if	(clat <= 0)
		rtl_trace(0,"AwZclDecodeReadConfResp() clat=%08x not treated\n",
					clat);

	return	clat;
}

static	int	AwZclDecodeReadAttrResp(char *f,t_zcl_dec_ctxt *ctxt)
{
/*
	W_CMD_READ_ATTR_RESP :	// status + attribut
	W_CMD_REPO_ATTR_EVNT :	// idem with no status
*/
	unsigned int	clat;	// cluster + attribut
	unsigned char	*param = ctxt->param;
	unsigned char	*pend = ctxt->pend;
	void		*punion = ctxt->punion;
	t_zcl_attribut	*at;

	at	= AwZclAttribut(ctxt->cl->cl_id,ctxt->at->at_id);
	if	(!at)
		return	-1;
	clat	= W_CLAT(ctxt->cl->cl_id,ctxt->at->at_id);

	// OUTPUT
	AW_SPRINTF(f,"clat=%08x&",clat);

#if	0
	switch	(clat)
	{
/****/	case	W_CLAT(W_CLU_ANALOGINPUT,W_ATT_ANALOGINPUT_APP_TYPE) :
/****/	case	W_CLAT(W_CLU_BINARYINPUT,W_ATT_BINARYINPUT_APP_TYPE) :
	{
	}
	break;
	}
#endif

	switch	(at->at_ssval)
	{
	case	1:
	{
		t_zcl_uint1_value val;

		
		memset	(&val,0,sizeof(val));
		memcpy	(&val,param,sizeof(val));
// ...		val.m_value	= val.m_value;
		memcpy	(punion,&val,sizeof(val));
		// OUTPUT
		AW_SPRINTF(f,"value=%u&xvalue=%02x",val.m_value,val.m_value);
		return	clat;
	}
	break;
	case	2:
	{
		t_zcl_uint2_value val;

		
		memset	(&val,0,sizeof(val));
		memcpy	(&val,param,sizeof(val));
		val.m_value	= ntohs(val.m_value);
		memcpy	(punion,&val,sizeof(val));
		// OUTPUT
		AW_SPRINTF(f,"value=%u&xvalue=%04x",val.m_value,val.m_value);
		return	clat;
	}
	break;
	case	4:
	{
		t_zcl_uint4_value val;

		
		memset	(&val,0,sizeof(val));
		memcpy	(&val,param,sizeof(val));
		val.m_value	= ntohl(val.m_value);
		memcpy	(punion,&val,sizeof(val));
		// OUTPUT
		AW_SPRINTF(f,"value=%u&xvalue=%08x",val.m_value,val.m_value);
		if	(ctxt->cl->cl_id == W_CLU_ANALOGINPUT)
		{
			AW_SPRINTF(f,"&fvalue=%f",*(float *)&val.m_value);
		}
		return	clat;
	}
	break;
	default :
		// no return if not a simple value
	break;
	}

	switch	(clat)
	{
/****/	case	W_CLAT(W_CLU_SIMPLEMETER,W_ATT_SIMPLEMETER_CUR_VALUE) :
	{
		AwZclDecodeMeteringValue(f,param,punion);

	}
	break;
/****/	case	W_CLAT(W_CLU_TICICE,W_ATT_TICICE_DATA) :
/****/	case	W_CLAT(W_CLU_TICICE,W_ATT_TICICE_PER0) :
/****/	case	W_CLAT(W_CLU_TICICE,W_ATT_TICICE_PER1) :

/****/	case	W_CLAT(W_CLU_TICCBE,W_ATT_TICCBE_DATA) :

/****/	case	W_CLAT(W_CLU_TICCJE,W_ATT_TICCJE_DATA) :
	{
		t_zcl_tic_data	val;

		memset	(&val,0,sizeof(val));

		val.m_sz	= *param++;
		if	(val.m_sz)
		{
#if	0
			val.m_data	= param;
#endif
			memcpy(val.m_data,param,val.m_sz);
		}
//printf("decode : sz=%d ptr=%x val='%s'\n",val.m_sz,val.m_data,val.m_data);
		memcpy	(punion,&val,sizeof(val));
		if (f)
		{
			char	tmp[3*W_ZCL_SIZE];
			// OUTPUT
			AW_SPRINTF(f,"sz=%d",val.m_sz);
			if	(val.m_sz)
			{
				AwBinToStr(val.m_data,val.m_sz,tmp,sizeof(tmp));
				AW_SPRINTF(f,"&%s=%s",at->at_name,tmp);
			}
		}
	}
	break;
/****/	case	W_CLAT(W_CLU_CONFIGURATION,W_ATT_CONFIGURATION_WIPADDR) :
	{
		t_zcl_byteN_value val;
		int		ssval	= -(at->at_ssval);
		unsigned char	lg;

		if	(ssval < 0 || ssval > W_ZCL_SIZE)
			return	-2;

		lg	= *param++;
		if	(lg <= 0 || lg > W_ZCL_SIZE)
			return	-3;
		memset	(&val,0,sizeof(val));
		val.m_valsz	= lg;
		memcpy	(&val.m_value[0],param,lg);
		memcpy	(punion,&val,sizeof(val));
		if (f)
		{
			char	tmp[3*W_ZCL_SIZE];
			// OUTPUT
			AwBinToStr(param,lg,tmp,sizeof(tmp));
			AW_SPRINTF(f,"value=%s",tmp);
		}
	}
	break;
/****/	case	W_CLAT(W_CLU_CONFIGURATION,W_ATT_CONFIGURATION_DESC) :
	{
		int		tot;
		int		epn = 0;

		t_zcl_simple_desc desc;

		memset	(&desc,0,sizeof(desc));
//		tot	= ntohs(*(ushort *)param);
		tot = *(param)*256 + *(param+1);
#ifdef	TRACE
printf("------------------------------   p1=%d p2=%d tot=%d\n",
						*param,*(param+1),tot);
#endif

#ifdef	TRACE
printf("------------------------------   szdesc=%d\n",tot);
#endif
		param	+= 2;		// drop size
		AwZclDecodeSimpleDesc(f,epn,param,&desc,tot,pend);
		memcpy	(punion,&desc,sizeof(desc));
	}
	break;
/****/	case	W_CLAT(W_CLU_CONFIGURATION,W_ATT_CONFIGURATION_DESCEPN) :
	{
		int		tot;
		int		i;
		int		epn = 0;
		int		nb;
		int		sz;
		char		tmp[128];
		t_zcl_cluster	*cls;

		t_zcl_simple_desc desc;

		memset	(&desc,0,sizeof(desc));
		tot = *(param)*256 + *(param+1);
#ifdef	TRACE
printf("------------------------------   p1=%d p2=%d tot=%d\n",
						*param,*(param+1),tot);
#endif

#ifdef	TRACE
printf("------------------------------   szdesc=%d\n",tot);
#endif
		param	+= 2;		// drop size
		nb	= *param;
		param++;
		desc.m_nbep	= nb;
		for (i = 0; i < nb && param < pend ; i++) 
		{
			epn	= *param;
			param++;
			sz = AwZclDecodeSimpleDesc(NULL,epn,param,&desc,tot,pend);
			param	= param + sz;
		}
		memcpy	(punion,&desc,sizeof(desc));
		if (!f)
			break;
		// OUTPUT
		AW_SPRINTF(f,"nbep=%d",desc.m_nbep);
		nb	= desc.m_nbclin;
		if(nb > 0)
			AW_SPRINTF(f,"&clusterinput=");
		for (i = 0; i < nb; i++) 
		{
			unsigned int clnum;

			clnum	= desc.m_clin[i];
			epn	= desc.m_epin[i];
			cls	= AwZclCluster(clnum);
			sprintf	(tmp,"unknown(%04x)",clnum);
			AW_SPRINTF(f,"%s.%d",cls?cls->cl_name:tmp,epn);
			if (i != nb-1)
			{
				AW_SPRINTF(f,",");
			}
		}
		nb	= desc.m_nbclout;
		if(nb > 0)
			AW_SPRINTF(f,"&clusteroutput=");
		for (i = 0; i < nb; i++) 
		{
			unsigned int clnum;

			clnum	= desc.m_clout[i];
			epn	= desc.m_epout[i];
			cls	= AwZclCluster(clnum);
			sprintf	(tmp,"unknown(%04x)",clnum);
			AW_SPRINTF(f,"%s.%d",cls?cls->cl_name:tmp,epn);
			if (i != nb-1)
			{
				AW_SPRINTF(f,",");
			}
		}
	}
	break;
/****/	case	W_CLAT(W_CLU_CONFIGURATION,W_ATT_CONFIGURATION_POWER) :
	{
		t_zcl_power_value val;
		int		i;

		memset	(&val,0,sizeof(val));
		val.m_sz	= *param;
		val.m_count	= *param++;
		val.m_mode	= *param++;
		val.m_count--;
		val.m_source	= *param++;
		val.m_count--;

		val.m_count	= val.m_count / 2;
		for	(i = 0; i < val.m_count; i++)
		{
			val.m_level[i] = *(param)*256 + *(param+1);
			param 	+= 2;
		}

		memcpy	(punion,&val,sizeof(val));
		if (f)
		{
			// OUTPUT
			AW_SPRINTF(f,"mode=%x",val.m_mode);
			AW_SPRINTF(f,"&source=%x",val.m_source);
			AW_SPRINTF(f,"&count=%d",val.m_count);
			AW_SPRINTF(f,"&level=");
			for	(i = 0; i < val.m_count; i++)
			{
				AW_SPRINTF(f,"%d",val.m_level[i]);
				if (i != val.m_count-1)
				{
					AW_SPRINTF(f,",");
				}
			}
		}
	}
	break;

/****/	default :
		rtl_trace(0,"AwZclDecodeReadAttrResp() clat=%08x not treated\n",
					clat);
	break;


	}

	return	clat;
}

static	int	AwZclDecodeRepoAttrResp(char *f,t_zcl_dec_ctxt *ctxt)
{
	unsigned int	clat;	// cluster + attribut
	clat	= AwZclDecodeReadAttrResp(f,ctxt);
	if	(clat <= 0)
		rtl_trace(0,"AwZclDecodeRepoAttrResp() clat=%08x not treated\n",
						clat);
	return	clat;
}

static	int	AwZclDecodeWritAttrRqst(char *f,t_zcl_dec_ctxt *ctxt)
{
	unsigned int	clat;	// cluster + attribut
	clat	= AwZclDecodeReadAttrResp(f,ctxt);
	if	(clat <= 0)
		rtl_trace(0,"AwZclDecodeWritAttrRqst() clat=%08x not treated\n",
						clat);
	return	clat;
}

int	AwZclDecode(char *f,t_zcl_msg *m,unsigned char *bin,int sz)
{
	int		ret = -1;
	unsigned char	cmid;
	unsigned short clid;
	unsigned short atid;
	t_zcl_command	*cmd;
	t_zcl_cluster	*cl;
	t_zcl_attribut	*at;
	unsigned char	*param;	// start of parameters
	unsigned char	*pend;	// end of structure
	int		type;	// zcl attribut type

	t_zcl_dec_ctxt	ctxt;

	unsigned char	c1,c2;

	if	(f)
		*f	= '\0';

	AwZclInitProto();

	if	(!m)
		return	-1;

	if	(bin)
	{
		if	(sz <= 0 || sz > sizeof(m->u_pack.m_data))
			return	-11;
		memset	(m,0,sizeof(t_zcl_msg));
		memcpy	(&m->u_pack.m_data,bin,sz);
	}

	// search flag 0x11
	if	((m->u_pack.m_head.m_flag & 0x1F) != 0x11)
	{
		rtl_trace(0,"AwZclDecode() flag error %x/%x not found\n",
		m->u_pack.m_head.m_flag,(m->u_pack.m_head.m_flag & 0x1F));
		return	-111;
	}

	m->m_epn	= (m->u_pack.m_head.m_flag)>>5;
	if	(m->m_epn >= W_ZCL_EPN_MAX)
		m->m_epn	= 0;
	m->m_size	= sz;


	cmid	= m->u_pack.m_head.m_cmd;
	clid	= ntohs(m->u_pack.m_head.m_cluster_id);

	// search command
	cmd		= AwZclCommand(cmid);
	if	(!cmd)
	{
		rtl_trace(0,"AwZclDecode() command %x not found (%d)\n",cmid,__LINE__);
		return -2;
	}

	// search cluster
	cl	= AwZclCluster(clid);
	if	(!cl)
	{
		rtl_trace(0,"AwZclDecode() cluster %x not found (%d)\n",cmid,__LINE__);
		return	-3;
	}
	
	// search attribut
	atid	= 0xFFFF;
	at	= NULL;
	switch	(cmd->cm_id)
	{
	case	W_CMD_READ_ATTR_RQST :
	case	W_CMD_READ_ATTR_RESP :
	case	W_CMD_REPO_ATTR_EVNT :
		atid	= ntohs(m->u_pack.m_read_attr_rqst.m_attr_id);
		at = AwZclAttribut(clid,atid);
		if	(!at)
		{
			rtl_trace(0,"AwZclDecode() attribut %x not found %x (%d)\n",
				atid,cmd->cm_id,__LINE__);
			return	-4;
		}
	break;
	case	W_CMD_WRIT_ATTR_RQST :
		atid	= ntohs(m->u_pack.m_writ_attr_rqst.m_attr_id);
		at = AwZclAttribut(clid,atid);
		if	(!at)
		{
			rtl_trace(0,"AwZclDecode() attribut %x not found %x (%d)\n",
				atid,cmd->cm_id,__LINE__);
			return	-44;
		}
	break;
	case	W_CMD_REPO_CONF_RQST :
	case	W_CMD_REPO_CONF_RESP :
	case	W_CMD_READ_CONF_RQST :
	case	W_CMD_READ_CONF_RESP :
		c1	= m->u_pack.m_data.m_byte[cmd->cm_id_offset];
		c2	= m->u_pack.m_data.m_byte[cmd->cm_id_offset+1];
		atid	= (c1 * 256 + c2);
//printf("c1=%d c2=%d atid=0x0%x\n",c1,c2,atid);
		at = AwZclAttribut(clid,atid);
		if	(!at)
		{
			rtl_trace(0,"AwZclDecode() attribut %x not found %x (%d)\n",
				atid,cmd->cm_id,__LINE__);
			return	-5;
		}
	break;
	case	W_CMD_COMMAND_RQST :	// no attribut for simple command
	break;
	default :
		rtl_trace(0,"AwZclDecode() command not treated %x\n",cmd->cm_id);
		return	-6;
	break;
	}

	m->m_cmd	= cmd->cm_id;
	m->m_cluster_id	= clid;
	m->m_attr_id	= atid;
	m->m_status	= 0;
	m->m_direction	= 0;
	m->m_cmd_value	= 0;

	if	(cmd->cm_id == W_CMD_COMMAND_RQST)
	{	// no attribut in this case, only a single byte value
		c1	= m->u_pack.m_data.m_byte[cmd->cm_val_offset];
		m->m_cmd_value	= c1;

		// OUTPUT
		AW_SPRINTF(f,"./%d/command/%s/%s/?value=%d\n",m->m_epn,
		cmd->cm_name,cl->cl_name,c1);

		return	W_CLAT(clid,0);
	}

	// search status on response
	if	(cmd->cm_response && cmd->cm_stat_offset)
	{
		m->m_status	= m->u_pack.m_data.m_byte[cmd->cm_stat_offset];
	}
	if	(cmd->cm_direction_offset)
	{
		m->m_direction	= 
			m->u_pack.m_data.m_byte[cmd->cm_direction_offset];
	}


	if	(cmd->cm_val_offset == 0)
	{	// no param

		// OUTPUT
		AW_SPRINTF(f,"./%d/%s/%s/%s/%s/",m->m_epn,
			cmd->cm_response?"response":"request",
			cmd->cm_name,cl->cl_name,at->at_name);
		if	(cmd->cm_response && cmd->cm_stat_offset)
		{
			AW_SPRINTF(f,"status=%d",m->m_status);
		}
		if	(cmd->cm_direction_offset)
		{
			AW_SPRINTF(f,"&direction=%d",m->m_direction);
		}
		AW_SPRINTF(f,"\n");
		return	W_CLAT(clid,atid);
	}


	// search value
	type	= at->at_type;
	if	(cmd->cm_val_offset > sz)
	{
		rtl_trace(0,"cl=%04x at=%04x valoffset=%d sz=%d\n",
			clid,atid,cmd->cm_val_offset,sz);
		return	-7;
	}
	param	= &(m->u_pack.m_data.m_byte[cmd->cm_val_offset]);
	pend	= &(m->u_pack.m_data.m_byte[0])+sz;

	// OUTPUT
	AW_SPRINTF(f,"./%d/%s/%s/%s/%s/?",m->m_epn,
	cmd->cm_response?"response":"request",
	cmd->cm_name,cl->cl_name,at->at_name);

	if	(cmd->cm_response && cmd->cm_stat_offset)
	{
		AW_SPRINTF(f,"status=%d&appt=%x&",m->m_status,
			m->u_pack.m_read_attr_resp.m_attr_type);
	}
	if	(cmd->cm_direction_offset)
	{
		AW_SPRINTF(f,"direction=%d&",m->m_direction);
	}

	ctxt.m		= m;
	ctxt.sz		= sz;
	ctxt.cl		= cl;
	ctxt.at		= at;
	ctxt.cmd	= cmd;
	ctxt.param	= param;
	ctxt.pend	= pend;
	ctxt.punion	= &(m->u_param.m_byteN_value);
	switch	(cmd->cm_id)
	{
	case	W_CMD_READ_ATTR_RQST :	// no param, already treated
	case	W_CMD_READ_CONF_RQST :	// no param, already treated
	case	W_CMD_REPO_CONF_RESP :	// no param, already treated
	case	W_CMD_COMMAND_RQST :	// basic param, already treated
		rtl_trace(0,"AwZclDecode() already treated %0x\n",
								cmd->cm_id);
		ret	= -8;
	break;

	case	W_CMD_READ_ATTR_RESP :	// status + attribut
		ret	= AwZclDecodeReadAttrResp(f,&ctxt);
	break;
	case	W_CMD_REPO_ATTR_EVNT :	// idem with no status
		ret	= AwZclDecodeRepoAttrResp(f,&ctxt);
	break;
	case	W_CMD_READ_CONF_RESP :
		ret	= AwZclDecodeReadConfResp(f,&ctxt);
	break;
	case	W_CMD_REPO_CONF_RQST :
		ret	= AwZclDecodeRepoConfRqst(f,&ctxt);
	break;
	case	W_CMD_WRIT_ATTR_RQST :
		ret	= AwZclDecodeWritAttrRqst(f,&ctxt);
	break;
	default :
		rtl_trace(0,"AwZclDecode() command not treated %0x\n",
								cmd->cm_id);
		ret	= -9;
	break;
	}

	// OUTPUT
	AW_SPRINTF(f,"\n");

	return	ret;
}

#if	0
//
//
#define	MAX_FIELDS_MAX_NUMBER	63

typedef	struct
{
	unsigned char	el_present;	// element present in frame
	unsigned char	el_num;		// elem num [0..CTCBE_FIELDS_MAX_NUMBER]
					// or [0..CJE_FIELDS_MAX_NUMBER]
					// or [0..ICE_FIELDS_MAX_NUMBER]
	unsigned char	el_type;	// elem type TYPE_U16,TYPE_CSTRING,...

	char		*el_name;	// elem name ("CONTRAT","DATECOUR",...)
	char		*el_cfmt;	// c format (printf(3))

	char		*el_filter;	// only use for encoding function
					// ex "PM,P*,????,10,1000.10,....
	unsigned short	el_mint;	// reporting min
	unsigned short	el_maxt;	// reporting max

	union				// depends on el_type
	{
		unsigned	char	u_c;
		unsigned	int	u_i;
				float	u_f;
				char	*u_str;	// null terminated c string
						// -> to msg->u_pack.m_data
	}		el_u;

}	t_zcl_tic_elem;

/*
 * u_str ne doit pas etre realloue mais pointe dans la trame binaire complete
 * du message recu qui se trouve dans msg->u_pack.m_data, et qui est aussi
 * le premier octet de cette struct. Le param msg de AwZclDecodeTic() peut donc
 * etre vu comme un (char *)
 */


/*!
 *
 * @brief parse une trame tic binaire et valorise un tableau de donnees avec
 * leur description en fonction du type de compteur; meme si t_zcl_tic_elem
 * contient son numero d'element, le tableau n'est pas compresse, chaque elem
 * se trouve a sa place. (un define par elem est peut etre necessaire)
 * @param humanout not used yet, will contain a human readable form of data
 * @param msg raw sensor message mapped to the driver t_zcl_msg structure
 * @param bin the tic data frame to decode
 * @param szbin size of tic data frame
 * @param cluster type
 * @param tbticelem table of elements to populate (allocted by caller)
 * @return < 0 si erreur, >= 0 le nombre de donnees
 * 
 */
int	AwZclDecodeTic(
		char *humanout, /*t_zcl_msg*/ void *msg,
		unsigned char *bin,int szbin,
		int type,
		t_zcl_tic_elem tbticelem[XXX_FIELDS_MAX_NUMBER]
	)
{
	...
}	

/*!
 *
 * @brief a partir d'une table d'element encode la frame binaire pour
 * selectionner le reporting en fonction de filter utilisateur
 * @param msg raw sensor message mapped to the driver t_zcl_msg structure
 * @param bin the tic data frame to encode
 * @param szmax max size of tic data frame
 * @param cluster type
 * @param tbticelem table of elements to report (allocted by caller)
 * @return < 0 si erreur, >= 0 taille de l'encodage de la partie tic data
 * 
 */
int	AwZclEncodeTic(/*t_zcl_msg*/ void *msg,
		unsigned char *bin,int szmax,
		int type,
		t_zcl_tic_elem tbticelem[XXX_FIELDS_MAX_NUMBER],int nbelem
{
	...
}


/*!
 *
 * @brief return the name of a tic element
 */
char	*AwZclNameTicElem(int type,int num)
{
}

/*!
 *
 * @brief return the number of a named tic element 
 */
int	AwZclNumTicElem(int type,char *name)
{
}

void	UsageEx()
{
	char		humanout[2000];
	t_zcl_msg	msg;
	int		szmsg;
	unsigned int	clat;	// cluster + attribut

	szmsg	= recvfrom(fd,(char *)&msg,sizeof(msg));
	ret	= AwZclDecode(humanout,&zcl,NULL,szmsg);
	printf	("%s\n",humanout);

	printf("zcldecode : ret=%08x cmd=%02x status=%x cl=%04x at=%04x\n",
		ret,msg.m_cmd,msg.m_status,msg.m_cluster_id,msg.m_attr_id);

	clat	= W_CLAT(msg.m_cluster_id,msg.m_attr_id);
	if	(clat != W_CLAT(W_CLU_TICICE,W_ATT_TICICE_DATA))
	{	// not a tic data
		return;
	}

	t_zcl_tic_elem	tbticelem[XXX_FIELDS_MAX_NUMBER];
	int		nbelem	= XXX_FIELDS_MAX_NUMBER;
	char		*ticdata	= msg.m_param.m_tic_data.m_data;
	int		szticdata	= msg.m_param.m_tic_data.m_sz;
	int		type		= msg.m_cluster_id;
	int		i;

	for	(i = 0 ; i < nbelem ; i++)
		tbticelem[i].el_present	= 0;

	nbelem	= AwZclDecodeTic(humanout,(char *)&msg,type,ticdata,szticdata);
	for	(i = 0 ; i < nbelem ; i++)
	{
		t_zcl_tic_elem	*el = &tbticelem[i];

		if	(!el->el_present)	continue;
		printf	("name=%s value=",el->el_name);
		switch	(el->el_type)
		{
		case	C_STRING :
			printf	("%s\n",el->el_u.u_str);
		break;
		case	TYPE_U32:
		case	TYPE_U16:
		...
			printf	(el->el_cfmt,el->el_u.u_int);
			printf	("\n");
		break;
		default:
			printf	("???\n");
		break;
		}
	}


	t_zcl_msg	rep;
	char		ticdata[XXXXX];

	memset	(&rep,0,sizeof(rep));
	rep.m_cmd		= W_CMD_REPO_CONF_RESP;
	rep.m_cluster_id	= W_CLU_TICICE;
	rep.m_attr_id		= W_CLU_TICICE_DATA;

	for	(i = 0 ; i < nbelem ; i++)
		tbticelem[i].el_present	= 0;
	tbticelem[XXX_ELEM].el_num	= XXX_ELEM;
	tbticelem[XXX_ELEM].el_mint	= 300;
	tbticelem[XXX_ELEM].el_maxt	= 3600;
	tbticelem[XXX_ELEM].el_filter	= "P???";

	ret	= AwZclEncodeTic(rep,ticdata,sizeof(ticdata),
		W_CLU_TICICE,tbticelem,nbelem);

	memcpy(rep.m_param.m_tic_data.m_data,ticdata,ret);
	rep.m_param.m_tic_data.m_sz	= ret;

	ret	= AwZclEncode(&rep,NULL,0);

	sendto(fd,(char *)&rep,ret,....);

}
#endif
