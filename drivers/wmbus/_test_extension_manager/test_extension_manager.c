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

//************************************************************************
//************************************************************************
//**
//** File : ./_test_extension_manager/test_extension_manager.c
//**
//** Description  :
//**
//**
//** Date : 2013-05-24
//** Author :  Mathias Louiset, Actility (c), 2013.
//** Modified by :
//**
//** Task :
//**
//** REVISION HISTORY:
//**
//**  Date      Version   Name
//**  --------    -------   ------------
//**
//************************************************************************
//************************************************************************

#include <stdlib.h>
#include <stdio.h>
#include <dlfcn.h>
#include <unistd.h>
#include <fcntl.h>   /* File control definitions */
#include <string.h>
#include <stddef.h>
#include <assert.h>

#include "rtlbase.h"
#include "rtllist.h"
#include "wmbus.h"

#include "extension_library.h"
#include "extension_manager.h"


int TraceLevel = 2;
int TraceDebug = 1;
int Operating = 0;

char* GetAdaptorName(void)
{
  return "wmbus/_test_extension_manager/extensions";
}

/******************************************************************************/
/*   _   _                      _          _
 *  | | | |_ _  __ _ _ _ _  _  | |_ ___ __| |_ ___
 *  | |_| | ' \/ _` | '_| || | |  _/ -_|_-<  _(_-<
 *   \___/|_||_\__,_|_|  \_, |  \__\___/__/\__/__/
 *                       |__/
 */

/**
 *
 */
void test_add()
{
  printf("*************************************\n");
  printf("******** test_add *******************\n");

  extensionManager_t * manager = new_extensionManager_t();

  assert(0 == manager->add(manager, "libThatDoesNotExists", "noMatter"));
  assert(NULL == manager->get(manager, "libThatDoesNotExists"));

  assert(1 == manager->add(manager, "libextension1", "extension1"));
  assert(NULL != manager->get(manager, "libextension1"));

  assert(1 == manager->add(manager, "libextension2", "extension2"));
  assert(NULL != manager->get(manager, "libextension2"));

  assert(0 == manager->add(manager, "libextension3", "wrongSymbol"));
  assert(NULL == manager->get(manager, "libextension3"));


  manager->free(manager);

  printf("\ntest_add OK\n\n");
}

/**
 *
 */
void test_invoke()
{
  const char in[] = "input string";
  const int outLen = 255;
  char out[outLen];  

  printf("*************************************\n");
  printf("******** test_invoke *******************\n");

  extensionManager_t * manager = new_extensionManager_t();
  extensionLibrary_t * library;

  assert(1 == manager->add(manager, "libextension1", "extension1"));
  assert(1 == manager->add(manager, "libextension2", "extension2"));

  library = manager->get(manager, "libextension1");
  assert(NULL != library);
  assert(1 == library->invoke(library, (char *)in, sizeof(in), out, outLen));
  assert(! strcmp("extension1 computation done", out));

  library = manager->get(manager, "libextension2");
  assert(NULL != library);
  assert(-1 == library->invoke(library, (char *)in, sizeof(in), out, outLen));
  assert(! strcmp("extension2 computation failed", out));

  // WARNING! prototype of implemented method in extension lib has to be the right one,
  // as far as dlsym only looks for the name of the symbol but not the parameters.
  // If one tries to invoke the function with wrong parameters, it will cause the rise
  // of a SEGFLT exception.

  manager->free(manager);

  printf("\ntest_invoke OK\n\n");
}

/**
 *
 */
void test_stats()
{
  const char in[] = "input string";
  const int outLen = 255;
  char out[outLen];  

  printf("*************************************\n");
  printf("******** test_invoke *******************\n");

  extensionManager_t * manager = new_extensionManager_t();
  extensionLibrary_t * library;

  assert(1 == manager->add(manager, "libextension1", "extension1"));
  assert(1 == manager->add(manager, "libextension2", "extension2"));
  assert(1 == manager->add(manager, "libextension3", "extension3"));

  library = manager->get(manager, "libextension1");
  assert(NULL != library);
  assert(1 == library->invoke(library, (char *)in, sizeof(in), out, outLen));
  assert(! strcmp("extension1 computation done", out));
  assert(1 == library->invoke(library, (char *)in, sizeof(in), out, outLen));

  library = manager->get(manager, "libextension2");
  assert(NULL != library);
  assert(-1 == library->invoke(library, (char *)in, sizeof(in), out, outLen));
  assert(! strcmp("extension2 computation failed", out));
  assert(-1 == library->invoke(library, (char *)in, sizeof(in), out, outLen));
  assert(-1 == library->invoke(library, (char *)in, sizeof(in), out, outLen));

  library = manager->get(manager, "libextension3");
  assert(NULL != library);
  assert(1 == library->invoke(library, (char *)in, 200, out, outLen));
  assert(-1 == library->invoke(library, (char *)in, 201, out, outLen));
  assert(1 == library->invoke(library, (char *)in, 200, out, outLen));
  assert(1 == library->invoke(library, (char *)in, 200, out, outLen));

  // check statistics
  library = manager->get(manager, "libextension1");
  assert(2 == library->nbTotalInvocations);
  assert(2 == library->nbOkInvocations);

  library = manager->get(manager, "libextension2");
  assert(3 == library->nbTotalInvocations);
  assert(0 == library->nbOkInvocations);

  library = manager->get(manager, "libextension3");
  assert(4 == library->nbTotalInvocations);
  assert(3 == library->nbOkInvocations);

  manager->free(manager);

  printf("\ntest_invoke OK\n\n");
}

/******************************************************************************/
/**
 * @brief The main entry point oof the test program.
 * It invokes every tests.
 * @param argc
 * @param argv
 * @return 0 if all tests complete successfully; the program is
 * aborted if a test fails (see tests)
 */
int main(int argc, char *argv[])
{
  if (argc == 2)
  {
    TraceLevel = atoi(argv[1]);
    rtl_init();
    rtl_tracelevel(TraceLevel);
  }

  test_add();
  test_invoke();
  test_stats();

  printf("\t ___\n");
  printf("\t/ __|_  _ __ __ ___ ______\n");
  printf("\t\\__ \\ || / _/ _/ -_|_-<_-<\n");
  printf("\t|___/\\_,_\\__\\__\\___/__/__/\n");
  printf("\t\n");

  return 0;
}
