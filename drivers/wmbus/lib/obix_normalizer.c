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

#include <math.h>
#include <stdio.h>


static inline
float
scale(float value, float range, float offset)
{
  if (range != offset)
  {
    if ((range - offset) > 0)
    {
      return value * pow(10, range - offset);
    }
    else
    {
      return value / pow(10, offset - range);
    }
  }

  return value;
}
#define SCALE(base, mask, offset) \
if ((*vib & ~(mask)) == (base)) {                       \
    *value = scale(*value, (*vib & (mask)), (offset));  \
    *vib = (base) + (offset);                           \
    return;                                             \
}

static inline
float
scale_time(float value, int code)
{
  switch (code)
  {
    default:
    case 0: // seconds
      return value;

    case 1: // min
      return 60.0f * value;

    case 2: // hour
      return 60.0f * 60.0f * value;

    case 3: // day
      return 24.0f * 60.0f * 60.0f * value;
  }
}
#define SCALE_TIME(base) \
if ((*vib & ~0x03) == (base)) {                         \
    *value = scale_time(*value, (*vib & 0x03));         \
    *vib = (base);                                      \
    return;                                             \
}


void
obixNormalizer(unsigned long long int *vib, float *value)
{
  /*
   *  Primary VIF-Code
   */
  SCALE(0x00, 0x07, 3);   // Energy               Wh      0x00 - 0x07
  SCALE(0x08, 0x07, 0);   // Energy               J       0x08 - 0x0F
  SCALE(0x10, 0x07, 6);   // Volume               m3      0x10 - 0x17
  SCALE(0x18, 0x07, 3);   // Mass                 Kg      0x18 - 0x1F
  SCALE_TIME(0x20);       // On Time              s       0x20 - 0x23
  SCALE_TIME(0x24);       // Operation Time       s       0x24 - 0x27
  SCALE(0x28, 0x07, 3);   // Power                W       0x28 - 0x2F
  SCALE(0x30, 0x07, 0);   // Power                J/h     0x30 - 0x37
  SCALE(0x38, 0x07, 6);   // Volume Flow          m3/h    0x38 - 0x3F
  SCALE(0x40, 0x07, 7);   // Volume Flow          m3/min  0x40 - 0x47
  SCALE(0x48, 0x07, 9);   // Volume Flow          m3/s    0x48 - 0x4F
  SCALE(0x50, 0x07, 3);   // Mass Flow            Kg/h    0x50 - 0x57
  SCALE(0x58, 0x03, 3);   // Flow Temperature     °C      0x58 - 0x5B
  SCALE(0x5C, 0x03, 3);   // Return Temperature   °C      0x5C - 0x5F
  SCALE(0x60, 0x03, 3);   // Temperature Diff     °C      0x60 - 0x63
  SCALE(0x64, 0x03, 3);   // External Temperature °C      0x64 - 0x67
  SCALE(0x68, 0x03, 3);   // Pressure             bar     0x68 - 0x6B
  // TODO Date
  // TODO DateTime
  // TODO Extended Date
  // TODO Extended DateTime
  SCALE_TIME(0x70);       // Average duration     s       0x70 - 0x73
  SCALE_TIME(0x74);       // Actuall duration     s       0x74 - 0x77

  /*
   *  Main VIFE-Code Extension table (VIF = 0xFD);
   */
  SCALE(0xFD40, 0x0F, 9);   // Volts              V      0xFD40 - 0xFD4F
  SCALE(0xFD50, 0x0F, 12);  // Current            A      0xFD50 - 0xFD5F
}

#if 0
int
main(int argc, char **argv)
{
  unsigned long long int vib;
  float value;

  vib = 0x03;
  value = 1.0f;
  obixNormalizer(&vib, &value);
  printf("%llu\n%f\n\n", vib, value);

  vib = 0x05;
  value = 2.0f;
  obixNormalizer(&vib, &value);
  printf("%llu\n%f\n\n", vib, value);

  vib = 0x01;
  value = 3.0f;
  obixNormalizer(&vib, &value);
  printf("%llu\n%f\n\n", vib, value);

  return 0;
}
#endif
