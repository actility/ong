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
//** File : ./lib/uart_buf_control.c
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
#include "azap_api_common.h"
#include "azap_api_af.h"
#include "azap_api_zcl.h"
#include "azap_tools.h"
#include "uart_buf_control.h"
#include "format_znp.h"

/*
 * Local definitions
 */
#define MAX_DEBUG_BUF_SIZE 255

/*****************************************************************************/
/* Prototypes for local functions */
static void uartBufControl_t_init(uartBufControl_t *This);

static bool isFcsValid(azap_uint8 len, azap_uint8 cmd0, azap_uint8 cmd1, azap_uint8 *pData,
    azap_uint8 fcs);
#ifndef __NO_TRACE__
static char *dumpPacket(azap_uint8 len, azap_uint8 cmd0, azap_uint8 cmd1,
    azap_uint8 *pData, azap_uint8 fcs);
#endif


/*
 *  ___       __  __                      _           _
 * | _ )_  _ / _|/ _|___ _ _   __ ___ _ _| |_ _ _ ___| |
 * | _ \ || |  _|  _/ -_) '_| / _/ _ \ ' \  _| '_/ _ \ |
 * |___/\_,_|_| |_| \___|_|   \__\___/_||_\__|_| \___/_|
 *
 */


/*****************************************************************************/
/* Constructors / destructor */

/* static allocation */

/**
 * @brief Initialize the uartBufControl_t struct
 * @param This the class instance
 */
static void uartBufControl_t_init(uartBufControl_t *This)
{
  RTL_TRDBG(TRACE_DETAIL, "uartBufControl_t::init\n");

  /* Assign the pointers on functions */
  This->getNextZNPMsg = uartBufControl_t_getNextZNPMsg;
  This->put = uartBufControl_t_put;
  This->get = uartBufControl_t_get;
  This->getBufferedSize = uartBufControl_t_getBufferedSize;

  /* Initialize the members */
  This->bufHead = 0;
  This->bufTail = 0;
  This->bufSize = 0;
  This->pBuffer = NULL;

  This->statePending = ST_WAITING_SOP;
  This->startDate = 0;
  This->cmd0Pending = 0;
  This->cmd1Pending = 0;
  This->lenPending = 0;
}

/**
 * @brief do static allocation of a uartBufControl_t.
 * @param size the size of the buffer control.
 * @return the copy of the object
 */
uartBufControl_t uartBufControl_t_create(azap_uint16 size)
{
  uartBufControl_t This;
  uartBufControl_t_init(&This);
  This.free = uartBufControl_t_free;
  // one more byte to prevent to fall in a "tail over head" error case
  This.bufSize = size + 1;
  This.pBuffer = (azap_uint8 *) malloc(This.bufSize * sizeof(azap_uint8));
  RTL_TRDBG(TRACE_DETAIL, "uartBufControl_t::create (This:0x%.8x) (size:%d)\n", &This, size);
  return This;
}

/**
 * @brief Destructor for static allocation
 */
void uartBufControl_t_free(uartBufControl_t *This)
{
  RTL_TRDBG(TRACE_DETAIL, "uartBufControl_t::free (This:0x%.8x) (size:%d)\n", This, This->bufSize);
  if (This->pBuffer)
  {
    free(This->pBuffer);
  }
}

/* dynamic allocation */
/**
 * @brief do dynamic allocation of a uartBufControl_t.
 * @param size the size of the buffer control.
 * @return the copy of the object
 */
uartBufControl_t *new_uartBufControl_t(azap_uint16 size)
{
  uartBufControl_t *This = malloc(sizeof(uartBufControl_t));
  if (!This)
  {
    return NULL;
  }
  uartBufControl_t_init(This);
  This->free = uartBufControl_t_newFree;
  // one more byte to prevent to fall in a "tail over head" error case
  This->bufSize = size + 1;
  This->pBuffer = (azap_uint8 *) malloc(This->bufSize * sizeof(azap_uint8));
  RTL_TRDBG(TRACE_DETAIL, "new uartBufControl_t (This:0x%.8x) (size:%d)\n", This, size);
  return This;
}

/**
 * @brief Destructor for dynamic allocation
 */
void uartBufControl_t_newFree(uartBufControl_t *This)
{
  RTL_TRDBG(TRACE_DETAIL, "uartBufControl_t::newFree (This:0x%.8x) (size:%d)\n", This,
      This->bufSize);
  if ((This) && (This->pBuffer))
  {
    free(This->pBuffer);
  }
  free(This);
}

/* implementation methods */

/**
 * @brief Put ZNP message in buffer ready for rx/tx transmission.
 * @param This the instance of the object
 * @param len the length of the ZNP message
 * @param pData buffer that conveys the ZNP message
 * @return 1 if the message can be put successfully, 0 otherwise.
 */
azap_uint16 uartBufControl_t_put(uartBufControl_t *This, azap_uint16 len, azap_uint8 *pData)
{
  azap_uint16 res = 0;

  RTL_TRDBG(TRACE_DETAIL, "uartBufControl_t::put (This:0x%.8x) (size:%d) (len:%d) "
      "(h/t:%d/%d)\n", This, This->bufSize, len, This->bufHead, This->bufTail);
  /* put all or none */
  if ((pData) && (len) && (This->bufSize - This->getBufferedSize(This) - 1 >= len ))
  {
    RTL_TRDBG(TRACE_IMPL, "uartBufControl_t::put ok (This:0x%.8x) (len:%d) (packet:%s)\n",
        This, len, azap_uint8ArrayToString(pData, len));
//    RTL_TRDBG(TRACE_INFO, "uartBufControl_t::put ok (This:0x%.8x) (len:%d) (packet:%s)\n",
//      This, len, azap_uint8ArrayToString(pData, len));
    res = len;
    if ((This->bufHead > This->bufTail) || (This->bufSize - This->bufTail >= len))
    {
      /* the easiest case, no need to return at buffer beginning */
      memcpy(&(This->pBuffer[This->bufTail]), pData, len);
      This->bufTail += len;
      // bufTail has reach the end of the buffer
      if (This->bufTail == This->bufSize)
      {
        This->bufTail = 0;
      }
    }
    else
    {
      /* need to do it in two phases */
      azap_uint16 l = This->bufSize - This->bufTail;
      memcpy(&(This->pBuffer[This->bufTail]), pData, l);
      pData += l;
      memcpy(&(This->pBuffer[0]), pData, len - l);
      This->bufTail = len - l;
    }
  }
  return res;
}

/**
 * @brief Retrieve the number of buffered characters that are ready to be processed.
 * @anchor uartBufControl_t_getBufferedSize
 * @param This the instance of the object
 * @return the total number of buffered characters
 */
azap_uint16 uartBufControl_t_getBufferedSize(uartBufControl_t *This)
{
  int res = This->bufTail;
  res -= This->bufHead;

  if (res < 0)
  {
    /* modulo the buffer size */
    res += This->bufSize;
  }

//  RTL_TRDBG(TRACE_FULL, "uartBufControl_t::getBufferedSize (This:0x%.8x) (res:%d)\n", This,
//    res);

  return (azap_uint16)res;
}

/**
 * @brief Get the next buffered characters.
 * @param This the instance of the object
 * @param len the number of characters to be read
 * @param pData buffer that conveys the ZNP message
 * @return the length of read characters
 */
azap_uint16 uartBufControl_t_get(struct uartBufControl_t *This, azap_uint16 len,
    azap_uint8 *pData)
{
  azap_uint16 res = 0;

  RTL_TRDBG(TRACE_DETAIL, "uartBufControl_t::get (This:0x%.8x) (size:%d) "
      "(len:%d) (h/t:%d/%d)\n", This, This->bufSize, len, This->bufHead,
      This->bufTail);
  // get all at once or don't return anything
  if ((pData) && (len) && (This) && (This->getBufferedSize(This) >= len))
  {
    RTL_TRDBG(TRACE_DETAIL, "uartBufControl_t::get ok (This:0x%.8x) (len:%d)\n",
        This, len);
    res = len;
    if ((This->bufTail > This->bufHead) || (This->bufSize - This->bufHead >= len))
    {
      /* the easiest way */
      memcpy(pData, &(This->pBuffer[This->bufHead]), len);
      This->bufHead += len;
      // bufHead has reach the end of the buffer
      if (This->bufHead == This->bufSize)
      {
        This->bufHead = 0;
      }
    }
    else /* head > tail */
    {
      azap_uint16 l = This->bufSize - This->bufHead;
      memcpy(pData, &(This->pBuffer[This->bufHead]), l);
      pData += l;
      memcpy(pData, &(This->pBuffer[0]), len - l);
      This->bufHead = len - l;
    }
  }
  return res;
}

/**
 * @brief Get the next ZNP message in buffer, ready to be processed.
 * @param This the instance of the object
 * @param len the length of the ZNP message
 * @param cmd0 byte #0 of ZNP command; indicates the command layer family (APS, NWK, ...)
 * as well as the synchronous/asynchronous flag (SREQ/SRSP or AREQ/ARSP)
 * @param cmd1 byte #1 of ZNP command; indicates the command in the layer family.
 * @param pMsgData buffer that conveys the ZNP message
 * @return 1 if a message is available and can be retrieved successfully, 0 otherwise.
 */
bool uartBufControl_t_getNextZNPMsg(uartBufControl_t *This, azap_uint8 *len, azap_uint8 *cmd0,
    azap_uint8 *cmd1, azap_uint8 *pMsgData)
{
  bool res = false;
  bool stop = false;
  azap_uint8 readByte;
  azap_uint16 bytesAvail;

//  RTL_TRDBG(TRACE_FULL, "uartBufControl_t::getNextZNPMsg (This:0x%.8x) (state:%d) "
//    "(h/t:%d/%d)\n", This, This->statePending, This->bufHead, This->bufTail);

  /*
   * First, look if there is a pending request than was too long to process
   * and if any, reset the parsing pseudo-automate
   */
  if (This->statePending != ST_WAITING_SOP)
  {
    time_t now = rtl_tmms();
    if (now - This->startDate >= 5000)
    {
      // too long to process the pending packet... give it up
      RTL_TRDBG(TRACE_ERROR, "uartBufControl_t::getNextZNPMsg - unable to get packet within 5s "
          "(This:0x%.8x) (state:%d) (len:%d) (cmd0:0x%.2x) (cmd1:0x%.2x) (bufferedSize:%d)\n", This,
          This->statePending, This->lenPending, This->cmd0Pending, This->cmd1Pending,
          This->getBufferedSize(This));
      This->statePending = ST_WAITING_SOP;
    }
  }

  /*
   * Look for bufferized data, if there is enough to build a ZNP message
   */
  if (pMsgData)
  {
    while ( (bytesAvail = This->getBufferedSize(This)) && (!stop) )
    {
      /* There are things to read ! */
      switch (This->statePending)
      {
        case ST_WAITING_SOP:
        {
          This->get(This, 1, &readByte);
          if (MT_UART_SOF == readByte)
          {
            This->startDate = rtl_tmms();
            This->statePending = ST_WAITING_LEN;
          }
          else
          {
            // other expected return ? 0x07 ?
            // In this case *len = 0 ? and *cmd0 = 0 and *cmd1 = readByte
          }
        }
        break;

        case ST_WAITING_LEN:
        {
          This->get(This, 1, &This->lenPending);
          This->statePending = ST_WAITING_CMD0;
        }
        break;

        case ST_WAITING_CMD0:
        {
          This->get(This, 1, &This->cmd0Pending);
          This->statePending = ST_WAITING_CMD1;
        }
        break;

        case ST_WAITING_CMD1:
        {
          This->get(This, 1, &This->cmd1Pending);
          This->statePending = ST_WAITING_DATA_AND_FCS;
        }
        break;

        case ST_WAITING_DATA_AND_FCS:
        {
          // We wait for having the full data buffer and the FCS to complete
          if ((bytesAvail >= This->lenPending + 1) &&
              (This->get(This, This->lenPending, pMsgData) == This->lenPending) &&
              (This->get(This, 1, &readByte) == 1))
          {
            *cmd0 = This->cmd0Pending;
            *cmd1 = This->cmd1Pending;
            *len = This->lenPending;
            res = true;

            // check FCS validity
            if (! isFcsValid(This->lenPending, This->cmd0Pending, This->cmd1Pending, pMsgData,
                readByte ))
            {
              // wrong FCS !
              RTL_TRDBG(TRACE_ERROR, "uartBufControl_t::getNextZNPMsg - wrong FCS (This:0x%.8x) "
                  "(packet:%s)\n", This, dumpPacket(This->lenPending, This->cmd0Pending,
                      This->cmd1Pending, pMsgData, readByte ));
            }
            else
            {
              RTL_TRDBG(TRACE_IMPL, "uartBufControl_t::getNextZNPMsg - ok (This:0x%.8x) "
                  "(packet:%s)\n", This, dumpPacket(This->lenPending,
                      This->cmd0Pending, This->cmd1Pending, pMsgData, readByte ));
//              RTL_TRDBG(TRACE_INFO, "uartBufControl_t::getNextZNPMsg - ok (This:0x%.8x) "
//                "(packet:%s)\n", This, dumpPacket(This->lenPending, This->cmd0Pending,
//                This->cmd1Pending, pMsgData, readByte ));
            }

            // reset state for next message
            This->statePending = ST_WAITING_SOP;
          }
          stop = true;
        }
        break;

        default:
          // what ?!
          break;
      }
    }
  }
  return res;
}





/*  _                 _    __              _   _
 * | |   ___  __ __ _| |  / _|_  _ _ _  __| |_(_)___ _ _  ___
 * | |__/ _ \/ _/ _` | | |  _| || | ' \/ _|  _| / _ \ ' \(_-<
 * |____\___/\__\__,_|_| |_|  \_,_|_||_\__|\__|_\___/_||_/__/
 *
 */

/**
 * @brief Check if provided FCS is valid or not.
 * @param len the received payload length
 * @param cmd0 the value of the cmd0 byte (see ZNP specs)
 * @param cmd1 the value of the cmd1 byte (see ZNP specs)
 * @param pData the packet payload
 * @param fcs the value of the FCS
 * @return true if FCS is valid, false otherwise.
 */
static bool isFcsValid(azap_uint8 len, azap_uint8 cmd0, azap_uint8 cmd1, azap_uint8 *pData,
    azap_uint8 fcs)
{
  azap_uint8 i;
  azap_uint8 f = len ^ cmd0 ^ cmd1;
  for (i = 0; i < len; i++)
  {
    f ^= pData[i];
  }
  return (bool) ( f == fcs );
}

#ifndef __NO_TRACE__
/**
 * @brief Build a string representation of a ZNP packet
 * @param len the length of the payload
 * @param cmd0 the value of the cmd0 byte (see ZNP specs)
 * @param cmd1 the value of the cmd1 byte (see ZNP specs)
 * @param pData the payload
 * @param fcs the value of the FCS
 * @return the built string representation
 */
static char *dumpPacket(azap_uint8 len, azap_uint8 cmd0, azap_uint8 cmd1,
    azap_uint8 *pData, azap_uint8 fcs)
{
  azap_uint8 i;
  short remainLen = MAX_DEBUG_BUF_SIZE - 1;
  char szTemp[8];
  static char packet[MAX_DEBUG_BUF_SIZE];
  memset(packet, 0, MAX_DEBUG_BUF_SIZE);

  // The packet header
  sprintf(packet, "0x%.2x 0x%.2x 0x%.2x 0x%.2x ", MT_UART_SOF, len, cmd0, cmd1);
  remainLen -= 20;

  // The payload
  for (i = 0; i < len; i++)
  {
    if (remainLen >= 5)
    {
      remainLen -= 5;
      sprintf(szTemp, "0x%.2x ", pData[i]);
      strcat(packet, szTemp);
    }
    else if (remainLen >= 3)
    {
      // not enough space remaining in the buffer
      remainLen -= 3;
      strcat(packet, "...");
      break;
    }
  }

  // and the FCS
  if (remainLen >= 5)
  {
    sprintf(szTemp, "0x%.2x ", fcs);
    strcat(packet, szTemp);
  }
  else if (remainLen >= 3)
  {
    // not enough space remaining in the buffer
    strcat(packet, "...");
  }

  return packet;
}
#endif

