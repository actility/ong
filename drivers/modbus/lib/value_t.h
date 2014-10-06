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
//** File : value_t.h
//**
//** Description  :
//**
//**
//** Date : 09/08/2014
//** Author :  Mathias Louiset, Actility (c), 201r4
//**
//************************************************************************
//************************************************************************


#ifndef _VALUE_T__H_
#define _VALUE_T__H_

/**
 * @file value_t.h
 * @brief The file gives the definition of a ModBus value.
 * @author mlouiset
 * @date 2014-09-08
 */

typedef struct _ValueArray_t {
  uint16_t size;
  uint16_t *array;
} ValueArray_t;

typedef struct Value_t
{
  /* methods */
  /** @ref Value_t_newFree */
  void (* free) (struct Value_t *);
  void (* setBool) (struct Value_t *, bool val);
  bool (* boolValueHasChanged) (struct Value_t *, bool val);
  void (* setArray) (struct Value_t *, uint16_t *val, uint16_t size);
  bool (* arrayValueHasChanged) (struct Value_t *, uint16_t *val, uint16_t size);

  /* members */
  /** value data type */
  int typeID; // based on modbusTypeID, e.g. ACCESS_BOOL, ACCESS_ARRAY...

  /** The value */
  union {
    bool boolValue;
    ValueArray_t arrayValue;
  } u;

} Value_t;

/* dynamic allocation */
Value_t *new_Value_t();
void Value_t_newFree(Value_t *This);
void Value_t_setBool(Value_t *This, bool val);
bool Value_t_boolValueHasChanged(Value_t *This, bool val);
void Value_t_setArray(Value_t *This, uint16_t *val, uint16_t size);
bool Value_t_arrayValueHasChanged(Value_t *This, uint16_t *val, uint16_t size);

#endif

