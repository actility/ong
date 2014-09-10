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
//** File : driver/m2m_zcl_0x0003_abstraction.c
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

#include "rtlbase.h"
#include "rtlimsg.h"

#include "dIa.h"
#include "xoapipr.h"
#include "cmnstruct.h"
#include "cmnproto.h"
#include "adaptor.h"

#include "azap_types.h"
#include "azap_api.h"

#include "azap_zcl_attr_val.h"
#include "azap_attribute.h"
#include "azap_endpoint.h"
#include "driver_req_context.h"
#include "zigbee_node.h"
#include "azap_listener.h"
#include "m2m_abstraction.h"
#include "m2m_zcl_abstraction.h"
#include "m2m_zcl_0x0003_abstraction.h"
#include "driver_req_context.h"
#include "interface_mapping_desc.h"
#include "zigbee_han.h"

extern zigbeeHan_t *theHan;
extern azap_uint8 azapPort;

/*****************************************************************************/
/* Prototypes for local functions */
static int m2mZcl0x0003Abstraction_t_doZclIdentify(m2mZclAbstraction_t *This,
    zigbeeNode_t *node, azapEndPoint_t *ep, void *obixReqContent);
static int m2mZcl0x0003Abstraction_t_doZclIdentifyQuery(m2mZclAbstraction_t *This,
    zigbeeNode_t *node, azapEndPoint_t *ep, void *obixReqContent, int reqId);

/*****************************************************************************/
/* Constructors / destructor */

/* static allocation */

/**
 * @brief Initialize the m2mZcl0x0003Abstraction_t struct
 * @param This the class instance
 */
static void m2mZcl0x0003Abstraction_t_init(m2mZcl0x0003Abstraction_t *This)
{
  RTL_TRDBG(TRACE_DETAIL, "m2mZcl0x0003Abstraction_t::init\n");

  /* Assign the pointers on functions */
  This->getConsolidatedData = m2mZclAbstraction_t_getConsolidatedData; // no changes
  This->collectConsolidationElts = m2mZclAbstraction_t_collectConsolidationElts; // no changes
  This->doZclOperation = m2mZcl0x0003Abstraction_t_doZclOperation;
  This->doZclOperationResp = m2mZcl0x0003Abstraction_t_doZclOperationResp;
  This->getZclAttribute = m2mZclAbstraction_t_getZclAttribute; // no changes
  This->setZclAttribute = m2mZclAbstraction_t_setZclAttribute; // no changes
  This->configureReporting = m2mZclAbstraction_t_configureReporting; // no changes
  This->canReport = m2mZclAbstraction_t_canReport; // no changes

  /* Initialize the members */
  This->endPointId = 0;
  This->clusterId = 0;
}

/* dynamic allocation */
/**
 * @brief do dynamic allocation of a m2mZcl0x0003Abstraction_t.
 * @param epId the end point identifier.
 * @return the copy of the object
 */
m2mZcl0x0003Abstraction_t *new_m2mZcl0x0003Abstraction_t(azap_uint8 epId)
{
  m2mZcl0x0003Abstraction_t *This = malloc(sizeof(m2mZcl0x0003Abstraction_t));
  if (!This)
  {
    return NULL;
  }
  m2mZcl0x0003Abstraction_t_init(This);
  This->free = m2mZclAbstraction_t_newFree; // no changes

  This->endPointId = epId;
  This->clusterId = ZCL_CLUSTER_ID_GEN_IDENTIFY;

  RTL_TRDBG(TRACE_DETAIL, "new m2mZcl0x0003Abstraction_t (id:0x%.2x.0x%.4x)\n",
      This->endPointId, This->clusterId);
  return This;
}

/* implementation methods */

/**
 * @brief Do invoke the ZCL operation specified by the request URI.
 * @anchor m2mZcl0x0003Abstraction_t_doZclOperation
 * @param This the class instance
 * @param operation the ZCL operation code retrieved from the request URI.
 * @param node the node instance on which the ZCL operation refers to.
 * @param ep the end point on the remote node on which the ZCL operation refers to.
 * @param obixReqContent the request content, already decoded as oBIX content type.
 * @param reqId the dIa request identifier to provide on completion.
 * @return 0 if the ZCL operation can be built and sent to the targeted node, a negative
 * number otherwise.
 */
int m2mZcl0x0003Abstraction_t_doZclOperation(m2mZclAbstraction_t *This,
    azap_uint8 operation, zigbeeNode_t *node, azapEndPoint_t *ep, void *obixReqContent,
    int reqId)
{
  int ret = ERR_UNSUPPORTED_OPERATION;
  if (! node)
  {
    return ERR_INVALID_NODE;
  }
  if (! ep)
  {
    return ERR_INVALID_EP;
  }

  switch (operation)
  {
    case COMMAND_IDENTIFY:
      ret = m2mZcl0x0003Abstraction_t_doZclIdentify(This, node, ep, obixReqContent);
      break;

    case COMMAND_IDENTIFY_QUERY:
      ret = m2mZcl0x0003Abstraction_t_doZclIdentifyQuery(This, node, ep, obixReqContent,
          reqId);
      break;

    default:
    {
      RTL_TRDBG(TRACE_INFO, "m2mZcl0x0003Abstraction_t_doZclOperation - warning: unsupported "
          "ZCL operation (0x%.2x)\n", operation);
    }
    break;
  }

  return ret;
}


/**
 * @brief Do invoke the ZCL "Identify" command.
 * @param This the class instance
 * @param node the node instance on which the ZCL operation refers to.
 * @param ep the end point on the remote node on which the ZCL operation refers to.
 * @param obixReqContent the request content, already decoded as oBIX content type.
 * @return 0 if the ZCL operation can be built and sent to the targeted node, a negative
 * number otherwise.
 */
static int m2mZcl0x0003Abstraction_t_doZclIdentify(m2mZclAbstraction_t *This,
    zigbeeNode_t *node, azapEndPoint_t *ep, void *obixReqContent)
{
  int res = OK_ONE_WAY_REQUEST;
  int identifyTime;
  azap_uint8 payload[2];
  azap_uint8 len = 2;
  azap_uint8 *pPtr = payload;
  azapZclAttrVal_t *zclIdentifyTime;
  if (!obixReqContent)
  {
    return ERR_CONTENT_MISSING;
  }

  if ((res = getObixIntValue(obixReqContent, "zbIdentifyTime", &identifyTime)) < 0)
  {
    RTL_TRDBG(TRACE_ERROR, "m2mZcl0x0003Abstraction_t_doZclIdentify - error: unable "
        "to find parameter zbIdentifyTime in request content\n");
    return ERR_INVALID_ARG_1;
  }

  zclIdentifyTime = new_azapZclAttrVal_t(ZCL_DATATYPE_UINT16);
  zclIdentifyTime->setIntegerValue(zclIdentifyTime, identifyTime);
  zclIdentifyTime->serialize(zclIdentifyTime, &pPtr, &len);
  zclIdentifyTime->free(zclIdentifyTime);

  if (! azapZclOutgoingCmdReq(azapPort, AZAP_REQ_TTL_START_AT_API, NULL,
      node->cp_cmn.cmn_num, getMatchingAdminEndPoint(ep->appProfId), ep->id, This->clusterId,
      ZCL_FRAME_CLIENT_SERVER_DIR, false, COMMAND_IDENTIFY, 0xFF, payload, sizeof(payload)))
  {
    res = ERR_FAILED_TO_SEND_REQ;
  }

  return res;
}

/**
 * @brief Do invoke the ZCL "IdentifyQuery" command.
 * @param This the class instance
 * @param node the node instance on which the ZCL operation refers to.
 * @param ep the end point on the remote node on which the ZCL operation refers to.
 * @param obixReqContent the request content, already decoded as oBIX content type.
 * @param reqId the dIa request identifier to provide on completion.
 * @return 0 if the ZCL operation can be built and sent to the targeted node, a negative
 * number otherwise.
 */
static int m2mZcl0x0003Abstraction_t_doZclIdentifyQuery(m2mZclAbstraction_t *This,
    zigbeeNode_t *node, azapEndPoint_t *ep, void *obixReqContent, int reqId)
{
  driverReqContext_t *context = new_driverReqContext_t(DRV_REQ_OPERATION_FROM_DIA);
  context->issuerData = (void *)((long)reqId);

  if (! azapZclOutgoingCmdReq(azapPort, AZAP_REQ_TTL_START_AT_API, context,
      node->cp_cmn.cmn_num, getMatchingAdminEndPoint(ep->appProfId), ep->id,
      This->clusterId, ZCL_FRAME_CLIENT_SERVER_DIR, false, COMMAND_IDENTIFY_QUERY,
      COMMAND_IDENTIFY_QUERY_RSP, NULL, 0))
  {
    context->free(context);
    return ERR_FAILED_TO_SEND_REQ;
  }

  return OK_WAITING_FOR_COMPLETION;
}

/**
 * @brief Interpret the ZCL command response depending on the abstraction context.
 * Here in this situation, no command response is expected, so ignore the command (?)
 * @anchor m2mZclAbstraction_t_doZclOperationResp
 * @param This the class instance
 * @param operation the ZCL command code received from the device.
 * @param cmdData the received ZCL command payload.
 * @param cmdLen the payload length.
 * @param reqId the dIa request identifier to provide for completion.
 */
void m2mZcl0x0003Abstraction_t_doZclOperationResp(m2mZclAbstraction_t *This,
    azap_uint8 operation, azap_byte *cmdData, azap_uint8 cmdLen, int reqId)
{
  int size = 0;
  char *statusCode;
  char *type;
  char *content = NULL;
  azap_uint8 len = 2;
  azap_byte *pPtr;
  azapZclAttrVal_t *attr;
  void *xo;

  if (COMMAND_IDENTIFY_QUERY_RSP == operation)
  {
    // ok, so decode the payload
    attr = new_azapZclAttrVal_t(ZCL_DATATYPE_INT16);
    pPtr = cmdData;
    attr->deserialize(attr, &pPtr, &len);
    SetVar("r_value", attr->getStringRep(attr));
    xo = WXoNewTemplate("ope_resp_0003_00.xml", XOML_PARSE_OBIX);

    //XoSaveAscii(xo,stdout);

    if (xo)
    {
      content = WXoSerializeFree(xo, XO_FMT_STD_XML, XOML_PARSE_OBIX,
          &size, &type);
      statusCode = "STATUS_CREATED";
    }
    else
    {
      statusCode = "STATUS_INTERNAL_SERVER_ERROR";
    }
    // and invoke the dIa completion
    diaIncomingRequestCompletion(reqId, content, size, type, statusCode);
    if (content)
    {
      free(content);
    }
  }
  // else not supported yet
}

