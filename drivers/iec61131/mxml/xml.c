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
#include <stddef.h>
#include <unistd.h>
#include <sys/time.h>
#include <mxml.h>
#include "malloc.h"

void disp_mallinfo() {
	struct mallinfo info = mallinfo();
	printf ("mallinfo : total allocated space:  %d bytes\n", info.uordblks);
}

void scan_variable(mxml_node_t *top) {
	void *node = top;
	int flg = MXML_DESCEND_FIRST;
	while	((node = mxmlFindElement(node, top, "variable", NULL, NULL, flg))) {
		char *name = (char *)mxmlElementGetAttr(node, "name");
		printf ("  variable %s\n", name);
		flg = MXML_NO_DESCEND;
	}
}

void scan_globalVars(mxml_node_t *top) {
	void *node = top;
	int flg = MXML_DESCEND_FIRST;
	while	((node = mxmlFindElement(node, top, "globalVars", NULL, NULL, flg))) {
		//char *name = (char *)mxmlElementGetAttr(node, "name");
		printf (" globalVars\n");
		scan_variable(node);
		flg = MXML_NO_DESCEND;
	}
}

void scan_pouInstances(mxml_node_t *top) {
	mxml_node_t *node = top;
	int flg = MXML_DESCEND_FIRST;
	while	((node = mxmlFindElement(node, top, "pouInstance", NULL, NULL, flg))) {
		char *name = (char *)mxmlElementGetAttr(node, "name");
		printf ("   pouInstance %s\n", name);
		flg = MXML_NO_DESCEND;
	}
}

void scan_tasks(mxml_node_t *top) {
	mxml_node_t *node = top;
	int flg = MXML_DESCEND_FIRST;
	while	((node = mxmlFindElement(node, top, "task", NULL, NULL, flg))) {
		char *name = (char *)mxmlElementGetAttr(node, "name");
		printf ("  Task %s\n", name);
		scan_pouInstances(node);
		flg = MXML_NO_DESCEND;
	}
}

void scan_resources(mxml_node_t *top) {
	mxml_node_t *node = top;
	int flg = MXML_DESCEND_FIRST;
	while	((node = mxmlFindElement(node, top, "resource", NULL, NULL, flg))) {
		char *name = (char *)mxmlElementGetAttr(node, "name");
		printf (" Resource %s\n", name);
		scan_globalVars(node);
		scan_tasks(node);
		flg = MXML_NO_DESCEND;
	}
}

void scan_configurations(mxml_node_t *top) {
	mxml_node_t *node	= top;
	//	<configuration> is not directly under <project>. We use MXML_DESCEND.
	int flg = MXML_DESCEND;
	while	((node = mxmlFindElement(node, top, "configuration", NULL, NULL, flg))) {
		char *name = (char *)mxmlElementGetAttr(node, "name");
		printf ("Configuration %s\n", name);
		scan_globalVars(node);
		scan_resources(node);
		flg = MXML_NO_DESCEND;
	}
}

int cnt=0;
#define NB 10000

void Do(char *file) {
	struct timeval start, end;
	struct timezone tz;
	int i;

	disp_mallinfo();
	gettimeofday(&start, &tz);

	for (i=0; i<NB; i++, cnt++) {
		FILE *fp = fopen(file, "r");
		mxml_node_t *script = mxmlLoadFile(NULL, fp, MXML_OPAQUE_CALLBACK);
		//scan_configurations(script);
		mxmlDelete (script);
		//rewind (fp);
		fclose (fp);
	}

	gettimeofday(&end, &tz);
	disp_mallinfo();

	end.tv_sec -= start.tv_sec;
	if	(end.tv_usec < start.tv_usec) {
		end.tv_usec += 1000000;
		end.tv_sec --;
	}
	end.tv_usec -= start.tv_usec;
	double sec = (double)end.tv_sec + ((double)end.tv_usec/1000000);
	double nbpersec = NB / sec;
	printf ("cnt=%d usec=%f %f/s\n", cnt, sec, nbpersec);
}

int main(int ac, char **av) {
	FILE *fp = fopen(av[1], "r");
	int i, nb=1;
	if	(!fp)
		return	1;
	disp_mallinfo();
	mxml_node_t *script = mxmlLoadFile(NULL, fp, MXML_OPAQUE_CALLBACK);
	disp_mallinfo();
	fclose (fp);
	if	(ac >= 3)
		nb	= atoi(av[2]);

	for	(i=0; i<nb; i++)
		scan_configurations(script);
	mxmlDelete (script);

	Do (av[1]);
	return	0;
}
