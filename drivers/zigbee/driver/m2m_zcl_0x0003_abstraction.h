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
//** File : driver/m2m_zcl_0x0003_abstraction.h
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


#ifndef __M2M_ZCL_0x0003_ABSTRACTION__H_
#define __M2M_ZCL_0x0003_ABSTRACTION__H_

/**
 * @file m2m_zcl_0x0003_abstraction.h
 * @brief Gives the definition of a data and command interpreter for ZigBee Identify cluster.
 * @author mlouiset
 * @date 2012-09-25
 */


/**
 * @struct m2mZcl0x0003Abstraction_t
 * @brief the generic definition of a data and command interpreter.
 */
typedef struct m2mZcl0x0003Abstraction_t
{
  /* generic methods */
  void (*free) (struct m2mZclAbstraction_t *);
  char *(*getConsolidatedData) (struct m2mZclAbstraction_t *This,
      azap_uint16 attrId, azapZclAttrVal_t *val);
  int (*getZclAttribute) (struct m2mZclAbstraction_t *This,
      zigbeeNode_t *node, azapEndPoint_t *ep, azap_uint16 attrId, int reqId);
  int (*setZclAttribute) (struct m2mZclAbstraction_t *This,
      zigbeeNode_t *node, azapEndPoint_t *ep, azap_uint16 attrId, void *obix,
      int reqId);
  /** @ref m2mZcl0x0003Abstraction_t_doZclOperation */
  int (*doZclOperation) (struct m2mZclAbstraction_t *This, azap_uint8 operation,
      zigbeeNode_t *node, azapEndPoint_t *ep, void *obixReqContent, int reqId);
  /** @ref m2mZcl0x0003Abstraction_t_doZclOperationResp */
  void (*doZclOperationResp) (struct m2mZclAbstraction_t *This, azap_uint8 operation,
      azap_byte *cmdData, azap_uint8 cmdLen, int reqId);
  void (*collectConsolidationElts) (struct m2mZclAbstraction_t *This,
      azapEndPoint_t *ep);
  void (*configureReporting) (struct m2mZclAbstraction_t *This,
      azapZclCfgReportCommand_t *cfgReportCmd);
  bool (*canReport) (struct m2mZclAbstraction_t *This, azap_uint16 attrId);

  /* generic members */
  /** For list management */
  struct list_head chainLink;

  /** The end point identifier */
  azap_uint8 endPointId;
  /** The cluster identifier */
  azap_uint16 clusterId;

  /* =================================================== */
  /* add all specific member here */
  /* generic methods */


  /* generic members */


} m2mZcl0x0003Abstraction_t;

/* dynamic allocation */
m2mZcl0x0003Abstraction_t *new_m2mZcl0x0003Abstraction_t(azap_uint8 epId);

int m2mZcl0x0003Abstraction_t_doZclOperation(m2mZclAbstraction_t *This,
    azap_uint8 operation, zigbeeNode_t *node, azapEndPoint_t *ep, void *obixReqContent,
    int reqId);
void m2mZcl0x0003Abstraction_t_doZclOperationResp(m2mZclAbstraction_t *This,
    azap_uint8 operation, azap_byte *cmdData, azap_uint8 cmdLen, int reqId);

#endif


