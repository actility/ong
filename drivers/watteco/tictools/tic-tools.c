
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
 * 		TIC tools for variable TIC frames management
 * \author
 *      P.E. Goudet <pe.goudet@watteco.com>
 */

/*============================ INCLUDE =======================================*/

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "tic-tools.h"

// ** Data management (very simplistic)
// TODO: really manage timestamps ... maybe heavy ?
const unsigned char	monthdays[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

// TODO: Manage correct ICE atributes and formating
/* 
 * Formats (Atributs ?) à prévoir pour ICE
- ICE -------
JJ/MM/AA HH/MM/SS : DATE       : Dates
xxxxxYkw     : PAX            : puissance moyennes actives
xxxxxxxkWh   : EA,EAp,EAp1    : Indexes energies actives
xxxxxxxkWh   : ERP,ERPp,ERPp1 : Indexes energies reactives positives
xxxxxxxkvarh : ERNp,ERNp1     : Indexes energies reactives negatives
xxxxxkW      : PS,PA          : Puissances actives
xxxxxkvar    : PREA           : Puissances reactives (Signées -32768/+32767)
xxxxxxV      : U              : Tension
xxx%         : KDC, KDCD      : Coefficient ...
xxxxx,xx     : TGPHI          : TG Phi Réel Positif/Negatif
*/

const char FMT_UNDEF[]     = "";

const char FMT_s[]     = "%s";
const char FMT_c[]     = "%c";
const char FMT_02x[]   = "%02x";
const char FMT_d[]     = "%d";
const char FMT_ld[]    = "%ld";

const char FMT_02d[]   = "%02d";
const char FMT_03d[]   = "%03d";
const char FMT_05d[]   = "%05d";
const char FMT_07d[]   = "%07d"; 
const char FMT_09d[]   = "%09d";
 
const char FMT_05ld[]  = "%05ld";
const char FMT_06ld[]  = "%06ld"; 
 
const char FMT_d_percent[] = "%d%%";

const char FMT_d_kW[]    = "%dkW";
const char FMT_d_kvar[]  = "%dkvar";

const char FMT_05d_kwh[]   = "%05ldkwh";

const char FMT_d_V[]       = "%dV";
 
const char FMT_ld_kWh[]   = "%07ldkWh"; 
const char FMT_ld_kvarh[] = "%07ldkvarh";

const char FMT_05_2f[]      = "%05.2f";


// ** Unknown meter *******************************************************
// **************************************************************************
#define UNKNOWN_FIELDS_MAX_NUMBER 0
const TIC_expected_field_t UNKNOWN_expected_fields[UNKNOWN_FIELDS_MAX_NUMBER];

// ** Concentrateur Teleport / Compteur Bleu Electronique CT/CBE ************
// **************************************************************************
#define CTCBE_FIELDS_MAX_NUMBER 36
const TIC_expected_field_t CTCBE_expected_fields[CTCBE_FIELDS_MAX_NUMBER]=
{
	/* Should be ordened compared to real stream to optimize parsing (cf no_field which is not re-initialized )*/
	// Byte 0
	{"ADIR1",   TYPE_U16,0,FMT_03d},
	{"ADIR2",   TYPE_U16,0,FMT_03d},
	{"ADIR3",   TYPE_U16,0,FMT_03d},
	{"ADCO",    TYPE_CSTRING,0,FMT_s},
	{"OPTARIF", TYPE_CSTRING,0,FMT_s},
	{"ISOUSC",  TYPE_U8,0,FMT_02d},
	{"BASE",    TYPE_U32,0,FMT_09d},     
	{"HCHC",    TYPE_U32,0,FMT_09d},
	// Byte 1
	{"HCHP",    TYPE_U32,0,FMT_09d},
	{"EJPHN",   TYPE_U32,0,FMT_09d},
	{"EJPHPM",	TYPE_U32,0,FMT_09d},
	{"BBRHCJB",	TYPE_U32,0,FMT_09d},
	{"BBRHPJB",	TYPE_U32,0,FMT_09d},
	{"BBRHCJW", TYPE_U32,0,FMT_09d},
	{"BBRHPJW",	TYPE_U32,0,FMT_09d},
	{"BBRHCJR", TYPE_U32,0,FMT_09d},
	// Byte 2
	{"BBRHPJR", TYPE_U32,0,FMT_09d},
	{"PEJP",    TYPE_U8,0,FMT_02d},
	{"GAZ",    	TYPE_U32,0,FMT_07d},
	{"AUTRE",   TYPE_U32,0,FMT_07d},
	{"PTEC",    TYPE_CSTRING,0,FMT_s},
	{"DEMAIN",  TYPE_CSTRING,0,FMT_s},
	{"IINST",   TYPE_U16,0,FMT_03d},
	{"IINST1",  TYPE_U16,0,FMT_03d},
	// Byte 3
	{"IINST2",	TYPE_U16,0,FMT_03d},
	{"IINST3",  TYPE_U16,0,FMT_03d},
	{"ADPS",    TYPE_U16,0,FMT_03d},
	{"IMAX",    TYPE_U16,0,FMT_03d},
	{"IMAX1",   TYPE_U16,0,FMT_03d},
	{"IMAX2",   TYPE_U16,0,FMT_03d},
	{"IMAX3",   TYPE_U16,0,FMT_03d},
	{"PMAX",    TYPE_U32,0,FMT_05d},
	// Byte 4
	{"PAPP",    TYPE_U32,0,FMT_05d},
	{"HHPHC",   TYPE_CHAR,0,FMT_c},    
	{"MOTDETAT",TYPE_CSTRING,0,FMT_s},
	{"PPOT",    TYPE_U8,0,FMT_02x}
};
	

// ** Compteur Jaune Electronique *******************************************
// **************************************************************************
#define CJE_FIELDS_MAX_NUMBER 37
const TIC_expected_field_t CJE_expected_fields[CJE_FIELDS_MAX_NUMBER]=
{
	/* Should be ordened compared to real stream to optimize parsing (cf no_field which is not re-initialized )*/
	// Byte 0
	{"JAUNE",	TYPE_hmDM,0,FMT_UNDEF},		// {hh:mn:jj:mm}:pt:dp:abcde:kp
	{"JAUNE",	TYPE_CSTRING,ATTRIBUTE_IS_SUBFIELD,FMT_s},	// pt
	{"JAUNE",	TYPE_CSTRING,ATTRIBUTE_IS_SUBFIELD,FMT_s},  // dp
	{"JAUNE",	TYPE_U24,ATTRIBUTE_IS_SUBFIELD,FMT_05ld},	// abcde
	{"JAUNE",	TYPE_U8,ATTRIBUTE_IS_SUBFIELD,FMT_02d},		// kp
	{"ENERG",	TYPE_6U24,0,FMT_06ld},		// 111111:222222:...:666666
	{"ENERG",	TYPE_U24,ATTRIBUTE_IS_SUBFIELD,FMT_06ld},		// 222222
	{"ENERG",	TYPE_U24,ATTRIBUTE_IS_SUBFIELD,FMT_06ld},		// 333333
	// Byte 1
	{"ENERG",	TYPE_U24,ATTRIBUTE_IS_SUBFIELD,FMT_06ld},		// 444444
	{"ENERG",	TYPE_U24,ATTRIBUTE_IS_SUBFIELD,FMT_06ld},		// 555555
	{"ENERG",	TYPE_U24,ATTRIBUTE_IS_SUBFIELD,FMT_06ld},		// 666666
	{"PERCC",	TYPE_DMh,0,FMT_UNDEF},		// jj:mm:hh{:cg}
	{"PERCC",	TYPE_U8,ATTRIBUTE_IS_SUBFIELD,FMT_02d},		// cg
	{"PMAXC",	TYPE_4U24,0,FMT_05ld},		// 11111:22222:...:44444
	{"PMAXC",	TYPE_U24,ATTRIBUTE_IS_SUBFIELD,FMT_05ld},		// 22222
	{"PMAXC",	TYPE_U24,ATTRIBUTE_IS_SUBFIELD,FMT_05ld},		// 33333
	// Byte 2
	{"PMAXC",	TYPE_U24,ATTRIBUTE_IS_SUBFIELD,FMT_05ld},		// 44444
	{"TDEPA",	TYPE_4U24,0,FMT_05ld},		// 11111:22222:...:44444
	{"TDEPA",	TYPE_U24,ATTRIBUTE_IS_SUBFIELD,FMT_05ld},		// 22222
	{"TDEPA",	TYPE_U24,ATTRIBUTE_IS_SUBFIELD,FMT_05ld},		// 33333
	{"TDEPA",	TYPE_U24,ATTRIBUTE_IS_SUBFIELD,FMT_05ld},		// 44444
	{"PERCP",	TYPE_DMh,0,FMT_UNDEF},		// {jj:mm:hh}:cg
	{"PERCP",	TYPE_U8,ATTRIBUTE_IS_SUBFIELD,FMT_02d},		// cg
	{"PMAXP",	TYPE_4U24,0,FMT_05ld},		// 11111:22222:...:44444
	// Byte 3
	{"PMAXP",	TYPE_U24,ATTRIBUTE_IS_SUBFIELD,FMT_05ld},		// 22222
	{"PMAXP",	TYPE_U24,ATTRIBUTE_IS_SUBFIELD,FMT_05ld},		// 33333
	{"PMAXP",	TYPE_U24,ATTRIBUTE_IS_SUBFIELD,FMT_05ld},		// 44444
	{"PSOUSC",	TYPE_4U24,0,FMT_05ld},		// 11111:22222:...:44444
	{"PSOUSC",	TYPE_U24,ATTRIBUTE_IS_SUBFIELD,FMT_05ld},		// 22222
	{"PSOUSC",	TYPE_U24,ATTRIBUTE_IS_SUBFIELD,FMT_05ld},		// 33333
	{"PSOUSC",	TYPE_U24,ATTRIBUTE_IS_SUBFIELD,FMT_05ld},		// 44444
	{"PSOUSP",	TYPE_4U24,0,FMT_05ld},		// 11111:22222:...:44444
	// Byte 4
	{"PSOUSP",	TYPE_U24,ATTRIBUTE_IS_SUBFIELD,FMT_05ld},		// 22222
	{"PSOUSP",	TYPE_U24,ATTRIBUTE_IS_SUBFIELD,FMT_05ld},		// 33333
	{"PSOUSP",	TYPE_U24,ATTRIBUTE_IS_SUBFIELD,FMT_05ld},		// 44444
	{"FCOU",	TYPE_hm,0,FMT_UNDEF},		// {hh:mn}:dd
	{"FCOU",	TYPE_U8,ATTRIBUTE_IS_SUBFIELD,FMT_02d}		// dd
};

// ** Interface Client Emeraude *********************************************
// **************************************************************************
#define ICE_FIELDS_MAX_NUMBER 45
const TIC_expected_field_t ICE_expected_fields[ICE_FIELDS_MAX_NUMBER]=
{
	/* Should be ordened compared to real stream to optimize parsing (cf no_field which is not re-initialized )*/
	// Byte 0
	{"CONTRAT",	TYPE_CSTRING,0,FMT_s},
	{"DATECOUR",TYPE_DMYhms,0,FMT_UNDEF},
	{"DATE",TYPE_DMYhms,0,FMT_UNDEF}, /* A la place de DATECOUR sur compteurs v2.4 */
	{"EA",		TYPE_U24,0,FMT_05ld },
	{"ERP",		TYPE_U24,0,FMT_05ld },
	{"PTCOUR",	TYPE_CSTRING,0,FMT_s},
	{"PREAVIS",	TYPE_CSTRING,0,FMT_s},
	{"MODE",	TYPE_EMPTY,0,FMT_s},
	// Byte 1
	// TODO: Manage PAX format with a complémentary byte info see documentation
	//       Indicateur de puissance tronqué ?
	{"DATEPA1",	TYPE_DMYhms,0,FMT_UNDEF}, 
	{"PA1",		TYPE_U16,0,FMT_d_kW},
	{"DATEPA2",	TYPE_DMYhms,0,FMT_UNDEF},
	{"PA2",		TYPE_U16,0,FMT_d_kW},
	{"DATEPA3",	TYPE_DMYhms,0,FMT_UNDEF},
	{"PA3",		TYPE_U16,0,FMT_d_kW},
	{"DATEPA4",	TYPE_DMYhms,0,FMT_UNDEF},
	{"PA4",		TYPE_U16,0,FMT_d_kW},
	// Byte 2
	{"DATEPA5",	TYPE_DMYhms,0,FMT_UNDEF},
	{"PA5",		TYPE_U16,0,FMT_d_kW},
	{"DATEPA6",	TYPE_DMYhms,0,FMT_UNDEF},
	{"PA6",		TYPE_U16,0,FMT_d_kW},
    // Ce champs variable permet d'ignorer et "switcher" sur le descripteur ICE/Energie/Periode
	{"*p*",	TYPE_U24,ATTRIBUTE_ICE_pp1_FIELD,FMT_05d},
	//{"*p*",	TYPE_U24,ATTRIBUTE_NOT_MANAGED_FIELD,FMT_05d},
	{"KDC",		TYPE_U8,0,FMT_d_percent},
	{"KDCD",	TYPE_U8,0,FMT_d_percent},
	{"TGPHI",	TYPE_CSTRING,0,FMT_s}, /* Optionnellement ici sur compteurs v2.4 */
	// Byte 3
	{"PSP",		TYPE_U16,0,FMT_d_kW},
	{"PSPM",	TYPE_U16,0,FMT_d_kW},
	{"PSHPH",	TYPE_U16,0,FMT_d_kW},
	{"PSHPD",	TYPE_U16,0,FMT_d_kW},
	{"PSHCH",	TYPE_U16,0,FMT_d_kW},
	{"PSHCD",	TYPE_U16,0,FMT_d_kW},
	{"PSHPE",	TYPE_U16,0,FMT_d_kW},
	{"PSHCE",	TYPE_U16,0,FMT_d_kW},
	// Byte 4
	{"PSJA",	TYPE_U16,0,FMT_d_kW},
	{"PSHH",	TYPE_U16,0,FMT_d_kW},
	{"PSHD",	TYPE_U16,0,FMT_d_kW},
	{"PSHM",	TYPE_U16,0,FMT_d_kW},
	{"PSDSM",	TYPE_U16,0,FMT_d_kW},
	{"PSSCM",	TYPE_U16,0,FMT_d_kW}, /* Optionnellement ici sur compteurs v2.4 */
	{"MODE",	TYPE_EMPTY,0,FMT_s},
	{"PA1MN",	TYPE_U16,0,FMT_d_kW},
	// Byte 5
	{"PA10MN",	TYPE_U16,0,FMT_d_kW},
	{"PREA1MN",	TYPE_I16,0,FMT_d_kvar},
	{"PREA10MN",TYPE_I16,0,FMT_d_kvar},
	// TODO: FLOAT management (tic-parser,tic-formater,tic-tools))
	// {"TGPHI",	TYPE_FLOAT,0,FMT_05_2f},
	{"TGPHI",	TYPE_CSTRING,0,FMT_s},
	{"U10MN",	TYPE_U16,0,FMT_d_V}	
};

#define ICE_p_FIELDS_MAX_NUMBER  (3 + 3 + (14*3))
const TIC_expected_field_t ICE_p_expected_fields[ICE_p_FIELDS_MAX_NUMBER]=
{
	//Byte 0
	{"DEBUTp",	TYPE_DMYhms,0,FMT_UNDEF,0},
	{"FINp",	TYPE_DMYhms,0,FMT_UNDEF,6},
	{"CAFp",	TYPE_U16,0,FMT_05d,12},
	
	{"DATEEAp",TYPE_DMYhms,ATTRIBUTE_NON_TIC_FIELD,FMT_UNDEF,14},
	{"EApP",	TYPE_U24,0,FMT_ld_kWh,20},
	{"EApPM",	TYPE_U24,0,FMT_ld_kWh,23},
	{"EApHCE",	TYPE_U24,0,FMT_ld_kWh,26},
	{"EApHCH",	TYPE_U24,0,FMT_ld_kWh,29},
	//Byte 1
	{"EApHH",	TYPE_U24,0,FMT_ld_kWh,32},
	{"EApHCD",	TYPE_U24,0,FMT_ld_kWh,35},
	{"EApHD",	TYPE_U24,0,FMT_ld_kWh,38},
	{"EApJA",	TYPE_U24,0,FMT_ld_kWh,41},
	{"EApHPE",	TYPE_U24,0,FMT_ld_kWh,44},
	{"EApHPH",	TYPE_U24,0,FMT_ld_kWh,47},
	{"EApHPD",	TYPE_U24,0,FMT_ld_kWh,50},
	{"EApSCM",	TYPE_U24,0,FMT_ld_kWh,53},
	// Byte 2
	{"EApHM",	TYPE_U24,0,FMT_ld_kWh,56},
	{"EApDSM",	TYPE_U24,0,FMT_ld_kWh,59},
	
	{"DATEERPp",TYPE_DMYhms,ATTRIBUTE_NON_TIC_FIELD,FMT_UNDEF,62},
	{"ERPpP",	TYPE_U24,0,FMT_ld_kvarh,68},
	{"ERPpPM",	TYPE_U24,0,FMT_ld_kvarh,71},
	{"ERPpHCE",	TYPE_U24,0,FMT_ld_kvarh,74},
	{"ERPpHCH",	TYPE_U24,0,FMT_ld_kvarh,77},
	{"ERPpHH",	TYPE_U24,0,FMT_ld_kvarh,80},
	// Byte 3
	{"ERPpHCD",	TYPE_U24,0,FMT_ld_kvarh,83},
	{"ERPpHD",	TYPE_U24,0,FMT_ld_kvarh,86},
	{"ERPpJA",	TYPE_U24,0,FMT_ld_kvarh,89},
	{"ERPpHPE",	TYPE_U24,0,FMT_ld_kvarh,92},
	{"ERPpHPH",	TYPE_U24,0,FMT_ld_kvarh,95},
	{"ERPpHPD",	TYPE_U24,0,FMT_ld_kvarh,98},
	{"ERPpSCM",	TYPE_U24,0,FMT_ld_kvarh,101},
	{"ERPpHM",	TYPE_U24,0,FMT_ld_kvarh,104},
	// Byte 4
	{"ERPpDSM",	TYPE_U24,0,FMT_ld_kvarh,107},
		
	{"DATEERNp",TYPE_DMYhms,ATTRIBUTE_NON_TIC_FIELD,FMT_UNDEF,110},
	{"ERNpP",	TYPE_U24,0,FMT_ld_kvarh,116},
	{"ERNpPM",	TYPE_U24,0,FMT_ld_kvarh,119},
	{"ERNpHCE",	TYPE_U24,0,FMT_ld_kvarh,122},
	{"ERNpHCH",	TYPE_U24,0,FMT_ld_kvarh,125},
	{"ERNpHH",	TYPE_U24,0,FMT_ld_kvarh,128},
	{"ERNpHCD",	TYPE_U24,0,FMT_ld_kvarh,131},
	// Byte 5
	{"ERNpHD",	TYPE_U24,0,FMT_ld_kvarh,134},
	{"ERNpJA",	TYPE_U24,0,FMT_ld_kvarh,137},
	{"ERNpHPE",	TYPE_U24,0,FMT_ld_kvarh,140},
	{"ERNpHPH",	TYPE_U24,0,FMT_ld_kvarh,143},
	{"ERNpHPD",	TYPE_U24,0,FMT_ld_kvarh,146},
	{"ERNpSCM",	TYPE_U24,0,FMT_ld_kvarh,149},
	{"ERNpHM",	TYPE_U24,0,FMT_ld_kvarh,152},
	{"ERNpDSM",	TYPE_U24,0,FMT_ld_kvarh,155}	
	// Byte 6
};

#define ICE_p1_FIELDS_MAX_NUMBER (3 + 3 + (14*3))
const TIC_expected_field_t ICE_p1_expected_fields[ICE_p1_FIELDS_MAX_NUMBER]=
{
	//Byte 0
	{"DEBUTp1",	TYPE_DMYhms,0,FMT_UNDEF,0},
	{"FINp1",	TYPE_DMYhms,0,FMT_UNDEF,6},
	{"CAFp1",	TYPE_U16,0,FMT_05d,12},
	
	{"DATEEAp1",TYPE_DMYhms,ATTRIBUTE_NON_TIC_FIELD,FMT_UNDEF,14},
	{"EAp1P",	TYPE_U24,0,FMT_ld_kWh,20},
	{"EAp1PM",	TYPE_U24,0,FMT_ld_kWh,23},
	{"EAp1HCE",	TYPE_U24,0,FMT_ld_kWh,26},
	{"EAp1HCH",	TYPE_U24,0,FMT_ld_kWh,29},
	//Byte 1
	{"EAp1HH",	TYPE_U24,0,FMT_ld_kWh,32},
	{"EAp1HCD",	TYPE_U24,0,FMT_ld_kWh,35},
	{"EAp1HD",	TYPE_U24,0,FMT_ld_kWh,38},
	{"EAp1JA",	TYPE_U24,0,FMT_ld_kWh,41},
	{"EAp1HPE",	TYPE_U24,0,FMT_ld_kWh,44},
	{"EAp1HPH",	TYPE_U24,0,FMT_ld_kWh,47},
	{"EAp1HPD",	TYPE_U24,0,FMT_ld_kWh,50},
	{"EAp1SCM",	TYPE_U24,0,FMT_ld_kWh,53},
	// Byte 2
	{"EAp1HM",	TYPE_U24,0,FMT_ld_kWh,56},
	{"EAp1DSM",	TYPE_U24,0,FMT_ld_kWh,59},
	
	{"DATEERPp1",TYPE_DMYhms,ATTRIBUTE_NON_TIC_FIELD,FMT_UNDEF,62},
	{"ERPp1P",	TYPE_U24,0,FMT_ld_kvarh,68},
	{"ERPp1PM",	TYPE_U24,0,FMT_ld_kvarh,71},
	{"ERPp1HCE",TYPE_U24,0,FMT_ld_kvarh,74},
	{"ERPp1HCH",TYPE_U24,0,FMT_ld_kvarh,77},
	{"ERPp1HH",	TYPE_U24,0,FMT_ld_kvarh,80},
	// Byte 3
	{"ERPp1HCD",TYPE_U24,0,FMT_ld_kvarh,83},
	{"ERPp1HD",	TYPE_U24,0,FMT_ld_kvarh,86},
	{"ERPp1JA",	TYPE_U24,0,FMT_ld_kvarh,89},
	{"ERPp1HPE",TYPE_U24,0,FMT_ld_kvarh,92},
	{"ERPp1HPH",TYPE_U24,0,FMT_ld_kvarh,95},
	{"ERPp1HPD",TYPE_U24,0,FMT_ld_kvarh,98},
	{"ERPp1SCM",TYPE_U24,0,FMT_ld_kvarh,101},
	{"ERPp1HM",	TYPE_U24,0,FMT_ld_kvarh,104},
	// Byte 4
	{"ERPp1DSM",TYPE_U24,0,FMT_ld_kvarh,107},
		
	{"DATEERNp1",TYPE_DMYhms,ATTRIBUTE_NON_TIC_FIELD,FMT_UNDEF,110},
	{"ERNp1P",	TYPE_U24,0,FMT_ld_kvarh,116},
	{"ERNp1PM",	TYPE_U24,0,FMT_ld_kvarh,119},
	{"ERNp1HCE",TYPE_U24,0,FMT_ld_kvarh,122},
	{"ERNp1HCH",TYPE_U24,0,FMT_ld_kvarh,125},
	{"ERNp1HH",	TYPE_U24,0,FMT_ld_kvarh,128},
	{"ERNp1HCD",TYPE_U24,0,FMT_ld_kvarh,131},
	// Byte 5
	{"ERNp1HD",	TYPE_U24,0,FMT_ld_kvarh,134},
	{"ERNp1JA",	TYPE_U24,0,FMT_ld_kvarh,137},
	{"ERNp1HPE",TYPE_U24,0,FMT_ld_kvarh,140},
	{"ERNp1HPH",TYPE_U24,0,FMT_ld_kvarh,143},
	{"ERNp1HPD",TYPE_U24,0,FMT_ld_kvarh,146},
	{"ERNp1SCM",TYPE_U24,0,FMT_ld_kvarh,149},
	{"ERNp1HM",	TYPE_U24,0,FMT_ld_kvarh,152},
	{"ERNp1DSM",TYPE_U24,0,FMT_ld_kvarh,155}	
	// Byte 6	
};

// ** Table of managed meter types ****************************************
// **************************************************************************
const TIC_meter_descriptor_t tic_meter_descriptor[TIC_NB_DESC_TYPE] = 
{
	{MT_UNKNOWN,	UNKNOWN_expected_fields,	UNKNOWN_FIELDS_MAX_NUMBER}, 
	{MT_CT, 		CTCBE_expected_fields,		CTCBE_FIELDS_MAX_NUMBER}, 
	{MT_CBEMM, 		CTCBE_expected_fields,		CTCBE_FIELDS_MAX_NUMBER}, 
	{MT_CBEMM_ICC, 	CTCBE_expected_fields,		CTCBE_FIELDS_MAX_NUMBER}, 
	{MT_CBETM, 		CTCBE_expected_fields,		CTCBE_FIELDS_MAX_NUMBER}, 
	{MT_CJE,		CJE_expected_fields,		CJE_FIELDS_MAX_NUMBER}, 
	{MT_ICE,		ICE_expected_fields,		ICE_FIELDS_MAX_NUMBER},
	{MT_NULL,		NULL, 0}, // End of managed Meter Begin of complementary managed descriptor
	{MT_ICE_p,		ICE_p_expected_fields,		ICE_p_FIELDS_MAX_NUMBER},
	{MT_ICE_p1,		ICE_p1_expected_fields,		ICE_p1_FIELDS_MAX_NUMBER}
};


//----------------------------------------------------------------------------
// Use to calculate all timeouts relative to TIC Frame réception
unsigned char tic_frame_rx_expected_period(TIC_meter_type_t mt){
	return( mt == MT_UNKNOWN ? 12 : /* Max of frame transmission for ICE */
		  ( mt == MT_NULL ? 12 :    /* Max of frame transmission for ICE */
		  ( mt == MT_CT ? 2 :
		  ( mt == MT_CBEMM ? 2 :
		  ( mt == MT_CBEMM_ICC ? 2 :
		  ( mt == MT_CBETM ? 2 :
		  ( mt == MT_CJE ? 5 :
		  ( mt == MT_ICE ? 12 :
		  ( 12 )))))))));		  
}

//----------------------------------------------------------------------------
TIC_meter_descriptor_t * tic_metertype_to_meterdesc(TIC_meter_type_t mt){
	return (TIC_meter_descriptor_t *) (mt < MT_END ? &(tic_meter_descriptor[mt]) :  NULL );
};


//----------------------------------------------------------------------------
void tic_set_expfields_on_label(const TIC_expected_field_t ** hndl_expfields, unsigned char * nb_expfields, char * label) {
	unsigned char ndesc = 0, nf = 0;	
	const TIC_expected_field_t * pt_expfields = NULL;
			
	while ((ndesc < TIC_NB_MANAGED_METER)) {
//printf("ndesc:%d, ",ndesc); 
		if (pt_expfields != tic_meter_descriptor[ndesc].expected_fields) {
			pt_expfields = tic_meter_descriptor[ndesc].expected_fields;
			*nb_expfields = tic_meter_descriptor[ndesc].expected_fields_number;
			
//printf("*nb_expfields:%d, nf: ",*nb_expfields); 
			for (nf = 0; (nf < (5 < *nb_expfields ? 5 : *nb_expfields)); nf++) { /* Should be found in first 5 fields !! */
//printf("%d (%s==%s) ,",nf,pt_expfields[nf].label,label); 
				if (strcmp(pt_expfields[nf].label,label) == 0) break;
			}
			if (nf < (5 < *nb_expfields ? 5 : *nb_expfields)) break; 
		}						
		ndesc++;
//printf("\n");
	}
	if (ndesc == TIC_NB_MANAGED_METER) {
		pt_expfields = NULL; *nb_expfields = 0;
	}
	*hndl_expfields = pt_expfields;
}

//----------------------------------------------------------------------------
TIC_meter_descriptor_t * find_meter_desc(const TIC_expected_field_t * pt_expfields, TIC_desc_t *desc){
	
	unsigned char * tbdesc = desc->u8;
	
	if (tic_meter_descriptor[0].expected_fields == pt_expfields)
		return (tic_metertype_to_meterdesc(MT_UNKNOWN));
		
	if (tic_meter_descriptor[1].expected_fields == pt_expfields) {
		//MT_CBETM:     ADCO (b3), IINST1(b23)  (Pour accepter alerte ou trame nomale)
		if ((TIC_FIELD_GET_BIT(tbdesc, 3) != 0) && (TIC_FIELD_GET_BIT(tbdesc, 23) != 0)) 
			return (tic_metertype_to_meterdesc(MT_CBETM));
			
		//MT_CBEMM_ICC: ADCO (b3), PAPP(B32)
		if ((TIC_FIELD_GET_BIT(tbdesc, 3) != 0) && (TIC_FIELD_GET_BIT(tbdesc, 32) != 0)) 
			return (tic_metertype_to_meterdesc(MT_CBEMM_ICC));
			
		//MT_CBEMM:     ADCO (b3), IINST(b22)
		if ((TIC_FIELD_GET_BIT(tbdesc, 3) != 0) && (TIC_FIELD_GET_BIT(tbdesc, 22) != 0)) 
			return (tic_metertype_to_meterdesc(MT_CBEMM));
			
		//MT_CT:        ADCO (b3)
		if ((TIC_FIELD_GET_BIT(tbdesc, 3) != 0) != 0) 
			return (tic_metertype_to_meterdesc(MT_CT));		
	} 

	if (tic_meter_descriptor[5].expected_fields == pt_expfields) 
		return (tic_metertype_to_meterdesc(MT_CJE));

	if (tic_meter_descriptor[6].expected_fields == pt_expfields) 
		return (tic_metertype_to_meterdesc(MT_ICE));
	
	
	return (TIC_meter_descriptor_t *) (NULL);
};
	
//----------------------------------------------------------------------------
unsigned char tic_nb_fields_selected(TIC_desc_t *desc) {
	unsigned char nbf = 0, byte, i;
	i=TIC_DESC_NB_BYTES;
	while (i-- > 0) {
		byte = desc->u8[i];
		while (byte > 0) {
			if (byte & 0x01) nbf++;
			byte >>= 1;
		};
	};
		
	if (TIC_FIELD_GET_BIT(desc->u8, DESC_UNPACKED_BIT)) nbf--;
	if (TIC_FIELD_GET_BIT(desc->u8, DESC_OBSOLETE_BIT)) nbf--;
	
	return (nbf);
}

//----------------------------------------------------------------------------
unsigned char tic_str_compute_checksum(char * strbuf) {
	unsigned char chks=0;
	while (*strbuf != '\0') {chks += *strbuf;  
//printf("%c.%c[%02x]",*strbuf,chks,chks);
		strbuf++;
	}
	chks &= 0x3F;
	chks += 0x20;
	return (chks);
}

//----------------------------------------------------------------------------
unsigned char * tic_get_value_at_index(TIC_meter_descriptor_t * pt_tic_descriptor,
	TIC_BUF_TYPE *tb,
	unsigned char idx) {
		
	// Return a pointer to the value at index if it exists
	
	unsigned char * tbdesc = TB_GET_DESC(tb);
	unsigned char * tbbuf = TB_GET_BUF(tb);
	
	unsigned char ifield = 0;

	if (idx >= pt_tic_descriptor->expected_fields_number) return NULL;
	
	while (idx > 0) {
		if (TIC_FIELD_GET_BIT(tbdesc, ifield) != 0) {
			switch (pt_tic_descriptor->expected_fields[ifield].type) {
				case (TYPE_CSTRING):
					tbbuf +=  strlen((char *)tbbuf) + 1;
				break;

				case (TYPE_U8):
				case (TYPE_CHAR):
					tbbuf += 1;
				break;

				case (TYPE_hm):
				case (TYPE_I16):
				case (TYPE_U16):
					tbbuf += 2;
				break;

				case (TYPE_DMh):
				case (TYPE_U24):
				case (TYPE_4U24):
				case (TYPE_6U24):
					tbbuf += 3;
				break;

				
				case (TYPE_hmDM):
				case (TYPE_U32):
					tbbuf += 4;
				break;
				
				case (TYPE_DMYhms):
					tbbuf += 6;
				break;
			
				default:
					// This case should never occurs
				break;
			} // switch (expected_fields[j].type)
		}// if (TIC_FIELD_GET_BIT(tbdesc,ifield) != 0)
		ifield++; idx--;
	}// while (ifield < idx)

	return((TIC_FIELD_GET_BIT(tbdesc,ifield) != 0) ? tbbuf : NULL);
}

//----------------------------------------------------------------------------
unsigned char tic_generic_string_match(unsigned char * sgen,unsigned char * s) {
	// Only first parameter string can embed '?' to indicate any character once
	// Only first parameter string can embed '*' to indicate any character zero or many
	// TODO: manage a "despecializator '\' to allow the characters '?' and '*'
	//       Beware that reserved space for this can of string could be doubled
	// TODO: The algorithm is very basic when * is found it only look for the last occurence 
	//       of next char in s ... algorithm should be reccursive but not really adapted to 
	//       enbeded programms
	// return 1 sgen match s 0 either
	unsigned char *tmp;
	while ((*sgen != '\0') && (*s != '\0')) {
		if (*sgen == '*') {
			// Pass any other * char
			while ((*sgen != '\0') && (*sgen == '*')) sgen++; 
			// Find last occurence of new sgen in current s
			tmp = NULL;
			while (*s != '\0') {
				if ((*sgen == '?') ||(*s == *sgen)) tmp = s;
				s++;
			}
			if (tmp != NULL) {
				s = tmp; sgen++; 
				if (*s != '\0') s++;
			}		
		} else {
			if (!((*sgen == '?') || (*sgen == *s))) return(0);
			sgen++;s++;	
		}
	}
	
	// Pass any * terminating genric string
	while ((*sgen != '\0') && (*sgen == '*')) sgen++;

	if (*sgen != '\0') return (0);
	if (*s != '\0')    return (0);

	return (1);

}

//----------------------------------------------------------------------------
unsigned char tic_check_changes(TIC_meter_descriptor_t * pt_tic_descriptor,
	TIC_BUF_TYPE *tbcrit,
	TIC_BUF_TYPE *tbbefore,
	TIC_BUF_TYPE *tbafter,
	TIC_desc_t * desc_result ) {
		
	// Return fields that are changed in the result descriptor

	unsigned char * tbdesc = TB_GET_DESC(tbcrit);
	unsigned char * tbbuf  = TB_GET_BUF(tbcrit);
	unsigned char * result = desc_result->u8; 

	unsigned char ifield = 0;
	unsigned char nfield=pt_tic_descriptor->expected_fields_number;
	
	unsigned long delta;
	
	unsigned char i,sz;
	unsigned char nb_field_changed = 0;
	memset(result,0x00, TIC_DESC_NB_BYTES);

	unsigned char *before, *after;
	while (nfield-- > 0) {
		if (TIC_FIELD_GET_BIT(tbdesc,ifield) != 0) {
			before = tic_get_value_at_index(pt_tic_descriptor, tbbefore,ifield);
			after  = tic_get_value_at_index(pt_tic_descriptor, tbafter,ifield);
			if ((before != NULL ) && (after  != NULL )) {
				// If the values to compare exists, compare them

				switch (pt_tic_descriptor->expected_fields[ifield].type) {
					case (TYPE_CSTRING):
						// Generic string comparison according to specifications
						if ((strcmp ((char*)after,(char*)before) != 0)) { // If string has changed
							// Test if after or before = criterion string
							if (tic_generic_string_match(tbbuf,before)) {
								TIC_FIELD_SET_BIT(result,ifield); nb_field_changed++;
							} else if (tic_generic_string_match(tbbuf,after)) {
								TIC_FIELD_SET_BIT(result,ifield); nb_field_changed++;
							}
						}
						tbbuf +=  strlen((char*)tbbuf) + 1;
					break;

					case (TYPE_CHAR):
						// Generic character comparison according to specifications
						if ( ((*tbbuf == '?') || (*tbbuf == *before) || (*tbbuf == *after)) &&
						     (*after != *before) ) { TIC_FIELD_SET_BIT(result,ifield); nb_field_changed++;}
						tbbuf +=  1;
					break;

					case (TYPE_U8):
						if ( (*((unsigned char*)tbbuf) != (unsigned char) (0xFF)) && 
							 (*((unsigned char*)tbbuf) != (unsigned char) (0x00)) ){
							delta = (
							 (*((unsigned char*)before) > *((unsigned char*)after)) ?
							 (*((unsigned char*)before) - *((unsigned char*)after)) :
							 (*((unsigned char*)after) - *((unsigned char*)before))
							);
							if (delta > *((unsigned char*)tbbuf)) {TIC_FIELD_SET_BIT(result,ifield); nb_field_changed++;}
						}
						tbbuf += 1;
					break;

					case (TYPE_U16): {
						unsigned short tbb;	
						U16p_TO_U16(tbbuf,tbb);
						if ((tbb != (unsigned short) (0xFFFF)) && 
							(tbb != (unsigned short) (0x0000))) {
							unsigned short bef, aft;	
							U16p_TO_U16(before,bef);	
							U16p_TO_U16(after,aft); 
							delta = ( (bef > aft) ? (bef - aft) : (aft - bef) );
							if (delta > tbb){TIC_FIELD_SET_BIT(result,ifield); nb_field_changed++;}
						}
						tbbuf += 2;
					}
					break;

					case (TYPE_I16): {
						unsigned short tbb;	
						U16p_TO_U16(tbbuf,tbb);
						if ((tbb != (unsigned short) (0xFFFF)) && 
							(tbb != (unsigned short) (0x0000))) {
							unsigned short bef, aft;	
							I16p_TO_I16(before,bef);	
							I16p_TO_I16(after,aft); 
							delta = ( (bef > aft) ? (bef - aft) : (aft - bef) );
							if (delta > tbb){TIC_FIELD_SET_BIT(result,ifield); nb_field_changed++;}
						}
						tbbuf += 2;
					}
					break;

					case (TYPE_4U24): 
					case (TYPE_6U24): 
					case (TYPE_U24): {
						unsigned long tbb;	
						U24p_TO_U32(tbbuf,tbb);
						if ((tbb != (unsigned long) (0xFFFFFF)) && 
							(tbb != (unsigned long) (0x000000))) {
							unsigned long bef, aft;	
							U24p_TO_U32(before,bef);	
							U24p_TO_U32(after,aft); 
							delta = ( (bef > aft) ? (bef - aft) : (aft - bef) );
							if (delta > tbb ){TIC_FIELD_SET_BIT(result,ifield); nb_field_changed++;}
						}
						tbbuf += 4;
					}
						
					break;
					
					case (TYPE_U32):{
						unsigned long tbb;
						U32p_TO_U32(tbbuf,tbb);
						if ((tbb != (unsigned long) (0xFFFFFFFF)) && 
							(tbb != (unsigned long) (0x00000000))) {
							unsigned long bef, aft;	
							U32p_TO_U32(before,bef);	
							U32p_TO_U32(after,aft); 
							delta = ( (bef > aft) ? (bef - aft) : (aft - bef) );
							if (delta > tbb){TIC_FIELD_SET_BIT(result,ifield); nb_field_changed++;}
						}
						tbbuf += 4;
					}
					break;
					
					case (TYPE_DMYhms):
						sz = 6; goto lbl_process_date_field;
					case (TYPE_hmDM):	
						sz = 4; goto lbl_process_date_field;				
					case (TYPE_DMh):
						sz = 3; goto lbl_process_date_field;
					case (TYPE_hm):	
						sz = 2;
lbl_process_date_field:			
						{
							unsigned char *tbb, *bef, *aft;
							tbb = tbbuf; aft = after; bef = before;
							for (i = 0; i < sz; i++) { // For all date fields one is enough to trig				
								if ((*tbb != (unsigned char) (0xFF)) &&
								    (*tbb != (unsigned char) (0x00))){
									if ((*bef != *aft) && ((*aft % *tbb) == 0)){ // different and multiple of criterion									
										TIC_FIELD_SET_BIT(result,ifield);nb_field_changed++;
										break; // Field changed get out of for()
									}
								} // if tbbuf is a criterion
								bef++;aft++;tbb++;
							} // for
							tbbuf += sz;
						}
					break;

					default:
						// This case should never occurs
					break;
				} // switch (expected_fields[j].type)

			} else { // The field is not present either in after or before but not in both
				// The report will be requested anyway for this field change
				// Notice: that if the field has deseapered, the report will be done without this field !
				if ((before != NULL ) || (after  != NULL )) {
					TIC_FIELD_SET_BIT(result,ifield); nb_field_changed++;
				}
			}// else if (after != NULL )

		} // if (TIC_FIELD_GET_BIT(tbdesc,ifield) != 0)
		ifield++;
	}  // while (nfield-- > 0)

	return(nb_field_changed);
}

//----------------------------------------------------------------------------
signed short tic_deserialize(TIC_meter_descriptor_t * pt_tic_descriptor,
	TIC_BUF_TYPE *tb,
	unsigned char* buf,
	short buf_size) {
	// return size of deserialized buffer inside tb without desc bytes
	// if -1 => the buf_size is too small to store deserialzed frame !!

	// THIS IS IMPLEMENTED TO TRY TO GET IT ARCHITECTURE AGNOSTIC (endianess)
	
	unsigned char * tbdesc;
	unsigned char * tbbuf;
	unsigned char * tmpbuf;
	unsigned char ifield, nfield;
	unsigned char len, svg_size = buf_size;	
	
	tbdesc  = buf;

	tbbuf  = TB_GET_BUF(tb);
	tmpbuf = buf;

	buf_size -= TIC_DESC_NB_BYTES; if (buf_size < 0) return -1; 
	
	memcpy(TB_GET_DESC(tb),tbdesc,TIC_DESC_NB_BYTES);
	tmpbuf += TIC_DESC_NB_BYTES;	

	ifield=0;
	nfield=pt_tic_descriptor->expected_fields_number;

	while (nfield-- > 0) {
		if (TIC_FIELD_GET_BIT(tbdesc,ifield) != 0) {

			switch (pt_tic_descriptor->expected_fields[ifield].type) {
				case (TYPE_CSTRING):
					len = strlen((char *)tmpbuf) + 1;
					buf_size -= len; if (buf_size < 0) return -1; 
					strcpy((char *)tbbuf,(char *)tmpbuf); 
					tmpbuf += len; tbbuf +=  len;
				break;

				case (TYPE_U8):
				case (TYPE_CHAR):
					buf_size--; if (buf_size < 0) return -1;
					*tbbuf = *tmpbuf;
					tmpbuf += 1; tbbuf += 1;
				break;
				
				case (TYPE_I16): 
				case (TYPE_U16): // U16/I16 stored Big endian anyway in RAM, as the data stream ...
				 				 // Value process (like check_change()) must convert to u16/I16 to use U16p_TO_U16 macro
					buf_size-=2; if (buf_size < 0) return -1;
					memcpy((char *)tbbuf,(char *)tmpbuf,2);
					tmpbuf += 2; tbbuf += 2;
				break;

				case (TYPE_4U24):
				case (TYPE_6U24):
				case (TYPE_U24): // U24 stored Big endian anyway in RAM, as the data stream ...
				 				 // Value process (like check_change()) must convert to u32 to use U24p_TO_U32 macro
					buf_size-=3; if (buf_size < 0) return -1;				 				 
					memcpy((char *)tbbuf,(char *)tmpbuf,3);
					tmpbuf += 3; tbbuf += 3;
				break;

				case (TYPE_U32): // U32 stored Big endian anyway in RAM, as the data stream ...
				 				 // Value process (like check_change()) must convert to u32 to use U32p_TO_U32 macro
					buf_size-=4; if (buf_size < 0) return -1;				 				 
					memcpy((char *)tbbuf,(char *)tmpbuf,4);
					tmpbuf += 4; tbbuf += 4;
				break;
								
				case (TYPE_DMYhms):
					buf_size-=6; if (buf_size < 0) return -1;
					memcpy((char *)tbbuf,(char *)tmpbuf,6);
					tmpbuf += 6; tbbuf += 6;
				break;
				
				case (TYPE_hmDM):
					buf_size-=4; if (buf_size < 0) return -1;
					memcpy((char *)tbbuf,(char *)tmpbuf,4);
					tmpbuf += 4; tbbuf += 4;
				break;
				
				case (TYPE_DMh):
					buf_size-=4; if (buf_size < 0) return -1;
					memcpy((char *)tbbuf,(char *)tmpbuf,3);
					tmpbuf += 3; tbbuf += 3;
				break;
				
				case (TYPE_hm):
					buf_size-=2; if (buf_size < 0) return -1;
					memcpy((char *)tbbuf,(char *)tmpbuf,2);
					tmpbuf += 2; tbbuf += 2;
				break;

				default:
					// This case should never occurs
				break;
				
			} // switch (expected_fields[j].type)
		} // if (TIC_FIELD_GET_BIT(tbdesc,ifield) != 0)
		ifield++;
	} // while (nfield-- > 0) 

	// Set the size of the frame at the end of buffers
	(TB_GET_DESC(tb))[svg_size] = (tbbuf - TB_GET_DESC(tb));

	return(tbbuf - TB_GET_BUF(tb));
}

//----------------------------------------------------------------------------
signed short tic_serialize(TIC_meter_descriptor_t * pt_tic_descriptor, 
	unsigned char* buf,
	TIC_BUF_TYPE *tb, 
	TIC_desc_t * desc_filter,
	short buf_size) {
	
	// TODO : make some verifications according to buf_size
		
	// return size of serialized buffer inside buf included 8 desc bytes
	// Only existing and validated filed through filter field bit are returned

	// THIS IS IMPLEMENTED TO TRY TO GET IT ARCHITECTURE AGNOSTIC (endianess)
	
	// Now possible to serialize a TIC buffer from an "UNPACKED" TIC Buffer source
	
	unsigned char * tbdesc;
	unsigned char * tbbuf;
	unsigned char * tmpbuf;
	unsigned char ifield;
	unsigned char nfield;
	unsigned char len;
	unsigned char src_is_unpacked;
	
	TIC_desc_t desc_res;
	unsigned char * result; 
	unsigned char * filter; 

	tbdesc = TB_GET_DESC(tb);
	tbbuf  = TB_GET_BUF(tb);
	tmpbuf = buf;
	
	src_is_unpacked = TIC_FIELD_GET_BIT(tbdesc, DESC_UNPACKED_BIT);

	// the real serialized descriptor is set at the end with result
	memset(desc_res.u8,0x00,TIC_DESC_NB_BYTES);
	result=desc_res.u8;
	filter=desc_filter->u8;

	tmpbuf += TIC_DESC_NB_BYTES;
	ifield=0;
	nfield=pt_tic_descriptor->expected_fields_number;

	while (nfield-- > 0) {		
		if (TIC_FIELD_GET_BIT(tbdesc,ifield) != 0) {
			
			if (src_is_unpacked)
				tbbuf = TB_GET_BUF(tb) + (pt_tic_descriptor->expected_fields[ifield].abs_pos);
			 
			switch (pt_tic_descriptor->expected_fields[ifield].type) {
				case (TYPE_CSTRING):
					len = strlen((char *)tbbuf) + 1;
				    if (TIC_FIELD_GET_BIT(filter,ifield) != 0) {
						strcpy((char *)tmpbuf,(char *)tbbuf);
						TIC_FIELD_SET_BIT(result,ifield);
						tmpbuf += len;
				    }
					tbbuf +=  len;
				break;

				case (TYPE_U8):
				case (TYPE_CHAR):
				    if (TIC_FIELD_GET_BIT(filter,ifield) != 0) {
						*tmpbuf = *tbbuf;
						TIC_FIELD_SET_BIT(result,ifield);
						tmpbuf += 1;
				    };
				    tbbuf += 1;
				break;

				case (TYPE_I16):
				case (TYPE_U16):
				    if (TIC_FIELD_GET_BIT(filter,ifield) != 0) {
						memcpy((char *)tmpbuf,(char *)tbbuf,2);
						TIC_FIELD_SET_BIT(result,ifield);
						tmpbuf += 2;
				    };
				     tbbuf += 2;
				break;

				case (TYPE_4U24):
				case (TYPE_6U24):
				case (TYPE_U24): // U24 stored Big endian anyway in RAM, as the data stream ...
				 				 // Value process (like check_change()) must convert to u32 to use U24p_TO_U32 macro
				    if (TIC_FIELD_GET_BIT(filter,ifield) != 0) {
						memcpy((char *)tmpbuf,(char *)tbbuf,3);
						TIC_FIELD_SET_BIT(result,ifield);
						tmpbuf += 3;
				    };
				    tbbuf += 3;
				break;

				case (TYPE_U32):
				    if (TIC_FIELD_GET_BIT(filter,ifield) != 0) {
						memcpy((char *)tmpbuf,(char *)tbbuf,4);
						TIC_FIELD_SET_BIT(result,ifield);
						tmpbuf += 4;
				    };
				    tbbuf += 4;
				break;
								
				case (TYPE_DMYhms):
				    if (TIC_FIELD_GET_BIT(filter,ifield) != 0) {
						memcpy((char *)tmpbuf,(char *)tbbuf,6);
						TIC_FIELD_SET_BIT(result,ifield);
						tmpbuf += 6;
				    };
				    tbbuf += 6;
				break;
				
				case (TYPE_hmDM):
				    if (TIC_FIELD_GET_BIT(filter,ifield) != 0) {
						memcpy((char *)tmpbuf,(char *)tbbuf,4);
						TIC_FIELD_SET_BIT(result,ifield);
						tmpbuf += 4;
				    };
				    tbbuf += 4;
				break;
				
				case (TYPE_DMh):
				    if (TIC_FIELD_GET_BIT(filter,ifield) != 0) {
						memcpy((char *)tmpbuf,(char *)tbbuf,3);
						TIC_FIELD_SET_BIT(result,ifield);
						tmpbuf += 3;
				    };
				    tbbuf += 3;
				break;
				
				case (TYPE_hm):
				    if (TIC_FIELD_GET_BIT(filter,ifield) != 0) {
						memcpy((char *)tmpbuf,(char *)tbbuf,3);
						TIC_FIELD_SET_BIT(result,ifield);
						tmpbuf += 2;
				    };
				    tbbuf += 2;
				break;

				default:
					// This case should never occurs
				break;
			} // switch (expected_fields[j].type)
		} // if (TIC_FIELD_GET_BIT(tbdesc,ifield) != 0)
		ifield++;
	} // while (nfield-- > 0) 
	
	memcpy(buf,result,TIC_DESC_NB_BYTES);
	
	// Set the size of the frame at the end of buffers
	buf[buf_size] = (tmpbuf - buf);

	return(tmpbuf - buf);
}

//----------------------------------------------------------------------------
signed short tic_serialize_report_criteria(TIC_meter_descriptor_t * pt_tic_descriptor, 
	unsigned char* buf,
	TIC_BUF_TYPE *tb, 
	short buf_size) {
	
	// TODO : make some verifications according to buf_size
		
	// return size of serialized buffer inside buf included 8 desc bytes
	// Only existing and validated filed through filter field bit are returned

	// THIS IS IMPLEMENTED TO TRY TO GET IT ARCHITECTURE AGNOSTIC (endianess)
	
	// Now possible to serialize a TIC buffer from an "UNPACKED" TIC Buffer source
	
	unsigned char * tbdesc;
	unsigned char * tbbuf;
	unsigned char * tmpbuf;
	unsigned char ifield;
	unsigned char nfield;
	unsigned char len,sz,i;
	unsigned char src_is_unpacked;
	
	unsigned char * criteria; 
	unsigned char * filter; 

	tbdesc = TB_GET_DESC(tb);
	tbbuf  = TB_GET_BUF(tb);
	
	src_is_unpacked = TIC_FIELD_GET_BIT(tbdesc, DESC_UNPACKED_BIT);

    filter   = TB_CRIT_GET_FILTER_DESC(buf); memset(filter,0x00,TIC_DESC_NB_BYTES);
    criteria = TB_CRIT_GET_TRIG_DESC(buf);   memset(criteria,0x00,TIC_DESC_NB_BYTES);
    tmpbuf   = TB_CRIT_GET_TRIG_BUF(buf);
	
	ifield=0;
	nfield=pt_tic_descriptor->expected_fields_number;

	while (nfield-- > 0) {		
		if (TIC_FIELD_GET_BIT(tbdesc,ifield) != 0) {
			
			if (src_is_unpacked)
				tbbuf = TB_GET_BUF(tb) + (pt_tic_descriptor->expected_fields[ifield].abs_pos);
			 
			switch (pt_tic_descriptor->expected_fields[ifield].type) {
				case (TYPE_CSTRING):
					len = strlen((char *)tbbuf) + 1;
				    if (*(char *)tbbuf != '\0') {
						strcpy((char *)tmpbuf,(char *)tbbuf);
						TIC_FIELD_SET_BIT(criteria,ifield);
						tmpbuf += len;
				    };
					TIC_FIELD_SET_BIT(filter,ifield);
					tbbuf +=  len;
				break;
				
				case (TYPE_CHAR):
				    if ( (*((unsigned char*)tbbuf) != '\0') ){
						*tmpbuf = *tbbuf;
						TIC_FIELD_SET_BIT(criteria,ifield);
						tmpbuf += 1;
				    };
					TIC_FIELD_SET_BIT(filter,ifield);
				    tbbuf += 1;
				break;

				case (TYPE_U8):
				    if ( (*((unsigned char*)tbbuf) != (unsigned char) (0xFF)) && 
						 (*((unsigned char*)tbbuf) != (unsigned char) (0x00)) ){
						*tmpbuf = *tbbuf;
						TIC_FIELD_SET_BIT(criteria,ifield);
						tmpbuf += 1;
				    };
					TIC_FIELD_SET_BIT(filter,ifield);
				    tbbuf += 1;
				break;

				case (TYPE_I16):
				case (TYPE_U16): {
					unsigned short tbb;	
					U16p_TO_U16(tbbuf,tbb);
					if ((tbb != (unsigned short) (0xFFFF)) && 
						(tbb != (unsigned short) (0x0000))) {
						memcpy((char *)tmpbuf,(char *)tbbuf,2);
						TIC_FIELD_SET_BIT(criteria,ifield);
						tmpbuf += 2;
				    };
					TIC_FIELD_SET_BIT(filter,ifield);
				    tbbuf += 2;
				} break;

				case (TYPE_4U24):
				case (TYPE_6U24):
				case (TYPE_U24): {
					unsigned long tbb;	
					U24p_TO_U32(tbbuf,tbb);
					if ((tbb != (unsigned long) (0xFFFFFF)) && 
						(tbb != (unsigned long) (0x000000))) {
						memcpy((char *)tmpbuf,(char *)tbbuf,3);
						TIC_FIELD_SET_BIT(criteria,ifield);
						tmpbuf += 3;
				    };
					TIC_FIELD_SET_BIT(filter,ifield);
				    tbbuf += 3;
				} break;

				case (TYPE_U32):{
					unsigned long tbb;
					U32p_TO_U32(tbbuf,tbb);
					if ((tbb != (unsigned long) (0xFFFFFFFF)) && 
						(tbb != (unsigned long) (0x00000000))) {
						memcpy((char *)tmpbuf,(char *)tbbuf,4);
						TIC_FIELD_SET_BIT(criteria,ifield);
						tmpbuf += 4;
				    };
					TIC_FIELD_SET_BIT(filter,ifield);
				    tbbuf += 4;
				} break;
											
				case (TYPE_DMYhms):
					sz = 6; 
					goto lbl_process_date_field1;
				case (TYPE_hmDM):	
					sz = 4; goto lbl_process_date_field1;				
				case (TYPE_DMh):
					sz = 3; goto lbl_process_date_field1;
				case (TYPE_hm):	
					sz = 2;
lbl_process_date_field1:			
					{
						unsigned char *tbb;
						tbb = tbbuf;
						for (i = 0; i < sz; i++) { // For all date fields one is enough to trig				
							if ((*tbb != (unsigned char) (0xFF)) &&
							    (*tbb != (unsigned char) (0x00))){
								TIC_FIELD_SET_BIT(criteria,ifield);
								memcpy((char *)tmpbuf,(char *)tbbuf,sz);
								tmpbuf += sz;
								break; // Field changed get out of for()
							} // if tbbuf is a criterion
							tbb++;
						} // for
						TIC_FIELD_SET_BIT(filter,ifield);
						tbbuf += sz;
					}
				break;
				
				default:
					// This case should never occurs
				break;
			} // switch (expected_fields[j].type)
		} // if (TIC_FIELD_GET_BIT(tbdesc,ifield) != 0)
		ifield++;
	} // while (nfield-- > 0) 
		
	// Set the size of the frame at the end of buffers
	buf[buf_size] = (tmpbuf - buf);

	return(tmpbuf - buf);
}
