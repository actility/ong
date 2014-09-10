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
//** File : ./lib/format_znp_sys.h
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
 * @file format_znp_sys.h
 * @brief Defines tool functions for SYS interface.
 */
#ifndef _FORMAT_ZNP_SYS__H_
#define _FORMAT_ZNP_SYS__H_

/* SYS messages family */
#define MT_SYS_SOFT_RESET_AREQ                0x0041
#define MT_SYS_RESET_IND                      0x8041
#define MT_SYS_VERSION_SREQ                   0x0221
#define MT_SYS_VERSION_SRSP                   0x0241
#define MT_SYS_NV_READ_SREQ                   0x0821
#define MT_SYS_NV_READ_SRSP                   0x0861
#define MT_SYS_NV_WRITE_SREQ                  0x0921
#define MT_SYS_NV_WRITE_SRSP                  0x0961


bool znpSendSysNvRead(uartBufControl_t *buf, azap_uint16 id, azap_uint8 offset);
bool znpSendSysNvWrite(uartBufControl_t *buf, azap_uint16 id, azap_uint8 offset,
    azap_uint8 len, azap_uint8 *value);
bool parseSysResetIndVersion(azap_uint8 *resetIndPayload, azap_uint8 len,
    char *stringVersion);

#endif


