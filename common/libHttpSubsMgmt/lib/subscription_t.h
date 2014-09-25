
#ifndef __SUBSCRIPTION_T__H_
#define __SUBSCRIPTION_T__H_



typedef struct subscription_t
{
  void (*free) (struct subscription_t *);
  int (*subscribe) (struct subscription_t *, void *issuerData,
    PF_SOH_CREATE_RESPONSE_CB createRespCb);
  int (*reSubscribe) (struct subscription_t *, void *issuerData, 
    PF_SOH_UPDATE_RESPONSE_CB updateRespCb);
  int (*unsubscribe) (struct subscription_t *, void *issuerData, 
    PF_SOH_DELETE_RESPONSE_CB deleteRespCb);
  void (*notify) (struct subscription_t *, char *content);
  void (*display) (struct subscription_t *, PF_HTTP_SUBS_PRINT_CB dispFunc, 
    void *handle);

  unsigned long id;
  char idStr[BUF_SIZE_M]; 
  char *nsclProxy; 
  char *targetID; 
  char *reqEntity; 
  SUBS_TYPE type;
  unsigned long minTimeBtwnNotif;
  bool fullInitialReport; 
  time_t lastSubsDate;
  time_t lastNotifyDate;
  SUBS_PENDING_STATE state;
  char *pendingTid;

  PF_HTTP_SUBS_NOTIFY_CB notifyCb;
  PF_HTTP_SUBS_ERROR_CB errorCb;
  void *issuerData; 

  struct list_head chainLink;
    
} subscription_t;

subscription_t *new_subscription_t(char *nsclProxy, char *targetID, char *reqEntity, 
  SUBS_TYPE type, unsigned long minTimeBetweenNotif, bool fullInitialReport, 
  PF_HTTP_SUBS_NOTIFY_CB notifyCb, PF_HTTP_SUBS_ERROR_CB pfHttpSubsErrorCb, 
  void *issuerData);
void subscription_t_newFree(subscription_t *This);

int subscription_t_subscribe(subscription_t *This, void *issuerData, 
  PF_SOH_CREATE_RESPONSE_CB createRespCb);
int subscription_t_reSubscribe(subscription_t *This, void *issuerData, 
  PF_SOH_UPDATE_RESPONSE_CB updateRespCb);
int subscription_t_unsubscribe(subscription_t *This, void *issuerData, 
  PF_SOH_DELETE_RESPONSE_CB deleteRespCb);
void subscription_t_notify(subscription_t *This, char *content);
void subscription_t_display(subscription_t *This, PF_HTTP_SUBS_PRINT_CB dispFunc, 
    void *handle);

#endif


