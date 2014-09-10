
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

#ifndef	RTL_CLIF_H
#define	RTL_CLIF_H


// a buffer waiting for "poll write event" on a tcp/socket for CLI
typedef	struct	s_lout
{
	struct list_head list;	// the chain list of pending data
	char	*o_buf;		// a buffer
	int	o_len;		// lenght of o_buf
}	t_clout;

// a CLI connection on admin port
typedef	struct
{
	int	cl_fd;		// associated socket
	int	cl_pos;		// position in cl_cmd
	char	cl_cmd[1000];	// the user input accumulated until '\n' | '\r'
	time_t	cl_lasttcpread;	// time of last remote read
	time_t	cl_lasttcpwrite;// time of last remote write
	int	cl_nbkey;	// number of retry for password
	int	cl_okkey;	// password ok
	t_clout	cl_lout;	// list of pending data
	char	cl_data[256];	// sensor address to sniff ('*' == all)
}	t_clif;


#endif
