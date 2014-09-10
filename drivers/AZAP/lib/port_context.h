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
//** File : ./lib/port_context.h
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



#ifndef _AZAP_PORT_CONTECT__H_
#define _AZAP_PORT_CONTECT__H_


#define SMALL_BUF_SIZE 32

#define USBBAUD_RATE B115200
#define USBCHARACTER_SIZE CS8 //8n1

#define SB_FORCE_BOOT               0xF8
#define SB_FORCE_RUN               (SB_FORCE_BOOT ^ 0xFF)

#define TTY_ACM_PREFIX "ttyACM"
#define DONGLE_PRODUCT_NAME "TI CC2531 USB CDC"
#define TTY_SCAN_ROOT_DIR "/sys/class/tty/"


/**
 * @struct portContext_t
 * @brief Defines all attributes for a connection to a CC2531 dongle.
 */
typedef struct portContext_t
{
  /* methods */
  /** @ref portContext_t_startZnp */
  void (* startZnp) (struct portContext_t *);
  /** @ref portContext_t_isStarted */
  bool (* isStarted) (struct portContext_t *);
  /** @ref portContext_t_doStartZnpCompletion */
  void (* doStartZnpCompletion) (struct portContext_t *, bool success);
  /** @ref portContext_t_stopZnp */
  void (* stopZnp) (struct portContext_t *);
  /** @ref portContext_t_dumpStatistics */
  void (*dumpStatistics) (struct portContext_t *This, char *out);

  /** @ref portContext_t_connect */
  bool (* connect) (struct portContext_t *);
  /** @ref portContext_t_disconnect */
  bool (* disconnect) (struct portContext_t *);

  /** @ref portContext_t_open */
  bool (* open) (struct portContext_t *);
  /** @ref portContext_t_jumpZnp */
  bool (* jumpZnp) (struct portContext_t *);
  /** @ref portContext_t_processRxMsg */
  void (* processRxMsg) (struct portContext_t *);

  /** @ref portContext_t_addEndPoint */
  bool (* addEndPoint) (struct portContext_t *, azapEndPoint_t *);
  /** @ref portContext_t_getEndPoint */
  azapEndPoint_t *(* getEndPoint) (struct portContext_t *This, azap_uint8 id);

  /** @ref portContext_t_processZnpMsg */
  void (* processZnpMsg) (struct portContext_t *, azap_uint8, azap_uint8, azap_uint8,
      azap_uint8 *);
  /** @ref portContext_t_processZDOMsgCbIncoming */
  void (* processZDOMsgCbIncoming) (struct portContext_t *, azap_uint8, azap_uint8 *);
  /** @ref portContext_t_parseAndProcessAfDataConfirm */
  void (* parseAndProcessAfDataConfirm) (struct portContext_t *, azap_uint8, azap_uint8 *);
  /** @ref portContext_t_processAfIncomingMsg */
  void (* processAfIncomingMsg) (struct portContext_t *, azap_uint8, azap_uint8 *);

  /** @ref portContext_t_sendZdo */
  bool (* sendZdo) (struct portContext_t *This, AZAP_REQ_TTL_MGMT_MODE ttlMode,
      void *issuerContext, AZAP_REQUEST_TYPE type, bool responseExpected,
      azap_uint16 nwkAddrDest, azap_uint8 remoteEndPoint, azap_uint8 len,
      azap_uint8 *payload);
  /** @ref portContext_t_sendZcl */
  bool (* sendZcl) (struct portContext_t *This, AZAP_REQ_TTL_MGMT_MODE ttlMode,
      void *issuerContext, AZAP_REQUEST_TYPE type, bool responseExpected,
      azap_uint16 nwkAddrDest, azap_uint8 remoteEndPoint, azap_uint8 localEndPoint,
      azap_uint16 clusterId, azap_uint8 clusterDir, azap_uint16 manufacturerCode,
      bool defaultResponse, azap_uint8 cmdCode, azap_uint8 expectedCmdCode,
      azap_uint8 len, azap_uint8 *payload);

  /** @ref portContext_t_free */
  void (*free) (struct portContext_t *);


  /* members */
  /** The port number associated to this context (from 0 to 4). */
  azap_uint8 portNumber;
  /** The set of call back functions for requests completion notifications. */
  t_azap_api_callbacks *pCallbacks;
  /** The global table for polling managed in main thread. */
  void *pMainTbPoll;
  /** The preferred RF channel to be used by the device to form the ZigBee PAN (from 11 to 26). */
  azap_uint32 defaultChannel;
  /** The type of the ZigBee node started on device (0 => ZCoord; 1 => ZRouter). */
  azap_uint8 deviceType;
  /** Sepcifies if the device should reset at start-up or should resume from last run. */
  bool resetHanAtReboot;

  /**
   * The extended address associated to the ZigBee node.
   * If the provided at port context initialization, the AZAP layer will look for a device
   * matching this address among the mounted TTY on the system.
   * If not, this field will be retrieved and populated by the AZAP layer during the ZNP start-up.
   */
  azap_uint8 deviceIeee[IEEE_ADDR_LEN];
  /** The short address associated to the device in the PAN (or network address). */
  azap_uint16 shortAddr;
  /** The extended address associated to the PAN the device belongs to. */
  azap_uint8 panIdExt[IEEE_ADDR_LEN];
  /** The short address associated to the PAN the device belongs to. */
  azap_uint16 panId;
  /** The ZStack version string */
  char zstackVersion[SMALL_BUF_SIZE];

  /** The path to the mount point associated to the TTY for the harware device. */
  char deviceTTYName[SMALL_BUF_SIZE];
  /** The file descriptor associated to the device. */
  int deviceFd;

  /** The buffer controller for received bytes via UART. */
  uartBufControl_t *rx;
  /** The buffer controller for bytes to be transmitted via UART. */
  uartBufControl_t *tx;

  /** When set, handle the pseudo state machine for ZNP start procedure. */
  znpStartInstance_t *startInst;
  /** Flag which indicates that startZnp was invoked and not stopped. */
  bool started;

  /** To manage the hardware device constraints and to manage requests duration/reemissions. */
  requestManager_t *requestManager;

  /** The list of end points */
  struct list_head endPointList;

  /** UART statistics: number of read */
  unsigned long uartNbRead;
  /** UART statistics: read max duration */
  unsigned int uartMaxReadTimeMs;
  /** UART statistics: read total duration */
  unsigned long uartTotalReadTimeMs;
  /** UART statistics: number of write */
  unsigned long uartNbWrite;
  /** UART statistics: write max duration */
  unsigned int uartMaxWriteTimeMs;
  /** UART statistics: write total duration */
  unsigned long uartTotalWriteTimeMs;

} portContext_t;

/* static allocation */
portContext_t portContext_t_create(azap_uint8 port);
void portContext_t_free(portContext_t *This);

/* dynamic allocation */
portContext_t *new_portContext_t(azap_uint8 port);
void portContext_t_newFree(portContext_t *This);

/* implementation methods */
void portContext_t_startZnp(portContext_t *);
bool portContext_t_isStarted(portContext_t *);
void portContext_t_doStartZnpCompletion(portContext_t *, bool);
void portContext_t_stopZnp(portContext_t *);
void portContext_t_dumpStatistics(portContext_t *This, char *out);

bool portContext_t_connect(portContext_t *);
bool portContext_t_disconnect(portContext_t *);

bool portContext_t_open(portContext_t *);
bool portContext_t_jumpZnp(portContext_t *);
void portContext_t_processRxMsg(portContext_t *);

bool portContext_t_addEndPoint(portContext_t *, azapEndPoint_t *);
azapEndPoint_t *portContext_t_getEndPoint(portContext_t *, azap_uint8);

void portContext_t_processZnpMsg(portContext_t *This, azap_uint8 len, azap_uint8 cmd0,
    azap_uint8 cmd1, azap_uint8 *payload);
void portContext_t_processZDOMsgCbIncoming(portContext_t *This, azap_uint8 len,
    azap_uint8 *payload);
void portContext_t_parseAndProcessAfDataConfirm(portContext_t *, azap_uint8 len,
    azap_uint8 *payload);
void portContext_t_processAfIncomingMsg(portContext_t *This, azap_uint8 len,
    azap_uint8 *payload);

bool portContext_t_sendZdo(portContext_t *This, AZAP_REQ_TTL_MGMT_MODE ttlMode,
    void *issuerContext, AZAP_REQUEST_TYPE type, bool responseExpected,
    azap_uint16 nwkAddrDest, azap_uint8 remoteEndPoint, azap_uint8 len,
    azap_uint8 *payload);
bool portContext_t_sendZcl(portContext_t *This, AZAP_REQ_TTL_MGMT_MODE ttlMode,
    void *issuerContext, AZAP_REQUEST_TYPE type, bool responseExpected,
    azap_uint16 nwkAddrDest, azap_uint8 remoteEndPoint, azap_uint8 localEndPoint,
    azap_uint16 clusterId, azap_uint8 clusterDir, azap_uint16 manufacturerCode,
    bool defaultResponse, azap_uint8 cmdCode, azap_uint8 expectedCmdCode,
    azap_uint8 len, azap_uint8 *payload);

/* Other static methods */
bool getTTYMappedName(azap_uint8 *targetedIeee, char *bindingDevice);

#endif


