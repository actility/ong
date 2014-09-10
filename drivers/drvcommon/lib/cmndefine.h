
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

#ifndef _CMNDEFINE_H_
#define _CMNDEFINE_H_

#define	MAX_VAR		256

#define	GetTelnetKey()	GetVar("s_telnetkey")
#define	GetTelnetPort()	GetVar("s_telnetport")

#define	GetBoxName()	GetVar("w_boxname")
#define	GetDomainName()	GetVar("w_domainname")
#define	GetNetId()	GetVar("w_netid")
#define	GetDriverId()	GetVar("w_driverid")
#define	GetDriverPath()	GetVar("w_driverpath")
#define	GetIpuId()	GetVar("w_ipuid")
#define	GetCoapAddrL()	GetVar("w_coapaddr_l")
#define	GetCoapPortL()	GetVar("w_coapport_l")
#define	GetCoapAddrR()	GetVar("w_coapaddr_r")
#define	GetCoapPortR()	GetVar("w_coapport_r")
#define	GetPlcAddr()	GetVar("w_plcaddr")
#define	GetPlcPort()	GetVar("w_plcport")

#define	ABS(x)		((x) > 0 ? (x) : -(x))
#define	MSTIC		100
#define	STRERRNO	strerror(errno)


#define	CLI_MAX_FD	32

#define	CLI_NFOUND	0
#define	CLI_PROMPT	1
#define	CLI_DISC	2
#define	CLI_KEY		3
#define	CLI_EMPTY	4
#define	CLI_MORE	5

#define	NOK_DIA			0
#define	MAX_REQUEST_DIA		100	// client side
#define	INIT_TIMEOUT_DIA	180	// client side before request sent
#define	REQUEST_TIMEOUT_DIA	180	// client side after request sent
#define	MAX_RETRY_DIA		0	// number of retries
/*
 *	MAX_PENDING_DIA must be set to 1 when using lib micro dia
 *	otherwise [1..MAX_REQUEST_DIA]
 */
#define	MAX_PENDING_DIA		1	// client side waiting for response

#define	NUSE_REQUEST_DIA	0	// request not used
#define	INIT_REQUEST_DIA	1	// in use but not sent
#define	SENT_REQUEST_DIA	2	// sent and waiting for response

#define	CLT_SIDE_DIA		"dia client side"
#define	CLT_SIDE_DIA_C		"dia client side/create"
#define	CLT_SIDE_DIA_R		"dia client side/retrieve"
#define	CLT_SIDE_DIA_U		"dia client side/update"
#define	CLT_SIDE_DIA_D		"dia client side/delete"
#define	SRV_SIDE_DIA		"dia server side"

#define	SCL_ROOT_APP		"/m2m/applications"
#define	SCL_ROOT_ACC		"/m2m/accessRights"

#define	SCL_IPU			"IPU_"
#define	SCL_NET			"NW_"
#define	SCL_DEV			"DEV_"
#define	SCL_APP			"APP_"

#define	METHOD_IPU		1	
#define	METHOD_NET		2	
#define	METHOD_DEV		3	
#define	METHOD_APP		4	
#define	METHOD_DRV		5

#define	SRV_REQUEST_TIMEOUT_DIA	900

#define	RQT_UPDATE_ELEM		1	// + update <applications> "elem"
#define	RQT_UPDATE_CONT		2	// + update <containers> "cont"
#define	RQT_NOT_FOUND		128	// not found on retrieve

#define	RQT_ISSET_FLAGS(r,f)		(((r)->rq_flags & (f)) == (f))
#define	RQT_ISSET_UPDATE_ELEM(r)	RQT_ISSET_FLAGS(r,RQT_UPDATE_ELEM)
#define	RQT_ISSET_UPDATE_CONT(r)	RQT_ISSET_FLAGS(r,RQT_UPDATE_CONT)
#define	RQT_ISSET_NOT_FOUND(r)		RQT_ISSET_FLAGS(r,RQT_NOT_FOUND)

#define	RQT_UNSET_FLAGS(r,f)		((r)->rq_flags &= ~(f))
#define	RQT_UNSET_UPDATE_ELEM(r)	RQT_UNSET_FLAGS(r,RQT_UPDATE_ELEM)
#define	RQT_UNSET_UPDATE_CONT(r)	RQT_UNSET_FLAGS(r,RQT_UPDATE_CONT)
#define	RQT_UNSET_NOT_FOUND(r)		RQT_UNSET_FLAGS(r,RQT_NOT_FOUND)

#endif
