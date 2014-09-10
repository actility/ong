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
//** File : driver/m2m_zcl_abstraction.c
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

#include <rtlbase.h>
#include <rtlimsg.h>

#include "dIa.h"
#include "cmnstruct.h"
#include "cmnproto.h"
#include "xoapipr.h"
#include "adaptor.h"

#include "azap_types.h"
#include "azap_api.h"
#include "azap_api_zcl.h"

#include "azap_zcl_attr_val.h"
#include "azap_attribute.h"
#include "azap_endpoint.h"
#include "driver_req_context.h"
#include "zigbee_node.h"
#include "m2m_abstraction.h"
#include "m2m_zcl_abstraction.h"
#include "driver_req_context.h"
#include "azap_listener.h"
#include "interface_mapping_desc.h"
#include "zigbee_han.h"

extern azap_uint8 azapPort;
extern zigbeeHan_t *theHan;

/*****************************************************************************/
/* Prototypes for local functions */

/*****************************************************************************/
/* Constructors / destructor */

/* static allocation */

/**
 * @brief Initialize the m2mZclAbstraction_t struct
 * @param This the class instance
 */
void m2mZclAbstraction_t_init(m2mZclAbstraction_t *This)
{
  RTL_TRDBG(TRACE_DETAIL, "m2mZclAbstraction_t::init\n");

  /* Assign the pointers on functions */
  This->getConsolidatedData = m2mZclAbstraction_t_getConsolidatedData;
  This->collectConsolidationElts = m2mZclAbstraction_t_collectConsolidationElts;
  This->doZclOperation = m2mZclAbstraction_t_doZclOperation;
  This->doZclOperationResp = m2mZclAbstraction_t_doZclOperationResp;
  This->getZclAttribute = m2mZclAbstraction_t_getZclAttribute;
  This->setZclAttribute = m2mZclAbstraction_t_setZclAttribute;
  This->configureReporting = m2mZclAbstraction_t_configureReporting;
  This->canReport = m2mZclAbstraction_t_canReport;

  /* Initialize the members */
  This->endPointId = 0;
  This->clusterId = 0;
}

/* dynamic allocation */
/**
 * @brief do dynamic allocation of a m2mZclAbstraction_t.
 * @param epId  the end point identifier
 * @param clId  the cluster identifier
 * @return the copy of the object
 */
m2mZclAbstraction_t *new_m2mZclAbstraction_t(azap_uint8 epId, azap_uint16 clId)
{
  m2mZclAbstraction_t *This = malloc(sizeof(m2mZclAbstraction_t));
  if (!This)
  {
    return NULL;
  }
  m2mZclAbstraction_t_init(This);
  This->free = m2mZclAbstraction_t_newFree;

  This->endPointId = epId;
  This->clusterId = clId;

  RTL_TRDBG(TRACE_DETAIL, "new m2mZclAbstraction_t (id:0x%.2x.0x%.4x)\n", epId, clId);
  return This;
}

/**
 * @brief Destructor for dynamic allocation
 * @anchor m2mZclAbstraction_t_newFree
 * @param This the class instance
 */
void m2mZclAbstraction_t_newFree(m2mZclAbstraction_t *This)
{
  RTL_TRDBG(TRACE_DETAIL, "m2mZclAbstraction_t::newFree (id:0x%.2x.0x%.4x)\n",
      This->endPointId, This->clusterId);
  free(This);
}

/* implementation methods */
/**
 * @brief Run the consolidation data method associated to the cluster identifier.
 * @anchor m2mZclAbstraction_t_getConsolidatedData
 * @param This the class instance
 * @param attrId the attribute identifier on which we need a data consolidation.
 * @param val the value to be consolidated.
 */
char *m2mZclAbstraction_t_getConsolidatedData(m2mZclAbstraction_t *This,
    azap_uint16 attrId, azapZclAttrVal_t *val)
{
  (void)This;
  (void)attrId;
  return val->getStringRep(val);
}

/**
 * @brief Collect data that are mandatory for consolidation on ZCL reports.
 * @anchor m2mZclAbstraction_t_collectConsolidationElts
 * @param This the class instance
 * @param ep the representation of the remote end point application.
 */
void m2mZclAbstraction_t_collectConsolidationElts(m2mZclAbstraction_t *This,
    azapEndPoint_t *ep)
{
  (void)This;
  (void)ep;
  // nothing to do
}

/**
 * @brief Send a AZAP request to retrieve the attribute value.
 * @anchor m2mZclAbstraction_t_getZclAttribute
 * @param This the class instance
 * @param node the node instance on which the ZCL operation refers to.
 * @param ep the end point on the remote node on which the ZCL operation refers to.
 * @param attrId the identifier of the attribute to retrieve.
 * @param reqId the dIa request identifier to provide for completion.
 * @return 1 if the ZCL read operation can be built and sent to the targeted node, a
 * negative number indicating the error reason otherwise.
 */
int m2mZclAbstraction_t_getZclAttribute(m2mZclAbstraction_t *This,
    zigbeeNode_t *node, azapEndPoint_t *ep, azap_uint16 attrId, int reqId)
{
  int ret = OK_WAITING_FOR_COMPLETION;
  driverReqContext_t *context;
  azapZclReadCommand_t *readCmd;
  if (! node)
  {
    return ERR_INVALID_NODE;
  }
  if (! ep)
  {
    return ERR_INVALID_EP;
  }

  readCmd = malloc((1 * sizeof(azap_uint16)) + sizeof(azapZclReadCommand_t) );
  readCmd->numAttr = 1;
  readCmd->attrId[0] = attrId;

  context = new_driverReqContext_t(DRV_REQ_READ_ATTR_FROM_DIA);
  context->issuerData = (void *)((long)reqId);

  if (! azapZclReadCmdReq(azapPort, AZAP_REQ_TTL_START_AT_API, context,
      node->cp_cmn.cmn_num, getMatchingAdminEndPoint(ep->appProfId),
      ep->id, This->clusterId, ZCL_FRAME_CLIENT_SERVER_DIR, 0, readCmd))
  {
    context->free(context);
    ret = ERR_FAILED_TO_SEND_REQ;
  }
  free(readCmd);

  return ret;
}

/**
 * @brief Send a AZAP request to update the attribute value.
 * @anchor m2mZclAbstraction_t_setZclAttribute
 * @param This the class instance
 * @param node the node instance on which the ZCL operation refers to.
 * @param ep the end point on the remote node on which the ZCL operation refers to.
 * @param attrId the identifier of the attribute to retrieve.
 * @param obix the dIa request content decoded as oBIX format.
 * @param reqId the dIa request identifier to provide for completion.
 * @return 1 if the ZCL write operation can be built and sent to the targeted node, 0 otherwise.
 */
int m2mZclAbstraction_t_setZclAttribute(m2mZclAbstraction_t *This,
    zigbeeNode_t *node, azapEndPoint_t *ep, azap_uint16 attrId, void *obix, int reqId)
{
  int ret = OK_WAITING_FOR_COMPLETION;
  driverReqContext_t *context;
  azapZclDiscoverCommand_t discoverCmd;
  char *val = NULL;
  if (! node)
  {
    return ERR_INVALID_NODE;
  }
  if (! ep)
  {
    return ERR_INVALID_EP;
  }
  if (! obix)
  {
    return ERR_CONTENT_MISSING;
  }

  if (NULL == (val = XoNmGetAttr(obix, "val", NULL)))
  {
    return ERR_INVALID_ARG_1;
  }

  discoverCmd.startAttrId = attrId;
  discoverCmd.maxNumAttrIds = 1;

  context = new_driverReqContext_t(DRV_REQ_WRITE_ATTR_FROM_DIA);
  context->issuerData = (void *)((long)reqId);
  context->issuerData2 = malloc(strlen(val) + 1);
  sprintf(context->issuerData2, "%s", val);

  if (!azapZclDiscoverCmdReq(azapPort, AZAP_REQ_TTL_START_AT_API, context,
      node->cp_cmn.cmn_num, getMatchingAdminEndPoint(ep->appProfId),
      ep->id, This->clusterId,
      ZCL_FRAME_CLIENT_SERVER_DIR, &discoverCmd))
  {
    free(context->issuerData2);
    context->free(context);
    return ERR_FAILED_TO_SEND_REQ;
  }

  return ret;
}

/**
 * @brief Do invoke the ZCL operation specified by the request URI.
 * @anchor m2mZclAbstraction_t_doZclOperation
 * @param This the class instance
 * @param operation the ZCL operation code retrieved from the request URI.
 * @param node the node instance on which the ZCL operation refers to.
 * @param ep the end point on the remote node on which the ZCL operation refers to.
 * @param obixReqContent the request content, already decoded as oBIX content type.
 * @param reqId the dIa request identifier to provide for completion.
 * @return 1 if the ZCL operation can be built and sent to the targeted node, 0 otherwise.
 */
int m2mZclAbstraction_t_doZclOperation(m2mZclAbstraction_t *This, azap_uint8 operation,
    zigbeeNode_t *node, azapEndPoint_t *ep, void *obixReqContent, int reqId)
{
  int ret = ERR_UNSUPPORTED_OPERATION;

  RTL_TRDBG(TRACE_ERROR, "m2mZclAbstraction_t_doZclOperation - error: unsupported "
      "ZCL operation (0x%.2x)\n", operation);

  return ret;
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
void m2mZclAbstraction_t_doZclOperationResp(m2mZclAbstraction_t *This,
    azap_uint8 operation, azap_byte *cmdData, azap_uint8 cmdLen, int reqId)
{
  int size = 0;
  char *statusCode;
  char *type;
  char *content = getErrorInfoResponse("ZCL", ZCL_STATUS_FAILURE, &statusCode,
      &size, &type);
  diaIncomingRequestCompletion((int)reqId, content, size, type, statusCode);
  if (content)
  {
    free(content);
  }
}

/**
 * @brief Collect the data regarding configuration on ZCL reporting.
 * @anchor m2mZclAbstraction_t_configureReporting
 * @param This the class instance
 * @param cfgReportCmd the ZCL command that will be sent to the device.
 */
void m2mZclAbstraction_t_configureReporting(m2mZclAbstraction_t *This,
    azapZclCfgReportCommand_t *cfgReportCmd)
{
  // nothing to do
}

/**
 * @brief Indicates if the report can be forwarded to the SCL or not.
 * To determine if the report can be forwarded to the SCL, one must verify when the last
 * report happens, and check if it is not too frequent.
 * @anchor m2mZclAbstraction_t_canReport
 * @param This the class instance
 * @param attrId the attribute identifier on which we need a data consolidation.
 * @param return true if ZCL report can be forwarded to SCL, false otherwise.
 */
bool m2mZclAbstraction_t_canReport(m2mZclAbstraction_t *This, azap_uint16 attrId)
{
  return true;
}


