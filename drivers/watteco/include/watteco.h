
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

// ############################################################################
//                                            Initialization (once at start) of
//                                            internal descriptions tables for
//    Zcl watteco is coded/encoded with       commands, attributs and clsuters.
//    two C functions and do not allocate     constructed by hand in file     
//    any dynamic memory.                     infoproto.c: AwZclInitProto()     
//    the main structure is the message     +---------------------------------+ 
//    structure t_zcl_msg including         |        t_zcl_attribut []        | 
//    the raw binary message and its C      +---------------------------------+ 
//    form representation.                  +---------------------------------+ 
//    Optionnaly, a message can be encoded  |        t_zcl_cluster []         | 
//    to a memory space outside the C struc.+---------------------------------+ 
//    Incoming messsage can be directly     +---------------------------------+ 
//    mapped (read) in the C struct.        |        t_zcl_command []         | 
//    Optionnaly, a message can be decoded  +---------------------------------+ 
//    from a memory space outside the C struct.                 |              
//    Parameters to ZCL commands are coded/decoded in structs:  |            
//     t_zcl_byteN_value (wip address)                          |              
//     t_zcl_uint4_value (all values on 24-32bits)              |              
//     t_zcl_uint2_value (all values on 16bits)                 |              
//     t_zcl_uint1_value (all values on 8bits)                  |               
//     t_zcl_simple_config (configure reporting Temp,Illu,Occup)|              
//     t_zcl_metering_value (Simple-Metering-current-metering)  |              
//     t_zcl_metering_config (configure reporting Metering)     |
//     t_zcl_simple_desc (actually can not be encoded)          |
//                                                              |               
//                      +---------------------------------+     |               
//                      |        t_zcl_msg                |     |               
//                      +---------------------------------+     |               
//                      |  u_pack field :                 | <---+               
//                +---< |  binary data as received from   | <---+               
//                |     |  captors                        |     |               
//                |     |  (Internal uses only)           |     |               
//                |     +---------------------------------+     |               
//                |     |  m_xxxx field : C representation|     |               
// decoding       |---> |  of common fields of a ZCL msg  | >---| encoding      
// decode.c       |     |  (User part)                    |     | encode.c      
// AwZclDecode()  |     |                                 |     | AwZclEncode() 
//                |     +---------------------------------+     |               
//                |     |  m_param field: C representation|     |               
//                +---> |  of structured parameters       | >---+               
//                      |  depending on command type,     |                     
//                      |  cluster and attribut types     |                     
//                      |  (User part)                    |                     
//                      +---------------------------------+                     
//                                                                             
//   void  AwZclInitProto();                                             
//   int   AwZclDecode(FILE *opttrace,t_zcl_msg *m,uchar *optbin,int optszmax);
//   int   AwZclEncode(t_zcl_msg *mess,uchar *optbin,int optszmax);
// ############################################################################
//                                                                             

/*
 *	ZCL definitions
 */

#define	W_ZCL_CLUSTER_MAX	50	// Actility implementation limitation
#define	W_ZCL_ATTR_MAX		15	// Actility implementation limitation

#define	W_ZCL_SIZE		200	// supposed max size of a zcl message
#define	W_ZCL_TIC_SIZE		170	// max size of TIC binary data
#define	W_ZCL_EPN_MAX		8	// max end point

/* list of commands ID */
#define	W_CMD_READ_ATTR_RQST	0x00	// read attribut request
#define	W_CMD_READ_ATTR_RESP	0x01	// read attribut response

#define	W_CMD_WRIT_ATTR_RQST	0x05	// write attribut request

#define	W_CMD_REPO_CONF_RQST	0x06	// configure-reporting request
#define	W_CMD_REPO_CONF_RESP	0x07	// configure-reporting response

#define	W_CMD_READ_CONF_RQST	0x08	// read configure-reporting request
#define	W_CMD_READ_CONF_RESP	0x09	// read configure-reporting response

#define	W_CMD_REPO_ATTR_EVNT	0x0A	// report attribut response (event)

#define	W_CMD_COMMAND_RQST	0x50	// command request

#define	W_CLAT(cl,at)		((cl << 16) + (at)) // avoid switch of switch

/* list of clusters ID */
/* list of attributs ID */
#define	W_CLU_UNUSED		0xFFFF	// Actility implementation limitation
#define	W_CLU_ONOFF		0x0006
#define				W_ATT_ONOFF_ONOFF		0x0000
#define	W_CLU_SIMPLEMETER	0x0052
#define				W_ATT_SIMPLEMETER_CUR_VALUE	0x0000
#define	W_CLU_TEMPERATURE	0x0402
#define				W_ATT_TEMPERATURE_CUR_VALUE	0x0000
#define				W_ATT_TEMPERATURE_MIN_VALUE	0x0001
#define				W_ATT_TEMPERATURE_MAX_VALUE	0x0002
#define	W_CLU_HUMIDITY		0x0405
#define				W_ATT_HUMIDITY_CUR_VALUE	0x0000
#define				W_ATT_HUMIDITY_MIN_VALUE	0x0001
#define				W_ATT_HUMIDITY_MAX_VALUE	0x0002
#define	W_CLU_OCCUPANCY		0x0406
#define				W_ATT_OCCUPANCY_OCCUPANCY	0x0000
#define				W_ATT_OCCUPANCY_OCCUPANCY_TYPE	0x0001
#define				W_ATT_OCCUPANCY_OCCUPANCY_PIRO	0x0010
#define				W_ATT_OCCUPANCY_OCCUPANCY_PIRU	0x0011
#define	W_CLU_ANALOGINPUT	0x000C
#define				W_ATT_ANALOGINPUT_CUR_VALUE	0x0055
#define				W_ATT_ANALOGINPUT_APP_TYPE	0x0100
#define	W_CLU_BINARYINPUT	0x000F
#define				W_ATT_BINARYINPUT_POLARITY	0x0054
#define				W_ATT_BINARYINPUT_CUR_VALUE	0x0055
#define				W_ATT_BINARYINPUT_APP_TYPE	0x0100
#define				W_ATT_BINARYINPUT_EDGE_SELECT	0x0400
#define				W_ATT_BINARYINPUT_DEBOUNCE	0x0401
#define				W_ATT_BINARYINPUT_COUNTER	0x0402
#define	W_CLU_ILLUMINANCE	0x0400
#define				W_ATT_ILLUMINANCE_CUR_VALUE	0x0000
#define				W_ATT_ILLUMINANCE_MIN_VALUE	0x0001
#define				W_ATT_ILLUMINANCE_MAX_VALUE	0x0002
#define	W_CLU_CONFIGURATION	0x0050
#define				W_ATT_CONFIGURATION_WIPADDR	0x0000
#define				W_ATT_CONFIGURATION_PANID	0x0001
#define				W_ATT_CONFIGURATION_DESC	0x0002
#define				W_ATT_CONFIGURATION_DESCEPN	0x0004
#define				W_ATT_CONFIGURATION_POWER	0x0006
#define	W_CLU_TICICE		0x0053
#define				W_ATT_TICICE_DATA		0x0000
#define				W_ATT_TICICE_PER0		0x0001
#define				W_ATT_TICICE_PER1		0x0002
#define				W_ATT_TICICE_TYPE		0x0010
#define	W_CLU_TICCBE		0x0054
#define				W_ATT_TICCBE_DATA		0x0000
#define				W_ATT_TICCBE_TYPE		0x0010
#define	W_CLU_TICCJE		0x0055
#define				W_ATT_TICCJE_DATA		0x0000
#define				W_ATT_TICCJE_TYPE		0x0010


// ****************************************************************************
//
//	structures to describe zcl binary messages
//	helpful for decoding, initialized by calling AwZclInitProto()
//
// ****************************************************************************


#ifdef	ANDROID
#define	ushort	unsigned short
#endif
typedef	struct
{
	ushort		at_cl_id;
	ushort		at_id;
	char		*at_name;
	unsigned char	at_type; // ZCL type 0x29(temp), ...
	signed int	at_ssval; // 1,2 or 4 bytes in t_zcl_uintX_value
				  // if < 0 size of bytes in t_zcl_byteN_value
				  // if == 0 antoher t_zcl_xxxxx
}	t_zcl_attribut;

typedef	struct
{
	ushort		cl_id;
	char		*cl_name;
	short		cl_nb_attr;
	t_zcl_attribut	*cl_tb_attr[W_ZCL_ATTR_MAX];
}	t_zcl_cluster;

typedef	struct
{
	unsigned char	cm_id;
	char		*cm_name;
	unsigned char	cm_response;		// 0 request, 1 response

	unsigned char	cm_id_offset;		// offset atid if present
	unsigned char	cm_val_offset;		// offset value if present 
	unsigned char	cm_type_offset;		// offset attype if present
	unsigned char	cm_stat_offset;		// offset status if present
	unsigned char	cm_direction_offset;	// offset direction if present
	
}	t_zcl_command;

#define	STRUCTPACKED	__attribute__ ((packed))

// ****************************************************************************
// structures for commands parameters are included in u_param union
// where all fields are in "host" format and used to interface binary message
// and C structures
// here STRUCTPACKED is not mandatory
// ****************************************************************************

// parameters for read/write attributs
struct	s_zcl_byteN_value
{
	unsigned	char	m_valsz;
	unsigned	char	m_value[W_ZCL_SIZE];
}	STRUCTPACKED;
typedef	struct s_zcl_byteN_value	t_zcl_byteN_value;

struct	s_zcl_uint1_value
{
	unsigned	char	m_value;
}	STRUCTPACKED;
typedef	struct s_zcl_uint1_value	t_zcl_uint1_value;

struct	s_zcl_uint2_value
{
	unsigned	short	m_value;
}	STRUCTPACKED;
typedef	struct s_zcl_uint2_value	t_zcl_uint2_value;

struct	s_zcl_uint4_value
{
	unsigned	int	m_value;
}	STRUCTPACKED;
typedef	struct s_zcl_uint4_value	t_zcl_uint4_value;

typedef	struct s_zcl_power_value
{
	unsigned	char	m_sz;
	unsigned	char	m_mode;
	unsigned	char	m_source;
	unsigned	char	m_count;	// #elem in source/level
	unsigned	short	m_level[10];
}	STRUCTPACKED	t_zcl_power_value;

typedef	struct
{
	unsigned	char	m_metersz;
	unsigned	int	m_sumWh;	// 3 bytes only
	unsigned	int	m_sumVarh;	// 3 bytes only
	unsigned	short	m_count;	// number of samples
	unsigned	short	m_curWh;
	unsigned	short	m_curVarh;
}	STRUCTPACKED	t_zcl_metering_value;

typedef	struct
{
	unsigned	char	m_nbclin;
	unsigned	short	m_clin[10];
	unsigned	char	m_epin[10];
	unsigned	char	m_nbclout;
	unsigned	short	m_clout[10];
	unsigned	char	m_epout[10];
	unsigned	char	m_nbep;
}	STRUCTPACKED	t_zcl_simple_desc;

typedef	struct
{
	unsigned	char	m_sz;
	unsigned	char	m_data[W_ZCL_TIC_SIZE];
}	STRUCTPACKED	t_zcl_tic_data;

#define	m_min_change	u.uim_min_change
// parameters for read/write reportings
struct	s_zcl_simple_config		// simple reporting configuration
					// here simple does mean simple-metering
{
	unsigned	short	m_min_time;
	unsigned	short	m_max_time;
#if	0
	unsigned	int	m_min_change; // depends on cluster 1,2,4 bytes
					      // or float simple precision
#endif
	union
	{
		unsigned	int	uim_min_change;
		float			ufm_min_change;
		
	}	u;
}	STRUCTPACKED;

typedef	struct s_zcl_simple_config	t_zcl_simple_config,
					t_zcl_onoff_config,
					t_zcl_temp_config,
					t_zcl_humi_config,
					t_zcl_occup_config,
					t_zcl_ainput_config,
					t_zcl_binput_config,
					t_zcl_illu_config;

typedef	struct
{
	unsigned	short	m_min_time;
	unsigned	short	m_max_time;
	unsigned	char	m_metersz;
	unsigned	int	m_sumWh;	// 3 bytes only
	unsigned	int	m_sumVarh;	// 3 bytes only
	unsigned	short	m_count;	// number of samples
	unsigned	short	m_curWh;
	unsigned	short	m_curVarh;
	// TODO use t_zcl_metering_value ???
}	STRUCTPACKED	t_zcl_metering_config;

typedef	struct
{
	unsigned	short	m_min_time;
	unsigned	short	m_max_time;
	unsigned	char	m_sz;
	unsigned	char	m_data[W_ZCL_TIC_SIZE];
}	STRUCTPACKED	t_zcl_tic_config;

typedef	struct s_zcl_power_config
{
	unsigned	short	m_min_time;
	unsigned	short	m_max_time;
	unsigned	char	m_mode;
	unsigned	char	m_source;
	unsigned	short	m_level[10];
}	STRUCTPACKED	t_zcl_power_config;

// ****************************************************************************
// structures for commands are included in u_pack 
// where all fields are in "network" format
// here STRUCTPACKED is mandatory
// these structure are not really useful, easy access with msg.m_xxxx
// ****************************************************************************

#define	W_COMMON_HEADER	\
	unsigned	char	m_flag;\
	unsigned	char	m_cmd;\
	unsigned	short	m_cluster_id;

typedef	struct	// common header
{
	W_COMMON_HEADER;
}	STRUCTPACKED 	t_zcl_head;

typedef	struct	// W_CMD_READ_ATTR_RQST	"read-attribut"
{
	W_COMMON_HEADER;
	unsigned	short	m_attr_id;
}	STRUCTPACKED 	t_zcl_read_attr_rqst;

typedef	struct	// W_CMD_READ_ATTR_RESP	"read-attribut"
{
	W_COMMON_HEADER;
	unsigned	short	m_attr_id;
	unsigned	char	m_status;
	unsigned	char	m_attr_type;
}	STRUCTPACKED	t_zcl_read_attr_resp;

typedef	struct	// W_CMD_REPO_CONF_RQST	"configure-reporting"
{
	W_COMMON_HEADER;
	unsigned	char	m_direction;
	unsigned	short	m_attr_id;
	unsigned	char	m_attr_type;
}	STRUCTPACKED	t_zcl_repo_conf_rqst;

typedef	struct	// W_CMD_REPO_CONF_RESP	"configure-reporting"
{
	W_COMMON_HEADER;
	unsigned	char	m_status;
	unsigned	char	m_direction;
	unsigned	short	m_attr_id;
}	STRUCTPACKED	t_zcl_repo_conf_resp;

typedef	struct	// W_CMD_READ_CONF_RQST	"read-configure-reporting"
{
	W_COMMON_HEADER;
	unsigned	char	m_direction;
	unsigned	short	m_attr_id;
}	STRUCTPACKED	t_zcl_read_conf_rqst;


typedef	struct	// W_CMD_READ_CONF_RESP	"read-configure-reporting"
{
	W_COMMON_HEADER;
	unsigned	char	m_status;
	unsigned	char	m_direction;
	unsigned	short	m_attr_id;
	unsigned	char	m_attr_type;
}	STRUCTPACKED	t_zcl_read_conf_resp;

typedef	struct	// W_CMD_WRIT_ATTR_RQST
{
	W_COMMON_HEADER;
	unsigned	short	m_attr_id;
	unsigned	char	m_attr_type;
}	STRUCTPACKED 	t_zcl_writ_attr_rqst;


typedef	struct	// W_CMD_COMMAND_RQST	
{
	W_COMMON_HEADER;
}	STRUCTPACKED	t_zcl_command_rqst;	// simple command

typedef	struct	// W_CMD_REPO_ATTR_EVNT	=> event
{
	W_COMMON_HEADER;
	unsigned	short	m_attr_id;
	unsigned	char	m_attr_type;
}	STRUCTPACKED	t_zcl_repo_attr_resp;	// event

typedef	struct
{
	unsigned	char	m_byte[W_ZCL_SIZE];
}	STRUCTPACKED	t_zcl_data;


// ****************************************************************************
// ZCL message structure
//	- u_pack  : binary message internal use only
//	- m_*     : user part
//	- u_param : user part 
// ****************************************************************************
typedef	struct
{
	// u_pack directly mapped on binary message
	// all fields in u_pack are in "network" format
	// binary message is stored in m_data
	// access to these fields are not necessary to use the API
	union
	{
		t_zcl_head 		m_head;

		t_zcl_read_attr_rqst	m_read_attr_rqst;
		t_zcl_read_attr_resp	m_read_attr_resp;

		t_zcl_repo_conf_rqst	m_repo_conf_rqst;
		t_zcl_repo_conf_resp	m_repo_conf_resp;

		t_zcl_read_conf_rqst	m_read_conf_rqst;
		t_zcl_read_conf_resp	m_read_conf_resp;

		t_zcl_writ_attr_rqst 	m_writ_attr_rqst;
		t_zcl_repo_attr_resp	m_repo_attr_resp;
		t_zcl_data		m_data;
	}	u_pack;


	// now all fields are in "host" format for developper
	// they are setted after decoding and must be setted before encoding
	unsigned	char	m_epn;		// end point number
	unsigned	char	m_cmd;
	unsigned	char	m_status;
	unsigned	char	m_direction;
	unsigned	short	m_cluster_id;
	unsigned	short	m_attr_id;
	unsigned	int	m_size;		// encoded size
	unsigned	char	m_xxcode; 	// D decoded , E encoded
	unsigned	char	m_cmd_value;	// for W_CMD_COMMAND_RQST

	// type of union u_param is gived by W_CLAT(m_cluster_id,m_attr_id)
	// all fields are in "host" format for developper
	// they are setted after decoding and must be setted before encoding
	union
	{
		t_zcl_byteN_value	m_byteN_value;
		t_zcl_uint4_value	m_uint4_value;
		t_zcl_uint2_value	m_uint2_value;
		t_zcl_uint1_value	m_uint1_value;

		t_zcl_metering_value	m_metering_value;

		t_zcl_simple_config	m_simple_config;
		t_zcl_onoff_config	m_onoff_config;
		t_zcl_temp_config	m_temp_config;
		t_zcl_humi_config	m_humi_config;
		t_zcl_occup_config	m_occup_config;
		t_zcl_ainput_config	m_ainput_config;
		t_zcl_binput_config	m_binput_config;
		t_zcl_illu_config	m_illu_config;

		t_zcl_metering_config	m_metering_config;
		t_zcl_tic_config	m_tic_config;
		t_zcl_power_config	m_power_config;

		t_zcl_simple_desc	m_simple_desc;

		t_zcl_power_value	m_power_value;

		t_zcl_tic_data		m_tic_data;
	}	u_param;

}	STRUCTPACKED	t_zcl_msg;


/*
#define	AW_FPRINTF(f,...) { if (f) { fprintf(f,__VA_ARGS__); fflush(f); } }
 */
#define	AW_SPRINTF(b,...) { if (b) { AwSprintf(b,__VA_ARGS__); } }

void		AwZclInitProto();
int		AwZclEncode(t_zcl_msg *mess,unsigned char *bin,int szmax);
int		AwZclDecode(char *buf,t_zcl_msg *m,unsigned char *bin,int sz);

char	*AwZclPowerSourceTxt(unsigned char source);

t_zcl_command	*AwZclCommand(unsigned	char cmd);
t_zcl_cluster	*AwZclCluster(unsigned short cluster);
t_zcl_attribut	*AwZclAttribut(ushort clid,ushort atid);


