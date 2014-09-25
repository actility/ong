#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>
#include <string.h>

#include "rtlbase.h"
#include "xoapipr.h"

#include "song_over_http_api.h"
#include "http_subs_api.h"
#include "common.h"
#include "subscription_t.h"
#include "subscriptionsList_t.h"
#include "http_subs_soh_impl.h"


char *g_httpSrvFQDN = NULL;
int g_httpSrvPort = 0;
int g_subsDuration = 0;
int g_resubsMgmtTmr = 0;
int g_resubsSafetyDelay = 0;
bool g_init = false;

subscriptionsList_t *g_subsList = NULL;

static void httpSubsSohSubscribeRespCb (SOH_STATUS_CODE statusCode,
  char *resourceURI, unsigned char *content, size_t len, char *contentType,
  sohHeader_t **optHeaders, char *tid, void *issuerData);
static void httpSubsSohReSubscribeRespCb (SOH_STATUS_CODE statusCode,
  unsigned char *content, size_t len, char *contentType,
  sohHeader_t **optHeaders, char *tid, void *issuerData);
static void httpSubsSohUnsubscribeRespCb (SOH_STATUS_CODE statusCode,
  unsigned char *content, size_t len, char *contentType,
  sohHeader_t **optHeaders, char *tid, void *issuerData);


static int httpSubsXoInit()
{
  char m2mXoFile[255];
  char *rootact;
  int ret;
  rootact = getenv("ROOTACT");
  if ((!rootact || !*rootact))
  {
    printf("%s unset, abort\n", "ROOTACT");
    return 0;
  }

  XoInit(0);
  sprintf(m2mXoFile, "%s/etc/xo/.", rootact);
  XoLoadNameSpaceDir(m2mXoFile);

  sprintf(m2mXoFile, "%s/etc/xo/m2m.xor", rootact);
  ret = XoExtLoadRef(m2mXoFile);
  if  (ret < 0)
  {
    RTL_TRDBG(TRACE_ERROR, "ERROR cannot load '%s' ret=%d\n", m2mXoFile, ret);
    return 0;
  }
  return 1;
}

int httpSubsInit(char *httpSrvFQDN, int httpSrvPort, int subsDuration, int resubsMgmtTmr, 
  int resubsSafetyDelay)
{
  int res;
  RTL_TRDBG(TRACE_INFO, ">>> httpSubsInit (httpSrvFQDN:%s) (httpSrvPort:%d) (subsDuration:%d)"
    "(resubsMgmtTmr:%d) (resubsSafetyDelay:%d)\n", httpSrvFQDN, httpSrvPort, subsDuration,
    resubsMgmtTmr, resubsSafetyDelay);
  g_httpSrvFQDN = strdup(httpSrvFQDN);
  g_httpSrvPort = httpSrvPort; 
  g_subsDuration = subsDuration;
  g_resubsMgmtTmr = resubsMgmtTmr;
  g_resubsSafetyDelay = resubsSafetyDelay;
  sohInit(httpSrvPort, httpSubsSohCreateCb, httpSubsSohRetrieveCb, httpSubsSohUpdateCb, 
    httpSubsSohDeleteCb);

  g_subsList = new_subscriptionsList_t();
  res = httpSubsXoInit();
  if (1 == res) 
  {
    g_init = true; 
  }

  return res;
}

int httpSubsUninit()
{
  g_init = false;
  RTL_TRDBG(TRACE_INFO, ">>> httpSubsUninit\n");
  sohUninit();
  g_subsList->free(g_subsList);
  g_subsList = NULL;
  free(g_httpSrvFQDN);
  g_httpSrvFQDN = NULL;
  return 1;
}

void httpSubsClockMs()
{
  int i = 0;
  subscription_t *subs;
  time_t now;
  static time_t lastReSubsciptionTime = 0;

  if (g_subsList)
  {
    sohClockMs();

    rtl_timemono(&now);

    if (lastReSubsciptionTime + g_resubsMgmtTmr < now)
    { 
      // look for re-subscriptions
      RTL_TRDBG(TRACE_DEBUG, "httpSubsClockMs: look for re-subscriptions\n");
      while (NULL != (subs = g_subsList->getNth(g_subsList, i)))
      {
        if (SUBS_ST_EXPIRED == subs->state)
        {
          RTL_TRDBG(TRACE_INFO, "httpSubsClockMs: subscription %d has expired; "
            "refreshing it\n", subs->id);
          subs->subscribe(subs, subs, httpSubsSohSubscribeRespCb);
        }
        else if ((SUBS_ST_ACTIVE == subs->state) &&
                 (now - subs->lastSubsDate > g_subsDuration - g_resubsSafetyDelay))
        {
          RTL_TRDBG(TRACE_INFO, "httpSubsClockMs: subscription %d need to be refreshed\n",
            subs->id);
          subs->reSubscribe(subs, subs, httpSubsSohReSubscribeRespCb);
        }
        i++;
      }
      rtl_timemono(&lastReSubsciptionTime);
    }
 
  }
}

int httpSubsCreate(unsigned long *subscriptionId, PF_HTTP_SUBS_NOTIFY_CB pfHttpSubsNotificationCb, 
  PF_HTTP_SUBS_ERROR_CB pfHttpSubsErrorCb, void *issuerData, char *reqEntity, 
  char *nsclProxyHostPort, char *targetID, SUBS_TYPE type, bool fullInitialReport, 
  unsigned long minTimeBtwnNotif)
{
  subscription_t *newSubs = NULL;

  if (! g_init) 
  {
    RTL_TRDBG(TRACE_ERROR, ">>> httpSubsCreate aborted; not initialized.");
    return 0;
  }

  newSubs = new_subscription_t(nsclProxyHostPort, targetID, reqEntity, type, 
    minTimeBtwnNotif, fullInitialReport, pfHttpSubsNotificationCb, 
    pfHttpSubsErrorCb, issuerData);

  if (! newSubs) return 0;

  *subscriptionId = newSubs->id;
  
  RTL_TRDBG(TRACE_INFO, ">>> httpSubsCreate (id:%lu) (proxy:%s) (reqEntity:%s) "
    "(targetID:%s) (type:%d) (fullInitialReport:%d) (minTimeBtwnNotif:%d)\n", 
    newSubs->id, nsclProxyHostPort, reqEntity, 
    targetID, type, fullInitialReport, minTimeBtwnNotif);
  return newSubs->subscribe(newSubs, newSubs, httpSubsSohSubscribeRespCb);
}

int httpSubsDelete(unsigned long subscriptionId)
{
  subscription_t *subs;
  if (! g_init) 
  {
    RTL_TRDBG(TRACE_ERROR, ">>> httpSubsDelete aborted; not initialized.");
    return 0;
  }

  subs = g_subsList->get(g_subsList, subscriptionId);

  RTL_TRDBG(TRACE_INFO, ">>> httpSubsDelete (id:%lu)\n", subscriptionId);

  if (! subs) return 0;

  return subs->unsubscribe(subs, subs, httpSubsSohUnsubscribeRespCb);
}

void httpSubsDisplayAll(PF_HTTP_SUBS_PRINT_CB dispFunc, void *handle)
{
  int i = 0;
  subscription_t *subs;
  if (! g_init) 
  {
    dispFunc(handle, "Stack not initialized. Aborted.\n");
  }
  else
  {
    dispFunc(handle, "List of active subscriptions:\n");
    while (NULL != (subs = g_subsList->getNth(g_subsList, i)))
    {
      subs->display(subs, dispFunc, handle);
      i++;
    }
  }
}

static void httpSubsSohSubscribeRespCb (SOH_STATUS_CODE statusCode,
  char *resourceURI, unsigned char *content, size_t len, char *contentType,
  sohHeader_t **optHeaders, char *tid, void *issuerData)
{
  char errMsg[BUF_SIZE_L];
  subscription_t *subs = (subscription_t *)issuerData;
  if (!subs) return;
  if (!g_init) return; 


  RTL_TRDBG(TRACE_INFO, "<<< httpSubsSohSubscribeRespCb (id:%lu) (statusCode:%d)\n", 
    subs->id, statusCode);
  switch (statusCode)
  {
    case SOH_SC_CREATED:
      // successfully create, can be added in the managed list.
      subs->state = SUBS_ST_ACTIVE;
      if (! g_subsList->get(g_subsList, subs->id))
      {
        g_subsList->add(g_subsList, subs);
      }
      break;

    case SOH_SC_CONFLICT:
      // already exist, need to be refreshed
      subs->state = SUBS_ST_ACTIVE;
      if (! g_subsList->get(g_subsList, subs->id))
      {
        g_subsList->add(g_subsList, subs);
      }
      subs->reSubscribe(subs, subs, httpSubsSohReSubscribeRespCb);
      break;

    default:
      snprintf(errMsg, BUF_SIZE_L, "Failed to subscribe (statusCode:%d) (msg:%s)", 
        statusCode, content);
      subs->errorCb(subs->id, subs->issuerData, SUBS_ERR_SUBSCRIPTION_CREATE_FAILED, errMsg);
      RTL_TRDBG(TRACE_ERROR, "httpSubsSohSubscribeRespCb: Unsupported response "
        "code received on subscribe; aborted (id:%lu) (statusCode:%d) (msg:%s)\n",
        subs->id, statusCode, content);
      subs->free(subs);
      break;
  }
}

static void httpSubsSohReSubscribeRespCb (SOH_STATUS_CODE statusCode,
  unsigned char *content, size_t len, char *contentType,
  sohHeader_t **optHeaders, char *tid, void *issuerData)
{
  char errMsg[BUF_SIZE_L];
  subscription_t *subs = (subscription_t *)issuerData;
  if (!subs) return;
  if (!g_init) return; 
  RTL_TRDBG(TRACE_INFO, "<<< httpSubsSohReSubscribeRespCb (id:%lu) (statusCode:%d)\n", 
    subs->id, statusCode);
  switch (statusCode)
  {
    case SOH_SC_OK:
    case SOH_SC_NO_CONTENT:
      // successfully updated, change state
      subs->state = SUBS_ST_ACTIVE;
      break;

    case SOH_SC_NOT_FOUND:
      subs->state = SUBS_ST_EXPIRED;
      subs->subscribe(subs, subs, httpSubsSohSubscribeRespCb);
      break;

    default:
      snprintf(errMsg, BUF_SIZE_L, "Failed to re-subscribe (statusCode:%d) (msg:%s)", 
        statusCode, content);
      subs->errorCb(subs->id, subs->issuerData, SUBS_ERR_SUBSCRIPTION_UPDATE_FAILED, errMsg);
      RTL_TRDBG(TRACE_ERROR, "httpSubsSohReSubscribeRespCb: unsupported response "
        "code received on resubscribe; will try again later (id:%lu) (statusCode:%d) "
        "(msg:%s)\n", subs->id, statusCode, content) 
      subs->state = SUBS_ST_EXPIRED;
      break;
  }
}

static void httpSubsSohUnsubscribeRespCb (SOH_STATUS_CODE statusCode,
  unsigned char *content, size_t len, char *contentType,
  sohHeader_t **optHeaders, char *tid, void *issuerData)
{
  char errMsg[BUF_SIZE_L];
  subscription_t *subs = (subscription_t *)issuerData;
  if (!subs) return;
  if (!g_init) return; 
  RTL_TRDBG(TRACE_INFO, "<<< httpSubsSohUnsubscribeRespCb (id:%lu) (statusCode:%d)\n", 
    subs->id, statusCode);
  switch (statusCode)
  {
    case SOH_SC_OK:
    case SOH_SC_NO_CONTENT:
      // successfully delete, can be remove from the managed list.
      g_subsList->remove(g_subsList, subs->id);
      subs->free(subs);
      break;

    default:
      snprintf(errMsg, BUF_SIZE_L, "Failed to un-subscribe (statusCode:%d) (msg:%s)", 
        statusCode, content);
      subs->errorCb(subs->id, subs->issuerData, SUBS_ERR_SUBSCRIPTION_DELETE_FAILED, errMsg);
      RTL_TRDBG(TRACE_ERROR, "httpSubsSohUnsubscribeRespCb: unsupported response "
        "code received on unsubscribe; will try again later (id:%lu) (statusCode:%d) "
        "(msg:%s)\n", subs->id, statusCode, content) 
      g_subsList->remove(g_subsList, subs->id);
      subs->free(subs);
      break;
  }
}


