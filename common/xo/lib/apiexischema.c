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

/**
 * @file apiexishema.c
 * @brief Manage EXI schemas
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <rtlhtbl.h>


#include "xoapipr.h"

#include "xmlstr.h"

#include <EXISerializer.h>
#include <stringManipulate.h>
#include <grammarGenerator.h>

#define MAX_XSD_FILES_COUNT 200

static EXIPSchema g_schema;
EXIPSchema *g_schemaPtr = NULL;

static int my_strcut(char *str, char *tok, char **args, int szargs) {
        register char   *pt, *opt;
        int     n;

        opt = pt = str;
        n       = 0;
        memset (args, 0, szargs*sizeof(char *));
        while   ((pt = strstr(pt, tok)) && (n < szargs-1)) {
                *pt++           = 0;
                args[n++]       = opt;
                opt             = pt;
        }
        if      (*opt && (n < szargs))
                args[n++]       = opt;
        return  n;
}

/**
 * @brief Load a list of schema files. The resulting schema can be accessable with the
 *	g_schemaPtr global variable.
 * @param fileNames List of filenames
 * @param schemaFilesCount The number of filenames in the list
 * @return void
 */
/*DEF*/ void XoLoadEXISchemaFiles(char** fileNames, unsigned int schemaFilesCount)
{
	FILE *schemaFile;
	BinaryBuffer buffer[MAX_XSD_FILES_COUNT];
	errorCode tmp_err_code;
	unsigned int i, cnt=0;

	if (schemaFilesCount > MAX_XSD_FILES_COUNT)
		schemaFilesCount	= MAX_XSD_FILES_COUNT;

	for (i = 0; i < schemaFilesCount; i++) {
		schemaFile = fopen(fileNames[i], "rb" );
		if(!schemaFile)
		{
			char	tmp[1024]; 

// chercher d'abord chez jb
			sprintf	(tmp,"%s/etc/xo/%s",
				getenv("ROOTACT"),fileNames[i]);
			schemaFile = fopen(tmp, "rb" );
			if(!schemaFile)
			{
				sprintf	(tmp,"%s/m2mxoref/exixsd/%s",
					getenv("ROOTACT"),fileNames[i]);
				schemaFile = fopen(tmp, "rb" );
			}
			if(!schemaFile)
				continue;
		}

		//printf ("XoLoadEXISchemaFiles %s\n", fileNames[i]);
		fseek(schemaFile, 0, SEEK_END);
		buffer[cnt].bufLen = ftell(schemaFile) /* + 1 */;
		fseek(schemaFile, 0, SEEK_SET);

		buffer[cnt].buf = (char *) malloc(buffer[cnt].bufLen);
		if (!buffer[cnt].buf) {
			fprintf(stderr,"[%s:%d] Memory allocation error!", __FILE__, __LINE__);
			fclose(schemaFile);
			continue;
		}

		if(fread(buffer[cnt].buf, buffer[cnt].bufLen, 1, schemaFile)!=1)
		{
			fprintf(stderr,"[%s:%d] fread error!", __FILE__, __LINE__);
			fclose(schemaFile);
			continue;
		}

		buffer[cnt].bufContent = buffer[cnt].bufLen;
		buffer[cnt].ioStrm.readWriteToStream = NULL;
		buffer[cnt].ioStrm.stream = NULL;
		cnt ++;
	}

	tmp_err_code = generateSchemaInformedGrammars(buffer, cnt, SCHEMA_FORMAT_XSD_EXI, NULL, &g_schema, NULL);

	if	(tmp_err_code) {
		fprintf (stderr,"[%s:%d] generateSchemaInformedGrammars with %d schemas => error %d\n",
			__FILE__, __LINE__, cnt, tmp_err_code);
	}

	if(tmp_err_code != EXIP_OK) {
		g_schemaPtr	= NULL;
	}
	else {
		g_schemaPtr	= &g_schema;
	}

	for(i = 0; i < cnt; i++)
		free(buffer[i].buf);
}

/**
 * @brief Load schema file list (separated by comma). The resulting schema can be accessable with the
 *	g_schemaPtr global variable.
 * @param str A comma separated list of files
 * @return void
 */
/*DEF*/ void XoLoadEXISchemas(char* str)
{
	char	*fileNames[MAX_XSD_FILES_COUNT];
	int	n;

	str	= strdup(str);	// my_strcut modifies str
	n	= my_strcut(str, ",", fileNames, MAX_XSD_FILES_COUNT);
	XoLoadEXISchemaFiles (fileNames, n);
	free (str);
}

/**
 * @brief Load all .exs files in the path. The resulting schema can be accessable with the
 *	g_schemaPtr global variable.
 * @param path Path where to search schema files
 * @return void
 */
/*DEF*/	void XoLoadEXISchemaDir(char *path)
{
	void	*dir;
	char	*name;
	char	*point;
	char	*fileNames[MAX_XSD_FILES_COUNT];
	int schemaFilesCount = 0, i;

	dir	= xo_openDir(path);
	if	(!dir)
		return;

	while	( (name = xo_readAbsDir(dir)) && *name && (schemaFilesCount < MAX_XSD_FILES_COUNT)) {
		if	(*name == '.' && *(name+1) == '.')
			continue;
		point	= xo_suffixname(name);
		if	(!point || !*point || strcmp(point, ".exs"))
			continue;

		fileNames[schemaFilesCount++]	= strdup(name);
	}
	xo_closeDir(dir);

	XoLoadEXISchemaFiles (fileNames, schemaFilesCount);

	for	(i=0; i<schemaFilesCount; i++)
		free (fileNames[i]);

	return;
}

void XoFreeExiSchema() {
	if	(g_schemaPtr) {
		destroySchema (g_schemaPtr);
		g_schemaPtr	= NULL;
	}
}

EXIPSchema *XoExiLoadSchema(char *schemaID) {
	static void *g_schemaTbl = NULL;
	if	(!g_schemaTbl)
		g_schemaTbl = rtl_htblCreate(128, NULL);

	void *sch = rtl_htblGet(g_schemaTbl, schemaID);
	if	(sch) {
		g_schemaPtr	= sch;
	}
	else {
		char	tmp[100];
/*
#define NFILES 4
		char *files[NFILES] = { tmp, "contentInstances.exs", "contentInstance.exs", "common.exs" };
*/
#define NFILES 2
		char *files[NFILES] = { tmp, "common.exs" };

		XoFreeExiSchema ();
		strcpy (tmp, schemaID);
		strcat (tmp, ".exs");
		XoLoadEXISchemaFiles(files, NFILES);
		if	(g_schemaPtr)
			rtl_htblInsert (g_schemaTbl, schemaID, g_schemaPtr);
	}
	return	g_schemaPtr;
}
