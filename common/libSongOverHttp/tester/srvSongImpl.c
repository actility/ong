
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <curl/curl.h>
#include <errno.h>
#include <poll.h>
#include <microhttpd.h>

#include "rtlbase.h"

#include "song_over_http_api.h"
#include "srvSongImpl.h"

const char erorMsgTemplate[] =
  "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\"?>"
  "<m2m:errorInfo xmlns:m2m=\"http://uri.etsi.org/m2m\">"
  "<m2m:statusCode>%s</m2m:statusCode>"
  "<m2m:additionalInfo>%s</m2m:additionalInfo>"
  "</m2m:errorInfo>";
#define MAX_BUF_SIZE_B 255
#define MAX_BUF_SIZE_H 1023


SOH_RCODE testerSohCreateCb (char *reqEntity, char *targetID,
  unsigned char *content, size_t len, char *contentType, sohHeader_t **optHeaders,
  char *tid)
{
  char szResourceURI[MAX_BUF_SIZE_B];
  sprintf(szResourceURI, "%s/newRsc", targetID);
  sohCreateResponse(SOH_SC_CREATED, szResourceURI, content, len, contentType, NULL, tid);
  return SOH_RC_OK;
}

SOH_RCODE testerSohRetrieveCb (char *reqEntity, char *targetID,
  sohHeader_t **optHeaders, char *tid)
{
  char szContent[MAX_BUF_SIZE_H];
  char szTemp[MAX_BUF_SIZE_H];
  if (strlen(targetID) % 2)
  {
    sprintf(szContent, "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\"?>"
      "<m2m:contentInstance xmlns:m2m=\"http://uri.etsi.org/m2m\" />");
    sohRetrieveResponse(SOH_SC_OK, (unsigned char *)szContent, strlen(szContent), 
      "application/xml", NULL, tid);
  }
  else
  {
    sprintf(szTemp, "%s: resource not found", targetID);
    sprintf(szContent, erorMsgTemplate, "STATUS_METHOD_NOT_FOUND", szTemp);
    sohRetrieveResponse(SOH_SC_NOT_FOUND, (unsigned char *)szContent, strlen(szContent), 
      "application/xml", NULL, tid);
  }
  return SOH_RC_OK;
}

SOH_RCODE testerSohUpdateCb (char *reqEntity, char *targetID,
  unsigned char *content, size_t len, char *contentType, sohHeader_t **optHeaders, 
  char *tid)
{
  char szContent[MAX_BUF_SIZE_H];
  char szTemp[MAX_BUF_SIZE_H];
  if (strlen(targetID) % 2)
  {
    sohUpdateResponse(SOH_SC_OK, content, len, contentType, NULL, tid);
  }
  else
  {
    sprintf(szTemp, "%s: resource not found", targetID);
    sprintf(szContent, erorMsgTemplate, "STATUS_METHOD_NOT_FOUND", szTemp);
    sohUpdateResponse(SOH_SC_NOT_FOUND, (unsigned char *)szContent, strlen(szContent),
      "application/xml", NULL, tid);
  }
  return SOH_RC_OK;
}

SOH_RCODE testerSohDeleteCb (char *reqEntity, char *targetID,                                     
  sohHeader_t **optHeaders, char *tid)
{
  char szContent[MAX_BUF_SIZE_H];
  char szTemp[MAX_BUF_SIZE_H];
  if (strlen(targetID) % 2)
  {
    sohDeleteResponse(SOH_SC_NO_CONTENT, NULL, 0, NULL, NULL, tid);
  }
  else
  {
    sprintf(szTemp, "%s: resource not found", targetID);
    sprintf(szContent, erorMsgTemplate, "STATUS_METHOD_NOT_FOUND", szTemp);
    sohDeleteResponse(SOH_SC_NOT_FOUND, (unsigned char *)szContent, strlen(szContent), 
      "application/xml", NULL, tid);
  }
  return SOH_RC_OK;
}


