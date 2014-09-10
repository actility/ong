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


#ifndef _EXTENSION_MANAGER__H_
#define _EXTENSION_MANAGER__H_

/**
 * @struct extensionManager_t
 * @brief Defines a tool for extension libraries management.
 */
typedef struct extensionManager_t
{
  /* methods */
  /** @ref extensionManager_t_add */
  int (* add) (struct extensionManager_t *, char *libName, char *method);
  /** @ref extensionManager_t_get */
  extensionLibrary_t *(* get) (struct extensionManager_t *, char *libName);

  /** @ref extensionManager_t_dumpInfo */
  void (* dumpInfo) (struct extensionManager_t *, char *out, int outLen);


  /** @ref extensionManager_t_free */
  void (* free) (struct extensionManager_t *);

  /* members */
  /** The list of extensions libraries */
  struct list_head extensionsList;

} extensionManager_t;

/* static allocation */
extensionManager_t extensionManager_t_create();
void extensionManager_t_free(extensionManager_t *This);

/* dynamic allocation */
extensionManager_t *new_extensionManager_t();
void extensionManager_t_newFree(extensionManager_t *This);

/* implementation methods */
int extensionManager_t_add(extensionManager_t *This, char *libName, char *method);
extensionLibrary_t *extensionManager_t_get(extensionManager_t *This, char *libName);

void extensionManager_t_dumpInfo(extensionManager_t *This, char *out, int outLen);


#endif

