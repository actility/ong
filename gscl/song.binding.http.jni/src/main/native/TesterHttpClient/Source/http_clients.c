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
#include "admin_cli.h"
#include "header_t.h"                                                                               
#include "response_t.h"
#include "transaction_t.h"
#include "transactionsList_t.h"
#include "freeTransactionsList_t.h"
#include "httpClient_t.h"
#include "http_clients.h"



static void cliHttpResponseCb(void *issuerHandle, transaction_t *transaction);

static int g_nbClients = 0;
static int g_nbMaxClients = 0;
static httpClient_t ** g_clients = NULL;

extern int g_traceLevel;

void httpClientsInit(int nbMaxClients)
{
  curl_global_init(CURL_GLOBAL_ALL);
  g_nbMaxClients = nbMaxClients;
  g_clients = malloc(g_nbMaxClients * sizeof(httpClient_t *));
  memset(g_clients, 0, g_nbMaxClients * sizeof(httpClient_t *));

  g_traceLevel = 5;
}

void httpClientsUninit()
{
  int i;
  for(i = 0; i < g_nbMaxClients; i++)
  {
    if (g_clients[i])
    {
      g_clients[i]->free(g_clients[i]); 
    }
  }
  free(g_clients);
}

httpClient_t * createHttpClient(t_cli *cl)
{
  httpClient_t *res = NULL;
  int i;
  if (g_nbClients < g_nbMaxClients)
  {
    res = new_httpClient_t(0, cliHttpResponseCb, cl);
    res->setMaxSocketsPerHost(res, 50);
    res->setMaxTotalSockets(res, 50);
    for(i = 0; i < g_nbMaxClients; i++)
    {
      if (NULL == g_clients[i]) break;
    }
    g_clients[i] = res;
    g_nbClients ++;
  }
  return res;
}

void releaseHttpClient(t_cli *cl)
{
  int i;
  t_cli *cl_i;
  for(i = 0; i < g_nbMaxClients; i++)
  {
    cl_i = (t_cli *)g_clients[i]->issuerHandle;
    if (cl->cl_fd == cl_i->cl_fd)
    {
      g_clients[i] = NULL;  
      g_nbClients --;
      break;
    }
  }
}

void httpClientsClockMs()
{
  int i;
  httpClient_t *client;
  int nbClients;

  {
    i = 0;
    nbClients = g_nbClients;

    do
    {
      client = g_clients[i];
      if (client)
      {
        client->processMessages(client);
        client->trackTimedOutMessages(client);                                                      
      }
      i++;
    } while ((client) && (i < nbClients));
  }

} 

static void cliHttpResponseCb(void *issuerHandle, transaction_t *transaction)
{
  char *urlInfo = NULL;
  long response;
  header_t *h;
  int i = 0;

  t_cli *cl = (t_cli *)issuerHandle;

  curl_easy_getinfo(transaction->curlEasyHandle, CURLINFO_RESPONSE_CODE, &response);
  curl_easy_getinfo(transaction->curlEasyHandle, CURLINFO_EFFECTIVE_URL, &urlInfo);
  RTL_TRDBG(TRACE_DEBUG, "cliHttpResponseCb (transId:%s) (content:%0.8p) (len:%ld) "
    "(url:%s)\n", transaction->transId, transaction->response->content, 
    transaction->response->contentLength, urlInfo);

  if (cl->tidRInThreadRunning)
  {
    if (transaction->response->statusCode >= 100)
    {
      pthread_mutex_lock(&cl->stats.mutex);
      cl->stats.rcvRspOK ++;
      cl->stats.statusCodes[transaction->response->statusCode] ++;
      pthread_mutex_unlock(&cl->stats.mutex);
    }    
    else
    {
      pthread_mutex_lock(&cl->stats.mutex);
      cl->stats.rcvRspErr ++;
      cl->stats.statusCodes[transaction->response->statusCode] ++;
      pthread_mutex_unlock(&cl->stats.mutex);
    }

    if (cl->verbose)
    {
      AdmPrint(cl, "<- %s %d %s (transId:%s)\n", transaction->response->protocol, 
        transaction->response->statusCode, transaction->response->reasonPhrase, 
        transaction->transId);
    }
  }
  else
  {
    AdmPrint(cl, "===============================================\n");
    AdmPrint(cl, "===> Response received for %s (url:%s):\n", transaction->transId, 
      urlInfo);
    AdmPrint(cl, "> %s %d %s\n", transaction->response->protocol, 
      transaction->response->statusCode, transaction->response->reasonPhrase);
    while ((h = transaction->response->getNthHeader(transaction->response, i)))
    {
      AdmPrint(cl, "> %s: %s\n", h->name, h->values);
      i++;
    }
    AdmPrint(cl, "\n");
    if (transaction->response->content)
    { 
      AdmPrint(cl, (char *)transaction->response->content);
    }
    AdmPrint(cl, "\n");
  }
 
  cl->cl_client->removeReq(cl->cl_client, transaction->transId); 
}


