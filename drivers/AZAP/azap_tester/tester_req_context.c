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
//** File : ./azap_tester/tester_req_context.c
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

#include "azap_types.h"

#include "tester_req_context.h"

/*****************************************************************************/
/* Prototypes for local functions */
static void testerReqContext_t_init(testerReqContext_t *This);

/*****************************************************************************/
/* Constructors / destructor */

/* static allocation */

/**
 * @brief Initialize the testerReqContext_t struct
 * @param This the class instance
 */
static void testerReqContext_t_init(testerReqContext_t *This)
{
  RTL_TRDBG(TRACE_DETAIL, "testerReqContext_t::init\n");

  /* Assign the pointers on functions */

  /* Initialize the members */
  This->type = TSTER_REQ_UNKNOWN;
  This->nwkAddr = 0;
  This->nbPendingRequests = 0;
  This->clusterId = 0;
  This->srcEP = 0;
  This->dstEP = 0;
  This->issuerData = NULL;
}

/* dynamic allocation */
/**
 * @brief do dynamic allocation of a testerReqContext_t.
 * @param type the request context type
 * @return the copy of the object
 */
testerReqContext_t *new_testerReqContext_t(TSTER_REQ_TYPE type)
{
  testerReqContext_t *This = malloc(sizeof(testerReqContext_t));
  if (!This)
  {
    return NULL;
  }
  testerReqContext_t_init(This);
  This->free = testerReqContext_t_newFree;
  This->type = type;
  RTL_TRDBG(TRACE_DETAIL, "new testerReqContext_t (This:0x%.8x) (type:%d)\n",
      This, type);
  return This;
}

/**
 * @brief Destructor for dynamic allocation
 * @anchor testerReqContext_t_newFree
 */
void testerReqContext_t_newFree(testerReqContext_t *This)
{
  RTL_TRDBG(TRACE_DETAIL, "testerReqContext_t::newFree (This:0x%.8x)\n", This);
  free(This);
}

/* implementation methods */





