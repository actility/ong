
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <curl/curl.h>
#include <errno.h>
#include <poll.h>
#include <microhttpd.h>

#include "rtllist.h"
#include "rtlbase.h"

#include "common.h"
#include "song_over_http_api.h"
#include "srvTransaction_t.h"
#include "srvTransactionsList_t.h"

#define SRV_TRANS_ID_PREFIX "SRV_TID_"
static unsigned long g_srvTransId = 0;


static void srvTransaction_t_init(srvTransaction_t *This);

/**
 * Initialize the data structure.
 * @param This the object-like instance.
 */
static void srvTransaction_t_init(srvTransaction_t *This)
{
  RTL_TRDBG(TRACE_DEBUG, "srvTransaction_t::init (This:0x%.8x)\n", This);

  /* Assign the pointers on functions */
  This->setReqUrl = srvTransaction_t_setReqUrl;
  This->addReqHeader = srvTransaction_t_addReqHeader;
  This->setReqContentType = srvTransaction_t_setReqContentType;
  This->setReqEntity = srvTransaction_t_setReqEntity;
  This->setReqMethod = srvTransaction_t_setReqMethod;
  This->setReqBody = srvTransaction_t_setReqBody;
  This->sendResp = srvTransaction_t_sendResp;
  This->dumpInfo = srvTransaction_t_dumpInfo;

  /* Initialize the members */
  memset(This->transId, 0, MAX_BUF_SIZE_XS);
  This->start = 0;
  This->expired = 0;
  This->mhdConnection = NULL;
  This->reqUrl = NULL;
  This->reqContentType = NULL;
  This->reqEntity = NULL;
  This->reqBody = NULL;
  This->reqBodyLen = 0;
  This->method = SOH_UNKNOWN;
  memset(This->headers, 0, sizeof(sohHeader_t *) * (MAX_HEADERS+1));
}

/* dynamic allocation */

/**
 * Constructor.
 * @param con the connection handle for microhttpd.
 * @return the built instance of srvTransaction_t.
 */
srvTransaction_t *new_srvTransaction_t(struct MHD_Connection *con)
{
  srvTransaction_t *This = malloc(sizeof(srvTransaction_t));
  if (!This)
  {
    return NULL;
  }
  srvTransaction_t_init(This);
  This->free = srvTransaction_t_newFree;
  This->start = rtl_tmmsmono();
  This->mhdConnection = con;
  sprintf(This->transId, "%s%012lu", SRV_TRANS_ID_PREFIX, g_srvTransId++); 

  RTL_TRDBG(TRACE_DEBUG, "new srvTransaction_t (transId:%s) (This:0x%.8x)\n", 
    This->transId, This);
  return This;
}

/**
 * Destructor.
 * @param This the object-like instance.
 */
void srvTransaction_t_newFree(srvTransaction_t *This)
{
  int i;
  RTL_TRDBG(TRACE_DEBUG, "srvTransaction_t::newFree (This:0x%.8x) (transId:%s)\n", This, 
    This->transId);

  if (This->reqUrl)
  {
    free(This->reqUrl);
  }
  if (This->reqBody)
  {
    free(This->reqBody);
  }
  if (This->reqEntity)
  {
    free(This->reqEntity);
  }
  if (This->reqContentType)
  {
    free(This->reqContentType);
  }
  i = 0;
  while (This->headers[i] && i < MAX_HEADERS) 
  {
    free(This->headers[i]->name);
    free(This->headers[i]->values);
    free(This->headers[i]);
    i++;
  }
  free(This);
}

/**
 * Set the request URL parameter.
 * @param This the object-like instance.
 * @param url the URL to set.
 */
void srvTransaction_t_setReqUrl(srvTransaction_t *This, const char *url)
{
  if (! url) return;

  RTL_TRDBG(TRACE_DEBUG, "srvTransaction_t::setReqUrl (This:0x%.8x) (transId:%s) (url:%s)\n", 
    This, This->transId, url);
  if (This->reqUrl) 
  {
    free(This->reqUrl);
  }
  This->reqUrl = strdup(url);
}

/**
 * Set the request contentType parameter.
 * @param This the object-like instance.
 * @param contentType the contentType to set.
 */
void srvTransaction_t_setReqContentType (srvTransaction_t *This, const char *contentType)
{
  if (! contentType) return;

  RTL_TRDBG(TRACE_DEBUG, "srvTransaction_t::setReqContentType (This:0x%.8x) (transId:%s) "
    "(contentType:%s)\n", This, This->transId, contentType);
  if (This->reqContentType) 
  {
    free(This->reqContentType);
  }
  This->reqContentType = strdup(contentType);
}

/**
 * Set the requesting entity parameter.
 * @param This the object-like instance.
 * @param reqEntity the requesting entity to set.
 */
void srvTransaction_t_setReqEntity (srvTransaction_t *This, char *reqEntity)
{
  if (! reqEntity) return;

  RTL_TRDBG(TRACE_DEBUG, "srvTransaction_t::setReqEntity (This:0x%.8x) (transId:%s)"
    " (reqEntity:%s)\n", This, This->transId, reqEntity);
  if (This->reqEntity) 
  {
    free(This->reqEntity);
  }
  This->reqEntity = strdup(reqEntity);
}

/**
 * Set a new header value to the request.
 * @param This the object-like instance.
 * @param name the header name.
 * @param value the header value.
 */
void srvTransaction_t_addReqHeader(srvTransaction_t *This, const char *name, 
  const char *value)
{
  int i = 0;
  int found = 0;
  char *szTemp;
  while (This->headers[i] && (i < MAX_HEADERS) && !found) 
  {
    if (!strcmp(This->headers[i]->name, name))
    {
      found = 1;
    }
    else
    {
      i++;
    }
  }

  if (found)
  {
    // append value
    szTemp = malloc(strlen(value) + strlen(This->headers[i]->values) + 1 + 2);
    sprintf(szTemp, "%s, %s", This->headers[i]->values, value);
    free(This->headers[i]->values);
    This->headers[i]->values = szTemp;
  }
  else if (i < MAX_HEADERS)
  {
    // add new header
    This->headers[i] = malloc(sizeof(sohHeader_t));
    This->headers[i]->name = strdup(name);
    This->headers[i]->values = strdup(value);
  }
  // else header is not added...
}

/**
 * Set the request method.
 * @param This the object-like instance.
 * @param method the SONG method, can be CREATE (POST), RETRIEVE (GET), UPDATE (PUT) or 
 * DELETE (DELETE).
 */
void srvTransaction_t_setReqMethod(srvTransaction_t *This, SOH_METHOD method)
{
  RTL_TRDBG(TRACE_DEBUG, "srvTransaction_t::setReqMethod (This:0x%.8x) (transId:%s) "
    "(method:%d)\n", This, This->transId, method);

  This->method = method;
}

/**
 * Set the request content.
 * @param This the object-like instance.
 * @param body the request content to set.
 * @param len the content length.
 */
void srvTransaction_t_setReqBody(srvTransaction_t *This, const char *body, size_t len)
{
  RTL_TRDBG(TRACE_DEBUG, "srvTransaction_t::setReqBody (This:0x%.8x) (transId:%s) "
    "(len:%d)\n", This, This->transId, len);
  if (This->reqBody)
  {
    free(This->reqBody);
    This->reqBody = NULL;
  }
  if (len && body && *body)
  {
    This->reqBody = malloc(len * sizeof(unsigned char) + 1);
    memcpy(This->reqBody, body, len * sizeof(unsigned char));
    This->reqBodyLen = len;
    // theorically useless for unsigned char buffer, but can be usefull 
    // and prevent from copying the buffer if one known that content is 
    // printable... so, just in case...
    This->reqBody[len] = 0;
  }
}

/**
 * Send the request.
 * @param This the object-like instance.
 * @return MHD_YES if the request can be sent, MHD_NO otherwise.
 */
int srvTransaction_t_sendResp(srvTransaction_t *This, SOH_STATUS_CODE statusCode, 
  char *resourceURI, unsigned char *content, size_t len, char *contentType, 
  sohHeader_t **optHeaders)
{
  int ret;
  int i = 0; 
  struct MHD_Response *response;
  char szTemp[MAX_BUF_SIZE_XS];

  RTL_TRDBG(TRACE_INFO, ">>> srvTransaction_t::sendResp (This:0x%.8x) (transId:%s)\n",
    This, This->transId);

  response = MHD_create_response_from_buffer (len, (void *) content,
    MHD_RESPMEM_MUST_COPY);
  if (!response)
    return MHD_NO;

  sprintf(szTemp, "%zu", len);
  MHD_add_response_header (response, MHD_HTTP_HEADER_CONTENT_LENGTH, szTemp);
  if (len && content && contentType)
  {
    MHD_add_response_header (response, MHD_HTTP_HEADER_CONTENT_TYPE, contentType);
  }

  if (resourceURI)
  {
    MHD_add_response_header (response, MHD_HTTP_HEADER_CONTENT_LOCATION, resourceURI);
    MHD_add_response_header (response, MHD_HTTP_HEADER_LOCATION, resourceURI);
  }

  // add the optional headers
  if (optHeaders)
  {
    while (optHeaders[i])
    {
      MHD_add_response_header (response, optHeaders[i]->name, optHeaders[i]->values);
      i++;
    }
  }

  ret = MHD_queue_response (This->mhdConnection, statusCode, response);
  MHD_destroy_response (response);

  return ret;

}

/**
 * Used to display information regarding the pending sessions.
 * @param This the object-like instance.
 * @param dest the buffer in which dumping the built information.
 * @return dest.
 */
char *srvTransaction_t_dumpInfo(srvTransaction_t *This, char *dest)
{
  time_t now = 0;
  rtl_timemono(&now);
  if (This->expired)
  {
    sprintf(dest, "(transId:%s)(url:%s)(expiredSince:%lus)", This->transId, This->reqUrl, 
      now - This->expired);
  }
  else
  {
    sprintf(dest, "(transId:%s)(url:%s)(duration:%lus)", This->transId, This->reqUrl, 
      now - This->start);
  }
  return dest; 
}


