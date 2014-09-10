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
#include <string.h>

#include "rtlbase.h"
#include "rtllist.h"
#include "cmnstruct.h"
#include "speccproto.h"

#include "wmbus.h"
#include "extension_library.h"

extern int Operating;

/*****************************************************************************/
/* Constructors / destructor */

/* static allocation */

/**
 * @brief Initialize the extensionLibrary_t struct
 * @anchor extensionLibrary_t_init
 * @param This the class instance
 */
static void extensionLibrary_t_init(extensionLibrary_t *This)
{
  char load_path[1024];
  RTL_TRDBG(TRACE_DETAIL, "extensionLibrary_t::init\n");

  /* Assign the pointers on functions */
  This->invoke = extensionLibrary_t_invoke;
  This->dumpInfo = extensionLibrary_t_dumpInfo;

  /* Initialize the members */
  This->loaded = 0;
  This->libHandler = NULL;
  This->fnHandler = NULL;
  This->nbTotalInvocations = 0;
  This->nbOkInvocations = 0;

  // try to load the library and to find the function
  if (!Operating)
  {
    snprintf(load_path, 1024, "%s/%s/lib/preprocessor/%s.so", getenv("ROOTACT"),
        GetAdaptorName(), This->libName);
  }
  else
  {
    snprintf(load_path, 1024, "%s/lib/%s/preprocessor/%s.so", getenv("ROOTACT"),
        GetAdaptorName(), This->libName);
  }

  This->libHandler = dlopen(load_path, RTLD_LAZY | RTLD_LOCAL);
  if (This->libHandler == NULL)
  {
    RTL_TRDBG(TRACE_INFO, "Can't load shared library : '%s'\n", load_path);
    return ;
  }

  This->fnHandler = dlsym(This->libHandler, This->methodName);
  if (This->fnHandler == NULL)
  {
    RTL_TRDBG(TRACE_INFO, "Can't load function '%s' from library '%s'\n",
        This->methodName, This->libName);
    return ;
  }

  This->loaded = 1;
}


/**
 * @brief do static allocation of a extensionLibrary_t.
 * @anchor extensionLibrary_t_create
 * @param libName the name of the extension library to open.
 * @param methodName the name of the method to load.
 * @return the copy of the object
 */
extensionLibrary_t extensionLibrary_t_create(char *libName, char *methodName)
{
  extensionLibrary_t This;
  This.libName = strdup(libName);
  This.methodName = strdup(methodName);
  extensionLibrary_t_init(&This);
  This.free = extensionLibrary_t_free;
  RTL_TRDBG(TRACE_DETAIL, "extensionLibrary_t::create (This:0x%.8x)\n", &This);
  return This;
}

/**
 * @brief Destructor for static allocation
 * @anchor extensionLibrary_t_free
 * @param This the instance of the extension library representation.
 */
void extensionLibrary_t_free(extensionLibrary_t *This)
{
  RTL_TRDBG(TRACE_DETAIL, "extensionLibrary_t::free (This:0x%.8x)\n", This);

  // free the member resources here
  if (This->loaded && This->libHandler)
  {
    dlclose(This->libHandler);
  }
  free(This->libName);
  free(This->methodName);
}

/* dynamic allocation */
/**
 * @brief do dynamic allocation of a extensionLibrary_t.
 * @anchor new_extensionLibrary_t
 * @param libName the name of the extension library to open.
 * @param methodName the name of the method to load.
 * @return the copy of the object
 */
extensionLibrary_t *new_extensionLibrary_t(char *libName, char *methodName)
{
  extensionLibrary_t *This = malloc(sizeof(extensionLibrary_t));
  if (!This)
  {
    return NULL;
  }
  This->libName = strdup(libName);
  This->methodName = strdup(methodName);
  extensionLibrary_t_init(This);
  This->free = extensionLibrary_t_newFree;
  RTL_TRDBG(TRACE_DETAIL, "new extensionLibrary_t (This:0x%.8x)\n", This);
  return This;
}

/**
 * @brief Destructor for dynamic allocation
 * @anchor extensionLibrary_t_newFree
 * @param This the instance of the extension library representation.
 */
void extensionLibrary_t_newFree(extensionLibrary_t *This)
{
  RTL_TRDBG(TRACE_DETAIL, "extensionLibrary_t::newFree (This:0x%.8x)\n", This);
  // free the member resources here
  if (This->loaded && This->libHandler)
  {
    dlclose(This->libHandler);
  }
  free(This->libName);
  free(This->methodName);
  free(This);
}

/* implementation methods */

/**
 * @brief Invoke the function provided in the extension library.
 * @anchor extensionLibrary_t_invoke
 * @param This the instance of the extension library representation.
 * @param input the input buffer to convert.
 * @param inputLen the length of the input buffer.
 * @param output the buffer where the result has to be dumped.
 * @param outputMaxLen the size of the output buffer.
 * @return -1 if the function call fails, a positive or null integer otherwise, providing
 * with the extension lib call return code.
 */
int extensionLibrary_t_invoke(extensionLibrary_t *This, char *input, int inputLen,
    char *output, int outputMaxLen)
{
  int ret = 0;
  This->nbTotalInvocations++;
  if (! This->loaded)
  {
    RTL_TRDBG(TRACE_ERROR, "extensionLibrary_t::invoke - ERROR: library was not loaded "
        "successfully (This:0x%.8x)\n", This);
    return -1;
  }

  ret = This->fnHandler(input, inputLen, output, outputMaxLen);
  if (ret < 0)
  {
    RTL_TRDBG(TRACE_INFO, "Preprocessor failed, error = %d\n", ret);
    return -1;
  }
  RTL_TRDBG(TRACE_INFO, "Preprocessing data with '%s:%s' done (ret:%d)\n", This->libName,
      This->methodName, ret);

  This->nbOkInvocations++;

  return ret;
}

/**
 * @brief Build a string representation of extension library.
 * @param This the instance of the extension library representation.
 * @param out the buffer where the result has to be dumped.
 * @param outLen the size of the output buffer.
 */
void extensionLibrary_t_dumpInfo(extensionLibrary_t *This, char *out, int outLen)
{
  snprintf(out, outLen, "{ (lib:%s) (fct:%s) (loaded:%d) (invoke:%ld/%ld) }",
      This->libName, This->methodName, This->loaded, This->nbOkInvocations,
      This->nbTotalInvocations);
}

