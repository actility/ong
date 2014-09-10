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

#ifndef _SENSOR__H_
#define _SENSOR__H_

#include <stdint.h>
#include <time.h>

#define   cp_self         cp_cmn.cmn_self
#define   cp_num          cp_cmn.cmn_num
#define   cp_serial       cp_cmn.cmn_serial
#define   cp_ieee         cp_cmn.cmn_ieee
#define   cp_dia_devok    cp_cmn.cmn_dia_devok
#define     cp_dia_appok    cp_cmn.cmn_dia_appok
#define     cp_dia_datok    cp_cmn.cmn_dia_datok
#define   cp_dia_metok    cp_cmn.cmn_dia_metok
#define   cp_dia_accok    cp_cmn.cmn_dia_accok
#define   cp_dia_repok    cp_cmn.cmn_dia_repok
#define     cp_dia_reperr   cp_cmn.cmn_dia_reperr


#define WMBUS_MAX_PROFILES_PER_DEVICE   16
#define WMBUS_MAX_ATTR_PER_DEVICE   64

typedef struct
{
  unsigned long long int vib;
  time_t date;
} t_sensor_last_report;

typedef  struct
{
  // Driver common members
  t_cmn_sensor  cp_cmn;

  // Specific wmbus members
  char *cp_nicename;
  unsigned long long int cp_addr;
  unsigned char cp_ctxtLoaded;

  // Add wmbus specifc device attribute
  unsigned long long int  profiles[WMBUS_MAX_PROFILES_PER_DEVICE];
  int nb_profiles;

  // Store date after a report for minInterval
  t_sensor_last_report reports[WMBUS_MAX_ATTR_PER_DEVICE];
  int nb_reports;
} t_sensor;

t_cmn_sensor *SensorFindByIeeeNumber(unsigned long long int ieee);
void DoSensorDetected(long long int tocreate);
int CanReport(int cluster, int attr, int member, t_cmn_sensor *dev);

void WmbusSensorClockSc(time_t now);
void WmbusSensorEvent(t_imsg *imsg, void *data);

#endif

