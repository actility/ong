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
//** File : driver/dia_impl.c
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


#include <stdio.h>      /************************************/
#include <stdlib.h>     /*     run in main thread           */
#include <string.h>     /************************************/
#include <unistd.h>
#include <stddef.h>
#include <errno.h>

#include "rtlbase.h"
#include "rtlimsg.h"

#include "xoapipr.h"
#include "dIa.h"

#include "cmndefine.h"
#include "cmnstruct.h"
#include "cmnproto.h"
#include "adaptor.h"

#include "azap_types.h"
#include "azap_tools.h"
#include "azap_api_common.h"
#include "azap_api_zcl.h"
#include "azap_api_zdo.h"
#include "azap_zcl_attr_val.h"
#include "azap_attribute.h"
#include "azap_endpoint.h"
#include "interface_mapping_desc.h"
#include "driver_req_context.h"
#include "zigbee_node.h"
#include "zigbee_han.h"

extern void *MdlCfg;
extern zigbeeHan_t *theHan;
extern char *knownSensorPath;

/*************************************************/
/* Prototypes for local functions */
static void setSearchStringInAppObj(t_cmn_sensor *cmn, int app, void *dst);
static void setSearchStringInDevObj(t_cmn_sensor *cmn, void *dst);
void setInterfaceListInObj(t_cmn_sensor *cmn, int app, void *dst);
static char *sensorClusterToInterfaceName(azap_uint16 cluster);
static char *getRetargetingSubPath(char *accessRight);
static void setServerItfDescriptorVar(azapEndPoint_t *ep, azap_uint16 clId,
    azap_uint16 attrId, int n);
static void setServerItfRetargetedVar(zigbeeNode_t *node, azapEndPoint_t *ep,
    azap_uint16 clId, azap_uint16 attrId, int n, char *accessRight);
static void setVarsForCluster(zigbeeNode_t *node, azapEndPoint_t *ep,
    azap_uint16 clusterId, azap_uint8 clusterDir, void *itf);

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
 * @brief Call back implementation of the dIa request formatting pre-processing.
 * @param reqname name of the dIa request (e.g. DiaAppElemCreate, DiaAppInstCreate...).
 * @param preq pointer on the dIa request structure.
 * @param target targeted URI
 * @param cmn pointer on the common sensor structure
 * @param xo the XO root document that will constitute the request content.
 * @param templatename the name of the template file to use.
 * @return always 0...
 */
int WDiaUCBPreLoadTemplate(char *reqname, t_dia_req *preq, char *target,
    t_cmn_sensor *cmn, void **xo, char *templatename)
{
  (void)xo;
  (void)templatename;
  char szTemp[16];

  RTL_TRDBG(TRACE_IMPL, "WDiaUCBPreLoadTemplate for '%s/%s'\n", reqname, preq->rq_name);

  if (!cmn)
  {
    return 0;
  }
  if (!preq)
  {
    return 0;
  }

  SetVar("d_ieee", cmn->cmn_ieee);
  snprintf(szTemp, sizeof(szTemp), "%d", preq->rq_app);
  SetVar("a_num", szTemp);

  ////////////////////////////// DiaDatContCreate /////////////////////////////
  if (0 == strcmp(reqname, "DiaDatContCreate"))
  {
    char cnum[128];
    sprintf(cnum, "%04x.0x%04x.%d.m2m", preq->rq_cluster, preq->rq_attribut,
        preq->rq_member);
    SetVar("c_num", cnum);
    return 0;
  }

  ////////////////////////////// DiaAppInstCreate /////////////////////////////
  if (0 == strcmp(reqname, "DiaAppInstCreate"))
  {
    if ((cmn) && (cmn->cmn_self))
    {
      zigbeeNode_t *node = (zigbeeNode_t *)cmn->cmn_self;
      azapEndPoint_t *ep;
      if ((node) && (NULL != (ep = node->getEndPoint(node, preq->rq_app) )))
      {
        SetVar("a_appProfId", azap_uint16ToString(ep->appProfId, true));
        SetVar("a_appDevId", azap_uint16ToString(ep->appDeviceId, true));
        snprintf(szTemp, sizeof(szTemp), "%d", ep->appDevVer);
        SetVar("a_appDevVers", szTemp);
      }
    }

    return 0;
  }

  ////////////////////////////// DiaDevInstRetrieve ///////////////////////////
  if (0 == strcmp(reqname, "DiaDevInstRetrieve"))
  {
    SetVar("d_shortAddr", azap_uint16ToString(cmn->cmn_num, true));
  }

  ////////////////////////////// DiaDevInstCreate /////////////////////////////
  if (0 == strcmp(reqname, "DiaDevInstCreate"))
  {
    SetVar("d_shortAddr", azap_uint16ToString(cmn->cmn_num, true));
    if ((cmn) && (cmn->cmn_self))
    {
      zigbeeNode_t *node = (zigbeeNode_t *)cmn->cmn_self;
      SetVar("w_manufacturer", azap_uint16ToString(node->manuCode, true));
      switch (node->nodeType)
      {
        case DEV_LOGICAL_TYPE_COORD:
          SetVar("d_type", "coordinator");
          break;
        case DEV_LOGICAL_TYPE_ROUTER:
          SetVar("d_type", "router");
          break;
        case DEV_LOGICAL_TYPE_DEV:
          SetVar("d_type", "endDevice");
          break;
      }
      switch (node->powerMode)
      {
        case 0:
          SetVar("d_powMode", "rxidle");
          break;
        case 1:
          SetVar("d_powMode", "rxperiod");
          break;
        case 2:
          SetVar("d_powMode", "rxstim");
          break;
      }
      switch (node->powerSource)
      {
        case 1:
          SetVar("d_powSrc", "const");
          break;
        case 2:
          SetVar("d_powSrc", "rechar");
          break;
        case 4:
          SetVar("d_powSrc", "dispos");
          break;
      }
      switch (node->powerLevel)
      {
        case 0:
          SetVar("d_powLvl", "critical");
          break;
        case 4:
          SetVar("d_powLvl", "l33");
          break;
        case 8:
          SetVar("d_powLvl", "l66");
          break;
        case 12:
          SetVar("d_powLvl", "l100");
          break;
      }
    }
    return 0;
  }

  return 0;
}


/**
 * @brief Call back implementation of the dIa request formatting pre-processing.
 * @param reqname name of the dIa request (e.g. DiaAppElemCreate, DiaAppInstCreate...).
 * @param preq pointer on the dIa request structure.
 * @param target targeted URI
 * @param cmn pointer on the common sensor structure
 * @param xo the XO root document that will constitute the request content.
 * @return always 0...
 */
int WDiaUCBPostLoadTemplate(char *reqname, t_dia_req *preq, char *target,
    t_cmn_sensor *cmn, void *xo)
{
  char tmp[256];
  char m2mvar[256];
  void *mdlitf = NULL;

  RTL_TRDBG(TRACE_IMPL, "WDiaUCBPostLoadTemplate for '%s/%s'\n", reqname, preq->rq_name);

  if (!xo)
  {
    return 0;
  }

  if (strcmp(reqname, "DiaNetInstCreate") == 0)
  {
    theHan->buildNodeList(theHan, xo);
    return 0;
  }
  if (strcmp(reqname, "DiaDevInstCreate") == 0)
  {
    if ((cmn) && (cmn->cmn_self))
    {
      zigbeeNode_t *node = (zigbeeNode_t *)cmn->cmn_self;
      node->buildAppList(node, xo);
    }
    return 0;
  }
  if (strcmp(reqname, "DiaDevElemCreate") == 0)
  {
    setSearchStringInDevObj(cmn, xo);
    return 0;
  }
  if (strcmp(reqname, "DiaAppElemCreate") == 0)
  {
    setSearchStringInAppObj(cmn, preq->rq_app, xo);
    return 0;
  }
  if (strcmp(reqname, "DiaAppInstCreate") == 0)
  {
    setInterfaceListInObj(cmn, preq->rq_app, xo);
    return 0;
  }
  if (strcmp(reqname, "DiaDatElemCreate") == 0)
  {
    setSearchStringInAppObj(cmn, preq->rq_app, xo);
    return 0;
  }

  //
  // modeling
  //

  if  (MdlCfg == NULL)
  {
    return  0;
  }

  if (strcmp(reqname, "DiaDatContCreate") == 0)
  {
    sprintf (tmp, "0x%04x", preq->rq_cluster);
    mdlitf  = MdlGetInterface(MdlCfg, NULL, "server", tmp);
    if (mdlitf)
    {
      sprintf (m2mvar, "0x%04x.0x%04x.%d.m2m",
          preq->rq_cluster, preq->rq_attribut, preq->rq_member);
      MdlConfigureDataContainerTemplate(mdlitf, xo, m2mvar);
    }
  }
  return 0;
}

/**
 * @brief Call back function invoked by drvcommon layer on dIa request error detection.
 * @param reqname the dIa request name (e.g. DiaDatContCreate)
 * @param preq the dIa request that failed.
 * @param cmn the sensor on which the request refers to (if relevant, NULL otherwise).
 * @return always 0.
 */
int WDiaUCBRequestError(char *reqname, t_dia_req *preq, t_cmn_sensor *cmn)
{
  (void)reqname;
  (void)preq;
  (void)cmn;
  // nothing to do
  return 0;
}

/**
 * @brief Call back function invoked by drvcommon layer on dIa request timeout detection.
 * @param reqname the dIa request name (e.g. DiaDatContCreate)
 * @param preq the dIa request that failed.
 * @param cmn the sensor on which the request refers to (if relevant, NULL otherwise).
 * @return always 0.
 */
int WDiaUCBRequestTimeout(char *reqname, t_dia_req *preq, t_cmn_sensor *cmn)
{
  (void)preq;
  (void)cmn;
  if  (strncmp(reqname, "DiaIpu", 6) == 0)
  {
    DiaIpuStart();
    return  0;
  }
  if  (strncmp(reqname, "DiaNet", 6) == 0)
  {
    DiaNetStart();
    return  0;
  }
  return 0;
}

/**
 * @brief Call back function invoked by drvcommon layer on dIa request success completion.
 * @param reqname the dIa request name (e.g. DiaDatContCreate)
 * @param preq the dIa request that failed.
 * @param cmn the sensor on which the request refers to (if relevant, NULL otherwise).
 * @return always 0.
 */
int WDiaUCBRequestOk(char *reqname, t_dia_req *preq, t_cmn_sensor *cmn)
{
  (void)reqname;
  (void)cmn;
  if (0 == strncmp(reqname, "DiaIpu", 6))
  {
    theHan->m2mIpuCreated = true;
  }
  if (0 == strncmp(reqname, "DiaNet", 6))
  {
    theHan->m2mNwkCreated = true;
  }

  return 0;
}

/**
 * @brief Call back function invoked by drvcommon layer when receiving a 2XX response for a
 * dIa DELETE request for DEV_*.
 * @param preq the sent request.
 * @param cmn the sensor on which the request refers to (if relevant, NULL otherwise).
 */
void WDiaUCBRequestOk_DiaDevDelOk(t_dia_req *preq, t_cmn_sensor *cmn)
{
  char fullPath[512];

  if ((cmn) && (cmn->cmn_self))
  {
    zigbeeNode_t *node = (zigbeeNode_t *)cmn->cmn_self;
    if (node->toDelete)
    {
      // erase the "known sensor" file associated to the node.
      sprintf(fullPath, "%s/%s.xml", knownSensorPath, node->cp_cmn.cmn_ieee);
      if (remove(fullPath))
      {
        RTL_TRDBG(TRACE_ERROR, "m2mAbstraction_t::doM2mDeviceLeave - error while removing file %s "
            "(errno:%d) (%s)", fullPath, errno, strerror(errno));
      }
      // delete the node from the HAN
      theHan->deleteNode(theHan, node->ieeeAddr);
    }
  }
}

/**
 * @brief Call back function invoked by drvcommon layer when receiving a 2XX response for a
 * dIa DELETE request for APP_*.
 * @param preq the sent request.
 * @param cmn the sensor on which the request refers to (if relevant, NULL otherwise).
 */
void WDiaUCBRequestOk_DiaAppDelOk(t_dia_req *preq, t_cmn_sensor *cmn)
{
  // nothing to do
}

/**
 * @brief Call back function invoked by drvcommon layer when receiving a 2XX response for a
 * dIa DELETE request for APP_*_AR*.
 * @param preq the sent request.
 * @param cmn the sensor on which the request refers to (if relevant, NULL otherwise).
 */
void WDiaUCBRequestOk_DiaAccDelOk(t_dia_req *preq, t_cmn_sensor *cmn)
{
  // nothing to do
}

///////////////////////////////////////////////////////////////////////////////

/*    _                 _    __              _   _
 *   | |   ___  __ __ _| |  / _|_  _ _ _  __| |_(_)___ _ _  ___
 *   | |__/ _ \/ _/ _` | | |  _| || | ' \/ _|  _| / _ \ ' \(_-<
 *   |____\___/\__\__,_|_| |_|  \_,_|_||_\__|\__|_\___/_||_/__/
 */

/**
 * @brief Gives the string representation of a cluster identifier.
 * @param cluster The cluster identifier.
 * @return the string representation.
 */
static char *sensorClusterToInterfaceName(azap_uint16 cluster)
{
  switch (cluster)
  {
    case ZCL_CLUSTER_ID_GEN_BASIC:
      return "Basic";
    case ZCL_CLUSTER_ID_GEN_IDENTIFY:
      return "Identify";
    case ZCL_CLUSTER_ID_GEN_ON_OFF:
      return "OnOff";
    case ZCL_CLUSTER_ID_SE_SIMPLE_METERING:
      return "SimpleMetering";
    case ZCL_CLUSTER_ID_MS_ILLUMINANCE_MEASUREMENT:
      return "IlluminanceMeasurement";
    case ZCL_CLUSTER_ID_MS_TEMPERATURE_MEASUREMENT:
      return "TemperatureMeasurement";
    case ZCL_CLUSTER_ID_MS_PRESSURE_MEASUREMENT:
      return "PressureMeasurement";
    case ZCL_CLUSTER_ID_MS_FLOW_MEASUREMENT:
      return "FlowMeasurement";
    case ZCL_CLUSTER_ID_MS_RELATIVE_HUMIDITY:
      return "RelativeHumidityMeasurement";
    case ZCL_CLUSTER_ID_MS_OCCUPANCY_SENSING:
      return "OccupancySensing";
  }
  RTL_TRDBG(TRACE_ERROR, "cannot find cluster name 0x%.4x\n", cluster);
  return  "";
}


/**
 * @brief Set the appropriate search strings in the XO document embedded in dIa request for
 * DEV application create.
 * @param cmn the common struct for the sensor the request refers to.
 * @param dst the XO document in which the search strings are to be added.
 */
static void setSearchStringInDevObj(t_cmn_sensor *cmn, void *dst)
{
  char tmp[256];
  zigbeeNode_t *node;
  azapEndPoint_t *ep;
  azapAttribute_t *attr;
  azap_uint8 i = 0;

  if ((cmn) && (cmn->cmn_self))
  {
    node = (zigbeeNode_t *)cmn->cmn_self;
    if (node)
    {
      // Look for Basic/ModelID attribute
      ep = node->getNthEndPoint(node, i);
      while (ep != NULL)
      {
        attr = ep->getAttribute(ep, ZCL_CLUSTER_ID_GEN_BASIC, ATTRID_BASIC_MODEL_ID);
        if (NULL != attr)
        {
          sprintf(tmp, "ETSI.ModelID/%s", attr->value->getStringRep(attr->value));
          XoNmAddInAttr(dst, "m2m:searchStrings.m2m:searchString", tmp, 0);
          break;
        }
        i++;
        ep = node->getNthEndPoint(node, i);
      }
    }
  }
}

/**
 * @brief Set the appropriate search strings in the XO document embedded in dIa request for
 * APP application create.
 * @param cmn the common struct for the sensor the request refers to.
 * @param app the application number (i.e. the end point identifier) the request refers to.
 * @param dst the XO document in which the search strings are to be added.
 */
static void setSearchStringInAppObj(t_cmn_sensor *cmn, int app, void *dst)
{
  char tmp[256];
  zigbeeNode_t *node;
  azapEndPoint_t *ep;
  azap_uint8 i;

  if ((cmn) && (cmn->cmn_self))
  {
    node = (zigbeeNode_t *)cmn->cmn_self;
    if ((node) && (NULL != (ep = node->getEndPoint(node, app) )))
    {
      sprintf(tmp, "ZigBee.ProfileID/0x%04x", ep->appProfId);
      XoNmAddInAttr(dst, "m2m:searchStrings.m2m:searchString", tmp, 0);
      sprintf(tmp, "ZigBee.DeviceID/0x%04x", ep->appDeviceId);
      XoNmAddInAttr(dst, "m2m:searchStrings.m2m:searchString", tmp, 0);
      for (i = 0; i < ep->appNumInClusters; i++)
      {
        if (NULL != MdlGetInterface(MdlCfg, NULL, "server",
            azap_uint16ToString(ep->appInClusterList[i], true)))
        {
          sprintf(tmp, "ZigBee.ClusterID/server/0x%04x", ep->appInClusterList[i]);
          XoNmAddInAttr(dst, "m2m:searchStrings.m2m:searchString", tmp, 0);
          sprintf(tmp, "ETSI.InterfaceID/%s.Srv",
              sensorClusterToInterfaceName(ep->appInClusterList[i]));
          XoNmAddInAttr(dst, "m2m:searchStrings.m2m:searchString", tmp, 0);
        }
      }
      for (i = 0; i < ep->appNumOutClusters; i++)
      {
        if (NULL != MdlGetInterface(MdlCfg, NULL, "client",
            azap_uint16ToString(ep->appInClusterList[i], true)))
        {
          sprintf(tmp, "ZigBee.ClusterID/client/0x%04x", ep->appOutClusterList[i]);
          XoNmAddInAttr(dst, "m2m:searchStrings.m2m:searchString", tmp, 0);
          sprintf(tmp, "ETSI.InterfaceID/%s.Clt",
              sensorClusterToInterfaceName(ep->appOutClusterList[i]));
          XoNmAddInAttr(dst, "m2m:searchStrings.m2m:searchString", tmp, 0);
        }
      }
    }
  }
}


/**
 * @brief Build the application interface list and add it to the "application" oBIX resource.
 * @param cmn the common structure instance for the targeted sensor.
 * @param app the application identifier, i.e. the end point identifier.
 * @param dst the oBIX document that represent the application.
 */
void setInterfaceListInObj(t_cmn_sensor *cmn, int app, void *dst)
{
  zigbeeNode_t *node;
  azapEndPoint_t *ep;
  void *mdlitf, * obj;
  char szTemp[64];
  int parseflags = XOML_PARSE_OBIX;
  azap_uint8 i;

  if (XoIsObix(dst) <= 0)
  {
    RTL_TRDBG(TRACE_ERROR, "setInterfaceListInObj - error: not an obix object\n");
    return;
  }

  if ((cmn) && (cmn->cmn_self))
  {
    node = (zigbeeNode_t *)cmn->cmn_self;
    if ((node) && (NULL != (ep = node->getEndPoint(node, app) )))
    {
      for (i = 0; i < ep->appNumInClusters; i++)
      {
        if (NULL != (mdlitf = MdlGetInterface(MdlCfg, NULL, "server",
            azap_uint16ToString(ep->appInClusterList[i], true))))
        {
          // set server interface in application resource
          snprintf(szTemp, sizeof(szTemp), "itf_inst_%.4x_server.xml",
              ep->appInClusterList[i]);
          setVarsForCluster(node, ep, ep->appInClusterList[i],
              ZCL_FRAME_SERVER_CLIENT_DIR, mdlitf);
          obj = WXoNewTemplate(szTemp, parseflags);
          if (obj)
          {
            // in obj template delete attributs/commands/points which
            // are not defined in model config
            MdlConfigureInterfaceTemplate(mdlitf, obj);
            // clear possible status="cfg-mandatory" in interface
            // even if modeling is not active
            MdlUnsetStatusInterfaceTemplate(obj);
            XoNmAddInAttr(dst, "[name=interfaces].[]", obj, 0, 0);
          }
        }
      }
      for (i = 0; i < ep->appNumOutClusters; i++)
      {
        if (NULL != (mdlitf = MdlGetInterface(MdlCfg, NULL, "client",
            azap_uint16ToString(ep->appOutClusterList[i], true))))
        {
          snprintf(szTemp, sizeof(szTemp), "itf_inst_%.4x_client.xml",
              ep->appOutClusterList[i]);
          setVarsForCluster(node, ep, ep->appOutClusterList[i],
              ZCL_FRAME_CLIENT_SERVER_DIR, mdlitf);
          obj = WXoNewTemplate(szTemp, parseflags);
          if (obj)
          {
            // in obj template delete attributs/commands/points which
            // are not defined in model config
            MdlConfigureInterfaceTemplate(mdlitf, obj);
            // clear possible status="cfg-mandatory" in interface
            // even if modeling is not active
            MdlUnsetStatusInterfaceTemplate(obj);
            XoNmAddInAttr(dst, "[name=interfaces].[]", obj, 0, 0);
          }
        }
      }
    }
  }
}


/**
 * @brief Set the two variables (a_configAttr and a_configVal) for a not-retargeted attribute.
 * This ZigBee attribute is either not provided in modeling.xml, either defined as
 * "config=descritor".
 * @param ep the local representation of the concerned end point on the remote node.
 * @param clId the cluster identifier the ZCL attribute belongs to.
 * @param attrId the attribute identifier.
 * @param n the variable number in the dIa template.
 */
static void setServerItfDescriptorVar(azapEndPoint_t *ep, azap_uint16 clId,
    azap_uint16 attrId, int n)
{
  char szTemp[32];
  azapAttribute_t *attrVal;

  if (! ep)
  {
    return;
  }

  sprintf(szTemp, "a_configAttr%d", n);
  SetVar(szTemp, "val");
  attrVal = ep->getAttribute(ep, clId, attrId);
  sprintf(szTemp, "a_configVal%d", n);
  if ((attrVal) && (attrVal->value))
  {
    SetVar(szTemp, attrVal->value->getStringRep(attrVal->value));
  }
  else
  {
    SetVar(szTemp, NULL);
  }

}

/**
 * @brief Gives the sub-path corresponding to the provided access right.
 * Default path is "retargeting1".
 * @param accessRight the AR level found in modeling configuration.
 * @return the sub-path corresponding to the provided access right.
 */
static char *getRetargetingSubPath(char *accessRight)
{
  char *res = SUB_PATH_AR1;  // default value
  if (accessRight)
  {
    if (!strcmp(accessRight, FILTER_SUB_PATH_AR1))
    {
      res = SUB_PATH_AR1;
    }
    else if (!strcmp(accessRight, FILTER_SUB_PATH_AR2))
    {
      res = SUB_PATH_AR2;
    }
    else if (!strcmp(accessRight, FILTER_SUB_PATH_AR3))
    {
      res = SUB_PATH_AR3;
    }
  }
  return res;
}

/**
 * @brief Set the two variables (a_configAttr and a_configVal) for a not-retargeted attribute.
 * This ZigBee attribute is provided in modeling.xml and "config=retargeting".
 * @param node the local representation of the remote node.
 * @param ep the local representation of the concerned end point on the remote node.
 * @param clId the cluster identifier the ZCL attribute belongs to.
 * @param attrId the attribute identifier.
 * @param n the variable number in the dIa template.
 * @param accessRight the AR associated to this retargeted attribute.
 */
static void setServerItfRetargetedVar(zigbeeNode_t *node, azapEndPoint_t *ep,
    azap_uint16 clId, azap_uint16 attrId, int n, char *accessRight)
{
  char szTemp[32];
  char szTemp2[255];

  if (! ep)
  {
    return;
  }
  if (! node)
  {
    return;
  }

  sprintf(szTemp, "a_configAttr%d", n);
  SetVar(szTemp, "href");
  sprintf(szTemp, "a_configVal%d", n);
  sprintf(szTemp2, "%s/APP_%s.%d/%s/0x%.4x.0x%.4x.attr", GetVar("s_rootapp"),
      node->cp_cmn.cmn_ieee, ep->id, getRetargetingSubPath(accessRight), clId, attrId);
  SetVar(szTemp, szTemp2);
}

/**
 * @brief Set the variables required for filling in the dIa template.
 * @param node the local representation of the remote node.
 * @param ep the local representation of the concerned end point on the remote node.
 * @param clusterId the cluster identifier that the interface defines.
 * @param clusterDir the cluster direction.
 * @param itf the xo representation of the interface modeling.
 */
static void setVarsForCluster(zigbeeNode_t *node, azapEndPoint_t *ep,
    azap_uint16 clusterId, azap_uint8 clusterDir, void *itf)
{
  interfaceMappingDesc_t *desc = theHan->getMapping(theHan, clusterId, clusterDir);
  int i;
  char szTemp[32];
  char *varVal;
  void *descriptor;
  char *config;
  char *accessRight = NULL;
  int retargeted;

  if (!itf)
  {
    return;
  }
  if (!desc)
  {
    return;
  }

  RTL_TRDBG(TRACE_DETAIL, "setVarsForCluster\n");

  if (ZCL_FRAME_SERVER_CLIENT_DIR == clusterDir)
  {
    // reset attribute values
    for (i = 0; i < ITF_MAPPING_MAX_CONFIG_ATTR; i++)
    {
      sprintf(szTemp, "a_configAttr%d", i);
      SetVar(szTemp, "");
      sprintf(szTemp, "a_configVal%d", i);
      SetVar(szTemp, "");
    }

    // look for retargeted/descriptor attributes
    for (i = 0; i < desc->nbAttributes; i++)
    {
      retargeted = 0;
      sprintf(szTemp, "0x%.4x.0x%.4x.0.attr", clusterId, desc->attributes[i]);
      varVal = GetVarNoWarn(szTemp);
      if ((varVal) && (*varVal) &&
          (NULL != (descriptor = MdlGetDescriptorByName(itf, varVal))))
      {
        config = XoNmGetAttr(descriptor, "ong:config", NULL, 0);
        if (!strcmp("retargeting", config))
        {
          retargeted = 1;
          accessRight = XoNmGetAttr(descriptor, "ong:accessRight", NULL, 0);
        }
      }

      if (retargeted)
      {
        setServerItfRetargetedVar(node, ep, clusterId, desc->attributes[i], i, accessRight);
      }
      else
      {
        setServerItfDescriptorVar(ep, clusterId, desc->attributes[i], i);
      }
    }
  }

  // look for retargeted/descriptor operations
  for (i = 0; i < desc->nbOperations; i++)
  {
    if (NULL != (descriptor = MdlGetDescriptorByName(itf, desc->operations[i])))
    {
      accessRight = XoNmGetAttr(descriptor, "ong:accessRight", NULL, 0);
    }

    if (accessRight)
    {
      sprintf(szTemp, "r_pathOpe%d", i);
      SetVar(szTemp, getRetargetingSubPath(accessRight));
    }
  }
}


