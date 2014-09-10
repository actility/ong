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
//** File : driver/interface_mapping_desc.h
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


#ifndef _INTERFACE_MAPPING_DESC__H_
#define _INTERFACE_MAPPING_DESC__H_


/**
 * @file interface_mapping_desc.h
 * @brief The file brings the definition of interfaceMappingDesc_t
 * @author mlouiset
 * @date 2012-09-17
 */


/*****************************************************************************/
/* Definitions */
#define FILTER_SUB_PATH_AR1 "level1"
#define SUB_PATH_AR1 "retargeting1"

#define FILTER_SUB_PATH_AR2 "level2"
#define SUB_PATH_AR2 "retargeting2"

#define FILTER_SUB_PATH_AR3 "level3"
#define SUB_PATH_AR3 "retargeting3"


#define ITF_MAPPING_MAX_CONFIG_OPE 10
#define ITF_MAPPING_MAX_CONFIG_ATTR 20
/**
 * @struct interfaceMappingDesc_t
 * The object for an interface mapping descriptor.
 * This descriptor allows defining the mapping between the dIa templates
 * variables names (e.g. ${a_configVal0}) and the ZigBee attributes.
 */
typedef struct interfaceMappingDesc_t
{
  /* methods */
  /** @ref interfaceMappingDesc_t_loadFromXoRef */
  void (*loadFromXoRef) (struct interfaceMappingDesc_t *, void *mappingObj);
  /** @ref interfaceMappingDesc_t_newFree */
  void (*free) (struct interfaceMappingDesc_t *);

  /* members */
  /** For list management */
  struct list_head chainLink;

  /** The cluster identifier. */
  azap_uint16 clusterId;
  /** The cluster direction. */
  azap_uint8 clusterDir;
  /** The number of mapped attributes. */
  azap_uint8 nbAttributes;
  /** The mapped attributes. The position in the table gives the a_configVal element,
      the value is the mapped attribute identifier. */
  azap_uint16 *attributes;
  /** The number of mapped attributes. */
  azap_uint8 nbOperations;
  /** The mapped attributes. The position in the table gives the a_configVal element,
      the value is the mapped operation name. */
  char **operations;

} interfaceMappingDesc_t;

/* dynamic allocation */
interfaceMappingDesc_t *new_interfaceMappingDesc_t();

void interfaceMappingDesc_t_loadFromXoRef(interfaceMappingDesc_t *This, void *mappingObj);
void interfaceMappingDesc_t_newFree(interfaceMappingDesc_t *This);

#endif

