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

#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>

#include <rtlbase.h>
#include <rtlimsg.h>

#include "wmbus.h"
#include "preprocessor.h"
#include "sensor.h"
#include "processor_parser.h"

void wmbusProcessor(unsigned char *data, int len)
{
  union
  {
    char data[WMBUS_RX_BUFFER_SIZE];
    wmBusHeader_t headers;
  } wMBusData;
  unsigned char ci = data[sizeof(wmBusHeader_t)];

  // Ensure that not is proprietary data
  if (ci != WMBUS_REPORT_APP_CODE)
  {
    char convertedData[WMBUS_RX_BUFFER_SIZE];

    RTL_TRDBG(TRACE_INFO, "Handling specific data (ci = %hhx)\n", ci);

    len = wmbusManufacturerPreprocessor((char *)data, len, convertedData, WMBUS_RX_BUFFER_SIZE);
    if (len < 0)
    {
      RTL_TRDBG(TRACE_ERROR, "Convertion of proprietary data failed\n");

      // Data are dropped
      return;
    }

    memcpy(wMBusData.data, convertedData,
        (WMBUS_RX_BUFFER_SIZE <= len) ? WMBUS_RX_BUFFER_SIZE : len);
  }
  else
  {
    memcpy(wMBusData.data, data,
        (WMBUS_RX_BUFFER_SIZE <= len) ? WMBUS_RX_BUFFER_SIZE : len);
  }

  // Ensure that the device exist
  unsigned long long int ieee = HEADER_TO_IEEE((&(wMBusData.headers)));
  t_cmn_sensor *dev = SensorFindByIeeeNumber(ieee);
  if (dev == NULL)
  {
    DoSensorDetected(ieee);

    // We drop data until the device is not created
    return;
  }

  //DUMP_MEMORY(wMBusData, len);

  // Read Call modeling ?
  wmbusParser (wMBusData.data + sizeof(wmBusHeader_t) + 1, len - sizeof(wmBusHeader_t) - 1, dev);
}

