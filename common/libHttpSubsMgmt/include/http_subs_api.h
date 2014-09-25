
#ifndef __HTTP_SUBS_MGMT__H__
#define __HTTP_SUBS_MGMT__H__

typedef enum _SUBS_TYPE
{
  SUBS_TYPE_UNKNOWN = 0,
  SUBS_TYPE_COMMON,
  SUBS_TYPE_CONTENT_INSTANCES,
  
} SUBS_TYPE;

typedef enum _SUBS_ERR_CODE
{
  SUBS_ERR_UNKNOWN = 0,
  SUBS_ERR_SUBSCRIPTION_CREATE_FAILED, 
  SUBS_ERR_SUBSCRIPTION_UPDATE_FAILED, 
  SUBS_ERR_SUBSCRIPTION_DELETE_FAILED, 

} SUBS_ERR_CODE;

/**
 * Prototype for "notify" call back method
 * @param subscriptionId the subscription integer identifier
 * @param issuerData the issuer handle passed by httpSubsCreate.
 * @param content the notify content.
 */
typedef void (*PF_HTTP_SUBS_NOTIFY_CB)(unsigned long subscriptionId, void *issuerData, 
  char *content);

/**
 * Prototype for error call back method
 * @param subscriptionId the subscription integer identifier
 * @param issuerData the issuer handle passed by httpSubsCreate.
 * @param errorCode the error code.
 * @param errorMsg the error message.
 */
typedef void (*PF_HTTP_SUBS_ERROR_CB)(unsigned long subscriptionId, void *issuerData, 
  SUBS_ERR_CODE errorCode, char *errorMsg);

/**
 * Prototype of call back method used to display subscriptions entries (httpSubsDisplayAll).
 * @param handle the issuer handle passed by httpSubsDisplayAll.
 * @param msg the format string, same behavior as 'printf' functions family.
 * @param va_list the argument to be formatted in msg.
 */
typedef void (*PF_HTTP_SUBS_PRINT_CB)(char *handle, char *msg, ...);

/**
 * Initialize the sub-layers of the library. The issuer must initialize rtlbase layer first.
 * @param httpSrvFQDN the IP address or FQDN that the SCL may use to connect to the HTTP
 * server ran by the library.
 * @param httpSrvPort the HTTP port that the SCL may use to connect to the HTTP
 * server ran by the library.
 * @param subsDuration the subscription duration; used to compute the expirationTime value
 * and used as input parameter in subscription refreshing algorithm. (in seconds)
 * @param resubsMgmtTmr defines at which frequency the refreshing algorithm will run. (in
 * seconds)
 * @param resubsSafetyDelay defines the safety delay to consider for refreshing the
 * subscription (in seconds). A subscription needs to be refreshed when:
 * now - last_subscription_date > subsDuration - resubsSafetyDelay.
 * @return true (1) if the initialization succeeds, false (0) otherwise.
 */
int httpSubsInit(char *httpSrvFQDN, int httpSrvPort, int subsDuration, int resubsMgmtTmr, 
  int resubsSafetyDelay);

/**
 * Un-initialize the sub-layers of the library.
 */
int httpSubsUninit();

/**
 * The library does not start any new thread. It runs in the issuer thread. So the issuer
 * prgm must periodically invoke this function so that the library can do its job.
 * Ideally, should be invoked every 1/10e of seconds.
 */
void httpSubsClockMs();

/**
 * Create a new subscription. The subscription is then managed (periodically refreshed)
 * by the library.
 * @param subscriptionId the subscription integer identifier (incremental identifier,
 * starting at 1)
 * @param pfHttpSubsNotificationCb pointer on the function invoked by the library when it 
 * receives a notification for the associated subscription.
 * @param pfHttpSubsErrorCb pointer on the function invoked by the library when an error 
 * is detected on a subscription.
 * @param issuerData the issuer handle to provide to pfHttpSubsNotificationCb invocation.
 * @param reqEntity the requesting entity the library has to use for creating the
 * subscription. (http://my-rms.actility.com:8080/m2m/applications/GA)
 * @param nsclProxyHostPort If provided, the NSCL host and port to use as proxy 
 * (e.g. my-rms.actility.com:8080). Can be NULL if the request has to be routed directly.
 * @param targetID the URI to the resource to subscribe to. (e.g.
 * http://my-rms.actility.com:8080/m2m?targetID=http://my-ong.actility.com:8080/m2m/applications).
 * The library adds "/subscriptions" prefix to build the real targetID used to perform 
 * the CREATE request.
 * @param subsType specifies if the resource concerned by the susbcription to create is a
 * contentInstances or any other resource.
 * @param fullInitialReport when set to true the library specifies in the created 
 * subscription that the initial notify must contains all the historized values. when set to
 * false, the initial notify only contains the last one.
 * @param minTimeBtwnNotif specifies the value to use as
 * <m2m:minimalTimeBetweenNotifications> when creating a subscription (so, in milliseconds).
 * When set to 0, the library does not specifies any <m2m:minimalTimeBetweenNotifications>
 * value.
 * @return true (1) if the subscription can be created successfully, false (0) otherwise.
 */
int httpSubsCreate(unsigned long *subscriptionId, PF_HTTP_SUBS_NOTIFY_CB pfHttpSubsNotificationCb, 
  PF_HTTP_SUBS_ERROR_CB pfHttpSubsErrorCb, void *issuerData, char *reqEntity, 
  char *nsclProxyHostPort, char *targetID, SUBS_TYPE subsType, bool fullInitialReport, 
  unsigned long minTimeBtwnNotif);

/**
 * Delete the subscription.
 * @param subscriptionId the subscription integer identifier
 * @return true (1) if the subscription can be created successfully, false (0) otherwise.
 */
int httpSubsDelete(unsigned long subscriptionId);

/**
 * Use "dispFunc" to display information related to created  subscriptions.
 * @param dispFunc the function to use to display the information.
 * @param handle the handle to provide to dispFunc.
 */
void httpSubsDisplayAll(PF_HTTP_SUBS_PRINT_CB dispFunc, void *handle);

#endif


