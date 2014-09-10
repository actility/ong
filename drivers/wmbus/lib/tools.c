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


char *
getDeviceTypeFromCode (unsigned char c)
{

  switch (c)
  {
    case 0x00:
      return "Other";
    case 0x01:
      return "Oil";
    case 0x02:
      return "Electricity";
    case 0x03:
      return "Gas";
    case 0x04:
      return "Heat";
    case 0x05:
      return "Steam";
    case 0x06:
      return "Warm Water";
    case 0x07:
      return "Water";
    case 0x08:
      return "Heat Cost Allocator";
    case 0x09:
      return "Compressed Air";
    case 0x0A:
      return "Cooling load meter";
    case 0x0B:
      return "Cooling load meter";
    case 0x0C:
      return "Heat";
    case 0x0D:
      return "Heat / Cooling load meter";
    case 0x0E:
      return "Bus / System component";
    case 0x0F:
      return "Unknown medium";
    case 0x15:
      return "Hot Water";
    case 0x16:
      return "Cold Water";
    case 0x17:
      return "Dual register Water meter";
    case 0x18:
      return "Pressure";
    case 0x19:
      return "A/D Converter";

    default:
      return "Reserved";
  }

  if (c >= 0x1A || (c >=  0x10 && c <=  0x14))
  {
    return "Reserved";
  }



}
