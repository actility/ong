
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


extern char *g_httpSrvFQDN;
extern int g_httpSrvPort;
extern int g_subsDuration;

static unsigned long g_subscriptionId = 0;

static void subscription_t_init(subscription_t *This);


/**
 * Initialize the data structure of the object-like.
 * @param This the object-like instance.
 */
static void subscription_t_init(subscription_t *This)
{
  RTL_TRDBG(TRACE_DEBUG, "subscription_t::init\n");

  /* Assign the pointers on functions */
  This->subscribe = subscription_t_subscribe;
  This->reSubscribe = subscription_t_reSubscribe;
  This->unsubscribe = subscription_t_unsubscribe;
  This->notify = subscription_t_notify;
  This->display = subscription_t_display;
  /* Initialize the members */
  This->id = 0;
  This->targetID = NULL;
  This->reqEntity = NULL;
  This->nsclProxy = NULL;
  This->type = SUBS_TYPE_UNKNOWN;
  This->fullInitialReport = false;
  This->minTimeBtwnNotif = 0;
  This->lastSubsDate = 0;
  This->lastNotifyDate = 0;
  This->pendingTid = NULL;
  This->state = SUBS_ST_INIT;
  This->notifyCb = NULL;
  This->errorCb = NULL;
  This->issuerData = NULL;
}

/**
 * Dynamic allocation of the object-like instance
 * @param name the name of the header.
 * @return the created object-like instance.
 */
subscription_t *new_subscription_t(char *nsclProxy, char *targetID, char *reqEntity, 
  SUBS_TYPE type, unsigned long minTimeBtwnNotif, bool fullInitialReport,
  PF_HTTP_SUBS_NOTIFY_CB notifyCb, PF_HTTP_SUBS_ERROR_CB errorCb, void *issuerData)
{
  subscription_t *This = malloc(sizeof(subscription_t));
  if (!This)
  {
    return NULL;
  }
  subscription_t_init(This);
  This->free = subscription_t_newFree;
  This->id = ++g_subscriptionId;
  This->nsclProxy = strdup(nsclProxy); 
  This->targetID = strdup(targetID); 
  This->reqEntity = strdup(reqEntity); 
  This->type = type;
  This->minTimeBtwnNotif = minTimeBtwnNotif;
  This->fullInitialReport = fullInitialReport; 
  This->notifyCb = notifyCb;
  This->errorCb = errorCb;
  This->issuerData = issuerData;

  sprintf(This->idStr, SUBS_STR_FORMAT, This->id, g_httpSrvFQDN, g_httpSrvPort); 

  RTL_TRDBG(TRACE_DEBUG, "new subscription_t (id:%lu) (This:%0.8p)\n", This->id, This);
  return This;
}

/**
 * Free resources of the object-like instance.
 * @param This the object-like instance.
 */
void subscription_t_newFree(subscription_t *This)
{
  RTL_TRDBG(TRACE_DEBUG, "subscription_t::newFree (id:%lu)\n", This->id);
  free(This->nsclProxy);
  free(This->targetID);
  free(This->reqEntity);
  free(This);
}


/**
 * Do send the CREATE request for the subscription.
 * @param This the object-like instance.
 * @param issuerData the handle to pass to the call back method that SOH will invoke on
 * response receipt.
 * @param createRespCb the call back method that SOH will invoke on response receipt.
 * @return 1 if the subscription can be built and sent successfully, 0 otherwise.
 */
int subscription_t_subscribe(subscription_t *This, 
  void *issuerData, PF_SOH_CREATE_RESPONSE_CB createRespCb)
{
  void *subs;
  void *filterCriteria;
  char *subsXml;
  char szTemp[BUF_SIZE_M];
  struct timeval tv;
  struct timezone tz;
  int rc = 1;

  if ((This->state != SUBS_ST_INIT) && 
      (This->state != SUBS_ST_EXPIRED))
  {
    RTL_TRDBG(TRACE_ERROR, "subscription_t::subscribe: "
      "subscription is in wrong state (id:%lu) (state:%s)\n", 
      This->id, stateStr(This->state));
    return 0;
  }

  subs = XoNmNew("m2m:subscription");
  // Fix here all that need to be fixed in m2mxoref...
  XoNmSetAttr(subs, "name$", "m2m:subscription", 0, 0);
  XoSetNameSpace(subs, "m2m");
  XoNmSetAttr(subs, "m2m:id", This->idStr, 0, 0);
 
  // minimalTimeBetweenNotifications
  if (This->minTimeBtwnNotif)
  { 
    sprintf(szTemp, "%lu", This->minTimeBtwnNotif);
    XoNmSetAttr(subs, "m2m:minimalTimeBetweenNotifications", szTemp, 0, 0);
  }

  // contact
  snprintf(szTemp, BUF_SIZE_M, "http://%s:%d/?%s=%lu", g_httpSrvFQDN, 
    g_httpSrvPort, SUBS_QPARAM_ID, This->id);
  XoNmSetAttr(subs, "m2m:contact", szTemp, 0, 0);

  gettimeofday(&tv, &tz);

  // filter criteria - createdSince
  if (! This->fullInitialReport)
  {
    filterCriteria = XoNmNew("m2m:ContentInstanceFilterCriteriaType");
    XoNmSetAttr(filterCriteria, "name$", "m2m:filterCriteria", 0, 0);
    XoSetNameSpace(filterCriteria, "m2m");
    XoSetNameSpace(filterCriteria, "xsi");
    XoNmSetAttr(filterCriteria, "xsi:type", "m2m:ContentInstanceFilterCriteriaType", 0, 0);
    rtl_gettimeofday_to_iso8601date(&tv, &tz, szTemp);
    XoNmSetAttr(filterCriteria, "createdSince", szTemp, 0, 0);
    XoNmSetAttr(subs, "m2m:filterCriteria", filterCriteria, 0, 0);
  }


  rtl_timemono(&This->lastSubsDate); 
  // expirationTime
  tv.tv_sec += g_subsDuration;
  rtl_gettimeofday_to_iso8601date(&tv, &tz, szTemp);
  XoNmSetAttr(subs, "m2m:expirationTime", szTemp, 0, 0);

  subsXml = xoSerializeAndFree(subs);

  sprintf(szTemp, "%s/subscriptions/", This->targetID);
  // sends the create request
  if (SOH_RC_OK != sohCreateRequest(This->nsclProxy, This->reqEntity, szTemp,
        (unsigned char *)subsXml, strlen(subsXml), CONTENT_TYPE_VALUE, 
        NULL, &This->pendingTid, issuerData, createRespCb))
  {
    RTL_TRDBG(TRACE_ERROR, "subscription_t::subscribe: "
      "cannot send CREATE request (id:%lu) (content:%s)\n", 
      This->id, subsXml);
    rc = 0;
    This->pendingTid = NULL;
  }
  else
  {
    This->state = SUBS_ST_WAITING_SUBSCRIBE_RESP;
  } 

  free(subsXml);

  return rc; 
}

/**
 * Do send the UPDATE request for the subscription.
 * @param This the object-like instance.
 * @param issuerData the handle to pass to the call back method that SOH will invoke on
 * response receipt.
 * @param updateRespCb the call back method that SOH will invoke on response receipt.
 * @return 1 if the subscription update can be built and sent successfully, 0 otherwise.
 */
int subscription_t_reSubscribe(subscription_t *This, 
  void *issuerData, PF_SOH_UPDATE_RESPONSE_CB updateRespCb)
{
  void *subs;
  char *subsXml;
  char szTemp[BUF_SIZE_M];
  struct timeval tv;
  struct timezone tz;
  int rc = 1;

  if (This->state != SUBS_ST_ACTIVE)
  {
    RTL_TRDBG(TRACE_ERROR, "subscription_t::reSubscribe: "
      "Subscription is in wrong state (id:%lu) (state:%s)\n", 
      This->id, stateStr(This->state));
    return 0;
  }
 
  subs = XoNmNew("m2m:subscription");
  XoSetNameSpace(subs, "m2m");
  // Fix here all that need to be fixed in m2mxoref...
  XoNmSetAttr(subs, "name$", "m2m:subscription", 0, 0);
 
  // minimalTimeBetweenNotifications
  if (This->minTimeBtwnNotif)
  { 
    sprintf(szTemp, "%lu", This->minTimeBtwnNotif);
    XoNmSetAttr(subs, "m2m:minimalTimeBetweenNotifications", szTemp, 0, 0);
  }

  rtl_timemono(&This->lastSubsDate); 
  // expirationTime
  gettimeofday(&tv, &tz);
  tv.tv_sec += g_subsDuration;
  rtl_gettimeofday_to_iso8601date(&tv, &tz, szTemp);
  XoNmSetAttr(subs, "m2m:expirationTime", szTemp, 0, 0);

  subsXml = xoSerializeAndFree(subs);

  sprintf(szTemp, "%s/subscriptions/%s", This->targetID, This->idStr);
  // sends the create request
  if (SOH_RC_OK != sohUpdateRequest(This->nsclProxy, This->reqEntity, szTemp,
        (unsigned char *)subsXml, strlen(subsXml), CONTENT_TYPE_VALUE, 
        NULL, &This->pendingTid, issuerData, updateRespCb))
  {
    RTL_TRDBG(TRACE_ERROR, "subscription_t::reSubscribe: "
      "cannot send UPDATE request (id:%lu) (content:%s)\n", 
      This->id, subsXml);
    rc = 0;
    This->pendingTid = NULL;
  } 
  else
  {
    This->state = SUBS_ST_WAITING_RESUBSCRIBE_RESP;
  } 

  free(subsXml);

  return rc; 
}

/**
 * Do send the DELETE request for the subscription.
 * @param This the object-like instance.
 * @param issuerData the handle to pass to the call back method that SOH will invoke on
 * response receipt.
 * @param deleteRespCb the call back method that SOH will invoke on response receipt.
 * @return 1 if the subscription can be delete successfully, 0 otherwise.
 */
int subscription_t_unsubscribe(subscription_t *This, 
  void *issuerData, PF_SOH_DELETE_RESPONSE_CB deleteRespCb)
{
  char szTemp[BUF_SIZE_M];
  int rc = 1;

// no need to check actual state for unsubscription... worst case is 
// unecessary request.
//  if (This->state != SUBS_ST_ACTIVE)
//  {
//    RTL_TRDBG(TRACE_ERROR, "subscription_t::unsubscribe: "
//      "Subscription is in wrong state (id:%lu) (state:%s)\n", 
//      This->id, stateStr(This->state));
//    return 0;
//  }

  sprintf(szTemp, "%s/subscriptions/%s", This->targetID, This->idStr);
  // sends the create request
  if (SOH_RC_OK != sohDeleteRequest(This->nsclProxy, This->reqEntity, szTemp,
        NULL, &This->pendingTid, issuerData, deleteRespCb))
  {
    RTL_TRDBG(TRACE_ERROR, "subscription_t::unsubscribe: "
      "cannot send DELETE request (id:%lu)\n", 
      This->id);
    rc = 0;
    This->pendingTid = NULL;
  } 
  else
  {
    This->state = SUBS_ST_WAITING_UNSUBSCRIBE_RESP;
  } 
  return rc; 
}

/**
 * Do inform the issuer of a NOTIFY request receipt for the subscription.
 * @param This the object-like instance.
 * @param content the content of the NOTIFY
 */
void subscription_t_notify(subscription_t *This, char *content)
{
  rtl_timemono(&This->lastNotifyDate); 
  This->notifyCb(This->id, This->issuerData, content);
}

/**
 * Format the subscription and use the provided function to display it.
 * @param This the object-like instance.
 * @param dispFunc
 * @param handle
 */
void subscription_t_display(subscription_t *This, PF_HTTP_SUBS_PRINT_CB dispFunc, 
  void *handle)
{
  time_t now;
  char lastSubs[BUF_SIZE_S];
  char lastNotif[BUF_SIZE_S];

  rtl_timemono(&now);
  if (This->lastSubsDate)
  {
    sprintf(lastSubs, "%lu seconds ago", now - This->lastSubsDate);
  }
  else
  {
    sprintf(lastSubs, "never");
  }

  if (This->lastNotifyDate)
  {
    sprintf(lastNotif, "%lu seconds ago", now - This->lastNotifyDate);
  }
  else
  {
    sprintf(lastNotif, "never");
  }

  dispFunc(handle, 
    "* id:          %lu\n"
    "  m2m:id:      %s\n"
    "  targetID:    %s\n"
    "  reqEntity:   %s\n"
    "  state:       %s\n" 
    "  last subs:   %s\n" 
    "  last notify: %s\n", 
    This->id, This->idStr, This->targetID, This->reqEntity, 
    stateStr(This->state), lastSubs, lastNotif);
}


