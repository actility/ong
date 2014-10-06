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
static void Point_t_init(Point_t *This);

/*****************************************************************************/
/* Constructors / destructor */

/* static allocation */

/**
 * @brief Initialize the Point_t struct
 * @param This the class instance
 */
static void Point_t_init(Point_t *This)
{
  RTL_TRDBG(TRACE_DETAIL, "Point_t::init\n");

  /* Assign the pointers on functions */
  This->setModbusType = Point_t_setModbusType;

  /* Initialize the members */
  INIT_LIST_HEAD(&(This->list));
  This->isInit = false;
  This->toDelete = false;

  This->nextMaxInterval = 0;
  This->nextRead = 0;
  This->minInterval = 0;
  This->maxInterval = 0;
  This->containerID = NULL;
  This->lastVal = new_Value_t();
  This->attr = new_Attr_t();
 
  This->rq_cluster = 0;
  This->rq_attribut = 0;
  This->rq_member = 0;
  This->xo = NULL;
}


/* dynamic allocation */
/**
 * @brief do dynamic allocation of a Point_t.
 * @return the copy of the object
 */
Point_t *new_Point_t()
{
  Point_t *This = malloc(sizeof(Point_t));
  if (!This)
  {
    return NULL;
  }
  Point_t_init(This);
  This->free = Point_t_newFree;

  RTL_TRDBG(TRACE_DETAIL, "new Point_t\n");
  return This;
}

/**
 * @brief Destructor for dynamic allocation
 * @anchor Point_t_newFree
 */
void Point_t_newFree(Point_t *This)
{
  RTL_TRDBG(TRACE_DETAIL, "Point_t::newFree (id:0x%.4x.0x%.4x.%d)\n",
      This->rq_cluster, This->rq_attribut, This->rq_member);

  if (This->xo != NULL) {
    XoFree(This->xo, 1);
  }                                                                                               
  This->lastVal->free(This->lastVal);
  This->attr->free(This->attr);

  free(This);
}

/* implementation methods */
void Point_t_setModbusType(Point_t * This, int modbusType)
{
  This->attr->modbusTypeID = modbusType;
  This->lastVal->typeID = modbusType;
}

