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
//** File : driver/zigbee_han.h
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



#ifndef __ZIGBEE_HAN__H_
#define __ZIGBEE_HAN__H_

#define ZIGBEE_MAX_PERMIT_JOIN_DURATION 254

/**
 * @struct zigbeeHan_t
 * This object is used to manage the zigBee sensor for a given HAN
 */
typedef struct zigbeeHan_t
{
  /* methods */
  /** @ref zigbeeHan_t_reloadKnownNodes */
  void (*reloadKnownNodes) (struct zigbeeHan_t *This, char *path);
  /** @ref zigbeeHan_t_dumpNodeList */
  void (*dumpNodeList) (struct zigbeeHan_t *This, pf_print printFunc, t_cli *cl);
  /** @ref zigbeeHan_t_dumpNode */
  void (*dumpNode) (struct zigbeeHan_t *This, pf_print printFunc, t_cli *cl,
      azap_uint16 addr);
  /** @ref zigbeeHan_t_syncNode */
  void (*syncNode) (struct zigbeeHan_t *This, pf_print printFunc, t_cli *cl,
      azap_uint16 addr);
  /** @ref zigbeeHan_t_getNodeByNwkAddr */
  zigbeeNode_t *(*getNodeByNwkAddr) (struct zigbeeHan_t *This, azap_uint16 addr);
  /** @ref zigbeeHan_t_getNodeByIeeeAddrStr */
  zigbeeNode_t *(*getNodeByIeeeAddrStr) (struct zigbeeHan_t *This, char *ieee);
  /** @ref zigbeeHan_t_getNodeByIeeeAddr */
  zigbeeNode_t *(*getNodeByIeeeAddr) (struct zigbeeHan_t *This, azap_uint8 *ieee);
  /** @ref zigbeeHan_t_addNode */
  void (*addNode) (struct zigbeeHan_t *This, zigbeeNode_t *node);
  /** @ref zigbeeHan_t_deleteNode */
  void (*deleteNode) (struct zigbeeHan_t *This, azap_uint8 *ieee);
  /** @ref zigbeeHan_t_buildNodeList */
  void (*buildNodeList) (struct zigbeeHan_t *This, void *dst);

  /** @ref zigbeeHan_t_loadMappingFromXoRef */
  void (*loadMappingFromXoRef) (struct zigbeeHan_t *This, void *itfMappings);
  /** @ref zigbeeHan_t_addMapping */
  int (*addMapping) (struct zigbeeHan_t *This, interfaceMappingDesc_t *itfMap);
  /** @ref zigbeeHan_t_getMapping */
  interfaceMappingDesc_t *(*getMapping) (struct zigbeeHan_t *This, azap_uint16 clId,
      azap_uint8 clDir);
  /** @ref zigbeeHan_t_publish */
  void (*publish) (struct zigbeeHan_t *This);

  /** @ref zigbeeHan_t_computeNextPermitJoinDuration */
  int (*computeNextPermitJoinDuration) (struct zigbeeHan_t *This, int newDuration);

  /** @ref zigbeeHan_t_newFree */
  void (*free) (struct zigbeeHan_t *);

  /* members */
  /** the RF channel frequency. */
  azap_uint8 rfChan;
  /** the PAN identifier. */
  azap_uint16 panId;
  /** the extended PAN identifier. */
  azap_uint8 panIdExt[IEEE_ADDR_LEN];
  /** the extended address of the coordinator. */
  azap_uint8 coordExtAddr[IEEE_ADDR_LEN];

  /** Indicates if the M2M representation for ZigBee IPU has been created. */
  bool m2mIpuCreated;
  /** Indicates if the M2M representation for ZigBee network has been created. */
  bool m2mNwkCreated;
  /** Indicates when the OpenAssociation procedure last end */
  time_t m2mOpenAssocEndTimeMono;

  /** The list of known sensors. */
  struct list_head knownNodesList;
  /** The list of mapped interfaces descritpions. */
  struct list_head configInterfacesMapping;

} zigbeeHan_t;

/* dynamic allocation */
zigbeeHan_t *new_zigbeeHan_t();
void zigbeeHan_t_newFree(zigbeeHan_t *This);

void zigbeeHan_t_dumpNodeList(zigbeeHan_t *This, pf_print printFunc, t_cli *cl);
void zigbeeHan_t_dumpNode(zigbeeHan_t *This, pf_print printFunc, t_cli *cl,
    azap_uint16 addr);
void zigbeeHan_t_syncNode(zigbeeHan_t *This, pf_print printFunc, t_cli *cl,
    azap_uint16 addr);
void zigbeeHan_t_reloadKnownNodes(zigbeeHan_t *This, char *path);
zigbeeNode_t *zigbeeHan_t_getNodeByNwkAddr(zigbeeHan_t *This, azap_uint16 addr);
zigbeeNode_t *zigbeeHan_t_getNodeByIeeeAddrStr(zigbeeHan_t *This, char *ieee);
zigbeeNode_t *zigbeeHan_t_getNodeByIeeeAddr(zigbeeHan_t *This, azap_uint8 *ieee);
void zigbeeHan_t_addNode(zigbeeHan_t *This, zigbeeNode_t *node);
void zigbeeHan_t_deleteNode(zigbeeHan_t *This, azap_uint8 *ieee);
void zigbeeHan_t_buildNodeList(zigbeeHan_t *This, void *dst);
void zigbeeHan_t_loadMappingFromXoRef(zigbeeHan_t *This, void *itfMappings);
int zigbeeHan_t_addMapping(zigbeeHan_t *This, interfaceMappingDesc_t *itfMap);
interfaceMappingDesc_t *zigbeeHan_t_getMapping(zigbeeHan_t *This, azap_uint16 clId,
    azap_uint8 clDir);
void zigbeeHan_t_publish(zigbeeHan_t *This);
int zigbeeHan_t_computeNextPermitJoinDuration(zigbeeHan_t *This, int newDuration);

#endif

