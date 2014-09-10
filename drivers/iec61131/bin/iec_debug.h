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

#ifndef _IEC_DEBUG_H
#define _IEC_DEBUG_H

#ifdef LINUX
#define IN_TEXT_SEGMENT __attribute__ ((section(".text#")))
#else
#define IN_TEXT_SEGMENT
#endif

#ifdef __NO_TRACE__
#undef RTL_TRDBG
#define RTL_TRDBG(lev,...)
#endif

// rtl_trace levels
enum
{
  TRACE_ERROR = 0,
  TRACE_API = 1,
  TRACE_INFO = 2,
  TRACE_IMPL = 5,
  TRACE_DETAIL = 9,
  TRACE_FULL = 10,
};

#define	TRACE_SET		1
#define	TRACE_STEP		1
#define	TRACE_BLOCK		1
#define	TRACE_ENGINE_SCHEDULE	1 // *LNI* Will move to higher level after
#define	TRACE_ENGINE_TIMING		1 // *LNI* Will move to higher level after
#define	TRACE_ACTION		2
#define	TRACE_TRANSITION	2
#define	TRACE_ST		2
#define	TRACE_GET		3
#define	TRACE_FBD		4	// *LNI*

#define DEV_DENIS
#define DEV_LAURENT

#ifdef DEV_LAURENT
#define DEBUG
#define PWM_DEV
#endif

#endif // _IEC_DEBUG_H