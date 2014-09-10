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
//** File : ./lib/azap_attribute.c
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

#include <rtlbase.h>

#include "azap_types.h"

#include "azap_zcl_attr_val.h"
#include "azap_attribute.h"

/*****************************************************************************/
/* Prototypes for local functions */
static void azapAttribute_t_init(azapAttribute_t *This);

/*****************************************************************************/
/* Constructors / destructor */

/* static allocation */

/**
 * @brief Initialize the azapAttribute_t struct
 * @param This the class instance
 */
static void azapAttribute_t_init(azapAttribute_t *This)
{
  RTL_TRDBG(TRACE_DETAIL, "azapAttribute_t::init\n");

  /* Assign the pointers on functions */

  /* Initialize the members */
  This->clusterId = 0;
  This->attrId = 0;
  This->manufacturerCode = 0;
  This->value = NULL;
}

/* dynamic allocation */
/**
 * @brief do dynamic allocation of a azapAttribute_t.
 * @param clId  the cluster identifier
 * @param atId  the attribute identifier
 * @return the copy of the object
 */
azapAttribute_t *new_azapAttribute_t(azap_uint16 clId, azap_uint16 atId)
{
  azapAttribute_t *This = malloc(sizeof(azapAttribute_t));
  if (!This)
  {
    return NULL;
  }
  azapAttribute_t_init(This);
  This->free = azapAttribute_t_newFree;

  This->clusterId = clId;
  This->attrId = atId;

  RTL_TRDBG(TRACE_DETAIL, "new azapAttribute_t (id:0x%.4x.0x%.4x)\n", clId, atId);
  return This;
}

/**
 * @brief Destructor for dynamic allocation
 * @anchor azapAttribute_t_newFree
 */
void azapAttribute_t_newFree(azapAttribute_t *This)
{
  RTL_TRDBG(TRACE_DETAIL, "azapAttribute_t::newFree (id:0x%.4x.0x%.4x)\n",
      This->clusterId, This->attrId);
  if (This->value)
  {
    This->value->free(This->value);
  }
  free(This);
}

/* implementation methods */


