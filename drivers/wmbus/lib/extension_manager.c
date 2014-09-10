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
#include <stddef.h>

#include "rtlbase.h"
#include "rtllist.h"
#include "wmbus.h"

#include "extension_library.h"
#include "extension_manager.h"

/*****************************************************************************/
/* Constructors / destructor */

/* static allocation */

/**
 * @brief Initialize the extensionManager_t struct
 * @anchor extensionManager_t_init
 * @param This the class instance
 */
static void extensionManager_t_init(extensionManager_t *This)
{
  RTL_TRDBG(TRACE_DETAIL, "extensionManager_t::init\n");

  /* Assign the pointers on functions */
  This->add = extensionManager_t_add;
  This->get = extensionManager_t_get;
  This->dumpInfo = extensionManager_t_dumpInfo;

  /* Initialize the members */
  INIT_LIST_HEAD(&This->extensionsList);

}


/**
 * @brief do static allocation of a extensionManager_t.
 * @anchor extensionManager_t_create
 * @return the copy of the object
 */
extensionManager_t extensionManager_t_create()
{
  extensionManager_t This;
  extensionManager_t_init(&This);
  This.free = extensionManager_t_free;
  RTL_TRDBG(TRACE_DETAIL, "extensionManager_t::create (This:0x%.8x)\n", &This);
  return This;
}

/**
 * @brief Destructor for static allocation
 * @anchor extensionManager_t_free
 */
void extensionManager_t_free(extensionManager_t *This)
{
  RTL_TRDBG(TRACE_DETAIL, "extensionManager_t::free (This:0x%.8x)\n", This);

  // free the member resources here
  while ( ! list_empty(&This->extensionsList))
  {
    extensionLibrary_t *el = list_entry(This->extensionsList.next, extensionLibrary_t, chainLink);
    list_del(&el->chainLink);
    el->free(el);
  }
}

/* dynamic allocation */
/**
 * @brief do dynamic allocation of a extensionManager_t.
 * @anchor new_extensionManager_t
 * @return the copy of the object
 */
extensionManager_t *new_extensionManager_t()
{
  extensionManager_t *This = malloc(sizeof(extensionManager_t));
  if (!This)
  {
    return NULL;
  }
  extensionManager_t_init(This);
  This->free = extensionManager_t_newFree;
  RTL_TRDBG(TRACE_DETAIL, "new extensionManager_t (This:0x%.8x)\n", This);
  return This;
}

/**
 * @brief Destructor for dynamic allocation
 * @anchor extensionManager_t_newFree
 */
void extensionManager_t_newFree(extensionManager_t *This)
{
  RTL_TRDBG(TRACE_DETAIL, "extensionManager_t::newFree (This:0x%.8x)\n", This);
  // free the member resources here
  while ( ! list_empty(&This->extensionsList))
  {
    extensionLibrary_t *el = list_entry(This->extensionsList.next, extensionLibrary_t, chainLink);
    list_del(&el->chainLink);
    el->free(el);
  }

  free(This);
}

/* implementation methods */

/**
 * @brief Add a new entry in the extension library list if it does not already exists and
 * library can be found.
 * @anchor extensionManager_t_add
 * @param This the class instance
 * @param libName the name of the extension library.
 * @param method the name of the method to load.
 * @return 0 if failed to add the entry (library or function in library not found), 1
 * otherwise (already known or added successfully).
 */
int extensionManager_t_add(extensionManager_t *This, char *libName, char *method)
{
  extensionLibrary_t *el = NULL;
  if (NULL != This->get(This, libName))
  {
    // already known
    return 1;
  }

  el = new_extensionLibrary_t(libName, method);
  if (el->loaded)
  {
    // loaded successfully
    list_add_tail(&el->chainLink, &This->extensionsList);
    return 1;
  }

  return 0;
}

/**
 * @brief Get the extension library representation that matches the provided libName.
 * @anchor extensionManager_t_get
 * @param This the class instance
 * @param libName the name of the extension library.
 * @return the extension library representation if found, null otherwise.
 */
extensionLibrary_t *extensionManager_t_get(extensionManager_t *This, char *libName)
{
  struct list_head *link;
  extensionLibrary_t *res = NULL;
  extensionLibrary_t *el = NULL;

  if (libName && *libName)
  {
    list_for_each (link, &This->extensionsList)
    {
      el = list_entry(link, extensionLibrary_t, chainLink);
      if (!strcmp(el->libName, libName))
      {
        res = el;
        break;
      }
    }
  }

  return res;
}

/**
 * @brief Build a string representation of all the loaded extension libraries.
 * @anchor extensionManager_t_dumpInfo
 * @param This the class instance.
 * @param out the buffer in which the representation is dumped in.
 * @param outLen the size of the out buffer.
 */
void extensionManager_t_dumpInfo(extensionManager_t *This, char *out, int outLen)
{
  extensionLibrary_t *el = NULL;
  struct list_head *link;
  char tmpBuf[255];
  int tmpLen;

  strcpy(out, "");
  list_for_each (link, &This->extensionsList)
  {
    el = list_entry(link, extensionLibrary_t, chainLink);
    el->dumpInfo(el, tmpBuf, 255);
    tmpLen = strlen(tmpBuf);
    if (outLen > tmpLen)
    {
      strcat(out, tmpBuf);
      outLen -= tmpLen;
    }
    else
    {
      if (outLen > 3)
      {
        strcat(out, "...");
      }
      break;
    }
  }
}


