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
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include "vasprintf.h"
#include "tools.h"
#include "operating.h"

static char *ConfigDir = NULL;
static char *TemplatesDir = NULL;
static char *LogsDir = NULL;
static int OngBox = 0;

void Operating_Init() {
	if	(ConfigDir)
		return;
	char *root = getenv("ROOTACT");
	char *path = NULL;
	struct stat st;

	iec_asprintf (&path, "%s/etc/iec61131", root);

	if	(stat(path, &st) < 0) {
		//	We are in a development system
		iec_asprintf (&ConfigDir, "%s/iec61131/config", root);
		iec_asprintf (&TemplatesDir, "%s/iec61131/files", root);
		iec_asprintf (&LogsDir, "/tmp/iec61131.log");
		OngBox = 0;
	}
	else {
		//	We are in an ONG box
		iec_asprintf (&ConfigDir, "%s/usr/etc/iec61131", root);
		if	(stat(ConfigDir, &st) < 0) {
			Free (ConfigDir);
			// Fallback to factory settings
			iec_asprintf (&ConfigDir, "%s/etc/iec61131", root);
		}
		iec_asprintf (&TemplatesDir, "%s/etc/iec61131/files", root);
		iec_asprintf (&LogsDir, "%s/var/log/iec61131/TRACE.log", root);
		//mkdir (LogsDir, 0600);
		OngBox = 1;
	}
	free (path);
}

char *Operating_ConfigDir() {
	Operating_Init();
	return	ConfigDir;
}

char *Operating_TemplatesDir() {
	Operating_Init();
	return	TemplatesDir;
}

char *Operating_LogsDir() {
	Operating_Init();
	return	LogsDir;
}

int Operating_OngBox() {
	Operating_Init();
	return	OngBox;
}
