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
//** File : driver/main.c
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
#include "xoapipr.h"
#include "dIa.h"

#include "azap_types.h"
#include "azap_api_af.h"
#include "azap_api_zcl.h"
#include "azap_api.h"
#include "azap_zcl_attr_val.h"
#include "azap_attribute.h"
#include "azap_endpoint.h"
#include "azap_listener.h"


#include "cmndefine.h"
#include "cmnstruct.h"

#include "cmnproto.h"
#include "speccproto.h"
#include "specextern.h"
#include "adaptor.h"

#include "interface_mapping_desc.h"
#include "driver_req_context.h"
#include "zigbee_node.h"
#include "m2m_abstraction.h"
#include "zigbee_han.h"

#define MAX_FILE_PATH 255


void *MdlCfg;

int ActiveSniffer = 0;
int TraceLevel = 2;
int TraceDebug = 1;
int TraceProto = 0;
int TraceSize = 200 * 1024; // for each file
int PseudoOng = 0;
int WithDia = 1;
bool resetHanAtReboot = false;
int DiaTraceRequest = 0;
int Operating = 1;
void *MainTbPoll;
void *MainIQ;
int running = 1;

azap_uint8 azapPort;
zigbeeHan_t *theHan = NULL;
char *knownSensorPath = NULL;

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
    Serial++;
  }
  return Serial;
}

void DoClockMs()
{
#if  0
  RTL_TRDBG(TRACE_FULL, "DoClockMs()\n");
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
  static unsigned  int  nbclock  = 0;
  time_t now = 0;

//  RTL_TRDBG(TRACE_FULL,"DoClockSc()\n");
  nbclock++;
  //time(&now);
  rtl_timemono(&now);
  AdmClockSc(now);
  DiaClockSc(now);
  azapClockSc(now);
  // watch for dIa request to be sent.
  theHan->publish(theHan);
}

/**
 * @brief Dispatcher for internal events
 * @param imsg the received event.
 */
void  DoInternalEvent(t_imsg *imsg)
{
  RTL_TRDBG(TRACE_API, "receive event cl=%d ty=%d\n", imsg->im_class, imsg->im_type);
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
  RTL_TRDBG(TRACE_API, "receive timer cl=%d ty=%d vsize=%ld\n",
      imsg->im_class, imsg->im_type, rtl_vsize(getpid()));

  switch (imsg->im_class)
  {
    case IM_M2M_ABSTR_TIMERS:
    {
      m2mAbstraction_t *m2m = getM2mAbstraction();
      m2m->doInternalTimer(m2m, imsg);
    }
    break;

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
  time_t  lasttimems  = 0;
  time_t  lasttimesc  = 0;
  time_t  now    = 0;
  //int  ret;

  t_imsg  *msg;

  while (running)
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
    now  = rtl_tmmsmono();
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
  printf("usage: %s [-h|--help] [-t <level>] [-d <0|1>] [-D] [-O] [-X] [-i]\n", prg);
  printf("Where:\n");
  printf("\t-h|--help : print this help message\n");
  printf("\t-t <level>: set the trace level to <level>\n");
  printf("\t-d <0|1>  : disable/enable trace debugging\n");
  printf("\t-D        : disable DIA layer\n");
  printf("\t-O        : enable pseudo ONG running mode\n");
  printf("\t-X        : enable trace on DIA requests/responses\n");
  printf("\t-i        : reset ZigBee HAN at start-up (if not provided the \n");
  printf("\t            HAN is resumed from previous run)\n");
  printf("\t--version : print the version string of the ZigBee driver\n");
  printf("\t            and its dependencies\n");
}

/**
 * @brief Do evaluate the arguments provided at program launch.
 * @param initial
 * @param argc the number of arguments
 * @param argv the table of arguments as strings
 * @return the
 */
int DoArg(int initial, int argc, char *argv[])
{
  extern char *optarg;
  extern int optind;
  int  c;
  char *pt;

  char *fmtgetopt  = "hXDOit:d:";

  int  i;

  for  (i = 1 ; i < argc ; i++)
  {
    if  (strcmp(argv[i], "--version") == 0)
    {
      pt  = GetAdaptorVersion();
      printf("%s\n", rtl_version());
      printf("%s\n", XoVersion());
      printf("%s\n", dia_getVersion());
      printf("%s\n", azapVersion());
      printf("%s\n", pt);
      if (strcmp(pt, zigbee_whatStr) != 0)
      {
        printf("%s\n", zigbee_whatStr);
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

  if  (initial)
  {
    return argc;
  }

  while ((c = getopt (argc, argv, fmtgetopt)) != -1)
  {
    switch (c)
    {
      case  'h' :
        Usage(argv[0], fmtgetopt);
        exit(0);
        break;
      case  't' :
        TraceLevel  = atoi(optarg);
        break;
      case  'd' :
        TraceDebug  = atoi(optarg);
        break;
      case  'i' :
        resetHanAtReboot = true;
        break;
      case  'D' :
        WithDia  = 0;
        break;
      case  'O' :
        PseudoOng  = 1;
        break;
      case  'X' :
        DiaTraceRequest  = 1;
        break;

      default :
        exit(1);
        break;
    }
  }

  return  argc;
}

/**
 */
void SetOption(char *name, char *value)
{
  if (strcmp(name, "tracelevel") == 0)
  {
    TraceLevel = atoi(value);
    return;
  }
  if (strcmp(name, "tracedebug") == 0)
  {
    TraceDebug = atoi(value);
    return;
  }
  if  (strcmp(name, "tracesize") == 0)
  {
    TraceSize = atoi(value);
    return;
  }
  if (strcmp(name, "withdia") == 0)
  {
    WithDia = atoi(value);
    return;
  }
  if (strcmp(name, "tracedia") == 0)
  {
    DiaTraceRequest = atoi(value);
    return;
  }
  if (strcmp(name, "pseudoong") == 0)
  {
    PseudoOng = atoi(value);
    return;
  }

  RTL_TRDBG(TRACE_ERROR, "ERROR parameter/option '%s' not found\n", name);
}

/**
 */
void DoConfig(int hot, int custom, char *fcfg)
{
  void *o;
  void *var;
  int parseerr;
  int nb;
  int i;

  o = XoReadXmlEx(fcfg, NULL, XOML_PARSE_OBIX, &parseerr);
  if  (parseerr > 0)
  {
    RTL_TRDBG(TRACE_ERROR, "ERROR config file '%s' parsing error=%d \n",
        fcfg, parseerr);
    return;
  }
  if  (parseerr < 0)
  {
    RTL_TRDBG(TRACE_ERROR, "ERROR config file '%s' loading error=%d \n",
        fcfg, parseerr);
    return;
  }

  if (!o)
  {
    RTL_TRDBG(TRACE_ERROR, "ERROR config file '%s' not found\n", fcfg);
    return;
  }
//  XoSaveAscii(o,stdout);
  nb = XoNmNbInAttr(o, "[name=parameters].[]", 0);
  for (i = 0 ; i < nb ; i++)
  {
    char *name;
    char *value;

    var = XoNmGetAttr(o, "[name=parameters].[%d]", 0, i);
    if (!var)
    {
      continue;
    }

    name = XoNmGetAttr(var, "name", 0);
    value = XoNmGetAttr(var, "val", 0);

    RTL_TRDBG(TRACE_ERROR, "hot=%d cust=%d parameter='%s' value='%s'\n",
        hot, custom, name, value);
    if (name && *name && value && *value)
    {
      SetOption(name, value);
    }
  }
  nb  = XoNmNbInAttr(o, "[name=variables].[]", 0);
  for  (i = 0 ; i < nb ; i++)
  {
    char  *name;
    char  *value;

    var  = XoNmGetAttr(o, "[name=variables].[%d]", 0, i);
    if  (!var)
    {
      continue;
    }

    name = XoNmGetAttr(var, "name", 0);
    value = XoNmGetAttr(var, "val", 0);

    RTL_TRDBG(TRACE_ERROR, " hot=%d cust=%d variable='%s' value='%s'\n",
        hot, custom, name, value);
    if  (name && *name && value && *value)
    {
      SetVar(name, value);
    }
  }

  // custom configuration can not change m2m mapping
  nb  = XoNmNbInAttr(o, "[name=mapping].[]", 0);
  for (i = 0 ; custom == 0 && i < nb ; i++)
  {
    //char  tmp[256];
    char  *name;
    char  *value;

    var = XoNmGetAttr(o, "[name=mapping].[%d]", 0, i);
    if  (!var)
    {
      continue;
    }

    name  = XoNmGetAttr(var, "name", 0);
    value = XoNmGetAttr(var, "val", 0);

    RTL_TRDBG(TRACE_ERROR, "hot=%d cust=%d mapping='%s' value='%s'\n",
        hot, custom, name, value);
    if (name && *name && value && *value)
    {
      AddVar(name);
      SetVar(name, value);
    }
  }

  if (custom == 0)
  {
    theHan->loadMappingFromXoRef(theHan, o);
  }
}

/**
 * @brief Initialize AZAP, add end points and ZCL attributes, and start the layer.
 * @return 1 if AZAP starts successfully, 0 otherwise.
 */
int startAzap()
{
  azap_uint16 haEndPointInClusterList[ ] =
  {
    ZCL_CLUSTER_ID_GEN_BASIC,
  };
  azap_uint16 haEndPointOutClusterList[ ] =
  {
  };

  /////////////////////////////////////////////////////////////////////////////
  // Initialization
  azapInit(NULL, &azapPort, MainTbPoll, getListener(), theHan->rfChan, 0, resetHanAtReboot);

  /////////////////////////////////////////////////////////////////////////////
  // Adding administrative end points
  azap_uint8 epId = addAdminEndPoint(ZCL_HA_PROFILE_ID);
  if (epId == 0)
  {
    RTL_TRDBG(TRACE_ERROR, "unable to allocate end point for profile 0x%.4x\n",
        ZCL_HA_PROFILE_ID);
    return 0;
  }
  azapAddEndPoint(
    azapPort,
    epId,
    ZCL_HA_PROFILE_ID,
    ZCL_HA_DEVICEID_TEST_DEVICE,
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
  azapAddZCLAttribute(azapPort, epId, ZCL_CLUSTER_ID_GEN_BASIC, ATTRID_BASIC_ZCL_VERSION,
      ZCL_DATATYPE_UINT8, ACCESS_CONTROL_READ, zclVersion, sizeof(zclVersion));
  // ATTRID_BASIC_HW_VERSION (0x0003)
  azap_byte hwVersion[] = { 0x00 };
  azapAddZCLAttribute(azapPort, epId, ZCL_CLUSTER_ID_GEN_BASIC, ATTRID_BASIC_HW_VERSION,
      ZCL_DATATYPE_UINT8, ACCESS_CONTROL_READ, hwVersion, sizeof(hwVersion));
  // ATTRID_BASIC_MANUFACTURER_NAME (0x0004)
  azap_byte manufacturer[] = { strlen("ACTILITY"), 'A', 'C', 'T', 'I', 'L', 'I', 'T', 'Y' };
  azapAddZCLAttribute(azapPort, epId, ZCL_CLUSTER_ID_GEN_BASIC, ATTRID_BASIC_MANUFACTURER_NAME,
      ZCL_DATATYPE_CHAR_STR, ACCESS_CONTROL_READ, manufacturer, sizeof(manufacturer));
  // ATTRID_BASIC_MODEL_ID (0x0005)
  azap_byte modelId[] = { strlen("ACT/ONG coord"), 'A', 'C', 'T', '/', 'O', 'N', 'G',
      ' ', 'c', 'o', 'o', 'r', 'd'
                        };
  azapAddZCLAttribute(azapPort, epId, ZCL_CLUSTER_ID_GEN_BASIC, ATTRID_BASIC_MODEL_ID,
      ZCL_DATATYPE_CHAR_STR, ACCESS_CONTROL_READ, modelId, sizeof(modelId));
  // ATTRID_BASIC_DATE_CODE (0x0006)
  azap_byte dateCode[] = { strlen("20110915"), '2', '0', '1', '1', '0', '9', '1', '5' };
  azapAddZCLAttribute(azapPort, epId, ZCL_CLUSTER_ID_GEN_BASIC, ATTRID_BASIC_DATE_CODE,
      ZCL_DATATYPE_CHAR_STR, ACCESS_CONTROL_READ, dateCode, sizeof(dateCode));
  // ATTRID_BASIC_POWER_SOURCE (0x0007)
  azap_byte powerSource[] = { POWER_SOURCE_MAINS_1_PHASE };
  azapAddZCLAttribute(azapPort, epId, ZCL_CLUSTER_ID_GEN_BASIC, ATTRID_BASIC_POWER_SOURCE,
      ZCL_DATATYPE_UINT8, ACCESS_CONTROL_READ, powerSource, sizeof(powerSource));

  // ATTRID_BASIC_LOCATION_DESC (0x0010)
  azap_byte LocationDescription[] = { 16, ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',
      ' ', ' ', ' ', ' ', ' ', ' '
                                    };
  azapAddZCLAttribute(azapPort, epId, ZCL_CLUSTER_ID_GEN_BASIC, ATTRID_BASIC_LOCATION_DESC,
      ZCL_DATATYPE_CHAR_STR, ACCESS_CONTROL_READ | ACCESS_CONTROL_WRITE, LocationDescription,
      sizeof(LocationDescription));
  // ATTRID_BASIC_PHYSICAL_ENV (0x0011)
  azap_byte physicalEnv[] = { 0 };
  azapAddZCLAttribute(azapPort, epId, ZCL_CLUSTER_ID_GEN_BASIC, ATTRID_BASIC_PHYSICAL_ENV,
      ZCL_DATATYPE_UINT8, ACCESS_CONTROL_READ | ACCESS_CONTROL_WRITE, physicalEnv,
      sizeof(physicalEnv));
  // ATTRID_BASIC_DEVICE_ENABLED (0x0012)
  azap_byte deviceEnabled[] = { DEVICE_ENABLED };
  azapAddZCLAttribute(azapPort, epId, ZCL_CLUSTER_ID_GEN_BASIC, ATTRID_BASIC_DEVICE_ENABLED,
      ZCL_DATATYPE_UINT8, ACCESS_CONTROL_READ | ACCESS_CONTROL_WRITE, deviceEnabled,
      sizeof(deviceEnabled));

  // second administrative end point, for SE profile
  epId = addAdminEndPoint(ZCL_SE_PROFILE_ID);
  if (epId == 0)
  {
    RTL_TRDBG(TRACE_ERROR, "unable to allocate end point for profile 0x%.4x\n",
        ZCL_SE_PROFILE_ID);
    return 0;
  }
  azapAddEndPoint(
    azapPort,
    epId,
    ZCL_SE_PROFILE_ID,
    ZCL_SE_DEVICEID_RANGE_EXTENDER,
    0,
    sizeof(haEndPointInClusterList) / sizeof(azap_uint16),
    haEndPointInClusterList,
    sizeof(haEndPointOutClusterList) / sizeof(azap_uint16),
    haEndPointOutClusterList);

  /////////////////////////////////////////////////////////////////////////////
  // and finally start the AZAP layer
  return azapStart(azapPort);
}


/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////


/**
 * @brief Delete all files of known nodes.
 * This operation is done when resetting the HAN.
 * @param path the path to the known-nodes files.
 */
static void deleteKnownNodes(char *path)
{
  void *dir;
  char *name;
  char *point;
  char fullPath[512];

  dir = rtl_openDir(path);
  if (!dir)
  {
    return;
  }

  while ( (name = rtl_readDir(dir)) && *name )
  {
    if (*name == '.' && *(name + 1) == '\0')
    {
      continue;
    }
    if (*name == '.' && *(name + 1) == '.')
    {
      continue;
    }
    point = rtl_suffixname(name);
    if ( point && *point && strcmp(point + 1, "xml") == 0 )
    {
      sprintf(fullPath, "%s/%s", path, name);
      remove(fullPath);
    }
  }
}

/**
 * Stop the driver's main loop.
 */
void stopService(int sig)
{
  RTL_TRDBG(TRACE_ERROR, "Service stopped (sig:%d)\n", sig);
  printf("Service stopped (sig:%d)\n", sig);
  running = 0;
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////


/**
 * The main entry point of the program...
 * @param argc number of arguments
 * @param argv the arguments as table of strings
 */
int main(int argc, char *argv[])
{
  char cfgroot[512];
  char cfgcust[512];
  char tmp[512];
  char *rootact;
  int initial;
  int hot;
  int custom;
  int i;
  int rfChan = 11;

  signal(SIGHUP, SIG_IGN);
  signal(SIGPIPE, SIG_IGN);
  signal(SIGTERM, stopService);

  rootact = getenv("ROOTACT");
  if ((!rootact || !*rootact))
  {
    printf("%s unset, abort\n", "ROOTACT");
    exit(1);
  }
  sprintf(cfgroot, "%s/etc/%s/spvconfig.xml", rootact, GetAdaptorName());
  sprintf(cfgcust, "%s/usr/etc/%s/spvconfig.xml", rootact, GetAdaptorName());

  DoArg(initial = 1, argc, argv);


  sprintf(tmp, "%s/usr/data/%s/knownsensors", rootact, GetAdaptorName());
  rtl_mkdirp(tmp);
  knownSensorPath = malloc(strlen(tmp) + 1);
  sprintf(knownSensorPath, "%s", tmp);

  rtl_init();

  rtl_tracemutex();
  rtl_tracelevel(TraceLevel);
  rtl_tracesizemax(TraceSize);
  sprintf(tmp, "%s/var/log/%s", rootact, GetAdaptorName());
  rtl_mkdirp(tmp);
  strcat(tmp, "/");
  strcat(tmp, "TRACE.log");
  rtl_tracerotate(tmp);
  RTL_TRDBG(TRACE_ERROR, "Starting azspv.x/main th=%lx pid=%d\n", (long)pthread_self(), getpid());
  RTL_TRDBG(TRACE_ERROR, "%s\n", rtl_version());
  RTL_TRDBG(TRACE_ERROR, "%s\n", XoVersion());
  RTL_TRDBG(TRACE_ERROR, "%s\n", dia_getVersion());
  RTL_TRDBG(TRACE_ERROR, "%s\n", azapVersion());
  RTL_TRDBG(TRACE_ERROR, "%s\n", zStackVersion(0));
  RTL_TRDBG(TRACE_ERROR, "%s\n", GetAdaptorVersion());

  MainTbPoll = rtl_pollInit();
  MainIQ = rtl_imsgInitIq();

  XoInit(0);
  sprintf(tmp, "%s/etc/xo/.", rootact);
  XoLoadNameSpaceDir(tmp);
  sprintf(tmp, "%s/etc/%s/.", rootact, GetAdaptorName());
  XoLoadNameSpaceDir(tmp);

  if (LoadM2MXoRef() < 0)
  {
    exit(1);
  }

  if (LoadDrvXoRef(GetAdaptorName()) < 0)
  {
    exit(1);
  }

  VarInit();  // before DoConfig()
  theHan = new_zigbeeHan_t(); // before DoConfig()

  // get host name and set variable accordingly, before DoConfig()
  if (0 == gethostname(tmp, sizeof(tmp)))
  {
    SetVar("w_boxname", tmp);
  }
  else
  {
    RTL_TRDBG(TRACE_ERROR, "Unable to get the host name; "
        "driver will run with erroneous host name %s", GetVar("w_boxname"));
  }


  DoConfig(hot = 0, custom = 0, cfgroot);
  DoConfig(hot = 0, custom = 1, cfgcust);

  // convert boxname and domainname to lowercase
  sprintf(tmp, "%s", GetVar("w_boxname"));
  for (i = 0; tmp[i]; i++)
  {
    tmp[i] = tolower(tmp[i]);
  }
  SetVar("w_boxname", tmp);
  sprintf(tmp, "%s", GetVar("w_domainname"));
  for (i = 0; tmp[i]; i++)
  {
    tmp[i] = tolower(tmp[i]);
  }
  SetVar("w_domainname", tmp);

  DoArg(initial = 0, argc, argv);
  rtl_tracelevel(TraceLevel);
  rtl_tracesizemax(TraceSize);

  RTL_TRDBG(TRACE_ERROR, "TraceLevel=%d\n", TraceLevel);
  RTL_TRDBG(TRACE_ERROR, "TraceSize=%d\n", TraceSize);
  RTL_TRDBG(TRACE_ERROR, "TraceDebug=%d\n", TraceDebug);
  RTL_TRDBG(TRACE_ERROR, "TraceProto=%d\n", TraceProto);
  RTL_TRDBG(TRACE_ERROR, "TraceDia=%d\n", DiaTraceRequest);
  RTL_TRDBG(TRACE_ERROR, "WithDia=%d\n", WithDia);
  RTL_TRDBG(TRACE_ERROR, "PseudoOng=%d\n", PseudoOng);
  RTL_TRDBG(TRACE_ERROR, "ResetHanAtReboot=%d\n", resetHanAtReboot);

  MdlCfg = MdlLoad(hot = 0);
  if (!MdlCfg)
  {
    exit(1);
  }

  if (resetHanAtReboot)
  {
    deleteKnownNodes(knownSensorPath);
  }

  AdmTcpInit();
  AdmTcpSetCB(AdmCmd);
  AdmTcpSetCBFree(AdmCliFree);

  if (WithDia)
  {
    DiaUdpInit();
  }

  // add new variables for this driver
  AddVar("a_appDevId");
  AddVar("a_appDevVers");
  AddVar("a_appProfId");
  AddVar("d_powLvl");
  AddVar("d_powMode");
  AddVar("d_powSrc");
  AddVar("d_shortAddr");
  AddVar("d_type");
  AddVar("w_panextid");
  for (i = 0; i < ITF_MAPPING_MAX_CONFIG_ATTR; i++)
  {
    sprintf(tmp, "a_configAttr%d", i);
    AddVar(tmp);
    sprintf(tmp, "a_configVal%d", i);
    AddVar(tmp);
  }
  for (i = 0; i < ITF_MAPPING_MAX_CONFIG_OPE; i++)
  {
    sprintf(tmp, "r_pathOpe%d", i);
    AddVar(tmp);
    SetVar(tmp, SUB_PATH_AR2);
  }
  if (GetVar("w_rfChan"))
  {
    rfChan = atoi(GetVar("w_rfChan"));
    if ((rfChan > 26) || (rfChan < 11))
    {
      rfChan = 11;
    }
  }

  theHan->rfChan = rfChan;


  if (!startAzap())
  {
    exit(1);
  }

  MainLoop();

  RTL_TRDBG(TRACE_ERROR, "end !!! zigbee.x/main th=%lx\n", (long)pthread_self());
  exit(1);
}


