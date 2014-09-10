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
#include <unistd.h>
#include <iostream>
#include "codegen.h"
#include "node.h"


int TraceDebug=1;
int TraceLevel=9;

using namespace std;

extern char *yy_scan_string(const char *);
extern void yy_delete_buffer(char *);
extern int yylex_destroy  (void);

extern int yyparse();
extern NBlock* programBlock;

int main(int argc, char **argv)
{
	char pgm[4096];
	int     fd, n;
	fd      = open(argv[1], 0);
	n = read (fd, pgm, sizeof(pgm)-1);
	pgm[n] = 0;
	close (fd);

	char *my_string_buffer = yy_scan_string (pgm);

	int my_parse_result  = yyparse ();
	if	(my_parse_result)
		return 1;
	//yy_delete_buffer (my_string_buffer);

	yylex_destroy();

	CodeGenContext context(NULL);
	IECParam *pret = context.generateCode(*programBlock);

	//delete programBlock;
	programBlock->undo();

	if	(pret)
		printf ("RET : %d\n", pret->iec.p.BOOLvar);
	else
		printf ("No RET\n");
	delete pret;
	return 0;
}
