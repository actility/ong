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

//************************************************************************
//************************************************************************
//**
//** File : ./lib/format_znp_zdo.c
//**
//** Description  :
//**
//**
//** Date : 11/19/2012
//** Author :  Mathias Louiset, Actility (c), 2012.
//** Modified by :
//**
//** Task :
//**
//** REVISION HISTORY:
//**
//**  Date      Version   Name
//**  --------    -------   ------------
//**
//************************************************************************
//************************************************************************


#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <rtlbase.h>
#include "azap_types.h"
#include "azap_tools.h"
#include "azap_api_common.h"
#include "azap_api_af.h"
#include "azap_api_zcl.h"
#include "uart_buf_control.h"
#include "format_znp.h"
#include "format_znp_zdo.h"
/**
 * @brief Send a ZDO_MSG_CB_REGISTER ZNP message to register a call back on ZDO cluster.
 * @param buf the destination buffer controller for ZNP transmission.
 * @param id the ZDO cluster identifier, defining the ZDO message type.
 * @return 1 if message has been bufferized successfully, 0 otherwise.
 */
bool znpSendZdoMsgCbRegister(uartBufControl_t *buf, azap_uint16 id)
{
  azap_uint8 msg[2]; // The maximum message size for a ZDO_MSG_CB_REGISTER
  msg[0] = LO_UINT16(id);
  msg[1] = HI_UINT16(id);
  return znpSend(buf, MT_ZDO_MSG_CB_REGISTER_SREQ, 2, msg);
}




