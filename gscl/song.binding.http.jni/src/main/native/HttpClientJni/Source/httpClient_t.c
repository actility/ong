#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <curl/curl.h>
#include <poll.h>
#include <pthread.h>
#include <jni.h>

#include "rtlbase.h"

#include "common.h"
#include "header_t.h"
#include "response_t.h"
#include "transaction_t.h"
#include "transactionsList_t.h"
#include "freeTransactionsList_t.h"
#include "httpClient_t.h"

#define CURLM_SETOPT(e,p,v,l) {       \
  CURLMcode rc = curl_multi_setopt(e,p,v); \
  if (rc != CURLM_OK) \
  { \
    LOG(TRACE_ERROR, "unable to set multi handle option " \
      "(param:%s) (value:%p) (rc:%d) (msg:%s) (line:%d)", #p, v, rc, \
      curl_multi_strerror(rc), l); \
  } \
}

/**
 * Initialize the httpClient_t data structure.
 * @param This the object-like instance to destroy.
 */
static void httpClient_t_init(httpClient_t *This)
{
  LOG(TRACE_DEBUG, "httpClient_t::init (This:0x%.8x)", This);

  /* Assign the pointers on functions */
  This->createReq = httpClient_t_createReq;
  This->getTransaction = httpClient_t_getTransaction;
  This->removeReq = httpClient_t_removeReq;
  This->sendReq = httpClient_t_sendReq;
  This->invokeRespCompletion = httpClient_t_invokeRespCompletion;
  This->processMessages = httpClient_t_processMessages;
  This->trackTimedOutMessages = httpClient_t_trackTimedOutMessages;
  This->lock = httpClient_t_lock;
  This->unlock = httpClient_t_unlock;
  This->setMaxTotalSockets = httpClient_t_setMaxTotalSockets;
  This->setMaxSocketsPerHost = httpClient_t_setMaxSocketsPerHost;
  This->setProxyUserPwd = httpClient_t_setProxyUserPwd;
  This->setProxyHostPort = httpClient_t_setProxyHostPort;
  This->setTcpNoDelay = httpClient_t_setTcpNoDelay;
  This->setRequestTimeout = httpClient_t_setRequestTimeout;
  This->setConnectionTimeout = httpClient_t_setConnectionTimeout;
  This->dumpInfo = httpClient_t_dumpInfo;

  /* Initialize the members */
  This->curlDebugEnable = 0;
  This->curlMultiHandle = NULL;
  This->freeTransactions = NULL;
  This->transactions = NULL;
  This->responseCb = NULL;
  This->issuerHandle = NULL;
  This->proxyHostPort = NULL;
  This->proxyUserPwd = NULL;
  This->tcpNoDelay = 0;
  This->requestTimeout = DEFAULT_TRANSACTION_TIMEOUT;
  This->connectionTimeout = DEFAULT_TRANSACTION_CONNECT_TIMEOUT;

  pthread_mutex_init(&This->mutex, NULL);
}

/* dynamic allocation */

/**
 * Constructor.
 * @param curlDebugEnable tells if debug mode has to be enabled at curl_multi layer.
 * @param responseCb the call back the httpClient_t will invoke when the responses arrive.
 * @param issuerHandle the issuer handler.
 * @return the new create instance of httpClient_t.
 */
httpClient_t *new_httpClient_t(short curlDebugEnable, PF_HTTP_RESPONSE_CB responseCb, void *issuerHandle)
{
  httpClient_t *This = malloc(sizeof(httpClient_t));
  if (!This)
  {
    return NULL;
  }
  httpClient_t_init(This);
  This->responseCb = responseCb;
  This->issuerHandle = issuerHandle;
  This->free = httpClient_t_newFree;
  This->curlDebugEnable = curlDebugEnable;
  This->curlMultiHandle = curl_multi_init();

  // max amount of connections hold in connection pool cache
  CURLM_SETOPT(This->curlMultiHandle, CURLMOPT_MAX_HOST_CONNECTIONS, DEFAULT_HTTPCLIENT_MAX_CONNECTS, __LINE__);
  // max amount of simultaneously open connections in total
  CURLM_SETOPT(This->curlMultiHandle, CURLMOPT_MAX_TOTAL_CONNECTIONS, DEFAULT_HTTPCLIENT_MAX_CONNECTS, __LINE__);
  CURLM_SETOPT(This->curlMultiHandle, CURLMOPT_PIPELINING, DEFAULT_HTTPCLIENT_PIPELINE, __LINE__);

  This->transactions = new_transactionsList_t();
  This->freeTransactions = new_freeTransactionsList_t(This, DEFAULT_HTTPCLIENT_MAX_CONNECTS);
  LOG(TRACE_DEBUG, "new httpClient_t (This:0x%.8x)", This);

  return This;
}

/**
 * Destructor
 * @param This the object-like instance to destroy.
 */
void httpClient_t_newFree(httpClient_t *This)
{
  LOG(TRACE_DEBUG, "httpClient_t::newFree (This:0x%.8x)", This);

  This->freeTransactions->free(This->freeTransactions);
  This->transactions->free(This->transactions);
  curl_multi_cleanup(This->curlMultiHandle);

  if (This->proxyHostPort)
  {
    free(This->proxyHostPort);
  }
  if (This->proxyUserPwd)
  {
    free(This->proxyUserPwd);
  }
  free(This);
}

/**
 * Set the max total number of sockets allowed in cache.
 * @param This the object-like instance.
 * @param maxTotalSockets the number to set.
 */
void httpClient_t_setMaxTotalSockets(httpClient_t *This, int maxTotalSockets)
{
  LOG(TRACE_INFO, ">>> httpClient_t::setMaxTotalSockets (This:0x%.8x) "
      "(maxTotalSockets:%d)", This, maxTotalSockets);
  This->lock(This);
  CURLM_SETOPT(This->curlMultiHandle, CURLMOPT_MAX_TOTAL_CONNECTIONS, maxTotalSockets, __LINE__);
  This->freeTransactions->free(This->freeTransactions);
  This->freeTransactions = new_freeTransactionsList_t(This, maxTotalSockets);
  This->unlock(This);
}

/**
 * Set the max number of sockets allowed in cache per host.
 * @param This the object-like instance.
 * @param maxSocketsPerHost the number to set.
 */
void httpClient_t_setMaxSocketsPerHost(httpClient_t *This, int maxSocketsPerHost)
{
  LOG(TRACE_INFO, ">>> httpClient_t::setMaxSocketsPerHost (This:0x%.8x) "
      "(maxSocketsPerHost:%d)", This, maxSocketsPerHost);
  This->lock(This);
  CURLM_SETOPT(This->curlMultiHandle, CURLMOPT_MAX_HOST_CONNECTIONS, maxSocketsPerHost, __LINE__);
  This->unlock(This);
}

/**
 * Set the proxy credentials.
 * @param This the object-like instance.
 * @param proxyUserPwd the credentials to set.
 */
void httpClient_t_setProxyUserPwd(httpClient_t *This, char *proxyUserPwd)
{
  LOG(TRACE_INFO, ">>> httpClient_t::setProxyUserPwd (This:0x%.8x) "
      "(proxyUserPwd:%s)", This, proxyUserPwd);
  This->lock(This);
  if (NULL != This->proxyUserPwd)
  {
    free(This->proxyUserPwd);
  }
  This->proxyUserPwd = strdup(proxyUserPwd);
  This->unlock(This);
}

/**
 * Set the proxy address.
 * @param This the object-like instance.
 * @param proxyHostPort the address to set.
 */
void httpClient_t_setProxyHostPort(httpClient_t *This, char *proxyHostPort)
{
  LOG(TRACE_INFO, ">>> httpClient_t::setProxyHostPort (This:0x%.8x) "
      "(proxyHostPort:%s)", This, proxyHostPort);
  This->lock(This);
  if (NULL != This->proxyHostPort)
  {
    free(This->proxyHostPort);
  }
  This->proxyHostPort = strdup(proxyHostPort);
  This->unlock(This);
}

/**
 * Set the TCP_NODELAY flag.
 * @param This the object-like instance.
 * @param tcpNoDelay the flag value (may be 0 or 1).
 */
void httpClient_t_setTcpNoDelay(httpClient_t *This, int tcpNoDelay)
{
  LOG(TRACE_INFO, ">>> httpClient_t::setTcpNoDelay (This:0x%.8x) "
      "(tcpNoDelay:%d)", This, tcpNoDelay);
  This->lock(This);
  This->tcpNoDelay = tcpNoDelay;
  This->unlock(This);
}

/**
 * Set the request timeout value.
 * @param This the object-like instance.
 * @param requestTimeout the request timeout value to set.
 */
void httpClient_t_setRequestTimeout(httpClient_t *This, long requestTimeout)
{
  LOG(TRACE_INFO, ">>> httpClient_t::setRequestTimeout (This:0x%.8x) "
      "(requestTimeout:%ld)", This, requestTimeout);
  This->lock(This);
  This->requestTimeout = requestTimeout;
  This->unlock(This);
}

/**
 * Set the connection timeout value.
 * @param This the object-like instance.
 * @param connectionTimeout the connection timeout value to set.
 */
void httpClient_t_setConnectionTimeout(httpClient_t *This, long connectionTimeout)
{
  LOG(TRACE_INFO, ">>> httpClient_t::setConnectionTimeout (This:0x%.8x) "
      "(connectionTimeout:%ld)", This, connectionTimeout);
  This->lock(This);
  This->connectionTimeout = connectionTimeout;
  This->unlock(This);
}

/**
 * Get a free transaction for the new request.
 * @param This the object-like instance to destroy.
 * @param transId the identifier of the transaction.
 */
void httpClient_t_createReq(httpClient_t *This, char *transId)
{
  transaction_t *trans;
  LOG(TRACE_INFO, ">>> httpClient_t::createReq (This:0x%.8x) (transId:%s)", This, transId);

  This->lock(This);
  trans = This->freeTransactions->recycle(This->freeTransactions);
  trans->setTransId(trans, transId);
  This->transactions->add(This->transactions, trans);
  This->unlock(This);
}

/**
 * Get the transaction_t instance by its transaction identifier.
 * @param This the object-like instance to destroy.
 * @param transId the identifier of the transaction to retrieve.
 * @return the transaction_t instance if found, NULL otherwise.
 */
transaction_t *httpClient_t_getTransaction(httpClient_t *This, char *transId)
{
  transaction_t *res;
  This->lock(This);
  res = This->transactions->get(This->transactions, transId);
  This->unlock(This);
  return res;
}

/**
 * Remove and recycle the transaction_t instance from the pending transactions.
 * @param This the object-like instance to destroy.
 * @param transId the identifier of the transaction to retrieve.
 */
void httpClient_t_removeReq(httpClient_t *This, char *transId)
{
  transaction_t *trans;
  CURLMcode ret;
  LOG(TRACE_INFO, ">>> httpClient_t::removeReq (This:0x%.8x) (transId:%s)", This, transId);
  This->lock(This);
  trans = This->transactions->remove(This->transactions, transId);
  if (trans)
  {
    ret = curl_multi_remove_handle(This->curlMultiHandle, trans->curlEasyHandle);
    if (ret != CURLM_OK)
    {
      LOG(TRACE_ERROR, "httpClient_t::removeReq: error - curl_multi_remove_handle "
          "not OK (This:0x%.8x) (ret:%d) (msg:%s)", This, ret, curl_easy_strerror(ret));
    }
    This->freeTransactions->add(This->freeTransactions, trans);
  }
  This->unlock(This);
}

/**
 * Add the CURL easy handle to the client multi handle, that will cause the curl layer
 * to send the request.
 * @param This the object-like instance to consider for this action.
 * @param transId the identifier of the transaction to send.
 * @param eh the easy handle for the transaction to send.
 * @return 1 if the request can be sent, 0 otherwise.
 */
int httpClient_t_sendReq(httpClient_t *This, char *transId, CURL *eh)
{
  CURLMcode ret;
//  int stillRunning;
  int res = 1;

  // add easyHandle to multiHandle so that the request will be sent
  This->lock(This);
  ret = curl_multi_add_handle(This->curlMultiHandle, eh);
  This->unlock(This);
  if (ret != CURLM_OK)
  {
    LOG(TRACE_ERROR, "httpClient_t::sendReq: error - failed to send request "
        "(This:0x%.8x) (transId:%s) (ret:%d) (msg:%s)", This, transId, ret, curl_easy_strerror(ret));
    res = 0;
  }

//  // we start some action by calling curl_multi_perform right away
//  This->lock(This);
//  ret = curl_multi_perform(This->curlMultiHandle, &stillRunning);
//  This->unlock(This);
//  if (ret != CURLM_OK)
//  {
//    LOG(TRACE_ERROR, "transaction_t::sendReq: error - curl_multi_perform not OK "
//      "(This:0x%.8x) (transId:%s) (ret:%d) (msg:%s) (stillRunning:%d)", This, transId, 
//      ret, curl_easy_strerror(ret), stillRunning);
//  }

  return res;
}

/**
 * Invoke the responseCb call back in order to notify the issuer of the end of the session.
 * @param This the object-like instance.
 * @param result the request result at the cURL level.
 * @param easyHandle the "easy" handle of libCurl for this 
 */
void httpClient_t_invokeRespCompletion(httpClient_t *This, CURLcode result, CURL *easyHandle)
{
  PF_HTTP_RESPONSE_CB pFunc = NULL;
  char *p = NULL;
  transaction_t *t = NULL;

  This->lock(This);
  curl_easy_getinfo(easyHandle, CURLINFO_PRIVATE, &p);
  t = (transaction_t *) p;
  This->unlock(This);

  if (t)
  {

    if (!t->response)
    {
      t->response = new_response_t();
      t->response->statusCode = result;
      t->response->reasonPhrase = strdup(curl_easy_strerror(result));
    }

    LOG(TRACE_INFO, "<<< httpClient_t::invokeRespCompletion (This:0x%.8x) (transId:%s) "
        "(statusCode:%d) (reasonPhrase:%s)", This, t->transId, t->response->statusCode, t->response->reasonPhrase);

    if (This->responseCb)
    {
      pFunc = (PF_HTTP_RESPONSE_CB) (This->responseCb);
      pFunc(This->issuerHandle, t);
    }
    else
    {
      LOG(TRACE_ERROR, "<<< httpClient_t::invokeRespCompletion: error - no call back "
          "(This:0x%.8x) (transId:%s) (statusCode:%d) (reasonPhrase:%s)", This, t->transId, t->response->statusCode,
          t->response->reasonPhrase);
    }
  }
}

/**
 * Add lock protection on client. 
 * @param This the object-like instance.
 */
void httpClient_t_lock(httpClient_t *This)
{
  pthread_mutex_lock(&This->mutex);
}

/**
 * Remove lock protection on client. 
 * @param This the object-like instance.
 */
void httpClient_t_unlock(httpClient_t *This)
{
  pthread_mutex_unlock(&This->mutex);
}

/**
 * Look for request and response to perform. 
 * @param This the object-like instance to destroy.
 */
void httpClient_t_processMessages(httpClient_t *This)
{
  CURLMsg *msg;
  CURL *easy;
  CURLcode res;
  CURLMcode ret;
  int msgsLeft;
  int cycles = 1;

  do
  {
    struct timeval timeout;
    int rc; /* select() return code */

    fd_set fdread;
    fd_set fdwrite;
    fd_set fdexcep;
    int maxfd = -1;

    FD_ZERO(&fdread);
    FD_ZERO(&fdwrite);
    FD_ZERO(&fdexcep);

    /* set a suitable timeout to play around with */
    timeout.tv_sec = 0;
    timeout.tv_usec = 10000;

    /* get file descriptors from the transfers */
    This->lock(This);
    ret = curl_multi_fdset(This->curlMultiHandle, &fdread, &fdwrite, &fdexcep, &maxfd);
    This->unlock(This);
    if (ret != CURLM_OK)
    {
      LOG(TRACE_ERROR, "httpClient_t::processMessages: error - curl_multi_fdset "
          "not OK (This:0x%.8x) (ret:%d) (msg:%s)", This, ret, curl_easy_strerror(ret));
    }
    else
    {
      /* 
       * On success, the value of maxfd is guaranteed to be
       * greater or equal than -1.  We call select(maxfd + 1, ...), specially in
       * case of (maxfd == -1), we call select(0, ...), which is basically
       * equal to sleep. 
       */

      if (maxfd >= -1)
      {
        rc = select(maxfd + 1, &fdread, &fdwrite, &fdexcep, &timeout);

        switch (rc)
        {
        case -1:
          /* select error */
          break;
        case 0: /* timeout */
        default: /* action */
          This->lock(This);
          ret = curl_multi_perform(This->curlMultiHandle, &msgsLeft);
          This->unlock(This);
          if (ret != CURLM_OK)
          {
            LOG(TRACE_ERROR, "httpClient_t::processMessages: error - curl_multi_perform "
                "not OK (This:0x%.8x) (ret:%d) (msg:%s) (msgsLeft:%d)", This, ret, curl_easy_strerror(ret), msgsLeft);
          }
          break;
        }
      }
      else
      {
        //break;
      }
    }
    cycles--;
  } while ((msgsLeft) && (cycles > 0));

  do
  {
    This->lock(This);
    msg = curl_multi_info_read(This->curlMultiHandle, &msgsLeft);
    This->unlock(This);
    if (msg)
    {
      LOG(TRACE_DEBUG, "httpClient_t::processMessages - new message read (This:%0.8p)"
          " (msgsLeft:%d)", This, msgsLeft);
      if (CURLMSG_DONE == msg->msg)
      {
        easy = msg->easy_handle;
        res = msg->data.result;
        This->invokeRespCompletion(This, res, easy);
      }
    }
  } while (msg);
}

/**
 * Look for requests that have timed out and that libcurl hasn't detected as so.
 * @param This the object-like instance to destroy.
 */
void httpClient_t_trackTimedOutMessages(httpClient_t *This)
{
  int index = 0;
  transaction_t *trans;
  double duration;

  This->lock(This);
  while ((trans = This->transactions->getNth(This->transactions, index)))
  {
    curl_easy_getinfo(trans->curlEasyHandle, CURLINFO_TOTAL_TIME, &duration);
    if ((duration * 1000) > (This->requestTimeout + 2))
    {
      LOG(TRACE_ERROR, "trackTimedOutMessages: timed out transaction found (%s)", trans->transId);
      This->unlock(This);
      This->invokeRespCompletion(This, CURLE_OPERATION_TIMEDOUT, trans->curlEasyHandle);
      This->lock(This);
    }
    index++;
  }
  This->unlock(This);
}

/**
 * Dump the HTTP client status, including for all pending sessions.
 * @param This the object-like instance.
 * @param pFunc a pointer on the function to invoke for dumping the information.
 * @param issuerHandle the handle to provide while invoking pFunc.
 */
void httpClient_t_dumpInfo(httpClient_t *This, PF_DUMP_INFO_CB pFunc, void *issuerHandle)
{
  transaction_t * trans;
  char szTemp[MAX_BUF_SIZE_H];
  int i = 0;
  This->lock(This);
  while ((trans = This->transactions->getNth(This->transactions, i)))
  {
    pFunc(issuerHandle, "#%d: %s\n", i, trans->dumpInfo(trans, szTemp));
    i++;
  }
  This->unlock(This);
  pFunc(issuerHandle, "%d transactions found (client:0x%.8x)\n\n", i, This);
}

