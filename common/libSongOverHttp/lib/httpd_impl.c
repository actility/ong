#include <sys/types.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <time.h>
#include <microhttpd.h>
#include <net/if.h>
#include <curl/curl.h>
#include <pthread.h>


#include "rtlbase.h"
#include "song_over_http_api.h"
#include "httpd_impl.h"
#include "common.h"
#include "srvTransaction_t.h"
#include "srvTransactionsList_t.h"

const char c_errorMsgTemplate[] =
  "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\"?>"
  "<m2m:errorInfo xmlns:m2m=\"http://uri.etsi.org/m2m\">"
  "<m2m:statusCode>%s</m2m:statusCode>"
  "<m2m:additionalInfo>%s</m2m:additionalInfo>"
  "</m2m:errorInfo>";


static unsigned int nrOfClients = 0;
static pthread_mutex_t httpdMutex;

static unsigned long nbTotalRecv = 0;
static unsigned long nbTotalPostRecv = 0;
static unsigned long nbTotalPutRecv = 0;
static unsigned long nbTotalGetRecv = 0;
static unsigned long nbTotalDeleteRecv = 0;
static unsigned long nbRecvProcessed = 0;
static unsigned long recvSz = 0;

static PF_SOH_CREATE_REQUEST_CB g_sohCreateCb = NULL;
static PF_SOH_RETRIEVE_REQUEST_CB g_sohRetrieve = NULL;
static PF_SOH_UPDATE_REQUEST_CB g_sohUpdateCb = NULL;
static PF_SOH_DELETE_REQUEST_CB g_sohDeleteCb = NULL;

extern struct MHD_Daemon *songHttpDaemon;
extern srvTransactionsList_t *pendingSrvTransactions;

/*****************************************************************************/
/* Prototypes for local functions */
static int headersIterator (void *cls, enum MHD_ValueKind kind, 
  const char *key, const char *value);
static int doPostProcessing (struct MHD_Connection *connection,
  const char *url, const char *uploadData, size_t *uploadDataSize, void **conCls);
static int doPutProcessing (struct MHD_Connection *connection,
  const char *url, const char *uploadData, size_t *uploadDataSize, void **conCls);
static int doGetProcessing (struct MHD_Connection *connection,
  const char *url, void **conCls);
static int doDeleteProcessing (struct MHD_Connection *connection,
  const char *url, void **conCls);
static void expireTransactions();
static int queryParametersIterator(void *cls, enum MHD_ValueKind kind, const char *key,
  const char *value);
  

/*****************************************************************************/
/**
 * Initialize the microhttpd implementation layer.
 */
void httpdInit(PF_SOH_CREATE_REQUEST_CB createCb, PF_SOH_RETRIEVE_REQUEST_CB retrieveCb,
  PF_SOH_UPDATE_REQUEST_CB updateCb, PF_SOH_DELETE_REQUEST_CB deleteCb)
{
  pthread_mutex_init(&httpdMutex, NULL);
  g_sohCreateCb = createCb;
  g_sohRetrieve = retrieveCb;
  g_sohUpdateCb = updateCb;
  g_sohDeleteCb = deleteCb;
}

void httpdUninit()
{
  // nothing to do for now on
}

/*****************************************************************************/
/* libmicrohttpd call backs */

/**
 * Immediately send a static page back to the client.
 * @param connection the MHD connection handle. With HTTP, a transaction maps a 
 * connection.
 * @param page the content to send in the response. 
 * @param statusCode the HTTP status code to use to build the response.
 * @return MHD_YES if the response can be queued successfully, MHD_NO otherwise.
 */
int sendStaticPage (struct MHD_Connection *connection, const char *page,
  int statusCode)
{
  int ret;
  struct MHD_Response *response;

  // The Content-Length is automatically added when using this callback.
  response = MHD_create_response_from_buffer (strlen (page), (void *) page,
    MHD_RESPMEM_MUST_COPY);
  if (!response)
    return MHD_NO;
  MHD_add_response_header (response, MHD_HTTP_HEADER_CONTENT_TYPE, "application/html");
  ret = MHD_queue_response (connection, statusCode, response);
  MHD_destroy_response (response);

  return ret;
}

/**
 * This callback is invoked by MHD when releasing the request/response context.
 */
void requestCompleted (void *cls, struct MHD_Connection *connection,                               
  void **conCls, enum MHD_RequestTerminationCode toe)
{
  srvTransaction_t *trans = *conCls;
  if (trans)
  {
pthread_mutex_lock(&httpdMutex);
    nrOfClients--;
    RTL_TRDBG(TRACE_DEBUG, "httpd: request_completed (nrOfClients:%d)\n", nrOfClients);
pthread_mutex_unlock(&httpdMutex);
    pendingSrvTransactions->remove(pendingSrvTransactions, trans->transId);
    trans->free(trans);
  }
  *conCls = NULL;
}

/**
 * This callback is invoked by MHD at every steps of the request analysis.
 * 
 */
int answerToConnection (void *cls, struct MHD_Connection *connection,
  const char *url, const char *method, const char *version, const char *uploadData,
  size_t *uploadDataSize, void **conCls)
{
  char errInfo[MAX_BUF_SIZE_B];
  char errMsg[MAX_BUF_SIZE_S];
  srvTransaction_t *trans = *conCls;
  char *user;
  char *pass;

  if (trans && (NULL == trans->reqEntity))
  {
    user = MHD_basic_auth_get_username_password (connection, &pass);
    trans->setReqEntity(trans, user);
  }

  if (0 == strcmp (method, MHD_HTTP_METHOD_POST))
  {
    return doPostProcessing(connection, url, uploadData, 
      uploadDataSize, conCls);
  }
  else if (0 == strcmp (method, MHD_HTTP_METHOD_PUT))
  {
    return doPutProcessing(connection, url, uploadData, 
      uploadDataSize, conCls);
  }
  else if (0 == strcmp (method, MHD_HTTP_METHOD_DELETE))
  {
    return doDeleteProcessing(connection, url, conCls);
  }
  else if (0 == strcmp (method, MHD_HTTP_METHOD_GET))
  {
    return doGetProcessing(connection, url, conCls);
  }
  snprintf(errMsg, MAX_BUF_SIZE_S, "method %s not allowed", method);
  snprintf(errInfo, MAX_BUF_SIZE_B, c_errorMsgTemplate, SOH_STATUS_METHOD_NOT_ALLOWED, 
    errMsg); 
  return sendStaticPage (connection, errInfo, MHD_HTTP_METHOD_NOT_ALLOWED);
}

/**
 * Use the provided function to dump statistic information.
 * @param pFunc the function to use.
 * @param issuerHandle the handler that is to be provided as first parameter of pFunc.
 */
void dumpHttpdStats(PF_DUMP_INFO_CB pFunc, void *issuerHandle)
{
pthread_mutex_lock(&httpdMutex);
  pFunc(issuerHandle, "HTTPD:\n");
  pFunc(issuerHandle, " (nbTotalRecv:%lu) (nbRecvProcessed:%lu) (recvSz:%lu) (nbPending:%d)\n",
    nbTotalRecv, nbRecvProcessed, recvSz, nrOfClients);
  pFunc(issuerHandle, " (nbTotalPostRecv:%lu) (nbTotalPutRecv:%lu) (nbTotalGetRecv:%lu) "
    "(nbTotalDeleteRecv:%d)\n", nbTotalPostRecv, nbTotalPutRecv, nbTotalGetRecv,
    nbTotalDeleteRecv);
pthread_mutex_unlock(&httpdMutex);
}


/**
 * HTTPD regular processing.
 */
void httpdClockMs()
{
  struct timeval tv;
  struct timeval *tvp;
  fd_set rs;
  fd_set ws;
  fd_set es;
  int max;
  MHD_UNSIGNED_LONG_LONG mhd_timeout;

  if (! songHttpDaemon) return;

  expireTransactions();
  max = 0;
  FD_ZERO (&rs);
  FD_ZERO (&ws);
  FD_ZERO (&es);
  if (MHD_YES != MHD_get_fdset (songHttpDaemon, &rs, &ws, &es, &max))
    return; /* fatal internal error */

  // 10ms
  mhd_timeout = 10;
  tv.tv_sec = mhd_timeout / 1000;
  tv.tv_usec = (mhd_timeout - (tv.tv_sec * 1000)) * 1000;
  tvp = &tv;

  if (-1 == select (max + 1, &rs, &ws, &es, tvp))                                               
  {
    if (EINTR != errno)
      fprintf (stderr, "Aborting due to error during select: %s\n",
        strerror (errno));
      return;
  }
  MHD_run (songHttpDaemon);
}

/**
 * Iterate on pending transactions and complete the ones that had expired.
 */
static void expireTransactions()
{
  srvTransaction_t *trans;
  int i = 0;
  time_t now;
  static time_t lastOccurence = 0;
  char errInfo[MAX_BUF_SIZE_B];

  now = rtl_tmmsmono();
  if (lastOccurence && (now - lastOccurence < 1000)) return; // don't do it to often

  lastOccurence = rtl_tmmsmono();

  while (NULL != (trans = pendingSrvTransactions->getNth(pendingSrvTransactions, i)))
  {
    if ((!trans->expired) && (now - trans->start > DEFAULT_TRANSACTION_TIMEOUT))
    {
      // transaction has expired
      RTL_TRDBG(TRACE_INFO, "expireTransactions: transaction %s had expired\n",
        trans->transId);
      trans->expired = now;
      snprintf(errInfo, MAX_BUF_SIZE_B, c_errorMsgTemplate, SOH_STATUS_GATEWAY_TIMEOUT, 
        "libSongOverHttp: issuer took too much time to respond"); 
      sendStaticPage (trans->mhdConnection, errInfo, MHD_HTTP_GATEWAY_TIMEOUT);
    }
    i++;
  }
}

/**
 * Iterate on HTTP headers received in request.
 */
static int headersIterator (void *cls, enum MHD_ValueKind kind, 
  const char *key, const char *value)
{
  srvTransaction_t *trans;

  if (MHD_HEADER_KIND != kind) return MHD_YES; // not really necessary ?!

  trans = (srvTransaction_t *)cls;
  RTL_TRDBG(TRACE_INFO, "headersIterator: new header (name:%s) (value:%s) (tid:%s)\n",
    key, value, trans->transId);

  if (!strcasecmp(key, MHD_HTTP_HEADER_AUTHORIZATION))
  {
    // this is done elsewhere, in answerToConnection
    return MHD_YES;
  }
  if (!strcasecmp(key, MHD_HTTP_HEADER_CONTENT_TYPE))
  {
    trans->setReqContentType(trans, value);
    return MHD_YES;
  }
 
  trans->addReqHeader(trans, key, value);

  return MHD_YES;
}

/**
 * Iterate on HTTP query parameters received in request.
 */
static int queryParametersIterator(void *cls, enum MHD_ValueKind kind, const char *key,
  const char *value)
{
  srvTransaction_t *trans;

  if (kind != MHD_GET_ARGUMENT_KIND) return MHD_NO;

  trans = (srvTransaction_t *)cls;
  RTL_TRDBG(TRACE_INFO, "queryParametersIterator: new parameter (name:%s) (value:%s) "
    "(tid:%s)\n", key, value, trans->transId);

  trans->addReqHeader(trans, key, value);

  return MHD_YES;
}

/**
 * Do the processing for an incoming POST.
 * @param connection the MHD connection handle.
 * @param url the request URL
 * @param uploadData the request content. Warning, depending on processing step,
 * may be NULL.
 * @param uploadDataSize the request content length.
 * @param conCls reference on the pointer on the srvTransaction_t instance,
 * NULL otherwise.
 * @return MHD_YES if things go well and processing must continue, MHD_NO otherwise.
 */
static int doPostProcessing (struct MHD_Connection *connection,
  const char *url, const char *uploadData, size_t *uploadDataSize,
  void **conCls)
{
  unsigned int nbPendingCon;
  srvTransaction_t *trans;
  char errInfo[MAX_BUF_SIZE_B];
  char errMsg[MAX_BUF_SIZE_S];

  if (*conCls == NULL)
  {
pthread_mutex_lock(&httpdMutex);
    nbTotalPostRecv++;
    nbTotalRecv++;
    nbPendingCon = nrOfClients;
pthread_mutex_unlock(&httpdMutex);
    // first pass
    if (nbPendingCon >= MAXCLIENTS)
    {
      snprintf(errMsg, MAX_BUF_SIZE_S, "Too much pending request (%u)", nbPendingCon);
      snprintf(errInfo, MAX_BUF_SIZE_B, c_errorMsgTemplate, SOH_STATUS_UNAVAILABLE,
        errMsg); 
      return sendStaticPage (connection, errInfo, MHD_HTTP_SERVICE_UNAVAILABLE);
    }

    trans = new_srvTransaction_t(connection);
    if (NULL == trans)
      return MHD_NO;

    pendingSrvTransactions->add(pendingSrvTransactions, trans);

    // arm the callbacks for getting headers and query parameters
    // both are managed as headers and raised to the issuer as "optHeaders"
    MHD_get_connection_values (connection, MHD_HEADER_KIND, headersIterator,
      trans);
    MHD_get_connection_values(connection, MHD_GET_ARGUMENT_KIND, 
      queryParametersIterator, trans);

    trans->setReqMethod(trans, SOH_CREATE);
    trans->setReqUrl(trans, url);
pthread_mutex_lock(&httpdMutex);
    nrOfClients++;
    RTL_TRDBG(TRACE_INFO, "httpd: answer_to_connection (nrOfClients:%d)\n", 
      nrOfClients);
pthread_mutex_unlock(&httpdMutex);
      
    *conCls = (void *)trans;

    return MHD_YES;
  }

  if (0 != *uploadDataSize)
  {
    // on the second callback invoke (for the same request), 
    // we can get the raw POST content
    trans = *conCls;
    if (NULL == trans)
      return MHD_NO;

    trans->setReqBody(trans, uploadData, *uploadDataSize);

    // ok, nothing else to read, set it to 0 
    *uploadDataSize = 0;
    *conCls = (void *)trans;
    return MHD_YES;
  }

  trans = *conCls;
  if (NULL == trans)
    return MHD_NO;

  // invoke the SOH call back
  g_sohCreateCb(trans->reqEntity, trans->reqUrl, trans->reqBody, trans->reqBodyLen, 
    trans->reqContentType, trans->headers, trans->transId); 

  return MHD_YES;
}


static int doPutProcessing (struct MHD_Connection *connection,
  const char *url, const char *uploadData, size_t *uploadDataSize, void **conCls)
{
  char errInfo[MAX_BUF_SIZE_B];
  snprintf(errInfo, MAX_BUF_SIZE_B, c_errorMsgTemplate, SOH_STATUS_NOT_IMPLEMENTED, 
    "PUT method not supported yet"); 
  return sendStaticPage (connection, errInfo, MHD_HTTP_NOT_IMPLEMENTED);
}

static int doGetProcessing (struct MHD_Connection *connection,
  const char *url, void **conCls)
{
  char errInfo[MAX_BUF_SIZE_B];
  snprintf(errInfo, MAX_BUF_SIZE_B, c_errorMsgTemplate, SOH_STATUS_NOT_IMPLEMENTED, 
    "GET method not supported yet"); 
  return sendStaticPage (connection, errInfo, MHD_HTTP_NOT_IMPLEMENTED);
}

static int doDeleteProcessing (struct MHD_Connection *connection,
  const char *url, void **conCls)
{
  char errInfo[MAX_BUF_SIZE_B];
  snprintf(errInfo, MAX_BUF_SIZE_B, c_errorMsgTemplate, SOH_STATUS_NOT_IMPLEMENTED, 
    "DELETE method not supported yet"); 
  return sendStaticPage (connection, errInfo, MHD_HTTP_NOT_IMPLEMENTED);
}

#if _HTTPD_TEST_

// to compile:
// gcc -D_HTTPD_TEST_ -I../.apu_build/centos6-x86_64/package_runtime/include/
//  -I../.apu_build/centos6-x86_64/package_runtime/rtbase/include/
//  -L../.apu_build/centos6-x86_64/package_runtime/lib -lmicrohttpd httpd_impl.c
//  -o httpd_impl_test.x
// to run:
// LD_LIBRARY_PATH=../.apu_build/centos6-x86_64/package_runtime/lib httpd_impl_test.x 
int TraceLevel = 0;
int TraceDebug = 0;
struct MHD_Daemon *songHttpDaemon = NULL;
srvTransactionsList_t *pendingSrvTransactions = NULL;

srvTransactionsList_t *new_srvTransactionsList_t()
{
  srvTransactionsList_t *This = malloc(sizeof(srvTransactionsList_t));
//  This->free = srvTransactionsList_t_newFree;
  return This;
}

void  rtl_tracedbg(int t,char *file,int line,char *fmt,...)
{
}

int main(int argc, char *argv[])
{
  int decTest = 500;
  int httpSrvPort = 9090;
  pendingSrvTransactions = new_srvTransactionsList_t();
  songHttpDaemon = MHD_start_daemon (0, httpSrvPort, NULL, NULL, 
    &answerToConnection, NULL, 
    MHD_OPTION_NOTIFY_COMPLETED, requestCompleted, NULL, 
    MHD_OPTION_END);

  while (1)
  {
    httpdClockMs();
  }

  MHD_stop_daemon (songHttpDaemon);

  return 0;
}
#endif

