
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

/* http.c */
void *HttpRun(void *param);
/* admcli.c */
void AdmSnifferCount(void);
void AdmCliFree(t_cli *cl);
void AdmUsage(t_cli *cl);
void AdmInfo(t_cli *cl);
int AdmCmd(t_cli *cl, char *pbuf);
void AdmZclProtoTee(int in, char *addr, char *bin, int sz);
/* zcludp.c */
int CB_ZclUdpRequest(void *tb, int fd, void *r1, void *r2, int revents);
int CB_ZclUdpEvent(void *tb, int fd, void *r1, void *r2, int revents);
int ZclUdpSendMessage(char *addr, char *zdata, int zlg);
int ZclGetFd(void);
void ZclUdpBind(void);
void ZclGetPrefixeAndBind(void);
void ZclUdpRunQueue(int force);
void ZclClockSc(time_t now);
void ZclClockMs();
void ZclUdpInit(void);
/* sensor.c */
char *SensorClusterToInterfaceName(int cluster);
void SensorCreateKnown(void);
void DoSensorDetection(int force);
void SensorClockSc(time_t now);
int SensorCmdSend(int target, char *buf);
int SensorCmdRecv(int target, char *buf);
int SensorCmdReset(int target);
int SensorCmdDelete(int target);
int SensorCmdConfig(int target);
int SensorCmdDiaSync(int target);
int SensorCmdDiaDelete(int target,int andforget);
void SensorSetDeviceListInObj(void *dst);
//void	iSensorRetrieveAttrCompletion(t_sensor *cp,t_zcl_msg *zmsg);
void iCmnSensorSetApplicationListInObj(t_cmn_sensor *cmn, void *dst);
void iCmnSensorSetSearchStringInObj(t_cmn_sensor *cmn, void *dst);
void iCmnSensorSetInterfaceListInObj(t_cmn_sensor *cmn, int app, void *dst);
int CmnNetworkExecuteMethod(char *ident, int targetlevel, void *obix, char *targetid, int tid);
int iCmnSensorExecuteMethod(t_cmn_sensor *cmn, int app, int cluster, int numm, int targetlevel, void *obix, char *targetid);
int iCmnSensorRetrieveAttrValue(t_cmn_sensor *cmn, int app, int cluster, int numm, int targetlevel, char *targetid, int tid);
int iCmnSensorUpdateAttrValue(t_cmn_sensor *cmn, int app, int cluster, int numm, int targetlevel, void *obix, char *targetid, int tid);
int SensorNumFindByAddr(char *addr);
t_cmn_sensor *CmnSensorNumSerial(int num, int serial);
t_cmn_sensor *CmnSensorFindByIeee(char *ieee);
void SensorZclEventForNumSoftw(unsigned char num, char *addr, unsigned char *buf, int sz);
void SensorZclEventFor(char *addr, unsigned char *buf, int sz);
void SensorEvent(t_imsg *imsg, void *data);
void SensorDump(void *cl, int shortf);
void SensorDumpState(void *cl);
void TicDump(void *cl, char *buf);
/* tictools.c */
char *TicBinToStr(int ct, unsigned int clat, unsigned char *binarytic, int sz);
/* config.c */
char *FindConfigCluster(unsigned int cluster, char *addr);
/* covconfig.c */
void DoCovDataTranslation(t_cov_to_zcl *cz);
void CovToZclLoadTranslator(void *mdl);
void CovToCzDump(void *cl, char *buf);
int CovToZclIndexWithDesc(t_zcl_simple_desc *desc, t_cov_to_zcl *tab[], 
		int eptab[],int max);
int CovToZclMaxReportTime(t_cov_to_zcl *tab[], int nb);
int CovToZclMinReportTime(t_cov_to_zcl *tab[], int nb);
/* var.c */
char *GetBoolValue(void);
char *GetDate(void);
char *GetTime(void);
char *GetAdaptorName(void);
char *GetEnvVarName(void);
char *GetGwContact(void);
char *GetDrvContact(void);
char *GetHostName(void);
char *GetHostTarget(void);
char *GetPlcTarget(void);
char *GetReqId(void);
/* diaucb.c */
int WDiaUCBRequestOk(char *reqname, t_dia_req *preq, t_cmn_sensor *cmn);
int WDiaUCBRequestTimeout(char *reqname, t_dia_req *preq, t_cmn_sensor *cmn);
int WDiaUCBRequestError(char *reqname, t_dia_req *preq, t_cmn_sensor *cmn);
int	WDiaUCBPreLoadTemplate(char *reqname,t_dia_req *preq,char *target,
		t_cmn_sensor *cmn,void **xo,char *templatename);
int WDiaUCBPostLoadTemplate(char *reqname, t_dia_req *preq, char *target, t_cmn_sensor *cmn, void *xo);
/* main.c */
char *GetAdaptorVersion(void);
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
void ServiceStatusResponse(void);
void ServiceStatus(int sig);
void ServiceStop(int sig);
char *DoFilePid(void);
void DoRadioMode();
void ServiceWritePid(void);
void ServiceExit(void);
int main(int argc, char *argv[]);
int MdlMain(int argc, char *argv[], char *cfgroot, char *cfgcust);
