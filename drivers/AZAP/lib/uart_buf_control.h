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
//** File : ./lib/uart_buf_control.h
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


#ifndef _UART_BUF_CONTROL__H_
#define _UART_BUF_CONTROL__H_

/**
 * @file uart_buf_control.h
 * @brief This file brings the definition of the object used for UART flow control management.
 * @see uartBufControl_t
 * @author mlouiset
 * @date 2012-07-06
 */

/*
 *  ___       __  __                      _           _
 * | _ )_  _ / _|/ _|___ _ _   __ ___ _ _| |_ _ _ ___| |
 * | _ \ || |  _|  _/ -_) '_| / _/ _ \ ' \  _| '_/ _ \ |
 * |___/\_,_|_| |_| \___|_|   \__\___/_||_\__|_| \___/_|
 *
 */

/**
 * @typedef znpMsgState_t
 * Defines the different parsing steps for ZNP messages.
 */
typedef enum
{
  ST_WAITING_SOP = 0,
  ST_WAITING_LEN,
  ST_WAITING_CMD0,
  ST_WAITING_CMD1,
  ST_WAITING_DATA_AND_FCS,
} znpMsgState_t;


/**
 * @struct uartBufControl_t
 * This structure allow manipulating buffered characters.
 * Typical usage is:<br>
 * <ul><li> to put read characters from poll, and get ZNP message to be processed on
 * periodic main loop tick,</li>
 * <li> to put ZNP messages to be sent, and write on poll when TTY ready.</li></ul>
 * @author ML
 * @date 2012-07-04
 */
typedef struct uartBufControl_t
{
  /* methods */
  /** @ref uartBufControl_t_getBufferedSize */
  azap_uint16 (*getBufferedSize) (struct uartBufControl_t *This);
  /** @ref uartBufControl_t_put */
  azap_uint16 (*put) (struct uartBufControl_t *This, azap_uint16 len, azap_uint8 *pData);
  /** @ref uartBufControl_t_get */
  azap_uint16 (*get) (struct uartBufControl_t *This, azap_uint16 len, azap_uint8 *pData);
  /** @ref uartBufControl_t_getNextZNPMsg */
  bool (*getNextZNPMsg) (struct uartBufControl_t *This, azap_uint8 *len, azap_uint8 *cmd0,
      azap_uint8 *cmd1, azap_uint8 *pMsgData);
  /** @ref uartBufControl_t_free */
  void (*free) (struct uartBufControl_t *);

  /* members */
  /** The reading current position in this buffer */
  volatile azap_uint16 bufHead;
  /** The writing current position in this buffer (the last byte to read is just before). */
  volatile azap_uint16 bufTail;
  /** The size of the circular buffer */
  azap_uint16 bufSize;
  /** Pointer on the buffer */
  azap_uint8 *pBuffer;

  /** State of the currently parsed ZNP message */
  znpMsgState_t statePending;
  /** CMD0 byte value of the currently parsed ZNP message */
  azap_uint8 cmd0Pending;
  /** CMD1 byte value of the currently parsed ZNP message */
  azap_uint8 cmd1Pending;
  /** LEN byte value of the currently parsed ZNP message */
  azap_uint8 lenPending;
  /** time of the last read bytes */
  time_t startDate;

} uartBufControl_t;

/* static allocation */
uartBufControl_t uartBufControl_t_create(azap_uint16 size);
void uartBufControl_t_free(uartBufControl_t *This);

/* dynamic allocation */
uartBufControl_t *new_uartBufControl_t(azap_uint16 size);
void uartBufControl_t_newFree(uartBufControl_t *This);

/* implementation methods */
azap_uint16 uartBufControl_t_getBufferedSize(uartBufControl_t *This);
azap_uint16 uartBufControl_t_put(uartBufControl_t *This, azap_uint16 len, azap_uint8 *pData);
azap_uint16 uartBufControl_t_get(struct uartBufControl_t *This, azap_uint16 len, azap_uint8 *pData);
bool uartBufControl_t_getNextZNPMsg(uartBufControl_t *This, azap_uint8 *len, azap_uint8 *cmd0,
    azap_uint8 *cmd1, azap_uint8 *pMsgData);


#endif


