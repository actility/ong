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
//** File : ./lib/format_znp_sys.c
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
#include "format_znp_sys.h"

/**
 * @brief Send a SYS_OSAL_NV_READ ZNP message.
 * @param buf the destination buffer controller for ZNP transmission.
 * @param id the NV_READ command identifier.
 * @param offset the address at which starting to write in the register for this NV entry.
 * @return 1 if message has been bufferized successfully, 0 otherwise.
 */
bool znpSendSysNvRead(uartBufControl_t *buf, azap_uint16 id, azap_uint8 offset)
{
  azap_uint8 msg[3];

  msg[0] = LO_UINT16(id);
  msg[1] = HI_UINT16(id);
  msg[2] = offset;

  return znpSend(buf, MT_SYS_NV_READ_SREQ, 3, msg);
}

/**
 * @brief Send a SYS_OSAL_NV_WRITE ZNP message.
 * @param buf the destination buffer controller for ZNP transmission.
 * @param id the NV_WRITE command identifier.
 * @param offset the address at which starting to write in the register for this NV entry.
 * @param len the length of the value to write.
 * @param value the array of bytes to be written.
 * @return 1 if message has been bufferized successfully, 0 otherwise.
 */
bool znpSendSysNvWrite(uartBufControl_t *buf, azap_uint16 id, azap_uint8 offset,
    azap_uint8 len, azap_uint8 *value)
{
  azap_uint8 msg[20]; // The maximum message size for a NV_WRITE

  if (len > 16)
  {
    return false;
  }

  msg[0] = LO_UINT16(id);
  msg[1] = HI_UINT16(id);
  msg[2] = offset;
  msg[3] = len;
  memcpy(&msg[4], value, len);

  return znpSend(buf, MT_SYS_NV_WRITE_SREQ, 4 + len, msg);
}

/**
 * @brief Parse the SYS_RESET_IND payload and extract the ZStack version.
 * @param resetIndPayload the payload of SYS_RESET_IND ZNP message received.
 * @param len the size of the payload received.
 * @param stringVersion the buffer where the string representation of the ZStack version is
 * dumped in.
 * @return 1 if the ZStack version can be retrieved successfully, 0 otherwise.
 */
bool parseSysResetIndVersion(azap_uint8 *resetIndPayload, azap_uint8 len, char *stringVersion)
{
  azap_uint8 transportRev, productId, majorRel, minorRel, hwRev;

  if (! resetIndPayload)
  {
    return false;
  }
  if (! stringVersion)
  {
    return false;
  }
  if (len != 6)
  {
    return false;
  }

  (void)parseUint8AndShift(&resetIndPayload, &len);
  transportRev = parseUint8AndShift(&resetIndPayload, &len);
  productId = parseUint8AndShift(&resetIndPayload, &len);
  majorRel = parseUint8AndShift(&resetIndPayload, &len);
  minorRel = parseUint8AndShift(&resetIndPayload, &len);
  hwRev = parseUint8AndShift(&resetIndPayload, &len);

  sprintf(stringVersion, "%d.%d.%d-%03d_%03d", majorRel, minorRel, hwRev, transportRev,
      productId);

  return true;
}

