
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


//
//	sensor	states
//

#define	ST_CP_NULL		0
#define	ST_CP_ALLOCATED		10
#define	ST_CP_WAITINGPANID	20
#define	ST_CP_WAITINGDESC	30
#define	ST_CP_CONFIGURING	40
#define	ST_CP_RUNNING		50
#define	ST_CP_DISCONNECTED	90
#define	ST_CP_ERROR		99

#define	ST_CP_STATE_MAX		100
#define	ST_CP_STATE_CURRENT	0
#define	ST_CP_STATE_ENTER	1
