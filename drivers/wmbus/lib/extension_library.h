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


#ifndef _EXTENSION_LIBRARY__H_
#define _EXTENSION_LIBRARY__H_

/**
 * @struct extensionLibrary_t
 * @brief Defines an access to an extension library.
 */
typedef struct extensionLibrary_t
{
  /* methods */
  /** @ref extensionLibrary_t_invoke */
  int (* invoke) (struct extensionLibrary_t *, char *input, int inputLen,
      char *output, int outputMaxLen);

  /** @ref extensionLibrary_t_dumpInfo */
  void (* dumpInfo) (struct extensionLibrary_t *, char *out, int outLen);


  /** @ref extensionLibrary_t_free */
  void (*free) (struct extensionLibrary_t *);

  /* members */
  /** For list management */
  struct list_head chainLink;

  /** The name of the extension library */
  char *libName;
  /** The name of the function in the extension library */
  char *methodName;
  /** Tells if the extension library was loaded successfully, and the function well found */
  int loaded;
  /** The handler on the opened library */
  void *libHandler;
  /** The handler on the function */
  fn_preprocessor fnHandler;
  /** The total number of times the function was invoked. */
  unsigned long nbTotalInvocations;
  /** The number of times the function succeeds. */
  unsigned long nbOkInvocations;

} extensionLibrary_t;

/* static allocation */
extensionLibrary_t extensionLibrary_t_create(char *libName, char *methodName);
void extensionLibrary_t_free(extensionLibrary_t *This);

/* dynamic allocation */
extensionLibrary_t *new_extensionLibrary_t(char *libName, char *methodName);
void extensionLibrary_t_newFree(extensionLibrary_t *This);

/* implementation methods */
int extensionLibrary_t_invoke(extensionLibrary_t *This, char *input, int inputLen,
    char *output, int outputMaxLen);

void extensionLibrary_t_dumpInfo(extensionLibrary_t *This, char *out, int outLen);


#endif

