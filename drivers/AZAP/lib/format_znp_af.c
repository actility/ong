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
//** File : ./lib/format_znp_af.c
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


#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <rtlbase.h>
#include "azap_types.h"
#include "azap_tools.h"
#include "azap_api.h"
#include "azap_api_common.h"
#include "azap_api_af.h"
#include "azap_api_zcl.h"
#include "uart_buf_control.h"
#include "azap_zcl_attr_val.h"
#include "azap_attribute.h"
#include "azap_endpoint.h"
#include "format_znp.h"
#include "format_znp_af.h"

static azap_uint8 g_zclTransactionId = 0;

/**
 * @brief Send a AF_REGISTER ZNP message to register a end point.
 * @param buf the destination buffer controller for ZNP transmission.
 * @param ep the end point to register.
 * @return 1 if message has been bufferized successfully, 0 otherwise.
 */
bool znpSendAfRegister(uartBufControl_t *buf, azapEndPoint_t *ep)
{
  azap_uint8 msg[MAX_ZNP_PAYLOAD_SIZE];
  azap_uint8 i;
  azap_uint8 len = 0;

  if (! ep)
  {
    return false;
  }

  memset(msg, 0, MAX_ZNP_PAYLOAD_SIZE);
  // end point identifier
  msg[len++] = ep->id;
  // application profile identifier
  msg[len++] = LO_UINT16(ep->appProfId);
  msg[len++] = HI_UINT16(ep->appProfId);
  // application device identifier
  msg[len++] = LO_UINT16(ep->appDeviceId);
  msg[len++] = HI_UINT16(ep->appDeviceId);
  // application device version
  msg[len++] = ep->appDevVer;
  // LatencyReq
  msg[len++] = 0;

  // number of server clusters
  msg[len++] = ep->appNumInClusters;
  // list of server cluster identifiers
  for (i = 0; i < ep->appNumInClusters; i++)
  {
    msg[len++] = LO_UINT16(ep->appInClusterList[i]);
    msg[len++] = HI_UINT16(ep->appInClusterList[i]);
  }

  // number of client clusters
  msg[len++] = ep->appNumOutClusters;
  // list of client cluster identifiers
  for (i = 0; i < ep->appNumOutClusters; i++)
  {
    msg[len++] = LO_UINT16(ep->appOutClusterList[i]);
    msg[len++] = HI_UINT16(ep->appOutClusterList[i]);
  }

  return znpSend(buf, MT_AF_REGISTER_SREQ, len, msg);
}

/**
 * @brief Send a AF DATA REQUEST packet over ZNP.
 * @param buf the destination buffer controller for ZNP transmission.
 * @param dstAddr the destination short address of the node to which send this packet.
 * @param dstEp the targeted end point on the destination node.
 * @param srcEp the source end point identifier.
 * @param clusterId the cluster identifier the command addresses.
 * @param transactionId the APS transaction identifier, that will be useful to correlate
 * to the response when it comes back. (by reference)
 * @param bufLen the length of the payload to encapsulate.
 * @param afBufCmd the payload to encapsulate.
 * @return 1 if message has been bufferized successfully, 0 otherwise.
 */
bool znpSendAfDataRequest(uartBufControl_t *buf, azap_uint16 dstAddr, azap_uint8 dstEp,
    azap_uint8 srcEp, azap_uint16 clusterId, azap_uint8 *transactionId, azap_uint16 bufLen,
    azap_uint8 *afBufCmd)
{
  azap_uint8 msg[MAX_ZNP_PAYLOAD_SIZE];
  azap_uint16 i;
  azap_uint8 len = 0;

  memset(msg, 0, MAX_ZNP_PAYLOAD_SIZE);
  // dst address mode, forced to 16bit
  msg[len++] = Addr16Bit;
  // dst address
  msg[len++] = LO_UINT16(dstAddr);
  msg[len++] = HI_UINT16(dstAddr);
  // padding for remaining spadce on the placeholder "dst address"
  for (i = 0; i < 6; i++)
  {
    msg[len++] = 0;
  }
  // dst end point identifier
  msg[len++] = dstEp;
  // no inter-PAN for now
  msg[len++] = 0;
  msg[len++] = 0;
  // src end point identifier
  msg[len++] = srcEp;
  // cluster identifier
  msg[len++] = LO_UINT16(clusterId);
  msg[len++] = HI_UINT16(clusterId);
  // transaction ID; can be useful to confirm that data was sent correctly (on
  // AF_DATA_CONFIRM receipt)
  msg[len++] = *transactionId;
  (*transactionId)++;
  // cluster options - not supported for now
  msg[len++] = AF_TX_OPTIONS_NONE;
  // radius
  msg[len++] = AF_DEFAULT_RADIUS;
  // and the encapsulated AF command
  msg[len++] = LO_UINT16(bufLen);
  msg[len++] = HI_UINT16(bufLen);
  if ( len + bufLen < MAX_ZNP_PAYLOAD_SIZE)
  {
    memcpy(&(msg[len]), afBufCmd, bufLen);
    len += bufLen;
    return znpSend(buf, MT_AF_DATA_EXT_SREQ, len, msg);
  }

  return 0;
}

/**
 * @brief Format and send a ZCL command over ZNP interface.
 * @param buf the destination buffer controller for ZNP transmission.
 * @param dstAddr the destination short address of the node to which send this packet.
 * @param dstEp the targeted end point on the destination node.
 * @param srcEp the source end point identifier.
 * @param zclCmd the ZCL command code
 * @param specific specify if the ZCL command is a ZCL profile command (when specific set to 0)
 * or a specific command (when specific set to 1)
 * @param clusterId the cluster identifier the command addresses.
 * @param direction the command direction (from server to client or from client to server)
 * @param manuCode the manufacturer code if manufacturer specific command or attributes.
 * When 0x0000 provided, the command is not manufacturer specific.
 * @param bufLen the length of the payload to encapsulate.
 * @param afBufCmd the payload to encapsulate.
 * @param transactionId the APS transaction identifier, that will be useful to correlate
 * to the response when it comes back. (by reference)
 * @return 1 if message has been bufferized successfully, 0 otherwise.
 */
bool znpSendZclCommand(uartBufControl_t *buf, azap_uint16 dstAddr, azap_uint8 dstEp,
    azap_uint8 srcEp, azap_uint8 zclCmd, bool specific, azap_uint16 clusterId, azap_uint8 direction,
    azap_uint16 manuCode, azap_uint16 bufLen, azap_uint8 *afBufCmd, azap_uint8 *transID)
{
  azap_uint8 msg[MAX_ZNP_PAYLOAD_SIZE];
  azap_uint16 len = 0;

  memset(msg, 0, MAX_ZNP_PAYLOAD_SIZE);

  // The ZCL header
  msg[len++] = 0;
  // frame type on bit 0-1
  if (specific)
  {
    msg[0] |= (ZCL_FRAME_TYPE_SPECIFIC_CMD & 0x03);
  }
  else
  {
    msg[0] |= (ZCL_FRAME_TYPE_PROFILE_CMD & 0x03);
  }
  if (0x0000 != manuCode)
  {
    msg[0] |= ( 0x01 << 2 );  // manufacturer specific on bit 2
  }
  msg[0] |= ( (direction & 0x01) << 3); // the command direction on bit 3
  // msg[0] |= ( (disableDefaultRsp & 0x01) << 4); //disable default response on bit 4
  // bits 5-7 are reserved

  // The manufacturer code if any specific
  if (0x0000 != manuCode)
  {
    msg[len++] = LO_UINT16(manuCode);
    msg[len++] = HI_UINT16(manuCode);
  }

  // the transaction sequence number, not supported for now.
  msg[len++] = 0;

  // the command code
  msg[len++] = zclCmd;

  // the encapsulated payload
  if (len + bufLen < MAX_ZNP_PAYLOAD_SIZE)
  {
    if ((afBufCmd) && (bufLen > 0))
    {
      memcpy(&(msg[len]), afBufCmd, bufLen);
      len += bufLen;
    }

    *transID = g_zclTransactionId;
    return znpSendAfDataRequest(buf, dstAddr, dstEp, srcEp, clusterId, &g_zclTransactionId,
        len, msg);
  }

  return 0;
}

/**
 * @brief Fill in the hdr parameter with the next read bytes in payload.
 * @param payload the payload of the received message.
 * @param len the remaining number of bytes in the provided payload.
 * @param hdr the ZCL header structure to fill in.
 */
void parseZCLHeader(azap_byte **payload, azap_uint8 *len, azapZclFrameHeader_t *hdr)
{
  azap_uint8 readByte = parseUint8AndShift(payload, len);
  memset(hdr, 0, sizeof(azapZclFrameHeader_t));

  // Parse the ZCL header - frame control
  hdr->fc.type = readByte & 0x03;
  hdr->fc.manuSpecific = ((readByte & 0x04) >> 2);
  hdr->fc.direction = ((readByte & 0x08) >> 3);
  hdr->fc.disableDefaultRsp = ((readByte & 0x10) >> 4);
  hdr->fc.reserved = ((readByte & 0xE0) >> 5);

  if (hdr->fc.manuSpecific)
  {
    hdr->manuCode = parseUint16AndShift(payload, len);
  }
  else
  {
    hdr->manuCode = 0;
  }
  hdr->transSeqNum = parseUint8AndShift(payload, len);
  hdr->commandId = parseUint8AndShift(payload, len);
}

/**
 * @brief Parse the data as ZCL payload for ZCL Read rsp command.
 * WARNING the return structure must be deallocated by the issuer.
 * @param len the size of the ZCL payload.
 * @param data the ZCL payload.
 * @return parsedData the structure filled in with parsed information.
 */
azapZclReadRspCommand_t *parseZCLReadCmdRsp(azap_uint8 len, azap_byte *data)
{
  azapZclReadRspCommand_t *parsedData = NULL;

  // We first need to determine the number of encoded attributes
  azap_uint8 pos = 0;
  azap_uint8 size;
  azap_uint8 numAttr = 0;
  while (pos < len)
  {
    pos += 2; // attrId
    if (ZCL_STATUS_SUCCESS == data[pos] )
    {
      size = azapGetZclDataLength(data[pos + 1], &data[pos + 2]);
      pos++;  // for data type
      if ((size > len) || (size + pos + 1 /* for status */ > len))
      {
        RTL_TRDBG(TRACE_ERROR, "parseZCLReadCmdRsp: abnormal parsing situation detected "
            "(numAttr:%d) (size:%d) (len:%d) (pos:%d)\n", numAttr, size, len, pos);
        return NULL;
      }
      pos += size;
    }
    pos++;    // for status byte
    numAttr++;
  }

  parsedData = malloc(sizeof(azapZclReadRspCommand_t) + (numAttr * sizeof(azapZclReadRspAttr_t)));
  if (parsedData)
  {
    parsedData->numAttr = 0;
    while ((len > 0) && (numAttr > parsedData->numAttr))
    {
      parsedData->attrList[parsedData->numAttr].attrId = parseUint16AndShift(&data, &len);
      parsedData->attrList[parsedData->numAttr].status = parseUint8AndShift(&data, &len);
      if (ZCL_STATUS_SUCCESS == parsedData->attrList[parsedData->numAttr].status )
      {
        parsedData->attrList[parsedData->numAttr].dataType = parseUint8AndShift(&data, &len);
        size = azapGetZclDataLength(parsedData->attrList[parsedData->numAttr].dataType, data);
        // no copy, "data" point at the current position of read data
        parsedData->attrList[parsedData->numAttr].data = data;
        data += size;
        len -= size;
      }
      parsedData->numAttr ++;
    }
  }

  return parsedData;
}

/**
 * @brief Parse the data as ZCL payload for ZCL Write rsp command.
 * WARNING the return structure must be deallocated by the issuer.
 * @param len the size of the ZCL payload.
 * @param data the ZCL payload.
 * @return parsedData the structure filled in with parsed information.
 */
azapZclWriteRspCommand_t *parseZCLWriteCmdRsp(azap_uint8 len, azap_byte *data)
{
  azapZclWriteRspCommand_t *parsedData;

  if (1 == len)
  {
    // write operation succeeds for all attributes
    parsedData = malloc(sizeof(azapZclWriteRspCommand_t) + sizeof(azapZclWriteRspAttr_t));
    if (parsedData)
    {
      parsedData->numAttr = 0;
      parsedData->attrList[parsedData->numAttr].status = parseUint8AndShift(&data, &len);
      parsedData->numAttr++;
    }
  }
  else
  {
    parsedData = malloc(sizeof(azapZclWriteRspCommand_t) + len);
    if (parsedData)
    {
      parsedData->numAttr = 0;
      while (len >= 3) /* 3 = sizeof(uint8) + sizeof(uint16) */
      {
        parsedData->attrList[parsedData->numAttr].status = parseUint8AndShift(&data, &len);
        parsedData->attrList[parsedData->numAttr].attrId = parseUint16AndShift(&data, &len);
        parsedData->numAttr++;
      }
    }
  }

  return parsedData;
}

/**
 * @brief Parse the data as ZCL payload for ZCL ConfigureReport rsp command.
 * WARNING the return structure must be deallocated by the issuer.
 * @param len the size of the ZCL payload.
 * @param data the ZCL payload.
 * @return parsedData the structure filled in with parsed information.
 */
azapZclCfgReportRspCommand_t *parseZCLConfigureReportCmdRsp(azap_uint8 len, azap_byte *data)
{
  // number attributes equals payload length divided by 4 (i.e. 1+1+2 for status, direction
  // and attribute identifier)
  azapZclCfgReportRspCommand_t *parsedData = malloc(sizeof(azapZclCfgReportRspCommand_t) +
      (((len / 4) + 1) * sizeof(azapZclCfgReportRspAttr_t)));


  if (parsedData)
  {
    parsedData->numAttr = 0;
    while (len >= 4) /* 4 = sizeof(uint8) + sizeof(uint8) + sizeof(uint16) */
    {
      parsedData->attrList[parsedData->numAttr].status = parseUint8AndShift(&data, &len);
      parsedData->attrList[parsedData->numAttr].direction = parseUint8AndShift(&data, &len);
      parsedData->attrList[parsedData->numAttr].attrId = parseUint16AndShift(&data, &len);
      parsedData->numAttr++;
    }
  }

  return parsedData;
}

/**
 * @brief Parse the data as ZCL payload for ZCL Report command.
 * WARNING the return structure must be deallocated by the issuer.
 * @param len the size of the ZCL payload.
 * @param data the ZCL payload.
 * @return parsedData the structure filled in with parsed information.
 */
azapZclReportCommand_t *parseZCLReportCmd(azap_uint8 len, azap_byte *data)
{
  azapZclReportCommand_t *parsedData = NULL;
  // We first need to determine the number of encoded attributes
  azap_uint8 pos = 0;
  azap_uint8 size = 0;
  azap_uint8 numAttr = 0;
  while (pos < len)
  {
    pos += 2; // attrId
    size = azapGetZclDataLength(data[pos], &data[pos + 1]);
    pos += 1; // dataType
    if ((size > len) || (size + pos > len))
    {
      RTL_TRDBG(TRACE_ERROR, "parseZCLReportCmd: abnormal parsing situation detected "
          "(numAttr:%d) (size:%d) (len:%d) (pos:%d)\n", numAttr, size, len, pos);
      return NULL;
    }
    pos += size; // data
    numAttr++;
  }

  parsedData = malloc(sizeof(azapZclReportCommand_t) + (numAttr * sizeof(azapZclReportAttr_t)));
  if (parsedData)
  {
    parsedData->numAttr = 0;
    while ((len > 0) && (numAttr > parsedData->numAttr))
    {
      parsedData->attrList[parsedData->numAttr].attrId = parseUint16AndShift(&data, &len);
      parsedData->attrList[parsedData->numAttr].dataType = parseUint8AndShift(&data, &len);
      size = azapGetZclDataLength(parsedData->attrList[parsedData->numAttr].dataType, data);
      // no copy, "data" point at the current position of read data
      parsedData->attrList[parsedData->numAttr].attrData = data;
      data += size;
      len -= size;
      parsedData->numAttr ++;
    }
    pos = 0;

  }

  return parsedData;
}

/**
 * @brief Parse the data as ZCL payload for ZCL ReadReportConfig rsp command.
 * WARNING the return structure must be deallocated by the issuer.
 * @param len the size of the ZCL payload.
 * @param data the ZCL payload.
 * @return parsedData the structure filled in with parsed information.
 */
azapZclReadReportCfgRspCommand_t *parseZCLReadReportConfigCmdRsp(azap_uint8 len, azap_byte *data)
{
  azapZclReadReportCfgRspCommand_t *parsedData = NULL;
  // We first need to determine the number of encoded attributes
  azap_uint8 pos = 0;
  azap_uint8 numAttr = 0;
  azap_uint8 status = 0;
  azap_uint8 direction = 0;
  azap_uint8 type = 0;
  azap_uint8 size;
  while (pos < len)
  {
    status = data[pos];
    direction = data[pos + 1];
    pos += 4; // shift size of status + direction + attrId
    if ( ZCL_STATUS_SUCCESS == status )
    {
      if ( ZCL_SEND_ATTR_REPORTS  == direction )
      {
        type = data[pos];
        pos += 5; // shift size of dataType + MinimalReportInterval + MaximalReportInterval
        if (azapZclIsAnalogDataType(type))
        {
          size = azapGetZclDataLength(type, NULL);
          if ((size > len) || (size + pos > len))
          {
            RTL_TRDBG(TRACE_ERROR, "parseZCLReadReportConfigCmdRsp: abnormal parsing situation "
                "detected (numAttr:%d) (size:%d) (len:%d) (pos:%d)\n", numAttr, size, len, pos);
            return NULL;
          }
          pos += size;
        }
      }
      else
      {
        pos += 2; // shift size of TimeoutPeriod
      }
    }
    numAttr++;
  }

  parsedData = malloc(sizeof(azapZclReadReportCfgRspCommand_t) +
      (numAttr * sizeof(azapZclReportCfgRspAttr_t)));
  if (parsedData)
  {
    parsedData->numAttr = 0;
    while ((len > 0) && (numAttr > parsedData->numAttr))
    {
      parsedData->attrList[parsedData->numAttr].status = parseUint8AndShift(&data, &len);
      parsedData->attrList[parsedData->numAttr].direction = parseUint8AndShift(&data, &len);
      parsedData->attrList[parsedData->numAttr].attrId = parseUint16AndShift(&data, &len);
      if ( ZCL_STATUS_SUCCESS == parsedData->attrList[parsedData->numAttr].status )
      {
        if ( ZCL_SEND_ATTR_REPORTS == parsedData->attrList[parsedData->numAttr].direction )
        {
          parsedData->attrList[parsedData->numAttr].timeoutPeriod = 0;
          parsedData->attrList[parsedData->numAttr].dataType = parseUint8AndShift(&data, &len);
          parsedData->attrList[parsedData->numAttr].minReportInt = parseUint16AndShift(&data, &len);
          parsedData->attrList[parsedData->numAttr].maxReportInt = parseUint16AndShift(&data, &len);
          parsedData->attrList[parsedData->numAttr].reportableChange = data;

          // no copy, "data" point at the current position of read data
          size = azapGetZclDataLength(parsedData->attrList[parsedData->numAttr].dataType, data);
          data += size;
          len -= size;
        }
        else
        {
          parsedData->attrList[parsedData->numAttr].dataType = 0;
          parsedData->attrList[parsedData->numAttr].minReportInt = 0;
          parsedData->attrList[parsedData->numAttr].maxReportInt = 0;
          parsedData->attrList[parsedData->numAttr].reportableChange = NULL;
          parsedData->attrList[parsedData->numAttr].timeoutPeriod = parseUint16AndShift(&data, &len);
        }
      }
      parsedData->numAttr ++;
    }
    pos = 0;

  }

  return parsedData;
}


/**
 * @brief Parse the data as ZCL payload for ZCL Discover rsp command.
 * WARNING the return structure must be deallocated by the issuer.
 * @param len the size of the ZCL payload.
 * @param data the ZCL payload.
 * @return parsedData the structure filled in with parsed information.
 */
azapZclDiscoverRspCommand_t *parseZCLDiscoverRspCmd(azap_uint8 len, azap_byte *data)
{
  // number attributes equals payload length divided by 3 (i.e. 2+1 for attribute
  // identifier and data type)
  azapZclDiscoverRspCommand_t *parsedData = malloc(sizeof(azapZclDiscoverRspCommand_t) +
      (((len / 3) + 1) * sizeof(azapZclDiscoverAttr_t)));

  if (parsedData)
  {
    parsedData->discComplete = parseUint8AndShift(&data, &len);
    parsedData->numAttr = 0;
    while (len >= 3) /* 3 = sizeof(uint16) + sizeof(uint8) */
    {
      parsedData->attrList[parsedData->numAttr].attrId = parseUint16AndShift(&data, &len);
      parsedData->attrList[parsedData->numAttr].dataType = parseUint8AndShift(&data, &len);
      parsedData->numAttr++;
    }
  }

  return parsedData;
}

/**
 * @brief Parse the data as ZCL payload for ZCL ConfigureReport command.
 * WARNING the return structure must be deallocated by the issuer.
 * @param len the size of the ZCL payload.
 * @param data the ZCL payload.
 * @return parsedData the structure filled in with parsed information.
 */
azapZclCfgReportCommand_t *parseZCLConfigureReportCmd(azap_uint8 len, azap_byte *data)
{
  azapZclCfgReportCommand_t *parsedData = NULL;
  // We first need to determine the number of encoded attributes
  azap_uint8 pos = 0;
  azap_uint8 numAttr = 0;
  azap_uint8 direction = 0;
  azap_uint8 type = 0;
  azap_uint8 size;
  while (pos < len)
  {
    direction = data[pos];
    pos += 3; // shift size of direction + attrId
    if ( ZCL_SEND_ATTR_REPORTS  == direction )
    {
      type = data[pos];
      pos += 5; // shift size of dataType + MinimalReportInterval + MaximalReportInterval
      if (azapZclIsAnalogDataType(type))
      {
        size = azapGetZclDataLength(type, NULL);
        if ((size > len) || (size + pos > len))
        {
          RTL_TRDBG(TRACE_ERROR, "parseZCLConfigureReportCmd: abnormal parsing situation "
              "detected (numAttr:%d) (size:%d) (len:%d) (pos:%d)\n", numAttr, size, len, pos);
          return NULL;
        }
        pos += size;
      }
    }
    else
    {
      pos += 2; // shift size of TimeoutPeriod
    }
    numAttr++;
  }

  parsedData = malloc(sizeof(azapZclCfgReportCommand_t) +
      (numAttr * sizeof(azapZclCfgReportAttr_t)));
  if (parsedData)
  {
    parsedData->numAttr = 0;
    while ((len > 0) && (numAttr > parsedData->numAttr))
    {
      parsedData->attrList[parsedData->numAttr].direction = parseUint8AndShift(&data, &len);
      parsedData->attrList[parsedData->numAttr].attrId = parseUint16AndShift(&data, &len);
      if ( ZCL_SEND_ATTR_REPORTS == parsedData->attrList[parsedData->numAttr].direction )
      {
        parsedData->attrList[parsedData->numAttr].timeoutPeriod = 0;
        parsedData->attrList[parsedData->numAttr].dataType = parseUint8AndShift(&data, &len);
        parsedData->attrList[parsedData->numAttr].minReportInt = parseUint16AndShift(&data, &len);
        parsedData->attrList[parsedData->numAttr].maxReportInt = parseUint16AndShift(&data, &len);
        parsedData->attrList[parsedData->numAttr].reportableChange = data;

        // no copy, "data" point at the current position of read data
        size = azapGetZclDataLength(parsedData->attrList[parsedData->numAttr].dataType, data);
        data += size;
        len -= size;
      }
      else
      {
        parsedData->attrList[parsedData->numAttr].dataType = 0;
        parsedData->attrList[parsedData->numAttr].minReportInt = 0;
        parsedData->attrList[parsedData->numAttr].maxReportInt = 0;
        parsedData->attrList[parsedData->numAttr].reportableChange = NULL;
        parsedData->attrList[parsedData->numAttr].timeoutPeriod = parseUint16AndShift(&data, &len);
      }
      parsedData->numAttr ++;
    }
    pos = 0;

  }

  return parsedData;
}




