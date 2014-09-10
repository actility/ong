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
//** File : ./lib/azap_endpoint.c
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
#include <stddef.h>

#include "rtlbase.h"
#include "rtllist.h"
#include "rtlimsg.h"

#include "azap_types.h"

#include "azap_zcl_attr_val.h"
#include "azap_attribute.h"
#include "azap_endpoint.h"

/*****************************************************************************/
/* Prototypes for local functions */
static void azapEndPoint_t_init(azapEndPoint_t *This);

/*****************************************************************************/
/* Constructors / destructor */

/* static allocation */

/**
 * @brief Initialize the azapEndPoint_t struct
 * @param This the class instance
 */
static void azapEndPoint_t_init(azapEndPoint_t *This)
{
  RTL_TRDBG(TRACE_DETAIL, "azapEndPoint_t::init\n");

  /* Assign the pointers on functions */
  This->addAttribute = azapEndPoint_t_addAttribute;
  This->getAttribute = azapEndPoint_t_getAttribute;
  This->getNthAttribute = azapEndPoint_t_getNthAttribute;

  /* Initialize the members */

  This->id = 0;
  This->appProfId = 0;
  This->appDeviceId = 0;
  This->appDevVer = 0;
  This->appNumInClusters = 0;
  This->appInClusterList = NULL;
  This->appNumOutClusters = 0;
  This->appOutClusterList = NULL;
  INIT_LIST_HEAD(&(This->clusterAttributeList));
}

/* dynamic allocation */
/**
 * @brief do dynamic allocation of a azapEndPoint_t.
 * @param id end point identifier
 * @param appProfId application profile identifier (0x0104: HA, 0X0109: SE)
 * @param appDeviceId application device identifier
 * @param appDevVer application device version
 * @param appNumInClusters number of server clusters
 * @param appInClusterList list of server cluster identifiers
 * @param appNumOutClusters number of client clusters
 * @param appOutClusterList list of client cluster identifiers
 * @return the copy of the object
 */
azapEndPoint_t *new_azapEndPoint_t(
  azap_uint8 id,
  azap_uint16 appProfId,
  azap_uint16 appDeviceId,
  azap_uint8 appDevVer,
  azap_uint8 appNumInClusters,
  azap_uint16 appInClusterList[],
  azap_uint8 appNumOutClusters,
  azap_uint16 appOutClusterList[])
{
  azapEndPoint_t *This = malloc(sizeof(azapEndPoint_t));
  if (!This)
  {
    return NULL;
  }
  azapEndPoint_t_init(This);
  This->free = azapEndPoint_t_newFree;

  This->id = id;
  This->appProfId = appProfId;
  This->appDeviceId = appDeviceId;
  This->appDevVer = appDevVer;

  This->appNumInClusters = appNumInClusters;
  This->appInClusterList = malloc(appNumInClusters * sizeof(azap_uint16));
  memcpy(This->appInClusterList, appInClusterList, appNumInClusters * sizeof(azap_uint16));

  This->appNumOutClusters = appNumOutClusters;
  This->appOutClusterList = malloc(appNumOutClusters * sizeof(azap_uint16));
  memcpy(This->appOutClusterList, appOutClusterList, appNumOutClusters * sizeof(azap_uint16));

  RTL_TRDBG(TRACE_DETAIL, "new azapEndPoint_t (This:0x%.8x)\n", This);
  return This;
}

/**
 * @brief Destructor for dynamic allocation
 * @anchor azapEndPoint_t_newFree
 * @param This the class instance
 */
void azapEndPoint_t_newFree(azapEndPoint_t *This)
{
  RTL_TRDBG(TRACE_DETAIL, "azapEndPoint_t::newFree (This:0x%.8x)\n", This);
  free(This->appInClusterList);
  free(This->appOutClusterList);
  while ( ! list_empty(&(This->clusterAttributeList)))
  {
    azapAttribute_t *attr = list_entry(This->clusterAttributeList.next,
        azapAttribute_t, chainLink);
    list_del(&attr->chainLink);
    attr->free(attr);
  }
  free(This);
}

/* implementation methods */

/**
 * @brief add an valued attribute to the end point if it does not exist yet.
 * @anchor azapEndPoint_t_addAttribute
 * @param This the class instance.
 * @param clId the cluster identifier the attribute belongs to.
 * @param attrId the attribute identifier.
 * @return the created azapAttribute_t instance if not already exist, the fetched
 * instance otherwise.
 */
azapAttribute_t *azapEndPoint_t_addAttribute(azapEndPoint_t *This, azap_uint16 clId,
    azap_uint16 attrId)
{
  azapAttribute_t *res = This->getAttribute(This, clId, attrId);
  if (NULL == res)
  {
    res = new_azapAttribute_t(clId, attrId);
    list_add_tail(&res->chainLink, &(This->clusterAttributeList));
  }
  return res;
}

/**
 * @brief Retrieve an AZAP attribute by its cluster identifier and attribute identifier.
 * @anchor azapEndPoint_t_getAttribute
 * @param This the class instance.
 * @param clId the cluster identifier to match.
 * @param attrId the attribute identifier to match.
 * @return the azapAttribute_t instance if found, NULL otherwise.
 */
azapAttribute_t *azapEndPoint_t_getAttribute(azapEndPoint_t *This, azap_uint16 clId,
    azap_uint16 attrId)
{
  azapAttribute_t *res = NULL;
  struct list_head *link;
  list_for_each (link, &(This->clusterAttributeList))
  {
    azapAttribute_t *attr = list_entry(link, azapAttribute_t, chainLink);
    if ((clId == attr->clusterId) && (attrId == attr->attrId))
    {
      res = attr;
      break;
    }
  }
  return res;
}

/**
 * @brief Retrieve an AZAP attribute by its position in the clusterAttributeList.
 * @anchor azapEndPoint_t_getAttribute
 * First index is 0, last index is (n-1) when there are n elements in the list.
 * @param This the class instance.
 * @param clId if not equals to 0xFFFF, specifies the cluster Id to match.
 * @param index the index position of the attribute to get.
 * @return the index-th azapAttribute_t instance if it exists, NULL otherwise.
 */
azapAttribute_t *azapEndPoint_t_getNthAttribute(azapEndPoint_t *This, azap_uint16 clId,
    azap_uint8 index)
{
  azapAttribute_t *res = NULL;
  struct list_head *link;
  list_for_each (link, &(This->clusterAttributeList))
  {
    azapAttribute_t *attr = list_entry(link, azapAttribute_t, chainLink);
    if ((0xFFFF == clId) || (attr->clusterId == clId))
    {
      if (0 == index)
      {
        res = attr;
        break;
      }
      index --;
    }
  }
  return res;
}

