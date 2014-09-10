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
//** File : driver/m2m_zcl_0x0702_abstraction.c
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
#include <math.h>

#include "rtlbase.h"
#include "rtlimsg.h"
#include "xoapipr.h"

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
#include "azap_listener.h"
#include "m2m_zcl_abstraction.h"
#include "m2m_zcl_0x0702_abstraction.h"
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
 * @brief Initialize the m2mZcl0x0702Abstraction_t struct
 * @param This the class instance
 */
static void m2mZcl0x0702Abstraction_t_init(m2mZcl0x0702Abstraction_t *This)
{
  RTL_TRDBG(TRACE_DETAIL, "m2mZcl0x0702Abstraction_t::init\n");

  /* Assign the pointers on functions */
  This->getConsolidatedData = m2mZcl0x0702Abstraction_t_getConsolidatedData;
  This->collectConsolidationElts = m2mZcl0x0702Abstraction_t_collectConsolidationElts;
  This->doZclOperation = m2mZclAbstraction_t_doZclOperation; // no changes
  This->doZclOperationResp = m2mZclAbstraction_t_doZclOperationResp; // no changes
  This->getZclAttribute = m2mZclAbstraction_t_getZclAttribute; // no changes
  This->setZclAttribute = m2mZclAbstraction_t_setZclAttribute; // no changes
  This->configureReporting = m2mZcl0x0702Abstraction_t_configureReporting;
  This->canReport = m2mZcl0x0702Abstraction_t_canReport;

  /* Initialize the members */
  This->endPointId = 0;
  This->clusterId = 0;

  /* generic members */
  This->multiplier = 1;
  This->divisor = 1;
  This->attrCsdMinReport = 0;
  This->lastCsdReportTmms = 0;
  This->attrIdMinReport = 0;
  This->lastIdReportTmms = 0;
}

/* dynamic allocation */
/**
 * @brief do dynamic allocation of a m2mZcl0x0702Abstraction_t.
 * @param epId the end point identifier.
 * @return the copy of the object
 */
m2mZcl0x0702Abstraction_t *new_m2mZcl0x0702Abstraction_t(azap_uint8 epId)
{
  m2mZcl0x0702Abstraction_t *This = malloc(sizeof(m2mZcl0x0702Abstraction_t));
  if (!This)
  {
    return NULL;
  }
  m2mZcl0x0702Abstraction_t_init(This);
  This->free = m2mZclAbstraction_t_newFree; // no changes

  This->endPointId = epId;
  This->clusterId = ZCL_CLUSTER_ID_SE_SIMPLE_METERING;

  RTL_TRDBG(TRACE_DETAIL, "new m2mZcl0x0702Abstraction_t (id:0x%.2x.0x%.4x)\n",
      This->endPointId, This->clusterId);
  return This;
}

/* implementation methods */
/**
 * @brief Run the consolidation data method associated to SimpleMetering cluster.
 * Where CSM MeasuredValue = CSM * multiplier / divisor
 * Where ID MeasuredValue = ID * multiplier / divisor
 * @anchor m2mZclAbstraction_t_getConsolidatedData
 * @param This the class instance
 * @param attrId the attribute identifier on which we need a data consolidation.
 * @param val the value to be consolidated.
 */
char *m2mZcl0x0702Abstraction_t_getConsolidatedData(m2mZclAbstraction_t *This,
    azap_uint16 attrId, azapZclAttrVal_t *val)
{
  static char strRep[32];
  char *res = NULL;
  double value = -1;
  m2mZcl0x0702Abstraction_t *child = (m2mZcl0x0702Abstraction_t *)This;

  // Multiplier and Divisor attribute values are encoded as unsigned 24-bit
  // integer according ZigBee specifications.

  switch (attrId)
  {
    case ATTRID_SE_CURRENT_SUMMATION_DELIVERED:
      child->lastCsdReportTmms = rtl_tmmsmono();
      value = (double)val->getIntegerValue(val);
      value *= child->multiplier;
      value /= child->divisor;
      sprintf(strRep, "%g", value);
      res = strRep;
      break;
    case ATTRID_SE_INSTANTANEOUS_DEMAND:
      child->lastIdReportTmms = rtl_tmmsmono();
      value = (double)val->getIntegerValue(val);
      value *= child->multiplier;
      value /= child->divisor;
      sprintf(strRep, "%g", value);
      res = strRep;
      break;
    default:
      RTL_TRDBG(TRACE_INFO, "m2mZcl0x0702Abstraction_t::getConsolidatedData - warning: "
          "unsupported attribute 0x%.4x\n", attrId);
      break;
  }
  return res;
}

/**
 * @brief Collect data that are mandatory for consolidation on ZCL reports.
 * @anchor m2mZcl0x0007Abstraction_t_collectConsolidationElts
 * @param This the class instance
 * @param ep the representation of the remote end point application.
 */
void m2mZcl0x0702Abstraction_t_collectConsolidationElts(m2mZclAbstraction_t *This,
    azapEndPoint_t *ep)
{
  m2mZcl0x0702Abstraction_t *child = (m2mZcl0x0702Abstraction_t *)This;
  azapAttribute_t *attr;

  // Get the multiplier if the node supports it
  attr = ep->getAttribute(ep, ZCL_CLUSTER_ID_SE_SIMPLE_METERING, ATTRID_SE_MULTIPLIER);
  if ((attr) && (attr->value))
  {
    child->multiplier = attr->value->getIntegerValue(attr->value);
    RTL_TRDBG(TRACE_INFO, "m2mZcl0x0702Abstraction_t::collectConsolidationElts - "
        "multiplier becomes %d\n", child->multiplier);
  }

  // Get the divisor if the node supports it
  attr = ep->getAttribute(ep, ZCL_CLUSTER_ID_SE_SIMPLE_METERING, ATTRID_SE_DIVISOR);
  if ((attr) && (attr->value))
  {
    child->divisor = attr->value->getIntegerValue(attr->value);
    RTL_TRDBG(TRACE_INFO, "m2mZcl0x0702Abstraction_t::collectConsolidationElts - "
        "divisor becomes %d\n", child->divisor);
  }
}

/**
 * @brief Collect the data regarding configuration on ZCL reporting.
 * @anchor m2mZcl0x0702Abstraction_t_configureReporting
 * @param This the class instance
 * @param cfgReportCmd the ZCL command that will be sent to the device.
 */
void m2mZcl0x0702Abstraction_t_configureReporting(m2mZclAbstraction_t *This,
    azapZclCfgReportCommand_t *cfgReportCmd)
{
  m2mZcl0x0702Abstraction_t *child = (m2mZcl0x0702Abstraction_t *)This;
  if (!cfgReportCmd)
  {
    return;
  }

  azap_uint8 i;
  for (i = 0; i < cfgReportCmd->numAttr; i++)
  {
    if (cfgReportCmd->attrList[i].reportableChange)
    {
      switch (cfgReportCmd->attrList[i].attrId)
      {
        case ATTRID_SE_CURRENT_SUMMATION_DELIVERED:
          child->attrCsdMinReport = cfgReportCmd->attrList[i].minReportInt;
          break;
        case ATTRID_SE_INSTANTANEOUS_DEMAND:
          child->attrIdMinReport = cfgReportCmd->attrList[i].minReportInt;
          break;
        default:
          break;
      }
    }
  }
}

/**
 * @brief Indicates if the report can be forwarded to the SCL or not.
 * To determine if the report can be forwarded to the SCL, one must verify when the last
 * report happens, and check if it is not too frequent.
 * @anchor m2mZcl0x0702Abstraction_t_canReport
 * @param This the class instance
 * @param attrId the attribute identifier on which we need a data consolidation.
 * @param return true if ZCL report can be forwarded to SCL, false otherwise.
 */
bool m2mZcl0x0702Abstraction_t_canReport(m2mZclAbstraction_t *This, azap_uint16 attrId)
{
  m2mZcl0x0702Abstraction_t *child = (m2mZcl0x0702Abstraction_t *)This;
  time_t now = rtl_tmmsmono();
  bool res = true;

  now += 1000;

  switch (attrId)
  {
    case ATTRID_SE_CURRENT_SUMMATION_DELIVERED:
      if ((child->attrCsdMinReport != 0) &&
          (((now - child->lastCsdReportTmms) / 1000) < child->attrCsdMinReport))
      {
        res = false;
      }
      break;
    case ATTRID_SE_INSTANTANEOUS_DEMAND:
      if ((child->attrIdMinReport != 0) &&
          (((now - child->lastIdReportTmms) / 1000) < child->attrIdMinReport))
      {
        res = false;
      }
      break;
    default:
      break;
  }
  return res;
}


