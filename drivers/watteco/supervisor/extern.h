
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

extern	void	*MdlCfg;
extern	void	*CovCfg;
extern	int	TraceDebug;
extern	int	TraceLevel;
extern	int	TraceProto;
extern	int	PseudoOng;
extern	int	WithDia;
extern	int	SoftwareSensor;
extern	int	HttpSensorList;
extern	int	RequestPANID;
extern	int	WithCovConfig;
extern	int	WithModeling;
extern	int	AllInputCluster;
extern	int	DuplicateCluster;
extern	int	MultiEndPoint;


extern	char	*RadioMode;
extern	int	DynRadioMode;
extern	int	rf212_sleeptime;
extern	float	rf212_msgfrequence;
extern	int	lora_sleeptime;
extern	float	lora_msgfrequence;

extern	int	DiaTraceRequest;

extern	void	*MainIQ;
extern	void	*MainTbPoll;
extern	int	ActiveSniffer;

extern	int	DiaNbReqInuse;
extern	int	DiaMaxReqInuse;
extern	int	DiaNbReqPending;
extern	int	DiaMaxReqPending;
extern	int	DiaConsecutiveTimeout;

extern	unsigned int	DiaSendCount;
extern	unsigned	int		DiaCreaCount;
extern	unsigned	int		DiaRetrCount;
extern	unsigned	int		DiaUpdaCount;
extern	unsigned	int		DiaDeleCount;
extern	unsigned int	DiaRecvCount;
extern	unsigned int	DiaRtryCount;
extern	unsigned int	DiaTimeCount;
extern	unsigned int	DiaIpuOk;
extern	unsigned int	DiaNetOk;
extern	unsigned int	DiaIpuTmt;
extern	unsigned int	DiaNetTmt;
extern	unsigned int	DiaIpuErr;
extern	unsigned int	DiaNetErr;
extern	unsigned int	DiaRepDrp;

extern	unsigned int	ZclSendErr;
extern	unsigned int	ZclInetErr;
extern	unsigned int	ZclMsgPending;
extern	unsigned int	ZclMsgMaxPending;

extern	float		ZclFreq;
extern	float		ZclPeriod;
extern	int		ZclTic;	
extern	int		ZclSleepTime;	

