
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>
#include <string.h>
#include <curl/curl.h>
#include <errno.h>
#include <poll.h>
#include <microhttpd.h>

#include "rtlbase.h"

#include "admin_cli.h"
#include "song_over_http_api.h"
#include "cltSongImpl.h"

static char *getStatusStr(SOH_STATUS_CODE statusCode)
{
  switch (statusCode)
  {
  case SOH_SC_OK :
    return "STATUS_OK";
  case SOH_SC_CREATED :
    return "STATUS_CREATED";
  case SOH_SC_ACCEPTED :
    return "STATUS_ACCEPTED";
  case SOH_SC_NO_CONTENT :
    return "STATUS_NO_CONTENT";

  case SOH_SC_BAD_REQUEST :
    return "STATUS_BAD_REQUEST";
  case SOH_SC_UNAUTHORIZED :
    return "STATUS_UNAUTHORIZED";
  case SOH_SC_FORBIDDEN :
    return "STATUS_FORBIDDEN";
  case SOH_SC_NOT_FOUND :
    return "STATUS_NOT_FOUND";
  case SOH_SC_METHOD_NOT_ALLOWED :
    return "STATUS_METHOD_NOT_ALLOWED";
  case SOH_SC_CONFLICT :
    return "STATUS_CONFLICT";

  case SOH_SC_INTERNAL_SERVER_ERROR :
    return "STATUS_INTERNAL_SERVER_ERROR";
  case SOH_SC_NOT_IMPLEMENTED :
    return "STATUS_NOT_IMPLEMENTED";
  case SOH_SC_SERVICE_UNAVAILABLE :
    return "STATUS_SERVICE_UNAVAILABLE";
  case SOH_SC_GATEWAY_TIMEOUT : 
    return "STATUS_GATEWAY_TIMEOUT";

  default:
    return "<unknown>";
  }
}

static void printToCli(t_cli *cl, SOH_STATUS_CODE statusCode,
  char *resourceURI, bool rscUriExpected, unsigned char *content, size_t len, 
  char *contentType, sohHeader_t **optHeaders, char *tid)
{
  int i = 0;

  AdmPrint(cl, "\n>>> %s (tid:%s)\n", getStatusStr(statusCode), tid);
  if (rscUriExpected)
  {
    AdmPrint(cl, "resourceURI: %s\n", resourceURI);
  }
  if (contentType)
  {
    AdmPrint(cl, "Content-Type: %s\n", contentType);
  }
  if (optHeaders)
  {
    while(optHeaders[i])
    { 
      AdmPrint(cl, "%s: %s\n", optHeaders[i]->name, optHeaders[i]->values);
      i++;
    }
  }
  if (content)
  {
    AdmPrint(cl, "%s\n", content);
  }
}


void testerCreateResponseCb (SOH_STATUS_CODE statusCode,
  char *resourceURI, unsigned char *content, size_t len, char *contentType,
  sohHeader_t **optHeaders, char *tid, void *issuerData)
{
  printToCli((t_cli *)issuerData, statusCode, resourceURI, true, content, len, contentType,
    optHeaders, tid);
}

void testerRetrieveResponseCb (SOH_STATUS_CODE statusCode,
  unsigned char *content, size_t len, char *contentType,
  sohHeader_t **optHeaders, char *tid, void *issuerData)
{
  printToCli((t_cli *)issuerData, statusCode, NULL, false, content, len, contentType,
    optHeaders, tid);
}

void testerUpdateResponseCb (SOH_STATUS_CODE statusCode,
  unsigned char *content, size_t len, char *contentType,
  sohHeader_t **optHeaders, char *tid, void *issuerData)
{
  printToCli((t_cli *)issuerData, statusCode, NULL, false, content, len, contentType,
    optHeaders, tid);
}

void testerDeleteResponseCb (SOH_STATUS_CODE statusCode,
  unsigned char *content, size_t len, char *contentType,
  sohHeader_t **optHeaders, char *tid, void *issuerData)
{
  printToCli((t_cli *)issuerData, statusCode, NULL, false, content, len, contentType,
    optHeaders, tid);
}

