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
//** File : driver/m2m_abstraction.c
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

#include "xoapipr.h"
#include "dIa.h"
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
#include "m2m_abstraction.h"
#include "interface_mapping_desc.h"

#include "zigbee_han.h"

extern azap_uint8 azapPort;
extern zigbeeHan_t *theHan;
extern void *MainIQ;
extern char *knownSensorPath;

/*****************************************************************************/
/* Prototypes for local functions */
static void m2mAbstraction_t_init(m2mAbstraction_t *This);
static int m2mAbstraction_t_doM2mOpen(m2mAbstraction_t *This,
    void *obixReqContent, int reqId);
static int m2mAbstraction_t_doM2mZbOpen(m2mAbstraction_t *This,
    driverReqContext_t *context);
static int m2mAbstraction_t_doM2mDeviceLeave(m2mAbstraction_t *This,
    zigbeeNode_t *node, int reqId);

/**
 * @brief Get the parameter named 'paramName' in obix obj provided as a boolean.
 * @param obix the obix object in which searching for 'paramName'.
 * @param paramName the name of the parameter to look for.
 * @param value the placeholder where the value is provided when found.
 * @return 0 if the parameter is retrieved successfully, or the error code if negative
 * number.
 */
int getObixBoolValue(void *obix, char *paramName, bool *value)
{
  int nb = 0;
  void *param = NULL;
  char *name = NULL;
  char *val = NULL;
  int found = 0;
  int res = 0;

  if (!obix)
  {
    return -1;
  }
  if (!paramName)
  {
    return -2;
  }
  if (!value)
  {
    return -3;
  }
  if ((nb = XoNmFirstAttr(obix, "[]")) <= 0)
  {
    return -5;
  }
  while ((nb > 0) && (!found))
  {
    if (NULL != (param = XoNmGetAttr(obix, "[]", NULL)))
    {
      name = XoNmGetAttr(param, "name", 0, 0);
      if ((0 == strcmp(name, paramName)) &&
          (NULL != (val = XoNmGetAttr(param, "val", NULL))))
      {
        found = 1;
        res = 0;
        if (0 == strcmp("true", val))
        {
          *value = true;
        }
        else
        {
          *value = false;
        }
      }
      else
      {
        res = -6;
      }
    }
    else
    {
      res = -7;
    }
    nb = XoNmNextAttr(obix, "[]");
  }
  return res;
}

/**
 * @brief Get the parameter named 'paramName' in obix obj provided as an integer.
 * @param obix the obix object in which searching for 'paramName'.
 * @param paramName the name of the parameter to look for.
 * @param value the placeholder where the value is provided when found.
 * @return 0 if the parameter is retrieved successfully, or the error code if negative
 * number.
 */
int getObixIntValue(void *obix, char *paramName, int *value)
{
  int nb = 0;
  void *param = NULL;
  char *name = NULL;
  char *val = NULL;
  int found = 0;
  int res = 0;

  if (!obix)
  {
    return -1;
  }
  if (!paramName)
  {
    return -2;
  }
  if (!value)
  {
    return -3;
  }
  if ((nb = XoNmFirstAttr(obix, "[]")) <= 0)
  {
    return -5;
  }
  while ((nb > 0) && (!found))
  {
    if (NULL != (param = XoNmGetAttr(obix, "[]", NULL)))
    {
      name = XoNmGetAttr(param, "name", 0, 0);
      if ((0 == strcmp(name, paramName)) &&
          (NULL != (val = XoNmGetAttr(param, "val", NULL))))
      {
        found = 1;
        res = 0;
        *value = atoi(val);
      }
      else
      {
        res = -6;
      }
    }
    else
    {
      res = -7;
    }
    nb = XoNmNextAttr(obix, "[]");
  }
  return res;
}

/**
 * @brief Get the parameter named 'paramName' in obix obj provided as a string character.
 * @param obix the obix object in which searching for 'paramName'.
 * @param paramName the name of the parameter to look for.
 * @param value the placeholder where the value is provided when found.
 * @param size the value size (maximum size)
 * @return 0 if the parameter is retrieved successfully, or the error code if negative
 * number.
 */
int getObixStrValue(void *obix, char *paramName, char *value, int size)
{
  int nb = 0;
  void *param = NULL;
  char *name = NULL;
  char *val = NULL;
  int found = 0;
  int res = 0;

  if (!obix)
  {
    return -1;
  }
  if (!paramName)
  {
    return -2;
  }
  if (!value)
  {
    return -3;
  }
  if ((nb = XoNmFirstAttr(obix, "[]")) <= 0)
  {
    return -5;
  }
  while ((nb > 0) && (!found))
  {
    if (NULL != (param = XoNmGetAttr(obix, "[]", NULL)))
    {
      name = XoNmGetAttr(param, "name", 0, 0);
      if ((0 == strcmp(name, paramName)) &&
          (NULL != (val = XoNmGetAttr(param, "val", NULL))))
      {
        found = 1;
        res = 0;
        //snprintf(value, size, val); // may be dangerous if it contains '%'
        strncpy(value, val, size);
        value[size - 1] = 0;
      }
      else
      {
        res = -6;
      }
    }
    else
    {
      res = -7;
    }
    nb = XoNmNextAttr(obix, "[]");
  }
  return res;
}

/**
 * @brief Generate a dIa response in case of error response from the device.
 * @param cmdFamily the ZigBee command family (i.e. ZDP or ZCL), used to build the
 * response content.
 * @param zStatus the ZigBee status received in the response from the device.
 * @param statusCode the dIa response status (by reference).
 * @param size the size of the dIa response body (by reference).
 * @param type the response content type (by reference).
 * @return the body of the dIa response. WARNING: one need to invoke "free" on returned
 * value.
 */
char *getErrorInfoResponse(char *cmdFamily, azap_uint8 zStatus, char **statusCode,
    int *size, char **type)
{
  char *content = NULL;
  void *xo;
  int serialtype = XO_FMT_STD_XML;
//  int parseflags = XOML_PARSE_OBIX;
  int parseflags = 0;
  char szTemp[64];

  if (ZCL_STATUS_SUCCESS == zStatus)
  {
    *size = 0;
    *type = "";
    *statusCode = "STATUS_OK";
  }
  else
  {
    SetVar("w_errStatus", "STATUS_INTERNAL_SERVER_ERROR");
    snprintf(szTemp, 64, "%s: %s status 0x%.2x", GetVar("w_ipuid"), cmdFamily, zStatus);
    SetVar("w_errAddInfo", szTemp);
    xo = WXoNewTemplate("err_info.xml", parseflags);
    if (xo)
    {
      content = WXoSerializeFree(xo, serialtype, parseflags, size, type);
    }
    *statusCode = "STATUS_INTERNAL_SERVER_ERROR";
  }

  return content;
}


/*****************************************************************************/
/* singleton manager */
m2mAbstraction_t *getM2mAbstraction()
{
  static m2mAbstraction_t *m2mAbstraction = NULL;

  if (! m2mAbstraction)
  {
    m2mAbstraction = new_m2mAbstraction_t();
  }

  return m2mAbstraction;
}

/* Constructors / destructor */

/* static allocation */

/**
 * @brief Initialize the m2mAbstraction_t struct
 * @param This the class instance
 */
static void m2mAbstraction_t_init(m2mAbstraction_t *This)
{
  RTL_TRDBG(TRACE_DETAIL, "m2mAbstraction_t::init\n");

  /* Assign the pointers on functions */
  This->doM2mOperation = m2mAbstraction_t_doM2mOperation;
  This->doInternalTimer = m2mAbstraction_t_doInternalTimer;

  /* Initialize the members */
}

/* dynamic allocation */
/**
 * @brief do dynamic allocation of a m2mAbstraction_t.
 * @return the copy of the object
 */
m2mAbstraction_t *new_m2mAbstraction_t()
{
  m2mAbstraction_t *This = malloc(sizeof(m2mAbstraction_t));
  if (!This)
  {
    return NULL;
  }
  m2mAbstraction_t_init(This);
  This->free = m2mAbstraction_t_newFree;

  RTL_TRDBG(TRACE_DETAIL, "new m2mAbstraction_t\n");
  return This;
}


/**
 * @brief Destructor for dynamic allocation
 * @anchor m2mAbstraction_t_newFree
 * @param This the class instance
 */
void m2mAbstraction_t_newFree(m2mAbstraction_t *This)
{
  RTL_TRDBG(TRACE_DETAIL, "m2mAbstraction_t::newFree\n");
  free(This);
}



/* implementation methods */
/**
 * @brief Do invoke the M2M generic operation specified by the request URI.
 * @anchor m2mAbstraction_t_doM2mOperation
 * @param This the class instance
 * @param operation the operation name extracted from the request URI received in the dIa message.
 * @param node the node instance on which the operation refers to.
 * @param epId the end point identifier on the remote node on which the operation refers to.
 * @param clId the cluster identifier (if applicable)
 * @param obixReqContent the request content, already decoded as oBIX content type.
 * @param reqId the dIa request identifier to provide for completion.
 * @return 1 if the operation can be built and sent to the targeted node, 0 otherwise.
 */
int m2mAbstraction_t_doM2mOperation(m2mAbstraction_t *This, char *operation,
    zigbeeNode_t *node, azap_uint8 epId, azap_uint16 clId, void *obixReqContent, int reqId)
{
  int ret = ERR_UNSUPPORTED_OPERATION;

  if (0 == strcmp(operation, "m2mOpen"))
  {
    ret = m2mAbstraction_t_doM2mOpen(This, obixReqContent, reqId);
  }
  else if (0 == strcmp(operation, "m2mLeave"))
  {
    ret = m2mAbstraction_t_doM2mDeviceLeave(This, node, reqId);
  }
  else
  {
    RTL_TRDBG(TRACE_ERROR, "m2mAbstraction_t_doM2mOperation - error: unsupported "
        "M2M operation (%s)\n", operation);
  }

  return ret;

}


/**
 * @brief Do invoke the M2M open operation.
 * @param This the class instance
 * @param obixReqContent the request content, already decoded as oBIX content type.
 * @param reqId the dIa request identifier to provide for completion.
 * @return 0 if the operation can be built and sent to the targeted node, an
 * error code (negative number) otherwise.
 */
static int m2mAbstraction_t_doM2mOpen(m2mAbstraction_t *This, void *obixReqContent, int reqId)
{
  int res = OK_WAITING_FOR_COMPLETION;
  char szDuration[32];
  int duration = 0;
  int rc;
  driverReqContext_t *context;

  if (! obixReqContent)
  {
    RTL_TRDBG(TRACE_ERROR, "m2mAbstraction_t::doM2mOpen - error: "
        "oBIX content is missing\n");
    res = ERR_CONTENT_MISSING;
  }
  else if ((rc = getObixStrValue(obixReqContent, "duration", szDuration, 32)) < 0)
  {
    RTL_TRDBG(TRACE_ERROR, "m2mAbstraction_t::doM2mOpen - error: "
        "duration is missing (rc:%d)\n", rc);
    res = ERR_INVALID_ARG_1;
  }
  else
  {
    duration = parseISO8601Duration(szDuration);
    RTL_TRDBG(TRACE_DETAIL, "m2mAbstraction_t::doM2mOpen - parsed duration (%s) gives (%d)\n",
        szDuration, duration);
    theHan->computeNextPermitJoinDuration(theHan, duration);
    context = new_driverReqContext_t(DRV_REQ_OPERATION_FROM_DIA);
    context->issuerData = (void *)((long)reqId);
    duration = m2mAbstraction_t_doM2mZbOpen(This, context);
    // restart a timer if needed
    if (duration == ZIGBEE_MAX_PERMIT_JOIN_DURATION)
    {
      duration -= 10; // must be raised before the end
      rtl_imsgAdd( MainIQ, rtl_timerAlloc(IM_M2M_ABSTR_TIMERS, IM_TIMER_OPEN_ASSOC_RESUME,
          1000 * duration, NULL, 0));
    }
  }
  return res;
}

/**
 * @brief Implement the M2M open operation for ZigBee driver.
 * @param This the class instance
 * @param context the context for AZAP requesting.
 * @return the open duration if the operation can be built and sent to the targeted node, 0
 * otherwise.
 */
static int m2mAbstraction_t_doM2mZbOpen(m2mAbstraction_t *This, driverReqContext_t *context)
{
  int duration = 0;

  duration = theHan->computeNextPermitJoinDuration(theHan, -1);

  // first sent to the coordinator, and also broadcast it on the HAN
  if (! (azapZdpMgmtPermitJoinReq(azapPort, AZAP_REQ_TTL_START_AT_API, context,
      0x0000, duration, true, true) &&
      azapZdpMgmtPermitJoinReq(azapPort, AZAP_REQ_TTL_START_AT_API, NULL, 0xFFFF,
          duration, true, true)))
  {
    context->free(context);
    duration = 0;
  }

  return duration;
}

/**
 * @brief Implement the M2M device leave operation for ZigBee driver.
 * Regarding ZigBee IPU, this consists in deleting M2M resources, erasing the associated
 * "known device" file and removing the entry from the device list of the HAN.
 * This function only marks the device as "to be deleted", and the "publishing" background
 * task will do the job. Have a look to publish/unpublish methods of zigbeeNode_t and
 * zigbeeHan_t.
 * @param This the class instance
 * @param node the representation of the ZigBee node for the IPU.
 * @param reqId the dIa request identifier.
 * @return 1 if the operation succeeds, 0 otherwise.
 */
static int m2mAbstraction_t_doM2mDeviceLeave(m2mAbstraction_t *This,
    zigbeeNode_t *node, int reqId)
{
  node->toDelete = true;
  node->saveToFile(node, theHan->panId, theHan->panIdExt, theHan->rfChan, knownSensorPath);

  return OK_ONE_WAY_REQUEST;
}

/**
 * @brief Do process internal timer events for M2M abstraction.
 * @param This the class instance.
 * @param imsg the message associated to the timer.
 */
void m2mAbstraction_t_doInternalTimer(m2mAbstraction_t *This, t_imsg *imsg)
{
  driverReqContext_t *context;
  int duration = 0;
  switch (imsg->im_type)
  {
    case IM_TIMER_OPEN_ASSOC_RESUME:
    {
      context = new_driverReqContext_t(DRV_REQ_OPERATION_FROM_DIA_NO_COMPLETION);
      duration = m2mAbstraction_t_doM2mZbOpen(This, context);
      // restart a timer if needed
      if (duration == ZIGBEE_MAX_PERMIT_JOIN_DURATION)
      {
        duration -= 10; // must be raised before the end
        rtl_imsgAdd( MainIQ, rtl_timerAlloc(IM_M2M_ABSTR_TIMERS, IM_TIMER_OPEN_ASSOC_RESUME,
            1000 * duration, NULL, 0));
      }
    }
    break;

    default:
      RTL_TRDBG(TRACE_ERROR, "m2mAbstraction_t::doInternalTimer - error: unsupported "
          "event (%d)\n", imsg->im_type);
      break;
  }
}


