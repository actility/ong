
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

#ifndef _SPECCPROTO_H_ 
#define _SPECCPROTO_H_ 

/*
 * these functions must be coded in the specific part of the driver
 */

char *GetAdaptorName();
char *GetAdaptorVersion();
char *GetEnvVarName();
char *GetHostName(void);
char *GetHostTarget(void);
char *GetPlcTarget(void);
char *GetReqId(void);

t_cmn_sensor *CmnSensorNumSerial(int num, int serial);
t_cmn_sensor *CmnSensorFindByIeee(char *ieee);
int CmnNetworkExecuteMethod(char *ident,int targetlevel,void *obix,char *targetid,int tid);
int	iCmnSensorExecuteMethod(t_cmn_sensor *cmn,int app,int cluster,int numm,
  int targetlevel,void *obix,char *targetid,int tid);
int iCmnSensorRetrieveAttrValue(t_cmn_sensor *cmn,int app,int cluster,int numm,
  int targetlevel,char *targetid,int tid);
int iCmnSensorUpdateAttrValue(t_cmn_sensor *cmn,int app,int cluster,int numm,
  int targetlevel,void *obix,char *targetid,int tid);


#endif
