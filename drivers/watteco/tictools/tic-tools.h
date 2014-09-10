
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

#ifndef _TIC_TOOLS_H_
#define _TIC_TOOLS_H_

// -- Character constants used to parse or format TIC Frames -------------------------------
#ifdef TIC_TOOLS_FAKE_ETX_STX
	// No ETX/STX for minimal-net or console line input test
	// TODO: Thes mode may cause dysfunctions .. think about it when viewing/testing results
	#define	STX				'<'
	#define	ETX				'>'
	#define EOT 			0x04 // '!' : Dangerous for test with CRC. Uncomment to test EOT
#else
	#define	STX				0x02 /* Start of stream */
	#define	ETX				0x03 /* End of stream */
	#define EOT 			0x04 /* End of transmission => interruption de transmission */
#endif

#define	LF				0x0A
#define	CR				0x0D
#define	DELIM			0x20	

// -- I16<=>I16 Big Endian conversions (ignoring compilator alignement) -------------------------------------
#define I16_TO_I16p(i16,i16p) { \
	i16p[1] = (i16 & 0xFF); \
	i16p[0] = (i16 >> 8 ) & 0xFF ; \
}

#define I16p_TO_I16(i16p,i16) { \
	i16  = i16p[0]; i16 <<= 8; \
	i16 += i16p[1]; \
}
// -- U16<=>U16 Big Endian conversions (ignoring compilator alignement) -------------------------------------
#define U16_TO_U16p(u16,u16p) { \
	u16p[1] = (u16 & 0xFF); \
	u16p[0] = (u16 >> 8 ) & 0xFF ; \
}

#define U16p_TO_U16(u16p,u16) { \
	u16  = u16p[0]; u16 <<= 8; \
	u16 += u16p[1]; \
}

// -- U32<=>U24 Big Endian conversions  (ignoring compilator alignement) -------------------------------------
#define U32_TO_U24p(u32,u24p) { \
	u24p[2] = (u32 & 0xFF); \
	u24p[1] = (u32 >> 8 ) & 0xFF ; \
	u24p[0] = (u32 >> 16) & 0xFF ; \
}

#define U24p_TO_U32(u24p,u32) { \
	u32  = u24p[0]; u32 <<= 8; \
	u32 += u24p[1]; u32 <<= 8; \
	u32 += u24p[2]; \
}

// -- U32<=>U32 Big Endian conversions (ignoring compilator alignement) -------------------------------------
#define U32_TO_U32p(u32,u32p) { \
	u32p[3] = (u32 & 0xFF); \
	u32p[2] = (u32 >> 8 ) & 0xFF ; \
	u32p[1] = (u32 >> 16) & 0xFF ; \
	u32p[0] = (u32 >> 24) & 0xFF ; \
}

#define U32p_TO_U32(u32p,u32) { \
	u32  = u32p[0]; u32 <<= 8; \
	u32 += u32p[1]; u32 <<= 8; \
	u32 += u32p[2]; u32 <<= 8; \
	u32 += u32p[3]; \
}
	
// -------------------------------------------------------------------------------
// FIELD DESCRIPTORS and BUFFERS MANAGEMENT
// -------------------------------------------------------------------------------
// String and buffers sizes 
#define STR_VALUE_SZ_MAX	43 // 13 // 43 ==> CJE avec le ENERG !!!		
#define STR_LABEL_SZ_MAX	10
 
// Fileds descriptor management
#define TIC_DESC_NB_BYTES	(8) 
#define TIC_DESC_NB_BITS	(8 * TIC_DESC_NB_BYTES) // MUST be a multiple of 8 (1 byte)

// Depends on the typeS of meterS that the sensor must manage
// For now the ICE Meter presents the maximum possible size (Cf specifications)
#define TIC_FIELDS_UNPACKED_MAX_SIZE_BUF   158 /* taille max dans le cas des sub descripteurs ICE en accès absolu ! */
#define TIC_FIELDS_PACKED_MAX_SIZE_BUF     108 /* taille max dans le cas des descripteurs packed en général ! */
#define TIC_CRITERIA_PACKED_MAX_SIZE_BUF   24  /* taille max dans le cas des citères ! */

typedef union {
	unsigned char  u8 [TIC_DESC_NB_BYTES];
	unsigned short u16[TIC_DESC_NB_BYTES / 2];
	unsigned long  u32[TIC_DESC_NB_BYTES / 4];
} TIC_desc_t;

typedef unsigned char TIC_BUF_TYPE;

#define TB_GET_DESC(tb) (tb)
#define TB_GET_BUF(tb) (&(tb[TIC_DESC_NB_BYTES]))

#define TB_CRIT_GET_FILTER_DESC(tb) (tb)
#define TB_CRIT_GET_TRIG_DESC(tb)	(&(tb[TIC_DESC_NB_BYTES]))
#define TB_CRIT_GET_TRIG_BUF(tb)	(&(tb[TIC_DESC_NB_BYTES + TIC_DESC_NB_BYTES]))

// Bits access in descriptor assuming a BIG endian storing in RAM
// TODO: Not really optimized (time consumming) implement faster "shifting access" to bits if possible
//       NOTA: First step '/8' replaced by '>>3'
#define TIC_FIELD_GET_BIT(ba,fnum) ((ba)[(TIC_DESC_NB_BYTES -1) - (((unsigned char)fnum)>>3)]  &  (((unsigned char)(0x01)) << (((unsigned char)fnum)%8))) 
#define TIC_FIELD_SET_BIT(ba,fnum) ((ba)[(TIC_DESC_NB_BYTES -1) - (((unsigned char)fnum)>>3)] |=  (((unsigned char)(0x01)) << (((unsigned char)fnum)%8)))
#define TIC_FIELD_CLR_BIT(ba,fnum) ((ba)[(TIC_DESC_NB_BYTES -1) - (((unsigned char)fnum)>>3)] &= ~(((unsigned char)(0x01)) << (((unsigned char)fnum)%8)))

// 2 Reserved big endian bits for specific signification ==> only 62 TIC FILEDS possibles !!!
#define DESC_OBSOLETE_BIT 63   // if OBSOLETE = 1 the buffer is condidered as obsolete. See specifications. This bit is applicatively managed
#define DESC_UNPACKED_BIT 62   // if UNPACKED = 1 the buffer is not packed. ie. data are set at the absolute position. Only used for ICE sub buffers parsing 

// TIC binary data management
typedef enum {
	TYPE_EMPTY = 0,
	TYPE_CHAR,
	TYPE_CSTRING,
	TYPE_U8,
	TYPE_U16,
	TYPE_I16,
	TYPE_U24,
	TYPE_4U24,
	TYPE_6U24,
	TYPE_U32,
	TYPE_DMYhms,
	TYPE_hmDM,
	TYPE_DMh,
	TYPE_hm
} TIC_binary_type_t;

// -1 => '\0' terminated string
// -2 => specific type must be dynammically calculated
#define TIC_GET_TYPE_OFFSET(type) \
	(type == TYPE_EMPTY            ? 0  : \
	 (type == TYPE_CSTRING         ? -1 : \
	  (type == TYPE_CHAR           ? 1  : \
	   (type == TYPE_U8            ? 1  : \
	    (type == TYPE_U16          ? 2  : \
	     (type == TYPE_I16         ? 2  : \
	      (type == TYPE_U24        ? 3  : \
	       (type == TYPE_U32       ? 4  : \
	        (type == TYPE_DMYHMS   ? 6 : -2)))))))))


typedef struct {
	char label[STR_LABEL_SZ_MAX];
	TIC_binary_type_t type;
	unsigned char attribute;
	const char * fmt;
	unsigned char abs_pos;
} TIC_expected_field_t;

#define ATTRIBUTE_NORMAL 0x00
#define ATTRIBUTE_IS_SUBFIELD 0x01
#define ATTRIBUTE_NOT_MANAGED_FIELD 0x02
#define ATTRIBUTE_ICE_pp1_FIELD 0x04
#define ATTRIBUTE_NON_TIC_FIELD 0x08


#define EXCEPTION_VARIABLE_FIELD "!_vf_"


// -------------------------------------------------------------------------------
// MULTIPLE METER TYPES MANAGEMENT
// -------------------------------------------------------------------------------
typedef enum {
	MT_UNKNOWN=0,
	MT_CT,			// Concentrateur Teleport
	MT_CBEMM,		// Compteur Bleu Electronique Monophasé Multitarif
	MT_CBEMM_ICC,	// Compteur Bleu Electronique Monophasé  Multitarif extension ICC 
	MT_CBETM,		// Compteur Bleu Electronique Triphasé Multitarif
	MT_CJE,			// Compteur Jaune
	MT_ICE,			// Compteur Clientelle Emeraude
	MT_NULL,	    // Specific NULL meter type (used for delimiting end of meter type and used for search)

    // Following descriptors are used to manage specifique Energies values for ICE
	MT_ICE_p,      // Compteur Clientelle Emerqude: Energies Period P extract
	MT_ICE_p1,     // Compteur Clientelle Emerqude: Energies Period P-1 extract
	
	MT_END
} TIC_meter_type_t;

typedef struct {
	const TIC_meter_type_t meter_type;	
	const TIC_expected_field_t * expected_fields;
	const unsigned char expected_fields_number;	
}TIC_meter_descriptor_t;

#define TIC_NB_DESC_TYPE (MT_END)
#define TIC_NB_MANAGED_METER (MT_NULL)

extern const TIC_meter_descriptor_t tic_meter_descriptor[TIC_NB_DESC_TYPE];

//----------------------------------------------------------------------------
// Various TIC management functions
//----------------------------------------------------------------------------
unsigned char tic_frame_rx_expected_period(TIC_meter_type_t mt);
TIC_meter_descriptor_t * tic_metertype_to_meterdesc(TIC_meter_type_t mt);
void tic_set_expfields_on_label(const TIC_expected_field_t ** hndl_expfields, unsigned char * nb_expfields, char * label);
TIC_meter_descriptor_t * find_meter_desc(const TIC_expected_field_t * pt_expfields, TIC_desc_t *desc);

unsigned char tic_str_compute_checksum(char * strbuf);
unsigned char tic_nb_fields_selected(TIC_desc_t * desc);
unsigned char * tic_get_value_at_index(TIC_meter_descriptor_t * pt_tic_descriptor,
				TIC_BUF_TYPE *tb, 
				unsigned char idx);
unsigned char tic_generic_string_match(unsigned char * sgen,unsigned char * s);
unsigned char tic_check_changes(TIC_meter_descriptor_t * pt_tic_descriptor, 
				TIC_BUF_TYPE *tbcrit,
				TIC_BUF_TYPE *tbbefore,
				TIC_BUF_TYPE *tbafter,
				TIC_desc_t * result);
signed short tic_deserialize(TIC_meter_descriptor_t * pt_tic_descriptor, 
				TIC_BUF_TYPE *tb, 
				unsigned char* buf,
				short buf_size);
signed short tic_serialize(TIC_meter_descriptor_t * pt_tic_descriptor, 
				unsigned char* buf, 
				TIC_BUF_TYPE *tb, TIC_desc_t * filter,
				short buf_size); 			
signed short tic_serialize_report_criteria(TIC_meter_descriptor_t * pt_tic_descriptor, 
	unsigned char* buf,
	TIC_BUF_TYPE *tb, 
	short buf_size);


#endif // _TIC_TOOLS_H_
