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
//** File : attr_t.c
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
static void Attr_t_init(Attr_t *This);

/*****************************************************************************/
/* Constructors / destructor */

/* static allocation */

/**
 * @brief Initialize the Attr_t struct
 * @param This the class instance
 */
static void Attr_t_init(Attr_t *This)
{
  RTL_TRDBG(TRACE_DETAIL, "Attr_t::init\n");

  /* Assign the pointers on functions */
  This->buildXoFromProductDescriptor = Attr_t_buildXoFromProductDescriptor;

  /* Initialize the members */
  INIT_LIST_HEAD(&(This->list));

  This->modbusType = NULL;
  This->modbusTypeID = ACCESS_VOID;
  This->modbusAccess = NULL;
  This->isReadable = false;
  This->isWritable = false;
  This->mask = 0;
  This->bytes = 0;
  This->reg = 0;
  This->map = 0;
  This->xoModelAttr = NULL;
}


/* dynamic allocation */
/**
 * @brief do dynamic allocation of a Attr_t.
 * @return the copy of the object
 */
Attr_t *new_Attr_t()
{
  Attr_t *This = malloc(sizeof(Attr_t));
  if (!This)
  {
    return NULL;
  }
  Attr_t_init(This);
  This->free = Attr_t_newFree;

  RTL_TRDBG(TRACE_DETAIL, "new Attr_t\n");
  return This;
}

/**
 * @brief Destructor for dynamic allocation
 * @anchor Attr_t_newFree
 */
void Attr_t_newFree(Attr_t *This)
{
  RTL_TRDBG(TRACE_DETAIL, "Attr_t::newFree (type:%s) (access:%s)\n",
      This->modbusType, This->modbusAccess);

  // do not free xoModelAttr, modbusType and modbusAccess, 
  // as it is references to product catalog

  free(This);
}

/* implementation methods */
/**
 * Build the XO buffer for the oBIX representation of a attribute of type "array".
 * @param This the attribute.
 * @param array the read raw data to represent.
 * @param size number on read registers.
 * @return the built XO buffer.
 */
void *Attr_t_buildXoFromProductDescriptor(Attr_t *This, uint16_t *array, uint16_t size)
{
  void *obixReport = NULL;
  void *obixAttr = NULL;
  int nbSubAttrs, iSubAttrs;
  void *subAttr;
  int modbusType;
  void *param;
  unsigned int offsetBeg = 0, offsetEnd = 0;
  uint8_t valArray[256];
  uint16_t valSize;
  char strVal[256];
  int i;
  void *xoModel;
  
  if (!This->xoModelAttr) return NULL;

  xoModel = This->xoModelAttr;
  obixReport = XoNmNew("o:obj");

  nbSubAttrs = XoNmNbInAttr(xoModel, "modbus:subattributes", 0);
  for (iSubAttrs = 0; iSubAttrs < nbSubAttrs; iSubAttrs++) {
    subAttr = XoNmGetAttr(xoModel, "modbus:subattributes[%d]", 0, iSubAttrs);
    CONTINUE_IF_NULL(subAttr);

    param = XoNmGetAttr(subAttr, "modbus:type", NULL, 0);
    CONTINUE_IF_NULL(param);

    obixAttr = XoNmNew(obixTypeFromModbus(param, &modbusType));
    CONTINUE_IF_NULL(param);
    
    param = XoNmGetAttr(subAttr, "modbus:name", NULL, 0);
    if (param) {
      XoNmSetAttr(obixAttr, "name", param, 0);
    } else {
      RTL_TRDBG(TRACE_ERROR, "Attr_t::buildXoFromProductDescriptor: Unable to extract "
        "name (type:%s) (access:%s)\n", This->modbusType, This->modbusAccess);
      XoFree(obixAttr, 1);
      continue;
    }

    param = XoNmGetAttr(subAttr, "modbus:offset", NULL, 0);
    if ((param) && 
        (2 == sscanf(param, "/beginbyte/%u/endbyte/%u/", &(offsetBeg), &(offsetEnd))) && 
        (offsetBeg <= offsetEnd) && 
        (offsetEnd < (size*2))) {
      memset(valArray, 0, sizeof(valArray));
      valSize = (offsetEnd - offsetBeg) + 1;
      for (i = 0; i < valSize; i += 2) {
        // modbus has big-endian encoding for registers (2-bytes blocks) but little endian
        // inside a register.
        // put modbus words into uint8_t array in the rights order
        // (little endian, for all targets?)
        uint16_t value = array[(offsetBeg + i)/2];
        if ((valSize == 1) && (offsetBeg % 2)) {
          value = value >> 8;
        }
        memcpy(valArray + i, &value, 2);
      }
      modbusValueToString(valArray, valSize, modbusType, strVal);
      XoNmSetAttr(obixAttr, "val", strVal, 0);
    } else {
      RTL_TRDBG(TRACE_ERROR, "Attr_t::buildXoFromProductDescriptor: Unable to extract "
        "value, no offset (type:%d) (access:%s) (offset:%s) (offsetBeg:%u) (offsetEnd:%u) "
        "(size:%u)\n", This->modbusType, This->modbusAccess, param, 
        offsetBeg, offsetEnd, size);
      XoFree(obixAttr, 1);
      continue;
    }

    XoNmAddInAttr(obixReport, "[]", obixAttr, 0, 0);
  }

  return obixReport;
}


