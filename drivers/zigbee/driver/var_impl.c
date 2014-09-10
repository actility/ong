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
//** File : driver/var_impl.c
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

/*! @file var_impl.c
 *
 */
#include <stdio.h>      /************************************/
#include <stdlib.h>     /*     run in main thread           */
#include <string.h>     /************************************/
#include <unistd.h>
#include <stddef.h>

#include "rtlbase.h"
#include "rtlimsg.h"
#include "xoapipr.h"
#include "dIa.h"

#include "cmndefine.h"
#include "cmnstruct.h"
#include "cmnproto.h"
#include "adaptor.h"

#include "var_impl.h"

int NbVar;
int InternalVar;
t_var TbVar[MAX_VAR] =
{
// v_name   v_default   v_fct
  { "s_telnetkey", "123", NULL, },
  { "s_telnetport", "2001", NULL, },
  { "s_adaptor", NULL, GetAdaptorName, },
  { "s_envvar", NULL, GetEnvVarName, },
  { "s_rootapp", SCL_ROOT_APP, NULL, },
  { "s_rootacc", SCL_ROOT_ACC, NULL, },
  { "w_manufacturer", "", NULL, },
  { "w_boxname", "CYH-00FADAFADAFA", NULL, },
  { "w_domainname", "actility.com", NULL, },
  { "w_driverid", "GIP", NULL, },
  { "w_driverpath", "zigbee", NULL, },
  { "w_ipuid", "IPU_ZigBee", NULL, },
  { "w_coapaddr_l", "::1", NULL, },
  { "w_coapport_l", "5685", NULL, },
  { "w_coapaddr_r", "::1", NULL, },
  { "w_coapport_r", "5683", NULL, },
  { "w_rfChan", "11", NULL, },
  { "w_panid", "0000", NULL, },
  { "w_hostname", NULL, GetHostName, },
  { "w_hosttarget", NULL, GetHostTarget, },
  { "w_plcaddr", "", NULL, },
  { "w_plcport", "", NULL, },
  { "w_plctarget", NULL, GetPlcTarget, },
  { "w_reqid", NULL, GetReqId, },
  { "w_netid", NULL, NULL, },
  { "w_gwcontact", NULL, GetGwContact, },
  { "w_drvcontact", NULL, GetDrvContact, },
  { "w_date", NULL, GetDate, },
  { "w_time", NULL, GetTime, },
  { "w_template", NULL, NULL, },
  { "d_ieee", NULL, NULL, },
  { "a_num", "1", NULL, },
  { "c_num", NULL, NULL, },
  { "r_value", NULL, NULL, },
  { "w_errStatus", NULL, NULL, },
  { "w_errAddInfo", NULL, NULL, },
  { "w_tpkdev", "", NULL, },
  { "w_nsclbaseuri", "http://nsc1.actility.com:8088/m2m", NULL, },
  { "w_scl_httpport", "8080", NULL, },
};


char  *GetDate()
{
  static  char  val[128];

  InternalVar = 1;
  rtl_aaaammjj(0, val);
  InternalVar = 0;
  return  val;
}

char  *GetTime()
{
  static  char  val[128];

  InternalVar = 1;
  rtl_hhmmss(0, val);
  InternalVar = 0;
  return  val;
}

char  *GetAdaptorName()
{
  static  char  val[128];

  InternalVar = 1;
  sprintf (val, "%s", ADAPTOR_NAME);
  InternalVar = 0;
  return  val;
}

char  *GetEnvVarName()
{
  static  char  val[128];

  InternalVar = 1;
  sprintf (val, "%s", DRIVER_DATA_DIR);
  InternalVar = 0;
  return  val;
}

char  *GetGwContact()
{
  static  char  val[128];

  InternalVar = 1;
  sprintf (val, "%s:%s", AddBrackets(GetCoapAddrR()), GetCoapPortR());
  InternalVar = 0;
  return  val;
}

char  *GetDrvContact()
{
  static  char  val[128];

  InternalVar = 1;
  sprintf (val, "%s:%s", AddBrackets(GetCoapAddrL()), GetCoapPortL());
  InternalVar = 0;
  return  val;
}

char  *GetHostName()
{
  static  char  val[128];

  InternalVar = 1;
  sprintf (val, "%s.%s", GetBoxName(), GetDomainName());
  InternalVar = 0;
  return  val;
}

char  *GetHostTarget()
{
  static  char  val[128];

  InternalVar = 1;
//  sprintf (val, "%s", "coap://localhost");
  sprintf (val, "coap://%s:%s", AddBrackets(GetCoapAddrR()), GetCoapPortR());
  InternalVar = 0;
  return  val;
}

char *GetReqId() // requesting identity
{
  static char val[256];

  InternalVar = 1;
  sprintf (val, "coap://%s:%s%s/%s", GetHostName(), GetCoapPortR(), SCL_ROOT_APP,
      GetDriverId());
  InternalVar = 0;
  return  val;
}


char *GetPlcTarget()
{
  static char val[128];
  char *plcaddr;
  char *plcport;

  InternalVar = 1;
  plcaddr = GetPlcAddr();
  plcport = GetPlcPort();
  if (plcaddr && *plcaddr && plcport && *plcport)
  {
    sprintf(val, "coap://%s:%s", AddBrackets(plcaddr), plcport);
  }
  else
  {
    val[0]  = '\0';
  }
  InternalVar = 0;
  return val;
}

