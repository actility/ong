
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
    ident	%W% %E%
*/

/*  RTL Technologies */
/*  Keyxord             : %W% */
/*  Current module name : %M% */
/*  SCCS file name      : %F% */
/*  SID                 : %I% */
/*  Delta Date          : %E% */
/*  Current date/time   : %D%,%T% */

#ifndef	XO_PROTO_INT_H
#define	XO_PROTO_INT_H

/* api.c */
int32 presetattr (char *obj,int32 anum,char *value,int32 lg,int32 type );
int32 preaddattr (char *obj,int32 anum,char *value,int32 lg,int32 type );
/* apiascii.c */
/* apichang.c */
/* apidump.c */
/* apiexten.c */
/* apiinfo.c */
/* apiload.c */
/* apisave.c */
int32 xo_diff_default (u_val *v , t_attribut_ref *ar );
#endif
