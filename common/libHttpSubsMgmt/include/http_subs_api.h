
#ifndef __HTTP_SUBS_MGMT__H__
#define __HTTP_SUBS_MGMT__H__

typedef enum _SUBS_TYPE
{
  SUBS_TYPE_UNKNOWN = 0,
  SUBS_TYPE_COMMON,
  SUBS_TYPE_CONTENT_INSTANCES,
  
} SUBS_TYPE;

typedef void (*PF_HTTP_SUBS_NOTIFY_CB)(unsigned long subscriptionId, void *issuerData, 
  char *content);
typedef void (*PF_HTTP_SUBS_PRINT_CB)(char *handle, char *msg, ...);


int httpSubsInit(char *httpSrvFQDN, int httpSrvPort, int subsDuration, int resubsMgmtTmr, 
  int resubsSafetyDelay);
int httpSubsUninit();
void httpSubsClockMs();

int httpSubsCreate(unsigned long *subscriptionId, PF_HTTP_SUBS_NOTIFY_CB pfHttpSubsNotificationCb, 
  void *issuerData, char *reqEntity, char *nsclBaseUri, char *targetID, SUBS_TYPE subsType, 
  bool fullInitialReport, unsigned long minTimeBtwnNotif);
int httpSubsDelete(unsigned long subscriptionId);

void httpSubsDisplayAll(PF_HTTP_SUBS_PRINT_CB dispFunc, void *handle);

#endif

