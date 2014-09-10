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
//** File : driver/driver_req_context.c
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


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stddef.h>

#include "rtlbase.h"
#include "rtllist.h"
#include "rtlimsg.h"


#include "xoapipr.h"
#include "dIa.h"

#include "cmndefine.h"
#include "cmnstruct.h"

#include "cmnproto.h"
#include "specextern.h"
#include "adaptor.h"

#include "azap_types.h"

#include "driver_req_context.h"

/*****************************************************************************/
/* Prototypes for local functions */
static void driverReqContext_t_init(driverReqContext_t *This);

/*****************************************************************************/
/* Constructors / destructor */

/* static allocation */

/**
 * @brief Initialize the driverReqContext_t struct
 * @param This the class instance
 */
static void driverReqContext_t_init(driverReqContext_t *This)
{
  RTL_TRDBG(TRACE_DETAIL, "driverReqContext_t::init\n");

  /* Assign the pointers on functions */

  /* Initialize the members */
  This->type = DRV_REQ_UNKNOWN;
  This->serialNum = 0;
  This->nwkAddr = 0;
  This->nbPendingRequests = 0;
  This->issuerData = NULL;
  This->issuerData2 = NULL;
}

/* dynamic allocation */
/**
 * @brief do dynamic allocation of a driverReqContext_t.
 * @param type the request context type
 * @return the copy of the object
 */
driverReqContext_t *new_driverReqContext_t(DRV_REQ_TYPE type)
{
  driverReqContext_t *This = malloc(sizeof(driverReqContext_t));
  if (!This)
  {
    return NULL;
  }
  driverReqContext_t_init(This);
  This->free = driverReqContext_t_newFree;
  This->type = type;
  RTL_TRDBG(TRACE_DETAIL, "new driverReqContext_t (This:0x%.8x) (type:%d)\n",
      This, type);
  return This;
}

/**
 * @brief Destructor for dynamic allocation
 * @anchor driverReqContext_t_newFree
 */
void driverReqContext_t_newFree(driverReqContext_t *This)
{
  RTL_TRDBG(TRACE_DETAIL, "driverReqContext_t::newFree (This:0x%.8x)\n", This);
  free(This);
}

/* implementation methods */





