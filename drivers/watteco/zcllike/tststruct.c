
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

#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdarg.h>
#include <string.h>
#include <time.h>
#include <signal.h>
#include <ctype.h>
#include <sys/types.h>

#include <unistd.h>
#include <errno.h>
#include <err.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#include "_whatstr.h"
#include "aw.h"
#include "watteco.h"


int main(int argc,char *argv[])
{
	t_zcl_msg	msg;
	void		*ad1;
	void		*ad2;

	printf	("sizeof(t_zcl_msg)=%d\n",sizeof(msg));

	ad1	= &(msg.u_pack.m_read_conf_resp.m_status);
	ad2	= &(msg.u_pack.m_writ_attr_rqst.m_attr_id);
	if	(ad1 == ad2)
		printf("packing seems ok\n");
	else
	{
		printf("packing error\n");
		exit(1);
	}

	ad1	= &(msg.u_pack.m_data.m_byte[4]);
	ad2	= &(msg.u_pack.m_read_attr_resp.m_attr_id);
	if	(ad1 == ad2)
		printf("packing seems ok\n");
	else
	{
		printf("packing error\n");
		exit(1);
	}

	printf("packing ok\n");

	exit(0);
}
