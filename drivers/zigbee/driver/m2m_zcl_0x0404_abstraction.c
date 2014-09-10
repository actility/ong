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
//** File : driver/m2m_zcl_0x0404_abstraction.c
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
#include "m2m_zcl_abstraction.h"
#include "m2m_zcl_0x0404_abstraction.h"
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
 * @brief Initialize the m2mZcl0x0404Abstraction_t struct
 * @param This the class instance
 */
static void m2mZcl0x0404Abstraction_t_init(m2mZcl0x0404Abstraction_t *This)
{
  RTL_TRDBG(TRACE_DETAIL, "m2mZcl0x0404Abstraction_t::init\n");

  /* Assign the pointers on functions */
  This->getConsolidatedData = m2mZcl0x0404Abstraction_t_getConsolidatedData;
  This->collectConsolidationElts = m2mZclAbstraction_t_collectConsolidationElts; // no changes
  This->doZclOperation = m2mZclAbstraction_t_doZclOperation; // no changes
  This->doZclOperationResp = m2mZclAbstraction_t_doZclOperationResp; // no changes
  This->getZclAttribute = m2mZclAbstraction_t_getZclAttribute; // no changes
  This->setZclAttribute = m2mZclAbstraction_t_setZclAttribute; // no changes
  This->configureReporting = m2mZcl0x0404Abstraction_t_configureReporting;
  This->canReport = m2mZcl0x0404Abstraction_t_canReport;

  /* Initialize the members */
  This->endPointId = 0;
  This->clusterId = 0;
}

/* dynamic allocation */
/**
 * @brief do dynamic allocation of a m2mZcl0x0404Abstraction_t.
 * @param epId the end point identifier.
 * @return the copy of the object
 */
m2mZcl0x0404Abstraction_t *new_m2mZcl0x0404Abstraction_t(azap_uint8 epId)
{
  m2mZcl0x0404Abstraction_t *This = malloc(sizeof(m2mZcl0x0404Abstraction_t));
  if (!This)
  {
    return NULL;
  }
  m2mZcl0x0404Abstraction_t_init(This);
  This->free = m2mZclAbstraction_t_newFree; // no changes

  This->endPointId = epId;
  This->clusterId = ZCL_CLUSTER_ID_MS_FLOW_MEASUREMENT;

  RTL_TRDBG(TRACE_DETAIL, "new m2mZcl0x0404Abstraction_t (id:0x%.2x.0x%.4x)\n",
      This->endPointId, This->clusterId);
  return This;
}

/* implementation methods */
/**
 * @brief Run the consolidation data method associated to FlowMeasurement cluster.
 * Where MeasuredValue = 10 x Flow
 * @anchor m2mZcl0x0004Abstraction_t_getConsolidatedData
 * @param This the class instance
 * @param attrId the attribute identifier on which we need a data consolidation.
 * @param val the value to be consolidated.
 * @return the consolidated data if supported attribute, NULL otherwise.
 */
char *m2mZcl0x0404Abstraction_t_getConsolidatedData(m2mZclAbstraction_t *This,
    azap_uint16 attrId, azapZclAttrVal_t *val)
{
  static char strRep[32];
  double value = -1;

  if ((ATTRID_MS_FLOW_MEASUREMENT_MEASURED_VALUE != attrId) &&
      (ATTRID_MS_FLOW_MEASUREMENT_TOLERANCE != attrId))
  {
    RTL_TRDBG(TRACE_INFO, "m2mZcl0x0404Abstraction_t::getConsolidatedData - warning: "
        "unsupported attribute 0x%.4x\n", attrId);
    return NULL;
  }

  if ( (((long long)0x0000000000000000LL) > val->getIntegerValue(val)) ||
      (((long long)0x000000000000FFFELL) < val->getIntegerValue(val)) )
  {
    // Out of range values (cf ZigBee ZCL specifications)
    value = 0;
  }
  else
  {
    value = (double)val->getIntegerValue(val);
    value /= 10;
  }
  sprintf(strRep, "%g", value);
  return strRep;
}


/**
 * @brief Collect the data regarding configuration on ZCL reporting.
 * @anchor m2mZcl0x0404Abstraction_t_configureReporting
 * @param This the class instance
 * @param cfgReportCmd the ZCL command that will be sent to the device.
 */
void m2mZcl0x0404Abstraction_t_configureReporting(m2mZclAbstraction_t *This,
    azapZclCfgReportCommand_t *cfgReportCmd)
{
}

/**
 * @brief Indicates if the report can be forwarded to the SCL or not.
 * To determine if the report can be forwarded to the SCL, one must verify when the last
 * report happens, and check if it is not too frequent.
 * @anchor m2mZcl0x0404Abstraction_t_canReport
 * @param This the class instance
 * @param attrId the attribute identifier on which we need a data consolidation.
 * @param return true if ZCL report can be forwarded to SCL, false otherwise.
 */
bool m2mZcl0x0404Abstraction_t_canReport(m2mZclAbstraction_t *This, azap_uint16 attrId)
{
  return true;
}


