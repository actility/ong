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
//** File : driver/interface_mapping_desc.c
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
#include "xoapipr.h"
#include "dIa.h"

#include "cmnstruct.h"
#include "adaptor.h"

#include "azap_types.h"
#include "azap_api_zcl.h"

#include "interface_mapping_desc.h"

/*****************************************************************************/
/* Prototypes for local functions */
static void interfaceMappingDesc_t_init(interfaceMappingDesc_t *This);

/*****************************************************************************/
/* Constructors / destructor */

/* static allocation */

/**
 * @brief Initialize the interfaceMappingDesc_t struct
 * @param This the class instance
 */
static void interfaceMappingDesc_t_init(interfaceMappingDesc_t *This)
{
  RTL_TRDBG(TRACE_DETAIL, "interfaceMappingDesc_t::init\n");

  /* Assign the pointers on functions */
  This->loadFromXoRef = interfaceMappingDesc_t_loadFromXoRef;

  /* Initialize the members */

  This->clusterId = 0xFFFF;
  This->clusterDir = ZCL_FRAME_CLIENT_SERVER_DIR;
  This->nbAttributes = 0;
  This->attributes = NULL;
  This->nbOperations = 0;
  This->operations = NULL;
}

/* dynamic allocation */
/**
 * @brief do dynamic allocation of a interfaceMappingDesc_t.
 * @return the copy of the object.
 */
interfaceMappingDesc_t *new_interfaceMappingDesc_t()

{
  interfaceMappingDesc_t *This = malloc(sizeof(interfaceMappingDesc_t));
  if (!This)
  {
    return NULL;
  }
  interfaceMappingDesc_t_init(This);
  This->free = interfaceMappingDesc_t_newFree;
  RTL_TRDBG(TRACE_DETAIL, "new interfaceMappingDesc_t (This:0x%.8x)\n", This);
  return This;
}

/**
 * @brief Destructor for dynamic allocation
 * @anchor interfaceMappingDesc_t_newFree
 * @param This the class instance
 */
void interfaceMappingDesc_t_newFree(interfaceMappingDesc_t *This)
{
  azap_uint8 i;
  RTL_TRDBG(TRACE_DETAIL, "interfaceMappingDesc_t::newFree (This:0x%.8x)\n", This);
  if (This->attributes)
  {
    free(This->attributes);
  }
  if (This->operations)
  {
    for (i = 0; i < This->nbOperations; i++)
    {
      free(This->operations[i]);
    }
    free(This->operations);
  }

  free(This);
}

/* implementation methods */
/**
 * @brief Load the attributes mapping from the config file.
 * @anchor interfaceMappingDesc_t_loadFromXoRef
 * @param This the class instance
 * @param mappingObj the pointer on the Xo object to map.
 */
void interfaceMappingDesc_t_loadFromXoRef(interfaceMappingDesc_t *This, void *mappingObj)
{
  char *name, *val;
  int nb, i, n, v;
  void *elt;

  if (!mappingObj)
  {
    return;
  }

  name = XoNmGetAttr(mappingObj, "name", 0);
  if (!name)
  {
    return;
  }
  if (!sscanf(name, "cluster.%x.", &v))
  {
    return;
  }

  if (!strcmp(name + 15, "server"))
  {
    This->clusterId = v;
    This->clusterDir = ZCL_FRAME_SERVER_CLIENT_DIR;
  }
  else if (!strcmp(name + 15, "client"))
  {
    This->clusterId = v;
    This->clusterDir = ZCL_FRAME_CLIENT_SERVER_DIR;
  }
  else
  {
    return;
  }

  nb = XoNmNbInAttr(mappingObj, "[name=attributes].[]", 0);
  if (nb > 0)
  {
    if (This->attributes)
    {
      free(This->attributes);
    }
    This->attributes = malloc(nb * sizeof(azap_uint16));
    memset(This->attributes, 0, nb * sizeof(azap_uint16));
    This->nbAttributes = nb;
    for (i = 0; ((i < nb) && (i < ITF_MAPPING_MAX_CONFIG_ATTR)); i++)
    {
      elt = XoNmGetAttr(mappingObj, "[name=attributes].[%d]", 0, i);
      if (!elt)
      {
        continue;
      }
      name = XoNmGetAttr(elt, "name", 0);
      val = XoNmGetAttr(elt, "val", 0);
      if ((val) && (sscanf(name, "attr.%d", &n)) && (n < nb))
      {
        sscanf(val, "0x%04x", &v);
        This->attributes[n] = v;
        RTL_TRDBG(TRACE_ERROR, "itf mapping [clId:0x%.4x] [clDir:%d] [attr.%d] "
            "[zbAttr:0x%.4x]\n", This->clusterId, This->clusterDir, n,
            This->attributes[n]);
      }
    }
  }

  nb = XoNmNbInAttr(mappingObj, "[name=operations].[]", 0);
  if (nb > 0)
  {
    if (This->operations)
    {
      free(This->operations);
    }
    This->operations = malloc(nb * sizeof(char *));
    memset(This->operations, 0, nb * sizeof(char *));
    This->nbOperations = nb;
    for (i = 0; ((i < nb) && (i < ITF_MAPPING_MAX_CONFIG_OPE)); i++)
    {
      elt = XoNmGetAttr(mappingObj, "[name=operations].[%d]", 0, i);
      if (!elt)
      {
        continue;
      }
      name = XoNmGetAttr(elt, "name", 0);
      val = XoNmGetAttr(elt, "val", 0);
      if ((val) && (sscanf(name, "ope.%d", &n)) && (n < nb))
      {
        This->operations[n] = strdup(val);
        RTL_TRDBG(TRACE_ERROR, "itf mapping [clId:0x%.4x] [clDir:%d] [ope.%d] "
            "[zbOpe:%s]\n", This->clusterId, This->clusterDir, n,
            This->operations[n]);
      }
    }
  }
}






