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
//** File : driver/zigbee_node.h
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



#ifndef __ZIGBEE_NODE__H_
#define __ZIGBEE_NODE__H_
/*
 *   Here is an overview of the class hierarchy:
 *
 *    +---------------------------------+
 *    | zigbeeHan_t                     |
 *    +---------------------------------+
 *    | struct list_head knownNodesList |
 *    | ...                             |
 *    +---------------------------------+
 *                  /\
 *                  \/
 *                  |
 *                  |
 *     +---------------------+            +-----------------+
 *     | zigbeeNode_t        |            | t_cmn_sensor    |
 *     +---------------------+            +-----------------+
 *     | t_cmn_sensor cp_cmn |/\__________| void * cmn_self | <- Note: cmn_self points at
 *     | ...                 |\/          | ...             |    the associated zigbeeNode_t
 *     +---------------------+            +-----------------+
 */
/**
 * @struct zigbeeNode_t
 * This object is used to manage the zigBee sensor for a given HAN
 */
typedef struct zigbeeNode_t
{
  /* methods */
  /** @ref zigbeeNode_t_clearDiaCounters */
  void (*clearDiaCounters) (struct zigbeeNode_t *This);
  /** @ref zigbeeNode_t_loadFromFile */
  int (*loadFromFile) (struct zigbeeNode_t *This, azap_uint16 panId,
      azap_uint8 *extPanId, azap_uint8 rfChan, char *path, char *file);
  /** @ref zigbeeNode_t_saveToFile */
  int (*saveToFile) (struct zigbeeNode_t *This, azap_uint16 panId,
      azap_uint8 *extPanId, azap_uint8 rfChan, char *path);
  /** @ref zigbeeNode_t_dumpNode */
  void (*dumpNode) (struct zigbeeNode_t *This, pf_print printFunc, t_cli *cl,
      int verbose);
  /** @ref zigbeeNode_t_addEndPoint */
  int (*addEndPoint) (struct zigbeeNode_t *This, azapEndPoint_t *ep);
  /** @ref zigbeeNode_t_getEndPoint */
  azapEndPoint_t *(*getEndPoint) (struct zigbeeNode_t *This, azap_uint8 epId);
  /** @ref zigbeeNode_t_getNthEndPoint */
  azapEndPoint_t *(*getNthEndPoint) (struct zigbeeNode_t *This, int index);
  /** @ref zigbeeNode_t_addM2mZclAbstraction */
  struct m2mZclAbstraction_t *(*addM2mZclAbstraction) (struct zigbeeNode_t *This,
      azap_uint8 epId, azap_uint16 clId);
  /** @ref zigbeeNode_t_getM2mZclAbstraction */
  struct m2mZclAbstraction_t *(*getM2mZclAbstraction) (struct zigbeeNode_t *This,
      azap_uint8 epId, azap_uint16 clId);
  /** @ref zigbeeNode_t_buildAppList */
  void (*buildAppList) (struct zigbeeNode_t *This, void *dst);
  /** @ref zigbeeNode_t_createDataContainers */
  void (*createDataContainers) (struct zigbeeNode_t *This);
  /** @ref zigbeeNode_t_publish */
  int (*publish) (struct zigbeeNode_t *This);
  /** @ref zigbeeNode_t_unpublish */
  int (*unpublish) (struct zigbeeNode_t *This);
  /** @ref zigbeeNode_t_isFirstAudit */
  bool (*isFirstAudit) (struct zigbeeNode_t *This);
  /** @ref zigbeeNode_t_endAudit */
  void (*endAudit) (struct zigbeeNode_t *This, bool success, azap_uint16 panId,
      azap_uint8 *extPanId, azap_uint8 rfChan, char *path);
  /** @ref zigbeeNode_t_newFree */
  void (*free) (struct zigbeeNode_t *);

  /* members */
  /** The common part of the sensor (i.e. the node in case of ZigBee). */
  t_cmn_sensor cp_cmn;

  /** For nodes list management. */
  struct list_head chainLink;
  /** The IEEE address of the node (AKA the extended address). */
  azap_uint8 ieeeAddr[IEEE_ADDR_LEN];

  /** The context of AZAP requests for device audit. */
  driverReqContext_t *azapAuditContext;
  /** Date of the last audit procedure. */
  time_t lastAuditDate;
  /** The number of reporting containers to create. */
  azap_int8 nbContainersToCreate;
  /** The manufacturer code. */
  azap_uint16 manuCode;
  /** The node type: can be a router or a end device. */
  azap_uint8 nodeType;
  /** The device power mode. */
  azap_uint8 powerMode;
  /** The device power source. */
  azap_uint8 powerSource;
  /** The device power level. */
  azap_uint8 powerLevel;
  /** Indicates that the device is to be deleted. */
  bool toDelete;

  /** The list of end points. */
  struct list_head endPointList;
  /** The list of M2M abstraction clusters. */
  struct list_head m2mZclAbstractionList;

} zigbeeNode_t;

/* dynamic allocation */
zigbeeNode_t *new_zigbeeNode_t(azap_uint16 nwkAddr, azap_uint8 *ieeeAddr);
zigbeeNode_t *new_zigbeeNode_t_ext();
void zigbeeNode_t_clearDiaCounters(zigbeeNode_t *This);
int zigbeeNode_t_loadFromFile(zigbeeNode_t *This, azap_uint16 panId,
    azap_uint8 *extPanId, azap_uint8 rfChan, char *path, char *file);
int zigbeeNode_t_saveToFile(zigbeeNode_t *This, azap_uint16 panId,
    azap_uint8 *extPanId, azap_uint8 rfChan, char *path);
void zigbeeNode_t_newFree(zigbeeNode_t *This);
void zigbeeNode_t_dumpNode(zigbeeNode_t *This, pf_print printFunc, t_cli *cl,
    int verbose);
int zigbeeNode_t_addEndPoint(zigbeeNode_t *This, azapEndPoint_t *ep);
azapEndPoint_t *zigbeeNode_t_getEndPoint(zigbeeNode_t *This, azap_uint8 epId);
azapEndPoint_t *zigbeeNode_t_getNthEndPoint(zigbeeNode_t *This, int index);
struct m2mZclAbstraction_t *zigbeeNode_t_addM2mZclAbstraction(zigbeeNode_t *This,
    azap_uint8 epId, azap_uint16 clId);
struct m2mZclAbstraction_t *zigbeeNode_t_getM2mZclAbstraction(zigbeeNode_t *This,
    azap_uint8 epId, azap_uint16 clId);
void zigbeeNode_t_buildAppList(zigbeeNode_t *This, void *dst);
void zigbeeNode_t_createDataContainers(zigbeeNode_t *This);
int zigbeeNode_t_publish(zigbeeNode_t *This);
int zigbeeNode_t_unpublish(zigbeeNode_t *This);
bool zigbeeNode_t_isFirstAudit(zigbeeNode_t *This);
void zigbeeNode_t_endAudit(zigbeeNode_t *This, bool success, azap_uint16 panId,
    azap_uint8 *extPanId, azap_uint8 rfChan, char *path);

#endif

