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
//** File : ./include/azap_endpoint.h
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


#ifndef _AZAP_ENDPOINT__H_
#define _AZAP_ENDPOINT__H_

/**
 * @file azap_endpoint.h
 * @brief The file brings the definition of  ZigBee end point.
 * @author mlouiset
 * @date 2012-07-19
 */

/**
 * @def ZB_MAX_ENDPOINT_ID
 * @brief The maximum end point identifier on ZigBee device, as defined by the protocol.
 */
#define ZB_MAX_ENDPOINT_ID 240


/**
 * @struct azapEndPoint_t
 * The object for ZigBee end point.
 */
typedef struct azapEndPoint_t
{
  /* methods */
  /** @ref azapEndPoint_t_addAttribute */
  azapAttribute_t *(*addAttribute) (struct azapEndPoint_t *, azap_uint16 clId,
      azap_uint16 attrId);
  /** @ref azapEndPoint_t_getAttribute */
  azapAttribute_t *(*getAttribute) (struct azapEndPoint_t *, azap_uint16 clId,
      azap_uint16 attrId);
  /** @ref azapEndPoint_t_getNthAttribute */
  azapAttribute_t *(*getNthAttribute) (struct azapEndPoint_t *, azap_uint16 clId,
      azap_uint8 index);
  /** @ref azapEndPoint_t_newFree */
  void (*free) (struct azapEndPoint_t *);

  /* members */
  /** For list management */
  struct list_head chainLink;

  /** The end point identifier, between 1 and ZB_MAX_ENDPOINT_ID */
  azap_uint8 id;
  /** The application profile identifier (0x0104: HA, 0X0109: SE) */
  azap_uint16 appProfId;
  /** The application device identifier */
  azap_uint16 appDeviceId;
  /** The application device version */
  azap_uint8 appDevVer;
  /** The number of server clusters */
  azap_uint8 appNumInClusters;
  /** The list of server cluster identifiers */
  azap_uint16 *appInClusterList;
  /** The number of client clusters */
  azap_uint8 appNumOutClusters;
  /** list of client cluster identifiers */
  azap_uint16 *appOutClusterList;

  /** list of clusterAttributes */
  struct list_head clusterAttributeList;

} azapEndPoint_t;

/* dynamic allocation */
azapEndPoint_t *new_azapEndPoint_t(azap_uint8 id, azap_uint16 appProfId, azap_uint16 appDeviceId,
    azap_uint8 appDevVer, azap_uint8 appNumInClusters, azap_uint16 appInClusterList[],
    azap_uint8 appNumOutClusters, azap_uint16 appOutClusterList[]);
azapAttribute_t *azapEndPoint_t_addAttribute(azapEndPoint_t *, azap_uint16 clId,
    azap_uint16 attrId);
azapAttribute_t *azapEndPoint_t_getAttribute(azapEndPoint_t *, azap_uint16 clId,
    azap_uint16 attrId);
azapAttribute_t *azapEndPoint_t_getNthAttribute(azapEndPoint_t *, azap_uint16 clId,
    azap_uint8 index);
void azapEndPoint_t_newFree(azapEndPoint_t *This);

#endif

