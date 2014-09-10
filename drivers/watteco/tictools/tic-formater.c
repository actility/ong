
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

/*
 * Copyright (c) 2012, Watteco
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the Institute nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE INSTITUTE AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE INSTITUTE OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 */

/**
 * \file
 * 		TIC formater
 * \author
 *      P.E. Goudet <pe.goudet@watteco.com>
 */

/*============================ INCLUDE =======================================*/

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "tic-formater.h"

// Currently selected meter descriptor ==> Frame format
static TIC_meter_descriptor_t * tic_descriptor;


static unsigned char checksum;
static unsigned long tmp_ulong;
//static signed long tmp_long;
static unsigned short tmp_ushort;
static signed short tmp_short;

//----------------------------------------------------------------------------
// Must be called at least once before using tic_formater !!
void tic_formater_init(TIC_meter_descriptor_t * tic_desc) {
	tic_descriptor = tic_desc;
	checksum=0;
}

//----------------------------------------------------------------------------
// Return position in tic_buf buffer after field formated in the record
 
extern const char FMT_ld_kWh[]; 
extern const char FMT_ld_kvarh[];
const char * pt_var_fmt;
unsigned char * tic_formater_record_put(char * strbuf, unsigned char rec_num, unsigned char *pt_buf, unsigned char * tbdesc) {
	char *ptr;
	signed char max;

	if (tic_descriptor->expected_fields[rec_num].attribute & ATTRIBUTE_IS_SUBFIELD) {
		// Can't format a full record from subfiled number !! 
		// Thing should have been done with first field of multiple field record
		// do not do anything
		return(pt_buf);
	}

	ptr = strbuf;
	pt_var_fmt = NULL;
	
	// first manage field label for all records
	ptr += sprintf(ptr, "%s%c", tic_descriptor->expected_fields[rec_num].label,DELIM);
										
	switch (tic_descriptor->expected_fields[rec_num].type) {
			
		case (TYPE_CSTRING):
			sprintf(ptr, tic_descriptor->expected_fields[rec_num].fmt, pt_buf);	pt_buf += strlen((char*)pt_buf) + 1;
		break;

		case (TYPE_CHAR):
			sprintf(ptr, tic_descriptor->expected_fields[rec_num].fmt, *pt_buf); pt_buf += 1;
		break;

		case (TYPE_U8):
			sprintf(ptr, tic_descriptor->expected_fields[rec_num].fmt, *pt_buf); pt_buf += 1;
		break;
	
		case (TYPE_U16):
			U16p_TO_U16(pt_buf, tmp_ushort); 
			sprintf(ptr, tic_descriptor->expected_fields[rec_num].fmt, tmp_ushort); pt_buf += 2;
		break;
	
		case (TYPE_I16):
			I16p_TO_I16(pt_buf, tmp_short); 
			sprintf(ptr, tic_descriptor->expected_fields[rec_num].fmt, tmp_short); pt_buf += 2;
		break;

		case (TYPE_U24):
			U24p_TO_U32(pt_buf, tmp_ulong); 
			sprintf(ptr, 
				(pt_var_fmt == NULL ? tic_descriptor->expected_fields[rec_num].fmt : pt_var_fmt), tmp_ulong);
			pt_buf += 3;
		break;								

		case (TYPE_6U24): // CJE counter cases: needing "reformating" : 111111:222222:...:666666 or 11111:22222:...:44444
						  // Can have up to 4 or 6 ':' separated u24, to store in successive fields
			U24p_TO_U32(pt_buf, tmp_ulong); sprintf(ptr, tic_descriptor->expected_fields[rec_num].fmt, tmp_ulong); ptr += 6; pt_buf += 3;
			rec_num++; 
			max = 5; 
			while ( (max>0) && 
					(tic_descriptor->expected_fields[rec_num].attribute & ATTRIBUTE_IS_SUBFIELD) &&
					(TIC_FIELD_GET_BIT(tbdesc,rec_num) != 0) ) {
				U24p_TO_U32(pt_buf, tmp_ulong);
				ptr += sprintf(ptr, ":");
				ptr += sprintf(ptr, tic_descriptor->expected_fields[rec_num].fmt, tmp_ulong); 
				pt_buf+=3;
				// Step to next attribute 
				rec_num++;	max--;
			};
		break;

		case (TYPE_4U24): 
			U24p_TO_U32(pt_buf, tmp_ulong); sprintf(ptr, tic_descriptor->expected_fields[rec_num].fmt, tmp_ulong); ptr += 5; pt_buf += 3;
			rec_num++; 
			max = 3; 
			while ( (max>0) && 
					(tic_descriptor->expected_fields[rec_num].attribute & ATTRIBUTE_IS_SUBFIELD) &&
					(TIC_FIELD_GET_BIT(tbdesc,rec_num) != 0) ) {
				U24p_TO_U32(pt_buf, tmp_ulong);
				ptr += sprintf(ptr, ":");
				ptr += sprintf(ptr, tic_descriptor->expected_fields[rec_num].fmt, tmp_ulong); 
				pt_buf+=3;
				// Step to next attribute 
				rec_num++;	max--;
			};
		break; 

		case (TYPE_U32):
			U32p_TO_U32(pt_buf, tmp_ulong); 
			sprintf(ptr, 
				(pt_var_fmt == NULL ? tic_descriptor->expected_fields[rec_num].fmt : pt_var_fmt), tmp_ulong);
			pt_buf += 4;
		break;
	
		case (TYPE_DMYhms):
			// "JJ/MM/AA HH/MM/SS"
			sprintf(ptr, "%02d/%02d/%02d %02d/%02d/%02d",
				*(char *)(pt_buf)  ,*(char *)(pt_buf+1),*(char *)(pt_buf+2),
				*(char *)(pt_buf+3),*(char *)(pt_buf+4),*(char *)(pt_buf+5)); 
			 pt_buf += 6;
		break;
	
		case (TYPE_hmDM):  // CJE counter cases: needing "sub-parsing": {hh:mn:jj:mm}:pt:dp:abcde:kp
			sprintf(ptr, "%02d:%02d:%02d:%02d:",pt_buf[0],pt_buf[1],pt_buf[2],pt_buf[3]); ptr += 12; pt_buf+=4;
			sprintf(ptr, "%s:",(char *)pt_buf); ptr += strlen((char*)pt_buf) + 1; pt_buf += strlen((char*)pt_buf) + 1;
			sprintf(ptr, "%s:",(char *)pt_buf); ptr += strlen((char*)pt_buf) + 1; pt_buf += strlen((char*)pt_buf) + 1;
			U24p_TO_U32(pt_buf, tmp_ulong);
			sprintf(ptr, "%05ld:", tmp_ulong); ptr += 6; pt_buf+=3;
			sprintf(ptr, "%02d",*pt_buf); pt_buf+=1;
		break;
	
		case (TYPE_DMh):  // CJE counter cases: needing "sub-parsing":  {jj:mm:hh}:cg
			sprintf(ptr, "%02d:%02d:%02d:",pt_buf[0],pt_buf[1],pt_buf[2]); ptr += 9; pt_buf+=3;
			sprintf(ptr, "%02d",*pt_buf); pt_buf+=1;
		break;
	
		case (TYPE_hm):  // CJE counter cases: needing "sub-parsing":  {hh:mn}:dd
			sprintf(ptr, "%02d:%02d:",pt_buf[0],pt_buf[1]); ptr += 6; pt_buf+=2;
			sprintf(ptr, "%02d",*pt_buf); pt_buf+=1;
		break;

		default:
			// Get out without any change neither put in buf if TYPE not recognized
			return pt_buf;
		break;
	};

	// Checksum calculation
	//checksum = 'x';
	checksum = tic_str_compute_checksum(strbuf);
	ptr = strbuf + strlen(strbuf);
	sprintf(ptr, "%c%c%c",DELIM,checksum,LF); 
	return(pt_buf);
}

//----------------------------------------------------------------------------
// Basic example needing Big Output buffer .. could be rewritten for specific application use case
void tic_formater_frame_put(char* buf, unsigned char * tbdesc, unsigned char * tbbuf) {
	
	unsigned char ifield;
	unsigned char nfield;
	
	ifield=0;
	nfield=tic_descriptor->expected_fields_number;	


	sprintf(buf, "%c%c",STX,LF); buf += strlen(buf);

	while (nfield-- > 0) {		
		if (TIC_FIELD_GET_BIT(tbdesc,ifield) != 0) {
			tbbuf = tic_formater_record_put(buf, ifield, tbbuf, tbdesc);
// fprintf(stdout,"%s",buf);
			buf += strlen(buf);
		} // if (TIC_FIELD_GET_BIT(tbdesc,ifield) != 0)
		ifield++;
	} // while (nfield-- > 0) 

	sprintf(buf, "%c%c",ETX,LF); buf += strlen(buf);
};

