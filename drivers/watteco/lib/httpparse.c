
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
#include <stdarg.h>
#include <string.h>
#include <time.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/stat.h>

#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <err.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#include "rtlbase.h"
#include "aw.h"
#include "watteco.h"

	// <h2>Routes</h2>aaaa:...:ZZZZ/128 (via XXXX:...:ZZZZ)<br>
	// aaaa:...:ZZZZ/128 (via XXXX:...:ZZZZ)<br> ... <br> ... <br>
	// </body></html>

int AwParseRouter(char *res,char *tab[])	/* TS */
{
	int	nb = 0;
	char	*pt;
	char	*ha;
	char	*ne;
	char	*ed;
	char	*nd;

	ne	= HTTP_PARSE_START;
	ed	= HTTP_PARSE_END;
	nd	= HTTP_PARSE_SEP;

	ha	= res;
	if ((pt=strstr(ha,ne)) == NULL)
	{
//		err(1,"no '%s'\n",ne);
		return	-1;
	}
	ha	= pt + strlen(ne);
	if ((pt=strstr(ha,ed)) == NULL)
	{
//		err(1,"no '%s'\n",ed);
		return	-2;
	}
	*pt	= '\0';

	while (ha && *ha && (pt=strstr(ha,nd)))
	{
		char	*adr;

		while (*ha == ' ' || *ha == '\r' || *ha == '\n' || *ha == '\t')
			ha++;
		if (!*ha)
			break;
		*pt	= '\0';

		if ((adr=strchr(ha,'/')) || (adr=strchr(ha,' ')) || (adr=strchr(ha,'(')))
			*adr	= '\0';
//printf("+ '%s'\n",ha);
		adr	= strdup(ha);
		if (!adr)
			break;
		tab[nb]	= adr;
		ha	= pt + strlen(nd);
		nb++;
		if (nb >= MAX_SSR_PER_RTR)
		{
			RTL_TRDBG(0,"too much sensors in HTTP response\n");
			break;
		}
	}

	return	nb;
}
