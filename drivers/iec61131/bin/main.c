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

/* main.c --
 *
 * Copyright (C) 2012 Actility
 *
 * This file is part of the iec61131-3 interpreter. Please see
 * README for terms of use.
 **
 */


#include <stdio.h>
#include <stddef.h>
#include <strings.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <ctype.h>
#include <poll.h>
#include <mxml.h>
#include <rtlbase.h>
#include <rtllist.h>
#include <rtlhtbl.h>
#include <dIa.h>

#include "plc.h"
#include "iec_debug.h"
#include "admin_cli.h"
#include "iec_config.h"
#include "operating.h"
#include "vasprintf.h"
#include "tools.h"
#include "_version.h"


void            *MainTbPoll;
int TraceDebug=1;
int TraceLevel=9;
void *g_initialHeap;

typedef struct {
	char *filename;
	int level;
} T_file;

T_file _files[50];
int _count = 0;

void Usage(char *prg) {
  printf("usage: %s [-h|--help] ([-f <plc>] [-t <level>])*N [-I <dir>]\n", prg);
  printf("Where:\n");
  printf("\t-h|--help : print this help message\n");
  printf("\t-f <plc>: set the PLC file\n");
  printf("\t-t <level>: set the trace level to <level>\n");
  printf("\t-I <dir>: set the include's directory\n");
  printf("\tNOTE : the couple -f -t can be repeted N times to load N scripts\n");
}

void Version() {
	printf ("%s\n", iec61131_whatStr);
}

int main(int argc,char *argv[]) {
	char	*stdInclude = NULL;
	int	opt;
	int i;
	BOOL trace_in_call_args = FALSE;

	g_initialHeap	= sbrk(0);

	for	(i=1; i<argc; i++) {
		if  (!strcmp(argv[i], "--version")) {
			Version();
			exit (0);
		}
		if  (!strcmp(argv[i], "--help")) {
			Usage (argv[0]);
			exit (0);
		}
	}

	while ((opt = getopt(argc, argv, "f:t:I:vh")) != -1) {
		switch (opt) {
		case 'f' :
			_files[_count].filename = strdup(optarg);
			_count ++;
			break;
		case 't' :
			if	(_count == 0)
				TraceLevel	= atoi(optarg);
			if	(_count > 0)
				_files[_count-1].level = atoi(optarg);;
			trace_in_call_args = TRUE;
			break;
		case 'I' : stdInclude = optarg; break;
		case 'v' : Version(); exit (0); break;
		case 'h' : Usage(argv[0]); exit(0); break;
		default:
			exit( 1 );
			break;
		}
	}

	if	(IEC_loadConfig () < 0)
		exit (1);

	char hostname[128];
	strcpy (hostname, IEC_Config_Default("w_boxname", ""));
	if	(!*hostname)
		gethostname(hostname, sizeof(hostname));
	IEC_AddConfig("w_boxname", stringToLower(hostname));

	if	(IEC_Config("w_reqEntity") == NULL) {
/*
		"coap://" + $w_boxname + "." + $w_domainname + ":" + $w_coapport_r + $s_rootapp + "/" + $w_driverid
*/
		char *reqEntity = NULL;
		iec_asprintf (&reqEntity, "coap://%s.%s:%s%s/%s",
			IEC_Config("w_boxname"), IEC_Config("w_domainname"),
			IEC_Config("w_coapport_r"), IEC_Config("s_rootapp"),
			IEC_Config("w_driverid"));
		IEC_AddConfig("w_reqEntity", reqEntity);
		free (reqEntity);
	}
	if	(IEC_Config("w_scl_httpport") == NULL) {
		IEC_AddConfig("w_scl_httpport", "8080");
	}

	if	(trace_in_call_args == FALSE)
		TraceLevel = IEC_Config_Int_Default("tracelevel", TraceLevel);

	if	(Operating_OngBox())
		rtl_tracerotate (Operating_LogsDir());

	rtl_tracelevel (TraceLevel);
	rtl_tracesizemax (IEC_Config_Int_Default("tracesizemax", 50*1024*1024));

	MainTbPoll = rtl_pollInit ();

	AdmTcpInit();

	//	iec initialization
	iec_init(stdInclude);

	//	Actility init
	extern void Actility_init();
	Actility_init ();

	iec_m2mInit ();
/*
	for	(i=0; i<_count; i++) {
		iec_user_t *user;
		user	= User_New(_files[i].filename, _files[i].level);
		free (_files[i].filename);
	}
*/
	for	(i=0; i<_count; i++) {
		iec_user_t *user;
		struct stat st;
		char tmp[128];

		if (stat(_files[i].filename, &st) < 0) {
			RTL_TRDBG (TRACE_ERROR, "Can't access directory %s\n", _files[i].filename);
			continue;
		}

		if (S_ISREG(st.st_mode)) {
			RTL_TRDBG (TRACE_ERROR, "Loading file %s\n", _files[i].filename);
			user	= User_New(_files[i].filename, _files[i].level);
		}
		else {
			sprintf (tmp, "%s/plc.xml", _files[i].filename);
			if	(stat(tmp, &st) < 0)
				sprintf (tmp, "%s/plc.exi", _files[i].filename);
			RTL_TRDBG (TRACE_ERROR, "Loading file %s\n", tmp);
			user	= User_New(tmp, _files[i].level);

			if	(user) {
				sprintf (tmp, "%s/m2mMapping.xml", _files[i].filename);
				RTL_TRDBG (TRACE_ERROR, "Loading mapping file %s\n", tmp);
				User_LoadFileMapping (user, tmp);
			}
			else
				RTL_TRDBG (TRACE_ERROR, "Can't open %s\n", tmp);
		}

		free (_files[i].filename);
	}

	//	events loop
	while (1) {
		IEC_loadConfig ();
		TIME interval;
		iec_getMinWait (&interval);
		int ms	= (interval.tv_sec*1000)+((interval.tv_nsec+500000)/1000000);

		struct timeval tv;
		iec_m2m_nextTimer (&tv);
		int ms2 = tv.tv_sec * 1000;

		if	(ms2 > 0 && ms2 < ms)
			ms	= ms2;

		rtl_poll(MainTbPoll, ms);
		checkGlitch (interval);
		Users_Next ();
		iec_m2mPoll ();
	}
	return	0;
}
