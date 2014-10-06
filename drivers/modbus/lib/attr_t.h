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
//** File : attr_t.h
//**
//** Description  :
//**
//**
//** Date : 09/08/2014
//** Author :  Mathias Louiset, Actility (c), 201r4
//**
//************************************************************************
//************************************************************************


#ifndef _ATTR_T__H_
#define _ATTR_T__H_

/**
 * @file attr_t.h
 * @brief The file gives the definition of a Attr in retargeting config
 * @author mlouiset
 * @date 2014-09-08
 */

typedef struct Attr_t
{
  /* methods */
  void *(*buildXoFromProductDescriptor) (struct Attr_t *, uint16_t *array, uint16_t size);
  /** @ref Attr_t_newFree */
  void (*free) (struct Attr_t *);

  /* members */
  struct list_head list;
  char *modbusType; // ref to product
  int modbusTypeID;
  char *modbusAccess; // ref to product
  bool isReadable;
  bool isWritable;

  // Decoded version of modbusAccess
  uint16_t mask;
  uint16_t bytes;
  int reg, map;
  void *xoModelAttr; // ref to product "<subattributes>"

} Attr_t;

/* dynamic allocation */
Attr_t *new_Attr_t();
void Attr_t_newFree(Attr_t *This);

void *Attr_t_buildXoFromProductDescriptor(Attr_t *This, uint16_t *array, uint16_t size);

#endif


