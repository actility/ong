
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

#include	<stdio.h>
#include	<stdlib.h>
#include	<string.h>
#include	<time.h>

#include	"xoapipr.h"
#include	"intstr.h"

t_objet_ref	*XoTabObjUser ;
int		XoNbObjUser  = 0 ;

main(argc,argv)
int	argc ;
char	*argv[] ;
{

	printf	("t_dyn                 %d\n",(int)sizeof(t_dyn)) ;
	printf	("u_val                 %d\n",(int)sizeof(u_val)) ;
	printf	("t_basic_attr          %d\n",(int)sizeof(t_basic_attr)) ;
	printf	("t_list                %d\n",(int)sizeof(t_list)) ;
	printf	("t_list_info           %d\n",(int)sizeof(t_list_info)) ;
	printf	("u_attr                %d\n",(int)sizeof(u_attr)) ;
	printf	("t_obj                 %d\n",(int)sizeof(t_obj)) ;

	exit	(0) ;
}
