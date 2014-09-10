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

#ifndef _PARAMU_H_
#define _PARAMU_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "iec_types.h"
#include "iec_types_all.h"

typedef union param_u {
	IEC_BOOL	BOOLvar;		// uint8_t

	IEC_SINT	SINTvar;		// int8_t
	IEC_INT		INTvar;			// int16_t
	IEC_DINT	DINTvar;		// int32_t
	IEC_LINT	LINTvar;		// int64_t

	IEC_USINT	USINTvar;		// uint8_t
	IEC_UINT	UINTvar;		// uint16_t
	IEC_UDINT	UDINTvar;		// uint32_t
	IEC_ULINT	ULINTvar;		// uint64_t

	IEC_BYTE	BYTEvar;		// uint8_t
	IEC_WORD	WORDvar;		// uint16_t
	IEC_DWORD	DWORDvar;		// uint32_t
	IEC_LWORD	LWORDvar;		// uint64_t

	IEC_REAL	REALvar;		// float
	IEC_LREAL	LREALvar;		// double

	IEC_TIME	TIMEvar;
	IEC_DATE	DATEvar;
	IEC_DT		DTvar;
	IEC_TOD		TODvar;

	IEC_STRING	STRINGvar;

	void	*DERIVEDvar;
} param_u_t;

#define BOOLtype	0
#define	SINTtype	1
#define	INTtype		2
#define	DINTtype	3
#define	LINTtype	4
#define	USINTtype	5
#define	UINTtype	6
#define	UDINTtype	7
#define	ULINTtype	8
#define	BYTEtype	9
#define	WORDtype	10
#define	DWORDtype	11
#define	LWORDtype	12
#define	REALtype	13
#define	LREALtype	14
#define	TIMEtype	15
#define	DATEtype	16
#define	DTtype		17
#define	TODtype		18
#define	STRINGtype	19
#define	DERIVEDtype	20

typedef struct {
	param_u_t	p;
	char		t;
} param_ret_t;

#define IEC_VARNAME_SIZE	128
typedef struct {
	char	name[IEC_VARNAME_SIZE];
	int	refcount;
	IEC_DT	lastModifiedTime;
	param_u_t	value;
	char	type;
	unsigned char flags;
#define VARFLAG_PRIORITY_HIGH 1
#define VARFLAG_RETAIN 2
#define VARFLAG_INHEAP 4
#define VARFLAG_CHANGED 8
} iec_variable_t;


#ifdef __cplusplus
};
#endif

#endif /* _PARAMU_H_ */
