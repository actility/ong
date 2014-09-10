#include <stdio.h>
#include <stdlib.h>                                                                                 
#include <unistd.h>
#include <stdbool.h>
#include <string.h>

#include "rtlbase.h"

#include "song_over_http_api.h"
#include "http_subs_api.h"
#include "common.h"
#include "http_subs_soh_impl.h"
#include "subscription_t.h"
#include "subscriptionsList_t.h"

extern subscriptionsList_t *g_subsList;

//const char erorMsgTemplate[] =
//  "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\"?>"
//  "<m2m:errorInfo xmlns:m2m=\"http://uri.etsi.org/m2m\">"
//  "<m2m:statusCode>%s</m2m:statusCode>"
//  "<m2m:additionalInfo>%s</m2m:additionalInfo>"
//  "</m2m:errorInfo>";

SOH_RCODE httpSubsSohCreateCb(char *reqEntity, char *targetID, unsigned char *content,
  size_t len, char *contentType, sohHeader_t **optHeaders, char *tid)
{
  int i = 0;
  unsigned long subsId = -1;
  subscription_t *subs = NULL;
  char *szContent;
  char szTemp[BUF_SIZE_L];
  
  if ((NULL != contentType) && 
      (0 == strncasecmp(contentType, CONTENT_TYPE_VALUE, strlen(CONTENT_TYPE_VALUE))))
  { 
    // need to parse the targetID to see if this is a notify
    while ((! subs) && optHeaders && (optHeaders[i]))
    {
      if ((0 == strcmp(optHeaders[i]->name, SUBS_QPARAM_ID)) &&
          (optHeaders[i]->values))
      {
        subsId = atoi(optHeaders[i]->values);
        subs = g_subsList->get(g_subsList, subsId); 
      }
      i++;
    }
    
    if (! subs)
    {
      RTL_TRDBG(TRACE_INFO, "httpSubsSohCreateCb - received message from unknown "
        "subscription (id:%lu)\n", subsId);
      // send a 404 Not Found response back to the client
      szContent = buildErrInfoContent(SOH_SC_NOT_FOUND, szTemp);
      sohCreateResponse(SOH_SC_NOT_FOUND, NULL, (unsigned char *)szContent, 
        strlen(szContent), "application/xml", NULL, tid);
      free(szContent);
    }
    else
    {
      RTL_TRDBG(TRACE_INFO, "httpSubsSohCreateCb - received notify (id:%lu)\n",
        subs->id);
      // should we look at the status code before ??
  
      // trigger the issuer
      subs->notifyCb(subs->id, subs->issuerData, (char *)content);
      // send a 200 OK response back to the client
      sohCreateResponse(SOH_SC_OK, NULL, NULL, 0, NULL, NULL, tid);
    }
  }
  else
  {
    RTL_TRDBG(TRACE_INFO, "httpSubsSohCreateCb - received unexpected request "
      "(targetID:%s) (contentType:%s)\n", targetID, contentType);
    szContent = buildErrInfoContent(SOH_SC_INTERNAL_SERVER_ERROR, "unexpected request here");
    sohCreateResponse(SOH_SC_INTERNAL_SERVER_ERROR, NULL, (unsigned char *)szContent, 
        strlen(szContent), "application/xml", NULL, tid);
    free(szContent);
  }
  return SOH_RC_OK;
}

SOH_RCODE httpSubsSohRetrieveCb(char *reqEntity, char *targetID,
  sohHeader_t **optHeaders, char *tid)
{
  char *szContent = buildErrInfoContent(SOH_SC_NOT_IMPLEMENTED,
    "libHttpSubsMgmt: operation not supported here");
  sohRetrieveResponse(SOH_SC_NOT_IMPLEMENTED, (unsigned char *)szContent, 
    strlen(szContent), "application/xml", NULL, tid);
  free(szContent);
  return SOH_RC_OK;
}

SOH_RCODE httpSubsSohUpdateCb(char *reqEntity, char *targetID, unsigned char *content,
  size_t len, char *contentType, sohHeader_t **optHeaders, char *tid)
{
  char *szContent = buildErrInfoContent(SOH_SC_NOT_IMPLEMENTED,
    "libHttpSubsMgmt: operation not supported here");
  sohUpdateResponse(SOH_SC_NOT_IMPLEMENTED, (unsigned char *)szContent, strlen(szContent),
    "application/xml", NULL, tid);
  free(szContent);
  return SOH_RC_OK;
}

SOH_RCODE httpSubsSohDeleteCb(char *reqEntity, char *targetID,
  sohHeader_t **optHeaders, char *tid)
{
  char *szContent = buildErrInfoContent(SOH_SC_NOT_IMPLEMENTED,
    "libHttpSubsMgmt: operation not supported here");
  sohDeleteResponse(SOH_SC_NOT_IMPLEMENTED, (unsigned char *)szContent, strlen(szContent),
    "application/xml", NULL, tid);
  free(szContent);
  return SOH_RC_OK;
}


