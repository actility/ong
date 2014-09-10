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
#include <fcntl.h>
#include <malloc.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include <rtlbase.h>
#include <iec_debug.h>

#include "codegen.h"


int TraceDebug=0;
int TraceLevel=0;
char *pgm;

extern char *yy_scan_string(const char *);
extern void yy_delete_buffer(char *);
extern int yylex_destroy  (void);

extern int yyparse();
extern inter_node_t *programBlock;


void usage(char *prog) {
	fprintf (stderr, "%s -f <ST file> -t <trace level> -c <count>\n", prog);
}

void disp_mallinfo() {
	struct mallinfo info = mallinfo();
	RTL_TRDBG (TRACE_DETAIL, "### mallinfo : total allocated space:  %d bytes\n", info.uordblks);
}

int main(int argc, char **argv) {
	int     fd, n, count=1;
	int	opt;
	char	*file = NULL;

	while ((opt = getopt(argc, argv, "f:t:c:")) != -1) {
		switch (opt) {
		case 'f' :
			file	= optarg;
			break;
		case 't' :
			TraceLevel	= atoi(optarg);
			TraceDebug	= 1;
			break;
		case 'c' :
			count	= atoi(optarg);
			break;
		}
	}

	if	(!file) {
		usage (argv[0]);
		exit (1);
	}
	fd      = open(file, 0);
	if (fd < 0)
		exit (1);
	struct stat st;
	stat (file, &st);
	pgm	= malloc(st.st_size+1);
	n = read (fd, pgm, st.st_size);
	if	(n < 0)
		exit (2);
	pgm[n] = 0;
	close (fd);

	disp_mallinfo();

	rtl_tracelevel(TraceLevel);

	RTL_TRDBG (TRACE_DETAIL, "ST code : %s\n", pgm);

	int i;
	for (i=0; i<count; i++) {
		extern char *yy_scan_bytes();
		char *my_string_buffer = yy_scan_bytes (pgm, n+1);

		int my_parse_result  = yyparse ();
		if	(my_parse_result)
			exit (1);
		yy_delete_buffer (my_string_buffer);

		yylex_destroy();

		disp_mallinfo();

		param_ret_t *pret = codeGen (programBlock, NULL);
		if	(i == count-1) {
			if	(pret) {
				RTL_TRDBG (TRACE_DETAIL, "Code returns boolean %d\n", pret->p.BOOLvar);
			}
			else {
				RTL_TRDBG (TRACE_DETAIL, "Code returns nothing\n");
			}
		}
		if	(pret)
			free (pret);

		disp_mallinfo();
		cleanUp (programBlock);
		disp_mallinfo();
	}

	free (pgm);
	return 0;
}
