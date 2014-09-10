
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

static	int	AwZclEncodeSimpleConfig(t_zcl_simple_config *cfg,unsigned char *str,int nbb)
{
	int	lgout	= 0;
	unsigned int	v;

	*(str+lgout)	= (cfg->m_min_time)>>8;
	*(str+lgout+1)	= (cfg->m_min_time)&0x00FF;
	lgout		+= 2;
	*(str+lgout)	= (cfg->m_max_time)>>8;
	*(str+lgout+1)	= (cfg->m_max_time)&0x00FF;
	lgout		+= 2;
	switch	(nbb)
	{
	case	1:
		*(str+lgout)	= (cfg->m_min_change)&0x000000FF;
		lgout		+= 1;
	break;
	case	4:
		v		= htonl(cfg->m_min_change);
		memcpy(str+lgout,(char *)&v,4);	// TODO
		lgout		+= 4;
	break;
	case	2:
	default :
		*(str+lgout)	= (cfg->m_min_change)>>8;
		*(str+lgout+1)	= (cfg->m_min_change)&0x00FF;
		lgout		+= 2;
	break;
	}

	return	lgout;
}

static	int	AwZclEncodeMeteringConfig(t_zcl_metering_config *cfg,unsigned char *str)
{
	int	lgout	= 0;

	*(str+lgout)	= (cfg->m_min_time)>>8;
	*(str+lgout+1)	= (cfg->m_min_time)&0x00FF;
	lgout		+= 2;
	*(str+lgout)	= (cfg->m_max_time)>>8;
	*(str+lgout+1)	= (cfg->m_max_time)&0x00FF;
	lgout		+= 2;

	if	(cfg->m_metersz == 0)
		cfg->m_metersz	= 0x0c;
	*(str+lgout)	= (cfg->m_metersz);
	lgout		+= 1;

	*(str+lgout)	= (cfg->m_sumWh & 0x00FF0000) >> 16;
	*(str+lgout+1)	= (cfg->m_sumWh & 0x0000FF00) >> 8;
	*(str+lgout+2)	= (cfg->m_sumWh & 0x000000FF);
	lgout		+= 3;

	*(str+lgout)	= (cfg->m_sumVarh & 0x00FF0000) >> 16;
	*(str+lgout+1)	= (cfg->m_sumVarh & 0x0000FF00) >> 8;
	*(str+lgout+2)	= (cfg->m_sumVarh & 0x000000FF);
	lgout		+= 3;

	*(str+lgout)	= (cfg->m_count)>>8;
	*(str+lgout+1)	= (cfg->m_count)&0x00FF;
	lgout		+= 2;

	*(str+lgout)	= (cfg->m_curWh)>> 8;
	*(str+lgout+1)	= (cfg->m_curWh)&0x00FF;
	lgout		+= 2;

	*(str+lgout)	= (cfg->m_curVarh)>> 8;
	*(str+lgout+1)	= (cfg->m_curVarh)&0x00FF;
	lgout		+= 2;

	return	lgout;
}

static	int	AwZclEncodeTicConfig(t_zcl_tic_config *cfg,unsigned char *str)
{
	int	lgout	= 0;

	*(str+lgout)	= (cfg->m_min_time)>>8;
	*(str+lgout+1)	= (cfg->m_min_time)&0x00FF;
	lgout		+= 2;
	*(str+lgout)	= (cfg->m_max_time)>>8;
	*(str+lgout+1)	= (cfg->m_max_time)&0x00FF;
	lgout		+= 2;

	*(str+lgout)	= (cfg->m_sz);
	lgout		+= 1;
	if	(cfg->m_sz == 0)
		return	lgout;

	memcpy	(str+lgout,cfg->m_data,cfg->m_sz);
	lgout		+= cfg->m_sz;

	return	lgout;
}

static	int	AwZclEncodePowerConfig(t_zcl_power_config *cfg,unsigned char *str)
{
	int	lgout	= 0;
	int	sz	= 0;
	int	count	= 0;
	int	i;

	*(str+lgout)	= (cfg->m_min_time)>>8;
	*(str+lgout+1)	= (cfg->m_min_time)&0x00FF;
	lgout		+= 2;
	*(str+lgout)	= (cfg->m_max_time)>>8;
	*(str+lgout+1)	= (cfg->m_max_time)&0x00FF;
	lgout		+= 2;

	if	((cfg->m_source & 0x01) == 0x01)
		count++;
	if	((cfg->m_source & 0x02) == 0x02)
		count++;
	if	((cfg->m_source & 0x04) == 0x04)
		count++;
	if	((cfg->m_source & 0x08) == 0x08)
		count++;
	if	((cfg->m_source & 0x10) == 0x10)
		count++;

	sz	= 2 /* mode + source */ + count * 2;
	*(str+lgout)	= sz;
	lgout		+= 1;
	*(str+lgout)	= cfg->m_mode;
	lgout		+= 1;
	*(str+lgout)	= cfg->m_source;
	lgout		+= 1;

	for	(i = 0 ; i < count ; i++)
	{
		*(str+lgout)	= (cfg->m_level[i])>>8;
		*(str+lgout+1)	= (cfg->m_level[i])&0x00FF;
		lgout		+= 2;
	}

	return	lgout;
}

static	int	AwZclEncodeRepoConfRqst(t_zcl_msg *mess,unsigned char *str,int szmax)
{
	int		lgout	= 0;
	unsigned int	clat;	// cluster + attribut
	t_zcl_attribut	*at;

	at	= AwZclAttribut(mess->m_cluster_id,mess->m_attr_id);
	clat	= W_CLAT(mess->m_cluster_id,mess->m_attr_id);

	switch	(clat)
	{
/*2o*/	case	W_CLAT(W_CLU_TEMPERATURE,	W_ATT_TEMPERATURE_CUR_VALUE) :
	case	W_CLAT(W_CLU_HUMIDITY,		W_ATT_HUMIDITY_CUR_VALUE) :
	case	W_CLAT(W_CLU_ILLUMINANCE,	W_ATT_ILLUMINANCE_CUR_VALUE) :
	case	W_CLAT(W_CLU_BINARYINPUT,	W_ATT_BINARYINPUT_DEBOUNCE) :
	{
		t_zcl_simple_config	*cfg;

		*(str+lgout)	= (at->at_type);
		lgout		+= 1;
		cfg	= &(mess->u_param.m_simple_config);
		lgout	+= AwZclEncodeSimpleConfig(cfg,str+lgout,2);
	}
	break;
/*1o*/	case	W_CLAT(W_CLU_ONOFF,		W_ATT_ONOFF_ONOFF) :
	case	W_CLAT(W_CLU_OCCUPANCY,		W_ATT_OCCUPANCY_OCCUPANCY) :
	case	W_CLAT(W_CLU_BINARYINPUT,	W_ATT_BINARYINPUT_CUR_VALUE) :
	case	W_CLAT(W_CLU_BINARYINPUT,	W_ATT_BINARYINPUT_POLARITY) :
	case	W_CLAT(W_CLU_BINARYINPUT,	W_ATT_BINARYINPUT_EDGE_SELECT) :
	{
		t_zcl_simple_config	*cfg;

		*(str+lgout)	= (at->at_type);
		lgout		+= 1;
		cfg	= &(mess->u_param.m_simple_config);
		lgout	+= AwZclEncodeSimpleConfig(cfg,str+lgout,1);
	}
	break;
/*4o*/	case	W_CLAT(W_CLU_ANALOGINPUT,	W_ATT_ANALOGINPUT_CUR_VALUE) :
	case	W_CLAT(W_CLU_BINARYINPUT,	W_ATT_BINARYINPUT_COUNTER) :
	{
		t_zcl_simple_config	*cfg;

		*(str+lgout)	= (at->at_type);
		lgout		+= 1;
		cfg	= &(mess->u_param.m_simple_config);
		lgout	+= AwZclEncodeSimpleConfig(cfg,str+lgout,4);
	}
	break;
/*?o*/	case	W_CLAT(W_CLU_SIMPLEMETER,	W_ATT_SIMPLEMETER_CUR_VALUE) :
	{
		t_zcl_metering_config	*cfg;

		*(str+lgout)	= (at->at_type);
		lgout		+= 1;
		cfg	= &(mess->u_param.m_metering_config);
		lgout	+= AwZclEncodeMeteringConfig(cfg,str+lgout);
	}
	break;
/*?o*/	case	W_CLAT(W_CLU_TICICE,	W_ATT_TICICE_DATA) :
/*?o*/	case	W_CLAT(W_CLU_TICICE,	W_ATT_TICICE_PER0) :
/*?o*/	case	W_CLAT(W_CLU_TICICE,	W_ATT_TICICE_PER1) :

/*?o*/	case	W_CLAT(W_CLU_TICCBE,	W_ATT_TICCBE_DATA) :

/*?o*/	case	W_CLAT(W_CLU_TICCJE,	W_ATT_TICCJE_DATA) :
	{
		t_zcl_tic_config	*cfg;

		*(str+lgout)	= (at->at_type);
		lgout		+= 1;
		cfg	= &(mess->u_param.m_tic_config);
		lgout	+= AwZclEncodeTicConfig(cfg,str+lgout);
	}
	break;
/*?o*/	case	W_CLAT(W_CLU_CONFIGURATION,	W_ATT_CONFIGURATION_POWER) :
	{
		t_zcl_power_config	*cfg;

		*(str+lgout)	= (at->at_type);
		lgout		+= 1;
		cfg	= &(mess->u_param.m_power_config);
		lgout	+= AwZclEncodePowerConfig(cfg,str+lgout);
	}
	break;
	default :
		rtl_trace(0,"AwZclEncodeRepoConfRqst() clat not treated %08x\n",
				clat);
		mess->m_xxcode	= 0;
	break;
	}

	return	lgout;
}

static	int	AwZclEncodeReadConfResp(t_zcl_msg *mess,unsigned char *str,int szmax)
{
	unsigned int	lgout;
	unsigned int	clat;	// cluster + attribut

	clat	= W_CLAT(mess->m_cluster_id,mess->m_attr_id);

	lgout	= AwZclEncodeRepoConfRqst(mess,str,szmax);
	if	( lgout <= 0)
		rtl_trace(0,"AwZclEncodeRepoConfResp() clat not treated %08x\n",
				clat);


	return	lgout;
}

static	int	AwZclEncodeReadAttrResp(t_zcl_msg *mess,unsigned char *str,int szmax)
{
	int		lgout	= 0;
	unsigned int	clat;	// cluster + attribut
	t_zcl_attribut	*at;

	at	= AwZclAttribut(mess->m_cluster_id,mess->m_attr_id);
	if	(!at)
		return	0;
	clat	= W_CLAT(mess->m_cluster_id,mess->m_attr_id);

	if	(at->at_ssval > 0)
	{
		*(str+lgout)	= (at->at_type);
		lgout		+= 1;
		switch	(at->at_ssval)
		{
		case	1:
		{
			t_zcl_uint1_value	*rep;
			rep		= &(mess->u_param.m_uint1_value);
			*(str+lgout)	= (rep->m_value);
			lgout		+= 1;
			return	lgout;
		}
		break;
		case	2:
		{
			t_zcl_uint2_value	*rep;

			rep		= &(mess->u_param.m_uint2_value);
			*(str+lgout)	= (rep->m_value)>>8;
			*(str+lgout+1)	= (rep->m_value)&0x00FF;
			lgout		+= 2;
			return	lgout;
		}
		break;
		case	4:
		{
			t_zcl_uint4_value	*rep;
			unsigned	int	v;

			rep		= &(mess->u_param.m_uint4_value);
			v		= htonl(rep->m_value);
			memcpy(str+lgout,(char *)&v,4);	// TODO
			lgout		+= 4;
			return	lgout;
		}
		break;
		}
		return	lgout;
	}
	if	(at->at_ssval < 0)
	{
		t_zcl_byteN_value	*rep;
		int		ssval	= -(at->at_ssval);

		rep		= &(mess->u_param.m_byteN_value);
		if	(ssval > W_ZCL_SIZE)
			ssval	= W_ZCL_SIZE;

		if	(rep->m_valsz > ssval)
			rep->m_valsz	= ssval;

		*(str+lgout)	= (at->at_type);
		lgout		+= 1;
		*(str+lgout)	= (rep->m_valsz);
		lgout		+= 1;
		memcpy(str+lgout,rep->m_value,rep->m_valsz);
		lgout	+= rep->m_valsz;
		return	lgout;
	}

	switch	(clat)
	{
/****/	case	W_CLAT(W_CLU_SIMPLEMETER,W_ATT_SIMPLEMETER_CUR_VALUE) :
	{
		t_zcl_metering_value	*rep;

		rep		= &(mess->u_param.m_metering_value);
		*(str+lgout)	= (at->at_type);
		lgout		+= 1;
		*(str+lgout)	= (rep->m_metersz);
		lgout		+= 1;

		*(str+lgout)	= (rep->m_sumWh & 0x00FF0000) >> 16;
		*(str+lgout+1)	= (rep->m_sumWh & 0x0000FF00) >> 8;
		*(str+lgout+2)	= (rep->m_sumWh & 0x000000FF);
		lgout		+= 3;

		*(str+lgout)	= (rep->m_sumVarh & 0x00FF0000) >> 16;
		*(str+lgout+1)	= (rep->m_sumVarh & 0x0000FF00) >> 8;
		*(str+lgout+2)	= (rep->m_sumVarh & 0x000000FF);
		lgout		+= 3;

		*(str+lgout)	= (rep->m_count)>>8;
		*(str+lgout+1)	= (rep->m_count)&0x00FF;
		lgout		+= 2;

		*(str+lgout)	= (rep->m_curWh)>> 8;
		*(str+lgout+1)	= (rep->m_curWh)&0x00FF;
		lgout		+= 2;

		*(str+lgout)	= (rep->m_curVarh)>> 8;
		*(str+lgout+1)	= (rep->m_curVarh)&0x00FF;
		lgout		+= 2;
	}
	break;
/****/	case	W_CLAT(W_CLU_TICICE,	W_ATT_TICICE_DATA) :
/****/	case	W_CLAT(W_CLU_TICICE,	W_ATT_TICICE_PER0) :
/****/	case	W_CLAT(W_CLU_TICICE,	W_ATT_TICICE_PER1) :

/****/	case	W_CLAT(W_CLU_TICCBE,	W_ATT_TICCBE_DATA) :

/****/	case	W_CLAT(W_CLU_TICCJE,	W_ATT_TICCJE_DATA) :
	{
		t_zcl_tic_data	*rep;

		*(str+lgout)	= (at->at_type);
		lgout		+= 1;
		rep		= &(mess->u_param.m_tic_data);
//printf("encode : sz=%d ptr=%x val='%s'\n",rep->m_sz,rep->m_data,rep->m_data);
		*(str+lgout)	= rep->m_sz;
		lgout		+= 1;
		if	(rep->m_sz > 0)
		{
			memcpy	(str+lgout,rep->m_data,rep->m_sz);
			lgout		+= rep->m_sz;
		}
	}
	break;
	default :
		rtl_trace(0,"AwZclEncodeReadAttrResp() clat not treated %08x\n",
				clat);
		mess->m_xxcode	= 0;
	break;
	}

	return	lgout;
}

static	int	AwZclEncodeWritAttrRqst(t_zcl_msg *mess,unsigned char *str,int szmax)
{
	unsigned int	lgout;
	unsigned int	clat;	// cluster + attribut

	clat	= W_CLAT(mess->m_cluster_id,mess->m_attr_id);
	lgout	= AwZclEncodeReadAttrResp(mess,str,szmax);
	if	( lgout <= 0)
		rtl_trace(0,"AwZclEncodeWritAttrRqst() clat not treated %08x\n",
				clat);

	return	lgout;
}

int	AwZclEncode(t_zcl_msg *mess,unsigned char *str,int szmax)
{
	int	lgout	= 0;
	unsigned char	cmd;

	AwZclInitProto();

	if	(str == NULL)
	{	// encode in this messsage
		str	= &(mess->u_pack.m_data.m_byte[0]);
		szmax	= sizeof(mess->u_pack.m_data);
	}

	memset	(str,0,szmax);
	mess->m_size	= 0;
	mess->m_xxcode	= 'E';

	if	(mess->m_epn >= W_ZCL_EPN_MAX)
		mess->m_epn	= 0;

	str[lgout++]	= (0x11)|((mess->m_epn)<<5);
	str[lgout++]	= mess->m_cmd;
	*(str+lgout)	= (mess->m_cluster_id)>>8;
	*(str+lgout+1)	= (mess->m_cluster_id)&0x00FF;
	lgout		+= 2;

	cmd	= mess->m_cmd;
	switch	(mess->m_cmd)
	{
	case	W_CMD_READ_ATTR_RQST :
		*(str+lgout)	= (mess->m_attr_id)>>8;
		*(str+lgout+1)	= (mess->m_attr_id)&0x00FF;
		lgout		+= 2;
		goto	encodedone;
	break;
	case	W_CMD_REPO_CONF_RQST :
		*(str+lgout)	= mess->m_direction;
		lgout		+= 1;
		*(str+lgout)	= (mess->m_attr_id)>>8;
		*(str+lgout+1)	= (mess->m_attr_id)&0x00FF;
		lgout		+= 2;
		lgout += AwZclEncodeRepoConfRqst(mess,str+lgout,szmax);
	break;
	case	W_CMD_WRIT_ATTR_RQST :
		*(str+lgout)	= (mess->m_attr_id)>>8;
		*(str+lgout+1)	= (mess->m_attr_id)&0x00FF;
		lgout		+= 2;
		lgout += AwZclEncodeWritAttrRqst(mess,str+lgout,szmax);
	break;
	case	W_CMD_COMMAND_RQST :	// no attribut
		*(str+lgout)	= (mess->m_cmd_value);
		lgout		+= 1;
		goto	encodedone;
	break;
	case	W_CMD_READ_CONF_RQST :
		*(str+lgout)	= mess->m_direction;
		lgout		+= 1;
		*(str+lgout)	= (mess->m_attr_id)>>8;
		*(str+lgout+1)	= (mess->m_attr_id)&0x00FF;
		lgout		+= 2;
	break;

// encode responses for tests ...
	case	W_CMD_READ_ATTR_RESP :
		*(str+lgout)	= (mess->m_attr_id)>>8;
		*(str+lgout+1)	= (mess->m_attr_id)&0x00FF;
		lgout		+= 2;
		*(str+lgout)	= mess->m_status;
		lgout		+= 1;
		lgout += AwZclEncodeReadAttrResp(mess,str+lgout,szmax);
	break;
	case	W_CMD_REPO_ATTR_EVNT :	// idem without status
		*(str+lgout)	= (mess->m_attr_id)>>8;
		*(str+lgout+1)	= (mess->m_attr_id)&0x00FF;
		lgout		+= 2;
		lgout += AwZclEncodeReadAttrResp(mess,str+lgout,szmax);
	break;
	case	W_CMD_REPO_CONF_RESP :
		*(str+lgout)	= mess->m_status;
		lgout		+= 1;
		*(str+lgout)	= mess->m_direction;
		lgout		+= 1;
		*(str+lgout)	= (mess->m_attr_id)>>8;
		*(str+lgout+1)	= (mess->m_attr_id)&0x00FF;
		lgout		+= 2;
	break;

	case	W_CMD_READ_CONF_RESP :
		*(str+lgout)	= mess->m_status;
		lgout		+= 1;
		*(str+lgout)	= mess->m_direction;
		lgout		+= 1;
		*(str+lgout)	= (mess->m_attr_id)>>8;
		*(str+lgout+1)	= (mess->m_attr_id)&0x00FF;
		lgout		+= 2;
		lgout += AwZclEncodeReadConfResp(mess,str+lgout,szmax);
	break;

	default :
		rtl_trace(0,"AwZclEncode() command not treated %0x\n",cmd);
		mess->m_xxcode	= 0;
	break;
	}

encodedone :
//	mess->u_pack.m_head.m_flag	= (0x11)|((mess->m_epn)<<5);
	mess->m_size			= lgout;

#if	0
	if	(!mess->m_xxcode)
		return	-1;
#endif
	return	lgout;
}

