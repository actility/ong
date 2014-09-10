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
//** File : ./include/azap_tools.h
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


#ifndef _AZAP_TOOLS__H_
#define _AZAP_TOOLS__H_

#define STR_MAX_SIZE 255


char *azap_uint8ToString(azap_uint8 data, bool hexprefix);
char *azap_uint16ToString(azap_uint16 data, bool hexprefix);
char *azap_extAddrToString(azap_uint8 *data, bool hexprefix);
char *azap_byteArrayToString(azap_byte *data, azap_uint16 len);
char *azap_uint8ArrayToString(azap_uint8 *array, azap_uint16 len);
char *azap_uint16ArrayToString(azap_uint16 *array, azap_uint16 len);

bool stringToAzap_extAddr(azap_uint8 *extAddr, char *strRep);
azap_uint16 stringToAzap_byteArray(azap_byte *data, azap_uint16 len, char *strRep);

bool isExtAddrNull(azap_uint8 *extAddr);

#endif

