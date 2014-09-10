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
//** File : ./lib/format_znp.h
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


/**
 * @file format_znp.h
 * @brief Defines utils for ZNP packet manipulations and common ZNP packets.
 */
#ifndef _FORMAT_ZNP__H_
#define _FORMAT_ZNP__H_

/**
 * @def MAX_ZNP_PAYLOAD_SIZE
 * @brief a ZNP payload cannot exceed 255 bytes.
 */
#define MAX_ZNP_PAYLOAD_SIZE 255

#define MT_UART_SOF 0xFE

/* takes a byte out of a uint32 : var - uint32,  ByteNum - byte to take out (0 - 3) */
#define BREAK_UINT32( var, ByteNum ) \
          (azap_uint8)((azap_uint32)(((var) >>((ByteNum) * 8)) & 0x00FF))

#define BUILD_UINT32(Byte0, Byte1, Byte2, Byte3) \
          ((azap_uint32)((azap_uint32)((Byte0) & 0x00FF) \
          + ((azap_uint32)((Byte1) & 0x00FF) << 8) \
          + ((azap_uint32)((Byte2) & 0x00FF) << 16) \
          + ((azap_uint32)((Byte3) & 0x00FF) << 24)))

#define BUILD_UINT16(loByte, hiByte) \
          ((azap_uint16)(((loByte) & 0x00FF) + (((hiByte) & 0x00FF) << 8)))

#define HI_UINT16(a) (((a) >> 8) & 0xFF)
#define LO_UINT16(a) ((a) & 0xFF)

#define BUILD_UINT8(hiByte, loByte) \
          ((azap_uint8)(((loByte) & 0x0F) + (((hiByte) & 0x0F) << 4)))

#define HI_UINT8(a) (((a) >> 4) & 0x0F)
#define LO_UINT8(a) ((a) & 0x0F)


/* Prototypes */

/* Data parsing methods */
azap_uint8 parseUint8AndShift(azap_byte **dataPtr, azap_uint8 *remainingLen);
azap_uint8 *parseUint8ArrayAndShift(azap_byte **dataPtr, azap_uint8 num,
    azap_uint8 *remainingLen);
azap_uint16 parseUint16AndShift(azap_byte **dataPtr, azap_uint8 *remainingLen);
azap_uint16 *parseUint16ArrayAndShift(azap_byte **dataPtr, azap_uint8 num,
    azap_uint8 *remainingLen);
azap_uint32 parseUint32AndShift(azap_byte **dataPtr, azap_uint8 *remainingLen);
void parseExtAddrAndShift(azap_uint8 *extAddr, azap_byte **dataPtr,
    azap_uint8 *remainingLen);

/* Data formatting methods */
void azap_uint8CpyPayloadAndShift(azap_uint8 val, azap_byte **dataPtr);
void azap_uint8ArrayCpyPayloadAndShift(azap_uint8 *val, azap_uint8 num,
    azap_byte **dataPtr);
void azap_uint16CpyPayloadAndShift(azap_uint16 val, azap_byte **dataPtr);
void azap_uint16ArrayCpyPayloadAndShift(azap_uint16 *val, azap_uint8 num,
    azap_byte **dataPtr);
void azap_extAddrCpyPayloadAndShift(azap_uint8 *val, azap_byte **dataPtr);



bool znpSend(uartBufControl_t *buf, azap_uint16 cmd, azap_uint8 len,
    azap_uint8 *payload);

#endif


