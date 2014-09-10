
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
 *	environment variable to get root directory of Actility/watteco datas
 *	getenv(3)
 */
#define	AW_DATA_DIR		"AW_DATA_DIR"

/*
 *	max sensors per router-controller
 */
#define	MAX_SSR_PER_RTR		50	// TODO test only/ really 50

/*
 *	max applications per sensors
 */
#define	MAX_APP_PER_SSR		8	// application == endpoint ...

/*
 *	max interfaces/clusters per sensors
 */
#define	MAX_ITF_PER_SSR		10

/*
 *	max members per attribut
 */
#define	MAX_MBR_PER_ATT		5

/*
 *	max commands per cluster
 */
#define	MAX_CMD_PER_CLS		5

/*
 *	request http to router server to get sensors list
 *	strings to start and stop parsing response
 *	separator
 *	size max for response
 */
#define	HTTP_GET_ELEM_LIST		"GET / HTTP/1.1 \r\n"
#define	HTTP_PARSE_START		"<h2>Routes</h2>"
#define	HTTP_PARSE_END			"</body>"
#define	HTTP_PARSE_SEP			"<br>"
#define	HTTP_RESPONSE_SIZE		10000


char		*AwVersion();

char 		*AwGetPanPref(char *addr);
char		*AwGetPanAddr(char *pan);
char		*AwGetRadioMode(char *mode);
int		AwParseRouter(char *res,char *tab[]);
int 		AwSensorDiscover(time_t *last,char *tbcur[],char *tbdel[],
								char *tbnew[]);
int		AwSensorSoftware(time_t *last,char *tbcur[],char *tbdel[],
								char *tbnew[]);


int		AwFileCmp(char *f1,char *f2);

unsigned char	*AwStrToBin(char *str,unsigned char bin[],int *lg);
char 		*AwBinToStr(unsigned char *bin,int lg,char dst[],int max);
void		AwPrintBin(FILE *f,unsigned char *buf,int lg);
