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


#define GetTelnetKey()  GetVar("s_telnetkey")
#define GetTelnetPort() GetVar("s_telnetport")

#define GetBoxName()  GetVar("w_boxname")
#define GetDomainName() GetVar("w_domainname")
#define GetNetId()  GetVar("w_netid")
#define GetDriverId() GetVar("w_driverid")
#define GetDriverPath() GetVar("w_driverpath")
#define GetIpuId()  GetVar("w_ipuid")
#define GetCoapAddrL()  GetVar("w_coapaddr_l")
#define GetCoapPortL()  GetVar("w_coapport_l")
#define GetCoapAddrR()  GetVar("w_coapaddr_r")
#define GetCoapPortR()  GetVar("w_coapport_r")

#define ABS(x)    ((x) > 0 ? (x) : -(x))
#define MSTIC   100
#define STRERRNO  strerror(errno)


#define CLI_MAX_FD  32

#define CLI_NFOUND  0
#define CLI_PROMPT  1
#define CLI_DISC  2
#define CLI_KEY   3
#define CLI_EMPTY 4
#define CLI_MORE  5



#define SENSOR_WAIT_RESP_TIMER  (11*60) // sc
#define SENSOR_RESET_DISC_TIMER (60)  // sc

#define ERR_SENSOR_REPO_CONF  10


#define IM_DEF      1000
#define IM_TIMER_GEN    1001
#define         IM_TIMER_GEN_V    10000 // ms

#define IM_LIST_SENSOR_UPDATE 1100
#define IM_LIST_SENSOR_SAME 1101
#define IM_NETWORK_ID_UPDATE  1102


#define IM_SENSOR   2000
#define IM_TIMER_SENSOR   2001
#define         IM_TIMER_SENSOR_V 10000 // ms
#define IM_SENSOR_START   2100
#define IM_SENSOR_RESET   2101
#define IM_SENSOR_DELETE  2102
#define IM_SENSOR_CONFIG_NEXT 2103
#define IM_SENSOR_CONFIG_DONE 2104


#define NOK_DIA     0
#define MAX_REQUEST_DIA   100 // client side

#define CLT_SIDE_DIA    "dia client side"
#define SRV_SIDE_DIA    "dia server side"


#define SCL_ROOT_APP    "/m2m/applications"
#define SCL_ROOT_ACC    "/m2m/accessRights"

#define SCL_IPU     "IPU_"
#define SCL_NET     "NW_"
#define SCL_DEV     "DEV_"
#define SCL_APP     "APP_"

#define METHOD_IPU    1
#define METHOD_NET    2
#define METHOD_DEV    3
#define METHOD_APP    4

#define CZ_MAX  100

