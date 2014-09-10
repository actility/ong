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

extern  int TraceDebug;
extern  int TraceLevel;
extern  int TraceProto;
extern  int PseudoOng;
extern  int WithDia;

extern  int DiaTraceRequest;

extern  void  *MainIQ;
extern  void  *MainTbPoll;

extern  int DiaNbReqPending;
extern  int DiaMaxReqPending;
extern  int DiaConsecutiveTimeout;

extern  unsigned int  DiaSendCount;
extern  unsigned int  DiaRecvCount;
extern  unsigned int  DiaRtryCount;
extern  unsigned int  DiaTimeCount;
extern  unsigned int  DiaIpuOk;
extern  unsigned int  DiaNetOk;
extern  unsigned int  DiaIpuTmt;
extern  unsigned int  DiaNetTmt;
extern  unsigned int  DiaIpuErr;
extern  unsigned int  DiaNetErr;
extern  unsigned int  DiaRepDrp;

