
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

#include	"cmndefine.h"

#define	SERVICE_STATUS_FILE	"/tmp/awspv.status.test"

#define	DIA_KO()	(DiaIpuOk == 0 || DiaNetOk == 0)

#define	CZ_MAX		(W_ZCL_CLUSTER_MAX * W_ZCL_ATTR_MAX)
#define	CZ_MAX_PER_SS	(3 * W_ZCL_ATTR_MAX)


#define	SENSOR_WAIT_RESP_TIMER	(11*60)	// sc
#define	SENSOR_RESET_DISC_TIMER	(60)	// sc

#define	ERR_SENSOR_REPO_CONF	10


#define	IM_DEF			1000
#define	IM_TIMER_GEN		1001
#define					IM_TIMER_GEN_V		10000	// ms

#define	IM_LIST_SENSOR_UPDATE	1100
#define	IM_LIST_SENSOR_SAME	1101
#define	IM_NETWORK_ID_UPDATE	1102
#define	IM_SERVICE_STATUS_RQST	1103


#define	IM_SENSOR		2000
#define	IM_TIMER_SENSOR		2001
#define					IM_TIMER_SENSOR_V	10000	// ms
#define	IM_SENSOR_START		2100
#define	IM_SENSOR_RESET		2101
#define	IM_SENSOR_DELETE	2102
#define	IM_SENSOR_CONFIG_NEXT	2103
#define	IM_SENSOR_CONFIG_DONE	2104
#define	IM_SENSOR_CONFIG_STOP	2105



