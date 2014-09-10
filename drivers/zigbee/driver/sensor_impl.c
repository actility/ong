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
//** File : driver/sensor_impl.c
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
#include "m2m_abstraction.h"
#include "m2m_zcl_abstraction.h"
#include "m2m_zdo_abstraction.h"


/*****************************************************************************/
/* extern variables */
extern zigbeeHan_t *theHan;



/*      _   ___ ___    __              _   _
 *     /_\ | _ \_ _|  / _|_  _ _ _  __| |_(_)___ _ _  ___
 *    / _ \|  _/| |  |  _| || | ' \/ _|  _| / _ \ ' \(_-<
 *   /_/ \_\_| |___| |_|  \_,_|_||_\__|\__|_\___/_||_/__/
 */

/*
 * Theses functions are invoked through the common part with other drivers. So they have to
 * be implemented.
 */

/**
 * @brief Get the sensor common struct to built dIa requests.
 * Invoked in dia*.[h|c]
 * @param num for ZigBee driver this num maps the short address of the node.
 * @param serial the serial identifier associated to the sensor.
 * @return the common struct for the sensor that matches num and serial if found, NULL
 * otherwise.
 */
t_cmn_sensor *CmnSensorNumSerial(int num, int serial)
{
  t_cmn_sensor *res = NULL;
  zigbeeNode_t *node = theHan->getNodeByNwkAddr(theHan, num);
  if (! node)
  {
    // nothing to do
  }
  else if (node->cp_cmn.cmn_serial != serial)
  {
    RTL_TRDBG(TRACE_ERROR, "CmnSensorNumSerial - error: serial and num are desynchronized"
        "(serial expected:%d) (serial:%d) (num:%s) (IEEE:%s)\n", serial,
        node->cp_cmn.cmn_serial, azap_uint16ToString(num, true),
        azap_extAddrToString(node->ieeeAddr, true));
  }
  else
  {
    res = &(node->cp_cmn);
  }

  return res;
}

/**
 * @brief Get the sensor common struct by the string representation of its IEEE address.
 * Invoked in dia*.[h|c]
 * @param ieee the string representation of the IEEE address to match
 * @return the common struct for the sensor that matches the provided string representation
 * if found, NULL otherwise.
 */
t_cmn_sensor *CmnSensorFindByIeee(char *ieee)
{
  t_cmn_sensor *res = NULL;
  zigbeeNode_t *node = theHan->getNodeByIeeeAddrStr(theHan, ieee);
  if (node)
  {
    res = &(node->cp_cmn);
  }

  return res;
}


/**
 * @brief Call back raised on incoming dIa create request for operation.
 * Invoked in diaserver.c
 * @param cmn the common structure instance for the targeted sensor.
 * @param app the application number (i.e. the end point identifier targeted on the remote
 * node).
 * @param cluster the cluster identifier the command refers to.
 * @param numm the operation number in case of ZCL operation, -1 otherwise (ZDO).
 * @param targetlevel the access right level (i.e. 1, 2 or 3) (useless here)
 * @param obix the dIa request content decoded as an oBIX document.
 * @param targetId the requesting URI of the incoming dIa request.
 * @param reqId the dIa request identifier (to provide for completion).
 * return 0 in case of success, a negative number indicating the error code otherwise.
 */
int iCmnSensorExecuteMethod(t_cmn_sensor *cmn, int app, int cluster, int numm,
    int targetlevel, void *obix, char *targetId, int reqId)
{
  zigbeeNode_t *node;
  azapEndPoint_t *ep;
  m2mZclAbstraction_t *abstraction;
  (void)targetlevel;
  char *operation;

  if (!cmn)
  {
    return ERR_INVALID_NODE;
  }

  node = (zigbeeNode_t *)cmn->cmn_self;
  if (!node)
  {
    return ERR_INVALID_NODE;
  }
  if (-1 != numm)
  {
    ep = node->getEndPoint(node, app);
    if (!ep)
    {
      return ERR_INVALID_EP;
    }
    abstraction = node->getM2mZclAbstraction(node, app, cluster);
    if (!abstraction)
    {
      return ERR_UNSUPPORTED_INTERFACE;
    }

    return abstraction->doZclOperation(abstraction, numm, node, ep, obix, reqId);
  }

  if ((! targetId) || (! *targetId))
  {
    return ERR_INVALID_REQ_URI;
  }

  operation = rindex(targetId, '/');
  operation ++;
  if (!strncmp(operation, "zb", strlen("zb")))
  {
    m2mZdoAbstraction_t *zdo = getZdoAbstraction();
    return zdo->doZdoOperation(zdo, operation, node, app, cluster, obix, reqId);
  }

  // else m2m operation
  m2mAbstraction_t *m2m = getM2mAbstraction();
  return m2m->doM2mOperation(m2m, operation, node, app, cluster, obix, reqId);
}

/**
 * @brief Call back raised on incoming dIa Retrieve request for attribute.
 * Invoked in diaserver.c
 * @param cmn the common structure instance for the targeted sensor.
 * @param app the application number (i.e. the end point identifier targeted on the remote
 * node).
 * @param cluster the cluster identifier the command refers to.
 * @param numm the attribute number the request refers to.
 * @param targetlevel the access right level (i.e. 1, 2 or 3) (useless here)
 * @param targetId the requesting URI of the incoming dIa request.
 * @param reqId the dIa request identifier (to provide for completion).
 * return positive number in case of success, a negative number indicating
 * the error code otherwise.
 */
int iCmnSensorRetrieveAttrValue(t_cmn_sensor *cmn, int app, int cluster, int numm,
    int targetlevel, char *targetId, int reqId)
{
  zigbeeNode_t *node;
  azapEndPoint_t *ep;
  m2mZclAbstraction_t *abstraction;
  (void)targetlevel;
  (void)targetId;
  if (!cmn)
  {
    return ERR_INVALID_NODE;
  }

  node = (zigbeeNode_t *)cmn->cmn_self;
  if (!node)
  {
    return ERR_INVALID_NODE;
  }
  ep = node->getEndPoint(node, app);
  if (!ep)
  {
    return ERR_INVALID_EP;
  }
  abstraction = node->getM2mZclAbstraction(node, app, cluster);
  if (!abstraction)
  {
    return ERR_UNSUPPORTED_INTERFACE;
  }

  return abstraction->getZclAttribute(abstraction, node, ep, numm, reqId);
}

/**
 * @brief Call back raised on incoming dIa Retrieve request for attribute.
 * Invoked in diaserver.c
 * @param cmn the common structure instance for the targeted sensor.
 * @param app the application number (i.e. the end point identifier targeted on the remote
 * node).
 * @param cluster the cluster identifier the command refers to.
 * @param numm the attribute number the request refers to.
 * @param targetlevel the access right level (i.e. 1, 2 or 3) (useless here)
 * @param obix the dIa request content decoded as an oBIX document.
 * @param targetId the requesting URI of the incoming dIa request.
 * @param reqId the dIa request identifier (to provide for completion).
 * return positive number in case of success, a negative number indicating
 * the error code otherwise.
 */
int iCmnSensorUpdateAttrValue(t_cmn_sensor *cmn, int app, int cluster, int numm,
    int targetlevel, void *obix, char *targetId, int reqId)
{
  zigbeeNode_t *node;
  azapEndPoint_t *ep;
  m2mZclAbstraction_t *abstraction;
  (void)targetlevel;
  (void)targetId;
  if (!cmn)
  {
    return ERR_INVALID_NODE;
  }

  node = (zigbeeNode_t *)cmn->cmn_self;
  if (!node)
  {
    return ERR_INVALID_NODE;
  }
  ep = node->getEndPoint(node, app);
  if (!ep)
  {
    return ERR_INVALID_EP;
  }
  abstraction = node->getM2mZclAbstraction(node, app, cluster);
  if (!abstraction)
  {
    return ERR_UNSUPPORTED_INTERFACE;
  }

  return abstraction->setZclAttribute(abstraction, node, ep, numm, obix, reqId);
}


