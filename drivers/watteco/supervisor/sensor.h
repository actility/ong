
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


#define	cp_self		cp_cmn.cmn_self
#define	cp_num		cp_cmn.cmn_num
#define	cp_serial	cp_cmn.cmn_serial
#define	cp_ieee		cp_cmn.cmn_ieee
#define	cp_dia_devok	cp_cmn.cmn_dia_devok
#define	cp_dia_appok	cp_cmn.cmn_dia_appok
#define	cp_dia_datok	cp_cmn.cmn_dia_datok
#define	cp_dia_metok	cp_cmn.cmn_dia_metok
#define	cp_dia_accok	cp_cmn.cmn_dia_accok
#define	cp_dia_repok	cp_cmn.cmn_dia_repok
#define	cp_dia_reperr	cp_cmn.cmn_dia_reperr

// a sensor/sensor/device ...
typedef	struct
{
	t_cmn_sensor	cp_cmn;		// common members

	int		cp_error;
	char		*cp_nicename;
	char		*cp_addr;
	float		cp_multiplyvalue;
	float		cp_addvalue;
	unsigned short	cp_panid;
	unsigned char	cp_softw;	// software sensors 0 et 17 interdits
	unsigned char	cp_simulated;	// simulated sensors
	unsigned char	cp_ctxtLoaded;
	unsigned char	cp_dia_deleting;	// dia delete running
	unsigned char	cp_andforget;

	int		cp_state;
	time_t		cp_stateAt;
	int		cp_stateP;
	int		cp_startNoDelay;
	time_t		cp_startAt;

	t_zcl_simple_desc cp_desc;	// clusters description
	int		cp_toCfg;	// num cluster to configure
	int		cp_nbCfg;	// # clusters to configure
	int		cp_configured;	// well configured if != 0
	int		cp_configLock;	// do not auto config
	int		cp_implictCluster;// number of implicit cluster

	int		cp_nbItf;	// interfaces found in template
	int		cp_tbItf[MAX_ITF_PER_SSR];// interfaces found (cluster)
	int		cp_epItf[MAX_ITF_PER_SSR];// interfaces found (cluster)

	int		cp_nbCov;
	t_cov_to_zcl	*cp_tbCov[CZ_MAX_PER_SS];
	int		cp_epCov[CZ_MAX_PER_SS];
	int		cp_minMaxCov;
	int		cp_minMinCov;

	int		cp_nbDat;	// interf/data found in template
	int		cp_nbMet;	// interf/method found in template


	// zcl infos & counters
	char		cp_zcl_waitResp;
	unsigned int	cp_zcl_waitClat;
	time_t		cp_zcl_waitTime;
	time_t		cp_zcl_sendAt;
	time_t		cp_zcl_recvAt;
	time_t		cp_zcl_lastAt;
	unsigned int	cp_zcl_sendcount;	// spv -> sensor
	unsigned int	cp_zcl_recvcount;	// sensor -> spv
	unsigned int	cp_zcl_cmndcount;	// spv -> sensor
	unsigned int	cp_zcl_respcount;	// sensor -> spv response cmds
	unsigned int	cp_zcl_evntcount;	// sensor -> spv report events
	unsigned int	cp_zcl_rerrcount;	// sensor -> spv errors
	unsigned int	cp_zcl_methcount;	// spv -> sensor (no wait ack)
	unsigned int	cp_zcl_readcount;	// spv -> sensor (wait resp)
	unsigned int	cp_zcl_writcount;	// spv -> sensor (no wait ack)
	unsigned int	cp_zcl_readTid;
	unsigned int	cp_zcl_readClat;
	time_t		cp_zcl_readAt;
	t_zcl_power_value cp_power;	// clusters description
	
	// infos depending on sensor type
	int		cp_stateOnOff;	// -1,0,1 unk,off,on
	int		cp_ticType;	// type of TIC counter
}	t_sensor;
