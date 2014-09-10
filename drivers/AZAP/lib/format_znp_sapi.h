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
//** File : ./lib/format_znp_sapi.h
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


#ifndef _FORMAT_ZNP_SAPI__H_
#define _FORMAT_ZNP_SAPI__H_

/* SAPI messages family */
#define MT_SAPI_GET_DEVICE_INFO_SREQ                   0x0626
#define MT_SAPI_GET_DEVICE_INFO_SRSP                   0x0666


typedef enum
{
  DEV_INFO_STATE = 0x00,  /**< Device state, 1 byte long (@sa ZDO_DEV_STATUS) */
  DEV_INFO_IEEE_ADDR = 0x01, /**< Device IEEE address, 8 bytes long */
  DEV_INFO_SHORT_ADDR = 0x02, /**< Device short address, 2 bytes */
  DEV_INFO_PEER_SHORT_ADDR = 0x03, /**< Short address of the parent device, 2 bytes long */
  DEV_INFO_PEER_IEEE_ADDR = 0x04, /**< IEEE address of the parent device, 8 bytes long */
  DEV_INFO_RF_CHAN = 0x05, /**< Channel on which the ZigBee network is operating */
  DEV_INFO_PAN_ID = 0x06, /**< PAN ID of the ZigBee network, 2 bytes long */
  DEV_INFO_EXT_PAN_ID = 0x07, /**< Extended PAN Id of the ZigBee network, 8 bytes long */
} DEV_INFO_PARAM;


bool parseSapiFindDeviceConf(
  azap_byte *payload,
  azap_uint8 len,
  azap_uint16 *nwkAddr,
  azap_uint8 *ieeeAddr);

#endif

