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
//** File : ./lib/postponed_request_elts.h
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


#ifndef _POSTPONED_REQUEST_ELTS__H_
#define _POSTPONED_REQUEST_ELTS__H_

/**
 * @file postponed_request_elts.h
 * @brief This object allows storing parameters for ZigBee pending requests.
 * It is used for requestContext_t.
 * @author mlouiset
 * @date 2012-08-28
 */


/**
 * @struct postponedRequestElts_t
 * The object is used for AZAP requests that could not have been sent directly.
 * This data structure aims at storing parameters for requests that have to be sent later
 * on, when ZStack running on the CC253X is able to process it again. As a consequence, it
 * contains the intersection of parameters for all API functions.
 */
typedef struct postponedRequestElts_t
{
  /* methods */
  /** @ref postponedRequestElts_t_newFree */
  void (*free) (struct postponedRequestElts_t *);

  /* members */
  bool responseExpected;
  azap_uint8 localEndp;
  bool defaultResponse;
  azap_uint8 cmdCode;
  azap_byte *cmdData;
  azap_uint8 dataLen;

} postponedRequestElts_t;

/* dynamic allocation */
postponedRequestElts_t *new_postponedRequestElts_t(bool needResponse);
void postponedRequestElts_t_newFree(postponedRequestElts_t *This);

#endif

