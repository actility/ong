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
//** File : ./include/azap_attribute.h
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


#ifndef _AZAP_ATTRIBUTE__H_
#define _AZAP_ATTRIBUTE__H_

/**
 * @file azap_attribute.h
 * @brief The file gives the definition of a ZigBee cluster attribute.
 * @author mlouiset
 * @date 2012-09-12
 */


/**
 * @struct azapAttribute_t
 * @brief The object for ZigBee cluster attribute.
 * As only few attributes along the whole supported clusters on the audited end point
 * will be stored/retrieved with this struct, AZAP attributes are identified by there
 * ZigBee cluster identifier and there ZigBee attribute identifier. They are all
 * managed in the same list in the AZAP end point structure.
 * @see azapEndPoint_t
 */
typedef struct azapAttribute_t
{
  /* methods */
  /** @ref azapAttribute_t_newFree */
  void (*free) (struct azapAttribute_t *);

  /* members */
  /** For list management */
  struct list_head chainLink;

  /** The cluster identifier */
  azap_uint16 clusterId;
  /** The attribute identifier */
  azap_uint16 attrId;
  /** The manufacturer code if specific. */
  azap_uint16 manufacturerCode;
  /** The attribute value */
  azapZclAttrVal_t *value;

} azapAttribute_t;

/* dynamic allocation */
azapAttribute_t *new_azapAttribute_t(azap_uint16 clId, azap_uint16 atId);
void azapAttribute_t_newFree(azapAttribute_t *This);

#endif

