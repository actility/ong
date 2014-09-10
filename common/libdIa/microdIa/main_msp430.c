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
#include <io430.h>
#include <stdio.h>
#include "microdIa.h"

void cb_diaCreateResponse(dia_context_t *dia, char *scode, char *resourceURI, void *ctt, int len, char* cttType,
        Opt *optHeader, int tid) {
}

int cb_write2Network(dia_context_t *dia, unsigned char *buf, int sz) {
      printf ("Write Net %d\n", sz);
        return  0;
}

dia_callbacks_t callbacks = {
        NULL,
        NULL,
        NULL,
        cb_diaCreateResponse,
        NULL,
        NULL,
        NULL,
        NULL,
        NULL,
        cb_write2Network
};

int main( void )
{
  // Stop watchdog timer to prevent time out reset
  WDTCTL = WDTPW + WDTHOLD;
  dia_context_t *dia = dia_createContext(&callbacks);
  int tid;
  
  diaCreateRequest (dia, NULL, "/titi/tata/toto", "<xml/>", 6, "application/xml", NULL, NULL, &tid);


  return 0;
}
