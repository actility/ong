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
//** File : ./lib/postponed_request_elts.c
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
 * @file postponed_request_elts.c
 * @brief This object allows storing parameters for ZigBee pending requests.
 * It is used for requestManager_t.
 * @author mlouiset
 * @date 2012-08-28
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <rtlbase.h>

#include "azap_types.h"
#include "azap_api_zcl.h"

#include "postponed_request_elts.h"

/*****************************************************************************/
/* Prototypes for local functions */
static void postponedRequestElts_t_init(postponedRequestElts_t *This);

/*****************************************************************************/
/* Constructors / destructor */

/* static allocation */

/**
 * @brief Initialize the postponedRequestElts_t struct
 * @param This the class instance
 */
static void postponedRequestElts_t_init(postponedRequestElts_t *This)
{
  RTL_TRDBG(TRACE_DETAIL, "postponedRequestElts_t::init\n");

  /* Assign the pointers on functions */

  /* Initialize the members */
  This->responseExpected = false;
  This->localEndp = 0;
  This->defaultResponse = false;
  This->cmdCode = 0;
  This->cmdData = NULL;
  This->dataLen = 0;

}

/* dynamic allocation */
/**
 * @brief do dynamic allocation of a postponedRequestElts_t.
 * @param needResponse specifies if a response is expected for this request.
 * @return the copy of the object
 */
postponedRequestElts_t *new_postponedRequestElts_t(bool needResponse)
{
  postponedRequestElts_t *This = malloc(sizeof(postponedRequestElts_t));
  if (!This)
  {
    return NULL;
  }
  postponedRequestElts_t_init(This);
  This->free = postponedRequestElts_t_newFree;
  This->responseExpected = needResponse;
  RTL_TRDBG(TRACE_DETAIL, "new postponedRequestElts_t (This:0x%.8x)\n", This);
  return This;
}

/**
 * @brief Destructor for dynamic allocation
 * @anchor postponedRequestElts_t_newFree
 * @param This the class instance
 */
void postponedRequestElts_t_newFree(postponedRequestElts_t *This)
{
  RTL_TRDBG(TRACE_DETAIL, "postponedRequestElts_t::newFree (This:0x%.8x)\n", This);
  if (This->cmdData)
  {
    free(This->cmdData);
  }
  free(This);
}

