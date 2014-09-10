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

/*! @file sensor.c
 *
 */
#include <stdio.h>      /************************************/
#include <stdlib.h>     /*     run in main thread           */
#include <string.h>     /************************************/
#include <unistd.h>
#include <pthread.h>
#include <errno.h>
#include <limits.h>
#include <signal.h>
#include <ctype.h>
#include <dlfcn.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>

#include "rtlbase.h"
#include "rtlimsg.h"

#include "adaptor.h"

#include "xoapipr.h"

#include "define.h"
#include "state.h"
#include "cmnstruct.h"
#include "struct.h"

#include "sensor.h"
#include "wmbus.h"

#include "cproto.h"
#include "extern.h"
#include "manufacturer.h"
#include "preprocessor.h"
#include "dongle.h"

extern  void *MainIQ;
extern void *MdlCfg;
extern char *rootactPath;
extern  int Operating;

#define MAX_WMBUS_DEVICE    100
static t_sensor *TbSensor[MAX_WMBUS_DEVICE];
static int NbSensor = 0;

#define CHECK_IS_OBIX(dst) \
  if (!dst) return; \
  if (XoIsObix(dst) <= 0) {RTL_TRDBG(TRACE_ERROR,"ERROR not an obix object\n");return;}

#define DIA_KO()  (DiaIpuOk == 0 || DiaNetOk == 0)

int CanCreateCnt(int cluster, int attr, int member);

void DiaAppCreateAllCnt(t_sensor *cp);
void DiaDevStartAll(void);
void SensorSetDeviceListInObj(void *dst);
void DiaSensorSetSearchStringInObj(t_cmn_sensor *cmn, void *dst);
void DiaSensorSetApplicationListInObj(t_cmn_sensor *cmn, void *dst);
void DiaSensorSetInterfaceListInObj(t_cmn_sensor *cmn, int app, void *dst);

void iCmnSensorSetDefinedInterfaceListInObj(t_cmn_sensor *cmn, int app, void *dst,
    char *name, void *mdlitf);


/*
 *  Utils
 *  ------------------------------------------------------------------------------------------------
 */

/*
 *  wMBus IEEE format : Manufacturer (0000) Version (00) SN (00000000)
 *
 */
static  inline
void
AddrToIeee(unsigned long long int addr, char *dst)
{
  snprintf(dst, 32, "%014llx", addr);
}

static
t_sensor *
SensorNumSerial(int num, int serial)
{
  if  (num < 0 || num >= MAX_SSR_PER_RTR || TbSensor[num] == NULL)
  {
    RTL_TRDBG(TRACE_ERROR, "ERROR bad num sensor %d\n", num);
    return  NULL;
  }

  if  (TbSensor[num]->cp_serial != serial)
  {
    RTL_TRDBG(TRACE_ERROR, "ERROR bad serial sensor %d\n", serial);
    return  NULL;
  }

  return  TbSensor[num];
}

static
t_sensor *
SensorFindByIeee(char *ieee)
{
  unsigned long long int addr;
  int i;

  sscanf(ieee, "%llx", &addr);

  for (i = 0; i < MAX_WMBUS_DEVICE; i++)
  {
    if  (TbSensor[i] && (TbSensor[i]->cp_addr == addr))
    {
      return  TbSensor[i];
    }
  }

  return  NULL;
}

t_cmn_sensor *
SensorFindByIeeeNumber(unsigned long long int ieee)
{
  int i;

  for (i = 0; i < MAX_WMBUS_DEVICE; i++)
  {
    if  (TbSensor[i] && (TbSensor[i]->cp_addr == ieee))
    {
      return &(TbSensor[i]->cp_cmn);
    }
  }

  return  NULL;
}

/*!
 *
 * @brief called by DIA server module to retrieve the common structure of a
 * sensor with its ieee address. Mainly used for retargeting purpose.
 * @param ieee sensor ieee address
 * @return :
 *  - the address of the common structure in the sensor found
 *  - NULL if bad ieee address
 *
 * used by diaserver.c
 */
t_cmn_sensor *
CmnSensorFindByIeee(char *ieee)
{
  t_sensor  *cp;

  cp = SensorFindByIeee(ieee);
  if (cp)
  {
    return  &cp->cp_cmn;
  }

  return  NULL;
}

/*
 * used by diadef.h
 */
t_cmn_sensor *
CmnSensorNumSerial(int num, int serial)
{
  t_sensor  *cp;

  cp = SensorNumSerial(num, serial);
  if (cp)
  {
    return  &cp->cp_cmn;
  }

  return  NULL;
}

int
CanReport(int cluster, int attr, int member, t_cmn_sensor *dev)
{
  int i, nb;
  time_t now;

  if  (DIA_KO())
  {
    RTL_TRDBG(TRACE_IMPL, "Report Data (vib = x%x) -> Don't report -> dia KO\n", cluster);
    DiaRepDrp++;
    return  0;
  }

  // Ensure the M2M Point is in the modeling file
  if (CanCreateCnt(cluster, attr, member) == 0)
  {
    RTL_TRDBG(TRACE_IMPL, "Report Data (vib = x%x) -> Don't report -> modeling\n", cluster);
    return 0;
  }

  // Ensure minInterval
  //now = time(NULL);
  rtl_timemono(&now);
  t_sensor *sensor = (t_sensor *)dev;
  nb = sensor->nb_reports;
  for (i = 0; i < nb; i++)
  {
    if (sensor->reports[i].vib == cluster)
    {
      if (sensor->reports[i].date < now)
      {
        // Set date to now + minInterval
        RTL_TRDBG(TRACE_IMPL, "Report Data (vib = x%x) -> report -> next at %llx\n", cluster, sensor->reports[i].date);
        sensor->reports[i].date = now + CovGetMinIntervalForAttr(cluster);
        return 1;
      }

      RTL_TRDBG(TRACE_IMPL, "Report Data (vib = x%x) -> Don't report -> date > now\n", cluster);
      return 0;
    }
  }

  // Ensure free space in the list
  if ((nb + 1) > WMBUS_MAX_ATTR_PER_DEVICE)
  {
    RTL_TRDBG(TRACE_IMPL, "Report Data (vib = x%x) -> report -> too much attr\n", cluster);
    return 1; // Report if we can't check cov
  }

  // Not found, add an entry
  sensor->reports[nb].vib = cluster;
  sensor->reports[nb].date = now + CovGetMinIntervalForAttr(cluster);
  sensor->nb_reports++;

  RTL_TRDBG(TRACE_IMPL, "Report Data (vib = x%x) -> report -> attr created\n", cluster);
  return 1;
}

int
CanCreateCnt(int cluster, int attr, int member)
{
  // If model is not loading, we report all
  if  (MdlCfg)
  {
    char m2mvar[256];
    char *m2mid;

    sprintf (m2mvar, "0x%04x.0x%04x.%d.m2m", cluster, attr, member);
    m2mid = GetVarNoWarn(m2mvar);
    if (!m2mid || !*m2mid)
    {
      RTL_TRDBG(TRACE_IMPL, "modeling no mapping for '%s' => drop report\n", m2mvar);
      return  0;
    }
  }

  return  1;
}

static
void
ModelInterface(void *mdlitf, void *obj)
{
  // in obj template delete attributs/commands/points which
  // are not defined in model config
  if  (MdlCfg)
  {
    MdlConfigureInterfaceTemplate(mdlitf, obj);
  }

  // clear possible status="cfg-mandatory" in interface
  // event if modeling is not active

  MdlUnsetStatusInterfaceTemplate(obj);
}

/*
 * DIA Utils
 * ----------------------------------------------------------------------------
 */
int
WDiaUCBPreLoadTemplate(char *reqname, t_dia_req *preq, char *target, t_cmn_sensor *cmn,
    void **xo, char *templatename)
{
  t_sensor *sensor = (t_sensor *)cmn;
  unsigned long long int tmp;
  char buffer[128];

  if (cmn != NULL)
  {
    SetVar("d_ieee", cmn->cmn_ieee);

    tmp = IEEE_TO_MID(sensor->cp_addr);
    SetVar("w_mid", wmbusManufacturerCodeToString(tmp));

    tmp = IEEE_TO_SN(sensor->cp_addr);
    snprintf(buffer, 128, "%llx", tmp);
    SetVar("w_id", buffer);

    tmp = IEEE_TO_VER(sensor->cp_addr);
    snprintf(buffer, 128, "%llx", tmp);
    SetVar("w_version", buffer);

    SetVar("w_type", "Other");  // FFS
  }


  if (0 == strcmp(reqname, "DiaDatContCreate"))
  {
    char cnum[128];
    sprintf(cnum, "%04x.0x%04x.%d.m2m", preq->rq_cluster, preq->rq_attribut, preq->rq_member);
    SetVar("c_num", cnum);
  }

  SetVar("a_num", "114"); // x72

//c_num


  return 0;
}

int
WDiaUCBPostLoadTemplate(char *reqname, t_dia_req *preq, char *target, t_cmn_sensor *cmn, void *xo)
{
  if (!strcmp("DiaNetInstCreate", reqname))
  {
    SensorSetDeviceListInObj(xo);
  }
  else if (!strcmp("DiaAppInstCreate", reqname))
  {
    DiaSensorSetInterfaceListInObj(cmn, preq->rq_app, xo);
  }
  else if (!strcmp("DiaAppElemCreate", reqname))
  {
    DiaSensorSetSearchStringInObj(cmn, xo);
  }
  else if (!strcmp("DiaDevInstCreate", reqname))
  {
    DiaSensorSetApplicationListInObj(cmn, xo);
  }

  return 0;
}

int
WDiaUCBRequestOk(char *reqname, t_dia_req *preq, t_cmn_sensor *cmn)
{
  /*
   *  M2M Application created, send next request
   */
  if (!strcmp("DiaIpuOk", reqname))
  {
    DiaIpuOk++;
//        DiaNetStart();
  }
  else if (!strcmp("DiaNetOk", reqname))
  {
    DiaNetOk++;
//        DiaDevStartAll();
//    } else
//    if (!strcmp("DiaDevOk", reqname)) {
//        DiaAppStart(cmn->cmn_num, cmn->cmn_serial, WMBUS_REPORT_APP_CODE);
//    } else
//    if (!strcmp("DiaAppOk", reqname)) {
//        DiaAppCreateAllCnt((t_sensor *)cmn);
//    } else {
//        RTL_TRDBG(TRACE_IMPL,"Event '%s' not handled in %s\n", reqname, __func__);
  }

  return 0;
}

int
WDiaUCBRequestTimeout(char *reqname, t_dia_req *preq, t_cmn_sensor *cmn)
{
  /*
   *  Retry
   */
  if (!strcmp("DiaIpuError", reqname))
  {
    DiaIpuStart();
  }
  else if (!strcmp("DiaNetError", reqname))
  {
    DiaNetStart();
  }
  else if (!strcmp("DiaDevError", reqname))
  {
    DiaDevStart(cmn->cmn_num, cmn->cmn_serial);
  }
  else if (!strcmp("DiaAppError", reqname))
  {
    DiaAppStart(cmn->cmn_num, cmn->cmn_serial, WMBUS_REPORT_APP_CODE);
  }
  else
  {
    RTL_TRDBG(TRACE_IMPL, "Event '%s' not handled\n", reqname, __func__);
  }

  return 0;
}

int
WDiaUCBRequestError(char *reqname, t_dia_req *preq, t_cmn_sensor *cmn)
{
  RTL_TRDBG(3, "WDiaUCBRequestTimeout for '%s/%s'\n", reqname, preq->rq_name);

  if  (strncmp(reqname, "DiaIpu", 6) == 0)
  {
    DiaIpuOk  = 0;
    DiaIpuTmt++;
    DiaIpuStart();
    return  0;
  }
  if  (strncmp(reqname, "DiaNet", 6) == 0)
  {
    DiaNetOk  = 0;
    DiaNetTmt++;
    DiaNetStart();
    return  0;
  }

  return 0;
}


/*!
 *
 * @brief called by DIA client module to set the 'search strings' list for a
 * device.
 * see diaapp.c/diadat.c
 * @param cmn a pointer to the common structure of the sensor
 * @param dst destination object (xo/xml)
 * @return void
 *
 */
void
DiaSensorSetSearchStringInObj(t_cmn_sensor *cmn, void *dst)
{
  // TODO
}

/*!
 *
 * @brief called by DIA client module to set the applications list for a device.
 * see diadev.c
 * @param cmn a pointer to the common structure of the sensor
 * @param dst destination object (xo/xml)
 * @return void
 *
 */
void
DiaSensorSetApplicationListInObj(t_cmn_sensor *cmn, void *dst)
{
  int application_code = WMBUS_REPORT_APP_CODE;
  char target[256];
  void *dev;
  t_sensor *cp = cmn->cmn_self;

  CHECK_IS_OBIX(dst);

  dev = XoNmNew("o:ref");
  if (!dev)
  {
    RTL_TRDBG(TRACE_ERROR, "ERROR cannot allocate 'o:ref'\n");
    return;
  }

  XoNmSetAttr(dev, "name$", "ref", 0);

  snprintf(target, 256, "%s/APP_%s.%d/containers/DESCRIPTOR/contentInstances/latest/content",
      SCL_ROOT_APP, cp->cp_ieee, application_code);
  XoNmSetAttr(dev, "href", target, 0);

  XoNmAddInAttr(dst, "[name=applications].[]", dev, 0, 0);
}


#define _addCntToCreate(sensor,i) do{ if(CanCreateCnt(i,0,0)) { sensor->profiles[sensor->nb_profiles] = i; sensor->nb_profiles++; } }while(0)
inline
void
addCntToCreate(t_sensor *sensor, unsigned int id)
{
  RTL_TRDBG(TRACE_INFO, "Populate cnt to create for sensor %llx and interface %u\n",
      sensor->cp_addr, id);

  switch (id)
  {
    case 0x0003:
      _addCntToCreate(sensor, 0x03);
      _addCntToCreate(sensor, 0x2b);
      _addCntToCreate(sensor, 0xfd49);
      _addCntToCreate(sensor, 0xfd5c);
      break;

    default:
      ;
  }

  RTL_TRDBG(TRACE_INFO, "Done\n");
}

/*!
 *
 * @brief called by DIA client module to set the interfaces list for a
 * application.
 * see diaapp.c
 * @param cmn a pointer to the common structure of the sensor
 * @param app application number [1..N]
 * @param dst destination object (xo/xml)
 * @return void
 *
 */
void
DiaSensorSetInterfaceListInObj(t_cmn_sensor *cmn, int app, void *dst)
{
  int parse;
  void *o, *elem, *elem2;
  char  sfile[1024];
  char  profile[64];
  char  *profileID;
  char *tmp;
  unsigned long long int min, max, ieee;
  int i, j, nb, nb2, id;
  void *mdlitf;

  t_sensor *sensor = (t_sensor *)cmn;

  ieee = sensor->cp_addr;

  if  (!Operating)
  {
    snprintf (sfile, 1024, "%s%s/bdd.xml", rootactPath, GetAdaptorName());
  }
  else
  {
    snprintf(sfile, 1024, "%s/etc/%s/bdd.xml", rootactPath, GetAdaptorName());
  }

  if (access(sfile, R_OK) != 0)
  {
    RTL_TRDBG(TRACE_ERROR, "bdd.xml does not exist '%s'\n", sfile);
    return;
  }

  o = XoReadXmlEx(sfile, NULL, 0, &parse);
  if (!o)
  {
    RTL_TRDBG(TRACE_ERROR, "ERROR ret=%d cannot read xml '%s'\n", parse, sfile);
    return;
  }


  // Foreach
  nb  = XoNmNbInAttr(o, "wmbus:devices", 0);
  if (nb < 0)
  {
    RTL_TRDBG(TRACE_INFO, "Can't count device in bdd.xml, err = %d\n", nb);
    goto out;
  }

  RTL_TRDBG(TRACE_INFO, "BDD.xml READ OK, %d devices.\n", nb);

  for (i = 0; i < nb; i++)
  {
    elem  = XoNmGetAttr(o, "wmbus:devices[%d]", 0, i);
    if  (!elem)
    {
      continue;
    }

    tmp = XoNmGetAttr(elem, "wmbus:min", 0);
    sscanf(tmp, "%llx", &min);

    tmp = XoNmGetAttr(elem, "wmbus:max", 0);
    sscanf(tmp, "%llx", &max);

    RTL_TRDBG(TRACE_INFO, "iCmnSensorSetInterfaceListInObj =>>> min = %llx, max = %llx, "
        "ieee = %llx \n", min, max, ieee);


    if ((ieee >= min) && (ieee <= max))
    {

      nb2 = XoNmNbInAttr(elem, "wmbus:profiles", 0);
      for (j = 0; j < nb2; j++)
      {
        elem2 = XoNmGetAttr(elem, "wmbus:profiles[%d]", 0, j);

        profileID = XoNmGetAttr(elem2, "wmbus:id", 0);
        sscanf(profileID, "%x", &id);

        /*
         *  Search about a <interface filter="wmbus:/server/3"> in the modeling file
         *  If not found, the customer don't want to report this interface
         */
        if (MdlCfg)
        {
          sprintf(profile, "0x%.4x", id);
          mdlitf = MdlGetInterface(MdlCfg, "wmbus", "server", profile);
          if (!mdlitf)
          {
            // Go to next interface
            continue;
          }
        }

        /*
         *  Register all cnt for the interface (id)
         *  Only need will be created later by using CanReport function
         */
        addCntToCreate(sensor, id);

        sprintf(profile, "%.4x", id);
        iCmnSensorSetDefinedInterfaceListInObj(cmn, app, dst, profile, mdlitf);
      }

      XoFree(o, 1);
      return;
    }
  }

out:
  XoFree(o, 1);
}


/*!
 *
 * @brief called by DIA client module to set the devices (sensors) list for a
 * network.
 * see dianet.c
 * @param dst destination object (xo/xml)
 * @return void
 *
 */
void
SensorSetDeviceListInObj(void *dst)
{
  int   i;
  t_sensor  *cp;
  char    target[256];
  void    *dev;

  CHECK_IS_OBIX(dst);

  for (i = 0; i < MAX_WMBUS_DEVICE; i++)
  {
    cp  = TbSensor[i];
    if  (!cp)
    {
      continue;
    }

    dev = XoNmNew("o:ref");
    if (!dev)
    {
      RTL_TRDBG(TRACE_ERROR, "ERROR cannot allocate 'o:ref'\n");
      return;
    }

    XoNmSetAttr(dev, "name$", "ref", 0);
    snprintf(target, 256, "%s/DEV_%s/containers/DESCRIPTOR/contentInstances/latest/content", SCL_ROOT_APP, cp->cp_ieee);
    XoNmSetAttr(dev, "href", target, 0);
    XoNmAddInAttr(dst, "[name=nodes].[]", dev, 0, 0);
  }
}


void
iCmnSensorSetDefinedInterfaceListInObj(t_cmn_sensor *cmn, int app, void *dst, char *name,
    void *mdlitf)
{
  int     parseflags = XOML_PARSE_OBIX;
  char    path[1024];
  void    *obj;

  SetVar("d_ieee", cmn->cmn_ieee);

  snprintf(path, 1024, "%d", app);
  SetVar("a_num", path);

  snprintf (path, 1024, "itf_inst_%s_server.xml", name);
  obj = WXoNewTemplate(path, parseflags);
  if (!obj)
  {
    RTL_TRDBG(TRACE_ERROR, "cannot parse '%s'\n", path);
    return;
  }

  ModelInterface(mdlitf, obj);

  XoNmAddInAttr(dst, "[name=interfaces].[]", obj, 0, 0);
}


/*
 *  Device tools
 *  -----------------------------------------------------------------------------------------------------
 */

/*
 *  Init wmbus specific
 */
static
int
iSensorLoadContext(t_sensor *cp)
{
  void *o;
  int ret;
  int parse;
  char *pt;
  char sfile[1024];

  if  (!Operating)
  {
    snprintf (sfile, 1024, "%s/knownsensors/%014llx.xml", getenv(AW_DATA_DIR), cp->cp_addr);
  }
  else
  {
    snprintf(sfile, 1024, "%s/usr/data/%s/knownsensors/%014llx.xml", rootactPath, GetAdaptorName(), cp->cp_addr);
  }

  if (access(sfile, R_OK) != 0)
  {
    RTL_TRDBG(TRACE_INFO, "context does not exist '%s'\n", sfile);
    return  -1;
  }

  o = XoReadXmlEx(sfile, NULL, 0, &parse);
  if (!o)
  {
    RTL_TRDBG(TRACE_ERROR, "ERROR ret=%d cannot read xml '%s'\n", parse, sfile);
    return  -2;
  }

  RTL_TRDBG(TRACE_INFO, "cp=%p '%014llx' context retrieve\n", cp, cp->cp_addr);

  pt = XoNmGetAttr(o, "wmbus:nicename", &ret);
  if (pt)
  {
    cp->cp_nicename = strdup(pt);
  }

  cp->cp_ctxtLoaded = 1;

  XoFree(o, 1);
  return  1;
}

/*
 *  Init common members
 */
static
t_sensor *
WmbusSensorInit(t_sensor *cp, int num, long long int addr)
{
  cp->cp_self = (void *)cp;
  cp->cp_num = num;
  cp->cp_serial = DoSerial();   // Generate a very simple uuid for the object
  cp->cp_addr = addr;
  cp->nb_reports = 0;
  cp->nb_profiles = 0;

  AddrToIeee(cp->cp_addr, cp->cp_ieee);
  iSensorLoadContext(cp);

  return  cp;
}

/*
 *  Alloc a t_sensor and init it
 */
static  t_sensor *WmbusSensorNew(int num, long long int addr)
{
  t_sensor  *cp;

  cp = (t_sensor *) malloc(sizeof(t_sensor));
  if (!cp)
  {
    RTL_TRDBG(TRACE_ERROR, "malloc failure\n");
    return  NULL;
  }
  memset (cp, 0, sizeof(t_sensor));

  WmbusSensorInit(cp, num, addr);
  RTL_TRDBG(TRACE_IMPL, "New sensor cp=%p, addr='%014llx', nicename='%s'\n", cp,
      cp->cp_addr, cp->cp_nicename);

  return  cp;
}

/*
 *  Restore a device from xml context
 */
static  void DoSensorCreate(long long int tocreate)
{
  int   i;
  int   room = -1;
  t_sensor  *cp;

  // really does not exist ?
  for (i = 0; i < MAX_WMBUS_DEVICE; i++)
  {
    if  (TbSensor[i] && (TbSensor[i]->cp_addr == tocreate))
    {
      RTL_TRDBG(TRACE_API, "sensor '%014llx' already exists at room=%d\n", tocreate, i);
      return;
    }
  }

  // find a room
  for (i = 0 ; i < MAX_SSR_PER_RTR; i++)
  {
    if  (TbSensor[i] == NULL)
    {
      room = i;
      break;
    }
  }
  if (room < 0)
  {
    RTL_TRDBG(TRACE_ERROR, "max sensors reached (%d) !!!\n", NbSensor);
    return;
  }

  cp = WmbusSensorNew(room, tocreate);
  if  (cp == NULL)
  {
    return;
  }

  TbSensor[room] = cp;
  NbSensor++;

  RTL_TRDBG(TRACE_API, "create '%014llx' %x room=%d (%d)\n", tocreate, cp, room, NbSensor);
}

/*
 *  Restore the driver context
 *  Call DoSensorCreate for each device saved
 */
void
SensorCreateKnown(void)
{
  char  path[1024];
  void  *dir;
  char  *name;
  unsigned long long int addr;
  char  *point;

  if  (!Operating)
  {
    snprintf (path, 1024, "%s/knownsensors", getenv(AW_DATA_DIR));
  }
  else
  {
    snprintf(path, 1024, "%s/usr/data/%s/knownsensors", rootactPath, GetAdaptorName());
  }
  RTL_TRDBG(TRACE_API, "Loading know sensors from : %s\n", path);

  dir = rtl_openDir(path);
  if  (!dir)
  {
    return;
  }

  while ((name = rtl_readDir(dir)) && *name)
  {
    if (*name == '.' && *(name + 1) == '.')
    {
      continue;
    }

    point = rtl_suffixname(name);
    if (point && *point && strcmp(point, ".xml") == 0)
    {
      point = strchr(name, '.');
      if  (point)
      {
        *point  = '\0';
      }

      sscanf(name, "%llx", &addr);
      DoSensorCreate(addr);
    }
  }

  rtl_closeDir(dir);
}

/*
 *  Ensure that all device is created into the GSC
 */
void DiaDevStartAll(void)
{
  int i;
  t_sensor *cp;

  for (i = 0; i < MAX_WMBUS_DEVICE; i++)
  {
    cp  = TbSensor[i];
    if  (!cp)
    {
      continue;
    }

    DiaDevStart(i, cp->cp_serial);
  }
}

void DiaAppCreateAllCnt(t_sensor *cp)
{
  int i, nb;

  nb = cp->nb_profiles;
  RTL_TRDBG(TRACE_INFO, "Create cnt for %llx, %d to create\n", cp->cp_addr, nb);

  for (i = 0; i < nb; i++)
  {
    RTL_TRDBG(TRACE_INFO, "-> x%04x\n", cp->profiles[i]);
    // TESTER LA variable (spvconfig/mapping) pour savoir si il faut veraiment créer le ctn

    DiaDatStart(cp->cp_num, cp->cp_serial, WMBUS_REPORT_APP_CODE, cp->profiles[i], 0, 0);
  }
}

void
DoSensorDetected(long long int tocreate)
{
  int i;
  t_sensor *cp;

  // FFS : Write XML context ?
  DoSensorCreate(tocreate);

  // Register it into GSC
  for (i = 0; i < MAX_WMBUS_DEVICE; i++)
  {
    cp  = TbSensor[i];
    if  (!cp)
    {
      continue;
    }

    if (cp->cp_addr != tocreate)
    {
      continue;
    }

    DiaDevStart(i, cp->cp_serial);
    break;
  }
  DiaNetStart();
}











/*
 *  Events
 *  -----------------------------------------------------------------------------------------------------
 */

/*!
 *
 * @brief clock for sensors, must be called by main loop each second
 * @param now current time in second (time(2))
 * @return void
 *
 */
void
WmbusSensorClockSc(time_t now)
{
  int i;
  t_sensor *cp;

  if (0 != (now % 3))
  {
    return;
  }
  if (DiaDisconnected())
  {
    return;
  }

  if (! wmbusDongleReady())
  {
    return;
  }

  if (0 == DiaIpuOk)
  {
    DiaIpuStart();
    return;
  }

  if (0 == DiaNetOk)
  {
    DiaNetStart();
    return;
  }

  for (i = 0; i < MAX_WMBUS_DEVICE; i++)
  {
    cp = TbSensor[i];
    if (!cp)
    {
      continue;
    }

    // Either create the ACRs associated to the device applications
    if (0 == cp->cp_cmn.cmn_dia_accok)
    {
      DiaAccStart(cp->cp_cmn.cmn_num, cp->cp_cmn.cmn_serial, WMBUS_REPORT_APP_CODE, 1);
      DiaAccStart(cp->cp_cmn.cmn_num, cp->cp_cmn.cmn_serial, WMBUS_REPORT_APP_CODE, 2);
      DiaAccStart(cp->cp_cmn.cmn_num, cp->cp_cmn.cmn_serial, WMBUS_REPORT_APP_CODE, 3);
      return;
    }
    // Either create the device applications and associated containers
    else if (0 == cp->cp_cmn.cmn_dia_appok)
    {
      DiaAppStart(cp->cp_cmn.cmn_num, cp->cp_cmn.cmn_serial, WMBUS_REPORT_APP_CODE);
      return;
    }
    else if (0 == cp->cp_cmn.cmn_dia_datok)
    {
      DiaAppCreateAllCnt(cp);
      return;
    }
    // Either create the device applications and associated containers
    else if (0 == cp->cp_cmn.cmn_dia_devok)
    {
      DiaDevStart(cp->cp_cmn.cmn_num, cp->cp_cmn.cmn_serial);
      DiaNetStart(); // update the node list
      return;
    }
  }
}

void
WmbusSensorEvent(t_imsg *imsg, void *data)
{

}



/*
 *  Dia Server Callback
 *  -----------------------------------------------------------------------------------------------------
 */


/*
 * used by diaserver.c
 */
int
CmnNetworkExecuteMethod(char *ident, int targetlevel, void *obix, char *targetid)
{
  return 0;
}

/*!
 *
 * @brief called by DIA server module to execute a method on a device/appli
 * @param cmn a pointer to the common structure of the sensor
 * @param app application number
 * @param cluster cluster number
 * @param numm method number
 * @return void
 *
 * used by diaserver.c
 */
int
iCmnSensorExecuteMethod(t_cmn_sensor *cmn, int app, int cluster, int numm, int targetlevel, void *obix, char *targetid)
{
  int ret;
  void *libHandler;
  fn_cmd fnHandler;
  char load_path[1024];
  char lib[256];
  char fnct[256];
  char out[256];

  // Generate data
  ret = wmbusManufacturerCmdFindHandler(((t_sensor *)cmn)->cp_addr, lib, fnct);
  if (ret < 0)
  {
    RTL_TRDBG(TRACE_IMPL, "Can't find cmd handler\n");
    return -1;
  }

  if  (!Operating)
  {
    snprintf(load_path, 1024, "%s/%s/lib/preprocessor/%s.so",
        getenv("ROOTACT"),
        GetAdaptorName(),
        lib);
  }
  else
  {
    snprintf(load_path, 1024, "%s/etc/%s/preprocessor/%s.so", rootactPath, GetAdaptorName(), lib);
  }

  libHandler = dlopen(load_path, RTLD_LAZY | RTLD_LOCAL);
  if (libHandler == NULL)
  {
    RTL_TRDBG(TRACE_IMPL, "Can't load shared library : '%s'\n", load_path);
    return -1;
  }

  fnHandler = dlsym(libHandler, fnct);
  if (fnHandler == NULL)
  {
    RTL_TRDBG(TRACE_IMPL, "Can't load function '%s' from library '%s'\n", fnct, lib);
    return -1;
  }

  ret = fnHandler(out, 256, app, cluster, numm, ((t_sensor *)cmn)->cp_addr);
  if (ret < 0)
  {
    RTL_TRDBG(TRACE_IMPL, "Cmd failed, error = %d\n", ret);
    return -1;
  }
  dlclose(libHandler);

  // Send output buffer on the dongle
  wmbusDongleWrite (out, ret);

  return  0;
}

int
iCmnSensorRetrieveAttrValue(t_cmn_sensor *cmn, int app, int cluster, int numm, int targetlevel, char *targetid, int tid)
{
  return 0;
}

int
iCmnSensorUpdateAttrValue(t_cmn_sensor *cmn, int app, int cluster, int numm, int targetlevel, void *obix, char *targetid, int tid)
{
  return 0;
}
