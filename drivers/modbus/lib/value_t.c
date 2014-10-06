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
//** File : value_t.c
//**
//** Description  :
//**
//**
//** Date : 2014-09-08
//** Author :  Mathias Louiset, Actility (c), 2014.
//**
//************************************************************************
//************************************************************************


#include "modbus-acy.h"

/*****************************************************************************/
/* Prototypes for local functions */
static void Value_t_init(Value_t *This);
static void Value_t_freeValRsc(Value_t *This);

/*****************************************************************************/
/* Constructors / destructor */

/* static allocation */

/**
 * @brief Initialize the Value_t struct
 * @param This the class instance
 */
static void Value_t_init(Value_t *This)
{
  RTL_TRDBG(TRACE_DETAIL, "Value_t::init\n");

  /* Assign the pointers on functions */
  This->setBool = Value_t_setBool;
  This->boolValueHasChanged = Value_t_boolValueHasChanged;
  This->setArray = Value_t_setArray;
  This->arrayValueHasChanged = Value_t_arrayValueHasChanged;

  /* Initialize the members */
  This->typeID = ACCESS_VOID;
  memset(&(This->u), 0, sizeof(This->u));
}

static void Value_t_freeValRsc(Value_t *This)
{
  switch (This->typeID)
  { 
    case ACCESS_ARRAY:
      free(This->u.arrayValue.array);
      break;

    default:
      break;
  }
}

/* dynamic allocation */
/**
 * @brief do dynamic allocation of a Value_t.
 * @return the copy of the object
 */
Value_t *new_Value_t()
{
  Value_t *This = malloc(sizeof(Value_t));
  if (!This)
  {
    return NULL;
  }
  Value_t_init(This);
  This->free = Value_t_newFree;

  RTL_TRDBG(TRACE_DETAIL, "new Value_t\n");
  return This;
}

/**
 * @brief Destructor for dynamic allocation
 * @anchor Value_t_newFree
 */
void Value_t_newFree(Value_t *This)
{
  RTL_TRDBG(TRACE_DETAIL, "Value_t::newFree\n");
  Value_t_freeValRsc(This);
  free(This);
}

/* implementation methods */
void Value_t_setBool(Value_t *This, bool val)
{
  Value_t_freeValRsc(This);
  
  This->typeID = ACCESS_BOOL;
  This->u.boolValue = val;
}

bool Value_t_boolValueHasChanged(Value_t *This, bool val)
{
  if (This->typeID != ACCESS_BOOL) return false;
  return ( val != This->u.boolValue );
}

void Value_t_setArray(Value_t *This, uint16_t *val, uint16_t size)
{
  Value_t_freeValRsc(This);

  This->u.arrayValue.size = size;
  This->u.arrayValue.array = malloc(size * sizeof(uint16_t));
  memcpy(This->u.arrayValue.array, val, size*sizeof(uint16_t));
}

bool Value_t_arrayValueHasChanged(Value_t *This, uint16_t *val, uint16_t size)
{
  if (This->typeID != ACCESS_ARRAY) return false;
  if (size != This->u.arrayValue.size) return true;
  return ( 0 != memcmp(val, This->u.arrayValue.array, size * sizeof(uint16_t)) );
}


