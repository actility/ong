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

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <rtlbase.h>
#include <rtlimsg.h>

#ifndef WMBUS_TEST
#include "wmbus.h"
#include "cmnstruct.h"
#include "sensor.h"
#endif

#include "obix_normalizer.h"

//// Import from linux kernel (asm-generic)
//#define get_unaligned(ptr) ({ __typeof__(*(ptr)) __tmp; memcpy(&__tmp, (ptr), sizeof(__tmp)); __tmp; })

enum
{
  READ_DIF,
  READ_DIFE,
  READ_VIF,
  READ_VIFE,
  READ_DATA
};


void
#ifndef WMBUS_TEST
wmbusParser(char *data, int len, t_cmn_sensor *dev)
#else
wmbusParser(char *data, int len)
#endif
{
  int i, j, n, lvar /*, tmp*/;
  int state;
  unsigned char c;
  struct
  {
    int storage;
    int function;
    int coding;
    int tarif;
    int subunit;
  } dib;
  union
  {
    unsigned char c[8];
    long long int i;
  } bigInt;
  int max_dife = 10;
  unsigned long long int vib;
  float value = 0.0f;
  char valBuffer[256];

  const unsigned char bcdDecodeTable[] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'b', 'C', ' ', 'E', '-'};

  memset((void *)&dib, 0, sizeof(dib));

  state = READ_DIF;

  for (i = 0; i < len; i++)
  {
    c = data[i];

    //  printf("c = %hhx, state = %d\n", c, state);

    switch (state)
    {
      case READ_DIF:
        // Reset parser data
        dib.storage = 0;
        dib.function = 0;
        dib.coding = 0;
        dib.tarif = 0;
        dib.subunit = 0;
        vib = 0;
        valBuffer[0] = '\0';

        // start parsing
        dib.coding = c & 0x0F;
        dib.function = (c & 0x30) >> 4;
        dib.storage = (c & 0x40) ? 1 : 0;

        // Check for dif extension
        if (c & 0x80)
        {
          state = READ_DIFE;
        }
        else
        {
          state = READ_VIF;
        }
        break;

      case READ_DIFE:
        dib.storage |= (((int)c & 0x0F) << (4 * (10 - max_dife) + 1));
        dib.tarif |= (((int)c & 0x30) >> 4) << (2 * (10 - max_dife));
        dib.subunit |= ((c & 0x40) ? 1 : 0) << (10 - max_dife);

        if (c & 0x80)
        {
          max_dife--;
          if (max_dife == 0)
          {
            // Too many DIFE block, report seems invalid
            return;
          }
          state = READ_DIFE;
        }
        else
        {
          state = READ_VIF;
        }
        break;

      case READ_VIF:
        vib = c;

        // Check for vife extension
        if (c & 0x80)
        {
          if ((c == 0xFB) || (c == 0xFD))
          {
            state = READ_VIFE;
          }
          else
          {
            // VIF not supported
            return;
          }
        }
        else
        {
          state = READ_DATA;
        }
        break;

      case READ_VIFE:
        // VIFE 0xFB & 0xFD have only one byte size
        vib = (vib << 8) + c;
        state = READ_DATA;
        break;

      case READ_DATA:
        switch (dib.coding)
        {
          default:
          case 0:
            // No data
            valBuffer[0] = '\0';
            break;

          case 1:
            // int8_t
            value = c;
            obixNormalizer(&vib, &value);
            snprintf(valBuffer, 256, "%f", value);
            break;

          case 2:
            // int16_t
          {
//#ifndef WMBUS_TEST
//            RTL_TRDBG(TRACE_ERROR, "*** int16_t int len=%d, i=%d ***\n", len, i);
//#endif
//            int16_t *pt = (int16_t *)(data + i);
//            value = get_unaligned(pt);
            int16_t val;
            memcpy(&val, data + i, sizeof(int16_t));
            value = (float)val;
          }
          obixNormalizer(&vib, &value);
          snprintf(valBuffer, 256, "%f", value);
          i += 1;
          break;

          case 3:
            // int24_t
            value = (*(unsigned char *)(data + i)) + 256 * ((*(unsigned char *)(data + i + 1)) + 256 * (*(unsigned char *)(data + i + 2)));
            obixNormalizer(&vib, &value);
            snprintf(valBuffer, 256, "%f", value);
            i += 2;
            break;

          case 4:
            // int32_t
          {
//#ifndef WMBUS_TEST
//            RTL_TRDBG(TRACE_ERROR, "*** int32_t int len=%d, i=%d ***\n", len, i);
//#endif
//            int32_t *pt = (int32_t *)(data + i);
//            value = get_unaligned(pt);
            int32_t val;
            memcpy(&val, data + i, sizeof(int32_t));
            value = (float)val;
          }
          obixNormalizer(&vib, &value);
          snprintf(valBuffer, 256, "%f", value);
          i += 3;
          break;

          case 5:
            // float 32b
          {
//#ifndef WMBUS_TEST
//            RTL_TRDBG(TRACE_ERROR, "*** float len=%d, i=%d ***\n", len, i);
//#endif
//            float *pt = (float *)(data + i);
//            value = get_unaligned(pt);
            memcpy(&value, data + i, sizeof(float));
          }
          obixNormalizer(&vib, &value);
          snprintf(valBuffer, 256, "%f", value);
          i += 3;
          break;

          case 6:
            // int48_t
            bigInt.c[0] = *(unsigned char *)(data + i + 0);
            bigInt.c[1] = *(unsigned char *)(data + i + 1);
            bigInt.c[2] = *(unsigned char *)(data + i + 2);
            bigInt.c[3] = *(unsigned char *)(data + i + 3);
            bigInt.c[4] = *(unsigned char *)(data + i + 4);
            bigInt.c[5] = *(unsigned char *)(data + i + 5);
            bigInt.c[6] = 0;
            bigInt.c[7] = 0;

            // Move sign bit
            if (bigInt.c[5] & 0x80)
            {
              bigInt.c[7] = 0xFF;
              bigInt.c[6] = 0xFF;
            }

            value = (float)bigInt.i;
            //snprintf(valBuffer, 256, "%lld", bigInt.i);
            i += 5;
            break;

          case 7:
            // int64_t
          {
//#ifndef WMBUS_TEST
//            RTL_TRDBG(TRACE_ERROR, "*** long long int len=%d, i=%d ***\n", len, i);
//#endif
//            long long int *pt = (long long int *)(data + i);
//            value = get_unaligned(pt);
            long long int val;
            memcpy(&val, data + i, sizeof(long long int));
            value = (float)val;
          }
          obixNormalizer(&vib, &value);
          snprintf(valBuffer, 256, "%f", value);
          i += 7;
          break;

          // BCD 2, 4, 6, 8, 10 digit
          case 9:
          case 10:
          case 11:
          case 12:
          case 14:
            n = dib.coding - 8;
            for (j = 0; j < n; j++)
            {
              c = *(unsigned char *)(data + i + j);
              valBuffer[2 * j] = bcdDecodeTable[(c & 0xF0)  >> 4];
              valBuffer[2 * j + 1] = bcdDecodeTable[(c & 0x0F) ];
            }
            valBuffer[2 * j] = '\0';
            i += (n - 1);

          case 8:
            // Selection for readout
            break;

          case 13:
            // Variable lentgh
            /*tmp = */
            lvar = c;

            if ((lvar >= 0x00) && (lvar <= 0xBF))
            {
              // Text
              memcpy(valBuffer, data + i + 1, lvar);
              *(valBuffer + lvar) = '\0';
              i += lvar;
            }

            else if ((lvar >= 0xC0) && (lvar <= 0xC9))
            {
              // Positive BCD : (Lvar - 0xC0) * 2 digit   -> 0 to 18 digits
              n = lvar - 0xC0;
              for (j = 0; j < n; j++)
              {
                c = *(unsigned char *)(data + i + 1 + j);
                valBuffer[2 * j] = bcdDecodeTable[(c & 0xF0)  >> 4];
                valBuffer[2 * j + 1] = bcdDecodeTable[(c & 0x0F) ];
              }
              valBuffer[2 * j] = '\0';
              i += n;
            }

            else if ((lvar >= 0xD0) && (lvar <= 0xD9))
            {
              // Negative BCD : (Lvar - 0xD0) * 2 digit   -> 0 to 18 digits
              n = lvar - 0xD0;
              for (j = 0; j < n; j++)
              {
                c = *(unsigned char *)(data + i + 1 + j);
                valBuffer[2 * j + 1] = bcdDecodeTable[(c & 0xF0)  >> 4];
                valBuffer[2 * j + 2] = bcdDecodeTable[(c & 0x0F) ];
              }
              valBuffer[0] = '-';
              valBuffer[2 * j + 1] = '\0';
              i += n;
            }

            else if ((lvar >= 0xE0) && (lvar <= 0xEF))
            {
              // Not supported
              return;
            }

            else if (lvar == 0xF8)
            {
              // Float IEEE
              {
//#ifndef WMBUS_TEST
//                RTL_TRDBG(TRACE_ERROR, "*** float2 int len=%d, i=%d ***\n", len, i);
//#endif
//                float *pt = (float *)(data + i + 1);
//                value = get_unaligned(pt);
                memcpy(&value, data + i + 1, sizeof(float));
              }
              obixNormalizer(&vib, &value);
              snprintf(valBuffer, 256, "%f", value);
              i += 3;
            }

            else
            {
              // Reserved !
            }
            break;

          case 15:
            // Special function
            break;
        }

        if (valBuffer[0] != '\0')
        {
#ifndef WMBUS_TEST
          if (CanReport(vib, 0, 0, dev))
          {
            RTL_TRDBG(TRACE_INFO, "Report Data (vib = x%llx)\n", vib);
            DiaReportValue(dev->cmn_num, dev->cmn_serial, WMBUS_REPORT_APP_CODE, vib, 0, 0, "%s", valBuffer);
          }
#else
          printf("dib.storage\t= x%X\ndib.function\t= x%X\ndib.coding\t= x%X\ndib.tarif\t= x%X\ndib.subunit\t= x%X\nvib\t\t= x%llX\nvalue\t\t= %s\n\n",
              dib.storage, dib.function, dib.coding, dib.tarif, dib.subunit, vib, valBuffer);
#endif
        }

        state = READ_DIF;

        break;
    }
  }
}
