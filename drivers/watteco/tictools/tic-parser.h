
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

#ifndef _TIC_PARSER_H_
#define _TIC_PARSER_H_

#include "tic-tools.h"

typedef enum
{
	PEND_STX,
	PEND_LF,
	PEND_LABEL,
	PEND_VALUE,
	PEND_CHECKSUM,
	PEND_CR,
	PEND_PROCESS,
	STOPPED
} TIC_parser_states_t;

// -----------------------------------------------------------------------------------
// Note about size: +1 for allocation as last byte for currently used size in buffer
#define TIC_PARSER_UNPACKED_BUF_SZ (TIC_DESC_NB_BYTES + TIC_FIELDS_UNPACKED_MAX_SIZE_BUF)
#define TIC_PARSER_PACKED_BUF_SZ (TIC_DESC_NB_BYTES + TIC_FIELDS_PACKED_MAX_SIZE_BUF)
// -----------------------------------------------------------------------------------

typedef struct
{	
	// Currently selected meter descriptor ==> Frame format
	TIC_meter_descriptor_t * pt_tic_descriptor;
	
	// Other working parameters
	unsigned char chksum_check;
	unsigned char nb_field_to_valid_frame;
	
	TIC_BUF_TYPE pt_tic_buf[TIC_PARSER_PACKED_BUF_SZ + 1];
	
    // Sub buffers ICE case								   
	TIC_BUF_TYPE ice_p_pt_tic_buf[TIC_PARSER_UNPACKED_BUF_SZ + 1]; 
	TIC_BUF_TYPE ice_p1_pt_tic_buf[TIC_PARSER_UNPACKED_BUF_SZ + 1]; 								   
	
	// Current parser state information
	TIC_parser_states_t parser_state; // Current parser state	
	unsigned char *pt_tic_buf_buf;    // Current free position in tic_buffer storage 
	unsigned char no_field;           // Current field in descriptor
	
	// Sub parser state ICE case
	unsigned char *ice_p_pt_tic_buf_buf;    // Current free position in tic_buffer storage 
	unsigned char ice_p_no_field;           // Current field in descriptor
	unsigned char ice_p_block_initialised;  // Current state of Energy/period block parsing
	 
	unsigned char *ice_p1_pt_tic_buf_buf;   // Current free position in tic_buffer storage
	unsigned char ice_p1_no_field;       	// Current field in descriptor
	unsigned char ice_p1_block_initialised;  // Current state of Energy/period block parsing
	
	// Used/Updated on end of frame completion 
	unsigned char tic_meter_type_discovered;  
	unsigned short tic_input_frame_size; 
} TIC_parser_status_t;

//----------------------------------------------------------------------------
// Get the current meter-type
TIC_meter_type_t tic_parser_get_meter_type();

//----------------------------------------------------------------------------
// The call back function for frame recpetion must be initialized by the client
void tic_parser_set_frame_rx_handler(void (*f)(TIC_parser_status_t * stat));

//----------------------------------------------------------------------------
// TIC input parsing management
void tic_parser_init(
	TIC_meter_descriptor_t * tic_desc,	
	unsigned char chks_check, 
	unsigned char nbfv);
void tic_parser_reset();
void tic_parser_set_chksum_check(unsigned char chks_check);
void tic_parser_set_nb_field_to_valid(unsigned char nbf); 

int tic_parser_input_char(unsigned char c);

#endif //  _TIC_PARSER_H_
