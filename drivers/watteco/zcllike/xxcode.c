
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

int	TraceLevel	= 2;
int	TraceDebug	= 1;


int main(int argc,char *argv[])
{
	int		ret;
	unsigned	char	*binmsg	= NULL;
	unsigned	char	str[2000];
	unsigned	char	bin[2000];
			int	szbin	= sizeof(bin);

	t_zcl_msg	mess;

	AwZclInitProto();

	printf	("sizeof(t_zcl_msg)=%ld\n",sizeof(t_zcl_msg));
	printf	("sizeof(t_zcl_simple_desc)=%ld\n",sizeof(t_zcl_simple_desc));
#if	0
	printf	("sizeof(m_head)=%ld\n",sizeof(mess.u_pack.m_head));
#endif

	if (argc != 2)
	{
		printf("default command : ask for PanID\n");
		binmsg	= AwStrToBin((char *)"11,00,0050,0001",bin,&szbin);
	}
	else
		binmsg	= AwStrToBin((char *)argv[1],bin,&szbin);
	if (!binmsg || szbin <= 0)
	{
		err(1,"bad format\n");
		exit(1);
	}
	AwPrintBin(stdout,binmsg,szbin);

#if	0
	AwBinToStr(bin,szbin,str,sizeof(str));
	printf	("str='%s'\n",str);
#endif

	str[0]	= '\0';
	ret	= AwZclDecode((char *)str,&mess,binmsg,szbin);

	printf	("%s\n",str);

	printf	("decode ret=%08x %d\n",ret,ret);
	printf	("cmd=%x size=%d\n",mess.m_cmd,mess.m_size);
	printf	("status=%x direction=%x\n",
			mess.m_status,mess.m_direction);
	printf	("cluster=%04x\n",mess.m_cluster_id);
	printf	("attr=%04x\n",mess.m_attr_id);
	if	(mess.m_cmd == W_CMD_READ_ATTR_RESP)
	{
		printf	("appt=%0x\n",mess.u_pack.m_read_attr_resp.m_attr_type);
	}


	if	(ret > 0)
		printf	("decode success\n");
	else
		printf	("decode failure\n");

	if	(mess.m_cmd == W_CMD_READ_ATTR_RESP 
			&& mess.m_cluster_id == W_CLU_CONFIGURATION
			&& mess.m_attr_id == W_ATT_CONFIGURATION_DESC)
	{
		printf	("dont reencode message on W_CLU_CONFIGURATION\n");
		printf	("\n");
		printf	("\n");
	printf	("+++++++++++++++++++++++++++++++++++++++++++++++++++++++\n");
		return	0;
	}
	if	(mess.m_cmd == W_CMD_READ_ATTR_RESP 
			&& mess.m_cluster_id == W_CLU_CONFIGURATION
			&& mess.m_attr_id == W_ATT_CONFIGURATION_DESCEPN)
	{
		printf	("dont reencode message on W_CLU_CONFIGURATION\n");
		printf	("\n");
		printf	("\n");
	printf	("+++++++++++++++++++++++++++++++++++++++++++++++++++++++\n");
		return	0;
	}




	printf	("---------\n");
	fflush	(stdout);

//mess.u_param.m_raw_data_ptr.m_ptr = "ABC";

	ret	= AwZclEncode(&mess,NULL,0);
	AwBinToStr((unsigned char *)&mess.u_pack.m_data,ret,
						(char *)str,sizeof(str));
	printf	("encode ret=%d\n",ret);
	printf	("out='%s'\n",str);

	if	(memcmp(binmsg,&mess.u_pack.m_data,szbin) == 0)
		printf	("encode success\n");
	else
		printf	("encode failure\n");

	printf	("\n");
	printf	("\n");
	printf	("+++++++++++++++++++++++++++++++++++++++++++++++++++++++\n");



	exit(0);
}
