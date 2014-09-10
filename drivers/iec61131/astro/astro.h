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

#ifndef _ASTRO_H_
#define _ASTRO_H_

typedef struct {
	struct tm *tm;
	double	JD, j;
	double	Lat, Lon;
	double	M, C, L, R;
	double	EoT;
	double	Dec;
	double	cosHo, Ho;
	//double	cosAz, Az;
	double VL, TL, HL;
	double VC, TC, HC;
	double	h;
	int	DayTime;
	time_t	TL_epoch;
	time_t	TC_epoch;
} astro_t;

void astro_calculate(astro_t *ast);

#endif /*_ASTRO_H_*/
