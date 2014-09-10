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

#include <stdio.h>      /************************************/
#include <stdlib.h>     /*     run in main thread           */
#include <string.h>     /************************************/
#include <stdarg.h>
#include <stddef.h>
#include <unistd.h>
#include <pthread.h>
#include <errno.h>
#include <limits.h>
#include <signal.h>
#include <poll.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <netdb.h>

#include "rtlbase.h"
#include "rtllist.h"
#include "rtlimsg.h"

#include "adaptor.h"

#include "xoapipr.h"

#include "define.h"

#include "cmndefine.h"
#include "cmnstruct.h"
#include "cmnproto.h"

#include "cproto.h"
#include "extern.h"
#include "dIa.h"
#include "covconfig.h"
#include "wmbus.h"

#include "extension_library.h"
#include "extension_manager.h"

extern t_cli *TbCli[];
extern extensionManager_t *preprocessorMgmt;

/**
 * @brief Display the help message.
 * @param cl the CLI session handler.
 *
 */
void AdmUsage(t_cli *cl)
{
  AdmPrint(cl, "quit|exit - quit the telnet session.\n");
  AdmPrint(cl, "stop - shutdown the driver.\n");
  AdmPrint(cl, "who - display the list of active CLI connections.\n");
  AdmPrint(cl, "close - ends all CLI sessions.\n");
  AdmPrint(cl, "core - dump a core file of the current process.\n");
  AdmPrint(cl, "xoocc - display statistics on Xo usage.\n");

  AdmPrint(cl, "trace [level] - display the trace level or change it.\n");
  AdmPrint(cl, "getvar [varname] - display the internal variable(s) values.\n");
  AdmPrint(cl, "setvar varname [value] - change the value of internal variable.\n");
  AdmPrint(cl, "addvar varname - add a new internal variable.\n");

  AdmPrint(cl, "help - display this help message.\n");
  AdmPrint(cl, "info - display general information on driver state.\n");
  AdmPrint(cl, "covcfg - display the active COV configuration.\n");
  AdmPrint(cl, "extensionsInfo - display information related to extension libraries.\n");
}

/**
 * @brief Format and display information regarding dirver usage (dIa stats, enabled options...).
 * @param cl the CLI session handler.
 */
void AdmInfo(t_cli *cl)
{
  unsigned long vsize;
  unsigned int nbm;
  unsigned int nbt;

  vsize = rtl_vsize(getpid());

  nbm = rtl_imsgVerifCount(MainIQ);
  nbt = rtl_timerVerifCount(MainIQ);

  AdmPrint(cl, "vsize=%fMo (%u)\n", ((float)vsize) / 1024.0 / 1024.0, vsize);
  AdmPrint(cl, "pseudoong=%d withdia=%d\n", PseudoOng, WithDia);

  AdmPrint(cl, "#msg=%d\n", nbm);
  AdmPrint(cl, "#timer=%d\n", nbt);
  AdmPrint(cl, "diapending=%u(%u)\n", DiaNbReqPending, DiaMaxReqPending);
  AdmPrint(cl, "diasendcount=%u\n", DiaSendCount);
  AdmPrint(cl, "diarecvcount=%u\n", DiaRecvCount);
  AdmPrint(cl, "diatimecount=%u\n", DiaTimeCount);
  AdmPrint(cl, "diadisconnect=%u\n", DiaDisconnected());
}

/**
 * @brief Format and display information regarding loaded extension library and their usage.
 * @param cl the CLI session handler.
 */
void AdmExtensionsInfo(t_cli *cl)
{
  char info[1024];
  preprocessorMgmt->dumpInfo(preprocessorMgmt, info, 1024);
  AdmPrint(cl, "%s\n", info);
}


/**
 * Do evaluate and run the "trace" CLI command.
 * @param cl the CLI context.
 * @param buf the entered command.
 */
void AdmTrace(t_cli *cl, char *buf)
{
  int  v;
  buf  += 5;
  buf  = AdmAdvance(buf);
  if  (strlen(buf))
  {
    v  = atoi(buf);
    TraceLevel  = v;
    rtl_tracelevel(TraceLevel);
  }
  AdmPrint(cl, "trace=%d debug=%d proto=%d\n",
      TraceLevel, TraceDebug, TraceProto);
}


/**
 * @brief Call back method for driver' specific CLI commands.
 * @param cl the CLI session handler.
 * @param pbuf the received command.
 * @return CLI_PROMPT when the entered command is supported, CLI_NFOUND otherwise.
 */
int AdmCmd(t_cli *cl, char *pbuf)
{

  char  *buf  = pbuf;

  if  (!strlen(buf))
  {
    return  CLI_PROMPT;
  }

  if  (strcmp(buf, "help") == 0 || buf[0] == '?')
  {
    AdmUsage(cl);
    return  CLI_PROMPT;
  }

  if  (strcmp(buf, "info") == 0)
  {
    AdmInfo(cl);
    return  CLI_PROMPT;
  }

  if  (strncmp(buf, "trace", 5) == 0)
  {
    AdmTrace(cl, buf);
    return  CLI_PROMPT;
  }

  if (strncmp(buf, "covcfg", 6) == 0)
  {
    CovToCzDump(cl, buf);
    return  CLI_PROMPT;
  }

  if  (strcmp(buf, "extensionsInfo") == 0)
  {
    AdmExtensionsInfo(cl);
    return  CLI_PROMPT;
  }

  return  CLI_NFOUND;
}


