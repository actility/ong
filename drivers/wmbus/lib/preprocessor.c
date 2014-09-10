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

#include <stdlib.h>
#include <stdio.h>
#include <dlfcn.h>
#include <unistd.h>
#include <fcntl.h>   /* File control definitions */

#include "rtlbase.h"
#include "wmbus.h"
#include "xoapipr.h"
#include "speccproto.h"
#include <stddef.h>

#include "rtlbase.h"
#include "rtllist.h"
#include "wmbus.h"

#include "extension_library.h"
#include "extension_manager.h"

// FFS : Add a cache to avoid lot a dlopen / dlsym / dlclose

extern char *rootactPath;
extern  int Operating;
extern extensionManager_t *preprocessorMgmt;

int
wmbusManufacturerPreprocessorFindHandler(wmBusHeader_t *header, char *lib, char *fnct)
{
  void *o, *elem;
  int parse, i, nb;
  char sfile[1024];
  char *tmp;
  unsigned long long int ieee, min, max;

  *lib = '\0';
  *fnct = '\0';

  //DUMP_MEMORY(header, sizeof(wmBusHeader_t));

  if  (!Operating)
  {
    snprintf (sfile, 1024, "%s%s/lib/preprocessor/manufacturer.xml", rootactPath, GetAdaptorName());
  }
  else
  {
    snprintf(sfile, 1024, "%s/etc/%s/manufacturer.xml", rootactPath, GetAdaptorName());
  }

  if (access(sfile, R_OK) != 0)
  {
    RTL_TRDBG(TRACE_INFO, "manufacturer.xml does not exist '%s'\n", sfile);
    return  -1;
  }

  o = XoReadXmlEx(sfile, NULL, 0, &parse);
  if (!o)
  {
    RTL_TRDBG(TRACE_ERROR, "ERROR ret=%d cannot read xml '%s'\n", parse, sfile);
    return  -2;
  }

  // Foreach
  ieee = HEADER_TO_IEEE(header);
  nb  = XoNmNbInAttr(o, "wmbus:preprocessors", 0);

  for (i = 0; i < nb; i++)
  {
    elem  = XoNmGetAttr(o, "wmbus:preprocessors[%d]", 0, i);
    if  (!elem)
    {
      continue;
    }

    tmp = XoNmGetAttr(elem, "wmbus:min", 0);
    sscanf(tmp, "%llx", &min);

    tmp = XoNmGetAttr(elem, "wmbus:max", 0);
    sscanf(tmp, "%llx", &max);

    RTL_TRDBG(TRACE_INFO, "min = %llx, max = %llx, ieee = %llx \n", min, max, ieee);

    if ((ieee >= min) && (ieee <= max))
    {
      tmp = XoNmGetAttr(elem, "wmbus:module", 0);
      snprintf(lib, 256, "%s", tmp);

      tmp = XoNmGetAttr(elem, "wmbus:fcnt", 0);
      snprintf(fnct, 256, "%s", tmp);

      XoFree(o, 1);
      return 0;
    }
  }

  XoFree(o, 1);

  return -1;
}

int
wmbusManufacturerCmdFindHandler(unsigned long long int ieee, char *lib, char *fnct)
{
  void *o, *elem;
  int parse, i, nb;
  char sfile[1024];
  char *tmp;
  unsigned long long int min, max;

  *lib = '\0';
  *fnct = '\0';

  if  (!Operating)
  {
    snprintf (sfile, 1024, "%s%s/lib/preprocessor/manufacturer.xml", rootactPath, GetAdaptorName());
  }
  else
  {
    snprintf(sfile, 1024, "%s/etc/%s/manufacturer.xml", rootactPath, GetAdaptorName());
  }

  if (access(sfile, R_OK) != 0)
  {
    RTL_TRDBG(TRACE_INFO, "'%s' does not exist\n", sfile);
    return  -1;
  }

  o = XoReadXmlEx(sfile, NULL, 0, &parse);
  if (!o)
  {
    RTL_TRDBG(TRACE_ERROR, "ERROR ret=%d cannot read xml '%s'\n", parse, sfile);
    return  -2;
  }

  // Foreach
  nb  = XoNmNbInAttr(o, "wmbus:preprocessors", 0);

  for (i = 0; i < nb; i++)
  {
    elem  = XoNmGetAttr(o, "wmbus:preprocessors[%d]", 0, i);
    if  (!elem)
    {
      continue;
    }

    tmp = XoNmGetAttr(elem, "wmbus:min", 0);
    sscanf(tmp, "%llx", &min);

    tmp = XoNmGetAttr(elem, "wmbus:max", 0);
    sscanf(tmp, "%llx", &max);

    RTL_TRDBG(TRACE_INFO, "min = %llx, max = %llx, ieee = %llx \n", min, max, ieee);

    if ((ieee >= min) && (ieee <= max))
    {
      tmp = XoNmGetAttr(elem, "wmbus:module", 0);
      snprintf(lib, 256, "%s", tmp);

      tmp = XoNmGetAttr(elem, "wmbus:fcntCmd", 0);
      snprintf(fnct, 256, "%s", tmp);

      XoFree(o, 1);
      return 0;
    }
  }

  XoFree(o, 1);

  return -1;
}

int
wmbusManufacturerPreprocessor(char *input, int inputLen, char *output, int outputMaxLen)
{
  int ret;
  char lib[256];
  char fnct[256];
  extensionLibrary_t *library;

  // Use wMBus header to find the correct lib / fnct
  ret = wmbusManufacturerPreprocessorFindHandler((wmBusHeader_t *)input, lib, fnct);
  if (ret < 0)
  {
    RTL_TRDBG(TRACE_INFO, "Can't find preprocessor handler\n");
    return -1;
  }

  library = preprocessorMgmt->get(preprocessorMgmt, lib);
  if (NULL == library)
  {
    if (! preprocessorMgmt->add(preprocessorMgmt, lib, fnct))
    {
      RTL_TRDBG(TRACE_ERROR, "Can't load preprocessor handler (lib:%s) (fnct:%s)\n", lib, fnct);
      return -1;
    }
    library = preprocessorMgmt->get(preprocessorMgmt, lib);
  }

  return library->invoke(library, input, inputLen, output, outputMaxLen);
}

