/*! @file var.c
 *
 */
#include <stdio.h>            /************************************/
#include <stdlib.h>            /*     run in main thread           */
#include <string.h>            /************************************/
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

#include "knx.h"

int NbVar;
int InternalVar;

char *
GetAdaptorVersion(void)
{
    return knx_whatStr;
}

char *
GetDate(void)
{
    static char val[128];

    InternalVar = 1;
    rtl_aaaammjj(0, val);
    InternalVar = 0;
    return val;
}

char *
GetTime(void)
{
    static char val[128];

    InternalVar = 1;
    rtl_hhmmss(0, val);
    InternalVar = 0;
    return val;
}

char *
GetAdaptorName(void)
{
    return ADAPTOR_NAME;
}

char *
GetGwContact(void)
{
    static char val[128];

    InternalVar = 1;
    sprintf(val,"%s:%s",AddBrackets(GetCoapAddrR()),GetCoapPortR());
    InternalVar = 0;
    return    val;
}

char *
GetDrvContact(void)
{
    static char val[128];

    InternalVar = 1;
    sprintf(val,"%s:%s",AddBrackets(GetCoapAddrL()),GetCoapPortL());
    InternalVar = 0;
    return    val;
}

char *
GetHostName(void)
{
    static char val[128];

    InternalVar = 1;
    sprintf(val,"%s.%s",GetBoxName(),GetDomainName());
    InternalVar = 0;
    return    val;
}

char *
GetHostTarget(void)
{
  static  char  val[128];

  InternalVar = 1;
  sprintf (val, "coap://%s:%s", AddBrackets(GetCoapAddrR()), GetCoapPortR());
  InternalVar = 0;
  return  val;
}

char *
GetReqId(void)
{
    static char val[256];

    InternalVar = 1;
    sprintf (val, "coap://%s:%s%s/%s", GetHostName(), GetCoapPortR(), SCL_ROOT_APP, GetDriverId());
    InternalVar = 0;
    return val;
}

char *
GetPlcTarget(void)
{
    // PLC Engine not used
    return NULL;
}

t_var TbVar[MAX_VAR] = {

    // Variable block use by knx2obix parser
    VAR_INIT_EMPTY(w_template),
    VAR_INIT_EMPTY(raw_a),
    VAR_INIT_EMPTY(raw_Ack_Sup),
    VAR_INIT_EMPTY(raw_AlarmUnAck),
    VAR_INIT_EMPTY(raw_AlarmPriority),
    VAR_INIT_EMPTY(raw_AlarmText_Sup),
    VAR_INIT_EMPTY(raw_AlarmUnAck_Sup),
    VAR_INIT_EMPTY(raw_ApplicationArea),
    VAR_INIT_EMPTY(raw_b),
    VAR_INIT_EMPTY(raw_c),
    VAR_INIT_EMPTY(raw_character),
    VAR_INIT_EMPTY(raw_CLQ),
    VAR_INIT_EMPTY(raw_d),
    VAR_INIT_EMPTY(raw_Day),
    VAR_INIT_EMPTY(raw_DayOfMonth),
    VAR_INIT_EMPTY(raw_DayOfWeek),
    VAR_INIT_EMPTY(raw_DeltaTime),
    VAR_INIT_EMPTY(raw_e),
    VAR_INIT_EMPTY(raw_ErrorCode_Sup),
    VAR_INIT_EMPTY(raw_ErrorClass),
    VAR_INIT_EMPTY(raw_field1),
    VAR_INIT_EMPTY(raw_F),
    VAR_INIT_EMPTY(raw_Fault),
    VAR_INIT_EMPTY(raw_FloatValue),
    VAR_INIT_EMPTY(raw_Hour),
    VAR_INIT_EMPTY(raw_HourOfDay),
    VAR_INIT_EMPTY(raw_incrementedCode),
    VAR_INIT_EMPTY(raw_InAlarm),
    VAR_INIT_EMPTY(raw_InAlarm_Sup),
    VAR_INIT_EMPTY(raw_m0),
    VAR_INIT_EMPTY(raw_m1),
    VAR_INIT_EMPTY(raw_m2),
    VAR_INIT_EMPTY(raw_m3),
    VAR_INIT_EMPTY(raw_m4),
    VAR_INIT_EMPTY(raw_m5),
    VAR_INIT_EMPTY(raw_m6),
    VAR_INIT_EMPTY(raw_m7),
    VAR_INIT_EMPTY(raw_m8),
    VAR_INIT_EMPTY(raw_m9),
    VAR_INIT_EMPTY(raw_m10),
    VAR_INIT_EMPTY(raw_m11),
    VAR_INIT_EMPTY(raw_m12),
    VAR_INIT_EMPTY(raw_m13),
    VAR_INIT_EMPTY(raw_m14),
    VAR_INIT_EMPTY(raw_m15),
    VAR_INIT_EMPTY(raw_magicNumber),
    VAR_INIT_EMPTY(raw_manufacturerCode),
    VAR_INIT_EMPTY(raw_Minutes),
    VAR_INIT_EMPTY(raw_mode),
    VAR_INIT_EMPTY(raw_Month),
    VAR_INIT_EMPTY(raw_NT),
    VAR_INIT_EMPTY(raw_ND),
    VAR_INIT_EMPTY(raw_NDOW),
    VAR_INIT_EMPTY(raw_NWD),
    VAR_INIT_EMPTY(raw_NY),
    VAR_INIT_EMPTY(raw_OutOfService),
    VAR_INIT_EMPTY(raw_Overridden),
    VAR_INIT_EMPTY(raw_OwnIA),
    VAR_INIT_EMPTY(raw_Locked),
    VAR_INIT_EMPTY(raw_Locked_Sup),
    VAR_INIT_EMPTY(raw_LogNumber),
    VAR_INIT_EMPTY(raw_v),
    VAR_INIT_EMPTY(raw_UnsignedValue),
    VAR_INIT_EMPTY(raw_UserStopped),
    VAR_INIT_EMPTY(raw_revisionNumber),
    VAR_INIT_EMPTY(raw_s),
    VAR_INIT_EMPTY(raw_s0),
    VAR_INIT_EMPTY(raw_s1),
    VAR_INIT_EMPTY(raw_s2),
    VAR_INIT_EMPTY(raw_s3),
    VAR_INIT_EMPTY(raw_s4),
    VAR_INIT_EMPTY(raw_s5),
    VAR_INIT_EMPTY(raw_s6),
    VAR_INIT_EMPTY(raw_s7),
    VAR_INIT_EMPTY(raw_s8),
    VAR_INIT_EMPTY(raw_s9),
    VAR_INIT_EMPTY(raw_s10),
    VAR_INIT_EMPTY(raw_s11),
    VAR_INIT_EMPTY(raw_s12),
    VAR_INIT_EMPTY(raw_s13),
    VAR_INIT_EMPTY(raw_s14),
    VAR_INIT_EMPTY(raw_s15),
    VAR_INIT_EMPTY(raw_SceneNumber),
    VAR_INIT_EMPTY(raw_Seconds),
    VAR_INIT_EMPTY(raw_StepCode),
    VAR_INIT_EMPTY(raw_SignedValue),
    VAR_INIT_EMPTY(raw_RelSignedValue),
    VAR_INIT_EMPTY(raw_TimePeriod),
    VAR_INIT_EMPTY(raw_TS_Sup),
    VAR_INIT_EMPTY(raw_VerifyMode),
    VAR_INIT_EMPTY(raw_versionNumber),
    VAR_INIT_EMPTY(raw_Year),
    VAR_INIT_EMPTY(raw_WD),
    VAR_INIT_EMPTY(raw_SUTI),
    VAR_INIT_EMPTY(raw_CountVal),
    VAR_INIT_EMPTY(raw_ValInfField),
    VAR_INIT_EMPTY(dpt_raw),
    VAR_INIT_EMPTY(dpt_value),
    VAR_INIT_EMPTY(dpt_unit),

    // COAP
    VAR_INIT(w_coapaddr_l, "::1"),
    VAR_INIT(w_coapport_l, "5687"),
    VAR_INIT(w_coapaddr_r, "::1"),
    VAR_INIT(w_coapport_r, "5683"),
    
    // Telnet
    VAR_INIT(s_telnetkey, "123"),
    VAR_INIT(s_telnetport, "2003"),
    
    // Dia Tools
    VAR_FUNCTION(w_hostname, GetHostName),
    VAR_FUNCTION(w_reqid, GetReqId),
    VAR_FUNCTION(w_gwcontact, GetGwContact),
    VAR_FUNCTION(w_date, GetDate),
    VAR_FUNCTION(w_time, GetTime),
    VAR_FUNCTION(w_drvcontact, GetDrvContact),

    // Dia common
    VAR_INIT_EMPTY(d_ieee),
    VAR_INIT_EMPTY(a_num),
    VAR_INIT_EMPTY(w_template),
    VAR_INIT_EMPTY(c_num),
    VAR_INIT_EMPTY(r_value),
    
    // DIA Constants
    VAR_INIT(s_rootapp, SCL_ROOT_APP),
    VAR_INIT(s_rootacc, SCL_ROOT_ACC),
    VAR_INIT(s_adaptor, ADAPTOR_NAME),
    VAR_INIT(w_hosttarget, "coap://localhost"),
    VAR_INIT_EMPTY(w_boxname),
    VAR_INIT(w_domainname, "actility.com"),
    VAR_INIT(w_driverid, "GIP"),
    VAR_INIT(w_nsclbaseuri, "http://nsc1.actility.com:8088/m2m"),
    VAR_INIT(w_scl_httpport, "8080"),
    VAR_INIT_EMPTY(w_tpkdev),

    // KNX template
    VAR_INIT(w_ipuid, SCL_IPU "KNX"),
    VAR_INIT(w_netid, "NW_FAKE"),
    VAR_INIT(w_driverpath, "knx"),
    VAR_INIT(interfaceID, "FFS"),
    VAR_INIT(dpt_readable, "true"),
    VAR_INIT(dpt_writable, "true"),
    VAR_INIT_EMPTY(knx_dev_addr),
    VAR_INIT_EMPTY(knx_dev_area),
    VAR_INIT_EMPTY(knx_dev_area_id),
    VAR_INIT_EMPTY(knx_dev_line),
    VAR_INIT_EMPTY(knx_dev_line_id),
    VAR_INIT_EMPTY(knx_dev_manufacturer),
    VAR_INIT_EMPTY(knx_dev_manufacturer_name),
    VAR_INIT_EMPTY(knx_dev_fw),
    VAR_INIT_EMPTY(knx_dev_name),
    VAR_INIT_EMPTY(knx_dev_description),
    VAR_INIT_EMPTY(dpt_name),
    VAR_INIT_EMPTY(dpt_id),
    VAR_INIT_EMPTY(dpt_primary_id),
    VAR_INIT_EMPTY(dpt_sub_id),
    VAR_INIT_EMPTY(knxAddress),
    VAR_INIT_EMPTY(knxGroupAddress),
    VAR_INIT_EMPTY(knxObjectID),
    VAR_INIT_EMPTY(knxObjectName),
    VAR_INIT_EMPTY(knxObjectDescription),

    // EIBD options
    VAR_INIT_EMPTY(eibdoption1),
    VAR_INIT_EMPTY(eibdoption2),
    VAR_INIT_EMPTY(eibdoption3),
};



