
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

#ifndef _CMNPROTO_H_ 
#define _CMNPROTO_H_ 

/* diaclient.c */
int DiaDisconnected(void);
int DiaFull75();
t_dia_req *DiaFindRequestByDevSerial(int dev,int serial);
int WDiaCreateRequest(char *reqname, char *reqEntity, char *targetID, void *ctt, int len, char *cttType, void *optAttr, void *optHeader, int *tid);
int WDiaRetrieveRequest(char *reqname, char *reqEntity, char *targetID, void *optAttr, void *optHeader, int *tid);
void WDiaAddRequest(t_dia_req *req);
void DiaClientClockSc(time_t now);
void DiaClientInit(void *ptbcb);
/* diaserver.c */
void CB_DIAcREATErEQUEST(char *reqEntity, char *target);
void DiaServerClockSc(time_t now);
void DiaServerInit(void *ptbcb);
void diaIncomingRequestCompletion(int tid, void *ctt, int len, char * cttType, char * retcode);
/* diatools.c */
int WDiaCodeOk(char *code);
int WDiaParseMethod(char *target, char *ident, int *app, int *type, int *level, int *cluster, int *num);
char *WDiaAccessRightsTxt(int level);
void WAdmDumpRequest(void *cl, char *reqname, char *reqid, char *target, char *buf, int sz);
void WDiaTraceRequest(char *side, int tid, char *peer, char *reqname, char *reqid, char *target, char *buf, int sz, char *type);
void WDiaTraceResponse(char *side, int tid, char *peer, char *reqname, char *scode, char *target, char *buf, int sz, char *type);
void DiaClockSc(time_t now);
int CB_DiaUdpRequest(void *tb, int fd, void *r1, void *r2, int revents);
int CB_DiaUdpEvent(void *tb, int fd, void *r1, void *r2, int revents);
void *DiaUdpInit(void);
/* diakal.c */
void DiaKalStart(void);
void CmdDiaKal(t_cli *cli, char *buf);
/* diaipu.c */
void DiaTest(void);
void DiaIpuStart(void);
void CmdDiaIpu(t_cli *cli, char *buf);
/* dianet.c */
void DiaNetStart(void);
void CmdDiaNet(t_cli *cli, char *buf);
/* diadev.c */
void DiaDevStart(int dev, int serial);
void CmdDiaDev(t_cli *cli, char *buf);
/* diaapp.c */
void DiaAppStart(int dev, int serial, int app);
void CmdDiaApp(t_cli *cli, char *buf);
/* diadat.c */
void DiaDatStart(int dev, int serial, int app, int cluster, int attribut, int member);
void CmdDiaDat(t_cli *cli, char *buf);
/* diarep.c */
void DiaReportValue(int dev, int serial, int app, int cluster, int attr, int member, char *fmt, ...);
/* diaacc.c */
void DiaAccStart(int dev, int serial, int app, int acc);
void CmdDiaAcc(t_cli *cli, char *buf);
/* admtcp.c */
int AdmTcpMsgCount(t_cli *cl);
char *AdmAdvance(char *pt);
char *AdmAccept(char *pt);
char *AdmAcceptKeep(char *pt, char *dst);
void AdmPrintSync(int fd, char *fmt, ...);
void AdmPrint(t_cli *cl, char *fmt, ...);
void AdmPrintTable(t_cli *cl, int nb, int tab[], char *fmt, char pstart, char pend, char psep);
void AdmPrintDefTable(t_cli *cl, int nb, int tab[], char *fmt);
void AdmPrintTableUS(t_cli *cl, int nb, unsigned short tab[], char *fmt, char pstart, char pend, char psep);
void AdmPrintDefTableUS(t_cli *cl, int nb, unsigned short tab[], char *fmt);
int CB_AdmTcpRequest(void *tb, int fd, void *r1, void *r2, int revents);
int CB_AdmTcpEvent(void *tb, int fd, void *r1, void *r2, int revents);
int CB_AdmTcpListen(void *tb, int fd, void *r1, void *r2, int revents);
void AdmTcpInit(void);
void AdmTcpSetCB(int (*fct)(t_cli *, char *));
void AdmTcpSetCBFree(void (*fct)(t_cli *));
void AdmClockSc(time_t now);
/* ctools.c */
char *AddBrackets(char *addr);
char *SinceHHHHMMSS(time_t t, char *dst);
int LoadM2MXoRef(void);
int LoadDrvXoRef(char *adap);
/* xmltools.c */
int WXoIsTemplate(char *name);
void *WXoNewTemplate(char *name, int parseflags);
char *WXoSerializeFree(void *xo, int type, int flags, int *sz, char **ctype);
void WXoSaveXml(void *templ, FILE *fout);
/* model.c */
void *MdlLoad(int hot);
void *MdlGetInterface(void *mdl, char *pref, char *type, char *id);
void *MdlGetDescriptorByName(void *itf, char *name);
void MdlConfigureInterfaceTemplate(void *mdlitf, void *itf);
void MdlUnsetStatusInterfaceTemplate(void *itf);
void MdlConfigureDataContainerTemplate(void *mdlitf, void *xo, char *m2mvar);
int MdlGetNbCovConfigurationEntries(void *itf);
void *MdlGetCovConfigurationByNumEntry(void *itf, int n);
/* vartools.c */
void AddVar(char *name);
char *SetVar(char *name, char *value);
char *_GetVar(char *name, int warn);
char *GetVar(char *name);
char *GetVarNoWarn(char *name);
void VarInit(void);
void VarDump(void *cl);
/* iso8601.c */
int parseISO8601Duration(char *string);

#endif
