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
//** File : ./azap_tester/main.c
//**
//** Description  :
//**
//**
//** Date : 11/19/2012
//** Author :  Mathias Louiset, Actility (c), 2012.
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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <errno.h>
#include <limits.h>
#include <signal.h>
#include <poll.h>
#include <ctype.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#include "_whatstr.h"

#include "rtlbase.h"
#include "rtlimsg.h"

#include "azap_api.h"
#include "listener.h"
#include "admin_cli.h"

#define ABS(x)    ((x) > 0 ? (x) : -(x))
#define MSTIC   100
#define        IM_DEF                  1000
#define        IM_TIMER_GEN            1001
#define        IM_TIMER_GEN_V          10000   // ms


int TraceLevel = 9;
int TraceDebug = 1;
int TraceProto = 0;
int PseudoOng = 0;
int SoftwareSensor  = 0;
int WithDia = 1;
int resetHanAtReboot = 0;
int logOnStdOutput = 0;

int DiaTraceRequest = 0;

void *MainTbPoll = NULL;
void *MainIQ = NULL;

azap_uint8 azapPort;

/**
 * @brief Generate a serial number, e.g. for sensors
 * @return the serail number generated.
 */
int  DoSerial()
{
  static int Serial = INT_MAX;

  if (Serial == INT_MAX)
  {
    Serial = 1;
  }
  else
  {
    Serial ++;
  }
  return Serial;
}

void DoClockMs()
{
#if  0
  RTL_TRDBG(1, "DoClockMs()\n");
#endif

  azapClockMs();
}

/**
 * @brief Do synchronize each layer with actual time.
 * One second has elapsed since last call. Each layer that needs
 * to process periodical tasks (i.e. recurrent timers) can do this
 * from this call.
 * e.g. verifying each connections if some are inactives for a
 * long period.
 */
void  DoClockSc()
{
  static unsigned int nbclock = 0;
  time_t now = 0;

//  RTL_TRDBG(1,"DoClockSc()\n");
  nbclock++;
  rtl_timemono(&now);
  azapClockSc(now);
  AdmClockSc(now);
}

/**
 * @brief Dispatcher for internal events
 * @param imsg the received event.
 */
void  DoInternalEvent(t_imsg *imsg)
{
  RTL_TRDBG(1, "receive event cl=%d ty=%d\n", imsg->im_class, imsg->im_type);
  switch (imsg->im_class)
  {
//  case <msg class> :
//    doSomething();
//  break;

    default:
      break;
  }
}

/**
 * @brief Dispatcher for internal timers
 * @param imsg the received associated event.
 */
void  DoInternalTimer(t_imsg *imsg)
{
  RTL_TRDBG(1, "receive timer cl=%d ty=%d vsize=%ld\n",
      imsg->im_class, imsg->im_type, rtl_vsize(getpid()));

  switch (imsg->im_class)
  {
//  case <msg class> :
//    doSomething();
//  break;

    default:
      break;
  }
}

/**
 * @brief Main loop in the program.
 * forever:
 *   - process all pending internal events,
 *   - process a single external event
 *  (- process periodic scheduled tasks over all layers)
 *   - process all internal timers
 */
void  MainLoop()
{
  time_t lasttimems = 0;
  time_t lasttimesc = 0;
  time_t now = 0;
  //int ret = 0;

  t_imsg  *msg;

  while  (1)
  {
    // internal events
    while ((msg = rtl_imsgGet(MainIQ, IMSG_MSG)) != NULL)
    {
      DoInternalEvent(msg);
      rtl_imsgFree(msg);
    }

    // external events
    /* ret = */ rtl_poll(MainTbPoll, MSTIC);

    // clocks
    now  = rtl_tmms();
    if  (ABS(now - lasttimems) >= 100)
    {
      DoClockMs();
      lasttimems  = now;
    }
    if  (ABS(now - lasttimesc) >= 1000)
    {
      DoClockSc();
      lasttimesc  = now;
    }

    // internal timer
    while ((msg = rtl_imsgGet(MainIQ, IMSG_TIMER)) != NULL)
    {
      DoInternalTimer(msg);
      rtl_imsgFree(msg);
    }
  }
}


/**
 * @brief Print help message for program, print help on options.
 * @param prg the program name
 * @param fmt the formatted options
 */
void Usage(char *prg, char *fmt)
{
  printf("usage: %s %s\n", prg, fmt);
  printf(" where options are:\n");
  printf(" -h          print this help message\n");
  printf(" -t <level>  run with trace level <level>\n");
  printf(" -d <level>  run with debug level <level>\n");
  printf(" -o          redirect trace to standard output\n");
  printf(" -r          reset the HAN at start-up\n");
  printf("\n");
}

/**
 * @brief Do evaluate the arguments provided at program launch.
 * @param argc the number of arguments
 * @param argv the table of arguments as strings
 * @return the
 */
int DoArg(int argc, char *argv[])
{
  extern char *optarg;
  extern int optind;
  int  c;
  char *pt;

  char *fmtgetopt  = "ht:d:or";

  int  i;

  for  (i = 1 ; i < argc ; i++)
  {
    if  (strcmp(argv[i], "--version") == 0)
    {
      pt  = azapVersion();
      printf("%s\n", rtl_version());
      printf("%s\n", pt);
      if (strcmp(pt, azap_whatStr) != 0)
      {
        printf("%s\n", azap_whatStr);
        printf("compilation required\n");
      }
      exit(0);
    }
    if (strcmp(argv[i], "--help") == 0)
    {
      Usage(argv[0], fmtgetopt);
      exit(0);
    }
  }

  while ((c = getopt (argc, argv, fmtgetopt)) != -1)
  {
    switch (c)
    {
      case  'h' :
        Usage(argv[0], fmtgetopt);
        exit(0);
        break;
      case  'o' :
        logOnStdOutput = 1;
        break;
      case  'r' :
        resetHanAtReboot = 1;
        break;
      case  't' :
        TraceLevel  = atoi(optarg);
        break;
      case  'd' :
        TraceDebug  = atoi(optarg);
        break;

      default :
        break;
    }
  }

  return  argc;
}

/**
 */
void  SetOption(char *name, char *value)
{
  if  (strcmp(name, "tracelevel") == 0)
  {
    TraceLevel  = atoi(value);
    return;
  }
  if  (strcmp(name, "tracedebug") == 0)
  {
    TraceDebug  = atoi(value);
    return;
  }

  RTL_TRDBG(0, "ERROR parameter/option '%s' not found\n", name);
}

/**
 * @brief Initialize AZAP, add end points and ZCL attributes, and start the layer.
 */
void startAzap()
{
  azap_uint16 haEndPointInClusterList[ ] =
  {
    ZCL_CLUSTER_ID_GEN_BASIC,
  };
  azap_uint16 haEndPointOutClusterList[ ] =
  {
    ZCL_CLUSTER_ID_GEN_ALARMS,
  };

  /////////////////////////////////////////////////////////////////////////////
  // Initialization
  azapInit(NULL, &azapPort, MainTbPoll, getListener(), 11, 0, resetHanAtReboot);

  /////////////////////////////////////////////////////////////////////////////
  // Adding administrative end points
  azapAddEndPoint(
    azapPort,
    1,
    ZCL_HA_PROFILE_ID,
    ZCL_HA_DEVICEID_TEST_DEVICE,
    0,
    sizeof(haEndPointInClusterList) / sizeof(azap_uint16),
    haEndPointInClusterList,
    sizeof(haEndPointOutClusterList) / sizeof(azap_uint16),
    haEndPointOutClusterList);
  azapAddEndPoint(
    azapPort,
    2,
    ZCL_SE_PROFILE_ID,
    ZCL_SE_DEVICEID_RANGE_EXTENDER,
    0,
    sizeof(haEndPointInClusterList) / sizeof(azap_uint16),
    haEndPointInClusterList,
    sizeof(haEndPointOutClusterList) / sizeof(azap_uint16),
    haEndPointOutClusterList);

  /////////////////////////////////////////////////////////////////////////////
  // Adding ZCL attributes to administrative end points
  // Add BASIC cluster attributes
  // ATTRID_BASIC_ZCL_VERSION (0x0000)
  azap_byte zclVersion[] = { 0x00 };
  azapAddZCLAttribute(azapPort, 1, ZCL_CLUSTER_ID_GEN_BASIC, ATTRID_BASIC_ZCL_VERSION,
      ZCL_DATATYPE_UINT8, ACCESS_CONTROL_READ, zclVersion, sizeof(zclVersion));
  // ATTRID_BASIC_HW_VERSION (0x0003)
  azap_byte hwVersion[] = { 0x00 };
  azapAddZCLAttribute(azapPort, 1, ZCL_CLUSTER_ID_GEN_BASIC, ATTRID_BASIC_HW_VERSION,
      ZCL_DATATYPE_UINT8, ACCESS_CONTROL_READ, hwVersion, sizeof(hwVersion));
  // ATTRID_BASIC_MANUFACTURER_NAME (0x0004)
  azap_byte manufacturer[] = { strlen("ACTILITY"), 'A', 'C', 'T', 'I', 'L', 'I', 'T', 'Y' };
  azapAddZCLAttribute(azapPort, 1, ZCL_CLUSTER_ID_GEN_BASIC, ATTRID_BASIC_MANUFACTURER_NAME,
      ZCL_DATATYPE_CHAR_STR, ACCESS_CONTROL_READ, manufacturer, sizeof(manufacturer));
  // ATTRID_BASIC_MODEL_ID (0x0005)
  azap_byte modelId[] = { strlen("ACT/ONG coord"), 'A', 'C', 'T', '/', 'O', 'N', 'G',
      ' ', 'c', 'o', 'o', 'r', 'd'
                        };
  azapAddZCLAttribute(azapPort, 1, ZCL_CLUSTER_ID_GEN_BASIC, ATTRID_BASIC_MODEL_ID,
      ZCL_DATATYPE_CHAR_STR, ACCESS_CONTROL_READ, modelId, sizeof(modelId));
  // ATTRID_BASIC_DATE_CODE (0x0006)
  azap_byte dateCode[] = { strlen("20110915"), '2', '0', '1', '1', '0', '9', '1', '5' };
  azapAddZCLAttribute(azapPort, 1, ZCL_CLUSTER_ID_GEN_BASIC, ATTRID_BASIC_DATE_CODE,
      ZCL_DATATYPE_CHAR_STR, ACCESS_CONTROL_READ, dateCode, sizeof(dateCode));
  // ATTRID_BASIC_POWER_SOURCE (0x0007)
  azap_byte powerSource[] = { POWER_SOURCE_MAINS_1_PHASE };
  azapAddZCLAttribute(azapPort, 1, ZCL_CLUSTER_ID_GEN_BASIC, ATTRID_BASIC_POWER_SOURCE,
      ZCL_DATATYPE_UINT8, ACCESS_CONTROL_READ, powerSource, sizeof(powerSource));

  // ATTRID_BASIC_LOCATION_DESC (0x0010)
  azap_byte LocationDescription[] = { 16, ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',
      ' ', ' ', ' ', ' ', ' ', ' '
                                    };
  azapAddZCLAttribute(azapPort, 1, ZCL_CLUSTER_ID_GEN_BASIC, ATTRID_BASIC_LOCATION_DESC,
      ZCL_DATATYPE_CHAR_STR, ACCESS_CONTROL_READ | ACCESS_CONTROL_WRITE, LocationDescription,
      sizeof(LocationDescription));
  // ATTRID_BASIC_PHYSICAL_ENV (0x0011)
  azap_byte physicalEnv[] = { 0 };
  azapAddZCLAttribute(azapPort, 1, ZCL_CLUSTER_ID_GEN_BASIC, ATTRID_BASIC_PHYSICAL_ENV,
      ZCL_DATATYPE_UINT8, ACCESS_CONTROL_READ | ACCESS_CONTROL_WRITE, physicalEnv,
      sizeof(physicalEnv));
  // ATTRID_BASIC_DEVICE_ENABLED (0x0012)
  azap_byte deviceEnabled[] = { DEVICE_ENABLED };
  azapAddZCLAttribute(azapPort, 1, ZCL_CLUSTER_ID_GEN_BASIC, ATTRID_BASIC_DEVICE_ENABLED,
      ZCL_DATATYPE_UINT8, ACCESS_CONTROL_READ | ACCESS_CONTROL_WRITE, deviceEnabled,
      sizeof(deviceEnabled));

  /////////////////////////////////////////////////////////////////////////////
  // and finally start the AZAP layer
  azapStart(azapPort);
}

/**
 * The main entry point of the program...
 * @param argc number of arguments
 * @param argv the arguments as table of strings
 */
int main(int argc, char *argv[])
{

  signal(SIGPIPE, SIG_IGN);

  rtl_init();

  DoArg(argc, argv);

  rtl_tracemutex();
  rtl_tracelevel(TraceLevel);
  if (0 == logOnStdOutput)
  {
    rtl_tracerotate("./TRACE.log");
  }
  RTL_TRDBG(0, "start azspv.x/main th=%lx pid=%d\n", (long)pthread_self(), getpid());
  RTL_TRDBG(0, "%s\n", rtl_version());
  RTL_TRDBG(0, "%s\n", azapVersion());

  MainTbPoll = rtl_pollInit();
  MainIQ = rtl_imsgInitIq();

  //rtl_tracelevel(TraceLevel);

  RTL_TRDBG(0, "TraceLevel=%d\n", TraceLevel);
  RTL_TRDBG(0, "TraceDebug=%d\n", TraceDebug);

  AdmTcpInit();

  startAzap();

  MainLoop();

  RTL_TRDBG(0, "end !!! %s/main th=%lx\n", argv[0], (long)pthread_self());
  exit(1);
}


