
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "rtlbase.h"
#include "xoapipr.h"

#include "song_over_http_api.h"
#include "http_subs_api.h"
#include "common.h"
#include "subscription_t.h"


char *xoSerializeAndFree(void *xo)
{
  void *xoWriter;
  char *subsXml;

  //xoWriter = XoWritXmlMem(xo, 0, &subsXml, "m2m");
  xoWriter = XoWritXmlMem(xo, 0, &subsXml, NULL);
  if  (!xoWriter)
  {
    RTL_TRDBG(TRACE_ERROR, "ERROR cannot serialize (XML) xo '%s'\n",
      XoNmType(xo));
    XoFree(xo, 1);
    return  NULL;
  }
  XoWritXmlFreeCtxt(xoWriter);

  XoFree(xo, 1);
  return subsXml; 
}

char *stateStr(SUBS_PENDING_STATE state)
{
  switch(state)
  {
    case SUBS_ST_INIT:
      return "INIT";
    case SUBS_ST_WAITING_SUBSCRIBE_RESP:
      return "WAITING_SUBSCRIBE_RESP";
    case SUBS_ST_WAITING_RESUBSCRIBE_RESP:
      return "WAITING_RESUBSCRIBE_RESP";
    case SUBS_ST_WAITING_UNSUBSCRIBE_RESP:
      return "WAITING_UNSUBSCRIBE_RESP";
    case SUBS_ST_ACTIVE:
      return "ACTIVE";
    case SUBS_ST_EXPIRED:
      return "EXPIRED";
  }
  return "<UNKNOWN>";
}

char *statusCodeStr(SOH_STATUS_CODE statusCode)
{
  switch (statusCode)
  {
  case SOH_SC_OK:
    return "STATUS_OK";
  case SOH_SC_CREATED:
    return "STATUS_CREATED";
  case SOH_SC_ACCEPTED:
    return "STATUS_ACCEPTED";
  case SOH_SC_NO_CONTENT:
    return "STATUS_NO_CONTENT";

  case SOH_SC_BAD_REQUEST:
    return "STATUS_BAD_REQUEST";
  case SOH_SC_UNAUTHORIZED:
    return "STATUS_UNAUTHORIZED";
  case SOH_SC_FORBIDDEN:
    return "STATUS_FORBIDDEN";
  case SOH_SC_NOT_FOUND:
    return "STATUS_NOT_FOUND";
  case SOH_SC_METHOD_NOT_ALLOWED:
    return "STATUS_METHOD_NOT_ALLOWED";
  case SOH_SC_CONFLICT:
    return "STATUS_CONFLICT";

  case SOH_SC_INTERNAL_SERVER_ERROR:
    return "STATUS_INTERNAL_SERVER_ERROR";
  case SOH_SC_NOT_IMPLEMENTED:
    return "STATUS_NOT_IMPLEMENTED";
  case SOH_SC_SERVICE_UNAVAILABLE:
    return "STATUS_SERVICE_UNAVAILABLE";
  case SOH_SC_GATEWAY_TIMEOUT:
    return "STATUS_GATEWAY_TIMEOUT";
  }
  return "STATUS_INTERNAL_SERVER_ERROR";
}

char *buildErrInfoContent(SOH_STATUS_CODE statusCode, char *addInfo)
{
  void *errInfo;

  errInfo = XoNmNew("m2m:errorInfo");
  XoNmSetAttr(errInfo, "m2m:statusCode", statusCodeStr(statusCode), 0, 0); 
  XoNmSetAttr(errInfo, "m2m:additionalInfo", addInfo, 0, 0);

  return xoSerializeAndFree(errInfo); 
}


