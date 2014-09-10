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
//** File : driver/m2m_zdo_abstraction.c
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
#include <strings.h>
#include <limits.h>

#include <rtlbase.h>
#include <rtlimsg.h>

#include "dIa.h"
#include "cmnstruct.h"
#include "adaptor.h"

#include "azap_types.h"
#include "azap_api.h"
#include "azap_tools.h"

#include "azap_zcl_attr_val.h"
#include "azap_attribute.h"
#include "azap_endpoint.h"
#include "driver_req_context.h"
#include "zigbee_node.h"
#include "m2m_abstraction.h"
#include "m2m_zdo_abstraction.h"
#include "driver_req_context.h"
#include "interface_mapping_desc.h"
#include "zigbee_han.h"

#define MAX_STR_PARAM_SIZE 32

extern azap_uint8 azapPort;
extern zigbeeHan_t *theHan;

/*****************************************************************************/
/* Prototypes for local functions */
static void m2mZdoAbstraction_t_init(m2mZdoAbstraction_t *This);
static int m2mZdoAbstraction_t_doZdoMgmtPermitJoin(m2mZdoAbstraction_t *This,
    void *obixReqContent, int reqId);
static int m2mZdoAbstraction_t_doZdoMgmtBind(m2mZdoAbstraction_t *This,
    zigbeeNode_t *node, void *obixReqContent, int reqId);
static int m2mZdoAbstraction_t_doZdoMgmtRtg(m2mZdoAbstraction_t *This,
    zigbeeNode_t *node, void *obixReqContent, int reqId);
static int m2mZdoAbstraction_t_doZdoMgmtLeave(m2mZdoAbstraction_t *This,
    zigbeeNode_t *node, void *obixReqContent, int reqId);
static int m2mZdoAbstraction_t_doZdoBindUnbind(m2mZdoAbstraction_t *This,
    zigbeeNode_t *node, azap_uint8 epId, azap_uint16 clId,
    void *obixReqContent, bool bindOp, int reqId);


/*****************************************************************************/
/* singleton manager */
m2mZdoAbstraction_t *getZdoAbstraction()
{
  static m2mZdoAbstraction_t *zdoAbstraction = NULL;

  if (! zdoAbstraction)
  {
    zdoAbstraction = new_m2mZdoAbstraction_t();
  }

  return zdoAbstraction;
}

/* Constructors / destructor */

/* static allocation */

/**
 * @brief Initialize the m2mZdoAbstraction_t struct
 * @param This the class instance
 */
static void m2mZdoAbstraction_t_init(m2mZdoAbstraction_t *This)
{
  RTL_TRDBG(TRACE_DETAIL, "m2mZdoAbstraction_t::init\n");

  /* Assign the pointers on functions */
  This->doZdoOperation = m2mZdoAbstraction_t_doZdoOperation;

  /* Initialize the members */
}

/* dynamic allocation */
/**
 * @brief do dynamic allocation of a m2mZdoAbstraction_t.
 * @return the copy of the object
 */
m2mZdoAbstraction_t *new_m2mZdoAbstraction_t()
{
  m2mZdoAbstraction_t *This = malloc(sizeof(m2mZdoAbstraction_t));
  if (!This)
  {
    return NULL;
  }
  m2mZdoAbstraction_t_init(This);
  This->free = m2mZdoAbstraction_t_newFree;

  RTL_TRDBG(TRACE_DETAIL, "new m2mZdoAbstraction_t\n");
  return This;
}

/**
 * @brief Destructor for dynamic allocation
 * @anchor m2mZdoAbstraction_t_newFree
 * @param This the class instance
 */
void m2mZdoAbstraction_t_newFree(m2mZdoAbstraction_t *This)
{
  RTL_TRDBG(TRACE_DETAIL, "m2mZdoAbstraction_t::newFree\n");
  free(This);
}

/* implementation methods */
/**
 * @brief Do invoke the ZDO operation specified by the request URI.
 * @anchor m2mZdoAbstraction_t_doZdoOperation
 * @param This the class instance
 * @param operation the operation name extracted from the request URI received in the dIa message.
 * @param node the node instance on which the ZDO operation refers to.
 * @param epId the end point identifier on the remote node on which the ZDO operation refers to.
 * @param clId the cluster identifier (if applicable)
 * @param obixReqContent the request content, already decoded as oBIX content type.
 * @param reqId the dIa request identifier to provide for completion.
 * @return 1 if the ZDO operation can be built and sent to the targeted node, 0 otherwise.
 */
int m2mZdoAbstraction_t_doZdoOperation(m2mZdoAbstraction_t *This, char *operation,
    zigbeeNode_t *node, azap_uint8 epId, azap_uint16 clId, void *obixReqContent, int reqId)
{
  int ret = ERR_UNSUPPORTED_OPERATION;

  if (0 == strcmp(operation, "zbBind"))
  {
    ret = m2mZdoAbstraction_t_doZdoBindUnbind(This, node, epId, clId,
        obixReqContent, true, reqId);
  }
  else if (0 == strcmp(operation, "zbUnbind"))
  {
    ret = m2mZdoAbstraction_t_doZdoBindUnbind(This, node, epId, clId,
        obixReqContent, false, reqId);
  }
  else if (0 == strcmp(operation, "zbMgmtPermitJoin"))
  {
    ret = m2mZdoAbstraction_t_doZdoMgmtPermitJoin(This, obixReqContent, reqId);
  }
  else if (0 == strcmp(operation, "zbMgmtRtg"))
  {
    ret = m2mZdoAbstraction_t_doZdoMgmtRtg(This, node, obixReqContent, reqId);
  }
  else if (0 == strcmp(operation, "zbMgmtBind"))
  {
    ret = m2mZdoAbstraction_t_doZdoMgmtBind(This, node, obixReqContent, reqId);
  }
  else if (0 == strcmp(operation, "zbMgmtLeave"))
  {
    ret = m2mZdoAbstraction_t_doZdoMgmtLeave(This, node, obixReqContent, reqId);
  }
  else
  {
    RTL_TRDBG(TRACE_ERROR, "m2mZdoAbstraction_t_doZdoOperation - error: unsupported "
        "ZDO operation (%s)\n", operation);
  }

  return ret;
}


/**
 * @brief Do invoke the ZDO MgmtPermitJoint operation.
 * @param This the class instance
 * @param obixReqContent the request content, already decoded as oBIX content type.
 * @param reqId the dIa request identifier to provide for completion.
 * @return 0 if the ZDO operation can be built and sent to the targeted node, an
 * error code (negative number) otherwise.
 */
static int m2mZdoAbstraction_t_doZdoMgmtPermitJoin(m2mZdoAbstraction_t *This,
    void *obixReqContent, int reqId)
{
  driverReqContext_t *context;
  int res = OK_WAITING_FOR_COMPLETION;
  int duration = 0;
  int rc;

  if (! obixReqContent)
  {
    RTL_TRDBG(TRACE_ERROR, "m2mZdoAbstraction_t::doZdoMgmtPermitJoin - error: "
        "oBIX content is missing\n");
    res = ERR_CONTENT_MISSING;
  }
  else if ((rc = getObixIntValue(obixReqContent, "zbPermitDuration", &duration)) < 0)
  {
    RTL_TRDBG(TRACE_ERROR, "m2mZdoAbstraction_t::doZdoMgmtPermitJoin - error: "
        "zbPermitDuration is missing (rc:%d)\n", rc);
    res = ERR_INVALID_ARG_1;
  }
  else
  {
    if (duration == 255)
    {
      theHan->computeNextPermitJoinDuration(theHan, INT_MAX);
    }
    else
    {
      theHan->computeNextPermitJoinDuration(theHan, duration);
    }

    context = new_driverReqContext_t(DRV_REQ_OPERATION_FROM_DIA);
    context->issuerData = (void *)((long)reqId);
    // first sent to the coordinator,
    if (! azapZdpMgmtPermitJoinReq(azapPort, AZAP_REQ_TTL_START_AT_API, context,
        0x0000, duration, true, true))
    {
      context->free(context);
      res = ERR_FAILED_TO_SEND_REQ;
    }
    // and also broadcast it on the HAN
    if (! azapZdpMgmtPermitJoinReq(azapPort, AZAP_REQ_TTL_START_AT_API, NULL, 0xFFFF,
        duration, true, true))
    {
      res = ERR_FAILED_TO_SEND_REQ;
    }

    if (res != ERR_FAILED_TO_SEND_REQ)
    {
      // start timer for next occurance.
    }
  }
  return res;
}

/**
 * @brief Do invoke the ZDO MgmtRtg operation.
 * @param This the class instance
 * @param node the node instance on which the ZDO operation refers to.
 * @param obixReqContent the request content, already decoded as oBIX content type.
 * @param reqId the dIa request identifier to provide for completion.
 * @return 0 if the ZDO operation can be built and sent to the targeted node, an
 * error code (negative number) otherwise.
 */
static int m2mZdoAbstraction_t_doZdoMgmtRtg(m2mZdoAbstraction_t *This,
    zigbeeNode_t *node, void *obixReqContent, int reqId)
{
  driverReqContext_t *context;
  int res = OK_WAITING_FOR_COMPLETION;
  int index = 0;
  int rc;

  if (! node)
  {
    return ERR_INVALID_NODE;
  }
  if (! obixReqContent)
  {
    RTL_TRDBG(TRACE_ERROR, "m2mZdoAbstraction_t::doZdoMgmtRtg - error: "
        "oBIX content is missing\n");
    res = ERR_CONTENT_MISSING;
  }
  else if ((rc = getObixIntValue(obixReqContent, "zbStartIndex", &index)) < 0)
  {
    RTL_TRDBG(TRACE_ERROR, "m2mZdoAbstraction_t::doZdoMgmtRtg - error: "
        "zbStartIndex is missing (rc:%d)\n", rc);
    res = ERR_INVALID_ARG_1;
  }
  else
  {
    context = new_driverReqContext_t(DRV_REQ_OPERATION_FROM_DIA);
    context->issuerData = (void *)((long)reqId);
    // sent the request
    if (! azapZdpMgmtRtgReq(azapPort, AZAP_REQ_TTL_START_AT_API, context,
        node->cp_cmn.cmn_num, index))
    {
      context->free(context);
      res = ERR_FAILED_TO_SEND_REQ;
    }
  }
  return res;
}

/**
 * @brief Do invoke the ZDO MgmtBind operation.
 * @param This the class instance
 * @param node the node instance on which the ZDO operation refers to.
 * @param obixReqContent the request content, already decoded as oBIX content type.
 * @param reqId the dIa request identifier to provide for completion.
 * @return 0 if the ZDO operation can be built and sent to the targeted node, an
 * error code (negative number) otherwise.
 */
static int m2mZdoAbstraction_t_doZdoMgmtBind(m2mZdoAbstraction_t *This,
    zigbeeNode_t *node, void *obixReqContent, int reqId)
{
  driverReqContext_t *context;
  int res = OK_WAITING_FOR_COMPLETION;
  int index = 0;
  int rc;

  if (! node)
  {
    return ERR_INVALID_NODE;
  }
  if (! obixReqContent)
  {
    RTL_TRDBG(TRACE_ERROR, "m2mZdoAbstraction_t::doZdoMgmtBind - error: "
        "oBIX content is missing\n");
    res = ERR_CONTENT_MISSING;
  }
  else if ((rc = getObixIntValue(obixReqContent, "zbStartIndex", &index)) < 0)
  {
    RTL_TRDBG(TRACE_ERROR, "m2mZdoAbstraction_t::doZdoMgmtBind - error: "
        "zbStartIndex is missing (rc:%d)\n", rc);
    res = ERR_INVALID_ARG_1;
  }
  else
  {
    context = new_driverReqContext_t(DRV_REQ_OPERATION_FROM_DIA);
    context->issuerData = (void *)((long)reqId);
    // sent the request
    if (! azapZdpMgmtBindReq(azapPort, AZAP_REQ_TTL_START_AT_API, context,
        node->cp_cmn.cmn_num, index))
    {
      context->free(context);
      res = ERR_FAILED_TO_SEND_REQ;
    }
  }
  return res;
}


/**
 * @brief Do invoke the ZDO MgmtLeave operation.
 * @param This the class instance
 * @param node the node instance on which the ZDO operation refers to.
 * @param obixReqContent the request content, already decoded as oBIX content type.
 * @param reqId the dIa request identifier to provide for completion.
 * @return 0 if the ZDO operation can be built and sent to the targeted node, an
 * error code (negative number) otherwise.
 */
static int m2mZdoAbstraction_t_doZdoMgmtLeave(m2mZdoAbstraction_t *This,
    zigbeeNode_t *node, void *obixReqContent, int reqId)
{
  driverReqContext_t *context;
  // when the device leaves, it does not necessarily send a response...
  int res = OK_ONE_WAY_REQUEST;
  bool removeChildren = false;
  bool rejoin = false;
  int rc;

  if (! node)
  {
    return ERR_INVALID_NODE;
  }
  if (! obixReqContent)
  {
    RTL_TRDBG(TRACE_ERROR, "m2mZdoAbstraction_t::doZdoMgmtLeave - error: "
        "oBIX content is missing\n");
    res = ERR_CONTENT_MISSING;
  }
  else if ((rc = getObixBoolValue(obixReqContent, "zbRemoveChildren", &removeChildren)) < 0)
  {
    RTL_TRDBG(TRACE_ERROR, "m2mZdoAbstraction_t::doZdoMgmtLeave - error: "
        "zbRemoveChildren is missing (rc:%d)\n", rc);
    res = ERR_INVALID_ARG_2;
  }
  else if ((rc = getObixBoolValue(obixReqContent, "zbRejoin", &rejoin)) < 0)
  {
    RTL_TRDBG(TRACE_ERROR, "m2mZdoAbstraction_t::doZdoMgmtLeave - error: "
        "zbRejoin is missing (rc:%d)\n", rc);
    res = ERR_INVALID_ARG_3;
  }
  else
  {
    context = new_driverReqContext_t(DRV_REQ_OPERATION_FROM_DIA);
    context->issuerData = (void *)((long)reqId);
    // sent the request
    if (! azapZdpMgmtLeaveReq(azapPort, AZAP_REQ_TTL_START_AT_API, context,
        node->cp_cmn.cmn_num, node->ieeeAddr, removeChildren, rejoin))
    {
      context->free(context);
      res = ERR_FAILED_TO_SEND_REQ;
    }
  }
  return res;
}


/**
 * @brief Do invoke the ZDO Bind or ZDO Unbind operation.
 * @param This the class instance
 * @param node the node instance on which the ZDO operation refers to.
 * @param epId the end point identifier on the remote node on which the ZDO operation refers to.
 * @param clId the cluster identifier.
 * @param obixReqContent the request content, already decoded as oBIX content type.
 * @param bindOp set to 1 if ZDO Bind is requested, 0 if ZDO Unbind
 * @param reqId the dIa request identifier to provide for completion.
 * @return 1 if the ZDO operation can be built and sent to the targeted node, an
 * error code (negative number) otherwise.
 */
static int m2mZdoAbstraction_t_doZdoBindUnbind(m2mZdoAbstraction_t *This, zigbeeNode_t *node,
    azap_uint8 epId, azap_uint16 clId, void *obixReqContent, bool bindOp, int reqId)
{
  azapEndPoint_t *ep;
  char addrMode[MAX_STR_PARAM_SIZE];
  char destIEEE[MAX_STR_PARAM_SIZE];
  azap_uint8 dstExtAddr[IEEE_ADDR_LEN];
  int dstEp;
  int rc;
  driverReqContext_t *context;

  if (! node)
  {
    return ERR_INVALID_NODE;
  }
  if (! obixReqContent)
  {
    return ERR_CONTENT_MISSING;
  }

  ep = node->getEndPoint(node, epId);
  if (! ep)
  {
    return ERR_INVALID_EP;
  }

  if ((rc = getObixStrValue(obixReqContent, "zbDstAddrMode", addrMode, MAX_STR_PARAM_SIZE)) < 0)
  {
    RTL_TRDBG(TRACE_ERROR, "m2mZdoAbstraction_t::doZdoBind - error: "
        "zbDstAddrMode is missing (rc:%d)\n", rc);
    return ERR_INVALID_ARG_1;
  }

  if (0 != strcmp(addrMode, "0x03"))
  {
    RTL_TRDBG(TRACE_ERROR, "m2mZdoAbstraction_t::doZdoBind - error: "
        "unsupported zbDstAddrMode (%s)\n", addrMode);
    return ERR_INVALID_ARG_1;
  }

  if ((rc = getObixStrValue(obixReqContent, "zbDstAddress", destIEEE, MAX_STR_PARAM_SIZE)) < 0)
  {
    RTL_TRDBG(TRACE_ERROR, "m2mZdoAbstraction_t::doZdoBind - error: "
        "zbDstAddress is missing (rc:%d)\n", rc);
    return ERR_INVALID_ARG_2;
  }

  if (! stringToAzap_extAddr(dstExtAddr, destIEEE))
  {
    RTL_TRDBG(TRACE_ERROR, "m2mZdoAbstraction_t::doZdoBind - error: "
        "invalid zbDstAddress (%s)\n", destIEEE);
    return ERR_INVALID_ARG_2;
  }

  if ((rc = getObixIntValue(obixReqContent, "zbDstEndp", &dstEp)) < 0)
  {
    RTL_TRDBG(TRACE_ERROR, "m2mZdoAbstraction_t::doZdoBind - error: "
        "zbDstEndp is missing (rc:%d)\n", rc);
    return ERR_INVALID_ARG_3;
  }

  context = new_driverReqContext_t(DRV_REQ_OPERATION_FROM_DIA);
  context->issuerData = (void *)((long)reqId);

  if (bindOp)
  {
    if (! azapZdpBindReq(azapPort, AZAP_REQ_TTL_START_AT_API, context,
        node->cp_cmn.cmn_num, node->ieeeAddr, ep->id, clId, dstExtAddr,
        dstEp))
    {
      context->free(context);
      return ERR_FAILED_TO_SEND_REQ;
    }
  }
  else
  {
    if (! azapZdpUnbindReq(azapPort, AZAP_REQ_TTL_START_AT_API, context,
        node->cp_cmn.cmn_num, node->ieeeAddr, ep->id, clId, dstExtAddr,
        dstEp))
    {
      context->free(context);
      return ERR_FAILED_TO_SEND_REQ;
    }
  }

  return OK_WAITING_FOR_COMPLETION;
}



