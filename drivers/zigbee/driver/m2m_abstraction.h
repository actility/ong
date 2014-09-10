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
//** File : driver/m2m_abstraction.h
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


#ifndef __M2M_ABSTRACTION__H_
#define __M2M_ABSTRACTION__H_

/**
 * @file m2m_abstraction.h
 * @brief Tool functions and definitions for M2M abstraction.
 * @author mlouiset
 * @date 2012-09-25
 */

#define OK_WAITING_FOR_COMPLETION 1
#define OK_ONE_WAY_REQUEST 0

#define ERR_INVALID_NODE -10
#define ERR_INVALID_EP -11
#define ERR_UNSUPPORTED_INTERFACE -12
#define ERR_UNSUPPORTED_OPERATION -13
#define ERR_CONTENT_MISSING -14
#define ERR_INVALID_REQ_URI -15

#define ERR_INVALID_ARG_1 -51
#define ERR_INVALID_ARG_2 -52
#define ERR_INVALID_ARG_3 -53
#define ERR_INVALID_ARG_4 -54
#define ERR_INVALID_ARG_5 -55

#define ERR_FAILED_TO_SEND_REQ -100


/* ****** tool functions ******** */
int getObixBoolValue(void *obix, char *paramName, bool *value);
int getObixIntValue(void *obix, char *paramName, int *value);
int getObixStrValue(void *obix, char *paramName, char *value, int size);
char *getErrorInfoResponse(char *cmdFamily, azap_uint8 zStatus, char **statusCode,
    int *size, char **type);

/* ****** M2M abstraction ******** */
struct m2mAbstraction_t *getM2mAbstraction();

/**
 *  * @struct m2mAbstraction_t
 *   * @brief the generic definition of a data and command interpreter.
 *    */
typedef struct m2mAbstraction_t
{
  /* generic methods */
  /** @ref m2mAbstraction_t_newFree */
  void (*free) (struct m2mAbstraction_t *);
  /** @ref m2mAbstraction_t_doM2mOperation */
  int (*doM2mOperation) (struct m2mAbstraction_t *This, char *operation,
      zigbeeNode_t *node, azap_uint8 epId, azap_uint16 clId, void *obixReqContent,
      int reqId);
  void (*doInternalTimer) (struct m2mAbstraction_t *This, t_imsg *imsg);

  /* generic members */

} m2mAbstraction_t;

/* dynamic allocation */
m2mAbstraction_t *new_m2mAbstraction_t();
void m2mAbstraction_t_newFree(m2mAbstraction_t *This);

int m2mAbstraction_t_doM2mOperation(m2mAbstraction_t *This, char *operation,
    zigbeeNode_t *node, azap_uint8 epId, azap_uint16 clId, void *obixReqContent, int reqId);
void m2mAbstraction_t_doInternalTimer(m2mAbstraction_t *This, t_imsg *imsg);

#endif


