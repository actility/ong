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
//** File : driver/m2m_zdo_abstraction.h
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


#ifndef __M2M_ZDO_ABSTRACTION__H_
#define __M2M_ZDO_ABSTRACTION__H_

/**
 * @file m2m_zdo_abstraction.h
 * @brief Provides ZDO operation implementations.
 * @author mlouiset
 * @date 2012-09-25
 */


struct m2mZdoAbstraction_t *getZdoAbstraction();

/**
 * @struct m2mZdoAbstraction_t
 * @brief the generic definition of a data and command interpreter.
 */
typedef struct m2mZdoAbstraction_t
{
  /* generic methods */
  /** @ref m2mZdoAbstraction_t_newFree */
  void (*free) (struct m2mZdoAbstraction_t *);
  /** @ref m2mZdoAbstraction_t_doZdoOperation */
  int (*doZdoOperation) (struct m2mZdoAbstraction_t *This, char *operation,
      zigbeeNode_t *node, azap_uint8 epId, azap_uint16 clId, void *obixReqContent,
      int reqId);

  /* generic members */


} m2mZdoAbstraction_t;

/* dynamic allocation */
m2mZdoAbstraction_t *new_m2mZdoAbstraction_t();
void m2mZdoAbstraction_t_newFree(m2mZdoAbstraction_t *This);

int  m2mZdoAbstraction_t_doZdoOperation(m2mZdoAbstraction_t *This, char *operation,
    zigbeeNode_t *node, azap_uint8 epId, azap_uint16 clId, void *obixReqContent, int reqId);

#endif


