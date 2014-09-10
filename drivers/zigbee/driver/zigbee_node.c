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
//** File : driver/zigbee_node.c
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
#include <unistd.h>
#include <string.h>
#include <stddef.h>

#include "rtlbase.h"
#include "rtllist.h"
#include "rtlimsg.h"
#include "xoapipr.h"
#include "dIa.h"
#include "cmndefine.h"
#include "cmnstruct.h"
#include "cmnproto.h"
#include "adaptor.h"

#include "azap_types.h"
#include "azap_tools.h"
#include "azap_api.h"
#include "azap_api_zcl.h"

#include "azap_zcl_attr_val.h"
#include "azap_attribute.h"
#include "azap_endpoint.h"
#include "driver_req_context.h"
#include "zigbee_node.h"
#include "m2m_zcl_abstraction.h"
#include "m2m_zcl_0x0000_abstraction.h"
#include "m2m_zcl_0x0003_abstraction.h"
#include "m2m_zcl_0x0006_abstraction.h"
#include "m2m_zcl_0x0400_abstraction.h"
#include "m2m_zcl_0x0402_abstraction.h"
#include "m2m_zcl_0x0403_abstraction.h"
#include "m2m_zcl_0x0404_abstraction.h"
#include "m2m_zcl_0x0405_abstraction.h"
#include "m2m_zcl_0x0702_abstraction.h"
#include "azap_listener.h"
#include "interface_mapping_desc.h"
#include "zigbee_han.h"

extern void *MdlCfg;
extern zigbeeHan_t *theHan;

/*****************************************************************************/
/* Prototypes for local functions */
static void zigbeeNode_t_init(zigbeeNode_t *This);
static azapEndPoint_t *zigbeeNode_t_loadEpFromXo(zigbeeNode_t *This, void *xo,
    int eltNum, char *file);
static void zigbeeNode_t_loadAttributesFromXo(zigbeeNode_t *This, azapEndPoint_t *ep,
    void *xo);
static void zigbeeNode_t_saveEpToXo(zigbeeNode_t *This, azapEndPoint_t *ep, void *epDesc);

/*****************************************************************************/
/* Constructors / destructor */

/* static allocation */

/**
 * @brief Initialize the zigbeeNode_t struct
 * @param This the class instance
 */
static void zigbeeNode_t_init(zigbeeNode_t *This)
{
  RTL_TRDBG(TRACE_DETAIL, "zigbeeNode_t::init\n");

  /* Assign the pointers on functions */
  This->clearDiaCounters = zigbeeNode_t_clearDiaCounters;
  This->loadFromFile = zigbeeNode_t_loadFromFile;
  This->saveToFile = zigbeeNode_t_saveToFile;
  This->dumpNode = zigbeeNode_t_dumpNode;
  This->addEndPoint = zigbeeNode_t_addEndPoint;
  This->getEndPoint = zigbeeNode_t_getEndPoint;
  This->getNthEndPoint = zigbeeNode_t_getNthEndPoint;
  This->addM2mZclAbstraction = zigbeeNode_t_addM2mZclAbstraction;
  This->getM2mZclAbstraction = zigbeeNode_t_getM2mZclAbstraction;
  This->buildAppList = zigbeeNode_t_buildAppList;
  This->createDataContainers = zigbeeNode_t_createDataContainers;
  This->publish = zigbeeNode_t_publish;
  This->unpublish = zigbeeNode_t_unpublish;
  This->isFirstAudit = zigbeeNode_t_isFirstAudit;
  This->endAudit = zigbeeNode_t_endAudit;

  /* Initialize the members */
  memset(This->ieeeAddr, 0, IEEE_ADDR_LEN);
  memset(&(This->cp_cmn), 0, sizeof(t_cmn_sensor));
  This->azapAuditContext = NULL;
  This->lastAuditDate = 0;
  This->nbContainersToCreate = -1;
  This->manuCode = 0;
  This->nodeType = 0;
  This->powerMode = 0;
  This->powerSource = 0;
  This->powerLevel = 0;
  This->toDelete = false;

  This->cp_cmn.cmn_self = This;
  INIT_LIST_HEAD(&(This->endPointList));
  INIT_LIST_HEAD(&(This->m2mZclAbstractionList));
}

/* dynamic allocation */
/**
 * @brief do dynamic allocation of a zigbeeNode_t.
 * @param nwkAddr the node network address.
 * @param ieeeAddr the node IEEE address.
 * @return the copy of the object
 */
zigbeeNode_t *new_zigbeeNode_t(azap_uint16 nwkAddr, azap_uint8 *ieeeAddr)
{
  zigbeeNode_t *This = malloc(sizeof(zigbeeNode_t));
  if (!This)
  {
    return NULL;
  }
  zigbeeNode_t_init(This);

  This->cp_cmn.cmn_num = nwkAddr;
  memcpy(This->ieeeAddr, ieeeAddr, IEEE_ADDR_LEN);
  sprintf(This->cp_cmn.cmn_ieee, "%s", azap_extAddrToString(ieeeAddr, false));
  This->cp_cmn.cmn_serial = DoSerial();
  This->cp_cmn.cmn_dia_devok  = 0;
  This->cp_cmn.cmn_dia_appok  = 0;
  This->cp_cmn.cmn_dia_accok  = 0;
  This->cp_cmn.cmn_dia_datok  = 0;
  This->cp_cmn.cmn_dia_metok  = 0;
  This->cp_cmn.cmn_dia_repok  = 0;
  This->cp_cmn.cmn_dia_reperr  = 0;

  This->free = zigbeeNode_t_newFree;
  RTL_TRDBG(TRACE_DETAIL, "new zigbeeNode_t (nwkAddr:0x%.4x)\n", nwkAddr);
  return This;
}

/**
 * @brief do dynamic allocation of a zigbeeNode_t.
 * @return the copy of the object
 */
zigbeeNode_t *new_zigbeeNode_t_ext()
{
  zigbeeNode_t *This = malloc(sizeof(zigbeeNode_t));
  if (!This)
  {
    return NULL;
  }
  zigbeeNode_t_init(This);

  This->cp_cmn.cmn_serial = DoSerial();

  This->free = zigbeeNode_t_newFree;
  RTL_TRDBG(TRACE_DETAIL, "new zigbeeNode_t (serial:%d)\n", This->cp_cmn.cmn_serial);
  return This;
}


/**
 * @brief Destructor for dynamic allocation
 * @anchor zigbeeNode_t_newFree
 */
void zigbeeNode_t_newFree(zigbeeNode_t *This)
{
  RTL_TRDBG(TRACE_DETAIL, "zigbeeNode_t::newFree (nwkAddr:0x%.4x)\n", This->cp_cmn.cmn_num);
  while ( ! list_empty(&(This->endPointList)))
  {
    azapEndPoint_t *ep = list_entry(This->endPointList.next, azapEndPoint_t, chainLink);
    list_del(&ep->chainLink);
    ep->free(ep);
  }
  free(This);
}

/* implementation methods */
/**
 * @brief Clear all dIa request counters.
 * @anchor zigbeeNode_t_clearDiaCounters
 * As a consequence, the driver will automatically update/create the M2M resources
 * associated to the device.
 * @param This the class instance
 */
void zigbeeNode_t_clearDiaCounters(zigbeeNode_t *This)
{
  This->cp_cmn.cmn_dia_devok = 0;
  This->cp_cmn.cmn_dia_appok = 0;
  This->cp_cmn.cmn_dia_accok = 0;
  This->cp_cmn.cmn_dia_datok = 0;
  This->nbContainersToCreate = -1;
}


/**
 * @brief Restore an end point representation associated to this node from Xo object.
 * @param This the class instance
 * @param xo the Xo object corresponding to the file to load.
 * @param eltNum the end point number within the Xo object to restore.
 * @param file the name of the file from which the node is restored.
 */
static azapEndPoint_t *zigbeeNode_t_loadEpFromXo(zigbeeNode_t *This, void *xo,
    int eltNum, char *file)
{
  azapEndPoint_t *ep;
  int i, value, rc, nb, nbAttr;
  char *pPtr;
  azap_uint8 id = 0;
  azap_uint8 appDevVer = 0;
  azap_uint8 appNumInClusters = 0;
  azap_uint8 appNumOutClusters = 0;
  azap_uint16 appProfId = 0;
  azap_uint16 appDeviceId = 0;
  azap_uint16 *appInClusterList = NULL;
  azap_uint16 *appOutClusterList = NULL;
  void *e, *itf;
  m2mZclAbstraction_t *abstract;
  azapZclDiscoverRspCommand_t *discoverRsp;
  azapZclCfgReportCommand_t *cfgReportCmd;
  azapAttribute_t *attr;

  e = XoNmGetAttr(xo, "zigb:endPoints[%d]", NULL, eltNum);
  if (!e)
  {
    return NULL;
  }

  RTL_TRDBG(TRACE_IMPL, "zigbeeNode_t::loadEpFromFile: adding new end point (file:%s)\n",
      file);

  // end point identifier
  pPtr = XoNmGetAttr(e, "zigb:endPointId", &rc);
  if ((pPtr) && (*pPtr) && (sscanf(pPtr, "%x", &value)))
  {
    id = value;
  }

  // application profile identifier
  pPtr = XoNmGetAttr(e, "zigb:appProfId", &rc);
  if ((pPtr) && (*pPtr) && (sscanf(pPtr, "%x", &value)))
  {
    appProfId = value;
  }

  // application device identifier
  pPtr = XoNmGetAttr(e, "zigb:appDeviceId", &rc);
  if ((pPtr) && (*pPtr) && (sscanf(pPtr, "%x", &value)))
  {
    appDeviceId = value;
  }

  // application device version
  pPtr = XoNmGetAttr(e, "zigb:appDevVer", &rc);
  if ((pPtr) && (*pPtr) && (sscanf(pPtr, "%x", &value)))
  {
    appDevVer = value;
  }

  appNumInClusters = XoNmNbInAttr(e, "zigb:clusterIn");
  if (appNumInClusters > 0)
  {
    appInClusterList = malloc(appNumInClusters * sizeof(azap_uint16));
    for (i = 0; i < appNumInClusters; i++)
    {
      pPtr = XoNmGetAttr(e, "zigb:clusterIn[%d]", &rc, i);
      if ((pPtr) && (*pPtr) && (sscanf(pPtr, "%x", &value)))
      {
        appInClusterList[i] = (azap_uint16)value;
      }
    }
  }

  appNumOutClusters = XoNmNbInAttr(e, "zigb:clusterOut");
  if (appNumOutClusters > 0)
  {
    appOutClusterList = malloc(appNumOutClusters * sizeof(azap_uint16));
    for (i = 0; i < appNumOutClusters; i++)
    {
      pPtr = XoNmGetAttr(e, "zigb:clusterOut[%d]", &rc, i);
      if ((pPtr) && (*pPtr) && (sscanf(pPtr, "%x", &value)))
      {
        appOutClusterList[i] = (azap_uint16)value;
      }
    }
  }

  ep = new_azapEndPoint_t(id, appProfId, appDeviceId, appDevVer, appNumInClusters,
      appInClusterList, appNumOutClusters, appOutClusterList);

  if (appInClusterList)
  {
    free(appInClusterList);
  }
  if (appOutClusterList)
  {
    free(appOutClusterList);
  }

  if (This->addEndPoint(This, ep))
  {
    RTL_TRDBG(TRACE_IMPL, "zigbeeNode_t::loadEpFromFile: tuning end point %d (file:%s)\n",
        id, file);
    // restore ZCL abstraction instances
    for (i = 0; i < ep->appNumInClusters; i++)
    {
      if (NULL != MdlGetInterface(MdlCfg, NULL, "server",
          azap_uint16ToString(ep->appInClusterList[i], true)))
      {
        This->addM2mZclAbstraction(This, ep->id, ep->appInClusterList[i]);
      }
    }
    zigbeeNode_t_loadAttributesFromXo(This, ep, e);
    // restore ZCL abstraction instances
    for (i = 0; i < ep->appNumInClusters; i++)
    {
      if (NULL != (itf = MdlGetInterface(MdlCfg, NULL, "server",
          azap_uint16ToString(ep->appInClusterList[i], true))))
      {
        abstract = This->getM2mZclAbstraction(This, ep->id, ep->appInClusterList[i]);
        if ((abstract) && ((nb = MdlGetNbCovConfigurationEntries(itf)) > 0))
        {
          // Determine how many attributes are supported for this cluster
          nbAttr = 0;
          while (NULL != ep->getNthAttribute(ep, ep->appInClusterList[i], nbAttr))
          {
            nbAttr++;
          }
          // allocate the mocked ZCL discover response
          discoverRsp = malloc(sizeof(azapZclDiscoverRspCommand_t) +
              (nbAttr * sizeof(azapZclDiscoverAttr_t)));
          memset(discoverRsp, 0, sizeof(azapZclDiscoverRspCommand_t) +
              (nbAttr * sizeof(azapZclDiscoverAttr_t)));
          // fill in the mocked ZCL discover response
          discoverRsp->numAttr = nbAttr;
          discoverRsp->discComplete = 1;
          for (nbAttr = 0;
              (nbAttr < discoverRsp->numAttr) &&
              (NULL != (attr = ep->getNthAttribute(ep, ep->appInClusterList[i], nbAttr)));
              nbAttr++)
          {
            discoverRsp->attrList[nbAttr].attrId = attr->attrId;
            if (attr->value)
            {
              discoverRsp->attrList[nbAttr].dataType = attr->value->zigbeeDataType;
            }
          }

          // now configure the abstraction
          cfgReportCmd = getCfgReportCmdFromCovConfig(itf, nb, discoverRsp);
          abstract->configureReporting(abstract, cfgReportCmd);

          // and finally free resources
          for (nbAttr = 0; nbAttr < cfgReportCmd->numAttr; nbAttr++)
          {
            if (cfgReportCmd->attrList[nbAttr].reportableChange)
            {
              free(cfgReportCmd->attrList[nbAttr].reportableChange);
            }
          }
          free(cfgReportCmd);
          free(discoverRsp);
        }
      }
    }
  }
  else
  {
    ep->free(ep);
    ep = NULL;
  }

  return ep;
}

/**
 * @brief Restore ZigBee attribute elements from Xo object.
 * @param This the class instance
 * @param ep the end point local representation to which ZigBee attributes are associated.
 * @param xo the Xo object corresponding to end point sub-part of the file to load.
 */
static void zigbeeNode_t_loadAttributesFromXo(zigbeeNode_t *This, azapEndPoint_t *ep,
    void *xo)
{
  int i, nb, value, rc;
  char *pPtr;
  void *a;
  azap_uint16 clId, attrId;
  azap_uint8 len;
  azap_byte val[255];
  azap_byte *pVal;
  azapAttribute_t *attr;

  if (!ep)
  {
    return;
  }
  if (!xo)
  {
    return;
  }

  nb = XoNmNbInAttr(xo, "zigb:attributes");
  if (nb > 0)
  {
    for (i = 0 ; i < nb ; i++)
    {
      clId = 0;
      attrId = 0;
      a = XoNmGetAttr(xo, "zigb:attributes[%d]", NULL, i);
      // Get the cluster Id
      pPtr = XoNmGetAttr(a, "zigb:clusterId", &rc);
      if ((pPtr) && (*pPtr) && (sscanf(pPtr, "%x", &value)))
      {
        clId = value;
      }
      else
      {
        continue;
      }
      // Get the attr Id
      pPtr = XoNmGetAttr(a, "zigb:attrId", &rc);
      if ((pPtr) && (*pPtr) && (sscanf(pPtr, "%x", &value)))
      {
        attrId = value;
      }
      else
      {
        continue;
      }

      attr = ep->addAttribute(ep, clId, attrId);
      if (!attr)
      {
        continue;
      }

      // Get the manufacturer code
      pPtr = XoNmGetAttr(a, "zigb:manuCode", &rc);
      if ((pPtr) && (*pPtr) && (sscanf(pPtr, "%x", &value)))
      {
        attr->manufacturerCode = value;
      }

      // Get the data type
      pPtr = XoNmGetAttr(a, "zigb:dataType", &rc);
      if ((pPtr) && (*pPtr) && (sscanf(pPtr, "%x", &value)))
      {
        attr->value = new_azapZclAttrVal_t(value);
      }
      else
      {
        continue;
      }

      // Get the value
      pPtr = XoNmGetAttr(a, "zigb:value", &rc);
      memset(val, 0, 255);
      if ((pPtr) && (*pPtr) && (len = stringToAzap_byteArray(val, 255, pPtr)))
      {
        pVal = val;
        attr->value->deserialize(attr->value, &pVal, &len);
      }
    }
  }
}

/**
 * @brief Load a node descrition from a "knownsensors" file.
 * @anchor zigbeeNode_t_loadFromFile
 * @param This the class instance
 * @param panId the PAN identifier the device is associated to.
 * @param extPanId the extended PAN identifier the device is associated to.
 * @param rfChan the PAN RF channel frequency.
 * @param path the path to the file to load.
 * @param file the name of file to load.
 * @return 1 if the node can be rebuilt successfully, a negative number
 * providing the error code otherwise.
 */
int zigbeeNode_t_loadFromFile(zigbeeNode_t *This, azap_uint16 panId,
    azap_uint8 *extPanId, azap_uint8 rfChan, char *path, char *file)
{
  void *o;
  int res = 1;
  int rc, nb, parse, i, value;
  char *pPtr;
  char fullFileName[255];
  snprintf(fullFileName, 255, "%s/%s", path, file);
  if (access(fullFileName, R_OK) != 0)
  {
    RTL_TRDBG(TRACE_ERROR, "zigbeeNode_t_loadFromFile: error - "
        "unable to load node from file '%s'\n", fullFileName);
    return -1;
  }

  o = XoReadXmlEx(fullFileName, NULL, 0, &parse);
  if  (!o)
  {
    RTL_TRDBG(TRACE_ERROR, "zigbeeNode_t_loadFromFile: error cannot read xml "
        "(file:'%s') (parse:%d)\n", fullFileName, parse);
    return  -2;
  }

  // PAN identifier
  pPtr = XoNmGetAttr(o, "zigb:panId", &rc);
  if ((pPtr) && (*pPtr) && (sscanf(pPtr, "%x", &value)) && (value != panId))
  {
    RTL_TRDBG(TRACE_IMPL, "zigbeeNode_t_loadFromFile: ignoring this file, PAN id "
        "doesn't match (file:'%s') (panId:%s) (expected panId:0x%.4x)\n",
        fullFileName, pPtr, panId);
    return  -3;
  }

  // Extended PAN identifier
  pPtr = XoNmGetAttr(o, "zigb:extPanId", &rc);
  if ((pPtr) && (*pPtr) && (strcmp(pPtr, azap_extAddrToString(extPanId, true))))
  {
    RTL_TRDBG(TRACE_IMPL, "zigbeeNode_t_loadFromFile: ignoring this file, PAN id "
        "doesn't match (file:'%s') (extPanId:%s) (expected iextPanId:%s)\n",
        fullFileName, pPtr, azap_extAddrToString(extPanId, true));
    return  -4;
  }

  // RF Channel frequency
  pPtr = XoNmGetAttr(o, "zigb:rfChan", &rc);
  if ((pPtr) && (*pPtr) && (sscanf(pPtr, "%x", &value)) && (value != rfChan))
  {
    RTL_TRDBG(TRACE_IMPL, "zigbeeNode_t_loadFromFile: ignoring this file, RF Channel "
        "doesn't match (file:'%s') (RFChan:%s) (expected RFChan:0x%.2x)\n",
        fullFileName, pPtr, rfChan);
    return  -5;
  }

  // Manufacturer code
  pPtr = XoNmGetAttr(o, "zigb:manuCode", &rc);
  if ((pPtr) && (*pPtr) && (sscanf(pPtr, "%x", &value)))
  {
    This->manuCode = value;
  }

  // node type
  pPtr = XoNmGetAttr(o, "zigb:nodeType", &rc);
  if ((pPtr) && (*pPtr) && (sscanf(pPtr, "%x", &value)))
  {
    This->nodeType = value;
  }

  // power mode
  pPtr = XoNmGetAttr(o, "zigb:powerMode", &rc);
  if ((pPtr) && (*pPtr) && (sscanf(pPtr, "%x", &value)))
  {
    This->powerMode = value;
  }

  // power Source
  pPtr = XoNmGetAttr(o, "zigb:powerSource", &rc);
  if ((pPtr) && (*pPtr) && (sscanf(pPtr, "%x", &value)))
  {
    This->powerSource = value;
  }

  // power level
  pPtr = XoNmGetAttr(o, "zigb:powerLevel", &rc);
  if ((pPtr) && (*pPtr) && (sscanf(pPtr, "%x", &value)))
  {
    This->powerLevel = value;
  }

  // IEEE address
  pPtr = XoNmGetAttr(o, "zigb:ieeeAddr", &rc);
  if ((pPtr) && (*pPtr) && (stringToAzap_extAddr(This->ieeeAddr, pPtr)))
  {
    sprintf(This->cp_cmn.cmn_ieee, pPtr + 2); // "zigb:ieeeAddr" is saved with "0x" prefix
  }
  else
  {
    RTL_TRDBG(TRACE_ERROR, "zigbeeNode_t_loadFromFile: error - "
        "unable to retrieve the IEEE address from file '%s' (zigb:ieeeAddr=%s)\n",
        fullFileName, pPtr);
    res = -3;
  }

  // short address
  pPtr = XoNmGetAttr(o, "zigb:shortAddr", &rc);
  if ((pPtr) && (*pPtr) && (sscanf(pPtr, "%x", &value)))
  {
    This->cp_cmn.cmn_num = value;
  }
  else
  {
    RTL_TRDBG(TRACE_ERROR, "zigbeeNode_t_loadFromFile: error - "
        "unable to retrieve the short address from file '%s' (zigb:shortAddr=%s)\n",
        fullFileName, pPtr);
    res = -4;
  }

  // "to delete" optional flag
  pPtr = XoNmGetAttr(o, "zigb:toDelete", &rc);
  if ((pPtr) && (*pPtr) && (sscanf(pPtr, "%d", &value)))
  {
    This->toDelete = value;
  }

  if (This->toDelete) 
  {
    This->cp_cmn.cmn_dia_devok ++;
  }

  nb = XoNmNbInAttr(o, "zigb:endPoints");
  if (nb > 0)
  {
    for (i = 0 ; i < nb ; i++)
    {
      zigbeeNode_t_loadEpFromXo(This, o, i, fullFileName);
      if (This->toDelete) 
      {
        This->cp_cmn.cmn_dia_appok ++;
        This->cp_cmn.cmn_dia_accok += 3;
      }
    }
  }
  XoFree(o, 1);
  return res;
}
/**
 * @brief Save the end point local representation into the Xo object.
 * @param This the class instance
 * @param ep the local end point represenation.
 * @param epDesc the Xo object for the end point entity.
 */
static void zigbeeNode_t_saveEpToXo(zigbeeNode_t *This, azapEndPoint_t *ep, void *epDesc)
{
  int i;
  azapAttribute_t *attr;
  void *xoAttr;
  azap_byte val[255];
  azap_byte *pVal;
  azap_uint8 len;
  if (! ep)
  {
    return;
  }
  if (! epDesc)
  {
    return;
  }
  XoNmSetAttr(epDesc, "zigb:endPointId", azap_uint8ToString(ep->id, true), 0);
  XoNmSetAttr(epDesc, "zigb:appProfId", azap_uint16ToString(ep->appProfId, true), 0);
  XoNmSetAttr(epDesc, "zigb:appDeviceId", azap_uint16ToString(ep->appDeviceId, true), 0);
  XoNmSetAttr(epDesc, "zigb:appDevVer", azap_uint8ToString(ep->appDevVer, true), 0);
  for (i = 0; i < ep->appNumInClusters; i++)
  {
    XoNmAddInAttr(epDesc, "zigb:clusterIn",
        azap_uint16ToString(ep->appInClusterList[i], true), 0);
  }
  for (i = 0; i < ep->appNumOutClusters; i++)
  {
    XoNmAddInAttr(epDesc, "zigb:clusterOut",
        azap_uint16ToString(ep->appOutClusterList[i], true), 0);
  }
  i = 0;
  while (NULL != (attr = ep->getNthAttribute(ep, 0xFFFF, i)))
  {
    xoAttr = XoNmNew("zigb:attribute");
    XoNmSetAttr(xoAttr, "zigb:clusterId", azap_uint16ToString(attr->clusterId, true), 0);
    XoNmSetAttr(xoAttr, "zigb:attrId", azap_uint16ToString(attr->attrId, true), 0);
    if (0x0000 != attr->manufacturerCode)
    {
      XoNmSetAttr(xoAttr, "zigb:manuCode", azap_uint16ToString(attr->manufacturerCode, true),
          0);
    }
    if (attr->value)
    {
      XoNmSetAttr(xoAttr, "zigb:dataType", azap_uint8ToString(attr->value->zigbeeDataType,
          true), 0);
      len = 255;
      pVal = val;
      attr->value->serialize(attr->value, &pVal, &len);
      XoNmSetAttr(xoAttr, "zigb:value", azap_byteArrayToString(val, 255 - len), 0);
    }
    XoNmAddInAttr(epDesc, "zigb:attributes", xoAttr, 0, 0);
    i++;
  }
}

/**
 * @brief Save the node descrition to a "knownsensors" file.
 * @anchor zigbeeNode_t_saveToFile
 * @param This the class instance
 * @param panId the PAN identifier the device is associated to.
 * @param extPanId the extended PAN identifier the device is associated to.
 * @param rfChan the PAN RF channel frequency.
 * @param path the path to the file to create/replace.
 * @return 1 if the file can be saved successfully, 0 otherwise.
 */
int zigbeeNode_t_saveToFile(zigbeeNode_t *This, azap_uint16 panId,
    azap_uint8 *extPanId, azap_uint8 rfChan, char *path)
{
  int res = 0;
  int rc;
  char target[255];
  char temp[10];
  void *xo, *epDesc;
  azapEndPoint_t *ep;
  struct list_head *link;

  if (!path)
  {
    return 0;
  }

  xo = XoNmNew("zigb:node");
  if (!xo)
  {
    return 0;
  }

  XoNmSetAttr(xo, "name$", "zigb:node", 0);

  XoNmSetAttr(xo, "zigb:panId", azap_uint16ToString(panId, true), 0);
  XoNmSetAttr(xo, "zigb:extPanId", azap_extAddrToString(extPanId, true), 0);
  XoNmSetAttr(xo, "zigb:rfChan", azap_uint8ToString(rfChan, true), 0);
  XoNmSetAttr(xo, "zigb:manuCode", azap_uint16ToString(This->manuCode, true), 0);
  XoNmSetAttr(xo, "zigb:nodeType", azap_uint8ToString(This->nodeType, true), 0);
  XoNmSetAttr(xo, "zigb:powerMode", azap_uint8ToString(This->powerMode, true), 0);
  XoNmSetAttr(xo, "zigb:powerSource", azap_uint8ToString(This->powerSource, true), 0);
  XoNmSetAttr(xo, "zigb:powerLevel", azap_uint8ToString(This->powerLevel, true), 0);
  XoNmSetAttr(xo, "zigb:ieeeAddr", azap_extAddrToString(This->ieeeAddr, true), 0);
  XoNmSetAttr(xo, "zigb:shortAddr", azap_uint16ToString(This->cp_cmn.cmn_num, true), 0);
  sprintf(temp, "%d", This->toDelete); 
  XoNmSetAttr(xo, "zigb:toDelete", temp, 0);

  list_for_each (link, &(This->endPointList))
  {
    ep = list_entry(link, azapEndPoint_t, chainLink);
    epDesc = XoNmNew("zigb:endPoint");
    zigbeeNode_t_saveEpToXo(This, ep, epDesc);
    XoNmAddInAttr(xo, "zigb:endPoints", epDesc, 0, 0);
  }

  sprintf(target, "%s/%s.xml", path, This->cp_cmn.cmn_ieee);
  rc = XoWritXmlEx(target, xo, 0, "zigb");
  if (rc < 0)
  {
    RTL_TRDBG(TRACE_ERROR, "zigbeeNode_t_saveToFile: error - cannot save xml file "
        "(target:%s) (rc:%d)\n", target, rc);
  }
  XoFree(xo, 1);

  return res;
}

/**
 * @brief Dump using provided function information on the current node.
 * @anchor zigbeeNode_t_dumpNode
 * @param This the class instance
 * @param printFunc the function to invoke for dumping the built node image.
 * @param cl the CLI on which the node image should be dumped on.
 * @param verbose Display more information, such as ZCL attributes values.
 */
void zigbeeNode_t_dumpNode(zigbeeNode_t *This, pf_print printFunc, t_cli *cl,
    int verbose)
{
  struct list_head *link;
  int i;
  azapAttribute_t *attr;
  char szTemp[32];

  if (!printFunc)
  {
    return;
  }

  printFunc(cl, "* [serial:%d] [nwkAddr:0x%.4x] [ieee:0x%s] [type:%d] [pwrMode:%d]\n  ",
      This->cp_cmn.cmn_serial, This->cp_cmn.cmn_num, This->cp_cmn.cmn_ieee, This->nodeType,
      This->powerMode);

  if (This->toDelete)
  {
    printFunc(cl, "[TO BE DELETED] ");
  }
  if (0 != This->lastAuditDate)
  {
    printFunc(cl, "[last audit:%s] ", rtl_aaaammjjhhmmss(This->lastAuditDate, szTemp));
  }
  printFunc(cl, "[audit pending:%d] ", (NULL != This->azapAuditContext));
  if (This->azapAuditContext)
  {
    printFunc(cl, "[azapPendingReq:%d]\n", This->azapAuditContext->nbPendingRequests);
  }
  else
  {
    printFunc(cl, "\n");
  }
  printFunc(cl, "  [dIaDevOk:%d] [dIaAppOk:%d] [dIaArOk:%d] [dIaDatOk:%d/%d]\n"
      "  [dIaReportOk:%d] [dIaReportErr:%d]\n", This->cp_cmn.cmn_dia_devok,
      This->cp_cmn.cmn_dia_appok, This->cp_cmn.cmn_dia_accok,
      This->cp_cmn.cmn_dia_datok, This->nbContainersToCreate,
      This->cp_cmn.cmn_dia_repok, This->cp_cmn.cmn_dia_reperr);

  list_for_each (link, &(This->endPointList))
  {
    azapEndPoint_t *ep = list_entry(link, azapEndPoint_t, chainLink);
    printFunc(cl, "  - [endPointId:%d] [profileId:0x%.4x] [NbINCluster:%d] "
        "[nbOutCluster:%d]\n", ep->id, ep->appProfId, ep->appNumInClusters,
        ep->appNumOutClusters);
    printFunc(cl, "    [INClusterList: %s]\n", azap_uint16ArrayToString(
        ep->appInClusterList, ep->appNumInClusters));
    printFunc(cl, "    [OUTClusterList:%s]\n", azap_uint16ArrayToString(
        ep->appOutClusterList, ep->appNumOutClusters));

    if (verbose)
    {
      i = 0;
      while (NULL != (attr = ep->getNthAttribute(ep, 0xFFFF, i)))
      {
        printFunc(cl, "    [#%03d] [cluster:0x%.4x] [attr:0x%.4x]",
            i, attr->clusterId, attr->attrId, attr->manufacturerCode);
        if (0x0000 != attr->manufacturerCode)
        {
          printFunc(cl, " [manuCode:0x%.4x]", attr->manufacturerCode);
        }
        if (attr->value)
        {
          printFunc(cl, " [type:0x%.2x] [value:%s]", attr->value->zigbeeDataType,
              attr->value->getStringRep(attr->value));
        }
        printFunc(cl, "\n");
        i++;
      }
    }
  }
  printFunc(cl, "\n");
}

/**
 * @brief Add a end point to the node.
 * @anchor zigbeeNode_t_addEndPoint
 * @param This the class instance
 * @param ep the new end point to add to the list of known end points
 * @return 1 if end point can be added, 0 otherwise.
 */
int zigbeeNode_t_addEndPoint(zigbeeNode_t *This, azapEndPoint_t *ep)
{
  if (! ep)
  {
    return 0;
  }
  if (NULL != This->getEndPoint(This, ep->id))
  {
    return 0;
  }
  RTL_TRDBG(TRACE_INFO, "zigbeeNode_t::addEndPoint (nwkAddr:0x%.4x) (EP:%d)\n",
      This->cp_cmn.cmn_num, ep->id);
  list_add_tail(&ep->chainLink, &(This->endPointList));
  return 1;
}

/**
 * @brief Get the end point instance that matches the identifier epId.
 * @anchor zigbeeNode_t_getEndPoint
 * @param This the class instance
 * @param epId the end point identifier to retrieve.
 * @return the matching end point instance if found, NULL otherwise.
 */
azapEndPoint_t *zigbeeNode_t_getEndPoint(zigbeeNode_t *This, azap_uint8 epId)
{
  azapEndPoint_t *res = NULL;
  struct list_head *link;
  list_for_each (link, &(This->endPointList))
  {
    azapEndPoint_t *ep = list_entry(link, azapEndPoint_t, chainLink);
    if (epId == ep->id)
    {
      res = ep;
    }
  }
  return res;
}

/**
 * @brief Get the Nth end point instance from the list.
 * @anchor zigbeeNode_t_getNthEndPoint
 * @param This the class instance
 * @param index the element to retrieve, first index is 0.
 * @return the Nth end point instance if exists, NULL otherwise.
 */
azapEndPoint_t *zigbeeNode_t_getNthEndPoint(zigbeeNode_t *This, int index)
{
  azapEndPoint_t *res = NULL;
  struct list_head *link;
  list_for_each (link, &(This->endPointList))
  {
    azapEndPoint_t *ep = list_entry(link, azapEndPoint_t, chainLink);
    if (0 == index)
    {
      res = ep;
      break;
    }
    index --;
  }
  return res;
}

/**
 * @brief Add a M2M abstraction for ZCL cluster refering to the remote end point capabilities.
 * @anchor zigbeeNode_t_addM2mZclAbstraction
 * @param This the class instance
 * @param epId the end point identifier the abstraction object refers to.
 * @param clId the cluster identifier the abstraction object refers to.
 * @return the new added abstraction (or the one already representing the end point for
 * this cluster), or NULL if the driver does not support this cluster interface.
 */
m2mZclAbstraction_t *zigbeeNode_t_addM2mZclAbstraction(zigbeeNode_t *This,
    azap_uint8 epId, azap_uint16 clId)
{
  if (0 == epId)
  {
    return NULL;
  }
  // check if it does not already exit.
  m2mZclAbstraction_t *newOne = This->getM2mZclAbstraction(This, epId, clId);
  if (newOne)
  {
    return newOne;
  }

  // else check against supported cluster interfaces
  switch (clId)
  {
    case ZCL_CLUSTER_ID_GEN_BASIC:
      newOne = (m2mZclAbstraction_t *)new_m2mZcl0x0000Abstraction_t(epId);
      break;
    case ZCL_CLUSTER_ID_GEN_IDENTIFY:
      newOne = (m2mZclAbstraction_t *)new_m2mZcl0x0003Abstraction_t(epId);
      break;
    case ZCL_CLUSTER_ID_GEN_ON_OFF:
      newOne = (m2mZclAbstraction_t *)new_m2mZcl0x0006Abstraction_t(epId);
      break;
    case ZCL_CLUSTER_ID_MS_ILLUMINANCE_MEASUREMENT:
      newOne = (m2mZclAbstraction_t *)new_m2mZcl0x0400Abstraction_t(epId);
      break;
    case ZCL_CLUSTER_ID_MS_TEMPERATURE_MEASUREMENT:
      newOne = (m2mZclAbstraction_t *)new_m2mZcl0x0402Abstraction_t(epId);
      break;
    case ZCL_CLUSTER_ID_MS_PRESSURE_MEASUREMENT:
      newOne = (m2mZclAbstraction_t *)new_m2mZcl0x0403Abstraction_t(epId);
      break;
    case ZCL_CLUSTER_ID_MS_FLOW_MEASUREMENT:
      newOne = (m2mZclAbstraction_t *)new_m2mZcl0x0404Abstraction_t(epId);
      break;
    case ZCL_CLUSTER_ID_MS_RELATIVE_HUMIDITY:
      newOne = (m2mZclAbstraction_t *)new_m2mZcl0x0405Abstraction_t(epId);
      break;
    case ZCL_CLUSTER_ID_SE_SIMPLE_METERING:
      newOne = (m2mZclAbstraction_t *)new_m2mZcl0x0702Abstraction_t(epId);
      break;
    default:
      RTL_TRDBG(TRACE_ERROR, "zigbeeNode_t::addM2mZclAbstraction - error: this interface "
          "is not supported yet (nwkAddr:0x%.4x) (EP:%d) (clusterId:0x%.4x)\n",
          This->cp_cmn.cmn_num, epId, clId);
      newOne = NULL;
      break;
  }

  if (newOne)
  {
    RTL_TRDBG(TRACE_INFO, "zigbeeNode_t::addM2mZclAbstraction (nwkAddr:0x%.4x) (EP:%d) "
        "(clusterId:0x%.4x)\n", This->cp_cmn.cmn_num, epId, clId);
    list_add_tail(&newOne->chainLink, &(This->m2mZclAbstractionList));
  }
  return newOne;
}

/**
 * @brief Get the M2M abstraction that matches given cluster and remote end point identifier.
 * @anchor zigbeeNode_t_getM2mZclAbstraction
 * @param This the class instance
 * @param epId the end point identifier the abstraction object refers to.
 * @param clId the cluster identifier the abstraction object refers to.
 */
m2mZclAbstraction_t *zigbeeNode_t_getM2mZclAbstraction(zigbeeNode_t *This, azap_uint8 epId,
    azap_uint16 clId)
{
  m2mZclAbstraction_t *res = NULL;
  struct list_head *link;
  list_for_each (link, &(This->m2mZclAbstractionList))
  {
    m2mZclAbstraction_t *abstraction = list_entry(link, m2mZclAbstraction_t, chainLink);
    if ((epId == abstraction->endPointId) && (clId == abstraction->clusterId))
    {
      res = abstraction;
    }
  }
  return res;
}

/**
 * @brief Build the representation of the node application list and add it to the oBIX resource.
 * @anchor zigbeeHan_t_buildAppList
 * @param This the class instance
 * @param dst the oBIX document in which adding the node application list representation.
 */
void zigbeeNode_t_buildAppList(zigbeeNode_t *This, void *dst)
{
  struct list_head *link;
  char target[256];
  void *dev;

  if (XoIsObix(dst) <= 0)
  {
    RTL_TRDBG(TRACE_ERROR, "zigbeeNode_t::buildAppList - error: "
        "not an obix object\n");
    return;
  }

  list_for_each (link, &(This->endPointList))
  {
    azapEndPoint_t *ep = list_entry(link, azapEndPoint_t, chainLink);
    dev = XoNmNew("o:ref");
    if (!dev)
    {
      RTL_TRDBG(TRACE_ERROR, "zigbeeNode_t::buildAppList - error: "
          "cannot allocate 'o:ref' for end point %d\n", ep->id);
      continue;
    }
    XoNmSetAttr(dev, "name$", "ref", 0);

    sprintf(target, "%s/APP_%s.%d/containers/DESCRIPTOR/contentInstances/latest/content",
        SCL_ROOT_APP, This->cp_cmn.cmn_ieee, ep->id);
    XoNmSetAttr(dev, "href", target, 0);
    XoNmAddInAttr(dst, "[name=applications].[]", dev, 0, 0);
  }
}

/**
 * Create the containers required for data reporting/logging within the application.
 * @anchor zigbeeNode_t_createDataContainers
 * @param This the class instance
 */
void zigbeeNode_t_createDataContainers(zigbeeNode_t *This)
{
  struct list_head *link;
  azap_uint8 i;
  azap_uint8 j;
  azap_uint16 clusterId;
  void *mdlitf;
  interfaceMappingDesc_t *desc;
  char szTemp[32];
  char *varVal;
  void *descriptor;

  This->nbContainersToCreate = 0;

  list_for_each (link, &(This->endPointList))
  {
    azapEndPoint_t *ep = list_entry(link, azapEndPoint_t, chainLink);
    for (i = 0; i < ep->appNumInClusters; i++)
    {
      clusterId = ep->appInClusterList[i];
      if (NULL != (mdlitf = MdlGetInterface(MdlCfg, NULL, "server",
          azap_uint16ToString(clusterId, true))))
      {
        desc = theHan->getMapping(theHan, clusterId, ZCL_FRAME_SERVER_CLIENT_DIR);
        if (NULL != desc)
        {
          for (j = 0; j < desc->nbAttributes; j++)
          {
            sprintf(szTemp, "0x%.4x.0x%.4x.0.m2m", clusterId, desc->attributes[j]);
            varVal = GetVarNoWarn(szTemp);
            if ((varVal) && (*varVal) &&
                (NULL != (descriptor = MdlGetDescriptorByName(mdlitf, varVal))))
            {
              // Create the containers for reporting
              RTL_TRDBG(TRACE_INFO, "zigbeeNode_t::createDataContainers - creating container "
                  "for reporting (nwkAddr:0x%.4x) (ep:%d) (clusterId:0x%.4x) (attrId:0x%.4x)\n",
                  This->cp_cmn.cmn_num, ep->id, clusterId, desc->attributes[j]);
              This->nbContainersToCreate++;
              DiaDatStart(This->cp_cmn.cmn_num, This->cp_cmn.cmn_serial, ep->id, clusterId,
                  desc->attributes[j], 0);
            }
          }
        }
      }
    }
  }
}


/**
 * Send dIa requests to create all M2M resources associated to this device.
 * This function manages all the dIa requests regarding a device. But the requests
 * are not all sent at the same time but spread over time. Indeed this function is
 * invoked periodically.
 * @anchor zigbeeNode_t_publish
 * @param This the class instance
 * @return the number of sent requests.
 */
int zigbeeNode_t_publish(zigbeeNode_t *This)
{
  struct list_head *link;
  m2mZclAbstraction_t *abstraction;
  azap_uint8 i;
  int res = 0;

  if (This->azapAuditContext)
  {
    return 0;
  }
  if (This->toDelete)
  {
    return 0;
  }

  RTL_TRDBG(TRACE_DETAIL, "zigbeeNode_t::publish (nwkAddr:0x%.4x) (dia_accok:%d) "
      "(dia_appok:%d) (dia_devok:%d)\n", This->cp_cmn.cmn_num, This->cp_cmn.cmn_dia_accok,
      This->cp_cmn.cmn_dia_appok, This->cp_cmn.cmn_dia_devok);

  // Either create the ACRs associated to the device applications
  if (0 == This->cp_cmn.cmn_dia_accok)
  {
    list_for_each (link, &(This->endPointList))
    {
      azapEndPoint_t *ep = list_entry(link, azapEndPoint_t, chainLink);
      DiaAccStart(This->cp_cmn.cmn_num, This->cp_cmn.cmn_serial, ep->id, 1);
      DiaAccStart(This->cp_cmn.cmn_num, This->cp_cmn.cmn_serial, ep->id, 2);
      DiaAccStart(This->cp_cmn.cmn_num, This->cp_cmn.cmn_serial, ep->id, 3);
      res += 3;
    }
  }
  // Either create the application applications
  else if (0 == This->cp_cmn.cmn_dia_appok)
  {
    list_for_each (link, &(This->endPointList))
    {
      azapEndPoint_t *ep = list_entry(link, azapEndPoint_t, chainLink);
      DiaAppStart(This->cp_cmn.cmn_num, This->cp_cmn.cmn_serial, ep->id);
      res ++;

      for (i = 0; i < ep->appNumInClusters; i++)
      {
        abstraction = This->getM2mZclAbstraction(This, ep->id, ep->appInClusterList[i]);
        if (abstraction)
        {
          abstraction->collectConsolidationElts(abstraction, ep);
        }
      }
    }
  }
  // Either create the application applications containers
  else if ((This->nbContainersToCreate == -1) ||
      (This->nbContainersToCreate > This->cp_cmn.cmn_dia_datok))
  {
    This->cp_cmn.cmn_dia_datok = 0;
    This->createDataContainers(This);
  }
  // Either create the device applications
  else if (0 == This->cp_cmn.cmn_dia_devok)
  {
    DiaDevStart(This->cp_cmn.cmn_num, This->cp_cmn.cmn_serial);
    DiaNetStart();
    res += 2;
  }

  return res;
}

/**
 * Send dIa requests to delete all M2M resources associated to this device.
 * This function manages all the dIa requests regarding a device. But the requests
 * are not all sent at the same time but spread over time. Indeed this function is
 * invoked periodically.
 * @anchor zigbeeNode_t_unpublish
 * @param This the class instance
 * @return the number of sent requests.
 */
int zigbeeNode_t_unpublish(zigbeeNode_t *This)
{
  struct list_head *link;
  int res = 0;

  if (This->azapAuditContext)
  {
    return 0;
  }
  if (! This->toDelete)
  {
    return 0;
  }

  RTL_TRDBG(TRACE_DETAIL, "zigbeeNode_t::unpublish (nwkAddr:0x%.4x) (dia_accok:%d) "
      "(dia_appok:%d) (dia_devok:%d)\n", This->cp_cmn.cmn_num, This->cp_cmn.cmn_dia_accok,
      This->cp_cmn.cmn_dia_appok, This->cp_cmn.cmn_dia_devok);

  // Delete the application applications (to be done before deleting AR!)
  if (0 != This->cp_cmn.cmn_dia_appok)
  {
    list_for_each (link, &(This->endPointList))
    {
      azapEndPoint_t *ep = list_entry(link, azapEndPoint_t, chainLink);
      DiaAppDelete(This->cp_cmn.cmn_num, This->cp_cmn.cmn_serial, ep->id);
      res ++;
    }
  }
  // Delete the ACRs associated to the device applications
  else if (0 != This->cp_cmn.cmn_dia_accok)
  {
    list_for_each (link, &(This->endPointList))
    {
      azapEndPoint_t *ep = list_entry(link, azapEndPoint_t, chainLink);
      DiaAccDelete(This->cp_cmn.cmn_num, This->cp_cmn.cmn_serial, ep->id, 1);
      DiaAccDelete(This->cp_cmn.cmn_num, This->cp_cmn.cmn_serial, ep->id, 2);
      DiaAccDelete(This->cp_cmn.cmn_num, This->cp_cmn.cmn_serial, ep->id, 3);
      res += 3;
    }
  }
  else if (0 != This->cp_cmn.cmn_dia_devok)
  {
    // And finally delete the device applications
    DiaDevDelete(This->cp_cmn.cmn_num, This->cp_cmn.cmn_serial);
    DiaNetStart();
    res += 2;
  }
  return res;
}

/**
 * @brief Ends the device audit.
 * @anchor zigbeeNode_t_endAudit
 * @param This the class instance
 * @param success indicates if the audit ends with success or not.
 * @param panId the PAN identifier the device is associated to.
 * @param extPanId the extended PAN identifier the device is associated to.
 * @param rfChan the PAN RF channel frequency.
 * @param path the path to the file to create/replace.
 */
void zigbeeNode_t_endAudit(zigbeeNode_t *This, bool success, azap_uint16 panId,
    azap_uint8 *extPanId, azap_uint8 rfChan, char *path)
{
  This->azapAuditContext = NULL;
  if (success)
  {
    time(&(This->lastAuditDate));
    This->saveToFile(This, panId, extPanId, rfChan, path);
  }
}



/**
 * Tells if the node was already audited before.
 * @anchor zigbeeNode_t_isFirstAudit
 * @param This the class instance
 * @return true if no audit was completed til now for this node, false otherwise.
 */
bool zigbeeNode_t_isFirstAudit(zigbeeNode_t *This)
{
  return ( 0 == This->lastAuditDate );
}


