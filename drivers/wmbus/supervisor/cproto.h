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

#if 1

#include "cmnstruct.h"

/* admcli.c */
void AdmUsage(t_cli *cl);
void AdmInfo(t_cli *cl);
int AdmCmd(t_cli *cl, char *pbuf);
void AdmZclProtoTee(int in, char *addr, char *bin, int sz);
/* sensor.c */
void SensorCreateKnown(void);
void DoSensorDetection(void);
void SensorClockSc(time_t now);
int SensorCmdSend(int target, char *buf);
int SensorCmdRecv(int target, char *buf);
int SensorCmdReset(int target);
int SensorCmdDelete(int target);
int SensorCmdDiaSync(int target);
void SensorSetDeviceListInObj(void *dst);
void iCmnSensorSetApplicationListInObj(t_cmn_sensor *cmn, void *dst);
void iCmnSensorSetSearchStringInObj(t_cmn_sensor *cmn, void *dst);
void iCmnSensorSetInterfaceListInObj(t_cmn_sensor *cmn, int app, void *dst);
//int iCmnSensorExecuteMethod(t_cmn_sensor *cmn, int app, int cluster, int numm);
int SensorNumFindByAddr(char *addr);
t_cmn_sensor *CmnSensorNumSerial(int num, int serial);
t_cmn_sensor *CmnSensorFindByIeee(char *ieee);
void SensorZclEventForNumSimul(unsigned char num, char *addr, unsigned char *buf, int sz);
void SensorZclEventFor(char *addr, unsigned char *buf, int sz);
void SensorEvent(t_imsg *imsg, void *data);
void SensorDump(void *cl);
void SensorDumpState(void *cl);
/* config.c */
char *FindConfigCluster(unsigned int cluster, char *addr);
/* var.c */
char *GetDate(void);
char *GetTime(void);
char *GetAdaptorName(void);
char *GetGwContact(void);
char *GetDrvContact(void);
char *GetHostName(void);
char *GetHostTarget(void);
char *GetReqId(void);
void AddVar(char *name);
char *SetVar(char *name, char *value);
char *GetVar(char *name);
void VarInit(void);
void VarDump(void *cl);
/* main.c */
int DoSerial(void);
void DoClockMs(void);
void DoClockSc(void);
void DoInternalEvent(t_imsg *imsg);
void DoInternalTimer(t_imsg *imsg);
void FAKE_RTL_POLL(void);
void MainLoop(void);
void HttpStart(void);
void Usage(char *prg, char *fmt);
int DoArg(int initial, int argc, char *argv[]);
void SetOption(char *name, char *value);
void DoConfig(int hot, int custom, char *fcfg);
int main(int argc, char *argv[]);

#endif
