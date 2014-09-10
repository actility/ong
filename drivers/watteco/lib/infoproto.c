
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

#include "rtlbase.h"
#include "watteco.h"


static	t_zcl_attribut	TbAttribut[] =
{
	{
	W_CLU_ONOFF	, W_ATT_ONOFF_ONOFF	, 
	"on-off"	, 0x10,	1,
	},

	{
	W_CLU_SIMPLEMETER	,W_ATT_SIMPLEMETER_CUR_VALUE	,
	"cur-value"	, 0x41,	0,
	},

	{
	W_CLU_TEMPERATURE	,W_ATT_TEMPERATURE_CUR_VALUE	,
	"cur-value"	, 0x29,	2,
	},
	{
	W_CLU_TEMPERATURE	,W_ATT_TEMPERATURE_MIN_VALUE	,
	"min-value"	, 0x29,	2,
	},
	{
	W_CLU_TEMPERATURE	,W_ATT_TEMPERATURE_MAX_VALUE	,
	"max-value"	, 0x29,	2,
	},


	{
	W_CLU_HUMIDITY		,W_ATT_HUMIDITY_CUR_VALUE	,
	"cur-value"	, 0x21,	2,
	},
	{
	W_CLU_HUMIDITY		,W_ATT_HUMIDITY_MIN_VALUE	,
	"min-value"	, 0x21,	2,
	},
	{
	W_CLU_HUMIDITY		,W_ATT_HUMIDITY_MAX_VALUE	,
	"max-value"	, 0x21,	2,
	},



	{
	W_CLU_OCCUPANCY		,W_ATT_OCCUPANCY_OCCUPANCY	,
	"cur-value"	, 0x18,	1,
	},
	{
	W_CLU_OCCUPANCY		,W_ATT_OCCUPANCY_OCCUPANCY_TYPE	,
	"type"		, 0x30,	1,
	},
	{
	W_CLU_OCCUPANCY		,W_ATT_OCCUPANCY_OCCUPANCY_PIRO	,
	"piro"		, 0x20,	1,
	},
	{
	W_CLU_OCCUPANCY		,W_ATT_OCCUPANCY_OCCUPANCY_PIRU	,
	"piru"		, 0x20,	1,
	},



	{
	W_CLU_ANALOGINPUT	,W_ATT_ANALOGINPUT_CUR_VALUE	,
	"cur-value"	, 0x39,	4,
	},
	{
	W_CLU_ANALOGINPUT	,W_ATT_ANALOGINPUT_APP_TYPE	,
	"type"		, 0x23,	4,
	},


	{
	W_CLU_BINARYINPUT	,W_ATT_BINARYINPUT_POLARITY	,
	"polarity"	, 0x10,	1,
	},
	{
	W_CLU_BINARYINPUT	,W_ATT_BINARYINPUT_CUR_VALUE	,
	"cur-value"	, 0x10,	1,
	},
	{
	W_CLU_BINARYINPUT	,W_ATT_BINARYINPUT_APP_TYPE	,
	"type"		, 0x23,	4,
	},
	{
	W_CLU_BINARYINPUT	,W_ATT_BINARYINPUT_EDGE_SELECT	,
	"edge-select"	, 0x18,	1,
	},
	{
	W_CLU_BINARYINPUT	,W_ATT_BINARYINPUT_DEBOUNCE	,
	"debounce"	, 0x21,	2,
	},
	{
	W_CLU_BINARYINPUT	,W_ATT_BINARYINPUT_COUNTER	,
	"counter"	, 0x23,	4,
	},


	{
	W_CLU_ILLUMINANCE	,W_ATT_ILLUMINANCE_CUR_VALUE	,
	"cur-value"	, 0x21,	2,
	},
	{
	W_CLU_ILLUMINANCE	,W_ATT_ILLUMINANCE_MIN_VALUE	,
	"min-value"	, 0x21,	2,
	},
	{
	W_CLU_ILLUMINANCE	,W_ATT_ILLUMINANCE_MAX_VALUE	,
	"max-value"	, 0x21,	2,
	},


	{
	W_CLU_CONFIGURATION	,W_ATT_CONFIGURATION_WIPADDR	,
	"wip-addr"	, 0x41,	-16,	// [0..16] bytes
	},
	{
	W_CLU_CONFIGURATION	,W_ATT_CONFIGURATION_PANID	,
	"panid"		, 0x21,	2,
	},
	{
	W_CLU_CONFIGURATION	,W_ATT_CONFIGURATION_DESC	,
	"simple-desc"	, 0x4C,	0,
	},
	{
	W_CLU_CONFIGURATION	,W_ATT_CONFIGURATION_DESCEPN	,
	"simple-descepn"	, 0x4C,	0,
	},
	{
	W_CLU_CONFIGURATION	,W_ATT_CONFIGURATION_POWER	,
	"power"			, 0x41,	0,
	},

	{
	W_CLU_TICICE		,W_ATT_TICICE_DATA	,
	"data"		, 0x41,	0,
	},
	{
	W_CLU_TICICE		,W_ATT_TICICE_PER0	,
	"per0"		, 0x41,	0,
	},
	{
	W_CLU_TICICE		,W_ATT_TICICE_PER1	,
	"per1"		, 0x41,	0,
	},
	{
	W_CLU_TICICE		,W_ATT_TICICE_TYPE	,
	"type"		, 0x20,	1,
	},

	{
	W_CLU_TICCBE		,W_ATT_TICCBE_DATA	,
	"data"		, 0x41,	0,
	},
	{
	W_CLU_TICCBE		,W_ATT_TICCBE_TYPE	,
	"type"		, 0x20,	1,
	},

	{
	W_CLU_TICCJE		,W_ATT_TICCJE_DATA	,
	"data"		, 0x41,	0,
	},
	{
	W_CLU_TICCJE		,W_ATT_TICCJE_TYPE	,
	"type"		, 0x20,	1,
	},
};
static	int	NbAttribut	= sizeof(TbAttribut) / sizeof(t_zcl_attribut);

static	t_zcl_cluster	TbCluster[] =
{
	{
	W_CLU_ONOFF	, "on-off"		,
	},
	{
	W_CLU_SIMPLEMETER, "simple-metering"	,
	},

	{
	W_CLU_TEMPERATURE, "temperature-measurement"	,
	},

	{
	W_CLU_HUMIDITY, "humidity-measurement"	,
	},

	{
	W_CLU_OCCUPANCY, "occupancy-sensing"	,
	},

	{
	W_CLU_ANALOGINPUT, "analog-input"	,
	},

	{
	W_CLU_BINARYINPUT, "binary-input"	,
	},

	{
	W_CLU_ILLUMINANCE, "illuminance-measurement"	,
	},

	{	 
	W_CLU_CONFIGURATION, "configuration"	,
	},

	{
	W_CLU_TICICE, "tic-ice-metering"	,
	},
	{
	W_CLU_TICCBE, "tic-cbe-metering"	,
	},
	{
	W_CLU_TICCJE, "tic-cje-metering"	,
	},
};
static	int	NbCluster	= sizeof(TbCluster) / sizeof(t_zcl_cluster);


static	t_zcl_command	TbCommand[]	=
{
	{
	W_CMD_READ_ATTR_RQST , "read-attribut"	,	0, 
	.cm_id_offset=4, .cm_val_offset=0, .cm_type_offset=0, .cm_stat_offset=0
	, .cm_direction_offset=0
	},
	{
	W_CMD_READ_ATTR_RESP , "read-attribut"	,	1, 
	.cm_id_offset=4, .cm_val_offset=8, .cm_type_offset=7, .cm_stat_offset=6
	, .cm_direction_offset=0
	},


	{
	W_CMD_REPO_CONF_RQST , "configure-reporting"	,	0, 
	.cm_id_offset=5, .cm_val_offset=8, .cm_type_offset=7, .cm_stat_offset=0
	, .cm_direction_offset=4
	},
	{
	W_CMD_REPO_CONF_RESP , "configure-reporting"	,	1, 
	.cm_id_offset=6, .cm_val_offset=0, .cm_type_offset=0, .cm_stat_offset=4
	, .cm_direction_offset=5
	},

	{
	W_CMD_READ_CONF_RQST , "read-configure-reporting"	, 0, 
	.cm_id_offset=5, .cm_val_offset=0, .cm_type_offset=0, .cm_stat_offset=0
	, .cm_direction_offset=4
	},
	{
	W_CMD_READ_CONF_RESP , "read-configure-reporting"	, 1, 
	.cm_id_offset=6, .cm_val_offset=9, .cm_type_offset=8, .cm_stat_offset=4
	, .cm_direction_offset=5
	},

	{
	W_CMD_REPO_ATTR_EVNT , "report-attribut-event"		, 1, 
	.cm_id_offset=4, .cm_val_offset=7, .cm_type_offset=6, .cm_stat_offset=0
	, .cm_direction_offset=0
	},

	{
	W_CMD_WRIT_ATTR_RQST , "write-attribut"	,	0, 
	.cm_id_offset=4, .cm_val_offset=7, .cm_type_offset=6, .cm_stat_offset=0
	, .cm_direction_offset=0
	},

	{
	W_CMD_COMMAND_RQST , "command"	,	0, 
	.cm_id_offset=0, .cm_val_offset=4, .cm_type_offset=0, .cm_stat_offset=0
	, .cm_direction_offset=0
	},
};
static	int	NbCommand	= sizeof(TbCommand) / sizeof(t_zcl_command);



char	*AwZclPowerSourceTxt(unsigned char source)
{
	static	char	buf[128];
	int	lg;

	buf[0]	= '\0';

	if	((source & 0x01) == 0x01)
		strcat(buf,"const&");
	if	((source & 0x02) == 0x02)
		strcat(buf,"rechar&");
	if	((source & 0x04) == 0x04)
		strcat(buf,"dispos&");
	if	((source & 0x08) == 0x08)
		strcat(buf,"solar&");
	if	((source & 0x10) == 0x10)
		strcat(buf,"tic&");
	lg	= strlen(buf);
	if	(lg <= 0)
		return	buf;
	buf[lg-1]	= '\0';
	return	buf;
}


t_zcl_command	*AwZclCommand(unsigned	char cmd)
{
	int	i;

	for	(i = 0 ; i < NbCommand ; i++)
	{
		if	( TbCommand[i].cm_id == cmd )
			return	&TbCommand[i];
	}
	return	NULL;
}

t_zcl_cluster	*AwZclCluster(unsigned short cluster)
{
	int	i;

	for	(i = 0 ; i < NbCluster ; i++)
	{
		if	( TbCluster[i].cl_id == cluster )
			return	&TbCluster[i];
	}
//printf("cluster not found %u in AwZclCluster()\n",cluster);
	return	NULL;
}

t_zcl_attribut	*AwZclAttribut(ushort clid,ushort atid)
{
	int	i;
	t_zcl_attribut	*attribut;
	t_zcl_cluster	*cluster;

	cluster	= AwZclCluster(clid);
	if	(!cluster)
	{
		return	NULL;
	}

//printf("search in cluster %s %d\n",cluster->cl_name,cluster->cl_nb_attr);
	for	(i = 0 ; i < cluster->cl_nb_attr ; i++)
	{
		attribut = cluster->cl_tb_attr[i];
//printf("at_id=%d at_name='%s'\n",attribut->at_id,attribut->at_name);
		if	( attribut->at_id == atid )
			return	attribut;
	}
	return	NULL;
}

void	AwZclInitProto()
{
	static	int	done = 0;
	t_zcl_cluster	*cl;
	t_zcl_attribut	*at;
	int	a;

	if	(done)
		return;

	done	= 1;
	for	( a = 0 ; a < NbAttribut ; a++ )
	{
		at	= &TbAttribut[a];
		cl	= AwZclCluster(at->at_cl_id);
		if	( !cl )
		{
//printf("clusterid=%d not found for attr#=%d\n",at->at_cl_id,a);
			continue;
		}

		if ( cl->cl_nb_attr < W_ZCL_ATTR_MAX )
		{
			cl->cl_tb_attr[cl->cl_nb_attr] = at;
			cl->cl_nb_attr++;
RTL_TRDBG(5,"add attr '%s' in cluster '%s' %d\n",
			at->at_name,cl->cl_name,cl->cl_nb_attr);
		}
	}
}
