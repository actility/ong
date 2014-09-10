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

/*! @file var.c
 *
 */
#include <stdio.h>      /************************************/
#include <stdlib.h>     /*     run in main thread           */
#include <string.h>     /************************************/
#include <unistd.h>
#include <pthread.h>
#include <errno.h>
#include <limits.h>
#include <signal.h>
#include <poll.h>
#include <ctype.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#include "rtlbase.h"
#include "rtlimsg.h"
#include "xoapipr.h"
#include "_whatstr.h"

#include "adaptor.h"

#include "define.h"
#include "cmnstruct.h"

#include "cproto.h"
#include "cmnproto.h"
#include "speccproto.h"
#include "cmndefine.h"



#define MAX_VAR 256

// v_name   v_default   v_fct
t_var TbVar[MAX_VAR] =
{

  // Telnet configuration
  {"s_telnetkey",         "123",          NULL,           NULL},
  {"s_telnetport",        "2002",     NULL,           NULL},


  {
    "s_adaptor" , "wmbus",      GetAdaptorName,
  },

  {
    "s_rootapp" , SCL_ROOT_APP,   NULL,
  },

  {
    "s_rootacc" , SCL_ROOT_ACC,   NULL,
  },

  {
    "w_manufacturer", "1234",     NULL,
  },

  {
    "w_boxname" , "CYH-00FADAFADAFA", NULL,
  },

  {
    "w_domainname"  , "actility.com",   NULL,
  },

  {
    "w_driverid"  , "GIP",      NULL,
  },

  {
    "w_driverpath"  , "wmbus",    NULL,
  },

  {
    "w_ipuid" , "IPU_WMBus",    NULL,
  },

  {
    "w_coapaddr_l"  , "::1",      NULL,
  },

  {
    "w_coapport_l"  , "5686",     NULL,
  },

  {
    "w_coapaddr_r"  , "::1",      NULL,
  },

  {
    "w_coapport_r"  , "5683",     NULL,
  },

  {
    "w_panid" , "0000",     NULL,
  },

  {
    "w_hostname"  , NULL,     GetHostName,
  },

  {
    "w_hosttarget"  , NULL,     GetHostTarget,
  },

  {
    "w_plcaddr" , "",     NULL,
  },

  {
    "w_plcport" , "",     NULL,
  },

  {
    "w_plctarget" , NULL,     GetPlcTarget,
  },

  {
    "w_reqid" , NULL,     GetReqId,
  },

  {
    "w_gwcontact" , NULL,     GetGwContact,
  },

  {
    "w_drvcontact"  , NULL,     GetDrvContact,
  },

  {
    "w_date"  , NULL,     GetDate,
  },

  {
    "w_time"  , NULL,     GetTime,
  },

  {
    "w_template"  , NULL,     NULL,
  },

  {
    "d_ieee"  , NULL,     NULL,
  },

  {
    "a_num"   , "114",      NULL,
  },

  {
    "c_num"   , "",     NULL,
  },

  {
    "r_value" , NULL,     NULL,
  },

  {
    "w_tpkdev"  , "",     NULL,
  },

  {
    "w_nsclbaseuri", "http://nsc1.actility.com:8088/m2m", NULL,
  },

  {
    "w_scl_httpport", "8080", NULL,
  },

  // wMBus extra matadata
  {"w_device_tty",     "/dev/ttyUSB0",   NULL,           NULL},
  {"w_tty_prefix",     "ttyUSB",         NULL,           NULL},
  {"w_dongle_product", "AMB8465",        NULL,           NULL},
  {"w_tty_mapper",     "bin/tty_mapper", NULL,           NULL},
  {"w_mode",           "T2-other",       NULL,           NULL},
  {"w_rfch",           "11",             NULL,           NULL},
  {"w_dongle_mid",     "AMB",            NULL,           NULL},
  {"w_netid",          NULL,             NULL,           NULL},
  {"w_rfpwr",          "+10dBm",         NULL,           NULL},

  // Cuurent device metadata
  {"w_id",            "",       NULL,           NULL},
  {"w_mid",           "",       NULL,           NULL},
  {"w_version",       "",       NULL,           NULL},
  {"w_type",          "",       NULL,           NULL},



};

int NbVar;

int InternalVar;

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

char  *GetAdaptorVersion()
{
  return wmbus_whatStr;
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
  sprintf (val, "coap://%s:%s", AddBrackets(GetCoapAddrR()), GetCoapPortR());
  InternalVar = 0;
  return  val;
}

char  *GetReqId() // requesting identity
{
  static  char  val[256];

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

