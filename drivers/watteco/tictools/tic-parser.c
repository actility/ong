
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
 * 		TIC parser
 * \author
 *      P.E. Goudet <pe.goudet@watteco.com>
 */

/*============================ INCLUDE =======================================*/

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "tic-parser.h"

// Global variable used for parsing and temporary storage
// Currently these varables ar not stored in parser status to avoid to muche RAM memory usage
// when making multiple descriptors parsing during Meter type recognition process ...
static unsigned char istr;			  // Current string position
static unsigned char checksum;       // Current checksum calculation
static char field_label[STR_LABEL_SZ_MAX]; // Current input field_label string
static char field_value[STR_VALUE_SZ_MAX]; // Current input field_value string

// Thes Globals variables have only a local scope usage inside functions
// process_record_desc_only() or process_record_tic_buf()
static const TIC_expected_field_t * pt_expfields, * ice_p_pt_expfields, * ice_p1_pt_expfields;
static unsigned char nb_expfields,ice_p_nb_expfields,ice_p1_nb_expfields;
static unsigned char nf, ice_p_nf, ice_p1_nf;
static unsigned char max, process_it;
static unsigned char *ptr, *ptr2, tmp_uchar;
static unsigned long tmp_long;
static unsigned short tmp_u16;
static signed short tmp_i16;
static unsigned char *	pt_tic_buf_buf;

// DATE EXCEPTION IN PROTOCOL PROCESSING (ICE meter): "DD/MM/YY HH/MM/SS" (must ignore a DELIM in field_value !!)	
static unsigned char ignore_value_delim = 0;

void (*tic_parser_frame_rx_handler)(TIC_parser_status_t * stat) = NULL;

//========================================================================
// Internal effective data buffer and status struct
static TIC_parser_status_t parser_status;
//========================================================================

//----------------------------------------------------------------------------
TIC_meter_type_t tic_parser_get_meter_type() {
	return(parser_status.pt_tic_descriptor->meter_type);
}

//----------------------------------------------------------------------------
void tic_parser_set_frame_rx_handler(void (*f)(TIC_parser_status_t * stat)) {
	tic_parser_frame_rx_handler = f;
}

//----------------------------------------------------------------------------
void tic_parser_init( 
	TIC_meter_descriptor_t * tic_desc,
	unsigned char chks_check, 
	unsigned char nbfv) {
	// NOTICE: After init, reset is mandatory to start the parser !!!
		
	// TODO: Find a good way to avoid Race condition when acting on parser status varables
	// Minimal try using an unsused state STOPPED
 	parser_status.parser_state=STOPPED; 
	
	// Init parser parameters
	parser_status.pt_tic_descriptor = tic_desc; // tic_get_current_meter_descriptor();
	parser_status.chksum_check = chks_check; 
	parser_status.nb_field_to_valid_frame = nbfv;
	
	// NOTICE: After init, reset is mandatory to start the parser !!!
	
	// Reset ICE p an p-1 desc sub parser only onb init not to loose current informations
	// And set the unpacked bit of the descriptor
	memset(TB_GET_DESC(parser_status.ice_p_pt_tic_buf),0x00,TIC_DESC_NB_BYTES);
	TIC_FIELD_SET_BIT(parser_status.ice_p_pt_tic_buf, DESC_UNPACKED_BIT);
	memset(TB_GET_DESC(parser_status.ice_p1_pt_tic_buf),0x00,TIC_DESC_NB_BYTES);
	TIC_FIELD_SET_BIT(parser_status.ice_p1_pt_tic_buf, DESC_UNPACKED_BIT);
}
		
//----------------------------------------------------------------------------
void tic_parser_reset() {
	// TODO: Find a good way to avoid Race condition when acting on parser status varables
	// Minimal try using an unsused state STOPPED
 	parser_status.parser_state=STOPPED; 
	
	// Reset parser
	memset(TB_GET_DESC(parser_status.pt_tic_buf),0x00,TIC_DESC_NB_BYTES);
	parser_status.pt_tic_buf_buf = TB_GET_BUF(parser_status.pt_tic_buf);    
	parser_status.no_field = 0;
	 
	// Reset ICE p sub parser
	parser_status.ice_p_pt_tic_buf_buf = TB_GET_BUF(parser_status.ice_p_pt_tic_buf);    
	parser_status.ice_p_no_field = 0; 
	parser_status.ice_p_block_initialised = 0;
	 
	// Reset ICE p1 sub parser
	parser_status.ice_p1_pt_tic_buf_buf = TB_GET_BUF(parser_status.ice_p1_pt_tic_buf);    
	parser_status.ice_p1_no_field = 0; 
	parser_status.ice_p1_block_initialised = 0;
	
	parser_status.tic_input_frame_size = 0;
	parser_status.tic_meter_type_discovered = 0;
	
	// Re-init record parsing	
	istr = 0; checksum = 0;  	
	// Re-init meter discovering if needed
	if ((parser_status.pt_tic_descriptor == NULL) || (parser_status.pt_tic_descriptor->meter_type == MT_NULL)) {
		pt_expfields = NULL; nb_expfields = 0;
		ice_p_pt_expfields = NULL; ice_p_nb_expfields = 0;
		ice_p1_pt_expfields = NULL; ice_p1_nb_expfields = 0;
	};
		
 
	// DATE EXCEPTION IN PROTOCOL PROCESSING (ICE meter): "DD/MM/YY HH/MM/SS" (must ignore a DELIM in field_value !!)
 	ignore_value_delim = 0;
 
 	// Accept parser startup
	parser_status.parser_state=PEND_STX;
}

//----------------------------------------------------------------------------
unsigned char process_record_tic_buf(
	unsigned char rec_num, 
	unsigned char **hndl_tic_buf_buf,
	const TIC_expected_field_t * expected_fields,
	unsigned char * tic_buf_desc_u8,
	unsigned char * pt_tic_buf_buf_origin ) {
		
	// Decode and Store validated field in the field_buf buffer
		
	// hndl ptr access optimisation	and manage the absolute access case if pt_tic_buf_buf_origin != NULL
	pt_tic_buf_buf = (pt_tic_buf_buf_origin == NULL ? 
		*hndl_tic_buf_buf : 
		pt_tic_buf_buf_origin + expected_fields[rec_num].abs_pos);
	
	switch (expected_fields[rec_num].type) {

		case (TYPE_CSTRING):
			strcpy((char *)pt_tic_buf_buf,field_value); 
			pt_tic_buf_buf += strlen(field_value)+1; TIC_FIELD_SET_BIT(tic_buf_desc_u8,rec_num);		
		break;

		case (TYPE_CHAR):
			*(pt_tic_buf_buf) = *field_value; pt_tic_buf_buf++; TIC_FIELD_SET_BIT(tic_buf_desc_u8,rec_num);
		break;

		case (TYPE_U8):
			//*(pt_tic_buf_buf) = (char) atoi(field_value); 
			// TODO: Verify that scanf is not to power/time consumming 
			// if yes, change this only case with a specificty on "PPOT" field_label.
			// if not sscanf could be genralized ==> easying format reading 
			// (usage try here only because "PPOT" field is hexadecimal.)
			// !!!! Warning direct scanf on the pointer is wrong because of word alignment of scanf on MSP430/CCSV4			
			sscanf(field_value,expected_fields[rec_num].fmt,&tmp_uchar);
			*pt_tic_buf_buf = tmp_uchar;
			pt_tic_buf_buf++; TIC_FIELD_SET_BIT(tic_buf_desc_u8,rec_num);
			  			
		break;
		
		case (TYPE_U16):
			tmp_u16 = (unsigned short) atoi(field_value);									
			U16_TO_U16p(tmp_u16,pt_tic_buf_buf); pt_tic_buf_buf += 2; 
			TIC_FIELD_SET_BIT(tic_buf_desc_u8,rec_num);
		break;
		
		case (TYPE_I16):
			tmp_i16 = (signed short) atoi(field_value);									
			I16_TO_I16p(tmp_i16,pt_tic_buf_buf); pt_tic_buf_buf += 2; 
			TIC_FIELD_SET_BIT(tic_buf_desc_u8,rec_num);
		break;

		case (TYPE_U24):
			tmp_long = (unsigned long)strtol(field_value,NULL,10);									
			U32_TO_U24p(tmp_long,pt_tic_buf_buf); pt_tic_buf_buf += 3; TIC_FIELD_SET_BIT(tic_buf_desc_u8,rec_num);
		break;								

		case (TYPE_6U24): // CJE counter cases: needing "sub-parsing" : 111111:222222:...:666666 or 11111:22222:...:44444
		case (TYPE_4U24): // Can have up to 4 or 6 ':' separated u24, to store in successive fields
			ptr = (unsigned char *)field_value;
			max = (expected_fields[rec_num].type == TYPE_6U24 ? 6 : 4);
			while ((*ptr != DELIM) && (*ptr != '\0') && (max > 0)) {
				ptr2 = ptr; 
				// Find End of sub-field field_value
				while ((*ptr2 != DELIM) && (*ptr2 != ':') && (*ptr2 != '\0')) {ptr2++;};													
			// printf("\nFound: %s\n", ptr);
				
				tmp_long = (unsigned long)strtol((const char *)ptr,NULL,10);									
				U32_TO_U24p(tmp_long,pt_tic_buf_buf); pt_tic_buf_buf += 3;  TIC_FIELD_SET_BIT(tic_buf_desc_u8,rec_num);
			// printf("Long = %ld %08lx %02x%02x%02x\n",tmp_long, tmp_long,*(pt_tic_buf_buf - 3),*(pt_tic_buf_buf - 2),*(pt_tic_buf_buf - 1));
			
				if (*ptr2 == ':') {
					ptr = ptr2 + 1; // Step to the possible start of next field			
			// printf("\nNext: %s\n", ptr);
				} else {
					ptr = ptr2; 
				}					
										
				max--; // Decrease the expected number of sub-fields
				rec_num++;   // Increment the rec_num idx to immediatly jump over repeted expected fields
			}
		break; 

		case (TYPE_U32):
			tmp_long = (unsigned long)strtol(field_value,NULL,10);									
			U32_TO_U32p(tmp_long,pt_tic_buf_buf); pt_tic_buf_buf += 4; 
			TIC_FIELD_SET_BIT(tic_buf_desc_u8,rec_num);
		break;
		
		case (TYPE_DMYhms):
			// "JJ/MM/AA HH/MM/SS"
			ptr = (unsigned char *)field_value; max = 6;
			while (max > 0) {
				*(pt_tic_buf_buf) = (char) atoi((const char *)ptr);
				pt_tic_buf_buf += 1; ptr += 3; max--;
			}
			TIC_FIELD_SET_BIT(tic_buf_desc_u8,rec_num);
		break;
		
		case (TYPE_hmDM):  // CJE counter cases: needing "sub-parsing": {hh:mn:jj:mm}:pt:dp:abcde:kp
			ptr = (unsigned char *)field_value;
			max = 8; // 8 sub fields to decode 
			while ((*ptr != DELIM) && (*ptr != '\0') && (max > 0)) {
				ptr2 = ptr; 
				// Find End of sub-field field_value
				while ((*ptr2 != DELIM) && (*ptr2 != ':') && (*ptr2 != '\0')) {ptr2++;};
				if (* ptr2 == ':') *ptr2 = '\0';
				
				// Conditional processing of sub parsed fields
				if (max > 4) { // processing hh:mn:jj:mm
					*pt_tic_buf_buf	= (char) atoi((const char *)ptr); pt_tic_buf_buf++; TIC_FIELD_SET_BIT(tic_buf_desc_u8,rec_num);
				} else { // <=4: Use the possible format definition 
					rec_num++;   // Increment the rec_num idx to immediatly jump over repeted expected fields
					switch (expected_fields[rec_num].type) {
		
						case (TYPE_CSTRING):
							strcpy((char *)pt_tic_buf_buf,(const char *)ptr); pt_tic_buf_buf += strlen((const char *)ptr)+1; TIC_FIELD_SET_BIT(tic_buf_desc_u8,rec_num);
						break;

						case (TYPE_U24):
							tmp_long = (unsigned long)strtol((const char *)ptr,NULL,10);	
							U32_TO_U24p(tmp_long,pt_tic_buf_buf); pt_tic_buf_buf += 3; TIC_FIELD_SET_BIT(tic_buf_desc_u8,rec_num);
						break;

						case (TYPE_U8):
							*pt_tic_buf_buf = (char) atoi((const char *)ptr); pt_tic_buf_buf++; TIC_FIELD_SET_BIT(tic_buf_desc_u8,rec_num);
						break;
						
						default:
						break;
					}
				}
														
				ptr = (ptr2 + 1); // Step to the possible start of next field
				max--; // Decrease the expected number of sub-fields									
			}
		break;
		
		case (TYPE_DMh):  // CJE counter cases: needing "sub-parsing":  {jj:mm:hh}:cg
			ptr = (unsigned char *)field_value;
			max = 4; // 4 sub fields to decode 
			while ((*ptr != DELIM) && (*ptr != '\0') && (max > 0)) {
				ptr2 = ptr; 
				// Find End of sub-field field_value
				while ((*ptr2 != DELIM) && (*ptr2 != ':') && (*ptr2 != '\0')) {ptr2++;};
				if (* ptr2 == ':') *ptr2 = '\0';
				
				// Conditional processing of sub parsed fields
				if (max > 1) { // processing jj:mm:hh
					*pt_tic_buf_buf	= (char) atoi((const char *)ptr); pt_tic_buf_buf++; TIC_FIELD_SET_BIT(tic_buf_desc_u8,rec_num);
				} else { // <=1: Use the possible format definition
					rec_num++;   // Increment the rec_num idx to immediatly jump over repeted expected fields
					switch (expected_fields[rec_num].type) {

						case (TYPE_U8):
							*pt_tic_buf_buf = (char) atoi((const char *)ptr); pt_tic_buf_buf++; TIC_FIELD_SET_BIT(tic_buf_desc_u8,rec_num);
						break;
						
						default:
						break;
					}
				}																			
				ptr = (ptr2 + 1); // Step to the possible start of next field
				max--; // Decrease the expected number of sub-fields								
			}
		break;
		
		case (TYPE_hm):  // CJE counter cases: needing "sub-parsing":  {hh:mn}:dd
			ptr = (unsigned char *)field_value;
			max = 3; // 3 sub fields to decode 
			while ((*ptr != DELIM) && (*ptr != '\0') && (max > 0)) {
				ptr2 = ptr; 
				// Find End of sub-field field_value
				while ((*ptr2 != DELIM) && (*ptr2 != ':') && (*ptr2 != '\0')) {ptr2++;};
				if (* ptr2 == ':') *ptr2 = '\0';
				
				// Conditional processing of sub parsed fields
				if (max > 1) { // processing jj:mm:hh
					*pt_tic_buf_buf	= (char) atoi((const char *)ptr); pt_tic_buf_buf++; TIC_FIELD_SET_BIT(tic_buf_desc_u8,rec_num);
				} else { // <=1: Use the possible format definition
					rec_num++;   // Increment the rec_num idx to immediatly jump over repeted expected fields
					switch (expected_fields[rec_num].type) {

						case (TYPE_U8):
							*pt_tic_buf_buf = (char) atoi((const char *)ptr); pt_tic_buf_buf++; TIC_FIELD_SET_BIT(tic_buf_desc_u8,rec_num);
						break;
						
						default:
						break;
					}
				}																			
				ptr = (ptr2 + 1); // Step to the possible start of next field
				max--; // Decrease the expected number of sub-fields								
			}
		break;

		default:
			// This case should never occur
		break;
	}; 
	
	// hndl ptr access optimisation	
	*hndl_tic_buf_buf = pt_tic_buf_buf;
	
	return(rec_num);
}	
//----------------------------------------------------------------------------
char tmp_buf[256];
int tic_parser_input_char_TEST(unsigned char c) {
	// TODO: This interrupt funcion may become too long !!!!!!!

//parser_status.pt_tic_buf_buf[parser_status.tic_input_frame_size%TIC_FIELDS_MAX_SIZE_BUF] = c;	
	tmp_buf[parser_status.tic_input_frame_size%255]=c;
	
	parser_status.tic_input_frame_size++; // evaluate original TIC frame size

	// Return code 0 anycase for now ...
	return(0);
}

//----------------------------------------------------------------------------
// Add DATECOUR or DATE to the block buffer (last modification)
unsigned char ice_pp1_init_block(TIC_BUF_TYPE * pt_tic_buf,
    unsigned char ice_pp1_nf, 
    unsigned char ice_pp1_block_initialised,
	unsigned char * tic_buf_desc_u8,
	unsigned char * tic_buf_buf) {
		
	unsigned char clear_bits_until;	
	
	if ((ice_pp1_nf >3) && (ice_pp1_nf <18)) { // This field belongs to EAp[1] bloc
		if (ice_pp1_block_initialised == 3) return ice_pp1_block_initialised; // This block is already initialised
		ice_pp1_block_initialised = 3; 
		tic_buf_buf += 14; 
		clear_bits_until = 18;
		
	} else if ((ice_pp1_nf >18) && (ice_pp1_nf <33)) { // This field belongs to ERPp[1] bloc
		if (ice_pp1_block_initialised == 18) return ice_pp1_block_initialised;  // This block is already initialised
		ice_pp1_block_initialised = 18; 
		tic_buf_buf += 62; 
		clear_bits_until = 33;
					
	} else if ((ice_pp1_nf >33) && (ice_pp1_nf <48)) { // This field belongs to ERNp[1] bloc
		if (ice_pp1_block_initialised == 33) return ice_pp1_block_initialised; // This block is already initialised
		ice_pp1_block_initialised = 33;
		tic_buf_buf += 110; 
		clear_bits_until = 48;
		
	} else {
		return(0); // Anormal case 
	}
	
	if ((ptr = tic_get_value_at_index(tic_metertype_to_meterdesc(MT_ICE), pt_tic_buf, 1)) == NULL) // Try with DATECOUR
		if ((ptr = tic_get_value_at_index(tic_metertype_to_meterdesc(MT_ICE), pt_tic_buf, 2)) == NULL) // Try with DATE (v2.4)
			return(0);
				
	// Intialise buf at position
	//memset(tic_buf_buf,0x00,38);
	
	// TODO: Ci-dessous devrait être le bon code !!!
	// Intialise buf at position
	memset(tic_buf_buf,0x00,48);
	// Clear the corresponding bits
	while (ice_pp1_nf < clear_bits_until) {
		TIC_FIELD_CLR_BIT(tic_buf_desc_u8,ice_pp1_nf);
		ice_pp1_nf++; 
	}
		
	
	// Set block date
	memcpy(tic_buf_buf,ptr,6);
	// Set the bit for the correcponding date		
	TIC_FIELD_SET_BIT(tic_buf_desc_u8,ice_pp1_block_initialised);
	
	return(ice_pp1_block_initialised);
}

//----------------------------------------------------------------------------
// Should return 1 if a frame must be processed
int tic_parser_input_char(unsigned char c) {
	// TODO: This interrupt funcion may become too long !!!!!!!
	
	parser_status.tic_input_frame_size++; // evaluate original TIC frame size

	if (c == EOT) {
		// EOT is interrupt of transmission
		// Reject full current frame in case it arrives
		tic_parser_reset();

	} else 

	if (parser_status.parser_state > PEND_CR) { // (STOPPED) || (PEND_PROCESS)
		// Ignore incomming bytes when parser stopped or PEND_PROCESS

	} else 

	// Character processing according to state
	switch (parser_status.parser_state) {

		case (PEND_STX):
			if (c == STX) {
				//tic_parser_reset();
				parser_status.tic_input_frame_size=1; // Add first STX after size zeroed by parser_reset
				parser_status.parser_state = PEND_LF;			
			}
		break;

		case (PEND_CR): // Accept either CR/LF or only LF between lines
		case (PEND_LF):
			parser_status.parser_state = PEND_LABEL; istr = 0; checksum=0;
		break;

		case (PEND_LABEL):	
			if (c == LF) {
				// Just ignore surely a LF following CR
			} else if (c == ETX) {
				// Call for end of frame call back if at least one field was found
				// TODO: Could control a more the packet validity according to
				// EDF specifications
				// Small frame validity control: accept data only if more than x fields are received
				// Accepts short frames CBETM_CT ! ==> To be manage by client application (Cf EDF specifications)
// printf("ETX received with %d field(s)!\n",tic_nb_fields_selected(TB_GET_DESC(parser_status.pt_tic_buf)));	
				if (tic_nb_fields_selected((TIC_desc_t *)TB_GET_DESC(parser_status.pt_tic_buf)) >= parser_status.nb_field_to_valid_frame) {
					// Set lock on ressource until processed
					parser_status.parser_state = PEND_PROCESS;
								
					if ((pt_expfields != NULL) && 
						((parser_status.pt_tic_descriptor == NULL) || 
						 (parser_status.pt_tic_descriptor->meter_type == MT_NULL))) {
						// Expected fields list could be found, get tic_descriptor
						parser_status.pt_tic_descriptor = find_meter_desc(pt_expfields, (TIC_desc_t *)TB_GET_DESC(parser_status.pt_tic_buf));
						parser_status.tic_meter_type_discovered = 1;
					}

					// Set the size of the frame at the end of buffers
					parser_status.pt_tic_buf[TIC_PARSER_PACKED_BUF_SZ] 	  	   = (parser_status.pt_tic_buf_buf - parser_status.pt_tic_buf);
					parser_status.ice_p_pt_tic_buf[TIC_PARSER_UNPACKED_BUF_SZ] = (parser_status.ice_p_pt_tic_buf_buf - parser_status.ice_p_pt_tic_buf);
					parser_status.ice_p1_pt_tic_buf[TIC_PARSER_UNPACKED_BUF_SZ]= (parser_status.ice_p1_pt_tic_buf_buf - parser_status.ice_p1_pt_tic_buf);
				   	
// printf("Frame received !\n");	
					if (tic_parser_frame_rx_handler != NULL) {
						tic_parser_frame_rx_handler(&parser_status);
						return(1);
					}
				}
				
			} else {
				checksum += c;			
				if (c == DELIM) { 
					field_label[istr]='\0'; 
// printf("\nl: %s\n",field_label);	
					parser_status.parser_state = PEND_VALUE; istr=0;
					
					// DATE EXCEPTION IN PROTOCOL PROCESSING (ICE meter): "DD/MM/YY HH/MM/SS" (must ignore a DELIM in field_value !!)	
					// BEWARE: This an optimisation trick to avoid Real full field_labels parsing
					// finding the need of  field_label should be on data type: TYPE_
					// TODO: Find beter !!
					if ( ((field_label[0]=='D') && (field_label[1]=='A') && (field_label[2]=='T')) ||
						 ((field_label[0]=='D') && (field_label[1]=='E') && (field_label[2]=='B')) ||
						 ((field_label[0]=='F') && (field_label[1]=='I') && (field_label[2]=='N')) )
						ignore_value_delim = 1;
					
				} else if ((istr < (STR_LABEL_SZ_MAX -1)) &&
						   (c != LF) && (c != CR) && (c != STX) && (c != ETX))  { 
					field_label[istr++] = c;
					
				} else {
					// Reject full current frame
					tic_parser_reset();
				}
					
			}
		break;

		case (PEND_VALUE):				
			if ((c == DELIM) && (! ignore_value_delim)) {
				field_value[istr]='\0';

// printf("FIN Value: l: %s, v: %s , Chks = %c (%02x)\n",field_label, field_value,checksum,checksum);			
				parser_status.parser_state = PEND_CHECKSUM;
				
			} else if ((istr < (STR_VALUE_SZ_MAX -1)) &&
					   (c != LF) && (c != CR) && (c != STX) && (c != ETX)) { 
				checksum += c;
				field_value[istr++] = c;
				
// printf("l: %s, v: %s , Chks = %c (%02x) c = %c (%02x)\n",field_label, field_value,checksum,checksum,c,c);
								
				// DATE EXCEPTION IN PROTOCOL PROCESSING (ICE meter): "DD/MM/YY HH/MM/SS" (must ignore a DELIM in field_value !!)	
				// If one DELIM has been ignored uncheck need of ignore to ends field_value on next DELIM
				if (c == DELIM) ignore_value_delim =0;
								
			} else {
				// Reject full current frame
				tic_parser_reset();
			};
		break;

		case (PEND_CHECKSUM):

			#ifdef TIC_PARSER_NO_CHECKSUM
				checksum = c; // Fake validation of checksum
			#else
				if (parser_status.chksum_check) {
					checksum &= 0x3F; checksum += 0x20;
// printf("l: %s, v: %s , Chks = %c (%02x)\n",field_label, field_value,checksum,checksum);
				} else {
					checksum = c;
				}
			#endif

// printf("\nc==checksum: %c == %c\n", c,checksum);
			if (c==checksum) {
// printf("l: %s, v: %s , Chks = %c (%02x), c = %c (%02x)\n",field_label, field_value,checksum,checksum,c,c);
				// TODO: process validated input field
				process_it = 0;
	
								
				// if needed, try to find right list of expected fields according to first received field_label -------------------------------				
				if (pt_expfields == NULL) {
					if ((parser_status.pt_tic_descriptor == NULL) || 
						(parser_status.pt_tic_descriptor->meter_type == MT_NULL)) {
//printf("\n tic_set_expfields_on_field_label\n");
						tic_set_expfields_on_label(&pt_expfields, &nb_expfields, field_label);
					} else {
						pt_expfields = parser_status.pt_tic_descriptor->expected_fields;
						nb_expfields = parser_status.pt_tic_descriptor->expected_fields_number;						
					}		
					// Validate pointers to correct expected sub fields				
					if (pt_expfields == tic_meter_descriptor[MT_ICE].expected_fields) {							
						ice_p_pt_expfields = tic_meter_descriptor[MT_ICE_p].expected_fields;
						ice_p_nb_expfields = tic_meter_descriptor[MT_ICE_p].expected_fields_number;
						ice_p1_pt_expfields = tic_meter_descriptor[MT_ICE_p1].expected_fields;
						ice_p1_nb_expfields = tic_meter_descriptor[MT_ICE_p1].expected_fields_number;
					}
				}				
								   
#define PARSER_NOT_FOUND 0			
#define PARSER_PROCESS_GENERAL_FIELD 1
#define PARSER_IGNORE_FIELD 2
#define PARSER_PROCESS_ICE_P_FIELD 3
#define PARSER_PROCESS_ICE_P1_FIELD 4
										
				// Recherche du label
				// BEWARE: This label search bet on ordered records according to specifications (EDF) 
				//-----------------------------------------------------------------------------------
				for (nf = parser_status.no_field; ((nf < nb_expfields) && (!process_it)); nf++) {
						
					if (pt_expfields[nf].attribute & ATTRIBUTE_NOT_MANAGED_FIELD) {
						// The "not managed" field could be a generic string, so use generic comparison						
						if (tic_generic_string_match((unsigned char *)(pt_expfields[nf].label),(unsigned char *)field_label)) {
							process_it = PARSER_IGNORE_FIELD;					
						}
						
					} else if (strcmp(pt_expfields[nf].label,field_label) == 0) {
						process_it = PARSER_PROCESS_GENERAL_FIELD;						
						
					} else if (pt_expfields[nf].attribute & ATTRIBUTE_ICE_pp1_FIELD) {						
						if (tic_generic_string_match((unsigned char *)(pt_expfields[nf].label),(unsigned char *)field_label)) {
							if (tic_generic_string_match((unsigned char *)"*1*",(unsigned char *)field_label)) {
								// Find field_label in Period P minus 1 case
								for (ice_p1_nf = parser_status.ice_p1_no_field; ((ice_p1_nf < ice_p1_nb_expfields) && (!process_it)); ice_p1_nf++) {
									if (ice_p1_pt_expfields[ice_p1_nf].attribute & ATTRIBUTE_NOT_MANAGED_FIELD) {
										// The "not managed" field could be a generic string, so use generic comparison						
										if (tic_generic_string_match((unsigned char *)(ice_p1_pt_expfields[nf].label),(unsigned char *)field_label)) {
											process_it = PARSER_IGNORE_FIELD;					
										}
										
									} else if (strcmp(ice_p1_pt_expfields[ice_p1_nf].label,field_label) == 0) {
										process_it = PARSER_PROCESS_ICE_P1_FIELD;														
									}
								}	
								ice_p1_nf--;
							} else {
								// Find field_label in Period P case	
								for (ice_p_nf = parser_status.ice_p_no_field; ((ice_p_nf < ice_p_nb_expfields) && (!process_it)); ice_p_nf++) {
									if (ice_p_pt_expfields[ice_p_nf].attribute & ATTRIBUTE_NOT_MANAGED_FIELD) {
										// The "not managed" field could be a generic string, so use generic comparison						
										if (tic_generic_string_match((unsigned char *)(ice_p_pt_expfields[nf].label),(unsigned char *)field_label)) {
											process_it = PARSER_IGNORE_FIELD;					
										}
										
									} else if (strcmp(ice_p_pt_expfields[ice_p_nf].label,field_label) == 0) {
										process_it = PARSER_PROCESS_ICE_P_FIELD;						
										
									}
								}
								ice_p_nf--;
							} 
						}
					}								
				} // for
				nf--;
					
				// Process the identified received record
				//---------------------------------------
				if (process_it==PARSER_PROCESS_GENERAL_FIELD) {  // ***************************************						
					parser_status.no_field = process_record_tic_buf(
							nf,
							&(parser_status.pt_tic_buf_buf),
							pt_expfields,
							TB_GET_DESC(parser_status.pt_tic_buf),
							NULL);
							
				} else if (process_it==PARSER_IGNORE_FIELD) {  // *****************************************
					// Must Jump over (accept) currently parsed field
					// but stay on current expected field, has there could be multiple
					parser_status.no_field = nf;
					
				} else if (process_it==PARSER_PROCESS_ICE_P_FIELD) {  // ********************************* 
					// Add DATECOUR or DATE to the ice period p1 buffer (last modification)
					if (ice_p_nf > 3) {
						parser_status.ice_p_block_initialised = 
							ice_pp1_init_block(parser_status.pt_tic_buf,
		    					ice_p_nf,
		    					parser_status.ice_p_block_initialised,
								TB_GET_DESC(parser_status.ice_p_pt_tic_buf),
								TB_GET_BUF(parser_status.ice_p_pt_tic_buf));
					}
										
					// Manage differents ICE period p subfields fields
					parser_status.ice_p_no_field = process_record_tic_buf(
							ice_p_nf,
							&(parser_status.ice_p_pt_tic_buf_buf),
							ice_p_pt_expfields,
							TB_GET_DESC(parser_status.ice_p_pt_tic_buf),
							TB_GET_BUF(parser_status.ice_p_pt_tic_buf));
							
					// Continue main parsing on current label 
					parser_status.no_field = nf;
					
				} else if (process_it==PARSER_PROCESS_ICE_P1_FIELD) {  // ********************************* 
					// Add DATECOUR or DATE to the ice period p1 buffer (last modification)
					if (ice_p1_nf > 3) {
						parser_status.ice_p1_block_initialised = 
							ice_pp1_init_block(parser_status.pt_tic_buf,
		    					ice_p1_nf,
		    					parser_status.ice_p1_block_initialised,
								TB_GET_DESC(parser_status.ice_p1_pt_tic_buf),
								TB_GET_BUF(parser_status.ice_p1_pt_tic_buf));
					}
										
					// Manage differents ICE period p1 subfields fields
					parser_status.ice_p1_no_field = process_record_tic_buf(
							ice_p1_nf,
							&(parser_status.ice_p1_pt_tic_buf_buf),
							ice_p1_pt_expfields,
							TB_GET_DESC(parser_status.ice_p1_pt_tic_buf),
							TB_GET_BUF(parser_status.ice_p1_pt_tic_buf));
			
					// Continue main parsing on current label 
					parser_status.no_field = nf;
				}

										
				if (!process_it) {
					// TODO: Manage an error because of field not found
					tic_parser_reset();					
				} else {
					parser_status.parser_state = PEND_CR;
				}
				
			} else { // Chksum OK
				// Reject full current frame !!
				tic_parser_reset();				
// printf(", chks: KO\n");
			}

		break;

		default:
		   // Should not get here
		break;

	} // switch (parser_status.parser_state)
	

	// Return code 0 anycase for now ...
	return(0);
}

