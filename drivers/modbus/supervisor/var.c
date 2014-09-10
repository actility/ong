#include "modbus.h"

int NbVar;
int InternalVar;

char *
GetAdaptorVersion(void)
{
  return modbus_whatStr;
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
  sprintf(val,"[%s]:%s",GetCoapAddrR(),GetCoapPortR());
  InternalVar = 0;
  return  val;
}

char *
GetDrvContact(void)
{
  static char val[128];

  InternalVar = 1;
  sprintf(val,"[%s]:%s",GetCoapAddrL(),GetCoapPortL());
  InternalVar = 0;
  return  val;
}

char *
GetHostName(void)
{
  static char val[128];

  InternalVar = 1;
  sprintf(val,"%s.%s",GetBoxName(),GetDomainName());
  InternalVar = 0;
  return  val;
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

  // COAP
  VAR_INIT(w_coapaddr_l, "::1"),
  VAR_INIT(w_coapport_l, "5689"),
  VAR_INIT(w_coapaddr_r, "::1"),
  VAR_INIT(w_coapport_r, "5683"),
  
  // Telnet
  VAR_INIT(s_telnetkey, "123"),
  VAR_INIT(s_telnetport, "2005"),
  
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
  VAR_INIT(w_boxname, "no-ong-name"),
  VAR_INIT(w_domainname, "actility.com"),
  VAR_INIT(w_driverid, "GIP"),
  VAR_INIT(w_nsclbaseuri, "http://nsc1.actility.com:8088/m2m"),
  VAR_INIT(w_scl_httpport, "8080"),
  VAR_INIT_EMPTY(w_tpkdev),
  
  // Modebus config
  VAR_INIT(loggingDefaultCov, "PT1H"),
  
  // Modbus template
  VAR_INIT(w_driverpath, "modbus"),
  VAR_INIT(w_ipuid, SCL_IPU "MODBUS"),
  VAR_INIT(w_netid, "Dummy"),
  VAR_INIT_EMPTY(w_nettype),
  VAR_INIT_EMPTY(w_netuart),
  VAR_INIT_EMPTY(modbus_dev_manufacturer_name),
  VAR_INIT_EMPTY(modbus_dev_name),
  VAR_INIT_EMPTY(modbus_dev_version),
  VAR_INIT_EMPTY(modbus_dev_ref),
  VAR_INIT(a_num, "0"),
};



