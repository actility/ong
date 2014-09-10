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
//** File : ./lib/znp_start_sm.c
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

#include <rtlbase.h>
#include <stddef.h>
#include <rtllist.h>

#include "nvram.h"
#include "azap_api.h"

#include "uart_buf_control.h"
#include "azap_zcl_attr_val.h"
#include "azap_attribute.h"
#include "azap_endpoint.h"
#include "znp_start_sm.h"
#include "postponed_request_elts.h"
#include "request_context.h"
#include "request_manager.h"
#include "port_context.h"
#include "format_znp.h"
#include "format_znp_sys.h"
#include "format_znp_af.h"
#include "format_znp_zdo.h"
#include "format_znp_sapi.h"
#include "azap_tools.h"

// external variables locally used
extern portContext_t *g_portContexts[];

/*****************************************************************************/
/* Prototypes for local functions */
static void znpStartInstance_t_init(znpStartInstance_t *This);
static bool registerEndPoints(znpStartInstance_t *This, portContext_t *portCntxt);
static void writeParamInNvRam(znpStartInstance_t *This, portContext_t *portCntxt);
#ifndef __NO_TRACE__
static char *printState(AZAP_ZNP_START_STATE state);
static char *printStartupStatus(azap_uint8 status);
#endif

/* And local variables */
/** The ZDO call backs to register for Coordinator device type */
const azap_uint16 c_zdoMsgCbRegisterListCoord[] =
{
  Device_annce,
  NWK_addr_rsp,
  IEEE_addr_rsp,
  Node_Desc_rsp,
  Power_Desc_rsp,
  Simple_Desc_rsp,
  Active_EP_rsp,
  Bind_rsp,
  Unbind_rsp,
  Mgmt_Rtg_rsp,
  Mgmt_Bind_rsp,
  Mgmt_Permit_Join_rsp,
  Mgmt_Leave_rsp,
};

/*****************************************************************************/
/* Constructors / destructor */

/* static allocation */

/**
 * @brief Initialize the znpStartInstance_t struct
 * @param This the object instance
 */
static void znpStartInstance_t_init(znpStartInstance_t *This)
{
  RTL_TRDBG(TRACE_DETAIL, "znpStartInstance_t::init\n");

  /* Assign the pointers on functions */
  This->setState = znpStartInstance_t_setState;
  This->start = znpStartInstance_t_start;
  This->abandon = znpStartInstance_t_abandon;
  This->processZnpMsg = znpStartInstance_t_processZnpMsg;
  This->processWaitInitialResetResp = znpStartInstance_t_processWaitInitialResetResp;
  This->processWaitWriteParamResp = znpStartInstance_t_processWaitWriteParamResp;
  This->processWaitReadParamResp = znpStartInstance_t_processWaitReadParamResp;
  This->processWaitWriteResetResp = znpStartInstance_t_processWaitWriteResetResp;
  This->processWaitRegisterEndpointsResp = znpStartInstance_t_processWaitRegisterEndpointsResp;
  This->processWaitRegisterZdoCbResp = znpStartInstance_t_processWaitRegisterZdoCbResp;
  This->processWaitStartupFromAppResp = znpStartInstance_t_processWaitStartupFromAppResp;
  This->processZnpStartingIndication = znpStartInstance_t_processZnpStartingIndication;
  This->processWaitPanInfos = znpStartInstance_t_processWaitPanInfos;
  This->processWaitWritePanId = znpStartInstance_t_processWaitWritePanId;
  This->watchdog = znpStartInstance_t_watchdog;

  /* Initialize the members */
  This->state = AZAP_ZNP_NULL;
  This->elapsedSecondsCurrentState = 0;
  This->portNumber = 255; // 0 is a valid port
  This->writeParamRespExpected = 0;
  This->afRegisterEpRespExpected = 0;
  This->zdoCbRegisterRespExpected = 0;
  This->deviceInfoRespExpected = 0;
  This->nbCycles = 0;
}

/* dynamic allocation */
/**
 * @brief do dynamic allocation of a znpStartInstance_t.
 * @param port the port number associated to this instance of state machine.
 * @return the copy of the object
 */
znpStartInstance_t *new_znpStartInstance_t(azap_uint8 port)
{
  znpStartInstance_t *This = malloc(sizeof(znpStartInstance_t));
  if (!This)
  {
    return NULL;
  }
  znpStartInstance_t_init(This);
  This->free = znpStartInstance_t_newFree;
  This->portNumber = port;
  RTL_TRDBG(TRACE_DETAIL, "new znpStartInstance_t (This:0x%.8x) (port:%d)\n", This, port);
  return This;
}

/**
 * @brief Destructor for dynamic allocation
 * @anchor znpStartInstance_t_newFree
 * @param This the object instance
 */
void znpStartInstance_t_newFree(znpStartInstance_t *This)
{
  RTL_TRDBG(TRACE_DETAIL, "znpStartInstance_t::newFree (This:0x%.8x) (port:%d)\n", This,
      This->portNumber);
  free(This);
}

/* implementation methods */
/**
 * @brief Change current state to the one provided.
 * @anchor znpStartInstance_t_setState
 * @param This the object instance
 * @param state the new state to set.
 */
void znpStartInstance_t_setState(znpStartInstance_t *This, AZAP_ZNP_START_STATE state)
{
  RTL_TRDBG(TRACE_IMPL, "znpStartInstance_t::setState (port:%d) (former state:%s) "
      "(new state: %s)\n", This->portNumber, printState(This->state), printState(state));
  This->state = state;
  This->elapsedSecondsCurrentState = 0;
}

/**
 * @brief Start the "ZNP start procedure", i.e. send ZNP reset message.
 * @anchor znpStartInstance_t_start
 * @param This the object instance
 * @param tx the buffer for the transmitted ZNP packets
 */
void znpStartInstance_t_start(znpStartInstance_t *This, uartBufControl_t *tx)
{
  azap_uint8 val[1];
  // reset the local counters
  This->nbCycles = 0;
  // and immediately reset the firmware
  This->setState(This, AZAP_ZNP_WAIT_INITIAL_RESET_RESP);
  val[0] = 0x01; // serial bootloader reset
  znpSend(tx, MT_SYS_SOFT_RESET_AREQ, 1, val);
}

/**
 * @brief ask for the "ZNP start procedure" abandon.
 * @anchor znpStartInstance_t_abandon
 * @param This the object instance
 * @param extraDesc gives information element on reason for abandon.
 */
void znpStartInstance_t_abandon(znpStartInstance_t *This, char *extraDesc)
{
  RTL_TRDBG(TRACE_INFO, "znpStartInstance_t::abandon - %s (port:%d)\n",
      extraDesc, This->portNumber);
  g_portContexts[This->portNumber]->doStartZnpCompletion(
    g_portContexts[This->portNumber], false);
}

/**
 * @brief Look for "ZNP start procedure" state.
 * It watches every seconds since how much time the "ZNP start procedure" is in
 * its current state, and restarts the whole procedure if it detects that nothing
 * changed for more than 20 seconds.
 * @anchor znpStartInstance_t_startWatchdog
 * @param This the object instance
 */
void znpStartInstance_t_watchdog(znpStartInstance_t *This)
{
  This->elapsedSecondsCurrentState ++;
  RTL_TRDBG(TRACE_IMPL, "znpStartInstance_t::watchdog  (elapsedSecondsCurrentState:%d)\n",
      This->elapsedSecondsCurrentState);
  if (This->elapsedSecondsCurrentState >= 100)
  {
    RTL_TRDBG(TRACE_ERROR, "znpStartInstance_t::watchdog - failed to complete, restarting "
        "initialization phase (port:%d) (state:%s)\n", This->portNumber, printState(This->state));
    This->start(This, g_portContexts[This->portNumber]->tx);
  }
}

/**
 * @brief Process the received ZNP message and go forward in ZNP start procedure.
 * @anchor znpStartInstance_t_processZnpMsg
 * @param This the object instance
 * @param tx the buffer for the transmitted ZNP packets
 * @param len the length of the received payload
 * @param cmd0 the byte CMD0 of the received message
 * @param cmd1 the byte CMD1 of the received message
 * @param data the received payload
 */
void znpStartInstance_t_processZnpMsg(znpStartInstance_t *This, uartBufControl_t *tx,
    azap_uint8 len, azap_uint8 cmd0, azap_uint8 cmd1, azap_uint8 *data)
{
  azap_uint16 cmd = BUILD_UINT16(cmd0, cmd1);

  RTL_TRDBG(TRACE_DETAIL, "znpStartInstance_t::processZnpMsg (port:%d) (cmd0:0x%.2x) "
      "(cmd1:0x%.2x)\n", This->portNumber, cmd0, cmd1);

  switch (This->state)
  {
    case AZAP_ZNP_WAIT_INITIAL_RESET_RESP:
      This->processWaitInitialResetResp(This, tx, len, cmd, data);
      break;

    case AZAP_ZNP_WAIT_WRITE_PARAM_RESP:
      This->processWaitWriteParamResp(This, tx, len, cmd, data);
      break;

    case AZAP_ZNP_WAIT_READ_PARAM_RESP:
      This->processWaitReadParamResp(This, tx, len, cmd, data);
      break;

    case AZAP_ZNP_WAIT_WRITE_RESET_RESP:
      This->processWaitWriteResetResp(This, tx, len, cmd, data);
      break;

    case AZAP_ZNP_WAIT_REGISTER_ENDPOINTS_RESP:
      This->processWaitRegisterEndpointsResp(This, tx, len, cmd, data);
      break;

    case AZAP_ZNP_WAIT_REGISTER_ZDO_CALLBACKS_RESP:
      This->processWaitRegisterZdoCbResp(This, tx, len, cmd, data);
      break;

    case AZAP_ZNP_WAIT_STARTUP_FROM_APP_RESP:
      This->processWaitStartupFromAppResp(This, tx, len, cmd, data);
      break;

    case AZAP_ZNP_STARTING:
      This->processZnpStartingIndication(This, tx, len, cmd, data);
      break;

    case AZAP_ZNP_WAIT_PAN_INFOS:
      This->processWaitPanInfos(This, tx, len, cmd, data);
      break;

    case AZAP_ZNP_WAIT_WRITE_PAN_ID:
      This->processWaitWritePanId(This, tx, len, cmd, data);
      break;
    default:
      RTL_TRDBG(TRACE_ERROR, "znpStartInstance_t::processZnpMsg (port:%d) - unexpected "
          "event (%s)\n", This->portNumber, printState(This->state));
      break;
  }

}

/**
 * @brief Process the received ZNP message and in state AZAP_ZNP_WAIT_INITIAL_RESET_RESP.
 * @anchor znpStartInstance_t_processWaitInitialResetResp
 * @todo check RESET response status
 * @param This the object instance
 * @param tx the buffer for the transmitted ZNP packets
 * @param len the length of the received payload
 * @param cmd the received ZNP command.
 * @param data the received payload.
 */
void znpStartInstance_t_processWaitInitialResetResp(znpStartInstance_t *This,
    uartBufControl_t *tx, azap_uint8 len, azap_uint16 cmd, azap_uint8 *data)
{
  RTL_TRDBG(TRACE_DETAIL, "znpStartInstance_t::processWaitInitialResetResp (port:%d) "
      "(cmd:0x%.4x)\n", This->portNumber, cmd);

  if (MT_SYS_RESET_IND == cmd)
  {
    // retrieve the ZStack version
    if (g_portContexts[This->portNumber])
    {
      parseSysResetIndVersion(data, len, g_portContexts[This->portNumber]->zstackVersion);
    }

    // switch to next state and send next ZNP message
    This->setState(This, AZAP_ZNP_WAIT_WRITE_PARAM_RESP);
    writeParamInNvRam(This, g_portContexts[This->portNumber]);
  }
  else
  {
    RTL_TRDBG(TRACE_IMPL, "znpStartInstance_t::processWaitInitialResetResp - received cmd "
        "ignored (port:%d) (cmd:0x%.4x)\n", This->portNumber, cmd);
  }
}


/**
 * @brief Process the received ZNP message and in state AZAP_ZNP_WAIT_WRITE_PARAM_RESP.
 * @anchor znpStartInstance_t_processWaitWriteParamResp
 * @todo check NV_WRITE response status
 * @param This the object instance
 * @param tx the buffer for the transmitted ZNP packets
 * @param len the length of the received payload
 * @param cmd the received ZNP command.
 * @param data the received payload.
 */
void znpStartInstance_t_processWaitWriteParamResp(znpStartInstance_t *This,
    uartBufControl_t *tx, azap_uint8 len, azap_uint16 cmd, azap_uint8 *data)
{
  azap_uint8 val[1];

  RTL_TRDBG(TRACE_DETAIL, "znpStartInstance_t::processWaitWriteParamResp (port:%d) "
      "(cmd:0x%.4x)\n", This->portNumber, cmd);

  if (MT_SYS_RESET_IND == cmd)
  {
    // another extra MT_SYS_RESET_IND message here, this means the first write attempt
    // occured during last stack re-init, and then was probably ignored.
    // so re-emit the NV_WRITE of ZCD_NV_STARTUP_OPTION:
    This->processWaitInitialResetResp(This, tx, len, cmd, data);
  }
  else if (MT_SYS_NV_WRITE_SRSP == cmd)
  {
    if (!data || (STATUS_CODE_SUCCESS != data[0]))
    {
      char szDesc[64];
      sprintf(szDesc, "NV_WRITE request failed (status:%d)",
          data ? data[0] : -1);
      This->abandon(This, szDesc);
    }
    else
    {
      This->writeParamRespExpected --;

      if (0 == This->writeParamRespExpected)
      {
        // continue start-up procedure
        if (0 == This->nbCycles)
        {
          This->setState(This, AZAP_ZNP_WAIT_WRITE_RESET_RESP);
          val[0] = 0x01; // serial bootloader reset
          znpSend(tx, MT_SYS_SOFT_RESET_AREQ, 1, val);
        }
        else
        {
          This->setState(This, AZAP_ZNP_WAIT_READ_PARAM_RESP);
          znpSendSysNvRead(g_portContexts[This->portNumber]->tx, ZCD_NV_STARTUP_OPTION, 0);
        }
      }
      else
      {
        RTL_TRDBG(TRACE_IMPL, "znpStartInstance_t::processWaitWriteParamResp - still %d "
            "write acknowledgement(s) (port:%d) (cmd:0x%.4x)\n", This->writeParamRespExpected,
            This->portNumber, cmd);
      }
    }
  }
  else
  {
    RTL_TRDBG(TRACE_IMPL, "znpStartInstance_t::processWaitWriteParamResp - received cmd "
        "ignored (port:%d) (cmd:0x%.4x)\n", This->portNumber, cmd);
  }
}

/**
 * @brief Process the received ZNP message and in state AZAP_ZNP_WAIT_READ_PARAM_RESP.
 * @anchor znpStartInstance_t_processWaitReadParamResp
 * @todo check NV_WRITE response status
 * @param This the object instance
 * @param tx the buffer for the transmitted ZNP packets
 * @param len the length of the received payload
 * @param cmd the received ZNP command.
 * @param data the received payload.
 */
void znpStartInstance_t_processWaitReadParamResp(znpStartInstance_t *This,
    uartBufControl_t *tx, azap_uint8 len, azap_uint16 cmd, azap_uint8 *data)
{
  azap_uint8 expectedPanStartUpOpt;

  RTL_TRDBG(TRACE_DETAIL, "znpStartInstance_t::processWaitReadParamResp (port:%d) "
      "(cmd:0x%.4x)\n", This->portNumber, cmd);
  if (MT_SYS_NV_READ_SRSP == cmd)
  {
    if (g_portContexts[This->portNumber]->resetHanAtReboot)
    {
      expectedPanStartUpOpt = ZCD_STARTOPT_DEFAULT_CONFIG_STATE |
          ZCD_STARTOPT_DEFAULT_NETWORK_STATE;
    }
    else
    {
      expectedPanStartUpOpt = ZCD_STARTOPT_AUTO_START;
    }

    if ((3 == len) &&
        (STATUS_CODE_SUCCESS == data[0]) &&
        (expectedPanStartUpOpt == data[2]))
    {
      // Send a AF REGISTER request for each end point
      This->setState(This, AZAP_ZNP_WAIT_REGISTER_ENDPOINTS_RESP);
      if (! registerEndPoints(This, g_portContexts[This->portNumber]))
      {
        // exit the automate
        This->abandon(This, "End points registration issue");
      }
    }
    else
    {
      RTL_TRDBG(TRACE_ERROR, "znpStartInstance_t::processWaitReadParamResp - error: "
          "wrong start-up option 0x%.2x in NV RAM (expected one:0x%.2x)\n", data[2],
          expectedPanStartUpOpt);
      // exit the automate
      This->abandon(This, "Write issue (wrong start-up option)");
    }
  }
  else
  {
    RTL_TRDBG(TRACE_IMPL, "znpStartInstance_t::processWaitReadParamResp - received cmd "
        "ignored (port:%d) (cmd:0x%.4x)\n", This->portNumber, cmd);
  }
}


/**
 * @brief Process the received ZNP message and in state AZAP_ZNP_WAIT_WRITE_RESET_RESP.
 * @anchor znpStartInstance_t_processWaitWriteResetResp
 * @todo check RESET response status
 * @param This the object instance
 * @param tx the buffer for the transmitted ZNP packets
 * @param len the length of the received payload
 * @param cmd the received ZNP command.
 * @param data the received payload.
 */
void znpStartInstance_t_processWaitWriteResetResp(znpStartInstance_t *This,
    uartBufControl_t *tx, azap_uint8 len, azap_uint16 cmd, azap_uint8 *data)
{
  (void)len;
  RTL_TRDBG(TRACE_DETAIL, "znpStartInstance_t::processWaitWriteResetResp (port:%d) "
      "(cmd:0x%.4x)\n", This->portNumber, cmd);

  if ((MT_SYS_RESET_IND == cmd) && (g_portContexts[This->portNumber]))
  {
    // Send a AF REGISTER request for each end point
    This->setState(This, AZAP_ZNP_WAIT_REGISTER_ENDPOINTS_RESP);
    if (! registerEndPoints(This, g_portContexts[This->portNumber]))
    {
      // exit the automate
      This->abandon(This, "End points registration issue");
    }
  }
  else
  {
    RTL_TRDBG(TRACE_IMPL, "znpStartInstance_t::processWaitWriteResetResp - received cmd "
        "ignored (port:%d) (cmd:0x%.4x)\n", This->portNumber, cmd);
  }
}

/**
 * @brief Process the received ZNP message and in state AZAP_ZNP_WAIT_REGISTER_ENDPOINTS_RESP.
 * @anchor znpStartInstance_t_processWaitRegisterEndpointsResp
 * @todo register ZDO call backs depending on the device type (for now on only coordinator
 * case is settled).
 * @param This the object instance
 * @param tx the buffer for the transmitted ZNP packets
 * @param len the length of the received payload
 * @param cmd the received ZNP command.
 * @param data the received payload.
 */
void znpStartInstance_t_processWaitRegisterEndpointsResp(znpStartInstance_t *This,
    uartBufControl_t *tx, azap_uint8 len, azap_uint16 cmd, azap_uint8 *data)
{
  azap_uint8 i;
  (void)len;
  RTL_TRDBG(TRACE_DETAIL, "znpStartInstance_t::processWaitRegisterEndpointsResp (port:%d) "
      "(cmd:0x%.4x)\n", This->portNumber, cmd);

  if (MT_AF_REGISTER_SRSP == cmd)
  {
    if (!data || ((STATUS_CODE_SUCCESS != data[0]) && (0xb8 != data[0])))
    {
      char szDesc[64];
      sprintf(szDesc, "Failed to register end point (status:%d)",
          data ? data[0] : -1);
      This->abandon(This, szDesc);
    }
    else
    {
      This->afRegisterEpRespExpected --;

      if (0 == This->afRegisterEpRespExpected)
      {
        // next step is to register ZDO msg call backs
        This->setState(This, AZAP_ZNP_WAIT_REGISTER_ZDO_CALLBACKS_RESP);

        This->zdoCbRegisterRespExpected = sizeof(c_zdoMsgCbRegisterListCoord)
            / sizeof(azap_uint16);
        for (i = 0; i < This->zdoCbRegisterRespExpected; i++)
        {
          znpSendZdoMsgCbRegister(tx, c_zdoMsgCbRegisterListCoord[i]);
        }
      }
    }
  }
  else
  {
    RTL_TRDBG(TRACE_IMPL, "znpStartInstance_t::processWaitRegisterEndpointsResp - received "
        "cmd ignored (port:%d) (cmd:0x%.4x)\n", This->portNumber, cmd);
  }
}

/**
 * @brief Process the received ZNP message and in state AZAP_ZNP_WAIT_REGISTER_ZDO_CALLBACKS_RESP.
 * @anchor znpStartInstance_t_processWaitRegisterEndpointsResp
 * @param This the object instance
 * @param tx the buffer for the transmitted ZNP packets
 * @param len the length of the received payload
 * @param cmd the received ZNP command.
 * @param data the received payload.
 */
void znpStartInstance_t_processWaitRegisterZdoCbResp(znpStartInstance_t *This,
    uartBufControl_t *tx, azap_uint8 len, azap_uint16 cmd, azap_uint8 *data)
{
  (void)len;
  RTL_TRDBG(TRACE_DETAIL, "znpStartInstance_t::processWaitRegisterZdoCbResp (port:%d) "
      "(cmd:0x%.4x)\n", This->portNumber, cmd);

  if (MT_ZDO_MSG_CB_REGISTER_SRSP == cmd)
  {
    if (!data || ((STATUS_CODE_SUCCESS != data[0]) && (0xb8 != data[0])))
    {
      char szDesc[64];
      sprintf(szDesc, "Failed to register ZDO call back (status:%d)",
          data ? data[0] : -1);
      This->abandon(This, szDesc);
    }
    else
    {
      This->zdoCbRegisterRespExpected --;

      if (0 == This->zdoCbRegisterRespExpected)
      {
        // all call back registered, start application
        This->setState(This, AZAP_ZNP_WAIT_STARTUP_FROM_APP_RESP);
        znpSend(tx, MT_ZDO_STARTUP_FROM_APP_SREQ, 0, NULL);
      }
    }
  }
  else
  {
    RTL_TRDBG(TRACE_IMPL, "znpStartInstance_t::processWaitRegisterZdoCbResp - received "
        "cmd ignored (port:%d) (cmd:0x%.4x)\n", This->portNumber, cmd);
  }
}

/**
 * @brief Process the received ZNP message and in state AZAP_ZNP_WAIT_STARUP_FROM_APP_RESP.
 * @anchor znpStartInstance_t_processWaitStartupFromAppResp
 * @param This the object instance
 * @param tx the buffer for the transmitted ZNP packets
 * @param len the length of the received payload
 * @param cmd the received ZNP command.
 * @param data the received payload.
 */
void znpStartInstance_t_processWaitStartupFromAppResp(znpStartInstance_t *This,
    uartBufControl_t *tx, azap_uint8 len, azap_uint16 cmd, azap_uint8 *data)
{
  (void)len;
  RTL_TRDBG(TRACE_DETAIL, "znpStartInstance_t::processWaitStartupFromAppResp (port:%d) "
      "(cmd:0x%.4x)\n", This->portNumber, cmd);

  if (MT_ZDO_STARTUP_FROM_APP_SRSP == cmd)
  {
    // According to "CC2530ZNP Interface Specification.pdf" the SRSP payload only embeds
    // a status code that may indicate the following:
    // 0x00 - Restored network state
    // 0x01 - New network state
    // 0x02 - Leave and not Started
    if ((data) && (0x02 != data[0]))
    {
      if ( 0 == This->nbCycles )
      {
        // We need to do two entire cycles of this state machine to start successfully.
        This->nbCycles++;
        This->setState(This, AZAP_ZNP_WAIT_WRITE_PARAM_RESP);
        writeParamInNvRam(This, g_portContexts[This->portNumber]);
      }
      else
      {
        // ZStack is starting, now waiting for start completion
        This->setState(This, AZAP_ZNP_STARTING);
      }
    }
    else
    {
      char szDesc[64];
      sprintf(szDesc, "START_FROM_APP failed with status %d", data ? data[0] : -1);
      This->abandon(This, szDesc);
    }
  }
  else
  {
    RTL_TRDBG(TRACE_IMPL, "znpStartInstance_t::processWaitStartupFromAppResp - received "
        "cmd ignored (port:%d) (cmd:0x%.4x)\n", This->portNumber, cmd);
  }
}

/**
 * @brief Process the received ZNP message and in state AZAP_ZNP_STARTING.
 * @anchor znpStartInstance_t_processZnpStartingIndication
 * @param This the object instance
 * @param tx the buffer for the transmitted ZNP packets
 * @param len the length of the received payload
 * @param cmd the received ZNP command.
 * @param data the received payload.
 */
void znpStartInstance_t_processZnpStartingIndication(znpStartInstance_t *This,
    uartBufControl_t *tx, azap_uint8 len, azap_uint16 cmd, azap_uint8 *data)
{
  (void)len;
  RTL_TRDBG(TRACE_DETAIL, "znpStartInstance_t::processZnpStartingIndication (port:%d) "
      "(cmd:0x%.4x)\n", This->portNumber, cmd);

  if ((MT_ZDO_STATE_CHG_IND_AREQ == cmd) && (data))
  {

    RTL_TRDBG(TRACE_INFO, "znpStartInstance_t::processZnpStartingIndication state changed "
        "(port:%d) (new state:%s)\n", This->portNumber, printStartupStatus(data[0]));

    if ( ZDO_DEV_ZB_COORD == data[0])
    {
      // ZStack is started successfully, now retrieve PAN information
      This->setState(This, AZAP_ZNP_WAIT_PAN_INFOS);
      This->deviceInfoRespExpected = 4;
      azap_uint8 val[1];
      val[0] = DEV_INFO_SHORT_ADDR;
      znpSend(tx, MT_SAPI_GET_DEVICE_INFO_SREQ, 1, val);
      val[0] = DEV_INFO_IEEE_ADDR;
      znpSend(tx, MT_SAPI_GET_DEVICE_INFO_SREQ, 1, val);
      val[0] = DEV_INFO_PAN_ID;
      znpSend(tx, MT_SAPI_GET_DEVICE_INFO_SREQ, 1, val);
      val[0] = DEV_INFO_EXT_PAN_ID;
      znpSend(tx, MT_SAPI_GET_DEVICE_INFO_SREQ, 1, val);

    }
  }
  else
  {
    RTL_TRDBG(TRACE_IMPL, "znpStartInstance_t::processZnpStartingIndication - received "
        "cmd ignored (port:%d) (cmd:0x%.4x)\n", This->portNumber, cmd);
  }
}

/**
 * @brief Process the received ZNP message and in state AZAP_ZNP_WAIT_PAN_INFOS.
 * @anchor znpStartInstance_t_processWaitPanInfos
 * @param This the object instance
 * @param tx the buffer for the transmitted ZNP packets
 * @param len the length of the received payload
 * @param cmd the received ZNP command.
 * @param data the received payload.
 */
void znpStartInstance_t_processWaitPanInfos(znpStartInstance_t *This,
    uartBufControl_t *tx, azap_uint8 len, azap_uint16 cmd, azap_uint8 *data)
{
  (void)len;
  (void)tx;
  azap_uint8 val[2];
  RTL_TRDBG(TRACE_DETAIL, "znpStartInstance_t::processWaitPanInfos (port:%d) "
      "(cmd:0x%.4x)\n", This->portNumber, cmd);

  if (MT_SAPI_GET_DEVICE_INFO_SRSP == cmd)
  {
    switch (data[0])
    {
      case DEV_INFO_SHORT_ADDR:
        This->deviceInfoRespExpected --;
        g_portContexts[This->portNumber]->shortAddr = BUILD_UINT16(data[1], data[2]);
        break;
      case DEV_INFO_IEEE_ADDR:
        This->deviceInfoRespExpected --;
        memcpy(g_portContexts[This->portNumber]->deviceIeee, &data[1], IEEE_ADDR_LEN);
        break;
      case DEV_INFO_PAN_ID:
        This->deviceInfoRespExpected --;
        g_portContexts[This->portNumber]->panId = BUILD_UINT16(data[1], data[2]);
        break;
      case DEV_INFO_EXT_PAN_ID:
        This->deviceInfoRespExpected --;
        memcpy(g_portContexts[This->portNumber]->panIdExt, &data[1], IEEE_ADDR_LEN);
        break;
      default:
        break;
    }

    if (0 == This->deviceInfoRespExpected)
    {
      This->setState(This, AZAP_ZNP_WAIT_WRITE_PAN_ID);
      // Write the PAN ID in NV RAM
      val[0] = LO_UINT16(g_portContexts[This->portNumber]->panId);
      val[1] = HI_UINT16(g_portContexts[This->portNumber]->panId);
      znpSendSysNvWrite(tx, ZCD_NV_PANID, 0, 2, val);
    }
  }
  else
  {
    RTL_TRDBG(TRACE_IMPL, "znpStartInstance_t::processWaitPanInfos - received "
        "cmd ignored (port:%d) (cmd:0x%.4x)\n", This->portNumber, cmd);
  }
}

/**
 * @brief Process the received ZNP message and in state AZAP_ZNP_WAIT_WRITE_PAN_ID.
 * @anchor znpStartInstance_t_processWaitWritePanId
 * @todo check NV_WRITE response status
 * @param This the object instance
 * @param tx the buffer for the transmitted ZNP packets
 * @param len the length of the received payload
 * @param cmd the received ZNP command.
 * @param data the received payload.
 */
void znpStartInstance_t_processWaitWritePanId(znpStartInstance_t *This,
    uartBufControl_t *tx, azap_uint8 len, azap_uint16 cmd, azap_uint8 *data)
{
  RTL_TRDBG(TRACE_DETAIL, "znpStartInstance_t::processWaitWritePanId (port:%d) "
      "(cmd:0x%.4x)\n", This->portNumber, cmd);

  if (MT_SYS_NV_WRITE_SRSP == cmd)
  {
    g_portContexts[This->portNumber]->doStartZnpCompletion(
      g_portContexts[This->portNumber], true);
  }
  else
  {
    RTL_TRDBG(TRACE_IMPL, "znpStartInstance_t::processWaitWritePanId - received cmd "
        "ignored (port:%d) (cmd:0x%.4x)\n", This->portNumber, cmd);
  }
}

/*****************************************************************************/
/*   _                 _   _            _    __              _   _
 *  | |   ___  __ __ _| | | |_ ___  ___| |  / _|_  _ _ _  __| |_(_)___ _ _  ___
 *  | |__/ _ \/ _/ _` | | |  _/ _ \/ _ \ | |  _| || | ' \/ _|  _| / _ \ ' \(_-<
 *  |____\___/\__\__,_|_|  \__\___/\___/_| |_|  \_,_|_||_\__|\__|_\___/_||_/__/
 */

/**
 * @brief Send AF REGISTER requests via ZNP for end points registration.
 * @param This the znpStartInstance_t instance that requested for this operation.
 * @param portCntxt The ZNP interface context to sollicitate.
 * @return 1 if there are end points to register and all AF REGISTER requests can be
 * sent successfully, 0 otherwise.
 */
static bool registerEndPoints(znpStartInstance_t *This, portContext_t *portCntxt)
{
  struct list_head *link;
  azapEndPoint_t *ep;
  bool res = false;
  This->afRegisterEpRespExpected = 0;

  if (! list_empty(&portCntxt->endPointList) )
  {
    res = true;
    list_for_each (link, &portCntxt->endPointList)
    {
      ep = list_entry(link, azapEndPoint_t, chainLink);
      This->afRegisterEpRespExpected++;
      RTL_TRDBG(TRACE_INFO, "registerEndPoints - sending AF REGISTER request for end "
          "point %d (port:%d) (profile:0x%.4x)\n", ep->id, portCntxt->portNumber,
          ep->appProfId);
      if (! znpSendAfRegister(portCntxt->tx, ep))
      {
        RTL_TRDBG(TRACE_ERROR, "registerEndPoints - error: failed to send AF REGISTER "
            "(port:%d)\n", portCntxt->portNumber);
        res = false;
      }
    }
  }
  else
  {
    RTL_TRDBG(TRACE_ERROR, "registerEndPoints - error: no end points !! (port:%d)\n",
        portCntxt->portNumber);
  }
  return res;
}

/**
 * @brief Write parameters in NV RAM to save them, and be able to resume later on.
 * @param This the znpStartInstance_t instance that requested for this operation.
 * @param portCntxt The ZNP interface context to sollicitate.
 */
static void writeParamInNvRam(znpStartInstance_t *This, portContext_t *portCntxt)
{
  azap_uint8 val[4];

  if (!portCntxt)
  {
    return;
  }

  This->writeParamRespExpected = 0;

  // PAN startup option
  if (portCntxt->resetHanAtReboot)
  {
    val[0] = ZCD_STARTOPT_DEFAULT_CONFIG_STATE | ZCD_STARTOPT_DEFAULT_NETWORK_STATE;
  }
  else
  {
    val[0] = ZCD_STARTOPT_AUTO_START;
  }
  znpSendSysNvWrite(portCntxt->tx, ZCD_NV_STARTUP_OPTION, 0, 1, val);
  This->writeParamRespExpected++;

  // device type
  val[0] = portCntxt->deviceType;
  znpSendSysNvWrite(portCntxt->tx, ZCD_NV_LOGICAL_TYPE, 0, 1, val);
  This->writeParamRespExpected++;

  if (portCntxt->resetHanAtReboot)
  {
    // PAN ID
    val[0] = 0xFF;
    val[1] = 0xFF;
    znpSendSysNvWrite(portCntxt->tx, ZCD_NV_PANID, 0, 2, val);
    This->writeParamRespExpected++;
  }

  // RF channel
  val[0] = BREAK_UINT32(portCntxt->defaultChannel, 0);
  val[1] = BREAK_UINT32(portCntxt->defaultChannel, 1);
  val[2] = BREAK_UINT32(portCntxt->defaultChannel, 2);
  val[3] = BREAK_UINT32(portCntxt->defaultChannel, 3);
  znpSendSysNvWrite(portCntxt->tx, ZCD_NV_CHANLIST, 0, 4, val);
  This->writeParamRespExpected++;
}

#ifndef __NO_TRACE__
/**
 * @brief Build a string representation of a state.
 * @param state the state to be represented.
 * @return the resulting string representation.
 */
static char *printState(AZAP_ZNP_START_STATE state)
{
  switch (state)
  {
    case AZAP_ZNP_NULL:
      return "AZAP_ZNP_NULL";
    case AZAP_ZNP_WAIT_INITIAL_RESET_RESP:
      return "AZAP_ZNP_WAIT_INITIAL_RESET_RESP";
    case AZAP_ZNP_WAIT_WRITE_PARAM_RESP:
      return "AZAP_ZNP_WAIT_WRITE_PARAM_RESP";
    case AZAP_ZNP_WAIT_READ_PARAM_RESP:
      return "AZAP_ZNP_WAIT_READ_PARAM_RESP";
    case AZAP_ZNP_WAIT_WRITE_RESET_RESP:
      return "AZAP_ZNP_WAIT_WRITE_RESET_RESP";
    case AZAP_ZNP_WAIT_WRITE_NWK_PARAM_RESP:
      return "AZAP_ZNP_WAIT_WRITE_NWK_PARAM_RESP";
    case AZAP_ZNP_WAIT_REGISTER_ENDPOINTS_RESP:
      return "AZAP_ZNP_WAIT_REGISTER_ENDPOINTS_RESP";
    case AZAP_ZNP_WAIT_REGISTER_ZDO_CALLBACKS_RESP:
      return "AZAP_ZNP_WAIT_REGISTER_ZDO_CALLBACKS_RESP";
    case AZAP_ZNP_WAIT_STARTUP_FROM_APP_RESP:
      return "AZAP_ZNP_WAIT_STARTUP_FROM_APP_RESP";
    case AZAP_ZNP_STARTING:
      return "AZAP_ZNP_STARTING";
    case AZAP_ZNP_WAIT_PAN_INFOS:
      return "AZAP_ZNP_WAIT_PAN_INFOS";
    case AZAP_ZNP_WAIT_WRITE_PAN_ID:
      return "AZAP_ZNP_WAIT_WRITE_PAN_ID";
  }

  static char unknownState[16];
  sprintf(unknownState, "<UNKNOWN-%d>", state);
  return unknownState;
}

/**
 * @brief Give a human-readable meaning out of a ZDO application startup status.
 * @param status the state to be represented.
 * @return the resulting string representation.
 */
static char *printStartupStatus(azap_uint8 status)
{
  switch (status)
  {
    case ZDO_DEV_HOLD:
      return "Initialized - not started automatically";
    case ZDO_DEV_INIT:
      return "Initialized - not connected to anything";
    case ZDO_DEV_NWK_DISC:
      return "Discovering PAN's to join";
    case ZDO_DEV_NWK_JOINING:
      return "Joining a PAN";
    case ZDO_DEV_NWK_REJOIN:
      return "ReJoining a PAN, only for end devices";
    case ZDO_DEV_END_DEVICE_UNAUTH:
      return "Joined but not yet authenticated by trust center";
    case ZDO_DEV_END_DEVICE:
      return "Started as device after authentication";
    case ZDO_DEV_ROUTER:
      return "Device joined, authenticated and is a router";
    case ZDO_DEV_COORD_STARTING:
      return "Starting as Zigbee Coordinator";
    case ZDO_DEV_ZB_COORD:
      return "Started as Zigbee Coordinator";
    case ZDO_DEV_NWK_ORPHAN:
      return "Device has lost information about its parent...";
  }
  return "<UNKNOWN>";
}
#endif

