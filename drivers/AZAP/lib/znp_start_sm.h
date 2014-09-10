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
//** File : ./lib/znp_start_sm.h
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


/**
 * @file znp_start_sm.h
 * @author M. Louiset
 * @date 2012-07-17
 * @brief Define the structures and prototypes for functions designed for ZNP start management.
 */

#ifndef _ZNP_START_SM__H_
#define _ZNP_START_SM__H_

/**
 * @brief Defines the various state the instance have.
 */
typedef enum
{
  AZAP_ZNP_NULL,
  AZAP_ZNP_WAIT_INITIAL_RESET_RESP,
  AZAP_ZNP_WAIT_WRITE_PARAM_RESP,
  AZAP_ZNP_WAIT_WRITE_RESET_RESP,
  AZAP_ZNP_WAIT_READ_PARAM_RESP,
  AZAP_ZNP_WAIT_WRITE_NWK_PARAM_RESP,
  AZAP_ZNP_WAIT_REGISTER_ENDPOINTS_RESP,
  AZAP_ZNP_WAIT_REGISTER_ZDO_CALLBACKS_RESP,
  AZAP_ZNP_WAIT_STARTUP_FROM_APP_RESP,
  AZAP_ZNP_STARTING,
  AZAP_ZNP_WAIT_PAN_INFOS,
  AZAP_ZNP_WAIT_WRITE_PAN_ID,

} AZAP_ZNP_START_STATE;


/**
 * @struct znpStartInstance_t
 * @brief Defines the instance of a state machine for ZNP start procedure.
 */
typedef struct znpStartInstance_t
{
  /* methods */
  /** @ref znpStartInstance_t_newFree */
  void (*free) (struct znpStartInstance_t *);

  /** @ref znpStartInstance_t_start */
  void (*start) (struct znpStartInstance_t *, uartBufControl_t *tx);
  /** @ref znpStartInstance_t_abandon */
  void (*abandon) (struct znpStartInstance_t *, char *);
  /** @ref znpStartInstance_t_watchdog */
  void (*watchdog) (struct znpStartInstance_t *);

  /** @ref znpStartInstance_t_setState */
  void (*setState) (struct znpStartInstance_t *, AZAP_ZNP_START_STATE state);
  /** @ref znpStartInstance_t_processZnpMsg */
  void (*processZnpMsg) (struct znpStartInstance_t *, uartBufControl_t *tx,
      azap_uint8 len, azap_uint8 cmd0, azap_uint8 cmd1, azap_uint8 *data);

  /** @ref znpStartInstance_t_processWaitInitialResetResp */
  void (*processWaitInitialResetResp) (struct znpStartInstance_t *This,
      uartBufControl_t *tx, azap_uint8 len, azap_uint16 cmd, azap_uint8 *data);
  /** @ref znpStartInstance_t_processWaitWriteParamResp */
  void (*processWaitWriteParamResp) (struct znpStartInstance_t *This,
      uartBufControl_t *tx, azap_uint8 len, azap_uint16 cmd, azap_uint8 *data);
  /** @ref znpStartInstance_t_processWaitReadParamResp */
  void (*processWaitReadParamResp) (struct znpStartInstance_t *This,
      uartBufControl_t *tx, azap_uint8 len, azap_uint16 cmd, azap_uint8 *data);
  /** @ref znpStartInstance_t_processWaitWriteResetResp */
  void (*processWaitWriteResetResp) (struct znpStartInstance_t *This,
      uartBufControl_t *tx, azap_uint8 len, azap_uint16 cmd, azap_uint8 *data);
  /** @ref znpStartInstance_t_processWaitRegisterEndpointsResp */
  void (*processWaitRegisterEndpointsResp) (struct znpStartInstance_t *This,
      uartBufControl_t *tx, azap_uint8 len, azap_uint16 cmd, azap_uint8 *data);
  /** @ref znpStartInstance_t_processWaitRegisterZdoCbResp */
  void (*processWaitRegisterZdoCbResp) (struct znpStartInstance_t *This,
      uartBufControl_t *tx, azap_uint8 len, azap_uint16 cmd, azap_uint8 *data);
  /** @ref znpStartInstance_t_processWaitStartupFromAppResp */
  void (*processWaitStartupFromAppResp) (struct znpStartInstance_t *This,
      uartBufControl_t *tx, azap_uint8 len, azap_uint16 cmd, azap_uint8 *data);
  /** @ref znpStartInstance_t_processZnpStartingIndication */
  void (*processZnpStartingIndication) (struct znpStartInstance_t *This,
      uartBufControl_t *tx, azap_uint8 len, azap_uint16 cmd, azap_uint8 *data);
  /** @ref znpStartInstance_t_processWaitPanInfos */
  void (*processWaitPanInfos) (struct znpStartInstance_t *This,
      uartBufControl_t *tx, azap_uint8 len, azap_uint16 cmd, azap_uint8 *data);
  /** @ref znpStartInstance_t_processWaitWritePanId */
  void (*processWaitWritePanId) (struct znpStartInstance_t *This,
      uartBufControl_t *tx, azap_uint8 len, azap_uint16 cmd, azap_uint8 *data);


  /** The port number associated to this instance of state machine. */
  azap_uint8 portNumber;
  /** The actual instance state */
  AZAP_ZNP_START_STATE state;
  /** The last state change date */
  azap_uint8 elapsedSecondsCurrentState;
  /** Number of NV_WRITE SRSP expected for parameters setting */
  azap_uint8 writeParamRespExpected;
  /** Number of AF_REGISTER_SRSP expected for end points registration completion */
  azap_uint8 afRegisterEpRespExpected;
  /** Number of ZDO_MSG_CB_REGISTER_SRSP expected for ZDO call back registration completion */
  azap_uint8 zdoCbRegisterRespExpected;
  /** Number of SAPI_GET_DEVICE_INFO_SREQ expected */
  azap_uint8 deviceInfoRespExpected;
  /** Number of starting cycle done */
  azap_uint8 nbCycles;

} znpStartInstance_t;


/* dynamic allocation */
znpStartInstance_t *new_znpStartInstance_t(azap_uint8 port);
void znpStartInstance_t_newFree(znpStartInstance_t *This);

/* implementation methods */
void znpStartInstance_t_setState(znpStartInstance_t *, AZAP_ZNP_START_STATE state);

void znpStartInstance_t_start(znpStartInstance_t *This, uartBufControl_t *tx);
void znpStartInstance_t_abandon(znpStartInstance_t *This, char *extraDesc);
void znpStartInstance_t_watchdog(znpStartInstance_t *This);

void znpStartInstance_t_processZnpMsg(znpStartInstance_t *This, uartBufControl_t *tx,
    azap_uint8 len, azap_uint8 cmd0, azap_uint8 cmd1, azap_uint8 *data);

/* the various state processing */
// State AZAP_ZNP_WAIT_INITIAL_RESET_RESP
void znpStartInstance_t_processWaitInitialResetResp(znpStartInstance_t *This,
    uartBufControl_t *tx, azap_uint8 len, azap_uint16 cmd, azap_uint8 *data);
// State AZAP_ZNP_WAIT_WRITE_PARAM_RESP
void znpStartInstance_t_processWaitWriteParamResp(znpStartInstance_t *This,
    uartBufControl_t *tx, azap_uint8 len, azap_uint16 cmd, azap_uint8 *data);
// State AZAP_ZNP_WAIT_READ_PARAM_RESP
void znpStartInstance_t_processWaitReadParamResp(znpStartInstance_t *This,
    uartBufControl_t *tx, azap_uint8 len, azap_uint16 cmd, azap_uint8 *data);
// State AZAP_ZNP_WAIT_WRITE_RESET_RESP
void znpStartInstance_t_processWaitWriteResetResp(znpStartInstance_t *This,
    uartBufControl_t *tx, azap_uint8 len, azap_uint16 cmd, azap_uint8 *data);
// State AZAP_ZNP_WAIT_REGISTER_ENDPOINTS_RESP
void znpStartInstance_t_processWaitRegisterEndpointsResp(znpStartInstance_t *This,
    uartBufControl_t *tx, azap_uint8 len, azap_uint16 cmd, azap_uint8 *data);
// State AZAP_ZNP_WAIT_REGISTER_ZDO_CALLBACKS_RESP
void znpStartInstance_t_processWaitRegisterZdoCbResp(znpStartInstance_t *This,
    uartBufControl_t *tx, azap_uint8 len, azap_uint16 cmd, azap_uint8 *data);
// State AZAP_ZNP_WAIT_STARTUP_FROM_APP_RESP
void znpStartInstance_t_processWaitStartupFromAppResp(znpStartInstance_t *This,
    uartBufControl_t *tx, azap_uint8 len, azap_uint16 cmd, azap_uint8 *data);
// State AZAP_ZNP_STARTING
void znpStartInstance_t_processZnpStartingIndication(znpStartInstance_t *This,
    uartBufControl_t *tx, azap_uint8 len, azap_uint16 cmd, azap_uint8 *data);
// State AZAP_ZNP_WAIT_PAN_INFOS
void znpStartInstance_t_processWaitPanInfos(znpStartInstance_t *This,
    uartBufControl_t *tx, azap_uint8 len, azap_uint16 cmd, azap_uint8 *data);
// State AZAP_ZNP_WAIT_WRITE_PAN_ID
void znpStartInstance_t_processWaitWritePanId(znpStartInstance_t *This,
    uartBufControl_t *tx, azap_uint8 len, azap_uint16 cmd, azap_uint8 *data);



#endif

