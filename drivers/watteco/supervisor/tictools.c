
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

/*! @file tictools.c
 *
 */
#include <stdio.h>			/************************************/
#include <stdlib.h>			/*     run in main thread           */
#include <string.h>			/************************************/
#include <stdarg.h>
#include <unistd.h>
#include <pthread.h>
#include <errno.h>
#include <limits.h>
#include <signal.h>

#include <math.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#include "rtlbase.h"
#include "rtlimsg.h"

#include "adaptor.h"

#include "xoapipr.h"

#include "define.h"
#include "state.h"
#include "struct.h"

#include "sensor.h"


#include "cproto.h"
#include "cmnproto.h"
#include "extern.h"

#include "../tictools/tic-tools.h"


// Allocate one buffer for TIC decode,store and conversion
static TIC_BUF_TYPE tic_buf[TIC_DESC_NB_BYTES + TIC_FIELDS_UNPACKED_MAX_SIZE_BUF + 1];
static	char line_buf[1024];

char	*TicBinToStr(int ct,unsigned int clat,unsigned char *binarytic,int sz)
{
	char	*pt;
	int	n;
	unsigned char * pt_start = binarytic;
	TIC_meter_descriptor_t	*mdesc;

	int	mt;

	// TODO, le type de compteur ne suffit pas a decoder
	switch	(ct)
	{
	case	MT_ICE :
	case	MT_ICE_p :
	case	MT_ICE_p1 :
		switch	(clat)
		{
		case	W_CLAT(W_CLU_TICICE,W_ATT_TICICE_DATA) :
			mt	= MT_ICE;
		break;
		case	W_CLAT(W_CLU_TICICE,W_ATT_TICICE_PER0) :
			mt	= MT_ICE_p;
		break;
		case	W_CLAT(W_CLU_TICICE,W_ATT_TICICE_PER1) :
			mt	= MT_ICE_p1;
		break;
		}
	break;

	default :
		mt	= ct;
	break;
	}

	mdesc	= tic_metertype_to_meterdesc(mt);
	if	(!mdesc)
	{
		RTL_TRDBG(0,"ERROR invalid tic metertype=%d countertype=%d\n",
				mt,ct);
		return	NULL;
	}

	// deserialized buffer  
	n = tic_deserialize(
				mdesc,
				tic_buf,
				pt_start,
				sizeof(tic_buf)
			);

	if	(n < 0)
	{
		RTL_TRDBG(0,"ERROR tic_deserialize ret=%d\n",n);
		return	NULL;
	}

	tic_formater_init(tic_metertype_to_meterdesc(mt)) ;
	tic_formater_frame_put(line_buf, TB_GET_DESC(tic_buf), TB_GET_BUF(tic_buf));


	n	= sizeof(line_buf);
	pt	= line_buf;
	while	(*pt && n-- > 0)
	{
		switch(*pt)
		{
		case	0x02 :
			*pt	= ' ';
		break;
		case	0x03 :
			*pt	= ' ';
		break;
		case	0x0a :
			*pt	= ' ';
		break;
		case	0x0d :		// not present in tic watteco ?
			*pt	= ' ';
		break;
		case	'<' :
		case	'>' :
			*pt	= '?';
		break;
		case	'"' :
		case	'&' :
			*pt	= '?';
		break;
		default :
		break;
		}
		pt++;
	}

	return	line_buf;

}
