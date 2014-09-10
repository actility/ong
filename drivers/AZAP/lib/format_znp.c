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
//** File : ./lib/format_znp.c
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

/*   ___             _             _            _
 *  | _ \__ _ _ _ __(_)_ _  __ _  | |_ ___  ___| |___
 *  |  _/ _` | '_(_-< | ' \/ _` | |  _/ _ \/ _ \ (_-<
 *  |_| \__,_|_| /__/_|_||_\__, |  \__\___/\___/_/__/
 *                         |___/
 */

/**
 * @brief Parse the first byte as an azap_uint8 and shift pointer of azap_uint8 size.
 * WARNING: parsing method assumes taht the payload encoding is big endian.
 * @param data pointer on message payload, at the position the azap_uint8 is supposed to be.
 * @param remainingLen number of byte remaining to parse in the payload.
 * @return the parsed azap_uint8 value.
 */
azap_uint8 parseUint8AndShift(azap_byte **data, azap_uint8 *remainingLen)
{
  if (!data)
  {
    return 0;
  }
  if (*remainingLen < 1)
  {
    return 0;
  }

  azap_uint8 res = (*data)[0];
  (*data)++;
  (*remainingLen) -= 1;

  return res;
}

/**
 * @brief Parse the two first bytes as an azap_uint16 and shift pointer of azap_uint16 size.
 * @param data pointer on message payload, at the position the azap_uint16 is supposed to be.
 * @param remainingLen number of byte remaining to parse in the payload.
 * @return the parsed azap_uint16 value.
 */
azap_uint16 parseUint16AndShift(azap_byte **data, azap_uint8 *remainingLen)
{
  if (!data)
  {
    return 0;
  }
  if (*remainingLen < 2)
  {
    return 0;
  }

  azap_uint16 res = (azap_uint16)(0x00FF & (*data)[0]);
  res += (azap_uint16)((0x00FF & (*data)[1]) << 8);

  (*data) += 2;
  (*remainingLen) -= 2;

  return res;
}

/**
 * @brief Parse the four first bytes as an azap_uint32 and shift pointer of azap_uint32 size.
 * @param data pointer on message payload, at the position the azap_uint32 is supposed to be.
 * @param remainingLen number of byte remaining to parse in the payload.
 * @return the parsed azap_uint32 value.
 */
azap_uint32 parseUint32AndShift(azap_byte **data, azap_uint8 *remainingLen)
{
  if (!data)
  {
    return 0;
  }
  if (*remainingLen < 4)
  {
    return 0;
  }

  azap_uint16 res = BUILD_UINT32((*data)[0], (*data)[1], (*data)[2], (*data)[3]);

  (*data) += 4;
  (*remainingLen) -= 4;

  return res;
}

/**
 * @brief Parse the next 8 byte of data as an extended address and store the result in exAddr.
 * @param extAddr the buffer where the parsed address is copied in.
 * @param data pointer on message payload, at the position the extended address is supposed
 * to be.
 * @param remainingLen number of byte remaining to parse in the payload.
 */
void parseExtAddrAndShift(azap_uint8 *extAddr, azap_byte **data, azap_uint8 *remainingLen)
{
  if (!data)
  {
    return;
  }
  if (!extAddr)
  {
    return;
  }
  if (*remainingLen < 8)
  {
    return;
  }

  memcpy(extAddr, *data, IEEE_ADDR_LEN);
  (*data) += IEEE_ADDR_LEN;
  (*remainingLen) -= IEEE_ADDR_LEN;
}

/**
 * @brief Parse the next bytes of data as array of azap_uint8, array of num entries.
 * @param data pointer on message payload, at the position the azap_uint8 array is supposed
 * to be.
 * @param num number of entries in array to parse.
 * @param remainingLen number of byte remaining to parse in the payload.
 * @return the array, allocated with "malloc" primitive. The user has to free the resources
 * after usage.
 */
azap_uint8 *parseUint8ArrayAndShift(azap_byte **data, azap_uint8 num,
    azap_uint8 *remainingLen)
{
  if (0 == num)
  {
    return NULL;
  }
  if (! data)
  {
    return NULL;
  }
  if (*remainingLen < (num * sizeof(azap_uint8)))
  {
    return NULL;
  }

  azap_uint8 i;
  azap_uint8 *res = malloc(num * sizeof(azap_uint8));
  for (i = 0; i < num; i++)
  {
    res[i] = parseUint8AndShift(data, remainingLen);
  }

  return res;
}

/**
 * @brief Parse the next bytes of data as array of azap_uint16, array of num entries.
 * @param data pointer on message payload, at the position the azap_uint16 array is supposed
 * to be.
 * @param num number of entries in array to parse.
 * @param remainingLen number of byte remaining to parse in the payload.
 * @return the array, allocated with "malloc" primitive. The user has to free the resources
 * after usage.
 */
azap_uint16 *parseUint16ArrayAndShift(azap_byte **data, azap_uint8 num,
    azap_uint8 *remainingLen)
{
  if (0 == num)
  {
    return NULL;
  }
  if (! data)
  {
    return NULL;
  }
  if (*remainingLen < (num * sizeof(azap_uint16)))
  {
    return NULL;
  }

  azap_uint8 i;
  azap_uint16 *res = malloc(num * sizeof(azap_uint16));
  for (i = 0; i < num; i++)
  {
    res[i] = parseUint16AndShift(data, remainingLen);
  }

  return res;
}



/*    ___                   _   _   _             _            _
 *   | __|__ _ _ _ __  __ _| |_| |_(_)_ _  __ _  | |_ ___  ___| |___
 *   | _/ _ \ '_| '  \/ _` |  _|  _| | ' \/ _` | |  _/ _ \/ _ \ (_-<
 *   |_|\___/_| |_|_|_\__,_|\__|\__|_|_||_\__, |  \__\___/\___/_/__/
 *                                        |___/
 */

/**
 * @brief Copy the azap_uint8 into the buffer and shift the pointer of 1.
 * @param val the value to copy into the buffer.
 * @param dataPtr the pointer on the buffer to fill in with the value.
 */
void azap_uint8CpyPayloadAndShift(azap_uint8 val, azap_byte **dataPtr)
{
  if (!dataPtr)
  {
    return;
  }
  if (!(*dataPtr))
  {
    return;
  }

  *dataPtr[0] = val;
  (*dataPtr)++;
}

/**
 * @brief Copy the array of azap_uint8 into the buffer and shift the pointer of num.
 * @param val the value to copy into the buffer.
 * @param num number of elements in the array.
 * @param dataPtr the pointer on the buffer to fill in with the value.
 */
void azap_uint8ArrayCpyPayloadAndShift(azap_uint8 *val, azap_uint8 num, azap_byte **dataPtr)
{
  if (!dataPtr)
  {
    return;
  }
  if (!(*dataPtr))
  {
    return;
  }
  if (!val)
  {
    return;
  }
  if (0 == num)
  {
    return;
  }

  memcpy(*dataPtr, val, num);
  (*dataPtr) += num;
}

/**
 * @brief Copy the azap_uint16 into the buffer and shift the pointer of 2.
 * @param val the value to copy into the buffer.
 * @param dataPtr the pointer on the buffer to fill in with the value.
 */
void azap_uint16CpyPayloadAndShift(azap_uint16 val, azap_byte **dataPtr)
{
  if (!dataPtr)
  {
    return;
  }
  if (!(*dataPtr))
  {
    return;
  }

  (*dataPtr)[0] = LO_UINT16(val);
  (*dataPtr)[1] = HI_UINT16(val);
  (*dataPtr) += 2;
}

/**
 * @brief Copy the array of azap_uint16 into the buffer and shift the pointer of num * 2.
 * @param val the value to copy into the buffer.
 * @param num number of elements in the array.
 * @param dataPtr the pointer on the buffer to fill in with the value.
 */
void azap_uint16ArrayCpyPayloadAndShift(azap_uint16 *val, azap_uint8 num, azap_byte **dataPtr)
{
  if (!dataPtr)
  {
    return;
  }
  if (!(*dataPtr))
  {
    return;
  }
  if (!val)
  {
    return;
  }
  if (0 == num)
  {
    return;
  }

  azap_uint8 i;
  for (i = 0; i < num; i++)
  {
    azap_uint16CpyPayloadAndShift(val[i], dataPtr);
  }
}

/**
 * @brief Copy the IEEE address into the buffer and shift the pointer of the size of the value.
 * @param val the value to copy into the buffer.
 * @param dataPtr the pointer on the buffer to fill in with the value.
 */
void azap_extAddrCpyPayloadAndShift(azap_uint8 *val, azap_byte **dataPtr)
{
  if (!dataPtr)
  {
    return;
  }
  if (!(*dataPtr))
  {
    return;
  }
  if (!val)
  {
    return;
  }

  memcpy(*dataPtr, val, IEEE_ADDR_LEN);
  (*dataPtr) += IEEE_ADDR_LEN;
}


/**
 * @brief Build a ZNP message and bufferize it ready to be sent.
 * @param buf the destination buffer controller for ZNP transmission.
 * @param cmd the ZNP command to be sent.
 * @param len the size of the payload.
 * @param payload the ZNP message payload.
 * @return 1 if message has been bufferized successfully, 0 otherwise.
 */
bool znpSend(uartBufControl_t *buf, azap_uint16 cmd, azap_uint8 len, azap_uint8 *payload)
{
  azap_uint8 msg[MAX_ZNP_PAYLOAD_SIZE];
  azap_uint8 fcs = 0;
  azap_uint8 i;

  if (len > MAX_ZNP_PAYLOAD_SIZE - 5)
  {
    return false;
  }

  msg[0] = MT_UART_SOF;
  msg[1] = len;
  msg[2] = LO_UINT16(cmd);
  msg[3] = HI_UINT16(cmd);

  if (len > 0)
  {
    memcpy(&msg[4], payload, len);
  }

  for (i = 1; i < len + 4; i++)
  {
    fcs ^= msg[i];
  }

  msg[ len + 4 ] = fcs;

  return buf->put(buf, len + 5, msg);
}


