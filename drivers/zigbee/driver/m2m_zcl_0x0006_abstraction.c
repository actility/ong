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
//** File : driver/m2m_zcl_0x0006_abstraction.c
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
#include "cmnstruct.h"
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
#include "m2m_zcl_0x0006_abstraction.h"
#include "interface_mapping_desc.h"
#include "zigbee_han.h"

extern zigbeeHan_t *theHan;
extern azap_uint8 azapPort;

/*****************************************************************************/
/* Prototypes for local functions */

/*****************************************************************************/
/* Constructors / destructor */

/* static allocation */

/**
 * @brief Initialize the m2mZcl0x0006Abstraction_t struct
 * @param This the class instance
 */
static void m2mZcl0x0006Abstraction_t_init(m2mZcl0x0006Abstraction_t *This)
{
  RTL_TRDBG(TRACE_DETAIL, "m2mZcl0x0006Abstraction_t::init\n");

  /* Assign the pointers on functions */
  This->getConsolidatedData = m2mZclAbstraction_t_getConsolidatedData; // no changes
  This->collectConsolidationElts = m2mZclAbstraction_t_collectConsolidationElts; // no changes
  This->doZclOperation = m2mZcl0x0006Abstraction_t_doZclOperation;
  This->doZclOperationResp = m2mZclAbstraction_t_doZclOperationResp; // no changes
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
 * @brief do dynamic allocation of a m2mZcl0x0006Abstraction_t.
 * @param epId the end point identifier.
 * @return the copy of the object
 */
m2mZcl0x0006Abstraction_t *new_m2mZcl0x0006Abstraction_t(azap_uint8 epId)
{
  m2mZcl0x0006Abstraction_t *This = malloc(sizeof(m2mZcl0x0006Abstraction_t));
  if (!This)
  {
    return NULL;
  }
  m2mZcl0x0006Abstraction_t_init(This);
  This->free = m2mZclAbstraction_t_newFree; // no changes

  This->endPointId = epId;
  This->clusterId = ZCL_CLUSTER_ID_GEN_ON_OFF;

  RTL_TRDBG(TRACE_DETAIL, "new m2mZcl0x0006Abstraction_t (id:0x%.2x.0x%.4x)\n",
      This->endPointId, This->clusterId);
  return This;
}

/* implementation methods */

/**
 * @brief Do invoke the ZCL operation specified by the request URI.
 * @anchor m2mZcl0x0006Abstraction_t_doZclOperation
 * @param This the class instance
 * @param operation the ZCL operation code retrieved from the request URI.
 * @param node the node instance on which the ZCL operation refers to.
 * @param ep the end point on the remote node on which the ZCL operation refers to.
 * @param obixReqContent the request content, already decoded as oBIX content type.
 * @param reqId the dIa request identifier to provide on completion.
 * @return 0 if the ZCL operation can be built and sent to the targeted node, a negative
 * number otherwise.
 */
int m2mZcl0x0006Abstraction_t_doZclOperation(m2mZclAbstraction_t *This,
    azap_uint8 operation, zigbeeNode_t *node, azapEndPoint_t *ep, void *obixReqContent,
    int reqId)
{
  (void)reqId;
  int ret = OK_ONE_WAY_REQUEST;
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
    case COMMAND_ON:
    case COMMAND_OFF:
    case COMMAND_TOGGLE:
      if (! azapZclOutgoingCmdReq(azapPort, AZAP_REQ_TTL_START_AT_API, NULL,
          node->cp_cmn.cmn_num, getMatchingAdminEndPoint(ep->appProfId), ep->id,
          This->clusterId, ZCL_FRAME_CLIENT_SERVER_DIR, false, operation, 0xFF,
          NULL, 0))
      {
        ret = ERR_FAILED_TO_SEND_REQ;
      }
      break;

    default:
    {
      RTL_TRDBG(TRACE_INFO, "m2mZcl0x0006Abstraction_t_doZclOperation - warning: unsupported "
          "ZCL operation (0x%.2x)\n", operation);
      ret = ERR_UNSUPPORTED_OPERATION;
    }
    break;
  }

  return ret;
}


