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
//** File : point_t.h
//**
//** Description  :
//**
//**
//** Date : 09/08/2014
//** Author :  Mathias Louiset, Actility (c), 201r4
//**
//************************************************************************
//************************************************************************


#ifndef _POINT_T__H_
#define _POINT_T__H_

/**
 * @file point_t.h
 * @brief The file gives the definition of a Point in logging config
 * @author mlouiset
 * @date 2014-09-08
 */

typedef struct Point_t
{
  /* methods */
  /** @ref Point_t_newFree */
  void (*setModbusType) (struct Point_t *, int modbusType);
  void (*free) (struct Point_t *);

  /* members */
  struct list_head list;
  bool isInit;
  bool toDelete;

  time_t nextMaxInterval;
  time_t nextRead;
  int minInterval;
  int maxInterval;
  char *containerID;
  Value_t *lastVal;
 
  uint16_t rq_cluster, rq_attribut, rq_member;
  void *xo;
  
  Attr_t *attr;

} Point_t;

/* dynamic allocation */
Point_t *new_Point_t();
void Point_t_newFree(Point_t *This);

void Point_t_setModbusType(Point_t * This, int modbusType);

#endif


