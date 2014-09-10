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

#include <stdio.h>            /************************************/
#include <stdlib.h>            /*     run in main thread           */
#include <string.h>            /************************************/
#include <unistd.h>
#include <pthread.h>
#include <errno.h>
#include <limits.h>
#include <signal.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>

#include "rtlbase.h"
#include "rtlimsg.h"
#include "xoapipr.h"

#include "adaptor.h"

#include "define.h"
#include "state.h"
#include "struct.h"

#include "cproto.h"
#include "cmnproto.h"
#include "extern.h"
#include "wmbus.h"

t_cov *TbCz[CZ_MAX];
int NbCz;

// also used by cli => not static
void DoCovDataTranslation( t_cov *cz)
{
  // FFS
}


static
void
AddCovCz(int cluster, int attribut, void *cov)
{
  t_cov *cz;
  char *pt;
  int mint;
  int maxt;

  if (NbCz >= CZ_MAX)
  {
    RTL_TRDBG(TRACE_ERROR, "ERROR max cov translators %d...\n", NbCz);
    return;
  }

  cz = ( t_cov *) malloc(sizeof( t_cov));
  if (!cz)
  {
    return;
  }

  memset(cz, 0, sizeof( t_cov));
  cz->cz_index = NbCz;
  cz->cz_cluster = cluster;
  cz->cz_attribut = attribut;

  pt = XoNmGetAttr(cov, "ong:minInterval", NULL);
  if (!pt || !*pt)
  {
    mint = 5 * 60;
  }
  else
  {
    mint = parseISO8601Duration(pt);
  }
  cz->cz_mint = mint;

  pt = XoNmGetAttr(cov, "ong:maxInterval", NULL);
  if (!pt || !*pt)
  {
    maxt = 60 * 60;
  }
  else
  {
    maxt = parseISO8601Duration(pt);
  }
  cz->cz_maxt = maxt;
  /*
      mincov = XoNmGetAttr(cov,"ong:minCOV",NULL);
      if (!mincov || !*mincov)
          mincov = "1";
      cz->cz_mincov = mincov;

      DoCovDataTranslation(cz);
  */
  TbCz[NbCz] = cz;
  NbCz++;
}

void
CovInit(void *mdl)
{
  int nbitf;
  int i;
  void *itf;
  int nbcov;
  int c;
  void *cov;

  char *fitf;
  char *fatt;

  char *pt;
  int cluster;
  int attribut;

  nbitf = XoNmNbInAttr(mdl, "ong:interfaces");
  for    (i = 0 ; i < nbitf ; i++)
  {
    itf = XoNmGetAttr(mdl, "ong:interfaces[%d]", NULL, i);
    if    (!itf)
    {
      continue;
    }

    fitf = XoNmGetAttr(itf, "ong:filter", NULL, 0);
    if    (!fitf || !*fitf)
    {
      continue;
    }

    pt = strrchr(fitf, '/');
    if    (pt)
    {
      pt++;
    }
    else
    {
      pt = fitf;
    }
    sscanf    (pt, "%x", &cluster);
    nbcov = XoNmNbInAttr(itf, "ong:covConfigurations");
    for (c = 0 ; c < nbcov ; c++)
    {
      cov = XoNmGetAttr(itf, "ong:covConfigurations[%d]", NULL, c);
      if    (!cov)
      {
        continue;
      }

      fatt = XoNmGetAttr(cov, "ong:filter", NULL, 0);
      if    (!fatt || !*fatt)
      {
        continue;
      }

      pt = strrchr(fatt, '/');
      if (pt)
      {
        pt++;
      }
      else
      {
        pt = fatt;
      }
      sscanf (pt, "%x", &attribut);
      AddCovCz(cluster, attribut, cov);
    }
  }
}


void CovToCzDump(void *cl, char *buf)
{
  t_cov    *cz;
  int i;

  AdmPrint(cl, "current cov configuration loaded :\n");

  for (i = 0 ; i < NbCz ; i++)
  {
    cz = TbCz[i];
    if    (!cz)
    {
      continue;
    }

    AdmPrint(cl, "[%03d] cluster=0x%04x attr=0x%04x self=%x\n",
        cz->cz_index, cz->cz_cluster, cz->cz_attribut, cz);

    AdmPrint(cl, "[%03d] mint=%d maxt=%d\n",
        cz->cz_index, cz->cz_mint, cz->cz_maxt);

    AdmPrint(cl, "------------------------------\n");
  }
}

int
CovGetMinIntervalForAttr(int attr)
{
  int i;
  t_cov *cz;

  for (i = 0; i < NbCz; i++)
  {
    cz = TbCz[i];

    if (!cz)
    {
      continue;
    }

    if (cz->cz_attribut == attr)
    {
      return cz->cz_mint;
    }
  }

  // Not found
  return 5 * 60;
}

