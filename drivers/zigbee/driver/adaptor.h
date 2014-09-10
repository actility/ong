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
//** File : driver/adaptor.h
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


#ifndef _ZB_ADAPTOR__H_
#define _ZB_ADAPTOR__H_

#ifdef __NO_TRACE__
#undef RTL_TRDBG
#define RTL_TRDBG(lev,...)
#endif


#define IM_M2M_ABSTR_TIMERS 1000
#define IM_TIMER_OPEN_ASSOC_RESUME 1001

#define ADAPTOR_NAME  "zigbee"

typedef void (*pf_print)(t_cli *cl, char *fmt, ...);

char *GetAdaptorVersion();
int DoSerial();
int AdmCmd(t_cli *cl, char *pbuf);
void AdmCliFree(t_cli *cl);

#endif

