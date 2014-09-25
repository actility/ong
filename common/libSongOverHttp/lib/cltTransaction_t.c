
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <curl/curl.h>
#include <errno.h>
#include <poll.h>
#include <ctype.h>

#include "rtllist.h"
#include "rtlbase.h"

#include "common.h"
#include "song_over_http_api.h"
#include "header_t.h"
#include "response_t.h"
#include "cltTransaction_t.h"
#include "cltTransactionsList_t.h"
#include "freeCltTransactionsList_t.h"
#include "httpClient_t.h"

#define CURLE_SETOPT(e,p,v,l) {       \
  CURLcode rc = curl_easy_setopt(e,p,v); \
  if (rc != CURLE_OK) \
  { \
    RTL_TRDBG(TRACE_ERROR, "unable to set easy handle option " \
      "(param:%s) (value:%p) (rc:%d) (msg:%s) (line:%d)\n", #p, v, rc, \
      curl_easy_strerror(rc), l); \
  } \
}

#define CLT_TRANS_ID_PREFIX "CLT_TID_"
static unsigned long g_cltTransId = 0; 

static void cltTransaction_t_init(cltTransaction_t *This);

static void s_curl_prepareEasyHandle(cltTransaction_t *This);
//static int s_curl_progressCb(void *p, double dltotal, double dlnow, double ultotal, 
//  double ulnow);
static size_t s_curl_sendReqBodyCb(void *ptr, size_t size, size_t nmemb, void *userdata);

static size_t s_curl_receivedBodyCb(char *ptr, size_t size, size_t nmemb, void *userp);
static size_t s_curl_receivedHeaderCb(char *ptr, size_t size, size_t nmemb, void *userp);

static int s_curl_debugCb(CURL *eh, curl_infotype type, char *msg, size_t msgLen, void *userP);

static int urlencode(char *url, char *encUrl);

/**
 * Initialize the data structure.
 * @param This the object-like instance.
 */
static void cltTransaction_t_init(cltTransaction_t *This)
{
  RTL_TRDBG(TRACE_DEBUG, "cltTransaction_t::init (This:0x%.8x)\n", This);

  /* Assign the pointers on functions */
  This->setReqUrl = cltTransaction_t_setReqUrl;
  This->addReqHeader = cltTransaction_t_addReqHeader;
  This->setCompletionParam = cltTransaction_t_setCompletionParam;
  This->setReqMethod = cltTransaction_t_setReqMethod;
  This->setReqBody = cltTransaction_t_setReqBody;
  This->setReqProxy = cltTransaction_t_setReqProxy;
  This->setReqRequestingEntity = cltTransaction_t_setReqRequestingEntity;
  This->sendReq = cltTransaction_t_sendReq;
  This->invokeRespCompletion = cltTransaction_t_invokeRespCompletion;
  This->dumpInfo = cltTransaction_t_dumpInfo;

  /* Initialize the members */
  memset(This->transId, 0, MAX_BUF_SIZE_XS);
  sprintf(This->transId, "%s%012lu", CLT_TRANS_ID_PREFIX, g_cltTransId++);
  This->reqBody = NULL;
  This->reqBodyLen = 0;
  This->readReqBodyLen = 0;
  This->contenLenHeaderAdded = 0;
  This->response = NULL;
  This->reqHeaderlist = NULL;
  This->issuerData = NULL;
  This->method = SOH_UNKNOWN;
  memset(&This->callbacks, 0, sizeof(This->callbacks));
}

/* dynamic allocation */

/**
 * Constructor.
 * @param client the httpClient_t instance the created cltTransaction_t is attached to. 
 * @return the built instance of cltTransaction_t.
 */
cltTransaction_t *new_cltTransaction_t(httpClient_t *client)
{
  cltTransaction_t *This = malloc(sizeof(cltTransaction_t));
  if (!This)
  {
    return NULL;
  }
  cltTransaction_t_init(This);
  This->free = cltTransaction_t_newFree;
  This->reset = cltTransaction_t_reset;
  This->client = client;
  This->curlEasyHandle = curl_easy_init();

  s_curl_prepareEasyHandle(This);

  RTL_TRDBG(TRACE_DEBUG, "new cltTransaction_t (transId:%s) (This:0x%.8x) (easyHandle:0x%.8x)\n", 
    This->transId, This, This->curlEasyHandle);
  return This;
}

/**
 * Destructor.
 * @param This the object-like instance.
 */
void cltTransaction_t_newFree(cltTransaction_t *This)
{
  RTL_TRDBG(TRACE_DEBUG, "cltTransaction_t::newFree (This:0x%.8x) (transId:%s)\n", This, 
    This->transId);

  if (This->reqHeaderlist)
  {
    curl_slist_free_all(This->reqHeaderlist);
  }
  curl_easy_cleanup(This->curlEasyHandle);

  if (This->reqBody)
  {
    free(This->reqBody);
  }
  if (This->response)
  {
    This->response->free(This->response);
  }
  free(This);
}

/**
 * Reset, ready to reuse.
 * @param This the object-like instance.
 */
void cltTransaction_t_reset(cltTransaction_t *This)
{
  RTL_TRDBG(TRACE_DEBUG, "cltTransaction_t::reset (This:0x%.8x)\n", This);

  if (This->reqHeaderlist)
  {
    curl_slist_free_all(This->reqHeaderlist);
  }
  if (This->reqBody)
  {
    free(This->reqBody);
  }
  if (This->response)
  {
    This->response->free(This->response);
  }
  cltTransaction_t_init(This);

  curl_easy_reset(This->curlEasyHandle);

  s_curl_prepareEasyHandle(This);
}

/**
 * Set the request URL parameter.
 * @param This the object-like instance.
 */
void cltTransaction_t_setReqUrl(cltTransaction_t *This, char *url)
{
  RTL_TRDBG(TRACE_DEBUG, "cltTransaction_t::setReqUrl (This:0x%.8x) (transId:%s) (url:%s)\n", 
    This, This->transId, url);
  CURLE_SETOPT(This->curlEasyHandle, CURLOPT_URL, url, __LINE__);
}

/**
 * Set a new header value to the request.
 * @param This the object-like instance.
 * @param name the header name.
 * @param value the header value.
 */
void cltTransaction_t_addReqHeader(cltTransaction_t *This, char *name, char *value)
{
  RTL_TRDBG(TRACE_DEBUG, "cltTransaction_t::addReqHeader (This:0x%.8x) (transId:%s) (name:%s) "
    "(value:%s)\n", This, This->transId, name, value);
  if ((!name) || (!*name))
  {
    RTL_TRDBG(TRACE_ERROR, "cltTransaction_t::addHeader: error - no name provided"
      " (transId:%s)\n", This->transId);
    return;
  }
  if (!value)
  {
    RTL_TRDBG(TRACE_ERROR, "cltTransaction_t::addHeader: error - no value provided"
      " (transId:%s)\n", This->transId);
    return;
  }

  if (!strcasecmp(name, HEADER_CONTENT_LENGTH))
  {
    This->contenLenHeaderAdded = 1;
  }

  int len = strlen(name) + strlen(value) + 2 /* ": " */ + 1;
  char *header = (char *)malloc(len);
  memset(header, 0, len);
  sprintf(header, "%s: %s", name, value);
  This->reqHeaderlist = curl_slist_append(This->reqHeaderlist, header);
  free(header);
}

/**
 * Set the completion parameters.
 * @param This the object-like instance.
 * @param callback the pointer on the callback method to invoke on request completion.
 * @param issuerData the issuer handle to provide at completion call.
 */
void cltTransaction_t_setCompletionParam(cltTransaction_t *This, void *callback, void *issuerData)
{
  This->callbacks.createRespCb = callback; // they all have the same size.
  This->issuerData = issuerData;
}

/**
 * Set the request method.
 * @param This the object-like instance.
 * @param method the SONG method, can be CREATE (POST), RETRIEVE (GET), UPDATE (PUT) or 
 * DELETE (DELETE).
 */
void cltTransaction_t_setReqMethod(cltTransaction_t *This, SOH_METHOD method)
{
  RTL_TRDBG(TRACE_DEBUG, "cltTransaction_t::setReqMethod (This:0x%.8x) (transId:%s) (method:%d)\n",
    This, This->transId, method);

  This->method = method;
  switch (method)
  {
    case SOH_CREATE:
    {
      CURLE_SETOPT(This->curlEasyHandle, CURLOPT_POST, 1L, __LINE__);
      This->addReqHeader(This, HEADER_EXPECT, "");
      This->addReqHeader(This, HEADER_TRANSFER_ENCODING, "");
      This->addReqHeader(This, HEADER_CONTENT_TYPE, "");
    }
    break;

    case SOH_UPDATE:
    {
      CURLE_SETOPT(This->curlEasyHandle, CURLOPT_PUT, 1L, __LINE__);
      This->addReqHeader(This, HEADER_EXPECT, "");
      This->addReqHeader(This, HEADER_TRANSFER_ENCODING, "");
    }
    break;
  
    case SOH_DELETE:
    {
      CURLE_SETOPT(This->curlEasyHandle, CURLOPT_CUSTOMREQUEST, METHOD_DELETE, __LINE__);
    }
    break;

    case SOH_RETRIEVE:
    default:
      // nothing to do
      break;
  }
}

/**
 * Set the request content.
 * @param This the object-like instance.
 * @param body the request content to set.
 * @param len the content length.
 */
void cltTransaction_t_setReqBody(cltTransaction_t *This, unsigned char *body, size_t len)
{
  RTL_TRDBG(TRACE_DEBUG, "cltTransaction_t::setReqBody (This:0x%.8x) (transId:%s) (len:%d)\n",
    This, This->transId, len);
  if (This->reqBody)
  {
    free(This->reqBody);
    This->reqBody = NULL;
  }
  if (len && body && *body)
  {
    This->reqBody = malloc(len * sizeof(unsigned char));
    memcpy(This->reqBody, body, len * sizeof(unsigned char));
    This->reqBodyLen = len;
  }
}

/**
 * Set a proxy for the request.
 * @param This the object-like instance.
 * @param proxy the proxy host and port to set (must be formatted as "<host>:<port>")
 */
void cltTransaction_t_setReqProxy(cltTransaction_t *This, char *proxy)
{
  RTL_TRDBG(TRACE_DEBUG, "cltTransaction_t::setReqProxy (This:0x%.8x) (transId:%s) (proxy:%s)\n",
    This, This->transId, proxy);
  CURLE_SETOPT(This->curlEasyHandle, CURLOPT_PROXY, proxy, __LINE__);
//  CURLE_SETOPT(This->curlEasyHandle, CURLOPT_HTTPPROXYTUNNEL, 1L, __LINE__);
}

/**
 * Set the requesting entity.
 * @param This the object-like instance.
 * @param reqEntity the requesting entity
 */
void cltTransaction_t_setReqRequestingEntity(cltTransaction_t *This, char *reqEntity)
{
  char reqEntityEnc[MAX_BUF_SIZE_B];
  char authHeaderValue[MAX_BUF_SIZE_B];
 
  memset(reqEntityEnc, 0, MAX_BUF_SIZE_B);
  memset(authHeaderValue, 0, MAX_BUF_SIZE_B);
  urlencode(reqEntity, reqEntityEnc);
  reqEntityEnc[strlen(reqEntityEnc)] = ':';

  sprintf(authHeaderValue, "Basic ");
  rtl_base64_encode(authHeaderValue + 6 /* "Basic " */, reqEntityEnc, strlen(reqEntityEnc));

  This->addReqHeader(This, HEADER_AUTHORIZATION, authHeaderValue);
}

/**
 * Send the request.
 * @param This the object-like instance.
 * @return 1 if the request can be sent, 0 otherwise.
 */
int cltTransaction_t_sendReq(cltTransaction_t *This)
{
  char szContentLen[MAX_BUF_SIZE_S];
 
  RTL_TRDBG(TRACE_INFO, ">>> cltTransaction_t::sendReq (This:0x%.8x) (transId:%s)\n", 
    This, This->transId);

  // Add Content-Length header if not already done
  if (! This->contenLenHeaderAdded)
  {
    sprintf(szContentLen, "%zd", This->reqBodyLen);
    This->addReqHeader(This, HEADER_CONTENT_LENGTH, szContentLen);
  }

  // add collected headers
  CURLE_SETOPT(This->curlEasyHandle, CURLOPT_HTTPHEADER, This->reqHeaderlist, __LINE__);

  return This->client->sendReq(This->client, This->transId, This->curlEasyHandle);

}

/**
 * Invoke the response completion method.
 * @param This the object-like instance.
 */
void cltTransaction_t_invokeRespCompletion(cltTransaction_t *This)
{
  header_t *header = NULL;
  char *resourceUri = NULL;
  char *contentType = NULL;
  sohHeader_t **optHeaders = NULL;

  if (!This->callbacks.createRespCb) return;
  if (!This->response) return; 

  switch (This->method)
  {
    case SOH_CREATE:
    {
      // get the resource URI
      if (NULL != (header = This->response->getHeader(This->response, HEADER_CONTENT_LOCATION)))
      {
        resourceUri = header->values; 
      }
    
      // get the content type
      if (NULL != (header = This->response->getHeader(This->response, HEADER_CONTENT_TYPE)))
      {
        contentType = header->values; 
      }

      This->callbacks.createRespCb(This->response->statusCode, resourceUri, 
        This->response->content, This->response->contentLength, contentType,
        optHeaders, This->transId, This->issuerData);
    }
    break;

    case SOH_UPDATE:
    {
      // get the content type
      if (NULL != (header = This->response->getHeader(This->response, HEADER_CONTENT_TYPE)))
      {
        contentType = header->values; 
      }
      This->callbacks.updateRespCb(This->response->statusCode, 
        This->response->content, This->response->contentLength, contentType,
        optHeaders, This->transId, This->issuerData);
    }
    break;
  
    case SOH_DELETE:
    {
      // get the content type
      if (NULL != (header = This->response->getHeader(This->response, HEADER_CONTENT_TYPE)))
      {
        contentType = header->values; 
      }
      This->callbacks.deleteRespCb(This->response->statusCode,
        This->response->content, This->response->contentLength, contentType,
        optHeaders, This->transId, This->issuerData);
    }
    break;

    case SOH_RETRIEVE:
    {
      // get the content type
      if (NULL != (header = This->response->getHeader(This->response, HEADER_CONTENT_TYPE)))
      {
        resourceUri = header->values; 
      }
      This->callbacks.retrieveRespCb(This->response->statusCode, 
        This->response->content, This->response->contentLength, contentType,
        optHeaders, This->transId, This->issuerData);
    }
    break;

    default:
      // nothing to do
      break;
  }
}

/**
 * Used to display information regarding the pending sessions.
 * @param This the object-like instance.
 * @param dest the buffer in which dumping the built information.
 * @return dest.
 */
char *cltTransaction_t_dumpInfo(cltTransaction_t *This, char *dest)
{
  char *urlInfo = NULL;
  double duration;
  curl_easy_getinfo(This->curlEasyHandle, CURLINFO_EFFECTIVE_URL, &urlInfo);
  curl_easy_getinfo(This->curlEasyHandle, CURLINFO_TOTAL_TIME, &duration);
  sprintf(dest, "(transId:%s)(url:%s)(duration:%gs)", This->transId, urlInfo, 
    duration);
  return dest; 
}

///////////////////////////////////////////////////////////////////////////////
// cURL init and call back methods

/**
 * Init the cURL library easy handle.
 * @param This the object-like instance.
 */
static void s_curl_prepareEasyHandle(cltTransaction_t *This)
{
  if (This->client->curlDebugEnable)
  {
    CURLE_SETOPT(This->curlEasyHandle, CURLOPT_VERBOSE, 1L, __LINE__);
    CURLE_SETOPT(This->curlEasyHandle, CURLOPT_DEBUGFUNCTION, s_curl_debugCb, __LINE__);
    CURLE_SETOPT(This->curlEasyHandle, CURLOPT_DEBUGDATA, This, __LINE__);
  }

  /* to retrieve PRIVATE DATA later on checkConnection */
  CURLE_SETOPT(This->curlEasyHandle, CURLOPT_PRIVATE, This, __LINE__);
  /* read/write callback */
  CURLE_SETOPT(This->curlEasyHandle, CURLOPT_HEADER, 0, __LINE__);
  CURLE_SETOPT(This->curlEasyHandle, CURLOPT_WRITEFUNCTION, s_curl_receivedBodyCb, __LINE__);
  CURLE_SETOPT(This->curlEasyHandle, CURLOPT_WRITEDATA, This, __LINE__);
  CURLE_SETOPT(This->curlEasyHandle, CURLOPT_HEADERFUNCTION, s_curl_receivedHeaderCb, __LINE__);
  CURLE_SETOPT(This->curlEasyHandle, CURLOPT_HEADERDATA, This, __LINE__);
  CURLE_SETOPT(This->curlEasyHandle, CURLOPT_READFUNCTION, s_curl_sendReqBodyCb, __LINE__);
  CURLE_SETOPT(This->curlEasyHandle, CURLOPT_READDATA, This, __LINE__);
  /* progress callback */
  //CURLE_SETOPT(This->curlEasyHandle, CURLOPT_PROGRESSFUNCTION, s_curl_progressCb, __LINE__);
  //CURLE_SETOPT(This->curlEasyHandle, CURLOPT_PROGRESSDATA, This, __LINE__);
  //CURLE_SETOPT(This->curlEasyHandle, CURLOPT_NOPROGRESS, 0L, __LINE__);
  CURLE_SETOPT(This->curlEasyHandle, CURLOPT_NOPROGRESS, 1L, __LINE__);
  /* Timeout */
  CURLE_SETOPT(This->curlEasyHandle, CURLOPT_TIMEOUT_MS, This->client->requestTimeout, __LINE__);
  CURLE_SETOPT(This->curlEasyHandle, CURLOPT_CONNECTTIMEOUT_MS, This->client->connectionTimeout, 
    __LINE__);

  CURLE_SETOPT(This->curlEasyHandle, CURLOPT_FAILONERROR, 0, __LINE__);
  //CURLE_SETOPT(This->curlEasyHandle, CURLOPT_LOW_SPEED_TIME, 1, __LINE__);
  CURLE_SETOPT(This->curlEasyHandle, CURLOPT_NOSIGNAL, 1, __LINE__);

  if (This->client->tcpNoDelay)
  {
    CURLE_SETOPT(This->curlEasyHandle, CURLOPT_TCP_NODELAY, 1L, __LINE__);
  }
  else
  {
    CURLE_SETOPT(This->curlEasyHandle, CURLOPT_TCP_NODELAY, 0L, __LINE__);
  }

  if (This->client->proxyHostPort)
  {
    This->setReqProxy(This, This->client->proxyHostPort);
  }
  if (This->client->proxyUserPwd)
  {
    CURLE_SETOPT(This->curlEasyHandle, CURLOPT_PROXYUSERPWD, This->client->proxyUserPwd, 
      __LINE__);
  }
}

/**
 * Call back invoke approximatively every seconds during data transfer, to notify the app of
 * the progression.
 * @param p the issuer handle.
 * @param dltotal
 * @param dlnow
 * @param ultotal
 * @param ulnow
 * @return ?
 */
//static int s_curl_progressCb(void *p, double dltotal, double dlnow, double ultotal,
//  double ulnow)
//{
//  RTL_TRDBG(TRACE_DEBUG, "s_curl_progressCb\n");
//  return 0;
//}

/**
 * The call back is invoked when the content is received, if there is one.
 * @param ptr the content.
 * @param size the encoding size of each character.
 * @param nmemb the number of characters of the content.
 * @param userp A pointer to the cltTransaction_t associated to this session.
 * @return must return the number of read bytes, i.e. size * nmemb
 */
static size_t s_curl_receivedBodyCb(char *ptr, size_t size, size_t nmemb, void *userp)
{
  cltTransaction_t *This = (cltTransaction_t*)userp;

  RTL_TRDBG(TRACE_DEBUG, "s_curl_receivedBodyCb (transId:%s) (ptr:%0.8p) (size:%d) "
    "(nmemb:%d)\n", This?This->transId:"(nil)", ptr, size, nmemb);

  if (This->response)
  {
    // collect the response content
    if (nmemb * size > 0)
    {
      This->response->addToContent(This->response, (unsigned char *)ptr, nmemb * size);
    }

    return nmemb * size;
  }
  return 0;
}

/**
 * The call back is invoked for each received header, even the last blank line.
 * @param ptr the header line, normally terminated by "\r\n".
 * @param size the encoding size of each character.
 * @param nmemb the number of characters on the header line.
 * @param userp A pointer to the cltTransaction_t associated to this session.
 * @return must return the number of read bytes, i.e. size * nmemb
 */
static size_t s_curl_receivedHeaderCb(char *ptr, size_t size, size_t nmemb, void *userp)
{
  cltTransaction_t *This = (cltTransaction_t*)userp;
  int rc;
  //header_t * h;
  char szTemp[255];

  RTL_TRDBG(TRACE_DEBUG, "s_curl_receivedHeaderCb (transId:%s) (ptr:%0.8p) (size:%d)"
    " (nmemb:%d)\n", This?This->transId:"(nil)", ptr, size, nmemb);

  if (! This) return 0;

  if (NULL == This->response)
  {
    This->response = new_response_t();
    rc = This->response->parseStatusLine(This->response, ptr, size * nmemb);

    if (! rc)
    {
      This->response->statusCode = STATUS_CODE_PARSE_ERR;
      snprintf(szTemp, sizeof(szTemp), "Unable to parse the read status line (%s)", ptr); 
      This->response->reasonPhrase = strdup(szTemp);
    }
  }
  else if ((0 == strcmp(ptr, "\n")) || (0 == strcmp(ptr, "\r\n")))
  {
    // end of headers : nothing to do
  }
  else
  {
    This->response->parseHeader(This->response, ptr);
  }

  return nmemb * size;
}

/**
 * The call back is invoked when libcurl is encoding the outgoing request.
 * @param ptr the place where filling in the request content.
 * @param size the size of characters written in ptr.
 * @param nmemb the number of characters that are available in ptr for writing.
 * @param userdata A pointer to the cltTransaction_t associated to this session.
 * @return must return the number of sent bytes (which must be lesser than size * nmemb)
 */
static size_t s_curl_sendReqBodyCb(void *ptr, size_t size, size_t nmemb, void *userdata)
{
  cltTransaction_t *This = (cltTransaction_t*)userdata;
  size_t toWrite;
  size_t res;

  RTL_TRDBG(TRACE_DEBUG, "s_curl_sendReqBodyCb (transId:%s) (ptr:%0.8p) (size:%d)"
    " (nmemb:%d) (reqBodyLen:%d) (readReqBodyLen:%d)\n", This?This->transId:"(nil)", 
    ptr, size, nmemb, This?This->reqBodyLen:-1, This?This->readReqBodyLen:-1);

  if (! This) return 0;
  
  toWrite = (This->reqBodyLen - This->readReqBodyLen) /* * sizeof(unsigned char) */;

  if (0 == toWrite)
  {
    // all was already sent
    res = 0;
  }
  else 
  {
    if ( toWrite > (size * nmemb))
    {
      // not enough space available.
      toWrite = (size * nmemb);
    }
    memcpy(ptr, This->reqBody + This->readReqBodyLen, toWrite);
    This->readReqBodyLen += toWrite /* / sizeof(unsigned char)*/ ; 
    res = toWrite /* / sizeof(unsigned char)*/ ;
  }
  return res;
}

/**
 * The call back is invoked when CURLOPT_VERBOSE is enable.
 * @param eh the easy handler involved
 * @param type the debug message type.
 * @param msg the debug message (warning, is not null-string-terminated)
 * @param msgLen the length of the debug message.
 * @param userdata the transactionId involved.
 * @return must return 0;
 */
static int s_curl_debugCb(CURL *eh, curl_infotype type, char *msg, size_t msgLen,
  void *userdata)
{
  char message[MAX_BUF_SIZE_VL];
  char head[MAX_BUF_SIZE_H];
  cltTransaction_t *This = (cltTransaction_t*)userdata;
  int max = msgLen;

  if (! This) return 0;

  memset(head, 0, MAX_BUF_SIZE_H);
  memset(message, 0, MAX_BUF_SIZE_VL);

  switch (type)
  {
    case CURLINFO_TEXT:
      sprintf(head, "(transId:%s) [TXT   ] ", This->transId);
      break;
    case CURLINFO_HEADER_IN:
      sprintf(head, "(transId:%s) [HEAD_I] ", This->transId);
      break;
    case CURLINFO_HEADER_OUT:
      sprintf(head, "(transId:%s) [HEAD_O] ", This->transId);
      break;
    case CURLINFO_DATA_IN:
      sprintf(head, "(transId:%s) [DATA_I] ", This->transId);
      break;
    case CURLINFO_DATA_OUT:
      sprintf(head, "(transId:%s) [DATA_O] ", This->transId);
      break;
    case CURLINFO_SSL_DATA_IN:
      sprintf(head, "(transId:%s) [DSSL_I] ", This->transId);
      break;
    case CURLINFO_SSL_DATA_OUT:
      sprintf(head, "(transId:%s) [DSSL_O] ", This->transId);
      break;
    case CURLINFO_END:
      sprintf(head, "(transId:%s) [END   ] ", This->transId);
      break;
    default:
      sprintf(head, "(transId:%s) [UNK   ] ", This->transId);
      break;
  }

  if (max >= MAX_BUF_SIZE_VL)
  {
    max = MAX_BUF_SIZE_VL-1;
  }

  snprintf(message, max, "%s", msg);
  RTL_TRDBG(TRACE_DEBUG, "%s%s\n", head, message);

  return 0;
}

static int urlencode(char *url, char *encUrl)
{
  char *pPtr = NULL;
  int i = 0;
  if (!url) return 0;
  if (!encUrl) return 0;

  pPtr = url;
  while (*pPtr)                                                                                                               
  {
    switch (*pPtr)
    {
      case ':':
        encUrl[i] = '%';
        i++;
        encUrl[i] = '3';
        i++;
        encUrl[i] = 'a';
        i++;
        break;

      default:
        encUrl[i] = *pPtr;
        i++;
        break;
    }
    pPtr++;
  }
  encUrl[i] = 0;
  return 1;
}



