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

char manufacturerCodeBuffer[4] = {'\0', '\0', '\0', '\0'};

#define ASCII_OFFSET    0x40
#define CHARACTER_MASK  0x20

char *
wmbusManufacturerCodeToString(short code)
{
  manufacturerCodeBuffer[0] = (code / CHARACTER_MASK / CHARACTER_MASK) + ASCII_OFFSET;
  manufacturerCodeBuffer[1] = (code / CHARACTER_MASK) % CHARACTER_MASK + ASCII_OFFSET;
  manufacturerCodeBuffer[2] = (code % CHARACTER_MASK) + ASCII_OFFSET;
  manufacturerCodeBuffer[3] = '\0';

  return manufacturerCodeBuffer;
}

short
wmbusManufacturerStringToCode(char *code)
{
  return (code[0] - ASCII_OFFSET) * CHARACTER_MASK * CHARACTER_MASK +
      (code[1] - ASCII_OFFSET) * CHARACTER_MASK +
      (code[2] - ASCII_OFFSET);
}
