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
//** File : driver/zigbee_han.c
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
#include <unistd.h>
#include <stddef.h>
#include <limits.h>

#include "rtlbase.h"
#include "rtllist.h"
#include "rtlimsg.h"


#include "xoapipr.h"
#include "dIa.h"

#include "cmndefine.h"
#include "cmnstruct.h"

#include "cmnproto.h"
#include "specextern.h"
#include "adaptor.h"

#include "azap_types.h"
#include "azap_tools.h"
#include "azap_zcl_attr_val.h"
#include "azap_attribute.h"
#include "azap_endpoint.h"
#include "azap_api_common.h"
#include "azap_api_zdo.h"
#include "azap_api_zcl.h"

#include "interface_mapping_desc.h"
#include "driver_req_context.h"
#include "zigbee_node.h"
#include "zigbee_han.h"
#include "m2m_zdo_abstraction.h"
#include "m2m_abstraction.h"


/*****************************************************************************/
/* extern variables */
extern zigbeeHan_t *theHan;
extern azap_uint8 azapPort;
extern int DiaNbReqPending;

/*****************************************************************************/
/* Prototypes for local functions */
static void zigbeeHan_t_init(zigbeeHan_t *This);

/*****************************************************************************/
/* Constructors / destructor */

/* static allocation */

/**
 * @brief Initialize the zigbeeHan_t struct
 * @param This the class instance
 */
static void zigbeeHan_t_init(zigbeeHan_t *This)
{
  RTL_TRDBG(TRACE_DETAIL, "zigbeeHan_t::init\n");

  /* Assign the pointers on functions */
  This->reloadKnownNodes = zigbeeHan_t_reloadKnownNodes;
  This->dumpNodeList = zigbeeHan_t_dumpNodeList;
  This->dumpNode = zigbeeHan_t_dumpNode;
  This->syncNode = zigbeeHan_t_syncNode;
  This->getNodeByNwkAddr = zigbeeHan_t_getNodeByNwkAddr;
  This->getNodeByIeeeAddr = zigbeeHan_t_getNodeByIeeeAddr;
  This->getNodeByIeeeAddrStr = zigbeeHan_t_getNodeByIeeeAddrStr;
  This->addNode = zigbeeHan_t_addNode;
  This->deleteNode = zigbeeHan_t_deleteNode;
  This->buildNodeList = zigbeeHan_t_buildNodeList;
  This->loadMappingFromXoRef = zigbeeHan_t_loadMappingFromXoRef;
  This->addMapping = zigbeeHan_t_addMapping;
  This->getMapping = zigbeeHan_t_getMapping;
  This->publish = zigbeeHan_t_publish;
  This->computeNextPermitJoinDuration = zigbeeHan_t_computeNextPermitJoinDuration;
  /* Initialize the members */
  This->m2mIpuCreated = false;
  /** Indicates if the M2M representation for ZigBee network has been created. */
  This->m2mNwkCreated = false;
  This->m2mOpenAssocEndTimeMono = 0;

  This->panId = 0;
  memset(This->panIdExt, 0, IEEE_ADDR_LEN);
  This->rfChan = 0;
  INIT_LIST_HEAD(&(This->knownNodesList));
  INIT_LIST_HEAD(&(This->configInterfacesMapping));
}

/* dynamic allocation */
/**
 * @brief do dynamic allocation of a zigbeeHan_t.
 * @return the copy of the object
 */
zigbeeHan_t *new_zigbeeHan_t()
{
  zigbeeHan_t *This = malloc(sizeof(zigbeeHan_t));
  if (!This)
  {
    return NULL;
  }
  zigbeeHan_t_init(This);
  This->free = zigbeeHan_t_newFree;
  RTL_TRDBG(TRACE_DETAIL, "new zigbeeHan_t (This:0x%.8x)\n", This);
  return This;
}

/**
 * @brief Destructor for dynamic allocation
 * @param This the class instance
 * @anchor zigbeeHan_t_newFree
 */
void zigbeeHan_t_newFree(zigbeeHan_t *This)
{
  RTL_TRDBG(TRACE_DETAIL, "zigbeeHan_t::newFree (This:0x%.8x)\n", This);
  while ( ! list_empty(&(This->knownNodesList)))
  {
    zigbeeNode_t *node = list_entry(This->knownNodesList.next, zigbeeNode_t, chainLink);
    list_del(&node->chainLink);
    node->free(node);
  }
  free(This);
}

/* implementation methods */

/**
 * @brief Dump using provided function the list of known node of the HAN.
 * @anchor zigbeeHan_t_dumpNodeList
 * @param This the class instance
 * @param printFunc the function to invoke for dumping the built node list.
 * @param cl the CLI on which the list should be dumped on.
 */
void zigbeeHan_t_dumpNodeList(zigbeeHan_t *This, pf_print printFunc, t_cli *cl)
{
  zigbeeNode_t *node = NULL;
  struct list_head *link;
  time_t now;
  int remaining = 0;

  if (!This->m2mIpuCreated)
  {
    printFunc(cl, "IPU not created yet\n");
    return; 
  }

  if (!This->m2mNwkCreated)
  {
    printFunc(cl, "Network not created yet\n");
    return; 
  }

  {
    printFunc(cl, "IPU not created yet\n");
    return; 
  }

  rtl_timemono(&now);
  if (now < This->m2mOpenAssocEndTimeMono)
  {
    remaining = This->m2mOpenAssocEndTimeMono - now;
  }

  if (remaining > 3600)
  {
    printFunc(cl, "Association procedure: OPENED (for more than 1 hour)\n");
  }
  else if (remaining > 0)
  {
    printFunc(cl, "Association procedure: OPENED (for another %d seconds)\n", remaining);
  }
  else
  {
    printFunc(cl, "Association procedure: CLOSED\n");
  }

  printFunc(cl, "current sensors detected:\n");
  list_for_each (link, &(This->knownNodesList))
  {
    node = list_entry(link, zigbeeNode_t, chainLink);
    node->dumpNode(node, printFunc, cl, 0);
  }
}

/**
 * @brief Dump a known node by its network address.
 * @anchor zigbeeHan_t_dumpNode
 * @param This the class instance
 * @param printFunc the function to invoke for dumping the built node list.
 * @param cl the CLI on which the list should be dumped on.
 * @param addr the network address of the node to be dumped.
 */
void zigbeeHan_t_dumpNode(zigbeeHan_t *This, pf_print printFunc, t_cli *cl,
    azap_uint16 addr)
{
  zigbeeNode_t *node = This->getNodeByNwkAddr(This, addr);

  if (node)
  {
    node->dumpNode(node, printFunc, cl, 1);
  }
  else
  {
    printFunc(cl, "*** Unknown device\n");
  }
}

/**
 * @brief Synchronize SCL tree with info collected by the GIP for a node.
 * @anchor zigbeeHan_t_syncNode
 * @param This the class instance
 * @param printFunc the function to invoke for dumping the built node list.
 * @param cl the CLI on which the list should be dumped on.
 * @param addr the network address of the node to be dumped.
 */
void zigbeeHan_t_syncNode(zigbeeHan_t *This, pf_print printFunc, t_cli *cl,
    azap_uint16 addr)
{
  zigbeeNode_t *node = This->getNodeByNwkAddr(This, addr);

  if (node)
  {
    node->clearDiaCounters(node);
  }
  else
  {
    printFunc(cl, "*** Unknown device\n");
  }
}

/**
 * @brief Retrieve a node by its short address.
 * @anchor zigbeeHan_t_getNodeByNwkAddr
 * @param This the class instance
 * @param addr the short address of the node.
 * @return the node that matches the provided IEEE address if found, NULL otherwise.
 */
zigbeeNode_t *zigbeeHan_t_getNodeByNwkAddr(zigbeeHan_t *This, azap_uint16 addr)
{
  zigbeeNode_t *res = NULL;
  struct list_head *link;

  list_for_each (link, &(This->knownNodesList))
  {
    zigbeeNode_t *node = list_entry(link, zigbeeNode_t, chainLink);
    if (addr == node->cp_cmn.cmn_num)
    {
      res = node;
    }
  }
  return res;
}

/**
 * @brief Retrieve a node by the string representation of its IEEE address.
 * @anchor zigbeeHan_t_getNodeByIeeeAddrStr
 * @param This the class instance
 * @param ieee the IEEE address of the node.
 * @return the node that matches the provided IEEE address if found, NULL otherwise.
 */
zigbeeNode_t *zigbeeHan_t_getNodeByIeeeAddrStr(zigbeeHan_t *This, char *ieee)
{
  zigbeeNode_t *res = NULL;
  struct list_head *link;

  list_for_each (link, &(This->knownNodesList))
  {
    zigbeeNode_t *node = list_entry(link, zigbeeNode_t, chainLink);
    if (0 == strcmp(ieee, node->cp_cmn.cmn_ieee))
    {
      res = node;
    }
  }
  return res;
}

/**
 * @brief Retrieve a node by its IEEE address.
 * @anchor zigbeeHan_t_getNodeByIeeeAddr
 * @param This the class instance
 * @param ieee the IEEE address of the node.
 * @return the node that matches the provided IEEE address if found, NULL otherwise.
 */
zigbeeNode_t *zigbeeHan_t_getNodeByIeeeAddr(zigbeeHan_t *This, azap_uint8 *ieee)
{
  zigbeeNode_t *res = NULL;
  struct list_head *link;

  list_for_each (link, &(This->knownNodesList))
  {
    zigbeeNode_t *node = list_entry(link, zigbeeNode_t, chainLink);
    if (0 == memcmp(ieee, node->ieeeAddr, IEEE_ADDR_LEN))
    {
      res = node;
    }
  }
  return res;
}

/**
 * @brief Add a node to the HAN.
 * @anchor zigbeeHan_t_addNode
 * @param This the class instance
 * @param node the new node to add to the list of known
 */
void zigbeeHan_t_addNode(zigbeeHan_t *This, zigbeeNode_t *node)
{
  list_add_tail(&node->chainLink, &(This->knownNodesList));
}


/**
 * @brief Remove a node known by its IEEE address and delete it.
 * @anchor zigbeeHan_t_deleteNode
 * @param This the class instance
 * @param ieee the IEEE address of the node.
 */
void zigbeeHan_t_deleteNode(zigbeeHan_t *This, azap_uint8 *ieee)
{
  zigbeeNode_t *toDelete = NULL;
  struct list_head *link;

  list_for_each (link, &(This->knownNodesList))
  {
    zigbeeNode_t *node = list_entry(link, zigbeeNode_t, chainLink);
    if (0 == memcmp(ieee, node->ieeeAddr, IEEE_ADDR_LEN))
    {
      toDelete = node;
    }
  }

  if (toDelete)
  {
    list_del(&toDelete->chainLink);
    toDelete->free(toDelete);
  }
}

/**
 * @brief Build the representation of the HAN devices list and add it to the oBIX resource.
 * @anchor zigbeeHan_t_buildNodeList
 * @param This the class instance
 * @param dst the oBIX document in which adding the HAN devices list representation.
 */
void zigbeeHan_t_buildNodeList(zigbeeHan_t *This, void *dst)
{
  zigbeeNode_t *node;
  char target[256];
  struct list_head *link;
  void *dev;

  if (!dst)
  {
    return;
  }
  if (XoIsObix(dst) <= 0)
  {
    RTL_TRDBG(TRACE_ERROR, "zigbeeHan_t::buildNodeList - error: "
        "not an obix object\n");
    return;
  }

  list_for_each (link, &(This->knownNodesList))
  {
    node = list_entry(link, zigbeeNode_t, chainLink);
    if (node->toDelete)
    {
      continue;
    }
    dev = XoNmNew("o:ref");
    if  (!dev)
    {
      RTL_TRDBG(TRACE_ERROR, "zigbeeHan_t_buildNodeList - error: "
          "cannot allocate 'o:ref' for sensor %s\n", node->cp_cmn.cmn_ieee);
      continue;
    }

    XoNmSetAttr(dev, "name$", "ref", 0);
    sprintf(target, "%s/DEV_%s/containers/DESCRIPTOR/contentInstances/latest/content",
        SCL_ROOT_APP, node->cp_cmn.cmn_ieee);
    XoNmSetAttr(dev, "href", target, 0);
    XoNmAddInAttr(dst, "[name=nodes].[]", dev, 0, 0);
  }
}

/**
 * @brief Load the attributes mapping list from the config file.
 * @anchor zigbeeHan_t_buildNodeList
 * @param This the class instance
 * @param xoConfig The pointer on the Xo object.
 */
void zigbeeHan_t_loadMappingFromXoRef(zigbeeHan_t *This, void *xoConfig)
{
  int nb, i;
  void *xoItfMap;
  interfaceMappingDesc_t *itfMap;
  if (!xoConfig)
  {
    return;
  }

  // custom configuration can not change m2m mapping
  nb  = XoNmNbInAttr(xoConfig, "[name=interfacesMapping].[]", 0);
  for (i = 0 ; i < nb ; i++)
  {
    xoItfMap = XoNmGetAttr(xoConfig, "[name=interfacesMapping].[%d]", 0, i);
    if (!xoItfMap)
    {
      continue;
    }

    itfMap = new_interfaceMappingDesc_t();
    itfMap->loadFromXoRef(itfMap, xoItfMap);
    if ((itfMap->clusterId == 0xFFFF) ||
        (! This->addMapping(This, itfMap)))
    {
      itfMap->free(itfMap);
    }
  }
}

/**
 * @brief Add a new interface mapping entry to the HAN.
 * @anchor zigbeeHan_t_addMapping
 * @param This the class instance
 * @param itfMap the interfaceMappingDesc_t instance to add to the list.
 */
int zigbeeHan_t_addMapping(zigbeeHan_t *This, interfaceMappingDesc_t *itfMap)
{
  if (!itfMap)
  {
    return 0;
  }
  if (NULL != This->getMapping(This, itfMap->clusterId, itfMap->clusterDir))
  {
    return 0;
  }
  list_add_tail(&itfMap->chainLink, &(This->configInterfacesMapping));
  return 1;
}

/**
 * @brief Retrieve a interface mapping entry by its cluster identifier and direction.
 * @anchor zigbeeHan_t_getMapping
 * @param This the class instance.
 * @param clId The cluster identifier to look for.
 * @param clDir The cluster direction to look for.
 */
interfaceMappingDesc_t *zigbeeHan_t_getMapping(zigbeeHan_t *This, azap_uint16 clId,
    azap_uint8 clDir)
{
  interfaceMappingDesc_t *res = NULL;
  struct list_head *link;
  list_for_each (link, &(This->configInterfacesMapping))
  {
    interfaceMappingDesc_t *map = list_entry(link, interfaceMappingDesc_t, chainLink);
    if ((clId == map->clusterId) && (clDir == map->clusterDir))
    {
      res = map;
    }
  }
  return res;
}

/**
 * @brief Reload the devices' local representations from Actility/ZigBee XML files.
 * @anchor zigbeeHan_t_reloadKnownNodes
 * @param This the class instance.
 * @param path the path where Actility/ZigBee XML files are located.
 */
void zigbeeHan_t_reloadKnownNodes(zigbeeHan_t *This, char *path)
{
  void *dir;
  char *name;
  char *point;
  zigbeeNode_t *node;
  int rc;

  dir = rtl_openDir(path);
  if (!dir)
  {
    return;
  }

  RTL_TRDBG(TRACE_INFO, "Searching known sensors in '%s'\n", path);

  while ( (name = rtl_readDir(dir)) && *name )
  {
    if (*name == '.' && *(name + 1) == '\0')
    {
      continue;
    }
    if (*name == '.' && *(name + 1) == '.')
    {
      continue;
    }
    point = rtl_suffixname(name);
    if ( point && *point && strcmp(point + 1, "xml") == 0 )
    {
      RTL_TRDBG(TRACE_INFO, "zigbeeHan_t::reloadKnownNodes - found '%s'\n", name);
      node = new_zigbeeNode_t_ext();
      if ((rc = node->loadFromFile(node, This->panId, This->panIdExt, This->rfChan,
          path, name)) > 0)
      {
        if (! This->getNodeByIeeeAddr(This, node->ieeeAddr))
        {
          This->addNode(This, node);
          node = NULL;
        }
        else
        {
          RTL_TRDBG(TRACE_ERROR, "zigbeeHan_t::reloadKnownNodes - duplicated IEEE "
              "address.  Ignored. (file:'%s')\n", name);
        }
      }
      else
      {
        RTL_TRDBG(TRACE_INFO, "zigbeeHan_t::reloadKnownNodes - '%s' ignored "
            "(rc:%d)\n", name, rc);
      }

      // error detected, node is ignored.
      if (node)
      {
        node->free(node);
      }
    }
  }

  rtl_closeDir(dir);
}

/**
 * Send dIa requests to create all M2M resources associated to this HAN.
 * This function manages all the dIa requests regarding a device. But the requests
 * are not all sent at the same time but spread over time. Indeed this function is
 * invoked periodically.
 * @anchor zigbeeHan_t_publish
 * @param This the class instance.
 */
void zigbeeHan_t_publish(zigbeeHan_t *This)
{
  zigbeeNode_t *node;
  struct list_head *link;
  if (DiaDisconnected())
  {
    RTL_TRDBG(TRACE_IMPL, "zigbeeHan_t::publish - canceled: dIa is disconnected\n");
    return;
  }
  if (!This->m2mIpuCreated)
  {
    RTL_TRDBG(TRACE_IMPL, "zigbeeHan_t::publish - canceled: IPU is not created yet\n");
    return;
  }
  if (!This->m2mNwkCreated)
  {
    RTL_TRDBG(TRACE_IMPL, "zigbeeHan_t::publish - canceled: Network is not created yet\n");
    return;
  }
  if (DiaNbReqPending > 0)
  {
    RTL_TRDBG(TRACE_IMPL, "zigbeeHan_t::publish - canceled: still dIa requests not responded\n");
    return;
  }

  list_for_each (link, &(This->knownNodesList))
  {
    node = list_entry(link, zigbeeNode_t, chainLink);
    if (node->publish(node) || node->unpublish(node))
    {
      return;
    }
  }
}

/**
 * If provided, set the new m2mOpenAssocEndTimeMono value, and tells the value of the PermitJoin
 * duration to use.
 * WARNING: when using zbMgmtPermitJoin with 255, don't call this function!!!
 * @anchor zigbeeHan_t_computeNextPermitJoinDuration
 * @param This the class instance.
 * @param newDuration the new duration for the "open" association (or zbMgmtPermitJoin), -1
 * when not relevant.
 * @return the value to use for the azapMgmtPermitJoin call.
 */
int zigbeeHan_t_computeNextPermitJoinDuration(zigbeeHan_t *This, int newDuration)
{
  int nextPJDuration = 0;
  time_t now;
  int remaining = 0;

  rtl_timemono(&now);

  RTL_TRDBG(TRACE_IMPL, "zigbeeHan_t::computeNextPermitJoinDuration (This:0x%.8x) "
      "(now:%ld) (m2mOpenAssocEndTimeMono:%ld) (newDuration:%d)\n", This,
      now, This->m2mOpenAssocEndTimeMono, newDuration);

  if (now < This->m2mOpenAssocEndTimeMono)
  {
    remaining = This->m2mOpenAssocEndTimeMono - now;
  }
  if (newDuration == INT_MAX)
  {
    This->m2mOpenAssocEndTimeMono = INT_MAX;
  }
  else if ( (newDuration != -1) && ( (newDuration == 0) || (remaining < newDuration ) ) )
  {
    // set the new m2mOpenAssocEndTimeMono value
    This->m2mOpenAssocEndTimeMono = now + newDuration;
  }

  nextPJDuration = This->m2mOpenAssocEndTimeMono - now;
  if (nextPJDuration > ZIGBEE_MAX_PERMIT_JOIN_DURATION)
  {
    nextPJDuration = ZIGBEE_MAX_PERMIT_JOIN_DURATION;
  }

  return nextPJDuration;
}

/*          _
 *       __| |_ ___ ____ ___ _ __  _ __  ___ _ _
 *      / _` | '_\ V / _/ _ \ '  \| '  \/ _ \ ' \
 *      \__,_|_|  \_/\__\___/_|_|_|_|_|_\___/_||_|
 *
 *      _   ___ ___    __              _   _
 *     /_\ | _ \_ _|  / _|_  _ _ _  __| |_(_)___ _ _  ___
 *    / _ \|  _/| |  |  _| || | ' \/ _|  _| / _ \ ' \(_-<
 *   /_/ \_\_| |___| |_|  \_,_|_||_\__|\__|_\___/_||_/__/
 */

/**
 * @brief Call back from retargeted operations on the M2M network application.
 * @param ident the network identifier (i.e. the PAN IEEE address, in hex format)
 * @param targetlevel The access right level (i.e. 1, 2 or 3)
 * @param obix the request content decoded as oBIX buffer.
 * @param targetId the request URI of the dIa incoming request.
 * @param reqId the dIa request identifier to provide on completion.
 */
int CmnNetworkExecuteMethod(char *ident, int targetlevel, void *obix, char *targetId,
    int reqId)
{
  (void) ident;
  (void)targetlevel;
  char *operation;

  if ((! targetId) || (! *targetId))
  {
    return ERR_INVALID_REQ_URI;
  }

  operation = rindex(targetId, '/');
  operation ++;
  if (!strncmp(operation, "zb", strlen("zb")))
  {
    m2mZdoAbstraction_t *abstraction = getZdoAbstraction();
    return abstraction->doZdoOperation(abstraction, operation, NULL, 0, 0, obix, reqId);
  }

  // else m2m operation
  m2mAbstraction_t *m2m = getM2mAbstraction();
  return m2m->doM2mOperation(m2m, operation, NULL, 0, 0, obix, reqId);
}


