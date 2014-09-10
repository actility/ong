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
//** File : ./lib/port_context.c
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
#include <dirent.h>
#include <unistd.h>
#include <fcntl.h>   /* File control definitions */
#include <errno.h>
#include <termios.h> /* POSIX terminal control definitions */
#include <sys/ioctl.h> /* POSIX terminal control definitions */
#include <poll.h>
#include <stddef.h>

#include "rtlbase.h"
#include "rtllist.h"

#include "azap_api.h"
#include "azap_tools.h"

#include "uart_buf_control.h"
#include "azap_zcl_attr_val.h"
#include "azap_attribute.h"
#include "azap_endpoint.h"
#include "format_znp.h"
#include "format_znp_zdo.h"
#include "format_znp_af.h"
#include "format_znp_sapi.h"
#include "format_znp.h"

#include "znp_start_sm.h"
#include "postponed_request_elts.h"
#include "request_context.h"
#include "request_manager.h"
#include "port_context.h"
#include "azap_api_impl.h"


const int zigbeeRequestTimeout = 20;


/*****************************************************************************/
/* Prototypes for local functions */
static void portContext_t_init(portContext_t *This);
static int portContextEventPollCb(void *tb, int fd, void *r1, void *r2, int revents);
static void portContextUpdateReadStats(portContext_t *port, time_t startTime);
static void portContextUpdateWriteStats(portContext_t *port, time_t startTime);
static int portContextRequestPollCb(void *tb, int fd, void *r1, void *r2, int revents);
static bool portContext_t_sendRequest(portContext_t *This, AZAP_REQ_TTL_MGMT_MODE ttlMode,
    void *issuerContext, AZAP_REQUEST_TYPE type, bool responseExpected,
    azap_uint16 nwkAddrDest, azap_uint8 remoteEndPoint, azap_uint8 localEndPoint,
    azap_uint16 clusterId, azap_uint8 clusterDir, azap_uint16 manufacturerCode,
    bool defaultResponse, azap_uint8 cmdCode, azap_uint8 expectedCmdCode,
    azap_uint8 len, azap_uint8 *payload);

/*****************************************************************************/
/* Constructors / destructor */

/* static allocation */

/**
 * @brief Initialize the portContext_t struct
 * @anchor portContext_t_init
 * @param This the class instance
 */
static void portContext_t_init(portContext_t *This)
{
  RTL_TRDBG(TRACE_DETAIL, "portContext_t::init\n");

  /* Assign the pointers on functions */
  This->connect = portContext_t_connect;
  This->disconnect = portContext_t_disconnect;
  This->open = portContext_t_open;
  This->jumpZnp = portContext_t_jumpZnp;
  This->processRxMsg = portContext_t_processRxMsg;

  This->startZnp = portContext_t_startZnp;
  This->stopZnp = portContext_t_stopZnp;
  This->isStarted = portContext_t_isStarted;
  This->doStartZnpCompletion = portContext_t_doStartZnpCompletion;
  This->dumpStatistics = portContext_t_dumpStatistics;

  This->addEndPoint = portContext_t_addEndPoint;
  This->getEndPoint = portContext_t_getEndPoint;

  This->processZnpMsg = portContext_t_processZnpMsg;
  This->processZDOMsgCbIncoming = portContext_t_processZDOMsgCbIncoming;
  This->parseAndProcessAfDataConfirm = portContext_t_parseAndProcessAfDataConfirm;
  This->processAfIncomingMsg = portContext_t_processAfIncomingMsg;

  This->sendZdo = portContext_t_sendZdo;
  This->sendZcl = portContext_t_sendZcl;

  /* Initialize the members */
  This->portNumber = 0;
  This->pCallbacks = NULL;
  This->pMainTbPoll = NULL;
  This->defaultChannel = (1 << 11);
  This->deviceType = 0; /* coordinator */
  This->resetHanAtReboot = false;

  memset(This->deviceIeee, 0, IEEE_ADDR_LEN);
  This->shortAddr = 0;
  memset(This->panIdExt, 0, IEEE_ADDR_LEN);
  This->panId = 0;
  memset(This->zstackVersion, 0, SMALL_BUF_SIZE);
  sprintf(This->zstackVersion, "<not started yet>");

  memset(This->deviceTTYName, 0, SMALL_BUF_SIZE);
  This->deviceFd = -1;

  This->rx = NULL;
  This->tx = NULL;
  This->requestManager = NULL;

  This->startInst = NULL;
  This->started = false;

  INIT_LIST_HEAD(&This->endPointList);

  This->uartNbRead = 0;
  This->uartMaxReadTimeMs = 0;
  This->uartTotalReadTimeMs = 0;
  This->uartNbWrite = 0;
  This->uartMaxWriteTimeMs = 0;
  This->uartTotalWriteTimeMs = 0;

}

/**
 * @brief do static allocation of a portContext_t.
 * @anchor portContext_t_create
 * @param port the port number associated to this instance
 * @return the copy of the object
 */
portContext_t portContext_t_create(azap_uint8 port)
{
  portContext_t This;
  portContext_t_init(&This);
  This.portNumber = port;
  This.free = portContext_t_free;
  RTL_TRDBG(TRACE_DETAIL, "portContext_t::create (This:0x%.8x) (port:%d)\n", &This, port);
  return This;
}

/**
 * @brief Destructor for static allocation
 * @anchor portContext_t_free
 */
void portContext_t_free(portContext_t *This)
{
  RTL_TRDBG(TRACE_DETAIL, "portContext_t::free (This:0x%.8x) (port:%d)\n", This, This->portNumber);
  if (This->rx)
  {
    This->rx->free(This->rx);
  }
  if (This->tx)
  {
    This->tx->free(This->tx);
  }
  if (This->requestManager)
  {
    This->requestManager->free(This->requestManager);
  }
  while ( ! list_empty(&This->endPointList))
  {
    azapEndPoint_t *ep = list_entry(This->endPointList.next, azapEndPoint_t, chainLink);
    list_del(&ep->chainLink);
    ep->free(ep);
  }
}

/* dynamic allocation */
/**
 * @brief do dynamic allocation of a portContext_t.
 * @anchor new_portContext_t
 * @return the copy of the object
 */
portContext_t *new_portContext_t(azap_uint8 port)
{
  portContext_t *This = malloc(sizeof(portContext_t));
  if (!This)
  {
    return NULL;
  }
  portContext_t_init(This);
  This->portNumber = port;
  This->free = portContext_t_newFree;
  RTL_TRDBG(TRACE_DETAIL, "new portContext_t (This:0x%.8x) (port:%d)\n", This, port);
  return This;
}

/**
 * @brief Destructor for dynamic allocation
 * @anchor portContext_t_newFree
 */
void portContext_t_newFree(portContext_t *This)
{
  RTL_TRDBG(TRACE_DETAIL, "portContext_t::newFree (This:0x%.8x) (port:%d)\n", This,
      This->portNumber);
  if (This->rx)
  {
    This->rx->free(This->rx);
  }
  if (This->tx)
  {
    This->tx->free(This->tx);
  }
  if (This->requestManager)
  {
    This->requestManager->free(This->requestManager);
  }
  while ( ! list_empty(&This->endPointList))
  {
    azapEndPoint_t *ep = list_entry(This->endPointList.next, azapEndPoint_t, chainLink);
    list_del(&ep->chainLink);
    ep->free(ep);
  }
  free(This);
}

/* implementation methods */


/**
 * @brief Open the hardware device TTY and add resulting file description in rtl_poll manager.
 * @anchor portContext_t_connect
 * @param This port message context instance
 * @return 1 if operation succeeds, 0 otherwise
 */
bool portContext_t_connect(portContext_t *This)
{
  bool res = false;
  // if file descriptor is already set, this means the device is already connected
  if (-1 != This->deviceFd)
  {
    RTL_TRDBG(TRACE_ERROR, "Failed to connect port %d (device:%s) - already connected !\n",
        This->portNumber, azap_extAddrToString(This->deviceIeee, true));
  }
  // If we don't manage to retrieve the associated TTY, we cannot go forward
  else if (! getTTYMappedName(This->deviceIeee, This->deviceTTYName))
  {
    RTL_TRDBG(TRACE_IMPL, "Failed to find matching TTY (port:%d) (device:%s)\n",
        This->portNumber, azap_extAddrToString(This->deviceIeee, true));
  }
  // If we cannot open the TTY, we cannot go forward either
  else if (! This->open(This) )
  {
    RTL_TRDBG(TRACE_IMPL, "Failed to open TTY (port:%d) (device:%s)\n",
        This->portNumber, azap_extAddrToString(This->deviceIeee, true));
  }
  // Now it is up to ZNP to start the ZStack. Result is provided with azapStartCb
  else if ( This->jumpZnp(This) )
  {
    res = true;
  }

  return res;
}

/**
 * @brief Remove the file descriptor from rtl_poll manager and close the hardware device TTY.
 * @anchor portContext_t_disconnect
 * @param This port message context instance
 * @return 1 if operation succeeds, 0 otherwise
 */
bool portContext_t_disconnect(portContext_t *This)
{
  bool res = false;

  RTL_TRDBG(TRACE_INFO, "disconnect (port:%d) (fd:%d)\n", This->portNumber, This->deviceFd);

  if (-1 == This->deviceFd)
  {
    RTL_TRDBG(TRACE_ERROR, "disconnect: device not connected (port:%d)\n", This->portNumber);
  }
  else
  {
    rtl_pollRmv(This->pMainTbPoll, This->deviceFd);
    if (0 != close(This->deviceFd))
    {
      RTL_TRDBG(TRACE_ERROR, "disconnect: unable to close TTY (port:%d) (fd:%d) (errno:%d) "
          "(err:%s)\n", This->portNumber, This->deviceFd, errno, strerror(errno));
    }
    This->deviceFd = -1;
  }

  return res;
}



/**
 * @brief Open connection to the TTY associated with the targeted hardware device.
 * @anchor portContext_t_open
 * As a result, it also add the resulting file descriptor to the main poll table for
 * being notified of read/write events.
 * @param This port message context instance
 * @return 1 if operation succeeds, 0 otherwise
 */
bool portContext_t_open(portContext_t *This)
{
  bool res = false;
  struct termios options;

  This->deviceFd = open(This->deviceTTYName, O_RDWR | O_NOCTTY );

  if (-1 == This->deviceFd)
  {
    RTL_TRDBG(TRACE_IMPL, "Unable to open TTY %s (errno:%d) (err:%s)\n", This->deviceTTYName,
        errno, strerror(errno));
  }
  else
  {
    // configure the open FD
    fcntl(This->deviceFd, F_SETFL, 0);
    //fcntl(This->deviceFd, F_SETFL, O_ASYNC);

    RTL_TRDBG(TRACE_DETAIL, "portContext_t::open: configure port: %d\n", This->deviceFd);

    /* Get the current options for the port...  */
    tcgetattr(This->deviceFd, &options);

    /* HW flow control enabled */
    options.c_cflag |= CRTSCTS;

    /* setting baud rate */
//    cfsetispeed(&options, uartRecord.baudRate);
//    cfsetospeed(&options, uartRecord.baudRate);
    cfsetispeed(&options, USBBAUD_RATE);
    cfsetospeed(&options, USBBAUD_RATE);

    options.c_iflag &= ~( IGNBRK | BRKINT | PARMRK | ISTRIP | INLCR | IGNCR | ICRNL | IXON );

    /* setting character size */
    options.c_cflag &= ~CSIZE; /* Mask the character size bits */
    options.c_cflag |= USBCHARACTER_SIZE;    /* Select 8 data bits */

    /* setting parity */
    options.c_cflag &= ~PARENB;  /* no parity */
    options.c_cflag &= ~CSTOPB;


    /* set raw input, 1 second timeout */
    options.c_cflag |= ( CLOCAL | CREAD );
    options.c_lflag &= ~( ECHO | ECHONL | ICANON | ISIG | IEXTEN );
    options.c_oflag &= ~OPOST;
    /*READ_MODE_ISSUE+*/
    options.c_cc[VMIN] = 255;
    options.c_cc[VTIME] = 1;
//    options.c_cc[VMIN] = 0;
//    options.c_cc[VTIME] = 0;
    /*READ_MODE_ISSUE-*/

    tcsetattr(This->deviceFd, TCSANOW, &options);

    rtl_pollAdd(This->pMainTbPoll, This->deviceFd, portContextRequestPollCb,
        portContextEventPollCb, (void *)This, NULL);

    res = true;
  }

  return res;
}

/**
 * @brief Send command to jump to the ZNP firmware part.
 * @anchor portContext_t_jumpZnp
 * @param This port message context instance
 * @return 1 if operation succeeds, 0 otherwise
 */
bool portContext_t_jumpZnp(portContext_t *This)
{
  bool res = false;
  azap_uint8 pBuf[1];

  RTL_TRDBG(TRACE_DETAIL, "sending FORCE_RUN cmd (port:%d) (fd:%d)\n", This->portNumber,
      This->deviceFd);
  pBuf[0] = SB_FORCE_RUN;
  // send bell synchronously, to force jumping to ZNP firmware
  (void)write(This->deviceFd, &pBuf, 1);
  usleep(10000); // too bad...
  (void)write(This->deviceFd, &pBuf, 1);

  return res;
}

/**
 * @brief Start the port context and the ZNP stack as well.
 * @anchor portContext_t_startZnp
 * @param This the port context instance
 */
void portContext_t_startZnp(portContext_t *This)
{
  RTL_TRDBG(TRACE_INFO, "startZnp (port:%d)\n", This->portNumber);

  This->started = true;
  This->rx = new_uartBufControl_t(512);
  This->tx = new_uartBufControl_t(512);
  This->requestManager = new_requestManager_t();
  This->connect(This);

  This->startInst = new_znpStartInstance_t(This->portNumber);
  This->startInst->start(This->startInst, This->tx);
}

/**
 * @brief Stop the port context and the ZNP stack as well.
 * As a consequence, all buffered messages that were not sent are lost.
 * @anchor portContext_t_stopZnp
 * @param This the port context instance
 */
void portContext_t_stopZnp(portContext_t *This)
{
  RTL_TRDBG(TRACE_INFO, "stopZnp (port:%d)\n", This->portNumber);

  This->disconnect(This);
  if (This->startInst)
  {
    This->startInst->free(This->startInst);
  }
  This->startInst = NULL;
  if (This->rx)
  {
    This->rx->free(This->rx);
  }
  This->rx = NULL;
  if (This->tx)
  {
    This->tx->free(This->tx);
  }
  This->tx = NULL;
  if (This->requestManager)
  {
    This->requestManager->free(This->requestManager);
  }
  This->requestManager = NULL;
  This->started = false;
}

/**
 * @brief Indicates if the port is started.
 * WARNING: even if the port is started, this does not necessarily indicates that the ZNP
 * stack is started.
 * @anchor portContext_t_isStarted
 * @param This the port context instance
 * @return 1 if port context is started, 0 otherwise
 */
bool portContext_t_isStarted(portContext_t *This)
{
  return This->started;
}

/**
 * @brief Do complete the ZNP start procedure, invoking AZAP start call back with result.
 * @anchor portContext_t_doStartZnpCompletion
 * @param This the port context instance
 * @param success the result of the ZNP start procedure, 1 if success, 0 otherwise.
 */
void portContext_t_doStartZnpCompletion(portContext_t *This, bool success)
{
  if (NULL != This->startInst)
  {
    This->startInst->free(This->startInst);
    This->startInst = NULL;
  }

  azapProcessStartedCb(This->portNumber, success);
}

/**
 * @brief Dump statistics regarding port context.
 * @anchor portContext_t_dumpStatistics
 * @param This the port context instance
 * @param out the place where statistics are dumped.
 */
void portContext_t_dumpStatistics(portContext_t *This, char *out)
{
  static char szTemp[1024];
  if (!out)
  {
    return;
  }

  snprintf(szTemp, sizeof(szTemp), "UART port %d:\n"
      "- READ  [nb: %ld] [maxDur: %d ms] [avgDur: %ld ms] [totalDur: %ld ms]\n"
      "- WRITE [nb: %ld] [maxDur: %d ms] [avgDur: %ld ms] [totalDur: %ld ms]\n",
      This->portNumber, This->uartNbRead, This->uartMaxReadTimeMs,
      This->uartTotalReadTimeMs / This->uartNbRead,
      This->uartTotalReadTimeMs, This->uartNbWrite, This->uartMaxWriteTimeMs,
      This->uartTotalWriteTimeMs / This->uartNbWrite, This->uartTotalWriteTimeMs);
  strcat(out, szTemp);
}

/**
 * @brief Do process the ZNP received messages.
 * @anchor portContext_t_processRxMsg
 * @param This the port context instance.
 */
void portContext_t_processRxMsg(portContext_t *This)
{
  if (This->isStarted(This))
  {
    azap_uint8 len = 0;
    azap_uint8 cmd0 = 0;
    azap_uint8 cmd1 = 0;
    azap_uint8 payload[MAX_ZNP_PAYLOAD_SIZE];

    if (-1 == This->deviceFd)
    {
      RTL_TRDBG(TRACE_DETAIL, "processRxMsg - need to reconnect (port:%d) (fd:%d)\n",
          This->portNumber, This->deviceFd);
      if (NULL == This->startInst)
      {
        // port was disconnected (dongle unplugged or TTY lost)
        // we need to resume ZNP
        This->stopZnp(This);
        This->resetHanAtReboot = false;
        This->startZnp(This);
      }
      else
      {
        // try to reconnect
        This->connect(This);
      }
    }

    memset(payload, 0, MAX_ZNP_PAYLOAD_SIZE);
    if (This->rx->getNextZNPMsg(This->rx, &len, &cmd0, &cmd1, payload))
    {
      if (NULL != This->startInst)
      {
        This->startInst->processZnpMsg(This->startInst, This->tx, len, cmd0, cmd1, payload);
      }
      else
      {
        This->processZnpMsg(This, len, cmd0, cmd1, payload);
      }
    }
  }
}

/**
 * @brief Add a new end point to the device managed on this port context.
 * This method does not make a copy of the end point structure. If the operation
 * fails the end point structure is not freed, this is up to the caller to do so.
 * If the operation succeeds the added end points and there resources are freed
 * when the port is released.
 * @anchor portContext_t_addEndPoint.
 * @param This the port context instance.
 * @param ep the new end point to add.
 * @return 1 if end point has been added successfully, 0 otherwise. This method
 * can fail if an end point with the same identifier is already associated to
 * the port context.
 */
bool portContext_t_addEndPoint(portContext_t *This, azapEndPoint_t *ep)
{
  bool found = false;
  struct list_head *link;
  azapEndPoint_t *epElt;

  if (! ep)
  {
    return ! found;
  }

  list_for_each (link, &This->endPointList)
  {
    epElt = list_entry(link, azapEndPoint_t, chainLink);
    if (epElt->id == ep->id)
    {
      found = true;
      RTL_TRDBG(TRACE_ERROR, "portContext_t::addEndPoint - end point identifier %d already "
          "exits (port:%d)", ep->id, This->portNumber);
      break;
    }
  }

  if (! found)
  {
    list_add_tail(&ep->chainLink, &This->endPointList);
  }

  return !found;
}

/**
 * @brief Retrieve the end point from its identifier.
 * @anchor portContext_t_getEndPoint
 * @param This the port context instance.
 * @param id the end point identifier we are looking for.
 * @return the reference on the matching end point structure if found, NULL otherwise.
 */
azapEndPoint_t *portContext_t_getEndPoint(portContext_t *This, azap_uint8 id)
{
  azapEndPoint_t *res = NULL;
  struct list_head *link;
  azapEndPoint_t *epElt;

  list_for_each (link, &This->endPointList)
  {
    epElt = list_entry(link, azapEndPoint_t, chainLink);
    if (epElt->id == id)
    {
      res = epElt;
      break;
    }
  }

  return res;
}

/**
 * @brief Dispatch the received ZNP message to the method in charge of processing it.
 * @anchor portContext_t_processZnpMsg
 * @param This the object instance
 * @param len the length of the received payload
 * @param cmd0 the byte CMD0 of the received message
 * @param cmd1 the byte CMD1 of the received message
 * @param data the received payload
 */
void portContext_t_processZnpMsg(portContext_t *This, azap_uint8 len, azap_uint8 cmd0,
    azap_uint8 cmd1, azap_uint8 *payload)
{
  azap_uint16 cmd = BUILD_UINT16(cmd0, cmd1);

  RTL_TRDBG(TRACE_DETAIL, "portContext_t::processZnpMsg (port:%d) (cmd0:0x%.2x) "
      "(cmd1:0x%.2x)\n", This->portNumber, cmd0, cmd1);

  switch (cmd)
  {
      ///////////////////////////////////////////////////////////////////////////
      // ZDO operations
    case MT_ZDO_MSG_CB_INCOMING_AREQ:
      This->processZDOMsgCbIncoming(This, len, payload);
      break;
    case MT_ZDO_NWK_ADDR_SRSP:
    case MT_ZDO_IEEE_ADDR_SRSP:
    case MT_ZDO_NODE_DESC_SRSP:
    case MT_ZDO_POWER_DESC_SRSP:
    case MT_ZDO_SIMPLE_DESC_SRSP:
    case MT_ZDO_ACTIVE_EP_SRSP:
    case MT_ZDO_BIND_SRSP:
    case MT_ZDO_UNBIND_SRSP:
    case MT_ZDO_MGMT_RTG_SRSP:
    case MT_ZDO_MGMT_BIND_SRSP:
    case MT_ZDO_MGMT_LEAVE_SRSP:
    case MT_ZDO_MGMT_PERMIT_JOIN_SRSP:
      // this is the synchronous response for previous SREQ sent... for now on
      // we don't know how to handle these responses... so ignore it for now.
      break;
      ///////////////////////////////////////////////////////////////////////////
      // AF operations
    case MT_AF_DATA_CONFIRM_AREQ:
      This->parseAndProcessAfDataConfirm(This, len, payload);
      break;
    case MT_AF_INCOMING_MSG_AREQ:
      This->processAfIncomingMsg(This, len, payload);
      break;
    case MT_AF_DATA_EXT_SRSP:
      // this is the synchronous response for previous SREQ sent... for now on
      // we don't know how to handle these responses... so ignore it for now.
      break;

      ///////////////////////////////////////////////////////////////////////////
      // SAPI operations

      ///////////////////////////////////////////////////////////////////////////
    default:
      RTL_TRDBG(TRACE_ERROR, "portContext_t::processZnpMsg received unsupported command "
          "(port:%d) (cmd0:0x%.2x) (cmd1:0x%.2x)\n", This->portNumber, cmd0, cmd1);
      break;
  }
}

/**
 * @brief Parse header of incoming ZDO_MSG_CB_INCOMING message and invoke associated call back.
 * @anchor portContext_t_processZDOMsgCbIncoming
 * @param This the object instance
 * @param len the length of the received payload
 * @param data the received payload
 */
void portContext_t_processZDOMsgCbIncoming(portContext_t *This, azap_uint8 len,
    azap_uint8 *payload)
{
  //azap_uint8 wasBroadcast, securityUse, seqNum;
  azap_uint16 srcAddr, clusterId;
  //azap_uint16 macDstAddr;

  if (!payload)
  {
    return;
  }
  if (len < 9)
  {
    return;
  }

  srcAddr = parseUint16AndShift(&payload, &len);
  /* wasBroadcast = */
  parseUint8AndShift(&payload, &len);
  clusterId = parseUint16AndShift(&payload, &len);
  /* securityUse = */
  parseUint8AndShift(&payload, &len);
  /* seqNum = */
  parseUint8AndShift(&payload, &len);
  /* macDstAddr = */
  parseUint16AndShift(&payload, &len);

  switch (clusterId)
  {
      // incoming indications
    case Device_annce:
      azapNbZdoNotifications[This->portNumber] ++;
      azapProcessZdoDeviceAnnceCb(This, srcAddr, len, payload);
      break;

//    // incoming requests
//    case Bind_req:
//      azapProcessZdoBindReqCb(This, srcAddr, len, payload);
//      break;
//
//    case Unbind_req:
//      azapProcessZdoUnbindReqCb(This, srcAddr, len, payload);
//      break;

      // incoming responses
    case Bind_rsp:
      azapNbZdoRequestsCompleted[This->portNumber] ++;
//      context = This->requestManager->removePending(This->requestManager,
//        srcAddr, AZAP_ZDO_BIND_REQ, clusterId, 0, 0);
      azapProcessZdoBindRspCb(This, srcAddr, len, payload);
      break;

    case Unbind_rsp:
      azapNbZdoRequestsCompleted[This->portNumber] ++;
      azapProcessZdoUnbindRspCb(This, srcAddr, len, payload);
      break;

    case NWK_addr_rsp:
      azapNbZdoRequestsCompleted[This->portNumber] ++;
      azapProcessZdoNwkAddrRspCb(This, srcAddr, len, payload);
      break;

    case IEEE_addr_rsp:
      azapNbZdoRequestsCompleted[This->portNumber] ++;
      azapProcessZdoIeeeAddrRspCb(This, srcAddr, len, payload);
      break;

    case Node_Desc_rsp:
      azapNbZdoRequestsCompleted[This->portNumber] ++;
      azapProcessZdoNodeDescRspCb(This, srcAddr, len, payload);
      break;

    case Power_Desc_rsp:
      azapNbZdoRequestsCompleted[This->portNumber] ++;
      azapProcessZdoPowerDescRspCb(This, srcAddr, len, payload);
      break;

    case Simple_Desc_rsp:
      azapNbZdoRequestsCompleted[This->portNumber] ++;
      azapProcessZdoSimpleDescRspCb(This, srcAddr, len, payload);
      break;

    case Active_EP_rsp:
      azapNbZdoRequestsCompleted[This->portNumber] ++;
      azapProcessZdoActiveEPRspCb(This, srcAddr, len, payload);
      break;

    case Mgmt_Rtg_rsp:
      azapNbZdoRequestsCompleted[This->portNumber] ++;
      azapProcessZdoMgmtRtgRspCb(This, srcAddr, len, payload);
      break;

    case Mgmt_Bind_rsp:
      azapNbZdoRequestsCompleted[This->portNumber] ++;
      azapProcessZdoMgmtBindRspCb(This, srcAddr, len, payload);
      break;

    case Mgmt_Permit_Join_rsp:
      azapNbZdoRequestsCompleted[This->portNumber] ++;
      azapProcessZdoMgmtPermitJoinRspCb(This, srcAddr, len, payload);
      break;

    case Mgmt_Leave_rsp:
      azapNbZdoRequestsCompleted[This->portNumber] ++;
      azapProcessZdoMgmtLeaveRspCb(This, srcAddr, len, payload);
      break;

    default:
      RTL_TRDBG(TRACE_ERROR, "portContext_t::processZDOMsgCbIncoming received ZDO message "
          "from unsupported cluster ID 0x%.4x (port:%d)\n", clusterId, This->portNumber);
      break;
  }
}

/**
 * @brief Process AF_DATA_CONFIRM message.
 * <ul><li>if parsed status is success, free the associated message context</li>
 * <li>otherwise invoke associated call back method</li></ul>
 * @anchor portContext_t_parseAndProcessAfDataConfirm
 * @param This the port context instance
 * @param len received message length.
 * @param data the received message payload.
 */
void portContext_t_parseAndProcessAfDataConfirm(
  portContext_t *This,
  azap_uint8 len,
  azap_uint8 *data)
{

  azap_uint8 status = parseUint8AndShift(&data, &len);
  parseUint8AndShift(&data, &len); // endpoint
  azap_uint8 transId = parseUint8AndShift(&data, &len);

  if (ZCL_STATUS_SUCCESS != status)
  {
    requestContext_t *msgContext = This->requestManager->removePendingAf(
        This->requestManager, transId);

    if (NULL != msgContext)
    {
      RTL_TRDBG(TRACE_API, "AF_DATA_CONFIRM indicates that ZNP failed to send a message"
          "(transId:0x%.2x) (msgType:%d) (nwkAddr:0x%.4x) (endPoint:0x%.2x) (clusterId:0x%.4x) "
          "(clusterDir:%d) (status:0x%.2x)\n", transId, msgContext->type,
          msgContext->nwkAddrDest, msgContext->remoteEndp, msgContext->clusterId,
          msgContext->clusterDir, status);

      This->requestManager->callbackOnFailure(This->requestManager, msgContext, This);
      msgContext->free(msgContext);

    }
  }
}


/**
 * @brief Parse header of incoming AF_INCOMING_MSG message and invoke associated call back.
 * @anchor portContext_t_processAfIncomingMsg
 * @param This the object instance
 * @param len the length of the received payload
 * @param data the received payload
 */
void portContext_t_processAfIncomingMsg(portContext_t *This, azap_uint8 len,
    azap_uint8 *payload)
{
  azap_uint8 srcEP, dstEP, /* wasBroadcast, linkQuality, securityUse, seqNum,*/ afPktLen;
  azap_uint16 /*groupId,*/ clusterId, srcAddr;
  //azap_uint32 timestamp;

  RTL_TRDBG(TRACE_DETAIL, "portContext_t::processAfIncomingMsg (port:%d)\n",
      This->portNumber);

  if (!payload)
  {
    return;
  }
  if (len < 9)
  {
    return;
  }

  /* groupId = */ parseUint16AndShift(&payload, &len);
  clusterId = parseUint16AndShift(&payload, &len);
  srcAddr = parseUint16AndShift(&payload, &len);
  srcEP = parseUint8AndShift(&payload, &len);
  dstEP = parseUint8AndShift(&payload, &len);
  /* wasBroadcast = */
  parseUint8AndShift(&payload, &len);
  /* linkQuality = */
  parseUint8AndShift(&payload, &len);
  /* securityUse = */
  parseUint8AndShift(&payload, &len);
  /* timestamp = */
  parseUint32AndShift(&payload, &len);
  /* seqNum = */
  parseUint8AndShift(&payload, &len);
  afPktLen = parseUint8AndShift(&payload, &len);

  // Check various unconsistency here
  if (NULL == This->getEndPoint(This, dstEP))
  {
    RTL_TRDBG(TRACE_ERROR, "portContext_t::processAfIncomingMsg received AF packet "
        "but no end point match the one provided (port:%d) (srcAddr:0x%.4x) (dstEP:0x%.2x)\n",
        This->portNumber, srcAddr, dstEP);
    return;
  }

  if (len != afPktLen)
  {
    RTL_TRDBG(TRACE_ERROR, "portContext_t::processAfIncomingMsg inconsistent AF packet "
        "length detected (port:%d) (srcAddr:0x%.4x) (len:%d) (afPktLen:%d)\n",
        This->portNumber, srcAddr, len, afPktLen);
    return;
  }

  azapZclFrameHeader_t hdr;
  parseZCLHeader(&payload, &len, &hdr);

  if (hdr.fc.type)
  {
    azapNbZclRequestsCompleted[This->portNumber] ++;
    azapProcessZclIncomingCmdCb(This, srcAddr, srcEP, clusterId, &hdr, len,
        payload);
  }
  else
  {
    switch (hdr.commandId)
    {
      case ZCL_CMD_READ_RSP:
        azapNbZclRequestsCompleted[This->portNumber] ++;
        azapProcessZclReadCmdRspCb(This, srcAddr, srcEP, clusterId, &hdr, len,
            payload);
        break;
      case ZCL_CMD_WRITE_RSP:
        azapNbZclRequestsCompleted[This->portNumber] ++;
        azapProcessZclWriteCmdRspCb(This, srcAddr, srcEP, clusterId, &hdr, len,
            payload);
        break;
      case ZCL_CMD_CONFIG_REPORT:
        azapNbZclRequestsCompleted[This->portNumber] ++;
        azapProcessZclConfigReportCmdCb(This, srcAddr, srcEP, clusterId, &hdr,
            len, payload);
        break;
      case ZCL_CMD_CONFIG_REPORT_RSP:
        azapNbZclRequestsCompleted[This->portNumber] ++;
        azapProcessZclConfigReportCmdRspCb(This, srcAddr, srcEP, clusterId, &hdr,
            len, payload);
        break;
      case ZCL_CMD_REPORT:
        azapNbZclNotifications[This->portNumber] ++;
        azapProcessZclReportCmdCb(This, srcAddr, srcEP, clusterId, &hdr, len,
            payload);
        break;
      case ZCL_CMD_READ_REPORT_CFG_RSP:
        azapNbZclRequestsCompleted[This->portNumber] ++;
        azapProcessZclReadReportConfigCmdRspCb(This, srcAddr, srcEP, clusterId, &hdr,
            len, payload);
        break;
      case ZCL_CMD_DISCOVER_RSP:
        azapNbZclRequestsCompleted[This->portNumber] ++;
        azapProcessZclDiscoverCmdRspCb(This, srcAddr, srcEP, clusterId, &hdr, len,
            payload);
        break;

      case ZCL_CMD_DEFAULT_RSP:
        // an outgoing command sent priorly is acknowledged here.
        // If needed we can wait this acknowledgement to send dIa response...
        break;

        // unsupported foundation commands
      case ZCL_CMD_READ:
      case ZCL_CMD_WRITE:
      case ZCL_CMD_WRITE_UNDIVIDED:
      case ZCL_CMD_WRITE_NO_RSP:
      case ZCL_CMD_READ_REPORT_CFG:
      case ZCL_CMD_DISCOVER:
      default:
        RTL_TRDBG(TRACE_ERROR, "portContext_t::processAfIncomingMsg - error: "
            "received unsupported foundation command (port:%d) (srcAddr:0x%.4x) "
            "(cmd:0x%.2x)\n", This->portNumber, srcAddr, hdr.commandId);
        break;
    }
  }
}

/**
 * @brief Send or store a ZDO request and create associated request context.
 * @param This the object instance
 * @param ttlMode the TTL request management model (@see AZAP_REQ_TTL_MGMT_MODE).
 * @param issuerContext a placeholder where the issuer can store anything he needs,
 * and which will provided by AZAP in call back method.
 * @param type the AZAP request type (e.g. AZAP_ZDO_BIND_REQ, see @ref AZAP_REQUEST_TYPE)
 * @param responseExpected specifies if a response is expected, and as a consequence if a
 * request context should be created.
 * @param nwkAddrDest The destination short address.
 * @param remoteEndPoint The end point identifier on the destination node.
 * @param len the length of the payload to be sent
 * @param data the payload to be sent.
 * @return true if request can be sent (or stored if hardware device is not ready),
 * false otherwise.
 */
bool portContext_t_sendZdo(portContext_t *This, AZAP_REQ_TTL_MGMT_MODE ttlMode,
    void *issuerContext, AZAP_REQUEST_TYPE type, bool responseExpected,
    azap_uint16 nwkAddrDest, azap_uint8 remoteEndPoint, azap_uint8 len,
    azap_uint8 *payload)
{
  return portContext_t_sendRequest(This, ttlMode, issuerContext, type, responseExpected,
      nwkAddrDest, remoteEndPoint, 0, 0, 0, 0, false, 0, 0, len, payload);
}

/**
 * @brief Send or store a ZCL request and create associated request context.
 * @param This the object instance
 * @param ttlMode the TTL request management model (@see AZAP_REQ_TTL_MGMT_MODE).
 * @param issuerContext a placeholder where the issuer can store anything he needs,
 * and which will provided by AZAP in call back method.
 * @param type the AZAP request type (e.g. AZAP_ZDO_BIND_REQ, see @ref AZAP_REQUEST_TYPE)
 * @param responseExpected specifies if a response is expected, and as a consequence if a
 * request context should be created.
 * @param nwkAddrDest The destination short address.
 * @param remoteEndPoint The end point identifier on the destination node.
 * @param localEndPoint The local end point identifier
 * @param clusterId The cluster identifier the ZCL request refers to.
 * @param clusterDir The cluster direction the ZCL command implements.
 * @param manufacturerCode Gives the manufacturer code if relevant, 0 otherwise.
 * @param defaultResponse Specifies if the destination node should generate a AF
 * default response.
 * @param cmdCode the ZCL command code (mandatory for azapZclOutgoingCmdReq API).
 * @param expectedCmdCode the ZCL command code expected for the response.
 * @param len the length of the payload to be sent
 * @param data the payload to be sent
 * @return true if request can be sent (or stored if hardware device is not ready),
 * false otherwise.
 */
bool portContext_t_sendZcl(portContext_t *This, AZAP_REQ_TTL_MGMT_MODE ttlMode,
    void *issuerContext, AZAP_REQUEST_TYPE type, bool responseExpected,
    azap_uint16 nwkAddrDest, azap_uint8 remoteEndPoint, azap_uint8 localEndPoint,
    azap_uint16 clusterId, azap_uint8 clusterDir, azap_uint16 manufacturerCode,
    bool defaultResponse, azap_uint8 cmdCode, azap_uint8 expectedCmdCode,
    azap_uint8 len, azap_uint8 *payload)
{
  return portContext_t_sendRequest(This, ttlMode, issuerContext, type, responseExpected,
      nwkAddrDest, remoteEndPoint, localEndPoint, clusterId, clusterDir,
      manufacturerCode, defaultResponse, cmdCode, expectedCmdCode, len, payload);
}


/*****************************************************************************/
/*   _                 _   _            _    __              _   _
 *  | |   ___  __ __ _| | | |_ ___  ___| |  / _|_  _ _ _  __| |_(_)___ _ _  ___
 *  | |__/ _ \/ _/ _` | | |  _/ _ \/ _ \ | |  _| || | ' \/ _|  _| / _ \ ' \(_-<
 *  |____\___/\__\__,_|_|  \__\___/\___/_| |_|  \_,_|_||_\__|\__|_\___/_||_/__/
 */



/**
 * @fn static bool getTTYMappedName(azap_uint8 * targetedIeee, char *bindingDevice)
 * @brief Scan the USB devices plugged on the system and detect which one is CC2531 dongle.
 * WARNING: the function works on linux running kernel 2.6.18, but there is no warranty on
 * other systems.
 * @param targetedIeee the IEEE address to match, if provided. If not provided (i.e. if null),
 * assume the first met.
 * @param bindingDevice if CC2531 dongle found, provides with the mapped TTY name.
 * @return 1 if CC2531 dongle found, 0 otherwise.
 */
bool getTTYMappedName(azap_uint8 *targetedIeee, char *bindingDevice)
{
  int rc;
  char deviceLink[255];
  char devicePath[255];
  char readBuf[SMALL_BUF_SIZE];
  char temp[SMALL_BUF_SIZE];

  char productPath[255];
  FILE *product;

  char serialPath[255];
  FILE *serial;

  DIR *ttyDir;
  bool res = false;
  struct dirent *currentEntry;

  if (!bindingDevice)
  {
    return false;
  }

  sprintf(bindingDevice, "<unknown>");

  ttyDir = opendir(TTY_SCAN_ROOT_DIR);
  if (ttyDir)
  {
    while ((currentEntry = readdir(ttyDir)))
    {
      // scan /sys/class/tty/ content, looking for ttyACM* sub-dirs
      if (((currentEntry->d_type == DT_DIR) || (currentEntry->d_type == DT_LNK)) &&
          (!strncmp(currentEntry->d_name, TTY_ACM_PREFIX, strlen(TTY_ACM_PREFIX))))
      {
        snprintf(devicePath, sizeof(devicePath), "%s%s/device", TTY_SCAN_ROOT_DIR,
            currentEntry->d_name);
        rc = readlink(devicePath, deviceLink, sizeof(deviceLink));
        if (rc > 0)
        {
          deviceLink[rc] = 0;
          // if "device" symbolic link is valid, look for the product description
          snprintf(productPath, sizeof(productPath), "%s%s/%s/../product", TTY_SCAN_ROOT_DIR,
              currentEntry->d_name, deviceLink);
          product = fopen(productPath, "r");
          if (product)
          {
            rc = fread(readBuf, sizeof(char), sizeof(readBuf), product);
            if ((rc > 0) &&
                (!strncmp(readBuf, DONGLE_PRODUCT_NAME, strlen(DONGLE_PRODUCT_NAME))))
            {
              // The file content matches the CC2531 product description
              if (! isExtAddrNull(targetedIeee) )
              {
                snprintf(serialPath, sizeof(serialPath), "%s%s/%s/../serial",
                    TTY_SCAN_ROOT_DIR, currentEntry->d_name, deviceLink);
                serial = fopen(serialPath, "r");
                if (serial)
                {
                  sprintf(temp, "__%s", azap_extAddrToString(targetedIeee, true));
                  rc = fread(readBuf, sizeof(char), sizeof(readBuf), serial);
                  if ((rc > 0) &&
                      (!strncasecmp(readBuf, temp, strlen(temp))))
                  {
                    // The product description and the serial number both match
                    sprintf(bindingDevice, "/dev/%s", currentEntry->d_name);
                    res = true;
                  }
                  else if (-1 == rc)
                  {
                    RTL_TRDBG(TRACE_ERROR, "getTTYMappedName - Failed to read %s (errno:%d) "
                        "(err:%s)\n", serialPath, errno, strerror(errno));
                  }

                  if (0 != fclose(serial))
                  {
                    RTL_TRDBG(TRACE_ERROR, "getTTYMappedName - Failed to close %s (errno:%d) "
                        "(err:%s)\n", serialPath, errno, strerror(errno));
                  }
                }
              }
              else
              {
                // The product description matches and the serial number is ignored
                sprintf(bindingDevice, "/dev/%s", currentEntry->d_name);
                res = true;
              }
            }
            else if (-1 == rc)
            {
              RTL_TRDBG(TRACE_ERROR, "getTTYMappedName - Failed to read %s (errno:%d) "
                  "(err:%s)\n", productPath, errno, strerror(errno));
            }

            if (0 != fclose(product))
            {
              RTL_TRDBG(TRACE_ERROR, "getTTYMappedName - Failed to close %s (errno:%d) "
                  "(err:%s)\n", productPath, errno, strerror(errno));
            }
          }
          else
          {
            RTL_TRDBG(TRACE_ERROR, "getTTYMappedName - Failed to open %s (errno:%d) (err:%s)\n",
                product, errno, strerror(errno));
          }
        }
        else
        {
          RTL_TRDBG(TRACE_ERROR, "getTTYMappedName - Failed to read link %s (errno:%d) "
              "(err:%s)\n", devicePath, errno, strerror(errno));
        }
      }
    }

    if (0 != closedir(ttyDir))
    {
      RTL_TRDBG(TRACE_ERROR, "getTTYMappedName - Failed to close %s (errno:%d) (err:%s)\n",
          TTY_SCAN_ROOT_DIR, errno, strerror(errno));
    }
  }
  else
  {
    RTL_TRDBG(TRACE_ERROR, "getTTYMappedName - Failed to open %s (errno:%d) (err:%s)\n",
        TTY_SCAN_ROOT_DIR, errno, strerror(errno));
  }

  return res;
}


/**
 * @brief Call back function for rtlPoll to get the requested events.
 * This call back function is raised when rtlpoll needs to set the appropriate
 * POLL flags for the provided file descriptor.
 * This call back function is raised when poll event is detected on
 * TTY.
 * @param tb the table of all polled file descriptors
 * @param fd the file descriptor on which events were detected
 * @param r1 user's data placeholder #1 (here the portContext_t instance)
 * @param r2 user's data placeholder #2
 * @param revents the detected events
 * @return -1 in case of error detection, 0 otherwise
 */
static int portContextEventPollCb(void *tb, int fd, void *r1, void *r2, int revents)
{
  portContext_t *This = (portContext_t *)r1;
  int res = -1;

//  RTL_TRDBG(TRACE_FULL, "portContextEventPollCb (This:0x%.8x) (fd:%d)\n", This, fd);

  if (This)
  {
    if (fd != This->deviceFd)
    {
      RTL_TRDBG(TRACE_ERROR, "no ZigBee/TTY for fd=%d\n", fd);
      rtl_pollRmv(This->pMainTbPoll, This->deviceFd);
      This->deviceFd = -1;
    }
    else
    {
      /* we always want to read */
      res = POLLIN;
      /* do we have something to write ? */
      if (This->tx->getBufferedSize(This->tx))
      {
        res |= POLLOUT;
      }
    }
  }

  return res;
}

/**
 * @brief Update the UART statistics regarding read operation.
 * @param port the port context that manages the UART connection.
 * @param startTime the read operation start time.
 */
static void portContextUpdateReadStats(portContext_t *port, time_t startTime)
{
  time_t duration = rtl_tmms() - startTime;

  port->uartNbRead++;
  port->uartTotalReadTimeMs += duration;
  if (duration > port->uartMaxReadTimeMs)
  {
    port->uartMaxReadTimeMs = duration;
  }
}

/**
 * @brief Update the UART statistics regarding write operation.
 * @param port the port context that manages the UART connection.
 * @param startTime the write operation start time.
 */
static void portContextUpdateWriteStats(portContext_t *port, time_t startTime)
{
  time_t duration = rtl_tmms() - startTime;

  port->uartNbWrite++;
  port->uartTotalWriteTimeMs += duration;
  if (duration > port->uartMaxWriteTimeMs)
  {
    port->uartMaxWriteTimeMs = duration;
  }
}

/**
 * @brief Call back function for rtlPoll to get the requested events.
 * This call back function is raised when rtlpoll needs to set the appropriate
 * POLL flags for the provided file descriptor.
 * Here the question is: on the CLI session matching the provided file descriptor,
 * do we want to read ? and do we want to write ?
 * Here we always want to raise the POLLIN flag, and we eventually want to raise the
 * POLLOUT flag if we have something queued, waiting for write permission.
 * @param tb the table of all polled file descriptors
 * @param fd the file descriptor on which events were detected
 * @param r1 user's data placeholder #1 (here the portContext_t instance)
 * @param r2 user's data placeholder #2
 * @param revents the detected events
 * @return -1 in case of error detection, 0 otherwise
 */
static int portContextRequestPollCb(void *tb, int fd, void *r1, void *r2, int revents)
{
  portContext_t *This = (portContext_t *)r1;
  int res = -1;
  azap_uint8 rc;
  time_t now;

//  RTL_TRDBG(TRACE_FULL, "portContextRequestPollCb (This:0x%.8x) (fd:%d)\n", This, fd);

  if (This)
  {
    if (fd != This->deviceFd)
    {
      RTL_TRDBG(TRACE_ERROR, "no ZigBee/TTY for fd=%d\n", fd);
      rtl_pollRmv(This->pMainTbPoll, This->deviceFd);
      This->deviceFd = -1;
    }
    else
    {
      res = 0;
      azap_uint8 data[255];
      azap_uint8 len = 255;

      /////////////////////////////////////////////////////////////////////////
      /* We have something to read on TTY; ZigBee messages (responses or asynchronous req) */
      if ( (revents & POLLIN) == POLLIN )
      {
        RTL_TRDBG(TRACE_DETAIL, "event read ZigBee/TTY fd=%d\n", This->deviceFd);
        memset(data, 0, 255);

        now = rtl_tmms();
        len = read(This->deviceFd, data, sizeof(data));
        portContextUpdateReadStats(This, now);

        if (len)
        {
          RTL_TRDBG(TRACE_DETAIL, "read successful (len:%d) (%s)\n", len,
              azap_byteArrayToString(data, len));
          rc = This->rx->put(This->rx, len, data);
          if (rc != len)
          {
            RTL_TRDBG(TRACE_ERROR, "Error put (rc:%d) (len:%d)\n", rc, len);
          }
        }
        else
        {
          // POLLIN flag raised, but nothing to read; this means the device is disconnected
          This->disconnect(This);
        }
      }

      /////////////////////////////////////////////////////////////////////////
      /* We can write on TTY; send pending ZigBee requests */
      if ( (revents & POLLOUT) == POLLOUT )
      {
        memset(data, 0, 255);
        // we can write at maximum 255 caracters at once
        if (This->tx->getBufferedSize(This->tx) < 255)
        {
          len = This->tx->getBufferedSize(This->tx);
        }
        RTL_TRDBG(TRACE_DETAIL, "event write ZigBee/TTY fd=%d len=%d\n", This->deviceFd, len);
        This->tx->get(This->tx, len, data);

        now = rtl_tmms();
        rc = write(This->deviceFd, data, len);
        portContextUpdateWriteStats(This, now);

        if (rc != len)
        {
          RTL_TRDBG(TRACE_ERROR, "Error write (rc:%d) (len:%d)\n", rc, len);
        }
        else
        {
          RTL_TRDBG(TRACE_DETAIL, "write successful (len:%d) (%s)\n", len,
              azap_byteArrayToString(data, len));
        }
      }
    }
  }
  return res;
}

/**
 * @brief Send through ZNP or store a request and create associated request context.
 * @param This the object instance
 * @param ttlMode the TTL request management model (@see AZAP_REQ_TTL_MGMT_MODE).
 * @param issuerContext a placeholder where the issuer can store anything he needs,
 * and which will provided by AZAP in call back method.
 * @param type the AZAP request type (e.g. AZAP_ZDO_BIND_REQ, see @ref AZAP_REQUEST_TYPE)
 * @param responseExpected specifies if a response is expected, and as a consequence if a
 * request context should be created.
 * @param nwkAddrDest The destination short address.
 * @param remoteEndPoint The end point identifier on the destination node.
 * @param localEndPoint The local end point identifier
 * @param clusterId The cluster identifier the ZCL request refers to.
 * @param clusterDir The cluster direction the ZCL command implements.
 * @param manufacturerCode Gives the manufacturer code if relevant, 0 otherwise.
 * @param defaultResponse Specifies if the destination node should generate a AF
 * default response.
 * @param cmdCode the ZCL command code (mandatory for azapZclOutgoingCmdReq API).
 * @param expectedCmdCode the ZCL command code expected for the response.
 * @param len the length of the payload to be sent
 * @param data the payload to be sent
 * @return true if request can be sent (or stored if hardware device is not ready),
 * false otherwise.
 */
static bool portContext_t_sendRequest(portContext_t *This, AZAP_REQ_TTL_MGMT_MODE ttlMode,
    void *issuerContext, AZAP_REQUEST_TYPE type, bool responseExpected,
    azap_uint16 nwkAddrDest, azap_uint8 remoteEndPoint, azap_uint8 localEndPoint,
    azap_uint16 clusterId, azap_uint8 clusterDir, azap_uint16 manufacturerCode,
    bool defaultResponse, azap_uint8 cmdCode, azap_uint8 expectedCmdCode, azap_uint8 len,
    azap_uint8 *payload)
{
  bool res = false;
  requestContext_t *context = NULL;

  postponedRequestElts_t *params = new_postponedRequestElts_t(responseExpected);
  params->localEndp = localEndPoint;
  params->defaultResponse = defaultResponse;
  params->cmdCode = cmdCode;
  params->dataLen = len;

  if (This->requestManager->readyToSendDoublonCheck(This->requestManager, nwkAddrDest,
      type, clusterId, remoteEndPoint, expectedCmdCode))
  {
    requestContext_t *tmpReqContext = NULL;
    if (responseExpected)
    {
      context = This->requestManager->addPendingRequest(This->requestManager,
          issuerContext, nwkAddrDest, type, clusterId, clusterDir, manufacturerCode,
          remoteEndPoint, expectedCmdCode, zigbeeRequestTimeout, ttlMode);
    }
    else
    {
      // no response expected
      tmpReqContext = new_requestContext_t(zigbeeRequestTimeout, ttlMode);
      tmpReqContext->type = type;
      tmpReqContext->nwkAddrDest = nwkAddrDest;
      tmpReqContext->clusterId = clusterId;
      tmpReqContext->clusterDir = clusterDir;
      tmpReqContext->manufacturerCode = manufacturerCode;
      tmpReqContext->remoteEndp = remoteEndPoint;
      tmpReqContext->expectedCmdCode = expectedCmdCode;

      context = tmpReqContext;
    }
    params->cmdData = payload;
    context->requestParameters = params;

    res = This->requestManager->sendRequest(This->requestManager, context, This);

    // warning not to free resources provided through API !
    context->requestParameters->cmdData = NULL;
    context->requestParameters->free(context->requestParameters);
    context->requestParameters = NULL;
    if (tmpReqContext)
    {
      tmpReqContext->free(tmpReqContext);
    }
  }
  else
  {
    context = This->requestManager->addPostponedRequest(This->requestManager,
        issuerContext, nwkAddrDest, type, clusterId, clusterDir, manufacturerCode,
        remoteEndPoint, expectedCmdCode, params, zigbeeRequestTimeout, ttlMode);
    // copy data in this case only
    params->cmdData = malloc(len);
    memcpy(params->cmdData, payload, len);
    context->requestParameters = params;
    res = true;
  }

  if (! res)
  {
    // free request context, no need to invoke call back method
    context = This->requestManager->removePending(This->requestManager,
        context->nwkAddrDest, context->type, context->clusterId,
        context->remoteEndp, context->expectedCmdCode);
    context->free(context);
  }

  return res;
}


